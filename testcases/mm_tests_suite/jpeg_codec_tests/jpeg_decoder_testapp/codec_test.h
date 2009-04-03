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
F.GAFFIE/rb657c       3/05/2004    TLSbo39336    Initial version 
D.Simakov/smkd001c    7/02/2005    TLSbo47113    Update
D.Simakov/smkd001c    22/07/2005   TLSbo52627    Relocatability test caes was added
D.Simakov/smkd001c    05/12/2005   TLSbo59190    Bit-match check and load test were added
D.Simakov/smkd001c    31/01/2006   TLSbo61035    Centralization of common features   
=============================================================================*/

#ifndef __CODEC_TEST_H__
#define __CODEC_TEST_H__


/*==================================================================================================
                                         INCLUDE FILES
==================================================================================================*/

#include <jpeg_dec_interface.h>
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
#define MAX_THREADS                 4
#define INPUT_BUFFER_SZ          1024
#define INPUT_BUFFER_SZ_SUSP     1024
#define MAX_RAND_SIZE            1024


#define DM__() {printf("%s:%d %s()\n", __FILE__, __LINE__, __FUNCTION__); fflush(stdout);}

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
        int              mSuspension; 
        int              mDelay;
        int              mDisableLCD;
} sTestappConfig;

/* Set of parameters for each codec handler. */
typedef struct 
{
        unsigned int        mNoEntry;    
        char                mInpFileName[MAX_STR_LEN]; 
        char                mOutFileName[MAX_STR_LEN];
        char                mRefFileName[MAX_STR_LEN];
        int                 mIsReadyForBitMatching;     

        JPEGD_OUTPUT_FORMAT mOutFormat;
        JPEGD_DCT_METHOD    mDctMethod;
        int                 mOutWidth;
        int                 mOutHeight;
} sHandlerParams;

/* Codec handler. */
typedef struct
{  
        unsigned long        mIndex;
        

        /****************************************/
        /* Input and output (streams, buffers). */
        /****************************************/

        FILE               * mpInputStream;
        FILE               * mpOutputStream;  
        
        unsigned char      * mpOutBuffer[JPEGD_MAX_NUM_COMPS];  
        size_t               mOutBufferSz[JPEGD_MAX_NUM_COMPS];
        long                 mOutStrideWidth[JPEGD_MAX_NUM_COMPS];
        
        unsigned char        mpInpBuffer[INPUT_BUFFER_SZ];
                       

        /***************/
        /* Other data. */
        /***************/               

        sHandlerParams     * mpParams;
        JPEGD_Decoder_Object mDecObject;        
        int                  mLastCodecError;                
        int                  mDrawedLinesNo;     
        pthread_t            mThreadID;              
        int                  mLtpRetval;
        
        
        /********************/
        /* Suspension data. */
        /********************/
        
        long                 mTotalBytes;
        int                  mSuspType;
        int                  mSuspState;
        int                  mSuspFlag;
        int                  mSuspEof;
        unsigned int         mSavedMcuOffset;
        unsigned int         mSuspTargetBytes;        
                
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
