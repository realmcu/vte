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
    @file   hacc_test_1.h

    @brief  hacc API test header file
====================================================================================================
Revision History:
                            Modification     Tracking
Author (core ID)                Date          Number    Description of Changes
-------------------------   ------------    ----------  -------------------------------------------
S.ZAVJALOV/zvjs001c          10/08/2004     TLSbo40418   Initial version
S.ZAVJALOV/zvjs001c          01/10/2004     TLSbo40649   Version after inspection
S.ZAVJALOV/zvjs001c          04/11/2004     TLSbo43890   Chages for virt_to_phys() function
S.ZAVJALOV/zvjs001c          04/07/2005     TLSbo51629   Change hacc test strategy
A.URUSOV                     20/10/2005     TLSbo57061   New defines are added
==================================================================================================*/

#ifndef _HACC_TEST_MODULE_H_
#define _HACC_TEST_MODULE_H_

#ifdef __cplusplus
extern "C" {
#endif

/*==================================================================================================
                                 STRUCTURES AND OTHER TYPEDEFS
==================================================================================================*/
typedef struct
{
        unsigned long start_hacc_addr;
        int     data2hash_len;
        int     verbose_mode;
        short int stop_flag;
} hacc_test_param;


/*==================================================================================================
                                       DEFINES AND MACROS
==================================================================================================*/

#define HACC_BLOCK_SIZE		0x10
#define HACC_DEVICE_NAME	"hacc_test_module"
#define CASE_TEST_HACC_01	1
#define CASE_TEST_HACC_02       2
#define CASE_TEST_HACC_03	3
#define CASE_TEST_HACC_04       4
#define CASE_TEST_HACC_05       5

#ifdef CONFIG_PM
#define CASE_TEST_HACC_06       6
#define CASE_TEST_HACC_07       7
#endif                          /* CONFIG_PM */

#ifdef __cplusplus
}
#endif

#endif                          /* _HACC_TEST_MODULE_H_ */
