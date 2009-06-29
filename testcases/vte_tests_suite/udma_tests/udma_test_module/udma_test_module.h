/***
**Copyright 2006-2009 Freescale Semiconductor, Inc. All Rights Reserved.
**
**The code contained herein is licensed under the GNU General Public
**License. You may obtain a copy of the GNU General Public License
**Version 2 or later at the following locations:
**
**http://www.opensource.org/licenses/gpl-license.html
**http://www.gnu.org/copyleft/gpl.html
**/
/*================================================================================================*/
/**
        @file   udma_test_module.c

        @brief  UDMA test module H-file
====================================================================================================
Revision History:
                            Modification     Tracking
Author/core ID                  Date          Number    Description of Changes
-------------------------   ------------    ----------  -------------------------------------------
A.Ozerov/b00320              02/10/2006     TLSbo78550  Initial version.
A.Ozerov/b00320              01/11/2006     TLSbo81158  UDMA module was fixed for working with all platforms.
A.Ozerov/b00320              05/02/2007     TLSbo87473  Minor changes.

====================================================================================================
Portability:  ARM GCC
==================================================================================================*/
#ifndef __MXC_UDMA_TESTDRIVER_H__
#define __MXC_UDMA_TESTDRIVER_H__

#ifdef __KERNEL__
#include <linux/interrupt.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/dma-mapping.h>

#include <asm-arm/scatterlist.h>
#include <asm-arm/arch-mxc/dma.h>
#endif

//#ifdef DEBUG_TEST
#define UDMA_TRACE(fmt,args...)  printk("--- UDMA --->   %s: " fmt, __FUNCTION__, ## args)
//#else
//#define UDMA_TRACE(msg,...) do {} while(0)
//#endif

#define UDMA_SAND 5
#define UDMA_NAME "mxc_udma"

#ifdef CONFIG_IMX27
#define UDMA_TRANS_SIZE  0x1000
#define UDMA_INTERVAL_MEM 0x2000
#else
#define UDMA_BUF_SIZE 15
#endif

/* ioctl number */
#define	UDMA_IOC_MAGIC                           'v' /* stand for: Virtual dma driver */

#ifdef CONFIG_OTHER_PLATFORM

#define UDMA_IOC_SET_CONFIG                      _IO(UDMA_IOC_MAGIC, 0)
#define UDMA_IOC_SET_CALLBACK                    _IO(UDMA_IOC_MAGIC, 1)
/*#define UDMA_IOC_TEST_CHAINBUFFER                _IO(UDMA_IOC_MAGIC, 2)*/
#define UDMA_IOC_DATA_TRANSFER                   _IO(UDMA_IOC_MAGIC, 3)

#else

#define UDMA_IOC_RAM2RAM                         _IO(UDMA_IOC_MAGIC, 0)
#define UDMA_IOC_RAM2D2RAM2D                     _IO(UDMA_IOC_MAGIC, 1)
#define UDMA_IOC_RAM2RAM2D                       _IO(UDMA_IOC_MAGIC, 2)
#define UDMA_IOC_RAM2D2RAM                       _IO(UDMA_IOC_MAGIC, 3)
#define UDMA_IOC_HW_CHAINBUFFER                  _IO(UDMA_IOC_MAGIC, 4)
#define UDMA_IOC_SW_CHAINBUFFER                  _IO(UDMA_IOC_MAGIC, 5)

#endif

#endif /* __MXC_UDMA_TESTDRIVER_H__ */
