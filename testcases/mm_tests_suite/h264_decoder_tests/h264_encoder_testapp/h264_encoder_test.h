/*
  Copyright (C) 2004, Freescale Semiconductor, Inc. All Rights Reserved
  THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
  BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
  Freescale Semiconductor, Inc.
*/
	
/**
@file h264_encoder_test.h
	
@brief VTE C header template
	
@par Portability:
        Indicate if this module is portable to other compilers or platforms.
        If not, indicate specific reasons why is it not portable.
*/
	
/*======================== REVISION HISTORY ==================================
		
Author (core ID)      Date         CR Number    Description of Changes
-------------------   ----------   ----------   ------------------------------
I.Inkina/nknl001      1/07/2005    TLSbo52105   Initial version
D.Simakov/smkd001c    22/08/2005   TLSbo53252   Re-written
=============================================================================*/

#ifndef __H264_ENCODER_TEST_H__
#define __H264_ENCODER_TEST_H__

#ifdef __cplusplus
extern "C"{
#endif

/*==================================================================================================
                                         INCLUDE FILES
==================================================================================================*/

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>               
#include <sys/mman.h>
#include <fcntl.h>
#include <assert.h>
#include <pthread.h>
#include <math.h>

/* Harness Specific Include Files. */
#include "test.h"

/* Config's parser. */
#include "stuff/cfg_parser.h"


/*==================================================================================================
                                           CONSTANTS
==================================================================================================*/
#if !defined(TRUE)
    #define TRUE 1
#endif
#if !defined(FALSE)
    #define FALSE 0
#endif        

/*==================================================================================================
                                       DEFINES AND MACROS
==================================================================================================*/
#define DEFAULT_ITERATIONS 10

/*==================================================================================================
                                             ENUMS
==================================================================================================*/
/** Different test cases in the single application */
typedef enum 
{
    NOMINAL_FUNCTIONALITY = 0, 
    RELOCATABILITY,
    RE_ENTRANCE,
    PRE_EMPTION,    
    ENDURANCE,
    LOAD    
} eAacEncoderTestCases;


/*==================================================================================================
                                 STRUCTURES AND OTHER TYPEDEFS
==================================================================================================*/
typedef struct 
{
    int          mTestCase;
    int          mNumIter;
    const char * mConfigFilename;        
    int          mOutputBan; 
    int          mVerbose;
} sTestappConfig;

/*==================================================================================================
                                 GLOBAL VARIABLE DECLARATIONS
==================================================================================================*/
extern sTestappConfig gTestappConfig;

/*==================================================================================================
                                     FUNCTION PROTOTYPES
==================================================================================================*/
int VT_h264_encoder_setup();
int VT_h264_encoder_cleanup();
int VT_h264_encoder_test();


#ifdef __cplusplus
}
#endif

#endif //__H264_ENCODER_TEST_H__
