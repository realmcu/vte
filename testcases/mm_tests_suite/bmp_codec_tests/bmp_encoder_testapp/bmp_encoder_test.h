/*
  Copyright (C) 2004, Freescale Semiconductor, Inc. All Rights Reserved
  THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
  BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
  Freescale Semiconductor, Inc.
*/

/**
@file bmp_encoder_test.h

@brief VTE C header template

@par Portability:
        Indicate if this module is portable to other compilers or platforms.
        If not, indicate specific reasons why is it not portable.
*/

/*======================== REVISION HISTORY ==================================

Author (core ID)      Date         CR Number    Description of Changes
-------------------   ----------   ----------   ------------------------------
D.Simakov / smkd001c  26/07/2004   TLSbo40263   Initial version
D.Simakov / smkd001c  19/04/2005   TLSbo47117   Some new testcases were added
D.Simakov / smkd001c  27/07/2005   TLSbo52108   Color reduction test was added
=============================================================================*/


#ifndef bmp_ENCODER_TEST_H
#define bmp_ENCODER_TEST_H

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
#include <math.h>           // for sqrt in hogcpu
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <assert.h>
#include <sys/mman.h>

#include <bmp_enc_interface.h> // bmp encoder interface

/* Harness Specific Include Files. */
#include "test.h"
#include "usctest.h"

#include "stuff/fconf.h"

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
    RE_ENTRANCE,
    PRE_EMPTION,
    ENDURANCE,
    LOAD,
    COLOR_REDUCTION
} bmp_encoder_testcase_t;


/*==================================================================================================
                                 STRUCTURES AND OTHER TYPEDEFS
==================================================================================================*/
typedef struct {
    int test_case;
    int num_iter;
    const char * cfg_fname;
} testapp_config_t;

/*==================================================================================================
                                 GLOBAL VARIABLE DECLARATIONS
==================================================================================================*/
extern testapp_config_t testapp_cfg;

/*==================================================================================================
                                     FUNCTION PROTOTYPES
==================================================================================================*/
int VT_bmp_encoder_setup();
int VT_bmp_encoder_cleanup();
int VT_bmp_encoder_test();


#ifdef __cplusplus
}
#endif

#endif  // bmp_ENCODER_TEST_H //
