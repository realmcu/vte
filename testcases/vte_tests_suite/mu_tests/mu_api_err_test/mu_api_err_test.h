/*================================================================================================*/
/**
    @file   mu_api_err_test.h

    @brief  C header of the mu_api_err_test application that generates variuos errors
            and checks proper Messaging Unit driver reactions.
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
Igor Semenchukov (smng001c)    25/08/2004     TLSbo40411   Initial version
Igor Semenchukov (smng001c)    09/12/2004     TLSbo43804   Modify declarations and defines

==================================================================================================*/

#ifndef MU_API_ERR_TEST_H
#define MU_API_ERR_TEST_H

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
#define NR_DEVS  4      /* Number of MU devices       */
#define REG_SIZE 4      /* Register length in bytes   */
#define BAD_CNT  5      /* Improper length of message */
#define BIG_LEN  8192   /* Length of big message      */
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
int VT_mu_api_err_setup();
int VT_mu_api_err_cleanup();
int VT_mu_api_err_test(void);

#endif  /* MU_API_ERR_TEST_H */
