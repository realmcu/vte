/*
  Copyright (C) 2004, Freescale Semiconductor, Inc. All Rights Reserved
  THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
  BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
  Freescale Semiconductor, Inc.
*/
	
/**
@file jpeg2k_decoder_test.c
	
@brief VTE C header template
	
@par Portability:
        Indicate if this module is portable to other compilers or platforms.
        If not, indicate specific reasons why is it not portable.
*/
	
/*======================== REVISION HISTORY ==================================
		
Author (core ID)      Date         CR Number    Description of Changes
-------------------   ----------   ----------   ------------------------------
D.Simakov / smkd001c  30/08/2005   TLSbo53250   Initial version 
D.Simakov / smkd001c  30/08/2005   TLSbo53250   LCD support was added 
D.Simakov / smkd001c  18/10/1005   TLSbo57009   CLS() added
=============================================================================*/

#ifdef __cplusplus
extern "C"{
#endif

/*==================================================================================================
                                        INCLUDE FILES
==================================================================================================*/
/* Verification Test Environment Include Files */
#include "jpeg2k_decoder_test.h"

#include <pthread.h>
#include <math.h>
#include <stdarg.h>
#include <ctype.h>

/* JPEG2000 Decoder API. */
#include <j2kDecoderInterface.h>

#include "stuff/fb_draw_api.h"


/*==================================================================================================
                                        LOCAL MACROS
==================================================================================================*/
#define MAX_THREADS 4
#define SAFE_DELETE(p) {if(p){free(p);p=NULL;}}
#define NA "n/a"
#define M(m){printf("<<<--- %s --->>>\n",m);fflush(stdout);}


/**********************************************************************
* Macro name:  CALL_JPEG2K_DECODER()
* Description: Macro checks for any error in function execution
               based on the return value. In case of error, the function exits.
**********************************************************************/
#define CALL_JPEG2K_DECODER(Jpeg2KDecRoutine, name)   \
    pHandler->mLastJ2KDecError = Jpeg2KDecRoutine; \
    if( (pHandler->mLastJ2KDecError != J2K_SUCCESSFUL) )\
    {\
        TST_RESM( TWARN, "%s : %s fails #%d", __FUNCTION__, name, pHandler->mLastJ2KDecError);\
		return TFAIL;\
	}    

#define TST_RESM(s,format,...) \
{\
    if( LOAD != gTestappConfig.mTestCase ) \
        pthread_mutex_lock( &gMutex );\
    tst_resm((s), (format), ##__VA_ARGS__);\
    if( LOAD != gTestappConfig.mTestCase ) \
        pthread_mutex_unlock( &gMutex );\
}

#define alloc_fast(s) malloc(s)
#define alloc_slow(s) malloc(s)

#define CLS() \
{\
    const framebuffer_t * pFb = get_framebuffer();\
    assert( pFb );\
    argb_color_t white;\
    pFb->clear_screen( &white );\
}    
    
     

/*==================================================================================================
                          LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
==================================================================================================*/

/************************************************************************/
/*                                                                      */
/************************************************************************/
typedef struct
{
    char  mInputFileName[MAX_STR_LEN];
    char  mOutputFileName[MAX_STR_LEN];
    char  mReferenceFileNameY[MAX_STR_LEN]; 
    char  mReferenceFileNameU[MAX_STR_LEN]; 
    char  mReferenceFileNameV[MAX_STR_LEN]; 

    uint32 mResolution;
    //uint32 mLayers;

    //////////////////////////////////////////////////////////////////////////
    uint32 mEntryIndex;
} sHandlerParams;

/************************************************************************/
/*                                                                      */
/************************************************************************/
typedef struct 
{
    unsigned long           mIndex;
        
    const sHandlerParams  * mpParams;    
    FILE                  * mpInputStream;
            
    int                     mLastJ2KDecError; 

    unsigned char         * mpOutBuffer[SIZ_COMPONENTSIZE];
    unsigned char         * mpInpBuffer;    
    size_t                  mOutBufferSz[SIZ_COMPONENTSIZE];
    size_t                  mInpBufferSz;     
    
    J2KDecoderObject_t      mDecObject;
	J2KDecodeParamStruct_t  mDecParams;     //!< Write-only
	J2KStreamInfoStruct_t   mDecStreamInfo; //!< Read-only 

    /* PNSR info. */
   	double                  mCompPsnr[SIZ_COMPONENTSIZE]; 
    int                     mIsPsnrCalculated;

    pthread_t               mThreadID;          
    int                     mIsThreadFinished;
    int                     mLtpRetval;
} sJpeg2KDecoderHandler;

/*==================================================================================================
                                       LOCAL CONSTANTS
==================================================================================================*/


/*==================================================================================================
                                       LOCAL VARIABLES
==================================================================================================*/
static sJpeg2KDecoderHandler gJ2KDecHandlers[ MAX_THREADS ];
static int                   gNumThreads                     = 1;    			 
static int                   gThreadWithFocus                = -1;   	
static sLinkedList         * gpParamsList                    = NULL;
static pthread_mutex_t       gMutex;
/*==================================================================================================
                                       GLOBAL CONSTANTS
==================================================================================================*/

/*==================================================================================================
                                       GLOBAL VARIABLES
==================================================================================================*/

/*==================================================================================================
                                   LOCAL FUNCTION PROTOTYPES
==================================================================================================*/
int32 Jpeg2K_ReadStream( uint8 ** ppBuf, uint32 * pSz );

int  RunDecoder     ( void * ptr );
int  TestEngine     ( sJpeg2KDecoderHandler * pHandler );
void PrintProgress  ( sJpeg2KDecoderHandler * pHandler );
void DoDataOutput   ( sJpeg2KDecoderHandler * pHandler );
void ResetHandler   ( sJpeg2KDecoderHandler * pHandler );
void CleanupHandler ( sJpeg2KDecoderHandler * pHandler );
int  DoBitmatch     ( sJpeg2KDecoderHandler * pHandler );
int  DoFilesExist   ( const char * fname1, const char * fname2 );
void HogCpu         ();
void MakeEntry      ( char entry[WORDS_IN_ENTRY][MAX_STR_LEN], int nEntry );

/* Test cases */
int NominalFunctionalityTest ();
int RobustnessTest           ();
int ReLocatabilityTest       ();  
int ReEntranceTest           ();
int PreEmptionTest           ();
int EnduranceTest            ();
int LoadTest                 ();

/* Helper functions. */
int     Util_StrICmp                  ( const char * s1, const char *s2 );        
void    Util_WriteBitmap              ( const sJpeg2KDecoderHandler * pHandler );
void    Util_DisplayDecompressedImage ( const sJpeg2KDecoderHandler * pHandler );
void    Util_ConvertYuvToRgb          ( const uint8 * pCompData[3], uint8 * pRGBBufPtr, uint32 bufSize, uint8 ctType, int ncomp );
void    Util_CalculatePSNR            ( sJpeg2KDecoderHandler * pHandler, uint8 * pRefData[SIZ_COMPONENTSIZE] );
uint8 * Util_ReadFile                 ( const char * filename, size_t * pSz );
void    Util_SwapBytes                ( short * pWords, int count );

/*==================================================================================================
                                       LOCAL FUNCTIONS
==================================================================================================*/

/*================================================================================================*/
/*================================================================================================*/
int VT_jpeg2k_decoder_setup()
{      
    pthread_mutex_init( &gMutex, NULL );

    int i;
    /* Reset all handlers. */
    for( i = 0; i < MAX_THREADS; ++i )
        ResetHandler( gJ2KDecHandlers + i );    
        
    /**/
    gpParamsList = (sLinkedList*)malloc(sizeof(sLinkedList));
    gpParamsList->mpNext = NULL;
    gpParamsList->mpContent = NULL;

    if( !ParseConfig( gTestappConfig.mConfigFilename ) )        
    {
        TST_RESM( TWARN, "%s : can't parse the %s", 
                  __FUNCTION__, gTestappConfig.mConfigFilename );
        return TFAIL;
    }   
    
    /* Compute the actual number of threads. */
    if( RE_ENTRANCE == gTestappConfig.mTestCase || 
        PRE_EMPTION == gTestappConfig.mTestCase )
    {
        sLinkedList * pNode = gpParamsList;
        for( gNumThreads = 0; pNode && gNumThreads < MAX_THREADS; ++gNumThreads )
            pNode = pNode->mpNext;
    }

    if( gTestappConfig.mVerbose )
    {
        TST_RESM( TINFO, "Jpeg2k decoder revision : %d", GetJ2KDecoderVersionNo() );
    }
    
    return TPASS;
}


/*================================================================================================*/
/*================================================================================================*/
int VT_jpeg2k_decoder_cleanup()
{    
    pthread_mutex_destroy( &gMutex );

    if( gpParamsList ) 
        LList_Delete( gpParamsList );

    int i;
    for( i = 0; i < MAX_THREADS; ++i )
    {
        CleanupHandler( gJ2KDecHandlers + i );
        ResetHandler( gJ2KDecHandlers + i );        
    }
        
    return TPASS;
}


/*================================================================================================*/
/*================================================================================================*/
int VT_jpeg2k_decoder_test()
{
    int rv = TFAIL;            
    
    switch( gTestappConfig.mTestCase )
    {
		case NOMINAL_FUNCTIONALITY:	    
			TST_RESM( TINFO, "Nominal functionality test" );
			rv = NominalFunctionalityTest();
			TST_RESM( TINFO, "End of nominal functionality test" );		
    		break;

        case ROBUSTNESS:	    
			TST_RESM( TINFO, "Robustness test" );
			rv = RobustnessTest();
			TST_RESM( TINFO, "End of robustness test" );		
    		break;

        case RELOCATABILITY:
            TST_RESM( TINFO, "Relocatability test" );            
            rv = ReLocatabilityTest();
            TST_RESM( TINFO, "End relocatability test" );
            break;       
	    
		case RE_ENTRANCE:
			TST_RESM( TINFO, "Re-entrance test" );
			rv = ReEntranceTest();
			TST_RESM( TINFO, "End of re-entrance test" );		
			break;
	    
		case PRE_EMPTION:
			TST_RESM( TINFO, "Pre-emption test" );
			rv = PreEmptionTest();
			TST_RESM( TINFO, "End of pre-emption test" );		
			break;                    

		case ENDURANCE:
			TST_RESM( TINFO, "Endurance test" );
			rv = EnduranceTest();
			TST_RESM( TINFO, "End of endurance test" );		
			break;    

		case LOAD:
			TST_RESM( TINFO, "Load test" );
			rv = LoadTest();
			TST_RESM( TINFO, "End of load test" );		
			break;                                    
            
		default:
			TST_RESM( TINFO, "Wrong test case" );
			break;    
	}    
    
    return rv;
}


/*================================================================================================*/
/*================================================================================================*/
int32 Jpeg2K_ReadStream( uint8 ** ppBuf, uint32 * pSz )
{
	return J2K_SUCCESSFUL;
}


/*================================================================================================*/
/*================================================================================================*/
int RunDecoder( void * ptr )
{
    assert( ptr );    

    sJpeg2KDecoderHandler * pHandler = (sJpeg2KDecoderHandler*)ptr;
    
    /* Set priority for the PRE_EMPTION test case. */
    if( PRE_EMPTION == gTestappConfig.mTestCase )
    {
		int nice_inc = (int)(( 20.0f / gNumThreads ) * pHandler->mIndex);
		if( nice(nice_inc) < 0 )
		{
			TST_RESM( TWARN, "%s : nice(%d) has failed", 
                      __FUNCTION__, nice_inc );
		}
    }
    
    /* Run TestEngine. */
    pHandler->mLtpRetval = TestEngine( pHandler );        
    
    /* Perform bitmatching. */    
    if( pHandler->mIsPsnrCalculated )
    {              
        if( !DoBitmatch( pHandler ) )
        {
            pHandler->mLtpRetval = TFAIL;
            TST_RESM( TWARN, "bitmatch error");
        }
        else
        {
            TST_RESM( TINFO, "bitmatch passed");
        }
    }
    
    /* Give the focus to an incomplete thread. */
    pHandler->mIsThreadFinished = TRUE;    
    if( pHandler->mIndex == gThreadWithFocus )
    {
        int i;
        /* Search a first incomplete thread and assign them focus. */
		for( i = 0; i < gNumThreads; ++i )
		{            
			if( !gJ2KDecHandlers[i].mIsThreadFinished )
			{            
				gThreadWithFocus = gJ2KDecHandlers[i].mIndex;
				break;
			}
		}
    }

    /* Return LTP result. */
    return pHandler->mLtpRetval;
}


/*================================================================================================*/
/*================================================================================================*/
int TestEngine( sJpeg2KDecoderHandler * pHandler ) 
{
    assert( pHandler );
    assert( pHandler->mpParams );
    
    const sHandlerParams   * pParams        = pHandler->mpParams;
    J2KDecoderObject_t     * pDecObject     = &pHandler->mDecObject;
    J2KDecodeParamStruct_t * pDecParam      = &pHandler->mDecParams;
    J2KStreamInfoStruct_t  * pDecStreamInfo = &pHandler->mDecStreamInfo;
    J2KInitStruct_t          decInit;
    int                      i;
    
    /*****************************/
    /* Open all necessary files. */
    /*****************************/
    pHandler->mpInputStream = fopen( pParams->mInputFileName, "rb" );
    if( !pHandler->mpInputStream )
    {
        TST_RESM( TWARN, "%s : Can't open input file \'%s\'",
                  __FUNCTION__, pParams->mInputFileName );
        return TFAIL;                    
    }
               	       
    /*****************************************************************/
    /* Allocate memory for input buffer and read the input j2k file. */
    /*****************************************************************/ 
    pHandler->mpInpBuffer = Util_ReadFile( (const char*)pParams->mInputFileName, &pHandler->mInpBufferSz );    
    if( !pHandler->mpInpBuffer )
    {
        TST_RESM( TWARN, "%s : Can't allocate input buffer (%d bytes)",
            __FUNCTION__, pHandler->mInpBufferSz );
        return TFAIL;
    }        

    decInit.CBGetNewData = Jpeg2K_ReadStream;

    /****************************/
    /* Init the Jpeg2K decoder. */
    /****************************/    
    CALL_JPEG2K_DECODER(
        InitJ2KDecoder( pDecObject, decInit ),
        "InitJ2KDecoder" );

    /***************************/
    /* Obtain the stream info. */
    /***************************/    
    CALL_JPEG2K_DECODER(
        GetJ2KStreamInfo( pDecObject, 
                          pDecStreamInfo, 
                          pHandler->mpInpBuffer, 
                          pHandler->mInpBufferSz ),
        "GetJ2KStreamInfo" );       
        
    /***************************/
    /* Set decoder parameters. */
    /***************************/    
	if( 0 == pParams->mResolution )
	{
		TST_RESM( TWARN, "Resolution 0 is not valid. Decoding lowest resolution." 
                         "Please check \'%s\', line/entry #%lu", 
                  gTestappConfig.mConfigFilename, pParams->mEntryIndex+1 );
		pDecParam->mResolution = 1;
	}
	else if( pParams->mResolution > pDecStreamInfo->mResolution ) 
	{
		TST_RESM( TWARN, "Resolution %lu is not valid. Decoding highest resolution." 
                         "Please check \'%s\', line/entry #%lu", 
                  pParams->mResolution,
                  gTestappConfig.mConfigFilename, pParams->mEntryIndex + 1 );
		pDecParam->mResolution = pDecStreamInfo->mResolution;
	}
	else
	{
		pDecParam->mResolution = pParams->mResolution; 
	}
	pDecParam->mLayers = pDecStreamInfo->mLayers;

    /* Allocate decoder's memory. */
    pDecParam->mMemSize = pDecStreamInfo->mBytesOfMemory;
    pDecParam->mpMemptr = alloc_fast( pDecParam->mMemSize );    
    if( !pDecParam )
    {
        TST_RESM( TWARN, "%s : Can't allocate %d bytes memory", __FUNCTION__, pDecParam->mMemSize );
        return TFAIL;
    }

    CALL_JPEG2K_DECODER(
        SetJ2KDecodeParams( pDecObject, *pDecParam ),
        "SetJ2KDecodeParams" );

    /*******************************************/
    /* Allocate memory for the output buffers. */
    /*******************************************/    
	for( i = 0; i < pDecStreamInfo->mComponent; ++i )
	{
		pHandler->mOutBufferSz[i] = pDecStreamInfo->mResolutionWidth[i][pDecParam->mResolution-1] * 
                                    pDecStreamInfo->mResolutionHeight[i][pDecParam->mResolution-1];
		pHandler->mpOutBuffer[i] = (unsigned char*)malloc( sizeof(char)* pHandler->mOutBufferSz[i] );
	}   
        
    /* Print some information for the verbose mode. */
    if( gTestappConfig.mVerbose )
    {    
        /*    
		TST_RESM( TINFO, "Number Of Components = %d", pDecStreamInfo->mComponent);
		TST_RESM( TINFO, "Max Resolutions Supported = %d ", pDecStreamInfo->mResolution );
		TST_RESM( TINFO, "Max Resolution Width = %d Max Resolution Height = %d (for Component 0)",
						 pDecStreamInfo->mResolutionWidth[0][pDecStreamInfo->mResolution-1],
						 pDecStreamInfo->mResolutionHeight[0][pDecStreamInfo->mResolution-1] );*/       
    }	      
    
    /* Will assign focus, if it is not assigned. */
    if( gThreadWithFocus == -1 )
        gThreadWithFocus = pHandler->mIndex;    
    
    /*************************/
    /* Decode Jpeg2K stream. */
    /*************************/    
    CALL_JPEG2K_DECODER(
        DecodeJ2Kstream( pDecObject, 
                         pHandler->mpOutBuffer, 
                         pHandler->mpInpBuffer, 
                         pHandler->mInpBufferSz ),
        "DecodeJ2Kstream" );
    
    /*********************************/
    /* Output the decompressed data. */
    /*********************************/
    DoDataOutput( pHandler );
    
    /*******************/
    /* Calculate PSNR. */
    /*******************/    
    pHandler->mIsPsnrCalculated = FALSE;
    if( Util_StrICmp(pParams->mReferenceFileNameY, NA) )
    {        
        uint8 * pRefData[SIZ_COMPONENTSIZE] = {0};
        pRefData[0] = Util_ReadFile( pParams->mReferenceFileNameY, NULL );
        if( !pRefData[0] )
        {
            TST_RESM( TWARN, "%s : Can't open \'%s\'. PSNR will not be calculated.",
                __FUNCTION__, pParams->mReferenceFileNameY );
        }
        else
        {
            if( Util_StrICmp( pParams->mReferenceFileNameU, NA ) )
            {
                pRefData[1] = Util_ReadFile( pParams->mReferenceFileNameU, NULL );
                if( !pRefData[1] )
                {
                    TST_RESM( TWARN, "%s : Can't open \'%s\'. PSNR will not be calculated.",
                        __FUNCTION__, pParams->mReferenceFileNameY );
                    SAFE_DELETE( pRefData[0] );
                }
                else
                {
                    assert( Util_StrICmp( pParams->mReferenceFileNameV, NA ) );
                    pRefData[2] = Util_ReadFile( pParams->mReferenceFileNameV, NULL );
                    if( !pRefData[2] )
                    {
                        TST_RESM( TWARN, "%s : Can't open \'%s\'. PSNR will not be calculated.",
                            __FUNCTION__, pParams->mReferenceFileNameV );
                        SAFE_DELETE( pRefData[0] );
                        SAFE_DELETE( pRefData[1] );
                    }
                    else
                    {
                        Util_CalculatePSNR( pHandler, pRefData );
                        pHandler->mIsPsnrCalculated = TRUE;
                        SAFE_DELETE( pRefData[0] );
                        SAFE_DELETE( pRefData[1] );
                        SAFE_DELETE( pRefData[2] );
                    }
                }
            }
            else
            {
                Util_CalculatePSNR( pHandler, pRefData );
                pHandler->mIsPsnrCalculated = TRUE;
                SAFE_DELETE( pRefData[0] );
            }
        }
    }
   
    /************************/
    /* Cleanup the handler. */
    /************************/    
    CleanupHandler( pHandler );
    
    /* Return success. */
    return TPASS;
}


/*================================================================================================*/
/*================================================================================================*/
void DoDataOutput( sJpeg2KDecoderHandler * pHandler )
{
    assert( pHandler );    
    assert( pHandler->mpParams );     
    
    /*********************************************/
    /* Display the decompressed image if needed. */
    /*********************************************/
    if( gTestappConfig.mDisplayDecompressedImage )
    {
        CLS();
        Util_DisplayDecompressedImage( pHandler );
    }        

    /*****************************/
    /* Write a bitmap if needed. */
    /*****************************/
    if( gTestappConfig.mWriteBitmap )
        Util_WriteBitmap( pHandler );

    const J2KStreamInfoStruct_t * pDecStreamInfo = &pHandler->mDecStreamInfo;
    const sHandlerParams        * pParams        = pHandler->mpParams;

    char str[MAX_STR_LEN];    
	if( /*controlInfo->m_dumprgb*/ 1 )
	{
        int i;
		for( i = 0; i < pDecStreamInfo->mComponent; ++i )
		{
            FILE * pOutStream;
			sprintf( str, "%s_%c.raw", pParams->mOutputFileName, (i==0?'y':(i==1)?'u':'v') );
			pOutStream = fopen( str, "wb" );
			if( !pOutStream )
			{
				TST_RESM( TWARN, "Can't create %s", str );
                continue;
			}

			fwrite( pHandler->mpOutBuffer[i], sizeof(char), pHandler->mOutBufferSz[i], pOutStream );
			fclose( pOutStream );
		}
	}
}


/*================================================================================================*/
/*================================================================================================*/
void ResetHandler( sJpeg2KDecoderHandler * pHandler )
{
    assert( pHandler );
        
    memset( pHandler, 0, sizeof(sJpeg2KDecoderHandler) );    
    pHandler->mIndex             = 0;
    pHandler->mIsThreadFinished  = FALSE;    
}


/*================================================================================================*/
/*================================================================================================*/
void CleanupHandler( sJpeg2KDecoderHandler * pHandler )
{   
    assert( pHandler );

    /* Close all files we opened. */
    if( pHandler->mpInputStream )
    {
        fclose( pHandler->mpInputStream );
        pHandler->mpInputStream = NULL;
    }
    
    /**/
    SAFE_DELETE( pHandler->mDecParams.mpMemptr );
    pHandler->mDecParams.mMemSize = 0;

    /* Free input/output buffers. */
    SAFE_DELETE( pHandler->mpInpBuffer );
    pHandler->mInpBufferSz = 0;
    
    int i;
    for( i = 0; i < SIZ_COMPONENTSIZE; ++i )
    {    
        SAFE_DELETE( pHandler->mpOutBuffer[i] );
        pHandler->mOutBufferSz[i] = 0;        
    }
}


/*================================================================================================*/
/*================================================================================================*/
int DoBitmatch( sJpeg2KDecoderHandler * pHandler )
{
    assert( pHandler );
    assert( pHandler->mpParams );
    assert( pHandler->mIsPsnrCalculated == TRUE );

    if( gTestappConfig.mVerbose )
    {    
        if( pHandler->mDecStreamInfo.mComponent == 1 )
        {            
            TST_RESM( TINFO, "Thread[%lu] PSNRs : Y = %4.4f", 
                      pHandler->mIndex, pHandler->mCompPsnr[0] );
        }
        else
        {
            TST_RESM( TINFO, "Thread[%lu] PSNRs : Y = %4.4f, U = %4.4f, V = %4.4f", 
                      pHandler->mIndex,  
                      pHandler->mCompPsnr[0], pHandler->mCompPsnr[1], pHandler->mCompPsnr[2] );
        }
    }

    return pHandler->mIsPsnrCalculated;    
}


/*================================================================================================*/
/*================================================================================================*/
int DoFilesExist( const char * fname1, const char * fname2 )
{
    FILE * fstream1 = fopen( fname1, "r" );
    if( fstream1 )
    {
        fclose( fstream1 );
        FILE * fstream2 = fopen( fname2, "r" );
        if( fstream2 )
        {
            fclose( fstream2 );
            return TRUE;
        }
        else if( Util_StrICmp(fname2, NA) )
        {
            TST_RESM( TWARN, "%s not found", fname2 );
        }
    }
    else if( !Util_StrICmp(fname1, NA) )
    {
        TST_RESM( TWARN, "%s not found", fname1 );
    }
    return FALSE;
}


/*================================================================================================*/
/*================================================================================================*/
void HogCpu()
{
    while( 1 )
    {
	    sqrt( rand() );
    }
}


/*================================================================================================*/
/*================================================================================================*/
void MakeEntry( char entry[WORDS_IN_ENTRY][MAX_STR_LEN], int nEntry )
{
    sHandlerParams * pParams = (sHandlerParams*)malloc( sizeof(sHandlerParams) );    
    
    int n = 0;  
    strncpy( pParams->mInputFileName,      entry[n++], MAX_STR_LEN );   
    strncpy( pParams->mOutputFileName,     entry[n++], MAX_STR_LEN );
    strncpy( pParams->mReferenceFileNameY, entry[n++], MAX_STR_LEN );      
    strncpy( pParams->mReferenceFileNameU, entry[n++], MAX_STR_LEN );      
    strncpy( pParams->mReferenceFileNameV, entry[n++], MAX_STR_LEN );      

    pParams->mResolution = atoi( entry[n++] );
    //pParams->mLayers     = atoi( entry[n++] );
    pParams->mEntryIndex = nEntry;
    
    /* Adjust/check parameters here... */  
    
    if( !Util_StrICmp( pParams->mOutputFileName, NA ) )
    {        
        tst_brkm( TBROK, (void(*)())VT_jpeg2k_decoder_cleanup,
                  "Wrong output file name %s. The output file name must not be %s."
                  "Please check %s, line/entry #%lu", 
                  pParams->mOutputFileName, NA, gTestappConfig.mConfigFilename, nEntry + 1 );
    }
    
    int c1 = ( !Util_StrICmp(pParams->mReferenceFileNameY, NA) &&
               !Util_StrICmp(pParams->mReferenceFileNameU, NA) &&
               !Util_StrICmp(pParams->mReferenceFileNameV, NA) );
    int c2 = (  Util_StrICmp(pParams->mReferenceFileNameY, NA) &&
               !Util_StrICmp(pParams->mReferenceFileNameU, NA) &&
               !Util_StrICmp(pParams->mReferenceFileNameV, NA) );
    int c3 = (  Util_StrICmp(pParams->mReferenceFileNameY, NA) &&
                Util_StrICmp(pParams->mReferenceFileNameU, NA) &&
                Util_StrICmp(pParams->mReferenceFileNameV, NA) );
    if( !(c1 || c2 || c3) )
    {
        tst_brkm( TBROK, (void(*)())VT_jpeg2k_decoder_cleanup,
                  "Wrong reference file names. Please check %s, line/entry #%lu",
                  gTestappConfig.mConfigFilename, nEntry + 1 );
    }


    
    LList_PushBack( gpParamsList, pParams );
}


/*================================================================================================*/
/*================================================================================================*/
int NominalFunctionalityTest()
{
    sLinkedList * pNode;
    int i;
    int rv = TPASS;    
    sJpeg2KDecoderHandler * pHandler = gJ2KDecHandlers;     
        
    /* For the each entry */
    for( pNode = gpParamsList, i = 0; pNode; pNode = pNode->mpNext, ++i )
    {
        /* Reset the handler. */
        ResetHandler( pHandler );
    
        /* Get content. */
        pHandler->mpParams = (sHandlerParams*)pNode->mpContent;

        if( gTestappConfig.mVerbose )            
        {
            if( pHandler->mDecStreamInfo.mComponent == 1 )
            {
                TST_RESM( TINFO, "Thread[%lu] Input: %s, Reference: %s", 
                          pHandler->mIndex, 
                          pHandler->mpParams->mInputFileName, 
                          pHandler->mpParams->mReferenceFileNameY );
            }                          
            else
            {
                TST_RESM( TINFO, "Thread[%lu] Input: %s, ReferenceY: %s, ReferenceU: %s, ReferenceV: %s", 
                          pHandler->mIndex, 
                          pHandler->mpParams->mInputFileName, 
                          pHandler->mpParams->mReferenceFileNameY, 
                          pHandler->mpParams->mReferenceFileNameU,
                          pHandler->mpParams->mReferenceFileNameV );
            }
        }                        
                
        /* Run the Decoder. */
        rv += RunDecoder( pHandler );        
        CleanupHandler( pHandler );
    }
    
    return rv;    
}


/*================================================================================================*/
/*================================================================================================*/
int RobustnessTest()
{
    sLinkedList * pNode;
    int i;
    int rv = TPASS;    
    sJpeg2KDecoderHandler * pHandler = gJ2KDecHandlers;     
        
    /* For the each entry */
    for( pNode = gpParamsList, i = 0; pNode; pNode = pNode->mpNext, ++i )
    {
        /* Reset the handler. */
        ResetHandler( pHandler );
    
        /* Get content. */
        pHandler->mpParams = (sHandlerParams*)pNode->mpContent;

        if( gTestappConfig.mVerbose )            
        {
            if( pHandler->mDecStreamInfo.mComponent == 1 )
            {
                TST_RESM( TINFO, "Thread[%lu] Input: %s, Reference: %s", 
                          pHandler->mIndex, 
                          pHandler->mpParams->mInputFileName, 
                          pHandler->mpParams->mReferenceFileNameY );
            }                          
            else
            {
                TST_RESM( TINFO, "Thread[%lu] Input: %s, ReferenceY: %s, ReferenceU: %s, ReferenceV: %s", 
                          pHandler->mIndex, 
                          pHandler->mpParams->mInputFileName, 
                          pHandler->mpParams->mReferenceFileNameY, 
                          pHandler->mpParams->mReferenceFileNameU,
                          pHandler->mpParams->mReferenceFileNameV );
            }
        }                        
                
        /* Run the Decoder. */
        int res = RunDecoder( pHandler );        
        if( TPASS == res )
        {
            if( gTestappConfig.mVerbose )
            {
                TST_RESM( TWARN, "Robustness to %s failed", pHandler->mpParams->mInputFileName );
            }
            rv = TFAIL;
        }
        else
        {
            if( gTestappConfig.mVerbose )
            {
                TST_RESM( TPASS, "Robustness to %s passed", pHandler->mpParams->mInputFileName );
            }
        }
        CleanupHandler( pHandler );
    }
    
    return rv;
}


/*================================================================================================*/
/*================================================================================================*/
int ReLocatabilityTest()
{
    sLinkedList * pNode;
    int i, j;
    int rv = TPASS;    
    sJpeg2KDecoderHandler * pHandler = gJ2KDecHandlers;     
        
    /* For the each entry */
    for( pNode = gpParamsList, i = 0; pNode; pNode = pNode->mpNext, ++i )
    {    
        for( j = 0; j < gTestappConfig.mNumIter; ++j )
        {
            /* Reset the handler. */
            ResetHandler( pHandler );
    
            /* Get content. */
            pHandler->mpParams = (sHandlerParams*)pNode->mpContent;

            if( gTestappConfig.mVerbose && j == 0 )            
            {
                if( pHandler->mDecStreamInfo.mComponent == 1 )
                {
                    TST_RESM( TINFO, "Thread[%lu] Input: %s, Reference: %s", 
                              pHandler->mIndex, 
                              pHandler->mpParams->mInputFileName, 
                              pHandler->mpParams->mReferenceFileNameY );
                }                          
                else
                {
                    TST_RESM( TINFO, "Thread[%lu] Input: %s, ReferenceY: %s, ReferenceU: %s, ReferenceV: %s", 
                              pHandler->mIndex, 
                              pHandler->mpParams->mInputFileName, 
                              pHandler->mpParams->mReferenceFileNameY, 
                              pHandler->mpParams->mReferenceFileNameU,
                              pHandler->mpParams->mReferenceFileNameV );
                }
            }                        
                            
            /* Run the Decoder. */
            rv += RunDecoder( pHandler );        
            
            if( gTestappConfig.mVerbose )            
            {
                TST_RESM( TINFO, "Thread[%lu] Data memory was relocated", pHandler->mIndex );
            }
            CleanupHandler( pHandler );
        }
    }    
    return rv;    
}


/*================================================================================================*/
/*================================================================================================*/
int ReEntranceTest()
{
    int ReEntranceTestCore( sLinkedList * pHead );
    sLinkedList * pHead = gpParamsList;
    int rv = TPASS;
    int i;
    
    while( pHead )
    {   
        gThreadWithFocus = -1;     
        rv += ReEntranceTestCore( pHead );
        for( i = 0; i < gNumThreads && pHead; ++i )
        {
            ResetHandler( &gJ2KDecHandlers[i] );
            gJ2KDecHandlers[i].mIndex = i;
            pHead = pHead->mpNext;
        }            
    }
    
    return rv;
}


/*================================================================================================*/
/*================================================================================================*/
int ReEntranceTestCore( sLinkedList * pHead )
{
    assert( pHead );

    sLinkedList * pNode;
    int i;
    int rv = TPASS;    
    sJpeg2KDecoderHandler * pHandler;        
    
    /* Run all bitstreams in separate threads. */
    for( pNode = pHead, i = 0; pNode && i < gNumThreads; pNode = pNode->mpNext, ++i )
    {        
        pHandler = gJ2KDecHandlers + i;
        ResetHandler( pHandler );
        pHandler->mIndex = i;
        
        /* Get content. */
        pHandler->mpParams = (sHandlerParams*)pNode->mpContent;

        if( gTestappConfig.mVerbose )            
        {
            if( pHandler->mDecStreamInfo.mComponent == 1 )
            {
                TST_RESM( TINFO, "Thread[%lu] Input: %s, Reference: %s", 
                          pHandler->mIndex, 
                          pHandler->mpParams->mInputFileName, 
                          pHandler->mpParams->mReferenceFileNameY );
            }                          
            else
            {
                TST_RESM( TINFO, "Thread[%lu] Input: %s, ReferenceY: %s, ReferenceU: %s, ReferenceV: %s", 
                          pHandler->mIndex, 
                          pHandler->mpParams->mInputFileName, 
                          pHandler->mpParams->mReferenceFileNameY, 
                          pHandler->mpParams->mReferenceFileNameU,
                          pHandler->mpParams->mReferenceFileNameV );
            }
        }                        
        
        if( pthread_create( &pHandler->mThreadID, NULL, (void*)&RunDecoder, pHandler ) )
        {
            TST_RESM( TWARN, "%s : error creating thread %d", __FUNCTION__, i );
            return TFAIL;	    
        }	
    }    
    
    /* Wait for the each thread. */
    for( i = 0; i < gNumThreads; ++i )
    {
        pHandler = gJ2KDecHandlers + i;     
        pthread_join( pHandler->mThreadID, NULL );
    }
    for( i = 0; i < gNumThreads; ++i )
    {
	    pHandler = gJ2KDecHandlers + i;     
        rv += pHandler->mLtpRetval;
    }
    
    return rv;
}


/*================================================================================================*/
/*================================================================================================*/
int PreEmptionTest()
{
    return ReEntranceTest();
}


/*================================================================================================*/
/*================================================================================================*/
int EnduranceTest()
{
    int i;
    int rv = TPASS;
    
    for( i = 0; i < gTestappConfig.mNumIter; ++i )
    {
        if( gTestappConfig.mVerbose )            
            TST_RESM( TINFO, "The %d iteration has been started", i+1 );
        rv += NominalFunctionalityTest();	
        if( gTestappConfig.mVerbose )
            TST_RESM( TINFO, "The %d iteration has been completed", i+1 );
    }    
    
    return rv;
}


/*================================================================================================*/
/*================================================================================================*/
int LoadTest()
{
    int rv = TFAIL;
    pid_t pid;
    
    switch( pid = fork() )
    {
	    case -1:
	        TST_RESM( TWARN, "%s : fork failed", __FUNCTION__ );	    
    	    return rv;
	    case 0:       
            /* child process */
	        HogCpu();
    	default:               
            /* parent */
	        sleep(2);
	        rv = NominalFunctionalityTest();
    	    /* kill child process once decode/encode loop has ended */
	        if( kill( pid, SIGKILL ) != 0 )
	        {
        		TST_RESM( TWARN, "%s : Kill(SIGKILL) error", __FUNCTION__ );
	    	    return rv;
	        }
    }	

    return rv;
}


/*================================================================================================*/
/*================================================================================================*/
int Util_StrICmp( const char * s1, const char *s2 )
{
    for (;;)
    {
        int c1 = *s1;
        int c2 = *s2;
        if( isupper(c1) )
            c1 = tolower(c1);
        if( isupper(c2) )
            c2 = tolower(c2);
        if( c1 != c2 )
            return c1 - c2;
        if( c1 == '\0' )
            break;
        ++s1;
        ++s2;
    }
    return 0;
}


/*================================================================================================*/
/*================================================================================================*/
void Util_WriteBitmap( const sJpeg2KDecoderHandler * pHandler )
{
    typedef long           LONG;
    typedef unsigned short WORD;
    typedef unsigned long  DWORD;

    /*************************/
    /* Get width and height. */
    /*************************/
    uint32 width  = pHandler->mDecStreamInfo.mResolutionWidth[0][pHandler->mDecParams.mResolution-1];
    uint32 height = pHandler->mDecStreamInfo.mResolutionHeight[0][pHandler->mDecParams.mResolution-1];
    size_t imgSz  = width * height * 3;

    /*******************************/
    /* Check for image dimensions. */
    /*******************************/
    /*if( (width % 2) || (height % 2) )
    {
        TST_RESM( TWARN, "%s : Bitmap must have dimensions divisible by 2", __FUNCTION__ );
        return;
    }*/

    /*************************/
    /* Fill up bitmap header */
    /*************************/
    WORD  fileHdrbfType          = 0x4D42;
    DWORD fileHdrbfSize          = 14 + 40 + imgSz;
    WORD  fileHdrbfReserved1     = 0;
    WORD  fileHdrbfReserved2     = 0;    
    DWORD fileHdrbfOffBits       = 14 + 40;

    DWORD infoHdrbiSize          = 40;    
    LONG  infoHdrbiWidth         = width;
    LONG  infoHdrbiHeight        = height;
    WORD  infoHdrbiPlanes        = 1;
    WORD  infoHdrbiBitCount      = 24;
    DWORD infoHdrbiCompression   = 0;
    DWORD infoHdrbiSizeImage     = imgSz;
    LONG  infoHdrbiXPelsPerMeter = 0;
    LONG  infoHdrbiYPelsPerMeter = 0;
    DWORD infoHdrbiClrUsed       = 0;
    DWORD infoHdrbiClrImportant  = 0;

    /**********************/
    /* Convert YUV to RGB */
    /**********************/
    uint8 * pPixels = (uint8*)alloc_slow( imgSz );
    if( !pPixels )
    {
        TST_RESM( TWARN, "%s : Can't allocate %d bytes", __FUNCTION__, imgSz );
        return;
    }

    uint8 ctType = ( pHandler->mDecStreamInfo.mColorTransformFlag & 0x01 );
    const uint8 * pCompData[3];
    pCompData[0] = pHandler->mpOutBuffer[0];
    pCompData[1] = pHandler->mpOutBuffer[1];
    pCompData[2] = pHandler->mpOutBuffer[2];
    Util_ConvertYuvToRgb( pCompData, pPixels, width*height, ctType, pHandler->mDecStreamInfo.mComponent );

    /*****************/
    /* Write bitmap. */
    /*****************/
    char str[MAX_STR_LEN];
    sprintf( str, "%s.bmp", pHandler->mpParams->mOutputFileName );
    FILE * pOutStream = fopen( str, "wb" );
    if( pOutStream )
    {    
        fwrite( &fileHdrbfType,            1, sizeof(fileHdrbfType),          pOutStream );        
        fwrite( &fileHdrbfSize,            1, sizeof(fileHdrbfSize),          pOutStream ); 
        fwrite( &fileHdrbfReserved1,       1, sizeof(fileHdrbfReserved1),     pOutStream ); 
        fwrite( &fileHdrbfReserved2,       1, sizeof(fileHdrbfReserved2),     pOutStream );       
        fwrite( &fileHdrbfOffBits,         1, sizeof(fileHdrbfOffBits),       pOutStream );

        fwrite( &infoHdrbiSize,            1, sizeof(infoHdrbiSize),          pOutStream );
        fwrite( &infoHdrbiWidth,           1, sizeof(infoHdrbiWidth),         pOutStream ); 
        fwrite( &infoHdrbiHeight,          1, sizeof(infoHdrbiHeight),        pOutStream );
        fwrite( &infoHdrbiPlanes,          1, sizeof(infoHdrbiPlanes),        pOutStream );
        fwrite( &infoHdrbiBitCount,        1, sizeof(infoHdrbiBitCount),      pOutStream );
        fwrite( &infoHdrbiCompression,     1, sizeof(infoHdrbiCompression),   pOutStream ); 
        fwrite( &infoHdrbiSizeImage,       1, sizeof(infoHdrbiSizeImage),     pOutStream );
        fwrite( &infoHdrbiXPelsPerMeter,   1, sizeof(infoHdrbiXPelsPerMeter), pOutStream );
        fwrite( &infoHdrbiYPelsPerMeter,   1, sizeof(infoHdrbiYPelsPerMeter), pOutStream );
        fwrite( &infoHdrbiClrUsed,         1, sizeof(infoHdrbiClrUsed),       pOutStream );
        fwrite( &infoHdrbiClrImportant,    1, sizeof(infoHdrbiClrImportant),  pOutStream );

        int i;
        uint8 * ptr = pPixels + width * 3 * (height);
        for( i = 0; i < height; ++i )
        {
            int j;
            ptr -= width * 3;
            for( j = 0; j < width; ++j )
            {
                uint8 b = *ptr++;
                uint8 g = *ptr++;
                uint8 r = *ptr++;
                fwrite( &r, 1, 1, pOutStream ); 
                fwrite( &g, 1, 1, pOutStream );
                fwrite( &b, 1, 1, pOutStream );
            }
            ptr -= width * 3;
        }
        fclose( pOutStream );
    }
    else
    {
        TST_RESM( TWARN, "%s : Can't create \'%s\'", __FUNCTION__, str );
    }

    /************/
    /* Cleanup. */
    /************/
    SAFE_DELETE( pPixels );
}


/*================================================================================================*/
/*================================================================================================*/
void Util_DisplayDecompressedImage( const sJpeg2KDecoderHandler * pHandler )
{
    assert( pHandler );
    const framebuffer_t * pFb = get_framebuffer();
    assert( pFb );
    
    /*************************/
    /* Get width and height. */
    /*************************/
    uint32 width  = pHandler->mDecStreamInfo.mResolutionWidth[0][pHandler->mDecParams.mResolution-1];
    uint32 height = pHandler->mDecStreamInfo.mResolutionHeight[0][pHandler->mDecParams.mResolution-1];
    size_t pitch  = width * 3;
    
    /**********************/
    /* Convert YUV to RGB */
    /**********************/
    uint8 * pPixels = (uint8*)alloc_slow( pitch * height );
    if( !pPixels )
    {
        TST_RESM( TWARN, "%s : Can't allocate %d bytes", __FUNCTION__, pitch * height );
        return;
    }

    uint8 ctType = ( pHandler->mDecStreamInfo.mColorTransformFlag & 0x01 );
    const uint8 * pCompData[3];
    pCompData[0] = pHandler->mpOutBuffer[0];
    pCompData[1] = pHandler->mpOutBuffer[1];
    pCompData[2] = pHandler->mpOutBuffer[2];
    Util_ConvertYuvToRgb( pCompData, pPixels, width*height, ctType, pHandler->mDecStreamInfo.mComponent );        

        
    /***************************/
    /* Draw the each scanline. */
    /***************************/
    int i;
    uint8 * pPtr = pPixels;
    pixel_format_e pfmt = PF_RGB_888;
    for( i = 0; i < height; ++i )
    {
        pFb->draw_scanline( pPtr, 0, i, width, pfmt );
        pPtr += pitch;
    }
    
    /************/
    /* Cleanup. */
    /************/
    SAFE_DELETE( pPixels );
}


/*================================================================================================*/
/*================================================================================================*/
void Util_ConvertYuvToRgb( const uint8 * pCompData[3], uint8 * pRGBBufPtr, uint32 bufSize, uint8 ctType, int ncomp )
{
    #define LIMIT(X) (((X)>255)?(255):(((X)<0)? (0):(X)))
    #define FIX_MUL(a,b) ((a)*(b))
    
    const uint8 * yptr;
    const uint8 * cbptr;
    const uint8 * crptr;
    int   r,g,b;
    double R,G,B;
    uint32 i;
    
    int y,cb,cr;
    uint8 *rgbPtr = pRGBBufPtr;
    
    yptr  = pCompData[0];
    cbptr = pCompData[1];
    crptr = pCompData[2];
    
    if( ctType && ncomp == 3 ) //5x3
    {        
        for(i = 0; i < bufSize; ++i )
        {
            r = yptr[i] - 128;
            g = cbptr[i] - 128;
            b = crptr[i] - 128;
            
            G = r - ((b + g) >> 2);
            R = b + G; 
            B = g + G; 
            
            *rgbPtr++  =  (uint8)(LIMIT(R+128));
            *rgbPtr++  =  (uint8)(LIMIT(G+128));
            *rgbPtr++  =  (uint8)(LIMIT(B+128));
        }
    }
    else if( ncomp == 3 ) //9x7
    {
        for( i = 0;i < bufSize; ++i )
        {
            y  =   (int) yptr[i];
            cb =   (int) cbptr[i];
            cr =   (int) crptr[i];
            
            R = (double)(y + FIX_MUL((cr-128), 1.401977));
            G = (double)(y - FIX_MUL((cb-128), 0.344116) - FIX_MUL((cr-128), 0.714111));
            B = (double)(y + FIX_MUL((cb-128), 1.771972));
            
            *rgbPtr++  = (uint8)LIMIT(R);
            *rgbPtr++  = (uint8)LIMIT(G);
            *rgbPtr++  = (uint8)LIMIT(B);
        }
    }
    else
    {
        for(i = 0; i < bufSize; ++i )
        {
            r = yptr[i];
            
            G = r;
            R = r; 
            B = r; 
            
            *rgbPtr++  =  (uint8)(LIMIT(R));
            *rgbPtr++  =  (uint8)(LIMIT(G));
            *rgbPtr++  =  (uint8)(LIMIT(B));
        }        
    }
}


/*================================================================================================*/
/*================================================================================================*/
void Util_CalculatePSNR( sJpeg2KDecoderHandler * pHandler, uint8 * pRefData[SIZ_COMPONENTSIZE] )
{
    // PSNR for Entire Image -  Not for a Given Resolution
    
    assert( pHandler );
    
    J2KStreamInfoStruct_t * pDecStreamInfo = &pHandler->mDecStreamInfo;
    
    uint32 i, j;
    //double yTotalError = 0;
    
    int32  compErr = 0;
    double compErrSq[SIZ_COMPONENTSIZE] = {0.0};
    double avgCompErrSq = 0;
    
    
    for( i = 0; i < pDecStreamInfo->mComponent; ++i )
    {
        assert( pRefData[i] );
        for( j = 0; j < pHandler->mOutBufferSz[i]; ++j )
        {
            compErr = pRefData[i][j] - pHandler->mpOutBuffer[i][j];
            compErrSq[i] += (double)(compErr * compErr);
        }
    }
    
    for( i = 0; i < pDecStreamInfo->mComponent; ++i )
    {
        avgCompErrSq = compErrSq[i] / pHandler->mOutBufferSz[i];
        if( avgCompErrSq > 0.0 )
            pHandler->mCompPsnr[i] = 10 * log10( 65025.0 / avgCompErrSq );
    }
}


/*================================================================================================*/
/*================================================================================================*/
uint8 * Util_ReadFile( const char * filename, size_t * pSz )
{
    FILE * pInStream = fopen( filename, "rb" );    
    if( pInStream )
    {        
        fseek( pInStream, 0, SEEK_END );
        size_t sz = ftell( pInStream );
        fseek( pInStream, 0, SEEK_SET );
        uint8 * pData = (uint8*)alloc_fast( sizeof(char) * sz );
        fread( pData, 1, sz, pInStream );
        fclose( pInStream );
        if( pSz ) 
            *pSz = sz;
        return pData;
    }
    return NULL;
}


/*================================================================================================*/
/*================================================================================================*/
void Util_SwapBytes( short * pWords, int count )
{
    char * pByte;
    while( count-- )
    {
        pByte = (char*)(pWords+count);
        pByte[0] ^= pByte[1] ^= pByte[0] ^= pByte[1];
    }
}

#ifdef __cplusplus
}
#endif

