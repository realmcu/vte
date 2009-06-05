/*
  Copyright (C) 2004, Freescale Semiconductor, Inc. All Rights Reserved
  THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
  BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
  Freescale Semiconductor, Inc.
*/

/**
@file   mpeg4_decoder_test.h


@brief VTE C header template

Description of the file

@par Portability:
        Indicate if this module is portable to other compilers or platforms.
        If not, indicate specific reasons why is it not portable.
*/

/*======================== REVISION HISTORY ==================================

Author (core ID)      Date         CR Number    Description of Changes
-------------------   ----------   ----------   ------------------------------
Filinova Natalya      15/02/2005   TLSbo47115    BRIEF desc. of changes
Filinova Natalya      28/02/2005   TLSbo47115    BRIEF desc. of changes
Delaspre/rc149c                    TLSbo47115    update copyrights with Freescale
D.Simakov/smkd001c    01/04/2005   TLSbo49126    Updates according to the new API.
D.Simakov/smkd001c    21/07/2005   TLSbo52629    Relocatability test case was added 
D.Simakov/smkd001c    16/12/2005   TLSbo59667    Advanced decode w skip fixed 
=============================================================================*/

#ifndef MPEG4_DECODER_TEST_H
#define MPEG4_DECODER_TEST_H

#ifdef __cplusplus                            
extern "C"{
#endif

/*======================== INCLUDE FILES ====================================*/

/*======================== CONSTANTS ========================================*/



/*======================== DEFINES AND MACROS ===============================*/

#if !defined(TRUE) && !defined(FALSE)
#define TRUE  1
#define FALSE 0
#endif


#define DEFAULT_ITERATIONS        10                   /* default 10 iterations for decode process     */
#define BIT_BUFFER_SIZE           1024	               /* default size of buffer for reading bitstream */												
#define DEFAULT_START_FRAME       1                    /* default skipping from 1st frame              */
#define DEFAULT_NUM_TO_SKIP       5		       /* default 5 frames to skip                     */
#define DEFAULT_DISPLAY_TIME      0                    /* default no displaing decode time             */
#define DEFAULT_INPUT_BITSTREAM   "foreman_qcif.bits"  /* default input bitstream name                 */
#define DEFAULT_DELAY             0                    /* default no delay between showed frames       */
#define DEFAULT_COMM              0                    /* default no comments during decode process    */
#define DEFAULT_PATH              "./input/"            /* default path to input directory              */
#define FRAMEBUFFER               1                    /* default frame buffer is supported            */


#define ERROR -1
#define SUCCESS 0


/*======================== ENUMS ============================================*/
/** Different test cases in the single application */
typedef enum 
{
    NOMINAL_FUNCTIONALITY = 0,  /**< Nominal decoding */
    ADVANCED_FUNCTIONALITY,     /**< Decoding with skipping frames**/
    ENDURANCE,			/**< Endurance test */
    RE_ENTRANCE,		/**< Re-entrance */
    PRE_EMPTION,		/**< Pre-emption */
    LOAD_ENVIROUNMENT,		/**< Working in a load envirounment */
    ROBUSTNESS,                 /**< React to a bad input bit-stream, i.e. pdf document */
    RELOCATABILITY,
} MPEG4_DECODER_TESTCASES;

typedef enum
{
  WITHOUT_OUTPUT = 0,           /**< without forming output of decoding */
  FILES_OUTPUT = 1,             /**< output as files */
  VIDEO_OUTPUT = 2,             /**< output as showing decode frames on LCD */
  FILES_AND_VIDEO = 3,          /**< output as files and showing on LCD */
} OUTPUT;   /* output form */

typedef enum
{
  SKIP_TO_INTRA ,               /**< skip mode skipping to INTRA frame */
  SKIP_FRAMES,                  /**< skip mode skipping specified number of frames */
} SKIP_MODE;  /* skip mode */

/*======================== STRUCTURES AND OTHER TYPEDEFS ====================*/


/*======================== GLOBAL VARIABLE DECLARATIONS =====================*/


/*======================== FUNCTION PROTOTYPES ==============================*/
int VT_mpeg4_decoder_setup();
int VT_mpeg4_decoder_cleanup();
int VT_mpeg4_decoder_test( int testcase, int iter, int output, 
                           int skip_mode, int start_frame, int number, 
			   int time, char* input_fname, int delay,int comm,char* dir);

#ifdef __cplusplus
}
#endif

#endif  // MPEG4_DECODER_TEST_H //
