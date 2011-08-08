/*
 * Watchdog driver for IMX2 and later processors
 *
 *  Copyright (C) 2010 Wolfram Sang, Pengutronix e.K. <w.sang@pengutronix.de>
 *
 * some parts adapted by similar drivers from Darius Augulis and Vladimir
 * Zapolskiy, additional improvements by Wim Van Sebroeck.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published by
 * the Free Software Foundation.
 *
 * NOTE: MX1 has a slightly different Watchdog than MX2 and later:
 *
 *                      MX1:            MX2+:
 *                      ----            -----
 * Registers:           32-bit          16-bit
 * Stopable timer:      Yes             No
 * Need to enable clk:  No              Yes
 * Halt on suspend:     Manual          Can be automatic
 */

#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/miscdevice.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/platform_device.h>
#include <linux/watchdog.h>
#include <linux/clk.h>
#include <linux/fs.h>
#include <linux/io.h>
#include <linux/uaccess.h>
#include <linux/timer.h>
#include <linux/jiffies.h>
#include <mach/hardware.h>
#include <linux/slab.h>

#define IMX2_WDT_WCR            0x00            /* Control Register */
#define IMX2_WDT_WCR_WT         (0xFF << 8)     /* -> Watchdog Timeout Field */
#define IMX2_WDT_WCR_WRE        (1 << 3)        /* -> WDOG Reset Enable */
#define IMX2_WDT_WCR_WDE        (1 << 2)        /* -> Watchdog Enable */

#define IMX2_WDT_WSR            0x02            /* Service Register */
#define IMX2_WDT_SEQ1           0x5555          /* -> service sequence 1 */
#define IMX2_WDT_SEQ2           0xAAAA          /* -> service sequence 2 */

#define IMX2_WDT_MAX_TIME       128
#define IMX2_WDT_DEFAULT_TIME   60              /* in seconds */

#define WDOG_SEC_TO_COUNT(s)    ((s * 2 - 1) << 8)

#define IMX2_WDT_STATUS_OPEN    0
#define IMX2_WDT_STATUS_STARTED 1
#define IMX2_WDT_EXPECT_CLOSE   2

#define TIMER_MARGIN_MAX        127
#define TIMER_MARGIN_DEFAULT    60      /* 60 secs */
#define TIMER_MARGIN_MIN        1

static unsigned timer_margin = 5;
module_param(timer_margin, uint, 0);
MODULE_PARM_DESC(timer_margin, "initial watchdog timeout (in seconds)");

struct IMX2_wdt_data_t{
void __iomem *base;
struct clk *clk;
struct timer_list *timer;
int enable;
unsigned long status;
} imx2_wdt;

static inline void imx2_wdt_setup(void)
{
        u16 val = __raw_readw(imx2_wdt.base + IMX2_WDT_WCR);

        /* Strip the old watchdog Time-Out value */
        val &= ~IMX2_WDT_WCR_WT;
        /* Generate reset if WDOG times out */
        val &= ~IMX2_WDT_WCR_WRE;
        /* Keep Watchdog Disabled */
        val &= ~IMX2_WDT_WCR_WDE;
        /* Set the watchdog's Time-Out value */
        val |= WDOG_SEC_TO_COUNT(timer_margin);

        __raw_writew(val, imx2_wdt.base + IMX2_WDT_WCR);

        /* enable the watchdog */
        val |= IMX2_WDT_WCR_WDE;
        __raw_writew(val, imx2_wdt.base + IMX2_WDT_WCR);
}


static inline void imx2_wdt_ping(void)
{
   __raw_writew(IMX2_WDT_SEQ1, imx2_wdt.base + IMX2_WDT_WSR);
   __raw_writew(IMX2_WDT_SEQ2, imx2_wdt.base + IMX2_WDT_WSR);
}

static void imx2_wdt_start(void)
{
        if (!test_and_set_bit(IMX2_WDT_STATUS_STARTED, &imx2_wdt.status)) {
                /* at our first start we enable clock and do initialisations */
                clk_enable(imx2_wdt.clk);

                imx2_wdt_setup();
        } 

        /* Watchdog is enabled - time to reload the timeout value */
        imx2_wdt_ping();
}

static void imx2_wdt_set_timeout(int new_timeout)
{
        u16 val = __raw_readw(imx2_wdt.base + IMX2_WDT_WCR);

        /* set the new timeout value in the WSR */
        val &= ~IMX2_WDT_WCR_WT;
        val |= WDOG_SEC_TO_COUNT(new_timeout);
        __raw_writew(val, imx2_wdt.base + IMX2_WDT_WCR);
}


static void imx2_wdt_adjust_timeout(unsigned new_timeout)
{
	if (new_timeout < TIMER_MARGIN_MIN)
		new_timeout = TIMER_MARGIN_DEFAULT;
	if (new_timeout > TIMER_MARGIN_MAX)
		new_timeout = TIMER_MARGIN_MAX;
	timer_margin = new_timeout;
	imx2_wdt_set_timeout(timer_margin);
}

static void timer_handle(unsigned long arg)
{
	  if(imx2_wdt.enable == 1)
			imx2_wdt_ping();
		mod_timer(imx2_wdt.timer, jiffies + HZ * 1);
}

static int __exit imx2_wdt_test_remove(struct platform_device *pdev)
{
	pr_info("imx2 Watchdog test can not remove \n");
	return -EINVAL;
}



static int __init imx2_wdt_test_init(void)
{
	struct timer_list *timer;
	static struct resource imx2_wdt_resources[] = {
        {
                .start = MX6Q_WDOG1_BASE_ADDR,
                .end = MX6Q_WDOG1_BASE_ADDR + SZ_16K - 1,
                .flags = IORESOURCE_MEM,
        },
	};

	if ((timer_margin < TIMER_MARGIN_MIN) ||
	    (timer_margin > TIMER_MARGIN_MAX) ||
			(timer_margin < 1)) {
		pr_info("IMX2 watchdog error. wrong timer_margin %d\n",
			timer_margin);
		pr_info("Range: %d to %d seconds an must > 1\n", TIMER_MARGIN_MIN,
			TIMER_MARGIN_MAX);
		return -EINVAL;
	}
 	if (test_and_set_bit(IMX2_WDT_STATUS_OPEN, &imx2_wdt.status))
	         return -EBUSY;

	imx2_wdt.base = ioremap(imx2_wdt_resources[0].start,SZ_16K);
	pr_info("probe IMX2 WatchDog test Driver \n");

    timer = kmalloc(sizeof(*timer), GFP_KERNEL);
	if(NULL == timer)
			return -EINVAL;

	init_timer(timer);
	timer->data = 0;
	timer->expires = jiffies + HZ;
	timer->function = timer_handle;
	add_timer(timer);
    imx2_wdt.timer = timer;
	imx2_wdt_adjust_timeout(timer_margin);
	imx2_wdt_start();
	imx2_wdt.enable = 1;

	return 0;
}

static void __exit imx2_wdt_test_exit(void)
{
	pr_info("MXC WatchDog Driver test removed\n");
	if (imx2_wdt.timer != NULL)
	{
		del_timer(imx2_wdt.timer);
		kfree(imx2_wdt.timer);
	}
}

module_init(imx2_wdt_test_init);
module_exit(imx2_wdt_test_exit);

MODULE_AUTHOR("Freescale Semiconductor, Inc.");
MODULE_LICENSE("GPL");
