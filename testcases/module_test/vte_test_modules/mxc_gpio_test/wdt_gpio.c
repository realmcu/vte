/*
 * Copyright (C) 2012 Freescale Semiconductor, Inc. All Rights Reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */
#include <linux/module.h>
#include <linux/init.h>
#include <linux/list.h>
#include <linux/types.h>
#include <linux/kernel.h> 
#include <linux/interrupt.h>
#include <linux/moduleparam.h>
#include <linux/io.h>
#include <linux/slab.h>
#include <linux/vmalloc.h>
#include <linux/sched.h>
#include <linux/gpio.h>
#include <linux/timer.h>
#include <linux/delay.h>
#include <asm/atomic.h>
#include <mach/hardware.h>

#define  GPIO_PSR 8
#define  GPIO_DIR 4
#define  GPIO_DR  0


static unsigned test_count = 10000;
module_param(test_count,uint,0);
MODULE_PARM_DESC(test_count,"tested times x100ms");

static void __iomem *regA;
static void __iomem *regB;

static int test_init(void)
{
	int count = test_count;
	unsigned long speed = 0;
	unsigned long IOMUXC_IOMUXC_SW_MUX_CTL_PAD_WDOG_B = 0x02a0;
	unsigned long IOMUXC_IOMUXC_SW_PAD_CTL_PAD_WDOG_B = 0x05a8;
	u32 l;
	/*map GPIO3*/
	regA = ioremap(GPIO3_BASE_ADDR,SZ_32K); 
	regB = ioremap(MX6Q_IOMUXC_BASE_ADDR,SZ_32K); 

	printk(KERN_INFO "gpio tet sequence on GPIO3 18 bit\n");
	while(count--){
		l = 0x000005;
		__raw_writel(l ,regB + IOMUXC_IOMUXC_SW_MUX_CTL_PAD_WDOG_B);
		l = 0x4110b0;
		__raw_writel(l ,regB + IOMUXC_IOMUXC_SW_PAD_CTL_PAD_WDOG_B);
		l = 0xefa7f000;
		__raw_writel(l ,regA + GPIO_DR);
		l = 0x40000;
		__raw_writel(l ,regA + GPIO_DIR);
		l = 0x00000000;
		__raw_writel(l ,regB + IOMUXC_IOMUXC_SW_MUX_CTL_PAD_WDOG_B);
		msleep(100);
		l = 0x40000;
		__raw_writel(l ,regA + GPIO_DIR);
		l = 0xefa7f000;
		__raw_writel(l ,regA + GPIO_DR);
		l = 0x4110b0;
		__raw_writel(l ,regB + IOMUXC_IOMUXC_SW_PAD_CTL_PAD_WDOG_B);
		l = 0x000005;
		__raw_writel(l ,regB + IOMUXC_IOMUXC_SW_MUX_CTL_PAD_WDOG_B);
	}
	printk(KERN_INFO "gpio set speed is %ld us\n", speed);
	return -ENODEV;
}

static void test_exit(void)
{
	if (regA)
		iounmap(regA);
	if (regB)
		iounmap(regB);

    return;     
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Freescale Semiconductor, Inc.");
module_init(test_init);
module_exit(test_exit);
