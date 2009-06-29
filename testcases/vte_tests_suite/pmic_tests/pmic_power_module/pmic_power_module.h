/***
**Copyright 2005-2009 Freescale Semiconductor, Inc. All Rights Reserved.
**
**The code contained herein is licensed under the GNU General Public
**License. You may obtain a copy of the GNU General Public License
**Version 2 or later at the following locations:
**
**http://www.opensource.org/licenses/gpl-license.html
**http://www.gnu.org/copyleft/gpl.html
**/
/*!
 * @file   pmic_power_module.h
 * @brief  power_module test header file
 */

#ifndef MC13783_POWER_TEST_H
#define MC13783_POWER_TEST_H

#ifdef __cplusplus
extern "C" {
#endif


/*==============================================================================
                                           CONSTANTS
==============================================================================*/

/*==============================================================================
                                       DEFINES AND MACROS
==============================================================================*/
#define TPASS    0		/* Test passed flag */
#define TFAIL    1		/* Test failed flag */
#define TBROK    2		/* Test broken flag */
#define TWARN    4		/* Test warning flag */
#define TRETR    8		/* Test retire flag */
#define TINFO    16		/* Test information flag */
#define TCONF    32		/* Test not appropriate for configuration flag */

/* Test IOCTLS */

#ifdef CONFIG_MXC_PMIC_MC13783
#define PMIC_POWER_DEV "mc13783_power"
#else
#define PMIC_POWER_DEV "rr_power"
#endif

/*==============================================================================
                                             ENUMS
==============================================================================*/
	enum PMIC_POWER_TEST_IOCTL {
		PMIC_POWER_TEST_CONTROL = 1,
		PMIC_POWER_TEST_VOLTAGE = 2,
		PMIC_POWER_TEST_CONFIG = 3,
		PMIC_POWER_TEST_MISC = 4,
	};
/*==============================================================================
                                 STRUCTURES AND OTHER TYPEDEFS
==============================================================================*/

/*==============================================================================
                                 GLOBAL VARIABLE DECLARATIONS
==============================================================================*/

/*==============================================================================
                                     FUNCTION PROTOTYPES
==============================================================================*/

	int UT_rr_power_test_control(void);
	int UT_rr_power_test_voltage(void);
	int UT_rr_power_test_config(void);

#ifdef __cplusplus
}
#endif
#endif				/* MXC_TEST_H */
