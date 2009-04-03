/*
  Copyright (C) 2004, Freescale Semiconductor, Inc. All Rights Reserved
  THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
  BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
  Freescale Semiconductor, Inc.
*/

/**
@file png_decoder_test.h

@brief VTE C header PNG decoder



@par Portability:
        SCM-A11, Argon+
        arm-linux-gcc 3.4
        
*/

/*======================== REVISION HISTORY ==================================

Author (core ID)      Date         CR Number    Description of Changes
-------------------   ----------   ----------   ------------------------------
D.Simakov/smkd001c    08/12/2004   TLSbo41678   Initial version
D.Simakov/smkd001c    06/04/2005   TLSbo47116   The reentrance, preemptive and load
                                                testcases were added

=============================================================================*/

#ifndef PNG_DECODER_TEST_H
#define PNG_DECODER_TEST_H

#ifdef __cplusplus
extern "C"{
#endif

/*======================== INCLUDE FILES ====================================*/

#include <sys/types.h>  	// fork usage for reentrance test
#include <unistd.h> 		// fork usage for reentrance test
#include <pthread.h> 		// fork usage for reentrance test
#include <sys/time.h>		// timer usage for preemptivity
#include <math.h>               // for sqrt in hogcpu
#include <assert.h>             // for assert
#include <linux/fb.h>		// for frame buffer operations
#include <sys/mman.h>
#include <sys/ioctl.h>

#include <sys/stat.h>
#include <fcntl.h>
	       

#include <png_dec_interface.h>  // png decoder

/*======================== CONSTANTS ========================================*/

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

/*======================== DEFINES AND MACROS ===============================*/

#define DEFAULT_ITERATIONS 10

/*======================== ENUMS ============================================*/

/** Different test cases in the single application */
typedef enum 
{
    NOMINAL_FUNCTIONALITY = 0,  /**< Nominal encoding/decoding */
    ENDURANCE,			        /**< Endurance test */
    ROBUSTNESS,                 /**< React to a bad input bit-stream, i.e. pdf document */
    REENTRANCE,
    PREEMPTION,
    LOAD
} png_decoder_testcase_t;

/*======================== STRUCTURES AND OTHER TYPEDEFS ====================*/

typedef struct
{   
    int testcase;
    int iter;
    const char * cfg_fname;
    int delay;
} testapp_configuration_t;

/*======================== GLOBAL VARIABLE DECLARATIONS =====================*/

/*======================== FUNCTION PROTOTYPES ==============================*/

int VT_png_decoder_setup();
int VT_png_decoder_cleanup();
int VT_png_decoder_test();


#ifdef __cplusplus
}
#endif

#endif  // PNG_DECODER_TEST_H //
