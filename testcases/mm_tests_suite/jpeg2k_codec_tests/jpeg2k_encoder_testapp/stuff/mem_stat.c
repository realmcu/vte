#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

/*---------------------------------------------------------------------*/
/*---------------------------------------------------------------------*/
typedef struct LinkedList
{
    void              * mpPtr;
    size_t              mSz;
    struct LinkedList * mpNext;
} sLinkedList;


/*---------------------------------------------------------------------*/
/*---------------------------------------------------------------------*/
static sLinkedList * gpAllocStat = NULL;


/*---------------------------------------------------------------------*/
/*---------------------------------------------------------------------*/
static void LList_Delete( sLinkedList * pHead )
{
    sLinkedList * pNode = pHead;
    sLinkedList * pNext = pNode;

    while( pNode )
    {
        pNext = pNode->mpNext;        
        free( pNode ); pNode = NULL;
        pNode = pNext;
    }
}


/*---------------------------------------------------------------------*/
/*---------------------------------------------------------------------*/
static void LList_PushBack( sLinkedList * pHead, void * pPtr, size_t sz )
{
    assert( pHead );

    if( !pHead->mpPtr )
    {
        pHead->mpPtr = pPtr;
        pHead->mSz = sz;
    }
    else if( pHead->mpNext )
        LList_PushBack( pHead->mpNext, pPtr, sz );
    else
    {
        pHead->mpNext = (sLinkedList*)malloc( sizeof(sLinkedList) );
        pHead->mpNext->mpNext = NULL;
        pHead->mpNext->mpPtr = pPtr;
        pHead->mpNext->mSz = sz;
    }
}


/*---------------------------------------------------------------------*/
/*---------------------------------------------------------------------*/
static sLinkedList * LList_Find( sLinkedList * pHead, void * pPtr )
{
    assert( pHead );

    while( pHead )
    {
        if( pHead->mpPtr == pPtr )
            return pHead;
        pHead = pHead->mpNext;
    }
    return NULL;
}


/*---------------------------------------------------------------------*/
/*---------------------------------------------------------------------*/
static void LList_Remove( sLinkedList ** ppHead, sLinkedList * pNode )
{
    assert( ppHead );
    assert( *ppHead );
    
    sLinkedList * pPrev = NULL;
    sLinkedList * pCurr = *ppHead;
    sLinkedList * pNext = pCurr->mpNext;    
        
    while( pNode != pCurr && pCurr )
    {
        pPrev = pCurr;
        pCurr = pCurr->mpNext;
        pNext = pCurr->mpNext;
    }

    assert( pCurr && "NO SUCH NODE" );
    if( pPrev ) 
        pPrev->mpNext = pNext;       
    else
    {
        *ppHead = pNext;        
        if( !*ppHead )
        {
            *ppHead = (sLinkedList*)malloc( sizeof(sLinkedList) );
            assert( *ppHead && "FATAL ERROR" );
            (*ppHead)->mpNext = (*ppHead)->mpPtr = NULL;
            (*ppHead)->mSz = 0;
        }
    }            
        
    free( pNode );
}


/*---------------------------------------------------------------------*/
/*---------------------------------------------------------------------*/
static void PrintMemStat( const char * msg )
{
    sLinkedList * pNode = gpAllocStat;
    size_t total = 0;
    
    printf( "=================================================================================\n" );

    while( pNode )
    {
        if( pNode->mSz && pNode->mpPtr )
            printf( "%s : 0x%08x is still allocated (%4lu bytes)\n", msg,
                    (unsigned int)pNode->mpPtr, (unsigned long)pNode->mSz );
        total += pNode->mSz;
        pNode = pNode->mpNext;        
    }
    printf( "%s : Total size of the allocated memory is %lu bytes (%lu kb)\n", msg, (unsigned long)total,
            (unsigned long)((double)total/(1024)) );
    printf( "=================================================================================\n" );
}


/*---------------------------------------------------------------------*/
/*---------------------------------------------------------------------*/
static void Cleanup( void )
{
    PrintMemStat( "MEMORY LEAK" );                
    LList_Delete( gpAllocStat );
    gpAllocStat = NULL;
}


/*---------------------------------------------------------------------*/
/*---------------------------------------------------------------------*/
void * MemStat_Alloc( size_t sz )
{
    void * pPtr = NULL;

    assert( sz && "INVALID SIZE" );
    if( !gpAllocStat )
    {
        gpAllocStat = (sLinkedList*)malloc( sizeof(sLinkedList) );
        assert( gpAllocStat && "FATAL ERROR" );
        gpAllocStat->mpNext = gpAllocStat->mpPtr = NULL;
        gpAllocStat->mSz = 0;
        atexit( Cleanup );
    }
    pPtr = malloc( sz );
    assert( pPtr && "NO ENOUGHT MEMORY" );
#if 0    
    printf( "%s(%lu) : adress = %p\n", __FUNCTION__, sz, pPtr );
    fflush( stdout );
#endif        

    LList_PushBack( gpAllocStat, pPtr, sz );
    return pPtr;
}


/*---------------------------------------------------------------------*/
/*---------------------------------------------------------------------*/
void MemStat_Free( void * pPtr )
{
    sLinkedList * pNode = NULL;
#if 0    
    printf( "--> @ = %p ... ", pPtr );
#endif
    assert( pPtr && "INVALID POINTER" );
    assert( gpAllocStat && "MemStat_Alloc MUST BE USED TO ALLOCATE pPtr" );

    pNode = LList_Find( gpAllocStat, pPtr );
    assert( pNode && "MemStat_Alloc MUST BE USED TO ALLOCATE pPtr" );
#if 0    
    printf( "%s(%lu) : adress = %p\n", __FUNCTION__, pNode->mSz, pPtr );
    fflush( stdout );
#endif    
    LList_Remove( &gpAllocStat, pNode );
    
    free( pPtr );
}


/*---------------------------------------------------------------------*/
/*---------------------------------------------------------------------*/
void MemStat_GetStat()
{
    PrintMemStat( "MEMORY STAT" );
}
