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
    @file   mu_api_err_main.c

    @brief  Main file of the mu_api_err_test application that generates variuos errors
            and checks proper Messaging Unit driver reactions.
====================================================================================================
Revision History:
                              Modification     Tracking
Author (Core ID)                  Date          Number    Description of Changes
---------------------------   ------------    ----------  ------------------------------------------
Igor Semenchukov (smng001c)    25/08/2004     TLSbo40411   Initial version 
Igor Semenchukov (smng001c)    09/12/2004     TLSbo43804   Some cosmetic changes

====================================================================================================
Portability: Indicate if this module is portable to other compilers or platforms. 
             If not, indicate specific reasons why is it not portable.

==================================================================================================*/

/*==================================================================================================
Total Tests:    6

Test Name:      mu_api_err_test

Test Assertion
& Strategy:     This test performs Messaging Unit driver system calls with erroneous parameters
                and checks driver's adequate reaction. There are following attempts to test:
                    1) open invalid device file (it will be created before this);
                    2) open the same device file two times at once. Since device file
                       opening allocates a particular channel, the device must be busy;
                    3) write message that is non-aligned to the register size;
                    4) read message that is non-aligned to the register size;
                    5) write big amount of bytes;
                    6) call ioctl() with invalid command.
                The following Messaging Unit driver functions are tested:
                    mxc_mu_open()
                    mxc_mu_close()
                    mxc_mu_read()
                    mxc_mu_write()
                    mxc_mu_ioctl()
==================================================================================================*/

#ifdef __cplusplus
extern "C"{
#endif

/*==================================================================================================
                                        INCLUDE FILES
==================================================================================================*/

/* Standard Include Files */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

/* Harness Specific Include Files. */

#include "test.h"
#include "usctest.h"

/* Verification Test Environment Include Files */

#include "mu_api_err_test.h"

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


/*==================================================================================================
                                       GLOBAL CONSTANTS
==================================================================================================*/


/*==================================================================================================
                                       GLOBAL VARIABLES
==================================================================================================*/

/* Extern Global Variables */

extern int  Tst_count;                /* counter for tst_xxx routines.         */
extern char *TESTDIR;                 /* temporary dir created by tst_tmpdir() */

/* Global Variables */

char *TCID     = "mu_api_err_test";   /* test program identifier               */
int  TST_TOTAL = 6;                   /* total number of tests in this file    */

/*==================================================================================================
                                   GLOBAL FUNCTION PROTOTYPES
==================================================================================================*/
void cleanup();
void setup();
int main(int argc, char **argv);

/*==================================================================================================
                                   LOCAL FUNCTION PROTOTYPES
==================================================================================================*/


/*==================================================================================================
                                       GLOBAL FUNCTIONS
==================================================================================================*/

/*================================================================================================*/
/*===== cleanup =====*/
/**
@brief  Performs all one time clean up for this test on successful
	completion,  premature exit or failure. Closes all temporary
	files, removes all temporary directories exits the test with
	appropriate return code by calling tst_exit() function.cleanup

@param  Input :      None.
        Output:      None.
  
@return Nothing
*/
/*================================================================================================*/
void cleanup()
{

    /* VTE : Actions needed to get a stable target environment */

    int VT_rv = TFAIL;
		
    VT_rv = VT_mu_api_err_cleanup();
    if (VT_rv != TPASS)
    {
	tst_resm(TWARN, "VT_mu_api_err_cleanup() Failed : error code = %d", VT_rv);
    }

    /* Exit with appropriate return code. */

    tst_exit();
}

/*==================================================================================================
                                       LOCAL FUNCTIONS
==================================================================================================*/

/*================================================================================================*/
/*===== setup =====*/
/**
@brief  Performs all one time setup for this test. This function is
        typically used to capture signals, create temporary dirs
	and temporary files that may be used in the course of this test.

@param  Input :      None.
        Output:      None.
  
@return On failure - Exits by calling cleanup().
        On success - returns 0.
*/
/*================================================================================================*/
void setup()
{
    int VT_rv = TFAIL;
	
    /* VTE : Actions needed to prepare the test running */

    VT_rv = VT_mu_api_err_setup();
    if (VT_rv != TPASS)
    {
	tst_brkm(TBROK , cleanup, "VT_mu_api_err_setup() Failed : error code = %d", VT_rv);
    }
    
    return;
}


/*================================================================================================*/
/*===== main =====*/
/**
@brief  Entry point to this test-case. It parses all the command line
        inputs, calls the global setup and executes the test. It logs
	the test status and results appropriately using the LTP API's
	On successful completion or premature failure, cleanup() func
	is called and test exits with an appropriate return code.

@param  Input :  argc - number of command line parameters.
                 **argv - pointer to the array of the command line parameters.
        Output:  None
  
@return On failure - Exits by calling cleanup().
        On success - exits with 0 exit value.
*/
/*================================================================================================*/
int main(int argc, char **argv)
{
    int VT_rv = TFAIL;

    /* perform global test setup, call setup() function. */

    setup();

    /* Print test Assertion using tst_resm() function with argument TINFO. */

    tst_resm(TINFO, "Testing if %s test case is OK\n", TCID);

    VT_rv = VT_mu_api_err_test();
	
    /* VTE : print results and exit test scenario */

    if(VT_rv == TPASS)
        tst_resm(TPASS, "%s test case worked as expected", TCID);
    else
        tst_resm(TFAIL, "%s test case did NOT work as expected", TCID);
		
    cleanup();
	
    return VT_rv;
}

#ifdef __cplusplus
}
#endif
