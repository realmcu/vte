/*/
  Copyright (C) 2004, Freescale Semiconductor, Inc. All Rights Reserved
  THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
  BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
  Freescale Semiconductor, Inc.
*/

/**
@file png_decoder_main.c

@brief VTE C main source PNG decoder test

PNG decoder test main function

@par Portability:
        SCM-A11, Argon+
        arm-linux-gcc 3.4
*/

/* REVISION HISTORY 

Author (core ID)      Date         CR Number    Description of Changes
-------------------   ----------   ----------   ------------------------------
D.Simakov/smkd001c    12/07/2004   TLSbo41678   Initial version
D.Simakov/smkd001c    06/04/2005   TLSbo47116   The reentrance, preemptive and load
                                                testcases were added
*/

/*
Total Tests: 1

Test Name:   png_decoder_testapp_1

Test Assertion
& Strategy:  A brief description of the test Assertion and Strategy
             TO BE COMPLETED
*/



#ifdef __cplusplus
extern "C"{
#endif

/* INCLUDE FILES */

/* Standard Include Files */
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

/* Harness Specific Include Files. */
#include "test.h"
#include "usctest.h"

/* Verification Test Environment Include Files */
#include "png_decoder_test.h"

/* LOCAL CONSTANTS */

/* LOCAL MACROS */

/* LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS) */

/* LOCAL VARIABLES */

/* GLOBAL CONSTANTS */

/* GLOBAL VARIABLES */

/* Extern Global Variables */
extern int  Tst_count;               /* counter for tst_xxx routines.         */
extern char *TESTDIR;                /* temporary dir created by tst_tmpdir() */

/* Global Variables */
char *TCID      "png_decoder_testapp"; /* test program identifier.          */
int  TST_TOTAL  1;                   /* total number of tests in this file.   */

int testcase_flag     0;
int iter_flag         0;
int cfg_flag          0;
int delay_flag        0;
char * testcase_opt;
char * iter_opt;
char * cfg_opt;
char * delay_opt;

option_t options[] 
{
    { "T:",  &testcase_flag, &testcase_opt  },
    { "N:",  &iter_flag,     &iter_opt      },
    { "C:",  &cfg_flag,      &cfg_opt       },
    { "D:",  &delay_flag,    &delay_opt     },
    { NULL,  NULL,           NULL           }
};

testapp_configuration_t testapp_cfg;

/* GLOBAL FUNCTION PROTOTYPES */

void cleanup();
void setup();
int main( int argc, char ** argv );

/* GLOBAL FUNCTIONS */

/* cleanup */
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
    int VT_rv  TFAIL;

    VT_rv  VT_png_decoder_cleanup();
    if( VT_rv ! TPASS )
    {
        tst_resm( TWARN, "VT_png_decoder_cleanup() Failed : error code  %d", VT_rv );
    }

    tst_exit();
}

/* LOCAL FUNCTIONS */

/* help */
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
    printf( "Switches (names may be abbreviated):\n\n" );
    printf("-T <test case id> Id of the test according to the test plan\n");
    printf("-C <config>       Name of the config file\n");
    printf("-N <iter>         Inform the iteration of the loop in case of an endurance/stress test\n");
    printf("-D <delay>        Delay between decoding in seconds\n");
}


/* setup */
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
    int VT_rv  TFAIL;

    VT_rv  VT_png_decoder_setup();
    if( VT_rv ! TPASS )
    {
        tst_brkm( TBROK , cleanup, "VT_png_decoder_setup() Failed : error code  %d", VT_rv );
    }
    return;
}


/* main */
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
int main( int argc, char ** argv )
{
    int VT_rv  TFAIL;

    /* parse options. */

    char * msg;
    testapp_cfg.testcase  NOMINAL_FUNCTIONALITY;
    testapp_cfg.iter  DEFAULT_ITERATIONS;
    testapp_cfg.cfg_fname  NULL;
    testapp_cfg.delay  5;

    /* parse options. */
    if( NULL ! (msg  parse_opts( argc, argv, options, help )) )
    {
        tst_brkm( TBROK, cleanup, "OPTION PARSING ERROR - %s", msg );
    }
    if( testcase_flag )
    {
        testapp_cfg.testcase  atoi( testcase_opt );
    }
    if( iter_flag )
    {
        testapp_cfg.iter  atoi( iter_opt );
    }
    if( cfg_flag )
    {
        testapp_cfg.cfg_fname  cfg_opt;
    }
    else
    {
        tst_brkm( TBROK, cleanup, "Argument required -C" );
    }
    if( delay_flag )
    {
        testapp_cfg.delay  atoi( delay_opt );
    }

    /* perform global test setup, call setup() function. */
    setup();

    /* Print test Assertion using tst_resm() function with argument TINFO. */
    tst_resm( TINFO, "Testing if %s test case is OK", TCID );

    /* VTE : print results and exit test scenario */
    VT_rv  VT_png_decoder_test(); /* with the parameters needed come from parse_opt()) */

    if( VT_rv  TPASS )
        tst_resm( TPASS, "%s test case worked as expected", TCID );
    else
        tst_resm( TFAIL, "%s test case did NOT work as expected", TCID );


    cleanup(); /** OR tst_exit(); */

    return VT_rv;
}

#ifdef __cplusplus
}
#endif
