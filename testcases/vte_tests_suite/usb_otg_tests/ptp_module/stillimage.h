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

/*
* Freescale PTP structure define
*/
#ifndef __STILLIMAGE_H
#define __STILLIMAGE_H

typedef struct {

	unsigned int ContainerLength __attribute__ ((packed));

	unsigned short ContainerType __attribute__ ((packed));

	unsigned short Code __attribute__ ((packed));

	unsigned int TransactionID __attribute__ ((packed));

	unsigned int Parameter1 __attribute__ ((packed));

	unsigned int Parameter2 __attribute__ ((packed));

	unsigned int Parameter3 __attribute__ ((packed));

} stillimg_gen_container;

typedef enum {
	STILLIMAGE_ContainerType_undefined = 0,
	STILLIMAGE_ContainerType_Command_Block = 1,
	STILLIMAGE_ContainerType_Data_Block = 2,
	STILLIMAGE_ContainerType_Response_Block = 3,
	STILLIMAGE_ContainerType_Event_Block = 4
} stillimg_container_type;

typedef struct {

	unsigned int ContainerLength __attribute__ ((packed));

	unsigned short ContainerType __attribute__ ((packed));

	unsigned short Code __attribute__ ((packed));

	unsigned int TransactionID __attribute__ ((packed));

	unsigned int dataBuffer[32 * 1024] __attribute__ ((packed));

} stillimg_datablock_container;

typedef enum {
	STILLIMAGE_DATA_IN,
	STILLIMAGE_DATA_OUT,
} STILLIMAGE_DATA_DIRECTION;

typedef struct {

	unsigned short wLength __attribute__ ((packed));

	unsigned short Code __attribute__ ((packed));

	unsigned int Parameter1 __attribute__ ((packed));

	unsigned int Parameter2 __attribute__ ((packed));

	unsigned int Parameter3 __attribute__ ((packed));

} stillimg_dev_status_data;

typedef struct {

	unsigned int InterruptDataLength __attribute__ ((packed));

	unsigned short ContainerType __attribute__ ((packed));

	unsigned short EventCode __attribute__ ((packed));

	unsigned int TransactionID __attribute__ ((packed));

	unsigned int EventParameter1 __attribute__ ((packed));

	unsigned int EventParameter2 __attribute__ ((packed));

	unsigned int EventParameter3 __attribute__ ((packed));

} stillimg_asyn_event_int_data;

unsigned int stillimg_req(unsigned int deviceID, void *request,
			  unsigned int requestLength);

unsigned int stillimg_data(unsigned int deviceID, void *data,
			   unsigned int *dataLength,
			   unsigned int dataDirection);

unsigned int stillimg_response(unsigned int deviceID, void *response,
			       unsigned int responseLength);

unsigned int stillimg_req_data_response(unsigned int deviceID, void *request,
					unsigned int requestLength,
					void *data,
					unsigned int *dataLength,
					unsigned int dataDirection,
					void *response,
					unsigned int responseLength);

unsigned int stillimg_req_response(unsigned int deviceID, void *request,
				   unsigned int requestLength, void *response,
				   unsigned int responseLength);

unsigned int stillimg_cancel_req(unsigned int deviceID,
				 unsigned int TransactionID);

unsigned int stillimg_get_ext_event_data(unsigned int deviceID,
					 void *pEventDataBuffer,
					 unsigned int bufferSize);

unsigned int stillimg_dev_reset_req(unsigned int deviceID);

unsigned int stillimg_get_dev_status(unsigned int deviceID,
				     void *pStatusDataBuffer,
				     unsigned int bufferSize);

#endif				/* 
				 */
