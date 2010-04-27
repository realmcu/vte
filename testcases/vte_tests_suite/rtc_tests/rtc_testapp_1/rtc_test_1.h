/***
**Copyright (C) 2004-2009 Freescale Semiconductor, Inc. All Rights Reserved.
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
    @file   rtc_test_1.h

    @brief  RTC test 1 header
====================================================================================================
Revision History:
                            Modification     Tracking
Author (core ID)                Date          Number    Description of Changes
-------------------------   ------------    ----------  -------------------------------------------
V. BECKER / rc023c     01/04/2004     TLSbo38652   Initial version
V. BECKER / rc023c     25/05/2004     TLSbo38652   Change file name
C.Gagneraud cgag1c     08/11/2004     TLSbo44474   Fix #include issues.
Rakesh S Joshi / r65956      25/07/2007     ENGR46185   Changed the dev entry
====================================================================================================*/

#ifndef RTC_TEST1_H
#define RTC_TEST1_H

#ifdef __cplusplus
extern "C"{
#endif

/*==================================================================================================
                                         INCLUDE FILES
==================================================================================================*/
#include <time.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <linux/rtc.h>
#include <sys/ioctl.h>

/*==================================================================================================
                                           CONSTANTS
==================================================================================================*/


/*==================================================================================================
                                       DEFINES AND MACROS
==================================================================================================*/
// Victor: /dev/rtc will link to the true device
//#define RTC_DRIVER_NAME "/dev/rtc0"
#define RTC_DRIVER_NAME "/dev/rtc"

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
int VT_rtc_test1_setup();
int VT_rtc_test1_cleanup();

int VT_rtc_test1(void);



#ifdef __cplusplus
}
#endif

#endif
