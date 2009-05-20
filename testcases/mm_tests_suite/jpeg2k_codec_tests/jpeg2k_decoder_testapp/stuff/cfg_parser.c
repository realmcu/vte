/*---------------------------------------------------------------------*/
/*---------------------------------------------------------------------*/
#include "cfg_parser.h"
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

#if !defined(TRUE) && !defined(FALSE)
    #define TRUE  1
    #define FALSE 0
#endif


/*---------------------------------------------------------------------*/
/*---------------------------------------------------------------------*/
int ParseConfig( const char * fileName )
{
    FILE * in;
    char entry[WORDS_IN_ENTRY][MAX_STR_LEN];
    int n  0, nEntry  0;

    if( (in  fopen( fileName, "rt" )) )
    {
        while( !feof(in) )
        {
            fscanf( in, "%s", entry[n] );

            if( n  WORDS_IN_ENTRY-1 )
            {
                MakeEntry( entry, nEntry );
            }
            ++n;
            n % WORDS_IN_ENTRY;
            !n ? ++nEntry : 0;
        }
        return TRUE;
    }

    return FALSE;
}


/*---------------------------------------------------------------------*/
/*---------------------------------------------------------------------*/
void LList_Delete( sLinkedList * pHead )
{
    sLinkedList * pNode  pHead;
    sLinkedList * pNext  pNode;

    while( pNode )
    {
    pNext  pNode->mpNext;
        if( pNode->mpContent )
        {
            free( pNode->mpContent );
            pNode->mpContent  NULL;
        }
     free( pNode ); pNode  NULL;
    pNode  pNext;
    }
}

/*---------------------------------------------------------------------*/
/*---------------------------------------------------------------------*/
void LList_PushBack( sLinkedList * pHead, void * pContent )
{
    assert( pHead );

    if( !pHead->mpContent )
    {
        pHead->mpContent  pContent;
    }
    else if( pHead->mpNext )
        LList_PushBack( pHead->mpNext, pContent );
    else
    {
        pHead->mpNext  (sLinkedList*)malloc( sizeof(sLinkedList) );
        pHead->mpNext->mpNext  NULL;
        pHead->mpNext->mpContent  pContent;
    }
}
