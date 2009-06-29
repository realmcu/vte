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
/*================================================================================================*/
/**
    @file   mu_api_rw_test.h

    @brief  C header of the mu_api_rw_test test that checks Messaging Unit
            driver read() and write() system calls in both blocking and non-blocking modes.

====================================================================================================
Revision History:
                              Modification     Tracking
Author (Core ID)                  Date          Number    Description of Changes
---------------------------   ------------    ----------  ------------------------------------------
Igor Semenchukov (smng001c)    23/08/2004     TLSbo40411   Initial version 
Igor Semenchukov (smng001c)    08/12/2004     TLSbo43804   Rework after heavy MU drv modification;
                                                           merge two test cases in the one 

==================================================================================================*/

#ifndef MU_API_RW_TEST_H
#define MU_API_RW_TEST_H

/*==================================================================================================
                                         INCLUDE FILES
==================================================================================================*/


/*==================================================================================================
                                           CONSTANTS
==================================================================================================*/
#if !defined(TRUE) && !defined(FALSE)
#define TRUE  1
#define FALSE 0
#endif

/*==================================================================================================
                                       DEFINES AND MACROS
==================================================================================================*/
#define BUF_LEN  256
#define REG_SIZE 4   /* Register size in bytes */
#define NUM_DEVS 4   /* Number of MU devices   */

/*==================================================================================================
                                             ENUMS
==================================================================================================*/


/*==================================================================================================
                                 STRUCTURES AND OTHER TYPEDEFS
==================================================================================================*/


/*==================================================================================================
                                 GLOBAL VARIABLE DECLARATIONS
==================================================================================================*/


/*==================================================================================================
                                     FUNCTION PROTOTYPES
==================================================================================================*/
int  VT_mu_api_rw_setup();
int  VT_mu_api_rw_cleanup();
int  VT_mu_api_rw_test(char *msg, int blk);


#endif  /* MU_API_RW_TEST_H */
