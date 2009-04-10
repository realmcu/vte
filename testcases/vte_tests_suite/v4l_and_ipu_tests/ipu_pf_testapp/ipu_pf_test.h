/*================================================================================================*/
/**
        @file   ipu_pf_test.h

        @brief  Test scenario C header template.
*/
/*==================================================================================================

        Copyright (C) 2005, Freescale Semiconductor, Inc. All Rights Reserved
        THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
        BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
        Freescale Semiconductor, Inc.

====================================================================================================
Revision History:
                            Modification     Tracking
Author (core ID)                Date          Number    Description of Changes
-------------------------   ------------    ----------  -------------------------------------------
A.Smirnov                    26/05/2005      TLSbo49894  Initial version
D.Simakov/smkd001c           21/09/2005      TLSbo55077  Re-written
D.Kardakov                   30/08/2006      TLSbo75997  The new field in sTestappConfig struct was added
D.Kardakov                   02/01/2007      TLsbo87909  Bug with unit16_t type was fixed
Hake.H                       07/04/2009      n/a         add case for ipu pf pause and resume case in h264 process according to the ipu spec.
==================================================================================================*/
#ifndef _IPU_PF_TEST_H_
#define _IPU_PF_TEST_H_

#ifdef __cplusplus
extern "C"{
#endif

/*==================================================================================================
                                         INCLUDE FILES
==================================================================================================*/
/* Standard Include Files */
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <ctype.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>

/* IPU specific headers */
typedef unsigned long  __u32;
typedef unsigned long  uint32_t;
typedef unsigned short uint16_t;
#include <linux/version.h>

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,26))
  #include <linux/mxc_pf.h>
#else
 #include <asm/arch/mxc_pf.h>
#endif
/* Harness Specific Include Files.*/
#include <test.h>
#include <usctest.h>

/*==================================================================================================
                                       DEFINES AND MACROS
==================================================================================================*/
#if !defined(TRUE) && !defined(FALSE)
    #define TRUE  1
    #define FALSE 0
#endif

/*==================================================================================================
                                             ENUMS
==================================================================================================*/
enum
{
        PF_DISABLE_ALL          = 0,
        PF_MPEG4_DEBLOCK        = 1,
        PF_MPEG4_DERING         = 2,
        PF_MPEG4_DEBLOCK_DERING = 3,
        PF_H264_DEBLOCK         = 4,
};

/*==================================================================================================
                                 STRUCTURES AND OTHER TYPEDEFS
==================================================================================================*/
typedef struct
{
        int          mTestCase;         /*!< Test case number according to the test plan.*/
        const char * mInputFileName;    /*!< Filename of an input image. */
        const char * mOutputFileName;   /*!< Filename of the filtered image. */
        int          mVerbose;          /*!< Verbose mode. */
        int          mNumFilterCycle;   /*!< Number of image filtering*/
	int          mH264PauseRow;     /*!< H264 pause resume mode rows to pause*/
} sTestappConfig;

/*==================================================================================================
                                     FUNCTION PROTOTYPES
==================================================================================================*/
int VT_ipu_pf_setup   ( void );
int VT_ipu_pf_cleanup ( void );
int VT_ipu_pf_test    ( void );

#ifdef __cplusplus
}
#endif

#endif        /* _IPU_PF_TEST_H_ */
