/*================================================================================================*/
/**
    @file   rtc_test_2.h

    @brief  RTC test 2 header
*/
/*==================================================================================================

        Copyright (C) 2004, Freescale Semiconductor, Inc. All Rights Reserved
        THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
        BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
        Freescale Semiconductor, Inc.

====================================================================================================
Revision History:
                            Modification     Tracking
Author (core ID)                Date          Number    Description of Changes
-------------------------   ------------    ----------  -------------------------------------------
V. BECKER / rc023c       02/04/2004     TLSbo38652   Initial version
V. BECKER / rc023c       25/05/2004     TLSbo38652   Change file name
C. Gagneraud                 08/11/2004     TLSbo44474  Fix #include/warnings issues
L. DELASPRE / rc149c         08/12/2004     TLSbo40142   Update copyrights
L. DELASPRE / rc149c         13/12/2004     TLSbo45058   Update printed message
Rakesh S Joshi / r65956      25/07/2007     ENGR46185   Changed the dev entry

====================================================================================================
Portability:  ARM GCC  gnu compiler
==================================================================================================*/

#ifndef RTC_TEST2_H
#define RTC_TEST2_H

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
#define WAKEUP_ALARM_ENABLED 1
#define WAKEUP_ALARM_DISABLED 0
#define WKALM_PENDING 0
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
int VT_rtc_test2_setup();
int VT_rtc_test2_cleanup();

int VT_rtc_test2();

void cleanup();


#ifdef __cplusplus
}
#endif

#endif
