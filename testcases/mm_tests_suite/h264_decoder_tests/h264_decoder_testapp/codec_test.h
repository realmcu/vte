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
D.Simakov/smkd001c    07/04/2005   TLSbo47116   Initial version
D.Simakov/smkd001c    22/07/2005   TLSbo52630   Relocatability test case was added
D.Simakov             26/02/2006   TLSbo61035   Centralization of common features.
=============================================================================*/

#ifndef __CODEC_TEST_H__
#define __CODEC_TEST_H__


/*==================================================================================================
                                         INCLUDE FILES
==================================================================================================*/

#include <avcd_dec_api.h>
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

#define CACHE_BUF_SZ 512

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
        
        
        int              mIsReadyForBitMatching;     

} sHandlerParams;

/* Codec handler. */
typedef struct
{                          
        /****************************************/
        /* Input and output (streams, buffers). */
        /****************************************/

        FILE                  * mpInpStream;
        struct hKevFile       * mpOutKev;        
        unsigned char         * mpInpBufferRef;        
        size_t                  mNumBytesInTheInput;
        unsigned char         * mpRGBFrame;
        size_t                  mRGBFrameSz;
        

        /********************/
        /* Decoder's stuff. */
        /********************/

        sAVCDecoderConfig       mDecObject;        
        eAVCDRetType            mLastCodecError;                        

        
        /***************/
        /* Other data. */
        /***************/
        
        unsigned long           mIndex;
        sHandlerParams        * mpParams;                    
        pthread_t               mThreadID;                  
        int                     mLtpRetval;    
        
        
        /**********/
        /* Cache. */
        /**********/
        
        struct
        {
                unsigned char mcBuf[CACHE_BUF_SZ];
                size_t        mcIndex;
                size_t        mcBytes;  
                int           mcEof;
        } mInputCache;
                
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
