/*================================================================================================*/
/**
        @file   pmic_adc_module.c

        @brief  PMIC ADC test module header file
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
D.Khoroshev/B00313           07/06/2006     TLSbo64235  Initial version

====================================================================================================
Portability:  ARM GCC
==================================================================================================*/
#include <asm/ioctl.h>
#include <linux/config.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/device.h>
#include <linux/poll.h>
#include <linux/sched.h>
#include <linux/time.h>
#include <linux/delay.h>
#include <linux/wait.h>
#include <linux/version.h>
#include <linux/devfs_fs_kernel.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <asm/uaccess.h>

#include <asm/arch/pmic_status.h>
#include <asm/arch/pmic_external.h>
#include "asm/arch/pmic_power.h"

#include "pmic_adc_module.h"

#ifndef TRACEMSG
#define TRACEMSG(fmt,args...) printk(fmt, ##args)
#endif

#ifndef _K_D
#define _K_D(a...) (KERN_WARNING #a)
#endif

#ifndef CHECK_ERROR
#define CHECK_ERROR(a) \
        do { \
                int ret = 0; \
                if ((ret=a) != PMIC_SUCCESS) { \
                        TRACEMSG(#a" returned error %d", ret); \
                        return ret; \
                } \
        } while (0);
#endif

#ifndef CHECK_ERROR_KFREE
#define CHECK_ERROR_KFREE(a, b) \
        do { \
                int ret = 0; \
                if ((ret=a) != PMIC_SUCCESS) { \
                        TRACEMSG(#a" returned error %d", ret); \
                        b; \
                        return ret; \
                } \
        } while (0);
#endif


static int pmic_adc_major;
static type_event_notification tsi_event;
static int data_ready;
static int buf_size = 4096;        /* Buffer size */
static short *buffer;
static int buf_count;
static bool read_ts_installed = false;
static struct class_simple *pmic_adc_class;

static DECLARE_WAIT_QUEUE_HEAD(queue_tsi_read);
static DECLARE_WAIT_QUEUE_HEAD(adcdone_it);
static DECLARE_WAIT_QUEUE_HEAD(adc_tsi);
static DECLARE_WAIT_QUEUE_HEAD(pen_down);

/*!
 * This is the suspend of power management for the sc55112 ADC API.
 * It supports SAVE and POWER_DOWN state.
 *
 * @param        dev            the device
 * @param        state          the state
 * @param        level          the level
 *
 * @return       This function returns 0 if successful.
 */
static int pmic_adc_suspend(struct device *dev, u32 state, u32 level)
{
        /* not supported */
        return -1;
};

/*!
 * This is the resume of power management for the sc55112 ADC API.
 * It supports RESTORE state.
 *
 * @param        dev            the device
 * @param        level          the level
 *
 * @return       This function returns 0 if successful.
 */
static int pmic_adc_resume(struct device *dev, u32 level)
{
        /* not supported */
        return -1;
};

/*!
 * This function is used to update buffer of touch screen value in read mode.
 */
int update_buffer(void *not_used)
{
        t_touch_sample ts_value = { 0, 0, 0 };

        DEFINE_WAIT(wait);

        do {
                /*
                 * Wait for a pen down event notification.
                 */
                prepare_to_wait(&pen_down, &wait, TASK_INTERRUPTIBLE);
                schedule();
                finish_wait(&pen_down, &wait);

                TRACEMSG("Got pen_down event.\n");

                /*
                 * Get the touchpanel (X,Y) and pressure samples. Continue
                 * getting samples until the pressure is zero (i.e., the
                 * user is no longer pressing on the touchpanel).
                 *
                 * Note that we must initialize the pressure to zero before
                 * entering the sampling loop so that we avoid the msleep()
                 * delay that is required between successive samples.
                 */
                ts_value.pressure = 0;
                do {
                        if (ts_value.pressure > 0) {
                                /* Sleep for 100 milliseconds before resampling
                                 * the touchpanel. We must do this between
                                 * touchpanel samples so that the (X,Y)
                                 * coordinates have a fixed and known time
                                 * interval which can then be used to determine
                                 * the relative speed between samples.
                                 */
                                msleep(100);
                        }

                        /* Call our PMIC function to obtain the touchpanel's
                         * (X,Y) coordinate and pressure readings.
                         */
                        pmic_adc_get_touch_sample(&ts_value);

                        TRACEMSG("touchpanel (x=%d, y=%d, p=%d)\n",
                                 ts_value.x_position,
                                 ts_value.y_position, ts_value.pressure);

                        /* Add the new touchpanel sample to the data queue if
                         * there is still room available. Note that we must
                         * return a final sample with the (X,Y) and pressure
                         * values all zero to mark the end of a group of
                         * samples.
                         */
                        if (buf_count < (buf_size - 4)) {
                                buffer[buf_count + 0] =
                                    (short)ts_value.pressure;
                                buffer[buf_count + 1] =
                                    (short)ts_value.x_position;
                                buffer[buf_count + 2] =
                                    (short)ts_value.y_position;
                                buffer[buf_count + 3] = 0;        /* not used */

                                buf_count += 4;
                        }

                        /* Signal the application to read the touchpanel
                         * samples that are currently available in the data
                         * queue.
                         */
                        data_ready = 1;
                        wake_up(&queue_tsi_read);

                } while (ts_value.pressure >= 1);

        } while (read_ts_installed);

        return 0;
}

/*!
 * This is the callback function called on TSI PMIC event, used in synchronous
 * call.
 */
static void callback_tsi(void *param)
{
        if (read_ts_installed) {
                wake_up(&pen_down);
        }
}

static void ts_read_install(void)
{
        unsigned short val;

        /* Subscribe to Touch Screen interrupt */
        tsi_event.event = EVENT_TSI;
        tsi_event.callback = callback_tsi;
        tsi_event.param = NULL;
        pmic_event_subscribe(tsi_event);

        /* Enable V2 */
        pmic_power_regulator_on(V2);

        /* set touch screen in standby mode */
        pmic_adc_set_touch_mode(TS_STANDBY);

        /* Do a conversion
         * This is a bug of sc55112 -- we have to do a conversion to
         * pull up voltage of TSX1 and TSY1
         */
        pmic_adc_convert(AD7, &val);

        read_ts_installed = true;

        kernel_thread(update_buffer, NULL, CLONE_VM | CLONE_FS);
}

static void ts_read_uninstall(void)
{
        /* Unsubscribe the Touch Screen interrupt. */
        tsi_event.event = EVENT_TSI;
        pmic_event_unsubscribe(tsi_event);
        read_ts_installed = false;

        pmic_adc_set_touch_mode(TS_NONE);
}

/*!
 * This function implements the open method on a PMIC ADC device.
 *
 * @param        inode       pointer on the node
 * @param        file        pointer on the file
 * @return       This function returns 0.
 */
static int pmic_adc_open(struct inode *inode, struct file *file)
{
        TRACEMSG(_K_D("pmic_adc : pmic_adc_open()"));
        return 0;
}

/*!
 * This function implements the release method on a PMIC ADC device.
 *
 * @param        inode       pointer on the node
 * @param        file        pointer on the file
 * @return       This function returns 0.
 */
static int pmic_adc_release(struct inode *inode, struct file *file)
{
        TRACEMSG(_K_D("pmic_adc : pmic_adc_free()"));
        return 0;
}

/*!
 * This function is the read interface of PMIC ADC, It returns touch screen value.
 *
 * @param        file        pointer on the file
 * @param        buf         the user space buffer
 * @param        count       number of date
 * @param        ppos        pointer position
 *
 * @return       This function returns number of date read.
 */
static ssize_t pmic_adc_read(struct file *file, char *buf, size_t count,
                             loff_t * ppos)
{
        int ret = 0, i;
        TRACEMSG(_K_D("pmic_adc_read()"));

        /* return value of buffer with all touch screen value */
        /* is the buffer is empty return 0 */

        if ((buf_count != 0) && (count >= buf_count)) {
                copy_to_user(buf, buffer, buf_count * sizeof(short));
                ret = buf_count;
        } else {
                copy_to_user(buf, buffer, 4 * sizeof(short));
        }

        TRACEMSG(_K_D("pmic_adc : DataReady %d"), data_ready);
        TRACEMSG(_K_D("pmic_adc : contact %d, x %d, y %d, pad %d"),
                 buffer[0], buffer[1], buffer[2], buffer[3]);

        data_ready = 0;
        for (i = 0; i < buf_count; i++) {
                buffer[i] = 0;
        }
        buf_count = 0;

        return ret;
}

static unsigned int pmic_adc_poll(struct file *filp, poll_table * wait)
{
        unsigned int ret = 0;
        poll_wait(filp, &queue_tsi_read, wait);
        if (data_ready) {
                ret = POLLIN | POLLRDNORM;
        } else {
                ret = 0;
        }
        return ret;
}

/*!
 * This function implements IOCTL controls on a PMIC ADC device.
 *
 * @param        inode       pointer on the node
 * @param        file        pointer on the file
 * @param        cmd         the command
 * @param        arg         the parameter
 * @return       This function returns 0 if successful.
 */
static int pmic_adc_ioctl(struct inode *inode, struct file *file,
                          unsigned int cmd, unsigned long arg)
{

        t_adc_convert_param *convert_param;
        t_touch_mode touch_mode;
        t_touch_sample touch_sample;
        unsigned short b_current;
        t_adc_comp_param *comp_param;

        if ((_IOC_TYPE(cmd) != 'p') && (_IOC_TYPE(cmd) != 'D'))
                return -ENOTTY;

        switch (cmd) {
        case PMIC_ADC_INIT:
                pmic_adc_init();
                break;

        case PMIC_ADC_DEINIT:
                pmic_adc_deinit();
                break;

        case PMIC_ADC_CONVERT:
                if ((convert_param = kmalloc(sizeof(t_adc_convert_param),
                                             GFP_KERNEL)) == NULL) {
                        return -ENOMEM;
                }
                if (copy_from_user(convert_param, (t_adc_convert_param *) arg,
                                   sizeof(t_adc_convert_param))) {
                        kfree(convert_param);
                        return -EFAULT;
                }
                CHECK_ERROR_KFREE(pmic_adc_convert(convert_param->channel,
                                                   convert_param->result),
                                  (kfree(convert_param)));

                if (copy_to_user((t_adc_convert_param *) arg, convert_param,
                                 sizeof(t_adc_convert_param))) {
                        return -EFAULT;
                }
                kfree(convert_param);
                break;

        case PMIC_ADC_CONVERT_8X:
                if ((convert_param = kmalloc(sizeof(t_adc_convert_param),
                                             GFP_KERNEL)) == NULL) {
                        return -ENOMEM;
                }
                if (copy_from_user(convert_param, (t_adc_convert_param *) arg,
                                   sizeof(t_adc_convert_param))) {
                        kfree(convert_param);
                        return -EFAULT;
                }
                CHECK_ERROR_KFREE(pmic_adc_convert_8x(convert_param->channel,
                                                      convert_param->result),
                                  (kfree(convert_param)));

                if (copy_to_user((t_adc_convert_param *) arg, convert_param,
                                 sizeof(t_adc_convert_param))) {
                        return -EFAULT;
                }
                kfree(convert_param);
                break;

        case PMIC_ADC_CONVERT_MULTICHANNEL:
                if ((convert_param = kmalloc(sizeof(t_adc_convert_param),
                                             GFP_KERNEL)) == NULL) {
                        return -ENOMEM;
                }
                if (copy_from_user(convert_param, (t_adc_convert_param *) arg,
                                   sizeof(t_adc_convert_param))) {
                        kfree(convert_param);
                        return -EFAULT;
                }

                CHECK_ERROR_KFREE(pmic_adc_convert_multichnnel
                                  (convert_param->channel,
                                   convert_param->result),
                                  (kfree(convert_param)));

                if (copy_to_user((t_adc_convert_param *) arg, convert_param,
                                 sizeof(t_adc_convert_param))) {
                        return -EFAULT;
                }
                kfree(convert_param);
                break;

        case PMIC_ADC_SET_TOUCH_MODE:
                CHECK_ERROR(pmic_adc_set_touch_mode((t_touch_mode) arg));
                break;

        case PMIC_ADC_GET_TOUCH_MODE:
                CHECK_ERROR(pmic_adc_get_touch_mode(&touch_mode));
                if (copy_to_user((t_touch_mode *) arg, &touch_mode,
                                 sizeof(t_touch_mode))) {
                        return -EFAULT;
                }
                break;

        case GET_TOUCH_SCREEN_VALUE:
                TRACEMSG("pmic_adc_ioctl: GET_TOUCH_SCREEN_VALUE\n");

        case PMIC_ADC_GET_TOUCH_SAMPLE:
                TRACEMSG("pmic_adc_ioctl: " "PMIC_ADC_GET_TOUCH_SAMPLE\n");

                CHECK_ERROR(pmic_adc_get_touch_sample(&touch_sample));
                if (copy_to_user((t_touch_sample *) arg, &touch_sample,
                                 sizeof(t_touch_sample))) {
                        return -EFAULT;
                }
                break;

        case PMIC_ADC_GET_BATTERY_CURRENT:
                CHECK_ERROR(pmic_adc_get_battery_current(ADC_8CHAN_1X,
                                                         &b_current));
                if (copy_to_user((unsigned short *)arg, &b_current,
                                 sizeof(unsigned short))) {
                        return -EFAULT;
                }
                break;

        case PMIC_ADC_ACTIVATE_COMPARATOR:
                if ((comp_param = kmalloc(sizeof(t_adc_comp_param), GFP_KERNEL))
                    == NULL) {
                        return -ENOMEM;
                }
                if (copy_from_user(comp_param, (t_adc_comp_param *) arg,
                                   sizeof(t_adc_comp_param))) {
                        kfree(comp_param);
                        return -EFAULT;
                }
                CHECK_ERROR(pmic_adc_active_comparator(comp_param->wlow,
                                                       comp_param->whigh,
                                                       comp_param->callback));
                kfree(comp_param);
                break;

        case PMIC_ADC_DEACTIVE_COMPARATOR:
                CHECK_ERROR(pmic_adc_deactive_comparator());
                break;

        case TOUCH_SCREEN_READ_INSTALL:
                ts_read_install();
                TRACEMSG("pmic_adc_ioctl: TOUCH_SCREEN_READ_INSTALL\n");
                break;

        case TOUCH_SCREEN_READ_UNINSTALL:
                ts_read_uninstall();
                TRACEMSG("pmic_adc_ioctl: TOUCH_SCREEN_READ_UNINSTALL\n");
                break;

        default:
                TRACEMSG("pmic_adc_ioctl: unsupported ioctl command 0x%x\n",
                         cmd);
                return -EINVAL;
        }
        return 0;
}

static struct file_operations pmic_adc_fops = {
        .owner = THIS_MODULE,
        .poll = pmic_adc_poll,
        .read = pmic_adc_read,
        .ioctl = pmic_adc_ioctl,
        .open = pmic_adc_open,
        .release = pmic_adc_release,
};

/*
 * Initialization and Exit
 */
static int pmic_adc_module_probe(struct device *dev)
{
        struct class_device *temp_class;
        pmic_adc_major = register_chrdev(0, PMIC_ADC_DEVICE,
                                         &pmic_adc_fops);

        if (pmic_adc_major < 0) {
                TRACEMSG("Unable to get a major for %s",
                              PMIC_ADC_DEVICE);
                return pmic_adc_major;
        }

        pmic_adc_class = class_simple_create(THIS_MODULE, PMIC_ADC_DEVICE);
        if (IS_ERR(pmic_adc_class)) {
                goto err_out;
        }

        temp_class =
            class_simple_device_add(pmic_adc_class,
                                    MKDEV(pmic_adc_major, 0), NULL,
                                    PMIC_ADC_DEVICE);
        if (IS_ERR(temp_class)) {
                goto err_out;
        }

        devfs_mk_cdev(MKDEV(pmic_adc_major, 0), S_IFCHR | S_IRUGO | S_IWUSR,
                      PMIC_ADC_DEVICE);

        /* Allocate the buffer */
        buffer = (short *)kmalloc(buf_size * sizeof(short), GFP_KERNEL);
        if (buffer == NULL) {
                TRACEMSG("Unable to allocate buffer for %s device\n",
                              PMIC_ADC_DEVICE);
                return -ENOMEM;
        }
        memset(buffer, 0, buf_size * sizeof(short));

        CHECK_ERROR(pmic_adc_init());

        printk(KERN_INFO "PMIC ADC loaded\n");
        return 0;

    err_out:
        printk(KERN_ERR "Error creating pmic_adc class device.\n");
        devfs_remove(PMIC_ADC_DEVICE);
        class_simple_device_remove(MKDEV(pmic_adc_major, 0));
        class_simple_destroy(pmic_adc_class);
        unregister_chrdev(pmic_adc_major, PMIC_ADC_DEVICE);
        return -1;
}

static struct device_driver pmic_adc_driver_ldm = {
        .name = "pmic_test_adc",
        .bus = &platform_bus_type,
        .remove = NULL,
        .suspend = pmic_adc_suspend,
        .resume = pmic_adc_resume,
        .probe = pmic_adc_module_probe,
};
static int __init pmic_adc_module_init(void)
{
        int ret = 0;
        ret = driver_register(&pmic_adc_driver_ldm);
        if (!ret)
                ret = pmic_adc_module_probe(NULL);
        return ret;
}
static void __exit pmic_adc_module_exit(void)
{
        class_simple_device_remove(MKDEV(pmic_adc_major, 0));
        class_simple_destroy(pmic_adc_class);
        devfs_remove(PMIC_ADC_DEVICE);
        unregister_chrdev(pmic_adc_major, PMIC_ADC_DEVICE);
        driver_unregister(&pmic_adc_driver_ldm);
        printk(KERN_INFO "PMIC ADC unloaded\n");
}

/*
 * Module entry points
 */

module_init(pmic_adc_module_init);
module_exit(pmic_adc_module_exit);

MODULE_DESCRIPTION("PMIC Test ADC device driver");
MODULE_AUTHOR("Freescale Semiconductor, Inc.");
MODULE_LICENSE("GPL");
