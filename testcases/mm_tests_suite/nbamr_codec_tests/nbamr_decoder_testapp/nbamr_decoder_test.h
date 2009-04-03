/*
  Copyright (C) 2004, Freescale Semiconductor, Inc. All Rights Reserved
  THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
  BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
  Freescale Semiconductor, Inc.
*/

/**
@file nbamr_decoder_test.h

@brief VTE C header file of the NB AMR decoder test application.

Description of the file

@par Portability:
        Indicate if this module is portable to other compilers or platforms.
        If not, indicate specific reasons why is it not portable.
*/

/*======================== REVISION HISTORY ==================================

Author (core ID)           Date         CR Number    Description of Changes
-----------------------    ----------   ----------   -------------------------
I. Semenchukov/smng001c    18/01/2005   TLSbo46857   Initial version

=============================================================================*/

#ifndef NBAMR_DECODER_TEST_H
#define NBAMR_DECODER_TEST_H

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
    NOMINAL_FUNCTIONALITY = 0,  /* Nominal decoding test                     */
    REENTRANCE,                 /* Reentrance capabilities test              */
    RELOCATABILITY,             /* Decoder code relocatability test          */
    ROBUSTNESS,                 /* React to a bad input bitstream            */
} NBAMR_DECODER_TESTCASES;

/*======================== STRUCTURES AND OTHER TYPEDEFS ====================*/
typedef struct flist
{
    char inp_fname[MAX_STR_LEN];
    char out_fname[MAX_STR_LEN];
    char ref_fname[MAX_STR_LEN];
    int  bitstr_format;
    struct flist *next;

} flist_t;

/*======================== GLOBAL VARIABLE DECLARATIONS =====================*/


/*======================== FUNCTION PROTOTYPES ==============================*/

void cleanup();

int VT_nbamr_decoder_setup();
int VT_nbamr_decoder_cleanup();
int VT_nbamr_decoder_test(int testcase, char *listfile);

#ifdef __cplusplus
}
#endif

#endif  /* NBAMR_DECODER_TEST_H */
