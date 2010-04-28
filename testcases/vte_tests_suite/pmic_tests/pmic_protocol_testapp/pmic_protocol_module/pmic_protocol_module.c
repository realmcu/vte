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


#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/kdev_t.h>
#include <linux/major.h>
#include <linux/init.h>
#include <linux/fs.h>

#include <asm/uaccess.h>
#include <asm/arch/pmic_external.h>

static int pmic_major;
static struct class *pmic_class;

#define PMIC_NAME	"pmic"

static void callbackfn(void *event)
{
	unsigned int tmp = (unsigned int )event;
	printk(KERN_INFO "**********************************\n");
	printk(KERN_INFO "*** IT PMIC CALLBACK FUNCTION ****\n");
	printk(KERN_INFO "********* Event Nb : %d **********\n", tmp);
	printk(KERN_INFO "**********************************\n");
}

/*!
 * this function is used to initialize 'TypeEventNotification'
 * structure with the correct event.
 *
 * @param        event      event passed by the test application
 * @param        event_sub   event structure with IT and callback
 *
 */
void get_event(unsigned int event, type_event_notification * event_sub)
{
	pmic_event_init(event_sub);

	event_sub->event = event;
	event_sub->callback = callbackfn;
	event_sub->param = (void*)event;
}

/*!
 * This function implements IOCTL controls on a PMIC device.
 *
 * @param        inode       pointer on the node
 * @param        file        pointer on the file
 * @param        cmd         the command
 * @param        arg         the parameter
 * @return       This function returns 0 if successful.
 */
static int pmic_test_ioctl(struct inode *inode, struct file *file,
				 unsigned int cmd, unsigned long arg)
{
	register_info reg_info;
	type_event_notification event_sub;

	if (_IOC_TYPE(cmd) != 'P')
		return -ENOTTY;

	if (copy_from_user(&reg_info, (register_info *) arg,
			   sizeof(register_info))) {
				   printk("TEST CODE FAILED : COPY FROM USER FAILED \n");
		return -EFAULT;
	}

	switch (cmd) {
	case PMIC_READ_REG:
#ifdef CONFIG_MXC_PMIC_MC13783
		pmic_read_reg(0, reg_info.reg, &(reg_info.reg_value), PMIC_ALL_BITS);
#elif CONFIG_MXC_PMIC_SC55112
		pmic_read_reg(0, reg_info.reg, &(reg_info.reg_value));
#endif
		pr_debug("read reg %d %x\n", reg_info.reg, reg_info.reg_value);
		break;

	case PMIC_WRITE_REG:
		pmic_write_reg(0, reg_info.reg, reg_info.reg_value, PMIC_ALL_BITS);
		pr_debug("write reg %d %x\n", reg_info.reg, reg_info.reg_value);
		break;

	case PMIC_SUBSCRIBE:
		get_event(reg_info.event, &event_sub);
		//printk(" *** event sub %d\n", event_sub.event);
		pmic_event_subscribe(event_sub);
		//printk(" *** subscribe is done \n");
		pr_debug("subscribe done\n");
		break;

	case PMIC_UNSUBSCRIBE:
		get_event(reg_info.event, &event_sub);
		//printk(" *** event unsub %d\n", event_sub.event);
		pmic_event_unsubscribe(event_sub);
		//printk(" *** unsubscribe is done \n");
		pr_debug("unsubscribe done\n");
		break;

	default:
		printk("%d unsupported ioctl command\n", (int)cmd);
		return -EINVAL;
	}

	if (copy_to_user((register_info *) arg, &reg_info,
			 sizeof(register_info))) {
		return -EFAULT;
	}

	return 0;
}

/*!
 * This function implements the open method on a PMIC device.
 *
 * @param        inode       pointer on the node
 * @param        file        pointer on the file
 * @return       This function returns 0.
 */
static int pmic_test_open(struct inode *inode, struct file *file)
{
	pr_debug("open\n");
	return 0;
}

/*!
 * This function implements the release method on a PMIC device.
 *
 * @param        inode       pointer on the node
 * @param        file        pointer on the file
 * @return       This function returns 0.
 */

static int pmic_test_free(struct inode *inode, struct file *file)
{
	pr_debug("free\n");
	return 0;
}

/*!
 * This structure defines file operations for a PMIC device.
 */
static struct file_operations pmic_fops = {
	/*!
	 * the owner
	 */
	.owner = THIS_MODULE,
	/*!
	 * the ioctl operation
	 */
	.ioctl = pmic_test_ioctl,
	/*!
	 * the open operation
	 */
	.open = pmic_test_open,
	/*!
	 * the release operation
	 */
	.release = pmic_test_free,
};

static int __init pmic_test_init(void)
{
	struct class_device *pmic_device;

	pmic_major = register_chrdev(0, PMIC_NAME, &pmic_fops);
	if (pmic_major < 0) {
		printk("unable to get a major for pmic\n");
		return pmic_major;
	}

	pmic_class = class_create(THIS_MODULE, PMIC_NAME);
	if (IS_ERR(pmic_class)) {
		printk(KERN_ERR "Error creating pmic class.\n");
		goto err;
	}

	pmic_device =
	    class_device_create(pmic_class,NULL, MKDEV(pmic_major, 0),
				    NULL, PMIC_NAME);
	if (IS_ERR(pmic_device)) {
		printk(KERN_ERR "Error creating pmic class device.\n");
		goto err1;
	}


	//---devfs_mk_cdev(MKDEV(pmic_major, 0), S_IFCHR | S_IRUGO | S_IWUSR,
//		      PMIC_NAME);

	printk(KERN_INFO"pmic device: successfully loaded\n");
	return 0;
err1:
	class_destroy(pmic_class);
err:
	unregister_chrdev(pmic_major, PMIC_NAME);
	return -1;

}

static void __exit pmic_test_exit(void)
{
	//---devfs_remove(PMIC_NAME);

	class_device_destroy(pmic_class, MKDEV(pmic_major, 0));
	class_destroy(pmic_class);

	unregister_chrdev(pmic_major, PMIC_NAME);

	printk(KERN_INFO"pmic device: successfully unloaded\n");
}

/*
 * Module entry points
 */

subsys_initcall(pmic_test_init);
module_exit(pmic_test_exit);

MODULE_DESCRIPTION("PMIC Protocol test device driver");
MODULE_AUTHOR("FreeScale");
MODULE_LICENSE("GPL");
