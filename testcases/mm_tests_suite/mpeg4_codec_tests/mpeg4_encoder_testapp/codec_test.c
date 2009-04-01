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
I.Inkina\nknl001      25/11/2004   TLSbo45065    Initial version
D.Simakov             29/03/2006   TLSbo66281    Centralization of common features
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
#include <util/ycbcr.h>
#include <util/kev_io.h>


/*==================================================================================================
                                        LOCAL MACROS
==================================================================================================*/


/********************************************************************/
/* Macro checks for any error in function execution                 */
/* based on the return value. In case of error, the function exits. */                                                                    
/********************************************************************/

#define CALL_MPEG4_ENCODER(MPEG4EncRoutine, name)   \
do \
{ \
    pHandler->mLastCodecError = MPEG4EncRoutine; \
    if(pHandler->mLastCodecError != E_MPEG4E_SUCCESS ) \
    {\
        TST_RESM( TWARN, "%s : %s fails #%d [File: %s, line: %d]", __FUNCTION__, name, pHandler->mLastCodecError, __FILE__, __LINE__-2);\
        return TFAIL;\
    }\
} while(0)   

#define WARN_MSG(name,msg)   \
do \
{ \
    {\
        TST_RESM( TWARN, "%s : %s %s [File: %s, line: %d]", __FUNCTION__, name, msg, __FILE__, __LINE__-2);\
    }\
} while(0)   

#define WRITE_DATA(data,sz) fwrite((data),1,(sz),pHandler->mpOutStream)




/**************************/
/* Safe delete a pointer. */
/**************************/

#define SAFE_DELETE(p) do {if(p){Util_Free(p);p=0;}} while(0)

#define NO_SKIP 0
#define SKIP_N_FRAMES 1
#define SKIP_I_FRAME 2   


/***********/
/* DPRINTF */
/***********/

#define DPRINTF(fmt,...) do{printf("%s:%d %s() ", __FILE__, __LINE__, __FUNCTION__); printf((fmt), ##__VA_ARGS__); fflush(stdout);}while(0)


/*==================================================================================================
                                   FUNCTION PROTOTYPES
==================================================================================================*/

/* Other functions. */
static int     EncodingLoop                  ( sCodecHandler * pHandler );
static void    DetectEnter                   ( void );

/*==================================================================================================
                                       FUNCTIONS
==================================================================================================*/


/*================================================================================================*/
/*================================================================================================*/
int TestEngine( sCodecHandler * pHandler )
{
        assert( pHandler );
        assert( pHandler->mIndex < MAX_THREADS );
        assert( pHandler->mpParams );
        
        sMpeg4EncoderConfig     * pEncObj   = &pHandler->mEncObject;
        sMpeg4EMemAllocInfo     * pMemInfo  = &pEncObj->sMemInfo;        
        sHandlerParams          * pParams   = pHandler->mpParams;   
        int                       hasOutput = Util_StrICmp( pParams->mOutFileName, NA );
        
 
        /***********************************************/
        /* Open files that are necessary for decoding. */
        /***********************************************/
        
        pHandler->mpInpKev = Kev_Open( pParams->mInpFileName, FALSE );
        if( !pHandler->mpInpKev )
        {
                tst_brkm( TBROK, (void(*)())VT_codec_cleanup,
                        "%s : Can't open input kev file \'%s\'",
                        __FUNCTION__, pParams->mInpFileName );
        }

        if( hasOutput )
        {
                pHandler->mpOutStream = fopen( pParams->mOutFileName, "wb" );                
                if( !pHandler->mpOutStream )
                {
                        tst_brkm( TBROK, (void(*)())VT_codec_cleanup,
                                  "%s : Can't create output file \'%s\'",
                                  __FUNCTION__, pParams->mOutFileName );
                }
        }
        

        /***************************/
        /* Fill up the parameters. */
        /***************************/
        
        // Set Default Parameters for the encoder structure
        CALL_MPEG4_ENCODER(
            eMpeg4EncoderSetDefaultParam( pEncObj ),
            "eMpeg4EncoderSetDefaultParam" );
        
        // Set all the related parameters in the encoder config structure
        pEncObj->s32SourceWidth = Kev_Height( pHandler->mpInpKev );
        pEncObj->s32SourceHeight = Kev_Width( pHandler->mpInpKev );
                               
        // Caclulate the number of MB's present in the frame
        pHandler->mNumMB = ((pEncObj->s32SourceWidth + 15) >> 4) *
                ((pEncObj->s32SourceHeight + 15) >> 4);
       
        if( pParams->mSrcFps > 0 )
                pEncObj->s32SourceFrameRate = pParams->mSrcFps;
        
        if( pParams->mEncFps > 0 )
                pEncObj->s32EncodingFrameRate = pParams->mEncFps;
        
        if( pParams->mStartFrameNum > 0 )
                pEncObj->s32StartFrameTime = pParams->mStartFrameNum;
        
        if( pParams->mIPeriod > 0 )
                pEncObj->s32IntraPeriod = pParams->mIPeriod;
        
        if( pParams->mShortVideoHdr != pEncObj->s32ShortVideoHeader )
                pEncObj->s32ShortVideoHeader = pParams->mShortVideoHdr;
        
        if( pParams->mH263NoShortVideoHeader != pEncObj->s32H263NoShortVideoHeader )
                pEncObj->s32H263NoShortVideoHeader = pParams->mH263NoShortVideoHeader;
        
        if( pParams->mNumForcedIntra > 0 )
                pEncObj->s32NumForcedIntra = pParams->mNumForcedIntra;
        
        if( pParams->mVideoPacketEnable > 0 )
                pEncObj->s32VideoPacketEnable = pParams->mVideoPacketEnable;
        
        if( pParams->mDataPartitioningEnable > 0 )
                pEncObj->s32DataPartitioningEnable = pParams->mDataPartitioningEnable;
        
        if( pParams->mUseRVLC > 0 )
                pEncObj->s32UseRVLC = pParams->mUseRVLC;
        
        if( pParams->mResyncMarkSpacing > 0 )
                pEncObj->s32ResyncMarkSpacing = pParams->mResyncMarkSpacing;
        
        if( pParams->mResyncMarkMBSpacing > 0 )
                pEncObj->s32ResyncMarkMBSpacing = pParams->mResyncMarkMBSpacing;
        
        if( pParams->mIntraDcVlcThresh > 0 )
                pEncObj->s32IntraDcVlcThresh = pParams->mIntraDcVlcThresh;
        
        if( pParams->mIntraAcPredFlag > 0 )
                pEncObj->s32IntraAcPredFlag = pParams->mIntraAcPredFlag;
        
        if( pParams->mDelayFactor > 0 )
                pEncObj->u8DelayFactor = pParams->mDelayFactor;
        
        if( pParams->mQualityTradeoff > 0 )
                pEncObj->u8QualityTradeoff = pParams->mQualityTradeoff;
        
        pEncObj->s32Profile = 0;
        pEncObj->s32Level   = pParams->mLevel;
        
        if( pParams->mIntraVopQuant > 0 )
                pEncObj->s32IntraVopQuant = pParams->mIntraVopQuant;
        if( pParams->mInterVopQuant > 0 )
                pEncObj->s32InterVopQuant = pParams->mInterVopQuant;
        
        if (pParams->mTargetBitRate > 0)
                pEncObj->s32TargetBitRate = pParams->mTargetBitRate;


        pEncObj->s32Profile = 0;
        pEncObj->s32Level   = 3;
        pEncObj->s32SourceFrameRate = 15;
        pEncObj->s32EncodingFrameRate = 15;
        pEncObj->s32TargetBitRate = 64000;
                                

        /******************************************************************/
        /* Query and allocate the memory which are needed by the encoder. */
        /******************************************************************/

        CALL_MPEG4_ENCODER(
                eMpeg4EncoderQueryMemory(pEncObj),
                "eMpeg4EncoderQueryMemory" );

        int i;
        for( i = 0; i < pMemInfo->s32NumReqs; ++i ) 
        {                
                sMpeg4EMemBlock * pMemBlock = &pMemInfo->asMemBlks[i];
                if( pMemBlock->s32Size <= 0 )
                        continue;                
                pMemBlock->pvBuffer = (void*)Util_Malloc( pMemBlock->s32Size );                
                if( !pMemBlock->pvBuffer )
                {                        
                        tst_brkm( TBROK, (void(*)())VT_codec_cleanup,
                                "%s : Can't allocate %lu bytes memory",
                                __FUNCTION__, (unsigned long)pMemBlock->s32Size );                
                }
        }
        
        
        /***************************/
        /* Allocate input buffers. */
        /***************************/

        pHandler->mYBufSz = pEncObj->s32SourceWidth * pEncObj->s32SourceHeight;
        pHandler->mCrCbBufSz = pHandler->mYBufSz >> 2;
        pHandler->mpYBuf = (unsigned char*)Util_Malloc( pHandler->mYBufSz );
        pHandler->mpCrBuf = (unsigned char*)Util_Malloc( pHandler->mCrCbBufSz );
        pHandler->mpCbBuf = (unsigned char*)Util_Malloc( pHandler->mCrCbBufSz );
        if( !pHandler->mpYBuf || !pHandler->mpCrBuf || !pHandler->mpCbBuf )
        {
                tst_brkm( TBROK, (void(*)())VT_codec_cleanup,
                                "%s : Can't allocate memory",
                                __FUNCTION__ );       
        }
     

        // Allocating memory for force intra MB
        pHandler->mpSetRandomIntraMB = (unsigned char *)Util_Malloc( pHandler->mNumMB );
        if( !pHandler->mpSetRandomIntraMB )
        {
                tst_brkm( TBROK, (void(*)())VT_codec_cleanup,
                        "%s : Can't allocate %lu bytes memory",
                        __FUNCTION__, (unsigned long)pHandler->mNumMB );                
        }
        
        // Initialize the forced intra MB array
        for( i = 0; i < pHandler->mNumMB; ++i )
                pHandler->mpSetRandomIntraMB[i] = 0;
        
        // Set the pointer in the config structure 
        pEncObj->pu8SetRandomIntraMB =  pHandler->mpSetRandomIntraMB;
                
        
        /**************************/
        /* Set the output buffer. */
        /**************************/

        pEncObj->pu8OutputBufferPtr = pHandler->mpOutBuffer;
        pEncObj->s32MaxOutputBufferSize = OUTP_BUF_SZ;


        /***************************/
        /* Initialize the encoder. */
        /***************************/

        CALL_MPEG4_ENCODER(
                eMpeg4EncoderInit( pEncObj ),
                "eMpeg4EncoderInit" );

        if( pEncObj->s32BitstreamSize )
        {
                WRITE_DATA( pEncObj->pu8OutputBufferPtr, pEncObj->s32BitstreamSize );
        }


        /****************************/
        /* Start the encoding loop. */
        /****************************/
        
        if( TPASS != EncodingLoop(pHandler) )
                return TFAIL;


        pParams->mIsReadyForBitMatching = TRUE;
                
        if( gTestappConfig.mVerbose )
        {        
                TST_RESM( TINFO, "Thread[%lu][%lu] Encoding completed", 
                        pHandler->mIndex, pParams->mNoEntry );
        }

#ifdef DEBUG_TEST
        /* Current memory used by the app. */
        if( NOMINAL_FUNCTIONALITY == gTestappConfig.mTestCase )
                MemStat_GetStat();
#endif           


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

        sMpeg4EncoderConfig  * pEncObj     = &pHandler->mEncObject;
        
        /*********************************/
        /* Close the files we've opened. */
        /*********************************/
        
        if( pHandler->mpInpKev )
        {
                Kev_Close( pHandler->mpInpKev );
                pHandler->mpInpKev = NULL;
        }

        if( pHandler->mpOutStream )
        {
                fclose( pHandler->mpOutStream );
                pHandler->mpOutStream = NULL;
        }
                   
        
        /**************************/
        /* Free the frame memory. */
        /**************************/
        
        SAFE_DELETE( pHandler->mpYBuf );
        SAFE_DELETE( pHandler->mpCbBuf );
        SAFE_DELETE( pHandler->mpCrBuf );
        SAFE_DELETE( pEncObj->pu8SetRandomIntraMB );


        /******************************/
        /* Free the decoder's memory. */
        /******************************/
       
        sMpeg4EMemAllocInfo * pMemInfo = &pEncObj->sMemInfo;
        int i;
        for( i = 0; i < pMemInfo->s32NumReqs; ++i ) 
        {                
                sMpeg4EMemBlock * pMemBlock = &pMemInfo->asMemBlks[i];
                SAFE_DELETE(pMemBlock->pvBuffer);                
        }        
}


/*================================================================================================*/
/*================================================================================================*/
int DoBitmatch( sCodecHandler * pHandler )
{
        assert( pHandler );
        assert( pHandler->mpParams );
        
        return CompareFiles( pHandler, pHandler->mpParams->mOutFileName, pHandler->mpParams->mOutFileName );        
}


/*================================================================================================*/
/*================================================================================================*/
int IsBitmatchNeeded( sCodecHandler * pHandler )
{
        assert( pHandler );
        assert( pHandler->mpParams );
        if( !Util_StrICmp( pHandler->mpParams->mRefFileName, NA ) )
                return FALSE;
        return pHandler->mpParams->mIsReadyForBitMatching;                        
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
 

int ExtraTestCases( void )
{
        assert( !"Wrong test case" );
        return TFAIL;
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
static int EncodingLoop( sCodecHandler * pHandler )
{                       
        assert( pHandler );
        
        int                   nextFrame;
        sMpeg4EncoderConfig * pEncObj = &pHandler->mEncObject;                                                        
        int frameNo = 0;

        while(frameNo < Kev_GetNumFrames( pHandler->mpInpKev ))
        {                                
                /********************************************/
                /* Get the next frame number to be encoded. */
                /********************************************/
                
                CALL_MPEG4_ENCODER(
                        eMpeg4EncoderGetNextFrameNum(pEncObj),
                        "eMpeg4EncoderGetNextFrameNum" );
                        
                nextFrame = pEncObj->s32NextFrameNumber;
        
                
                /***************************************************************************/
                /* Read frames until the correct frame required by the encoder is reached. */
                /***************************************************************************/
                
                do
                {                   
                        // Read the next frame
                        if( !Kev_ReadFrame( pHandler->mpInpKev, 
                                frameNo, 
                                pHandler->mpYBuf, 
                                pEncObj->s32SourceHeight,
                                pHandler->mpCbBuf, 
                                pHandler->mpCrBuf, 
                                pEncObj->s32SourceHeight / 2 ) )
                        {
                                WARN_MSG( "Kev_ReadFrame", "fails" );
                                return TFAIL;
                        }
                        ++frameNo;
                        
                        pEncObj->pu8CurrYAddr = pHandler->mpYBuf;
                        pEncObj->pu8CurrCRAddr = pHandler->mpCrBuf;
                        pEncObj->pu8CurrCBAddr = pHandler->mpCbBuf;                
                        
                        if( frameNo > Kev_GetNumFrames( pHandler->mpInpKev ) )
                                return TPASS;                        
                } while(frameNo < nextFrame);

                pEncObj->pu8OutputBufferPtr = pHandler->mpOutBuffer;
                pEncObj->s32MaxOutputBufferSize = OUTP_BUF_SZ;

                
                /*************/
                /* Encoding. */
                /*************/

                CALL_MPEG4_ENCODER(
                        eMpeg4EncoderEncode(pEncObj),
                        "eMpeg4EncoderEncode" );

                if( pEncObj->s32BitstreamSize > 0 )
                {                
                        WRITE_DATA(pEncObj->pu8OutputBufferPtr, pEncObj->s32BitstreamSize);                        
                }
        }

        return TPASS;
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
