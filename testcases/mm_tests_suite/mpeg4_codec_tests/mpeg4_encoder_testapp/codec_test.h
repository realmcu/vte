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
I.Inkina              25/11/2004   TLSbo45065    Initial version
D.Simakov             26/02/2006   TLSbo66281    Centralization of common features.
=============================================================================*/

#ifndef __CODEC_TEST_H__
#define __CODEC_TEST_H__


/*==================================================================================================
                                         INCLUDE FILES
==================================================================================================*/

#include <mpeg4_enc_api.h>
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
        #define MAX_THREADS        2
#endif

#define DM__() {printf("%s:%d %s()\n", __FILE__, __LINE__, __FUNCTION__); fflush(stdout);}

#define OUTP_BUF_SZ 101376

/*==================================================================================================
                                             ENUMS
==================================================================================================*/

/* Test cases. */
enum 
{
        NOMINAL_FUNCTIONALITY, 
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
        
                
        int              mSrcFps;
        int              mEncFps;
        int              mStartFrameNum;
        int              mIPeriod;
        int              mShortVideoHdr;
        int              mH263NoShortVideoHeader;
        int              mNumForcedIntra;
        int              mVideoPacketEnable;
        int              mDataPartitioningEnable;
        int              mUseRVLC;
        int              mResyncMarkSpacing;
        int              mResyncMarkMBSpacing;
        int              mIntraDcVlcThresh;
        int              mIntraAcPredFlag;
        int              mDelayFactor;
        int              mQualityTradeoff;
        int              mLevel;
        int              mIntraVopQuant;
        int              mInterVopQuant;
        int              mTargetBitRate;

        int              mIsReadyForBitMatching;     

} sHandlerParams;

/* Codec handler. */
typedef struct
{                          
        /****************************************/
        /* Input and output (streams, buffers). */
        /****************************************/

        struct hKevFile       * mpInpKev;
        FILE                  * mpOutStream;                
        unsigned char           mpOutBuffer[OUTP_BUF_SZ];        
        unsigned char         * mpYBuf;
        unsigned char         * mpCrBuf;
        unsigned char         * mpCbBuf;
        size_t                  mYBufSz;
        size_t                  mCrCbBufSz;
        

        /********************/
        /* Encoder's stuff. */
        /********************/

        sMpeg4EncoderConfig     mEncObject;                
        eMPEG4ERetType          mLastCodecError;
        int                     mNumMB;
        unsigned char         * mpSetRandomIntraMB;

        
        /***************/
        /* Other data. */
        /***************/
        
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
