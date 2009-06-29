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
/**
@file eject_mmc_main.c

@brief VTE C main source MMC/SD device input/eject testcase

Description of the file

@par Portability: arm,  gcc,  montavista */

/*================================= REVISION HISTORY ==================================

Author (core ID)      Date         CR Number    Description of Changes
-------------------   ----------   ----------   ------------------------------
S.ZAVJALOV/zvjs001c   22/03/2005   tlsbo46706   Initial version
I.Inkina/nknl001      25/07/2005   TLSbo50891      Update open device 
E.Gromazina           14/10/2005    TLSbo56643     Update for the first MMC
=============================================================================*/

/*============================================================================
Total Tests: 1

Test Name:   eject_mmc_test

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
#include "eject_mmc_test.h"

/*======================== LOCAL CONSTANTS ==================================*/


/*======================== LOCAL MACROS =====================================*/

/*======================== LOCAL VARIABLES ==================================*/
int     flag_d = 0;
char   *d_opt;

/*======================== LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS) =======*/
/* Options given to the MMC test : -D :name of MMC 1 device */

option_t MMC_options[] = 
{

        {"D:", &flag_d, &d_opt},
        {NULL, NULL, NULL}
};

/*======================== GLOBAL CONSTANTS =================================*/


/*======================== GLOBAL VARIABLES =================================*/
/* Extern Global Variables */
extern int Tst_count;   /* counter for tst_xxx routines */
extern char *TESTDIR;   /* temporary dir created by tst_tmpdir(void) */

/* Global Variables */
char   *TCID = "eject_mmc_test";        /* test program name */
int     TST_TOTAL = 1;  /* total number of tests in this file */

/*======================== LOCAL FUNCTION PROTOTYPES ========================*/
void    setup(void);
void    help(void);
int     main(int argc, char **argv);

/*======================== LOCAL FUNCTIONS ==================================*/

/*========================= setup ===========================================*/
/**
Description of the function
@brief  Performs all one time setup for this test. This function is
        typically used to capture signals, create temporary dirs
        and temporary files that may be used in the course of this test.
@param  Input : None.
        Output: None.
@return On failure - Exits by calling cleanup().
        On success - returns 0.
=============================================================================*/

void setup(void)
{
        int     VT_rv = TFAIL;

        /* VTE : Actions needed to prepare the test running */
        VT_rv = VT_eject_mmc_setup();
        if (VT_rv != TPASS)
        {
                tst_brkm(TBROK, cleanup, "VT_eject_mmc_setup() Failed : error code = %d", VT_rv);
        }
        /* VTE */
        return;
}

/*========================== help ===========================================*/
/**
Description of the function
@brief  Describe the usage of the test application
        List the needed arguments
@param  Input : None.
        Output: None.
@return On failure - Exits by calling cleanup().
        On success - returns 0.
=============================================================================*/

void help(void)
{
        printf("options\n");
        printf("-D <name of mmc 1 device> \n");
        return;
}

/*======================= main =============================================*/
/**
@brief  Entry point to this test-case. It parses all the command line
        inputs, calls the global setup and executes the test. It logs
        the test status and results appropriately using the LTP API's
        On successful completion or premature failure, cleanup() func
        is called and test exits with an appropriate return code.

@param  Input : argc - number of command line parameters.
        Output: **argv - pointer to the array of the command line parameters.
                Describe input arguments to this test-case
                -l - Number of iteration
                -v - Prints verbose output
                -V - Prints the version number
@return On failure - Exits by calling cleanup().
        On success - exits with 0 exit value.
=============================================================================*/

int main(int argc, char **argv)
{
        int     VT_rv = TFAIL;
        char   *msg;

        param_mmc *par = NULL;

        par = malloc(sizeof(param_mmc));
        /* Parse user defined options */
        printf("Parse options\n");
        msg = parse_opts(argc, argv, MMC_options, help);

        if (msg != (char *) NULL)
        {
                tst_brkm(TBROK, cleanup, "OPTION PARSING ERROR - %s", msg);
        }

        if (!(flag_d))
        {
                tst_resm(TFAIL, "Error option of the device name \n");
                goto _exit_m;
        }
        else
                 sprintf(par->file_name_mmc_1, "%s", d_opt);

        /* parse options. */
        /** LTP test harness provides a function called parse_opts() that
        may be used to parse standard options. For a list of standard 
        option that are available refer on-line man page at the LTP 
        web-site */

        /* perform global test setup, call setup() function. */
        setup();

        /* Print test Assertion using tst_resm() function with argument TINFO. */
        tst_resm(TINFO, "Testing if %s test case is OK", TCID);

        /* VTE : print results and exit test scenario */
        VT_rv = VT_eject_mmc_test(par); /* with the parameters needed come from parse_opt()) */

        if (VT_rv == TPASS)
        {
                tst_resm(TPASS, "This test case works as expected");
        }
        else
        {
                tst_resm(TFAIL, "This test case does NOT work as expected");
        }

        cleanup();
               /** OR tst_exit(); */
        /* VTE */
      _exit_m:

        free(par);

        return VT_rv;

}

/*======================== GLOBAL FUNCTIONS =================================*/

/*============================= cleanup =====================================*/
/**
Description of the function
@brief  Performs all one time clean up for this test on successful
        completion,  premature exit or  failure. Closes all temporary
        files, removes all temporary directories exits the test with
        appropriate return code by calling tst_exit() function.cleanup

@param  Input : None.
        Output: None.
@return Nothing
=============================================================================*/

void cleanup(void)
{
        /* VTE : Actions needed to get a stable target environment */
        int     VT_rv = TFAIL;

        VT_rv = VT_eject_mmc_cleanup();
        if (VT_rv != TPASS)
        {
                tst_resm(TWARN, "VT_eject_mmc_cleanup() Failed : error code = %d", VT_rv);
        }
        tst_exit();
}


#ifdef __cplusplus
} 
#endif
