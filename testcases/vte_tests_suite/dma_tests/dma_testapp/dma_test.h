/*================================================================================================*/
/**
        @file   dma_test.h

        @brief  DMA test H-file
*/
/*==================================================================================================

        Copyright (C) 2006, Freescale Semiconductor, Inc. All Rights Reserved
        THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
        BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
        Freescale Semiconductor, Inc.

====================================================================================================
Revision History:
                            Modification     Tracking
Author/core ID                  Date          Number    Description of Changes
-------------------------   ------------    ----------  -------------------------------------------
D.Simakov / B00236           08/30/2006     TLSbo76801  Initial version
====================================================================================================
Portability:  ARM GCC
==================================================================================================*/

#ifndef __MXC_DMA_TEST_H__
#define __MXC_DMA_TEST_H__

/*==================================================================================================
                                        INCLUDE FILES
 ==================================================================================================*/
                                        
#include <linux/ioctl.h>
#include <dma_test_module.h>
#include <test.h>


/*==================================================================================================
                                         DEFINES AND MACROS
 ==================================================================================================*/

#define MX2_DMA_CHANNELS 16
#define MAX_DMA_CHANNELS MX2_DMA_CHANNELS

#define MX_DMA_CHANNELS  MX2_DMA_CHANNELS

#define DMA_MEM_SIZE_8  0x1
#define DMA_MEM_SIZE_16 0x2
#define DMA_MEM_SIZE_32 0x0

#define DMA_TYPE_LINEAR 0x0
#define DMA_TYPE_2D     0x01
#define DMA_TYPE_FIFO   0x2
#define DMA_TYPE_EBE    0x3

#define DMA_DONE                0x1000
#define DMA_BURST_TIMEOUT       0x1
#define DMA_REQUEST_TIMEOUT     0x2
#define DMA_TRANSFER_ERROR      0x4
#define DMA_BUFFER_OVERFLOW     0x8


/*==================================================================================================
                                STRUCTURES AND OTHER TYPEDEFS
 ==================================================================================================*/
                                                                                                    
/* Testapp configuration. */ 
typedef struct
{
        int     mTestCase;
        int     mVerbose;
        char  * mCustumParams; /* "srcmode dstmode srcport dstport X Y W count direction burstlength repeat" */
} sTestappConfig; 


/*==================================================================================================
                                GLOBAL VARIABLES
 ==================================================================================================*/

extern sTestappConfig gTestappConfig;

/*==================================================================================================
                                FUNCTIONS
 ==================================================================================================*/

int VT_dma_setup(void);
int VT_dma_test(void);
int VT_dma_cleanup(void);

#endif //__MXC_DMA_TEST_H__
