/*
  Copyright (C) 2004, Freescale Semiconductor, Inc. All Rights Reserved
  THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
  BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
  Freescale Semiconductor, Inc.
*/

/**
@file cfg_parser.c

@par Portability:
        ARM GCC
*/

/*======================== REVISION HISTORY ==================================

Author (core ID)      Date         CR Number    Description of Changes
-------------------   ----------   ----------   ------------------------------
D.Simakov / smkd001c  24/01/2006   TLSbo61035   Initial version
=============================================================================*/

/* MUST BE COMPILED WITH THE -DWORDS_IN_ENTRY=<...>*/
/* MUST BE COMPILED WITH THE -DMAX_STR_LEN=<...>*/
#include "cfg_parser.h"
#include "llist.h"
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
        unsigned int n = 0, nEntry = 0;                       

        if( (in = fopen( fileName, "rt" )) )
        {
                while( !feof(in) )
                {
                        fscanf( in, "%s", entry[n] );
                        
                        if( n == WORDS_IN_ENTRY-1 )
                        {
                                MakeEntry( entry, nEntry );
                        }
                        ++n;
                        n %= WORDS_IN_ENTRY;
                        !n ? ++nEntry : 0;
                }
                return TRUE;
        }  
        
        return FALSE;
}

