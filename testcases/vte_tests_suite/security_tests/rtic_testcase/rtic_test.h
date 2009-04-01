/*================================================================================================*/
/**
    @file   rtic_test.h

    @brief  rtic API test header file
*/
/*==================================================================================================

        Copyright 2004, Freescale Semiconductor, Inc. All Rights Reserved
        THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
        BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
        Freescale Semiconductor, Inc.

====================================================================================================
Revision History:
                            Modification     Tracking
Author (core ID)                Date          Number    Description of Changes
-------------------------   ------------    ----------  -------------------------------------------
S.ZAVJALOV/zvjs001c          19/10/2004     TLSbo43475   Initial version
A.URUSOV                     13/09/2005     TLSbo55076   Fix compilation issue and warnings
A.URUSOV                     01/11/2005     TLSbo57063   Compile under L26.1.14
==================================================================================================*/

#ifndef RTIC_TEST_H
#define RTIC_TEST_H

#ifdef __cplusplus
extern "C"{
#endif

/*==================================================================================================
                                         INCLUDE FILES
==================================================================================================*/

#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#include "test.h"
#include "usctest.h"

#include "rtic_test_module.h"

/*==================================================================================================
                                             ENUMS
==================================================================================================*/
typedef enum {
        /*!
         * Select Memory Block A1.
         */
        RTIC_A1,

        /*!
         * Select Memory Block A2.
         */
        RTIC_A2,

        /*!
         * Select Memory Block B1.
         */
        RTIC_B1,

        /*!
         * Select Memory Block B2.
         */
        RTIC_B2,

        /*!
         * Select Memory Block C1.
         */
        RTIC_C1,

        /*!
         * Select Memory Block C2.
         */
        RTIC_C2,

        /*!
         * Select Memory Block D1.
         */
        RTIC_D1,

        /*!
         * Select Memory Block D2.
         */
        RTIC_D2
} rtic_memblk;

/*==================================================================================================
                                     FUNCTION PROTOTYPES
==================================================================================================*/

void help(void);

int VT_rtic_test_setup();
int VT_rtic_test_cleanup();

int VT_rtic_test(void);

#ifdef __cplusplus
}
#endif

#endif  /* RTIC_TEST_H */
