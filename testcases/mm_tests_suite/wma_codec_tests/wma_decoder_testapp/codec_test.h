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
D.Simakov/smkd001c    01/06/2005   TLSbo48591	Initial version			
D.Simakov/smkd001c    22/07/2005   TLSbo52361	Relocatability test case was added			
D.Simakov             19/05/2006   TLSbo66279   Phase2
=============================================================================*/

#ifndef __CODEC_TEST_H__
#define __CODEC_TEST_H__


/*==================================================================================================
                                         INCLUDE FILES
==================================================================================================*/

#include <wmad_dec_interface.h>
#include <wmad_tables.h>
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
                
        int              mInterleaveOutputs;
        
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

        size_t                 mInputFileSize;
        unsigned char        * mpInputBuffer;
        unsigned char        * mpInputBufferRef;
        unsigned long          mInputBufferIdx; 
        
        size_t                 mOutputBufferSize;
        short                * mpOutputBuffer;
        

        /********************/
        /* Decoder's stuff. */
        /********************/
        
        WMADDecoderConfig      mDecConfig;
        WMADDecoderParams      mDecParams;
        tWMAFileStatus         mLastCodecError;      
        
        
        /***************/
        /* Other data. */
        /***************/
        
        unsigned long           mFramesCount;        
        
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
