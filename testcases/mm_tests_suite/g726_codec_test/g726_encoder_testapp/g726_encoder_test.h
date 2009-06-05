/*================================================================================================*/
/**
    @file   g726_encoder_test.h

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
D.Simakov / smkd001c         12/10/2004     TLSbo43522  Initial version 
D.Simakov / smkd001c         04/04/2005     TLSbo47116  The endurence and load testcases are added.
==================================================================================================*/

#ifndef g726_ENCODER_TEST_H
#define g726_ENCODER_TEST_H

#ifdef __cplusplus
extern "C"{
#endif

/*==================================================================================================
                                         INCLUDE FILES
==================================================================================================*/
#include <sys/types.h>  	// fork usage for reentrance test
#include <unistd.h> 		// fork usage for reentrance test
#include <pthread.h> 		// fork usage for reentrance test
#include <sys/time.h>		// timer usage for preemptivity
#include <math.h>           // for sqrt in hogcpu

#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
	       
#include <g726_enc_api.h>  // G.726 encoder

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
    RE_ENTRANCE,		        /**< Re-entrance */
    RE_LOCATABILITY,            /**< Re-locatability */
    ENDURANCE,
    LOAD    
} g726_encoder_testcase_t;


/*==================================================================================================
                                 STRUCTURES AND OTHER TYPEDEFS
==================================================================================================*/


/*==================================================================================================
                                 GLOBAL VARIABLE DECLARATIONS
==================================================================================================*/


/*==================================================================================================
                                     FUNCTION PROTOTYPES
==================================================================================================*/
int VT_g726_encoder_setup();
int VT_g726_encoder_cleanup();
int VT_g726_encoder_test( int testcase, int iter, const char * cfg_file );


#ifdef __cplusplus
}
#endif

#endif  // g726_ENCODER_TEST_H //
