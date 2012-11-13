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
#include <linux/clkdev.h>
#include <linux/input.h>
#include <linux/platform_device.h>
#if defined(CONFIG_CPU_FREQ)
#include <linux/cpufreq.h>
#endif
#include <asm/cpu.h>
#include <mach/clock.h>
#include <mach/hardware.h>

#include "crm_regs.h"

#define NUMBERS (2)

static char * epits[2] = {"epit0", "epit1"};

//static void __iomem *epit_base;
static int wfi_issue_timer;
static struct platform_device * epit_device;

struct epit_data_s {
	char * epitname;
	void __iomem * epit_base;
	struct clk * clk;
	unsigned int irq;	
};

static struct epit_data_s edata[NUMBERS];

static struct epit_data_s * get_instance_by_clk (struct clk *clk)
{
	int i;
	for (i = 0; i < NUMBERS; i++)
	{
		if (edata[i].clk && edata[i].clk == clk )
			return &edata[i];	
	}
	return NULL;	
}

static struct epit_data_s * get_instance_by_dev_name (const char * name)
{
	int i;
	for (i = 0 ; i < NUMBERS; i++)
	{
		if(edata[i].epitname && strcmp(edata[i].epitname,name) == 0)
			return &edata[i];
	}
	return NULL;
}

static int epit_clk_enable(struct clk *clk)
{
	u32 reg;	
	/* Keep EPIT clocks enabled. */
	reg =  __raw_readl(clk->enable_reg);	
    __raw_writel( (3 << clk->enable_shift) | reg, clk->enable_reg);
	return 0;
}

static void epit_clk_disable(struct clk *clk)
{
	u32 reg;
	struct epit_data_s * pdata =  get_instance_by_clk(clk);
	if (pdata == NULL)
	{
		printk("no instance found");
		return;
	}
		
	reg = __raw_readl(pdata->epit_base + 0x0);
	reg &= (~0x1);	
 	__raw_writel(reg, pdata->epit_base + 0x0);
	/* Keep EPIT clocks disabled. */
	reg =  __raw_readl(clk->enable_reg);	
    __raw_writel( ~(3 << clk->enable_shift) & reg, clk->enable_reg);
}

static int epit_clk_set_parent(struct clk *clk, struct clk *parent)
{
	u32 reg;
	struct epit_data_s * pdata =  get_instance_by_clk(clk);
	struct clk * tclk;
	if (pdata == NULL)
	{
		printk(" %s no instance found\n", __func__);
		return -EINVAL;
	}
	/* Disable the EPIT by setting EN=0 in EPIT_EPITCR  */
	reg = __raw_readl(pdata->epit_base + 0x0);
	reg &= (~0x1);	
	__raw_writel(reg, pdata->epit_base + 0x0);
	/* Program OM=00 in the EPIT_EPITCR */
	reg &= ~0xc00000;
	__raw_writel(reg, pdata->epit_base + 0x0);
	/* Disable the EPIT interrupts.  */
	disable_irq(MXC_INT_EPIT1);
	/* Program CLKSRC to desired clock source in EPIT_EPITCR. */
	tclk = clk_get(NULL,"periph_clk");
	if ( tclk == parent)
	{
		reg |= 0x1000000;	
	}else {
		tclk = clk_get(NULL,"ckih");
		if (tclk == parent)
			reg |= 0x2000000;
		else{
			reg |= 0x3000000;	
		}
	}
	__raw_writel(reg, pdata->epit_base + 0x0);
	/* Clear the EPIT status register (EPIT_EPITSR), that is, write "1" to clear (w1c). */
	__raw_writel(1, pdata->epit_base + 0x4);
	/* Enable the EPIT interrupts.  */
	enable_irq(pdata->irq);
	/* Set ENMOD= 1 in the EPIT_EPITCR, to bring the EPIT Counter to defined state */
	reg |= 0x00a000E;
	printk("write to base %x\n",reg);
	__raw_writel(reg, pdata->epit_base + 0x0);
	return 0;
}

static unsigned long epit_clk_get_rate(struct clk *clk)
{
	return clk_get_rate(clk->parent);
}

static struct clk epit0_clk = {
		.enable = epit_clk_enable,
		.enable_reg = MXC_CCM_CCGR1,
		.enable_shift = MXC_CCM_CCGRx_CG6_OFFSET,
		.disable = epit_clk_disable,
		.set_parent =  epit_clk_set_parent,
		.get_rate = epit_clk_get_rate,
};

static struct clk epit1_clk = {
		.enable = epit_clk_enable,
		.enable_reg = MXC_CCM_CCGR2,
		.enable_shift = MXC_CCM_CCGRx_CG7_OFFSET,
		.disable = epit_clk_disable,
		.set_parent =  epit_clk_set_parent,
		.get_rate = epit_clk_get_rate,
};

static struct clk_lookup epit_lu[] = {
	{ .dev_id = NULL, .con_id = "epit0_clk", .clk = &epit0_clk },
	{ .dev_id = NULL,   .con_id = "epit1_clk", .clk = &epit1_clk },
};

static irqreturn_t epit_irq(int irq, void *dev_id)
{
	u32 cmp;
	int i;
	struct timespec nstimeofday; 

	getnstimeofday(&nstimeofday); \
	cmp = nstimeofday.tv_nsec % 555;
	
	if (cmp < 0x85)
		cmp = 0x99;

	for (i = 0; i < NUMBERS; i++)
	{
		if (edata[i].irq == irq){
			__raw_writel(cmp, edata[i].epit_base + 0x08);
			__raw_writel(1, edata[i].epit_base + 0x4);
			break;
		}
	}
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

	u32 reg;
	struct epit_data_s * pdata = get_instance_by_dev_name(dev->driver->name);
	if (pdata == NULL)
	{
		return 0;	
	}
	reg = __raw_readl(pdata->epit_base + 0x0);

	if (strstr(buf, "1") != NULL) {
		wfi_issue_timer = 1;
		reg |= 0x1;
		__raw_writel(reg, pdata->epit_base + 0x0);
		__raw_writel(0x85, pdata->epit_base + 0x08);
		
	} else if (strstr(buf, "0") != NULL) {
		wfi_issue_timer = 0;
		reg &= ~0x1;
		__raw_writel(reg, pdata->epit_base + 0x0);
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
	struct clk * parent_clk;
	struct resource * p_res;
	
	/* just for module insert usage*/
	if (IS_ERR(clk_get(NULL,"epit0_clk")))
	{
		clkdev_add(&epit_lu[0]);
		clk_debug_register(epit_lu[0].clk);
	}
	if (IS_ERR(clk_get(NULL,"epit1_clk")))
	{
		clkdev_add(&epit_lu[1]);
		clk_debug_register(epit_lu[1].clk);
	}

	if (pdev->id > NUMBERS)
	{
		printk("mxc_WFI_ISSUE_probe failed id out range <2\n");
		return -EINVAL;
	}
	p_res = platform_get_resource(pdev,IORESOURCE_MEM, 0);
	edata[pdev->id].epit_base = ioremap(p_res->start, p_res->end  - p_res->start + 1);
	edata[pdev->id].epitname = epits[pdev->id]; 	
	/*
	 * Request the EPIT interrupt
	 */
	p_res = platform_get_resource(pdev,IORESOURCE_IRQ, 0);
	edata[pdev->id].irq = p_res->start; 
	err = request_irq((int)p_res->start, epit_irq, IRQF_SHARED,  edata[pdev->id].epitname,
			  pdev);
	if (err) {
		printk(KERN_ERR
		       "EPIT: Unable to attach to EPIT interrupt,err = %d",
		       err);
		goto err2;
	}

	/* Set up the EPIT to generate a constant compare. */
	#if 0
	reg = __raw_readl(edata[pdev->id].epit_base + 0x0);
	reg |= 0x20a000E;
	__raw_writel(reg, edata[pdev->id].epit_base + 0x0);
	#else
	p_res = platform_get_resource_byname(pdev, IORESOURCE_MEM, "clock parent");
	parent_clk = clk_get(NULL,(*(char **)p_res->start));
	if (IS_ERR(parent_clk))
	{
		err = -EINVAL;
		printk(KERN_ERR"clk get parent failed");
		goto err3;
	}
	printk("get parent %s@%x\n", *(char **)p_res->start, parent_clk);

	p_res = platform_get_resource_byname(pdev, IORESOURCE_MEM, "clock");
	edata[pdev->id].clk = (struct clk *)p_res->start;	
	clk_set_parent((struct clk *)p_res->start, parent_clk);
	err = clk_enable((struct clk *)p_res->start);
	if (err)
	{
		printk(KERN_ERR"clk enalbe epit failed = %d", err);
		goto err3;
	}
	#endif
	
	/* Set the LOAD register to 2usec interrupt. */
	__raw_writel(0x85, edata[pdev->id].epit_base + 0x08);

	err = sysfs_create_file(&pdev->dev.kobj, &dev_attr_enable.attr);
	if (err) {
		printk(KERN_ERR
		       "EPIT: Unable to register sysdev entry for EPIT");
		goto err3;
	}


	printk(KERN_INFO "mxc_WFI_ISSUE_probe\n");
	/* Set the current working point. */
	return err;
err3:
	free_irq(p_res->start,pdev);
err2:
	iounmap(edata[pdev->id].epit_base);
	dev_err(&pdev->dev, "Failed to probe WFI_ISSUE\n");
	return err;
}

static int __exit epit_remove(struct platform_device *pdev)
{
	pr_info("epit driver remove\n");
	free_irq(edata[pdev->id].irq, pdev);
	iounmap(edata[pdev->id].epit_base);
	sysfs_remove_file(&pdev->dev.kobj, &dev_attr_enable.attr);
	clk_disable(edata[pdev->id].clk);
	return 0;
}

static struct platform_driver mxc_wfi_issue_driver = {
        .probe =  mxc_wfi_issue_probe,
		.remove = __exit_p(epit_remove),
		.driver = {
		   .name = "epit0",
		   .owner = THIS_MODULE,
		   },
};


static char * clock_parent = "ckih";

static int __init wfi_issue_init(void)
{
	int ret;
	static struct resource epit0_resource[4] = {
		{
			.name = "address",
			.start = EPIT1_BASE_ADDR,
        	.end = EPIT1_BASE_ADDR + SZ_4K - 1,
        	.flags = IORESOURCE_MEM,
        },
		{
		.name = "epit0 irq",
		.start = MXC_INT_EPIT1,
        .end = MXC_INT_EPIT1,
        .flags = IORESOURCE_IRQ,
		},
		/*clock parent name*/
		{
		.name = "clock parent",
		.start = (int)&clock_parent,
		.end =  (int)&clock_parent,
		.flags = IORESOURCE_MEM,
		},
		/* clock data  */
		{
		.name = "clock",
		.start = (int)&epit0_clk,
		.end = (int)&epit0_clk,
		.flags = IORESOURCE_MEM,
		},
	};	

	epit_device = platform_device_register_simple("epit0", 0, epit0_resource, 4);
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
#if 0
	u32 reg;
	reg = __raw_readl(epit_base + 0x0);
	reg &= (~0x1);	
 	__raw_writel(reg, epit_base + 0x0);
	
	/* Keep EPIT clocks disabled. */
	reg =  __raw_readl(MXC_CCM_CCGR1);	
    __raw_writel( ~(3 << MXC_CCM_CCGRx_CG6_OFFSET) & reg, MXC_CCM_CCGR1);
#endif
	platform_device_unregister(epit_device);
	pr_info("remove epit timer can not support hotplug, reboot to enable it\n");
} 

module_init(wfi_issue_init);
module_exit(wfi_issue_exit);

MODULE_AUTHOR("Freescale Semiconductor, Inc.");
MODULE_DESCRIPTION("EPIT driver");
MODULE_LICENSE("GPL");
