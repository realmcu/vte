/***
**Copyright (C) 2005-2009 Freescale Semiconductor, Inc. All Rights Reserved.
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
        @file   pmic_convity_test.h

        @brief  Header file for PMIC Connectivity driver test.
====================================================================================================
Revision History:
                            Modification     Tracking
Author/core ID                  Date          Number     Description of Changes
-------------------------   ------------    ----------   -------------------------------------------
V.Halabuda/hlbv001           16/08/2005     TLSbo52696   Initial version
V.Halabuda/hlbv001           02/09/2005     TLSbo58397   Update for linux-2.6.10-rel-L26_1_14
A.Ozerov/b00320              15/05/2006     TLSbo64237   Code was cast in accordance to coding conventions.
A.Ozerov/b00320              10/08/2006     TLSbo74269   Some changes.

==================================================================================================*/
#ifndef PMIC_CONVITY_TEST_H
#define PMIC_CONVITY_TEST_H

#ifdef __cplusplus
extern "C"{
#endif


/*==================================================================================================
                                        INCLUDE FILES
==================================================================================================*/
#if !defined(TRUE) && !defined(FALSE)
#define TRUE  1
#define FALSE 0
#endif

/* Standard Include Files */
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <errno.h>

/* Harness Specific Include Files. */
#include "test.h"
#include "usctest.h"

#include <linux/pmic_status.h>
#include <asm/arch-mxc/pmic_convity.h>
#include "pmic_convity_module.h"

/*==================================================================================================
                                            CONSTANTS
==================================================================================================*/

/*==================================================================================================
                                        DEFINES AND MACROS
==================================================================================================*/

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
int VT_pmic_convity_test_setup(void);
int VT_pmic_convity_test_cleanup(void);
int VT_pmic_convity_test(void);

#ifdef __cplusplus
}
#endif

#endif  /* PMIC_CONVITY_TEST_H */
