/*================================================================================================*/
/**
    @file   mu_api_rwstress_test.h

    @brief  C header file of the mu_api_rwstress_test test that performs stress tests of Messaging
            Unit driver read() and write() system calls
*/
/*==================================================================================================

  Copyright (C) 2004, Freescale Semiconductor, Inc. All Rights Reserved
  THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
  BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
  Freescale Semiconductor, Inc.

====================================================================================================
Revision History:
                              Modification     Tracking
Author (Core ID)                  Date          Number    Description of Changes
---------------------------   ------------    ----------  ------------------------------------------
Igor Semenchukov (smng001c)    24/08/2004     TLSbo40411   Initial version
Igor Semenchukov (smng001c)    09/12/2004     TLSbo43804   Modify declarations and defines

==================================================================================================*/

#ifndef MU_API_RWSTRESS_TEST_H
#define MU_API_RWSTRESS_TEST_H

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
#define NUM_FORKS 4    /* Default number of children                     */
#define NUM_ITER  2    /* Default number of read/write cycles            */
#define BUF_LEN   256
#define NUM_REGS  4    /* Number of registers                            */
#define REG_SIZE  4    /* Register length in bytes                       */
#define OPEN_TRY  50   /* Number of tries to open a device if it is busy */

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
int  VT_mu_api_rwstress_setup();
int  VT_mu_api_rwstress_cleanup();
int VT_mu_api_rwstress_test(char *msg, int blk, int rw_count, int num_child);

#endif  /* MU_API_RWSTRESS_TEST_H */
