/*================================================================================================*/
/**
    @file   aaclc_decoder_test.h

    @brief  C header file of the AAC LC decoder test application.
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
Igor Semenchukov/smng001c    26/10/2004     TLSbo43521   Initial version
Igor Semenchukov/smng001c    04/02/2005     TLSbo47179   Changed include directives (lib headers)
Igor Semenchukov/smng001c    29/03/2005     TLSbo48795   Updated due to changes in the codec's code
D.Simakov/smkd001c           22/10/2005     TLSbo57009   dif32 is used for bit-matching
==================================================================================================*/

#ifndef AACLC_DECODER_TEST_H
#define AACLC_DECODER_TEST_H

#ifdef __cplusplus
extern "C"{
#endif

/*==================================================================================================
                                         INCLUDE FILES
==================================================================================================*/
#include <aacd_dec_interface.h> /* Chans */
#include <aac_fix_defs.h>
#include <aac_fix_types.h>
#include <aac_fix_proto.h>
#include <aacd_tables.h>       /* May be removed soon. Needed for tables array aptable[] */

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
#define DEFAULT_ITERATIONS 10
#define CHNL_DELIM         '+'
#ifndef MAX_STR_LEN
    #define MAX_STR_LEN 80
#endif

/*==================================================================================================
                                             ENUMS
==================================================================================================*/
/* Different test cases in the single application */
typedef enum
{
    NOMINAL_FUNCTIONALITY = 0,  /* Nominal encoding/decoding test                     */
    PREEMPTION,                 /* Test in the preemptive environment                 */
    REENTRANCE,                 /* Reentrance capabilities test                       */
    RELOCATABILITY,             /* Decoder code relocatability test                   */
    LOAD_ENVIRONMENT,		/* Working in the load environment                    */
    ENDURANCE,			/* Endurance decoder test                             */
    ADTS_STREAMS,               /* ADTS streams resync feature test                   */
} AACLC_DECODER_TESTCASES;

/*==================================================================================================
                                 STRUCTURES AND OTHER TYPEDEFS
==================================================================================================*/

/* Linked list; used for holding input, output and reference file names */

typedef struct flist
{
    char inp_fname[MAX_STR_LEN];
    char out_fname[MAX_STR_LEN];
    char ref_fname[Chans][MAX_STR_LEN];
    int  ch;
    struct flist *next;

} flist_t;

/*==================================================================================================
                                 GLOBAL VARIABLE DECLARATIONS
==================================================================================================*/


/*==================================================================================================
                                     FUNCTION PROTOTYPES
==================================================================================================*/
int VT_aaclc_decoder_setup();
int VT_aaclc_decoder_cleanup();
int VT_aaclc_decoder_test(int testcase, int iter, char *listfile);

#ifdef __cplusplus
}
#endif

#endif  /* AACLC_DECODER_TEST_H */
