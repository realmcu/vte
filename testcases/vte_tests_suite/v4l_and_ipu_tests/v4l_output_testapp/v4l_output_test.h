/***
**Copyright 2005-2009 Freescale Semiconductor, Inc. All Rights Reserved.
**
**The code contained herein is licensed under the GNU General Public
**License. You may obtain a copy of the GNU General Public License
**Version 2 or later at the following locations:
**
**http://www.opensource.org/licenses/gpl-license.html
**http://www.gnu.org/copyleft/gpl.html
**/
/*================================================================================================*/
/**
    @file   v4l_output_test.h

    @brief  Test scenario C header template.
====================================================================================================
Revision History:
                            Modification     Tracking
Author (core ID)                Date          Number    Description of Changes
-------------------------   ------------    ----------  -------------------------------------------
Smirnov Artyom                10/04/05      TLSbo49894  BRIEF description of changes made 
Bezrukov.S/SBAZR1C            08/17/2005    TLSbo53919  Remove the Brightness feature
KHOROSHEV.D		      09/29/2005    TLSbo55077  Review version

==================================================================================================*/

#ifndef _V4L_OUTPUT_TEST_H_
#define _V4L_OUTPUT_TEST_H_

#ifdef __cplusplus
extern "C"{ 
#endif

/*==================================================================================================
                                        INCLUDE FILES
==================================================================================================*/

#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
/*
#include <linux/compiler.h>
*/
#include <linux/videodev.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

/* Harness Specific Include Files. */
#include "test.h"
#include "usctest.h"

/*==================================================================================================
                                            CONSTANTS
==================================================================================================*/


/*==================================================================================================
                                        DEFINES AND MACROS
==================================================================================================*/

#define V4L_OUTPUT_DEVICE "/dev/video16"
#define OUTPUT_FILE       "v4l_output_file"
#define INPUT_FILE        "output_BGR24"

/* V4L private CIDs */

#define V4L2_CID_NORMAL    0
#define V4L2_CID_VERFLIP   1
#define V4L2_CID_HORFLIP   2
#define V4L2_CID_ROT180    3
#define V4L2_CID_ROT90CW   4
#define V4L2_CID_ROT90CWVF 5
#define V4L2_CID_ROT90CWHF 6
#define V4L2_CID_ROT90CCW  7

#define MAX_BUFF_NUM       5

/* 
* Dump file structure:
* 
*          Data offset | 
* Content -------------+---------------------------------------------- 
*        0x0000-0x000F | Image format as C string: 
*                      | - "RGB565" 
*                      | - "BGR24" 
*                      | - "RGB24" 
*                      | - "BGR32" 
*                      | - "RGB32" 
*                      | - "YUV422P" 
*                      | - "YUV420" 
*        0x0010-0x001F | Image X size as C string 
*        0x0020-0x002F | Image Y size as C string 
*        0x0030-0x00FF | <RESERVED_AREA> 
*        0x0100-0xXXXX | Raw data to/from IPU, V4L(2) or FB buffers */

#define FIELDS_LENGTH            0x0010

#define FMT_OFFSET               0
#define X_SIZE_OFFSET            0x0010
#define Y_SIZE_OFFSET            0x0020
#define RESERVED_AREA_OFFSET     0x0030
#define RAW_DATA_OFFSET          0x0100

#ifdef CONFIG_SDC       /* SDC */
#define MXCFB_SCREEN_WIDTH      240
#define MXCFB_SCREEN_HEIGHT     320
#endif

#ifdef CONFIG_ADC       /* ADC */
#define MXCFB_SCREEN_WIDTH      176
#define MXCFB_SCREEN_HEIGHT     220
#endif


/*==================================================================================================
                                            ENUMS
==================================================================================================*/


/*==================================================================================================
                                STRUCTURES AND OTHER TYPEDEFS
==================================================================================================*/
/* 
* typedef unsigned char u8; typedef unsigned short u16; typedef unsigned long u32; */

typedef struct
{
        unsigned char *start;
        size_t  offset;
        unsigned int length;

} video_buffer;

typedef struct
{
        char   *v4l_dev_file;
        char   *input_file;
        char   *output_file;
        char   *input_fmt;
        char   *output_fmt;
        int     nb_rot;
        int     crop_on;
        int     nb_buffers;
        int     x_ratio;
        int     y_ratio;
        int     x_size_src;     /* input frame size */
        int     y_size_src;
        int     x_size_dst;     /* output frame size */
        int     y_size_dst;
	int     x_offset;
	int     y_offset;
        int     x_size_crop;    /* cropping rectangle */
        int     y_size_crop;
        int     frame_size;
        int     test_type;      /* type of operation */

} params;


typedef struct
{
        char    image_fmt[FIELDS_LENGTH];
        char    image_width[FIELDS_LENGTH];
        char    image_height[FIELDS_LENGTH];
        char    reserved_area[RAW_DATA_OFFSET - RESERVED_AREA_OFFSET];

} dump_file_header;

/*==================================================================================================
                                GLOBAL VARIABLE DECLARATIONS
==================================================================================================*/


/*==================================================================================================
                                    FUNCTION PROTOTYPES
==================================================================================================*/

int     VT_v4l_output_setup(void);
int     VT_v4l_output_cleanup(void);
int     VT_v4l_output_test(void);

#ifdef __cplusplus
} 
#endif

#endif                          /* _V4L_OUTPUT_TEST_H_ */
