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
    @file   rtic_test_module.h

    @brief  rtic module API test header file

====================================================================================================
Revision History:
                            Modification     Tracking
Author (core ID)                Date          Number    Description of Changes
-------------------------   ------------    ----------  -------------------------------------------
S.ZAVJALOV/zvjs001c          19/10/2004     TLSbo43475   Initial version
A.URUSOV                     13/09/2005     TLSbo55076   Fix compilation issue and warnings
A.URUSOV                     01/11/2005     TLSbo57063   Compile under L26.1.14
==================================================================================================*/

#ifndef _RTIC_TEST_MODULE_H_
#define _RTIC_TEST_MODULE_H_

/*==================================================================================================
                                 STRUCTURES AND OTHER TYPEDEFS
==================================================================================================*/

typedef struct
{
        int     verbose_mode;
        unsigned long *data;
        int     data_length;
        int     block_memory;
        int     interrupt;
} rtic_test_param;

/*==================================================================================================
                                       DEFINES AND MACROS
==================================================================================================*/

#define RTIC_DEVICE_NAME		"rtic_test_module"

#define CASE_TEST_RTIC_ONETIME		1
#define CASE_TEST_RTIC_RUNTIME		2
#define CASE_TEST_RTIC_GET_CONTROL      3
#define CASE_TEST_RTIC_GET_FAULTADDRESS 4

#endif                          /* _RTIC_TEST_MODULE_H_ */
