
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

#ifndef __USBSTILLIMAGE_H
#define __USBSTILLIMAGE_H

#include <linux/ioctl.h>
#define PTP_IOC_MAGIC  'M'

    /* We use PTP_IOC_MAGIC as magic number */

#define USBSIDEV_GET_DEVICE_LIST    _IOR(PTP_IOC_MAGIC, 0, char *)
#define USBSIDEV_GET_DEVICE_INFO   _IOR(PTP_IOC_MAGIC, 1, char *)
#define USBSIDEV_OPEN_SESSION 		_IOW(PTP_IOC_MAGIC, 2, unsigned int )

#define USBSIDEV_CLOSE_SESSION 	_IOW(PTP_IOC_MAGIC, 3, unsigned int )
#define USBSIDEV_GET_NUM_OBJECTS  _IOR(PTP_IOC_MAGIC, 4, unsigned int )

#define USBSIDEV_GET_STORAGE_IDS  		_IOR(PTP_IOC_MAGIC, 5, char* )
#define USBSIDEV_GET_STORAGE_INFO  	_IOR(PTP_IOC_MAGIC, 6, char* )
#define USBSIDEV_GET_OBJECT_HANDLES   _IOR(PTP_IOC_MAGIC, 7, char* )
#define USBSIDEV_GET_OBJECT_INFO  	       _IOR(PTP_IOC_MAGIC, 8, char*)
#define USBSIDEV_GET_THUMB		  	_IOR(PTP_IOC_MAGIC, 9, char*)
#define USBSIDEV_START_GET_OBJECT  	_IOR(PTP_IOC_MAGIC, 10, char* )
#define USBSIDEV_GET_OBJECT		  	_IOR(PTP_IOC_MAGIC, 11, char*)
#define USBSIDEV_END_GET_OBJECT	  	_IOR(PTP_IOC_MAGIC, 12, unsigned int )

#define USBSIDEV_GET_WHOLE_OBJECT	_IOR(PTP_IOC_MAGIC, 13, char*)
#define USBSIDEV_START_SEND_OBJECT	_IOR(PTP_IOC_MAGIC, 14, char*)
#define USBSIDEV_SEND_OBJECT		  	_IOR(PTP_IOC_MAGIC, 15, char*)
#define USBSIDEV_END_SEND_OBJECT		 _IOR(PTP_IOC_MAGIC, 16, unsigned int)

#define USBSIDEV_SET_OBJECT_PROTECTION  	_IOW(PTP_IOC_MAGIC, 17, char*)
#define USBSIDEV_DELETE_OBJECT		  		_IOW(PTP_IOC_MAGIC, 18, char*)
#define USBSIDEV_FORMAT_STORE		  		_IOW(PTP_IOC_MAGIC, 19, char*)
#define USBSIDEV_RESET_DEVICE		  		_IOR(PTP_IOC_MAGIC, 20,  unsigned int)

#define USBSIDEV_SELF_TEST				 _IOR(PTP_IOC_MAGIC, 21, unsigned int)
#define USBSIDEV_POWER_DOWN			 _IOR(PTP_IOC_MAGIC, 22, unsigned int)
#define USBSIDEV_CANCEL_TRANSACTION	_IOW(PTP_IOC_MAGIC, 23, unsigned int)

typedef struct {

	unsigned char Manufacturer[64];

	unsigned char Model[64];

	unsigned char DeviceVersion[64];

	unsigned char SerialNumber[64];

	unsigned short OperationsSupported[32];

} usbsidev_dev_info;

typedef struct {

	unsigned short ObjectFormat;

	unsigned short ProtectionStatus;

	unsigned int ObjectCompressedSize;

	unsigned int ThumbCompressedSize;

	unsigned int ThumbPixWidth;

	unsigned int ThumbPixHeight;

	unsigned int ImagePixWidth;

	unsigned int ImagePixHeight;

	unsigned int ImageBitDepth;

	unsigned short AssociationType;

	unsigned char Filename[256];

	unsigned char CaptureDate[64];

	unsigned char ModificationDate[64];

} usbsidev_obj_info;

typedef struct {

	unsigned short AccessCapability;

	unsigned int MaxCapacity;

	unsigned int FreeSpaceInBytes;

	unsigned char StorageDescription[64];

	unsigned char VolumeLabel[64];

} usbsidev_stg_info;

typedef struct {

	unsigned int deviceID[8];

} usbsidev_para_get_dev_list;

typedef struct {

	unsigned int deviceID;

	usbsidev_dev_info DeviceInfo;

} usbsidev_para_get_dev_info;

typedef struct {

	unsigned int deviceID;

	unsigned int *pStorageIDList;

	unsigned int StorageIDCount;

} usbsidev_para_get_stg_ids;

typedef struct {

	unsigned int deviceID;

	unsigned int storageID;

	usbsidev_stg_info StorageInfo;

} usbsidev_para_get_stg_info;

typedef struct {

	unsigned int deviceID;

	unsigned int *pFileIDList;

	unsigned int fileCount;

} usbsidev_para_get_stg_handle;

typedef struct {

	unsigned int deviceID;

	unsigned int fileID;

	usbsidev_obj_info ObjectInfo;

} usbsidev_para_get_obj_info;

typedef struct {

	unsigned int deviceID;

	unsigned int fileID;

	unsigned char *pThumbBuffer;

	unsigned int bufferLength;

} usbsidev_para_get_thumb;

typedef struct {

	unsigned int deviceID;

	unsigned int fileID;

} usbsidev_para_start_get_obj;

typedef struct {

	unsigned int deviceID;

	unsigned char *pBuffer;

	unsigned int bufferLength;

} usbsidev_para_get_obj;

typedef struct {

	unsigned int deviceID;

	unsigned char fileName[256];

	unsigned int fileSize;

	unsigned int dirID;

	unsigned int bForceToImageFile;

} usbsidev_para_start_send_obj;

typedef struct {

	unsigned int deviceID;

	unsigned char *pBuffer;

	unsigned int nBytesToWrite;

} usbsidev_para_send_obj;

typedef struct {

	unsigned int deviceID;

	unsigned int fileID;

	unsigned int bProtection;

} usbsidev_para_set_obj_protect;

typedef struct {

	unsigned int deviceID;

	unsigned int fileID;

} usbsidev_para_del_obj;

typedef struct {

	unsigned int deviceID;

	unsigned int storageID;

} usbsidev_para_format_store;

#endif				/* 
				 */
