/*
  Copyright (C) 2004, Freescale Semiconductor, Inc. All Rights Reserved
  THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
  BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
  Freescale Semiconductor, Inc.
*/

/**
@file common_codec_test.c

@par Portability:
        ARM GCC
*/

/*======================== REVISION HISTORY ==================================

Author (core ID)      Date         CR Number    Description of Changes
-------------------   ----------   ----------   ------------------------------
D.Simakov / smkd001c  24/01/2006   TLSbo61035   Initial version 
D.Simakov / smkd001c  08/02/2006   TLSbo61035   Util_ReadFile and Util_SwapBytes were added
D.Simakov             27/02/2006   TLSbo61035   Util_Realloc was added
=============================================================================*/

/*==================================================================================================
                                        INCLUDE FILES
==================================================================================================*/

/* Standard Include Files. */
#include <errno.h>
#include <unistd.h> 
#include <sys/mman.h> 
#include <pthread.h>
#include <math.h>                
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>

                  
/* Harness Specific Include Files. */
#include "test.h"

/* Other Include Files. */
#include "codec_test.h"

/* Common Include Files. */
#include <common_codec_test.h>
#include <util/cfg_parser.h>
#include <util/mem_stat.h>
#include <util/llist.h>



/*==================================================================================================
                                       GLOBAL VARIABLES
==================================================================================================*/

sLinkedList   * gpParamsList                    = NULL;
sCodecHandler   gCodecHandler[MAX_THREADS]; 
pthread_mutex_t gMutex                          = PTHREAD_MUTEX_INITIALIZER; 
int             gNumThreads                     = 1;
int (*CompareFiles)( sCodecHandler*, const char*, const char* ) = CompareFilesFast;


/*==================================================================================================
                                       FUNCTIONS
==================================================================================================*/

/*================================================================================================*/
/*================================================================================================*/
int VT_codec_setup( void )
{
        int i;
        /* Reset all handlers. */
        for( i = 0; i < MAX_THREADS; ++i )
                ResetHandler( gCodecHandler + i );        
        
        /**/
        gpParamsList = (sLinkedList*)malloc(sizeof(sLinkedList));
        gpParamsList->mpNext = NULL;
        gpParamsList->mpContent = NULL;
        
        if( !ParseConfig( gTestappConfig.mConfigFilename ) )        
        {
                TST_RESM( TWARN, "%s : Can't parse the %s", 
                        __FUNCTION__, gTestappConfig.mConfigFilename );
                return TFAIL;
        }   
        
        /* Compute the actual number of threads. */
        //if( IsCurrentTestCaseMultithreaded() ) 
        if( RE_ENTRANCE == gTestappConfig.mTestCase || 
                PRE_EMPTION == gTestappConfig.mTestCase )
        {
                sLinkedList * pNode = gpParamsList;
                for( gNumThreads = 0; pNode && gNumThreads < MAX_THREADS; ++gNumThreads )
                        pNode = pNode->mpNext;
        }

        if( gTestappConfig.mSlowBitMatching )
                CompareFiles = CompareFilesSlow;
        
        return TPASS; 
}


/*================================================================================================*/
/*================================================================================================*/
int VT_codec_cleanup( void )
{
        if( gpParamsList ) 
        {
                LList_Delete( gpParamsList );
                gpParamsList = NULL;
        }                
        
        int i;
        for( i = 0; i < MAX_THREADS; ++i )
        {
                CleanupHandler( gCodecHandler + i );
                ResetHandler( gCodecHandler + i );        
        }
        
        return TPASS; 
}


/*================================================================================================*/
/*================================================================================================*/
int VT_codec_test( void )
{
        int rv = TFAIL;            
        
        switch( gTestappConfig.mTestCase )
        {
        case NOMINAL_FUNCTIONALITY:    
                rv = NominalFunctionalityTest();
                break;
                
        case RELOCATABILITY:
                rv = ReLocatabilityTest();
                break;
                
        case RE_ENTRANCE:
                rv = ReEntranceTest();
                break;
                
        case PRE_EMPTION:
                rv = PreEmptionTest();
                break;                    
                
        case ENDURANCE:
                rv = EnduranceTest();
                break;    
                
        case LOAD:
                rv = LoadTest();
                break;                                    
                
        default:
                rv = ExtraTestCases();
                break;    
        }    
        
#ifdef POST_BIT_MATCHING
        /* Perform bitmatching */
        if( gTestappConfig.mVerbose )
        {        
                TST_RESM( TINFO, "Perform bit-matching..." );
        }

        sLinkedList * pNode;
        sCodecHandler handler;        
        handler.mIndex = 0;
        for( pNode = gpParamsList; pNode; pNode = pNode->mpNext )
        {        
                sHandlerParams * pParams = (sHandlerParams*)pNode->mpContent;
                handler.mpParams = pParams;                
                if( !pParams->mIsReadyForBitMatching )
                        continue;
                char cfiles[MAX_STR_LEN];
#ifndef MANY_REF_OUT_FILES                                
                const char * fname1 = pParams->mOutFileName;
                const char * fname2 = pParams->mRefFileName;
                sprintf( cfiles, "(%s vs %s)", fname1, fname2 );
                if( DoFilesExist( fname1, fname2 ) )
#else
                sprintf( cfiles, " " );
                if( IsBitmatchNeeded( &handler ) )
#endif //MANY_REF_OUT_FILES                
                {              
                        if( !DoBitmatch( &handler ) )
                        {
                                if( gTestappConfig.mVerbose )
                                {
                                        TST_RESM( TFAIL, "Entry[%lu] Bitmatch failed %s", pParams->mNoEntry, cfiles );        
                                }
                                rv = TFAIL;
                        }
                        else
                        {
                                if( gTestappConfig.mVerbose )
                                {
                                        TST_RESM( TINFO, "Entry[%lu] Bitmatch passed %s", pParams->mNoEntry, cfiles );
                                }
                        }        
                }    
        }   
#endif
        return rv; 
}


/*================================================================================================*/
/*================================================================================================*/
int RunCodec( void * pContext )
{
        assert( pContext );    
        
        sCodecHandler * pHandler = (sCodecHandler*)pContext;
        
        /* Set priority for the PRE_EMPTION Test case. */
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
        CleanupHandler( pHandler );        
        
#ifndef POST_BIT_MATCHING
        /* Perform bit-matching */
        char cfiles[MAX_STR_LEN];
#ifndef MANY_REF_OUT_FILES                        
        const char * fileName1 = pHandler->mpParams->mOutFileName;
        const char * fileName2 = pHandler->mpParams->mRefFileName;
        sprintf( cfiles, "(%s vs %s)", fileName1, fileName2 );        
        if( DoFilesExist( fileName1, fileName2 ) )
#else
        sprintf( cfiles, " " );
        if( IsBitmatchNeeded( pHandler ) )
#endif //MANY_REF_OUT_FILES        
        {              
                if( !DoBitmatch( pHandler ) )
                {
                        if( gTestappConfig.mVerbose )
                                TST_RESM( TFAIL, "Thread[%lu][%lu] Bitmatch failed %s", 
                                pHandler->mIndex, pHandler->mpParams->mNoEntry, cfiles );        
                        pHandler->mLtpRetval = TFAIL;
                }
                else
                {
                        if( gTestappConfig.mVerbose )
                                TST_RESM( TINFO, "Thread[%lu][%lu] Bitmatch passed %s", 
                                pHandler->mIndex, pHandler->mpParams->mNoEntry, cfiles );                    
                }        
        } 
#endif
        
        /* Return LTP result. */
        return pHandler->mLtpRetval;
}


/*================================================================================================*/
/*================================================================================================*/
void ResetHandler( sCodecHandler * pHandler )
{
        assert( pHandler );        
        memset( pHandler, 0, sizeof(sCodecHandler) );            
} 


/*================================================================================================*/
/*================================================================================================*/
int CompareFilesFast( sCodecHandler * pHandler, const char * fname1, const char * fname2 )
{       
        assert( fname1 && fname2 );
        
        int out, ref;

        struct stat fstat_out, fstat_ref;
        char *fptr_out, *fptr_ref;
        size_t filesize;
        int i;
        
        if( (out = open(fname1, O_RDONLY)) < 0 )
        {
                if( gTestappConfig.mVerbose )
                {
                        TST_RESM( TWARN, "%s : Thread[%lu][%lu] Can't open %s", __FUNCTION__, pHandler->mIndex, pHandler->mpParams->mNoEntry, fname1 );
                }
                return FALSE;
        }
        if( (ref = open(fname2, O_RDONLY)) < 0 )
        {
                if( gTestappConfig.mVerbose )
                {                
                        TST_RESM( TWARN, "%s : Thread[%lu][%lu] Can't open %s", __FUNCTION__, pHandler->mIndex, pHandler->mpParams->mNoEntry, fname2 );
                }
                close(out);
                return FALSE;
        }
        fstat( out, &fstat_out );
        fstat( ref, &fstat_ref );
        if( fstat_out.st_size != fstat_ref.st_size )
        {   
                if( gTestappConfig.mVerbose )
                {                
                        TST_RESM( TWARN, "%s : Thread[%lu][%lu] Files (%s vs %s) have the different sizes (%lu vs %lu)",
                                __FUNCTION__, pHandler->mIndex, pHandler->mpParams->mNoEntry, fname1, fname2,
                                (unsigned long)fstat_out.st_size,
                                (unsigned long)fstat_ref.st_size );
                }
                close(out);
                close(ref);
                return FALSE;
        }
        filesize = fstat_out.st_size;
        fptr_out = (char*)mmap( 0, filesize, PROT_READ, MAP_SHARED, out, 0 );
        if( fptr_out == MAP_FAILED )
        {
                if( gTestappConfig.mVerbose )
                {                
                        TST_RESM( TWARN, "%s : Thread[%lu][%lu] (FATAL) mmap failed for %s", __FUNCTION__, pHandler->mIndex, pHandler->mpParams->mNoEntry, fname1 );
                }
                close( out );
                close( ref );
                return FALSE;
        }
        fptr_ref = (char*) mmap(0, filesize, PROT_READ, MAP_SHARED, ref, 0);
        if( fptr_ref == MAP_FAILED )
        {
                if( gTestappConfig.mVerbose )
                {                
                        TST_RESM( TWARN, "%s : Thread[%lu][%lu] (FATAL) mmap failed for %s", __FUNCTION__, pHandler->mIndex, pHandler->mpParams->mNoEntry, fname2 );
                }
                munmap( fptr_out, fstat_out.st_size );
                close( out );
                close( ref );
                return FALSE;
        }
        close( out );
        close( ref );
        for( i = 0; i < filesize; ++i )
        {
                if( *(fptr_ref + i) != *(fptr_out + i) )
                {
                        if( gTestappConfig.mVerbose )
                        {                        
                                TST_RESM( TWARN, "%s : Thread[%lu][%lu] (%s vs %s) byte %d", __FUNCTION__, pHandler->mIndex, pHandler->mpParams->mNoEntry, fname1, fname2, i );
                        }
                        munmap( fptr_ref, fstat_ref.st_size );
                        munmap( fptr_out, fstat_out.st_size );
                        return FALSE;
                }
        }
        munmap( fptr_ref, filesize );
        munmap( fptr_out, filesize );
        return TRUE; 
}


/*================================================================================================*/
/*================================================================================================*/
int CompareFilesSlow( sCodecHandler * pHandler, const char * fname1, const char * fname2 )
{
        FILE * pf1, *pf2;
        int rd1, rd2;
        unsigned char b1, b2;
        unsigned int nByte;
        
        if( fname1 && fname2 )
        {
                pf1 = fopen( fname1, "rb" );
                pf2 = fopen( fname2, "rb" );
                
                if( pf1 && pf2 )
                {
                        while( TRUE )
                        {
                                if( feof(pf1) != feof(pf2) )
                                {
                                        if( gTestappConfig.mVerbose )
                                        {                                        
                                                TST_RESM( TWARN, "%s : Thread[%lu][%lu] Files (%s vs %s) have the different sizes",
                                                        __FUNCTION__, pHandler->mIndex, pHandler->mpParams->mNoEntry, fname1, fname2 );
                                        }
                                        break; /* bitmach fails */
                                }
                                
                                rd1 = fread( &b1, 1, sizeof(b1), pf1 );
                                rd2 = fread( &b2, 1, sizeof(b2), pf2 );
                                ++nByte;
                                
                                if( b1 != b2 )
                                {
                                        if( gTestappConfig.mVerbose )
                                        {                                        
                                                TST_RESM( TWARN, "%s : Thread[%lu][%lu] (%s vs %s) byte %d", __FUNCTION__, pHandler->mIndex, pHandler->mpParams->mNoEntry, fname1, fname2, nByte );
                                        }
                                        break; /* bitmach fails */
                                }
                                
                                if( feof(pf1) && feof(pf2) )
                                {
                                        fclose( pf1 );
                                        fclose( pf2 );
                                        return TRUE;
                                }
                        }
                }
                
                if( pf1 ) fclose( pf1 );
                if( pf2 ) fclose( pf2 );
        }
        if( gTestappConfig.mVerbose )
        {        
                TST_RESM( TWARN, "%s : Thread[%lu][%lu] Unable to open files %s or %s",
                        __FUNCTION__, pHandler->mIndex, pHandler->mpParams->mNoEntry, fname1, fname2 );
        }
        return FALSE;
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


/*================================================================================================*/
/*================================================================================================*/
void HogCpu( void )
{
        while( 1 )
        {
                sqrt( rand() );
        }
} 


/*================================================================================================*/
/*================================================================================================*/
int NominalFunctionalityTest( void )
{
        sLinkedList * pNode;
        int i;
        int rv = TPASS;    
        sCodecHandler * pHandler = gCodecHandler;     
        
        /* For the each entry */
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
                
                /* Run the Encoder. */
                rv += RunCodec( pHandler );                        
        }
        
        return rv;    
} 


/*================================================================================================*/
/*================================================================================================*/
int ReLocatabilityTest( void )
{
        sLinkedList * pNode;
        int i, j;
        int rv = TPASS;    
        sCodecHandler * pHandler = gCodecHandler;     
        
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
                                if( gTestappConfig.mVerbose )            
                                {
                                        PrintInfo( pHandler );
                                }               
                        }                        
                        
                        /* Run the Encoder. */
                        rv += RunCodec( pHandler );        
                        
                        if( gTestappConfig.mVerbose )            
                        {
                                TST_RESM( TINFO, "Thread[%lu][%lu] Data memory was relocated", pHandler->mIndex, pHandler->mpParams->mNoEntry );
                        }                        
                }
        }    
        return rv;    
} 


/*================================================================================================*/
/*================================================================================================*/
int ReEntranceTest( void )
{
        int ReEntranceTestCore( sLinkedList * pHead );
        sLinkedList * pHead = gpParamsList;
        int rv = TPASS;
        int i;
        
        while( pHead )
        {   
                rv += ReEntranceTestCore( pHead );
                for( i = 0; i < gNumThreads && pHead; ++i )
                {
                        ResetHandler( &gCodecHandler[i] );
                        gCodecHandler[i].mIndex = i;
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
        sCodecHandler * pHandler;        
        
        /* Run all bitstreams in separate threads. */
        for( pNode = pHead, i = 0; pNode && i < gNumThreads; pNode = pNode->mpNext, ++i )
        {        
                pHandler = gCodecHandler + i;
                ResetHandler( pHandler );
                pHandler->mIndex = i;
                
                /* Get content. */
                pHandler->mpParams = (sHandlerParams*)pNode->mpContent;
                
                if( gTestappConfig.mVerbose )            
                {
                        PrintInfo( pHandler );
                }                             
                
                if( pthread_create( &pHandler->mThreadID, NULL, (void*)&RunCodec, pHandler ) )
                {
                        TST_RESM( TWARN, "%s : error creating thread %d", __FUNCTION__, i );
                        return TFAIL;	    
                }
        }    
        
        /* Wait for the each thread. */
        for( i = 0; i < gNumThreads; ++i )
        {
                pHandler = gCodecHandler + i;     
                pthread_join( pHandler->mThreadID, NULL );
        }
        for( i = 0; i < gNumThreads; ++i )
        {
                pHandler = gCodecHandler + i;     
                rv += pHandler->mLtpRetval;
        }
        
        return rv;
}


/*================================================================================================*/
/*================================================================================================*/
int PreEmptionTest( void )
{
        return ReEntranceTest();
}
 

/*================================================================================================*/
/*================================================================================================*/
int EnduranceTest( void )
{
        int i;
        int rv = TPASS;
        
        for( i = 0; i < gTestappConfig.mNumIter; ++i )
        {
                if( gTestappConfig.mVerbose )    
                {
                        TST_RESM( TINFO, "The %d iteration has been started", i+1 );
                }
                rv += NominalFunctionalityTest();	
                if( gTestappConfig.mVerbose )
                {
                        TST_RESM( TINFO, "The %d iteration has been completed", i+1 );
                }
        }    
        
        return rv;
} 


/*================================================================================================*/
/*================================================================================================*/
int LoadTest( void )
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
void * Util_Malloc( size_t bytes )
{
#ifdef DEBUG_TEST
    return NOMINAL_FUNCTIONALITY == gTestappConfig.mTestCase ? MemStat_Alloc( bytes ) : malloc( bytes );
#else
    return malloc( bytes );
#endif
}


/*================================================================================================*/
/*================================================================================================*/
void * Util_Realloc( void * pPtr, size_t bytes )
{
#ifdef DEBUG_TEST
    return NOMINAL_FUNCTIONALITY == gTestappConfig.mTestCase ? MemStat_ReAlloc( pPtr, bytes ) : realloc( pPtr, bytes );
#else
    return realloc( pPtr, bytes );
#endif
}



/*================================================================================================*/
/*================================================================================================*/
void Util_Free( void * pPtr )
{
    assert( pPtr );

#ifdef DEBUG_TEST
    return NOMINAL_FUNCTIONALITY == gTestappConfig.mTestCase ? MemStat_Free( pPtr ) : free( pPtr );
#else
    return free( pPtr );
#endif
}
 

/*================================================================================================*/
/*================================================================================================*/
void * Util_ReadFile( const char * filename, size_t * pSz )
{
    FILE * pInStream = fopen( filename, "rb" );    
    if( pInStream )
    {        
        fseek( pInStream, 0, SEEK_END );
        size_t sz = ftell( pInStream );
        fseek( pInStream, 0, SEEK_SET );
        void * pData = Util_Malloc( sizeof(char) * sz );
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
