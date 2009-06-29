/***
**Copyright 2006-2009 Freescale Semiconductor, Inc. All Rights Reserved.
**
**The code contained herein is licensed under the GNU General Public
**License. You may obtain a copy of the GNU General Public License
**Version 2 or later at the following locations:
**
**http://www.opensource.org/licenses/gpl-license.html
**http://www.gnu.org/copyleft/gpl.html
**/
/*================================================================================================

        @file   rng_test_module.c

        @brief  rng API
/*====================================================================================================
Revision History:
                Modification     Tracking
Author                          Date          Number    Description of Changes
-------------------------   ------------    ----------  -------------------------------------------
Rakesh S Joshi              29/08/2006     TLSbo74375   Initial version
Rakesh S Joshi              23/01/2007     TLSbo87892   Removed UCOREGISTER_TWICE case

====================================================================================================
Portability:  ARM GCC
==================================================================================================

==================================================================================================
Total Tests: 1

Test Strategy:  Examine the RNG module functions
=================================================================================================

==================================================================================================
                                        INCLUDE FILES
==================================================================================================*/
/* Standard Include Files */
#include <errno.h>

/* Harness Specific Include Files. */
#include "test.h"
#include "usctest.h"

/* Verification Test Environment Include Files */
#include "rnga_test.h"

/*==================================================================================================
                                       GLOBAL VARIABLES
==================================================================================================*/
/* Extern Global Variables */
extern int  Tst_count;               /* counter for tst_xxx routines.         */
extern char *TESTDIR;                /* temporary dir created by tst_tmpdir(...) */

/* Global Variables */
char *TCID     = "rng_test"; 	     /* test program identifier.          */
int  TST_TOTAL = 1;                  /* total number of tests in this file.   */
int  rng_testcase = 0;
unsigned long argument = 0;
/*==================================================================================================
                                   GLOBAL FUNCTION PROTOTYPES
==================================================================================================*/
void cleanup(void);
void setup(void);
int main(int argc, char **argv);

/*==================================================================================================
                                   LOCAL FUNCTION PROTOTYPES
==================================================================================================*/

void help(void);

/*==================================================================================================
                                       GLOBAL FUNCTIONS
==================================================================================================*/

/*================================================================================================*/
/*===== cleanup =====*/
/**
@brief  Performs all one time clean up for this test on successful
        completion,  premature exit or  failure. Closes all temporary
        files, removes all temporary directories exits the test with
        appropriate return code by calling tst_exit() function.cleanup

@param

@return
*/
/*================================================================================================*/
void cleanup(void)
{
        /* VTE : Actions needed to get a stable target environment */
        int VT_rv = TFAIL;

        VT_rv = VT_rng_test_cleanup();
        if (VT_rv != TPASS)
        {
                tst_resm(TWARN, "VT_cleanup() Failed : error code = %d", VT_rv);
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

@param

@return On failure - Exits by calling cleanup().
        On success - returns 0.
*/
/*================================================================================================*/
void setup(void)
{
        int VT_rv = TFAIL;

        /* VTE : Actions needed to prepare the test running */
        VT_rv = VT_rng_test_setup();
        if (VT_rv != TPASS)
        {
                tst_brkm(TBROK , cleanup, "VT_rng_setup() Failed : error code = %d", VT_rv);
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

@param  Input :      argc - number of command line parameters.
        Output:      **argv - pointer to the array of the command line parameters.
        Describe input arguments to this test-case
        -l - Number of iteration
        -v - Prints verbose output
        -V - Prints the version number

@return On failure - Exits by calling cleanup().
        On success - exits with 0 exit value.
*/
/*================================================================================================*/
int main(int argc, char **argv)
{
        int VT_rv = TFAIL;
        int t_num = 0, a_num = 0;
        char *msg, *t_copt, *a_copt;

        option_t options[] = {
                { "T:", &t_num, &t_copt},                       /* Testcases numbers */
                { "A:", &a_num, &a_copt},                       /* Argument for testcase */
                { NULL, NULL, NULL }                            /* NULL required to end array */
        };

        if ( (msg = parse_opts(argc, argv, options, &help)) != (char *) NULL )
        {
                tst_resm(TFAIL, "OPTION PARSING ERROR - %s", msg);
                return TFAIL;
        }

	/* Print test Assertion using tst_resm() function with argument TINFO. */
        tst_resm(TINFO, "Testing %s testcase.", TCID);

	if (t_num)
        {
                if      (strcmp(t_copt, "RNG-UCO-REG-POOL") == 0) rng_testcase =CASE_TEST_RNG_UCOREGISTER_POOL;
                else if (strcmp(t_copt, "RNG-UCO-REG-FLAGS") == 0) rng_testcase =CASE_TEST_RNG_UCOREGISTER_FLAGS;
                else if (strcmp(t_copt, "RNG-GET-RND-BLK") == 0) rng_testcase =CASE_TEST_RNG_GETRANDOM_BLK;
                else if (strcmp(t_copt, "RNG-GET-RND-NONBLK") == 0) rng_testcase =CASE_TEST_RNG_GETRANDOM_NONBLK;
                else if (strcmp(t_copt, "RNG-GET-RND-NONBLK-CALLBK") == 0) rng_testcase =CASE_TEST_RNG_GETRANDOM_NONBLK_CALLBACK;
                else if (strcmp(t_copt, "RNG-ADD-SEEDS-BLK") == 0) rng_testcase =CASE_TEST_RNG_ADDENTROPY_BLK;
                else if (strcmp(t_copt, "RNG-ADD-SEEDS-NONBLK") == 0) rng_testcase =CASE_TEST_RNG_ADDENTROPY_NONBLK;
                else tst_brkm(TBROK, cleanup, "Invalid arg for -T: %s", t_copt);
        }else{
                tst_resm(TFAIL, "-T should be specified");
                return TFAIL;
        }

        if ((rng_testcase == CASE_TEST_RNG_GETRANDOM_BLK) ||(rng_testcase == CASE_TEST_RNG_GETRANDOM_NONBLK )||(rng_testcase == CASE_TEST_RNG_GETRANDOM_NONBLK_CALLBACK )||(rng_testcase == CASE_TEST_RNG_ADDENTROPY_BLK) ||(rng_testcase == CASE_TEST_RNG_ADDENTROPY_NONBLK)){
        if (a_num){
                argument =  atol(a_copt);
                if (argument <= 0 ){
                        tst_resm(TFAIL, "Invalid arg for -A: %s", a_copt);
                        return TFAIL;
                }else{
                }
        }else{
                tst_resm(TFAIL, "-A should be specified");
                return TFAIL;
        }

       }




        /* perform global test setup, call setup() function. */
        setup();

        /* VTE : Execute test, print results and exit test scenario */
        VT_rv = VT_rng_test();

        if(VT_rv == TPASS)
        {
                tst_resm(TPASS, "test case %s worked as expected", TCID);
        }
        else
        {
                tst_resm(TFAIL, "test case %s did NOT work as expected", TCID);
        }

        /* cleanup test allocated resources */
        cleanup();

        return VT_rv;
}

