/***
**Copyright 2004-2009 Freescale Semiconductor, Inc. All Rights Reserved.
**
**The code contained herein is licensed under the GNU General Public
**License. You may obtain a copy of the GNU General Public License
**Version 2 or later at the following locations:
**
**http://www.opensource.org/licenses/gpl-license.html
**http://www.gnu.org/copyleft/gpl.html
**/

/**
@file ssi_dam_test.h

@par Portability:
        ARM GCC
*/

/*======================== REVISION HISTORY ==================================

Author (core ID)      Date         CR Number    Description of Changes
-------------------   ----------   ----------   ------------------------------
Tony THOMASSIN/RB595C 29/07/2004   TLSbo41151   SSI/DAM test development 
I.Inkina/nknl001      29/06/2005   TLSbo50735   code was improved  
V.Halabuda/hlbv001    05/08/2005   TLSbo53363   update for linux-2.6.10-rel-1.12.arm
I.Inkina/nknl001      22/09/2005   TLSbo55818   update for VTE_1.13  
D.Khoroshev/b00313    12/01/2005   TLSbo56844   Add SC55112 support
D.Simakov             05/06/2006   TLSbo67103   Re-written
D.Kardakov            11/09/2006   TLSbo71015   update for L26_21 release
=============================================================================*/

#ifndef __SSI_DAM_TEST_H__
#define __SSI_DAM_TEST_H__


/*==================================================================================================
                                         INCLUDE FILES
==================================================================================================*/

#include <testmod_ssi.h>
/*==================================================================================================
                                       DEFINES AND MACROS
==================================================================================================*/

#if !defined(TRUE)
    #define TRUE 1
#endif
#if !defined(FALSE)
    #define FALSE 0
#endif

#define MAX_STR_LEN 512

#define DM__() {printf("%s:%d %s()\n", __FILE__, __LINE__, __FUNCTION__); fflush(stdout);}


/*==================================================================================================
                                 STRUCTURES AND OTHER TYPEDEFS
==================================================================================================*/

typedef struct
{
        int     mTestCase;
        char *  mSndName;        
        int     mWriteChunkSz;
} sTestappConfig; 

/*==================================================================================================
                                 GLOBAL VARIABLE DECLARATIONS
==================================================================================================*/

extern sTestappConfig  gTestappConfig;     


/*==================================================================================================
                                     FUNCTION PROTOTYPES
==================================================================================================*/

/* VTE */
int VT_ssi_dam_setup    ( void );
int VT_ssi_dam_cleanup  ( void );
int VT_ssi_dam_test     ( void );

#endif //__SSI_DAM_TEST_H__
