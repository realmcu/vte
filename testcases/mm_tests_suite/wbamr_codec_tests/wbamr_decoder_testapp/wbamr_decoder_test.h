/*================================================================================================*/
/**
    @file   wbamr_decoder_test.h

    @brief  C header file of the WB AMR decoder test application.
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
Igor Semenchukov/smng001c    30/11/2004     TLSbo43523   Initial version
D.Simakov / smkd001c         08/06/2005     TLSbo50994   Reentrance test case was improved
==================================================================================================*/

#ifndef WBAMR_DECODER_TEST_H
#define WBAMR_DECODER_TEST_H

#ifdef __cplusplus
extern "C"{
#endif

/*==================================================================================================
                                         INCLUDE FILES
==================================================================================================*/
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>    /* pthread_...() functions */
#include <math.h>       /* sqrt()                  */
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
       

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
#ifndef MAX_STR_LEN
    #define MAX_STR_LEN 80
#endif

/*==================================================================================================
                                             ENUMS
==================================================================================================*/
/* Different test cases in the single application */
typedef enum
{
    NOMINAL_FUNCTIONALITY = 0,  /* Nominal decoding test                             */
    REENTRANCE,                 /* Reentrance capabilities test                      */
    RELOCATABILITY,             /* Decoder code relocatability test                  */
    ROBUSTNESS,                 /* React to a bad input bitstream, i.e. pdf document */
} WBAMR_DECODER_TESTCASES;

/*==================================================================================================
                                 STRUCTURES AND OTHER TYPEDEFS
==================================================================================================*/
typedef struct flist
{
    char inp_fname[MAX_STR_LEN];
    char out_fname[MAX_STR_LEN];
    char ref_fname[MAX_STR_LEN];
    struct flist *next;

} flist_t;

/*==================================================================================================
                                 GLOBAL VARIABLE DECLARATIONS
==================================================================================================*/


/*==================================================================================================
                                     FUNCTION PROTOTYPES
==================================================================================================*/
int VT_wbamr_decoder_setup();
int VT_wbamr_decoder_cleanup();
int VT_wbamr_decoder_test(int testcase, char *listfile);

flist_t *mk_entry(const char *inp_fname, const char *out_fname, const char *ref_fname);
void    delete_list(flist_t *list);
int     read_cfg(const char *filename, flist_t **pplist);

#ifdef __cplusplus
}
#endif

#endif  /* WBAMR_DECODER_TEST_H */
