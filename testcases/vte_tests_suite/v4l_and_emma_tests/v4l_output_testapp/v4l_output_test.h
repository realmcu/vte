/*================================================================================================*/
/**
    @file   v4l_output_test.h

    @brief  Test scenario C header template.*/
/*==================================================================================================

    Copyright (C) 2005, Freescale Semiconductor, Inc. All Rights Reserved
    THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
    BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
    Freescale Semiconductor, Inc.
    
====================================================================================================
Revision History:
                            Modification     Tracking
Author (core ID)                Date          Number    Description of Changes
-------------------------   ------------    ----------  -------------------------------------------
Kardakov Dmitriy/ID         09/11/06        TLSbo76802  Initial version 
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

#include <linux/compiler.h>
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

#define V4L_OUTPUT_DEVICE "//dev//v4l/video16"
#define OUTPUT_FILE       "v4l_output_file"
#define INPUT_FILE        "output_BGR24"

/* V4L private CIDs */

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
*        0x0100-0xXXXX | Raw data to/from eMMA, V4L(2) or FB buffers */

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
        int     crop_on;
        int     nb_buffers;
        int     x_ratio;
        int     y_ratio;
        int     x_size_src;     /* input frame size */
        int     y_size_src;
        int     x_size_dst;     /* output frame size */
        int     y_size_dst;
        int     x_size_crop;    /* cropping rectangle */
        int     y_size_crop;
        int     frame_size;
        int     test_type;      /* type of operation */
        int     frame_rate;     /*frame rate of video reply*/ 

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
