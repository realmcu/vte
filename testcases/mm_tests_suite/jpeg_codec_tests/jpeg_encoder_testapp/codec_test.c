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
F.GAFFIE/rb657c       03/05/2004   TLSbo39336   Initial version
D.Simakov/smkd001c    07/02/2005   TLSbo47179   Bad dependancies in the mm tests
                                                application build process
D.Simakov/smkd001c    25/10/2005   TLSbo59191   Improved
D.Simakov/smkd001c    24/01/2006   TLSbo61035   Centralization of common features
*/

/*
                                        INCLUDE FILES
*/

/* Standard Include Files. */
#include <errno.h>
#include <stdio.h>
#include <assert.h>
#include <pthread.h>

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
* Macro name:  CALL_JPEG_ENCODER()
* Description: Macro checks for any error in function execution
               based on the return value. In case of error, the function exits.
**********************************************************************/
#define CALL_JPEG_ENCODER(JpegEncRoutine, name)   \
do \
{ \
    pHandler->mLastCodecError  JpegEncRoutine; \
    if( pHandler->mLastCodecError > JPEG_ENC_ERR_RECL_START ) \
    {\
        TST_RESM( TWARN, "%s : %s fails #%d [File: %s, line: %d]", __FUNCTION__, name, pHandler->mLastCodecError, __FILE__, __LINE__-5);\
        return TFAIL;\
    }\
} while(0)

/*
                                   FUNCTION PROTOTYPES
*/

/* Callbacks. */
JPEG_ENC_UINT8 Jpeg_PushOutput ( JPEG_ENC_UINT8 ** ppOutBuf,
                                 JPEG_ENC_UINT32 * pOutBufLen,
                                 JPEG_ENC_UINT8    flush,
                                 void *            pEncObject,
                                 JPEG_ENC_MODE     EncMode );

/* Extra test cases. */
int ThumbEncodingTest          ( void );

/* Helper functions. */
void FillExifParams            ( jpeg_enc_exif_parameters * pParams );
void FillJpegEncoderParams     ( jpeg_enc_parameters * pEncParams,
                                 const sHandlerParams * pHandlerParams );

/*
                                       FUNCTIONS
*/

/**/
/**/
JPEG_ENC_UINT8 Jpeg_PushOutput( JPEG_ENC_UINT8 ** ppOutBuf,
                                JPEG_ENC_UINT32 * pOutBufLen,
                                JPEG_ENC_UINT8    flush,
                                void *            pEncObject,
                                JPEG_ENC_MODE     EncMode )
{
        JPEG_ENC_UINT32 i;
        sCodecHandler * pHandler  NULL;

        /*********************************/
        /* Find the appropriate handler. */
        /*********************************/
        for( i  0; i < MAX_THREADS; ++i )
        {
                if( &gCodecHandler[i].mEncObject  pEncObject )
                {
                        pHandler  &gCodecHandler[i];
                        break;
                }
        }
        assert( pHandler );

        if( !(*ppOutBuf) )
        {
                /* This function is called for the 1'st time from the
                 * codec */
                *ppOutBuf  pHandler->mpOutBuffer;
                *pOutBufLen  OUTP_BUFFER_SIZE;
        }
        else if( 1  flush )
        {
                /* Flush the buffer*/
                /* This example code flushes the buffer into a file */
                /* File Write to be commented while profiling */
                for( i  0; i < *pOutBufLen; ++i )
                {
                        fputc( *(*ppOutBuf + i), pHandler->mpOutputStream );
                }
        }
        else
        {
                /* File Write to be commented while profiling */

                /* This example code flushes the buffer into a file */
                for( i  0; i < OUTP_BUFFER_SIZE; ++i )
                {
                        fputc( *(*ppOutBuf + i),  pHandler->mpOutputStream );
                }
                /* Now provide a new buffer */
                *ppOutBuf  pHandler->mpOutBuffer;
                *pOutBufLen  OUTP_BUFFER_SIZE;
        }
        return 1; /* Success */
}


/**/
/**/
int ThumbEncodingTestCore( sCodecHandler * pHandler1, sCodecHandler * pHandler2 )
{
        assert( pHandler1 && pHandler2 );
        assert( pHandler1->mpParams && pHandler2->mpParams );
        assert( JPEG_ENC_MAIN   pHandler1->mpParams->mMode &&
                JPEG_ENC_THUMB  pHandler2->mpParams->mMode );

        /* Run TestEngine. */
        pHandler1->mLtpRetval  TestEngine( pHandler1 );
        pHandler2->mLtpRetval  TestEngine( pHandler2 );

        /* Make the final JPEG. */
        const char * fileName1  pHandler1->mpParams->mOutFileName;
        const char * fileName2  pHandler2->mpParams->mOutFileName;
        char finalFilename[MAX_STR_LEN];
        strncpy( finalFilename, fileName1, MAX_STR_LEN );
        strncat( finalFilename, ".jpg", MAX_STR_LEN );
        char cmd[MAX_STR_LEN];
        sprintf( cmd, "cat %s %s > %s", fileName2, fileName1, finalFilename );
        if( -1  system( cmd ) )
        {
                tst_brkm( TBROK, (void(*)())VT_codec_cleanup,
                        "%s : Can't create file \'%s\'",
                        __FUNCTION__, finalFilename );
        }

        /* Prepare for bit-matching. */
        strncat( pHandler1->mpParams->mOutFileName, ".jpg", MAX_STR_LEN );
        pHandler2->mpParams->mIsReadyForBitMatching  FALSE;

#ifndef POST_BIT_MATCHING
        /* Perform bit-matching */
        fileName1  pHandler1->mpParams->mOutFileName;
        fileName2  pHandler1->mpParams->mRefFileName;
        if( DoFilesExist( fileName1, fileName2 ) )
        {
                if( !DoBitmatch( pHandler1 ) )
                {
                        if( gTestappConfig.mVerbose )
                                TST_RESM( TFAIL, "Thread[%lu][%lu] Bitmatch failed (%s vs %s)",
                                pHandler1->mIndex, pHandler1->mpParams->mNoEntry, fileName1, fileName2 );
                        pHandler1->mLtpRetval  TFAIL;
                        pHandler2->mLtpRetval  TFAIL;
                }
                else
                {
                        if( gTestappConfig.mVerbose )
                                TST_RESM( TINFO, "Thread[%lu][%lu] Bitmatch passed (%s vs %s)",
                                pHandler1->mIndex, pHandler1->mpParams->mNoEntry, fileName1, fileName2 );
                }
        }
#endif

        /* Return LTP result. */
        return TPASS  pHandler1->mLtpRetval && TPASS  pHandler2->mLtpRetval ? TPASS : TFAIL;
}


/**/
/**/
int ThumbEncodingTest()
{
        sLinkedList * pNode;
        int i;
        int rv  TPASS;
        sCodecHandler * pHandler1  &gCodecHandler[0];
        sCodecHandler * pHandler2  &gCodecHandler[1];

        /* For the each entry */
        for( pNode  gpParamsList, i  0; pNode; pNode  pNode->mpNext, ++i )
        {
                if( !pNode->mpNext )
                        break;

                /* Reset the handlers. */
                ResetHandler( pHandler1 );
                ResetHandler( pHandler2 );

                /* Get content. */
                pHandler1->mpParams  (sHandlerParams*)pNode->mpContent;
                pHandler2->mpParams  (sHandlerParams*)pNode->mpNext->mpContent;

                /* Check... */
                if( JPEG_ENC_THUMB  pHandler1->mpParams->mMode &&
                        JPEG_ENC_MAIN   pHandler2->mpParams->mMode )
                {
                        sCodecHandler * pHandler  pHandler1;
                        pHandler1  pHandler2;
                        pHandler2  pHandler;
                }
                else if( pHandler1->mpParams->mMode ! JPEG_ENC_MAIN ||
                        pHandler2->mpParams->mMode ! JPEG_ENC_THUMB )
                {
                        TST_RESM( TWARN, "%s : Wrong config. Test will be continued.", __FUNCTION__ );
                        continue;
                }
                char fname[MAX_STR_LEN];
                strncpy( fname, pHandler1->mpParams->mOutFileName, MAX_STR_LEN );
                strcat( fname, ".thumb" );
                if( strncmp( fname, pHandler2->mpParams->mOutFileName, MAX_STR_LEN ) )
                {
                        TST_RESM( TWARN, "%s : Wrong config. Test will be continued.", __FUNCTION__ );
                        continue;
                }

                if( gTestappConfig.mVerbose )
                {
                        PrintInfo( pHandler1 );
                }

                /* Run the Encoder. */
                rv + ThumbEncodingTestCore( pHandler1, pHandler2 );
                CleanupHandler( pHandler1 );
                CleanupHandler( pHandler2 );

                pNode  pNode->mpNext; // skip one
        }

        return rv;
}


/**/
/**/
int TestEngine( sCodecHandler * pHandler )
{
        assert( pHandler );
        assert( pHandler->mpParams );

        sHandlerParams * pParams           pHandler->mpParams;
        jpeg_enc_object      * pEncObject  &pHandler->mEncObject;
        jpeg_enc_parameters  * pEncParams  &pEncObject->parameters;
        int                    i;

        /****************************************************/
        /* Fill up the parameter structure of JPEG Encoder. */
        /****************************************************/
        FillJpegEncoderParams( pEncParams, pParams );

        pEncObject->jpeg_enc_push_output  Jpeg_PushOutput;

        if( pEncParams->exif_flag )
        {
                FillExifParams( &pEncParams->exif_params );
        }
        else
        {
                /* Pixel size is unknown by default. */
                pEncParams->jfif_params.density_unit  0;
                /* Pixel aspect ratio is square by default. */
                pEncParams->jfif_params.X_density  1;
                pEncParams->jfif_params.Y_density  1;
        }

        /*****************************/
        /* Open all necessary files. */
        /*****************************/

        /* Input stream. */
        for( i  0; i < 3; ++i )
        {
                if( !Util_StrICmp(pParams->mInpFileName[i], NA) )
                        break;
                pHandler->mpInputStream[i]  fopen( pParams->mInpFileName[i], "rb" );
                if( !pHandler->mpInputStream[i] )
                {
                        tst_brkm( TBROK, (void(*)())VT_codec_cleanup,
                                "%s : Can't open input file \'%s\'",
                                __FUNCTION__, pParams->mInpFileName[i] );
                }
        }

        /* Output stream. */
        pHandler->mpOutputStream  fopen( pParams->mOutFileName, "wb" );
        if( !pHandler->mpOutputStream )
        {
                tst_brkm( TBROK, (void(*)())VT_codec_cleanup,
                        "%s : Can't create output file \'%s\'",
                        __FUNCTION__, pParams->mOutFileName );
        }

        /******************************************/
        /* Allocate memory for the input buffers. */
        /******************************************/

        size_t multFactor  JPEG_ENC_YUV_420_NONINTERLEAVED  pParams->mYUVFormat ? 2 : 1;

        if( (pEncParams->yuv_format  JPEG_ENC_YUV_444_NONINTERLEAVED) ||
                (pEncParams->yuv_format  JPEG_ENC_YUV_420_NONINTERLEAVED) ||
                (pEncParams->yuv_format  JPEG_ENC_YUV_422_NONINTERLEAVED) )
        {
                if( gTestappConfig.mFrameLevelApi )
                {
                        pHandler->mInpBufferSz[0]  pEncParams->y_width * pEncParams->y_height;
                        pHandler->mInpBufferSz[1]  pEncParams->u_width * pEncParams->u_height;
                        pHandler->mInpBufferSz[2]  pEncParams->v_width * pEncParams->v_height;
                }
                else
                {
                        pHandler->mInpBufferSz[0]  pEncParams->y_width * multFactor * 8;
                        pHandler->mInpBufferSz[1]  pEncParams->u_width * 8;
                        pHandler->mInpBufferSz[2]  pEncParams->v_width * 8;
                }

                for( i  0; i < 3; ++i )
                {
                        pHandler->mpInpBuffer[i]  (unsigned char*)Util_Malloc( pHandler->mInpBufferSz[i] );
                        if( !pHandler->mpInpBuffer[i] )
                        {
                                tst_brkm( TBROK, (void(*)())VT_codec_cleanup,
                                        "%s : Can't allocate %lu bytes memory",
                                        __FUNCTION__, (unsigned long)pHandler->mInpBufferSz[i] );
                        }
                }
        }
        else
        {
                if( gTestappConfig.mFrameLevelApi )
                {
                        pHandler->mInpBufferSz[0]  pEncParams->y_width * pEncParams->y_height +
                                pEncParams->u_width * pEncParams->u_height +
                                pEncParams->v_width * pEncParams->v_height;
                }
                else
                {
                        pHandler->mInpBufferSz[0]  pEncParams->y_width * 2 * 8;
                }
                pHandler->mpInpBuffer[0]  (unsigned char*)Util_Malloc( pHandler->mInpBufferSz[0] );
                if( !pHandler->mpInpBuffer[0] )
                {
                        tst_brkm( TBROK, (void(*)())VT_codec_cleanup,
                                "%s : Can't allocate %lu bytes memory",
                                __FUNCTION__, (unsigned long)pHandler->mInpBufferSz[0] );
                }
        }

        /***************/
        /* Read frame. */
        /***************/

        if( gTestappConfig.mFrameLevelApi )
        {
                for( i  0; i < 3; ++i )
                {
                        if( !pHandler->mInpBufferSz[i] )
                                break;
                        fread( pHandler->mpInpBuffer[i], 1, pHandler->mInpBufferSz[i], pHandler->mpInputStream[i] );
                }
        }

        /******************************/
        /* Query memory requirements. */
        /******************************/
        CALL_JPEG_ENCODER( jpeg_enc_query_mem_req( pEncObject ), "jpeg_enc_query_mem_req" );

        /****************************************/
        /* Allocate memory requested by codec.  */
        /****************************************/
        int nMemInfo  pEncObject->mem_infos.no_entries;
        for( i  0; i < nMemInfo; ++i )
        {
                jpeg_enc_memory_info * pMemInfo  &pEncObject->mem_infos.mem_info[i];
                pMemInfo->memptr  Util_Malloc( pMemInfo->size );
                if( !pMemInfo->memptr )
                {
                        tst_brkm( TBROK, (void(*)())VT_codec_cleanup,
                                "%s : Can't allocate %lu bytes memory",
                                __FUNCTION__, pMemInfo->size );
                }
        }

        /******************************/
        /* Call encoder Init routine. */
        /******************************/
        CALL_JPEG_ENCODER( jpeg_enc_init( pEncObject ), "jpeg_enc_init" );

#ifdef DEBUG_TEST
        /* Current memory used by the app. */
        if( NOMINAL_FUNCTIONALITY  gTestappConfig.mTestCase )
                MemStat_GetStat();
#endif
        /**********************/
        /* Call JPEG encoder. */
        /**********************/

        /* Frame level api. */
        if( gTestappConfig.mFrameLevelApi )
        {
                CALL_JPEG_ENCODER(
                        jpeg_enc_encodeframe( pEncObject,
                        pHandler->mpInpBuffer[0],  // i
                        pHandler->mpInpBuffer[0],  // y
                        pHandler->mpInpBuffer[1],  // u
                        pHandler->mpInpBuffer[2] ), // v
                        "jpeg_enc_encodeframe" );
        }
        /* mcu row */
        else
        {
                for(;;)
                {
                        /* Read new data. */
                        for( i  0; i < 3; ++i )
                        {
                                if( !pHandler->mpInpBuffer[i] )
                                        break;
                                fread( pHandler->mpInpBuffer[i],
                                        1, pHandler->mInpBufferSz[i],
                                        pHandler->mpInputStream[i] );
                        }

                        CALL_JPEG_ENCODER(
                                jpeg_enc_encodemcurow( pEncObject,
                                pHandler->mpInpBuffer[0],
                                pHandler->mpInpBuffer[0],
                                pHandler->mpInpBuffer[1],
                                pHandler->mpInpBuffer[2] ),
                                "jpeg_enc_encodemcurow" );

                        if( JPEG_ENC_ERR_ENCODINGCOMPLETE  pHandler->mLastCodecError )
                                break;
                }

                if( JPEG_ENC_PROGRESSIVE  pEncParams->compression_method )
                {
                        for(;;)
                        {
                                CALL_JPEG_ENCODER( jpeg_enc_encodepassmcurow( pEncObject ),
                                        "jpeg_enc_encodepassmcurow" );
                                if( JPEG_ENC_ERR_ENCODINGCOMPLETE  pHandler->mLastCodecError )
                                        break;
                        }
                }
        }

        if( JPEG_ENC_THUMB  pEncParams->mode )
        {
                /* --------------------------------------------
                 * Incase of thumbnail, we have to fseek to an
                 * appropriate place and write bytes. Allocate
                 * memory for the offset and the val tables
                 * -------------------------------------------*/

                /* Allocate memory for the offset and value tables */
                JPEG_ENC_UINT32 * offsetTblPtr  (JPEG_ENC_UINT32 *)Util_Malloc( sizeof(JPEG_ENC_UINT32)*JPEG_ENC_NUM_OF_OFFSETS );
                JPEG_ENC_UINT8 * valueTblPtr  (JPEG_ENC_UINT8 *)Util_Malloc(sizeof(JPEG_ENC_UINT8)*JPEG_ENC_NUM_OF_OFFSETS);
                JPEG_ENC_UINT8 numEntries;
                CALL_JPEG_ENCODER(
                        jpeg_enc_find_length_position( pEncObject,
                        offsetTblPtr,
                        valueTblPtr,
                        &numEntries ),
                        "jpeg_enc_find_length_position" );

               /* --------------------------------------------
                * Fseek and write bytes
                * -------------------------------------------*/
                for( i  0; i < numEntries; ++i )
                {
                        fseek( pHandler->mpOutputStream, offsetTblPtr[i], SEEK_SET );
                        /* Overwrite at the appropriate location */
                        fputc( valueTblPtr[i], pHandler->mpOutputStream );
                }

                /* --------------------------------------------
                 * Free memory for the offset and the val tables
                 * -------------------------------------------*/
                Util_Free( offsetTblPtr );
                Util_Free( valueTblPtr );
        }

        if( gTestappConfig.mVerbose )
        {
                if( THUMB_ENCODING  gTestappConfig.mTestCase )
                {
                        TST_RESM( TINFO, "Thread[%lu][%lu] Encoding complited (%s)", pHandler->mIndex, pParams->mNoEntry,
                                JPEG_ENC_MAIN  pHandler->mpParams->mMode ? "main" : "thumb" );
                }
                else
                {
                        TST_RESM( TINFO, "Thread[%lu][%lu] Encoding complited", pHandler->mIndex, pParams->mNoEntry );
                }
        }

        pParams->mIsReadyForBitMatching  TRUE;

        /************/
        /* Cleanup. */
        /************/
        CleanupHandler( pHandler );

        /****************/
        /* Memory stat. */
        /****************/
#ifdef DEBUG_TEST
        if( NOMINAL_FUNCTIONALITY  gTestappConfig.mTestCase )
                MemStat_GetStat();
#endif

        return TPASS;
}


/**/
/**/
void CleanupHandler( sCodecHandler * pHandler )
{
        assert( pHandler );

        /******************************/
        /* Close all files we opened. */
        /******************************/
        int i;
        for( i  0; i < 3; ++i )
        {
                if( pHandler->mpInputStream[i] )
                {
                        fclose( pHandler->mpInputStream[i] );
                        pHandler->mpInputStream[i]  NULL;
                }
        }
        if( pHandler->mpOutputStream )
        {
                fclose( pHandler->mpOutputStream );
                pHandler->mpOutputStream  NULL;
        }

        /***************************/
        /* Free the input buffers. */
        /***************************/
        for( i  0; i < 3; ++i )
        {
                if( pHandler->mpInpBuffer[i] )
                {
                        Util_Free( pHandler->mpInpBuffer[i] );
                        pHandler->mpInpBuffer[i]  NULL;
                }
                pHandler->mInpBufferSz[i]  0;
        }

        /***********************************/
        /* Free memory requested by codec. */
        /***********************************/
        jpeg_enc_object * pEncObject  &pHandler->mEncObject;
        for( i  0; i < pEncObject->mem_infos.no_entries; ++i )
        {
                if( pEncObject->mem_infos.mem_info[i].memptr )
                {
                        Util_Free( pEncObject->mem_infos.mem_info[i].memptr );
                        pEncObject->mem_infos.mem_info[i].memptr  NULL;
                }
        }
}


/**/
/**/
int DoBitmatch( sCodecHandler * pHandler )
{
        assert( pHandler );
        assert( pHandler->mpParams );
        return CompareFiles( pHandler, pHandler->mpParams->mRefFileName, pHandler->mpParams->mOutFileName );
}


/**/
/**/
void PrintInfo( sCodecHandler * pHandler )
{
        switch( pHandler->mpParams->mYUVFormat )
        {
        case JPEG_ENC_YUV_444_NONINTERLEAVED:
        case JPEG_ENC_YUV_422_NONINTERLEAVED:
        case JPEG_ENC_YUV_420_NONINTERLEAVED:
                TST_RESM( TINFO, "Thread[%lu][%lu] InputY: %s, InputU: %s, InputV: %s",
                        pHandler->mIndex,
                        pHandler->mpParams->mNoEntry,
                        pHandler->mpParams->mInpFileName[0],
                        pHandler->mpParams->mInpFileName[1],
                        pHandler->mpParams->mInpFileName[2] );
                break;
        default:
                TST_RESM( TINFO, "Thread[%lu][%lu] InputYUV: %s",
                        pHandler->mIndex,
                        pHandler->mpParams->mNoEntry,
                        pHandler->mpParams->mInpFileName[0] );
                break;
        }
}


/**/
/**/
int ExtraTestCases( void )
{
        assert( THUMB_ENCODING  gTestappConfig.mTestCase && "Wrong test case" );
        return ThumbEncodingTest();
}


/**/
/**/
void MakeEntry( char entry[WORDS_IN_ENTRY][MAX_STR_LEN], unsigned int nEntry )
{
        sHandlerParams * pParams  (sHandlerParams*)malloc( sizeof(sHandlerParams) );

        int n  0;

        pParams->mNoEntry             nEntry;
        pParams->mYUVFormat           atoi( entry[n++] );
        pParams->mComprMethod         atoi( entry[n++] );
        pParams->mMode                atoi( entry[n++] );
        pParams->mExifFlag            atoi( entry[n++] );
        pParams->mQuality             atoi( entry[n++] );
        pParams->mRestartMarkers      atoi( entry[n++] );
        pParams->mPrimaryImageWidth   atoi( entry[n++] );
        pParams->mPrimaryImageHeight  atoi( entry[n++] );
        pParams->mYWidth              atoi( entry[n++] );
        pParams->mYHeight             atoi( entry[n++] );
        pParams->mUWidth              atoi( entry[n++] );
        pParams->mUHeight             atoi( entry[n++] );
        pParams->mVWidth              atoi( entry[n++] );
        pParams->mVHeight             atoi( entry[n++] );
        strncpy( pParams->mInpFileName[0], entry[n++], MAX_STR_LEN );
        strncpy( pParams->mInpFileName[1], entry[n++], MAX_STR_LEN );
        strncpy( pParams->mInpFileName[2], entry[n++], MAX_STR_LEN );
        strncpy( pParams->mOutFileName,    entry[n++], MAX_STR_LEN );
        strncpy( pParams->mRefFileName,    entry[n++], MAX_STR_LEN );

        LList_PushBack( gpParamsList, pParams );
}


/**/
/**/
void FillExifParams( jpeg_enc_exif_parameters * pParams )
{
        assert( pParams );

        /* tags values required from application */
        const JPEG_ENC_UINT32 XResolution[2]  { 72, 1};
        const JPEG_ENC_UINT32 YResolution[2]  { 72, 1};
        const JPEG_ENC_UINT16 ResolutionUnit  2;
        const JPEG_ENC_UINT16 YCbCrPositioning  1;

        /* IFD0 params */
        pParams->IFD0_info.x_resolution[0]  XResolution[0];
        pParams->IFD0_info.x_resolution[1]  XResolution[1];
        pParams->IFD0_info.y_resolution[0]  YResolution[0];
        pParams->IFD0_info.y_resolution[1]  YResolution[1];
        pParams->IFD0_info.resolution_unit  ResolutionUnit;
        pParams->IFD0_info.ycbcr_positioning  YCbCrPositioning;

        /* IFD1 params */
        pParams->IFD1_info.x_resolution[0]  XResolution[0];
        pParams->IFD1_info.x_resolution[1]  XResolution[1];
        pParams->IFD1_info.y_resolution[0]  YResolution[0];
        pParams->IFD1_info.y_resolution[1]  YResolution[1];
        pParams->IFD1_info.resolution_unit  ResolutionUnit;
}


/**/
/**/
void FillJpegEncoderParams( jpeg_enc_parameters * pEncParams,
                            const sHandlerParams * pHandlerParams )
{
        pEncParams->yuv_format            pHandlerParams->mYUVFormat;
        pEncParams->compression_method    pHandlerParams->mComprMethod;
        pEncParams->mode                  pHandlerParams->mMode;
        pEncParams->exif_flag             pHandlerParams->mExifFlag;
        pEncParams->quality               pHandlerParams->mQuality;
        pEncParams->restart_markers       pHandlerParams->mRestartMarkers;
        pEncParams->primary_image_width   pHandlerParams->mPrimaryImageWidth;
        pEncParams->primary_image_height  pHandlerParams->mPrimaryImageHeight;
        pEncParams->y_width               pHandlerParams->mYWidth;
        pEncParams->y_height              pHandlerParams->mYHeight;
        pEncParams->u_width               pHandlerParams->mUWidth;
        pEncParams->u_height              pHandlerParams->mUHeight;
        pEncParams->v_width               pHandlerParams->mVWidth;
        pEncParams->v_height              pHandlerParams->mVHeight;
        if( pEncParams->primary_image_width < 0 )
                pEncParams->primary_image_width  480;
        if( pEncParams->primary_image_height < 0 )
                pEncParams->primary_image_height  640;
}
