/*
* Copyright (C) 2010 Freescale Semiconductor, Inc. All Rights Reserved.
*
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
*
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
*
# You should have received a copy of the GNU General Public License along
# with this program; if not, write to the Free Software Foundation, Inc.,
# 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/
/**
*<p>Title: this is a kernel module for wd test code copied from mxc_wd.c</p>
*<p>Description: load this module set the timeout PARAM</p>
*<p>a kernel thread will feed dog in timeout interval</p>
*<p>a reset ssytem if timeout</p>
*<p>Copyright: 2010 Freescale Semiconductor, Inc. All Rights Reserved.</p>
*<p>Company:Freescale Semiconductor, Inc.<\p>
*@author:
*@file:
*@version:
*/

#include <linux/module.h>
#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/watchdog.h>
#include <linux/reboot.h>
#include <linux/init.h>
#include <linux/err.h>
#include <linux/platform_device.h>
#include <linux/interrupt.h>
#include <linux/clk.h>
#include <linux/io.h>
#include <linux/uaccess.h>
#include <linux/timer.h>
#include <linux/slab.h>
#include <asm/atomic.h>
#include <mach/hardware.h>
#include "mxc_wdt.h"

#define DVR_VER "2.0"

#define WDOG_SEC_TO_COUNT(s)  ((s * 2) << 8)
#define WDOG_COUNT_TO_SEC(c)  ((c >> 8) / 2)

static void __iomem  *wdt_base_reg;

static unsigned timer_margin = 5;
module_param(timer_margin, uint, 0);
MODULE_PARM_DESC(timer_margin, "initial watchdog timeout (in seconds)");

static unsigned dev_num;

struct mxc_wdt_data_t{
struct resource *res;
struct timer_list *timer;
int enable;
} mxc_wdt_data;


static void mxc_wdt_ping(void *base)
{
	/* issue the service sequence instructions */
	__raw_writew(WDT_MAGIC_1, base + MXC_WDT_WSR);
	__raw_writew(WDT_MAGIC_2, base + MXC_WDT_WSR);
}


static void mxc_wdt_enable(void *base)
{
	u16 val;

	val = __raw_readw(base + MXC_WDT_WCR);
	val |= WCR_WDE_BIT;
	__raw_writew(val, base + MXC_WDT_WCR);
}

static void mxc_wdt_disable(void *base)
{
	/* disable not supported by this chip */
}

static void mxc_wdt_adjust_timeout(unsigned new_timeout)
{
	if (new_timeout < TIMER_MARGIN_MIN)
		new_timeout = TIMER_MARGIN_DEFAULT;
	if (new_timeout > TIMER_MARGIN_MAX)
		new_timeout = TIMER_MARGIN_MAX;
	timer_margin = new_timeout;
}

static void mxc_wdt_set_timeout(void *base)
{
	u16 val;
	val = __raw_readw(base + MXC_WDT_WCR);
	val = (val & 0x00FF) | WDOG_SEC_TO_COUNT(timer_margin);
	__raw_writew(val, base + MXC_WDT_WCR);
	val = __raw_readw(base + MXC_WDT_WCR);
	timer_margin = WDOG_COUNT_TO_SEC(val);
}

static struct resource mxc_wdt_resources[] = {
	{
		.start = WDOG1_BASE_ADDR,
		.end = WDOG1_BASE_ADDR,
		.flags = IORESOURCE_MEM,
	},
};

static void timer_handle(unsigned long arg)
{
	  if(mxc_wdt_data.enable == 1)
			mxc_wdt_ping(wdt_base_reg);
		mod_timer(mxc_wdt_data.timer, jiffies + HZ * 1);
}

static int __exit mxc_wdt_test_remove(struct platform_device *pdev)
{
	pr_info("MXC Watchdog test can not remove \n");
	return -EINVAL;
}

static int __init mxc_wdt_test_init(void)
{
	struct resource *res;
	struct timer_list *timer;
	pr_info("MXC WatchDog test Driver %s\n", DVR_VER);

	if ((timer_margin < TIMER_MARGIN_MIN) ||
	    (timer_margin > TIMER_MARGIN_MAX) ||
			(timer_margin < 1)) {
		pr_info("MXC watchdog error. wrong timer_margin %d\n",
			timer_margin);
		pr_info("Range: %d to %d seconds an must > 1\n", TIMER_MARGIN_MIN,
			TIMER_MARGIN_MAX);
		return -EINVAL;
	}

	pr_info("probe MXC WatchDog test Driver %s\n", DVR_VER);
  /*copy from the arch/arm/march_mx5/devices.c*/
	if (cpu_is_mx53() || cpu_is_mx50()) {
		#define MX5_OFFSET 0x20000000
		mxc_wdt_resources[0].start -= MX5_OFFSET;
		mxc_wdt_resources[0].end -= MX5_OFFSET;
	}
	mxc_wdt_data.res = &mxc_wdt_resources[0];
	res = mxc_wdt_data.res;
	#if 0
	/*do not. This resource should be used*/
	/* reserve static register mappings */
	mem = request_mem_region(res->start, res->end - res->start + 1,
				 pdev->name);
	if (mem == NULL)
		return -EBUSY;
 #endif

	wdt_base_reg = ioremap(res->start, res->end - res->start + 1);
	mxc_wdt_disable(wdt_base_reg);
	mxc_wdt_adjust_timeout(timer_margin);
  
  timer = kmalloc(sizeof(*timer), GFP_KERNEL);
	if(NULL == timer)
			return -EINVAL;

	init_timer(timer);
	timer->data = dev_num;
	timer->expires = jiffies + HZ;
	timer->function = timer_handle;
	add_timer(timer);
  mxc_wdt_data.timer = timer;

	mxc_wdt_adjust_timeout(timer_margin);
	mxc_wdt_disable(wdt_base_reg);
	mxc_wdt_set_timeout(wdt_base_reg);
  mxc_wdt_enable(wdt_base_reg);
	mxc_wdt_ping(wdt_base_reg);
	mxc_wdt_data.enable = 1;

	return 0;
}

static void __exit mxc_wdt_test_exit(void)
{
	pr_info("MXC WatchDog Driver test removed\n");
}

module_init(mxc_wdt_test_init);
module_exit(mxc_wdt_test_exit);

MODULE_AUTHOR("Freescale Semiconductor, Inc.");
MODULE_LICENSE("GPL");
