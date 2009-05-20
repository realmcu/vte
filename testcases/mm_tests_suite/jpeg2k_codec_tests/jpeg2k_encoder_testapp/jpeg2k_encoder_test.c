/*
  Copyright (C) 2004, Freescale Semiconductor, Inc. All Rights Reserved
  THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
  BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
  Freescale Semiconductor, Inc.
*/

/**
@file jpeg2k_encoder_test.c

@brief VTE C header template

@par Portability:
        Indicate if this module is portable to other compilers or platforms.
        If not, indicate specific reasons why is it not portable.
*/

/* REVISION HISTORY 

Author (core ID)      Date         CR Number    Description of Changes
-------------------   ----------   ----------   ------------------------------
D.Simakov / smkd001c  30/08/2005   TLSbo53250   Initial version
D.Simakov / smkd001c  06/12/2005   TLSbo59888   Bit-to-bit matching with
                                                reference was added
*/

#ifdef __cplusplus
extern "C"{
#endif

/*
                                        INCLUDE FILES
*/
/* Verification Test Environment Include Files */
#include "jpeg2k_encoder_test.h"

#include <pthread.h>
#include <math.h>
#include <stdarg.h>
#include <ctype.h>

/* JPEG 2000 Encoder API. */
#include <j2kEncoderInterface.h>
#include <j2kEncoderError.h>
#include <j2kDecoderInterface.h>

#include "stuff/mem_stat.h"

/*
                                        LOCAL MACROS
*/
#define MAX_THREADS 2
#define SAFE_DELETE(p) {if(p){Util_Free(p);pNULL;}}
#define NA "n/a"
#define M(m){printf("<<<--- %s --->>>\n",m);fflush(stdout);}

#define J2K_MAX_NO_OF_COMPONENTS 3
#define J2K_ENC_PROGRESSION_LRCP     0 //0000 0000
#define J2K_ENC_PROGRESSION_RLCP     1 //0000 0001
#define J2K_ENC_PROGRESSION_RPCL     2 //0000 0010
#define J2K_ENC_PROGRESSION_PCRL     3 //0000 0011
#define J2K_ENC_PROGRESSION_CPRL     4 //0000 0100

/**********************************************************************
* Macro name:  CALL_JPEG2K_ENCODER()
* Description: Macro checks for any error in function execution
               based on the return value. In case of error, the function exits.
**********************************************************************/
#define CALL_JPEG2K_ENCODER(Jpeg2KEncRoutine, name)   \
    pHandler->mLastJ2KEncError  Jpeg2KEncRoutine; \
    if( (pHandler->mLastJ2KEncError ! J2K_SUCCESSFUL) )\
    {\
        TST_RESM( TWARN, "%s : %s fails #%d", __FUNCTION__, name, pHandler->mLastJ2KEncError);\
  return TFAIL;\
 }

#define TST_RESM(s,format,...) \
{\
    if( LOAD ! gTestappConfig.mTestCase ) \
        pthread_mutex_lock( &gMutex );\
    tst_resm((s), (format), ##__VA_ARGS__);\
    if( LOAD ! gTestappConfig.mTestCase ) \
        pthread_mutex_unlock( &gMutex );\
}

#define DECLARE_Jpeg2K_WriteStream(n) \
    int32 Jpeg2K_WriteStream##n( uint8 * pOutBuf, uint32 bufLen ) \
    { \
        return Jpeg2K_WriteStream( n, pOutBuf, bufLen ); \
    }

#define DECLARE_Jpeg2K_ReadStream(n) \
    int32 Jpeg2K_ReadStream##n( uint8 * pInputBuf, J2KGetTileStruct_t * pTileInfo ) \
    { \
        return Jpeg2K_ReadStream( n, pInputBuf, pTileInfo ); \
    }

/*
                          LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
*/

/************************************************************************/
/*                                                                      */
/************************************************************************/
typedef struct
{
    char  mInputFileName[J2K_MAX_NO_OF_COMPONENTS][MAX_STR_LEN];
    char  mOutputFileName[MAX_STR_LEN];
    char  mRefFileName[MAX_STR_LEN];

    uint32 mWidth;
    uint32 mHeight;
    uint32 mComponents;
    uint16 mQualityLevel;
    uint32 mResolution;
    uint8  mXSampleDist[J2K_MAX_NO_OF_COMPONENTS];
    uint8  mYSampleDist[J2K_MAX_NO_OF_COMPONENTS];
    uint8  mIsLossless;
    uint32 mProgOrder;
    int32  mTiledData;

    uint32 mEntryIndex;
} sHandlerParams;

/************************************************************************/
/*                                                                      */
/************************************************************************/
typedef struct
{
    unsigned long           mIndex;

    const sHandlerParams  * mpParams;
    FILE                  * mpInputStream[J2K_MAX_NO_OF_COMPONENTS];
    FILE                  * mpOutputStream;

    int                     mLastJ2KEncError;

    unsigned char         * mpInpBuffer[J2K_MAX_NO_OF_COMPONENTS];
    size_t                  mInpBufferSz[J2K_MAX_NO_OF_COMPONENTS];

    J2KEncoderObject_t      mEncObject;
 J2KEncodeParamStruct_t  mEncParams;
    size_t                  mActualOutputSz;

    /* PSNR info. */
   double                  mCompPsnr[J2K_MAX_NO_OF_COMPONENTS];
    int                     mIsPsnrCalculated[J2K_MAX_NO_OF_COMPONENTS];

    char                    mDecodedFileName[J2K_MAX_NO_OF_COMPONENTS][MAX_STR_LEN];
    char                    mRefFileName[J2K_MAX_NO_OF_COMPONENTS][MAX_STR_LEN];

    pthread_t               mThreadID;
    int                     mIsThreadFinished;
    int                     mLtpRetval;
} sJpeg2KEncoderHandler;

/*
                                       LOCAL CONSTANTS
*/


/*
                                       LOCAL VARIABLES
*/
static sJpeg2KEncoderHandler gJ2KEncHandlers[ MAX_THREADS ];
static int                   gNumThreads                      1;
static int                   gThreadWithFocus                 -1;
static sLinkedList         * gpParamsList                     NULL;
static pthread_mutex_t       gMutex;
fptrOutWrite                 gpJpeg2K_WriteStream[MAX_THREADS];
fptrGetImageData             gpJpeg2K_ReadStream[MAX_THREADS];

/*
                                       GLOBAL CONSTANTS
*/

/*
                                       GLOBAL VARIABLES
*/

/*
                                   LOCAL FUNCTION PROTOTYPES
*/
int32 Jpeg2K_ReadStream  ( uint32 threadIdx, uint8 * pInputBuf, J2KGetTileStruct_t * pTileInfo );
int32 Jpeg2K_WriteStream ( uint32 threadIdx, uint8 * pOutBuf, uint32 bufLen );

int  RunEncoder     ( void * ptr );
int  TestEngine     ( sJpeg2KEncoderHandler * pHandler );
void PrintProgress  ( sJpeg2KEncoderHandler * pHandler );
void ResetHandler   ( sJpeg2KEncoderHandler * pHandler );
void CleanupHandler ( sJpeg2KEncoderHandler * pHandler );
int  DoBitmatch     ( sJpeg2KEncoderHandler * pHandler );
int  DoBitmatchRef  ( sJpeg2KEncoderHandler * pHandler );
int  DoFilesExist   ( const char * fname1, const char * fname2 );
void HogCpu         ();
void MakeEntry      ( char entry[WORDS_IN_ENTRY][MAX_STR_LEN], int nEntry );

/* Test cases. */
int NominalFunctionalityTest ();
int ReLocatabilityTest       ();
int ReEntranceTest           ();
int PreEmptionTest           ();
int EnduranceTest            ();
int LoadTest                 ();

/* Helper functions. */
int     Util_StrICmp                   ( const char * s1, const char *s2 );
void    Util_ConvertYUVToRGB           ( const uint8 * pCompData[J2K_MAX_NO_OF_COMPONENTS], uint8 * pRGBBufPtr, uint32 bufSize, uint8 ctType, int ncomp );
void    Util_ConvertRGBtoYUV           ( uint8 * pCompData[J2K_MAX_NO_OF_COMPONENTS], uint32 imgSize, uint32 ctType );
void    Util_CalculatePSNR             ( sJpeg2KEncoderHandler * pHandler, uint8 * pRefData, uint32 compIdx );
uint8 * Util_ReadFile                  ( const char * filename, size_t * pSz );
void    Util_GetSubSampledImageBuffers ( uint8 * pDst, uint8 * pSrc, uint32 compIndex, J2KImageInfoStruct_t * pImgInfo );
int     Util_DecodeJ2KFile             ( sJpeg2KEncoderHandler * pHandler );
void  * Util_Malloc                    ( size_t sz );
void    Util_Free                      ( void * pPtr );

/*
                                       LOCAL FUNCTIONS
*/

DECLARE_Jpeg2K_WriteStream(0)
DECLARE_Jpeg2K_WriteStream(1)
DECLARE_Jpeg2K_WriteStream(2)
DECLARE_Jpeg2K_WriteStream(3)

DECLARE_Jpeg2K_ReadStream(0)
DECLARE_Jpeg2K_ReadStream(1)
DECLARE_Jpeg2K_ReadStream(2)
DECLARE_Jpeg2K_ReadStream(3)

/**/
/**/
int VT_jpeg2k_encoder_setup()
{
    pthread_mutex_init( &gMutex, NULL );

    int i;
    /* Reset all handlers. */
    for( i  0; i < MAX_THREADS; ++i )
        ResetHandler( gJ2KEncHandlers + i );

    gpJpeg2K_ReadStream[0]  Jpeg2K_ReadStream0;
    gpJpeg2K_ReadStream[1]  Jpeg2K_ReadStream1;
    //gpJpeg2K_ReadStream[2]  Jpeg2K_ReadStream2;
    //gpJpeg2K_ReadStream[3]  Jpeg2K_ReadStream3;

    gpJpeg2K_WriteStream[0]  Jpeg2K_WriteStream0;
    gpJpeg2K_WriteStream[1]  Jpeg2K_WriteStream1;
    //gpJpeg2K_WriteStream[2]  Jpeg2K_WriteStream2;
    //gpJpeg2K_WriteStream[3]  Jpeg2K_WriteStream3;


    /**/
    gpParamsList  (sLinkedList*)malloc(sizeof(sLinkedList));
    gpParamsList->mpNext  NULL;
    gpParamsList->mpContent  NULL;

    if( !ParseConfig( gTestappConfig.mConfigFilename ) )
    {
        TST_RESM( TWARN, "%s : can't parse the %s",
                  __FUNCTION__, gTestappConfig.mConfigFilename );
        return TFAIL;
    }

    /* Compute the actual number of threads. */
    if( RE_ENTRANCE  gTestappConfig.mTestCase ||
        PRE_EMPTION  gTestappConfig.mTestCase )
    {
        sLinkedList * pNode  gpParamsList;
        for( gNumThreads  0; pNode && gNumThreads < MAX_THREADS; ++gNumThreads )
            pNode  pNode->mpNext;
    }

    if( gTestappConfig.mVerbose )
    {
        TST_RESM( TINFO, "Jpeg2k encoder revision : %d", GetJ2KEncoderVersionNo() );
    }

    return TPASS;
}


/**/
/**/
int VT_jpeg2k_encoder_cleanup()
{
    pthread_mutex_destroy( &gMutex );

    if( gpParamsList )
        LList_Delete( gpParamsList );

    int i;
    for( i  0; i < MAX_THREADS; ++i )
    {
        CleanupHandler( gJ2KEncHandlers + i );
        ResetHandler( gJ2KEncHandlers + i );
    }

    return TPASS;
}


/**/
/**/
int VT_jpeg2k_encoder_test()
{
    int rv  TFAIL;

    switch( gTestappConfig.mTestCase )
    {
  case NOMINAL_FUNCTIONALITY:
   TST_RESM( TINFO, "Nominal functionality Test" );
   rv  NominalFunctionalityTest();
   TST_RESM( TINFO, "End of nominal functionality Test" );
   break;

        case RELOCATABILITY:
            TST_RESM( TINFO, "Relocatability Test" );
            rv  ReLocatabilityTest();
            TST_RESM( TINFO, "End relocatability Test" );
            break;

  case RE_ENTRANCE:
   TST_RESM( TINFO, "Re-entrance Test" );
   rv  ReEntranceTest();
   TST_RESM( TINFO, "End of re-entrance Test" );
   break;

  case PRE_EMPTION:
   TST_RESM( TINFO, "Pre-emption Test" );
   rv  PreEmptionTest();
   TST_RESM( TINFO, "End of pre-emption Test" );
   break;

  case ENDURANCE:
   TST_RESM( TINFO, "Endurance Test" );
   rv  EnduranceTest();
   TST_RESM( TINFO, "End of endurance Test" );
   break;

  case LOAD:
   TST_RESM( TINFO, "Load Test" );
   rv  LoadTest();
   TST_RESM( TINFO, "End of load Test" );
   break;

  default:
   TST_RESM( TINFO, "Wrong Test case" );
   break;
 }

    return rv;
}


/**/
/**/
int32 Jpeg2K_ReadStream( uint32 threadIdx, uint8 * pInputBuf, J2KGetTileStruct_t * pTileInfo )
{
    assert( threadIdx < MAX_THREADS );

    uint8 * pEncInpBuffer  gJ2KEncHandlers[ threadIdx ].mpInpBuffer[pTileInfo->mCompNum];
    if( pEncInpBuffer )
    {
        uint8 * pDstPtr  pInputBuf;
        uint8 * pSrcPtr  pEncInpBuffer +
                          pTileInfo->mYIndex * (pTileInfo->mCompWidth * pTileInfo->mTileHeight) +
                          pTileInfo->mXIndex * pTileInfo->mTileWidth;

        uint32 i;
        for( i  0; i<pTileInfo->mReqHeight; ++i )
        {
            memcpy( pDstPtr, pSrcPtr, pTileInfo->mReqWidth );

            pDstPtr + pTileInfo->mReqWidth;
            pSrcPtr + pTileInfo->mCompWidth;
        }
        return J2K_SUCCESSFUL;
    }

    return J2K_ENC_ERR_INPUT_BUFFER_READ_FAILED;
}


/**/
/**/
int32 Jpeg2K_WriteStream( uint32 threadIdx, uint8 * pOutBuf, uint32 bufLen )
{
    assert( threadIdx < MAX_THREADS );

    sJpeg2KEncoderHandler * pHandler  &gJ2KEncHandlers[threadIdx];
    if( pHandler->mpOutputStream )
    {
        fwrite( pOutBuf, sizeof(char), bufLen, pHandler->mpOutputStream );
        pHandler->mActualOutputSz + bufLen;

        return J2K_SUCCESSFUL;
    }
    return J2K_ENC_ERR_OUT_FILE_WRITE_FAILED;
}


/**/
/**/
int RunEncoder( void * ptr )
{
    assert( ptr );

    sJpeg2KEncoderHandler * pHandler  (sJpeg2KEncoderHandler*)ptr;
    assert( pHandler->mpParams );

    /* Set priority for the PRE_EMPTION Test case. */
    if( PRE_EMPTION  gTestappConfig.mTestCase )
    {
  int nice_inc  (int)(( 20.0f / gNumThreads ) * pHandler->mIndex);
  if( nice(nice_inc) < 0 )
  {
   TST_RESM( TWARN, "%s : nice(%d) has failed",
                      __FUNCTION__, nice_inc );
  }
    }

    /* Run TestEngine. */
    pHandler->mLtpRetval  TestEngine( pHandler );

    /* Perform bit-matching. */
    if( pHandler->mIsPsnrCalculated[0] && !Util_StrICmp( pHandler->mpParams->mRefFileName, NA ) )
    {
        if( !DoBitmatch( pHandler ) )
        {
            if( gTestappConfig.mVerbose )
            {
                TST_RESM( TFAIL, "Thread[%lu] Bitmatch failed", pHandler->mIndex );
            }

            pHandler->mLtpRetval  TFAIL;
        }
        else if( gTestappConfig.mVerbose )
        {
            TST_RESM( TINFO, "Thread[%lu] Bitmatch passed", pHandler->mIndex );
        }
    }
    else if( Util_StrICmp( pHandler->mpParams->mRefFileName, NA ) )
    {
        /* Perform bit-to-bit-matching */
        const char * fileName1  pHandler->mpParams->mOutputFileName;
        const char * fileName2  pHandler->mpParams->mRefFileName;
        if( !DoBitmatchRef( pHandler ) )
        {
            if( gTestappConfig.mVerbose )
            {
                TST_RESM( TFAIL, "Thread[%lu] Bitmatch failed (%s vs %s)",
                          pHandler->mIndex, fileName1, fileName2 );
                pHandler->mLtpRetval  TFAIL;
            }
        }
        else
        {
            if( gTestappConfig.mVerbose )
            {
                TST_RESM( TINFO, "Thread[%lu] Bitmatch passed (%s vs %s)",
                          pHandler->mIndex, fileName1, fileName2 );
            }
        }
    }

    /* Give the focus to an incomplete thread. */
    pHandler->mIsThreadFinished  TRUE;
    if( pHandler->mIndex  gThreadWithFocus )
    {
        int i;
        /* Search a first incomplete thread and assign them focus. */
  for( i  0; i < gNumThreads; ++i )
  {
   if( !gJ2KEncHandlers[i].mIsThreadFinished )
   {
    gThreadWithFocus  gJ2KEncHandlers[i].mIndex;
    break;
   }
  }
    }

    /* Return LTP result. */
    return pHandler->mLtpRetval;
}


/**/
/**/
int TestEngine( sJpeg2KEncoderHandler * pHandler )
{
    assert( pHandler );
    assert( pHandler->mpParams );

    const sHandlerParams   * pParams         pHandler->mpParams;
    J2KEncoderObject_t     * pEncObject      &pHandler->mEncObject;
    J2KEncodeParamStruct_t * pEncParam       &pHandler->mEncParams;
    J2KEncoderInitStruct_t   encInit;
    int                      i;
    size_t                   totalInputBufSz;

    /*****************************/
    /* Open all necessary files. */
    /*****************************/

    /* Input stream. */
    for( i  0; i < pParams->mComponents; ++i )
    {
        assert( Util_StrICmp(pParams->mInputFileName[i], NA) );
        pHandler->mpInputStream[i]  fopen( pParams->mInputFileName[i], "rb" );
        if( !pHandler->mpInputStream[i] )
        {
            tst_brkm( TBROK, (void(*)())VT_jpeg2k_encoder_cleanup,
                      "%s : Can't open input file \'%s\'",
                       __FUNCTION__, pParams->mInputFileName[i] );
        }
    }

    /* Output stream. */
    pHandler->mpOutputStream  fopen( pParams->mOutputFileName, "wb" );
    if( !pHandler->mpOutputStream )
    {
        tst_brkm( TBROK, (void(*)())VT_jpeg2k_encoder_cleanup,
                    "%s : Can't create output file \'%s\'",
                  __FUNCTION__, pParams->mOutputFileName );
    }

    /****************************/
    /* Init the Jpeg2K encoder. */
    /****************************/
    if( pParams->mTiledData )
 {
  encInit.mIsTiledInput      1;
  encInit.mCbGetImageData    gpJpeg2K_ReadStream[pHandler->mIndex];
  encInit.mCbWriteJ2KStream  gpJpeg2K_WriteStream[pHandler->mIndex];
 }
 else
 {
  encInit.mIsTiledInput      0;
  encInit.mCbGetImageData    0;
  encInit.mCbWriteJ2KStream  gpJpeg2K_WriteStream[pHandler->mIndex];
 }
    CALL_JPEG2K_ENCODER(
        InitJ2KEncoder( pEncObject, &encInit ),
        "InitJ2KEncoder" );

    /***************************/
    /* Set encoder parameters. */
    /***************************/
 pEncParam->mImInfo.mXRsiz  (uint8*)Util_Malloc( J2K_MAX_NO_OF_COMPONENTS );
 pEncParam->mImInfo.mYRsiz  (uint8*)Util_Malloc( J2K_MAX_NO_OF_COMPONENTS );
    assert( pEncParam->mImInfo.mXRsiz && pEncParam->mImInfo.mYRsiz );

 pEncParam->mImInfo.mImWidth       pParams->mWidth;
 pEncParam->mImInfo.mImHeight      pParams->mHeight;
 pEncParam->mImInfo.mBpp           8;
 pEncParam->mImInfo.mImComponents  pParams->mComponents;

 for( i  0; i < pEncParam->mImInfo.mImComponents; ++i )
 {
  pEncParam->mImInfo.mXRsiz[i]  pParams->mXSampleDist[i];
  pEncParam->mImInfo.mYRsiz[i]  pParams->mYSampleDist[i];
 }

 /* Read R G B files or 1 Y file. */
 size_t imageSz  pEncParam->mImInfo.mImWidth * pEncParam->mImInfo.mImHeight;
 for( i  0; i < pEncParam->mImInfo.mImComponents; ++i )
 {
        pHandler->mpInpBuffer[i]  Util_ReadFile( pParams->mInputFileName[i],
                                                  &pHandler->mInpBufferSz[i] );
        if( pHandler->mInpBufferSz[i] ! imageSz )
        {
            TST_RESM( TWARN, "%s : Probably \'%s\' is corrupt or dimensions %lu x %lu are invalid.",
                      __FUNCTION__, pParams->mInputFileName[i],
                      pParams->mWidth, pParams->mHeight );
            return TFAIL;
        }

  totalInputBufSz + pHandler->mInpBufferSz[i];
 }

    if( 3  pParams->mComponents )
 {
  /* Do ICT or RCT. */
  Util_ConvertRGBtoYUV( pHandler->mpInpBuffer, imageSz, pParams->mIsLossless );
 }

 uint8 subSample  ( ( (pParams->mComponents  1) && ((pParams->mXSampleDist[0] ! 1) || (pParams->mYSampleDist[0] ! 1)) ) ||
            ( (pParams->mComponents  3) && ((pParams->mXSampleDist[0] ! 1) || (pParams->mYSampleDist[0] ! 1) ||
              (pParams->mXSampleDist[1] ! 1) || (pParams->mYSampleDist[1] ! 1) ||
              (pParams->mXSampleDist[2] ! 1) || (pParams->mYSampleDist[2] ! 1)) ) );

    if( subSample )
 {
  if( 3  pParams->mComponents )
  {
   if( pParams->mXSampleDist[0] ! pParams->mXSampleDist[1] ? 1 :
       (pParams->mXSampleDist[0] ! pParams->mXSampleDist[2] ? 1 : 0 ) )
   {
    assert( !"not supported" );
   }

   if( pParams->mYSampleDist[0] ! pParams->mYSampleDist[1] ? 1 :
       (pParams->mYSampleDist[0] ! pParams->mYSampleDist[2] ? 1 : 0 ) )
   {
    assert( !"not supported" );
   }
  }

  totalInputBufSz  0;
  for( i  0; i < pEncParam->mImInfo.mImComponents; ++i )
  {
   pHandler->mInpBufferSz[i] 
    ((pParams->mWidth+pParams->mXSampleDist[i]-1)/pParams->mXSampleDist[i])*
       ((pParams->mHeight+pParams->mYSampleDist[i]-1)/pParams->mYSampleDist[i]);

   uint8 * pRefBuffer  (uint8*)Util_Malloc( pHandler->mInpBufferSz[i] );
            if( !pRefBuffer )
            {
                tst_brkm( TBROK, (void(*)())VT_jpeg2k_encoder_cleanup,
                          "%s : Can't allocate %d bytes to holds intermediate results",
                          __FUNCTION__, pHandler->mInpBufferSz[i] );
            }

   Util_GetSubSampledImageBuffers( pRefBuffer,
                                            pHandler->mpInpBuffer[i],
                                            i,
           &pEncParam->mImInfo);

   totalInputBufSz + pHandler->mInpBufferSz[i];
            memcpy( pHandler->mpInpBuffer[i], pRefBuffer, pHandler->mInpBufferSz[i] );
            SAFE_DELETE( pRefBuffer );
  }
 }
 else
 {
  for( i  0; i < pEncParam->mImInfo.mImComponents; ++i )
  {
   pHandler->mpInpBuffer[i]  pHandler->mpInpBuffer[i];
  }
 }

    /*****************************/
    /* Dump the reference data.  */
    /*****************************/
    #if 1
    char str[MAX_STR_LEN];
    strncpy( str, pParams->mOutputFileName, MAX_STR_LEN );
    str[strlen(str)-4]  0;
    for( i  0; i < pParams->mComponents; ++i)
    {
        sprintf( pHandler->mRefFileName[i], "%s_Ref_%c.raw",
                 str, (i0?'y':(i1)?'u':'v') );
        FILE * out  fopen( pHandler->mRefFileName[i], "wb" );
        if( !out )
        {
            TST_RESM( TWARN, "%s : Can't create \'%s\'", __FUNCTION__, pHandler->mRefFileName );
            continue;
        }

        fwrite( pHandler->mpInpBuffer[i], sizeof(char), pHandler->mInpBufferSz[i], out );
        fclose( out );
    }
    #endif

 pEncParam->mResolutions   pParams->mResolution;
 pEncParam->mLayers        1;
 pEncParam->mpMemptr       NULL;
 if( !((pParams->mProgOrder  J2K_ENC_PROGRESSION_LRCP) ||
    (pParams->mProgOrder  J2K_ENC_PROGRESSION_RLCP)) )
 {
  assert( !"not supported" );
 }

 pEncParam->mProgOrder         pParams->mProgOrder;
 pEncParam->mCompressionType   pParams->mIsLossless;
 pEncParam->mQLevel            pParams->mQualityLevel;

 /* Obtain Memory Info. */
    uint32 memSz;
    CALL_JPEG2K_ENCODER(
        GetJ2KMemInfo( pEncObject, pEncParam, &memSz ),
        "GetJ2KMemInfo" );

    pEncParam->mpMemptr  Util_Malloc( memSz );
    if( !pEncParam->mpMemptr )
    {
        tst_brkm( TBROK, (void(*)())VT_jpeg2k_encoder_cleanup,
                  "%s : Can't allocate %lu bytes memory", __FUNCTION__, memSz );
    }

    CALL_JPEG2K_ENCODER(
        SetJ2KEncodeParams( pEncObject, pEncParam ),
        "SetJ2KEncodeParams" );

    /* Print some information for the verbose mode. */
    if( gTestappConfig.mVerbose )
    {
        if( pParams->mIsLossless  0 )
        {
            TST_RESM( TINFO, "Thread[%lu] Lossy encoding", pHandler->mIndex );
        }
    }

#ifdef DEBUG_TEST
    if( NOMINAL_FUNCTIONALITY  gTestappConfig.mTestCase )
        MemStat_GetStat();
#endif

    /* Will assign focus, if it is not assigned. */
    if( gThreadWithFocus  -1 )
        gThreadWithFocus  pHandler->mIndex;

    /******************/
    /* Encode stream. */
    /******************/
    const uint8 * pInpBuffer[3];
    pInpBuffer[0]  pHandler->mpInpBuffer[0];
    pInpBuffer[1]  pHandler->mpInpBuffer[1];
    pInpBuffer[2]  pHandler->mpInpBuffer[2];
    CALL_JPEG2K_ENCODER(
        EncodeJ2K( pEncObject, pParams->mTiledData ? NULL : pInpBuffer ),
        "EncodeJ2K" );

    if( gTestappConfig.mVerbose )
    {
        TST_RESM( TINFO, "Thread[%lu] Encoding OK", pHandler->mIndex );
    }

    /***********************/
    /* Decode encoded j2k. */
    /***********************/
    if( !Util_StrICmp( pParams->mRefFileName, NA ) )
    {
        if( pHandler->mpOutputStream )
        {
            fclose( pHandler->mpOutputStream );
            pHandler->mpOutputStream  NULL;
        }
        if( TPASS ! Util_DecodeJ2KFile( pHandler ) )
            return TFAIL;
    }

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
void ResetHandler( sJpeg2KEncoderHandler * pHandler )
{
    assert( pHandler );

    memset( pHandler, 0, sizeof(sJpeg2KEncoderHandler) );
    pHandler->mIndex              0;
    pHandler->mIsThreadFinished   FALSE;
}


/**/
/**/
void CleanupHandler( sJpeg2KEncoderHandler * pHandler )
{
    assert( pHandler );

    /******************************/
    /* Close all files we opened. */
    /******************************/
    int i;
    for( i  0; i < J2K_MAX_NO_OF_COMPONENTS; ++i )
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

    SAFE_DELETE( pHandler->mEncParams.mImInfo.mXRsiz );
    SAFE_DELETE( pHandler->mEncParams.mImInfo.mYRsiz );

    /**/
    SAFE_DELETE( pHandler->mEncParams.mpMemptr );

    for( i  0; i < J2K_MAX_NO_OF_COMPONENTS; ++i )
    {
        SAFE_DELETE( pHandler->mpInpBuffer[i] );
        pHandler->mInpBufferSz[i]  0;
    }
}


/**/
/**/
int DoBitmatch( sJpeg2KEncoderHandler * pHandler )
{
    assert( pHandler );
    assert( pHandler->mpParams );
    assert( pHandler->mIsPsnrCalculated[0]  TRUE );

    int bmResult  TRUE;
    if( 1 )
    {
        int i;
        for( i  0; i < pHandler->mpParams->mComponents; ++i )
        {
            if( pHandler->mIsPsnrCalculated[i] )
            {
                if( pHandler->mCompPsnr[i] )
                {
                    if( gTestappConfig.mVerbose )
                    {
                        TST_RESM( TINFO, "Thread[%lu] %c PSNR is %4.4f",
                                  pHandler->mIndex,
                                  i0 ? 'Y' : (i1 ? 'U' : 'V'), pHandler->mCompPsnr[i] );
                    }
                }
                else
                {
                    if( pHandler->mpParams->mIsLossless  0 )
                    {
                        bmResult  FALSE;
                    }

                    if( gTestappConfig.mVerbose )
                    {
                        TST_RESM( TINFO, "Thread[%lu] %c plane matches one to one with reference",
                                  pHandler->mIndex,
                                  i0 ? 'Y' : (i1 ? 'U' : 'V') );
                    }
                }
            }
        }
    }

    return bmResult;
}


/**/
/**/
int DoBitmatchRef( sJpeg2KEncoderHandler * pHandler )
{
    /* Assertions */
    assert( pHandler );
    assert( pHandler->mpParams );

    int out, ref;
    struct stat fstat_out, fstat_ref;
    char *fptr_out, *fptr_ref;
    size_t filesize;
    int i;

    const char * fname1  pHandler->mpParams->mOutputFileName;
    const char * fname2  pHandler->mpParams->mRefFileName;

    if( (out  open(fname1, O_RDONLY)) < 0 )
    {
        return FALSE;
    }
    if( (ref  open(fname2, O_RDONLY)) < 0 )
    {
    close(out);
        return FALSE;
    }
    fstat( out, &fstat_out );
    fstat( ref, &fstat_ref );
    if( fstat_out.st_size ! fstat_ref.st_size )
    {
        close(out);
        close(ref);
        return FALSE;
    }
    filesize  fstat_out.st_size;
    fptr_out  (char*)mmap( 0, filesize, PROT_READ, MAP_SHARED, out, 0 );
    if( fptr_out  MAP_FAILED )
    {
        close( out );
        close( ref );
        return FALSE;
    }
    fptr_ref  (char*) mmap(0, filesize, PROT_READ, MAP_SHARED, ref, 0);
    if( fptr_ref  MAP_FAILED )
    {
        close( out );
        close( ref );
        return FALSE;
    }
    close( out );
    close( ref );
    for( i  0; i < filesize; ++i )
    {
        if( *(fptr_ref + i) ! *(fptr_out + i) )
        {
            munmap( fptr_ref, fstat_ref.st_size );
            munmap( fptr_out, fstat_out.st_size );
            return FALSE;
        }
    }
    munmap( fptr_ref, filesize );
    munmap( fptr_out, filesize );
    return TRUE;
}


/**/
/**/
int DoFilesExist( const char * fname1, const char * fname2 )
{
    FILE * fstream1  fopen( fname1, "r" );
    if( fstream1 )
    {
        fclose( fstream1 );
        FILE * fstream2  fopen( fname2, "r" );
        if( fstream2 )
        {
            fclose( fstream2 );
            return TRUE;
        }
        else if( Util_StrICmp(fname2, NA) )
        {
            if( gTestappConfig.mVerbose )
            {
                TST_RESM( TWARN, "%s not found", fname2 );
            }
        }
    }
    else if( !Util_StrICmp(fname1, NA) )
    {
        if( gTestappConfig.mVerbose )
        {
            TST_RESM( TWARN, "%s not found", fname1 );
        }
    }
    return FALSE;
}


/**/
/**/
void HogCpu()
{
    while( 1 )
    {
     sqrt( rand() );
    }
}


/**/
/**/
void MakeEntry( char entry[WORDS_IN_ENTRY][MAX_STR_LEN], int nEntry )
{
    sHandlerParams * pParams  (sHandlerParams*)malloc( sizeof(sHandlerParams) );

    int n  0;
    strncpy( pParams->mInputFileName[0],   entry[n++], MAX_STR_LEN );
    strncpy( pParams->mInputFileName[1],   entry[n++], MAX_STR_LEN );
    strncpy( pParams->mInputFileName[2],   entry[n++], MAX_STR_LEN );
    strncpy( pParams->mOutputFileName,     entry[n++], MAX_STR_LEN );
    strncpy( pParams->mRefFileName,        entry[n++], MAX_STR_LEN );

    pParams->mWidth           atoi( entry[n++] );
    pParams->mHeight          atoi( entry[n++] );
    pParams->mComponents      atoi( entry[n++] );
    pParams->mQualityLevel    atoi( entry[n++] );
    pParams->mResolution      atoi( entry[n++] );
    pParams->mXSampleDist[0]  atoi( entry[n++] );
    pParams->mYSampleDist[0]  atoi( entry[n++] );
    pParams->mXSampleDist[1]  atoi( entry[n++] );
    pParams->mYSampleDist[1]  atoi( entry[n++] );
    pParams->mXSampleDist[2]  atoi( entry[n++] );
    pParams->mYSampleDist[2]  atoi( entry[n++] );
    pParams->mIsLossless      atoi( entry[n++] );
    pParams->mProgOrder       atoi( entry[n++] );
    pParams->mTiledData       atoi( entry[n++] );

    pParams->mEntryIndex      nEntry;

    #if 0
    printf( "pParams->mInputFileName[0]  %s\n", pParams->mInputFileName[0] );
    printf( "pParams->mInputFileName[1]  %s\n", pParams->mInputFileName[1] );
    printf( "pParams->mInputFileName[2]  %s\n", pParams->mInputFileName[2] );
    printf( "pParams->mOutputFileName    %s\n", pParams->mOutputFileName );
    printf( "pParams->mRefFileName       %s\n", pParams->mRefFileName );
    printf( "pParams->mWidth             %d\n", pParams->mWidth );
    printf( "pParams->mHeight            %d\n", pParams->mHeight );
    printf( "pParams->mComponents        %d\n", pParams->mComponents );
    printf( "pParams->mQualityLevel      %d\n", pParams->mQualityLevel );
    printf( "pParams->mResolution        %d\n", pParams->mResolution );
    printf( "pParams->mXSampleDist[0]    %d\n", pParams->mXSampleDist[0] );
    printf( "pParams->mYSampleDist[0]    %d\n", pParams->mYSampleDist[0] );
    printf( "pParams->mXSampleDist[1]    %d\n", pParams->mXSampleDist[1] );
    printf( "pParams->mYSampleDist[1]    %d\n", pParams->mYSampleDist[1] );
    printf( "pParams->mXSampleDist[2]    %d\n", pParams->mXSampleDist[2] );
    printf( "pParams->mYSampleDist[2]    %d\n", pParams->mYSampleDist[2] );
    printf( "pParams->mIsLossless        %d\n", pParams->mIsLossless );
    printf( "pParams->mProgOrder         %d\n", pParams->mProgOrder );
    printf( "pParams->mTiledData         %d\n", pParams->mTiledData );
    #endif

    /* Adjust/check parameters here... */

    if( !Util_StrICmp( pParams->mOutputFileName, NA ) )
    {
        tst_brkm( TBROK, (void(*)())VT_jpeg2k_encoder_cleanup,
                  "Wrong output file name %s. The output file name must not be %s."
                  "Please check %s, line/entry #%lu",
                  pParams->mOutputFileName, NA, gTestappConfig.mConfigFilename, nEntry + 1 );
    }

    int verbose  gTestappConfig.mVerbose;
    gTestappConfig.mVerbose  0;
    if( !DoFilesExist( pParams->mRefFileName, pParams->mRefFileName ) )
        strncpy( pParams->mRefFileName, NA, MAX_STR_LEN );
    gTestappConfig.mVerbose  verbose;

    LList_PushBack( gpParamsList, pParams );
}


/**/
/**/
int NominalFunctionalityTest()
{
    sLinkedList * pNode;
    int i;
    int rv  TPASS;
    sJpeg2KEncoderHandler * pHandler  gJ2KEncHandlers;

    /* For the each entry */
    for( pNode  gpParamsList, i  0; pNode; pNode  pNode->mpNext, ++i )
    {
        /* Reset the handler. */
        ResetHandler( pHandler );

        /* Get content. */
        pHandler->mpParams  (sHandlerParams*)pNode->mpContent;

        if( gTestappConfig.mVerbose )
        {
            if( pHandler->mpParams->mComponents  1 )
            {
                TST_RESM( TINFO, "Thread[%lu] Input[0]: %s",
                          pHandler->mIndex,
                          pHandler->mpParams->mInputFileName[0] );
            }
            else
            {
                TST_RESM( TINFO, "Thread[%lu] Input[0]: %s, Input[1]: %s, Input[2]: %s",
                          pHandler->mIndex,
                          pHandler->mpParams->mInputFileName[0],
                          pHandler->mpParams->mInputFileName[1],
                          pHandler->mpParams->mInputFileName[2] );
            }
        }

        /* Run the Encoder. */
        rv + RunEncoder( pHandler );
        CleanupHandler( pHandler );
    }

    return rv;
}


/**/
/**/
int ReLocatabilityTest()
{
    sLinkedList * pNode;
    int i, j;
    int rv  TPASS;
    sJpeg2KEncoderHandler * pHandler  gJ2KEncHandlers;

    /* For the each entry */
    for( pNode  gpParamsList, i  0; pNode; pNode  pNode->mpNext, ++i )
    {
        for( j  0; j < gTestappConfig.mNumIter; ++j )
        {
            /* Reset the handler. */
            ResetHandler( pHandler );

            /* Get content. */
            pHandler->mpParams  (sHandlerParams*)pNode->mpContent;

            if( gTestappConfig.mVerbose && j  0 )
            {
                if( gTestappConfig.mVerbose )
                {
                    if( pHandler->mpParams->mComponents  1 )
                    {
                        TST_RESM( TINFO, "Thread[%lu] Input[0]: %s",
                            pHandler->mIndex,
                            pHandler->mpParams->mInputFileName[0] );
                    }
                    else
                    {
                        TST_RESM( TINFO, "Thread[%lu] Input[0]: %s, Input[1]: %s, Input[2]: %s",
                            pHandler->mIndex,
                            pHandler->mpParams->mInputFileName[0],
                            pHandler->mpParams->mInputFileName[1],
                            pHandler->mpParams->mInputFileName[2] );
                    }
                }
            }

            /* Run the Encoder. */
            rv + RunEncoder( pHandler );

            if( gTestappConfig.mVerbose )
            {
                TST_RESM( TINFO, "Thread[%lu] Data memory was relocated", pHandler->mIndex );
            }
            CleanupHandler( pHandler );
        }
    }
    return rv;
}


/**/
/**/
int ReEntranceTest()
{
    int ReEntranceTestCore( sLinkedList * pHead );
    sLinkedList * pHead  gpParamsList;
    int rv  TPASS;
    int i;

    while( pHead )
    {
        gThreadWithFocus  -1;
        rv + ReEntranceTestCore( pHead );
        for( i  0; i < gNumThreads && pHead; ++i )
        {
            ResetHandler( &gJ2KEncHandlers[i] );
            gJ2KEncHandlers[i].mIndex  i;
            pHead  pHead->mpNext;
        }
    }

    return rv;
}


/**/
/**/
int ReEntranceTestCore( sLinkedList * pHead )
{
    assert( pHead );

    sLinkedList * pNode;
    int i;
    int rv  TPASS;
    sJpeg2KEncoderHandler * pHandler;

    /* Run all bitstreams in separate threads. */
    for( pNode  pHead, i  0; pNode && i < gNumThreads; pNode  pNode->mpNext, ++i )
    {
        pHandler  gJ2KEncHandlers + i;
        ResetHandler( pHandler );
        pHandler->mIndex  i;

        /* Get content. */
        pHandler->mpParams  (sHandlerParams*)pNode->mpContent;

        if( gTestappConfig.mVerbose )
        {
            if( pHandler->mpParams->mComponents  1 )
            {
                TST_RESM( TINFO, "Thread[%lu] Input[0]: %s",
                    pHandler->mIndex,
                    pHandler->mpParams->mInputFileName[0] );
            }
            else
            {
                TST_RESM( TINFO, "Thread[%lu] Input[0]: %s, Input[1]: %s, Input[2]: %s",
                    pHandler->mIndex,
                    pHandler->mpParams->mInputFileName[0],
                    pHandler->mpParams->mInputFileName[1],
                    pHandler->mpParams->mInputFileName[2] );
            }
        }

        if( pthread_create( &pHandler->mThreadID, NULL, (void*)&RunEncoder, pHandler ) )
        {
            TST_RESM( TWARN, "%s : error creating thread %d", __FUNCTION__, i );
            return TFAIL;
        }
    }

    /* Wait for the each thread. */
    for( i  0; i < gNumThreads; ++i )
    {
        pHandler  gJ2KEncHandlers + i;
        pthread_join( pHandler->mThreadID, NULL );
    }
    for( i  0; i < gNumThreads; ++i )
    {
     pHandler  gJ2KEncHandlers + i;
        rv + pHandler->mLtpRetval;
    }

    return rv;
}


/**/
/**/
int PreEmptionTest()
{
    return ReEntranceTest();
}


/**/
/**/
int EnduranceTest()
{
    int i;
    int rv  TPASS;

    for( i  0; i < gTestappConfig.mNumIter; ++i )
    {
        if( gTestappConfig.mVerbose )
            TST_RESM( TINFO, "The %d iteration has been started", i+1 );
        rv + NominalFunctionalityTest();
        if( gTestappConfig.mVerbose )
            TST_RESM( TINFO, "The %d iteration has been completed", i+1 );
    }

    return rv;
}


/**/
/**/
int LoadTest()
{
    int rv  TFAIL;
    pid_t pid;

    switch( pid  fork() )
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
         rv  NominalFunctionalityTest();
        /* kill child process once decode/encode loop has ended */
         if( kill( pid, SIGKILL ) ! 0 )
         {
       TST_RESM( TWARN, "%s : Kill(SIGKILL) error", __FUNCTION__ );
         return rv;
         }
    }

    return rv;
}


/**/
/**/
int Util_StrICmp( const char * s1, const char *s2 )
{
    for (;;)
    {
        int c1  *s1;
        int c2  *s2;
        if( isupper(c1) )
            c1  tolower(c1);
        if( isupper(c2) )
            c2  tolower(c2);
        if( c1 ! c2 )
            return c1 - c2;
        if( c1  '\0' )
            break;
        ++s1;
        ++s2;
    }
    return 0;
}


/**/
/**/
void Util_ConvertYUVToRGB( const uint8 * pCompData[J2K_MAX_NO_OF_COMPONENTS], uint8 * pRGBBufPtr, uint32 bufSize, uint8 ctType, int ncomp )
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
    uint8 *rgbPtr  pRGBBufPtr;

    yptr   pCompData[0];
    cbptr  pCompData[1];
    crptr  pCompData[2];

    if( ctType && ncomp  3 ) //5x3
    {
        for(i  0; i < bufSize; ++i )
        {
            r  yptr[i] - 128;
            g  cbptr[i] - 128;
            b  crptr[i] - 128;

            G  r - ((b + g) >> 2);
            R  b + G;
            B  g + G;

            *rgbPtr++    (uint8)(LIMIT(R+128));
            *rgbPtr++    (uint8)(LIMIT(G+128));
            *rgbPtr++    (uint8)(LIMIT(B+128));
        }
    }
    else if( ncomp  3 ) //9x7
    {
        for( i  0;i < bufSize; ++i )
        {
            y     (int) yptr[i];
            cb    (int) cbptr[i];
            cr    (int) crptr[i];

            R  (double)(y + FIX_MUL((cr-128), 1.401977));
            G  (double)(y - FIX_MUL((cb-128), 0.344116) - FIX_MUL((cr-128), 0.714111));
            B  (double)(y + FIX_MUL((cb-128), 1.771972));

            *rgbPtr++   (uint8)LIMIT(R);
            *rgbPtr++   (uint8)LIMIT(G);
            *rgbPtr++   (uint8)LIMIT(B);
        }
    }
    else
    {
        for(i  0; i < bufSize; ++i )
        {
            r  yptr[i];

            G  r;
            R  r;
            B  r;

            *rgbPtr++    (uint8)(LIMIT(R));
            *rgbPtr++    (uint8)(LIMIT(G));
            *rgbPtr++    (uint8)(LIMIT(B));
        }
    }
}


/**/
/**/
void Util_ConvertRGBtoYUV(uint8 *compData[J2K_MAX_NO_OF_COMPONENTS],uint32 imgSize, uint32 ctType)
{
    #define LIMIT(X) (((X)>255)?(255):(((X)<0)? (0):(X)))
    #define FIX_MUL(a,b) ((a)*(b))

 uint8 *c0, *c1, *c2;

 c0  compData[0];
 c1  compData[1];
 c2  compData[2];

 if(ctType) //5x3
 {
  uint32 i;
  for (i  0; i < imgSize; i++)
  {
   int r, g, b, y, u, v;
   r  c0[i]-128;
   g  c1[i]-128;
   b  c2[i]-128;
   y  (r + (g << 1) + b) >> 2;
   u  b - g;
   v  r - g;
   c0[i]  LIMIT(y+128);
   c1[i]  LIMIT(u+128);
   c2[i]  LIMIT(v+128);
  }
 }
 else //9x7
 {
  uint32 i;
  for (i  0; i < imgSize; i++)
  {
   int r, g, b, y, u, v;
   r  c0[i]-128;
   g  c1[i]-128;
   b  c2[i]-128;
   y  ((int)(FIX_MUL(r, 2449) + FIX_MUL(g, 4809) + FIX_MUL(b, 934))+4096)>>13; //corrected
   u  ((int)(-FIX_MUL(r, 1382) - FIX_MUL(g, 2714) + FIX_MUL(b, 4096))+4096)>>13;
   v  ((int)(FIX_MUL(r, 4096) - FIX_MUL(g, 3430) - FIX_MUL(b, 666))+4096)>>13;
   c0[i]  LIMIT(y+128);
   c1[i]  LIMIT(u+128);
   c2[i]  LIMIT(v+128);
  }
 }

}


/**/
/**/
void Util_CalculatePSNR( sJpeg2KEncoderHandler * pHandler,
                         uint8 * pDecodedData, uint32 compIdx )
{
    assert( pHandler );
    assert( pHandler->mpParams );

 uint32 j;
 int32  compErr  0;
 double compErrSq 0.0;
 double avgCompErrSq  0;

    for( j  0; j < pHandler->mInpBufferSz[compIdx]; ++j )
 {
  compErr  (pHandler->mpInpBuffer[compIdx][j] - pDecodedData[j]);
  compErrSq + (double)(compErr * compErr);
 }

 avgCompErrSq  compErrSq / pHandler->mInpBufferSz[compIdx];
 if( avgCompErrSq > 0.0 )
  pHandler->mCompPsnr[compIdx]  10 * log10( 65025.0 / avgCompErrSq );
}


/**/
/**/
void Util_GetSubSampledImageBuffers( uint8 * pDst, uint8 * pSrc, uint32 compIndex, J2KImageInfoStruct_t * pImgInfo )
{
    assert( pDst && pSrc && pImgInfo && compIndex < J2K_MAX_NO_OF_COMPONENTS );

    uint32  i, j;
    uint32  xDist, yDist, dW;
    uint8 * dPtr, * sPtr ;
    uint32  k;

    dPtr  pDst;
    sPtr  pSrc;
    xDist  pImgInfo->mXRsiz[compIndex];
    yDist  pImgInfo->mYRsiz[compIndex];
    dW   (pImgInfo->mImWidth+xDist-1)/xDist;

    for( i  0; i < (pImgInfo->mImHeight+yDist-1)/yDist; ++i )
    {
        for( j  0, k  0; j < pImgInfo->mImWidth; j + xDist, ++k )
        {
            *(dPtr+k)  *(sPtr+j);
        }
        dPtr + dW;
        sPtr + pImgInfo->mImWidth * yDist;
    }
}


/**/
/**/
uint8 * Util_ReadFile( const char * filename, size_t * pSz )
{
    FILE * pInStream  fopen( filename, "rb" );
    if( pInStream )
    {
        fseek( pInStream, 0, SEEK_END );
        size_t sz  ftell( pInStream );
        fseek( pInStream, 0, SEEK_SET );
        uint8 * pData  (uint8*)Util_Malloc( sizeof(char) * sz );
        fread( pData, 1, sz, pInStream );
        fclose( pInStream );
        if( pSz )
            *pSz  sz;
        return pData;
    }
    return NULL;
}


/**/
/**/
int Util_DecodeJ2KFile( sJpeg2KEncoderHandler * pHandler )
{
    assert( pHandler );
    assert( NULL  pHandler->mpOutputStream );

    #define CALL_JPEG2K_DECODER(Jpeg2KDecRoutine, name)   \
    rv  Jpeg2KDecRoutine; \
    if( (rv ! J2K_SUCCESSFUL) )\
    {\
        TST_RESM( TWARN, "%s : %s fails #%d", __FUNCTION__, name, rv);\
  break;\
 }

    int rv;
    char outFileName[MAX_STR_LEN];
    strncpy( outFileName, pHandler->mpParams->mOutputFileName, MAX_STR_LEN );
    outFileName[strlen(outFileName)-4]  0;

    const sHandlerParams *  pParams  pHandler->mpParams;
    J2KDecoderObject_t      decObject;
 J2KDecodeParamStruct_t  decParams;
 J2KStreamInfoStruct_t   decStreamInfo;
    J2KInitStruct_t         decInit;
    uint8                 * pOutBuffer[J2K_MAX_NO_OF_COMPONENTS]  {0};
    size_t                  outBufferSz[J2K_MAX_NO_OF_COMPONENTS];
    uint8                 * pInpBuffer  NULL;
    size_t                  inpBufferSz;

    do
    {
        /********************/
        /* Read j2k stream. */
        /********************/
        pInpBuffer  Util_ReadFile( pParams->mOutputFileName, &inpBufferSz );
        if( !pInpBuffer || inpBufferSz ! pHandler->mActualOutputSz )
        {
            TST_RESM( TWARN, "%s : Can't read \'%s\'", __FUNCTION__, pParams->mOutputFileName );
            SAFE_DELETE( pInpBuffer );
            break;
        }

        /*****************/
        /* Init decoder. */
        /*****************/
        decInit.CBGetNewData  (int32 (*)(uint8**,uint32*))1;
        CALL_JPEG2K_DECODER(
            InitJ2KDecoder( &decObject, decInit ),
            "InitJ2KDecoder" );

        /***************************/
        /* Obtain the stream info. */
        /***************************/
        CALL_JPEG2K_DECODER(
            GetJ2KStreamInfo( &decObject,
            &decStreamInfo,
            pInpBuffer,
            inpBufferSz ),
            "GetJ2KStreamInfo" );

        if( pParams->mComponents ! decStreamInfo.mComponent )
        {
            TST_RESM( TWARN, "%s : Num components in the encoded file \'%s\' is %d (must be %lu)",
                      __FUNCTION__, pParams->mOutputFileName, decStreamInfo.mComponent, pParams->mComponents );
            break;
        }

        /***************************/
        /* Set decoder parameters. */
        /***************************/
        decParams.mResolution  decStreamInfo.mResolution;
        decParams.mLayers      decStreamInfo.mLayers;

        /* Allocate decoder's memory. */
        decParams.mMemSize  decStreamInfo.mBytesOfMemory;
        decParams.mpMemptr  Util_Malloc( decParams.mMemSize );
        if( !decParams.mpMemptr )
        {
            TST_RESM( TWARN, "%s : Can't allocate %d bytes memory for decoder", __FUNCTION__, decParams.mMemSize );
            break;
        }

        CALL_JPEG2K_DECODER(
            SetJ2KDecodeParams( &decObject, decParams ),
            "SetJ2KDecodeParams" );

        if( gTestappConfig.mVerbose )
        {
            TST_RESM( TINFO, "Thread[%lu] Decoding OK", pHandler->mIndex );
        }

        /*******************************************/
        /* Allocate memory for the output buffers. */
        /*******************************************/
        int i;
        for( i  0; i < decStreamInfo.mComponent; ++i )
        {
            outBufferSz[i]  decStreamInfo.mResolutionWidth[i][decParams.mResolution-1] *
                             decStreamInfo.mResolutionHeight[i][decParams.mResolution-1];
            pOutBuffer[i]  (uint8*)Util_Malloc( outBufferSz[i] );
            if( !pOutBuffer )
            {
                TST_RESM( TWARN, "%s : Can't allocate %d bytes memory for output buffer", __FUNCTION__, outBufferSz[i] );
                break;
            }
        }

        /******************/
        /* Decode stream. */
        /******************/
        CALL_JPEG2K_DECODER(
            DecodeJ2Kstream( &decObject,
                              pOutBuffer,
                              pInpBuffer,
                              inpBufferSz ),
            "DecodeJ2Kstream" );

        /*************/
        /* Dump YUV. */
        /*************/
        #if 1
        for( i  0; i < decStreamInfo.mComponent; ++i )
        {
            FILE * pOutStream;
            sprintf( pHandler->mDecodedFileName[i], "%s_dec_%c.raw", outFileName, (i0?'y':(i1)?'u':'v') );
            pOutStream  fopen( pHandler->mDecodedFileName[i], "wb" );
            if( !pOutStream )
            {
                TST_RESM( TWARN, "%s : Can't create %s", __FUNCTION__, pHandler->mDecodedFileName[i] );
                continue;
            }

            fwrite( pOutBuffer[i], sizeof(char), outBufferSz[i], pOutStream );
            fclose( pOutStream );
        }
        #endif

        /*******************/
        /* Calculate PSNR. */
        /*******************/
        pHandler->mIsPsnrCalculated[0]  FALSE;
        pHandler->mIsPsnrCalculated[1]  FALSE;
        pHandler->mIsPsnrCalculated[2]  FALSE;
        for( i  0; i < pParams->mComponents; ++i )
        {
            if( pHandler->mpInpBuffer[i] && pOutBuffer[i] )
            {
                Util_CalculatePSNR( pHandler, pOutBuffer[i], i );
                pHandler->mIsPsnrCalculated[i]  TRUE;
            }
        }

        /*******************************/
        /* Cleanup and return success. */
        /*******************************/
        SAFE_DELETE( decParams.mpMemptr );
        SAFE_DELETE( pInpBuffer );
        for( i  0; i < J2K_MAX_NO_OF_COMPONENTS; ++i )
        {
            SAFE_DELETE( pOutBuffer[i] );
        }
        return TPASS;

    } while( FALSE );

    SAFE_DELETE( decParams.mpMemptr );
    SAFE_DELETE( pInpBuffer );
    int i;
    for( i  0; i < J2K_MAX_NO_OF_COMPONENTS; ++i )
    {
        SAFE_DELETE( pOutBuffer[i] );
    }

    return TFAIL;
}


/**/
/**/
void * Util_Malloc( size_t sz )
{
#ifdef DEBUG_TEST
    return NOMINAL_FUNCTIONALITY  gTestappConfig.mTestCase ? MemStat_Alloc( sz ) : malloc( sz );
#else
    return malloc( sz );
#endif
}


/**/
/**/
void Util_Free( void * pPtr )
{
    assert( pPtr );

#ifdef DEBUG_TEST
    return NOMINAL_FUNCTIONALITY  gTestappConfig.mTestCase ? MemStat_Free( pPtr ) : free( pPtr );
#else
    return free( pPtr );
#endif
}

#ifdef __cplusplus
}
#endif

