/*
  Copyright (C) 2004, Freescale Semiconductor, Inc. All Rights Reserved
  THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
  BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
  Freescale Semiconductor, Inc.
*/

/**
@file llist.c

@par Portability:
        ARM GCC
*/

/*======================== REVISION HISTORY ==================================

Author (core ID)      Date         CR Number    Description of Changes
-------------------   ----------   ----------   ------------------------------
D.Simakov / smkd001c  24/01/2006   TLSbo61035   Initial version
=============================================================================*/

#include <stdlib.h>
#include <assert.h>
#include "llist.h"


/*---------------------------------------------------------------------*/
/*---------------------------------------------------------------------*/
void LList_Delete( sLinkedList * pHead )
{       
        sLinkedList * pNode = pHead;
        sLinkedList * pNext = pNode;
        
        while( pNode )
        {
                pNext = pNode->mpNext;
                if( pNode->mpContent ) 
                {
                        free( pNode->mpContent );
                        pNode->mpContent = NULL;
                }
                free( pNode ); pNode = NULL;
                pNode = pNext;
        } 
}


/*---------------------------------------------------------------------*/
/*---------------------------------------------------------------------*/
void LList_PushBack( sLinkedList * pHead, void * pContent )
{
        assert( pHead && pContent );
        
        if( !pHead->mpContent )
        {
                pHead->mpContent = pContent;
        }
        else if( pHead->mpNext )
                LList_PushBack( pHead->mpNext, pContent );
        else
        {
                pHead->mpNext = (sLinkedList*)malloc( sizeof(sLinkedList) );
                pHead->mpNext->mpNext = NULL;
                pHead->mpNext->mpContent = pContent;
        }
}


/*---------------------------------------------------------------------*/
/*---------------------------------------------------------------------*/
void LList_Remove( sLinkedList ** ppHead, sLinkedList * pNode )
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
        }            

        if( pNode->mpContent )
        {
            free( pNode->mpContent );
            pNode->mpContent = NULL;
        }        
        free( pNode );
}


/*---------------------------------------------------------------------*/
/*---------------------------------------------------------------------*/
sLinkedList * LList_Find( sLinkedList * pHead, void * pContent, int (*compare)(void*, void*) )
{
        assert( pHead && pContent && compare );
        
        while( pHead )
        {
                if( compare( pHead->mpContent, pContent ) )
                {
                        return pHead;
                }                                            
                pHead = pHead->mpNext;
        }
        return NULL;
}

