/***
**Copyright (C) 2005-2009 Freescale Semiconductor, Inc. All Rights Reserved.
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
        @file   pmic_test_light.h

        @brief  Header file for PMIC (mc13783 and sc55112) Light driver test scenario.
====================================================================================================
Revision History:
                            Modification     Tracking
Author/core ID                  Date          Number    Description of Changes
-------------------------   ------------    ----------  -------------------------------------------
D.Khoroshev/b00313           07/25/2005     TLSbo52699   Initial version
D.Khoroshev/b00313           08/29/2005     TLSbo52699   Rework version
I.Inkina/nknl001             28/12/2005     TLSbo61037   Update for MXC91231 and MXC91131
D.Khoroshev/b00313           07/25/2005     TLSbo66285   Update for VTE 2.01
D.Khoroshev/b00313           08/31/2006     TLSbo76979   Added support for both SC55112 and MC13783
                                                         platforms
==================================================================================================*/
#ifndef SC55112_TEST_LIGHT_H
#define SC55112_TEST_LIGHT_H

#ifdef __cplusplus
extern "C"{
#endif


#ifndef true
#define true 1
#endif

#ifndef false
#define false 0
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
#include <linux/wait.h>
#include <linux/pmic_status.h>
#include <linux/pmic_light.h>

/*==================================================================================================
                                        DEFINES AND MACROS
==================================================================================================*/
#define    MAX_NUMBER_LEVEL      6
#define    MAX_DUTY_CYCLE        15
#define    MAX_TIME_CYCLE        3
#define    MAX_BOOST_ABMS        7
#define    MAX_BOOST_ABR         3
#define    PMIC_LIGHT_DEV        "pmic_light"

#define    PMIC_LIGHT_TEST_TCLED_IOCTL                  0
#define    PMIC_LIGHT_TEST_TCLED_CONFIG_FUN_PATTERN     1
#define    PMIC_LIGHT_TEST_TCLED_CONFIG_FUN_TEST        2
#define    PMIC_LIGHT_TEST_TCLED_CONFIG_IND_MODE        3
#define    PMIC_LIGHT_TEST_BKLIT_IOCTL                  4
#define    PMIC_LIGHT_TEST_BKLIT_RAMP_CONFIG             5

/*!
 * @struct t_tcled_enable_param
 * @brief enable setting.
typedef struct {
    t_funlight_bank bank;*/   /*!< Bank */
 //   t_tcled_mode mode;  /*!< Mode */
/*} t_tcled_enable_param;
 */

/*==================================================================================================
                                    FUNCTION PROTOTYPES
==================================================================================================*/
int VT_pmic_light_test_setup(void);
void VT_pmic_light_test_cleanup(void);
int VT_pmic_light_test(int switch_fct);

#ifdef __cplusplus
}
#endif

#endif        /* SC55112_TEST_LIGHT_H */
