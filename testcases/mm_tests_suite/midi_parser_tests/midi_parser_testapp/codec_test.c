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
D.Simakov             30/05/2006   TLSbo66276   Phase2
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
 * Macro name:  CALL_MIDI_PARSER()
 * Description: Macro checks for any error in function execution
                based on the return value. In case of error, the function exits.
**********************************************************************/
#define CALL_MIDI_PARSER(MidParRoutine, name)   \
do \
{ \
    pHandler->mLastCodecError = MidParRoutine; \
    if( pHandler->mLastCodecError >= MIDIPAR_WARNING_BASE ) \
    {\
        TST_RESM( TWARN, "%s : %s fails #%d [File: %s, line: %d]", __FUNCTION__, name, pHandler->mLastCodecError, __FILE__, __LINE__-5);\
        return TFAIL;\
    }\
} while(0)   

#define SAFE_DELETE(p) {if(p){Util_Free(p);p=0;}}            

#define MIDIPAR_INP_BUF_SIZE 1000

/*==================================================================================================
                                   VARIABLES
==================================================================================================*/


/*==================================================================================================
                                   FUNCTION PROTOTYPES
==================================================================================================*/

/* Callbacks. */
void             * MidiPar_AppMalloc ( MIDIPAR_U32 size );
void               MidiPar_AppFree   ( void * ptr );
MIDIPAR_RET_TYPE   MidiPar_GetData   ( MIDI_Input_Details * pInputDetails, 
                                       MIDIPAR_U32 offset );

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
void * MidiPar_AppMalloc( MIDIPAR_U32 size )
{
        return Util_Malloc( size );
}


/*================================================================================================*/
/*================================================================================================*/
void MidiPar_AppFree( void * ptr )
{
        SAFE_DELETE( ptr );
}


/*================================================================================================*/
/*================================================================================================*/
MIDIPAR_RET_TYPE MidiPar_GetData( MIDI_Input_Details * pInputDetails, 
                                 MIDIPAR_U32 offset )
{
        /* Find the appropriate handler */
        sCodecHandler * pHandler = NULL;
        int i;
        for( i = 0; i < MAX_THREADS; ++i )
        {
                if( &gCodecHandler[i].mMidiParInpDetails == pInputDetails )
                        pHandler = gCodecHandler + i;
        }
        assert( pHandler );
        assert( pHandler->mpInpStream );
        
        FILE * inpFile =  pHandler->mpInpStream;
        
        MIDIPAR_U32 cnt = 0;
        MIDIPAR_U8* pInputBuffer = NULL;
        MIDIPAR_S16 ch;
        
        /* Free the already stored input buffer */
        SAFE_DELETE( pInputDetails->midi_buf );	
        
        /* Allocate memory for the input buffer */
        pInputBuffer = (MIDIPAR_U8*)Util_Malloc( MIDIPAR_INP_BUF_SIZE );
        memset( pInputBuffer, 0, MIDIPAR_INP_BUF_SIZE );
        if( !pInputBuffer )
                return MIDIPAR_MEMALLOC_ERROR;   
        
        fseek( inpFile, offset, SEEK_SET );
        // Read the complete MIDI file into the buffer
        while( (ch = getc(inpFile)) != EOF && cnt < MIDIPAR_INP_BUF_SIZE )
        {
                pInputBuffer[cnt++] = (MIDIPAR_U8)ch;
        }
        pInputDetails->midi_size = cnt;
        if( 0 == cnt )
        {
                free( pInputBuffer );
                pInputDetails->midi_buf = NULL;
        }
        else
        {
                pInputDetails->midi_buf  = (MIDIPAR_U32*)pInputBuffer;
        }	
        
        return MIDIPAR_SUCCESS;
}


/*================================================================================================*/
/*================================================================================================*/
int TestEngine( sCodecHandler * pHandler ) 
{
        assert( pHandler );
        assert( pHandler->mpParams );
        
        /* Short names */
        MIDIPAR_Config      * pMidiParCfg        = &pHandler->mMidiParConfig;
        MIDIPAR_Info        * pMidiParInfo       = &pHandler->mMidiParInfo;
        MIDI_Input_Details  * pMidiParInpDetails = &pHandler->mMidiParInpDetails;
        MIDI_Output_Details * pMidiParOutdetails = &pHandler->mMidiParOutdetails;    
        sHandlerParams      * pParams            = pHandler->mpParams;
        
        int parseState = 0;
        
        /* Open all necessary files */        
        pHandler->mpInpStream = fopen( pParams->mInpFileName, "rb" );
        if( !pHandler->mpInpStream )
        {
                tst_resm( TWARN, "%s : Can't open %s", __FUNCTION__, pParams->mInpFileName );
                return TFAIL;
        }
        
        int hasOutput = Util_StrICmp( pParams->mOutFileName, NA );  
        if( hasOutput )
        {
                pHandler->mpOutStream = fopen( pParams->mOutFileName, "wb" );
                if( !pHandler->mpOutStream )
                {
                        tst_resm( TWARN, "%s : Can't create %s", __FUNCTION__, pParams->mOutFileName );
                        return TFAIL;
                }
        }
        
        /* Fill up the callbacks */
        pMidiParCfg->midiparapp_mem_alloc = MidiPar_AppMalloc;
        pMidiParCfg->midiparapp_mem_free  = MidiPar_AppFree;
        pMidiParCfg->midiparapp_get_data  = MidiPar_GetData;
        
        /* Assign the pMidiParOutdetails with the data */
        pMidiParOutdetails->midi_buf        = NULL;
        pMidiParOutdetails->midi_num_events = 0;
        pMidiParOutdetails->midi_start_time = 0;
        
        /* Assign the pMidiParInpDetails with the data */
        pMidiParInpDetails->midi_buf    = NULL;
        pMidiParInpDetails->midi_size   = 0;
        pMidiParInpDetails->midi_currentPos = 0;
        
        /* Initialize the parameters struct */
        CALL_MIDI_PARSER( 
                midipar_initialize_parser( pMidiParCfg, pMidiParInpDetails ),
                "midipar_initialize_parser" );
        
        /* Call get_info to get duration, num tracks etc for the MIDI File */
        CALL_MIDI_PARSER( 
                midipar_get_info( pMidiParCfg, pMidiParInfo ),
                "midipar_get_info" );
        
        /* Print the duration of the file */
        if( gTestappConfig.mVerbose )
        {
                tst_resm( TINFO, "Thread[%lu][%lu] duration = %d second, " 
                        "event size = %d, num tracks = %d, format = %d",
                        pHandler->mIndex,        
                        pParams->mNoEntry,
                        pMidiParInfo->midipar_file_duration,
                        pMidiParInfo->midipar_event_size,
                        pMidiParInfo->midipar_num_tracks,
                        pMidiParInfo->midipar_format );     
        }      
        
#ifdef DEBUG_TEST
        /* Current memory used by the app. */
        if( NOMINAL_FUNCTIONALITY == gTestappConfig.mTestCase )
            MemStat_GetStat();
#endif          
        
        /* Main parse loop */
        while( parseState != MIDIPAR_PARSING_COMPLETE )   
        {
                /* The frame duration supplied by the application. This can be varied 
                in order to Fast Forward to a particular time during playback. */
                pMidiParOutdetails->midi_event_duration = pHandler->mpParams->mFrameDuration;
                
                /* Parse one frame of midi events. A link list containing
                MIDI_Events arranged according to ascending time is created. */
                CALL_MIDI_PARSER( 
                        midipar_parse_frame( pMidiParCfg, pMidiParOutdetails ),
                        "midipar_parse_frame" );
                
                /* Keep it */
                parseState = pHandler->mLastCodecError;            
                
                /* TEST FF RW ??? */
                pMidiParOutdetails->midi_start_time += pMidiParOutdetails->midi_event_duration;
                
                /* Output data */
                if( pHandler->mpOutStream )
                        DoDataOutput( pHandler );                
        }

        TST_RESM( TINFO, "Thread[%lu][%lu] Parsing completed", pHandler->mIndex, pParams->mNoEntry );
        pParams->mIsReadyForBitMatching = TRUE;                       
        
        /* Cleanup the handler */
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
        /* Close stream */
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
        SAFE_DELETE( pHandler->mMidiParInpDetails.midi_buf );
}


/*================================================================================================*/
/*================================================================================================*/
int DoBitmatch( sCodecHandler * pHandler )
{
        assert( pHandler );
        assert( pHandler->mpParams );
        
        sHandlerParams * pParams = pHandler->mpParams;
        
        unsigned int  mielDiv = 0, timidityDiv = 0;
        unsigned char mielTemp = 0;
        unsigned char timidityTemp = 0;
        unsigned char filesMatch = 1;
        unsigned int  byteCount = 0;
        FILE * mielFp = NULL, * timidityFp = NULL;
        
        const char * mielFileName = pParams->mOutFileName;
        const char * timidityFileName = pParams->mRefFileName;
        
        /* Open the miel parser output */
        mielFp = fopen( mielFileName, "rb" );
        assert( mielFp && "can't open a file" );
        
        /* Open timidity parser output */
        timidityFp = fopen( timidityFileName, "rb" );
        assert( timidityFp && "can't open a file" );
        
        while( ( !feof(mielFp) ) && ( !feof(timidityFp) ) )
        {
                mielDiv = 0;
                
                /* Read the miel parser out file */
                fread( &mielTemp, 1, 1, mielFp );
                mielDiv |= mielTemp << 24;
                
                fread( &mielTemp, 1, 1, mielFp );
                mielDiv |= mielTemp << 16;
                
                fread( &mielTemp, 1, 1, mielFp );
                mielDiv |= mielTemp << 8;
                
                fread( &mielTemp, 1, 1, mielFp );
                mielDiv |= mielTemp;
                
                timidityDiv = 0;
                
                /* Read the timidity parser out file */
                fread( &timidityTemp, 1, 1, timidityFp );
                timidityDiv |= timidityTemp << 24;
                
                fread( &timidityTemp, 1, 1, timidityFp );
                timidityDiv |= timidityTemp << 16;
                
                fread( &timidityTemp, 1, 1, timidityFp );
                timidityDiv |= timidityTemp << 8;
                
                fread( &timidityTemp, 1, 1, timidityFp );
                timidityDiv |= timidityTemp;
                
                /* Check whether it is a non-midi event */
                fread( &mielTemp, 1, 1, mielFp );
                fread( &timidityTemp, 1, 1, timidityFp );
                
                if( (0xff != mielTemp) && (0xf0 != mielTemp) && (0xf7 != mielTemp) )
                {
                        /* This is a midi event */
                        if( mielTemp != timidityTemp )
                        {
                                filesMatch = 0;
                                break;
                        }
                        
                        /* Compare the division */
                        if( mielDiv != timidityDiv )
                        {
                                filesMatch = 0;
                                break;
                        }
                        
                        /* Read channel number */
                        fread( &mielTemp, 1, 1, mielFp );
                        fread( &timidityTemp, 1, 1, timidityFp );
                        if( mielTemp != timidityTemp )
                        {
                                filesMatch = 0;
                                break;
                        }
                        
                        /* Read first byte number */
                        fread( &mielTemp, 1, 1, mielFp );
                        fread( &timidityTemp, 1, 1, timidityFp );
                        if( mielTemp != timidityTemp )
                        {
                                filesMatch = 0;
                                break;
                        }
                        
                        /* Read second byte number */
                        fread( &mielTemp, 1, 1, mielFp );
                        fread( &timidityTemp, 1, 1, timidityFp );
                        if( mielTemp != timidityTemp )
                        {
                                filesMatch = 0;
                                break;
                        }
                }
                else
                {
                        /* Skip 3 bytes */
                        fread( &mielTemp, 1, 1, mielFp );	
                        fread( &timidityTemp, 1, 1, timidityFp );
                        fread( &mielTemp, 1, 1, mielFp );
                        fread( &timidityTemp, 1, 1, timidityFp );
                        fread( &mielTemp, 1, 1, mielFp );
                        fread( &timidityTemp, 1, 1, timidityFp );
                }
                byteCount += 8;
        }
        
        fclose( mielFp );
        fclose( timidityFp );
        
        /*
        if( filesMatch == 0 )
        {
        printf ("\n No match between %s and %s after %d bytes\n",argv[1], argv[2], byteCount);
    }*/
        
        return filesMatch;
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
        pParams->mFrameDuration = atoi( entry[n++] );
        strncpy( pParams->mInpFileName, entry[n++], MAX_STR_LEN );   
        strncpy( pParams->mOutFileName, entry[n++], MAX_STR_LEN );
        strncpy( pParams->mRefFileName, entry[n++], MAX_STR_LEN );
        LList_PushBack( gpParamsList, pParams );
}



/*================================================================================================*/
/*================================================================================================*/
void DoDataOutput(sCodecHandler * pHandler)
{
        assert( pHandler );
        assert( pHandler->mpOutStream );
        
        MIDI_Output_Details * pMidiParOutDetails = &pHandler->mMidiParOutdetails;
        MIDI_Event_Details  * pMidiParEventDetails = pMidiParOutDetails->midi_buf;
        FILE * outFile = pHandler->mpOutStream;
        MIDIPAR_U32 division = 0, i = 0;
        MIDIPAR_U8 dataToWrite = 0;	    
        
        /* For the each of events */
        for( i = 0; i < pMidiParOutDetails->midi_num_events; ++i )
        {
#ifdef PARSER_OUT_FOR_TIMIDITY_MATCH
                division = pMidiParEventDetails->midipar_division;
#else
                division = pMidiParEventDetails->midipar_time;
#endif            
                
                dataToWrite = (MIDIPAR_U8)( (division >> 24) & 0xff );
                fwrite( &dataToWrite, 1, 1, outFile );
                
                dataToWrite = (MIDIPAR_U8)( (division >> 16) & 0xff );
                fwrite( &dataToWrite, 1, 1, outFile );
                
                dataToWrite = (MIDIPAR_U8)( (division >> 8) & 0xff );
                fwrite( &dataToWrite, 1, 1, outFile );
                
                dataToWrite = (MIDIPAR_U8)( division & 0xff );
                fwrite( &dataToWrite, 1, 1, outFile );
                
                dataToWrite = pMidiParEventDetails->midipar_type;
                fwrite( &dataToWrite, 1, 1, outFile );
                
                dataToWrite = pMidiParEventDetails->midipar_channel;
                fwrite( &dataToWrite, 1, 1, outFile );
                
                dataToWrite = pMidiParEventDetails->midipar_first_byte;
                fwrite( &dataToWrite, 1, 1, outFile );
                
                dataToWrite = pMidiParEventDetails->midipar_second_byte;
                fwrite( &dataToWrite, 1, 1, outFile );
                pMidiParEventDetails = (MIDI_Event_Details*)pMidiParEventDetails->midipar_next;
        }
}


