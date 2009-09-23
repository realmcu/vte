/***
**Copyright 2004-2009 Freescale Semiconductor, Inc. All Rights Reserved.
**
**The code contained herein is licensed under the GNU General Public
**License. You may obtain a copy of the GNU General Public License
**Version 2 or later at the following locations:
**
**http://www.opensource.org/licenses/gpl-license.html
**http://www.gnu.org/copyleft/gpl.html
**/

/**
@file   v4l_capture_test.h

@brief  V4L capture test scenario C header.

Description of the file

@par Portability: ARM GCC

*/

/*======================== REVISION HISTORY ==================================

Author (core ID)      Date         CR Number    Description of Changes
-------------------   ----------   ----------   ------------------------------
A.Geniatov/gntl002    20/07/2004   TLSbo40898   Initial version
A.Geniatov/gntl002    27/09/2004   TLSbo40898   Change after review
Filinova Natalya      29/03/2005   TLSbo48417   Added enum cases.
                                                Moved several functions prototypes
                                                to v4l_capture_test.c.

Delaspre/rc149c       07/12/2004   TLSbo40142   update copyrights with Freescale
N.Filinova/nfili1c    28/09/2005   TLSbo54946   Change after review
N.Filinova/nfili1c    20/10/2005   TLSbo56683   Reinitializetion all cropping, resize etc to nominal value
N.Filinova/nfili1c   23/11/2005   TLSbo58746   Add the cropping restangle
D.Kazachkov/e1403c    15/03/2006   TLSbo63410	call VIDIOC_S_FBUF before call VIDIOC_OVERLAY

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
/*
#include <linux/compiler.h> 
*/
#include <linux/videodev2.h>
#include <linux/fb.h>
#include <sys/mman.h>
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
} eCases;

typedef enum
{
  eInCSI_IC_MEM = 0,
  eInCSI_MEM,
} eInputs;

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
#ifndef MAD_TEST_MODIFY
	 int mNeedAsk;
#endif
	 int mVerbose;
        const char * mPixFormat;
        int mOverlayType;
	int mFrameRate;
	int mIsBlock;
	int inputSrc;
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
