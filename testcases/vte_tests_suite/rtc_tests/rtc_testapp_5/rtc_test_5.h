/*================================================================================================*/
/**
    @file   rtc_test_5.h

    @brief  RTC test 5 header
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
Rakesh S Joshi / r65956      25/07/2007		ENGR46185	Changed the dev entry

====================================================================================================
Portability:  ARM GCC  gnu compiler
==================================================================================================*/

#ifndef RTC_TEST5_H
#define RTC_TEST5_H

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
int VT_rtc_test5_setup();
int VT_rtc_test5_cleanup();

int VT_rtc_test5(int switch_test);

void cleanup();


#ifdef __cplusplus
}
#endif

#endif
