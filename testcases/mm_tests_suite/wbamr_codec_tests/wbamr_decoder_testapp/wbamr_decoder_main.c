/**/
/**
    @file   wbamr_decoder_main.c

    @brief  LTP Motorola main file of the WB AMR decoder test application.
*/
/*

  Copyright (C) 2004, Freescale Semiconductor, Inc. All Rights Reserved
  THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
  BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
  Freescale Semiconductor, Inc.


Revision History:
                            Modification     Tracking
Author                          Date          Number    Description of Changes
-------------------------   ------------    ----------  -------------------------------------------
Igor Semenchukov/smng001c    30/11/2004     TLSbo43523   Initial version
Igor Semenchukov/smng001c    28/02/2005     TLSbo47117   Changed printf() entries with tst_...()


Portability: Indicate if this module is portable to other compilers or platforms.
             If not, indicate specific reasons why is it not portable.

*/

/*
Total Tests: 5

Test Name:   wbamr_decoder_testapp

Test Assertion
& Strategy:  The WB AMR (Wide band Adaptive Multi-Rate) decoder test application performs several
             tests. Some of them are required by the decoder documentation.
             The application itself consists of four test cases, which are selected via parameter
             -T <test_id>. The accordance between testcases and their IDs is presented below:
                Nominal functionality test     - 0 (default)
                Reentrance test                - 1
                Relocatability test            - 2
                Robustness test                - 3

            Names of input, output and reference files are taken from file lists, located in ./list
            folder. Default file list is named def_test_files. Test case 3 has its own file list,
            robustness_test_files. Only input file names are mandatory; if output and reference
            files aren't required, their names can be replaced by n/a. Please see ./list folder
            for examples.

            All tests have common sequence of actions, described in detail in the documentation.
            For each test (input) file the application allocates memory for decoder configuration
            structure, asks for amount of  decoder memory via decoder routine, allocates this
            memory, calls decoder initialization routine, opens files, then decodes streams.
*/

#ifdef __cplusplus
extern "C"{
#endif

/*
                                        INCLUDE FILES
*/

/* Standard Include Files */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

/* Harness Specific Include Files */

#include "test.h"
#include "usctest.h"

/* Verification Test Environment Include Files */

#include "wbamr_decoder_test.h"

/*
                                        LOCAL MACROS
*/


/*
                          LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
*/


/*
                                       LOCAL CONSTANTS
*/


/*
                                       LOCAL VARIABLES
*/

/* parse_opts() parameters. First option (testcase) has flag -T and passes testcase ID.
 * Possible values for ID are from 0 through 3. Second option has flag -L and takes list file
 * name as its argument. All options require arguments.
 */

int  testcase_flag  0; /* What test we will run                               */
int  listfile_flag  0; /* We want to pass customized list with test files     */
char *testcase_opt;
char *listfile_opt;

option_t options[] 
{
    { "T:", &testcase_flag, &testcase_opt },
    { "L:", &listfile_flag, &listfile_opt },
    { NULL, NULL,           NULL          }
};

/*
                                       GLOBAL CONSTANTS
*/


/*
                                       GLOBAL VARIABLES
*/
/* Extern Global Variables */

extern int  Tst_count;                    /* counter for tst_xxx routines            */
extern char *TESTDIR;                     /* temporary dir created by tst_tmpdir()   */

/* Global Variables */

char *TCID      "wbamr_decoder_testapp"; /* test program identifier                 */
int  TST_TOTAL  1;                    /* total number of tests in this file      */

/*
                                   GLOBAL FUNCTION PROTOTYPES
*/
void cleanup();
void setup();
int  main(int argc, char **argv);

/*
                                   LOCAL FUNCTION PROTOTYPES
*/
void help();

/*
                                       GLOBAL FUNCTIONS
*/

/**/
/* cleanup */
/**
@brief  Performs all one time clean up for this test on successful
 completion,  premature exit or  failure. Closes all temporary
 files, removes all temporary directories exits the test with
 appropriate return code by calling tst_exit() function.cleanup

@param  Input :      None.
        Output:      None.

@return Nothing
*/
/**/
void cleanup()
{
    int VT_rv  TFAIL;

    /* VTE : Actions needed to get a stable target environment */

    VT_rv  VT_wbamr_decoder_cleanup();
    if (VT_rv ! TPASS)
    {
 tst_resm(TWARN, "VT_wbamr_decoder_cleanup() Failed : error code  %d", VT_rv);
    }

    tst_exit();
}

/*
                                       LOCAL FUNCTIONS
*/

/**/
/* setup */
/**
@brief  Performs all one time setup for this test. This function is
        typically used to capture signals, create temporary dirs
 and temporary files that may be used in the course of this test.

@param  Input :      None.
        Output:      None.

@return On failure - Exits by calling cleanup().
        On success - returns 0.
*/
/**/
void setup()
{
    int VT_rv  TFAIL;

    /* VTE : Actions needed to prepare the test running */

    VT_rv  VT_wbamr_decoder_setup();
    if (VT_rv ! TPASS)
    {
 tst_brkm(TBROK , cleanup, "VT_wbamr_decoder_setup() Failed : error code  %d", VT_rv);
    }

    return;
}

/**/
/* main */
/**
@brief  Entry point to this test-case. It parses all the command line
        inputs, calls the global setup and executes the test. It logs
 the test status and results appropriately using the LTP API's
 On successful completion or premature failure, cleanup() func
 is called and test exits with an appropriate return code.

@param  Input :      argc   - number of command line parameters.
                     **argv - pointer to the array of the command line parameters.
         Describe input arguments to this test-case
    -l - Number of iteration
    -v - Prints verbose output
    -V - Prints the version number
                                -T - number of testcase to run
                                -L - name of list file
        Output:      None

@return On failure - Exits by calling cleanup().
        On success - exits with 0 exit value.
*/
/**/
int main(int argc, char **argv)
{
    int  VT_rv  TFAIL;
    char *msg;
    int  testcase  NOMINAL_FUNCTIONALITY;

    /* ZOB */ tst_resm(TINFO, "--> main()", TCID);

    /* parse options. */

    if ( (msg  parse_opts(argc, argv, options, help)) ! NULL )
    {
 tst_brkm(TBROK, cleanup, "OPTION PARSING ERROR - %s", msg);
    }
    if (testcase_flag)
 testcase  atoi(testcase_opt);

    /* perform global test setup, call setup() function. */

    setup();

    /* Print test Assertion using tst_resm() function with argument TINFO. */

    tst_resm(TINFO, "Testing if %s test case is OK", TCID);

    VT_rv  VT_wbamr_decoder_test(testcase, listfile_opt);

    /* VTE : print results and exit test scenario */

    if (VT_rv  TPASS)
        tst_resm(TPASS, "%s test case worked as expected", TCID);
    else
        tst_resm(TFAIL, "%s test case did NOT work as expected", TCID);

    cleanup();

    return VT_rv;
}

/**/
/* help */
/**
@brief  Inform of the available options and the associated parameters.

@param  Input :      None.
        Output:      None.

@return None.
*/
/**/
void help()
{
    printf("Switches (names may be abbreviated):\n\n");
    printf("-L listfile - name of list file\n");
    printf("-T testcase - test case ID\n");
    printf("  Available testcases are:\n");
    printf("     0 - Nominal functionality test\n");
    printf("     1 - Reentrance test\n");
    printf("     2 - Relocatability test\n");
    printf("     3 - Robustness test\n\n");
}
