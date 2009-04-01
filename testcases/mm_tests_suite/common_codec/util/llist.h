/*
  Copyright (C) 2004, Freescale Semiconductor, Inc. All Rights Reserved
  THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
  BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
  Freescale Semiconductor, Inc.
*/

/**
@file llist.h

@par Portability:
        ARM GCC
*/

/*======================== REVISION HISTORY ==================================

Author (core ID)      Date         CR Number    Description of Changes
-------------------   ----------   ----------   ------------------------------
D.Simakov / smkd001c  24/01/2006   TLSbo61035   Initial version
=============================================================================*/

#ifndef __LLIST_H__
#define __LLIST_H__

/**********************/
/* Simple linked list */
/**********************/
typedef struct LinkedList
{
        void * mpContent;    
        struct LinkedList * mpNext;
} sLinkedList;

void          LList_Delete   ( sLinkedList * pHead );
void          LList_PushBack ( sLinkedList * pHead, void * pContent );
void          LList_Remove   ( sLinkedList ** ppHead, sLinkedList * pNode );
sLinkedList * LList_Find     ( sLinkedList * pHead, void * pContent, int (*compare)(void*, void*) );


#endif //__LLIST_H__
