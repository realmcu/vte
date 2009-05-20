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
D.Simakov/smkd001c    24/06/2005   TLSbo51185 Working version
D.Simakov/smkd001c    30/06/2005   TLSbo52235   Video desplay was added
D.Simakov/smkd001c    22/07/2005   TLSbo52363   Relocatability test caes was added
D.Simakov             20/04/2006   TLSbo67494   Compilation break
D.Simakov             15/05/2006   TLSbo66278   Phase2
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
#include <sys/time.h>

/* Harness Specific Include Files. */
#include "test.h"

/* Other Include Files. */
#include "codec_test.h"

/* Common Includes. */
#include <common_codec_test.h>
#include <util/fb_draw_api.h>
#include <util/ycbcr.h>
#include <util/mem_stat.h>



/*
                                        LOCAL MACROS
*/

#define DPRINTF(fmt,...) do{printf("%s:%d %s() ", __FILE__, __LINE__, __FUNCTION__); printf((fmt), ##__VA_ARGS__); fflush(stdout);}while(0)
#define RGB_BPP 3

/**********************************************************************
 * Macro name:  CALL_RA_DECODER()
 * Description: Macro checks for any error in function execution
                based on the return value. In case of error, the function exits.
**********************************************************************/
#define CALL_REALVIDEO_DECODER(RvDecRoutine, name)   \
do \
{ \
    pHandler->mLastCodecError  RvDecRoutine; \
    if( pHandler->mLastCodecError ! RV_S_OK ) \
    {\
        TST_RESM( TWARN, "%s : %s fails #%d [File: %s, line: %d]", __FUNCTION__, name, pHandler->mLastCodecError, __FILE__, __LINE__-5);\
        return TFAIL;\
    }\
} while(0)

#define CALL_REALVIDEO_DECODER_B(RvDecRoutine, name)   \
do \
{ \
    pHandler->mLastCodecError  RvDecRoutine; \
    if( !pHandler->mLastCodecError ) \
    {\
        TST_RESM( TWARN, "%s : %s fails #%d [File: %s, line: %d]", __FUNCTION__, name, pHandler->mLastCodecError, __FILE__, __LINE__-5);\
        return TFAIL;\
    }\
} while(0)

#define SAFE_DELETE(p) {if(p){Util_Free(p);p0;}}

#define DEFAULT_RV_IMAGE_FMT(fmt,w,h)\
    {\
        fmt.dimensions.width  w;\
        fmt.dimensions.height  h;\
        fmt.rectangle.width  w;\
        fmt.rectangle.height  h;\
        fmt.yuv_info.y_pitch  w;\
        fmt.yuv_info.u_pitch  fmt.yuv_info.u_pitch  w / 2;\
    }


#define XRAW_IS_PSC_RV8(p) ( !(*((p)+0)) && !(*((p)+1)) && (*((p)+2)1) \
    && !(*((p)+3)&170) && !(*((p)+4)&170) && !(*((p)+5)&170) \
    && ((*((p)+6)&170)2))

#define XRAW_IS_PSC(p) ( \
        (*((p)+0)85) && \
        (*((p)+1)85) && \
        (*((p)+2)85) && \
        (*((p)+3)85) && \
        !(*((p)+4)&170) && \
        !(*((p)+5)&170) && \
        !(*((p)+6)&170) && \
        ((*((p)+7)&170)2))

/*
                                   VARIABLES
*/
static int gInitCounter  0;

/*
                                   FUNCTION PROTOTYPES
*/

/* Callbacks. */

/* Extra test cases. */
int               RobustnessTestCase( void );

/* Other. */
int               DecodingLoop( sCodecHandler * pHandler );
void              DoDataOutput(sCodecHandler * pHandler);
void              SetBufferPointers(struct RV_Image *pImage, void *pBuffer);
struct RV_Image * ReadInputNext(sCodecHandler * pHandler);
int               DecompressImage(sCodecHandler * pHandler,
                    size_t nSegments, RV_Segment_Info * pSegmentInfo);
void              ReadInput(sCodecHandler * pHandler);

/*
                                       LOCAL FUNCTIONS
*/

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
        assert(pHandler);
        assert(pHandler->mpParams);

        if( !gInitCounter )
        {
                CALL_REALVIDEO_DECODER_B(RVDecoder_Load(), "RVDecoder_Load");
        }
        ++gInitCounter;

        RV_Decoder_config *pDecConfig  &pHandler->mDecConfig;
        struct RVDecoder *pRVDecoder  &pHandler->mRVDecoder;
        sHandlerParams *pParams  pHandler->mpParams;

        /* Allocate memory for the RGB framebuffer. */
        if( !gTestappConfig.mDisableLCD )
        {
                pHandler->mpRgbFramebuffer  Util_Malloc(pParams->mWidth * pParams->mHeight * RGB_BPP);
                if (!pHandler->mpRgbFramebuffer)
                {
                        tst_brkm( TBROK, (void(*)())VT_codec_cleanup,
                                "%s : Can't allocate memory",
                                __FUNCTION__ );
                }
                if( RE_ENTRANCE ! gTestappConfig.mTestCase &&
                    PRE_EMPTION ! gTestappConfig.mTestCase)
                {
                        const sFramebuffer * pFb  GetFramebuffer();
                        assert(pFb);
                        sColor white;
                        white.mAll  0xffffffff;
                        pFb->ClearScreen(&white);
                }
        }

        /* Open all necessary files. */
        pHandler->mpInpStream  fopen(pParams->mInpFileName, "rb");
        if( !pHandler->mpInpStream )
        {
                tst_brkm( TBROK, (void(*)())VT_codec_cleanup,
                        "%s : Can't open input file - %s",
                        __FUNCTION__, pParams->mInpFileName );
        }
        int hasOutput  Util_StrICmp( pParams->mOutFileName, NA );
        if( hasOutput )
        {
                pHandler->mpOutStream  fopen(pParams->mOutFileName, "wb");
                if( !pHandler->mpOutStream )
                {
                        tst_brkm( TBROK, (void(*)())VT_codec_cleanup,
                                "%s : Can't create output file - %s",
                                __FUNCTION__, pParams->mOutFileName );
                }
        }


        pHandler->mImageFormat.fid  RV_FID_RV89COMBO;
        DEFAULT_RV_IMAGE_FMT(pHandler->mImageFormat, pParams->mWidth, pParams->mHeight);
        pHandler->mCurrentImage.format  pHandler->mImageFormat;

        /* Allocate memory for the input buffer. */

        size_t  areaSz  pParams->mWidth * pParams->mHeight;

        /* buffer size for RV89Combo */
        pHandler->mInputBufSz  areaSz + 4095;
        /* Double the read buffer size to allow reading files which */
        /* were experimentally created with larger-than-standard */
        /* pictures. */
        pHandler->mInputBufSz * 2;
        /* Finally, add a few bytes to our buffer.  Due to our parsing */
        /* mechanism, the buffer must be able to hold one image, plus */
        /* the picture start code of the subsequent image. */
        pHandler->mInputBufSz + 4;

        pHandler->mpInputBuf  (unsigned char *)Util_Malloc(pHandler->mInputBufSz);
        if (!pHandler->mpInputBuf)
        {
                tst_brkm( TBROK, (void(*)())VT_codec_cleanup,
                        "%s : Can't allocate memory for input buffer",
                        __FUNCTION__ );
        }

        pHandler->mpInputBufPtr  pHandler->mpInputBuf;

        /* Fill the buffers. */
        ReadInput(pHandler);

        /* Instantiate a decoder RealVideo, and set decoder options. */
        RV_Status status  RV_S_OK;
        RV_MSG_Simple simpleMsg;
        struct RV_Image src;

        // need to add RVDecoderQueryDecMem here to get the memory requirements of the decoder
        src.yuv_info.y_plane  pHandler->mpInputBuf;
        src.format.fid  pHandler->mImageFormat.fid;

        /* Query for memory is required by decoder. */
        CALL_REALVIDEO_DECODER(
                RVDecoderQueryDecMem(pDecConfig, &src, pParams->mIsRV8),
                "RVDecoderQueryDecMem");

        /* Allocate the required memory. */
        int     numReqs  pDecConfig->RV_Dec_mem_info.RV_Dec_num_reqs;
        int     i;

        for (i  0; i < numReqs; ++i)
        {
                RV_Dec_Mem_Alloc_Info_Sub *pSub  &pDecConfig->RV_Dec_mem_info.mem_info_sub[i];
                pSub->app_base_ptr  Util_Malloc(pSub->size * sizeof(char));
                if (!pSub->app_base_ptr)
                {
                        tst_brkm( TBROK, (void(*)())VT_codec_cleanup,
                                "%s : Can't allocate memory",
                                __FUNCTION__ );
                }
        }

        /* Initialize the decoder. */
        RVDecoder_Init(pDecConfig, pRVDecoder, pHandler->mImageFormat.fid, &status);
        if (RV_S_OK ! status)
        {
                TST_RESM(TWARN, "%s : RVDecoder_Init fails #%d", __FUNCTION__, status);
                return TFAIL;
        }

        /* Set any requested decoder options. */
        if (pParams->mSmoothingPostfilter)
        {
                simpleMsg.message_id  RV_MSG_ID_Smoothing_Postfilter;
                simpleMsg.value1  RV_MSG_ENABLE;
                simpleMsg.value2  0;   /* not used */
                CALL_REALVIDEO_DECODER(
                        Decoder_Custom_Message( pRVDecoder->m_coredecoder,
                                                &simpleMsg.message_id ),
                        "Decoder_Custom_Message");
        }
        if (pParams->mIsRV8)
        {
                simpleMsg.message_id  RV_MSG_ID_RealVideo8;
                simpleMsg.value1  RV_MSG_ENABLE;
                simpleMsg.value2  0;   /* not used */
                CALL_REALVIDEO_DECODER(Decoder_Custom_Message
                                       (pRVDecoder->m_coredecoder, &simpleMsg.message_id),
                                       "Decoder_Custom_Message");
        }
        if (pParams->mLatencyMode)
        {
                simpleMsg.message_id  RV_MSG_ID_Latency_Display_Mode;
                simpleMsg.value1  RV_MSG_ENABLE;
                simpleMsg.value2  0;   /* not used */
                CALL_REALVIDEO_DECODER(Decoder_Custom_Message
                                       (pRVDecoder->m_coredecoder, &simpleMsg.message_id),
                                       "Decoder_Custom_Message");
        }

        if (TPASS ! DecodingLoop(pHandler))
        {
                TST_RESM(TWARN, "%s : DecodingLoop fails", __FUNCTION__);
                return TFAIL;
        }

        pParams->mIsReadyForBitMatching  TRUE;

        /* Cleanip the handler */
        CleanupHandler(pHandler);

        /* Return succees */
        return TPASS;
}


/**/
/**/
void CleanupHandler( sCodecHandler * pHandler )
{
        assert(pHandler);

        /* Close all the open files */
        if (pHandler->mpInpStream)
        {
                fclose(pHandler->mpInpStream);
                pHandler->mpInpStream  NULL;
        }
        if (pHandler->mpOutStream)
        {
                fclose(pHandler->mpOutStream);
                pHandler->mpOutStream  NULL;
        }

        /* Free input/output buffers. */
        if (pHandler->mpInputBuf)
        {
                SAFE_DELETE(pHandler->mpInputBuf);
                pHandler->mInputBufSz  0;
        }
        if (pHandler->mpOutputBuf)
        {
                SAFE_DELETE(pHandler->mpOutputBuf);
                pHandler->mOutputBufSz  0;
        }

        SAFE_DELETE(pHandler->mpRgbFramebuffer);

        /* Free required memory. */
        RV_Decoder_config *pDecConfig  &pHandler->mDecConfig;
        int     numReqs  pDecConfig->RV_Dec_mem_info.RV_Dec_num_reqs;
        int     i;

        for (i  0; i < numReqs; ++i)
        {
                RV_Dec_Mem_Alloc_Info_Sub *pSub  &pDecConfig->RV_Dec_mem_info.mem_info_sub[i];

                SAFE_DELETE(pSub->app_base_ptr);
        }

        if( gInitCounter )
        {
                RVDecoder_Unload();
        }
        else
                --gInitCounter;
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
        sHandlerParams *pParams  (sHandlerParams *) malloc(sizeof(sHandlerParams));

        int     n  0;

        pParams->mWidth  atoi(entry[n++]);
        pParams->mHeight  atoi(entry[n++]);
        pParams->mSmoothingPostfilter  atoi(entry[n++]);
        pParams->mIsRV8  atoi(entry[n++]);
        pParams->mLatencyMode  atoi(entry[n++]);
        // pParams->mPercentPacketLoss  atoi( entry[n++] );
        pParams->mFps  25;

        strncpy(pParams->mInpFileName, entry[n++], MAX_STR_LEN);
        strncpy(pParams->mOutFileName, entry[n++], MAX_STR_LEN);
        strncpy(pParams->mRefFileName, entry[n++], MAX_STR_LEN);


        /* Adjust parameters here... */

        if (pParams->mWidth < 0 || pParams->mHeight < 0)
        {
                if (gTestappConfig.mVerbose)
                        TST_RESM(TWARN, "%s [Entry #%d]: width and height must be greater than 0."
                                 " Check the config.", __FUNCTION__, nEntry);
                SAFE_DELETE(pParams);
                return;
        }

        LList_PushBack(gpParamsList, pParams);
}


/**/
/**/
int DecodingLoop( sCodecHandler * pHandler )
{
        assert(pHandler);
        assert(pHandler->mpParams);

        RV_Decoder_config *pDecConfig  &pHandler->mDecConfig;
        struct RVDecoder *pRVDecoder  &pHandler->mRVDecoder;
        sHandlerParams *pParams  pHandler->mpParams;

        const struct RV_Image *pCompressedImage;

#define MAX_SEGMENTS 128
        RV_Segment_Info segmentInfo[MAX_SEGMENTS];
        U32     nSegments;
        U32    *pSegmentOffsets;

        struct RV_Dimensions dim;

        /* Create an area in which to decompress images. */
        /* We always decode to YUV12.  If display is desired, then we */
        /* pass that YUV12 image to pDisplayRealVideo. */

        pHandler->mDecompressedImage.format.fid  RV_FID_YUV12;
        pHandler->mDecompressedImage.format.dimensions  pHandler->mImageFormat.dimensions;
        DEFAULT_RV_IMAGE_FMT(pHandler->mDecompressedImage.format,
                             pHandler->mImageFormat.dimensions.width,
                             pHandler->mImageFormat.dimensions.height);

        dim.width  pHandler->mDecompressedImage.format.dimensions.width;
        dim.height  pHandler->mDecompressedImage.format.dimensions.height;
        pHandler->mOutputBufSz  (3 * dim.width * dim.height) / 2;

        /* Align the output buffer to 32 bytes for cache performance, */
        /* and because some postfilter optimizations require 8-byte */
        /* alignment of all three planes. */
        pHandler->mpOutputBuf  (unsigned char *) Util_Malloc(pHandler->mOutputBufSz + 31);
        if (!pHandler->mpOutputBuf)
        {
                TST_RESM(TWARN, "%s : Can't allocate %d bytes or the output buffer",
                         __FUNCTION__, pHandler->mOutputBufSz);
                return TFAIL;
        }

        pHandler->mpOutputBufAlignedPtr  pHandler->mpOutputBuf;
        if ((U32) (pHandler->mpOutputBuf) & 31)
                pHandler->mpOutputBufAlignedPtr + (32 - ((U32) (pHandler->mpOutputBuf) & 31));

        SetBufferPointers(&pHandler->mDecompressedImage, pHandler->mpOutputBufAlignedPtr);

        /* Start the decoder sequence. */
        CALL_REALVIDEO_DECODER(Decoder_Start_Sequence(pDecConfig,
                                                      pRVDecoder->m_coredecoder,
                                                      &pHandler->mImageFormat,
                                                      &pHandler->mDecompressedImage.format),
                               "Decoder_Start_Sequence");

        struct timeval begin,
                end;

        while (1)
        {
                gettimeofday(&begin, NULL);
                pHandler->mFpsDelay  (1000000. / pParams->mFps);
                if (NOMINAL_FUNCTIONALITY ! gTestappConfig.mTestCase ||
                    gTestappConfig.mDisableLCD)
                        pHandler->mCurrentDelay  pHandler->mFpsDelay;

                if (pHandler->mCurrentDelay > pHandler->mFpsDelay)
                {
                        pCompressedImage  ReadInputNext(pHandler);
                        if (!pCompressedImage)
                                break;

                        pHandler->mDecompressedImage.size  pHandler->mOutputBufSz;
                        pHandler->mDecompressedImage.sequence_number 
                            pCompressedImage->sequence_number;

                        nSegments  0;
                        pSegmentOffsets  NULL;
                        segmentInfo[0].is_valid  TRUE;
                        segmentInfo[0].offset  0;

                        if (TPASS ! DecompressImage(pHandler, nSegments, segmentInfo))
                        {
                                TST_RESM(TWARN, "%s : DecompressImage fails", __FUNCTION__);
                                return TFAIL;
                        }
                        pHandler->mCurrentDelay  0;
                }
                gettimeofday(&end, NULL);
                int     delay  (end.tv_sec - begin.tv_sec) * 1000000 +
                    (end.tv_usec - begin.tv_usec);
                pHandler->mCurrentDelay + delay;
        }

        return TPASS;
}



/**/
/**/
void DoDataOutput(sCodecHandler * pHandler)
{
        assert(pHandler);
        assert(pHandler->mpOutStream);
        assert(pHandler->mpParams);

        struct RV_Image *pDecompressedImage  &pHandler->mDecompressedImage;
        FILE   *out  pHandler->mpOutStream;
        sHandlerParams *pParams  pHandler->mpParams;

        assert(pDecompressedImage->size ! 0);
        assert(pDecompressedImage->yuv_info.y_plane);

        if (fwrite(pDecompressedImage->yuv_info.y_plane, pDecompressedImage->size, 1, out) ! 1)
        {
                TST_RESM(TWARN, "%s : I/O error #d", __FUNCTION__, ferror(out));
        }

        if (!gTestappConfig.mDisableLCD &&
            gTestappConfig.mTestCase ! RE_ENTRANCE && gTestappConfig.mTestCase ! PRE_EMPTION)
        {
                size_t  uOffset  pParams->mWidth * pParams->mHeight;
                size_t  vOffset  uOffset + (pParams->mWidth / 2) * (pParams->mHeight / 2);
                unsigned char *yPtr  (unsigned char *) pDecompressedImage->yuv_info.y_plane;
                unsigned char *uPtr  (unsigned char *) yPtr + uOffset;
                unsigned char *vPtr  (unsigned char *) yPtr + vOffset;

                /* YUV > RGB */
                YCrCbToRGB(yPtr, pParams->mWidth,
                           vPtr, uPtr,
                           pParams->mWidth / 2,
                           pHandler->mpRgbFramebuffer,
                           pParams->mWidth, pParams->mHeight, RGB_888);

                /* Draw the RGB frame */
                int     i;
                const sFramebuffer *pFb  GetFramebuffer();

                assert(pFb);
                int     stride  pParams->mWidth * RGB_BPP;

                for (i  0; i < pParams->mHeight; ++i)
                {
                        unsigned char *pWhereFrom  pHandler->mpRgbFramebuffer + stride * i;

                        pFb->DrawScanline(pWhereFrom, 0, i, pParams->mWidth, PF_RGB_888);
                }
        }
}


/**/
/**/
void SetBufferPointers(struct RV_Image *pImage, void *pBuffer)
{
        assert(pImage && pBuffer);

        pImage->yuv_info.y_plane  (U8 *) pBuffer;
        if (!pBuffer)
        {
                pImage->yuv_info.u_plane  pImage->yuv_info.v_plane  0;
        }
        else if (pImage->format.fid  RV_FID_YUV12 || pImage->format.fid  RV_FID_IYUV)
        {
                pImage->yuv_info.u_plane  pImage->yuv_info.y_plane +
                    (pImage->format.yuv_info.y_pitch * pImage->format.rectangle.height);
                pImage->yuv_info.v_plane  pImage->yuv_info.u_plane +
                    (pImage->format.yuv_info.u_pitch * pImage->format.rectangle.height / 2);
        }
        else if (pImage->format.fid  RV_FID_YV12)
        {
                /* For YV12, the v plane comes before the u plane */
                pImage->yuv_info.v_plane  pImage->yuv_info.y_plane +
                    (pImage->format.yuv_info.y_pitch * pImage->format.rectangle.height);
                pImage->yuv_info.u_plane  pImage->yuv_info.v_plane +
                    (pImage->format.yuv_info.v_pitch * pImage->format.rectangle.height / 2);
        }
        else if (pImage->format.fid  RV_FID_YVU9)
        {
                /* For YVU9, the v plane comes before the u plane */
                pImage->yuv_info.v_plane  pImage->yuv_info.y_plane +
                    (pImage->format.yuv_info.y_pitch * pImage->format.rectangle.height);
                pImage->yuv_info.u_plane  pImage->yuv_info.v_plane +
                    (pImage->format.yuv_info.v_pitch * pImage->format.rectangle.height / 4);
        }
        else
                pImage->yuv_info.y_plane  (U8 *) pBuffer;
}


/**/
/**/
struct RV_Image *ReadInputNext(sCodecHandler * pHandler)
{
        assert(pHandler);
        assert(pHandler->mpParams);
        assert(pHandler->mpInpStream);

        sHandlerParams *pParams  pHandler->mpParams;

        U8     *p;

        if (!pHandler->mInputBufValidSz)
                return NULL;    /* Must be at EOF */

        ++pHandler->mFrameNumber;
        pHandler->mCurrentImage.sequence_number  pHandler->mFrameNumber;

        /* Now move pHandler->mpInputBufPtr to the next picture in the buffer, and determine */
        /* the current picture's size. */

        /* The bitstream to be returned begins at pHandler->mpInputBufPtr. */
        SetBufferPointers(&pHandler->mCurrentImage, pHandler->mpInputBufPtr);

        /* Now search for the end of this bitstream, by looking for */
        /* the next bitstream's picture header. */

        p  pHandler->mpInputBufPtr + 1;        /* Skip past first byte of picture header */

        for (; p < pHandler->mpInputBuf + (pHandler->mInputBufValidSz - 2); ++p)
        {
                if (pParams->mIsRV8)
                {
                        if (XRAW_IS_PSC_RV8(p))
                                break;
                }
                else
                {
                        if (XRAW_IS_PSC(p))
                                break;
                }
        }

        if (p < pHandler->mpInputBuf + (pHandler->mInputBufValidSz - 2))
        {
                pHandler->mCurrentImage.size  (U32) (p - pHandler->mpInputBufPtr);
                pHandler->mpInputBufPtr  p;
        }
        else
        {
                /* If we haven't seen the next picture start code, then either */
                /* we're at the end of the file */
                /* (pHandler->mInputBufValidSz < pHandler->mInputBufSz), or we need to shift */
                /* the current picture left, fill more of the buffer, and */
                /* continue scanning. */

                if (pHandler->mInputBufValidSz < pHandler->mInputBufSz)
                {
                        /* We're at EOF. */
                        pHandler->mCurrentImage.size 
                            (U32) ((pHandler->mpInputBuf + pHandler->mInputBufValidSz) -
                                   pHandler->mpInputBufPtr);
                        pHandler->mpInputBufPtr  pHandler->mpInputBuf;
                        pHandler->mInputBufValidSz  0;
                }
                else
                {
                        ReadInput(pHandler);
                        SetBufferPointers(&pHandler->mCurrentImage, pHandler->mpInputBufPtr);

                        p  pHandler->mpInputBufPtr + 1;        /* Skip past first byte of picture
                                                                 * header */
                        for (; p < pHandler->mpInputBuf + (pHandler->mInputBufValidSz - 2); ++p)
                        {
                                if (pParams->mIsRV8)
                                {
                                        if (XRAW_IS_PSC_RV8(p))
                                                break;
                                }
                                else
                                {
                                        if (XRAW_IS_PSC(p))
                                                break;
                                }
                        }
                        /* If we still haven't seen the next picture start code, */
                        /* then we better be at EOF. */
                        if (p > pHandler->mpInputBuf + (pHandler->mInputBufValidSz - 2))
                        {
                                if (pHandler->mInputBufValidSz < pHandler->mInputBufSz)
                                {
                                        /* at EOF */
                                        pHandler->mCurrentImage.size 
                                            (U32) ((pHandler->mpInputBuf +
                                                    pHandler->mInputBufValidSz) -
                                                   pHandler->mpInputBufPtr);
                                        /* Don't allow any more reading of this file. */
                                        pHandler->mInputBufValidSz  0;
                                }
                                else
                                {
                                        TST_RESM(TWARN,
                                                 "%s : Could not find next PSC, but not at EOF either.",
                                                 __FUNCTION__);
                                        /* Don't allow any more reading of this file. */
                                        /* And don't return the current image since it is */
                                        /* likely bogus. */
                                        pHandler->mInputBufValidSz  0;
                                        return NULL;
                                }
                        }
                        else
                        {
                                pHandler->mCurrentImage.size  (U32) (p - pHandler->mpInputBufPtr);
                                pHandler->mpInputBufPtr  p;
                        }
                }
        }
        return &pHandler->mCurrentImage;
}


/**/
/**/
int DecompressImage(sCodecHandler * pHandler,
                    size_t nSegments, RV_Segment_Info * pSegmentInfo)
{
        RV_Decoder_Decompression_Flags flags  0;
        RV_Decoder_Decompression_Notes notes;
        I32     temporalOffset;

        struct RVDecoder *pRVDecoder  &pHandler->mRVDecoder;
        struct RV_Image *pCompressedImage  &pHandler->mCurrentImage;
        struct RV_Image *mDecompressedImage  &pHandler->mDecompressedImage;

        /* Repeatedly send the same source image to the decoder, until the */
        /* decoder is done emitting all the frames for this input image. */
        do
        {
                /* Give the decoder the slice boundaries, if they're available */
                if (nSegments)
                {
                        RV_Segment_Info_MSG segMsg;

                        segMsg.message_id  RV_MSG_ID_Set_Decode_Segment_Info;
                        segMsg.number_of_segments  nSegments - 1;
                        segMsg.segment_info  pSegmentInfo;
                        CALL_REALVIDEO_DECODER(Decoder_Custom_Message
                                               (pRVDecoder->m_coredecoder, &segMsg.message_id),
                                               "Decoder_Custom_Message");
                }

                notes  temporalOffset  0;

                /* Call the main decoder function. */
                CALL_REALVIDEO_DECODER(Decoder_Decode(pRVDecoder->m_coredecoder,
                                                      pCompressedImage,
                                                      mDecompressedImage,
                                                      flags, &notes, temporalOffset),
                                       "Decoder_Decode");

                if (!(notes & RV_DDN_DONT_DRAW))
                {
                        DoDataOutput(pHandler);
                }

                /* Set flag for next invocation of Decode. */
                flags | RV_DDF_MORE_FRAMES;

        }
        while (notes & RV_DDN_MORE_FRAMES);

        return TPASS;
}

/**/
/**/
void ReadInput(sCodecHandler * pHandler)
{
        assert(pHandler);
        assert(pHandler->mpInpStream);

        FILE   *in  pHandler->mpInpStream;

        size_t  i,
                nBytesRemaining,
                nBytesRead;
        unsigned char *pBuffer  pHandler->mpInputBuf;

        /* First shift any valid information to the beginning of pHander->mpInputBuf. */

        nBytesRemaining  pHandler->mInputBufValidSz -
            (size_t) (pHandler->mpInputBufPtr - pHandler->mpInputBuf);

        for (i  0; i < nBytesRemaining; ++i)
                *pBuffer++  *pHandler->mpInputBufPtr++;

        pHandler->mpInputBufPtr  pHandler->mpInputBuf;

        /* Now fill the unused part of buffer from the file */
        nBytesRemaining  pHandler->mInputBufSz - (size_t) (pBuffer - pHandler->mpInputBuf);

        nBytesRead  fread(pBuffer, sizeof(pHandler->mpInputBuf[0]), nBytesRemaining, in);
        pHandler->mInputBufValidSz  nBytesRead + (size_t) (pBuffer - pHandler->mpInputBuf);
}
