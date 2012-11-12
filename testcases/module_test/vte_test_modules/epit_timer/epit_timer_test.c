/*
 * Copyright 2008-2011 Freescale Semiconductor, Inc. All Rights Reserved.
 */

/*
 * The code contained herein is licensed under the GNU General Public
 * License. You may obtain a copy of the GNU General Public License
 * Version 2 or later at the following locations:
 *
 * http://www.opensource.org/licenses/gpl-license.html
 * http://www.gnu.org/copyleft/gpl.html
 */

/*!
 * @file wfi_issue.c
 *
 * @brief A simple driver to generate frequent interrupts to verify the WFI issue on MX6.
 *
 *
 * @ingroup PM
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/io.h>
#include <linux/fs.h>
#include <linux/interrupt.h>
#include <linux/jiffies.h>
#include <linux/device.h>
#include <linux/sysdev.h>
#include <linux/delay.h>
#include <linux/clk.h>
#include <linux/regulator/consumer.h>
#include <linux/input.h>
#include <linux/platform_device.h>
#if defined(CONFIG_CPU_FREQ)
#include <linux/cpufreq.h>
#endif
#include <asm/cpu.h>

#include <mach/hardware.h>

#include "crm_regs.h"


static void __iomem *epit_base;
static int wfi_issue_timer;
static struct platform_device * epit_device;

static irqreturn_t epit_irq(int irq, void *dev_id)
{
	struct timespec nstimeofday; 
	u32 cmp;
	
	getnstimeofday(&nstimeofday); \
	cmp = nstimeofday.tv_nsec % 555;
	
	if (cmp < 0x85)
		cmp = 0x99;
	
	__raw_writel(cmp, epit_base + 0x08);

	__raw_writel(1, epit_base + 0x4);

	return IRQ_HANDLED;
}


static ssize_t wfi_issue_show(struct device *dev,
				struct device_attribute *attr, char *buf)
{
	if (wfi_issue_timer)
		return sprintf(buf, "wfi_issue_timer is ENABLED\n");
	else
		return sprintf(buf, "wfi_issue_timer is disabled\n");
}

static ssize_t wfi_issue_enable_store(struct device *dev,
				 struct device_attribute *attr,
				 const char *buf, size_t size)
{
	u32 reg = __raw_readl(epit_base + 0x0);
	if (strstr(buf, "1") != NULL) {
		wfi_issue_timer = 1;
		reg |= 0x1;
		__raw_writel(reg, epit_base + 0x0);
		__raw_writel(0x85, epit_base + 0x08);
		
	} else if (strstr(buf, "0") != NULL) {
		wfi_issue_timer = 0;
		reg &= ~0x1;
		__raw_writel(reg, epit_base + 0x0);
	}
	return size;
}



static DEVICE_ATTR(enable, S_IRUGO | S_IWUSR,
				wfi_issue_show, wfi_issue_enable_store);


/*!
 * This is the probe routine for the WFI_ISSUE driver.
 *
 * @param   pdev   The platform device structure
 *
 * @return         The function returns 0 on success
 */
static int __devinit mxc_wfi_issue_probe(struct platform_device *pdev)
{
	int err = 0;
	u32 reg;

	printk(KERN_INFO "mxc_WFI_ISSUE_probe\n");

	epit_base = ioremap(EPIT1_BASE_ADDR, SZ_4K);
	
	/* Keep EPIT clocks enabled. */
	reg =  __raw_readl(MXC_CCM_CCGR1);	
        __raw_writel( (3 << MXC_CCM_CCGRx_CG6_OFFSET) | reg, MXC_CCM_CCGR1);
	
	/*
	 * Request the EPIT interrupt
	 */
	err = request_irq(MXC_INT_EPIT1, epit_irq, IRQF_SHARED, "epit",
			  pdev);
	if (err) {
		printk(KERN_ERR
		       "EPIT: Unable to attach to EPIT interrupt,err = %d",
		       err);
		goto err2;
	}


	/* Set up the EPIT to generate a constant compare. */
	reg = __raw_readl(epit_base + 0x0);
	reg |= 0x20a000E;
	__raw_writel(reg, epit_base + 0x0);
	
	/* Set the LOAD register to 2usec interrupt. */
	__raw_writel(0x85, epit_base + 0x08);

	err = sysfs_create_file(&pdev->dev.kobj, &dev_attr_enable.attr);
	if (err) {
		printk(KERN_ERR
		       "EPIT: Unable to register sysdev entry for EPIT");
		goto err3;
	}


	/* Set the current working point. */
	return err;
err3:
err2:
	iounmap(epit_base);
	dev_err(&pdev->dev, "Failed to probe WFI_ISSUE\n");
	return err;
}

static int __exit epit_remove(struct platform_device *pdev)
{
	pr_info("epit driver remove\n");
	free_irq(MXC_INT_EPIT1, pdev);
	iounmap(epit_base);
	sysfs_remove_file(&pdev->dev.kobj, &dev_attr_enable.attr);
	return -EINVAL;
}

static struct platform_driver mxc_wfi_issue_driver = {
        .probe =  mxc_wfi_issue_probe,
	.remove         = __exit_p(epit_remove),
	.driver = {
		   .name = "epit",
		   .owner = THIS_MODULE,
		   },
};

static struct platform_device epit_dev = {
	.name = "epit",
};

static int wfi_issue_init(void)
{
	int ret;
	epit_device = platform_device_register_simple("epit", 0, NULL, 0);
	if (IS_ERR(epit_device)) {
                printk(KERN_ERR "epit_test: platform_device_register failed.\n");
                ret = PTR_ERR(epit_device);
                goto out;
	}
	return platform_driver_probe(&mxc_wfi_issue_driver, mxc_wfi_issue_probe);
out:
	platform_device_unregister(epit_device);
	return ret;
}

static void __exit wfi_issue_exit(void)
{
	u32 reg;
	reg = __raw_readl(epit_base + 0x0);
	reg &= (~0x1);	
 	__raw_writel(reg, epit_base + 0x0);
	
	/* Keep EPIT clocks disabled. */
	reg =  __raw_readl(MXC_CCM_CCGR1);	
        __raw_writel( ~(3 << MXC_CCM_CCGRx_CG6_OFFSET) & reg, MXC_CCM_CCGR1);
	platform_device_unregister(epit_device);
	pr_info("remove epit timer can not support hotplug, reboot to enable it\n");
} 

module_init(wfi_issue_init);
module_exit(wfi_issue_exit);

MODULE_AUTHOR("Freescale Semiconductor, Inc.");
MODULE_DESCRIPTION("EPIT driver");
MODULE_LICENSE("GPL");
