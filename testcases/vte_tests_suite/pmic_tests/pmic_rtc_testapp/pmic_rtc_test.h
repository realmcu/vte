/*================================================================================================*/
/**
        @file   pmic_rtc_test.h

        @brief  Header file of PMIC RTC driver test.
*/
/*==================================================================================================

        Copyright (C) 2006, Freescale Semiconductor, Inc. All Rights Reserved
        THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
        BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
        Freescale Semiconductor, Inc.

====================================================================================================
Revision History:
                            Modification     Tracking
Author/Core ID                  Date          Number    Description of Changes
-------------------------   ------------    ----------  -------------------------------------------
E.Gromazina/NONE             12/08/2005     TLSbo59968  PMIC RTC draft version.
A.Ozerov/b00320              06/07/2006     TLSbo61903  Test was changed in accordance to driver changes.
                                                        PMIC_DEVICE_RTC was changed.
Pradeep K/b01016             09/25/2006     TLSboXXXX   Updated to support PMIC API's
====================================================================================================
Portability: ARM GCC
==================================================================================================*/
#ifndef PMIC_TEST_RTC_H
#define PMIC_TEST_RTC_H

#ifdef __cplusplus
extern "C"{
#endif

#if !defined(TRUE) && !defined(FALSE)
#define TRUE  1
#define FALSE 0
#endif

/*==================================================================================================
                                        INCLUDE FILES
==================================================================================================*/
#include <sys/types.h>    /* open() */
#include <sys/stat.h>     /* open() */
#include <fcntl.h>        /* open() */
#include <sys/ioctl.h>    /* ioctl() */
#include <unistd.h>       /* close() */
#include <stdio.h>        /* sscanf() & perror() */
#include <stdlib.h>       /* atoi() */
#include <errno.h>
#include <linux/wait.h>
#include <linux/autoconf.h>
//#define __KERNEL__
//#include <asm/hardware.h>
//#undef __KERNEL__
//#include <mc13783_rtc.h>

#include <linux/pmic_rtc.h>


/*==================================================================================================
                                        DEFINES AND MACROS
==================================================================================================*/
//#define    PMIC_DEVICE_RTC   "/dev/mc13783_rtc"
#define    PMIC_DEVICE_RTC   "/dev/pmic_rtc"

/*==================================================================================================
                                    FUNCTION PROTOTYPES
==================================================================================================*/
int VT_pmic_rtc_test_setup(void);
int VT_pmic_rtc_test_cleanup(void);
int VT_pmic_rtc_test(int switch_fct);

#ifdef __cplusplus
}
#endif

#endif  /* PMIC_TEST_RTC_H */
