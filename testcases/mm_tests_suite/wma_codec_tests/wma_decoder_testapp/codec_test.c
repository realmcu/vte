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
D.Simakov/smkd001c    01/06/2005   TLSbo48591	Initial version			
S. V-Guilhou/svan01c  13/06/2005   TLSbo50938   P4 Codec Campaign / add traces
D.Simakov/smkd001c    22/07/2005   TLSbo52361	Relocatability test case was added			
D.Simakov             19/05/2006   TLSbo66279   Phase2
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


/*==================================================================================================
                                        LOCAL MACROS
==================================================================================================*/

#define MAX_FILE_SIZE 150000000
#define     BITS_MAX        32 
#define DPRINTF(fmt,...) do{printf("%s:%d %s() ", __FILE__, __LINE__, __FUNCTION__); printf((fmt), ##__VA_ARGS__); fflush(stdout);}while(0)

/**********************************************************************
 * Macro name:  CALL_WMA_DECODER()
 * Description: Macro checks for any error in function execution
                based on the return value. In case of error, the function exits.
**********************************************************************/
#define CALL_WMA_DECODER(WmaDecRoutine, name)   \
do \
{ \
    pHandler->mLastCodecError = WmaDecRoutine; \
    if( pHandler->mLastCodecError != cWMA_NoErr &&  pHandler->mLastCodecError != cWMA_NoMoreFrames ) \
    {\
        TST_RESM( TWARN, "%s : %s fails #%d [File: %s, line: %d]", __FUNCTION__, name, pHandler->mLastCodecError, __FILE__, __LINE__-5);\
        return TFAIL;\
    }\
} while(0)   

#define SAFE_DELETE(p) {if(p){Util_Free(p);p=0;}}            


/*==================================================================================================
                                   VARIABLES
==================================================================================================*/

static const void    * gpAppTables[50]; 
static pthread_once_t  gIsTablesInitialized = PTHREAD_ONCE_INIT;


/*==================================================================================================
                                   FUNCTION PROTOTYPES
==================================================================================================*/

/* Callbacks. */
WMAD_INT32 WMAD_GetNewData  ( WMAD_UINT8 ** ppBuffer, 
                              WMAD_UINT32 * pLen, 
                              WMADDecoderConfig * pDecConfig, 
                              WMAD_INT8 * pSeekNeeded );                             

void       WMAD_InitTables ( void );

/* Extra test cases. */
int               RobustnessTestCase( void );  

/* Other. */
void DoDataOutput(sCodecHandler * pHandler);
int Diff32(FILE * pStream1, FILE * pStream2, int nBits);


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
WMAD_INT32 WMAD_GetNewData(  WMAD_UINT8 ** ppBuffer, 
                             WMAD_UINT32 * pLen, 
                             WMADDecoderConfig * pDecConfig, 
                             WMAD_INT8 * pSeekNeeded ) 
{
        /* Find the appropriate handler */
        sCodecHandler * pHandler = NULL;
        int i;
        for( i = 0; i < MAX_THREADS; ++i )
        {
                if( &gCodecHandler[i].mDecConfig == pDecConfig )
                        pHandler = gCodecHandler + i;
        }
        assert( pHandler );
        assert( pHandler->mpInpStream );
        
        WMAD_UINT32 bytesRead = 2048;
        int         retVal = 0;
        
        /* Reset only if previous seeking is done. */
        if( 1 != *pSeekNeeded )
        {
                *pSeekNeeded = 0;
        }
        /* If seek is needed set the flag to one, otherwise reset to zero. */
        *pSeekNeeded = 1;
        
        if( bytesRead > (pHandler->mInputFileSize - pHandler->mInputBufferIdx) )
                bytesRead = pHandler->mInputFileSize - pHandler->mInputBufferIdx;
        
        *ppBuffer = pHandler->mpInputBufferRef; 
        *pLen = bytesRead;
        
        pHandler->mpInputBufferRef += bytesRead;
        pHandler->mInputBufferIdx += bytesRead;
        if( bytesRead <= 0 )
                retVal = 1;
        
        return retVal;    
}


/*================================================================================================*/
/*================================================================================================*/
void WMAD_InitTables(void)
{
        gpAppTables[0] = (const unsigned short *)g_rgiHuffDecTblMsk;
        gpAppTables[1] = (const unsigned short *)g_rgiHuffDecTblNoisePower;
        gpAppTables[2] = (const unsigned short*)g_rgiHuffDecTbl16smOb;
        gpAppTables[3] = (const unsigned short*)g_rgiHuffDecTbl44smOb;
        gpAppTables[4] = (const unsigned short*)g_rgiHuffDecTbl16ssOb;
        gpAppTables[5] = (const unsigned short*)g_rgiHuffDecTbl44ssOb;
        gpAppTables[6] = (const unsigned short*)g_rgiHuffDecTbl44smQb;
        gpAppTables[7] = (const unsigned short*)g_rgiHuffDecTbl44ssQb;
        gpAppTables[8] = (const unsigned short*)gRun16ssOb;
        gpAppTables[9] = (const unsigned short*)gRun16smOb;
        gpAppTables[10] = (const unsigned short*)gLevel16ssOb;
        gpAppTables[11] = (const unsigned short*)gLevel16smOb;
        gpAppTables[12] = (const unsigned short*)gRun44ssQb;
        gpAppTables[13] = (const unsigned short*)gLevel44ssQb;
        gpAppTables[14] = (const unsigned short*)gRun44smQb;
        gpAppTables[15] = (const unsigned short*)gLevel44smQb;
        gpAppTables[16] = (const unsigned short*)gRun44ssOb;
        gpAppTables[17] = (const unsigned short*)gLevel44ssOb;
        gpAppTables[18] = (const unsigned short*)gRun44smOb;
        gpAppTables[19] = (const unsigned short*)gLevel44smOb;
        gpAppTables[20] = (const unsigned short*)g_rgiBarkFreq;
        gpAppTables[21] = (const unsigned char*)gLZLTable;
        gpAppTables[22] = (const unsigned int*)g_InvQuadRootFraction;
        gpAppTables[23] = (const unsigned int*)g_InvQuadRootExponent;
        gpAppTables[24] = (const SinCosTable (*)[])rgSinCosTables;
        gpAppTables[25] = (const unsigned int *)g_InverseFraction;
        gpAppTables[26] = (const signed long *)bp1cosPIbynp;
        gpAppTables[27] = (const signed long *)bp1sinPIbynp;
        gpAppTables[28] = (unsigned char *)g_pmid;
        gpAppTables[29] = (const unsigned int *)getMask;
        gpAppTables[30] = (const signed long int (*)[])g_rgiLsfReconLevel;
        //gpAppTables[31] = (const signed long int (*)[])g_rgiLsfReconLevel_LP;
        gpAppTables[32] = (const signed long int *)rgDBPower10; 
        gpAppTables[33] = (const unsigned int *)g_SqrtFraction;
        gpAppTables[34] = (const signed long int *)rgiMaskMinusPower10;
        gpAppTables[35] = (const signed long int *)rgiMaskPlusPower10;
        gpAppTables[36] = (const char *)fOKOptions;
        gpAppTables[37] = (int **)Tables_32;
        gpAppTables[38] = (int **)Tables_64;
        gpAppTables[39] = (int **)Tables_128;
        gpAppTables[40] = (int **)Tables_256;
        gpAppTables[41] = (int **)Tables_512;
        gpAppTables[42] = (int **)Tables_1024;
}


/*================================================================================================*/
/*================================================================================================*/
int TestEngine( sCodecHandler * pHandler ) 
{
        assert( pHandler );
        
        pthread_once( &gIsTablesInitialized, WMAD_InitTables ); 
        
        /* Short names. */
        WMADDecoderConfig * pDecConfig = &pHandler->mDecConfig;
        WMADDecoderParams * pDecParams = &pHandler->mDecParams;   
        sHandlerParams    * pParams = pHandler->mpParams;
        
        /* Fill up the Relocated data position. */
        pDecConfig->app_initialized_data_start = (void*)gpAppTables;
        
        /* Open all necessary files. */                
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
                pHandler->mpOutStream = fopen( pParams->mOutFileName, "wb" );
                if( !pHandler->mpOutStream )
                {
                        tst_brkm( TBROK, (void(*)())VT_codec_cleanup,
                                "%s : Can't create output file",                
                                __FUNCTION__ );  
                }
        }
        
        /* Assign the call back function to the function pointer in the 
        config structure.  */     
        pDecConfig->app_swap_buf = WMAD_GetNewData;        
        
        /* Query for memory */
        CALL_WMA_DECODER( 
                eWMADQueryMem( pDecConfig ), 
                "eWMADQueryMem" );
        
        /* Allocate the required memory. */
        int i;
        WMADMemAllocInfo * pDecMemInfo = &pDecConfig->sWMADMemInfo;
        /* Number of memory chunk requests by the decoder */
        for( i = 0; i < pDecMemInfo->s32NumReqs; ++i )
        {
                WMADMemAllocInfoSub * pDecMemInfoSub = &pDecMemInfo->sMemInfoSub[i]; 
                if( WMAD_FAST_MEMORY == pDecMemInfoSub->s32WMADType )
                {
                        pDecMemInfoSub->app_base_ptr = Util_Malloc( pDecMemInfoSub->s32WMADSize );
                }
                else if( WMAD_SLOW_MEMORY == pDecMemInfoSub->s32WMADType )
                {
                        pDecMemInfoSub->app_base_ptr = Util_Malloc( pDecMemInfoSub->s32WMADSize );
                }
                else
                        assert( !"eWMADQueryMem has returned an unknown memory type" );
                
                if( !pDecMemInfoSub->app_base_ptr )
                {
                        tst_brkm( TBROK, (void(*)())VT_codec_cleanup,
                                "%s : Can't allocate memory",                
                                __FUNCTION__ );  
                }
        } 
        
        /* Find the size of file which is reqired for allocation of
        app_input_buffer. */
        fseek( pHandler->mpInpStream, 0, SEEK_END );
        pHandler->mInputFileSize = ftell( pHandler->mpInpStream );
        fseek( pHandler->mpInpStream, 0, SEEK_SET );
        if( pHandler->mInputFileSize > MAX_FILE_SIZE )
        {
                pHandler->mInputFileSize = 
                        ((MAX_FILE_SIZE % WMAD_INPUT_BUF_SIZE) + 1) * WMAD_INPUT_BUF_SIZE;
        }
        
        /* Allocate memory for the input buffer. */
        pHandler->mpInputBuffer = (unsigned char*)
                Util_Malloc( sizeof(unsigned char)*pHandler->mInputFileSize + 100 );
        if( !pHandler->mpInputBuffer )
        {
                tst_brkm( TBROK, (void(*)())VT_codec_cleanup,
                        "%s : Can't allocate memory",                
                        __FUNCTION__ );
        }
        pHandler->mpInputBufferRef = pHandler->mpInputBuffer;
        
        /* Copy the contents of the file into the input buffer. */
        int bytesRead, totalBytesRead = 0;    
        unsigned char * pInputBuffer = pHandler->mpInputBuffer;
        while( totalBytesRead < pHandler->mInputFileSize )
        {        
                bytesRead = fread( pInputBuffer,                            
                        1, WMAD_INPUT_BUF_SIZE,
                        pHandler->mpInpStream );
                pInputBuffer += bytesRead;
                totalBytesRead += bytesRead;
                
                /* This is the end of file. */
                if( bytesRead != WMAD_INPUT_BUF_SIZE )
                        break;
        }
        //pHandler->mInputFileSize = totalBytesRead;
        
        /* Call eInitWMADecoder to init the decoder. */
        CALL_WMA_DECODER( 
                eInitWMADecoder( pDecConfig, pDecParams, 
                pHandler->mpInputBufferRef, 
                pHandler->mInputFileSize ),
                "eInitWMADecoder" );
        
        /* Allocate memory for the output buffer. */
        pHandler->mOutputBufferSize = sizeof(short) * WMAD_FRAME_SIZE * 2;
        pHandler->mpOutputBuffer = (short*)
                Util_Malloc( pHandler->mOutputBufferSize );
        if( !pHandler->mpOutputBuffer )
        { 
                tst_brkm( TBROK, (void(*)())VT_codec_cleanup,
                        "%s : Can't allocate memory",                
                        __FUNCTION__ );
        }
        
        /* Print some information for the verbose mode. */
        if( gTestappConfig.mVerbose )
        {
                tst_resm( TINFO, "Thread[%lu][%lu] version = %lu, channels = %lu",                          
                        pHandler->mIndex,       
                        pParams->mNoEntry,      
                        pDecParams->us16Version,
                        pDecParams->us16Channels );
        }	                
    
#ifdef DEBUG_TEST
        /* Current memory used by the app. */
        if( NOMINAL_FUNCTIONALITY == gTestappConfig.mTestCase )
            MemStat_GetStat();
#endif
        /* Main decoding loop */
        int decodingResult;
        while( TRUE )
        {
                /* Decode the next frame. */
                CALL_WMA_DECODER(
                        eWMADecodeFrame( pDecConfig, pDecParams, 
                        pHandler->mpOutputBuffer, 
                        WMAD_FRAME_SIZE ),
                        "eWMADecodeFrame" );
                
                /* Keep result. */
                decodingResult = pHandler->mLastCodecError;
                
                ++pHandler->mFramesCount; 
                
                if( pDecParams->us16NumSamples > 0 )
                {
                        DoDataOutput( pHandler );
                }       
                
                if( cWMA_NoMoreFrames == decodingResult )
                        break;
        }
        
        TST_RESM( TINFO, "Thread[%lu][%lu] Decoding completed", pHandler->mIndex, pParams->mNoEntry );
        pParams->mIsReadyForBitMatching = TRUE; 
        
        /* Cleanip the handler */
        CleanupHandler( pHandler );
        	
#ifdef DEBUG_TEST
        /* Current memory used by the app. */
        if( NOMINAL_FUNCTIONALITY == gTestappConfig.mTestCase )
            MemStat_GetStat();
#endif	
        
        /* Return succees */
        return TPASS;
}


/*================================================================================================*/
/*================================================================================================*/
void CleanupHandler( sCodecHandler * pHandler )
{   
        /* Close all the open files */
        if( pHandler->mpInpStream )
        {
                fclose( pHandler->mpInpStream );
                pHandler->mpInpStream = NULL;
        }
        if( pHandler->mpOutStream )
        {       
                fclose( pHandler->mpOutStream );
                pHandler->mpOutStream = NULL;
        }
        
        /* Free the input buffer */
        SAFE_DELETE( pHandler->mpInputBuffer );
        SAFE_DELETE( pHandler->mpOutputBuffer );                  
        int i;
        WMADMemAllocInfo * pDecMemInfo = &pHandler->mDecConfig.sWMADMemInfo;
        /* Number of memory chunk requests by the decoder */
        for( i = 0; i < pDecMemInfo->s32NumReqs; ++i )
        {
                WMADMemAllocInfoSub * pDecMemInfoSub = &pDecMemInfo->sMemInfoSub[i]; 
                SAFE_DELETE( pDecMemInfoSub->app_base_ptr );
        } 
}


/*================================================================================================*/
/*================================================================================================*/
int DoBitmatch( sCodecHandler * pHandler )
{
        assert( pHandler );
        assert( pHandler->mpParams );                
        sHandlerParams * pParams = pHandler->mpParams;
        FILE * pStream1 = fopen( pParams->mOutFileName, "r" );
        FILE * pStream2 = fopen( pParams->mRefFileName, "r" );
        assert( pStream1 && pStream2 );
        int res = Diff32(pStream1, pStream2, 30);
        fclose( pStream1 );
        fclose( pStream2 );
        return res;
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
        pParams->mInterleaveOutputs = atoi( entry[0] );
        strncpy( pParams->mInpFileName, entry[1], MAX_STR_LEN );   
        strncpy( pParams->mOutFileName, entry[2], MAX_STR_LEN );
        strncpy( pParams->mRefFileName, entry[3], MAX_STR_LEN );
        LList_PushBack( gpParamsList, pParams );
}


/*================================================================================================*/
/*================================================================================================*/
void DoDataOutput(sCodecHandler * pHandler)
{
        assert( pHandler );
        assert( pHandler->mpOutStream );
        assert( pHandler->mpParams );
        
        unsigned int i;    
        WMADDecoderParams * pDecParams = &pHandler->mDecParams;
        FILE * outFile = pHandler->mpOutStream;
        
        if( pHandler->mpParams->mInterleaveOutputs )
        {    
                /* Left/Right Interleaved. */
                if( 2 == pDecParams->us16Channels )
                {
                        for( i = 0; i < pDecParams->us16NumSamples; ++i )
                        {
                                /* Left. */
                                fprintf( outFile, "%08lx\n", 
                                        (long)pHandler->mpOutputBuffer[i] );                
                                /* Right. */
                                fprintf( outFile, "%08lx\n", 
                                        (long)pHandler->mpOutputBuffer[i+WMAD_FRAME_SIZE] );
                        }
                }
                else if( 1 == pDecParams->us16Channels )
                {
                        for( i = 0; i < pDecParams->us16NumSamples; ++i )
                        {                
                                fprintf( outFile, "%08lx\n", 
                                        (long)pHandler->mpOutputBuffer[i] );
                        }
                }
        }
        else
        {    
                /* Left and Right seperate */        
                for( i = 0; i < pDecParams->us16NumSamples; ++i )
                {            
                        fprintf( outFile, "%08lx\n", 
                                (long)pHandler->mpOutputBuffer[i] );
                }
        }    
}


/*================================================================================================*/
/*================================================================================================*/
int Diff32(FILE * pStream1, FILE * pStream2, int nBits)
{
        if(nBits > BITS_MAX)
        {                
                nBits = BITS_MAX;
        }               

#define GETINP(v,f) {int _v; fscanf(f,"%lx", &(_v)); v=(double)_v;}        
        
        double diffMax = 0.0;
        while( !feof(pStream1) && !feof(pStream2) )
        {
                double fval1, fval2;        
                GETINP(fval1, pStream1);
                GETINP(fval2, pStream2);               
                double fdelta = fabs(fval1 - fval2);               
                diffMax = (diffMax < fdelta) ? fdelta : diffMax;
        }       
        
        int bitsAcc = BITS_MAX;        
        if( diffMax > 0 )
                bitsAcc = BITS_MAX - 1 - ((int) (log(diffMax) / log(2.0)));
        else
                bitsAcc = BITS_MAX;
    
        return bitsAcc >= nBits;
}
