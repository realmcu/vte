#ifndef __MEMSTAT_H__
#define __MEMSTAT_H__

#include <stdlib.h>

void * MemStat_Alloc    ( size_t sz );
void   MemStat_Free     ( void * pPtr );
void   MemStat_GetStat  ();

#endif //__MEMSTAT_H__
