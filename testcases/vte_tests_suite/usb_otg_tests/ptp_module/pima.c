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

#include "sys.h"
#include "stillimage.h"
#include "pima.h"
static unsigned char gPimaDataBuffer[32 * 1024];
static unsigned int gNextTransactionID = 0;

unsigned int pima_get_device_info(unsigned int deviceID,
				  pima_device_info * pDeviceInfo)
{

	unsigned int result;

	pima_request_block requestblock;

	pima_data_block datablock;

	pima_response_block responseblock;

	unsigned char *pData;

	unsigned int actualLength;

	unsigned short OperationsSupported;

	int i, j;

	unsigned char *p;

	unsigned int nOperationsSupported;

	requestblock.OperationCode = cpu_to_le16(PIMA_OP_GET_DEVICE_INFO);

	requestblock.TransactionID = cpu_to_le32(gNextTransactionID++);

	requestblock.Parameter1 = 0;

	requestblock.Parameter2 = 0;

	requestblock.Parameter3 = 0;

	datablock.OperationCode = requestblock.OperationCode;

	datablock.TransactionID = requestblock.TransactionID;

	datablock.dataBuffer = gPimaDataBuffer;

	datablock.dataDirection = STILLIMAGE_DATA_IN;

	actualLength = sizeof(gPimaDataBuffer);

	result =
	    stillimg_req_data_response(deviceID, &requestblock,
				       sizeof(requestblock),
				       &gPimaDataBuffer, &actualLength,
				       STILLIMAGE_DATA_IN, &responseblock,
				       sizeof(responseblock));

	if (result != OK)

		return result;

	if (responseblock.ResponseCode != cpu_to_le16(PIMA_RESP_OK))

		return cpu_to_le16(responseblock.ResponseCode);

	pData = gPimaDataBuffer + 12;

	pDeviceInfo->StandardVersion = extr_le16(&pData);

	pDeviceInfo->VendorExtensionID = extr_le32(&pData);

	pDeviceInfo->VendorExtensionVersion = extr_le16(&pData);

	pDeviceInfo->VendorExtensionDesc = extr_le_str(&pData);

	pDeviceInfo->FunctionalMode = extr_le16(&pData);

	p = pData;

	nOperationsSupported = extr_le32(&p);

	extr_le_ary16(&pData);

	memset(pDeviceInfo->OperationsSupported, 0,
	       sizeof(pDeviceInfo->OperationsSupported));

	for (i = 0, j = 0;
	     i < nOperationsSupported
	     && j <
	     sizeof(pDeviceInfo->OperationsSupported) /
	     sizeof(pDeviceInfo->OperationsSupported[0]); i++) {

		OperationsSupported = extr_le16(&p);

		if (OperationsSupported >= PIMA_OP_UNDEFINED
		    && OperationsSupported <= PIMA_OP_INITIATE_OPEN_CAPTURE) {

			pDeviceInfo->OperationsSupported[j] =
			    OperationsSupported;

			j++;

		}

	}

	pDeviceInfo->EventsSupported = extr_le_ary16(&pData);

	pDeviceInfo->DeviceProperties = extr_le_ary16(&pData);

	pDeviceInfo->CaptureFormats = extr_le_ary16(&pData);

	pDeviceInfo->ImageFormats = extr_le_ary16(&pData);

	pDeviceInfo->Manufacturer = extr_le_str(&pData);

	pDeviceInfo->Model = extr_le_str(&pData);

	pDeviceInfo->DeviceVersion = extr_le_str(&pData);

	pDeviceInfo->SerialNumber = extr_le_str(&pData);

	return OK;

}

unsigned int pima_open_session(unsigned int deviceID)
{

	unsigned int result;

	pima_request_block requestblock;

	pima_response_block responseblock;

	requestblock.OperationCode = cpu_to_le16(PIMA_OP_OPEN_SESSION);

	gNextTransactionID = 0;

	requestblock.TransactionID = cpu_to_le32(gNextTransactionID++);

	requestblock.Parameter1 = 1;

	requestblock.Parameter2 = 0;

	requestblock.Parameter3 = 0;

	result =
	    stillimg_req_response(deviceID, &requestblock,
				  sizeof(requestblock), &responseblock,
				  sizeof(responseblock));

	if (result != OK)

		return result;

	if (responseblock.ResponseCode != cpu_to_le16(PIMA_RESP_OK))

		return cpu_to_le16(responseblock.ResponseCode);

	return OK;

}

unsigned int pima_close_session(unsigned int deviceID)
{

	unsigned int result;

	pima_request_block requestblock;

	pima_response_block responseblock;

	requestblock.OperationCode = cpu_to_le16(PIMA_OP_CLOSE_SESSION);

	requestblock.TransactionID = cpu_to_le32(gNextTransactionID++);

	requestblock.Parameter1 = 0;

	requestblock.Parameter2 = 0;

	requestblock.Parameter3 = 0;

	result =
	    stillimg_req_response(deviceID, &requestblock,
				  sizeof(requestblock), &responseblock,
				  sizeof(responseblock));

	if (result != OK)

		return result;

	if (responseblock.ResponseCode != cpu_to_le16(PIMA_RESP_OK))

		return cpu_to_le16(responseblock.ResponseCode);

	return OK;

}

unsigned int pima_get_object_num(unsigned int deviceID)
{

	unsigned int result;

	pima_request_block requestblock;

	pima_response_block responseblock;

	requestblock.OperationCode = cpu_to_le16(PIMA_OP_GET_NUM_OBJECTS);

	requestblock.TransactionID = cpu_to_le32(gNextTransactionID++);

	requestblock.Parameter1 = cpu_to_le32(0xFFFFFFFF);

	requestblock.Parameter2 = cpu_to_le32(0);

	requestblock.Parameter3 = cpu_to_le32(0);

	result =
	    stillimg_req_response(deviceID, &requestblock,
				  sizeof(requestblock), &responseblock,
				  sizeof(responseblock));

	if (result != OK)

		return -1;

	printk(KERN_INFO "responseblock.ResponseCode=0x%x\n",
	       responseblock.ResponseCode);

	if (responseblock.ResponseCode != cpu_to_le16(PIMA_RESP_OK)) {

		requestblock.OperationCode =
		    cpu_to_le16(PIMA_OP_GET_NUM_OBJECTS);

		requestblock.TransactionID = cpu_to_le32(gNextTransactionID++);

		requestblock.Parameter1 = cpu_to_le32(0xFFFFFFFF);

		requestblock.Parameter2 = cpu_to_le32(0xFFFFFFFF);

		requestblock.Parameter3 = cpu_to_le32(0xFFFFFFFF);

		result =
		    stillimg_req_response(deviceID, &requestblock,
					  sizeof(requestblock),
					  &responseblock,
					  sizeof(responseblock));

		if (result != OK)

			return -1;

		printk(KERN_INFO "responseblock.ResponseCode=0x%x\n",
		       responseblock.ResponseCode);

		if (responseblock.ResponseCode != cpu_to_le16(PIMA_RESP_OK)) {

			return -1;

		}

	}

	return cpu_to_le32(responseblock.Parameter1);

}

/* get storage IDs*/
unsigned int pima_get_stgid(unsigned int deviceID, unsigned int *pStorageIDList,
			    unsigned int *pStorageIDCount)
{

	unsigned int result;

	pima_request_block requestblock;

	pima_data_block datablock;

	pima_response_block responseblock;

	unsigned char *pData;

	unsigned int actualLength;

	unsigned int StorageIDCount;

	int i;

	unsigned int stroageID;

	requestblock.OperationCode = cpu_to_le16(PIMA_OP_GET_STORAGE_IDS);

	requestblock.TransactionID = cpu_to_le32(gNextTransactionID++);

	requestblock.Parameter1 = 0;

	requestblock.Parameter2 = 0;

	requestblock.Parameter3 = 0;

	datablock.OperationCode = requestblock.OperationCode;

	datablock.TransactionID = requestblock.TransactionID;

	datablock.dataBuffer = gPimaDataBuffer;

	datablock.dataDirection = STILLIMAGE_DATA_IN;

	actualLength = sizeof(gPimaDataBuffer);

	result =
	    stillimg_req_data_response(deviceID, &requestblock,
				       sizeof(requestblock),
				       &gPimaDataBuffer, &actualLength,
				       STILLIMAGE_DATA_IN, &responseblock,
				       sizeof(responseblock));

	printk(KERN_INFO "stillimg_req_response result=0x%x\n", result);

	if (result != OK)

		return result;

	printk(KERN_INFO "responseblock.ResponseCode=0x%x\n",
	       responseblock.ResponseCode);

	if (responseblock.ResponseCode != cpu_to_le16(PIMA_RESP_OK))

		return cpu_to_le16(responseblock.ResponseCode);

	pData = gPimaDataBuffer + 12;

	StorageIDCount = extr_le16(&pData);

	pData = gPimaDataBuffer + 12;

	pData = extr_le_ary16(&pData);

	if (StorageIDCount > *pStorageIDCount)

		StorageIDCount = *pStorageIDCount;

	for (i = 0; i < StorageIDCount; i++) {

		stroageID = extr_le32(&pData);

		copy_to_user(&pStorageIDList[i], &stroageID, sizeof(stroageID));

	}

	*pStorageIDCount = StorageIDCount;

	return OK;

}

unsigned int pima_get_storage_info(unsigned int deviceID,
				   unsigned int storageID,
				   pima_storage_info * pStorageInfo)
{

	unsigned int result;

	pima_request_block requestblock;

	pima_data_block datablock;

	pima_response_block responseblock;

	unsigned char *pData;

	unsigned int actualLength;

	requestblock.OperationCode = cpu_to_le16(PIMA_OP_GET_STORAGE_INFO);

	requestblock.TransactionID = cpu_to_le32(gNextTransactionID++);

	requestblock.Parameter1 = cpu_to_le32(storageID);

	requestblock.Parameter2 = 0;

	requestblock.Parameter3 = 0;

	datablock.OperationCode = requestblock.OperationCode;

	datablock.TransactionID = requestblock.TransactionID;

	datablock.dataBuffer = gPimaDataBuffer;

	datablock.dataDirection = STILLIMAGE_DATA_IN;

	actualLength = sizeof(gPimaDataBuffer);

	result =
	    stillimg_req_data_response(deviceID, &requestblock,
				       sizeof(requestblock),
				       &gPimaDataBuffer, &actualLength,
				       STILLIMAGE_DATA_IN, &responseblock,
				       sizeof(responseblock));

	printk(KERN_INFO "stillimg_req_response result=0x%x\n", result);

	if (result != OK)

		return result;

	printk(KERN_INFO "responseblock.ResponseCode=0x%x\n",
	       responseblock.ResponseCode);

	if (responseblock.ResponseCode != cpu_to_le16(PIMA_RESP_OK))

		return cpu_to_le16(responseblock.ResponseCode);

	pData = gPimaDataBuffer + 12;

	pStorageInfo->StorageType = extr_le16(&pData);

	pStorageInfo->FilesystemType = extr_le16(&pData);

	pStorageInfo->AccessCapability = extr_le16(&pData);

	pStorageInfo->MaxCapacity = extr_le64(&pData);	/*64bits */

	pStorageInfo->FreeSpaceInBytes = extr_le64(&pData);	/*64bits */

	pStorageInfo->FreeSpaceInImages = extr_le32(&pData);

	pStorageInfo->StorageDescription = extr_le_str(&pData);

	pStorageInfo->VolumeLabel = extr_le_str(&pData);

	return OK;

}

unsigned int pima_get_object_handle(unsigned int deviceID,
				    unsigned int *pFileIDList,
				    unsigned int *pFileCount)
{

	unsigned int result;

	pima_request_block requestblock;

	pima_data_block datablock;

	pima_response_block responseblock;

	unsigned char *pData;

	unsigned int actualLength;

	unsigned int fileCount;

	int i;

	requestblock.OperationCode = cpu_to_le16(PIMA_OP_GET_OBJECT_HANDLES);

	requestblock.TransactionID = cpu_to_le32(gNextTransactionID++);

	requestblock.Parameter1 = cpu_to_le32(0x10001);

	requestblock.Parameter2 = 0;

	requestblock.Parameter3 = 0;

	datablock.OperationCode = requestblock.OperationCode;

	datablock.TransactionID = requestblock.TransactionID;

	datablock.dataBuffer = gPimaDataBuffer;

	datablock.dataDirection = STILLIMAGE_DATA_IN;

	actualLength = sizeof(gPimaDataBuffer);

	result =
	    stillimg_req_data_response(deviceID, &requestblock,
				       sizeof(requestblock),
				       &gPimaDataBuffer, &actualLength,
				       STILLIMAGE_DATA_IN, &responseblock,
				       sizeof(responseblock));

	if (result != OK)

		return result;

	if (responseblock.ResponseCode != cpu_to_le16(PIMA_RESP_OK))

		return cpu_to_le16(responseblock.ResponseCode);

	pData = gPimaDataBuffer + 12;

	fileCount = extr_le16(&pData);

	pData = gPimaDataBuffer + 12;

	pData = extr_le_ary16(&pData);

	if (fileCount > *pFileCount)

		fileCount = *pFileCount;

	for (i = 0; i < fileCount; i++) {

		pFileIDList[i] = extr_le32(&pData);

	}

	*pFileCount = fileCount;

	return OK;

}

unsigned int pima_get_object_info(unsigned int deviceID, unsigned int fileID,
				  pima_object_info * pima_objinfo)
{

	unsigned int result;

	pima_request_block requestblock;

	pima_data_block datablock;

	pima_response_block responseblock;

	unsigned char *pData;

	unsigned int actualLength;

	requestblock.OperationCode = cpu_to_le16(PIMA_OP_GET_OBJECT_INFO);

	requestblock.TransactionID = cpu_to_le32(gNextTransactionID++);

	requestblock.Parameter1 = cpu_to_le32(fileID);

	requestblock.Parameter2 = 0;

	requestblock.Parameter3 = 0;

	datablock.OperationCode = requestblock.OperationCode;

	datablock.TransactionID = requestblock.TransactionID;

	datablock.dataBuffer = gPimaDataBuffer;

	datablock.dataDirection = STILLIMAGE_DATA_IN;

	actualLength = sizeof(gPimaDataBuffer);

	result =
	    stillimg_req_data_response(deviceID, &requestblock,
				       sizeof(requestblock),
				       &gPimaDataBuffer, &actualLength,
				       STILLIMAGE_DATA_IN, &responseblock,
				       sizeof(responseblock));

	if (result != OK)

		return result;

	if (responseblock.ResponseCode != cpu_to_le16(PIMA_RESP_OK))

		return cpu_to_le16(responseblock.ResponseCode);

	pData = gPimaDataBuffer + 12;

	pima_objinfo->StorageID = extr_le32(&pData);

	pima_objinfo->ObjectFormat = extr_le16(&pData);

	pima_objinfo->ProtectionStatus = extr_le16(&pData);

	pima_objinfo->ObjectCompressedSize = extr_le32(&pData);

	pima_objinfo->ThumbFormat = extr_le16(&pData);

	pima_objinfo->ThumbCompressedSize = extr_le32(&pData);

	pima_objinfo->ThumbPixWidth = extr_le32(&pData);

	pima_objinfo->ThumbPixHeight = extr_le32(&pData);

	pima_objinfo->ImagePixWidth = extr_le32(&pData);

	pima_objinfo->ImagePixHeight = extr_le32(&pData);

	pima_objinfo->ImageBitDepth = extr_le32(&pData);

	pima_objinfo->ParentObject = extr_le32(&pData);

	pima_objinfo->AssociationType = extr_le16(&pData);

	pima_objinfo->AssociationDesc = extr_le32(&pData);

	pima_objinfo->SequenceNumber = extr_le32(&pData);

	pima_objinfo->Filename = extr_le_str(&pData);

	pima_objinfo->CaptureDate = extr_le_str(&pData);

	pima_objinfo->ModificationDate = extr_le_str(&pData);

	pima_objinfo->Keywords = extr_le_str(&pData);

	return OK;

}

unsigned int pima_get_thumb(unsigned int deviceID, unsigned int fileID,
			    unsigned char *pThumbBuffer,
			    unsigned int *bufferLength)
{

	unsigned int result;

	pima_request_block requestblock;

	pima_data_block datablock;

	pima_response_block responseblock;

	unsigned char *pData;

	unsigned int actualLength;

	requestblock.OperationCode = cpu_to_le16(PIMA_OP_GET_THUMB);

	requestblock.TransactionID = cpu_to_le32(gNextTransactionID++);

	requestblock.Parameter1 = cpu_to_le32(fileID);

	requestblock.Parameter2 = 0;

	requestblock.Parameter3 = 0;

	datablock.OperationCode = requestblock.OperationCode;

	datablock.TransactionID = requestblock.TransactionID;

	datablock.dataBuffer = gPimaDataBuffer;

	datablock.dataDirection = STILLIMAGE_DATA_IN;

	actualLength = sizeof(gPimaDataBuffer);

	result =
	    stillimg_req_data_response(deviceID, &requestblock,
				       sizeof(requestblock),
				       &gPimaDataBuffer, &actualLength,
				       STILLIMAGE_DATA_IN, &responseblock,
				       sizeof(responseblock));

	if (result != OK)

		return result;

	if (responseblock.ResponseCode != cpu_to_le16(PIMA_RESP_OK))

		return cpu_to_le16(responseblock.ResponseCode);

	pData = gPimaDataBuffer + 12;

	actualLength -= 12;

	copy_to_user(pThumbBuffer, pData,
		     actualLength >
		     *bufferLength ? *bufferLength : actualLength);

	*bufferLength =
	    actualLength > *bufferLength ? *bufferLength : actualLength;

	return OK;

}

static int bFirstGet = 1;

unsigned int pima_start_get_object(unsigned int deviceID, unsigned int fileID)
{

	unsigned int result;

	pima_request_block requestblock;

	requestblock.OperationCode = cpu_to_le16(PIMA_OP_GET_OBJECT);

	requestblock.TransactionID = cpu_to_le32(gNextTransactionID++);

	requestblock.Parameter1 = cpu_to_le32(fileID);

	requestblock.Parameter2 = 0;

	requestblock.Parameter3 = 0;

	result = stillimg_req(deviceID, &requestblock, sizeof(requestblock));

	if (result != OK) {

		printk(KERN_ERR "stillimg_req error, returns 0x%x\n", result);

	}

	bFirstGet = 1;

	printk(KERN_INFO "pima_start_get_object success\n");

	return OK;

}

unsigned int pima_get_object(unsigned int deviceID, unsigned char *pBuffer,
			     unsigned int *bufferLength)
{

	unsigned int result;

	unsigned char *pData;

	unsigned int bytesToGet;

	unsigned int actualLength;

	unsigned int bytesTotalGot;

	int bReachEnd = 0;

	if (bFirstGet) {

		if (*bufferLength % 512 < 12)

			*bufferLength -= 12;

	}

	if (*bufferLength + bFirstGet * 12 > sizeof(gPimaDataBuffer))

		actualLength = sizeof(gPimaDataBuffer);

	else

		actualLength = *bufferLength + bFirstGet * 12;

	bytesToGet = actualLength;

	result =
	    stillimg_data(deviceID, &gPimaDataBuffer, &actualLength,
			  STILLIMAGE_DATA_IN);

	if (result != OK) {

		if (result == 0xb5) {

			printk(KERN_INFO "No enough bytes\n");

			printk(KERN_INFO "bytesToGet=%d, actualLength=%d\n",
			       bytesToGet, actualLength);

		} else {

			printk(KERN_ERR "stillimg_data error, returns 0x%x\n",
			       result);

			*bufferLength = 0;

			return FAIL;

		}

	} else {

		if (actualLength < bytesToGet)

			bReachEnd = 1;

	}

	bytesTotalGot = actualLength - bFirstGet * 12;

	pData = gPimaDataBuffer + bFirstGet * 12;

	bFirstGet = 0;

	copy_to_user(pBuffer, pData, bytesTotalGot);

	if (bReachEnd) {

		*bufferLength = bytesTotalGot;

		return 0;

	}

	while (bytesTotalGot < *bufferLength) {

		if (*bufferLength - bytesTotalGot > sizeof(gPimaDataBuffer))

			actualLength = sizeof(gPimaDataBuffer);

		else

			actualLength = *bufferLength - bytesTotalGot;

		result =
		    stillimg_data(deviceID, &gPimaDataBuffer, &actualLength,
				  STILLIMAGE_DATA_IN);

		if (result != OK) {

			if (result == 0xb5)

				printk(KERN_INFO "No enough bytes\n");

			else {

				printk(KERN_ERR
				       "stillimg_data error, returns 0x%x\n",
				       result);

				*bufferLength = bytesTotalGot;

				return FAIL;

			}

		} else {

			if (actualLength < bytesToGet)

				bReachEnd = 1;

		}

		copy_to_user(pBuffer + bytesTotalGot, gPimaDataBuffer,
			     actualLength);

		bytesTotalGot += actualLength;

		if (bReachEnd) {

			*bufferLength = bytesTotalGot;

			return 0;

		}

	}

	*bufferLength = bytesTotalGot;

	return OK;

}

unsigned int pima_end_get_object(unsigned int deviceID)
{

	unsigned int result;

	pima_response_block responseblock;

	result =
	    stillimg_response(deviceID, &responseblock, sizeof(responseblock));

	if (result != OK)

		return result;

	if (responseblock.ResponseCode != cpu_to_le16(PIMA_RESP_OK))

		return cpu_to_le16(responseblock.ResponseCode);

	printk(KERN_INFO "pima_end_get_object success\n");

	return OK;

}

unsigned int pima_get_whole_object(unsigned int deviceID, unsigned int fileID,
				   unsigned char *pBuffer,
				   unsigned int *bufferLength)
{

	unsigned int result;

	pima_request_block requestblock;

	pima_response_block responseblock;

	unsigned char *pData;

	unsigned int actualLength;

	unsigned int totallen;

	unsigned int bytesGot;

	requestblock.OperationCode = cpu_to_le16(PIMA_OP_GET_OBJECT);

	requestblock.TransactionID = cpu_to_le32(gNextTransactionID++);

	requestblock.Parameter1 = cpu_to_le32(fileID);

	requestblock.Parameter2 = 0;

	requestblock.Parameter3 = 0;

	result = stillimg_req(deviceID, &requestblock, sizeof(requestblock));

	if (result != OK) {

		printk(KERN_ERR "stillimg_req error, returns 0x%x\n", result);

	}

	actualLength = sizeof(gPimaDataBuffer);

	result =
	    stillimg_data(deviceID, &gPimaDataBuffer, &actualLength,
			  STILLIMAGE_DATA_IN);

	if (result != OK) {

		if (result == 0xb5)

			printk(KERN_INFO "No enough bytes\n");

		else

			printk(KERN_ERR "stillimg_data error, returns 0x%x\n",
			       result);

	}

	pData = gPimaDataBuffer;

	totallen = extr_le32(&pData);

	totallen -= 12;

	pData = gPimaDataBuffer + 12;

	for (bytesGot = actualLength; bytesGot < totallen;
	     bytesGot += actualLength) {

		actualLength = sizeof(gPimaDataBuffer);

		result =
		    stillimg_data(deviceID, &gPimaDataBuffer, &actualLength,
				  STILLIMAGE_DATA_IN);

		if (result != OK) {

			if (result == 0xb5)

				printk(KERN_INFO "No enough bytes\n");

			else {

				printk(KERN_ERR
				       "stillimg_data error, returns 0x%x\n",
				       result);

				break;

			}

		}

	}

	result =
	    stillimg_response(deviceID, &responseblock, sizeof(responseblock));

	if (result != OK)

		return result;

	if (responseblock.ResponseCode != cpu_to_le16(PIMA_RESP_OK))

		return cpu_to_le16(responseblock.ResponseCode);

	printk(KERN_INFO "pima_get_object success\n");

	return OK;

}

unsigned int pima_get_partial_object(unsigned int deviceID,
				     unsigned int fileID,
				     unsigned char *pBuffer,
				     unsigned int offset,
				     unsigned int *pBytesRead)
{

	unsigned int result;

	pima_request_block requestblock;

	pima_response_block responseblock;

	unsigned char *pData;

	unsigned int actualLength;

	unsigned int totallen;

	unsigned int bytesGot;

	requestblock.OperationCode = cpu_to_le16(PIMA_OP_GET_PARTIAL_OBJ);

	requestblock.TransactionID = cpu_to_le32(gNextTransactionID++);

	requestblock.Parameter1 = cpu_to_le32(fileID);

	requestblock.Parameter2 = cpu_to_le32(offset);

	requestblock.Parameter3 = cpu_to_le32(*pBytesRead);

	result = stillimg_req(deviceID, &requestblock, sizeof(requestblock));

	if (result != OK) {

		printk(KERN_ERR "stillimg_req error, returns 0x%x\n", result);

	}

	actualLength = sizeof(gPimaDataBuffer);

	result =
	    stillimg_data(deviceID, &gPimaDataBuffer, &actualLength,
			  STILLIMAGE_DATA_IN);

	if (result != OK) {

		if (result == 0xb5)

			printk(KERN_INFO "No enough bytes\n");

		else {

			printk(KERN_ERR "stillimg_data error, returns 0x%x\n",
			       result);

		}

	}

	pData = gPimaDataBuffer;

	totallen = extr_le32(&pData);

	totallen -= 12;

	pData = gPimaDataBuffer + 12;

	memcpy(pBuffer, pData, actualLength);

	for (bytesGot = actualLength; bytesGot < totallen;
	     bytesGot += actualLength) {

		actualLength = sizeof(gPimaDataBuffer);

		result =
		    stillimg_data(deviceID, &gPimaDataBuffer, &actualLength,
				  STILLIMAGE_DATA_IN);

		if (result != OK) {

			if (result == 0xb5)

				printk(KERN_INFO "No enough bytes\n");

			else {

				printk(KERN_ERR
				       "stillimg_data error, returns 0x%x\n",
				       result);

				break;

			}

		}

		memcpy(pBuffer + bytesGot, gPimaDataBuffer, actualLength);

	}

	result =
	    stillimg_response(deviceID, &responseblock, sizeof(responseblock));

	if (result != OK)

		return result;

	if (responseblock.ResponseCode != cpu_to_le16(PIMA_RESP_OK))

		return cpu_to_le16(responseblock.ResponseCode);

	if (responseblock.Parameter1 != totallen) {

		printk(KERN_ERR "pima_get_partial_object: byte count error\n ");

		return FAIL;

	}

	*pBytesRead = totallen;

	printk(KERN_INFO "pima_get_partial_object success\n");

	return OK;

}

static int bFirstSend = 1;

static unsigned int sendFileLength = 0;

unsigned int pima_send_object_info(unsigned int deviceID,
				   unsigned char *pFileName,
				   unsigned int fileSize,
				   unsigned int dirID,
				   unsigned int bForceToImageFile)
{

	unsigned int result;

	pima_request_block requestblock;

	pima_response_block responseblock;

	stillimg_datablock_container *pDataContainer;

	unsigned char *pData;

	unsigned int actualLength;

	unsigned int objectInfoLength;

	int i;

	pima_object_info *pObjectInfo;

	unsigned int fileNameLength;

	requestblock.OperationCode = cpu_to_le16(PIMA_OP_SEND_OBJECT_INFO);

	requestblock.TransactionID = cpu_to_le32(gNextTransactionID++);

	requestblock.Parameter1 = cpu_to_le32(0x10001);

	requestblock.Parameter2 = cpu_to_le32(0xFFFFFFFF);

	requestblock.Parameter3 = 0;

	result = stillimg_req(deviceID, &requestblock, sizeof(requestblock));

	if (result != OK) {

		printk(KERN_ERR "stillimg_req error, returns 0x%x\n", result);

	}
	//copy data
	pDataContainer = (stillimg_datablock_container *) gPimaDataBuffer;

	pDataContainer->ContainerLength = cpu_to_le32(12);

	pDataContainer->ContainerType =
	    cpu_to_le16(STILLIMAGE_ContainerType_Data_Block);

	pDataContainer->Code = cpu_to_le16(PIMA_OP_SEND_OBJECT_INFO);

	pDataContainer->TransactionID = cpu_to_le32(gNextTransactionID++);

	pObjectInfo = (pima_object_info *) pDataContainer->dataBuffer;

	objectInfoLength = 0;

	objectInfoLength =
	    sizeof(pima_object_info) - sizeof(unsigned char *) -
	    sizeof(unsigned char *) -
	    sizeof(unsigned char *) - sizeof(unsigned char *);

	memset(pObjectInfo, 0, objectInfoLength);

	pData = (unsigned char *)pObjectInfo + objectInfoLength;

	if (bForceToImageFile)

		pObjectInfo->ObjectFormat = cpu_to_le16(0x3801);

	else

		pObjectInfo->ObjectFormat = cpu_to_le16(0x3000);

	pObjectInfo->ObjectCompressedSize = cpu_to_le32(fileSize);

	pObjectInfo->ParentObject = cpu_to_le32(dirID);

	fileNameLength = strlen(pFileName);

	if (fileNameLength > 255)

		fileNameLength = 255;

	*pData = (unsigned char)fileNameLength;

	pData++;

	objectInfoLength++;

	for (i = 0; i < fileNameLength; i++) {

		*pData = pFileName[i];

		*(pData + 1) = 0;

		pData += 2;

		objectInfoLength += 2;

	}

	*pData = 0;

	pData++;

	objectInfoLength++;

	*pData = 0;

	pData++;

	objectInfoLength++;

	*pData = 0;

	pData++;

	objectInfoLength++;

	pDataContainer->ContainerLength = cpu_to_le32(objectInfoLength + 12);

	actualLength = objectInfoLength + 12;

	result =
	    stillimg_data(deviceID, &gPimaDataBuffer, &actualLength,
			  STILLIMAGE_DATA_OUT);

	if (result != OK) {

		printk(KERN_ERR "stillimg_data error, returns 0x%x\n", result);

	}

	result =
	    stillimg_response(deviceID, &responseblock, sizeof(responseblock));

	if (result != OK)

		return result;

	if (responseblock.ResponseCode != cpu_to_le16(PIMA_RESP_OK))

		return cpu_to_le16(responseblock.ResponseCode);

	sendFileLength = fileSize;

	bFirstSend = 1;

	printk(KERN_INFO "pima_send_object_info success\n");

	return OK;

}

unsigned int pima_send_object(unsigned int deviceID, unsigned char *pBuffer,
			      unsigned int *pBytesToWrite)
{

	unsigned int result;

	unsigned char *pData;

	unsigned int actualLength;

	unsigned int bytesSent;

	if (bFirstSend) {

		pima_request_block requestblock;

		stillimg_datablock_container *pDataContainer;

		requestblock.OperationCode = cpu_to_le16(PIMA_OP_SEND_OBJECT);

		requestblock.TransactionID = cpu_to_le32(gNextTransactionID++);

		requestblock.Parameter1 = 0;

		requestblock.Parameter2 = 0;

		requestblock.Parameter3 = 0;

		result =
		    stillimg_req(deviceID, &requestblock, sizeof(requestblock));

		if (result != OK) {

			printk(KERN_ERR "stillimg_req error, returns 0x%x\n",
			       result);

		}

		pDataContainer =
		    (stillimg_datablock_container *) gPimaDataBuffer;

		pDataContainer->ContainerLength =
		    cpu_to_le32(sendFileLength + 12);

		pDataContainer->ContainerType =
		    cpu_to_le16(STILLIMAGE_ContainerType_Data_Block);

		pDataContainer->Code = cpu_to_le16(PIMA_OP_SEND_OBJECT);

		pDataContainer->TransactionID =
		    cpu_to_le32(gNextTransactionID++);

		pData = (unsigned char *)pDataContainer->dataBuffer;

	} else {

		pData = (unsigned char *)gPimaDataBuffer;

	}

	if (*pBytesToWrite > sizeof(gPimaDataBuffer) - bFirstSend * 12)

		actualLength = sizeof(gPimaDataBuffer) - bFirstSend * 12;

	else

		actualLength = *pBytesToWrite;

	/*copy the data from user buffer */
	copy_from_user(pData, pBuffer, actualLength);

	actualLength += bFirstSend * 12;

	result =
	    stillimg_data(deviceID, &gPimaDataBuffer, &actualLength,
			  STILLIMAGE_DATA_OUT);

	if (result != OK) {

		if (result == 0xb5)

			printk(KERN_INFO
			       "Too much bytes to send in one time\n");

		else {

			printk(KERN_ERR "stillimg_data error, returns 0x%x\n",
			       result);

			*pBytesToWrite = 0;

			return FAIL;

		}

	}

	bytesSent = actualLength - bFirstSend * 12;

	bFirstSend = 0;

	while (bytesSent < *pBytesToWrite) {

		if (*pBytesToWrite - bytesSent > sizeof(gPimaDataBuffer))

			actualLength = sizeof(gPimaDataBuffer);

		else

			actualLength = *pBytesToWrite - bytesSent;

		/*copy the data from user buffer */
		copy_from_user(gPimaDataBuffer, pBuffer + bytesSent,
			       actualLength);

		result =
		    stillimg_data(deviceID, &gPimaDataBuffer, &actualLength,
				  STILLIMAGE_DATA_OUT);

		if (result != OK) {

			if (result == 0xb5)

				printk(KERN_INFO
				       "Too much bytes to send in one time\n");

			else {

				printk(KERN_ERR
				       "stillimg_data error, returns 0x%x\n",
				       result);

				*pBytesToWrite = bytesSent;

				return FAIL;

			}

		}

		bytesSent += actualLength;

	}

	*pBytesToWrite = bytesSent;

	printk(KERN_INFO "pima_send_object success\n");

	return OK;

}

unsigned int pima_end_send_object(unsigned int deviceID)
{

	unsigned int result;

	pima_response_block responseblock;

	result =
	    stillimg_response(deviceID, &responseblock, sizeof(responseblock));

	if (result != OK)

		return result;

	if (responseblock.ResponseCode != cpu_to_le16(PIMA_RESP_OK))

		return cpu_to_le16(responseblock.ResponseCode);

	printk(KERN_INFO "pima_end_send_object success\n");

	return OK;

}

unsigned int pima_send_whole_object(unsigned int deviceID,
				    unsigned char *pBuffer,
				    unsigned int *pBytesToWrite)
{

	unsigned int result;

	pima_request_block requestblock;

	pima_response_block responseblock;

	stillimg_datablock_container *pDataContainer;

	unsigned char *pData;

	unsigned int actualLength;

	requestblock.OperationCode = cpu_to_le16(PIMA_OP_SEND_OBJECT);

	requestblock.TransactionID = cpu_to_le32(gNextTransactionID++);

	requestblock.Parameter1 = 0;

	requestblock.Parameter2 = 0;

	requestblock.Parameter3 = 0;

	result = stillimg_req(deviceID, &requestblock, sizeof(requestblock));

	if (result != OK) {

		printk(KERN_ERR "stillimg_req error, returns 0x%x\n", result);

	}

	pDataContainer = (stillimg_datablock_container *) gPimaDataBuffer;

	pDataContainer->ContainerLength = cpu_to_le32(*pBytesToWrite + 12);

	pDataContainer->ContainerType =
	    cpu_to_le16(STILLIMAGE_ContainerType_Data_Block);

	pDataContainer->Code = cpu_to_le16(PIMA_OP_SEND_OBJECT);

	pDataContainer->TransactionID = cpu_to_le32(gNextTransactionID++);

	pData = (unsigned char *)pDataContainer->dataBuffer;

	/*copy the data from user buffer */
	copy_from_user(pData, pBuffer, *pBytesToWrite);

	actualLength = *pBytesToWrite + 12;

	result =
	    stillimg_data(deviceID, &gPimaDataBuffer, &actualLength,
			  STILLIMAGE_DATA_OUT);

	if (result != OK) {

		printk(KERN_ERR "stillimg_data error, returns 0x%x\n", result);

	}

	*pBytesToWrite = actualLength - 12;

	result =
	    stillimg_response(deviceID, &responseblock, sizeof(responseblock));

	if (result != OK)

		return result;

	if (responseblock.ResponseCode != cpu_to_le16(PIMA_RESP_OK))

		return cpu_to_le16(responseblock.ResponseCode);

	printk(KERN_INFO "pima_send_whole_object success\n");

	return OK;

}

unsigned int pima_set_object_protection(unsigned int deviceID,
					unsigned int fileID,
					unsigned int bProtection)
{

	unsigned int result;

	pima_request_block requestblock;

	pima_response_block responseblock;

	requestblock.OperationCode = cpu_to_le16(PIMA_OP_SET_OBJECT_PROTECTION);

	requestblock.TransactionID = cpu_to_le32(gNextTransactionID++);

	requestblock.Parameter1 = cpu_to_le32(fileID);

	requestblock.Parameter2 = cpu_to_le32(bProtection != 0);

	requestblock.Parameter3 = 0;

	result =
	    stillimg_req_response(deviceID, &requestblock,
				  sizeof(requestblock), &responseblock,
				  sizeof(responseblock));

	if (result != OK)

		return result;

	if (responseblock.ResponseCode != cpu_to_le16(PIMA_RESP_OK))

		return cpu_to_le16(responseblock.ResponseCode);

	return OK;

}

unsigned int pima_del_obj(unsigned int deviceID, unsigned int fileID)
{

	unsigned int result;

	pima_request_block requestblock;

	pima_response_block responseblock;

	requestblock.OperationCode = cpu_to_le16(PIMA_OP_DELETE_OBJECT);

	requestblock.TransactionID = cpu_to_le32(gNextTransactionID++);

	requestblock.Parameter1 = cpu_to_le32(fileID);

	requestblock.Parameter2 = 0;

	requestblock.Parameter3 = 0;

	result =
	    stillimg_req_response(deviceID, &requestblock,
				  sizeof(requestblock), &responseblock,
				  sizeof(responseblock));

	if (result != OK)

		return result;

	if (responseblock.ResponseCode != cpu_to_le16(PIMA_RESP_OK))

		return cpu_to_le16(responseblock.ResponseCode);

	return OK;

}

unsigned int pima_format_store(unsigned int deviceID, unsigned int storageID)
{

	unsigned int result;

	pima_request_block requestblock;

	pima_response_block responseblock;

	requestblock.OperationCode = cpu_to_le16(PIMA_OP_FORMAT_STORE);

	requestblock.TransactionID = cpu_to_le32(gNextTransactionID++);

	requestblock.Parameter1 = cpu_to_le32(storageID);

	requestblock.Parameter2 = 0;

	requestblock.Parameter3 = 0;

	result =
	    stillimg_req_response(deviceID, &requestblock,
				  sizeof(requestblock), &responseblock,
				  sizeof(responseblock));

	if (result != OK)

		return result;

	if (responseblock.ResponseCode != cpu_to_le16(PIMA_RESP_OK))

		return cpu_to_le16(responseblock.ResponseCode);

	return OK;

}

unsigned int pima_reset_dev(unsigned int deviceID)
{

	unsigned int result;

	pima_request_block requestblock;

	pima_response_block responseblock;

	requestblock.OperationCode = cpu_to_le16(PIMA_OP_RESET_DEVICE);

	requestblock.TransactionID = cpu_to_le32(gNextTransactionID++);

	requestblock.Parameter1 = 0;

	requestblock.Parameter2 = 0;

	requestblock.Parameter3 = 0;

	result =
	    stillimg_req_response(deviceID, &requestblock,
				  sizeof(requestblock), &responseblock,
				  sizeof(responseblock));

	if (result != OK)

		return result;

	if (responseblock.ResponseCode != cpu_to_le16(PIMA_RESP_OK))

		return cpu_to_le16(responseblock.ResponseCode);

	return OK;

}

unsigned int pima_self_test(unsigned int deviceID)
{

	unsigned int result;

	pima_request_block requestblock;

	pima_response_block responseblock;

	requestblock.OperationCode = cpu_to_le16(PIMA_OP_SELF_TEST);

	requestblock.TransactionID = cpu_to_le32(gNextTransactionID++);

	requestblock.Parameter1 = 0;

	requestblock.Parameter2 = 0;

	requestblock.Parameter3 = 0;

	result =
	    stillimg_req_response(deviceID, &requestblock,
				  sizeof(requestblock), &responseblock,
				  sizeof(responseblock));

	if (result != OK)

		return result;

	if (responseblock.ResponseCode != cpu_to_le16(PIMA_RESP_OK))

		return cpu_to_le16(responseblock.ResponseCode);

	return OK;

}

unsigned int pima_power_down(unsigned int deviceID)
{

	unsigned int result;

	pima_request_block requestblock;

	pima_response_block responseblock;

	requestblock.OperationCode = cpu_to_le16(PIMA_OP_POWER_DOWN);

	requestblock.TransactionID = cpu_to_le32(gNextTransactionID++);

	requestblock.Parameter1 = 0;

	requestblock.Parameter2 = 0;

	requestblock.Parameter3 = 0;

	result =
	    stillimg_req_response(deviceID, &requestblock,
				  sizeof(requestblock), &responseblock,
				  sizeof(responseblock));

	if (result != OK)

		return result;

	if (responseblock.ResponseCode != cpu_to_le16(PIMA_RESP_OK))

		return cpu_to_le16(responseblock.ResponseCode);

	return OK;

}

unsigned int pima_cancel_trans(unsigned int deviceID,
			       unsigned int TransactionID)
{

	unsigned int result;

	stillimg_dev_status_data deviceStatusData;

	if (TransactionID == 0 || TransactionID == 1)

		TransactionID = gNextTransactionID - 1;

	result = stillimg_cancel_req(deviceID, TransactionID);

	if (result != OK)

		return result;

	while (1) {

		result =
		    stillimg_get_dev_status(deviceID, &deviceStatusData,
					    sizeof(deviceStatusData));

		if (result != OK)

			return result;

		if (deviceStatusData.Code == cpu_to_le16(PIMA_RESP_DEVICE_BUSY))

			continue;

		break;

	}

	if (deviceStatusData.Code == cpu_to_le16(PIMA_RESP_OK)) {

		printk(KERN_INFO "pima_cancel_trans success\n");

		return OK;

	}

	return deviceStatusData.Code;

}

unsigned int pima_handle_stall(unsigned int deviceID)
{

	unsigned int result;

	stillimg_dev_status_data deviceStatusData;

	result =
	    stillimg_get_dev_status(deviceID, &deviceStatusData,
				    sizeof(deviceStatusData));

	if (result != OK)

		return result;

	if (deviceStatusData.Code ==
	    cpu_to_le16(PIMA_RESP_TRANSACTION_CANCELLED)) {

		printk(KERN_INFO "Device canclled the transaction\n");

	}
	//if unrecoverable error
	if (deviceStatusData.Code == cpu_to_le16(PIMA_RESP_UNDEFINED)) {

		printk(KERN_ERR
		       "Device unrecoverable error. Try to reset device\n");

		result = stillimg_dev_reset_req(deviceID);

		if (result != OK) {

			printk(KERN_ERR "Device reset failed\n");

			return result;

		}

	}

	printk(KERN_INFO "pima_handle_stall finish\n");

	return OK;

}
