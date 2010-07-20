/***
**Copyright (C) 2006-2009 Freescale Semiconductor, Inc. All Rights Reserved.
**
**The code contained herein is licensed under the GNU General Public
**License. You may obtain a copy of the GNU General Public License
**Version 2 or later at the following locations:
**
**http://www.opensource.org/licenses/gpl-license.html
**http://www.gnu.org/copyleft/gpl.html
**/
/*================================================================================================

        @file   rng_test_module.c

        @brief  rng API

====================================================================================================
Revision History:
                Modification     Tracking
Author                          Date          Number    Description of Changes
-------------------------   ------------    ----------  -------------------------------------------
Rakesh S Joshi              29/08/2006     TLSbo74375   Initial version
====================================================================================================
Portability:  ARM GCC
==================================================================================================

==================================================================================================*/
#ifndef RNG_TEST_H
#define RNG_TEST_H

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
#include <rng_test_module.h>

#include "test.h"
#include "usctest.h"


/*==================================================================================================
                                       DEFINES AND MACROS
==================================================================================================*/

/*!
 * These values are returned for RNGA routines.
 */
/*!
 * Successfully finished the routine.
 */
#define RNG_SUCCESS 0

/*!
 * Encountered some errors in the routine.
 */
#define RNG_FAILURE -1

/*==================================================================================================
                                     FUNCTION PROTOTYPES
==================================================================================================*/

void help(void);

int VT_rng_test_setup();
int VT_rng_test_cleanup();

int VT_rng_test(void);

#ifdef __cplusplus
}
#endif

#endif  /* RNGA_TEST_H */
