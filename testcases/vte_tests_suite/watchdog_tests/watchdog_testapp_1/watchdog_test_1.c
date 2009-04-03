/*================================================================================================*/
/**
        @file   watchdog_test_1.c

        @brief Test second watchdog capacity to reset the system in the given timeout
*/
/*==================================================================================================

        Copyright (C) 2004, Freescale Semiconductor, Inc. All Rights Reserved
        THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
        BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
        Freescale Semiconductor, Inc.

====================================================================================================
Revision History:
                            Modification     Tracking
Author/core ID                  Date          Number    Description of Changes
-------------------------   ------------    ----------  -------------------------------------------
V.Becker/rc023c              23/07/2004     TLSbo40889  Initial version 
S.V-Guilhou/svan01c          24/08/2005     TLSbo53364  Adapt test suite for MXC91131
V.Khalabuda/b00306           06/07/2006     TLSbo63552  Update for ArgonLV support

====================================================================================================
Portability:  ARM GCC
==================================================================================================*/

/*==================================================================================================
                                        INCLUDE FILES
==================================================================================================*/
/* Standard Include Files */
#include <errno.h>

/* Harness Specific Include Files. */
#include <test.h>

/* Verification Test Environment Include Files */
#include "watchdog_test_1.h"

/*==================================================================================================
                                        LOCAL MACROS
==================================================================================================*/

/*==================================================================================================
                            LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
==================================================================================================*/

/*==================================================================================================
                                        LOCAL CONSTANTS
==================================================================================================*/

/*==================================================================================================
                                        LOCAL VARIABLES
==================================================================================================*/
int     file_desc = 0;

/*==================================================================================================
                                        GLOBAL CONSTANTS
==================================================================================================*/

/*==================================================================================================
                                        GLOBAL VARIABLES
==================================================================================================*/

/*==================================================================================================
                                    LOCAL FUNCTION PROTOTYPES
==================================================================================================*/

/*==================================================================================================
                                        LOCAL FUNCTIONS
==================================================================================================*/

/*================================================================================================*/
/*===== VT_watchdog_test1_setup =====*/
/**
@brief  Assumes the pre-condition of the test case execution

@param  None
    
@return On success - return TPASS
        On failure - return the error code
*/
/*================================================================================================*/
int VT_watchdog_test1_setup(void)
{
        int     rv = TFAIL;

        /* Open Second watchdog test driver */
        file_desc = open(WATCHDOG_TEST, O_RDWR);
        sleep(1);
        if (file_desc == -1)
        {
                tst_resm(TFAIL, "ERROR : Open Second watchdog test driver fails : %d", errno);
                perror("Error: cannot open /dev/wd_tst device \n");
        }
        else
        {
                rv = TPASS;
        }

        return rv;
}

/*================================================================================================*/
/*===== VT_watchdog_test1_cleanup =====*/
/**
@brief  Assumes the post-condition of the test case execution

@param  None
    
@return On success - return TPASS
        On failure - return the error code
*/
/*================================================================================================*/
int VT_watchdog_test1_cleanup(void)
{
        int     rv = TFAIL;
        int     retval = 0;

        /* Close Second watchdog test driver */
        retval = close(file_desc);
        if (retval == -1)
        {
                tst_resm(TFAIL, "ERROR : Close Second watchdog test driver fails : %d", errno);
                perror("Error: cannot close /dev/wd_tst2 device \n");
        }
        else
        {
                rv = TPASS;
        }

        return rv;
}

/*================================================================================================*/
/*===== VT_watchdog_test1 =====*/
/**
@brief  Calls a test driver located in the kernel
        which disables the OS interrupts (particularly the OS ticks) in order to 
        trigger the second watchdog. Second watchodg will not be serviced anymore 
        and a system reset (watchdog reset source) should occur.

@param  
    
@return On success - return TPASS
        On failure - return the error code
*/
/*================================================================================================*/
int VT_watchdog_test1(int use_case)
{
        tst_resm(TINFO, "WATCHDOG TEST STARTING...");

        int     rv = TFAIL;
        int     retval = -EFAULT;

        /* Call ioctl to start watchdog test driver */
        tst_resm(TINFO, "Call ioctl to start watchdog test driver");
        retval = ioctl(file_desc, MXCTEST_WATCHDOG, &use_case);

        switch (retval)
        {
        case -1:
                tst_resm(TFAIL, "ERROR : ioctl MXCTEST_WATCHDOG fails: %d", errno);
                perror("ioctl");
                break;

        case 1:
                tst_resm(TFAIL, "ERROR : invalid WATCHDOG test case: %d", use_case);
                break;

        case 0:
                rv = TPASS;
                break;

        default:
                break;
        }

        return rv;
}
