
#ifndef __CFG_PARSER_H__
#define __CFG_PARSER_H__

#define MAX_STR_LEN    256
#define WORDS_IN_ENTRY 4

/************************************************************************/
/* Simple linked list                                                   */
/************************************************************************/

typedef struct LinkedList
{
    void * mpContent;
    struct LinkedList * mpNext;
} sLinkedList;

void LList_Delete  ( sLinkedList * pHead );
void LList_PushBack( sLinkedList * pHead, void * pContent );

/************************************************************************/
/*                                                                      */
/************************************************************************/

void MakeEntry   ( char entry[WORDS_IN_ENTRY][MAX_STR_LEN], int nEntry );
int  ParseConfig ( const char * fileName );


#endif //__CFG_PARSER_H__
