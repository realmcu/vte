/***
**Copyright 2004-2009 Freescale Semiconductor, Inc. All Rights Reserved.
**
**The code contained herein is licensed under the GNU General Public
**License. You may obtain a copy of the GNU General Public License
**Version 2 or later at the following locations:
**
**http://www.opensource.org/licenses/gpl-license.html
**http://www.gnu.org/copyleft/gpl.html
**/
/*================================================================================================*/
/**
        @file   sdma_test_module.c

        @brief  SDMA API

====================================================================================================
Revision History:
                        Modification     Tracking
Author/core ID              Date          Number        Description of Changes
--------------------    ------------    ----------      -------------------------------------------
S. ZAVJALOV/------       13/07/2004     TLSbo40259      Initial version
L. DELASPRE/rc149c       13/08/2004     TLSbo40891      VTE 1.4 integration
C.GAGNERAUD/cgag1c       26/10/2004     TLSbo43815      Fix compilation warnings, add arg initialisation
                                                        for mxc_dma_setup_channel()
V.HALABUDA/HLBV001       05/07/2005     TLSbo52340      Update version for linux-2.6.11-CVS and fix
I.Inkina/nknl001         02/08/2005     TLsbo49843      Update initializing the memory
A.Ozerov/B00320          10/02/2006     TLSbo61734      Code was cast to coding conventions
A.Ozerov/b00320          11/12/2006     TLSbo84161      Minor changes.
D.Kardakov               07/30/2007     ENGR43546       "SDMA script not found" error was fixed.  
====================================================================================================
Portability:  ARM GCC
==================================================================================================*/

/*==================================================================================================
                                        INCLUDE FILES
==================================================================================================*/
/* Standard Include Files */
#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>       /* Added on 05/01/06 by Bunloeur Sean */
#include <linux/version.h>

/* Harness Specific Include Files. */
#include <linux/fs.h>
#include <stdarg.h>

/* API's Specific Include Files. */
#include <asm/arch/dma.h>

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,26))
#ifndef class_device_destroy
#define class_device_destroy  device_destroy
#endif
#ifndef class_device_create
#define class_device_create(cs, NULL, dev, parent, fmt,args...)  device_create(cs, parent, dev, fmt, ##args)
#endif
#endif

/*==================================================================================================
                                        DEFINES AND MACROS
==================================================================================================*/
#define MXC_SDMA_TEST_1         1
#define MXC_SDMA_TEST_2         2
#define MXC_SDMA_TEST_3         3

#define SDMA_DEV "sdma_test_module"

/*==================================================================================================
                                STRUCTURES AND OTHER TYPEDEFS
==================================================================================================*/
typedef struct
{
        unsigned int channel;
        unsigned int param;
        unsigned int bd_index;
} test_param;

/*==================================================================================================
                                    FUNCTION PROTOTYPES
==================================================================================================*/
static int mxc_module_ioctl(struct inode *inode, struct file *file, unsigned int cmd,
                            unsigned long arg);
int     mxc_sdma_test1(unsigned int arg, sdma_periphT ptype);
int     mxc_sdma_test2(unsigned int arg, unsigned int mass_num);
int     mxc_sdma_test3(unsigned int arg);

static int sdma_test_init(void);
void    sdma_test_exit(void);

void   *malloc_sdma(size_t size);
void    free_sdma(void *buf);

/*==================================================================================================
                                GLOBAL VARIABLE DECLARATIONS
==================================================================================================*/
int     dev_num;

static struct class *sdma_class;        /* added on 05/01/06 Bunloeur Sean */

static struct file_operations sdma_module_fops = 
{
        owner:THIS_MODULE,
        ioctl:mxc_module_ioctl,
};

/*================================================================================================*/
void sdma_test_callback(void *arg)
{
        return;
}

/*================================================================================================*/
void   *malloc_sdma(size_t size)
{
        void   *buf;

        buf = kmalloc(size, GFP_ATOMIC | GFP_DMA);
        return buf;
}

/*================================================================================================*/
void free_sdma(void *buf)
{
        kfree(buf);
}

/*================================================================================================*/
int mxc_sdma_test1(unsigned int arg, sdma_periphT ptype)
{
        int     channel[1];
        int     err;
        dma_channel_params c_params[1];
        dma_request_t r_params[1];

        *channel = arg;

        if ((err = mxc_request_dma(channel, "TEST1")) < 0)
        {
                printk(KERN_WARNING "Fail in %s: got channel %d\n", __FUNCTION__, *channel);
                return err;
        }

        /* default buffer descriptor */
        c_params->bd_number = -1;

        c_params->peripheral_type = ptype;
        switch (ptype)
        {
        case SSI:
        case MMC:
        case EXT:
                c_params->transfer_type = emi_2_per;
                c_params->event_id = 2; // How to choose this value?
                break;
        case MEMORY:
                c_params->transfer_type = emi_2_emi;
                break;
        default:
                printk(KERN_WARNING "Unhandle peripheral type: %d\n", ptype);
                mxc_free_dma(*channel);
                return -EINVAL;
                break;
        }

        if ((err = mxc_dma_setup_channel(*channel, c_params)) != 0)
        {
                printk(KERN_WARNING "Fail in %s: mxc_dma_setup_channel err = %d\n", __FUNCTION__,
                       err);
                mxc_free_dma(*channel);
                return err;
        }
        /* 
        * r_params->sourceAddr = src; r_params->destAddr = dest; r_params->count = 16*sizeof(__u32); */
        if ((err = mxc_dma_set_config(*channel, r_params, 0)) != 0)
        {
                printk(KERN_WARNING "Fail in %s: mxc_dma_set_config err = %d\n", __FUNCTION__, err);
                mxc_free_dma(*channel);
                return err;
        }

        if ((err = mxc_dma_get_config(*channel, r_params, 0)) != 0)
        {
                printk(KERN_WARNING "Fail in %s: mxc_dma_get_config err = %d\n", __FUNCTION__, err);
                return err;
        }

        mxc_free_dma(*channel);
        return 0;
}

/*================================================================================================*/
int mxc_sdma_test2(unsigned int arg, unsigned int mass_num)
{
        int     i,
                ret = 0;
        dma_channel_params c_params[1]; // Channel parameters
        dma_request_t r_params[1];      // Request parameters
        __u32  *src;
        __u32  *dest;

        int     channel[1];     // Will store a channel number
        int     err;

        *channel = arg;

        memset(c_params, 0, sizeof(dma_channel_params));
        memset(r_params, 0, sizeof(dma_request_t));
        if ((err = mxc_request_dma(channel, "TEST2")) != 0)
        {
                printk(KERN_WARNING "Fail in %s: mxc_request_dma err = %d\n", __FUNCTION__, err);
                return err;
        }
        /* 
        * Peripheral type is MEMORY. The full list of peripheral types defined in sdma_periphT enum.
        * Pay attention that there are peripherals , like UART for instance, that have different
        * peripheral types. This is because there are UARTs connected to SDMA via MAX, and others via
        * SPBA. Fail in these cases 2 different SDMA scripts used for each one of the UARTs. '_SP'
        * extention says that this is the UART that connected via SPBA. */
        c_params->peripheral_type = MEMORY;
        /* 
        * Transfer types. The full list of transfer types defined in sdma_transferT enum. Fail in
        * general when we talk about real peripheral (not MEMORY), each peripheral can use 4 types
        * of transfers: per_2_emi and emi_2_per are transfers from/to EMI to/from peripheral.
        * per_2_int and int_2_per are transfers from/to internal RAM to/from peripheral. emi_2_emi
        * is used for memory transfers. */
        c_params->transfer_type = emi_2_emi;
        /* 
        * Fail in peripheral transfers 3 additional parameters must be set: watermark_level - number
        * of bytes transferred on peripheral's dma request per_address - address of peripheral's
        * FIFO or buffer. Fail in case of FIRI, UART receive and ATA it the base address of
        * peripheral. event_id - event number */
        if ((err = mxc_dma_setup_channel(*channel, c_params)) != 0)
        {
                printk(KERN_WARNING "Fail in %s: mxc_dma_setup_channel err = %d\n", __FUNCTION__,
                       err);
                mxc_free_dma(*channel);
                return err;
        }


        src = sdma_malloc(mass_num * sizeof(__u32));
        dest = sdma_malloc(mass_num * sizeof(__u32));


        for (i = 0; i < mass_num; i++)
        {
                src[i] = i;
                dest[i] = 0;
        }
        /* 
        * Here we set the src address and destination address of memory in specific request. Fail in
        * peripheral transfers only one parameter should be set, depending on direction. Pay
        * attention, that peripheral's source or destination address is set previously during setup
        * channel. */
        r_params->sourceAddr = (__u8 *) sdma_virt_to_phys(src);
        r_params->destAddr = (__u8 *) sdma_virt_to_phys(dest);
        /* 
        * number of bytes to transfer */
        r_params->count = mass_num * sizeof(__u32);
        /* 
        * callback function. If it is not set, no response will be after the end of transfer
        * r_params->callback = sdma_test_callback; //(dma_callback_t) */

        if ((err = mxc_dma_set_config(*channel, r_params, 0)) != 0)
        {
                printk(KERN_WARNING "Fail in %s: mxc_dma_set_config err = %d\n", __FUNCTION__, err);
                mxc_free_dma(*channel);
                return err;
        }
        /* 
        * Here we enable the channel. After number of bytes that was set in count field is
        * transferred callback function will run (if it is set). And we will need to setup a new
        * request. Fail in peripheral case, it is important to understand the meaning of watermark
        * level parameter, that we set in channel parameters, and count parameter of request.
        * Watermark level parameter is a number of bytes that must be transferred every on every
        * peripheral dma request. Count is a number of bytes that the memory buffer can hold.
        * Callback function will be called after count bytes are transferred. Watermark level can be
        * bigger or smaller than the count. If watermark_level < count, then peripheral will trigger
        * several dma requests. On each peripheral request watermark_level bytes will be
        * transferred. After count bytes will be transferred, callback function will run and after
        * that we need to setup new request parameters and restart the channel. */
        if ((err = mxc_dma_start(*channel)) != 0)
        {
                printk(KERN_WARNING "Fail in %s: mxc_dma_start err = %d\n", __FUNCTION__, err);
                mxc_free_dma(*channel);
                return err;
        }

        printk(KERN_INFO "Cheking the source and the destination for equality...\n");
        for (i = 0; i < mass_num; i++)
        {
                if (src[i] != dest[i])
                {
                        printk(KERN_INFO "src[%d] = %d \t\t\t dest[%d] = %d\n", i, src[i], i,
                               dest[i]);
                        ret = 1;
                }
        }

        mxc_free_dma(*channel);
        sdma_free(src);
        sdma_free(dest);
        return ret;
}

/*================================================================================================*/
int mxc_sdma_test3(unsigned int arg)
{
        int     channel[1];
        int     err;

        *channel = arg;

        if ((err = mxc_request_dma(channel, "TEST3")) < 0)
        {
                printk(KERN_WARNING "Fail in %s: got channel %d\n", __FUNCTION__, *channel);
                return err;
        }

        if (channel == 0)
        {
                printk(KERN_WARNING "Channel request returned 0\n");
                return 1;
        }

        if ((err = mxc_request_dma(channel, "TEST1")) < 0)
        {
                mxc_free_dma(*channel);
                return 0;
        }

        mxc_free_dma(*channel);
        return 1;
}

/*================================================================================================*/
static int mxc_module_ioctl(struct inode *inode, struct file *file,
                            unsigned int cmd, unsigned long arg)
{
        int     ret = 1;
        unsigned int channel = ((test_param *) arg)->channel;
        unsigned int param = ((test_param *) arg)->param;

        // unsigned int bd_index = ((test_param*)arg)->bd_index;

        switch (cmd)
        {
        case MXC_SDMA_TEST_1:
                ret = mxc_sdma_test1(channel, param);
                break;
        case MXC_SDMA_TEST_2:
                ret = mxc_sdma_test2(channel, param);
                break;
        case MXC_SDMA_TEST_3:
                ret = mxc_sdma_test3(channel);
                break;
        }
        return ret;
}

/*================================================================================================*/
static int sdma_test_init(void)
{
        if ((dev_num = register_chrdev(0, SDMA_DEV, &sdma_module_fops)) < 0)
        {
                printk(KERN_WARNING "MXC Module Test: unable to register the dev\n");
                return 1;
        }

        sdma_class = class_create(THIS_MODULE, SDMA_DEV);
        if (IS_ERR(sdma_class))
        {
                printk(KERN_ALERT "class simple created failed\n");
                goto err_out;
        }

        if (IS_ERR(class_device_create(sdma_class, NULL, MKDEV(dev_num, 0), NULL, SDMA_DEV)))
        {
                printk(KERN_ALERT "class simple add failed\n");
                goto err_out;
        }

        printk(KERN_INFO "Module load succesful\n");
        return 0;

        err_out:
        printk(KERN_ERR "sdma : error creating sdma test module class.\n");
        class_device_destroy(sdma_class, MKDEV(dev_num, 0));
        class_destroy(sdma_class);
        unregister_chrdev(dev_num, SDMA_DEV);
        return -1;


}

/*================================================================================================*/
void sdma_test_exit(void)
{
        unregister_chrdev(dev_num, "sdma_test_module");
        class_device_destroy(sdma_class, MKDEV(dev_num, 0));
        class_destroy(sdma_class);
}

module_init(sdma_test_init);
module_exit(sdma_test_exit);

MODULE_DESCRIPTION("Test Module for SDMA MXC91331 drivers");
MODULE_LICENSE("GPL");
