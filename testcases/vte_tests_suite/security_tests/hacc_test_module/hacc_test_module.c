/*================================================================================================*/
/**
        @file   hacc_test_module.c

        @brief  hacc API
*/
/*==================================================================================================*/
/**
        Copyright (C) 2006, Freescale Semiconductor, Inc. All Rights Reserved
        THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
        BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
        Freescale Semiconductor, Inc.
*/
/*====================================================================================================
Revision History:
                Modification     Tracking
Author                          Date          Number    Description of Changes
-------------------------   ------------    ----------  -------------------------------------------
S.ZAVJALOV/zvjs001c          10/08/2004     TLSbo40418   Initial version
S.ZAVJALOV/zvjs001c          01/10/2004     TLSbo40649   Version after inspection
S.ZAVJALOV/zvjs001c          11/10/2004     TLSbo43283   Version for 1.5 release kernel
Christian Gagneraud (cgag1c) 26/10/2004     Tlsbo43824   Fix compilation issues and warnings
S.ZAVJALOV/zvjs001c          04/11/2004     TLSbo43890   Chages for virt_to_phys function
S.ZAVJALOV/zvjs001c          04/07/2005     TLSbo51629   Change hacc test strategy
A.URUSOV                     18/10/2005     TLSbo57061   New test functions are added
A.Ozerov/b00320              11/12/2006     TLSbo84161   Minor changes.

==================================================================================================
Total Tests: 1

Test Executable Name:  hacc_test_module.ko

Test Strategy: Examine the HAC module functions
=================================================================================================*/

/*==================================================================================================
                                        INCLUDE FILES
==================================================================================================*/

#include <linux/module.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <mxc_hacc.h>
#include <asm/arch-mxc/mxc_security_api.h>

#include "hacc_test_module.h"

/*==================================================================================================
                                    LOCAL VARIABLE DECLARATIONS
==================================================================================================*/

static __u32 major_dev_num;
static struct class *hacc_class;        /* Added by Pradeep K */

/*==================================================================================================
                                    LOCAL  FUNCTION PROTOTYPES
==================================================================================================*/

static int hacc_test_ioctl(struct inode *inode,
                           struct file *file, unsigned int cmd, unsigned long ularg);
static int hacc_test_init(void);
static void hacc_test_exit(void);
static void hacc_test_get_status(void);
static void hacc_test_stop(void);
hac_ret hacc_test_swrst(void);
void    hacc_test_burst_mode(void);
void    hacc_test_burst_read_nature(void);
void    hacc_test_suspend(void);
void    hacc_test_resume(void);

/*==================================================================================================
                                STRUCTURES AND OTHER TYPEDEFS
==================================================================================================*/

static struct file_operations hacc_test_fops = 
{
        owner:THIS_MODULE,
        ioctl:hacc_test_ioctl,
};

/*================================================================================================
                                        LOCAL  FUNCTIONS
==================================================================================================*/
/***********************************************************************
* hacc_test_init()                                                    *
**********************************************************************/
/**
* Module interface function which initializes the Linux device driver.
*
* It registers the HAC character device, sets up the base address for
* the HAC, and registers interrupt handlers.  It is called during
* insmod(8). The interface is defined by the Linux DDI/DKI.
*
* @return This function returns 0 upon success or a (negative) errno
* code on failure.
*
*/
/*=================================================================================================================*/
static int hacc_test_init(void)
{
        if ((major_dev_num = register_chrdev(0, HACC_DEVICE_NAME, &hacc_test_fops)) < 0)
        {
                printk(KERN_WARNING "HACC Module Test: unable to register the dev\n");
                return major_dev_num;
        }

        hacc_class = class_create(THIS_MODULE, "hacc_test_module");
        if (IS_ERR(hacc_class))
        {
                printk(KERN_ALERT "class simple created failed\n");
                goto err_out;
        }

        if (IS_ERR(class_device_create(hacc_class, NULL,
                                       MKDEV(major_dev_num, 0), NULL, "hacc_test_module")))
        {
                printk(KERN_ALERT "class simple add failed\n");
                goto err_out;
        }
        printk("hacc : creating devfs entry for hacc_test \n");

        /* if ((//---devfs_mk_cdev(MKDEV(major_dev_num, 0), S_IFCHR | S_IRUGO | S_IWUGO,
        * HACC_DEVICE_NAME)) < 0) { printk(KERN_WARNING "HACC Module Test: unable to create test
        * device\n"); return -1; } */
        printk(KERN_INFO "Module load successful\n");
        return 0;

        err_out:
        printk(KERN_ERR "hacc : error creating hacc test module class.\n");
        class_device_destroy(hacc_class, MKDEV(major_dev_num, 0));
        class_destroy(hacc_class);
        unregister_chrdev(major_dev_num, "hacc_test_module");
        return -1;
}

/*=================================================================================================================*/
/***********************************************************************
* hacc_test_exit()                                                    *
**********************************************************************/
/**
* Module interface function for unloading the device driver.
*
* This function is called during rmmod(8).  The interface is defined
* by the Linux DDI/DKI.
*
* It deregisters the SCC character device, unmaps memory, and
* deregisters the interrupt handler(s).
*
* Called by the kernel during an @c rmmod(8) operation, but also
* during error handling from #scc_test_init().
*
*/
/*=================================================================================================================*/
static void hacc_test_exit(void)
{
        unregister_chrdev(major_dev_num, HACC_DEVICE_NAME);
        // ---devfs_remove(HACC_DEVICE_NAME);
        class_device_destroy(hacc_class, MKDEV(major_dev_num, 0));
        class_destroy(hacc_class);
        printk(KERN_INFO "Module unload successful\n");
}

/*=================================================================================================================*/
/***********************************************************************
* hacc_test_ioctl()                                                    *
**********************************************************************/
/**
* Module interface function for ioctl() system call.
*
* This function serves as a control for the commands being
* passed by the application code.  Depending on what command has been
* sent, a specific function will occur.  The interface is defined by
* the Linux DDI/DKI.
*
* This routine handles the following valid commands:
*
* - #CASE_TEST_HACC_01 - build HAC hash from contiguous flash block
* - #CASE_TEST_HACC_02 - build HAC hash from non-contiguous flash block
*
* @pre Application code supplies a command with the related data (via the
* scc_data struct)
*
* @post A specific action is performed based on the requested command.
*
* @param inode    - struct that contains the major and minor numbers.
* @param file     - file pointer.
* @param cmd      - the requested command supplied by application code.
* @param ularg    - input struct provided by application code.
*
* @return 0 or an error
*/
/*=================================================================================================================*/
static int hacc_test_ioctl(struct inode *inode, struct file *file, unsigned int cmd,
                           unsigned long ularg)
{
        hac_hash_rlt hash_res;
        int     i;
        ulong   ul;
        hacc_test_param kernel_space_arg;
        hacc_test_param *user_space_arg = (hacc_test_param *) ularg;

        if (copy_from_user(&kernel_space_arg, user_space_arg, sizeof(hacc_test_param)) != 0)
        {
                printk(KERN_WARNING "HACC Module Test: ERROR : Failure copy from user space\n");
                return -1;
        }

        switch (cmd)
        {
                /* Contiguous */
        case CASE_TEST_HACC_01:
                hacc_test_get_status();
                if ((hac_hash_data
                     ((ulong) kernel_space_arg.start_hacc_addr, kernel_space_arg.data2hash_len,
                      HAC_START)) != HAC_SUCCESS)
                {
                        printk(KERN_WARNING "HACC Module Test: ERROR : Failure hashing data\n");
                        return -1;
                }
                while (hac_hashing_status() == HAC_BUSY)
                {
                        hacc_test_get_status();
                        if (kernel_space_arg.stop_flag)
                        {
                                hacc_test_stop();
                                hacc_test_get_status();
                                return 0;
                        }
                        if (kernel_space_arg.verbose_mode)
                                hacc_test_get_status();
                        printk(KERN_INFO
                               "HACC Module Test:  INFO: hac_hashing_status() is HAC_BUSY\n");
                }
                switch (hac_hashing_status())
                {
                case HAC_BUSY:
                        hacc_test_get_status();
                        if (kernel_space_arg.stop_flag)
                        {
                                hacc_test_stop();
                                hacc_test_get_status();
                        }
                case HAC_UNKNOWN:
                        hacc_test_get_status();
                case HAC_ERR:
                        hacc_test_get_status();
                        printk(KERN_WARNING
                               "HACC Module Test: ERROR : Encountered error while processing\n");
                        return -1;
                case HAC_DONE:
                        hacc_test_get_status();
                        if (kernel_space_arg.verbose_mode)
                                printk(KERN_INFO "HACC Module Test:  INFO: Hashing done\n");
                        break;
                }
                hac_hash_result(&hash_res);
                if (kernel_space_arg.verbose_mode)
                {
                        printk(KERN_INFO "HACC Module Test:  INFO: Hash result: ");
                        for (i = 0; i < 5; i++)
                                printk("0x%08lx ", (unsigned long) hash_res.hash_result[i]);
                        printk("\n");
                }
                hacc_test_get_status();
                break;
                /* None contiguous */
        case CASE_TEST_HACC_02:
                hacc_test_get_status();
                if ((hac_hash_data((ulong) kernel_space_arg.start_hacc_addr, 64, HAC_START)) !=
                    HAC_SUCCESS)
                {
                        printk(KERN_WARNING "HACC Module Test: ERROR : Failure hashing data\n");
                        return -1;
                }
                while (hac_hashing_status() == HAC_BUSY)
                {
                        hacc_test_get_status();
                        if (kernel_space_arg.stop_flag)
                        {
                                hacc_test_stop();
                                hacc_test_get_status();
                                return 0;
                        }
                        if (kernel_space_arg.verbose_mode)
                                printk(KERN_INFO
                                       "HACC Module Test:  INFO: hac_hashing_status() is HAC_BUSY\n");
                }
                switch (hac_hashing_status())
                {
                case HAC_BUSY:
                        hacc_test_get_status();
                        if (kernel_space_arg.stop_flag)
                        {
                                hacc_test_stop();
                                hacc_test_get_status();
                        }
                case HAC_UNKNOWN:
                        hacc_test_get_status();
                case HAC_ERR:
                        hacc_test_get_status();
                        printk(KERN_WARNING
                               "HACC Module Test: ERROR : Encountered error while processing\n");
                        return -1;
                case HAC_DONE:
                        hacc_test_get_status();
                        if (kernel_space_arg.verbose_mode)
                                printk(KERN_INFO "HACC Module Test:  INFO: Hashing done\n");
                        break;
                }
                for (ul = (kernel_space_arg.start_hacc_addr + 128);
                     ul < (kernel_space_arg.start_hacc_addr + kernel_space_arg.data2hash_len);
                     ul += 128)
                {
                        if ((hac_hash_data(ul, 64, HAC_CONTINUE)) != HAC_SUCCESS)
                        {
                                printk(KERN_WARNING
                                       "HACC Module Test: ERROR : Failure hashing data\n");
                                return -1;
                        }
                        while (hac_hashing_status() == HAC_BUSY)
                        {
                                hacc_test_get_status();
                                if (kernel_space_arg.stop_flag)
                                {
                                        hacc_test_stop();
                                        hacc_test_get_status();
                                        return 0;
                                }
                                if (kernel_space_arg.verbose_mode)
                                        hacc_test_get_status();
                                printk(KERN_INFO
                                       "HACC Module Test:  INFO: hac_hashing_status() is HAC_BUSY\n");
                        }
                        switch (hac_hashing_status())
                        {
                        case HAC_BUSY:
                                hacc_test_get_status();
                        case HAC_UNKNOWN:
                                hacc_test_get_status();
                        case HAC_ERR:
                                hacc_test_get_status();
                                printk(KERN_WARNING
                                       "HACC Module Test: ERROR : Encountered error while processing\n");
                                return -1;
                        case HAC_DONE:
                                hacc_test_get_status();
                                if (kernel_space_arg.verbose_mode)
                                        printk(KERN_INFO "HACC Module Test:  INFO: Hashing done\n");
                                break;
                        }
                }
                hac_hash_result(&hash_res);
                if (kernel_space_arg.verbose_mode)
                {
                        hacc_test_get_status();
                        printk(KERN_INFO "HACC Module Test:  INFO: Hash result: ");
                        for (i = 0; i < 5; i++)
                                printk("0x%08lx ", (unsigned long) hash_res.hash_result[i]);
                        printk("\n");
                }
                break;
                /* Contiguous and stop hashing process */
        case CASE_TEST_HACC_03:
                printk("HACC Module Test:  INFO: HAC Module get status\n");
                hacc_test_get_status();
                printk("HACC Module Test:  INFO: Software reset HAC Module is ");
                switch (hacc_test_swrst())
                {
                case HAC_SUCCESS:
                        printk("success.\n");
                        break;
                case HAC_FAILURE:
                        printk("failure.\n");
                        break;
                case HAC_HASH_BUSY:
                        printk("failure.\n");
                        printk(KERN_INFO
                               "HACC Module Test:  INFO: HAC module is busy in Hashing process.\n");
                        break;
                }
                hacc_test_get_status();
                break;
                /* HAC burst mode switch test */
        case CASE_TEST_HACC_04:
                hacc_test_burst_mode();
                break;
                /* HAC burst read nature configure test */
        case CASE_TEST_HACC_05:
                hacc_test_burst_read_nature();
                break;
#ifdef CONFIG_PM
                /* HAC suspends test */
        case CASE_TEST_HACC_06:
                hacc_test_suspend();
                break;
                /* HAC resumes test */
        case CASE_TEST_HACC_07:
                hacc_test_resume();
                break;
#endif                          /* CONFIG_PM */
        }
        return 0;
}

/*=================================================================================================================*/
/***********************************************************************
* hacc_test_get_status()                                              *
**********************************************************************/
/**
* Test of read a value from the HAC Control Register
* and trying to identification it as HAC Module Status
*
* @param
*
* @return
*/
/*=================================================================================================================*/
void hacc_test_get_status(void)
{
        printk("HACC Module Test:  INFO: HAC_CTL register value: %lu\n", hac_get_status());
        printk("HACC Module Test:  INFO: Hash Module Status is: ");
        if (((HAC_CTL & HAC_CTL_DONE) == HAC_CTL_DONE) ||
            ((HAC_CTL & HAC_CTL_ERROR) == HAC_CTL_ERROR))
        {
                printk("DONE and ERROR bit is set.\n");
                return;
        }
        if (0 != (HAC_CTL & HAC_CTL_BUSY))
        {
                printk("Accelerator Module Busy.\n");
                return;
        }
        if ((HAC_CTL & HAC_CTL_STOP) == HAC_CTL_STOP)
        {
                printk("Stop hashing processing.\n");
                return;
        }
        printk("unknown.\n");
}

/*=================================================================================================================*/
/***********************************************************************
* hacc_test_stop()                                                    *
**********************************************************************/
/**
* Test of Stops the Hashing of data when the Hashing is in progress
*
* @param
*
* @return
*/
/*=================================================================================================================*/
void hacc_test_stop(void)
{
        int     hac_stop_flag = 0;

        hac_stop_flag = hac_stop();
        if (0 == hac_stop_flag)
        {
                printk("HAC Module Stopped Successfully\n");
        }
        else if (-1 == hac_stop_flag)
        {
                printk("HAC Module Stopped Failure\n");
        }
        else if (-2 == hac_stop_flag)
        {
                printk("HAC module is busy in Hashing process\n");
        }
}

/*=================================================================================================================*/
/***********************************************************************
* hacc_test_get_swrst()                                               *
**********************************************************************/
/* ! Test of initiates software reset of the entire HAC module. It resets all state machine to their
* default values. All status bits (BUSY/ERROR/DONE) and any pending interrupts are cleared. @return
* HAC_SUCCESS Successfully in doing software reset.\n HAC_FAILURE Error in doing software reset. 
*/
/*=================================================================================================================*/
hac_ret hacc_test_swrst(void)
{
        hac_ret ret_val;

        ret_val = hac_swrst();
        return ret_val;
}

/*=================================================================================================================*/
/***********************************************************************
* hacc_test_burst_mode()                                              *
**********************************************************************/
/* ! Test of configures the burst mode of the HAC. When Burst mode set in HAC Control register then
* ARM9 is configured for a 16-WORD burst, while Burst mode is cleared then ARM9 is configured for a
* incremental burst. 
@param 
@return 
*/
/*=================================================================================================================*/
void hacc_test_burst_mode(void)
{
        ulong   ret_val = 0;

        printk("HACC Module Test:  INFO: Trying to configure HAC Module burst mode.\n");
        /* Configures Incremental Burst Mode */
        ret_val = hac_burst_mode(HAC_INR_BURST);
        switch (ret_val)
        {
        case 0:        /* HAC_SUCCESS analog */
                printk
                    ("HACC Module Test:  INFO: Configure the incremental burst mode of the HAC is successfull.\n");
                break;
        case 1:        /* Cause: in the hac_burst_mode() function we've lost the sign of return * *
                        * value */
                printk("HACC Module Test:  INFO: Error in configuring incremental burst mode.\n");
                break;
        }
        /* Configures 16WORD Burst Mode */
        ret_val = hac_burst_mode(HAC_16WORD_BURST);
        switch (ret_val)
        {
        case 0:        /* HAC_SUCCESS analog */
                printk
                    ("HACC Module Test:  INFO: Configure the 16WORD burst mode of the HAC is successfull.\n");
                break;
        case 1:        /* Cause: in the hac_burst_mode() function we've lost the sign of return * *
                        * value */
                printk("HACC Module Test:  INFO: Error in configuring 16WORD burst mode.\n");
                break;
        }
}

/*=================================================================================================================*/
/***********************************************************************
* hacc_test_burst_read_nature()                                       *
**********************************************************************/
/* ! This test is trying to configure HAC burst read nature. 
@param 
@return 
*/
/*=================================================================================================================*/
void hacc_test_burst_read_nature(void)
{
        ulong   ret_val = 0;

        printk("HACC Module Test:  INFO: Trying to configure HAC Module burst mode.\n");
        /* Configure 16 word Burst read nature */
        ret_val = hac_burst_read(HAC_16WORD_BURST_READ);
        switch (ret_val)
        {
        case 0:        /* HAC_SUCCESS analog */
                printk
                    ("HACC Module Test:  INFO: Configure the 16 word Burst read nature is successfull.\n");
                break;
        case 1:        /* Cause: in the hac_burst_read() function we've lost the sign of return * *
                        * value */
                printk
                    ("HACC Module Test:  INFO: Error in configuring of the 16 word Burst read nature.\n");
                break;
        }
        /* Configure 8 word Burst read nature */
        ret_val = hac_burst_read(HAC_8WORD_BURST_READ);
        switch (ret_val)
        {
        case 0:        /* HAC_SUCCESS analog */
                printk
                    ("HACC Module Test:  INFO: Configure the 8 word Burst read nature is successfull.\n");
                break;
        case 1:        /* Cause: in the hac_burst_read() function we've lost the sign of return * *
                        * value */
                printk
                    ("HACC Module Test:  INFO: Error in configuring of the 8 word Burst read nature.\n");
                break;
        }
        /* Configure 4 word Burst read nature */
        ret_val = hac_burst_read(HAC_4WORD_BURST_READ);
        switch (ret_val)
        {
        case 0:        /* HAC_SUCCESS analog */
                printk
                    ("HACC Module Test:  INFO: Configure the 4 word Burst read nature is successfull.\n");
                break;
        case 1:        /* Cause: in the hac_burst_read() function we've lost the sign of return * *
                        * value */
                printk
                    ("HACC Module Test:  INFO: Error in configuring of the 4 word Burst read nature.\n");
                break;
        }
        /* Configure no word Burst read nature */
        ret_val = hac_burst_read(HAC_NO_WORD_BURST_READ);
        switch (ret_val)
        {
        case 0:        /* HAC_SUCCESS analog */
                printk
                    ("HACC Module Test:  INFO: Configure the no word Burst read nature is successfull.\n");
                break;
        case 1:        /* Cause: in the hac_burst_read() function we've lost the sign of return * *
                        * value */
                printk
                    ("HACC Module Test:  INFO: Error in configuring of the no word Burst read nature.\n");
                break;
        }
}

/*=================================================================================================================*/
#ifdef CONFIG_PM
/*=================================================================================================================*/
/***********************************************************************
* hacc_test_suspend()                                                 *
**********************************************************************/
/* ! This function is called to test the put of HAC in a low power state. 
@param 
@param 
*/
/*=================================================================================================================*/
void hacc_test_suspend(void)
{ /* 
    * ulong state = 0; hac_ret ret_val = HAC_SUCCESS; struct device dev;
    * 
    * printk("HACC Module Test: INFO: Trying to put the HAC Module in a low power state.\n"); */
        /* Test the suspend disable level */
        // ret_val = hac_suspend(&dev, state, SUSPEND_DISABLE);
        /* switch (ret_val) { case HAC_SUCCESS: printk ("HACC Module Test: INFO: The set on suspend
        * disable level is successfull.\n"); break; case HAC_FAILURE: printk("HACC Module Test:
        * INFO: Error in the set on suspend disable level.\n"); break; case HAC_HASH_BUSY:
        * printk("HACC Module Test: INFO: HAC module are busy in hash process.\n"); break; } */
        /* Test the suspend save state level */
        // ret_val = hac_suspend(&dev, state, SUSPEND_SAVE_STATE);
        /* switch (ret_val) { case HAC_SUCCESS: printk ("HACC Module Test: INFO: The set on suspend
        * save state level is successfull.\n"); break; case HAC_FAILURE: printk("HACC Module Test:
        * INFO: Error in the set on suspend save state level.\n"); break; case HAC_HASH_BUSY:
        * printk("HACC Module Test: INFO: HAC module are busy in hash process.\n"); break; } */
        /* Test the suspend power down level */
        // ret_val = hac_suspend(&dev, state, SUSPEND_POWER_DOWN);
        /* switch (ret_val) { case HAC_SUCCESS: printk ("HACC Module Test: INFO: The set on suspend
        * power down level is successfull.\n"); break; case HAC_FAILURE: printk("HACC Module Test:
        * INFO: Error in the set on suspend power down level.\n"); break; case HAC_HASH_BUSY:
        * printk("HACC Module Test: INFO: HAC module are busy in hash process.\n"); break; } */
}

/*=================================================================================================================*/
/***********************************************************************
* hacc_test_resume()                                                  *
**********************************************************************/
/* ! This function is called to test the bring of HAC from a low power state. 
@param 
@param 
*/
/*=================================================================================================================*/
void hacc_test_resume(void)
{
        /* 
        * hac_ret ret_val = HAC_SUCCESS; struct device dev;
        * 
        * printk("HACC Module Test: INFO: Trying to bring the HAC Module from a low power
        * state.\n"); */
        /* Test the resume power on level */
        /* ret_val = hac_resume(&dev, RESUME_POWER_ON); switch (ret_val) { case HAC_SUCCESS:
        * printk("HACC Module Test: INFO: The resume power on level set is successfull.\n"); break;
        * case HAC_FAILURE: printk("HACC Module Test: INFO: Error in the resume power on level
        * set.\n"); break; case HAC_HASH_BUSY: printk("HACC Module Test: INFO: HAC module are busy
        * in hash process.\n"); break; } */
        /* Test the resume restore state level */
        /* ret_val = hac_resume(&dev, RESUME_RESTORE_STATE); switch (ret_val) { case HAC_SUCCESS:
        * printk ("HACC Module Test: INFO: The resume restore state level set is successfull.\n");
        * break; case HAC_FAILURE: printk("HACC Module Test: INFO: Error in the resume restore state 
        * level set.\n"); break; case HAC_HASH_BUSY: printk("HACC Module Test: INFO: HAC module are
        * busy in hash process.\n"); break; } */
        /* Test resume enable level */
        /* ret_val = hac_resume(&dev, RESUME_ENABLE); switch (ret_val) { case HAC_SUCCESS:
        * printk("HACC Module Test: INFO: The resume enable level set is successfull.\n"); break;
        * case HAC_FAILURE: printk("HACC Module Test: INFO: Error in the resume enable level
        * set.\n"); break; case HAC_HASH_BUSY: printk("HACC Module Test: INFO: HAC module are busy
        * in hash process.\n"); break; } */
}

/*=================================================================================================================*/

#endif                          /* CONFIG_PM */

module_init(hacc_test_init);
module_exit(hacc_test_exit);

MODULE_DESCRIPTION("Test Module for HACC user_space_MXC91331 drivers");
MODULE_LICENSE("GPL");
