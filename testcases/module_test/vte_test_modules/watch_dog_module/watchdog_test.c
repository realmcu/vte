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
        @file   watchdog_test.c

        @brief  This file contains the implementation for the /dev interface.
====================================================================================================
Revision History:
                            Modification     Tracking
Author/core ID                  Date          Number    Description of Changes
-------------------------   ------------    ----------  -------------------------------------------
C.Gagneraud/cgag1c           09/11/2004     TLSbo44474  Warnings fixup.
S.V-Guilhou/svan01c          19/08/2005     TLSbo53364  Adapt test suite for MXC9113
V.Khalabuda/b00306           06/07/2006     TLSbo63552  Update for ArgonLV support

====================================================================================================
Portability:  ARM GCC
==================================================================================================*/

/*==================================================================================================
                                        INCLUDE FILES
==================================================================================================*/
/* Standard Include Files */
//lily #include <linux/config.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/version.h>
#include <linux/fs.h>
#include <linux/device.h>       /* Added on 05/01/06 by Bunloeur Sean */
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/interrupt.h>
#include <asm/uaccess.h>

#include "watchdog_test.h"

/*==================================================================================================
                                        GLOBAL VARIABLES
==================================================================================================*/
extern volatile unsigned short g_wdog1_enabled;
extern volatile unsigned short g_wdog2_enabled;
extern void mxc_wd_init(int port);
static struct class *wdog_class;

/*==================================================================================================
                                    FUNCTION PROTOTYPES
==================================================================================================*/

/*================================================================================================*/
/*===== watchdog_test_open =====*/
/**
@brief  This function implements the open method on a WATCHDOG_TEST device.

@param  inode       pointer on the node
        file        pointer on the file

@return This function returns 0.
*/
/*================================================================================================*/
static int watchdog_test_open(struct inode *inode, struct file *file)
{
        printk("watchdog_test : watchdog_test_open()\n");
        return 0;
}

static ssize_t watchdog_test_read(struct file *file, char *buf, size_t count,
                                loff_t * ppos)
{
        return 0;
}

static ssize_t watchdog_test_write(struct file *filp, const char *buf,
                                 size_t count, loff_t * ppos)
{
        return 0;
}

/*================================================================================================*/
/*===== watchdog_test_free =====*/
/**
@brief  This function implements the release method on a WATCHDOG_TEST device.

@param  inode       pointer on the node
        file        pointer on the file

@return This function returns 0.
*/
/*================================================================================================*/
static int watchdog_test_free(struct inode *inode, struct file *file)
{
        printk("watchdog_test : watchdog_test_free()\n");
        return 0;
}

static struct class *wdog_class; /* added on 05/01/06 Bunloeur Sean */

/* watchdog_test major */
//static int major_watchdog_test1;

#if defined(CONFIG_2_WDG)
/*================================================================================================*/
/*===== mxc_wdog_int =====*/
/**
@brief  This is the interrupt service routine for the watchdog timer.
        It occurs only when a watchdog is enabled but not gets serviced in time.
        This is a stub function as it just prints out a message now: WATCHDOG times out: <irq src numer>

@param  irq          wdog interrupt source number
        dev_id       this parameter is not used
        regs         pointer to a structure containing the processor registers and state prior to servicing the interrupt

@return always returns \b IRQ_HANDLED as defined in include/linux/interrupt.h.
*/
/*================================================================================================*/
static irqreturn_t mxc_wdog_int(int irq, void *dev_id, struct pt_regs *regs)
{
        printk("\nWATCHDOG times out: %d\n", irq);
        return IRQ_HANDLED;
}

/* ! This function binds the watchdog isr to the 2nd WATCHDOG timer interrupt. */
static int mxc_wdog2_int_bind(void)
{
        int     ret = 0;

        if (request_irq(INT_WDOG2, mxc_wdog_int, SA_INTERRUPT, "WDOG2", NULL))
        {
                printk(KERN_ERR "%s: IRQ%d already in use.\n", "WDOG2", INT_WDOG2);
                ret = -1;
        }
        return ret;
}
#endif

static int mxc_watchdog_test(int *arg)
{
        spinlock_t wdog_lock = SPIN_LOCK_UNLOCKED;
        unsigned long wdog_flags,
                i = 0;
        int     test_case = *arg;
        int     result = 0;

        switch (test_case)
        {
        case 0:
                /* Normal opeation with both WDOGs enabled - System operates as normal. */
                g_wdog1_enabled = 1;
                mxc_wd_init(0);
#if defined(CONFIG_2_WDG)
                g_wdog2_enabled = 1;
                mxc_wd_init(1);
#endif
                break;
        case 1:
                /* Both WDOGs enabled but stuck in a loop with all interrupts disabled - System
                * hangs. */
                g_wdog1_enabled = 1;
                mxc_wd_init(0);
#if defined(CONFIG_2_WDG)
                g_wdog2_enabled = 1;
                mxc_wd_init(1);
#endif

                spin_lock_irqsave(&wdog_lock, wdog_flags);
                while (1)
                {
                        if ((i++ % 100000) == 0)
                        {
                                printk("1st watchdog test\n");
                        }
                }
                spin_unlock_irqrestore(&wdog_lock, wdog_flags);
                break;
        case 2:
                /* The WDOG1 not serviced (both WDOGs are enabled) - System hangs. */
                g_wdog1_enabled = 0;
                mxc_wd_init(0);
#if defined(CONFIG_2_WDG)
                g_wdog2_enabled = 1;
                mxc_wd_init(1);
#endif
                break;
#if defined(CONFIG_2_WDG)
        case 3:
                /* Only the WDOG2 enabled but not serviced -
                 * Stuck in the isr with "WATCHDOG times out: 55" messages displayed */
                g_wdog2_enabled = 0;
                mxc_wdog2_int_bind();
                mxc_wd_init(1);
                break;
        case 4:
                /* Both WDOGs enabled but the WDOG2 not serviced -
                 * First many "WATCHDOG times out: 55" messages displayed; Then
                 * the system will hang due to the 1st WDOG not being serviced. */
                g_wdog1_enabled = 1;
                mxc_wd_init(0);
                mxc_wdog2_int_bind();
                g_wdog2_enabled = 0;
                mxc_wd_init(1);
                break;
#endif
        default:
                printk("invalid WDOG test case: %d\n", test_case);
                result = 1;
                break;
        }

        return result;
}

/*================================================================================================*/
/*===== watchdog_test_ioctl =====*/
/**
@brief  This function implements IOCTL for second watchdog test

@param  inode       pointer on the node
        file        pointer on the file
        cmd         the command
        arg         the parameter

@return This function returns 0 if successful.
*/
/*================================================================================================*/
static int watchdog_test_ioctl(struct inode *inode, struct file *file, unsigned int cmd,
                               unsigned long arg)
{
        int     result = 0;
        ulong *tempu32ptr, tempu32;

        tempu32 = (ulong) (*(ulong *) arg);
        tempu32ptr = (ulong *) arg;

        printk("watchdog_test : ioctl(), cmd : %d\n", cmd);

        switch (cmd)
        {
        case MXCTEST_WATCHDOG:
                printk("Starting watchdog test !!\n");
                result = mxc_watchdog_test((int *) arg);
                break;

        default:
                printk("Unknown case !!\n");
                result = -1;
                break;

        }

        return result;
}

/*==================================================================================================
                                GLOBAL VARIABLE DECLARATIONS
==================================================================================================*/
/* This structure defines file operations for a WATCHDOG_TEST device */
static struct file_operations watchdog_test_fops =
{
        owner:        THIS_MODULE,
        open:         watchdog_test_open,
        release:      watchdog_test_free,
        read:         watchdog_test_read,
        write:        watchdog_test_write,
        ioctl:        watchdog_test_ioctl,
};

/*================================================================================================*/
/*===== watchdog_test_init =====*/
/**
@brief  This function implements the init function of the WATCHDOG_TEST device.
        This function is called when the module is loaded.

@param  None

@return This function returns 0.
*/
/*================================================================================================*/
static int __init watchdog_test_init(void)
{
        struct class_device *temp_class;
        int res;

        res = register_chrdev(MXC_WDOG_TM_MAJOR, "wd_tst", &watchdog_test_fops);

        if (res < 0)
        {
                printk(KERN_WARNING "MXC Test: unable to register the dev\n");
                return res;
        }

        wdog_class = class_create(THIS_MODULE, "wd_tst");
        if (IS_ERR(wdog_class))
        {
                printk(KERN_ERR "Error creating mxc_wdog_tm class.\n");
                //---devfs_remove("wd_tst");
                unregister_chrdev(MXC_WDOG_TM_MAJOR, "wd_tst");
                class_device_destroy(wdog_class, MKDEV(MXC_WDOG_TM_MAJOR, 0));
                return PTR_ERR(wdog_class);
        }

        temp_class = class_device_create(wdog_class,NULL,
                                             MKDEV(MXC_WDOG_TM_MAJOR, 0), NULL, "wd_tst");
        if (IS_ERR(temp_class))
        {
                printk(KERN_ERR "Error creating mxc_wdog_tm class device.\n");
                class_device_destroy(wdog_class, MKDEV(MXC_WDOG_TM_MAJOR, 0));
                class_destroy(wdog_class);
                //---devfs_remove("wd_tst");
                unregister_chrdev(MXC_WDOG_TM_MAJOR, "wd_tst");
                return -1;
        }

        //---devfs_mk_cdev(MKDEV(MXC_WDOG_TM_MAJOR, 0), S_IFCHR | S_IRUGO | S_IWUGO, "wd_tst");
        return 0;
}

/*================================================================================================*/
/*===== watchdog_test_exit =====*/
/**
@brief  This function implements the exit function of the WATCHDOG_TEST device.
        This function is called when the module is unloaded.

@param  None

@return This function returns 0.
*/
/*================================================================================================*/
static void __exit watchdog_test_exit(void)
{
        unregister_chrdev(MXC_WDOG_TM_MAJOR, "wd_tst");
        //---devfs_remove("wd_tst");
        class_device_destroy(wdog_class, MKDEV(MXC_WDOG_TM_MAJOR, 0));
        class_destroy(wdog_class);
        printk("watchdog_test : successfully unloaded\n");
}


/*================================================================================================*/
/* Module entry points */
module_init(watchdog_test_init);
module_exit(watchdog_test_exit);

MODULE_DESCRIPTION("WATCHDOG_TEST char device driver");
MODULE_AUTHOR("Freescale Semiconductor, Inc.");
MODULE_LICENSE("GPL");
