/*================================================================================================*/
/**
    @file   bmp_decoder_test.h

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
D.Simakov / smkd001c         12/07/2004     TLSbo40263   Initial version
D.Simakov / smkd001c         02/03/2005     TLSbo47117   Updated
D.Simakov / smkd001c         25/03/2005     TLSbo48683   Improved
D.Simakov / smkd001c         02/06/2005     TLSbo50899  Robustness test case was improved
==================================================================================================*/

#ifndef bmp_decoder_TEST_H
#define bmp_decoder_TEST_H

#ifdef __cplusplus
extern "C"{
#endif

/*==================================================================================================
                                         INCLUDE FILES
==================================================================================================*/
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <asm/ioctls.h>
#include <linux/fb.h>		// for frame buffer operations
#include <sys/types.h>  // fork usage for reentrance test
#include <unistd.h> // fork usage for reentrance test
#include <pthread.h> // fork usage for reentrance test
#include <sys/time.h>		// timer usage for preemptivity
#include <signal.h>
#include <stdio.h>
#include <asm/mman.h>
#include <math.h>
#include <assert.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <bmp_interface.h>

/*==================================================================================================
                                           CONSTANTS
==================================================================================================*/

/*==================================================================================================
                                       DEFINES AND MACROS
==================================================================================================*/

/* List all the test Id / Case	*/
//#define EXHAUSTIVE_DECODE		0
//#define ENDURANCE				1
//#define ROBUSTNESS_1    2
//#define ROBUSTNESS_2            3
//#define REENTRANCE				4
//#define PREEMPTIVITY			5
//#define LOAD_TEST				6

#define ITERATIONS				10

#define INPUT_BUF_SIZE  2048 /* size of the input buffer for each decoding thread */
#define DECODER_THREAD 	2	 /* max number of decoding thread */


/*==================================================================================================
                                             ENUMS
==================================================================================================*/
/** bmp_decoder_EX type */
typedef enum
{
    NOMINAL_FUNCTIONALITY = 0,
    ENDURANCE,
    ROBUSTNESS_1, /* some wrong file */
    ROBUSTNESS_2, /* some wrong file with the bmp hdr */
    RE_ENTRANCE,
    PRE_EMPTION,
    LOAD_TEST
} bmp_decoder_testcases_t;


/*==================================================================================================
                                 STRUCTURES AND OTHER TYPEDEFS
==================================================================================================*/

/* structure that groups all variables needed for each decoding thread */
typedef struct
{
	unsigned int instance_id;
  unsigned char * input_file_name;
	unsigned char * output_file_name;
    unsigned char * ref_fname;
	BMP_Decoder_Params dec_param;
	int endurance;
	unsigned int offsetYDualScreen;
    int width, height;
} bmp_decoder_thread;

/*==================================================================================================
                                 GLOBAL VARIABLE DECLARATIONS
==================================================================================================*/

/* Global thread structures */
bmp_decoder_thread thread_decoder[DECODER_THREAD];
pthread_t tid[DECODER_THREAD];

/* These array of data/pointers are datas that each threads need and
that will be also needed in the callback
They are declared statically since the number of decoding thread
is hard coded in this test application */
BMP_UINT8 BMP_input_buffer[DECODER_THREAD][INPUT_BUF_SIZE];
BMP_Decoder_Object *p_dec_obj[DECODER_THREAD];
FILE * input_file[DECODER_THREAD];
FILE * output_file[DECODER_THREAD];
FILE * ref_fstream[DECODER_THREAD];

/*==================================================================================================
                                     FUNCTION PROTOTYPES
==================================================================================================*/
int VT_bmp_decoder_setup();
int VT_bmp_decoder_cleanup();
int VT_bmp_decoder_test(int test_case, int iter, const char * cfg_file);



#ifdef __cplusplus
}
#endif

#endif  // bmp_decoder_TEST_H //
