/*
  Copyright (C) 2004, Freescale Semiconductor, Inc. All Rights Reserved
  THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
  BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
  Freescale Semiconductor, Inc.
*/

/**
@file gif_decoder_test.h

@brief VTE C header template

Description of the file

@par Portability:
        Indicate if this module is portable to other compilers or platforms.
        If not, indicate specific reasons why is it not portable.
*/

/*======================== REVISION HISTORY ==================================

Author (core ID)      Date         CR Number    Description of Changes
-------------------   ----------   ----------   ------------------------------
 E.Gromazina          25/02/2005   TLSbo47116   Initial version
 D.Simakov / smkd001c 06/04/2005   TLSbo47116   The endurance and load test cases 
                                                were added
 A.Pshenichnikov      07/12/2005   TLSbo59709   bugs with the preemptive and
						reentrance test cases were fixed 
=============================================================================*/


#ifndef GIF_DECODER_TEST_H
#define GIF_DECODER_TEST_H

#ifdef __cplusplus
extern "C"{
#endif

/*======================== INCLUDE FILES ====================================*/

#include <sys/types.h>  	// fork usage for reentrance test
#include <unistd.h> 		// fork usage for reentrance test
#include <pthread.h> 		// fork usage for reentrance test
#include <sys/time.h>		// timer usage for preemptivity
#include <math.h>           // for sqrt in hogcpu

#include <sys/stat.h>
#include <fcntl.h>
	       

#include <gif_dec_interface.h>

/*======================== CONSTANTS ========================================*/

#if !defined(TRUE)
    #define TRUE 1
#endif
#if !defined(FALSE)
    #define FALSE 0
#endif        

/*======================== DEFINES AND MACROS ===============================*/

#define DEFAULT_ITERATIONS         5
#define MAX_CODEC_THREADS        4

#ifdef DEBUG_TEST
#include "sub/mem_stat.h"
#define alloc_fast(A)        MemStat_Alloc(A)
#define alloc_slow(A) 	     MemStat_Alloc(A)
#else  
#define alloc_fast(A)        malloc(A)
#define alloc_slow(A)        malloc(A)
#endif 

/*==================================ENUMS====================================*/

typedef enum
{
    NOMINAL_FUNCTIONALITY,
    PRE_EMPTION,
    RE_ENTRANCE,
    ENDURANCE,
    LOAD
} png_decoder_testcase_t;


/*======================== STRUCTURES AND OTHER TYPEDEFS ====================*/


/*======================== GLOBAL VARIABLE DECLARATIONS =====================*/


/*======================== FUNCTION PROTOTYPES ==============================*/

int VT_gif_decoder_setup();
int VT_gif_decoder_cleanup();
int VT_gif_decoder_test( int testcase, int iter, char * cfg_file );


#ifdef __cplusplus
}
#endif

#endif  // GIG_DECODER_TEST_H //
