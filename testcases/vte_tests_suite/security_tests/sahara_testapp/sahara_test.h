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
        @file   sahara_test.h

        @brief  Header file for Securitty Sahara2 driver test scenario
====================================================================================================
Revision History:
                            Modification     Tracking
Author/Core ID                  Date          Number    Description of Changes
-------------------------   ------------    ----------  -------------------------------------------
A.Urusov/NONE                09/11/2005     TLSbo57926  Initial version
A.Ozerov/NONE                01/12/2005     TLSbo58662  Update for linux-2.6.10-rel-L26_1_14
A.Ozerov/NONE                19/12/2005     TLSbo58662  Update for linux-2.6.10-cvs-L26_1_15
D.Simakov/smkd001c           21/09/2006     TLSbo76069  Compilation error
                                                        
====================================================================================================
Portability:  ARM GCC 
==================================================================================================*/
#ifndef SAHARA_TEST_H
#define SAHARA_TEST_H

#ifdef __cplusplus
extern "C" {
#endif

/*==================================================================================================
                                        INCLUDE FILES
==================================================================================================*/
/* Standard Include Files */
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h> 
#include <errno.h>

/* Harness Specific Include Files. */
#include <test.h>
#include <usctest.h>
#include <linux/autoconf.h>

/* API's Specific Include Files. */
#include <fsl_shw.h>

/* Verification Test Environment Include Files */
#include <sahara_module.h>

/*==================================================================================================
                                     DEFINES AND MACROS 
==================================================================================================*/
#define POOL_SIZE 10

/*==================================================================================================
                                    FUNCTION PROTOTYPES
==================================================================================================*/
int     VT_sahara_test_setup(void);
int     VT_sahara_test_cleanup(void);
int     VT_sahara_test(SAHARA_TEST_IOCTL ctx_testcase);

#ifdef __cplusplus
}
#endif

#endif        /* SAHARA_TEST_H */
