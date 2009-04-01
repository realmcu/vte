/*================================================================================================*/
/**
        @file   pmic_rtc_module.c

        @brief  PMIC RTC test module
*/
/*==================================================================================================

        Copyright (C) 2006, Freescale Semiconductor, Inc. All Rights Reserved
        THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
        BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
        reescale Semiconductor, Inc.

====================================================================================================
Revision History:
                            Modification     Tracking
Author/core ID                  Date          Number    Description of Changes
-------------------------   ------------    ----------  -------------------------------------------
Pradeep K / b01016           09/25/2006     TLSboXXX    Initial version

====================================================================================================
Portability:  ARM GCC
==================================================================================================*/

/*==================================================================================================
                                        INCLUDE FILES
==================================================================================================*/
//#include <linux/config.h>
#include <linux/poll.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/device.h>
#include <linux/version.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <asm/uaccess.h>

#include "pmic_rtc_module.h"

/*==================================================================================================
                                       GLOBAL VARIABLES
==================================================================================================*/

static struct class *pmic_rtc_class;
static int pmic_rtc_major = 0;

static void callback_alarm_asynchronous(void);
static pmic_event_callback_t alarm_event;
static bool pmic_rtc_done = 0;

/*==================================================================================================
                                       DEFINES AND MACROS
==================================================================================================*/
#ifdef __ALL_TRACES__
#define TRACEMSG_RTC(fmt,args...)  printk(fmt,##args)
#else                           /* __ALL_TRACES__ */
#define TRACEMSG_RTC(fmt,args...)
#endif                          /* __ALL_TRACES__ */

/*================================================================================================*/

#define CHECK_ERROR(a)                  \
do {                                    \
        int ret = (a);                  \
        if(ret != PMIC_SUCCESS)         \
        return ret;                     \
} while (0)

/*==================================================================================================
                                     LOCAL FUNCTION
==================================================================================================*/
/*!
 * This is the callback function called on TSI Pmic event, used in asynchronous
 * call.
 */
static void callback_alarm_asynchronous(void)
{
        pmic_rtc_done = true;
}
/*================================================================================================*/

/*!
 * This is the callback function is used in test code for (un)sub.
 */
static void callback_test_sub(void)
{
        printk(KERN_INFO"*****************************************");
        printk(KERN_INFO"***** Pmic RTC 'Alarm IT CallBack' *****");
        printk(KERN_INFO"*****************************************");
}
/*================================================================================================*/
/*!
 * This function implements IOCTL controls on a PMIC RTC device.
 *
 * @param        inode       pointer on the node
 * @param        file        pointer on the file
 * @param        cmd         the command
 * @param        arg         the parameter
 * @return       This function returns 0 if successful.
 */
/*===============================================================================================*/
static int pmic_rtc_ioctl(struct inode *inode, struct file *file,
                          unsigned int cmd, unsigned long arg)
{
        struct timeval *pmic_time = NULL;

        if (_IOC_TYPE(cmd) != 'p')
                return -ENOTTY;

        if (arg) {
                if ((pmic_time = kmalloc(sizeof(struct timeval),
                                         GFP_KERNEL)) == NULL) {
                        return -ENOMEM;
                }
                /* if (copy_from_user(pmic_time, (struct timeval *)arg,
                        sizeof(struct timeval)))
                {
                        return -EFAULT;
                }
                */
        }

        switch (cmd) {
        case PMIC_RTC_SET_TIME:
                if (copy_from_user(pmic_time, (struct timeval *)arg,
                                   sizeof(struct timeval))) {
                        return -EFAULT;
                }
                TRACEMSG_RTC(_K_D("SET RTC"));
                CHECK_ERROR(pmic_rtc_set_time(pmic_time));
                break;
        case PMIC_RTC_GET_TIME:
                if (copy_to_user((struct timeval *)arg, pmic_time,
                                 sizeof(struct timeval))) {
                        return -EFAULT;
                }
                TRACEMSG_RTC(_K_D("GET RTC"));
                CHECK_ERROR(pmic_rtc_get_time(pmic_time));
                break;
        case PMIC_RTC_SET_ALARM:
                if (copy_from_user(pmic_time, (struct timeval *)arg,
                                   sizeof(struct timeval))) {
                        return -EFAULT;
                }
                TRACEMSG_RTC(_K_D("SET RTC ALARM"));
                CHECK_ERROR(pmic_rtc_set_time_alarm(pmic_time));
                break;
        case PMIC_RTC_GET_ALARM:
                if (copy_to_user((struct timeval *)arg, pmic_time,
                                 sizeof(struct timeval))) {
                        return -EFAULT;
                }
                TRACEMSG_RTC(_K_D("GET RTC ALARM"));
                CHECK_ERROR(pmic_rtc_get_time_alarm(pmic_time));
                break;
        case PMIC_RTC_WAIT_ALARM:
                TRACEMSG_RTC(_K_I("WAIT ALARM..."));
                CHECK_ERROR(pmic_rtc_event_sub(RTC_IT_ALARM,
                                               callback_test_sub));
                CHECK_ERROR(pmic_rtc_wait_alarm());
                TRACEMSG_RTC(_K_I("ALARM DONE"));
                CHECK_ERROR(pmic_rtc_event_unsub(RTC_IT_ALARM,
                                                 callback_test_sub));
                break;
        case PMIC_RTC_ALARM_REGISTER:
                TRACEMSG_RTC(_K_I("PMIC RTC ALARM REGISTER"));
                alarm_event.param = NULL;
                alarm_event.func = (void *)callback_alarm_asynchronous;
                CHECK_ERROR(pmic_event_subscribe(EVENT_TODAI, alarm_event));
                break;
        case PMIC_RTC_ALARM_UNREGISTER:
                TRACEMSG_RTC(_K_I("PMIC RTC ALARM UNREGISTER"));
                alarm_event.param = NULL;
                alarm_event.func = (void *)callback_alarm_asynchronous;
                CHECK_ERROR(pmic_event_unsubscribe(EVENT_TODAI, alarm_event));
                pmic_rtc_done = false;
                break;
        default:
                TRACEMSG_RTC(_K_D("%d unsupported ioctl command"), (int)cmd);
                return -EINVAL;
        }

        if (arg) {
                if (copy_to_user((struct timeval *)arg, pmic_time,
                                 sizeof(struct timeval))) {
                        return -EFAULT;
                }
                kfree(pmic_time);
        }

        return 0;
}

/*===============================================================================================*/
/* Called without the kernel lock - fine */
static unsigned int pmic_rtc_poll(struct file *file, poll_table * wait)
{
        /*poll_wait(file, &pmic_rtc_wait, wait); */

        if (pmic_rtc_done)
                return POLLIN | POLLRDNORM;
        return 0;
}

/*================================================================================================*/

/*!
 * This function implements the open method on a PMIC RTC device.
 *
 * @param        inode       pointer on the node
 * @param        file        pointer on the file
 * @return       This function returns 0.
 */
static int pmic_rtc_open(struct inode *inode, struct file *file)
{
        return 0;
}
/*================================================================================================*/

/*!
 * This function implements the release method on a PMIC RTC device.
 *
 * @param        inode       pointer on the node
 * @param        file        pointer on the file
 * @return       This function returns 0.
 */
static int pmic_rtc_release(struct inode *inode, struct file *file)
{
        return 0;
}

/*=================================================================================================
GLOBAL VARIABLE DECLARATIONS
==================================================================================================*/
static struct file_operations pmic_rtc_fops = {
        .owner = THIS_MODULE,
        .ioctl = pmic_rtc_ioctl,
        .poll = pmic_rtc_poll,
        .open = pmic_rtc_open,
        .release = pmic_rtc_release,
};

/*================================================================================================*/

/*
 * Init and Exit
 */

static int __init pmic_rtc_init(void)
{
        printk("PMIC RTC Test: creating virtual device\n");
        pmic_rtc_major = register_chrdev(0, PMIC_RTC_DEV, &pmic_rtc_fops);

        if (pmic_rtc_major < 0) {
                printk(KERN_WARNING "PMIC RTC Test: unable to register the device\n");
                return pmic_rtc_major;
        }

        pmic_rtc_class = class_create(THIS_MODULE, PMIC_RTC_DEV);
                if (IS_ERR(pmic_rtc_class)) {
                printk(KERN_ALERT "class simple create failed\n");
                   goto err_out;
                }

                if (IS_ERR(class_device_create(pmic_rtc_class,NULL,MKDEV(pmic_rtc_major, 0), NULL,PMIC_RTC_DEV))) {
                printk(KERN_ALERT "class simple add failed\n");
                        goto err_out;
        }

        //---devfs_mk_cdev(MKDEV(pmic_rtc_major, 0), S_IFCHR | S_IRUGO | S_IWUGO, PMIC_RTC_DEV);
        return 0;

        err_out:
                        printk(KERN_ERR "PMIC_RTC : error creating pmic rtc test module class.\n");
                        class_device_destroy(pmic_rtc_class , MKDEV(pmic_rtc_major, 0));
                        class_destroy(pmic_rtc_class);
                        unregister_chrdev(pmic_rtc_major, PMIC_RTC_DEV);
                        return -1;

}

/*================================================================================================*/
static void __exit pmic_rtc_exit(void)
{
        unregister_chrdev(pmic_rtc_major, PMIC_RTC_DEV);
        //---devfs_remove(PMIC_RTC_DEV);
        class_device_destroy(pmic_rtc_class, MKDEV(pmic_rtc_major, 0));
        class_destroy(pmic_rtc_class);
        printk("PMIC RTC Test: removing virtual device\n");
}

/*================================================================================================*/

module_init(pmic_rtc_init);
module_exit(pmic_rtc_exit);

MODULE_DESCRIPTION("Test Module for PMIC RTC driver");
MODULE_LICENSE("GPL");
