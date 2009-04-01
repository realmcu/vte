/*
  Copyright (C) 2004, Freescale Semiconductor, Inc. All Rights Reserved
  THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
  BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
  Freescale Semiconductor, Inc.
*/

/**
@file mem_stat.h

@par Portability:
        ARM GCC
*/

/*======================== REVISION HISTORY ==================================

Author (core ID)      Date         CR Number    Description of Changes
-------------------   ----------   ----------   ------------------------------
D.Simakov / smkd001c  24/01/2006   TLSbo61035   Initial version
=============================================================================*/

#ifndef __MEMSTAT_H__
#define __MEMSTAT_H__

#include <stdlib.h>

void * MemStat_Alloc    ( size_t sz );
void   MemStat_Free     ( void * pPtr );
void * MemStat_ReAlloc  ( void * pPtr, size_t sz );
void   MemStat_GetStat  ( void );


#endif //__MEMSTAT_H__
