/*================================================================================================

        @file   rng_test_module.c

        @brief  rng API

==================================================================================================*/
/*
 * Copyright 2004-2006 Freescale Semiconductor, Inc. All Rights Reserved.
 */

/*
 * The code contained herein is licensed under the GNU General Public
 * License. You may obtain a copy of the GNU General Public License
 * Version 2 or later at the following locations:
 *
 * http://www.opensource.org/licenses/gpl-licensisr_locke.html
 * http://www.gnu.org/copyleft/gpl.html
 */

/*====================================================================================================
Revision History:
                            Modification     Tracking
Author                          Date          Number    Description of Changes
-------------------------   ------------    ----------  -------------------------------------------
Rakesh S Joshi               29/08/2006     TLSbo74375  Initial version
A.Ozerov/b00320              11/12/2006     TLSbo84161  Minor changes.
Rakesh S Joshi               23/01/2007     TLSbo87892  Added schedular to NonBlock RNGA tests.

====================================================================================================
Portability:  ARM GCC
==================================================================================================

==================================================================================================
Total Tests: 1

Test Executable Name:  rng_test_module.ko

Test Strategy:  Examine the RNG module functions
=================================================================================================

==================================================================================================
                                        INCLUDE FILES
==================================================================================================*/

#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/device.h>
#include <linux/delay.h>
#include <linux/fs.h>
#include <linux/version.h>
#include <asm/arch-mxc/mxc_security_api.h>
#include "rng_test_module.h"

/*==================================================================================================
                                 GLOBAL VARIABLE DECLARATIONS
==================================================================================================*/

static __u32 major_dev_num;
static struct class *rng_class;
static int left_out, count, i;
static uint32_t reference_no;
static uint8_t *data_buffer;
static uint8_t *data_buffer_nb;
int ret_rng = RNG_SUCCESS;
static unsigned int result_count;
fsl_shw_result_t *result;
rng_random_seed get_struct;

/*==================================================================================================
                                     FUNCTION PROTOTYPES
==================================================================================================*/

static int rng_test_init (void);
static DECLARE_WAIT_QUEUE_HEAD(rngdone);
void rng_test_exit (void);
void rng_request(fsl_shw_uco_t *user_ctx);

/*==================================================================================================
                                      FUNCTION DECLARATIONS
===================================================================================================*/

static int rng_open(struct inode *inode,  struct file *filp)
{
        return 0;
}

static int rng_release(struct inode *inode, struct file *filp)
{
        return 0;
}


void rng_request(fsl_shw_uco_t *user_ctx)
{
        unsigned long arg;
        fsl_shw_return_t ret;
            ret = fsl_shw_get_results(user_ctx, count, result, &result_count);
            if (ret == FSL_RETURN_OK_S){
                if(fsl_shw_ro_get_status((result)) == FSL_RETURN_OK_S)
                    data_buffer_nb[i] = data_buffer[fsl_shw_ro_get_reference(result)];
                i++;
             }else
                printk(KERN_ERR "RANDOM NUMBER NOT OBTAINED\n");
            wake_up(&rngdone);

}


static int rng_test_ioctl(struct inode *inode, struct file *file, unsigned int cmd, unsigned long arg)
{
        fsl_shw_uco_t *user_ctx = kmalloc(sizeof(fsl_shw_uco_t), GFP_KERNEL);
        int i = 0;
        int ret_rng = RNG_SUCCESS;
        DEFINE_WAIT(wait);
        switch (cmd)
        {
                case CASE_TEST_RNG_UCOREGISTER_POOL:
                /* This CASE Checks the FSL_SHW_REGISTER_USER when Pool size is passed as ZERO while initializing the
                UCO(USER CONTEXT OBJECT). So the FSL_SHW_REGISTER_USER() should fail with a FSL_RETURN_NO_RESOURCE_S
                return Value. Then the TEST works as expected.*/

                        if (user_ctx != NULL) {
                                fsl_shw_return_t ret;
                                fsl_shw_uco_init(user_ctx, 0);
                                ret = fsl_shw_register_user(user_ctx);
                                if (ret == FSL_RETURN_NO_RESOURCE_S) {
                                        printk(KERN_ERR "TEST CASE WORKED AS EXPECTED\n");
                                }else{
                                        printk(KERN_ERR "\nRNG DRIVER FAILED");
                                        printk(KERN_ERR "ERROR:UCO REGISTERED WHEN POOL SIZE IS ZERO.\n");
                                        ret_rng = RNG_FAILURE;
                                        ret = fsl_shw_deregister_user(user_ctx);
                                        if (ret == FSL_RETURN_OK_S)
                                        {
                                                printk(KERN_INFO "rng driver deregistered\n");
                                        }else{
                                                printk(KERN_ERR "RNG DRIVER FAILED TO DEREGISTER)\n");
                                                ret_rng = RNG_FAILURE;
                                        }
                                }
                        }
                        else
                        {
                                printk(KERN_ERR "Failed to Allocate memory for USER CONTEXT\n");
                                ret_rng = RNG_FAILURE;
                        }
                break;

                case CASE_TEST_RNG_UCOREGISTER_FLAGS:
                /* This CASE checks the FSL_SHW_REGISTER_USER when the callback and blocking flags are both set.
                fsl_shw_register_user() will fail with a FSL_RETURN_BAD_FLAG_S return value. That is, callback
                is not supported in blocking mode. Then the TEST works as expected.*/

                        if (user_ctx != NULL) {
                                fsl_shw_return_t ret;
                                fsl_shw_uco_init(user_ctx, 3);
                                fsl_shw_uco_set_flags(user_ctx,FSL_UCO_BLOCKING_MODE); /* set to blocking mode */
                                fsl_shw_uco_set_flags(user_ctx,FSL_UCO_CALLBACK_MODE); /* set to callback mode */
                                ret = fsl_shw_register_user(user_ctx);
                                if (ret == FSL_RETURN_BAD_FLAG_S){
                                        printk(KERN_INFO "TEST CASE WORKED AS EXPECTED\n");
                                }else{
                                        printk(KERN_ERR "\nRNG DRIVER FAILED");
                                        printk(KERN_ERR "ERROR: UCO REGISTERED WHEN BOTH BLOCKING AND CALLBACK MODE FLAGS ARE SET.\n");
                                        ret_rng = RNG_FAILURE;
                                        ret = fsl_shw_deregister_user(user_ctx);
                                        if (ret == FSL_RETURN_OK_S)
                                        {
                                                printk(KERN_ERR "rng driver deregistered\n");
                                        }else{
                                                printk(KERN_ERR "RNG DRIVER FAILED TO DEREGISTERED)\n");
                                                ret_rng = RNG_FAILURE;
                                        }
                                }
                                fsl_shw_uco_clear_flags(user_ctx, FSL_UCO_BLOCKING_MODE);
                                fsl_shw_uco_clear_flags(user_ctx, FSL_UCO_CALLBACK_MODE);
                        }else{
                                printk(KERN_ERR "Failed to Allocate memory for USER CONTEXT\n");
                                ret_rng = RNG_FAILURE;
                        }
                break;

                case CASE_TEST_RNG_GETRANDOM_BLK:
                        /* This test will get the random number in Blocking Mode. */
                        if (user_ctx != NULL) {
                                fsl_shw_return_t ret;
                                fsl_shw_uco_init(user_ctx, 3);
                                fsl_shw_uco_set_flags(user_ctx,FSL_UCO_BLOCKING_MODE); /* set to blocking mode */
                                fsl_shw_uco_clear_flags(user_ctx, FSL_UCO_CALLBACK_MODE); /* Make sure the callback mode is cleared */
                                ret = fsl_shw_register_user(user_ctx);
                                if (ret != FSL_RETURN_OK_S) {
                                        printk(KERN_ERR "RNG DRIVER failed to  REGISTER\n");
                                        ret_rng = RNG_FAILURE;
                                }else{
                                        printk(KERN_INFO "RNG DRIVER REGISTERED\n");
                                        left_out = copy_from_user(&get_struct,(rng_random_seed *)arg,sizeof(get_struct));
                                        if(left_out) {
                                                printk(KERN_WARNING "%d of Bytes have not been copied from user\n",left_out);
                                                ret_rng = RNG_FAILURE;
                                        } else {
                                                count = (get_struct.rng_no + sizeof(uint32_t) - 1) / 4;
                                                count *= 4;
                                                data_buffer = kmalloc(count, GFP_KERNEL);
                                                ret= fsl_shw_get_random(user_ctx, count, data_buffer);
                                                if (ret != FSL_RETURN_OK_S) {
                                                        printk(KERN_ERR "RNG DRIVER FAILED TO GET THE RANDOM NUMBERS\n");
                                                        ret_rng = RNG_FAILURE;
                                                }else{
                                                        get_struct.rng_data = data_buffer ;
                                                        get_struct.rng_no = count;
                                                        left_out = copy_to_user(((rng_random_seed *)arg)->rng_data, data_buffer, get_struct.rng_no);
                                                        if(left_out) {
                                                                printk(KERN_WARNING "%d Bytes have not been copied to user\n",left_out);
                                                                ret_rng = RNG_FAILURE;
                                                        }
                                                }
                                        }
                                        ret = fsl_shw_deregister_user(user_ctx);
                                        if (ret == FSL_RETURN_OK_S){
                                                printk(KERN_INFO "RNG DRIVER DEREGISTERED SUCCESSFULLY\n");
                                        }else{
                                                printk(KERN_ERR "RNG DRIVER FAILED TO DEREGISTER\n");
                                                ret_rng = RNG_FAILURE;
                                        }
                                }
                                kfree(data_buffer);
                                fsl_shw_uco_clear_flags(user_ctx, FSL_UCO_BLOCKING_MODE);
                        }else{
                                printk(KERN_ERR "Failed to Allocate memory for USER CONTEXT\n");
                                ret_rng = RNG_FAILURE;
                        }
                break;
                case CASE_TEST_RNG_GETRANDOM_NONBLK:
                        /* This test will get the random number in Non-Blocking Mode, without callback function */
                        if (user_ctx != NULL) {
                                fsl_shw_return_t ret;
                                fsl_shw_uco_init(user_ctx, 3);
                                fsl_shw_uco_clear_flags(user_ctx,FSL_UCO_CALLBACK_MODE); /* Clear to callback mode */
                                fsl_shw_uco_clear_flags(user_ctx, FSL_UCO_BLOCKING_MODE); /* Clear the Blocking mode */
                                ret = fsl_shw_register_user(user_ctx);
                                if (ret != FSL_RETURN_OK_S) {
                                        printk(KERN_ERR "RNG DRIVER FAILED TO REGISTER\n");
                                        ret_rng = RNG_FAILURE;
                                }else{
                                        printk(KERN_INFO "RNG DRIVER REGISTERED\n");
                                        left_out = copy_from_user(&get_struct,(rng_random_seed *)arg,sizeof(get_struct));
                                        if(left_out) {
                                                printk(KERN_WARNING "%d Bytes have not been copied from user\n",left_out);
                                                ret_rng = RNG_FAILURE;
                                        } else {
                                                count = (get_struct.rng_no + sizeof(uint32_t) - 1) / 4;
                                                count *= 4;
                                                data_buffer = kmalloc(count, GFP_KERNEL);
                                                data_buffer_nb = kmalloc(count, GFP_KERNEL);
                                                result = kmalloc((sizeof(fsl_shw_result_t) * count), GFP_KERNEL);
                                                for (reference_no = 0; reference_no < count; reference_no++) {
                                                        fsl_shw_uco_set_reference(user_ctx, reference_no);
                                                        ret= fsl_shw_get_random(user_ctx, count, data_buffer);
                                                        if (ret != FSL_RETURN_OK_S) {
                                                            printk(KERN_ERR "RNG DRIVER FAILED TO REQUEST %d SERVICE\n",reference_no);
                                                            ret_rng = RNG_FAILURE;
                                                        }
                                                }
                                                i = 0;
                                                do {
                                                        ret = fsl_shw_get_results(user_ctx, count, result, &result_count);
                                                        if (ret == FSL_RETURN_OK_S){
                                                                i++;
                                                                mdelay(5);
                                                        }
                                                } while (i < count);

                                                for (i=0; i < count; i++){
                                                        if(fsl_shw_ro_get_status((result+i)) == FSL_RETURN_OK_S) {
                                                            data_buffer_nb[i] = data_buffer[fsl_shw_ro_get_reference(result+i)];
                                                        }else{
                                                                printk(KERN_WARNING "%d RANDOM NO NOT OBTAINED\n",i);
                                                        }
                                                }
                                                get_struct.rng_data = data_buffer_nb ;
                                                get_struct.rng_no = count;
                                                        left_out = copy_to_user(((rng_random_seed *)arg)->rng_data, data_buffer_nb, get_struct.rng_no);
                                                        if(left_out) {
                                                                printk(KERN_WARNING "%d Bytes have not been copied to user\n",left_out);
                                                                ret_rng = RNG_FAILURE;
                                                        }
                                        }
                                        ret = fsl_shw_deregister_user(user_ctx);
                                        if (ret == FSL_RETURN_OK_S)
                                        {
                                                printk(KERN_INFO "RNG DRIVER DEREGISTERED SUCCESSFULLY\n");
                                        }else{
                                                printk(KERN_ERR "RNG DRIVER FAILED TO DEREGISTER\n");
                                                ret_rng = RNG_FAILURE;
                                        }
                                }
                        }else{
                                printk(KERN_ERR "Failed to Allocate memory for USER CONTEXT\n");
                                ret_rng = RNG_FAILURE;
                        }
                break;

                case CASE_TEST_RNG_GETRANDOM_NONBLK_CALLBACK:
                        /* This test will get the random number in Non-Blocking Mode,with call back function enabled */
                        if (user_ctx != NULL) {
                                fsl_shw_return_t ret;
                                fsl_shw_uco_init(user_ctx, 3);
                                fsl_shw_uco_set_flags(user_ctx, FSL_UCO_CALLBACK_MODE); /* Set to CallBack mode */
                                fsl_shw_uco_clear_flags(user_ctx, FSL_UCO_BLOCKING_MODE); /*Clear the Blocking mode */

                                fsl_shw_uco_set_callback(user_ctx, rng_request);
                                ret = fsl_shw_register_user(user_ctx);
                                if (ret != FSL_RETURN_OK_S) {
                                        printk(KERN_ERR "RNG DRIVER FAILED TO REGISTER\n");
                                        ret_rng = RNG_FAILURE;
                                }else{
                                        printk(KERN_INFO "RNG DRIVER REGISTERED\n");
                                        left_out = copy_from_user(&get_struct,(rng_random_seed *)arg,sizeof(get_struct));
                                        if(left_out) {
                                                printk(KERN_WARNING "%d Bytes have not been copied from user\n",left_out);
                                                ret_rng = RNG_FAILURE;
                                        } else {
                                                count = (get_struct.rng_no + sizeof(uint32_t) - 1) / 4;
                                                count *= 4;
                                                data_buffer = kmalloc(count, GFP_KERNEL);
                                                data_buffer_nb = kmalloc(count, GFP_KERNEL);
                                                result = kmalloc((sizeof(fsl_shw_result_t) * count), GFP_KERNEL);
                                                for (reference_no = 0; reference_no < count; reference_no++) {
                                                        fsl_shw_uco_set_reference(user_ctx, reference_no);
                                                        ret=fsl_shw_get_random(user_ctx, 1 , data_buffer);
                                                        if (ret != FSL_RETURN_OK_S) {
                                                            printk(KERN_ERR "RNG DRIVER FAILED TO REQUEST %d SERVICE\n",reference_no);
                                                            ret_rng = RNG_FAILURE;
                                                        }
                                                }
                                                prepare_to_wait(&rngdone, &wait, TASK_UNINTERRUPTIBLE);
                                                schedule();
                                                finish_wait(&rngdone, &wait);

                                                get_struct.rng_data = data_buffer_nb ;
                                                get_struct.rng_no = count;
                                                left_out = copy_to_user(((rng_random_seed *)arg)->rng_data, data_buffer_nb, get_struct.rng_no);
                                                if(left_out) {
                                                    printk(KERN_WARNING "%d Bytes have not been copied to user\n",left_out);
                                                    ret_rng = RNG_FAILURE;
                                                }
                                        }
                                }
                        }else{
                                printk(KERN_ERR "Failed to Allocate memory for USER CONTEXT\n");
                                ret_rng = RNG_FAILURE;
                        }
                break;

                case CASE_TEST_RNG_ADDENTROPY_BLK:
                        /* This test will add the new seed value in Blocking Mode. */
                        if (user_ctx != NULL) {
                                fsl_shw_return_t ret;

                                fsl_shw_uco_init(user_ctx, 3);
                                fsl_shw_uco_set_flags(user_ctx,FSL_UCO_BLOCKING_MODE); /* set to blocking mode */
                                fsl_shw_uco_clear_flags(user_ctx, FSL_UCO_CALLBACK_MODE); /* Make sure the callback mode is cleared */
                                ret = fsl_shw_register_user(user_ctx);
                                if (ret != FSL_RETURN_OK_S) {
                                        printk(KERN_ERR "RNG DRIVER FAILED TO  REGISTER\n");
                                }else{
                                        printk(KERN_INFO "RNG DRIVER REGISTERED\n");
                                        left_out = copy_from_user(&get_struct,(rng_random_seed *)arg,sizeof(get_struct));
                                        if(left_out){
                                                printk(KERN_WARNING "%d of Bytes have not been copied from user\n", left_out);
                                                ret_rng = RNG_FAILURE;
                                        }else{
                                                count = get_struct.rng_no;
                                                data_buffer = kmalloc(count, GFP_KERNEL);
                                                ret = fsl_shw_add_entropy(user_ctx, count, data_buffer);
                                                if (ret != FSL_RETURN_OK_S ){
                                                    printk(KERN_ERR "ERROR WHILE ADDING ENTROPY\n");
                                                    ret_rng = RNG_FAILURE;
                                                }
                                        }
                                        ret = fsl_shw_deregister_user(user_ctx);
                                        if (ret == FSL_RETURN_OK_S)
                                        {
                                            printk(KERN_INFO "RNG DRIVER DEREGISTERED SUCCESSFULLY\n");
                                        }else{
                                            printk(KERN_ERR "RNG DRIVER failed to DEREGISTER\n");
                                            ret_rng = RNG_FAILURE;
                                        }
                                }
                                fsl_shw_uco_clear_flags(user_ctx, FSL_UCO_BLOCKING_MODE);
                        }else{
                                printk(KERN_ERR "Failed to Allocate memory for USER CONTEXT\n");
                                ret_rng = RNG_FAILURE;
                        }
                break;

                case CASE_TEST_RNG_ADDENTROPY_NONBLK:
                        /* This test will add the new seed value in Non-Blocking Mode. This is similar to the Blocking mode*/
                        if (user_ctx != NULL) {
                                fsl_shw_return_t ret;

                                fsl_shw_uco_init(user_ctx, 3);
                                fsl_shw_uco_clear_flags(user_ctx,FSL_UCO_BLOCKING_MODE);/* clear blocking mode */
                                fsl_shw_uco_clear_flags(user_ctx,FSL_UCO_CALLBACK_MODE); /* clear the callback mode */
                                ret = fsl_shw_register_user(user_ctx);
                                if (ret != FSL_RETURN_OK_S) {
                                        printk(KERN_ERR "RNG DRIVER FAILED TO REGISTER\n");
                                }else{
                                        printk(KERN_INFO "RNG DRIVER REGISTERED\n");
                                        left_out = copy_from_user(&get_struct,(rng_random_seed *)arg,sizeof(get_struct));
                                        if(left_out){
                                                printk(KERN_ERR "%d of Bytes have not been copied from user\n", left_out);
                                                ret_rng = RNG_FAILURE;
                                        }else{
                                                count = get_struct.rng_no;
                                                data_buffer = kmalloc(count, GFP_KERNEL);
                                                ret = fsl_shw_add_entropy(user_ctx, count, data_buffer);
                                                if (ret != FSL_RETURN_OK_S ){
                                                    printk(KERN_ERR "Error while adding entropy\n");
                                                    ret_rng = RNG_FAILURE;
                                                }
                                        }
                                        ret = fsl_shw_deregister_user(user_ctx);
                                        if (ret == FSL_RETURN_OK_S)
                                        {
                                            printk(KERN_INFO "RNG DRIVER DEREGISTERED SUCCESSFULLY\n");
                                        }else{
                                            printk(KERN_ERR "RNG DRIVER failed to DEREGISTER\n");
                                            ret_rng = RNG_FAILURE;
                                        }
                                }
                                fsl_shw_uco_clear_flags(user_ctx, FSL_UCO_BLOCKING_MODE);
                        }else{
                                printk(KERN_ERR "Failed to Allocate memory for USER CONTEXT\n");
                                ret_rng = RNG_FAILURE;
                        }

                break;

                default:
                        printk(KERN_ERR "RNG TEST: INVALID OPTION");
                break;
        }/* END OF SWITCH STATEMENT */
        kfree(user_ctx);
        return ret_rng;
}/* END of rng_test_ioctl */

/*==================================================================================================
                                 STRUCTURES AND OTHER TYPEDEFS
==================================================================================================*/

static struct file_operations rng_test_fops =
{
        owner:THIS_MODULE,
        open:rng_open,
        release:rng_release,
        ioctl:rng_test_ioctl,
};


/*=================================================================================================================*/

static int __init rng_test_init(void)
{

        if ((major_dev_num = register_chrdev(0, RNG_DEVICE_NAME, &rng_test_fops)) < 0 )
        {
                printk(KERN_WARNING "rng Module Test: unable to register the dev\n");
                return major_dev_num;
        }

        rng_class = class_create(THIS_MODULE, RNG_DEVICE_NAME);

        if (IS_ERR(rng_class))
           {
 	       printk(KERN_ALERT "class simple created failed\n");
                   goto err_out;
           }
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,26))
        if (IS_ERR(class_device_create(rng_class,NULL,
                                              MKDEV(major_dev_num, 0), NULL,
                                              RNG_DEVICE_NAME)))
#else
        if (IS_ERR(device_create(rng_class,NULL,
                                              MKDEV(major_dev_num, 0),
                                              RNG_DEVICE_NAME)))
#endif
           {
 	       printk(KERN_ALERT "class simple add failed\n");
                   goto err_out;
           }
        printk(KERN_INFO "Module load succesful\n");
        return 0;

err_out:
        printk(KERN_ERR "rng : error creating rng test module class.\n");
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,26))
        class_device_destroy(rng_class, MKDEV(major_dev_num, 0));
#else
        device_destroy(rng_class, MKDEV(major_dev_num, 0));
#endif      
        class_destroy(rng_class);
        unregister_chrdev(major_dev_num, RNG_DEVICE_NAME);
        return -1;

}

/*=================================================================================================================*/

void __exit rng_test_exit(void)
{
        unregister_chrdev(major_dev_num, RNG_DEVICE_NAME);
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,26))
        class_device_destroy(rng_class, MKDEV(major_dev_num, 0));
#else
        device_destroy(rng_class, MKDEV(major_dev_num, 0));
#endif

        class_destroy(rng_class);
        printk(KERN_INFO "Module unload succesful\n");
}


/*=================================================================================================================*/

module_init(rng_test_init);
module_exit(rng_test_exit);

MODULE_DESCRIPTION("Test Module for RNG Argon + drivers");
MODULE_LICENSE("GPL");


