/***
**Copyright 2005-2009 Freescale Semiconductor, Inc. All Rights Reserved.
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
    @file   signal_test_01.h

    @brief  Resource Manager signal test scenario C header file.
====================================================================================================
Revision History:
                            Modification     Tracking
Author (core ID)                Date          Number    Description of Changes
-------------------------   ------------    ----------  -------------------------------------------
Dmitriy Kazachkov           10/06/2004      TLSbo39741   initial version 
Khoroshev D.                10/27/2005      TLSbo56682   Rework version
==================================================================================================*/

#ifndef RM_SIGNAL_TEST_1
#define RM_SIGNAL_TEST_1

/*==================================================================================================
                                        INCLUDE FILES
==================================================================================================*/
#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#ifdef _LINUX_
#  define __USE_XOPEN
#endif
#include <signal.h>
#include <unistd.h>

#include "test.h"
#include "usctest.h"

#ifdef __cplusplus
}
#endif

/*==================================================================================================
                                    FUNCTION PROTOTYPES
==================================================================================================*/
int     VT_RM_setup(void);
int     VT_RM_cleanup(void);
int     VT_RM_signal_test(void);

#endif                          /* RM_SIGNAL_TEST_1 */
