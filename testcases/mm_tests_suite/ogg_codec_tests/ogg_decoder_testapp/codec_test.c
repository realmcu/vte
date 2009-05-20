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
D.Simakov             24/01/2006   TLSbo61035   Initial version
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
#include <util/fb_draw_api.h>


/*
                                        LOCAL MACROS
*/

/**********************************************************************
 * Macro name:  CALL_OGG_DECODER()
 * Description: Macro checks for any error in function execution
                based on the return value. In case of error, the function exits.
**********************************************************************/
#define CALL_OGG_DECODER(OggDecRoutine, name)   \
do \
{ \
    pHandler->mLastCodecError  OggDecRoutine; \
    if( pHandler->mLastCodecError < 0 ) \
    {\
        TST_RESM( TWARN, "%s : %s fails #%d [File: %s, line: %d]", __FUNCTION__, name, pHandler->mLastCodecError, __FILE__, __LINE__-5);\
        return TFAIL;\
    }\
} while(0)

#define SAFE_DELETE(p) {if(p){Util_Free(p);p0;}}

#define DPRINTF(fmt,...) do{printf("%s:%d %s() ", __FILE__, __LINE__, __FUNCTION__); printf((fmt), ##__VA_ARGS__); fflush(stdout);}while(0)

/*
                                   FUNCTION PROTOTYPES
*/

/* Callbacks. */
size_t  Ogg_fread                     ( void * pBuf, size_t num, size_t sz, void * pDataSrc );
int     Ogg_fseek                     ( void * pDataSrc, ogg_int64_t offset, int whence );
int     Ogg_fclose                    ( void * pDataSrc );
long    Ogg_ftell                     ( void * pDataSrc );

/* Extra test cases. */
int     RobustnessTestCase            ( void );


/*
                                       LOCAL FUNCTIONS
*/

/**/
/**/
size_t Ogg_fread( void * pBuf, size_t num, size_t sz, void * pDataSrc )
{
        sCodecHandler * pHandler  (sCodecHandler*)pDataSrc;
        int bytesRd  num*sz;

        if( bytesRd < pHandler->mFileSz - pHandler->mBytesRd )
        {
                memcpy(pBuf, pHandler->mpBuff, bytesRd );
                pHandler->mBytesRd + bytesRd;
                pHandler->mpBuff + bytesRd;
        }
        else
        {
                bytesRd  pHandler->mFileSz - pHandler->mBytesRd;
                memcpy( pBuf, pHandler->mpBuff, bytesRd );
                pHandler->mBytesRd + bytesRd;
                pHandler->mpBuff + bytesRd;

        }
        return bytesRd;
}


/**/
/**/
int Ogg_fseek( void * pDataSrc, ogg_int64_t offset, int whence )
{
        sCodecHandler * pHandler  (sCodecHandler*)pDataSrc;
        assert(pHandler);

        int bytesRd  (int)(offset+whence);
        if( bytesRd  1 )
        {
                pHandler->mBytesRd  0;
                pHandler->mpBuff  pHandler->mpBuffHead;
        }
        else
        {
                pHandler->mBytesRd  bytesRd;
                pHandler->mpBuff  pHandler->mpBuffHead;
        }
        return 0;
}


/**/
/**/
int Ogg_fclose( void * pDataSrc )
{
        sCodecHandler * pHandler  (sCodecHandler*)pDataSrc;
        assert(pHandler);
        assert(pHandler->mpInpStream);
        int ret  0;
        if(pHandler)
        {
                ret  fclose(pHandler->mpInpStream);
                pHandler->mpInpStream  NULL;
        }
        SAFE_DELETE(pHandler->mpBuffHead);
        pHandler->mpBuff  NULL;
        return ret;
}


/**/
/**/
long Ogg_ftell( void * pDataSrc )
{
        sCodecHandler * pHandler  (sCodecHandler*)pDataSrc;
        assert(pHandler);
        return pHandler->mBytesRd;
}


/**/
/**/
int RobustnessTest( void )
{
        sLinkedList * pNode;
        int i;
        int rv  TPASS;
        sCodecHandler * pHandler  gCodecHandler;


        /***********************/
        /* For the each entry. */
        /***********************/

        for( pNode  gpParamsList, i  0; pNode; pNode  pNode->mpNext, ++i )
        {
                /* Reset the handler. */
                ResetHandler( pHandler );

                /* Get content. */
                pHandler->mpParams  (sHandlerParams*)pNode->mpContent;

                if( gTestappConfig.mVerbose )
                {
                        PrintInfo( pHandler );
                }

                /* Run the Decoder. */
                int ret  RunCodec( pHandler );
                if( TPASS  ret )
                {
                        if( gTestappConfig.mVerbose )
                                TST_RESM( TFAIL, "Thread[%lu][%lu] Robustness failed",
                                          pHandler->mIndex, pHandler->mpParams->mNoEntry );
                        rv  TFAIL;
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


/**/
/**/
int TestEngine( sCodecHandler * pHandler )
{
        assert( pHandler );
        assert( pHandler->mpParams );

        sHandlerParams         * pParams         pHandler->mpParams;
        sOggVorbisDecObj       * pDecObj         &pHandler->mDecObject;


        /*****************************/
        /* Open all necessary files. */
        /*****************************/

        pHandler->mpInpStream  fopen( pParams->mInpFileName, "rb" );
        if( !pHandler->mpInpStream )
        {
                TST_RESM( TWARN, "%s : Can't open input file \'%s\'",
                        __FUNCTION__, pParams->mInpFileName );
                return TFAIL;
        }

        pHandler->mpOutStream  fopen( pParams->mOutFileName, "wb" );
        if( !pHandler->mpOutStream )
        {
                TST_RESM( TWARN, "%s : Can't create output file \'%s\'",
                        __FUNCTION__, pParams->mOutFileName );
                return TFAIL;
        }

        pHandler->mpBuffHead  Util_ReadFile( pParams->mInpFileName, &pHandler->mFileSz );
        if( !pHandler->mpBuffHead )
        {
                TST_RESM( TWARN, "%s : Can't read input file",
                        __FUNCTION__ );
                return TFAIL;
        }
        pHandler->mBytesRd  0;
        pHandler->mpBuff   pHandler->mpBuffHead;


        /*********************************/
        /* Make alignment output buffer. */
        /*********************************/

        pHandler->mpOutBufferA  (unsigned char *)(((unsigned int)pHandler->mpOutBuffer + 3) & ~0x3);

        /*****************************************/
        /* Initialization of the decoder object. */
        /*****************************************/

        pDecObj->datasource     (void *)pHandler->mpBuff;
        pDecObj->read_func      Ogg_fread;
        pDecObj->seek_func      Ogg_fseek;
        pDecObj->close_func     Ogg_fclose;
        pDecObj->tell_func      Ogg_ftell;
        pDecObj->pcmout         (char*)pHandler->mpOutBufferA;
        pDecObj->output_length  CHUNK_SIZE;


        /********************************/
        /* Query & allocate the memory. */
        /********************************/

        CALL_OGG_DECODER(
                OggVorbisQueryMem(pDecObj),
                "OggVorbisQueryMem" );

        pDecObj->pvOggDecObj  (void *)Util_Malloc(pDecObj->OggDecObjSize);
        if( !pDecObj->pvOggDecObj )
        {
                tst_brkm( TBROK, (void(*)())VT_codec_cleanup,
                        "%s : Can't allocate %lu bytes memory",
                        __FUNCTION__, (unsigned long)pDecObj->OggDecObjSize );
        }
        memset( pDecObj->pvOggDecObj, 0, pDecObj->OggDecObjSize );

        // Decoder internal buffer allocation.
        pDecObj->buf_size  pHandler->mLastCodecError;
        pDecObj->decoderbuf  (unsigned char*)Util_Malloc(pDecObj->buf_size);

        if( !pDecObj->decoderbuf )
        {
                tst_brkm( TBROK, (void(*)())VT_codec_cleanup,
                        "%s : Can't allocate %lu bytes memory",
                        __FUNCTION__, (unsigned long)pDecObj->buf_size );
        }
        memset( pDecObj->decoderbuf, 0, pDecObj->buf_size );
        pDecObj->datasource  (void*)pHandler;


        /***************************/
        /* Initialize the decoder. */
        /***************************/

        CALL_OGG_DECODER(
                OggVorbisDecoderInit(pDecObj),
                "OggVorbisDecoderInit" );
        if( 0 ! pHandler->mLastCodecError )
        {
                TST_RESM(TWARN, "Thread[%lu][%lu] Input does not appear to be an Ogg bitstream",
                        pHandler->mIndex, pParams->mNoEntry );
        }


#ifdef DEBUG_TEST
        /* Current memory used by the app. */
        if( NOMINAL_FUNCTIONALITY  gTestappConfig.mTestCase )
                MemStat_GetStat();
#endif

        /******************/
        /* Decoding loop. */
        /******************/

        for(;;)
        {
                CALL_OGG_DECODER(
                        OggVorbisDecode(pDecObj),
                        "OggVorbisDecode" );

                if( 0  pHandler->mLastCodecError )
                {
                        // eof
                        break;
                }
                else
                {
                        size_t sz  (size_t)pHandler->mLastCodecError;
                        fwrite( pHandler->mpOutBufferA, 1, sz, pHandler->mpOutStream );
                }
        }

        pHandler->mpParams->mIsReadyForBitMatching  TRUE;

        /************************/
        /* Cleanup the handler. */
        /************************/

        CleanupHandler( pHandler );

#ifdef DEBUG_TEST
        if( NOMINAL_FUNCTIONALITY  gTestappConfig.mTestCase )
                MemStat_GetStat();
#endif

        /* Return success. */
        return TPASS;
}


/**/
/**/
void CleanupHandler( sCodecHandler * pHandler )
{
        assert( pHandler );
        int needCleanup  FALSE;


        /******************************/
        /* Close all files we opened. */
        /******************************/

        if( pHandler->mpInpStream )
        {
                fclose( pHandler->mpInpStream );
                pHandler->mpInpStream  NULL;
                needCleanup  TRUE;
        }

        if( pHandler->mpOutStream )
        {
                fclose( pHandler->mpOutStream );
                pHandler->mpOutStream  NULL;
        }


        /********************************/
        /* Free the buffers we've used. */
        /********************************/

        SAFE_DELETE( pHandler->mDecObject.decoderbuf );
        SAFE_DELETE( pHandler->mDecObject.pvOggDecObj );
        SAFE_DELETE( pHandler->mpBuffHead );


        /*******************************/
        /* Cleanup the decoder (once). */
        /*******************************/

        if( needCleanup )
                OggVorbisCleanup(&pHandler->mDecObject);
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
        assert( ROBUSTNESS  gTestappConfig.mTestCase && "Wrong test case" );
        return RobustnessTest();
}


/**/
/**/
void MakeEntry( char entry[WORDS_IN_ENTRY][MAX_STR_LEN], int nEntry )
{
        sHandlerParams * pParams  (sHandlerParams*)malloc( sizeof(sHandlerParams) );

        int n  0;
        strncpy( pParams->mInpFileName,  entry[n++], MAX_STR_LEN );
        strncpy( pParams->mOutFileName,  entry[n++], MAX_STR_LEN );
        strncpy( pParams->mRefFileName,  entry[n++], MAX_STR_LEN );

        pParams->mNoEntry  nEntry;


        /****************************/
        /* Adjust/check parameters. */
        /****************************/

        if( !Util_StrICmp( pParams->mOutFileName, NA ) )
        {
                tst_brkm( TBROK, (void(*)())VT_codec_cleanup,
                        "Wrong output file name %s. The output file name must not be %s."
                        "Please check %s, line/entry #%lu",
                        pParams->mOutFileName, NA, gTestappConfig.mConfigFilename, nEntry + 1 );
        }

        LList_PushBack( gpParamsList, pParams );
}

