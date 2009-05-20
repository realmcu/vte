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

#ifndef __PIMA_H
#define __PIMA_H

typedef enum {
	PIMA_OP_UNDEFINED = 0x1000,
	PIMA_OP_GET_DEVICE_INFO = 0x1001,
	PIMA_OP_OPEN_SESSION = 0x1002,
	PIMA_OP_CLOSE_SESSION = 0x1003,
	PIMA_OP_GET_STORAGE_IDS = 0x1004,
	PIMA_OP_GET_STORAGE_INFO = 0x1005,
	PIMA_OP_GET_NUM_OBJECTS = 0x1006,
	PIMA_OP_GET_OBJECT_HANDLES = 0x1007,
	PIMA_OP_GET_OBJECT_INFO = 0x1008,
	PIMA_OP_GET_OBJECT = 0x1009,
	PIMA_OP_GET_THUMB = 0x100A,
	PIMA_OP_DELETE_OBJECT = 0x100B,
	PIMA_OP_SEND_OBJECT_INFO = 0x100C,
	PIMA_OP_SEND_OBJECT = 0x100D,
	PIMA_OP_INITIATE_CAPTURE = 0x100E,
	PIMA_OP_FORMAT_STORE = 0x100F,
	PIMA_OP_RESET_DEVICE = 0x1010,
	PIMA_OP_SELF_TEST = 0x1011,
	PIMA_OP_SET_OBJECT_PROTECTION = 0x1012,
	PIMA_OP_POWER_DOWN = 0x1013,
	PIMA_OP_GET_DEVICE_PROP_DESC = 0x1014,
	PIMA_OP_GET_DEVICE_PROP_VALUE = 0x1015,
	PIMA_OP_SET_DEVICE_PROP_VALUE = 0x1016,
	PIMA_OP_RESET_DEVICE_PROP_VALUE = 0x1017,
	PIMA_OP_TERMINATE_OPEN_CAPTURE = 0x1018,
	PIMA_OP_MOVE_OBJECT = 0x1019,
	PIMA_OP_COPY_OBJECT = 0x101A,
	PIMA_OP_GET_PARTIAL_OBJ = 0x101B,
	PIMA_OP_INITIATE_OPEN_CAPTURE = 0x101C
} PIMA_OPERATION_CODE;

typedef enum {
	PIMA_RESP_UNDEFINED = 0x2000,
	PIMA_RESP_OK = 0x2001,
	PIMA_RESP_GENERAL_ERROR = 0x2002,
	PIMA_RESP_SESSION_NOT_OPEN = 0x2003,
	PIMA_RESP_INVALID_TRANSACTION_ID = 0x2004,
	PIMA_RESP_OPERATION_NOT_SUPPORT = 0x2005,
	PIMA_RESP_PARAM_NOT_SUPPORT = 0x2006,
	PIMA_RESP_INCOMPLETE_TRANSFER = 0x2007,
	PIMA_RESP_INVALID_STORAGE_ID = 0x2008,
	PIMA_RESP_INVALID_OBJECT_HANDLE = 0x2009,
	PIMA_RESP_DEVICE_PROP_NOT_SUPPORT = 0x200A,
	PIMA_RESP_INVALID_OBJECT_FORMAT = 0x200B,
	PIMA_RESP_STORE_FULL = 0x200C,
	PIMA_RESP_OBJECT_WRITE_PROTECTED = 0x200D,
	PIMA_RESP_STORE_READ_ONLY = 0x200E,
	PIMA_RESP_ACCESS_DENIED = 0x200F,
	PIMA_RESP_NO_THUMBNAIL = 0x2010,
	PIMA_RESP_SELF_TEST_FAILED = 0x2011,
	PIMA_RESP_PARTIAL_DELETION = 0x2012,
	PIMA_RESP_STORE_NOT_AVAILABLE = 0x2013,
	PIMA_RESP_SPEC_FORMAT_UNSUPPORT = 0x2014,
	PIMA_RESP_NO_VALID_OBJECT_INFO = 0x2015,
	PIMA_RESP_INVALID_CODE_FORMAT = 0x2016,
	PIMA_RESP_UNKNOWN_VENDOR_CODE = 0x2017,
	PIMA_RESP_CAPTURE_ALREADY_TERMINATED = 0x2018,
	PIMA_RESP_DEVICE_BUSY = 0x2019,
	PIMA_RESP_INVALID_PARENT_OBJECT = 0x201A,
	PIMA_RESP_INVALID_DEVICE_PROP_FORMAT = 0x201B,
	PIMA_RESP_INVALID_DEVICE_PROP_VALUE = 0x201C,
	PIMA_RESP_INVALID_PARAMETER = 0x201D,
	PIMA_RESP_SESSION_ALREADY_OPENED = 0x201E,
	PIMA_RESP_TRANSACTION_CANCELLED = 0x201F,
	PIMA_RESP_SPEC_DEST_UNSUPPORT = 0x2020
} PIMA_RESPONSE_CODE;

typedef struct {

	unsigned short StandardVersion;

	unsigned int VendorExtensionID;

	unsigned short VendorExtensionVersion;

	unsigned char *VendorExtensionDesc;

	unsigned short FunctionalMode;

	unsigned short OperationsSupported[32];

	unsigned char *EventsSupported;

	unsigned char *DeviceProperties;

	unsigned char *CaptureFormats;

	unsigned char *ImageFormats;

	unsigned char *Manufacturer;

	unsigned char *Model;

	unsigned char *DeviceVersion;

	unsigned char *SerialNumber;

} pima_device_info;

typedef struct {

	unsigned int StorageID __attribute__ ((packed));

	unsigned short ObjectFormat __attribute__ ((packed));

	unsigned short ProtectionStatus __attribute__ ((packed));

	unsigned int ObjectCompressedSize __attribute__ ((packed));

	unsigned short ThumbFormat __attribute__ ((packed));

	unsigned int ThumbCompressedSize __attribute__ ((packed));

	unsigned int ThumbPixWidth __attribute__ ((packed));

	unsigned int ThumbPixHeight __attribute__ ((packed));

	unsigned int ImagePixWidth __attribute__ ((packed));

	unsigned int ImagePixHeight __attribute__ ((packed));

	unsigned int ImageBitDepth __attribute__ ((packed));

	unsigned int ParentObject __attribute__ ((packed));

	unsigned short AssociationType __attribute__ ((packed));

	unsigned int AssociationDesc __attribute__ ((packed));

	unsigned int SequenceNumber __attribute__ ((packed));

	unsigned char *Filename __attribute__ ((packed));

	unsigned char *CaptureDate __attribute__ ((packed));

	unsigned char *ModificationDate __attribute__ ((packed));

	unsigned char *Keywords __attribute__ ((packed));

} pima_object_info;

typedef struct {

	unsigned short StorageType;

	unsigned short FilesystemType;

	unsigned short AccessCapability;

	unsigned int MaxCapacity;

	unsigned int FreeSpaceInBytes;

	unsigned int FreeSpaceInImages;

	unsigned char *StorageDescription;

	unsigned char *VolumeLabel;

} pima_storage_info;

typedef struct {

	unsigned short OperationCode __attribute__ ((packed));

	unsigned int TransactionID __attribute__ ((packed));

	unsigned int Parameter1 __attribute__ ((packed));

	unsigned int Parameter2 __attribute__ ((packed));

	unsigned int Parameter3 __attribute__ ((packed));

} pima_request_block;

typedef struct {

	unsigned short OperationCode __attribute__ ((packed));

	unsigned int TransactionID __attribute__ ((packed));

	unsigned char *dataBuffer __attribute__ ((packed));

	unsigned int dataDirection __attribute__ ((packed));

} pima_data_block;

typedef struct {

	unsigned short ResponseCode __attribute__ ((packed));

	unsigned int TransactionID __attribute__ ((packed));

	unsigned int Parameter1 __attribute__ ((packed));

	unsigned int Parameter2 __attribute__ ((packed));

	unsigned int Parameter3 __attribute__ ((packed));

} pima_response_block;

unsigned int pima_get_device_info(unsigned int deviceID,
				  pima_device_info * pDeviceInfo);

unsigned int pima_open_session(unsigned int deviceID);

unsigned int pima_close_session(unsigned int deviceID);

unsigned int pima_get_object_num(unsigned int deviceID);

unsigned int pima_get_stgid(unsigned int deviceID,
			    unsigned int *pStorageIDList,
			    unsigned int *pStorageIDCount);

unsigned int pima_get_storage_info(unsigned int deviceID,
				   unsigned int storageID,
				   pima_storage_info * pStorageInfo);

unsigned int pima_get_object_handle(unsigned int deviceID,
				    unsigned int *pFileIDList,
				    unsigned int *pFileCount);

unsigned int pima_get_object_info(unsigned int deviceID, unsigned int fileID,
				  pima_object_info * pima_objinfo);

unsigned int pima_get_thumb(unsigned int deviceID, unsigned int fileID,
			    unsigned char *pThumbBuffer,
			    unsigned int *bufferLength);

unsigned int pima_start_get_object(unsigned int deviceID, unsigned int fileID);

unsigned int pima_get_object(unsigned int deviceID, unsigned char *pBuffer,
			     unsigned int *bufferLength);

unsigned int pima_end_get_object(unsigned int deviceID);

unsigned int pima_get_whole_object(unsigned int deviceID, unsigned int fileID,
				   unsigned char *pBuffer,
				   unsigned int *bufferLength);

unsigned int pima_get_partial_object(unsigned int deviceID,
				     unsigned int fileID,
				     unsigned char *pBuffer,
				     unsigned int offset,
				     unsigned int *pBytesRead);

unsigned int pima_send_object_info(unsigned int deviceID,
				   unsigned char *pFileName,
				   unsigned int fileSize,
				   unsigned int dirID,
				   unsigned int bForceToImageFile);

unsigned int pima_send_object(unsigned int deviceID, unsigned char *pBuffer,
			      unsigned int *pBytesToWrite);

unsigned int pima_end_send_object(unsigned int deviceID);

unsigned int pima_send_whole_object(unsigned int deviceID,
				    unsigned char *pBuffer,
				    unsigned int *pBytesToWrite);

unsigned int pima_set_object_protection(unsigned int deviceID,
					unsigned int fileID,
					unsigned int bProtection);

unsigned int pima_del_obj(unsigned int deviceID, unsigned int fileID);

unsigned int pima_format_store(unsigned int deviceID, unsigned int storageID);

unsigned int pima_reset_dev(unsigned int deviceID);

unsigned int pima_self_test(unsigned int deviceID);

unsigned int pima_power_down(unsigned int deviceID);

unsigned int pima_cancel_trans(unsigned int deviceID,
			       unsigned int TransactionID);

unsigned int pima_handle_stall(unsigned int deviceID);

#endif				/*
				 */
