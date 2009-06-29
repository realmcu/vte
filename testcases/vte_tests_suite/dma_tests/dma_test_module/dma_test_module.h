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
/*================================================================================================*/
/**
        @file   dma_test_module.c

        @brief  DMA test module H-file
====================================================================================================
Revision History:
                            Modification     Tracking
Author/core ID                  Date          Number    Description of Changes
-------------------------   ------------    ----------  -------------------------------------------
D.Simakov / B00236           08/30/2006     TLSbo76801  Initial version
====================================================================================================
Portability:  ARM GCC
==================================================================================================*/

#ifndef __MXC_DMA_TESTDRIVER_H__
#define __MXC_DMA_TESTDRIVER_H__

#ifndef TEST_USER_SPACE
#include <linux/ioctl.h>
#include <asm/arch/hardware.h>
#endif

// ioctl number
#define	DMA_IOC_MAGIC           'v' // stand for: Virtual dma driver

#define	DMA_IOC_SET_SOURCE_ADDR                 _IO(DMA_IOC_MAGIC, 0)
#define	DMA_IOC_SET_DEST_ADDR                   _IO(DMA_IOC_MAGIC, 1)
#define	DMA_IOC_SET_COUNT                       _IO(DMA_IOC_MAGIC, 2)
#define	DMA_IOC_SET_BURST_LENGTH                _IO(DMA_IOC_MAGIC, 3)
#define	DMA_IOC_SET_REQUEST_TMOUT               _IO(DMA_IOC_MAGIC, 4)
#define DMA_IOC_SET_BUS_UTILZ                   _IO(DMA_IOC_MAGIC, 5)
#define DMA_IOC_SET_SOURCE_MODE                 _IO(DMA_IOC_MAGIC, 6)
#define DMA_IOC_SET_DEST_MODE                   _IO(DMA_IOC_MAGIC, 7)
#define DMA_IOC_SET_MEMORY_DIRECTION            _IO(DMA_IOC_MAGIC, 8)
#define DMA_IOC_SET_SOURCE_SIZE                 _IO(DMA_IOC_MAGIC, 9)
#define DMA_IOC_SET_DEST_SIZE                   _IO(DMA_IOC_MAGIC, 10)

#define DMA_IOC_REQUEST_2D_REG                  _IO(DMA_IOC_MAGIC, 11)
#define DMA_IOC_FREE_2D_REG                     _IO(DMA_IOC_MAGIC, 12)
#define DMA_IOC_SET_2D_REG_W_SIZE               _IO(DMA_IOC_MAGIC, 13)
#define DMA_IOC_SET_2D_REG_X_SIZE               _IO(DMA_IOC_MAGIC, 14)
#define DMA_IOC_SET_2D_REG_Y_SIZE               _IO(DMA_IOC_MAGIC, 15)
#define DMA_IOC_GET_2D_REG_SET_NR               _IO(DMA_IOC_MAGIC, 16)

#define DMA_IOC_COPY_FROM_DMA_MEMORY            _IO(DMA_IOC_MAGIC, 17)
#define DMA_IOC_COPY_TO_DMA_MEMORY              _IO(DMA_IOC_MAGIC, 18)
#define DMA_IOC_RESIZE_DMA_MEMORY               _IO(DMA_IOC_MAGIC, 19)

#define  DMA_IOC_SET_VERIFY                     _IO(DMA_IOC_MAGIC, 21)
#define  DMA_IOC_QUERY_REPEAT                   _IO(DMA_IOC_MAGIC, 22)
#define  DMA_IOC_SET_REPEAT                     _IO(DMA_IOC_MAGIC, 23)
#define  DMA_IOC_SET_ACRPT                      _IO(DMA_IOC_MAGIC, 24)
#define  DMA_IOC_SET_CONFIG                     _IO(DMA_IOC_MAGIC, 25)
#define DMA_IOC_SET_TRANSFER_AGAIN              _IO(DMA_IOC_MAGIC, 27)
#define DMA_IOC_TEST_CHAINBUFFER                _IO(DMA_IOC_MAGIC, 28)

//Transfer mode
#define LINEAR_MEMORY_MODE      0
#define _2D_MEMORY_MODE         1

//Memory direction
#define ADDR_INC                0
#define ADDR_DEC                1

//Size of data transfer
#define _32_BIT                 0
#define _16_BIT                 1
#define _8_BIT                  2

#ifndef TEST_USER_SPACE
// Supplemental information for the dma channels. 
typedef unsigned char READ_WRITE_DIRECTION; 
#define READ_ 1
#define WRITE_ 2
typedef struct
{
        pid_t pid;
        struct file *filp;
        unsigned long memory_for_dma;
        size_t dma_memory_size;
        unsigned long memory_for_user;
        unsigned long user;
        READ_WRITE_DIRECTION rw;
        char* memory_source;//Shirley added for dma-16channel-test
        char* memory_dest; //Shirley added for dma-16channel-test
        char* memory_oripage; //Shirley added for dma-16channel-test
}dma_ownership_t;

typedef struct
{
        dmach_t channel;
}dma_2d_ownership_t;
#endif

#define DMA_CH_BASE(x) (DMA_BASE_ADDR+0x080+0x040*(x))


#endif //__MXC_DMA_TESTDRIVER_H__
