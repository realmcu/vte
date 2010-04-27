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
/*================================================================================================*/
/**
        @file   pmic_battery_test.h

        @brief  Header file for PMIC Battery driver test scenario.
====================================================================================================
Revision History:
                            Modification     Tracking
Author/core ID                  Date          Number    Description of Changes
-------------------------   ------------    ----------  -------------------------------------------
A.Ozerov/NONE                18/01/2006     TLSbo61037  Initial version
A.Ozerov/b00320              12/07/2006     TLSbo64238  Changes for L26_1_19 release.
Pradeep K/b01016             09/25/2006     TLSboXXXX   Updated for PMIC API's

====================================================================================================
Portability: ARM GCC
==================================================================================================*/
#ifndef PMIC_TEST_BATTERY_H
#define PMIC_TEST_BATTERY_H

#ifdef __cplusplus
extern "C"{
#endif


/*==================================================================================================
                                        DEFINES AND MACROS
==================================================================================================*/
#define        CONFIG_PARAM_NUMBER 15
#define        BAT_THRESHOLD_MAX 3

//#ifdef CONFIG_MXC_PMIC_SC55112
//#ifdef CONFIG_MXC_MC13783_PMIC
#define        PMIC_BATTERY_DEV "pmic_battery"
///*#else
//#define        PMIC_BATTERY_DEV "mc13783_battery"
//#endif
//*/
//#ifndef __KERNEL__
//#define __KERNEL__
//#endif


//#ifndef bool
//#define bool int
//#endif

//#ifdef __KERNEL__
//#undef __KERNEL__
//#endif

#if !defined(TRUE) && !defined(FALSE)
#define TRUE  1
#define FALSE 0
#endif

/*==================================================================================================
                                        INCLUDE FILES
==================================================================================================*/
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
//#include <linux/wait.h>
#include <linux/autoconf.h>

/* API's Include Files */
#include <linux/pmic_status.h>

//#ifdef CONFIG_MXC_PMIC_SC55112
//#ifdef CONFIG_MXC_MC13783_PMIC
#include <linux/pmic_battery.h>
/*#else
#include <mc13783_battery.h>
#endif
*/
/*==================================================================================================
                                    FUNCTION PROTOTYPES
==================================================================================================*/
int VT_pmic_battery_test_cleanup(void);
int VT_pmic_battery_test_setup(void);
int VT_pmic_battery_test(int switch_fct, FILE *file);

#ifdef __cplusplus
}
#endif

#endif        /* PMIC_TEST_BATTERY_H */
