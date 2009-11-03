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
        @file   scc_test_module.c
        @brief This is a test module for the SCC.
               This module and its associated reference test program are intended
               to demonstrate the use of the SCC block in various ways.
               The module runs in a Linux environment.  Portation notes are
               included at various points which should be helpful in moving the
               code to a different environment.
====================================================================================================
Revision History:
                            Modification     Tracking
Author/core ID                  Date          Number    Description of Changes
-------------------------   ------------    ----------  -------------------------------------------
A.Urusov/NONE                29/09/2005     TLSbo55835   Initial version
A.Ozerov/NONE                03/02/2006     TLSbo61735   Mapping was removed
Y.Batrakov/NONE              20/09/2006     TLSbo78563   Fixed compilation for imx27 platform
D.Simakov                    13/11/2006     TLSbo80386   tests are unable to access registers for i.MX27ADS
====================================================================================================
====================================================================================================
Total Tests: 1

Test Executable Name:  scc_test_module.ko

Test Strategy: Examine the SCC driver common software operations
=================================================================================================*/

/*==================================================================================================
                                        INCLUDE FILES
==================================================================================================*/
#include <linux/module.h>
#include <linux/device.h>
#include <linux/version.h>
#include <linux/io.h>
//---#include <linux/devfs_fs_kernel.h>
#include <linux/fs.h>
#include <asm/uaccess.h>

//#include <asm/arch-mxc/mxc_security_api.h>
//#include <asm/arch-mxc/mxc_scc.h>
#include <mach/mxc_scc.h>
#include "scc_test_module.h"


/*==================================================================================================
                                   LOCAL VARIABLE DECLARATIONS
==================================================================================================*/
#define SCC_READ_REGISTER(offset) __raw_readl(scc_base+(offset))
#define SCC_WRITE_REGISTER(offset,value) (void)__raw_writel(value, scc_base+(offset))
#define SCC_BYTE_OFFSET(bp) ((uint32_t)(bp) % sizeof(uint32_t))

static volatile void *scc_base;
static __u32 major_dev_num;
static struct class *scc_class; /* Added by Pradeep K*/

/*==================================================================================================
                                     LOCAL FUNCTION PROTOTYPES
==================================================================================================*/
static int scc_test_init(void);
static int scc_test_open(struct inode *inode, struct file *file);
static int scc_test_release(struct inode *inode, struct file *file);
static void scc_test_cleanup(void);
static int scc_test_ioctl(struct inode *inode, struct file *file, unsigned int cmd,
                          unsigned long scc_data);
static int scc_test_get_configuration(unsigned long scc_data);
static int scc_test_read_register(unsigned long scc_data);
static int scc_test_write_register(unsigned long scc_data);
static int scc_test_cipher(uint32_t cmd, unsigned long scc_data);
static int scc_test_zeroize(unsigned long scc_data);

//extern int //---devfs_mk_cdev(dev_t dev, umode_t mode, const char *fmt, ...);

/*==================================================================================================
                                   STRUCTURES AND OTHER TYPEDEFS
==================================================================================================*/
/**
 * Interface jump vector for calls into the device driver.
 *
 * This struct changes frequently in Linux kernel versions.  By initializing
 * elements by name, we can avoid structure mismatches.  Other elements get
 * NULL/0 by default (after all, this is a global initializer).
 */
static struct file_operations scc_test_fops = {
        .owner = THIS_MODULE,
        .open = scc_test_open,
        .ioctl = scc_test_ioctl,
        .release = scc_test_release,
};

/*================================================================================================*/

static inline int offset_within_smn(uint32_t register_offset)
{
        return register_offset >= SMN_STATUS && register_offset <= SMN_TIMER;
}


static inline int offset_within_scm(uint32_t register_offset)
{
        return (register_offset >= SCM_RED_START);
}


static scc_return_t check_register_offset(uint32_t register_offset)
{
        int return_value = SCC_RET_FAIL;
        /* Is it valid word offset ? */
        if (SCC_BYTE_OFFSET(register_offset) == 0)
        {
                /* Yes. Is register within SCM? */
                if (offset_within_scm(register_offset))
                {
                        return_value = SCC_RET_OK;      /* yes, all ok */
                }
                /* Not in SCM.  Now look within the SMN */
                else if (offset_within_smn(register_offset))
                {
                        return_value = SCC_RET_OK;      /* yes, all ok */
                }
        }
        return return_value;
}


/***********************************************************************
 * scc_test_init()                                                     *
 **********************************************************************/
/**
 * Module interface function which initializes the Linux device driver.
 *
 * It registers the SCC character device, sets up the base address for
 * the SCC, and registers interrupt handlers.  It is called during
 * insmod(8).  The interface is defined by the Linux DDI/DKI.
 *
 * @return This function returns 0 upon success or a (negative) errno
 * code on failure.
 *
 * @todo An appropriate "major" number must be selected, or generated
 * automatically (along with mknod() call!?)
 */
/*================================================================================================*/
static int scc_test_init(void)
{
        if ((major_dev_num = register_chrdev(0, SCC_TEST_DEVICE_NAME, &scc_test_fops)) < 0)
        {
                printk(KERN_WARNING "SCC Module Test: unable to register the dev\n");
                return major_dev_num;
        }

        scc_class = class_create(THIS_MODULE, "scc_test_module");
        if (IS_ERR(scc_class))
        {
            printk(KERN_ALERT "class simple created failed\n");
            goto err_out;
        }
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,26))
        if (IS_ERR(class_device_create(scc_class, NULL,
                                           MKDEV(major_dev_num, 0), NULL,
                                           "scc_test_module")))
#elif (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,28))
        if (IS_ERR(device_create(scc_class, NULL,
                                           MKDEV(major_dev_num, 0),NULL,
                                           "scc_test_module")))
#else
        if (IS_ERR(device_create(scc_class, NULL,
                                           MKDEV(major_dev_num, 0),
                                           "scc_test_module")))

#endif
        {
            printk(KERN_ALERT "class simple  add failed\n");
                   goto err_out;
        }
        printk("scc : creating devfs entry for scc_test \n");

/*        if ((//---devfs_mk_cdev(MKDEV(major_dev_num, 0),
                           S_IFCHR | S_IRUGO | S_IWUGO,
                           SCC_TEST_DEVICE_NAME)) < 0)
        {
                printk(KERN_WARNING "SCC Module Test: unable to create test device\n");
                return -1;
        }
*/
        scc_base = ioremap_nocache(SCC_BASE, SCC_ADDRESS_RANGE);

        printk(KERN_INFO "Module load successful\n");
        return 0;

err_out:
        printk(KERN_ERR "scc : error creating scc test module class.\n");
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,26))
        class_device_destroy(scc_class, MKDEV(major_dev_num, 0));
#else
        device_destroy(scc_class, MKDEV(major_dev_num, 0));
#endif

        class_destroy(scc_class);
        unregister_chrdev(major_dev_num, "scc_test_module");
        return -1;
}


/*================================================================================================*/
/***********************************************************************
 * scc_test_ioctl()                                                    *
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
 * - #SCC_GET_CONFIGURATION - Return driver, SMN, and MXC91231 versions, memory
 *    sizes, block size.
 * - #SCC_READ_REG          - Read register from SCC.
 * - #SCC_WRITE_REG         - Write register to SCC.
 * - #SCC_ENCRYPT           - Encrypt Red memory into Black memory.
 * - #SCC_DECRYPT           - Decrypt Black memory into Red memory.
 * - #SCC_SET_ALARM         - Signal a software alarm to the SCC.
 * - #SCC_ZEROIZE           - Zeroize Red & Black memories of the SCC.
 * - #SCC_SET_MON_SEC_FAIL  - Register function to be called should a
 *                            Security Failure be signaled by the SCC.
 * - #SCC_STOP_MON_SEC_FAIL - Deregister a function previously registered
 *                            with function scc_monitor_security_failure.
 * @pre Application code supplies a command with the related data (via the
 * scc_data struct)
 *
 * @post A specific action is performed based on the requested command.
 *
 * @param inode    - struct that contains the major and minor numbers.
 * @param file     - file pointer.
 * @param cmd      - the requested command supplied by application code.
 * @param scc_data - input struct provided by application code.
 *
 * @return 0 or an error code (IOCTL_SCC_xxx)
 */
/*================================================================================================*/
static int scc_test_ioctl(struct inode *inode, struct file *file, unsigned int cmd,
                          unsigned long scc_data)
{
        int     error_code = IOCTL_SCC_OK;

        switch (cmd)
        {
        case SCC_TEST_GET_CONFIGURATION:
                error_code = scc_test_get_configuration(scc_data);
                break;

        case SCC_TEST_READ_REG:
                error_code = scc_test_read_register(scc_data);
                break;

        case SCC_TEST_WRITE_REG:
                error_code = scc_test_write_register(scc_data);
                break;

        case SCC_TEST_ENCRYPT:
        case SCC_TEST_DECRYPT:
                /* kick off the function */
                error_code = scc_test_cipher(cmd, scc_data);
                break;  /* encrypt/decrypt */

        case SCC_TEST_SET_ALARM:
                scc_set_sw_alarm();     /* call the real driver here */
                break;

        case SCC_TEST_ZEROIZE:
                error_code = scc_test_zeroize(scc_data);
                break;

        case SCC_TEST_SET_MON_SEC_FAIL:
                error_code = scc_monitor_security_failure(scc_set_sw_alarm);
                break;

        case SCC_TEST_STOP_MON_SEC_FAIL:
                scc_stop_monitoring_security_failure(scc_set_sw_alarm);
                break;

        default:
#ifdef SCC_DEBUG
                printk(KERN_WARNING "SCC TEST: Error in ioctl(): (%d) is an invalid command\n", cmd);
#endif
                error_code = -IOCTL_SCC_INVALID_CMD;
                break;

        }       /* End switch */

        return error_code;
}

/*================================================================================================*/
/***********************************************************************
 * scc_read_open()                                                     *
 **********************************************************************/
/**
 * Module interface function for open() system call.
 *
 * This function increments the "IN USE" count.  It is called during
 * an open(). The interface is defined by the Linux DDI/DKI.
 *
 * @param inode - struct that contains the major and minor numbers for
 * the device.
 * @param file  - file pointer.
 *
 * @return 0 if successful, error code if not
 */
/*================================================================================================*/
static int scc_test_open(struct inode *inode, struct file *file)
{
//#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,0)
        try_module_get(THIS_MODULE);
//#else
//        MOD_INC_USE_COUNT;
//#endif

#ifdef SCC_DEBUG
        printk(KERN_INFO "SCC TEST: Device Opened\n");
#endif
        return 0;
}

/*================================================================================================*/
/***********************************************************************
 * scc_test_release()                                                  *
 **********************************************************************/
/**
 * Module interface function for close() system call.
 *
 * This function decrements the "IN USE" count.  It is called during a
 * close().  The interface is defined by the Linux DDI/DKI.
 *
 * @param inode - struct that contains the major and minor numbers.
 * @param file  - file pointer
 *
 * @return 0 (always - errors are ignored)
 */
/*================================================================================================*/
static int scc_test_release(struct inode *inode, struct file *file)
{
//#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,0)
        module_put(THIS_MODULE);
//#else
//        MOD_DEC_USE_COUNT;
//#endif

#ifdef SCC_DEBUG
        printk(KERN_WARNING "SCC TEST: Inside release\n");
#endif
        return 0;
}

/*================================================================================================*/
/***********************************************************************
 * scc_test_cleanup()                                                  *
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
/*================================================================================================*/
static void scc_test_cleanup(void)
{
        iounmap((void *)scc_base);

        /* turn off the mapping to the device special file */
        unregister_chrdev(major_dev_num, SCC_TEST_DEVICE_NAME);
        //---devfs_remove(SCC_TEST_DEVICE_NAME);
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,26))
        class_device_destroy(scc_class,MKDEV(major_dev_num, 0));
#else
        device_destroy(scc_class,MKDEV(major_dev_num, 0));
#endif
        class_destroy(scc_class);
        printk(KERN_INFO "Module unload succesful\n");

#ifdef SCC_DEBUG
        printk(KERN_INFO "SCC TEST: Cleaned up\n");
#endif

        return;
}

/*================================================================================================*/
/***********************************************************************
 * scc_test_get_configuration()                                        *
 **********************************************************************/
/**
 * Internal routine to handle ioctl command #SCC_GET_CONFIGURATION.
 *
 * @param scc_data - address is user space of scc_configuration_access
 * which is passed with the ioctl() system call.
 *
 * @return 0 on success, IOCTL_xxx on failure.
 *
 * This function does not access the SCC, it just passes up static,
 * previously retrieved information.
 *
 */
/*================================================================================================*/
static int scc_test_get_configuration(unsigned long scc_data)
{
        int     error_code = IOCTL_SCC_OK;

#ifdef SCC_DEBUG
        printk(KERN_INFO "SCC TEST: Configuration\n");
#endif

        /* now copy out (write) the data into user space */
        /** @todo make sure scc_get_configuration never returns null! */
        if (copy_to_user((void *) scc_data, scc_get_configuration(), sizeof(scc_config_t)))
        {
#ifdef SCC_DEBUG
                printk(KERN_WARNING "SCC TEST: Error writing data to user\n");
#endif
                error_code = -IOCTL_SCC_IMPROPER_ADDR;
        }

        return error_code;

} /* scc_get_configuration */

/*================================================================================================*/
/***********************************************************************
 * scc_test_read_register()                                                 *
 **********************************************************************/
/**
 * Read a register value from the SCC.
 *
 * @param scc_data - the address in user memory of the scc_reg_access struct
 * passed in the ioctl().
 *
 * @return 0 for success, an error code on failure.
 */
/*================================================================================================*/
static int scc_test_read_register(unsigned long scc_data)
{
        scc_reg_access  reg_struct;
        scc_return_t    scc_return = -1;
        int             error_code = IOCTL_SCC_OK;

        if (copy_from_user(&reg_struct, (void *) scc_data, sizeof(reg_struct)))
        {
#ifdef SCC_DEBUG
                printk(KERN_WARNING "SCC TEST: Error reading reg struct from user\n");
#endif
                error_code = -IOCTL_SCC_IMPROPER_ADDR;
        }
        else
        {
                /* call the real driver here */
                scc_return=check_register_offset(reg_struct.reg_offset);

                if (scc_return != SCC_RET_OK)
                {
                        error_code = -IOCTL_SCC_IMPROPER_ADDR;
                }
                else
                {
                        reg_struct.reg_data = SCC_READ_REGISTER(reg_struct.reg_offset);
                }
        }
        reg_struct.function_return_code = scc_return;
        copy_to_user((void *) scc_data, &reg_struct, sizeof(reg_struct));
        return error_code;
}

/*================================================================================================*/
/***********************************************************************
 * scc_test_write_register()
 **********************************************************************/
/**
 * Write a register value to the SCC.
 *
 * @param scc_data - the address in user memory of the scc_reg_access struct
 * passed in the ioctl().
 *
 * @return 0 for success, an error code on failure.
 */
/*================================================================================================*/
static int scc_test_write_register(unsigned long scc_data)
{
        scc_reg_access  reg_struct;
        scc_return_t    scc_return = -1;
        int             error_code = IOCTL_SCC_OK;

        /* Try to copy user's reg_struct */
        if (copy_from_user(&reg_struct, (void *) scc_data, sizeof(reg_struct)))
        {
#ifdef SCC_DEBUG
                printk(KERN_WARNING "SCC TEST: Error reading reg struct from user\n");
#endif
                error_code = -IOCTL_SCC_IMPROPER_ADDR;
        }
        else
        {

                /* call the real driver here */

                printk(KERN_INFO "reg_offset: 0x%08X, reg_data: 0x%08X\n",
                       (unsigned) reg_struct.reg_offset,
                       (unsigned) reg_struct.reg_data);

                scc_return=check_register_offset(reg_struct.reg_offset);

                if (scc_return != SCC_RET_OK)
                {
                        error_code = -IOCTL_SCC_IMPROPER_ADDR;
                }
                else
                {
                        SCC_WRITE_REGISTER(reg_struct.reg_offset,reg_struct.reg_data);
                }
        }

        reg_struct.function_return_code = scc_return;
        copy_to_user((void *) scc_data, &reg_struct, sizeof(reg_struct));
        return error_code;
}

/*================================================================================================*/
/*****************************************************************************/
/* scc_test_cipher() */
/*****************************************************************************/
/**
 * FUNC. scc_test_cipher
 * DESC. This function does the set up and the initiation of the Triple DES
 * (3DES) Encryption or Decryption depending on what has been requested by
 * application code (user space).
 *
 * Precondition: Application code has requested a 3DES Encryption/Decryption.
 * Postcondition: The SCC registers are properly set up and 3DES is initiated
 * for the SCC.
 *
 * @param cmd      - SCC_ENCRYPT = Encryption or SCC_DECRYPT = Decryption
 * @param scc_data - Input struct provided by application code.
 */
/*================================================================================================*/
static int scc_test_cipher(uint32_t cmd, unsigned long scc_data)
{
        scc_encrypt_decrypt cipher_struct;      /* local copy */
        uint32_t            *area_in = 0;
        uint32_t            *area_out = 0;
        scc_enc_dec_t       enc_dec = SCC_DECRYPT;
        char                *input;
        char                *output;
        scc_return_t        return_code = -1;
        int                 error_code = IOCTL_SCC_OK;
        int                 copy_code = 0;

        if (copy_from_user(&cipher_struct, (void *) scc_data, sizeof(cipher_struct)))
        {
#ifdef SCC_DEBUG
                printk(KERN_WARNING "SCC TEST: Error reading cipher struct from user\n");
#endif
                error_code = -IOCTL_SCC_IMPROPER_ADDR;
        }
        else
        {

                area_in = kmalloc(cipher_struct.data_in_length + 16, GFP_USER);

                area_out = kmalloc(cipher_struct.data_out_length + 16, GFP_USER);

                if (cmd == SCC_TEST_ENCRYPT)
                {
                        enc_dec = SCC_ENCRYPT;
                }

                if (!area_in || !area_out)
                {
                        error_code = -IOCTL_SCC_NO_MEMORY;
                }
                else
                {

                        /* For testing, set up internal pointers to reflect the block/word offset of
                         * user memory. */
                        input = (char *) area_in + (int) (cipher_struct.data_in) % 8;
                        output = (char *) area_out + (int) (cipher_struct.data_out) % 8;
#ifdef SCC_DEBUG
                        printk(KERN_INFO "SCC TEST: Input: 0x%p, Output: 0x%p\n", input, output);
#endif

                        copy_code = copy_from_user(input, cipher_struct.data_in,
                                                   cipher_struct.data_in_length);

                        if (copy_code != 0)
                        {
                                error_code = -IOCTL_SCC_NO_MEMORY;
                        }
                        else
                        {
#ifdef SCC_DEBUG
                                printk(KERN_INFO "SCC TEST: Running cipher of %ld bytes.\n",
                                       cipher_struct.data_in_length);
#endif
                                /* call the real driver here */
                                return_code = scc_crypt(cipher_struct.data_in_length, input,
#if 1
                                                        ((cipher_struct.crypto_mode ==
                                                          SCC_CBC_MODE)) ?
                                                          (char *) &cipher_struct.init_vector : NULL,
#else
                                                          (char *) &cipher_struct.init_vector,
#endif
                                                          enc_dec, cipher_struct.crypto_mode,
                                                          cipher_struct.check_mode, output,
                                                          &cipher_struct.data_out_length);

                                if (return_code == SCC_RET_OK)
                                {
                                        copy_code = copy_to_user(cipher_struct.data_out, output,
                                                                 cipher_struct.data_out_length);
                                }
#ifdef SCC_DEBUG
                                else
                                {
                                        error_code = -IOCTL_SCC_FAILURE;
                                        printk(KERN_INFO "SCC TEST: scc_crypt returned %d\n", return_code);
                                }
#endif

                                if (copy_code != 0 || return_code != 0)
                                {
#ifdef SCC_DEBUG
                                        if (copy_code)
                                        {
                                                printk(KERN_INFO "SCC TEST: copy_to_user returned %d\n",
                                                       copy_code);
                                        }
#endif
                                        error_code = ENOMEM;
                                }
                        }

                }       /* else allocated memory */
        }       /* else copy of user struct succeeded */

        cipher_struct.function_return_code = return_code;
        copy_to_user((void *) scc_data, &cipher_struct, sizeof(cipher_struct));


        /* clean up */
        if (area_in)
        {
                kfree(area_in);
        }
        if (area_out)
        {
                kfree(area_out);
        }

        return error_code;
}

/*================================================================================================*/
/*****************************************************************************/
/* scc_test_zeroize() */
/*****************************************************************************/
/**
 * Have the driver Zeroize the SCC memory
 *
 * @param scc_data - the data location
 *
 * @return 0 on success, -errno on failure
 */
/*================================================================================================*/
static int scc_test_zeroize(unsigned long scc_data)
{
        scc_return_t return_code = scc_zeroize_memories();

        copy_to_user((void *) scc_data, &return_code, sizeof(return_code));

        return 0;
}

/*================================================================================================*/
/** Tell Linux where to invoke driver on module load  */
module_init(scc_test_init);

/** Tell Linux where to invoke driver on module unload  */
module_exit(scc_test_cleanup);

/** Tell Linux this isn't GPL code */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Freescale Semiconductor");
MODULE_DESCRIPTION("Test Device Driver for SCC (SMN/MXC91231 Driver");

/** Create a place to track/notify sleeping processes */
DECLARE_WAIT_QUEUE_HEAD(waitQueue);
