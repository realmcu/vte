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
#include <linux/version.h>
#if LINUX_VERSION_CODE < KERNEL_VERSION(3,10,0)
#include <mach/hardware.h>
#endif
#include <linux/slab.h>

#ifdef CONFIG_OF
#include <linux/of.h>
#include <linux/of_address.h>
#endif

#define IMX2_WDT_WCR            0x00            /* Control Register */
#define IMX2_WDT_WCR_WT         (0xFF << 8)     /* -> Watchdog Timeout Field */
#define IMX2_WDT_WCR_WRE        (1 << 3)        /* -> WDOG Reset Enable */
#define IMX2_WDT_WCR_WDE        (1 << 2)        /* -> Watchdog Enable */
#define IMX2_WDT_WCR_WDZST		(1 << 0)	/* -> Watchdog timer Suspend */

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

		/* Suspend watch dog timer in low power mode, write once-only */
		val |= IMX2_WDT_WCR_WDZST;
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

#ifdef CONFIG_PM

static int
wdt_suspend(struct device *dev)
{
	/*ping the watch when enter so that the sleep time can manager*/
	if(imx2_wdt.enable == 1)
   		imx2_wdt_ping();
	return 0;
}

static int
wdt_resume(struct device *dev)
{
	/*ping the watch when enter so that the sleep time can manager*/
	if(imx2_wdt.enable == 1)
   		imx2_wdt_ping();
	return 0;
}

static const struct dev_pm_ops wdt_test_pm_ops = {
	.suspend	= wdt_suspend,
	.resume		= wdt_resume,
	.freeze		= wdt_suspend,
	.thaw		= wdt_resume,
	.poweroff	= wdt_suspend,
	.restore	= wdt_resume,
};
#endif



static struct platform_driver wdt_driver = {
        .remove         = __exit_p(imx2_wdt_test_remove),
        .driver         = {
                .name   = "imx2_wdt",
                .owner  = THIS_MODULE,
#ifdef CONFIG_PM
				.pm = &wdt_test_pm_ops,
#endif
        },
};

static struct platform_device * wdt_device;

static int wdt_probe(struct platform_device * pdev)
{
	struct timer_list *timer;
#ifdef CONFIG_OF
    struct device_node *np;
#else
	static struct resource imx2_wdt_resources[] = {
        {
                .start = MX6Q_WDOG1_BASE_ADDR,
                .end = MX6Q_WDOG1_BASE_ADDR + SZ_16K - 1,
                .flags = IORESOURCE_MEM,
        },
	};
#endif

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

#ifdef CONFIG_OF
    np = of_find_compatible_node(NULL, NULL, "fsl,imx21-wdt");
    imx2_wdt.base = of_iomap(np, 0);
    WARN_ON(!imx2_wdt.base);
#else
	imx2_wdt.base = ioremap(imx2_wdt_resources[0].start,SZ_16K);
#endif

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

static int __init imx2_wdt_test_init(void)
{
	int ret;

	wdt_device = platform_device_register_simple("imx2_wdt", -1, NULL, 0);
    if (IS_ERR(wdt_device)) {
        printk(KERN_ERR "wdt_test: platform_device_register failed.\n");
        ret = PTR_ERR(wdt_device);
        goto out;
    }

	return platform_driver_probe(&wdt_driver, wdt_probe);
out:
	platform_device_unregister(wdt_device);
	return ret;
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
