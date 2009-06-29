/***
**Copyright 2007-2009 Freescale Semiconductor, Inc. All Rights Reserved.
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
        @file   watchdog_test_3.c

        @brief Test second watchdog capacity to reset the system in the given timeout
====================================================================================================
Revision History:
                            Modification     Tracking
Author/core ID                  Date          Number    Description of Changes
-------------------------   ------------    ----------  --------------------------------------------
Gamma Gao/b14842           12/24/2007                         Added for  MX37 platform
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
#include "watchdog_test_3.h"

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
extern struct watchdog_info ident;
/*==================================================================================================
                                    LOCAL FUNCTION PROTOTYPES
==================================================================================================*/

/*==================================================================================================
                                        LOCAL FUNCTIONS
==================================================================================================*/

/*================================================================================================*/
/*===== VT_watchdog_test3_setup =====*/
/**
@brief  Assumes the pre-condition of the test case execution

@param  None
    
@return On success - return TPASS
        On failure - return the error code
*/
/*================================================================================================*/
int VT_watchdog_test3_setup(void)
{
        int     rv = TFAIL;

        /* Open watchdog misc test driver */
        fd = open(WATCHDOG_TEST, O_WRONLY);
        sleep(1);
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
/*===== VT_watchdog_test3_cleanup =====*/
/**
@brief  Assumes the post-condition of the test case execution

@param  None
    
@return On success - return TPASS
        On failure - return the error code
*/
/*================================================================================================*/
int VT_watchdog_test3_cleanup(void)
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
/*===== VT_watchdog_test3 =====*/
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
int VT_watchdog_test3(void)
{
        tst_resm(TINFO, "WATCHDOG TEST STARTING...");
        int    status=0;
	 int    bootreason=0;
        int     rv = TFAIL;
        int     retval = -EFAULT;
	int time=50;
	  /* Call ioctl to start watchdog test driver */
        tst_resm(TINFO, "Trying to set timeout value=%d seconds", time);
        ioctl(fd, WDIOC_SETTIMEOUT, &time);
        tst_resm(TINFO, "The actual timeout was set to %d seconds", time);
        ioctl(fd, WDIOC_GETTIMEOUT, &time);
        tst_resm(TINFO, "Now reading back -- The timeout is %d seconds", time);
       if (test == 0) 
       	{  
            /*Test Ioctl:WDIOC_GETSUPPORT*/
	    printf("Test Ioctl:WDIOC_GETSUPPORT...\n");
           retval=ioctl(fd, WDIOC_GETSUPPORT, &ident);
	    if(retval == 0)
	    	{
	    	printf("watchdog_info:\nwatchdog_info.identity=%s\nwatchdog_info.options=%d\nwatchdog_info.firmware_version=%d\n",ident.identity,ident.options,ident.firmware_version);
	    	printf("WDIOC_GETSUPPORT passes\n");
		rv = TPASS;
	    	}
		else
			{
			printf("ERROR :WDIOC_GETSUPPORT fails\n");
			}
	   }
	  /*Test Ioctl:WDIOC_GETSTATUS*/
	   else if(test == 1)
	   {  
	    printf("Test Ioctl:WDIOC_GETSTATUS...\n");
           retval=ioctl(fd, WDIOC_GETSTATUS, &status);	    
	    if(retval == 0)
	    	{
	    	printf("status is %d\n",status);
	    	printf("WDIOC_GETSTATUS passes\n");
		rv = TPASS;
	    	}
		else
			{
			printf("WDIOC_GETSTATUS fails\n");
			}
	   }
	   /*Test Ioctl:WDIOC_GETBOOTSTATUS*/
	   else
	   	{ 
	    printf("Test Ioctl:WDIOC_GETBOOTSTATUS...\n");
           retval=ioctl(fd, WDIOC_GETBOOTSTATUS, &bootreason); 
	    if(retval == 0)
	    	{
	    	printf("bootreason is %d\n",bootreason);
	    	printf("WDIOC_GETBOOTSTATUS passes\n");
		rv = TPASS;
	    	}
		else
			{
			printf("WDIOC_GETBOOTSTATUS fails\n");		
			}
	   }
        return rv;
}
