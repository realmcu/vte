/*
  Copyright (C) 2004, Freescale Semiconductor, Inc. All Rights Reserved
  THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
  BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
  Freescale Semiconductor, Inc.
*/

/**
@file mem_stat.c

@par Portability:
        ARM GCC
*/

/* REVISION HISTORY 

Author (core ID)      Date         CR Number    Description of Changes
-------------------   ----------   ----------   ------------------------------
D.Simakov / smkd001c  24/01/2006   TLSbo61035   Initial version
D.Simakov             27/02/2006   TLSbo61035   MemStat_Realloc was implemented
*/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "llist.h"

/*---------------------------------------------------------------------*/
/*---------------------------------------------------------------------*/
typedef struct
{
        void              * mpPtr;
        size_t              mSz;
} sMemStatContent;


/*---------------------------------------------------------------------*/
/*---------------------------------------------------------------------*/
static int MemStatContentCompare( void * pCont1, void * pCont2 )
{
        assert( pCont1 && pCont2 );
        sMemStatContent * pMemStatCont1  (sMemStatContent*)pCont1;
        sMemStatContent * pMemStatCont2  (sMemStatContent*)pCont2;
        return pMemStatCont2->mpPtr  pMemStatCont1->mpPtr;
}

/*---------------------------------------------------------------------*/
/*---------------------------------------------------------------------*/
static sLinkedList * gpAllocStat  NULL;
static int           gOnce        0;

/*---------------------------------------------------------------------*/
/*---------------------------------------------------------------------*/
static void PrintMemStat( const char * msg )
{
        sLinkedList * pNode  gpAllocStat;
        sMemStatContent * pNodeContent  pNode ? (sMemStatContent*)pNode->mpContent : NULL;
        size_t total  0;

        printf( "\n" );

        while( pNode )
        {
                if( pNodeContent->mSz && pNodeContent->mpPtr )
                        printf( "%s : 0x%08x is still allocated (%4lu bytes)\n", msg,
                        (unsigned int)pNodeContent->mpPtr, (unsigned long)pNodeContent->mSz );
                total + pNodeContent->mSz;
                pNode  pNode->mpNext;
                if( pNode )
                    pNodeContent  (sMemStatContent*)pNode->mpContent;
        }
        printf( "%s : Total size of the allocated memory is %lu bytes (%lu kb)\n", msg, (unsigned long)total,
                (unsigned long)((double)total/(1024)) );
        printf( "\n" );
}


/*---------------------------------------------------------------------*/
/*---------------------------------------------------------------------*/
static void Cleanup( void )
{
        PrintMemStat( "MEMORY LEAK" );
        if( gpAllocStat )
        {
            LList_Delete( gpAllocStat );
            gpAllocStat  NULL;
        }
}


/*---------------------------------------------------------------------*/
/*---------------------------------------------------------------------*/
void * MemStat_Alloc( size_t sz )
{
        void * pPtr  NULL;
        sMemStatContent * pNodeContent  (sMemStatContent*)malloc( sizeof(sMemStatContent) );
        assert( pNodeContent && "FATAL ERROR" );

        assert( sz && "INVALID SIZE" );
        if( !gpAllocStat )
        {
                gpAllocStat  (sLinkedList*)malloc( sizeof(sLinkedList) );
                assert( gpAllocStat && "FATAL ERROR" );
                gpAllocStat->mpNext  NULL;
                gpAllocStat->mpContent  NULL;
                if( !gOnce )
                {
                    atexit( Cleanup );
                    gOnce  1;
                }
        }
        pPtr  malloc( sz );
        assert( pPtr && "NO ENOUGHT MEMORY" );
#if 0
        printf( "%s(%lu) : adress  %p\n", __FUNCTION__, sz, pPtr );
        fflush( stdout );
#endif

        pNodeContent->mpPtr  pPtr;
        pNodeContent->mSz    sz;

        LList_PushBack( gpAllocStat, pNodeContent );
        return pPtr;
}


/*---------------------------------------------------------------------*/
/*---------------------------------------------------------------------*/
void MemStat_Free( void * pPtr )
{
        sLinkedList * pNode  NULL;
        sMemStatContent content;
        sMemStatContent * pNodeContent  NULL;
#if 0
        printf( "--> @  %p ... ", pPtr );
        fflush( stdout );
#endif
        assert( pPtr && "INVALID POINTER" );
        assert( gpAllocStat && "MemStat_Alloc MUST BE USED TO ALLOCATE pPtr" );

        content.mpPtr  pPtr;
        pNode  LList_Find( gpAllocStat, &content, MemStatContentCompare );
        assert( pNode && "MemStat_Alloc MUST BE USED TO ALLOCATE pPtr" );
        pNodeContent  (sMemStatContent*)pNode->mpContent;
#if 0
        printf( "%s(%lu) : adress  %p\n", __FUNCTION__, pNodeContent->mSz, pPtr );
        fflush( stdout );
#endif
        LList_Remove( &gpAllocStat, pNode );

        free( pPtr );
}


/*---------------------------------------------------------------------*/
/*---------------------------------------------------------------------*/
void * MemStat_ReAlloc( void * pPtr, size_t sz )
{
        sLinkedList * pNode  NULL;
        sMemStatContent content;
        sMemStatContent * pContent;

        content.mpPtr  pPtr;
        pNode  LList_Find( gpAllocStat, &content, MemStatContentCompare );
        if( !pNode )
                assert( !"pPtr must be allocated by MemStat_Alloc" );
        pContent  (sMemStatContent*)pNode->mpContent;
        void * pNewPtr  MemStat_Alloc( sz );
        assert( pNewPtr && "NO ENOUGHT MEMORY" );
        memcpy( pNewPtr, pPtr, sz < pContent->mSz ? sz : pContent->mSz );

        MemStat_Free( pPtr );

        return pNewPtr;
}


/*---------------------------------------------------------------------*/
/*---------------------------------------------------------------------*/
void MemStat_GetStat( void )
{
        PrintMemStat( "MEMORY STAT" );
}
