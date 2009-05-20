/*
  Copyright (C) 2004, Freescale Semiconductor, Inc. All Rights Reserved
  THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
  BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
  Freescale Semiconductor, Inc.
*/

/**
@file imelody_conv_test.h

@brief VTE C header imelody to midi converter test case

Description of the file

@par Portability:
        Indicate if this module is portable to other compilers or platforms.
        If not, indicate specific reasons why is it not portable.
*/

/*======================== REVISION HISTORY ==================================

Author (core ID)      Date         CR Number    Description of Changes
-------------------   ----------   ----------   ------------------------------
S.ZAVJALOV/zvjs001c   19/10/2004   TLSbo47116   Initial version
D.Simakov/smkd001c    07/04/2005   TLSbo47116   Imroved, endurance, load and
                                                robustness test cases were added.
D.Simakov/smkd001c    24/10/2005   TLSbo57009   Update
=============================================================================*/

#ifndef IMELODY_CONV_TEST_H
#define IMELODY_CONV_TEST_H

#ifdef __cplusplus
extern "C"{
#endif

/*======================== INCLUDE FILES ====================================*/

#include "imelody_conv_api.h"

/*======================== CONSTANTS ========================================*/


/*======================== DEFINES AND MACROS ===============================*/

#define MAX_STR_LEN 80

#if !defined(TRUE) && !defined(FALSE)
#define TRUE  1
#define FALSE 0
#endif

#define IMYCONV_INP_BUF_SIZE 20
#define IMYCONV_OUT_BUF_SIZE 20

#define MAX_iMY_CONV_THREADS 2

/*======================== ENUMS ============================================*/

/** imelody_conv_EX type */
typedef enum
{
    imelody_conv_EX_0 = 0,   /**< Example stuff 0. */
    imelody_conv_EX_1        /**< Example stuff 1. */
} imelody_conv_EX_T;

typedef enum
{
    iMY_NOMINAL,
    iMY_REENTER,
    iMY_ENDURANCE,
    iMY_LOAD,
    iMY_ROBUSTNESS
}iMY_TEST_CASES;

/*======================== STRUCTURES AND OTHER TYPEDEFS ====================*/

typedef struct
{
    char name[MAX_STR_LEN];
    IMYCONV_FILE *ptr;
}filehandle;

typedef struct
{
    int			number;
    filehandle          inp_file,
                        out_file;
    pthread_t           tid;
    int                 th_err;
    sIMYConvConfigType *psImyConfig;
    sIMYConvConfigType sImyConfig;
}imy_thread;

typedef struct flist
{
    char inp_fname[MAX_STR_LEN];
    char out_fname[MAX_STR_LEN];
    char ref_fname[MAX_STR_LEN];
    struct flist *next;
} flist_t;

/*======================== GLOBAL VARIABLE DECLARATIONS =====================*/


/*======================== FUNCTION PROTOTYPES ==============================*/

void cleanup();

int VT_imelody_conv_setup();
int VT_imelody_conv_cleanup();

int VT_imelody_conv_test(int test_num, char *cfg_file_name);



#ifdef __cplusplus
}
#endif

#endif  // IMELODY_CONV_TEST_H //

