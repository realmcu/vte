/*================================================================================================*/
/**
    @file   amr_decoder_test_X.h

    @brief  Test scenario C header template.
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
F.GAFFIE/rb657c              21/07/2004     TLSbo40930  Initial version
L.DELASPRE/rc149c            18/10/2004     TLSbo43867  update with new API

==================================================================================================*/

#ifndef amr_decoder_TEST_H
#define amr_decoder_TEST_H

#ifdef __cplusplus
extern "C"{
#endif

/*==================================================================================================
                                         INCLUDE FILES
==================================================================================================*/
#include <stdlib.h>
#include <asm/fcntl.h>
#include <pthread.h>
#include <semaphore.h>
#include <asm/ioctls.h>
#include <linux/fb.h>		// for frame buffer operations
#include <sys/types.h> // fork usage for reentrance test
#include <unistd.h> // fork usage for reentrance test
#include <pthread.h> // fork usage for reentrance test
#include <sys/time.h>		// timer usage for preemptivity
#include <signal.h>
#include <stdio.h>
#include <asm/mman.h>
#include <math.h>

#include "TranslateGlobalSymbols.h"
#include "nb_amr_dec_api.h"
#include "nb_amr_globals.h"

#ifdef NB_AMR_MMSIO
#include "mms_format.h"
#endif
/*==================================================================================================
                                           CONSTANTS
==================================================================================================*/

/*==================================================================================================
                                       DEFINES AND MACROS
==================================================================================================*/

/* List all the test Id / Case	*/
#define EXHAUSTIVE_DECODE		0
#define ENDURANCE				1
#define ROBUSTNESS				2
#define REENTRANCE				3
	#define USING_THREAD		1
	#define USING_PROCESS		0
#define PREEMPTIVITY			4
#define LOAD_TEST				5

#define ITERATIONS				10

#define INPUT_BUF_SIZE  	2048	/* size of the input buffer for each decoding thread */
#define DECODER_THREAD 2		/* max number of decoding thread */
#define LENGTH					128

//#define MMS_FORMAT				0
//#define BITSTREAM_FORMAT		1



/*==================================================================================================
                                             ENUMS
==================================================================================================*/


/*==================================================================================================
                                 STRUCTURES AND OTHER TYPEDEFS
==================================================================================================*/

/* structure that groups all variables needed for each decoding thread */
typedef struct
{
	unsigned char *input_amr_file_name;
	unsigned char *output_pcm_file_name;
	unsigned char *reference_file;
	U32 u32APPDInstanceID;
	S32 s32RXFrameTypeMode;

} amr_decoder_thread;

/*==================================================================================================
                                 GLOBAL VARIABLE DECLARATIONS
==================================================================================================*/


amr_decoder_thread thread_decoder[DECODER_THREAD];


/*==================================================================================================
                                     FUNCTION PROTOTYPES
==================================================================================================*/
int VT_amr_decoder_setup();
int VT_amr_decoder_cleanup();
int VT_amr_decoder_test(int Id, int Case, int Iter);



#ifdef __cplusplus
}
#endif

#endif  // amr_decoder_TEST_H //
