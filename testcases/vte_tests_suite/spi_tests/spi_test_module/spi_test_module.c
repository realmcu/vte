/***
**Copyright 2005-2009 Freescale Semiconductor, Inc. All Rights Reserved.
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
        @file   spi_test_module.c

        @brief  LTP MXC SPI test module.
====================================================================================================
Revision History:
                            Modification     Tracking
Author/core ID                  Date          Number    Description of Changes
-------------------------   ------------    ----------  --------------------------------------------
A.Smirnov                    16/06/2005     TLSbo51450  Initial version
V.Khalabuda/hlbv001          30/06/2005     TLSbo52341  Update version for linux-2.6.11-CVS and fix
I.Semenchukov/smng001c       14/07/2005     TLSbo52341  Fix bug that causes makes test to stuck
S.Bezrukov/sbazr1c           22/09/2005     TLSbo51450  Decrease kernel tick counter to stabilize test
V.Khalabuda/b00306           17/04/2006     TLSbo67010  Update version for linux-2.6.10-rel-L26_1_17
D.Kazachkov/b00316           30/05/2006     TLSbo67010  Update version for linux-2.6.10-rel-L26_1_18
V.Khalabuda/b00306           17/04/2006     TLSbo72876  Arrangement of device create using classes
D.Khoroshev/b00313           02/01/2006     TLSbo86657  Adaptation to linux 2.6.18 spi driver model

====================================================================================================
Portability: ARM GCC

==================================================================================================*/

/*==================================================================================================
Total Tests: 1

Test Name:   spi_test_module.ko

Test Assertion
& Strategy:  A brief description of the test Assertion and Strategy
            TO BE COMPLETED
==================================================================================================*/

/*==================================================================================================
                                        INCLUDE FILES
==================================================================================================*/
#include <linux/interrupt.h>
#include <linux/autoconf.h>

/* Common API's Include Files */
#include <linux/delay.h>
/* Verification Test Environment Include Files */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/device.h>      /* Added on 05/01/06 by Bunloeur Sean */
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/err.h>
#include <linux/fs.h>

#include <linux/kdev_t.h>
#include <linux/major.h>
#include <linux/string.h>
#include <linux/version.h>

#include <asm/irq.h>
#include <mach/gpio.h>
#include <mach/hardware.h>
#include <asm/mach-types.h>
#include <linux/semaphore.h>
#include <asm/uaccess.h>

#include <linux/spi/spi.h>
#include "spi_test_module.h"

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,28))

#ifndef class_device_destroy
#define class_device_destroy  device_destroy
#endif
#ifndef class_device_create
#define class_device_create(cs, NULL, dev, parent, fmt,args...)  device_create(cs, parent, dev, NULL, fmt, ##args)
#endif
#ifndef class_device
#define class_device device
#endif

#else

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,26))
#ifndef class_device_destroy
#define class_device_destroy  device_destroy
#endif
#ifndef class_device_create
#define class_device_create(cs, NULL, dev, parent, fmt,args...)  device_create(cs, parent, dev, fmt, ##args)
#endif
#ifndef class_device
#define class_device device
#endif
#endif

#endif

#define DPRINTK(fmt, args...) printk(fmt, ##args)

#define SPI1 0
#define SPI2 1
#define SPI3 2

int major_spi;
static struct class *spi_class;

int major_spi = 0;

struct spi_driver *spi_test_drv = NULL;

struct spi_priv_data
{
        struct spi_device *spi;
        struct semaphore sem;
        char readbuffer[BUFF_TEST_MAX_SIZE];
        char writebuffer[BUFF_TEST_MAX_SIZE];
        int rem_count;
};

static struct spi_priv_data *spi_priv[3] = { 0, 0, 0 };

/* lock used to control access to spi registers */

int spi_test_setup(struct spi_device *spi);

static inline int spi_rw(struct spi_device *spi, u8 * buf, size_t len)
{
        struct spi_transfer t = {
                .tx_buf = (const void *)buf,
                .rx_buf = buf,
                .len = len,
                .cs_change = 0,
                .delay_usecs = 0,
        };
        struct spi_message m;

        spi_message_init(&m);
        spi_message_add_tail(&t, &m);
        if (spi_sync(spi, &m) != 0 || m.status != 0)
                return -1;
        return (len - m.actual_length);
}

static int spi_test_open(struct inode *inode, struct file *file)
{
        unsigned int minor = MINOR(inode->i_rdev);

        DPRINTK("(%s)minor = %d\n", __FUNCTION__, minor);

        file->private_data = spi_priv[minor];
        return 0;
}

static int spi_test_release(struct inode *inode, struct file *file)
{
        unsigned int minor = MINOR(inode->i_rdev);

        DPRINTK("(%s)minor = %d\n", __FUNCTION__, minor);

        file->private_data = NULL;
        return 0;
}

static ssize_t spi_test_read(struct file* file, char* buf, size_t bytes, loff_t * ppos)
{
        struct spi_priv_data* priv;
        unsigned int minor;
        int res;

        minor = MINOR(file->f_dentry->d_inode->i_rdev);
        DPRINTK("(%s)minor = %d\n", __FUNCTION__, minor);


        if (bytes > BUFF_TEST_MAX_SIZE)
                bytes = BUFF_TEST_MAX_SIZE;

        priv = (struct spi_priv_data*)file->private_data;

        down(&priv->sem);

        if (bytes > priv->rem_count)
                bytes = priv->rem_count;
        
        res = copy_to_user((void*)buf, (void*)priv->readbuffer, bytes);
        if (res > 0)
        {
                up(&priv->sem);
                return -EFAULT;
        }
        
        priv->rem_count -= bytes;
        if (priv->rem_count > 0)
        {
                /* shift remaining data if not all was read */
                int i;
                for(i=0; i < priv->rem_count; i++)
                        priv->readbuffer[i] = priv->readbuffer[bytes+i];
        }

        up(&priv->sem);

        return bytes;
}

static ssize_t spi_test_write(struct file* file, const char* buf, size_t count, loff_t * ppos)
{
        struct spi_priv_data* priv;
        unsigned int minor;
        int res = 0;

        minor = MINOR(file->f_dentry->d_inode->i_rdev);

        DPRINTK("(%s)minor = %d\n", __FUNCTION__, minor);

        if (count > BUFF_TEST_MAX_SIZE)
                count = BUFF_TEST_MAX_SIZE;

        priv = (struct spi_priv_data*)file->private_data;

        down(&priv->sem);

        memset(priv->writebuffer, 0, BUFF_TEST_MAX_SIZE);
        res = copy_from_user((void*)priv->writebuffer, (void*)buf, count);
        if (res > 0)
        {
                up(&priv->sem);
                return -EFAULT;
        }

        res = spi_rw(priv->spi, priv->writebuffer, count);

        if (count - res > BUFF_TEST_MAX_SIZE - priv->rem_count)
        {
                memcpy(priv->readbuffer, priv->writebuffer, count - res);
                priv->rem_count = count - res;
        }
        else
        {
                memcpy(priv->readbuffer + priv->rem_count, priv->writebuffer, count - res);
                priv->rem_count += count - res;
        }

        if(res == 0)
        {
                
                DPRINTK("All data was sent to SPI\n");
        }
        else
        {
                DPRINTK("Error: %d bytes has been left\n", res);
        }

        up(&priv->sem);
        return res;
}

static int spi_test_ioctl(struct inode *inode, struct file *file, unsigned int cmd, unsigned long arg)
{
        int     res = 0;
        struct  spi_priv_data *priv;
        /* int     minor = MINOR(file->f_dentry->d_inode->i_rdev); */
        u8 buf[4] = {};

        priv = (struct spi_priv_data*)file->private_data;

        switch (cmd)
        {

        case SPI_SEND_FRAME:
                {        
                unsigned long ref_jiffy, time_diff;
                int i, ret = 0;
                ref_jiffy = jiffies;
                memset(buf, 0, sizeof buf);
                for (i = 0; i < arg; i++)
                {
                        ret = spi_write(priv->spi, buf, 1);
                
                }
                time_diff = jiffies - ref_jiffy;

                /* Time measured in jiffies */

                if (time_diff <= 2)
                        res = -1;

                DPRINTK(KERN_ALERT "Time diff: %lu\n", time_diff);
                }
                break;
        default:
                DPRINTK("spi test module : ioctl() unknow !\n");
                return -EINVAL;
        }
        return res;
}

/*! This structure defines file operations for a SPI device. */
static struct file_operations spi_test_fops =
{
        owner:          THIS_MODULE,
        open:           spi_test_open,
        release:        spi_test_release,
        read:           spi_test_read,
        write:          spi_test_write,
        ioctl:          spi_test_ioctl,
};

static int __devinit spi_test_probe(struct spi_device *spi)
{
        int ret = 0;
        int master = -1;
        struct class_device *temp_class;
        char device_name[10];

        ret = spi_test_setup(spi);
        if (ret != 0) {
                return -1;
        }
        
        if (!strcmp(spi->dev.bus_id, "spi1.4"))
        {
                master = 0;
        }
        else if (!strcmp(spi->dev.bus_id, "spi2.4"))
        {
                master = 1;
        }
        else if(!strcmp(spi->dev.bus_id, "spi3.4"))
        {
                master = 2;
        }

        if (master < 0)
        {
                DPRINTK("Unknown device %s\n", spi->dev.bus_id);
                return -EINVAL;
        }

        spi_priv[master] = (struct spi_priv_data*)kmalloc(sizeof(struct spi_priv_data),
                                                                        GFP_KERNEL);
        spi_priv[master]->rem_count = 0;
        spi_priv[master]->spi = spi;
        sema_init(&spi_priv[master]->sem, 1);
        
        snprintf(device_name, 10, DEV_MXC_SPI "%d", master);                
        
        temp_class =
                class_device_create(spi_class, NULL,
                                    MKDEV(major_spi, master), NULL,
                                    device_name);

        if (IS_ERR(temp_class)) {
                DPRINTK(KERN_ERR "Error creating spi_class class device %s.\n",
                                                                        device_name);
                kfree(spi_priv[master]);
                return PTR_ERR(temp_class);
        }

        DPRINTK("SPI%d successfully loaded. Bus id %s\n", master, spi->dev.bus_id);

        return 0;
}

static int __devexit spi_test_remove(struct spi_device *spi)
{
        int master = -1;
        DPRINTK(KERN_INFO "Device %s removed\n", spi->dev.bus_id);

        if (!strcmp(spi->dev.bus_id, "spi1.4"))
        {
                master = 0;
        }
        else if (!strcmp(spi->dev.bus_id, "spi2.4"))
        {
                master = 1;
        }
        else if (!strcmp(spi->dev.bus_id, "spi3.4"))
        {
                master = 2;
        }

        if (master < 0)
        {
                DPRINTK("Unknown device %s\n", spi->dev.bus_id);
                return -EINVAL;        
        }
        
        kfree(spi_priv[master]);
        class_device_destroy(spi_class, MKDEV(major_spi, master));        

        return 0;
}

static int spi_test_suspend(struct spi_device *spi, pm_message_t message)
{
        return 0;
}

/*!
 * This function brings the SPI slave device back from low-power mode/state.
 *
 * @param       spi     the SPI slave device
 *
 * @return      Returns 0 on SUCCESS and error on FAILURE.
 */
static int spi_test_resume(struct spi_device *spi)
{
        return 0;
}

/*!
 * This structure contains pointers to the power management callback functions.
 */
static struct spi_driver spi_test_driver = {
        .driver = {
                    .name = "loopback_spi",
                    .bus = &spi_bus_type,
                    .owner = THIS_MODULE,
                    },
        .probe = spi_test_probe,
        .remove = __devexit_p(spi_test_remove),
        .suspend = spi_test_suspend,
        .resume = spi_test_resume
};

int spi_test_setup(struct spi_device *spi)
{
        /* Setup the SPI slave i.e.PMIC */
        DPRINTK("spi test driver %p\n", &spi_test_driver);

        spi->mode = SPI_MODE_2;
        spi->bits_per_word = 32;

        return spi_setup(spi);
}

/*================================================================================================*/
/*===== spi_module_init =====*/
/**
@brief  This function implements the init function of the SPI device.
        This function is called when the module is loaded.

@param  None

@return This function returns 0.
*/
/*================================================================================================*/
static int __init spi_module_init(void)
{
        int ret=0;

        DPRINTK("Loading spi test module...\n");
        major_spi = register_chrdev(0, DEV_MXC_SPI, &spi_test_fops);
        if (major_spi < 0)
        {
                DPRINTK(KERN_WARNING "Unable to get a major for spi");
                return major_spi;
        }        

        spi_class = class_create(THIS_MODULE, DEV_MXC_SPI);
        if (IS_ERR(spi_class))
                goto err_out1;

        DPRINTK("Registering spi driver...\n");
        return spi_register_driver(&spi_test_driver);

        err_out1:
        unregister_chrdev(major_spi, DEV_MXC_SPI);
        return ret;
}

static void __exit spi_module_exit(void)
{
        class_destroy(spi_class);
        unregister_chrdev(major_spi, DEV_MXC_SPI);

        spi_unregister_driver(&spi_test_driver);
        DPRINTK("SPI driver: successfully unregistered\n");
}

subsys_initcall(spi_module_init);
module_exit(spi_module_exit);

MODULE_DESCRIPTION("SPI test device driver");
MODULE_AUTHOR("Freescale Semiconductor, Inc.");
MODULE_LICENSE("GPL");
