/*/
  Copyright (C) 2004, Freescale Semiconductor, Inc. All Rights Reserved
  THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
  BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
  Freescale Semiconductor, Inc.
*/

/**
@file TEMPLATE_main.c

@brief VTE C main source template

Description of the file

@par Portability:
        Indicate if this module is portable to other compilers or platforms.
        If not, indicate specific reasons why is it not portable.
*/

/*======================== REVISION HISTORY ==================================

Author (core ID)      Date         CR Number    Description of Changes
-------------------   ----------   ----------   ------------------------------
Developer Name/ID     DD/MM/YYYY   TLSboXXXXX   BRIEF desc. of changes 
Delaspre/rc149c       07/12/2004   TLSbo40142   update copyrights with Freescale

=============================================================================*/

/*============================================================================
Total Tests: 1

Test Name:   TEMPLATE_testapp_X

Test Assertion
& Strategy:  A brief description of the test Assertion and Strategy
             TO BE COMPLETED
=============================================================================*/


#ifdef __cplusplus
extern "C"{
#endif

/*======================== INCLUDE FILES ====================================*/
/* Standard Include Files */
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
    
/* Harness Specific Include Files. */
#include "test.h"
#include "usctest.h"

/* Verification Test Environment Include Files */
#include "TEMPLATE_test_X.h"

/*======================== LOCAL CONSTANTS ==================================*/
#if !defined(TRUE) && !defined(FALSE)
#define TRUE  1
#define FALSE 0
#endif

/*======================== LOCAL MACROS =====================================*/


/*======================== LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS) =======*/


/*======================== LOCAL VARIABLES ==================================*/
int num_arg, n_num=0;
char *n_copt;

option_t options[] = {
    { "n:", &n_num, &n_copt },  /* Test Number */
    { NULL, NULL, NULL }        /* NULL required to end array */
};

/*======================== GLOBAL CONSTANTS =================================*/


/*======================== GLOBAL VARIABLES =================================*/
/* Extern Global Variables */
extern int  Tst_count;               /* counter for tst_xxx routines         */
extern char *TESTDIR;                /* temporary dir created by tst_tmpdir()*/

/* Global Variables */
char *TCID     = "TEMPLATE_testapp_X"; /* test program name                  */
int  TST_TOTAL = 1;                  /* total number of tests in this file   */

/*======================== LOCAL FUNCTION PROTOTYPES ========================*/
void setup();
void help();
int main(int argc, char **argv);

/*======================== LOCAL FUNCTIONS ==================================*/

/*===== setup =====*/
/**
Description of the function
@brief  Performs all one time setup for this test. This function is
        typically used to capture signals, create temporary dirs
		and temporary files that may be used in the course of this test.
@pre None
@post None
@param  Input : None.
        Output: None.
@return On failure - Exits by calling cleanup().
        On success - returns 0.
@bug No bug
@deprecated Not applicable
@todo Nothing
*/
void setup()
{
 	int VT_rv = TFAIL;
    /* Capture signals. */

    /** Insert code here */

    /* Create temporary directories */

    /** Insert code here */
   
    /* Create temporary files. */

    /** Insert real code here. In case an unexpected failure occurs 
    report it and exit setup(). Follow the code below for example.
 
    if ((fd = open(fname, O_RDWR|O_CREAT, 0700)) == -1 )
    {
        tst_brkm(TBROK, cleanup,
           "Unable to open %s for read/write.  Error:%d, %s", 
           fname, errno, strerror(errno));
    } */
	
    /* VTE : Actions needed to prepare the test running */
		VT_rv = VT_TEMPLATE_setup();
		if (VT_rv != TPASS)
		{
			tst_brkm(TBROK , cleanup, "VT_TEMPLATE_setup() Failed : error code = %d", VT_rv);
		}
	/* VTE */
    
    return;
}

/*===== help =====*/
/**
Description of the function
@brief  Describe the usage of the test application
        List the needed arguments
@pre None
@post None
@param  Input : None.
        Output: None.
@return On failure - Exits by calling cleanup().
        On success - returns 0.
@bug No bug
@deprecated Not applicable
@todo Nothing
*/
void help()
{
    printf("Usage : %s -n <test case number>\n");
	printf("\t-n 1:\t to get TPASS result\n");
	printf("\t-n 2:\t to get TPASS result\n");
	printf("\t-n other value:\t to get TBROK result\n");

    return;
}

/*===== main =====*/
/**
@brief  Entry point to this test-case. It parses all the command line
        inputs, calls the global setup and executes the test. It logs
		the test status and results appropriately using the LTP API's
		On successful completion or premature failure, cleanup() func
		is called and test exits with an appropriate return code.
@pre None
@post None
@param  Input : argc - number of command line parameters.
        Output: **argv - pointer to the array of the command line parameters.
				Describe input arguments to this test-case
				-l - Number of iteration
				-v - Prints verbose output
				-V - Prints the version number
@return On failure - Exits by calling cleanup().
        On success - exits with 0 exit value.
@bug No bug
@deprecated Not applicable
@todo Nothing
*/
int main(int argc, char **argv)
{
	int VT_rv = TFAIL;
    
    /* parse options. */
    /** LTP test harness provides a function called parse_opts() that
    may be used to parse standard options. For a list of standard 
    option that are available refer on-line man page at the LTP 
    web-site */
   	char *msg;
	if ( (msg = parse_opts(argc, argv, options, &help)) != (char *) NULL )
    {
        tst_brkm(TBROK, cleanup, "OPTION PARSING ERROR - %s", msg);
    }
    
    /* Argument handle */
    if (n_num)
	{
        num_arg = atoi( n_copt );
        if (num_arg < 1)
        {
            tst_brkm(TBROK, cleanup, "Invalid argument for -n: %s",n_copt);
        }
	}
    else
	{
        tst_brkm(TBROK, cleanup, "Required argument -n");	
    }
    
    
    /* perform global test setup, call setup() function. */
    setup();
    
    /* Print test Assertion using tst_resm() function with argument TINFO. */
    tst_resm(TINFO, "Testing if %s test case is OK", TCID);
    
    /* Test Case Body. */
    
    /** Insert real code goes here. In case of unexpected failure, or 
    failure not directly related to the entity being tested report 
    using tst_brk() or tst_brkm() functions.
    
    if (something bad happened)
    {
        tst_brkm(TBROK, cleanup, "Unable to open test file %s", fname);
    } */
    
    /* VTE : print results and exit test scenario */
    VT_rv = VT_TEMPLATE_test_X(num_arg); /*with the parameters needed come from parse_opt())*/
    
    if(VT_rv == TPASS)
    {
        tst_resm(TPASS, "This test case worked as expected");
    }
    else
    {
        tst_resm(TFAIL, "This test case did NOT work as expected");
    }
    
    cleanup(); /** OR tst_exit(); */
	/* VTE */
	
	return VT_rv;
	
}


/*======================== GLOBAL FUNCTIONS =================================*/

/*===== cleanup =====*/
/**
Description of the function
@brief  Performs all one time clean up for this test on successful
		completion,  premature exit or  failure. Closes all temporary
		files, removes all temporary directories exits the test with
		appropriate return code by calling tst_exit() function.cleanup
@pre None
@post None
@param  Input : None.
        Output: None.
@return Nothing
@bug No bug
@deprecated Not applicable
@todo Nothing
*/
void cleanup()
{
    /* VTE : Actions needed to get a stable target environment */
		int VT_rv = TFAIL;
		
		VT_rv = VT_TEMPLATE_cleanup();
		if (VT_rv != TPASS)
		{
			tst_resm(TWARN, "VT_TEMPLATE_cleanup() Failed : error code = %d", VT_rv);
		}
	/* VTE */
		
	/* Close all open file descriptors. */

    /** Insert code here. In case an unexpected failure occurs report it 
    and exit setup(), the following lines of code will demonstrate 
    this.

    if (close(fd) == -1) 
    {
       tst_resm(TWARN, "close(%s) Failed, errno=%d : %s",
           fname, errno, strerror(errno));
    } */
 
    /* Remove all temporary directories used by this test. */

    /** Insert real code here */
    
    /* kill child processes if any. */
   
    /** Insert code here */

    /* Exit with appropriate return code. */

    tst_exit();
}


#ifdef __cplusplus
}
#endif