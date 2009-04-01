/*==================================================================================================

        Copyright (C) 2007, Freescale Semiconductor, Inc. All Rights Reserved
        THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
        BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
        Freescale Semiconductor, Inc.

====================================================================================================
Revision History:
                            Modification     Tracking
Author/Core ID                  Date          Number     Description of Changes
-------------------------   ------------    -----------  -------------------------------------------
A.Ozerov/b00320              29/05/2007      ENGR37685   Initial version
==================================================================================================*/

#ifndef __SYSTEST2_TEST_H__
#define __SYSTEST2_TEST_H__

/*==================================================================================================
                                         INCLUDE FILES
==================================================================================================*/
/* Harness Specific Include Files.*/
#include <test.h>
#include <usctest.h>

/*==================================================================================================
                                       DEFINES AND MACROS
==================================================================================================*/
#if !defined(TRUE) && !defined(FALSE)
    #define TRUE  1
    #define FALSE 0
#endif

#define MAX_STR_LEN 80

/*==================================================================================================
                                             ENUMS
==================================================================================================*/

/*==================================================================================================
                                 STRUCTURES AND OTHER TYPEDEFS
==================================================================================================*/
typedef struct
{
        int          mThreadToExecute;          /*!< Number of thread to execute. */
        char         mmc1DevName[MAX_STR_LEN];  /*!< MMC/SD 1 device name. */
        char         mmc2DevName[MAX_STR_LEN];  /*!< MMC/SD 2 device name. */
} sTestappConfig;

/*==================================================================================================
                                     FUNCTION PROTOTYPES
==================================================================================================*/
int VT_systest2_setup   ( void );
int VT_systest2_cleanup ( void );
int VT_systest2_test    ( void );

#endif /* __SYSTEST2_TEST_H__ */
