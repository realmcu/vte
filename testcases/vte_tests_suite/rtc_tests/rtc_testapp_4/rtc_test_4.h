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
    @file   rtc_test_4.h

    @brief  RTC test 4 header
====================================================================================================
Revision History:
                            Modification     Tracking
Author (core ID)                Date          Number    Description of Changes
-------------------------   ------------    ----------  -------------------------------------------
V. BECKER / rc023c           08/04/2004     TLSbo38652  Initial version
V. BECKER / rc023c           25/05/2004     TLSbo38652  Change file name
L. DELASPRE/rc149c           23/06/2004     TLSbo39941   VTE 1.3 integration
C. Gagneraud                 08/11/2004     TLSbo44474  Fix #include/warnings issues
Rakesh S Joshi / r65956      25/07/2007     ENGR46185   Changed the dev entry
====================================================================================================*/

#ifndef RTC_TEST4_H
#define RTC_TEST4_H

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
#define BAD_FREQUENCY 65
#define BAD_FREQUENCY2 1024
#define WAKEUP_ALARM_ENABLED 1
#define WKALM_NOT_PENDING 1

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
void cleanup();

int VT_rtc_test4_setup();
int VT_rtc_test4_cleanup();

int VT_rtc_test4(void);



#ifdef __cplusplus
}
#endif

#endif
