/*================================================================================================*/
/**
        @file   watchdog_test_2.c

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
-------------------------   ------------    ----------  --------------------------------------------
V.Khalabuda/b00306           06/07/2006     TLSbo69495  Update for MX27 platform in accordance with changes in time.c file
Gamma Gao/b14842             12/24/2007                 Chang for MX37 platform(Kernel 2.6.22)
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
#include "watchdog_test_2.h"

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
int     fd = 0;

/*==================================================================================================
                                        GLOBAL CONSTANTS
==================================================================================================*/

/*==================================================================================================
                                        GLOBAL VARIABLES
==================================================================================================*/
extern int timeout, sleep_sec, test;

/*==================================================================================================
                                    LOCAL FUNCTION PROTOTYPES
==================================================================================================*/

/*==================================================================================================
                                        LOCAL FUNCTIONS
==================================================================================================*/

/*================================================================================================*/
/*===== VT_watchdog_test2_setup =====*/
/**
@brief  Assumes the pre-condition of the test case execution

@param  None
    
@return On success - return TPASS
        On failure - return the error code
*/
/*================================================================================================*/
int VT_watchdog_test2_setup(void)
{
        int     rv = TFAIL;

        /* Open watchdog misc test driver */
        fd = open(WATCHDOG_TEST, O_WRONLY);
        printf("sleep %d s after open\n",sleep_sec);
        sleep(sleep_sec);
        if (fd == -1)
        {
                tst_resm(TFAIL, "ERROR : Open watchdog misc test driver fails : %d", errno);
        }
        else
        {
                rv = TPASS;
        }

        return rv;
}

/*================================================================================================*/
/*===== VT_watchdog_test2_cleanup =====*/
/**
@brief  Assumes the post-condition of the test case execution

@param  None
    
@return On success - return TPASS
        On failure - return the error code
*/
/*================================================================================================*/
int VT_watchdog_test2_cleanup(void)
{
        int     rv = TFAIL;
        int     retval = 0;

        /* Close watchdog misc test driver */
        retval = close(fd);
        if (retval == -1)
        {
                tst_resm(TFAIL, "ERROR : Close watchdog misc test driver fails : %d", errno);
        }
        else
        {
                rv = TPASS;
        }

        return rv;
}

/*================================================================================================*/
/*===== VT_watchdog_test2 =====*/
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
int VT_watchdog_test2(void)
{
        tst_resm(TINFO, "WATCHDOG TEST STARTING...");

        int     rv = TFAIL;
        int     retval = -EFAULT;
	 int      i=1;
        int      writen=0;
        /* Call ioctl to start watchdog test driver */
        tst_resm(TINFO, "Trying to set timeout value=%d seconds", timeout);
        ioctl(fd, WDIOC_SETTIMEOUT, &timeout);
        tst_resm(TINFO, "The actual timeout was set to %d seconds", timeout);
        ioctl(fd, WDIOC_GETTIMEOUT, &timeout);
        tst_resm(TINFO, "Now reading back -- The timeout is %d seconds", timeout);
        /*Gamma Gao/b14842 Changed below:add counter "i" and some print info.*/
        while (1)
        {      if(((i++)%10)!=0) 
        	{
                if (test == 0)
                {
                        retval = ioctl(fd, WDIOC_KEEPALIVE, 0);
                        if (retval == -1)
                        {
                                tst_resm(TFAIL, "ERROR : ioctl MXCTEST_WATCHDOG fails: %d", errno);
                        }
                        else if (retval == 1)
                        {
                                tst_resm(TFAIL, "ERROR : invalid WATCHDOG test case: %d", test);
                        }
                        else if (retval == 0)
                        {
                                rv = TPASS;
		                  printf("ioctl MXCTEST_WATCHDOG passes\n");
                        }
                }
                else
                {
                        writen=write(fd, "\0", 1);
			   if(writen)
			   	{
			   	printf("write passes\n");
				rv = TPASS;
			   	}
                }
                printf("sleep %d s after ioctl or write\n",sleep_sec);
                sleep(sleep_sec);
        	}
		else
		 break;
        }

        return rv;
}
