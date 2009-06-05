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
D.Simakov/smkd001c    23/05/2005   TLSbo47117	Initial version		
D.Simakov/smkd001c    17/06/2005   TLSbo51635	Frame rate was added.	
D.Simakov             19/05/2006   TLSbo66280   Phase2
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
#include <sys/time.h>
    
/* Harness Specific Include Files. */
#include "test.h"

/* Other Include Files. */
#include "codec_test.h"

/* Common Includes. */
#include <common_codec_test.h>
#include <util/mem_stat.h>
#include <util/kev_io.h>
#include <util/fb_draw_api.h>
#include <util/ycbcr.h>



/*==================================================================================================
                                        LOCAL MACROS
==================================================================================================*/

#define DPRINTF(fmt,...) do{printf("%s:%d %s() ", __FILE__, __LINE__, __FUNCTION__); printf((fmt), ##__VA_ARGS__); fflush(stdout);}while(0)

/**********************************************************************
 * Macro name:  CALL_WMV_DECODER()
 * Description: Macro checks for any error in function execution
                based on the return value. In case of error, the function exits.
**********************************************************************/
#define CALL_WMV_DECODER(WmvDecRoutine, name)   \
do \
{ \
    pHandler->mLastCodecError = WmvDecRoutine; \
    if( pHandler->mLastCodecError >= E_WMV9D_CORRUPTED_BITS ) \
    {\
        TST_RESM( TWARN, "%s : %s fails #%d [File: %s, line: %d]", __FUNCTION__, name, pHandler->mLastCodecError, __FILE__, __LINE__-5);\
        return TFAIL;\
    }\
} while(0)   

#define SAFE_DELETE(p) {if(p){Util_Free(p);p=0;}}            

#define GETDATA(b,sz,n) fread((void*)(b),(sz),(n),pHandler->mpInpStream)

/*==================================================================================================
                                   VARIABLES
==================================================================================================*/


/*==================================================================================================
                                   FUNCTION PROTOTYPES
==================================================================================================*/

/* Callbacks. */
WMV9D_S32 WMV_GetNewData( WMV9D_S32 bufSz, 
                          WMV9D_U8 * pBuf, 
                          WMV9D_S32 * pEof, 
                          WMV9D_Void * pAppContext );                             


/* Extra test cases. */
int               RobustnessTestCase( void );  

/* Other. */
int AllocDecMemory( sCodecHandler * pHandler );
void DoDataOutput(sCodecHandler * pHandler);
int ReadHeaderInfo( sCodecHandler * pHandler );


/*==================================================================================================
                                       LOCAL FUNCTIONS
==================================================================================================*/

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
WMV9D_S32 WMV_GetNewData( WMV9D_S32 bufSz, 
                          WMV9D_U8 * pBuf, 
                          WMV9D_S32 * pEof, 
                          WMV9D_Void * pAppContext )
{
        sCodecHandler * pHandler = (sCodecHandler*)pAppContext;
        assert( pHandler );
        
        int bytesRead = -1;
        
        /* The first call is for the sequence data, return that from the handler */
        if( pHandler->mSeqDataSz > 0 )
        {
                assert( bufSz >= pHandler->mSeqDataSz );
                
                memcpy( pBuf, pHandler->mpSeqData, pHandler->mSeqDataSz );
                bytesRead = pHandler->mSeqDataSz;
                pHandler->mSeqDataSz = 0;
                *pEof = 1;                
                SAFE_DELETE( pHandler->mpSeqData );                
                return bytesRead;
        }
        
        if( pHandler->mBytesLeft > bufSz )
        {
                bytesRead = fread( pBuf, 1, bufSz, pHandler->mpInpStream );
                *pEof = 0;
                pHandler->mBytesLeft -= bufSz;
        }
        else
        {
                bytesRead = fread( pBuf, 1, pHandler->mBytesLeft, pHandler->mpInpStream );
                *pEof = 1;
                pHandler->mBytesLeft -= pHandler->mBytesLeft;
        }
        return bytesRead;
}


/*================================================================================================*/
/*================================================================================================*/
int TestEngine( sCodecHandler * pHandler ) 
{
        assert( pHandler );
                
        sWmv9DecObjectType * pDecObj = &pHandler->mDecObj;
        sHandlerParams * pParams = pHandler->mpParams;
        
        
        /*******************/
        /* Open the files. */
        /*******************/

        pHandler->mpInpStream = fopen( pParams->mInpFileName, "rb" );
        if( !pHandler->mpInpStream )
        {
                tst_brkm( TBROK, (void(*)())VT_codec_cleanup,
                        "%s : Can't open input file",                
                        __FUNCTION__ );  
        }

        int hasOutput = Util_StrICmp( pParams->mOutFileName, NA );  
        if( hasOutput )
        {                
                pHandler->mpOutStream = Kev_Open( pParams->mOutFileName, 1 );
                if( !pHandler->mpOutStream )
                {
                        tst_brkm( TBROK, (void(*)())VT_codec_cleanup,
                                "%s : Can't open output file",                
                                __FUNCTION__ );  
                }
        }
        
        /*********************/
        /* Read header info. */
        /*********************/
        
        if( !ReadHeaderInfo( pHandler ) )
        {
                TST_RESM( TWARN, "%s : ReadHeaderInfo failed", __FUNCTION__ );
                return TFAIL;        
        }
        
        
        /*************************************************************************/
        /* Give memory to the decoder for the size, type and alignment returned. */
        /*************************************************************************/        

        if( TPASS != AllocDecMemory( pHandler ) )
                return TFAIL;        
        
        
        /*********************************************************/
        /* Do the initialization of the structure, and call init */
        /*********************************************************/

        pDecObj->pfCbkBuffRead = WMV_GetNewData;
        pDecObj->pvAppContext  = pHandler;
        pDecObj->sDecParam.eCompressionFormat = eWMV9DCompFormat( pHandler->mCompFmtString );
        pDecObj->sDecParam.s32FrameRate   = pHandler->mFrameRate;
        pDecObj->sDecParam.s32BitRate     = pHandler->mBitRate;
        pDecObj->sDecParam.u16FrameWidth  = (WMV9D_U16)pHandler->mWidth;
        pDecObj->sDecParam.u16FrameHeight = (WMV9D_U16)pHandler->mHeight;    
        
        
        /***************************/
        /* Initialize the decoder. */
        /***************************/

        CALL_WMV_DECODER(
                eWMV9DInit( pDecObj ),
                "eWMV9DInit" );
        

        /**********************************/
        /* Set the frame width and height */
        /**********************************/
        
        if( hasOutput )
                Kev_SetDims( pHandler->mpOutStream, pHandler->mWidth, pHandler->mHeight );        
        
             
        /******************************************/
        /* Set the number of frames to be decoded */
        /******************************************/

        if( pHandler->mNumFrames2Decode <= 0 )
                pHandler->mNumFrames2Decode = pHandler->mNumTotalFrames;        
        
                
        /************************************************/
        /* Decode the bitstream and produce the output. */
        /************************************************/
        
        if( RE_ENTRANCE != gTestappConfig.mTestCase &&
            PRE_EMPTION != gTestappConfig.mTestCase &&
            !gTestappConfig.mDisableLCD )
        {
                const sFramebuffer * pFb = GetFramebuffer();
                assert(pFb);
                sColor white;
                white.mAll = 0xffffffff;
                pFb->ClearScreen(&white);
        }

        sWmv9DecParamsType * pDecParams = &pDecObj->sDecParam;
        
        int bytesRead = 0;
        int buffer = 0;
        sWmv9DecYCbCrBufferType * pOutBuf = &pDecObj->sDecParam.sOutputBuffer;
        struct timeval begin, end;
        
        while( pHandler->mFramesCount < pHandler->mNumFrames2Decode )
        {
                gettimeofday( &begin, NULL );
                
                if( pDecParams->s32FrameRate > 0 && !pHandler->mFpsInCfg ) 
                        pHandler->mFpsDelay = ( 1000000. / pDecObj->sDecParam.s32FrameRate );            
                
                if( NOMINAL_FUNCTIONALITY != gTestappConfig.mTestCase ||
                    gTestappConfig.mDisableLCD )
                        pHandler->mCurrentDelay = pHandler->mFpsDelay;
                
                if( pHandler->mCurrentDelay >= pHandler->mFpsDelay )
                {
                        bytesRead = GETDATA( &buffer, 4, 1 );
                        if( bytesRead <= 0 )
                        {
                                TST_RESM( TWARN, "%s : GETDATA failed", __FUNCTION__ );
                                return TFAIL;
                        }        
                        // Read the next frame's extra data
                        if( pHandler->mRcvVersion == 1 )
                        {
                                int timeStamp;           
                                buffer &= 0x3fffffff;
                                bytesRead = GETDATA( &timeStamp, 4, 1 );
                                if( bytesRead != 4 )
                                {
                                        TST_RESM( TWARN, "%s : GETDATA failed", __FUNCTION__ );
                                        return TFAIL;
                                }
                        }
                        
                        // Set the read parameters in the input handler 
                        pHandler->mTotalBytes = pHandler->mBytesLeft = buffer;
                        
                        // Decode the frame 
                        CALL_WMV_DECODER(
                                eWMV9DDecode( pDecObj, buffer ),
                                "eWMV9DDecode" );                        
                        
                        assert( pOutBuf->s32CbRowSize == pOutBuf->s32CrRowSize );
                        
                        // Output the decoded data
                        DoDataOutput( pHandler );
                        
                        /* The frame has been decoded succesful */
                        ++pHandler->mFramesCount;
                                                
                        pHandler->mCurrentDelay = 0;
                }
                
                gettimeofday( &end, NULL );
                int delay = ( end.tv_sec - begin.tv_sec) * 1000000 + 
                        ( end.tv_usec - begin.tv_usec );
                pHandler->mCurrentDelay += delay;                                                        
        }

        TST_RESM( TINFO, "Thread[%lu][%lu] Decoding completed", pHandler->mIndex, pParams->mNoEntry );

        pParams->mIsReadyForBitMatching = TRUE;

        /* Cleanup the handler */
        CleanupHandler( pHandler );
        
        /* Return succees */
        return TPASS;          
}


/*================================================================================================*/
/*================================================================================================*/
void CleanupHandler( sCodecHandler * pHandler )
{   
        int once = FALSE;
        /* Close all the open files */
        if( pHandler->mpInpStream )
        {
                fclose( pHandler->mpInpStream );
                pHandler->mpInpStream = NULL;
                once = TRUE;
        }
        if( pHandler->mpOutStream )
        {       
                Kev_Close( pHandler->mpOutStream );
                pHandler->mpOutStream = NULL;
        }
        
        /* free used memory */
        sWmv9DecObjectType       * pDecObj = &pHandler->mDecObj;
        sWmv9DecMemAllocInfoType * pMemInfo = &pDecObj->sMemInfo;
        int i;
        for( i = 0; i < pMemInfo->s32NumReqs; ++i )
        {
                sWmv9DecMemBlockType * pMemBlk = &pMemInfo->asMemBlks[i];
                if( pMemBlk->s32Size > 0 )
                {
                        SAFE_DELETE( pMemBlk->pvUnalignedBuffer );                        
                }        
        }    
        SAFE_DELETE( pHandler->mpRgbBuffer );

        if( once )
                eWMV9DFree( &pHandler->mDecObj );
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
int IsBitmatchNeeded( sCodecHandler * pHandler )
{
        assert( pHandler );
        assert( pHandler->mpParams );
        return pHandler->mpParams->mIsReadyForBitMatching;                        
}


/*================================================================================================*/
/*================================================================================================*/
void PrintInfo( sCodecHandler * pHandler )
{
        assert( pHandler );
        TST_RESM( TINFO, "Thread[%lu][%lu] Input: %s, Ref: %s", 
                pHandler->mIndex, 
                pHandler->mpParams->mNoEntry,
                pHandler->mpParams->mInpFileName, 
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
        int n = 0;
        strncpy( pParams->mInpFileName, entry[n++], MAX_STR_LEN );   
        strncpy( pParams->mOutFileName, entry[n++], MAX_STR_LEN );
        strncpy( pParams->mRefFileName, entry[n++], MAX_STR_LEN );
        pParams->mDesiredFps = atoi(entry[n++]);
        LList_PushBack( gpParamsList, pParams );
}


/*================================================================================================*/
/*================================================================================================*/
int AllocDecMemory( sCodecHandler * pHandler )
{
        assert( pHandler );
        
        int    i;
        void * pUnalignedBuf;
        void * pOldBuf;
        int    extraSz;
        int    mask;
        int    totalMemSz = 0;
        
        sWmv9DecObjectType       * pDecObj  = &pHandler->mDecObj;
        sWmv9DecMemAllocInfoType * pMemInfo = &pDecObj->sMemInfo;
        
        
        /*****************/
        /* Query memory. */
        /*****************/
        
        CALL_WMV_DECODER(
                eWMV9DQuerymem( pDecObj, pHandler->mHeight, pHandler->mWidth ),
                "eWMV9DQuerymem" );
        
        
        /*********************************/
        /* Allocate the required memory. */
        /*********************************/
        
        for( i = 0; i < pMemInfo->s32NumReqs; ++i )
        {
                sWmv9DecMemBlockType * pMemBlk = pMemInfo->asMemBlks + i;
                
                /* Get the extra amount to be allocated for the alignment */
                switch( pMemBlk->eMemAlign )
                {
                case E_WMV9D_ALIGN_NONE:
                        extraSz = 0;
                        mask = 0xffffffff;
                        break;
                        
                case E_WMV9D_ALIGN_HALF_WORD:
                        extraSz = 1;
                        mask = 0xfffffffe;
                        break;
                        
                case E_WMV9D_ALIGN_WORD:
                        extraSz = 3;
                        mask = 0xfffffffc;
                        break;
                        
                case E_WMV9D_ALIGN_DWORD:
                        extraSz = 7;
                        mask = 0xfffffff8;
                        break;
                        
                case E_WMV9D_ALIGN_QWORD:
                        extraSz = 15;
                        mask = 0xfffffff0;
                        break;
                        
                case E_WMV9D_ALIGN_OCTAWORD:
                        extraSz = 31;
                        mask = 0xffffffe0;
                        break;
                        
                default :
                        extraSz = -1;  /* error condition */
                        mask = 0x00000000;
                        return FALSE;
                        break;
                }
                
                /* Save the old pointer, in case memory is being reallocated */
                pOldBuf = pMemBlk->pvBuffer;
                
                /* Allocate new memory, if required */
                if( pMemBlk->s32Size > 0 )
                {
                        if( WMV9D_IS_SLOW_MEMORY(pMemBlk->s32MemType) )
                                pUnalignedBuf = Util_Malloc( pMemBlk->s32Size + extraSz );
                        else
                                pUnalignedBuf = Util_Malloc( pMemBlk->s32Size + extraSz );
                        
                        pMemBlk->pvBuffer = (void*)
                                (((WMV9D_S32)pUnalignedBuf + extraSz) & mask);
                        totalMemSz += (pMemBlk->s32Size + extraSz);                    
                }
                else
                {
                        pUnalignedBuf = NULL;
                        pMemBlk->pvBuffer = NULL;
                }
                
                /* Check if the memory is being reallocated */
                if( WMV9D_IS_SIZE_CHANGED(pMemBlk->s32MemType) )
                {
                        if( pMemBlk->s32OldSize > 0 )
                        {
                                WMV9D_S32 copySz = pMemBlk->s32OldSize;
                                
                                if( pMemBlk->s32Size < copySz )
                                        copySz = pMemBlk->s32Size;
                                
                                if( WMV9D_NEEDS_COPY_AT_RESIZE(pMemBlk->s32MemType) )
                                        memcpy( pMemBlk->pvBuffer, pMemBlk, copySz );
                                
                                Util_Free( pMemBlk->pvUnalignedBuffer );
                        }
                }
                
                /* Now save the new unaligned buffer pointer */
                pMemBlk->pvUnalignedBuffer = pUnalignedBuf;        
        }
        
        return TPASS;
}


/*================================================================================================*/
/*================================================================================================*/
void DoDataOutput(sCodecHandler * pHandler)
{
        assert( pHandler );
        assert( pHandler->mpOutStream );
        assert( pHandler->mpParams );

        sWmv9DecObjectType * pDecObj = &pHandler->mDecObj;
        sWmv9DecYCbCrBufferType * pOutBuf = &pDecObj->sDecParam.sOutputBuffer;
        
        
        if( pHandler->mpOutStream )
        {                
                /* Write the frame in kev directory */   
                Kev_WriteFrame( pHandler->mpOutStream, pDecObj->sDecParam.u32CurrFrameNum,
                        (unsigned char*)pOutBuf->pu8YBuf, pOutBuf->s32YRowSize,
                        (unsigned char*)pOutBuf->pu8CbBuf, (unsigned char*)pOutBuf->pu8CrBuf,
                        pOutBuf->s32CbRowSize );               
        }
        
        if( !gTestappConfig.mDisableLCD || (
             gTestappConfig.mTestCase  != RE_ENTRANCE &&
             gTestappConfig.mTestCase  != PRE_EMPTION) )
        {              
                if( !pHandler->mpRgbBuffer )                        
                        pHandler->mpRgbBuffer = (unsigned char*)Util_Malloc(pHandler->mWidth*pHandler->mHeight*3);
                if( !pHandler->mpRgbBuffer )
                {
                        tst_brkm( TBROK, (void(*)())VT_codec_cleanup,
                                "%s : Can't allocate memory",                
                                __FUNCTION__ );  
                }

                /* YUV => RGB */
                YCrCbToRGB( (unsigned char*)pOutBuf->pu8YBuf, 
                             pOutBuf->s32YRowSize, 
                             (unsigned char*)pOutBuf->pu8CrBuf, 
                             (unsigned char*)pOutBuf->pu8CbBuf, 
                             pOutBuf->s32CbRowSize, 
                             pHandler->mpRgbBuffer,
                             pHandler->mWidth, pHandler->mHeight, RGB_888 );
                
                /* Draw the RGB frame */                     
                int i;
                const sFramebuffer * pFb = GetFramebuffer();
                assert( pFb );
                int stride = pHandler->mWidth * 3;
                for( i = 0; i < pHandler->mHeight; ++i )
                {
                        unsigned char * ptr = pHandler->mpRgbBuffer + stride * i;                        
                        pFb->DrawScanline( ptr, 0, i, pHandler->mWidth, PF_RGB_888 );                        
                }
        }
}


/*================================================================================================*/
/*================================================================================================*/
int ReadHeaderInfo( sCodecHandler * pHandler )
{
        int bytesRead;
        int buffer;
        int codecVersion, hdrExtn;
               
        sHandlerParams * pParams = pHandler->mpParams;
        assert(pParams);
        
        bytesRead = GETDATA( &buffer, 4, 1 );
        if( bytesRead == 0 )
                return FALSE;        
        
        pHandler->mRcvVersion = (buffer >> 30) & 0x1;
        
        codecVersion = buffer >> 24;
        
        // Number of frames present is in lower 3 bytes 
        pHandler->mNumTotalFrames = buffer & 0xffffff;
        
        hdrExtn = (buffer >> 31) & 0x1;   
        
        if( pHandler->mRcvVersion == 0 )
                codecVersion &= 0x7f;  // no point, bit 30 is already 0
        else
                codecVersion &= 0x3f;
        
        // Set the compression format 
        if( codecVersion == 0 )
        {
                if( gTestappConfig.mVerbose )        
                        TST_RESM( TINFO, "Thread[%lu][%lu] Bitstream is encoded in WMV standard, version 1",
                        pHandler->mIndex, pParams->mNoEntry );                
                pHandler->mCompFmtString = "WMV1";
        }
        else if( codecVersion == 1 )
        {   
                if( gTestappConfig.mVerbose )               
                        TST_RESM( TINFO, "Thread[%lu][%lu] Bitstream is encoded in MPEG4 standard, version 3",
                        pHandler->mIndex, pParams->mNoEntry );                
                pHandler->mCompFmtString = "MP43";                
        }
        else if( codecVersion == 2 )
        {
                if( gTestappConfig.mVerbose )               
                        TST_RESM( TINFO, "Thread[%lu][%lu] Bitstream is encoded in WMV standard, version 2",
                        pHandler->mIndex, pParams->mNoEntry );                
                pHandler->mCompFmtString = "WMV2";                               
        }
        else if( codecVersion == 3 )
        {
                if( gTestappConfig.mVerbose )               
                        TST_RESM( TINFO, "Thread[%lu][%lu] Bitstream is encoded in MPEG4 standard, version 2",
                        pHandler->mIndex, pParams->mNoEntry );                
                pHandler->mCompFmtString = "MP42";                     
        }
        else if( codecVersion == 4 )
        {
                if( gTestappConfig.mVerbose )               
                        TST_RESM( TINFO, "Thread[%lu][%lu] Bitstream is encoded in MPEG4 standard, MPEG4 with short video header",
                        pHandler->mIndex, pParams->mNoEntry );                
                pHandler->mCompFmtString = "MP4S";                   
        }
        else if( codecVersion == 5 )
        {
                if( gTestappConfig.mVerbose )               
                        TST_RESM( TINFO, "Thread[%lu][%lu] Bitstream is encoded in WMV standard, version 3",
                        pHandler->mIndex, pParams->mNoEntry );                
                pHandler->mCompFmtString = "WMV3";                       
        }
        else
        {
                pHandler->mCompFmtString = NULL;        
                return FALSE;
        }
        
        // Read any extension of the header, if present 
        // Actually this is the sequence layer data        
        if( hdrExtn != 0 )
        {
                bytesRead = GETDATA( &pHandler->mSeqDataSz, 4, 1 );
                if( bytesRead == 0 )
                        return FALSE;                
                
                pHandler->mpSeqData = Util_Malloc( pHandler->mSeqDataSz );                
                if( !pHandler->mpSeqData ) 
                {
                        tst_brkm( TBROK, (void(*)())VT_codec_cleanup,
                                "%s : Can't allocate memory",                
                                __FUNCTION__ );  
                }
                
                bytesRead = GETDATA( pHandler->mpSeqData, 1, pHandler->mSeqDataSz );
                if( bytesRead == 0 )
                        return FALSE;                
        }
        
        // Get the height of the frames
        if( GETDATA( &pHandler->mHeight, 4, 1 ) == 0 )
                return FALSE;                
        if( pHandler->mHeight <= 0 )
                return FALSE;        
        
        // Get the width of the frames 
        if( GETDATA( &pHandler->mWidth, 4, 1 ) == 0 )
                return FALSE;                
        if( pHandler->mWidth <= 0 )
                return FALSE;        
        
        if( pHandler->mRcvVersion == 1 ) 
        {
                WMV9D_S32  rcvAdditionalHeaderSz, preRoll;
                
                // additional header size
                if( GETDATA( &rcvAdditionalHeaderSz, 4, 1 ) == 0 )
                        return FALSE;                
                
                if( GETDATA( &preRoll, 4, 1 ) == 0 )
                        return FALSE;                
                
                preRoll &= 0x0fffffff;
                
                // Bit rate, 4 bytes 
                if( GETDATA( &pHandler->mBitRate, 4, 1 ) == 0 )
                        return FALSE;                
                
                // Frame rate, 4 bytes 
                if( GETDATA( &pHandler->mFrameRate, 4, 1 ) == 0 )
                        return FALSE;                
        }
        else
        {
                // Set to some default value 
                pHandler->mBitRate = 0;
                pHandler->mFrameRate = 0;
        } 
        
        return TRUE;
}
