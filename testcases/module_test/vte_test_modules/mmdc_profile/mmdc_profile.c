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
#include <linux/kernel.h> 
#include <linux/interrupt.h>
#include <linux/slab.h>
#include <linux/vmalloc.h>
#include <linux/sched.h>
#include <linux/moduleparam.h>
#include <linux/platform_device.h>
#include <linux/miscdevice.h>
#include <mach/hardware.h>
#include <linux/io.h>
#include <linux/clk.h>
#include <linux/fs.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Freescale Semiconductor, Inc.");

typedef struct 
{
	unsigned int mdctl;
	unsigned int mdpdc;
	unsigned int mdotc;
	unsigned int mdcfg0;
	unsigned int mdcfg1;
	unsigned int mdcfg2;
	unsigned int mdmisc;
	unsigned int mdscr;
	unsigned int mdref;
	unsigned int mdwcc;
	unsigned int mdrcc;
	unsigned int mdrwd;
	unsigned int mdor;
	unsigned int mdmrr;
	unsigned int mdcfg3lp;
	unsigned int mdmr4;
	unsigned int mdasp;

	unsigned int adopt_base_offset_fill[239];
	unsigned int maarcr;
	unsigned int mapsr;
	unsigned int maexidr0;
	unsigned int maexidr1;
	unsigned int madpcr0;
	unsigned int madpcr1;
	unsigned int madpsr0;
	unsigned int madpsr1;
	unsigned int madpsr2;
	unsigned int madpsr3;
	unsigned int madpsr4;
	unsigned int madpsr5;
	unsigned int masbs0;
	unsigned int masbs1;
	unsigned int ma_reserved1;
	unsigned int ma_reserved2;
	unsigned int magenp;
	
	unsigned int phy_base_offset_fill[239];
	unsigned int mpzqhwctrl;
	unsigned int mpzqswctrl;
	unsigned int mpwlgcr;
	unsigned int mpwldectrl0;
	unsigned int mpwldectrl1;
	unsigned int mpwldlst;
	unsigned int mpodtctrl;
	unsigned int mpredqby0dl;
	unsigned int mpredqby1dl;
	unsigned int mpredqby2dl;
	unsigned int mpredqby3dl;
	unsigned int mpwrdqby0dl;
	unsigned int mpwrdqby1dl;
	unsigned int mpwrdqby2dl;
	unsigned int mpwrdqby3dl;
	unsigned int mpdgctrl0;
	unsigned int mpdgctrl1;
	unsigned int mpdgdlst;
	unsigned int mprddlctl;
	unsigned int mprddlst;
	unsigned int mpwrdlctl;
	unsigned int mpwrdlst;
	unsigned int mpsdctrl;
	unsigned int mpzqlp2ctl;
	unsigned int mprddlhwctl;
	unsigned int mpwrdlhwctl;
	unsigned int mprddlhwst0;
	unsigned int mprddlhwst1;
	unsigned int mpwrdlhwst0;
	unsigned int mpwrdlhwst1;
	unsigned int mpwlhwerr;
	unsigned int mpdghwst0;
	unsigned int mpdghwst1;
	unsigned int mpdghwst2;
	unsigned int mpdghwst3;
	unsigned int mppdcmpr1;
	unsigned int mppdcmpr2;
	unsigned int mpswdar;
	unsigned int mpswdrdr0;
	unsigned int mpswdrdr1;
	unsigned int mpswdrdr2;
	unsigned int mpswdrdr3;
	unsigned int mpswdrdr4;
	unsigned int mpswdrdr5;
	unsigned int mpswdrdr6;
	unsigned int mpswdrdr7;
	unsigned int mpmur;
	unsigned int mpwrcadl;
	unsigned int mpdccr;
	unsigned int mpbc;
} MMDC_t;

typedef MMDC_t *pMMDC_t;

/********************* Profiler Types & Functions ************************/
typedef struct 
{
	unsigned int total_cycles;
	unsigned int busy_cycles;
	unsigned int read_accesses;
	unsigned int write_accesses;
	unsigned int read_bytes;
	unsigned int write_bytes;
	unsigned int data_load;
	unsigned int utilization;
	unsigned int access_utilization;
} MMDC_PROFILE_RES_t;

typedef enum 
{
    RES_FULL,
    RES_UTILIZATION
} MMDC_RES_TYPE_t;

struct MMDC_data_t{
pMMDC_t  __iomem base;
} mmdc_data[2];

static int enable;

/**************************** Functions ***************************************/


/************************ Profiler Functions **********************************/
static void start_mmdc_profiling(pMMDC_t mmdc)
{
	/*
	mmdc->madpcr0 = 0xA;		// Reset counters and clear Overflow bit
	mmdc->madpcr0 = 0x1;		// Enable counters
	*/
	writel(0xA, &(mmdc->madpcr0));
	writel(0x1, &(mmdc->madpcr0));
}

static void stop_mmdc_profiling(pMMDC_t mmdc)
{
	/*
	mmdc->madpcr0 = 0x0;		// Disable counters
	*/
	writel(0x0, &(mmdc->madpcr0));
}

static void pause_mmdc_profiling(pMMDC_t mmdc)
{
	/*
	mmdc->madpcr0 = 0x3;		// PRF_FRZ = 1
	*/
	writel(0x3, &(mmdc->madpcr0));
}

static void resume_mmdc_profiling(pMMDC_t mmdc)
{
	/*
	mmdc->madpcr0 = 0x1;		// PRF_FRZ = 0
	*/
	writel(0x1, &(mmdc->madpcr0));
}


static int get_mmdc_profiling_results(pMMDC_t mmdc, MMDC_PROFILE_RES_t *results)
{
	results->total_cycles 	= mmdc->madpsr0;
	results->busy_cycles 	= mmdc->madpsr1;
	results->read_accesses	= mmdc->madpsr2;
	results->write_accesses	= mmdc->madpsr3;
	results->read_bytes		= mmdc->madpsr4;
	results->write_bytes	= mmdc->madpsr5;
	results->utilization	= (int)(((float)results->read_bytes+(float)results->write_bytes)/((float)results->busy_cycles * 16) * 100);
	results->data_load  	= (int)((float)results->busy_cycles/(float)results->total_cycles * 100);
	results->access_utilization	= (int)(((float)results->read_bytes+(float)results->write_bytes)/((float)results->read_accesses + (float)results->write_accesses));
	return !(mmdc->madpcr0 & 0x8); 	// Return "0" if Overflow occurred
}

#if 0
static void print_mmdc_profiling_results(MMDC_PROFILE_RES_t results, MMDC_RES_TYPE_t print_type)
{
	if (print_type == RES_FULL)
	{
		printf("\nMMDC Profiling results:\n");
		printf("***********************\n");
		printf("Total cycles count: %d\n",results.total_cycles);
		printf("Busy cycles count: %d\n",results.busy_cycles);
		printf("Read accesses count: %d\n",results.read_accesses);
		printf("Write accesses count: %d\n",results.write_accesses);
		printf("Read bytes count: %d\n",results.read_bytes);
		printf("Write bytes count: %d\n",results.write_bytes);
	}
	printf("Utilization: %d%%\n",results.utilization);
	printf("Bus Load: %d%%\n",results.data_load);
	printf("Bytes\/Access: %d\n\n",results.access_utilization);
}
#endif

static const struct file_operations mmdc_fops = {
	.owner = THIS_MODULE,	
};

static ssize_t TCC_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	MMDC_PROFILE_RES_t results = {0};
	if (enable == 1)
	{
		get_mmdc_profiling_results(mmdc_data[0].base,&results);
	}
	return sprintf(buf, "Total cycles count: %u\n",results.total_cycles);
}

DEVICE_ATTR(TCC, 0644, TCC_show, NULL);

static ssize_t store_profile(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
	enable = simple_strtol(buf, NULL, 0);
	if (enable == 1) {
		start_mmdc_profiling(mmdc_data[0].base);
		start_mmdc_profiling(mmdc_data[1].base);
		pr_info("MMDC profile start \n");
	} else if (enable == 2){
		pause_mmdc_profiling(mmdc_data[0].base);	
		pause_mmdc_profiling(mmdc_data[1].base);	
		pr_info("MMDC profile pause \n");
	} else if (enable == 3){
		resume_mmdc_profiling(mmdc_data[0].base);	
		resume_mmdc_profiling(mmdc_data[1].base);	
		pr_info("MMDC profile resmue \n");
	}else {
		stop_mmdc_profiling(mmdc_data[0].base);
		stop_mmdc_profiling(mmdc_data[1].base);
		pr_info("MMDC profile stop \n");
	}

	return count;		
}

static ssize_t
show_profile(struct device *dev, struct device_attribute *attr, char *buf)
{
	return sprintf(buf, "%d", enable);
}


DEVICE_ATTR(profile, 0644, show_profile, store_profile);

static int __exit mmdc_remove(struct platform_device *pdev)
{
	iounmap((void *)mmdc_data[0].base);
	iounmap((void *)mmdc_data[1].base);
	return 0;
}

static struct platform_driver mmdc_driver = {
        .remove         = __exit_p(mmdc_remove),
        .driver         = {
                .name   = "mmdc_profile",
                .owner  = THIS_MODULE,
        },
};

static int __init mmdc_probe(struct platform_device *pdev)
{
	int ret;
	static struct clk *mmdc_clk0, *mmdc_clk1;

	static struct resource mmdc_resources[] = {
        {
                .start = MMDC_P0_BASE_ADDR,
                .end = MMDC_P0_BASE_ADDR + SZ_16K - 1,
                .flags = IORESOURCE_MEM,
        },
        {
                .start =  MMDC_P0_BASE_ADDR,
                .end =  MMDC_P0_BASE_ADDR + SZ_16K - 1,
                .flags = IORESOURCE_MEM,
        },
	};

	mmdc_data[0].base = ioremap(mmdc_resources[0].start,SZ_16K);
	mmdc_data[1].base = ioremap(mmdc_resources[1].start,SZ_16K);
	pr_info("probe MMDC profile Driver \n");
	mmdc_clk0 = clk_get(&pdev->dev, "mmdc_ch0_axi");
    if (IS_ERR(mmdc_clk0)) {
		dev_err(&pdev->dev, "no mmdc0 clock.\n");
		return PTR_ERR(mmdc_clk0);
    }
    ret = clk_enable(mmdc_clk0);
    if (ret) {
		dev_err(&pdev->dev, "can't enable mmdc0 clock.\n");
		goto put_clk;
    }
	mmdc_clk1 = clk_get(&pdev->dev, "mmdc_ch1_axi");
    if (IS_ERR(mmdc_clk1)) {
		dev_err(&pdev->dev, "no mmdc1 clock.\n");
		return PTR_ERR(mmdc_clk1);
    }
    ret = clk_enable(mmdc_clk1);
    if (ret) {
		dev_err(&pdev->dev, "can't enable mmdc1 clock.\n");
		goto put_clk;
    }

	ret = device_create_file(&pdev->dev, &dev_attr_TCC);
	ret |= device_create_file(&pdev->dev, &dev_attr_profile);
	return ret;
#if 0
release_clk:
	clk_disable(mmdc_clk0);
	clk_disable(mmdc_clk1);
#endif
put_clk:
	clk_put(mmdc_clk0);
	clk_put(mmdc_clk1);
	return ret;
}

static struct platform_device * mmdc_device;

static int __init  mmdc_init(void)
{
    int ret;	
	mmdc_device = platform_device_register_simple("mmdc_profile",
                                -1, NULL, 0);
     if (IS_ERR(mmdc_device)) {
                printk(KERN_ERR "mmdc: platform_device_register failed.\n");
                ret = PTR_ERR(mmdc_device);
                goto out;
     }
	return platform_driver_probe(&mmdc_driver, mmdc_probe);;
out:
	platform_device_unregister(mmdc_device);
	return ret;
}

static void  __exit mmdc_exit(void)
{
	platform_device_unregister(mmdc_device);
	platform_driver_unregister(&mmdc_driver);
}


module_init(mmdc_init);
module_exit(mmdc_exit);

