/*
  Copyright (C) 2004, Freescale Semiconductor, Inc. All Rights Reserved
  THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
  BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
  Freescale Semiconductor, Inc.
*/

/**
@file nbamr_encoder_test.h

@brief VTE C header file of the NB AMR encoder test application.

Description of the file

@par Portability:
        Indicate if this module is portable to other compilers or platforms.
        If not, indicate specific reasons why is it not portable.
*/

/*======================== REVISION HISTORY ==================================

Author (core ID)           Date         CR Number    Description of Changes
-----------------------    ----------   ----------   -------------------------
I. Semenchukov/smng001c    24/01/2005   TLSbo46857   Initial version
I. Semenchukov/smng001c    23/03/2005   TLSbo48795   Made changes to reflect with new version

=============================================================================*/

#ifndef NBAMR_ENCODER_TEST_H
#define NBAMR_ENCODER_TEST_H

#ifdef __cplusplus
extern "C"{
#endif

/*======================== INCLUDE FILES ====================================*/


/*======================== CONSTANTS ========================================*/
#if !defined(TRUE) && !defined(FALSE)
#define TRUE  1
#define FALSE 0
#endif

/*======================== DEFINES AND MACROS ===============================*/
#ifndef MAX_STR_LEN
    #define MAX_STR_LEN 80
#endif

/*======================== ENUMS ============================================*/
/* Different test cases in the single application */

typedef enum
{
    NOMINAL_FUNCTIONALITY = 0, /* Nominal encoding test w/bit rate selection */
    REENTRANCE,                /* Reentrance capabilities test               */
    RELOCATABILITY,            /* Decoder code relocatability test           */
} NBAMR_ENCODER_TESTCASES;

/*======================== STRUCTURES AND OTHER TYPEDEFS ====================*/
typedef struct flist
{
    char inp_fname[MAX_STR_LEN];
    char out_fname[MAX_STR_LEN];
    char ref_fname[MAX_STR_LEN];
    char br_mode[MAX_STR_LEN];
    int  dtx;
    int  bitstr_format;
    struct flist *next;

} flist_t;

/*======================== GLOBAL VARIABLE DECLARATIONS =====================*/


/*======================== FUNCTION PROTOTYPES ==============================*/

void cleanup();

int VT_nbamr_encoder_setup();
int VT_nbamr_encoder_cleanup();
int VT_nbamr_encoder_test(int testcase, char *listfile);

#ifdef __cplusplus
}
#endif

#endif  /* NBAMR_ENCODER_TEST_H */
