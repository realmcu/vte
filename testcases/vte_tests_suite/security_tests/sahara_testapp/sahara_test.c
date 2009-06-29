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
        @file   sahara_test.c

        @brief  Securitty Sahara2 driver test scenario
====================================================================================================
Revision History:
                            Modification     Tracking
Author/Core ID                  Date          Number    Description of Changes
-------------------------   ------------    ----------  -------------------------------------------
A.Ozerov/NONE                10/10/2005     TLSbo55834  Initial version
A.Urusov/NONE                09/11/2005     TLSbo57926  New tests are added
A.Urusov/NONE                18/11/2005     TLSbo58663  Segmentation failed error is fixed
A.Ozerov/NONE                01/12/2005     TLSbo58662  Update for linux-2.6.10-rel-L26_1_14
A.Ozerov/NONE                19/12/2005     TLSbo58662  Update for linux-2.6.10-cvs-L26_1_15
A.Ozerov/NONE                31/01/2006     TLSbo61952  Problem with fsl_shw_register_user was fixed

====================================================================================================
Portability:  ARM GCC 
==================================================================================================*/

/*==================================================================================================
                                        INCLUDE FILES
==================================================================================================*/
/* Verification Test Environment Include Files */
#include "sahara_test.h"

/*================================================================================================*/

/*==================================================================================================
                                        LOCAL VARIABLES
==================================================================================================*/
static int fd = 0;
static fsl_shw_uco_t ctx_config;

/*================================================================================================*/
static char *IOCTLS[]=
{
        "SAHARA_TEST_RUN_CALLBACK",
        "SAHARA_TEST_RUN_HASH",
        "SAHARA_TEST_RUN_HMAC1",
        "SAHARA_TEST_RUN_HMAC2",
        "SAHARA_TEST_RUN_RESULT",
        "SAHARA_TEST_RUN_SYMMETRIC",
        "SAHARA_TEST_RUN_WRAP",
        "SAHARA_TEST_RUN_RANDOM",
        "SAHARA_TEST_SHOW_CAPABILITIES",
        "SAHARA_TEST_RUN_ENCRYPT_DECRYPT",
        "SAHARA_TEST_REGISTER_USER",
        "SAHARA_TEST_DEREGISTER_USER",
};

/*================================================================================================*/
/*===== VT_sahara_test_setup =====*/
/** 
@brief  This function assumes the pre-condition of the test case execution

@param  none

@return On success - return TPASS 
        On failure - return TFAIL 
*/
/*================================================================================================*/
int VT_sahara_test_setup(void)
{
        char    strdev[256]="/dev/";
        strcat(strdev, SAHARA_TEST_DEVICE);

        if ((fd = open(strdev, O_RDWR)) < 0)
        {
                tst_resm(TFAIL, "VT_sahara_test_setup() Failed open device");
                return TFAIL;
        }
        tst_resm(TINFO, "VT_sahara_test_setup(): Complete successful");

        return TPASS;
}

/*================================================================================================*/
/*===== VT_sahara_test_cleanup =====*/
/** 
@brief  This function assumes the post-condition of the test case execution

@param  none

@return On success - return TPASS 
        On failure - return TFAIL 
*/
/*================================================================================================*/
int VT_sahara_test_cleanup(void)
{
        if (fd > 0)
                close (fd);

        return TPASS;
}

/*================================================================================================*/
/*===== VT_sahara_test =====*/
/** 
@brief  Sahara2 Driver test scenario

@param  none

@return On success - return TPASS 
        On failure - return the error code 
*/
/*================================================================================================*/
int VT_sahara_test(SAHARA_TEST_IOCTL ctx_testcase)
{
        int     VT_rv = TPASS;

        memset(&ctx_config, 0, sizeof(fsl_shw_uco_t));

        ctx_config.pool_size = POOL_SIZE;
        ctx_config.flags = FSL_UCO_BLOCKING_MODE;
        ctx_config.sahara_openfd = -1;
        ctx_config.mem_util = NULL;
        ctx_config.callback = NULL;
        
        if (ioctl(fd, SAHARA_TEST_REGISTER_USER, &ctx_config) != 0)
        {
                VT_rv = TFAIL;
                tst_resm(VT_rv, "Failed registry user");
        }

        tst_resm(TINFO, "Sahara2 operation - %s", IOCTLS[ctx_testcase]);
        if (ioctl(fd, ctx_testcase, &ctx_config) != 0)
        {
                VT_rv = TFAIL;
                tst_resm(VT_rv, "Failed Sahara2 operation, ERROR CODE is %d", errno);
        }

        if (ioctl(fd, SAHARA_TEST_DEREGISTER_USER, &ctx_config) != 0)
        {
                VT_rv = TFAIL;
                tst_resm(VT_rv, "Failed deregistry user");
        }

        return VT_rv;
}
