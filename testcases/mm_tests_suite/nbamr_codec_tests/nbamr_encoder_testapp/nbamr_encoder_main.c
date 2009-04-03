/*/
  Copyright (C) 2004, Freescale Semiconductor, Inc. All Rights Reserved
  THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
  BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
  Freescale Semiconductor, Inc.
*/

/**
@file nbamr_encoder_main.c

@brief VTE C main source file of the NB AMR encoder test application.

Description of the file

@par Portability:
        Indicate if this module is portable to other compilers or platforms.
        If not, indicate specific reasons why is it not portable.
*/

/*======================== REVISION HISTORY ==================================

Author (core ID)           Date         CR Number    Description of Changes
-----------------------    ----------   ----------   -------------------------
I. Semenchukov/smng001c    24/01/2005   TLSbo46857   Initial version
I. Semenchukov/smng001c    28/02/2005   TLSbo47115   Changed printf() entries with tst_...()

=============================================================================*/

/*============================================================================
Total Tests: 3

Test Name:   nbamr_encoder_testapp

Test Assertion
& Strategy:  The NB AMR (Narrow band Adaptive Multi-Rate) encoder test
             application performs several tests. Some of them are required by
             the encoder documentation. The application itself consists of
             three test cases, which are selected via parameter -T <test_id>.
             The accordance between testcases and their IDs is presented below:
                Nominal functionality test     - 0 (default)
                Reentrance test                - 1
                Relocatability test            - 2

            Names of input, output and reference files, bitrate and DTX
            flag are taken from file lists, located in ./list folder.
            Default file list depends on #define's and may be
            def_test_files.{vad1,vad2,mms_vad1,mms_vad2}. Input file names,
            bitrate values and DTX flag are mandatory; output and reference
            files aren't required and their names can be replaced by n/a in
            this case. Please see ./list folder for examples.

            All tests have common sequence of actions, described in detail
            in the documentation. For each test (input) file the application
            allocates memory for encoder configuration structure, asks for
            amount of  encoder memory via encoder routine, allocates this
            memory, calls encoder initialization routine, opens files,
            then encodes streams.
=============================================================================*/

#ifdef __cplusplus
extern "C"{
#endif

/*======================== INCLUDE FILES ====================================*/
/* Standard Include Files */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

/* Harness Specific Include Files */

#include "test.h"
#include "usctest.h"

/* Verification Test Environment Include Files */

#include "nbamr_encoder_test.h"

/*======================== LOCAL CONSTANTS ==================================*/


/*======================== LOCAL MACROS =====================================*/


/*======================== LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS) =======*/


/*======================== LOCAL VARIABLES ==================================*/
/* parse_opts() parameters. First option (testcase) has flag -T and passes
 * testcase ID. Possible values for ID are from 0 through 2. Second option has
 * flag -L and takes list file name as its argument.
 * All options require arguments.
 */

int  testcase_flag = 0; /* What test we will run                             */
int  listfile_flag = 0; /* We want to pass customized list with test files   */
char *testcase_opt;
char *listfile_opt;

option_t options[] =
{
    { "T:", &testcase_flag, &testcase_opt },
    { "L:", &listfile_flag, &listfile_opt },
    { NULL, NULL,           NULL          }
};

/*======================== GLOBAL CONSTANTS =================================*/


/*======================== GLOBAL VARIABLES =================================*/
/* Extern Global Variables */

extern int  Tst_count;               /* counter for tst_xxx routines         */
extern char *TESTDIR;                /* temporary dir created by tst_tmpdir()*/

/* Global Variables */

char *TCID     = "nbamr_encoder_testapp"; /* test program name               */
int  TST_TOTAL = 1;                    /* total number of tests in this file */

/*======================== LOCAL FUNCTION PROTOTYPES ========================*/
void setup();
void help();
int  main(int argc, char **argv);

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
    
    VT_rv = VT_nbamr_encoder_setup();
    if (VT_rv != TPASS)
    {
	tst_brkm(TBROK , cleanup, "VT_nbamr_encoder_setup() Failed : error code = %d", VT_rv);
    }

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
    printf("Switches (names may be abbreviated):\n\n");
    printf("-L listfile - name of list file\n");
    printf("-T testcase - test case ID\n");
    printf("  Available testcases are:\n");
    printf("     0 - Nominal functionality test\n");
    printf("     1 - Reentrance test\n");
    printf("     2 - Relocatability test\n\n");
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
    int  VT_rv = TFAIL;
    char *msg;
    int  testcase = NOMINAL_FUNCTIONALITY;

    /* parse options. */
    /** LTP test harness provides a function called parse_opts() that
    may be used to parse standard options. For a list of standard 
    option that are available refer on-line man page at the LTP 
    web-site */
    
    if ( (msg = parse_opts(argc, argv, options, help)) != NULL )
    {
	tst_brkm(TBROK, cleanup, "OPTION PARSING ERROR - %s", msg);
    }

    /* Argument handle */

    if (testcase_flag)
	testcase = atoi(testcase_opt);

    /* perform global test setup, call setup() function. */

    setup();

    /* Print test Assertion using tst_resm() function with argument TINFO. */

    tst_resm(TINFO, "Testing if %s test case is OK", TCID);

    VT_rv = VT_nbamr_encoder_test(testcase, listfile_opt);
	
    /* VTE : print results and exit test scenario */

    if (VT_rv == TPASS)
        tst_resm(TPASS, "This test case worked as expected");
    else
        tst_resm(TFAIL, "This test case did NOT work as expected");

    cleanup();

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
    int VT_rv = TFAIL;

    /* VTE : Actions needed to get a stable target environment */
    
    VT_rv = VT_nbamr_encoder_cleanup();
    if (VT_rv != TPASS)
    {
	tst_resm(TWARN, "VT_nbamr_encoder_cleanup() Failed : error code = %d", VT_rv);
    }

    tst_exit();
}

#ifdef __cplusplus
}
#endif
