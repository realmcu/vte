#include <stdio.h>
#include <linux/unistd.h>
#include "ptp_test.h"

#define UINT8       unsigned char
#define UINT16      unsigned short
#define UINT32      unsigned int

//#include "../usbstillimage.h"
#include "usbstillimage.h"

unsigned char gBuffer[16 * 1024];
int fd = -1;
FILE *fp = NULL;

void print_uni_str(UINT8 * unicodeString)
{
	if (!unicodeString) {
		printf("(null)");
		return;
	}
	while ((*unicodeString) | (*(unicodeString + 1))) {
		printf("%c", *unicodeString);
		unicodeString += 2;
	}
}

int GetDeviceList()
{
	int ret;
	usbsidev_para_get_dev_list para_GetDeviceList;

	ret = ioctl(fd, USBSIDEV_GET_DEVICE_LIST, &para_GetDeviceList);
	return ret;
}

static char *OperationInfoPIMA[] = {
	"Undefined",
	"GetDeviceInfo",
	"OpenSession",
	"CloseSession",
	"GetStorageIDs",
	"GetStorageInfo",
	"GetNumObjects",
	"GetObjectHandles",
	"GetObjectInfo",
	"GetObject",
	"GetThumb",
	"DeleteObject",
	"SendObjectInfo",
	"SendObject",
	"InitiateCapture",
	"FormatStore",
	"ResetDevice",
	"SelfTest",
	"SetObjectProtection",
	"PowerDown",
	"GetDevicePropDesc",
	"GetDevicePropValue",
	"SetDevicePropValue",
	"ResetDevicePropValue",
	"TerminateOpenCapture",
	"MoveObject",
	"CopyObject",
	"GetPartialObject",
	"InitiateOpenCapture"
};

char *GetOperationName(unsigned short operationID)
{
	if (operationID < 0x1000 || operationID > 0x101C)
		return "";
	return OperationInfoPIMA[operationID - 0x1000];
}

int GetDeviceInfo()
{
	int ret;
	int i;
	usbsidev_para_get_dev_info arg_devinfo;
	memset(&arg_devinfo, 0, sizeof(arg_devinfo));
	arg_devinfo.deviceID = 1;
	ret = ioctl(fd, USBSIDEV_GET_DEVICE_INFO, &arg_devinfo);
	print_uni_str(arg_devinfo.DeviceInfo.Manufacturer);
	printf("\n");
	print_uni_str(arg_devinfo.DeviceInfo.Model);
	printf("\n");
	print_uni_str(arg_devinfo.DeviceInfo.DeviceVersion);
	printf("\n");
	print_uni_str(arg_devinfo.DeviceInfo.SerialNumber);
	printf("\nOperationsSupported:\n");
	for (i = 0;
	     i <
	     sizeof(arg_devinfo.DeviceInfo.OperationsSupported) /
	     sizeof(arg_devinfo.DeviceInfo.OperationsSupported[0]); i++) {
		if (arg_devinfo.DeviceInfo.OperationsSupported[i] == 0)
			break;
		printf("0x%x  %s\n",
		       arg_devinfo.DeviceInfo.OperationsSupported[i],
		       GetOperationName(arg_devinfo.DeviceInfo.
					OperationsSupported[i]));
		//if(i%8==7) printf("\n");
	}
	printf("\n");
	return 0;
}

int GetStoragesInfo()
{
	ioctl(fd, USBSIDEV_OPEN_SESSION, 0);
	int ret;
	unsigned int buffer[16];
	int i;
	usbsidev_para_get_stg_ids arg_storageIDs;
	arg_storageIDs.deviceID = 0;
	arg_storageIDs.pStorageIDList = buffer;
	arg_storageIDs.StorageIDCount = 8;
	ret = ioctl(fd, USBSIDEV_GET_STORAGE_IDS, &arg_storageIDs);
	printf("storage IDs:\n");
	for (i = 0; i < arg_storageIDs.StorageIDCount; i++) {
		printf("0x%x  ", buffer[i]);
	}
	printf("\n");

	usbsidev_para_get_stg_info arg_getstorageinfo;

	for (i = arg_storageIDs.StorageIDCount - 1; i >= 0; i--) {
		arg_getstorageinfo.deviceID = 0;
		arg_getstorageinfo.storageID = buffer[i];

		ret = ioctl(fd, USBSIDEV_GET_STORAGE_INFO, &arg_getstorageinfo);
		if (ret != 0) {
			printf("Error: USBSIDEV_GET_STORAGE_INFO ret=%d\n",
			       ret);
			continue;
		}
		printf("Storgae ID: 0x%x\n", arg_getstorageinfo.storageID);
		printf("  AccessCapability=%d  ",
		       arg_getstorageinfo.StorageInfo.AccessCapability);
		printf("  MaxCapacity=%d  ",
		       arg_getstorageinfo.StorageInfo.MaxCapacity);
		printf("  FreeSpaceInBytes=%d",
		       arg_getstorageinfo.StorageInfo.FreeSpaceInBytes);
		printf("\n  Storage description:  ");
		print_uni_str(arg_getstorageinfo.StorageInfo.
			      StorageDescription);
		printf("\n  Storage label:  ");
		print_uni_str(arg_getstorageinfo.StorageInfo.VolumeLabel);
		printf("\n");
	}
	printf("That's all\n");
	ioctl(fd, USBSIDEV_CLOSE_SESSION, 0);
	return 0;
}

int GetFilesInfo()
{
	ioctl(fd, USBSIDEV_OPEN_SESSION, 0);
	int ret;
	unsigned int buffer[256];
	int i;
	int filecnt = 0;
	filecnt = ioctl(fd, USBSIDEV_GET_NUM_OBJECTS, 0);
	printf("Total file count: %d\n", filecnt);
	if (filecnt > 256)
		filecnt = 256;

	usbsidev_para_get_stg_handle arg_fileHandles;
	arg_fileHandles.deviceID = 0;
	arg_fileHandles.pFileIDList = buffer;
	arg_fileHandles.fileCount = filecnt;
	ret = ioctl(fd, USBSIDEV_GET_OBJECT_HANDLES, &arg_fileHandles);
	printf("File handles:  ");
	for (i = 0; i < filecnt; i++) {
		printf("0x%x  ", buffer[i]);
	}
	printf("\n\n");

	usbsidev_para_get_obj_info arg_getobjinfo;

	for (i = 0; i < filecnt; i++) {
		arg_getobjinfo.deviceID = 0;
		arg_getobjinfo.fileID = buffer[i];

		ret = ioctl(fd, USBSIDEV_GET_OBJECT_INFO, &arg_getobjinfo);
		printf("File ID:  0x%x        ", arg_getobjinfo.fileID);
		printf("Filename: ");
		print_uni_str(arg_getobjinfo.ObjectInfo.Filename);
		printf("\nCapture date: ");
		print_uni_str(arg_getobjinfo.ObjectInfo.CaptureDate);
		printf("  Modify date: ");
		print_uni_str(arg_getobjinfo.ObjectInfo.ModificationDate);
		printf("\n");
		printf("ObjectFormat=0x%x  ",
		       arg_getobjinfo.ObjectInfo.ObjectFormat);
		printf("AssociationType=0x%x  ",
		       arg_getobjinfo.ObjectInfo.AssociationType);
		printf("ProtectionStatus=%d\n",
		       arg_getobjinfo.ObjectInfo.ProtectionStatus);
		printf("ObjectCompressedSize=%d  ",
		       arg_getobjinfo.ObjectInfo.ObjectCompressedSize);
		printf("ThumbCompressedSize=%d\n",
		       arg_getobjinfo.ObjectInfo.ThumbCompressedSize);
		printf("ThumbPixWidth=%d  ",
		       arg_getobjinfo.ObjectInfo.ThumbPixWidth);
		printf("ThumbPixHeight=%d\n",
		       arg_getobjinfo.ObjectInfo.ThumbPixHeight);
		printf("ImagePixWidth=%d  ",
		       arg_getobjinfo.ObjectInfo.ImagePixWidth);
		printf("ImagePixHeight=%d  ",
		       arg_getobjinfo.ObjectInfo.ImagePixHeight);
		printf("ImageBitDepth=%d\n\n",
		       arg_getobjinfo.ObjectInfo.ImageBitDepth);
	}
	printf("That's all\n");
	ioctl(fd, USBSIDEV_CLOSE_SESSION, 0);
	return 0;
}

int GetFile(unsigned int fileID)
{
	ioctl(fd, USBSIDEV_OPEN_SESSION, 0);
	int ret;
	int i;
	int filelen = 0;
	void *buffer = malloc(64 * 1024);
	if (!buffer) {
		printf("Unable to malloc buffer\n");
		return -1;
	}

	fp = fopen("test.jpg", "wb+");

	usbsidev_para_get_obj_info arg_getobjinfo;
	arg_getobjinfo.deviceID = 0;
	arg_getobjinfo.fileID = fileID;

	ret = ioctl(fd, USBSIDEV_GET_OBJECT_INFO, &arg_getobjinfo);

	usbsidev_para_start_get_obj para1;
	para1.deviceID = 0;
	para1.fileID = fileID;
	ret = ioctl(fd, USBSIDEV_START_GET_OBJECT, &para1);
	printf("ioctl USBSIDEV_START_GET_OBJECT ret=%d\n", ret);
	filelen = arg_getobjinfo.ObjectInfo.ObjectCompressedSize;

	usbsidev_para_get_obj para2;

	int bytesGot = 0;
	while (1) {
		para2.deviceID = 0;
		para2.pBuffer = buffer;
		para2.bufferLength = 64 * 1024;
		ret = ioctl(fd, USBSIDEV_GET_OBJECT, &para2);
		printf
		    ("ioctl usbsidev_para_get_obj ret=%d, get %d bytes\n",
		     ret, para2.bufferLength);
		if (ret)
			break;
		fwrite(buffer, 1, para2.bufferLength, fp);
		bytesGot += para2.bufferLength;
		if (bytesGot >= filelen)
			break;
	}

	ioctl(fd, USBSIDEV_END_GET_OBJECT, 0);
	fclose(fp);
	free(buffer);
	ioctl(fd, USBSIDEV_CLOSE_SESSION, 0);
	return 0;
}

int GetFileThumb(unsigned int fileID)
{
	ioctl(fd, USBSIDEV_OPEN_SESSION, 0);
	int ret;
	void *buffer = malloc(64 * 1024);
	if (!buffer) {
		printf("Unable to malloc buffer\n");
		return -1;
	}
	memset(buffer, 0, sizeof(64 * 1024));

	usbsidev_para_get_thumb para_thumb;
	para_thumb.deviceID = 0;
	para_thumb.fileID = fileID;
	para_thumb.pThumbBuffer = buffer;
	para_thumb.bufferLength = 64 * 1024;
	ret = ioctl(fd, USBSIDEV_GET_THUMB, &para_thumb);
	printf("USBSIDEV_GET_THUMB resturns %d, bufferlen=%d\n", ret,
	       para_thumb.bufferLength);
	fp = fopen("thumb.jpg", "wb+");
	fwrite(buffer, 1, para_thumb.bufferLength, fp);
	fclose(fp);
	free(buffer);
	ioctl(fd, USBSIDEV_CLOSE_SESSION, 0);
	return 0;
}

int SendFile(char *fileName, unsigned int dirID)
{
	ioctl(fd, USBSIDEV_OPEN_SESSION, 0);
	int ret;
	int buffersendlen;
	void *bufferSend;
	fp = fopen(fileName, "rb");
	if (!fp) {
		printf("Unable to open file %s for read\n", fileName);
		return -1;
	}
	bufferSend = malloc(2 * 1024 * 1024);
	if (!bufferSend) {
		printf("Unable to malloc buffer\n");
		fclose(fp);
		return -1;
	}

	buffersendlen = fread(bufferSend, 1, 2 * 1024 * 1024, fp);
	printf("buffersendlen=%d\n", buffersendlen);
	fclose(fp);

	usbsidev_para_start_send_obj para_send1;
	para_send1.deviceID = 0;
	strcpy(para_send1.fileName, fileName);
	para_send1.fileSize = buffersendlen;
	para_send1.dirID = dirID;
	para_send1.bForceToImageFile = 1;
	ret = ioctl(fd, USBSIDEV_START_SEND_OBJECT, &para_send1);
	printf("ioctl USBSIDEV_START_SEND_OBJECT ret=%d\n", ret);

	usbsidev_para_send_obj para_send2;
	para_send2.deviceID = 0;
	para_send2.pBuffer = bufferSend;
	para_send2.nBytesToWrite = buffersendlen;

	ret = ioctl(fd, USBSIDEV_SEND_OBJECT, &para_send2);
	printf("ioctl USBSIDEV_SEND_OBJECT ret=%d, send %d bytes\n", ret,
	       para_send2.nBytesToWrite);

	ret = ioctl(fd, USBSIDEV_END_SEND_OBJECT, 0);
	printf("ioctl USBSIDEV_END_SEND_OBJECT ret=%d\n", ret);

	free(bufferSend);
	ioctl(fd, USBSIDEV_CLOSE_SESSION, 0);
	return 0;
}

int SetFileProtection(unsigned int fileID, int bProtection)
{
	ioctl(fd, USBSIDEV_OPEN_SESSION, 0);
	int ret;
	usbsidev_para_set_obj_protect arg_setprotection;
	arg_setprotection.deviceID = 0;
	arg_setprotection.fileID = fileID;
	arg_setprotection.bProtection = bProtection;
	ret = ioctl(fd, USBSIDEV_SET_OBJECT_PROTECTION, &arg_setprotection);
	printf("USBSIDEV_SET_OBJECT_PROTECTION ret=0x%x\n", ret);
	ioctl(fd, USBSIDEV_CLOSE_SESSION, 0);
	return 0;
}

int DeleteFile(unsigned int fileID)
{
	ioctl(fd, USBSIDEV_OPEN_SESSION, 0);
	int ret;
	usbsidev_para_del_obj arg_delete;
	arg_delete.deviceID = 0;
	arg_delete.fileID = fileID;
	ret = ioctl(fd, USBSIDEV_DELETE_OBJECT, &arg_delete);
	printf("USBSIDEV_DELETE_OBJECT ret=0x%x\n", ret);
	ioctl(fd, USBSIDEV_CLOSE_SESSION, 0);
	return 0;
}

int FormatStorage(unsigned int storID)
{
	ioctl(fd, USBSIDEV_OPEN_SESSION, 0);
	int ret;
	usbsidev_para_format_store arg_format;
	arg_format.deviceID = 0;
	arg_format.storageID = storID;
	ret = ioctl(fd, USBSIDEV_FORMAT_STORE, &arg_format);
	printf("USBSIDEV_FORMAT_STORE ret=0x%x\n", ret);
	ioctl(fd, USBSIDEV_CLOSE_SESSION, 0);
	return 0;
}

int main(int argc, char **argv)
{

	int ret;
	int i;
	char cmd;
	int idInput;
	char selectionInput;
	char filenameInput[32];

	int filecnt = 0;
	unsigned int bufferlen = 0;
	unsigned int arg[8], buffer[32];
	printf("USB Still Image unit test program\n");
	fd = open("/dev/ptp_usb", 0, 0);
	printf("fd is %x\n", fd);

	while (1) {
		printf
		    ("\nPlease input command:\n  0: Exit\n  1: Display device information\n  2: Display storage information\n  3: Display file information\n  \
4: Get file\n  5: Get file thumbnail\n  6: Send a file\n  7: Set file protection\n  8: Delete file\n  9: Format storgae\n");
		cmd = '\n';
		while (cmd == '\n')
			scanf("%c", &cmd);
		switch (cmd) {
		case '0':
			goto _exit;
		case '1':
			printf("Try to  get device information\n");
			GetDeviceList();
			GetDeviceInfo();
			break;
		case '2':
			printf("Try to  get storage information\n");
			GetStoragesInfo();
			break;
		case '3':
			printf("Try to  get files information\n");
			GetFilesInfo();
			break;
		case '4':
			printf("Please input the file ID to get:\n");
			scanf("%x", &idInput);
			printf("Try to get file 0x%x\n", idInput);
			GetFile(idInput);
			break;
		case '5':
			printf("Please input the file ID to get thumbnail:\n");
			scanf("%x", &idInput);
			printf("Try to get file thumbnail 0x%x\n", idInput);
			GetFileThumb(idInput);
			break;
		case '6':
			printf("Please input the file name to send:\n");
			scanf("%s", filenameInput);
			printf
			    ("Please input the dirID where the file to be put to\n");
			scanf("%x", &idInput);
			printf("Try to send file %s to dirID 0x%x\n",
			       filenameInput, idInput);
			SendFile(filenameInput, idInput);
			break;
		case '7':
			printf("Please input the file ID to set protection:\n");
			scanf("%x", &idInput);
			printf
			    ("Please input the file protection. '1' to set protection, '0' to clear protection:\n");
			selectionInput = '\n';
			while (selectionInput == '\n')
				scanf("%c", &selectionInput);
			if (selectionInput == '1') {
				printf("Try to set protection for file 0x%x\n",
				       idInput);
				SetFileProtection(idInput, 1);
			} else {
				printf
				    ("Try to clear protection for file 0x%x\n",
				     idInput);
				SetFileProtection(idInput, 0);
			}
			break;
		case '8':
			printf("Please input the file ID to delete:\n");
			scanf("%x", &idInput);
			printf("Try to delete file 0x%x\n", idInput);
			DeleteFile(idInput);
			break;
		case '9':
			printf("Please input the storage ID to format:\n");
			scanf("%x", &idInput);
			printf("Try to format storage 0x%x\n", idInput);
			FormatStorage(idInput);
			break;
		default:
			printf("Incorrect command: '%c'\n", cmd);
			break;
		}
	}

      _exit:

	close(fd);
	return 0;
}
