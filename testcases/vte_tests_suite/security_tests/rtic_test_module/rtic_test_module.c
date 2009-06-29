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
/*================================================================================================================*/
/**
        @file   rtic_test_module.c

        @brief  rtic API

===================================================================================================================
Revision History:
                            Modification     Tracking
Author                          Date          Number    Description of Changes
-------------------------   ------------    ----------  -------------------------------------------
S.ZAVJALOV/zvjs001c          19/10/2004     TLSbo43475   Initial version
A.URUSOV                     13/09/2005     TLSbo55076   Fix compilation issue and warnings
A.URUSOV                     01/11/2005     TLSbo57063   Compile under L26.1.14 and include added
.Ozerov/b00320               11/12/2006     TLSbo84161   Minor changes.

===================================================================================================================
Portability: ARM GCC
==================================================================================================================*/

/*=================================================================================================================
Total Tests: 1

Test Executable Name:  rtic_test_module.ko

Test Strategy: Examine the RTIC module functions
==================================================================================================================*/

/*==================================================================================================================
                                        INCLUDE FILES
===================================================================================================================*/

#include <linux/module.h>
#include <linux/device.h>
// ---#include <linux/devfs_fs_kernel.h>
#include <linux/fs.h>
#include <linux/delay.h>
#include <asm/uaccess.h>
#include "mxc_rtic.h"
#include <asm/arch-mxc/mxc_security_api.h>

#include "rtic_test_module.h"

/*==================================================================================================================
                                GLOBAL VARIABLE DECLARATIONS
===================================================================================================================*/

static __u32 major_dev_num;
static struct class *rtic_class;        /* Added by Pradeep K */
ulong  *rtic_virt_data = NULL;
ulong   rtic_phys_data = 0;

/*==================================================================================================================
                                    FUNCTION PROTOTYPES
===================================================================================================================*/

int     rtic_test_ioctl(struct inode *inode, struct file *file, unsigned int cmd,
                        rtic_test_param * arg);
static int rtic_test_init(void);
void    rtic_test_exit(void);

void    module_cleanup(char *arg_mes);

/*==================================================================================================================
                                STRUCTURES AND OTHER TYPEDEFS
===================================================================================================================*/

static struct file_operations rtic_test_fops = {
        .owner = THIS_MODULE,
        .ioctl = (void *) rtic_test_ioctl,
};

/*=================================================================================================================*/

static int rtic_test_init(void)
{
        if ((major_dev_num = register_chrdev(0, RTIC_DEVICE_NAME, &rtic_test_fops)) < 0)
        {
                printk(KERN_WARNING "RTIC Test Module: unable to register device\n");
                return major_dev_num;
        }

        rtic_class = class_create(THIS_MODULE, "rtic_test_module");
        if (IS_ERR(rtic_class))
        {
                printk(KERN_ALERT "class simple created failed\n");
                goto err_out;
        }

        if (IS_ERR
            (class_device_create
             (rtic_class, NULL, MKDEV(major_dev_num, 0), NULL, "rtic_test_module")))
        {
                printk(KERN_ALERT "class simple add failed\n");
                goto err_out;
        }
        printk("rtic : creating devfs entry for rtic_test \n");

        /* if ((//---devfs_mk_cdev(MKDEV(major_dev_num, 0), S_IFCHR | S_IRUGO | S_IWUGO,
        * RTIC_DEVICE_NAME)) < 0) { printk(KERN_WARNING "RTIC Test Module: unable to create device
        * file\n"); return -1; } */
        rtic_init();
        printk(KERN_INFO "Module load succesful\n");
        return 0;

        err_out:
        printk(KERN_ERR "rtic : error creating rtic test module class.\n");
        class_device_destroy(rtic_class, MKDEV(major_dev_num, 0));
        class_destroy(rtic_class);
        unregister_chrdev(major_dev_num, "rtic_test_module");
        return -1;
}

/*=================================================================================================================*/

void rtic_test_exit(void)
{
        unregister_chrdev(major_dev_num, RTIC_DEVICE_NAME);
        // ---devfs_remove(RTIC_DEVICE_NAME);
        class_device_destroy(rtic_class, MKDEV(major_dev_num, 0));
        class_destroy(rtic_class);
        printk(KERN_INFO "Module unload succesful\n");
}

/*=================================================================================================================*/

int rtic_test_ioctl(struct inode *inode, struct file *file, unsigned int cmd, rtic_test_param * arg)
{
        int     i;
        ulong   tmp;
        rtic_test_param kern_arg;
        rtic_hash_rlt rtic_res;
        rtic_mode rt_mode;

        copy_from_user(&kern_arg, arg, sizeof(rtic_test_param));
        rtic_virt_data = (ulong *) kmalloc((kern_arg.data_length + 4) * sizeof(ulong), GFP_KERNEL);
        if (rtic_virt_data == NULL)
        {
                module_cleanup("Failure allocate memory");
                return -1;
        }
        rtic_phys_data = virt_to_phys(rtic_virt_data);
        if ((tmp = ((ulong) rtic_phys_data % 4)) != 0)
        {
                rtic_phys_data = (rtic_phys_data + 4 - tmp);
                rtic_virt_data = (ulong *) ((ulong) rtic_virt_data + 4 - tmp);
        }
        copy_from_user(rtic_virt_data, kern_arg.data, kern_arg.data_length);

        /* RTIC One Time Hashing */
        if (kern_arg.verbose_mode)
                printk(KERN_INFO "RTIC Test Module: Try configure RTIC\n");
        if ((rtic_configure_mem_blk(rtic_phys_data, kern_arg.data_length, kern_arg.block_memory, 1)) !=
            RTIC_SUCCESS)
        {
                module_cleanup("Failure RTIC configure");
                return -1;
        }
        if (kern_arg.verbose_mode)
                printk(KERN_INFO "RTIC Test Module: Try configure RTIC mode\n");

        if ((rtic_configure_mode(RTIC_ONE_TIME, kern_arg.block_memory)) != RTIC_SUCCESS)
        {
                module_cleanup("Failure RTIC configure mode");
                return -1;
        }
        if (kern_arg.verbose_mode)
                printk(KERN_INFO "RTIC Test Module: Try configure RTIC interrupt\n");
        if ((rtic_configure_interrupt(0x01)) != RTIC_SUCCESS)
        {
                module_cleanup("Failure RTIC configure interrupt");
                return -1;
        }
        rtic_start_hash(RTIC_ONE_TIME);

        /* if ((rtic_configure_interrupt(kern_arg.interrupt)) != RTIC_SUCCESS) {
        * module_cleanup("Failure RTIC configure interrupt"); return -1; } */
        if (kern_arg.verbose_mode)
                printk(KERN_INFO "RTIC Test Module: Start OneTime hashing\n");
        printk("RTIC TEST DRV: RTIC Status register.0x%08lX\n", rtic_get_status());

        switch (cmd)
        {
        case CASE_TEST_RTIC_ONETIME:

                if ((rtic_get_status() & 0x04) == RTIC_STAT_HASH_ERR)
                {
                        module_cleanup("Failure data hashing");
                        return -1;
                }

                if (kern_arg.verbose_mode && ((rtic_get_status() & 0x02) == RTIC_STAT_HASH_DONE))
                        printk(KERN_INFO "RTIC Test Module: Hashing successful finished\n");
                rt_mode = RTIC_ONE_TIME;
                if (rtic_hash_result(kern_arg.block_memory, rt_mode, &rtic_res) != RTIC_SUCCESS)
                {
                        module_cleanup("Failure get hash result");
                        return -1;
                }
                if (kern_arg.verbose_mode)
                {
                        printk("RTIC Test Module: Hash result: ");
                        for (i = 0; i < 5; i++)
                        {
                                printk("0x%08lx ", rtic_res.hash_result[i]);
                        }
                        printk("\n");
                }
                break;

        case CASE_TEST_RTIC_RUNTIME:
                printk("RTIC TEst driv: RUN time ioctl called.\n");
                if (kern_arg.verbose_mode)
                        printk(KERN_INFO "RTIC Test Module: Try configure RTIC mode\n");
                if ((rtic_configure_mode(RTIC_RUN_TIME, kern_arg.block_memory)) != RTIC_SUCCESS)
                {
                        module_cleanup("Failure RTIC configure mode");
                        return -1;
                }
                if (kern_arg.verbose_mode)
                        printk(KERN_INFO "RTIC Test Module: Try configure RTIC interrupt\n");
                if ((rtic_configure_interrupt(kern_arg.interrupt)) != RTIC_SUCCESS)
                {
                        module_cleanup("Failure RTIC configure interrupt");
                        return -1;
                }
                if (kern_arg.verbose_mode)
                        printk(KERN_INFO "RTIC Test Module: Start RunTime hashing\n");
                rtic_start_hash(RTIC_RUN_TIME);
                if ((rtic_get_status() & 0x04) == RTIC_STAT_HASH_ERR)
                {
                        module_cleanup("Failure data hashing");
                        return -1;
                }
                /* if (kern_arg.verbose_mode && ((rtic_get_status() & 0x02) == RTIC_STAT_HASH_DONE))
                * printk(KERN_INFO "RTIC Test Module: Hashing successful finished\n"); if
                * (kern_arg.verbose_mode) printk(KERN_INFO "RTIC Test Module: RTIC Status register:
                * 0x%08lX\n", rtic_get_status()); */
                rt_mode = RTIC_RUN_TIME;
                /* if (rtic_hash_result(kern_arg.block_memory, rt_mode, &rtic_res) != RTIC_SUCCESS) {
                * module_cleanup("Failure get hash result\n"); return -1; } */
                if (kern_arg.verbose_mode)
                {
                        printk("RTIC Test Module: Hash result: ");
                        for (i = 0; i < 5; i++)
                        {
                                printk("0x%08lx ", rtic_res.hash_result[i]);
                        }
                        printk("\n");
                }

                break;

        case CASE_TEST_RTIC_GET_CONTROL:
                printk(KERN_INFO "RTIC Test Module: RTIC Hashing control register 0x%08X\n",
                       RTIC_CONTROL);
                if (RTIC_FAILURE == rtic_get_control())
                {
                        return -1;
                }
                break;

        case CASE_TEST_RTIC_GET_FAULTADDRESS:
                printk(KERN_INFO "RTIC Test Module: RTIC Fault address register 0x%08X\n",
                       RTIC_FAULTADDR);
                if (RTIC_FAILURE == rtic_get_faultaddress())
                {
                        return -1;
                }
                break;
        }
        module_cleanup("");
        return 0;
}

/*=================================================================================================================*/
void module_cleanup(char *arg_mes)
{
        if (arg_mes != "")
                printk(KERN_WARNING "RTIC Test Module: %s\n", arg_mes);
        if (rtic_virt_data != NULL)
                kfree(rtic_virt_data);
}

/*=================================================================================================================*/
module_init(rtic_test_init);
module_exit(rtic_test_exit);

MODULE_DESCRIPTION("Test Module for RTIC Argon + drivers");
MODULE_LICENSE("GPL");
