/*
  Copyright (C) 2004, Freescale Semiconductor, Inc. All Rights Reserved
  THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
  BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
  Freescale Semiconductor, Inc.
*/

/**
@file codec_test.c

@par Portability:
        ARM GCC
*/

/*======================== REVISION HISTORY ==================================

Author (core ID)      Date         CR Number    Description of Changes
-------------------   ----------   ----------   ------------------------------
D.Simakov/smkd001c    07/04/2005   TLSbo47116   Initial version
D.Simakov/smkd001c    22/07/2005   TLSbo52630   Relocatability test case was added
S. V-Guilhou/svna01c  14/12/2005   TLSbo57360   No need to add ".kev" at the reference files
D.Simakov/smkd001c    22/12/2005   TLSbo60964   fixes
D.Simakov             26/02/2006   TLSbo61035   Centralization of common features
=============================================================================*/



/*==================================================================================================
                                        INCLUDE FILES
==================================================================================================*/

/* Standard Include Files. */
#include <errno.h>
#include <stdio.h>
#include <assert.h>          
#include <pthread.h>
#include <math.h>
    
/* Harness Specific Include Files. */
#include "test.h"

/* Other Include Files. */
#include "codec_test.h"

/* Common Includes. */
#include <common_codec_test.h>
#include <util/mem_stat.h>
#include <util/fb_draw_api.h>
#include <util/kev_io.h>
#include <util/ycbcr.h>


/*==================================================================================================
                                        LOCAL MACROS
==================================================================================================*/


/********************************************************************/
/* Macro checks for any error in function execution                 */
/* based on the return value. In case of error, the function exits. */                                                                    
/********************************************************************/

#define CALL_H264_DECODER(H264DecRoutine, name)   \
do \
{ \
    pHandler->mLastCodecError = H264DecRoutine; \
    if( pHandler->mLastCodecError > E_AVCD_FF && pHandler->mLastCodecError != E_AVCD_NO_OUTPUT ) \
    {\
        TST_RESM( TWARN, "%s : %s fails #%d [File: %s, line: %d]", __FUNCTION__, name, pHandler->mLastCodecError, __FILE__, __LINE__-5);\
        return TFAIL;\
    }\
} while(0)   


/**************************/
/* Safe delete a pointer. */
/**************************/

#define SAFE_DELETE(p) do {if(p){Util_Free(p);p=0;}} while(0)
   

/************************************************************************/
/* Declare H264 callbacks. Needed for the MT tests, because of there is */
/* no information can be found about the caller object in the callbacks.*/
/************************************************************************/         

#define DECLARE_H264_ReadBuf(n) \
int H264_ReadBuf##n( unsigned char * pBuf, int sz, int * pLast ) \
{ \
    assert( n < MAX_THREADS ); \
    return H264_ReadBuf( pBuf, sz, pLast, n ); \
} 


/***********/
/* DPRINTF */
/***********/

#define DPRINTF(fmt,...) //do{printf("%s:%d %s() ", __FILE__, __LINE__, __FUNCTION__); printf((fmt), ##__VA_ARGS__); fflush(stdout);}while(0)


/*==================================================================================================
                                       LOCAL VARIABLES
==================================================================================================*/

static int (*gpH264_ReadBuf[MAX_THREADS])(unsigned char*, int, int*);
static pthread_once_t gAreCallbacksInitialized = PTHREAD_ONCE_INIT; 


/*==================================================================================================
                                   FUNCTION PROTOTYPES
==================================================================================================*/

/* Callbacks. */
int            H264_ReadBuf                  ( unsigned char * pBuf, int sz, int * pLast, size_t threadNo );

/* Extra test cases. */
int            RobustnessTestCase            ( void ); 

/* Other functions. */
static void    InitCallbacks                 ( void );
static int     AllocDecMemory                ( sCodecHandler * pHandler, int requery );
static int     AllocFrameMemory              ( sCodecHandler * pHandler );
static void    DrawDecompressedFrame         ( sCodecHandler * pHandler );
static int     GetInputByte                  ( sCodecHandler * pHandler );
static int     GetNalUnitAnnexB              ( sCodecHandler * pHandler );
static void    DetectEnter                   ( void );


/*==================================================================================================
                                       FUNCTIONS
==================================================================================================*/

/* Is there any way to make a compile-time loop? */
DECLARE_H264_ReadBuf(0)
DECLARE_H264_ReadBuf(1)
#if MAX_THREADS > 2
DECLARE_H264_ReadBuf(2) 
DECLARE_H264_ReadBuf(3)
#endif


/*================================================================================================*/
/*================================================================================================*/
int H264_ReadBuf( unsigned char * pBuf, int sz, int * pLast, size_t threadNo )
{
        DPRINTF( "sz = %d\n", sz );
        
        assert( pBuf && pLast );
        assert( threadNo < MAX_THREADS );
        
        sCodecHandler * pHandler = &gCodecHandler[threadNo];
        assert( pHandler->mpInpBufferRef );
        
        int retSz;
        if( pHandler->mNumBytesInTheInput >= sz )
        {        
                pHandler->mNumBytesInTheInput -= sz;        
                *pLast = (pHandler->mNumBytesInTheInput > 0) ? 0 : 1;
                memcpy( pBuf, pHandler->mpInpBufferRef, sz );
                pHandler->mpInpBufferRef += sz;
                retSz = sz;
        }
        else
        {
                retSz = pHandler->mNumBytesInTheInput;
                pHandler->mNumBytesInTheInput = 0;
                *pLast = 1;
                memcpy( pBuf, pHandler->mpInpBufferRef, sz );
        }
        
        DPRINTF( "START BYTE : %d\n", pBuf[0] );
        DPRINTF( "LAST       : %d\n", *pLast  );
        DPRINTF( "RET SIZE   : %d\n", retSz   );
                
        return retSz;
}


/*================================================================================================*/
/*================================================================================================*/
int RobustnessTest( void )
{
        sLinkedList * pNode;
        int i;
        int rv = TPASS;    
        sCodecHandler * pHandler = gCodecHandler;     
        
        
        /***********************/
        /* For the each entry. */
        /***********************/
        
        for( pNode = gpParamsList, i = 0; pNode; pNode = pNode->mpNext, ++i )
        {
                /* Reset the handler. */
                ResetHandler( pHandler );
                
                /* Get content. */
                pHandler->mpParams = (sHandlerParams*)pNode->mpContent;
                
                if( gTestappConfig.mVerbose )            
                {
                        PrintInfo( pHandler );
                }                        
                
                /* Run the Decoder. */
                int ret = RunCodec( pHandler );
                if( TPASS == ret ) 
                {
                        if( gTestappConfig.mVerbose )                                
                                TST_RESM( TFAIL, "Thread[%lu][%lu] Robustness failed", 
                                          pHandler->mIndex, pHandler->mpParams->mNoEntry );
                        rv = TFAIL;
                }
                else
                {
                        if( gTestappConfig.mVerbose )                                
                                TST_RESM( TINFO, "Thread[%lu][%lu] Robustness passed", 
                                          pHandler->mIndex, pHandler->mpParams->mNoEntry );
                }
        }
        
        return rv;    
} 
 

/*================================================================================================*/
/*================================================================================================*/
int TestEngine( sCodecHandler * pHandler )
{
        assert( pHandler );
        assert( pHandler->mIndex < MAX_THREADS );
        assert( pHandler->mpParams );

        pthread_once( &gAreCallbacksInitialized, InitCallbacks );                 
        
        sAVCDecoderConfig * pDecObj   = &pHandler->mDecObject;
        sHandlerParams    * pParams   = pHandler->mpParams;   
        int                 hasOutput = Util_StrICmp( pParams->mOutFileName, NA );
        
        /**/
        pDecObj->cbkAVCDBufRead = gpH264_ReadBuf[pHandler->mIndex];
        
        
        /*****************************************/
        /* Allocate memory for the input buffer. */
        /*****************************************/    
        
        pDecObj->s32InBufferLength = 2100000;
        pDecObj->pvInBuffer        = Util_Malloc( pDecObj->s32InBufferLength );
        if( !pDecObj->pvInBuffer )
        {
                tst_brkm( TBROK, (void(*)())VT_codec_cleanup,
                        "%s : Can't allocate %d bytes",
                        __FUNCTION__, pDecObj->s32InBufferLength );
        }     
        
        
        /***********************************************/
        /* Open files that are necessary for decoding. */
        /***********************************************/
        
        pHandler->mpInpStream = fopen( pParams->mInpFileName, "rb" );
        if( !pHandler->mpInpStream )
        {
                tst_brkm( TBROK, (void(*)())VT_codec_cleanup,
                        "%s : Can't open input file \'%s\'",
                        __FUNCTION__, pParams->mInpFileName );
        }

        if( hasOutput )
        {
                pHandler->mpOutKev = Kev_Open( pParams->mOutFileName, TRUE );                
                if( !pHandler->mpOutKev )
                {
                        tst_brkm( TBROK, (void(*)())VT_codec_cleanup,
                                  "%s : Can't open output kev file \'%s\'",
                                  __FUNCTION__, pParams->mOutFileName );
                }
        }
        
        
        /*************************************************************************/
        /* Give memory to the decoder for the size, type and alignment returned. */
        /*************************************************************************/   
        
        if( TPASS != AllocDecMemory( pHandler, FALSE ) )
                return TFAIL;
        

        /***************************/
        /* Initialize the decoder. */
        /***************************/
        
        CALL_H264_DECODER(
                eAVCDInitVideoDecoder( pDecObj ),
                "eAVCDInitVideoDecoder" );
        

        /*************************************************/
        /* Decode the bit stream and produce the output. */
        /*************************************************/        
        
        int enableLcd = !gTestappConfig.mDisableLCD &&
                        !(RE_ENTRANCE == gTestappConfig.mTestCase || 
                         PRE_EMPTION == gTestappConfig.mTestCase);
        if( enableLcd )
        {
                const sFramebuffer * pFb = GetFramebuffer();
                assert( pFb );

                sColor white;        
                white.mAll = 0xffffffff;
                pFb->ClearScreen( &white );
        }
        
        while( 1 )
        {
                if( E_AVCD_CHANGE_SERVICED != pHandler->mLastCodecError )
                {
                        pDecObj->s32InBufferLength = 2100000;
                        pDecObj->s32NumBytes = GetNalUnitAnnexB( pHandler );
                        DPRINTF( "E_AVCD_CHANGE_SERVICED: %d\n", (int)pDecObj->s32NumBytes ); 
                }
                pHandler->mNumBytesInTheInput = pDecObj->s32NumBytes;
                pHandler->mpInpBufferRef      = pDecObj->pvInBuffer;
                
                /* If end of bit stream is reached. */
                if( !pDecObj->s32NumBytes )
                        break;
                        
                /* Decode the next NAL unit. */
                CALL_H264_DECODER(
                        eAVCDecodeNALUnit( pDecObj, 0 ),
                        "eAVCDecodeNALUnit" );
                        
                if( E_AVCD_SEQ_CHANGE == pHandler->mLastCodecError )
                {
                
                        DPRINTF( "E_AVCD_SEQ_CHANGE PRE! rv #%d\n", pHandler->mLastCodecError );
                        if( TPASS != AllocDecMemory( pHandler, TRUE ) )
                                return TFAIL;
                        DPRINTF( " rv #%d\n", pHandler->mLastCodecError );
                        if( pDecObj->sConfig.s16FrameWidth || pDecObj->sConfig.s16FrameHeight )
                        {
                                if( !AllocFrameMemory( pHandler ) )
                                {
                                        TST_RESM( TWARN, "%s : AllocFrameMemory() failed [File: %s, line: %d]",
                                                __FUNCTION__, __FILE__, __LINE__ );
                                        return TFAIL;
                                }
                        }                         
                                                 
                }
                else if( E_AVCD_NOERROR == pHandler->mLastCodecError )
                {
                        /* Write the frame into output KEV. */
                        if( hasOutput )
                        {                                                  
                                sAVCDYCbCrStruct * pFrm = &pDecObj->sFrameData;
                                
                                Kev_SetDims( pHandler->mpOutKev, pFrm->s16FrameWidth, pFrm->s16FrameHeight);
                                
                                if( !Kev_WriteFrame( pHandler->mpOutKev, pFrm->s32FrameNumber, pFrm->pu8y,
                                                     pFrm->s16Xsize, pFrm->pu8cb, pFrm->pu8cr, pFrm->s16CxSize ) )
                                {
                                        TST_RESM( TWARN, "%s : Can't write frame to %s [File: %s, line: %d]",
                                                __FUNCTION__, pParams->mOutFileName, __FILE__, __LINE__ );
                                }
                                
                        }
                        
                        if( enableLcd )
                                DrawDecompressedFrame( pHandler );
                }                
        }
        
#ifdef DEBUG_TEST
        /* Current memory used by the app. */
        if( NOMINAL_FUNCTIONALITY == gTestappConfig.mTestCase )
                MemStat_GetStat();
#endif                


        pParams->mIsReadyForBitMatching = TRUE;
                
        if( gTestappConfig.mVerbose )
        {        
                TST_RESM( TINFO, "Thread[%lu][%lu] Decoding completed", 
                        pHandler->mIndex, pParams->mNoEntry );
        }


        /************/
        /* Cleanup. */
        /************/
        
        CleanupHandler( pHandler );

#ifdef DEBUG_TEST
        /* Current memory used by the app. */
        if( NOMINAL_FUNCTIONALITY == gTestappConfig.mTestCase )
                MemStat_GetStat();
#endif           
        
        
        if( gTestappConfig.mDelay )
                DetectEnter();
              
        return TPASS;
}


/*================================================================================================*/
/*================================================================================================*/
void CleanupHandler( sCodecHandler * pHandler ) 
{
        assert( pHandler );

        sAVCDecoderConfig * pDecObj = &pHandler->mDecObject;    
        sAVCDYCbCrStruct  * pFrm    = &pDecObj->sFrameData;
        int                 needCleanup = FALSE;

        /*********************************/
        /* Close the files we've opened. */
        /*********************************/
        
        if( pHandler->mpInpStream )
        {
                needCleanup = TRUE;
                fclose( pHandler->mpInpStream );
                pHandler->mpInpStream = NULL;
        }

        if( pHandler->mpOutKev )
        {
                Kev_Close( pHandler->mpOutKev );
                pHandler->mpOutKev = NULL;
        }

        
        /**************************/
        /* Free the input buffer. */
        /**************************/

        SAFE_DELETE( pHandler->mDecObject.pvInBuffer );
           
        
        /**************************/
        /* Free the frame memory. */
        /**************************/

        SAFE_DELETE( pFrm->pu8y );
        SAFE_DELETE( pFrm->pu8cb );
        SAFE_DELETE( pFrm->pu8cr );
        SAFE_DELETE( pHandler->mpRGBFrame );


        /******************************/
        /* Free the decoder's memory. */
        /******************************/
       
        sAVCDMemAllocInfo * pMemInfo = &pDecObj->sMemInfo;
        int i;
        for( i = 0; i < pMemInfo->s32NumReqs; ++i )
        {
                sAVCDMemBlock * pMemBlock = &pMemInfo->asMemBlks[i];
                SAFE_DELETE( pMemBlock->pvBuffer );
        }          


        /**************************************/
        /* Call eAVCDFreeVideoDecoder() once. */
        /**************************************/
        
        if( needCleanup )
        {
                pHandler->mLastCodecError = eAVCDFreeVideoDecoder( pDecObj );
                if( E_AVCD_NO_OUTPUT != pHandler->mLastCodecError )
                {
                        TST_RESM( TWARN, "%s : eAVCDFreeVideoDecoder fails #%d [File: %s, line: %d]", 
                                pHandler->mLastCodecError, __FUNCTION__, __FILE__, __LINE__ );
                }
        }
}


/*================================================================================================*/
/*================================================================================================*/
int DoBitmatch( sCodecHandler * pHandler )
{
        assert( pHandler );
        assert( pHandler->mpParams );
                
        sHandlerParams * pParams = pHandler->mpParams;                       
        char fname1[MAX_STR_LEN], fname2[MAX_STR_LEN];
        const char * comp[] = {"y", "cb", "cr"};
        int i;

        /* Compare Y, Cb and Cr components. */
        for( i = 0; i < 3; ++i )
        {                
                sprintf( fname1, "%s/%s/data", pParams->mOutFileName, comp[i] );
                sprintf( fname2, "%s/%s/data", pParams->mRefFileName, comp[i] );      
                if( !CompareFiles( pHandler, fname1, fname2 ) )
                        return FALSE; 
        }
        
        return TRUE;
} 


/*================================================================================================*/
/*================================================================================================*/
void PrintInfo( sCodecHandler * pHandler )
{
        assert( pHandler );
        assert( pHandler->mpParams );
        
        TST_RESM( TINFO, "Thread[%lu][%lu] Inp: %s, Out: %s, Ref: %s", 
                pHandler->mIndex, 
                pHandler->mpParams->mNoEntry,
                pHandler->mpParams->mInpFileName, 
                pHandler->mpParams->mOutFileName, 
                pHandler->mpParams->mRefFileName );        
}
 

/*================================================================================================*/
/*================================================================================================*/
int ExtraTestCases( void )
{
        assert( ROBUSTNESS == gTestappConfig.mTestCase && "Wrong test case" );
        return RobustnessTest();
} 

 
/*================================================================================================*/
/*================================================================================================*/
void MakeEntry( char entry[WORDS_IN_ENTRY][MAX_STR_LEN], int nEntry )
{
        sHandlerParams * pParams = (sHandlerParams*)malloc( sizeof(sHandlerParams) );    
        
        pParams->mNoEntry = nEntry;
        
        int n = 0;  
        strncpy( pParams->mInpFileName, entry[n++], MAX_STR_LEN );   
        strncpy( pParams->mOutFileName, entry[n++], MAX_STR_LEN );
        strncpy( pParams->mRefFileName, entry[n++], MAX_STR_LEN );      
                
        
        /****************************/
        /* Adjust/check parameters. */
        /****************************/        
        
        int verbose = gTestappConfig.mVerbose;
        gTestappConfig.mVerbose = 0;
        if( !DoFilesExist( pParams->mInpFileName, pParams->mInpFileName ) )
        {        
                gTestappConfig.mVerbose = verbose;
                tst_brkm( TBROK, (void(*)())VT_codec_cleanup,
                        "The input file \'%s\' does not exist."
                        "Please check \'%s\', line/entry #%lu", 
                        pParams->mInpFileName, NA, gTestappConfig.mConfigFilename, nEntry + 1 );
        }            
        gTestappConfig.mVerbose = verbose;
        
        LList_PushBack( gpParamsList, pParams );
} 


/*================================================================================================*/
/*================================================================================================*/
static void InitCallbacks( void )
{
        gpH264_ReadBuf[0] = H264_ReadBuf0;
        gpH264_ReadBuf[1] = H264_ReadBuf1;
#if MAX_THREADS > 2
        gpH264_ReadBuf[2] = H264_ReadBuf2;
        gpH264_ReadBuf[3] = H264_ReadBuf3;
#endif
}


/*================================================================================================*/
/*================================================================================================*/
static int AllocDecMemory( sCodecHandler * pHandler, int requery )
{
        assert( pHandler );
        
        sAVCDecoderConfig * pDecObj  = &pHandler->mDecObject;
        sAVCDMemAllocInfo * pMemInfo = &pDecObj->sMemInfo;
        if( !requery )
        {
                CALL_H264_DECODER( 
                        eAVCDInitQueryMem( pMemInfo ),
                        "eAVCDInitQueryMem" );
        }
        else
        {
                CALL_H264_DECODER(
                        eAVCDReQueryMem( pDecObj ),
                        "eAVCDReQueryMem" );                
        }
        
        int i;
        for( i = 0; i < pMemInfo->s32NumReqs; ++i )
        {
                sAVCDMemBlock * pMemBlock = &pMemInfo->asMemBlks[i];
                
                if( requery )
                {
                        if( 1 == pMemBlock->s32SizeDependant && 1 == pMemBlock->s32Allocate )
                        {
                                if( pMemBlock->pvBuffer && 1 == pMemBlock->s32Copy )
                                {
                                        pMemBlock->pvBuffer = Util_Realloc( pMemBlock->pvBuffer, pMemBlock->s32Size );
                                }
                                else
                                {
                                        SAFE_DELETE( pMemBlock->pvBuffer );
                                        pMemBlock->pvBuffer = Util_Malloc( pMemBlock->s32Size );
                                }
                                if( !pMemBlock->pvBuffer )
                                {                
                                        tst_brkm( TBROK, (void(*)())VT_codec_cleanup,
                                                "%s : Can't allocate %d bytes",
                                                __FUNCTION__, pMemBlock->s32Size );
                                }
                        }
                }
                else if( 1 == pMemBlock->s32Allocate )
                {
                        pMemBlock->pvBuffer = Util_Malloc( pMemBlock->s32Size );
                        if( !pMemBlock->pvBuffer )
                        {                
                                tst_brkm( TBROK, (void(*)())VT_codec_cleanup,
                                        "%s : Can't allocate %d bytes",
                                        __FUNCTION__, pMemBlock->s32Size );
                        }
                }
        }       
        return TPASS;
}


/*================================================================================================*/
/*================================================================================================*/
static int AllocFrameMemory( sCodecHandler * pHandler )
{
        assert( pHandler );
        
        sAVCDecoderConfig * pDecObj  = &pHandler->mDecObject;    
        sAVCDYCbCrStruct  * pFrm     = &pDecObj->sFrameData;
        sAVCDConfigInfo   * pCfgInfo = &pDecObj->sConfig;
        
        int xSz, ySz, cySz, cxSz;
        SAFE_DELETE( pFrm->pu8y );
        SAFE_DELETE( pFrm->pu8cb );
        SAFE_DELETE( pFrm->pu8cr );
        
        // Allocate memory to the frames
        pFrm->s16Xsize = pCfgInfo->s16FrameWidth + 16; // 192;
        xSz = pFrm->s16Xsize;
        
        ySz = pCfgInfo->s16FrameHeight + 16; // 172;
        pFrm->s16CxSize = pFrm->s16Xsize >> 1;
        cySz = ySz >> 1;
        cxSz = pFrm->s16CxSize;
        
        pFrm->pu8y = (unsigned char*)Util_Malloc( xSz * ySz );
        if( !pFrm->pu8y )
                return FALSE;    
        pFrm->pu8cb = (unsigned char*)Util_Malloc( cxSz * cySz );
        if( !pFrm->pu8cb )
        {
                SAFE_DELETE( pFrm->pu8y );
                return FALSE;
        }
        pFrm->pu8cr = (unsigned char*)Util_Malloc( cxSz * cySz );    
        if( !pFrm->pu8cr )
        {
                SAFE_DELETE( pFrm->pu8y );
                SAFE_DELETE( pFrm->pu8cb );
                return FALSE;
        }
        return TRUE;
}


/*================================================================================================*/
/*================================================================================================*/
static void DrawDecompressedFrame( sCodecHandler * pHandler )
{
        assert( pHandler );        
        
        sAVCDYCbCrStruct * pFrm = &pHandler->mDecObject.sFrameData;
                
        
        /*********************************************/
        /* Allocate the RGB buffer. Once per thread. */
        /*********************************************/
        
        if( !pHandler->mpRGBFrame )
        {
                pHandler->mRGBFrameSz = pFrm->s16FrameWidth * pFrm->s16FrameHeight * 3;
                pHandler->mpRGBFrame = (unsigned char*)Util_Malloc( pHandler->mRGBFrameSz );
                if( !pHandler->mpRGBFrame )
                {
                        TST_RESM( TWARN, "%s : Can't allocate %d bytes for intermediate RGB buffer [File: %s, line: %d]",
                                __FUNCTION__, pHandler->mRGBFrameSz, __FILE__, __LINE__ );
                        return;
                }
        }

        
        /***********************/
        /* Convert YUV to RGB. */                                                                     
        /***********************/
        
        YCrCbToRGB( pFrm->pu8y, pFrm->s16Xsize, pFrm->pu8cr, pFrm->pu8cb, pFrm->s16CxSize, 
                pHandler->mpRGBFrame, pFrm->s16FrameWidth, pFrm->s16FrameHeight, RGB_888 );

        
        /***********************/
        /* Draw the RGB frame. */
        /***********************/
        
        const sFramebuffer * pFb = GetFramebuffer();
        assert( pFb );

        int i;
        unsigned char * pPtr = pHandler->mpRGBFrame;
        ePixelFormat pfmt = PF_RGB_888;
        for( i = 0; i < pFrm->s16FrameHeight; ++i )
        {
                pFb->DrawScanline( pPtr, 0, i, pFrm->s16FrameWidth, pfmt );
                pPtr += (pFrm->s16FrameWidth * 3);
        } 
        
}


/*================================================================================================*/
/*================================================================================================*/
static int GetInputByte( sCodecHandler * pHandler )
{       
        assert( pHandler ); //...        
        
        if( pHandler->mInputCache.mcIndex >= pHandler->mInputCache.mcBytes )
        {
                pHandler->mInputCache.mcIndex = 0;
                
                pHandler->mInputCache.mcBytes = 
                        fread( (void*)pHandler->mInputCache.mcBuf, 
                                1, 
                                CACHE_BUF_SZ, 
                                pHandler->mpInpStream );
                
                if( !pHandler->mInputCache.mcBytes )
                {
                        pHandler->mInputCache.mcEof = TRUE;
                        return -1;
                }
        }        
        
        return pHandler->mInputCache.mcBuf[pHandler->mInputCache.mcIndex++];
}


/*================================================================================================*/
/*================================================================================================*/
static int GetNalUnitAnnexB( sCodecHandler * pHandler )
{
        assert( pHandler );        
        unsigned char * pBuf = pHandler->mDecObject.pvInBuffer;
        size_t          bufSz = pHandler->mDecObject.s32InBufferLength; 
        long            nBytes = 0, nTmp = 0;
        int             x;
        unsigned long   lastWord = 0xffffffff;
        
        // Skip leading zero-bytes
        while( (x = GetInputByte( pHandler )) == 0 );
        
        if( pHandler->mInputCache.mcEof )
                return 0;
               
        while( 1 )
        {
                x = GetInputByte( pHandler );
                
                if( pHandler->mInputCache.mcEof )
                {
                        int i;
                        
                        // Write out last 3 bytes
                        for( i = 3 - nTmp; i < 3; ++i )
                        {
                                pBuf[nBytes++] = (unsigned char)
                                        ( (lastWord >> (16 - 8 * i)) & 0xff );
                        }
                        break;
                }
                
                if( nBytes > bufSz )
                        break;
                
                lastWord = ((lastWord & 0x00ffffff) << 8) | (x & 0xff);
                
                // Check if the last 3 bytes are either start code or 0s
                if( !(lastWord & 0x00fffffe) )
                {
                        pBuf[nBytes++] = (unsigned char)
                                ( (lastWord >> 24) & 0xff );
                        --pHandler->mInputCache.mcIndex;
                        assert( pHandler->mInputCache.mcIndex >= 0 );
                        break;
                }
                                
                if( nTmp < 3 ) 
                        ++nTmp;
                else
                        pBuf[nBytes++] = (unsigned char)( (lastWord >> 24) & 0xff );
        }
        
        return nBytes;
}


/*================================================================================================*/
/*================================================================================================*/
static void DetectEnter( void )
{   
        if( !gTestappConfig.mDelay ) return;
        
        int fdConsole = 0; /* 0 is the video input */
        fd_set fdset;
        struct timeval timeout;
        char c;
        
        FD_ZERO( &fdset);
        FD_SET( fdConsole, &fdset );
        timeout.tv_sec = gTestappConfig.mDelay; /* set timeout !=0 => blocking select */
        timeout.tv_usec = 0;
        if( select( fdConsole+1, &fdset, 0, 0, &timeout ) > 0 )
        {
                do 
                {
                        read( fdConsole, &c, 1 );
                } while( c != 10 ); // i.e. line-feed 
        }
} 
