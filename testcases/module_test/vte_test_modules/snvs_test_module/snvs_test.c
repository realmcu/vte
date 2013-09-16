/*
 * Copyright (C) 2011 Freescale Semiconductor, Inc. All Rights Reserved.
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
#include <linux/device.h>
#include <linux/platform_device.h>
#include <asm/atomic.h>

#include "secvio.h"

static void caam_secvio_default(struct device *dev, u32 cause, void *ext)
{
     struct caam_drv_private_secvio *svpriv = dev_get_drvdata(dev);
     dev_err(dev, "Unhandled Security Violation Interrupt %d = %s\n",
         cause, svpriv->intsrc[cause].intname);
}


int match(struct device *dev, void *data)
{
	struct platform_device * pdev = to_platform_device(dev);
	if (strncmp(pdev->name,data,4) == 0)
		return 1;
	return 0;
}

static int test_init(void)
{
	int RC = 0;
	struct  device * dev; 
	printk("test snvs  handler start\n");
	dev = bus_find_device(&platform_bus_type,NULL,"caam",match);
	if (dev == NULL)
		return -EPERM;
	RC |= caam_secvio_install_handler(dev,SECVIO_CAUSE_SOURCE_0,caam_secvio_default,"test0",NULL);
	RC |= caam_secvio_install_handler(dev,SECVIO_CAUSE_SOURCE_1,caam_secvio_default,"test1",NULL);
	RC |= caam_secvio_install_handler(dev,SECVIO_CAUSE_SOURCE_2,caam_secvio_default,"test2",NULL);
	RC |= caam_secvio_install_handler(dev,SECVIO_CAUSE_SOURCE_4,caam_secvio_default,"test4",NULL);
	RC |= caam_secvio_install_handler(dev,SECVIO_CAUSE_SOURCE_5,caam_secvio_default,"test5",NULL);
	RC |= caam_secvio_remove_handler(dev,SECVIO_CAUSE_SOURCE_0);
	RC |= caam_secvio_remove_handler(dev,SECVIO_CAUSE_SOURCE_1);
	RC |= caam_secvio_remove_handler(dev,SECVIO_CAUSE_SOURCE_2);
	RC |= caam_secvio_remove_handler(dev,SECVIO_CAUSE_SOURCE_4);
	RC |= caam_secvio_remove_handler(dev,SECVIO_CAUSE_SOURCE_5);
	if (!RC){
		printk("test handler pass\n");
		return -EINVAL;
	}else{
		printk("test handler FAIL\n");
		return RC;
	}
}

static void test_exit(void)
{
    return;     
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Freescale Semiconductor, Inc.");
module_init(test_init);
module_exit(test_exit);
