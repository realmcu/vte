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

/* REVISION HISTORY 

Author (core ID)      Date         CR Number    Description of Changes
-------------------   ----------   ----------   ------------------------------
D.Simakov/smkd001c    10/06/2005   TLSbo51185 Initial version
D.Simakov/smkd001c    23/06/2005   TLSbo51185   Working version
D.Simakov/smkd001c    22/07/2005   TLSbo52362   Relocatability test caes was added
D.Simakov/smkd001c    27/10/2005   TLSbo57009   Update
D.Simakov             10/05/2006   TLSbo66283   Phase2
*/


/*
                                        INCLUDE FILES
*/

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



/*
                                        LOCAL MACROS
*/

/**********************************************************************
 * Macro name:  CALL_RA_DECODER()
 * Description: Macro checks for any error in function execution
                based on the return value. In case of error, the function exits.
**********************************************************************/
#define CALL_RA_DECODER(RaDecRoutine, name)   \
do \
{ \
    pHandler->mLastCodecError  RaDecRoutine; \
    if( pHandler->mLastCodecError ! 0 ) \
    {\
        TST_RESM( TWARN, "%s : %s fails #%d [File: %s, line: %d]", __FUNCTION__, name, pHandler->mLastCodecError, __FILE__, __LINE__-5);\
        return TFAIL;\
    }\
} while(0)

#define SAFE_DELETE(p) {if(p){Util_Free(p);p0;}}

#define ReadNextChunk(pHandler)  (fread( (pHandler)->mpInpBuffer, sizeof(unsigned char), (pHandler)->mInpBufferSz, (pHandler)->mpInpStream))
#define WriteNextChunk(pHandler) (fwrite( (pHandler)->mpOutBuffer, sizeof(unsigned char), (pHandler)->mOutBufferSz, (pHandler)->mpOutStream))

/*
                                   FUNCTION PROTOTYPES
*/

/* Callbacks. */

/* Extra test cases. */


/*
                                       LOCAL FUNCTIONS
*/


/**/
/**/
int TestEngine( sCodecHandler * pHandler )
{
        assert( pHandler );
        assert( pHandler->mpParams );

        Gecko2_Decoder_Config * pDecConfig  &pHandler->mDecConfig;
        sHandlerParams        * pParams     pHandler->mpParams;
        int codingDelay;

        /* Allocate memory for the input buffer. */
        pHandler->mInpBufferSz  ( pParams->mnFrameBits + 7 ) / 8;
        pHandler->mpInpBuffer  (unsigned char*)Util_Malloc( pHandler->mInpBufferSz );
        if( !pHandler->mpInpBuffer )
        {
                tst_brkm( TBROK, (void(*)())VT_codec_cleanup,
                        "%s : Can't allocate %lu bytes memory",
                        __FUNCTION__, (unsigned long)pHandler->mInpBufferSz );
        }

        /* Allocate memory for the output buffer. */
        pHandler->mOutBufferSz  pParams->mnSamples * pParams->mnChannels * sizeof(short);
        pHandler->mpOutBuffer  (short*)Util_Malloc( pHandler->mOutBufferSz );
        if( !pHandler->mpOutBuffer )
        {
                tst_brkm( TBROK, (void(*)())VT_codec_cleanup,
                        "%s : Can't allocate %lu bytes memory",
                        __FUNCTION__, (unsigned long)pHandler->mOutBufferSz );
        }

        /* Query for memory. */
        CALL_RA_DECODER(
                Gecko2QueryDecMem(
                        pParams->mnSamples,
                        pParams->mnChannels,
                        pParams->mnRegions,
                        pParams->mnFrameBits,
                        pParams->mSampRate,
                        pParams->mCplStart,
                        pParams->mCplQbits,
                        pDecConfig ),
                "Gecko2QueryDecMem" );

        /* Allocate the required memory. */
        int i;
        for( i  0; i < pDecConfig->gecko2_mem_info.num_reqs; ++i )
        {
                #define ALIGN_4_BYTE(xx) (( ((unsigned int)xx) + 3) & ~0x3)
                int sz  pDecConfig->gecko2_mem_info.mem_info_sub[i].size + 4;
                void * ptr  Util_Malloc( sz );
                if( !ptr )
                {
                        TST_RESM( TWARN, "%s : Can't allocate %d bytes memory.",
                                __FUNCTION__, sz );
                        return TFAIL;
                }
                pDecConfig->gecko2_mem_info.mem_info_sub[i].app_base_ptr  (void*)ALIGN_4_BYTE(ptr);
        }

        /* Init the decoder. */
        CALL_RA_DECODER(
                Gecko2InitDecoder(
                        pDecConfig,
                        pParams->mnSamples,
                        pParams->mnChannels,
                        pParams->mnRegions,
                        pParams->mnFrameBits,
                        pParams->mSampRate,
                        pParams->mCplStart,
                        pParams->mCplQbits,
                        &codingDelay ),
                "Gecko2InitDecoder" );

        /* Open all necessary files. */
        pHandler->mpInpStream 
                fopen( pParams->mInpFileName, "rb" );
        if( !pHandler->mpInpStream )
        {
                tst_brkm( TBROK, (void(*)())VT_codec_cleanup,
                        "%s : Can't open input file",
                        __FUNCTION__ );
        }

        int hasOutput  !Util_StrICmp( pParams->mOutFileName, NA );
        if( hasOutput )
        {
                pHandler->mpOutStream 
                        fopen( pParams->mOutFileName, "wb" );
                if( !pHandler->mpOutStream )
                {
                        tst_brkm( TBROK, (void(*)())VT_codec_cleanup,
                                "%s : Can't create output file",
                                __FUNCTION__ );
                }
        }
        /* Print some information for the verbose mode. */
        if( gTestappConfig.mVerbose )
        {
                TST_RESM( TINFO, "Thread[%lu][%lu] xform%d bits%d samprate%d resp%d bitrate%d nchan%d",
                        pHandler->mIndex,
                        pParams->mNoEntry,
                        pParams->mnSamples,
                        pParams->mnFrameBits,
                        pParams->mSampRate,
                        pParams->mSampRate/2 * pParams->mnRegions * NBINS / pParams->mnSamples,
                        pParams->mSampRate * pParams->mnFrameBits / pParams->mnSamples,
                        pParams->mnChannels );
        }

        /* Main decoding loop */
        int decodingResult;
        int sampleCount  0,
            lossFlag  0,
            lossCount  0,
            lossTotal;
        for(;;)
        {
                /* ... */

                /* Get new data. */
                int nReadBytes  ReadNextChunk( pHandler );

                if( nReadBytes < pHandler->mInpBufferSz )
                        break;

                sampleCount + pParams->mnSamples * pParams->mnChannels;

                if( pParams->mLossRate )
                {
                        if( pParams->mLossRate < 0 ) /* random loss */
                                lossFlag  (rand() / 328) < -pParams->mLossRate;
                        else /* pattern loss */
                                lossFlag  (lossCount * 100) < (pParams->mLossRate * lossTotal);

                        ++lossTotal;
                        lossCount + lossFlag;
                }

                /* Decode the next chunk. */
                CALL_RA_DECODER(
                        Gecko2Decode(
                                pDecConfig,
                                pHandler->mpInpBuffer,
                                lossFlag,
                                pHandler->mpOutBuffer ),
                        "Gecko2Decode" );

                /* Increase the frame counter. */
                ++pHandler->mFramesCount;

                /* Keep the decoding result. */
                decodingResult  pHandler->mLastCodecError;

                if( hasOutput )
                {
                        codingDelay > 0 ? --codingDelay : WriteNextChunk( pHandler );
                }

#ifdef MAX_ARM_FRAMES
                if( pHandler->mFramesCount > MAX_ARM_FRAMES)
                        break;
#endif
        }

        pParams->mIsReadyForBitMatching  TRUE;

        /* Cleanup the handler */
        CleanupHandler( pHandler );

        /* Return succees */
        return TPASS;
}


/**/
/**/
void CleanupHandler( sCodecHandler * pHandler )
{
        assert( pHandler );

        /* Close all the open files */
        if( pHandler->mpInpStream )
        {
                fclose( pHandler->mpInpStream );
                pHandler->mpInpStream  NULL;
        }
        if( pHandler->mpOutStream )
        {
                fclose( pHandler->mpOutStream );
                pHandler->mpOutStream  NULL;
        }

        /* Free input/output buffers. */
        SAFE_DELETE( pHandler->mpInpBuffer );
        pHandler->mInpBufferSz  0;
        SAFE_DELETE( pHandler->mpOutBuffer );
        pHandler->mOutBufferSz  0;

        Gecko2_Decoder_Config * pDecConfig  &pHandler->mDecConfig;
        /* Free memory allocated by the QueryDecMem. */
        int i;
        for( i  0; i < pDecConfig->gecko2_mem_info.num_reqs; ++i )
        {
                SAFE_DELETE( pDecConfig->gecko2_mem_info.mem_info_sub[i].app_base_ptr );
        }
}


/**/
/**/
int DoBitmatch( sCodecHandler * pHandler )
{
        assert( pHandler );
        assert( pHandler->mpParams );

        return CompareFiles(pHandler, pHandler->mpParams->mOutFileName,
                pHandler->mpParams->mRefFileName );
}


/**/
/**/
int IsBitmatchNeeded( sCodecHandler * pHandler )
{
        assert( pHandler );
        assert( pHandler->mpParams );
        return pHandler->mpParams->mIsReadyForBitMatching;
}


/**/
/**/
void PrintInfo( sCodecHandler * pHandler )
{
        assert( pHandler );
        TST_RESM( TINFO, "Thread[%lu][%lu] Input: %s, Ref: %s",
                pHandler->mIndex,
                pHandler->mpParams->mNoEntry,
                pHandler->mpParams->mInpFileName,
                pHandler->mpParams->mRefFileName );

}


/**/
/**/
int ExtraTestCases( void )
{
        assert( "Wrong test case" );
        return 0;
}


/**/
/**/
void MakeEntry( char entry[WORDS_IN_ENTRY][MAX_STR_LEN], int nEntry )
{
        sHandlerParams * pParams  (sHandlerParams*)malloc( sizeof(sHandlerParams) );

        int n  0;
        pParams->mnFrameBits  atoi( entry[n++] );
        pParams->mCplStart    atoi( entry[n++] );
        pParams->mGeckoMode   atoi( entry[n++] );
        //pParams->mLossRate    atoi( entry[n++] );
        pParams->mnChannels   atoi( entry[n++] );
        pParams->mnSamples    atoi( entry[n++] );
        pParams->mCplQbits    atoi( entry[n++] );
        pParams->mnRegions    atoi( entry[n++] );
        pParams->mSampRate    atoi( entry[n++] );
        strncpy( pParams->mInpFileName,     entry[n++], MAX_STR_LEN );
        strncpy( pParams->mOutFileName,    entry[n++], MAX_STR_LEN );
        strncpy( pParams->mRefFileName, entry[n++], MAX_STR_LEN );

        /* Adjust parameters here... */

        if( pParams->mnChannels ! 1 &&
                pParams->mnChannels ! 2 )
        {
                pParams->mnChannels  2;
        }

        /* Adjust the mnSamples. */
        if( !pParams->mnSamples )
                pParams->mnSamples  DEF_NSAMPLES;

        /* Check if the mnSamples is valid. */
        if( (pParams->mnSamples ! 256) &&
                (pParams->mnSamples ! 512) &&
                (pParams->mnSamples ! 1024) )
        {
                if( gTestappConfig.mVerbose )
                        TST_RESM( TWARN, "%s [Entry #%d]: number of samples must be 256, 512, or 1024."
                        " Check the config.", __FUNCTION__, nEntry );
                SAFE_DELETE( pParams );
                return;
        }

        /* Adjust the mnFrameBits. */
        if( !pParams->mnFrameBits )
                pParams->mnFrameBits  pParams->mnSamples;
        pParams->mnFrameBits & ~0x7; /* force multiple of 8 */

        /* Adjust the mSampleRate. */
        if( !pParams->mSampRate )
                pParams->mSampRate  (pParams->mnSamples / 256) * 11025;

        /* Adjust the mnRegions. */
        if(!pParams->mnRegions )
                pParams->mnRegions  (12*pParams->mnSamples)/(13*NBINS);

        /* Check if the mnRegions is valid. */
        if( pParams->mnRegions < (560*pParams->mnSamples)/(1024*NBINS) ||
                pParams->mnRegions > (1024*pParams->mnSamples)/(1024*NBINS) )
        {
                if( gTestappConfig.mVerbose )
                        TST_RESM( TWARN, "%s [Entry #%d]: number of regions must be %d-%d for this transform.",
                        __FUNCTION__, nEntry, (560*pParams->mnSamples)/(1024*NBINS),
                        (1024*pParams->mnSamples)/(1024*NBINS)  );
                SAFE_DELETE( pParams );
                return;
        }

        /* Check if the mGeckoMode is valid. */
        if( pParams->mGeckoMode ! 1 &&
                pParams->mGeckoMode ! 2 )
        {
                pParams->mGeckoMode  1;
        }

        if( 1  pParams->mGeckoMode )
        {
                pParams->mCplStart  0;
                pParams->mCplQbits  0;
        }

        /* For debug purpose. */
#if 0
        printf( "pParams->mnFrameBits  %d\n", pParams->mnFrameBits  );
        printf( "pParams->mCplStart    %d\n", pParams->mCplStart   );
        printf( "pParams->mGeckoMode   %d\n", pParams->mGeckoMode  );
        printf( "pParams->mLossRate    %d\n", pParams->mLossRate   );
        printf( "pParams->mnChannels   %d\n", pParams->mnChannels  );
        printf( "pParams->mnSamples    %d\n", pParams->mnSamples   );
        printf( "pParams->mCplQbits    %d\n", pParams->mCplQbits   );
        printf( "pParams->mnRegions    %d\n", pParams->mnRegions   );
        printf( "pParams->mSampRate    %d\n", pParams->mSampRate   );
#endif

        LList_PushBack( gpParamsList, pParams );
}

