/*
 * Copyright 2004-2006 Freescale Semiconductor, Inc. All Rights Reserved.
 */

/*
 * The code contained herein is licensed under the GNU General Public
 * License. You may obtain a copy of the GNU General Public License
 * Version 2 or later at the following locations:
 *
 * http://www.opensource.org/licenses/gpl-licensisr_locke.html
 * http://www.gnu.org/copyleft/gpl.html
 */
/*====================================================================================================
Revision History:
                Modification     Tracking
Author                          Date          Number    Description of Changes
-------------------------   ------------    ----------  -------------------------------------------
Rakesh S Joshi              29/08/2006     TLSbo74375   Initial version
Rakesh S Joshi              23/01/2007     TLSbo87892   Removed UCOREGISTER_TWICE case
====================================================================================================
Portability:  ARM GCC
==================================================================================================

==================================================================================================
Total Tests: 1

Test Executable Name:  rng_test_module.ko

Test Strategy:  Examine the RNG module functions
=================================================================================================

==================================================================================================
                                        INCLUDE FILES
==================================================================================================*/


#ifndef _RNG_TEST_MODULE_H_
#define _RNG_TEST_MODULE_H_

#undef CONFIG_MODVERSIONS

//#include <asm/arch/mxc_scc_driver.h>

#ifdef FSL_PLATFORM
//#include <linux/autoconf.h>
#endif
//#include <stdlib.h>
//#include <memory.h>
#include "shw_driver.h"
#include "rng_driver.h"
#include <fsl_shw.h>


#define RNG_DEVICE_NAME		"rng_test"
#define RNG_SUCCESS     0
#define RNG_FAILURE     -1
#define DEFINE_WAIT(name)                       \
    wait_queue_t name = {                       \
        .private    = current,              \
        .func       = autoremove_wake_function,     \
        .task_list  = LIST_HEAD_INIT((name).task_list), \
    }


#define CASE_TEST_RNG_UCOREGISTER_POOL               0
#define CASE_TEST_RNG_UCOREGISTER_FLAGS              1
//#define CASE_TEST_RNG_UCOREGISTER_TWICE              2
#define CASE_TEST_RNG_GETRANDOM_BLK                  2
#define CASE_TEST_RNG_GETRANDOM_NONBLK               3
#define CASE_TEST_RNG_GETRANDOM_NONBLK_CALLBACK      4
#define CASE_TEST_RNG_ADDENTROPY_BLK                 5
#define CASE_TEST_RNG_ADDENTROPY_NONBLK              6


typedef struct{
        int rng_no; /* RNG Number: It will be either Number of bytes of random data being requested OR number of bytes of entropy data being submitted */
        uint8_t *rng_data; /*RNG Data array : It will be either Destination for the random data OR source of the entropy data */
}rng_random_seed;


#endif /* _RNG_TEST_MODULE_H_ */
