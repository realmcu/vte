/*
  Copyright (C) 2004, Freescale Semiconductor, Inc. All Rights Reserved
  THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
  BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
  Freescale Semiconductor, Inc.
*/

/**
@file G723_decoder_test.h

@brief VTE C header template

Description of the file

@par Portability:
        Indicate if this module is portable to other compilers or platforms.
        If not, indicate specific reasons why is it not portable.
*/

/*======================== REVISION HISTORY ==================================

Author (core ID)      Date         CR Number    Description of Changes
-------------------   ----------   ----------   ------------------------------
Filinova Natalya      15/02/2005   TLSbo47117   BRIEF desc. of changes
Delaspre/rc149c       07/12/2004   TLSbo40142   update copyrights with Freescale
D.Simakov/smkd001c    15/04/2005   TLSbo47117   Pre-emptivity test case was added
D.Simakov/smkd001c    24/10/2005   TLSbo57009   FLIST_CFG was fixed
=============================================================================*/

#ifndef G723_DECODER_TEST_H
#define G723_DECODER_TEST_H

#ifdef __cplusplus
extern "C"{
#endif

/*======================== INCLUDE FILES ====================================*/

#include <sys/types.h>  // fork usage for reentrance test
#include <unistd.h> // fork usage for reentrance test
#include <pthread.h> // fork usage for reentrance test
#include <sys/time.h>		// timer usage for preemptivity
#include <sys/types.h>
#include <sys/stat.h>
#include <math.h>               // for sqrt in hogcpu
//#include <asm/mman.h>
#include <string.h>
//#include <asm/fcntl.h>
//#include <asm/ioctls.h>

/*======================== CONSTANTS ========================================*/

#if !defined(TRUE) && !defined(FALSE)
#define TRUE  1
#define FALSE 0
#endif

/*======================== DEFINES AND MACROS ===============================*/

#define DEFAULT_ITERATIONS 10
#define ERROR -1
#define SUCCESS 0

#define alloc_fast(A)       malloc(A)
#define alloc_slow(A)       malloc(A)
#define mem_free(A)         free(A)

#define FLIST_CFG           "g723d_cfg" /*configure file with input and reference data*/
#define MAX_STR_LEN         256
#define DEFAULT_CFG_DIR_PATH "./"             /*path to directory with configure file */

/*======================== ENUMS ============================================*/

/** Different test cases in the single application */
typedef enum
{
    NOMINAL_FUNCTIONALITY = 1,  /**< Nominal decoding */
    ENDURANCE,                  /**< Endurance test */
    RE_ENTRANCE,                /**< Re-entrance  */
    PRE_EMPTION,                /**< Pre-emption */
    RE_LOCATABILITY,            /**< Re-locability */
    LOAD_ENVIROUNMENT,          /**< Working in a load envirounment */
} G723_DECODER_TESTCASES;

/*======================== STRUCTURES AND OTHER TYPEDEFS ====================*/


/*======================== GLOBAL VARIABLE DECLARATIONS =====================*/


/*======================== FUNCTION PROTOTYPES ==============================*/

void cleanup();

int VT_G723_decoder_setup();
int VT_G723_decoder_cleanup();
int VT_G723_decoder_test( int testcase, int iter, int duration, int no_output, char* cfg_dir);


#ifdef __cplusplus
}
#endif

#endif  // G723_DECODER_TEST_X_H //

