/*
 * Copyright 2005-2006 Freescale Semiconductor, Inc. All Rights Reserved.
 */

/*
 * The code contained herein is licensed under the GNU General Public
 * License. You may obtain a copy of the GNU General Public License
 * Version 2 or later at the following locations:
 *
 * http://www.opensource.org/licenses/gpl-license.html
 * http://www.gnu.org/copyleft/gpl.html
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <asm/uaccess.h>
#include <linux/usb.h>
#include <linux/fs.h>

#include <linux/usb.h>

#include "sys.h"
#include "pima.h"
#include "stillimage.h"

#include "usbstillimage.h"

#define USBSIDEV_MINOR 200

#define USB_TRANSFER_TIMEOUT 10

#define KDATABUFFERLEN (32*1024)
#define KCTRLBUFFERLEN (256)

#define CHECK_DEVICE_VALID if(!gbDeviceConnected) return -EFAULT;

static int usbsidev_probe(struct usb_interface *intf,
			  const struct usb_device_id *id);

static void usbsidev_disconnect(struct usb_interface *intf);

static int usbsidev_ioctl(struct usb_interface *intf, unsigned int code,
			  void *buf);

static struct usb_device *usb_ptp_dev;

int pipeIn, pipeOut, pipeInterrupt;

struct urb *urbInterrupt = NULL;

stillimg_asyn_event_int_data *eventInterruptData;

static int gbDeviceConnected = 0;

static void *gKDataBuffer = NULL;

static void *gKCtrlBuffer = NULL;

static struct usb_device_id usbsidev_table[] = {
	{USB_INTERFACE_INFO(USB_CLASS_STILL_IMAGE, 1, 1)},
	{}
};

static struct usb_driver usb_stillimage_driver = {
	.name = "USB Still Image",
	.probe = usbsidev_probe,
	.disconnect = usbsidev_disconnect,
	.id_table = usbsidev_table,
	.ioctl = usbsidev_ioctl,
};

MODULE_DEVICE_TABLE(usb, usbsidev_table);

/*
 *	This function only print some debug messages.
 */
static void usbsidev_interrupt_callback(struct urb *urb, struct pt_regs *pt)
{

	printk(KERN_INFO "eventInterruptData->InterruptDataLength=%d\n",
	       eventInterruptData->InterruptDataLength);

	printk(KERN_INFO "eventInterruptData->ContainerType=0x%x\n",
	       eventInterruptData->ContainerType);

	printk(KERN_INFO "eventInterruptData->EventCode=0x%x\n",
	       eventInterruptData->EventCode);

	printk(KERN_INFO "eventInterruptData->TransactionID=0x%x\n",
	       eventInterruptData->TransactionID);

	printk(KERN_INFO "eventInterruptData->EventParameter1=0x%x\n",
	       eventInterruptData->EventParameter1);

	printk(KERN_INFO "eventInterruptData->EventParameter2=0x%x\n",
	       eventInterruptData->EventParameter2);

	printk(KERN_INFO "eventInterruptData->EventParameter3=0x%x\n",
	       eventInterruptData->EventParameter3);

	printk(KERN_INFO "usbsidev_interrupt_handle\n");

}

/*
 *	send zero-length control packet
 */
int usbsidev_ctrl(unsigned int deviceID, unsigned char bmRequestType,
		  unsigned char bRequest)
{

	return usb_control_msg(usb_ptp_dev, usb_sndctrlpipe(usb_ptp_dev, 0),
			       bRequest, bmRequestType, 0, 0, NULL, 0,
			       USB_TRANSFER_TIMEOUT * HZ);

}

/*
 *	recv control packets
 */
int usbsidev_ctrl_in(unsigned int deviceID, unsigned char bmRequestType,
		     unsigned char bRequest, unsigned short wLength, void *data)
{

	int ret;

	if (wLength > KCTRLBUFFERLEN) {

		printk(KERN_ERR "Limit wLength from %d tpo %d\n", wLength,
		       KCTRLBUFFERLEN);

		wLength = KCTRLBUFFERLEN;

	}

	ret =
	    usb_control_msg(usb_ptp_dev, usb_rcvctrlpipe(usb_ptp_dev, 0),
			    bRequest, bmRequestType, 0, 0, gKCtrlBuffer,
			    wLength, USB_TRANSFER_TIMEOUT * HZ);

	memcpy(data, gKCtrlBuffer, wLength);

	return ret;

}

/*
 *	send control packets
 */
int usbsidev_ctrl_out(unsigned int deviceID, unsigned char bmRequestType,
		      unsigned char bRequest,
		      unsigned short wLength, void *data)
{

	if (wLength > KCTRLBUFFERLEN) {

		printk(KERN_ERR "Limit wLength from %d tpo %d\n", wLength,
		       KCTRLBUFFERLEN);

		wLength = KCTRLBUFFERLEN;

	}

	memcpy(gKCtrlBuffer, data, wLength);

	return usb_control_msg(usb_ptp_dev, usb_sndctrlpipe(usb_ptp_dev, 0),
			       bRequest, bmRequestType, 0, 0, gKCtrlBuffer,
			       wLength, USB_TRANSFER_TIMEOUT * HZ);

}

/*
 *	recv bulk packets
 */
int usbsidev_in(unsigned char *buffer, unsigned int *bufferlen)
{

	int ret;

	unsigned int actualLength;

	if (*bufferlen > KDATABUFFERLEN)

		*bufferlen = KDATABUFFERLEN;

	actualLength = 0;

	ret =
	    usb_bulk_msg(usb_ptp_dev, pipeIn, gKDataBuffer, *bufferlen,
			 &actualLength, USB_TRANSFER_TIMEOUT * HZ);

	memcpy(buffer, gKDataBuffer, actualLength);

	*bufferlen = actualLength;

	return ret;

}

/*
 *	send bulk packets
 */
int usbsidev_out(unsigned char *buffer, unsigned int *bufferlen)
{

	int ret;

	unsigned int actualLength;

	if (*bufferlen > KDATABUFFERLEN)

		*bufferlen = KDATABUFFERLEN;

	memcpy(gKDataBuffer, buffer, *bufferlen);

	actualLength = 0;

	ret =
	    usb_bulk_msg(usb_ptp_dev, pipeOut, gKDataBuffer, *bufferlen,
			 &actualLength, USB_TRANSFER_TIMEOUT * HZ);

	*bufferlen = actualLength;

	return ret;

}

int usbsidev_clearstall(void)
{

	int ret;

	ret = usb_clear_halt(usb_ptp_dev, usb_sndctrlpipe(usb_ptp_dev, 0));
	printk(KERN_INFO "usb_clear_halt ctrl pipe result:%d\n", ret);

	ret = usb_clear_halt(usb_ptp_dev, usb_pipeendpoint(pipeIn));
	printk(KERN_INFO "usb_clear_halt pipeIn result:%d\n", ret);

	ret = usb_clear_halt(usb_ptp_dev, usb_pipeendpoint(pipeOut));
	printk(KERN_INFO "usb_clear_halt pipeOut result:%d\n", ret);

	return ret;

}

int usbsidev_probe(struct usb_interface *intf, const struct usb_device_id *id)
{

	int i;

	int address;

	struct usb_host_interface *desc;

	struct usb_endpoint_descriptor *endpoint;

	printk(KERN_INFO "Enter %s\n", __func__);

	desc = intf->cur_altsetting;

	usb_ptp_dev = usb_get_dev(interface_to_usbdev(intf));

	urbInterrupt = usb_alloc_urb(0, GFP_KERNEL);

	eventInterruptData =
	    kmalloc(sizeof(stillimg_asyn_event_int_data), GFP_KERNEL);

	memset(eventInterruptData, 0, sizeof(stillimg_asyn_event_int_data));

	for (i = 0; i < desc->desc.bNumEndpoints; i++) {

		endpoint = &(desc->endpoint[i].desc);

		address = endpoint->bEndpointAddress;

		printk(KERN_INFO
		       "endpoint[%d] address 0x%02x, bmAttributes=0x%x\n", i,
		       address, endpoint->bmAttributes);

		if ((endpoint->bmAttributes & USB_ENDPOINT_XFERTYPE_MASK) ==
		    USB_ENDPOINT_XFER_INT) {

			pipeInterrupt =
			    usb_rcvintpipe(usb_ptp_dev,
					   endpoint->bEndpointAddress);

			printk(KERN_INFO "pipeInterrupt 0x%x, address 0x%02x\n",
			       pipeInterrupt, endpoint->bEndpointAddress);

			usb_fill_int_urb(urbInterrupt, usb_ptp_dev,
					 pipeInterrupt, eventInterruptData,
					 sizeof
					 (stillimg_asyn_event_int_data),
					 usbsidev_interrupt_callback, NULL,
					 endpoint->bInterval);

			if (usb_submit_urb(urbInterrupt, GFP_KERNEL)) {

				printk("usb_submit_urb urbInterrupt failed\n");;

			}

		}

		if ((endpoint->bmAttributes & USB_ENDPOINT_XFERTYPE_MASK) ==
		    USB_ENDPOINT_XFER_BULK) {

			if (endpoint->bEndpointAddress & USB_DIR_IN) {

				pipeIn =
				    usb_rcvbulkpipe(usb_ptp_dev,
						    endpoint->bEndpointAddress);

				printk(KERN_INFO
				       "pipeIn 0x%x, address 0x%02x\n", pipeIn,
				       endpoint->bEndpointAddress);

			} else {

				pipeOut =
				    usb_sndbulkpipe(usb_ptp_dev,
						    endpoint->bEndpointAddress);

				printk(KERN_INFO
				       "pipeOut 0x%x, address 0x%02x\n",
				       pipeOut, endpoint->bEndpointAddress);

			}

		}

	}

	gKDataBuffer = kmalloc(KDATABUFFERLEN, GFP_KERNEL);

	gKCtrlBuffer = kmalloc(KCTRLBUFFERLEN, GFP_KERNEL);

	gbDeviceConnected = 1;

	printk(KERN_INFO "Exit %s\n", __func__);

	return 0;

}

static void usbsidev_disconnect(struct usb_interface *intf)
{

	gbDeviceConnected = 0;

	printk(KERN_INFO "Enter %s\n", __func__);

	usb_unlink_urb(urbInterrupt);

	usb_free_urb(urbInterrupt);

	kfree(gKDataBuffer);

	kfree(gKCtrlBuffer);

	kfree(eventInterruptData);

	usb_ptp_dev = NULL;

	pipeIn = -1;

	pipeOut = -1;

	printk(KERN_INFO "Exit %s\n", __func__);

}

static int usbsidev_ioctl(struct usb_interface *intf, unsigned int code,
			  void *buf)
{

	int result = 0;

	unsigned int cmd = code;

	unsigned long arg = (unsigned long)buf;

	if (cmd == USBSIDEV_GET_DEVICE_LIST) {

		usbsidev_para_get_dev_list para;

		if (!arg)

			return -EFAULT;

		memset(&para, 0, sizeof(para));

		if (gbDeviceConnected) {

			para.deviceID[0] = 1;

		}

		copy_to_user((void *)arg, &para, sizeof(para));

		return 0;

	}

	CHECK_DEVICE_VALID;

	//printk(KERN_INFO  "usbsidev_ioctl: cmd=0x%x, arg=0x%x\n", cmd, (unsigned int)arg);
	switch (cmd) {

	case USBSIDEV_GET_DEVICE_INFO:

		{

			usbsidev_para_get_dev_info para;

			pima_device_info *pDeviceInfo = NULL;

			int strlen = 0;

			if (copy_from_user(&para, (void *)arg, sizeof(para)) !=
			    0)

				return -EFAULT;

			pDeviceInfo = MALLOC(sizeof(pima_device_info));

			if (!pDeviceInfo)

				return -EFAULT;

			result =
			    pima_get_device_info(para.deviceID, pDeviceInfo);

			if (result != OK) {

				FREE(pDeviceInfo);

				return result;

			}

			memset(&para.DeviceInfo.Manufacturer, 0,
			       sizeof(para.DeviceInfo.Manufacturer));

			strlen =
			    *((unsigned char *)(pDeviceInfo->Manufacturer) -
			      1) * 2;

			if (strlen > sizeof(para.DeviceInfo.Manufacturer) - 2)

				strlen =
				    sizeof(para.DeviceInfo.Manufacturer) - 2;

			memcpy(&para.DeviceInfo.Manufacturer,
			       pDeviceInfo->Manufacturer, strlen);

			memset(&para.DeviceInfo.Model, 0,
			       sizeof(para.DeviceInfo.Model));

			strlen =
			    *((unsigned char *)(pDeviceInfo->Model) - 1) * 2;

			if (strlen > sizeof(para.DeviceInfo.Model) - 2)

				strlen = sizeof(para.DeviceInfo.Model) - 2;

			memcpy(&para.DeviceInfo.Model, pDeviceInfo->Model,
			       strlen);

			memset(&para.DeviceInfo.DeviceVersion, 0,
			       sizeof(para.DeviceInfo.DeviceVersion));

			strlen =
			    *((unsigned char *)(pDeviceInfo->DeviceVersion) -
			      1) * 2;

			if (strlen > sizeof(para.DeviceInfo.DeviceVersion) - 2)

				strlen =
				    sizeof(para.DeviceInfo.DeviceVersion) - 2;

			memcpy(&para.DeviceInfo.DeviceVersion,
			       pDeviceInfo->DeviceVersion, strlen);

			memset(&para.DeviceInfo.SerialNumber, 0,
			       sizeof(para.DeviceInfo.SerialNumber));

			strlen =
			    *((unsigned char *)(pDeviceInfo->SerialNumber) -
			      1) * 2;

			if (strlen > sizeof(para.DeviceInfo.SerialNumber) - 2)

				strlen =
				    sizeof(para.DeviceInfo.SerialNumber) - 2;

			memcpy(&para.DeviceInfo.SerialNumber,
			       pDeviceInfo->SerialNumber, strlen);

			memcpy(&para.DeviceInfo.OperationsSupported,
			       pDeviceInfo->OperationsSupported,
			       sizeof(para.DeviceInfo.OperationsSupported));

			if (pDeviceInfo)

				FREE(pDeviceInfo);

			copy_to_user((void *)arg, &para, sizeof(para));

			break;

		}

	case USBSIDEV_OPEN_SESSION:

		result = pima_open_session(arg);

		break;

	case USBSIDEV_CLOSE_SESSION:

		result = pima_close_session(arg);

		break;

	case USBSIDEV_GET_STORAGE_IDS:

		{

			usbsidev_para_get_stg_ids para;

			if (copy_from_user(&para, (void *)arg, sizeof(para)) !=
			    0)

				return -EFAULT;

			result =
			    pima_get_stgid(para.deviceID,
					   para.pStorageIDList,
					   &para.StorageIDCount);

			copy_to_user((void *)arg, &para, sizeof(para));

			break;

		}

	case USBSIDEV_GET_STORAGE_INFO:

		{

			usbsidev_para_get_stg_info para;

			pima_storage_info *pStorageInfo = NULL;

			int strlen = 0;

			if (copy_from_user(&para, (void *)arg, sizeof(para))
			    != 0)

				return -EFAULT;

			pStorageInfo = MALLOC(sizeof(pima_storage_info));

			if (!pStorageInfo)

				return -EFAULT;

			result =
			    pima_get_storage_info(para.deviceID,
						  para.storageID, pStorageInfo);

			if (result != OK) {

				FREE(pStorageInfo);

				return result;

			}

			para.StorageInfo.AccessCapability =
			    pStorageInfo->AccessCapability;

			para.StorageInfo.MaxCapacity =
			    pStorageInfo->MaxCapacity;

			para.StorageInfo.FreeSpaceInBytes =
			    pStorageInfo->FreeSpaceInBytes;

			memset(&para.StorageInfo.StorageDescription, 0,
			       sizeof(para.StorageInfo.StorageDescription));

			strlen =
			    *((unsigned char *)(pStorageInfo->
						StorageDescription) - 1) * 2;

			if (strlen >
			    sizeof(para.StorageInfo.StorageDescription) - 2)

				strlen =
				    sizeof(para.
					   StorageInfo.StorageDescription) - 2;

			memcpy(&para.StorageInfo.StorageDescription,
			       pStorageInfo->StorageDescription, strlen);

			memset(&para.StorageInfo.VolumeLabel, 0,
			       sizeof(para.StorageInfo.VolumeLabel));

			strlen =
			    *((unsigned char *)(pStorageInfo->VolumeLabel) -
			      1) * 2;

			if (strlen > sizeof(para.StorageInfo.VolumeLabel) - 2)

				strlen =
				    sizeof(para.StorageInfo.VolumeLabel) - 2;

			memcpy(&para.StorageInfo.VolumeLabel,
			       pStorageInfo->VolumeLabel, strlen);

			if (pStorageInfo)

				FREE(pStorageInfo);

			copy_to_user((void *)arg, &para, sizeof(para));

			break;

		}

	case USBSIDEV_GET_NUM_OBJECTS:

		result = pima_get_object_num(arg);

		break;

	case USBSIDEV_GET_OBJECT_HANDLES:

		{

			usbsidev_para_get_stg_handle para;

			if (copy_from_user(&para, (void *)arg, sizeof(para)) !=
			    0)

				return -EFAULT;

			result =
			    pima_get_object_handle(para.deviceID,
						   para.pFileIDList,
						   &para.fileCount);

			copy_to_user((void *)arg, &para, sizeof(para));

			break;

		}

	case USBSIDEV_GET_OBJECT_INFO:

		{

			usbsidev_para_get_obj_info para;

			pima_object_info *pObjectInfo = NULL;

			int strlen = 0;

			if (copy_from_user(&para, (void *)arg, sizeof(para))
			    != 0)

				return -EFAULT;

			pObjectInfo = MALLOC(sizeof(pima_object_info));

			if (!pObjectInfo)

				return -EFAULT;

			result =
			    pima_get_object_info(para.deviceID, para.fileID,
						 pObjectInfo);

			if (result != OK) {

				FREE(pObjectInfo);

				return result;

			}

			para.ObjectInfo.ObjectFormat =
			    pObjectInfo->ObjectFormat;

			para.ObjectInfo.ProtectionStatus =
			    pObjectInfo->ProtectionStatus;

			para.ObjectInfo.ObjectCompressedSize =
			    pObjectInfo->ObjectCompressedSize;

			para.ObjectInfo.ThumbCompressedSize =
			    pObjectInfo->ThumbCompressedSize;

			para.ObjectInfo.ThumbPixWidth =
			    pObjectInfo->ThumbPixWidth;

			para.ObjectInfo.ThumbPixHeight =
			    pObjectInfo->ThumbPixHeight;

			para.ObjectInfo.ImagePixWidth =
			    pObjectInfo->ImagePixWidth;

			para.ObjectInfo.ImagePixHeight =
			    pObjectInfo->ImagePixHeight;

			para.ObjectInfo.ImageBitDepth =
			    pObjectInfo->ImageBitDepth;

			para.ObjectInfo.AssociationType =
			    pObjectInfo->AssociationType;

			memset(&para.ObjectInfo.Filename, 0,
			       sizeof(para.ObjectInfo.Filename));

			strlen =
			    *((unsigned char *)(pObjectInfo->Filename) - 1) * 2;

			if (strlen > sizeof(para.ObjectInfo.Filename) - 2)

				strlen = sizeof(para.ObjectInfo.Filename) - 2;

			memcpy(&para.ObjectInfo.Filename,
			       pObjectInfo->Filename, strlen);

			memset(&para.ObjectInfo.CaptureDate, 0,
			       sizeof(para.ObjectInfo.CaptureDate));

			strlen =
			    *((unsigned char *)(pObjectInfo->CaptureDate) -
			      1) * 2;

			if (strlen > sizeof(para.ObjectInfo.CaptureDate) - 2)

				strlen =
				    sizeof(para.ObjectInfo.CaptureDate) - 2;

			memcpy(&para.ObjectInfo.CaptureDate,
			       pObjectInfo->CaptureDate, strlen);

			memset(&para.ObjectInfo.ModificationDate, 0,
			       sizeof(para.ObjectInfo.ModificationDate));

			strlen =
			    *((unsigned char *)(pObjectInfo->
						ModificationDate) - 1) * 2;

			if (strlen >
			    sizeof(para.ObjectInfo.ModificationDate) - 2)

				strlen =
				    sizeof(para.ObjectInfo.ModificationDate) -
				    2;

			memcpy(&para.ObjectInfo.ModificationDate,
			       pObjectInfo->ModificationDate, strlen);

			if (pObjectInfo)

				FREE(pObjectInfo);

			copy_to_user((void *)arg, &para, sizeof(para));

			break;

		}

	case USBSIDEV_GET_THUMB:

		{

			usbsidev_para_get_thumb para;

			if (copy_from_user(&para, (void *)arg, sizeof(para)) !=
			    0)

				return -EFAULT;

			result =
			    pima_get_thumb(para.deviceID, para.fileID,
					   para.pThumbBuffer,
					   &para.bufferLength);

			copy_to_user((void *)arg, &para, sizeof(para));

			break;

		}

	case USBSIDEV_START_GET_OBJECT:

		{

			usbsidev_para_start_get_obj para;

			if (copy_from_user(&para, (void *)arg, sizeof(para)) !=
			    0)

				return -EFAULT;

			result =
			    pima_start_get_object(para.deviceID, para.fileID);

			break;

		}

	case USBSIDEV_GET_OBJECT:

		{

			usbsidev_para_get_obj para;

			if (copy_from_user(&para, (void *)arg, sizeof(para)) !=
			    0)

				return -EFAULT;

			result =
			    pima_get_object(para.deviceID, para.pBuffer,
					    &para.bufferLength);

			copy_to_user((void *)arg, &para, sizeof(para));

			break;

		}

	case USBSIDEV_END_GET_OBJECT:

		result = pima_end_get_object(arg);

		break;

	case USBSIDEV_START_SEND_OBJECT:

		{

			usbsidev_para_start_send_obj para;

			if (copy_from_user(&para, (void *)arg, sizeof(para)) !=
			    0)

				return -EFAULT;

			result =
			    pima_send_object_info(para.deviceID, para.fileName,
						  para.fileSize, para.dirID,
						  para.bForceToImageFile);

			break;

		}

	case USBSIDEV_SEND_OBJECT:

		{

			usbsidev_para_send_obj para;

			if (copy_from_user(&para, (void *)arg, sizeof(para)) !=
			    0)

				return -EFAULT;

			result =
			    pima_send_object(para.deviceID, para.pBuffer,
					     &para.nBytesToWrite);

			copy_to_user((void *)arg, &para, sizeof(para));

			break;

		}

	case USBSIDEV_END_SEND_OBJECT:

		result = pima_end_send_object(arg);

		break;

	case USBSIDEV_SET_OBJECT_PROTECTION:

		{

			usbsidev_para_set_obj_protect para;

			if (copy_from_user(&para, (void *)arg, sizeof(para)) !=
			    0)

				return -EFAULT;

			result =
			    pima_set_object_protection(para.deviceID,
						       para.fileID,
						       para.bProtection);

			break;

		}

	case USBSIDEV_DELETE_OBJECT:

		{

			usbsidev_para_del_obj para;

			if (copy_from_user(&para, (void *)arg, sizeof(para)) !=
			    0)

				return -EFAULT;

			result = pima_del_obj(para.deviceID, para.fileID);

			break;

		}

	case USBSIDEV_FORMAT_STORE:

		{

			usbsidev_para_format_store para;

			if (copy_from_user(&para, (void *)arg, sizeof(para)) !=
			    0)

				return -EFAULT;

			result =
			    pima_format_store(para.deviceID, para.storageID);

			break;

		}

	case USBSIDEV_RESET_DEVICE:

		result = pima_reset_dev(arg);

		break;

	case USBSIDEV_SELF_TEST:

		result = pima_self_test(arg);

		break;

	case USBSIDEV_POWER_DOWN:

		result = pima_power_down(arg);

		break;

	}

	return result;

}

static int ptpusb_major;

#define DEV_NAME0 "ptp_usb"

static int ptp_open(struct inode *inode, struct file *filp)
{

	return 0;

}

static int ptp_ioctl(struct inode *inode, struct file *filp, u_int cmd,
		     u_long arg)
{

	return usbsidev_ioctl(NULL, cmd, (void *)arg);

}

static int ptp_release(struct inode *inode, struct file *filp)
{

	return 0;

}

static ssize_t ptp_read(struct file *file, char __user * buf, size_t count,
			loff_t * ppos)
{

	return count;

}

static ssize_t ptp_write(struct file *file, const char __user * buf,
			 size_t count, loff_t * ppos)
{

	return count;

}

struct file_operations ptpusb_fops = {
	.open = ptp_open,
	.ioctl = ptp_ioctl,
	.release = ptp_release,
	.read = ptp_read,
	.write = ptp_write,
};

static struct class *ptp_class;

static int __init usbsidev_init(void)
{

	int ret;
	struct class_device *cdev;

	ret = usb_register(&usb_stillimage_driver);

	if (ret)
		goto cleanup_on_fail;

	ptpusb_major = register_chrdev(0, DEV_NAME0, &ptpusb_fops);
	ptp_class = class_create(THIS_MODULE, DEV_NAME0);
	cdev =
	    class_device_create(ptp_class, NULL, MKDEV(ptpusb_major, 0), NULL,
				DEV_NAME0);

	sysfs_create_link(&ptp_class->subsys.kobj, &cdev->kobj, DEV_NAME0);
	printk(KERN_INFO "USB Still Image driver module_init\n");
	return 0;
      cleanup_on_fail:
	return -1;
}

static void __exit usbsidev_cleanup(void)
{

	usb_deregister(&usb_stillimage_driver);

	sysfs_remove_link(&ptp_class->subsys.kobj, DEV_NAME0);
	class_device_destroy(ptp_class, MKDEV(ptpusb_major, 0));
	class_destroy(ptp_class);

	unregister_chrdev(ptpusb_major, DEV_NAME0);

	printk(KERN_INFO "USB Still Image driver module_exit\n");

}

module_init(usbsidev_init);

module_exit(usbsidev_cleanup);

MODULE_AUTHOR("FSL");

MODULE_DESCRIPTION("USB Still Image driver");

MODULE_LICENSE("GPL");
