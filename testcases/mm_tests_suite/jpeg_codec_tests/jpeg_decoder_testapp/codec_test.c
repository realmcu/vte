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
F.GAFFIE/rb657c       3/05/2004    TLSbo39336    Initial version 
D.Simakov/smkd001c    7/02/2005    TLSbo47113    Update 
D.Simakov/smkd001c    22/07/2005   TLSbo52627    Relocatability test caes was added
D.Simakov/smkd001c    28/10/2005   TLSbo57009    Update
A.Pshenichnikov       28/11/2005   TLSbo59532    Memory leaks were fixed
D.Simakov/smkd001c    05/12/2005   TLSbo59190    Bit-match check and load test were added
D.Simakov/smkd001c    31/01/2006   TLSbo61035    Centralization of common features   
=============================================================================*/


/*==================================================================================================
                                        INCLUDE FILES
==================================================================================================*/

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
#include <util/fb_draw_api.h>
 

/*==================================================================================================
                                        LOCAL MACROS
==================================================================================================*/

/**********************************************************************
* Macro name:  CALL_JPEG_DECODER()
* Description: Macro checks for any error in function execution
               based on the return value. In case of error, the function exits.
**********************************************************************/
#define CALL_JPEG_DECODER(JpegDecRoutine, name)   \
do \
{ \
    pHandler->mLastCodecError = JpegDecRoutine; \
    if( pHandler->mLastCodecError >= JPEGD_ERR_REC_ERRORS_START ) \
    {\
        TST_RESM( TWARN, "%s : %s fails #%d [File: %s, line: %d]", __FUNCTION__, name, pHandler->mLastCodecError, __FILE__, __LINE__-5);\
        return TFAIL;\
    }\
} while(0)   
           

/*==================================================================================================
                                             ENUMS
==================================================================================================*/

enum
{
            SUSP_TYPE_NONE = 0,
            SUSP_TYPE_GET_FILE_INFO,        // Once in get_file_info (from get_new_data())
            SUSP_TYPE_QUERY_MEM,            // Once in query_mem (from get_new_data())
            SUSP_TYPE_INIT,                 // Once in decoder_init (from get_new_data())
            SUSP_TYPE_DEC_ONCE,             // Once in decode_mcu_row (from get_new_data())
            SUSP_TYPE_DEC_RANDOM,           // Randomly in decode_mcu_row (from get_new_data())
            SUSP_TYPE_DEC_EVERY_MCU,        // After every MCU
            SUSP_TYPE_DEC_MARKER,           /* Every marker (restart marker, all other markers after
                                                1st scan */
            SUSP_TYPE_DEC_MARKER_BODY,      /* Every marker body (all markers after
                                                1st scan */
            SUSP_TYPE_DEC_FIRST_LOOP,       // Suspend in the first-loop (MCU row loop, in jpeg_read_image_data)
            SUSP_TYPE_DEC_SECOND_LOOP,      // Suspend in the second-loop (loop to skip last few MCUs in case of scaling, in jpeg_read_image_data)
            SUSP_TYPE_DEC_BOTH_LOOPS,       // Suspend in the first and second loop
};

enum
{
            SUSP_STATE_NONE,        // Can not suspend
            SUSP_STATE_START,       // Can suspend now
            SUSP_STATE_SUSPENDED,   // Suspended
            SUSP_STATE_END,         // End of Suspension
};


/*==================================================================================================
                                   FUNCTION PROTOTYPES
==================================================================================================*/

/* Callbacks. */
JPEGD_UINT8 Jpeg_GetNewData           ( JPEGD_UINT8 ** ppBuf, 
                                        JPEGD_UINT32 * pLen, 
                                        JPEGD_UINT32   mcuOffset,
                                        JPEGD_UINT8    beginFlag, 
                                        void         * pDecObject );

/* Extra test cases. */
int RobustnessTestCase                ( void ); 

/* Helper functions. */
sCodecHandler * GetHandler            ( JPEGD_Decoder_Object * pDecObject );
void            WriteOutputs          ( sCodecHandler * pHandler );
void            DrawPictureInProgress ( sCodecHandler * pHandler );
void            DetectEnter           ( void );


/*==================================================================================================
                                       FUNCTIONS
==================================================================================================*/

/*================================================================================================*/
/*================================================================================================*/ 
JPEGD_UINT8 Jpeg_GetNewData( JPEGD_UINT8 ** ppBuf, 
                             JPEGD_UINT32 * pLen, 
                             JPEGD_UINT32   mcuOffset,
                             JPEGD_UINT8    beginFlag, 
                             void         * pObject )
{
        /*********************************/
        /* Find the appropriate handler. */
        /*********************************/
        
        sCodecHandler * pHandler = GetHandler( pObject );
        assert( pHandler );
        assert( pHandler->mpInputStream );

        int  nBytesRead = 0;
                       

        /********************************************/
        /* Seek to the beginning of the file again. */
        /********************************************/        
        
        if( 1 == beginFlag )
        {
                fseek( pHandler->mpInputStream, 0, SEEK_SET);
                pHandler->mTotalBytes = 0;
        }
        
        
        /**********************/
        /* Handle suspension. */
        /**********************/
        
        if( pHandler->mSuspFlag )
        {
                if( pHandler->mSuspEof )
                {
                        pHandler->mSuspState = SUSP_STATE_NONE;
                }
                if( SUSP_STATE_START == pHandler->mSuspState )
                {
                        if( pHandler->mTotalBytes >= pHandler->mSuspTargetBytes )
                        {
                                /* Suspend the codec */
                                *ppBuf = NULL;
                                *pLen = 0;
                                pHandler->mSavedMcuOffset = mcuOffset;
                                pHandler->mSuspState = SUSP_STATE_SUSPENDED;
                                return JPEGD_SUSPEND;
                        }
                }
                else if( SUSP_STATE_SUSPENDED == pHandler->mSuspState )
                {
                        /* 
                         * Was suspended, set pointers and length correctly.
                         * This assumes data pointer exists in the same buffer
                         */
                        pHandler->mTotalBytes -= pHandler->mSavedMcuOffset;
                        fseek( pHandler->mpInputStream, -pHandler->mSavedMcuOffset, SEEK_CUR );
                        if( SUSP_TYPE_DEC_RANDOM == pHandler->mSuspType )
                        {
                                int bytes;
                                pHandler->mSuspState = SUSP_STATE_START;
                                pHandler->mSuspTargetBytes += INPUT_BUFFER_SZ_SUSP;
                                bytes = 0;
                                while( bytes == 0 )
                                {
                                        bytes = (int)(MAX_RAND_SIZE * ( (float)rand() / RAND_MAX ));
                                }
                                pHandler->mSuspTargetBytes += bytes;
                        }
                        else
                        {
                                pHandler->mSuspState = SUSP_STATE_END;
                        }
                }
        }
        

        /********************************************************/
        /* Read next portion of the data and update the buffer. */
        /********************************************************/        
        
        nBytesRead = fread( pHandler->mpInpBuffer, 
                            1, 
                            INPUT_BUFFER_SZ_SUSP, 
                            pHandler->mpInputStream ); 
        
        *pLen = nBytesRead;
        *ppBuf = pHandler->mpInpBuffer;
        
        if( nBytesRead <= 0 )
        {
                nBytesRead = 0;
                *ppBuf = NULL;
                if( pHandler->mSuspFlag ) 
                        pHandler->mSuspEof = 1;
                return JPEGD_END_OF_FILE;
        }
        pHandler->mTotalBytes += nBytesRead;
        
        return JPEGD_SUCCESS;
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
        
        JPEGD_THUMBNAIL_TYPE   thumbnailType;
        JPEGD_UINT8            fileFormat;
        JPEGD_UINT32           minSizeExif = 0;
        
        JPEGD_Decoder_Object * pDecObject = &pHandler->mDecObject;
        JPEGD_Decoder_Params * pDecParams = &pDecObject->dec_param;
        JPEGD_Decoder_Info   * pDecInfo   = &pDecObject->dec_info;
        sHandlerParams       * pParams    = pHandler->mpParams;         
        
        memset( pDecObject, 0x00, sizeof(JPEGD_Decoder_Object) );
        pDecParams->exif_info_needed       = 0;
        pDecParams->decoding_mode          = JPEGD_PRIMARY;
        

        /*****************************/
        /* Open all necessary files. */
        /*****************************/
        
        /* Input stream. */        
        pHandler->mpInputStream = fopen( pParams->mInpFileName, "rb" );
        if( !pHandler->mpInputStream )
        {
                tst_brkm( TBROK, (void(*)())VT_codec_cleanup,
                        "%s : Can't open input file \'%s\'",                
                        __FUNCTION__, pParams->mInpFileName );
        }
                
        /* Output stream. */
        pHandler->mpOutputStream = fopen( pParams->mOutFileName, "wb" );
        if( !pHandler->mpOutputStream )
        {
                tst_brkm( TBROK, (void(*)())VT_codec_cleanup,
                        "%s : Can't create output file \'%s\'",
                        __FUNCTION__, pParams->mOutFileName );
        }               
          
        
        if( pHandler->mSuspFlag )
        {
                /* Initialize the suspension state */
                pHandler->mSuspState = SUSP_STATE_NONE;
                /* Initialize the type of suspension to be tested */
                pHandler->mSuspType = SUSP_TYPE_DEC_RANDOM;
                //pHandler->mSuspType = SUSP_TYPE_DEC_EVERY_MCU;
                //pHandler->mSuspType = SUSP_TYPE_DEC_MARKER;
                pHandler->mSuspTargetBytes = 2*INPUT_BUFFER_SZ_SUSP;
                if( pHandler->mSuspType == SUSP_TYPE_GET_FILE_INFO)
                {
                        pHandler->mSuspState = SUSP_STATE_START;
                }
        }

        
        /***************************/
        /* Fill up the parameters. */
        /***************************/
        
        pDecParams->desired_output_width  = pParams->mOutWidth;
        pDecParams->desired_output_height = pParams->mOutHeight;
        pDecParams->dct_method            = pParams->mDctMethod;
        pDecParams->output_format         = pParams->mOutFormat; 
        

        /********************/
        /* Setup callbacks. */
        /********************/
        
        CALL_JPEG_DECODER(  
                jpegd_register_jpegd_get_new_data( Jpeg_GetNewData, pDecObject ),
                "jpegd_get_new_data" );        
        

        /******************/
        /* Get file info. */
        /******************/
        
        CALL_JPEG_DECODER( 
                jpegd_get_file_info( pDecObject, 
                                     &fileFormat, 
                                     &thumbnailType, 
                                     &minSizeExif ),
                           "jpegd_get_file_info" );        
                           
        if( SUSP_TYPE_QUERY_MEM == pHandler->mSuspType && pHandler->mSuspFlag )
        {
                pHandler->mSuspState = SUSP_STATE_START;
        }
        

        /******************************/
        /* Query and allocate memory. */
        /******************************/

        /* Query memory. */
        JPEGD_Mem_Alloc_Info * pMemInfo = &pDecObject->mem_info;        
        CALL_JPEG_DECODER( 
                jpegd_query_dec_mem( pDecObject ),
                "jpegd_query_dec_mem" );
        
        /* Allocate the memory. */
        int i;
        for( i = 0; i < pMemInfo->num_reqs; ++i )        
        {
                pMemInfo->mem_info_sub[i].ptr = Util_Malloc( pMemInfo->mem_info_sub[i].size );	
                if( !pMemInfo->mem_info_sub[i].ptr )
                {
                        tst_brkm( TBROK, (void(*)())VT_codec_cleanup,
                                "%s : Can't allocate %lu bytes memory",
                                __FUNCTION__, (unsigned long)pMemInfo->mem_info_sub[i].size );
                } 
        }
        
        if( SUSP_TYPE_INIT == pHandler->mSuspType && pHandler->mSuspFlag )
        {
                pHandler->mSuspState = SUSP_STATE_START;
        }


        /****************************/
        /* Initialize the decoder.  */
        /****************************/        

        CALL_JPEG_DECODER( 
                jpegd_decoder_init( pDecObject ),
                "jpegd_decoder_init" );        
                     
        
        /******************/
        /* Alloc buffers. */
        /******************/

        JPEGD_Component_Info * pCompInfo;        
        int ci;        
        
        /* Allocate output buffer */
        if( JPEGD_OFMT_ENC_IMAGE_FMT == pDecParams->output_format )
        {
                for( ci = 0, pCompInfo = pDecInfo->comp_info; ci < pDecInfo->num_components; ci++, pCompInfo++ )
                {
                        pHandler->mOutBufferSz[ci] = pCompInfo->max_lines * 
                                                     pCompInfo->actual_output_width;
                        pHandler->mpOutBuffer[ci] = (unsigned char*)Util_Malloc( pHandler->mOutBufferSz[ci] );
                        pHandler->mOutStrideWidth[ci] = pCompInfo->actual_output_width;
                        if( !pHandler->mpOutBuffer[ci] )
                        {
                                tst_brkm( TBROK, (void(*)())VT_codec_cleanup,
                                        "%s : Can't allocate %lu bytes memory",
                                        __FUNCTION__, (unsigned long)pHandler->mpOutBuffer[ci] );
                        }
                }
        }
        else
        {
                int bpp = JPEGD_OFMT_RGB_565 == pDecObject->dec_param.output_format ? 2 : 3;                
                pHandler->mOutBufferSz[0] = bpp * 
                                            pDecInfo->max_lines * 
                                            pDecInfo->actual_output_width;                        
                pHandler->mpOutBuffer[0] = (unsigned char*)Util_Malloc( pHandler->mOutBufferSz[0] );
                pHandler->mOutStrideWidth[0] = pDecInfo->actual_output_width;
                if( !pHandler->mpOutBuffer[0] )
                {
                        tst_brkm( TBROK, (void(*)())VT_codec_cleanup,
                                "%s : Can't allocate %lu bytes memory",
                                __FUNCTION__, (unsigned long)pHandler->mpOutBuffer[0] );
                }
        }
                        
                
        if( pHandler->mSuspFlag )
        {
                if( SUSP_TYPE_DEC_ONCE == pHandler->mSuspType || 
                    SUSP_TYPE_DEC_RANDOM == pHandler->mSuspType )
                {
                        pHandler->mSuspState = SUSP_STATE_START;
                }
        }
        
#ifdef DEBUG_TEST
        /* Current memory used by the app. */
        if( NOMINAL_FUNCTIONALITY == gTestappConfig.mTestCase )
                MemStat_GetStat();
#endif   

        /******************/
        /* Decoding loop. */
        /******************/

        /* Decoder loop to generate the RGB outputs */
        do
        {
                /* Decode few lines */                
                CALL_JPEG_DECODER(
                        jpegd_decode_mcu_row( pDecObject, 
                                              pHandler->mpOutBuffer, 
                                              pHandler->mOutStrideWidth ),
                        "jpegd_decode_mcu_row" );
                
                /* Suspended */
                if( JPEGD_ERR_SUSPENDED == pHandler->mLastCodecError ) 
                {
                        continue;
                }
                                                               
                if( JPEGD_OFMT_ENC_IMAGE_FMT != pDecParams->output_format &&
                    RE_ENTRANCE != gTestappConfig.mTestCase && 
                    PRE_EMPTION != gTestappConfig.mTestCase && !gTestappConfig.mDisableLCD )
                {
                        DrawPictureInProgress( pHandler );
                }    
                
                WriteOutputs( pHandler );                                
        } while( pDecInfo->output_scanline < pDecInfo->actual_output_height );
        
        TST_RESM( TINFO, "Thread[%lu][%lu] Decoding complited", pHandler->mIndex, pParams->mNoEntry );
        pParams->mIsReadyForBitMatching = TRUE;
        

        /*****************/
        /* Clear screen. */
        /*****************/        
        
        if( JPEGD_OFMT_ENC_IMAGE_FMT != pDecParams->output_format &&
            RE_ENTRANCE != gTestappConfig.mTestCase && 
            PRE_EMPTION != gTestappConfig.mTestCase && !gTestappConfig.mDisableLCD )
        {
                DetectEnter();
                sColor white;
                white.mAll = 0xffffffff;
                const sFramebuffer * pFb = GetFramebuffer();
                assert( pFb );
                pFb->ClearScreen( &white );
        }


        
        /************************/
        /* Cleanup the handler. */
        /************************/

        CleanupHandler( pHandler );

#ifdef DEBUG_TEST
        if( NOMINAL_FUNCTIONALITY == gTestappConfig.mTestCase )
                MemStat_GetStat();
#endif  
        
        return TPASS;
}


/*================================================================================================*/
/*================================================================================================*/
void CleanupHandler( sCodecHandler * pHandler )
{
        assert( pHandler );
                

        /******************************/
        /* Close all files we opened. */
        /******************************/   
        
        if( pHandler->mpInputStream )
        {
                fclose( pHandler->mpInputStream );
                pHandler->mpInputStream = NULL;
        }  
        if( pHandler->mpOutputStream )
        {
                fclose( pHandler->mpOutputStream );
                pHandler->mpOutputStream = NULL;
        }
        

        /***************************/
        /* Free the input buffers. */
        /***************************/

        int i;
        for( i = 0; i < JPEGD_MAX_NUM_COMPS; ++i )
        {    
                if( pHandler->mpOutBuffer[i] )
                {
                        Util_Free( pHandler->mpOutBuffer[i] );
                        pHandler->mpOutBuffer[i] = NULL;
                }            
                pHandler->mOutBufferSz[i] = 0;        
                pHandler->mOutStrideWidth[i] = 0;
        }
        
        
        /***********************************/
        /* Free memory requested by codec. */
        /***********************************/	

        if( ROBUSTNESS != gTestappConfig.mTestCase )
        {        
                JPEGD_Mem_Alloc_Info * pMemInfo = &pHandler->mDecObject.mem_info;        
                for( i = 0; i < pMemInfo->num_reqs; ++i )
                {
                        if( pMemInfo->mem_info_sub[i].ptr )
                        {
                            Util_Free( pMemInfo->mem_info_sub[i].ptr );
                            pMemInfo->mem_info_sub[i].ptr = NULL;
                        }                    
                }             
        }                
}


/*================================================================================================*/
/*================================================================================================*/
int DoBitmatch( sCodecHandler * pHandler )
{
        assert( pHandler );
        assert( pHandler->mpParams );
        return CompareFiles( pHandler, pHandler->mpParams->mRefFileName, pHandler->mpParams->mOutFileName );
} 


/*================================================================================================*/
/*================================================================================================*/
void PrintInfo( sCodecHandler * pHandler )
{
        assert( pHandler );
        TST_RESM( TINFO, "Thread[%lu][%lu] Input: %s Output: %s Ref: %s",
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
void MakeEntry( char entry[WORDS_IN_ENTRY][MAX_STR_LEN], unsigned int nEntry )
{
        /****************************/
        /* Allocate sHandlerParams. */
        /****************************/

        sHandlerParams * pParams = (sHandlerParams*)malloc( sizeof(sHandlerParams) ); 
        if( !pParams )
        {
                tst_brkm( TBROK, (void(*)())VT_codec_cleanup,
                        "%s : Can't allocate %lu bytes memory",
                        __FUNCTION__, (unsigned long)sizeof(sHandlerParams) );
        }
        

        /********************/
        /* Parse the entry. */
        /********************/

        int n = 0;
        
        pParams->mNoEntry            = nEntry;            
        pParams->mOutFormat          = atoi( entry[n++] );
        pParams->mDctMethod          = atoi( entry[n++] );
        pParams->mOutWidth           = atoi( entry[n++] );
        pParams->mOutHeight          = atoi( entry[n++] );
        strncpy( pParams->mInpFileName, entry[n++], MAX_STR_LEN );
        strncpy( pParams->mOutFileName, entry[n++], MAX_STR_LEN );
        strncpy( pParams->mRefFileName, entry[n++], MAX_STR_LEN );
        

        /*************************************/
        /* Add the parsed entry to the list. */
        /*************************************/

        LList_PushBack( gpParamsList, pParams ); 
}
  

/*================================================================================================*/
/*================================================================================================*/
sCodecHandler * GetHandler( JPEGD_Decoder_Object * pDecObject )
{
        assert( pDecObject );
        
        int i;
        for( i = 0; i < MAX_THREADS; ++i )
        {
                if( &gCodecHandler[i].mDecObject == pDecObject )
                {
                        return gCodecHandler + i;
                }
        }
        return NULL;
}


/*================================================================================================*/
/*================================================================================================*/
void WriteOutputs( sCodecHandler * pHandler )
{
        assert( pHandler );  
        assert( pHandler->mpOutputStream );
        
        JPEGD_Decoder_Object * pDecObject = &pHandler->mDecObject;
        JPEGD_Decoder_Info   * pDecInfo   = &pDecObject->dec_info;    
        JPEGD_Component_Info * pCompInfo;    
        int ci, i;
        int sz, strideSz;            
        
        
        /************************************/
        /* Write out the decoded scanlines. */
        /************************************/
                
        unsigned char * pOutData;
        int row;
        
        if( JPEGD_OFMT_ENC_IMAGE_FMT == pDecObject->dec_param.output_format )
        {              
                
                /* Write component by component */
                for( ci = 0, pCompInfo = pDecInfo->comp_info; ci < pDecInfo->num_components; ++ci, ++pCompInfo )
                {
                        pOutData = pHandler->mpOutBuffer[ci];      
                        strideSz = pHandler->mOutStrideWidth[ci];
                        for( row = 0; row < pCompInfo->num_lines; ++row )
                        {
                                fwrite( pOutData,
                                        1,
                                        pCompInfo->actual_output_width,
                                        pHandler->mpOutputStream );
                                pOutData += strideSz;
                        }
                }
        }
        else
        {
                int bpp = JPEGD_OFMT_RGB_565 == pDecObject->dec_param.output_format ? 2 : 3;  
                pOutData = pHandler->mpOutBuffer[0];
                sz = bpp * pDecInfo->actual_output_width;
                strideSz = bpp * pHandler->mOutStrideWidth[0];
                
                /*
                 *  For RGB_565, outputs are written as half words,
                 *  So while writing into a file we need to treat
                 *  the outputs as half-words to take care of
                 *  endianness
                 */
                if( 2 == bpp )
                {
                        for( row = 0; row < pDecInfo->num_lines; ++row )
                        {
                                unsigned short * pOutData16 = (unsigned short *)pOutData;
                                for( i = 0; i < sz; i += 2 )
                                {
                                        unsigned short tmp;
                                        unsigned char  tmp1;
                                        tmp = *pOutData16++;
                                        tmp1 = (tmp >> 8) & 0xFF;
                                        putc( tmp1, pHandler->mpOutputStream );
                                        tmp1 = tmp & 0xFF;
                                        putc( tmp1, pHandler->mpOutputStream );
                                }                                
                                pOutData += strideSz;
                        }
                }
                else
                {
                        for( row = 0; row < pDecInfo->num_lines; ++row )
                        {
                                fwrite( pOutData, 
                                        1,
                                        sz,
                                        pHandler->mpOutputStream );
                                pOutData += strideSz;
                        }
                }                
        }               
}


/*================================================================================================*/
/*================================================================================================*/
void DrawPictureInProgress( sCodecHandler * pHandler )
{
        assert( pHandler );        
        assert( JPEGD_OFMT_ENC_IMAGE_FMT != pHandler->mDecObject.dec_param.output_format );
        
        JPEGD_Decoder_Object * pDecObject = &pHandler->mDecObject;
        JPEGD_Decoder_Params * pDecParams = &pDecObject->dec_param;
        JPEGD_Decoder_Info   * pDecInfo   = &pDecObject->dec_info;                
        int sz, strideSz, row;        
        unsigned char * pOutData;
        
        int bpp = 3;
        ePixelFormat pfmt = PF_RGB_888;
        if( JPEGD_OFMT_RGB_565 == pDecParams->output_format )
        {
                bpp = 2;
                pfmt = PF_RGB_565;
        }
        
        pOutData = pHandler->mpOutBuffer[0];
        sz = bpp * pDecInfo->actual_output_width;
        strideSz = bpp * pHandler->mOutStrideWidth[0];
        
        const sFramebuffer * pFb = GetFramebuffer();
        assert( pFb );
        for( row = 0; row < pDecInfo->num_lines; ++row )
        {
                pFb->DrawScanline( pOutData, 
                                   0, 
                                   pHandler->mDrawedLinesNo + row, 
                                   pDecInfo->actual_output_width, 
                                   pfmt );
                pOutData += strideSz;
        }
        pHandler->mDrawedLinesNo = pDecInfo->output_scanline;    
}


/*================================================================================================*/
/*================================================================================================*/
void DetectEnter( void )
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
