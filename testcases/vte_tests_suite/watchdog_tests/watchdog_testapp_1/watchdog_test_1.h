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
        @file   watchdog_test.h

        @brief  watchdog test header file
====================================================================================================
Revision History:
                            Modification     Tracking
Author/core ID                  Date          Number    Description of Changes
-------------------------   ------------    ----------  -------------------------------------------
V.Becker/rc023c              23/07/2004     TLSbo40889  Initial version
L.Delaspre/rc149c            02/08/2004     TLSbo40891  VTE 1.4 integration
L.Delaspre/rc149c            08/12/2004     TLSbo40142  update with Freescale identity
V.Khalabuda/b00306           06/07/2006     TLSbo63552  Update for ArgonLV support

==================================================================================================*/
#ifndef WATCHDOG_TEST_1_H
#define WATCHDOG_TEST_1_H

#ifdef __cplusplus
extern "C"{
#endif

/*==================================================================================================
                                        INCLUDE FILES
==================================================================================================*/
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include "watchdog_test.h"

/*==================================================================================================
                                            CONSTANTS
==================================================================================================*/

/*==================================================================================================
                                        DEFINES AND MACROS
==================================================================================================*/
#define WATCHDOG_TEST     "/dev/wd_tst"

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
int VT_watchdog_test1_setup(void);
int VT_watchdog_test1_cleanup(void);

int VT_watchdog_test1(int);


#ifdef __cplusplus
}
#endif

#endif        /* WATCHDOG_TEST_1_H */
