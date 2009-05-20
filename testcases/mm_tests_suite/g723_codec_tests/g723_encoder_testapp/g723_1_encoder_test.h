/*
  Copyright (C) 2004, Freescale Semiconductor, Inc. All Rights Reserved
  THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
  BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
  Freescale Semiconductor, Inc.
*/

/**
@file g723_1_encoder_test.h

@brief VTE C header template

Description of the file

@par Portability:
        Indicate if this module is portable to other compilers or platforms.
        If not, indicate specific reasons why is it not portable.
*/

/*======================== REVISION HISTORY ==================================

Author (core ID)      Date         CR Number    Description of Changes
-------------------   ----------   ----------   ------------------------------
I.Inkina/nknl001      27/12/2004   TLSbo47117   BRIEF desc. of changes
D.Simakov/smkd001c    15/04/2005   TLSbo47117   Some new testcases were added

=============================================================================*/


#ifndef G723_1_ENCODER_TEST_H
#define G723_1_ENCODER_TEST_H

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
#include "g723_enc_api.h"
#include "g723_common_api.h"

/*==================================================================================================
                                           CONSTANTS
==================================================================================================*/
#if !defined(TRUE) && !defined(FALSE)
#define TRUE  1
#define FALSE 0
#endif


/*==================================================================================================
                                       DEFINES AND MACROS
==================================================================================================*/
/******************************************************************************/
#define alloc_fast(A)        malloc(A)
#define alloc_slow(A)        malloc(A)
#define mem_free(A)          free(A)


#define DEFAULT_ITERATIONS     10
#define ENCODER_THREAD         4               /* max number of encoding thread */


#define   RELOCATE_CYCLE       10
#define   MAX_NUMBER_FILES     20
/*==================================================================================================
                                             ENUMS
==================================================================================================*/
/* List all the test Id / Case  */
typedef enum
{
    NOMINAL_FUNCTIONALITY,
    RELOCATABILITY,
    RE_ENTRANCE,

    PRE_EMPTION,
    ENDURANCE,
    LOAD
} g723_1_decoder_testcases_t;


/*==================================================================================================
                                 STRUCTURES AND OTHER TYPEDEFS
==================================================================================================*/

typedef struct
{
  char *file_name_input;
  char *file_name_output;
  char *file_name_compare;

  unsigned char *buf_pr;
  int buf_byte;
  int Index;
  int numberframe;

  sG723EEncoderConfigType psEncConfig;
  pthread_t tid;   /* Thread id */

} g732_encoder_thread_t;


/*==================================================================================================
                                 GLOBAL VARIABLE DECLARATIONS
==================================================================================================*/
  /* Global thread structures */

/*==================================================================================================
                                     FUNCTION PROTOTYPES
==================================================================================================*/
int VT_g723_1_encoder_setup();
int VT_g723_1_encoder_cleanup();
int VT_g723_1_encoder_test( int ,int, char*,int);





#ifdef __cplusplus
}
#endif
#endif  // g723_1_encoder_TEST_H //
