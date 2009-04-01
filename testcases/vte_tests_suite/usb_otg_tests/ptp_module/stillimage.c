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

#include "sys.h"
#include "stillimage.h"

#define REQUEST_TYPE_HOST_TO_DEVICE     0x21
#define REQUEST_TYPE_DEVICE_TO_HOST     0xA1

#define REQUEST_CODE_CANCEL_REQUEST      0x64
#define REQUEST_CODE_GET_EXTENDED_EVENT_DATA     0x65
#define REQUEST_CODE_DEVICE_RESET_REQUEST       0x66
#define REQUEST_CODE_GET_DEVICE_STATUS      0x67

static stillimg_gen_container CommandBlockContainer;

//static stillimg_datablock_container DataBlockContainer;  
static stillimg_gen_container ResponseBlockContainer;

extern int usbsidev_clearstall(void);

unsigned int stillimg_req(unsigned int deviceID, void *request,
			  unsigned int requestLength)
{

	unsigned int result;

	unsigned int actualLength;

	CommandBlockContainer.ContainerLength =
	    cpu_to_le32(sizeof(stillimg_gen_container));

	CommandBlockContainer.ContainerType =
	    cpu_to_le16(STILLIMAGE_ContainerType_Command_Block);

	memcpy(&(CommandBlockContainer.Code), request, requestLength);

	actualLength = sizeof(CommandBlockContainer);

	result = si_bulk_out(deviceID, &CommandBlockContainer, &actualLength);

	if (result == -EPIPE) {

		printk(KERN_INFO "clear stall\n");

		usbsidev_clearstall();

	}

	return result;

}

unsigned int stillimg_data(unsigned int deviceID, void *data,
			   unsigned int *dataLength, unsigned int dataDirection)
{

	unsigned int result;

	if (dataDirection == STILLIMAGE_DATA_IN) {

		result = si_bulk_in(deviceID, data, dataLength);

		if (result == -EPIPE) {

			printk(KERN_INFO "clear stall\n");

			usbsidev_clearstall();

		}

		if (result != OK)

			return result;

	} else {

		result = si_bulk_out(deviceID, data, dataLength);

		if (result == -EPIPE) {

			printk(KERN_INFO "clear stall\n");

			usbsidev_clearstall();

		}

		if (result != OK)

			return result;

	}

	return OK;

}

unsigned int stillimg_response(unsigned int deviceID, void *response,
			       unsigned int responseLength)
{

	unsigned int result;

	unsigned int actualLength;

	actualLength = sizeof(ResponseBlockContainer);

	result = si_bulk_in(deviceID, &ResponseBlockContainer, &actualLength);

	if (result == -EPIPE) {

		printk(KERN_INFO "clear stall\n");

		usbsidev_clearstall();

	}

	if (result != OK)

		return result;

	/*
	   if(ResponseBlockContainer.ContainerLength!=sizeof(stillimg_gen_container))
	   {
	   printk(KERN_ERR  "ResponseBlockContainer.ContainerLength error\n");
	   } */
	if (ResponseBlockContainer.ContainerType !=
	    cpu_to_le16(STILLIMAGE_ContainerType_Response_Block)) {

		printk(KERN_ERR
		       "ResponseBlockContainer.ContainerType error!\n");

		return FAIL;

	}

	memcpy(response, &(ResponseBlockContainer.Code),
	       actualLength -
	       (sizeof(ResponseBlockContainer.ContainerLength) +
		sizeof(ResponseBlockContainer.ContainerType)));

	return OK;

}

unsigned int stillimg_req_data_response(unsigned int deviceID, void *request,
					unsigned int requestLength,
					void *data,
					unsigned int *dataLength,
					unsigned int dataDirection,
					void *response,
					unsigned int responseLength)
{

	unsigned int result;

	if ((result = stillimg_req(deviceID, request, requestLength)) != OK)

		return result;

	if ((result =
	     stillimg_data(deviceID, data, dataLength, dataDirection)) != OK)

		return result;

	if ((result =
	     stillimg_response(deviceID, response, responseLength)) != OK)

		return result;

	return OK;

}

unsigned int stillimg_req_response(unsigned int deviceID, void *request,
				   unsigned int requestLength,
				   void *response, unsigned int responseLength)
{

	unsigned int result;

	if ((result = stillimg_req(deviceID, request, requestLength)) != OK)

		return result;

	if ((result =
	     stillimg_response(deviceID, response, responseLength)) != OK)

		return result;

	return OK;

}

unsigned int stillimg_cancel_req(unsigned int deviceID,
				 unsigned int TransactionID)
{

	unsigned char data[6];

	*((unsigned short *)data) = cpu_to_le16(0x4001);

	*((unsigned int *)(data + 2)) = cpu_to_le32(TransactionID);

	return si_control_out(deviceID, REQUEST_TYPE_HOST_TO_DEVICE,
			      REQUEST_CODE_CANCEL_REQUEST, 0x0006, &data[0]);

}

unsigned int stillimg_get_ext_event_data(unsigned int deviceID,
					 void *pEventDataBuffer,
					 unsigned int bufferSize)
{

	return si_control_in(deviceID, REQUEST_TYPE_DEVICE_TO_HOST,
			     REQUEST_CODE_GET_EXTENDED_EVENT_DATA, bufferSize,
			     pEventDataBuffer);

}

unsigned int stillimg_dev_reset_req(unsigned int deviceID)
{

	return si_control(deviceID, REQUEST_TYPE_HOST_TO_DEVICE,
			  REQUEST_CODE_DEVICE_RESET_REQUEST);

}

unsigned int stillimg_get_dev_status(unsigned int deviceID,
				     void *pStatusDataBuffer,
				     unsigned int bufferSize)
{

	return si_control_in(deviceID, REQUEST_TYPE_DEVICE_TO_HOST,
			     REQUEST_CODE_GET_DEVICE_STATUS, bufferSize,
			     pStatusDataBuffer);

}
