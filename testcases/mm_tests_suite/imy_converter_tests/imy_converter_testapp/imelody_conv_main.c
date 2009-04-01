/*/
  Copyright (C) 2004, Freescale Semiconductor, Inc. All Rights Reserved
  THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
  BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
  Freescale Semiconductor, Inc.
*/

/**
@file imelody_conv_main.c

@brief VTE C main source imelody to midi converter test case

Description of the file

@par Portability:
        Indicate if this module is portable to other compilers or platforms.
        If not, indicate specific reasons why is it not portable.
*/

/*======================== REVISION HISTORY ==================================

Author (core ID)      Date         CR Number    Description of Changes
-------------------   ----------   ----------   ------------------------------
S.ZAVJALOV/zvjs001c   19/10/2004   TLSbo47116   Initial version
D.Simakov/smkd001c    07/04/2005   TLSbo47116   Imroved, endurance, load and
                                                robustness test cases were added.
=============================================================================*/

/*============================================================================
Total Tests: 1

Test Name:   imelody_conv_test

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
#include "imelody_conv_test.h"

/*======================== LOCAL CONSTANTS ==================================*/
#if !defined(TRUE) && !defined(FALSE)
#define TRUE  1
#define FALSE 0
#endif

/*======================== LOCAL MACROS =====================================*/


/*======================== LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS) =======*/


/*======================== LOCAL VARIABLES ==================================*/
int test_case_name, t_num = 0, f_num = 0, vb_mode = 0, iter_num = 0;
char *t_copt, *cfg_file_name, *iter_opt;

option_t options[] = {
    { "T:", &t_num, &t_copt },  	/* Test case name */
    { "F:", &f_num, &cfg_file_name },  	/* Config file name */
    { "V", &vb_mode, NULL },  		/* Verbose mode */
    { "N:", &iter_num, &iter_opt },
    { NULL, NULL, NULL }        	/* NULL required to end array */
};

/*======================== GLOBAL CONSTANTS =================================*/


/*======================== GLOBAL VARIABLES =================================*/
/* Extern Global Variables */
extern int  Tst_count;               /* counter for tst_xxx routines         */
extern char *TESTDIR;                /* temporary dir created by tst_tmpdir()*/

/* Global Variables */
char *TCID     = "imelody_conv_test"; /* test program name                  */
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
    /* VTE : Actions needed to prepare the test running */
	VT_rv = VT_imelody_conv_setup();
	if (VT_rv != TPASS)
	{
	    tst_brkm(TBROK , cleanup, "VT_imelody_conv_setup() Failed : error code = %d", VT_rv);
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
    printf("Usage : %s -T <test case name> -F <configuration file name> -V\n", TCID);
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
    if (t_num)
    {
        test_case_name = atoi( t_copt );
        /*
    	if (strcmp(t_copt, "NORMAL") == 0)
	        test_case_name = iMY_NORMAL;
    	else
	        if (strcmp(t_copt, "REFERENCE") == 0)
		    test_case_name = iMY_REF;
	    else
    		if (strcmp(t_copt, "REENTER") == 0)
	    	    test_case_name = iMY_REENTER;
		else
		    tst_brkm(TBROK, cleanup, "Invalid arg for -T: %s", t_copt);
        */
    }
    else
    {
        tst_brkm(TBROK, cleanup, "Required argument -T");	
    }

    if (!f_num)
    {
        tst_brkm(TBROK, cleanup, "Required argument -F");	
    }
    
    iter_num = iter_num ? atoi( iter_opt ) : 10;
    
    /* perform global test setup, call setup() function. */
    setup();
    
    /* Print test Assertion using tst_resm() function with argument TINFO. */
    tst_resm(TINFO, "Testing if %s test case is OK", TCID);
    
    /* VTE : print results and exit test scenario */
    VT_rv = VT_imelody_conv_test(test_case_name, cfg_file_name); /*with the parameters needed come from parse_opt())*/
    
    if(VT_rv == TPASS)
    {
        tst_resm(TPASS, "This test case works as expected");
    }
    else
    {
        tst_resm(TFAIL, "This test case does NOT work as expected");
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
	
	VT_rv = VT_imelody_conv_cleanup();
	if (VT_rv != TPASS)
	{
	    tst_resm(TWARN, "VT_imelody_conv_cleanup() Failed : error code = %d", VT_rv);
	}
    tst_exit();
}


#ifdef __cplusplus
}
#endif
