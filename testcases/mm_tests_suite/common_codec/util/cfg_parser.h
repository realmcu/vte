/*
  Copyright (C) 2004, Freescale Semiconductor, Inc. All Rights Reserved
  THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
  BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
  Freescale Semiconductor, Inc.
*/

/**
@file cfg_parser.h

@par Portability:
        ARM GCC
*/

/*======================== REVISION HISTORY ==================================

Author (core ID)      Date         CR Number    Description of Changes
-------------------   ----------   ----------   ------------------------------
D.Simakov / smkd001c  24/01/2006   TLSbo61035   Initial version
=============================================================================*/

#ifndef __CFG_PARSER_H__
#define __CFG_PARSER_H__

void MakeEntry   ( char entry[WORDS_IN_ENTRY][MAX_STR_LEN], unsigned int nEntry );
int  ParseConfig ( const char * fileName );


#endif //__CFG_PARSER_H__
