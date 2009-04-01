/*
  Copyright (C) 2004, Freescale Semiconductor, Inc. All Rights Reserved
  THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
  BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
  Freescale Semiconductor, Inc.
*/

/**
@file common_codec_test.h

@par Portability:
        ARM GCC
*/

/*======================== REVISION HISTORY ==================================

Author (core ID)      Date         CR Number    Description of Changes
-------------------   ----------   ----------   ------------------------------
D.Simakov/smkd001c    24/01/2006   TLSbo61035   Initial version
D.Simakov/smkd001c    08/02/2006   TLSbo61035   Util_ReadFile and Util_SwapBytes were added
D.Simakov             27/02/2006   TLSbo61035   Util_Realloc was added
=============================================================================*/

#ifndef __COMMON_CODEC_TEST_H__
#define __COMMON_CODEC_TEST_H__

/*==================================================================================================
                                        MACROS
==================================================================================================*/

#define TST_RESM(s,format,...) \
{\
    if( LOAD != gTestappConfig.mTestCase ) \
        pthread_mutex_lock( &gMutex );\
    tst_resm((s), (format), ##__VA_ARGS__);\
    if( LOAD != gTestappConfig.mTestCase ) \
        pthread_mutex_unlock( &gMutex );\
} 

#define NA "n/a"

/*==================================================================================================
                                        FUNCTION PROTOTYPES
==================================================================================================*/

/* Test core. */
int    RunCodec                 ( void * pContext );
int    TestEngine               ( sCodecHandler * pHandler );
void   ResetHandler             ( sCodecHandler * pHandler );
void   CleanupHandler           ( sCodecHandler * pHandler );
int    DoBitmatch               ( sCodecHandler * pHandler );
int    CompareFilesFast         ( sCodecHandler * pHandler, const char * fname1, const char * fname2 );
int    CompareFilesSlow         ( sCodecHandler * pHandler, const char * fname1, const char * fname2 );
int    DoFilesExist             ( const char * fname1, const char * fname2 );
int    IsBitmatchNeeded         ( sCodecHandler * pHandler );
void   HogCpu                   ( void );
void   PrintInfo                ( sCodecHandler * pHandler );
int    ExtraTestCases           ( void );

/* Test cases. */
int    NominalFunctionalityTest ( void );
int    ReLocatabilityTest       ( void );  
int    ReEntranceTest           ( void );
int    PreEmptionTest           ( void );
int    EnduranceTest            ( void );
int    LoadTest                 ( void );

/* Helper. */
int     Util_StrICmp            ( const char * s1, const char *s2 ); 
void *  Util_Malloc             ( size_t bytes );
void    Util_Free               ( void * pPtr );
void *  Util_Realloc            ( void * pPtr, size_t bytes );
void *  Util_ReadFile           ( const char * filename, size_t * pSz );
void    Util_SwapBytes          ( short * pWords, int count );

/*==================================================================================================
                                 GLOBAL VARIABLE DECLARATIONS
==================================================================================================*/

extern sTestappConfig  gTestappConfig;             /* defined in the codec_main.c */        
extern sCodecHandler   gCodecHandler[MAX_THREADS]; /* defined in the common_codec_test.c */
extern sLinkedList   * gpParamsList;               /* defined in the common_codec_test.c */       
extern pthread_mutex_t gMutex;                     /* defined in the common_codec_test.c */     
extern int             gNumThreads;                /* defined in the common_codec_test.c */
extern int (*CompareFiles)( sCodecHandler * pHandler, const char*, const char* ); /* defined in the common_codec_test.c */

#endif //__COMMON_CODEC_TEST_H__
