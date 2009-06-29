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

#ifndef __SYS_H
#define __SYS_H
#include <linux/string.h>
#include <linux/slab.h>
#include <asm/uaccess.h>

#define OK      0
#define FAIL    -1

#define malloc(a)       kmalloc((a), GFP_KERNEL)
#define free(a)     kfree((a))
#define MALLOC(a)       kmalloc((a), GFP_KERNEL)
#define FREE(a)     kfree((a))

unsigned short extr_le16(unsigned char **ppBuffer);

unsigned int extr_le32(unsigned char **ppBuffer);

unsigned int extr_le64(unsigned char **ppBuffer);

unsigned char *extr_le_str(unsigned char **ppBuffer);

unsigned char *extr_le_ary16(unsigned char **ppBuffer);

void print_uni_str(unsigned char *unicodeString);

unsigned int si_control(unsigned int deviceID, unsigned char bmRequestType,
			unsigned char bRequest);

unsigned int si_control_in(unsigned int deviceID, unsigned char bmRequestType,
			   unsigned char bRequest,
			   unsigned short wLength, void *data);

unsigned int si_control_out(unsigned int deviceID,
			    unsigned char bmRequestType,
			    unsigned char bRequest,
			    unsigned short wLength, void *data);

unsigned int si_bulk_in(unsigned int deviceID, void *data,
			unsigned int *dataLength);

unsigned int si_bulk_out(unsigned int deviceID, void *data,
			 unsigned int *dataLength);

#endif				/* 
				 */
