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
/*================================================================================================*/
/**
        @file   pmic_test_CA.h

        @brief  Concurrent access test scenario header file for PMIC Protocol dirver test appliaction
====================================================================================================
Revision History:
                            Modification     Tracking
Author/core ID                  Date          Number    Description of Changes
-------------------------   ------------    ----------  --------------------------------------------
D.Khoroshev/b00313           07/20/2005     TLSbo52700  Initial version
D.Khoroshev/b00313           09/05/2005     TLSbo52700  Rework version
D.Khoroshev/b00313           11/17/2005     TLSbo58274  Update for additional DTS document

==================================================================================================*/
#ifndef PMIC_TEST_CA_H
#define PMIC_TEST_CA_H

#ifdef __cplusplus
extern "C"{
#endif

/*==================================================================================================
                                         INCLUDE FILES
==================================================================================================*/

/*==================================================================================================
                                           CONSTANTS
==================================================================================================*/
#define pmic_read	 		0
#define pmic_write			1
#define pmic_subscribe		2
#define pmic_unsubscribe	3
/*==================================================================================================
                                       DEFINES AND MACROS
==================================================================================================*/

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
int VT_pmic_test_CA(int thread_num);



#ifdef __cplusplus
}
#endif

#endif        /* PMIC_TEST_CA_H */
