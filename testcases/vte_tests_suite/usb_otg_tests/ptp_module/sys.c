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
#include "sys.h"

unsigned short extr_le16(unsigned char **ppBuffer)
{

	unsigned short val = (*(*ppBuffer)) | (*(*ppBuffer + 1) << 8);

	(*ppBuffer) += 2;

	return val;

}

unsigned int extr_le32(unsigned char **ppBuffer)
{

	unsigned int val =
	    (*(*ppBuffer)) | (*(*ppBuffer + 1) << 8) | (*(*ppBuffer + 2) << 16)

	    | (*(*ppBuffer + 3) << 24);

	(*ppBuffer) += 4;

	return val;

}

unsigned int extr_le64(unsigned char **ppBuffer)
{

	unsigned int val =
	    (*(*ppBuffer)) | (*(*ppBuffer + 1) << 8) | (*(*ppBuffer + 2) << 16)

	    | (*(*ppBuffer + 3) << 24);

	(*ppBuffer) += 8;

	return val;

}

unsigned char *extr_le_str(unsigned char **ppBuffer)
{

	static unsigned char nullstring[] =
	    { 0, '(', 0, 'n', 0, 'u', 0, 'l', 0, 'l', 0, ')', 0 };

	unsigned char *ret;

	unsigned char strlen = *(*ppBuffer);

	ret = (*ppBuffer) + 1;

	(*ppBuffer) += (1 + strlen * 2);

	if (strlen == 0)

		return &nullstring[1];

	return ret;

}

unsigned char *extr_le_ary16(unsigned char **ppBuffer)
{

	unsigned char *ret;

	unsigned int arraylen = extr_le32(ppBuffer);

	ret = (*ppBuffer);

	(*ppBuffer) += (arraylen * 2);

	return ret;

}

void print_uni_str(unsigned char *unicodeString)
{

	if (!unicodeString) {

		printk(KERN_INFO "(null)");

		return;

	}

	while ((*unicodeString) | (*(unicodeString + 1))) {

		printk(KERN_INFO "%c", *unicodeString);

		unicodeString += 2;

	}

}

extern int usbsidev_ctrl(unsigned int deviceID, unsigned char bmRequestType,
			 unsigned char bRequest);

extern int usbsidev_ctrl_in(unsigned int deviceID, unsigned char bmRequestType,
			    unsigned char bRequest, unsigned short wLength,
			    void *data);

extern int usbsidev_ctrl_out(unsigned int deviceID,
			     unsigned char bmRequestType,
			     unsigned char bRequest, unsigned short wLength,
			     void *data);

extern int usbsidev_in(unsigned char *buffer, unsigned int *bufferlen);

extern int usbsidev_out(unsigned char *buffer, unsigned int *bufferlen);

inline unsigned int si_control(unsigned int deviceID,
			       unsigned char bmRequestType,
			       unsigned char bRequest)
{

	return usbsidev_ctrl(deviceID, bmRequestType, bRequest);

}

inline unsigned int si_control_in(unsigned int deviceID,
				  unsigned char bmRequestType,
				  unsigned char bRequest,
				  unsigned short wLength, void *data)
{

	return usbsidev_ctrl_in(deviceID, bmRequestType, bRequest, wLength,
				data);

}

inline unsigned int si_control_out(unsigned int deviceID,
				   unsigned char bmRequestType,
				   unsigned char bRequest,
				   unsigned short wLength, void *data)
{

	return usbsidev_ctrl_out(deviceID, bmRequestType, bRequest, wLength,
				 data);

}

inline unsigned int si_bulk_in(unsigned int deviceID, void *data,
			       unsigned int *dataLength)
{

	return usbsidev_in(data, dataLength);

}

inline unsigned int si_bulk_out(unsigned int deviceID, void *data,
				unsigned int *dataLength)
{

	return usbsidev_out(data, dataLength);

}
