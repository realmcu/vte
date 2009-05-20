/*================================================================================================*/
/**
    @file   amr_encoder_test_X.h

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

#ifndef amr_encoder_TEST_H
#define amr_encoder_TEST_H

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
#include "nb_amr_enc_api.h"
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
#define EXHAUSTIVE_ENCODE		0
#define ENDURANCE				1
#define REENTRANCE				2
	#define USING_THREAD		1
	#define USING_PROCESS		0
#define PREEMPTIVITY			3
#define LOAD_TEST				4

#define ITERATIONS				10

#define ENCODER_THREAD 2		/* max number of decoding thread */
#define LENGTH					128


/*==================================================================================================
                                             ENUMS
==================================================================================================*/

/*==================================================================================================
                                 STRUCTURES AND OTHER TYPEDEFS
==================================================================================================*/

/* structure that groups all variables needed for each decoding thread */
typedef struct
{
	unsigned char *file_mode_info;
	unsigned char *input_pcm_file_name;
	unsigned char *output_amr_file_name;
	unsigned char *reference_file;
	unsigned int use_mode_file;
	unsigned char * ps8APPEModeStr;
	unsigned int u32APPEInstanceID;
	unsigned int s16APPEDtxFlag;


	/* add here all variable needed to configure the encoder */

} amr_encoder_thread;


/*==================================================================================================
                                 GLOBAL VARIABLE DECLARATIONS
==================================================================================================*/

/* Global thread structures */
amr_encoder_thread thread_encoder[ENCODER_THREAD];

/* These array of data/pointers are datas that each threads need and
that will be also needed in the callback
They are declared statically since the number of decoding thread
is hard coded in this test application */

/*==================================================================================================
                                     FUNCTION PROTOTYPES
==================================================================================================*/
int VT_amr_encoder_setup();
int VT_amr_encoder_cleanup();
int VT_amr_encoder_test(int Id, int Case, int Iter);



#ifdef __cplusplus
}
#endif

#endif  // amr_encoder_TEST_H //
