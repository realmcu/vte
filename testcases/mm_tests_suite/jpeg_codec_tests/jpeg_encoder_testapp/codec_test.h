/*
  Copyright (C) 2004, Freescale Semiconductor, Inc. All Rights Reserved
  THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
  BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
  Freescale Semiconductor, Inc.
*/

/**
@file codec_test.h

@par Portability:
        ARM GCC
*/

/*======================== REVISION HISTORY ==================================

Author (core ID)      Date         CR Number    Description of Changes
-------------------   ----------   ----------   ------------------------------
F.GAFFIE/rb657c       03/05/2004   TLSbo39336   Initial version 
D.Simakov/smkd001c    07/02/2005   TLSbo47179   Bad dependancies in the mm tests 
                                                application build process
D.Simakov/smkd001c    25/10/2005   TLSbo59191   Improved 
D.Simakov/smkd001c    24/01/2006   TLSbo61035   Centralization of common features 
=============================================================================*/

#ifndef __CODEC_TEST_H__
#define __CODEC_TEST_H__


/*==================================================================================================
                                         INCLUDE FILES
==================================================================================================*/

#include "jpeg_enc_interface.h"
#include <util/llist.h>

/*==================================================================================================
                                       DEFINES AND MACROS
==================================================================================================*/

#if !defined(TRUE)
    #define TRUE 1
#endif
#if !defined(FALSE)
    #define FALSE 0
#endif

#define DEFAULT_ITERATIONS   10
#define MAX_THREADS           4
#define OUTP_BUFFER_SIZE 1024*2

#define DM__() {printf("%s:%d %s()\n", __FILE__, __LINE__, __FUNCTION__); fflush(stdout);}

/*==================================================================================================
                                             ENUMS
==================================================================================================*/

/* Test cases. */
enum 
{
        NOMINAL_FUNCTIONALITY, 
        THUMB_ENCODING,
        RELOCATABILITY,
        RE_ENTRANCE,
        PRE_EMPTION,    
        ENDURANCE,
        LOAD        
};

/*==================================================================================================
                                 STRUCTURES AND OTHER TYPEDEFS
==================================================================================================*/

/* Testapp configuration. */ 
typedef struct
{
        int              mTestCase;
        int              mNumIter;
        const char *     mConfigFilename;
        int              mVerbose;    
        int              mSlowBitMatching; 
        int              mFrameLevelApi; 
} sTestappConfig;

/* Set of parameters for each codec handler. */
typedef struct 
{
        unsigned int        mNoEntry;    
        char                mInpFileName[3][MAX_STR_LEN]; 
        char                mOutFileName[MAX_STR_LEN];
        char                mRefFileName[MAX_STR_LEN];
        int                 mIsReadyForBitMatching;     

        JPEG_ENC_YUV_FORMAT mYUVFormat;
        int                 mComprMethod; 
        JPEG_ENC_MODE       mMode;
        int                 mExifFlag;
        int                 mQuality;
        int                 mRestartMarkers;
        int                 mPrimaryImageWidth;
        int                 mPrimaryImageHeight;
        int                 mYWidth;
        int                 mYHeight;
        int                 mUWidth;
        int                 mUHeight;
        int                 mVWidth;
        int                 mVHeight; 
} sHandlerParams;

/* Codec handler. */
typedef struct
{  
        unsigned long    mIndex;
        
        sHandlerParams * mpParams;    
        FILE           * mpInputStream[3]; /* Non interleaved: Y, U, V. Interleaved: YUV, 0, 0. */
        FILE           * mpOutputStream;
        
        int              mLastCodecError; 
        
        unsigned char  * mpInpBuffer[3];
        size_t           mInpBufferSz[3];     
        
        unsigned char    mpOutBuffer[OUTP_BUFFER_SIZE];
        
        jpeg_enc_object  mEncObject;
        
        pthread_t        mThreadID;              
        int              mLtpRetval;
} sCodecHandler;


/*==================================================================================================
                                 GLOBAL VARIABLE DECLARATIONS
==================================================================================================*/

extern sTestappConfig  gTestappConfig;             /* defined in the codec_main.c */        


/*==================================================================================================
                                     FUNCTION PROTOTYPES
==================================================================================================*/


/* These functions must be implemented in the codec_test.c. Remainder. */
/*
int  TestEngine       ( sCodecHandler * pHandler );
void CleanupHandler   ( sCodecHandler * pHandler );
int  DoBitmatch       ( sCodecHandler * pHandler );
void PrintInfo        ( sCodecHandler * pHandler );
int  ExtraTestCases   ( void );
void MakeEntry        ( char ** entry, unsigned int nEntry );
*/

/* VTE */
int VT_codec_setup    ( void );
int VT_codec_cleanup  ( void );
int VT_codec_test     ( void );

#endif //__CODEC_TEST_H__
