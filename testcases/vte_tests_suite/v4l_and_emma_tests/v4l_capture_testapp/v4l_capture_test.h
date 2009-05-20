/*
  Copyright (C) 2004, Freescale Semiconductor, Inc. All Rights Reserved
  THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
  BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
  Freescale Semiconductor, Inc.
*/

/**
@file   v4l_capture_test.h

@brief  V4L capture test scenario C header.

Description of the file

@par Portability: ARM GCC

*/

/*======================== REVISION HISTORY ==================================

Author (core ID)      Date         CR Number    Description of Changes
-------------------   ----------   ----------   ------------------------------
Kardakov Dmitriy/ID   09/11/06     TLSbo76802   Initial version
=============================================================================*/

#ifndef __V4L_CAPTURE_TEST_H
#define __V4L_CAPTURE_TEST_H

#ifdef __cplusplus
extern "C"{
#endif

/*======================== INCLUDE FILES ====================================*/


#include <ctype.h>              /* toupper()    */
#include <sys/types.h>          /* open()       */
#include <sys/stat.h>           /* open()       */
#include <fcntl.h>              /* open()       */
#include <sys/ioctl.h>          /* ioctl()      */
#include <unistd.h>             /* close()      */
#include <stdio.h>              /* sscanf() & perror() */

#include <stdlib.h>             /* atoi()       */
#include <asm/types.h>          /* for videodev2.h */
#include <linux/compiler.h>     /* for videodev2.h */
#include <linux/videodev2.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <math.h>
#include <string.h>
#include <malloc.h>


/*======================== CONSTANTS ========================================*/


/*======================== DEFINES AND MACROS ===============================*/

#if !defined(TRUE)
#define TRUE    1
#endif

#if !defined(FALSE)
#define FALSE   0
#endif

/*======================== ENUMS ============================================*/
typedef enum
{
        PRP_VF = 1,
        PRP_ENC_ON_D,
        PRP_ENC_TO_F,
        PRP_VID_TO_F,
} eCases;

/*======================== STRUCTURES AND OTHER TYPEDEFS ====================*/
typedef struct
{
        const char * mV4LDevice;
        const char * mOutputDevice;
        char * mOutputFile;
        int mWidth;
        int mHeight;
        int mCount;
        int mCaseNum;
        int mRotationMode;
        int mOutputFormat;
        int mCrop;
        struct v4l2_rect mCropRect;
        int mRotation;
        int mVerbose;
        const char * mPixFormat;
        int mOverlayType;
        int mFrameRate;
        int mDuration;

} sV4LTestConfig;
/*======================== GLOBAL VARIABLE DECLARATIONS =====================*/

extern sV4LTestConfig gV4LTestConfig;

/*======================== FUNCTION PROTOTYPES ==============================*/
void cleanup(void);

int VT_v4l_capture_setup(void);
int VT_v4l_capture_cleanup(void);
int VT_v4l_capture_test(void);

#ifdef __cplusplus
}
#endif

#endif  // V4L_CAPTURE_TEST_H //
