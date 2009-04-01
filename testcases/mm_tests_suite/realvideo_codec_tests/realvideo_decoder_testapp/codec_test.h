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
D.Simakov/smkd001c    10/06/2005   TLSbo51185	Initial version		
D.Simakov/smkd001c    24/06/2005   TLSbo51185	Working version
D.Simakov/smkd001c    30/06/2005   TLSbo52235   Video desplay was added
D.Simakov/smkd001c    22/07/2005   TLSbo52363   Relocatability test caes was added
D.Simakov             15/05/2006   TLSbo66278   Phase2
=============================================================================*/

#ifndef __CODEC_TEST_H__
#define __CODEC_TEST_H__


/*==================================================================================================
                                         INCLUDE FILES
==================================================================================================*/

#include <realvdo.h> 
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

#define DEFAULT_ITERATIONS         10
#ifndef MAX_THREADS
        #define MAX_THREADS                 4
#endif

#define DM__() {printf("%s:%d %s()\n", __FILE__, __LINE__, __FUNCTION__); fflush(stdout);}

#define NBINS   	  20
#define DEF_NSAMPLES 512

/*==================================================================================================
                                             ENUMS
==================================================================================================*/

/* Test cases. */
enum 
{
        NOMINAL_FUNCTIONALITY, 
        ROBUSTNESS,
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
        int              mDelay;
        int              mDisableLCD;
} sTestappConfig;

/* Set of parameters for each codec handler. */
typedef struct 
{
        unsigned int     mNoEntry;    
        char             mInpFileName[MAX_STR_LEN];
        char             mOutFileName[MAX_STR_LEN];
        char             mRefFileName[MAX_STR_LEN]; 
                
        int              mWidth;
        int              mHeight;
        RV_Boolean       mSmoothingPostfilter;
        RV_Boolean       mIsRV8;
        RV_Boolean       mLatencyMode;        
        U32              mPercentPacketLoss;     /* 0 .. 100 */        
        int              mFps;
        
        int              mIsReadyForBitMatching;     

} sHandlerParams;

/* Codec handler. */
typedef struct
{                          
        /****************************************/
        /* Input and output (streams, buffers). */
        /****************************************/
        
        FILE                  * mpInpStream;  
        FILE                  * mpOutStream;         

        unsigned char         * mpInputBuf;
        unsigned char         * mpInputBufPtr;
        size_t                  mInputBufSz;
        size_t                  mInputBufValidSz;

        unsigned char         * mpOutputBuf;
        unsigned char         * mpOutputBufAlignedPtr;
        size_t                  mOutputBufSz;
        

        /********************/
        /* Decoder's stuff. */
        /********************/

        RV_Decoder_config       mDecConfig;
        struct RVDecoder        mRVDecoder;        
        int                     mLastCodecError; 
        unsigned long           mFramesCount;

        
        /***************/
        /* Other data. */
        /***************/

        struct RV_Image_Format  mImageFormat;
        struct RV_Image         mCurrentImage;
        struct RV_Image         mDecompressedImage;
        int                     mFrameNumber;

        int                     mFpsDelay;
        int                     mCurrentDelay;  // in microseconds 
        unsigned char         * mpRgbFramebuffer;
        
        unsigned long           mIndex;
        sHandlerParams        * mpParams;                    
        pthread_t               mThreadID;                  
        int                     mLtpRetval;      
                
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
