/*================================================================================================*/
/**
    @file   wbmp_decoder_test.h

    @brief  Test scenario C header template.
*/
/*==================================================================================================

  Copyright (C) 2004, Freescale Semiconductor, Inc. All Rights Reserved
  THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
  BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
  Freescale Semiconductor, Inc.

====================================================================================================
Revision History:
                            Modification     Tracking
Author (core ID)                Date          Number    Description of Changes
-------------------------   ------------    ----------  -------------------------------------------
D.Simakov/smkd001c           12/07/2004     TLSbo40263  Initial version
D.Simakov/smkd001c           08/12/2004     TLSbo41675  Ported to read sources

==================================================================================================*/

#ifndef WBMP_DECODER_TEST_H
#define WBMP_DECODER_TEST_H

#ifdef __cplusplus
extern "C"{
#endif

/*==================================================================================================
                                         INCLUDE FILES
==================================================================================================*/
#include <sys/types.h>  // fork usage for reentrance test
#include <unistd.h> // fork usage for reentrance test
#include <pthread.h> // fork usage for reentrance test
#include <sys/time.h>		// timer usage for preemptivity
#include <math.h>               // for sqrt in hogcpu
#include <assert.h>             // for assert
#include <linux/fb.h>		// for frame buffer operations
#include <sys/mman.h>
#include <sys/ioctl.h>

#include <sys/stat.h>
#include <fcntl.h>


#include <wbmp_interface.h>  // wbmp decoder

/*==================================================================================================
                                           CONSTANTS
==================================================================================================*/
//#if !defined(TRUE) && !defined(FALSE)
//#define TRUE  1
//#define FALSE 0
//#endif
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
    NOMINAL_FUNCTIONALITY = 0,  /**< Nominal encoding/decoding */
    ROBUSTNESS,                 /**< React to a bad input bit-stream, i.e. pdf document */
    ENDURANCE,		        /**< Endurance test */
    LOAD,
    RE_ENTRANCE,        /**< Re-entrance */
    PRE_EMPTION,		        /**< Pre-emption */
} wbmp_decoder_testcase_t;


/*==================================================================================================
                                 STRUCTURES AND OTHER TYPEDEFS
==================================================================================================*/


/*==================================================================================================
                                 GLOBAL VARIABLE DECLARATIONS
==================================================================================================*/


/*==================================================================================================
                                     FUNCTION PROTOTYPES
==================================================================================================*/
int VT_wbmp_decoder_setup();
int VT_wbmp_decoder_cleanup();
int VT_wbmp_decoder_test( int testcase, int iter, const char * cfg_file );


#ifdef __cplusplus
}
#endif

#endif  // WBMP_DECODER_TEST_H //
