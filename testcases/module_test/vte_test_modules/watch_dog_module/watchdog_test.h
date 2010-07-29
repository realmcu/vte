/***
**Copyright 2006-2009 Freescale Semiconductor, Inc. All Rights Reserved.
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
        @file watchdog_test.h

        @brief ioctl related header file for watchdog test on MXC platform

        @ingroup  watchdog
====================================================================================================
Revision History:
                            Modification     Tracking
Author/core ID                  Date          Number    Description of Changes
-------------------------   ------------    ----------  -------------------------------------------
V.Khalabuda/b00306           06/07/2006     TLSbo63552  Update for ArgonLV support

====================================================================================================
Portability:  ARM GCC
==================================================================================================*/
#ifndef MXC_WDOG_TEST_H
#define MXC_WDOG_TEST_H

#ifdef __cplusplus
extern "C"{
#endif

/*==================================================================================================
                                            CONSTANTS
==================================================================================================*/
/*! Iommand: MXCTEST_WATCHDOG Tests one or both of the watchdogs */

/*! Command for disabling OS interrupts */
#define MXCTEST_WATCHDOG  0x10

/*==================================================================================================
                                        DEFINES AND MACROS
==================================================================================================*/

/*==================================================================================================
                                            ENUMS
==================================================================================================*/
enum TEST_MODULE_MAJOR_NUM
{
        MXC_TEST_MODULE_MAJOR = 300,
        MXC_WDOG_TM_MAJOR,
};

/*==================================================================================================
                                STRUCTURES AND OTHER TYPEDEFS
==================================================================================================*/

/*==================================================================================================
                                GLOBAL VARIABLE DECLARATIONS
==================================================================================================*/

/*==================================================================================================
                                    FUNCTION PROTOTYPES
==================================================================================================*/
#ifdef __cplusplus
}
#endif

#endif        /* MXC_WDOG_TEST_H */
