/*================================================================================================*/
/**
        @file   pmic_rtc_module.h

        @brief  PMIC RTC driver module header file
*/
/*==================================================================================================

        Copyright (C) 2006, Freescale Semiconductor, Inc. All Rights Reserved
        THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
        BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
        reescale Semiconductor, Inc.

====================================================================================================
Revision History:
                            Modification     Tracking
Author/core ID                  Date          Number    Description of Changes
-------------------------   ------------    ----------  -------------------------------------------
Pradeep K / b01016           09/25/2006      TLSboxxx     Initial version

====================================================================================================
Portability:  ARM GCC
==================================================================================================*/
#ifndef PMIC_RTC_TEST_H
#define PMIC_RTC_TEST_H

#include <linux/autoconf.h>
#include <linux/pmic_status.h>
#include <linux/pmic_rtc.h>

//#if defined( CONFIG_MXC_PMIC_SC55112 ) || defined( CONFIG_MXC_MC13783_PMIC )
#include <linux/pmic_external.h>
/*#elif defined( CONFIG_MXC_MC13783_LEGACY )
#include <mc13783_external.h>
#endif
*/

#ifdef __cplusplus
extern "C"{
#endif



/*==================================================================================================
                CONSTANTS
==================================================================================================*/

/*==================================================================================================
                DEFINES AND MACROS
==================================================================================================*/
#define TPASS    0    /* Test passed flag */
#define TFAIL    1    /* Test failed flag */
#define TBROK    2    /* Test broken flag */
#define TWARN    4    /* Test warning flag */
#define TRETR    8    /* Test retire flag */
#define TINFO    16   /* Test information flag */
#define TCONF    32   /* Test not appropriate for configuration flag */

#define PMIC_RTC_DEV "test_pmic_rtc"


/*==================================================================================================
        GLOBAL VARIABLE DECLARATIONS
==================================================================================================*/

/*==================================================================================================
        FUNCTION PROTOTYPES
==================================================================================================*/

#ifdef __cplusplus
}
#endif

#endif        /* PMIC_RTC_TEST_H */
