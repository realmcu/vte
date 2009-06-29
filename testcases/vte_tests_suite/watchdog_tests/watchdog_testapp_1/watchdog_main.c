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
        @file   watchdog_main.c

        @brief  watchdog test main function.
====================================================================================================
Revision History:
                            Modification     Tracking
Author/core ID                  Date          Number    Description of Changes
-------------------------   ------------    ----------  -------------------------------------------
V.Becker/rc023c              23/07/2004     TLSbo40889  Initial version 
S.V-Guilhou/svan01c          24/08/2005     TLSbo53364  Adapt test suite for MXC91131
V.Khalabuda/b00306           06/07/2006     TLSbo63552  Update for ArgonLV support

====================================================================================================
Portability:  ARM GCC
==================================================================================================*/

/*==================================================================================================
Total Tests: 1

Test Executable Name:  watchdog_testapp_1

Test Strategy:  Test of the second watchdog. OS interrupts will be disabled to trigger the second watchdog
=================================================================================================*/

/*==================================================================================================
                                        INCLUDE FILES
==================================================================================================*/
/* Standard Include Files */
#include <errno.h>

/* Harness Specific Include Files. */
#include <test.h>
#include <usctest.h>

/* Verification Test Environment Include Files */
#include "watchdog_test_1.h"

/*==================================================================================================
                                        LOCAL MACROS
==================================================================================================*/


/*==================================================================================================
                                        LOCAL VARIABLES
==================================================================================================*/
/* Binary flags : option is set or not */
int     flag_watchdog_number;

/* Option arguments */
char   *Watchdogopt;

/*==================================================================================================
                            LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
==================================================================================================*/
/* Options given to the Watchdog test. Argument is required for number of watchdog to be tested. */
option_t Watchdog_options[] =
{
        {"W:", &flag_watchdog_number, &Watchdogopt},        /* Watchdog use case */
        {NULL, NULL, NULL}
};

/*==================================================================================================
                                        LOCAL CONSTANTS
==================================================================================================*/

/*==================================================================================================
                                        GLOBAL CONSTANTS
==================================================================================================*/

/*==================================================================================================
                                        GLOBAL VARIABLES
==================================================================================================*/
/* Extern Global Variables */
extern int     Tst_count;        /* counter for tst_xxx routines.  */
extern char   *TESTDIR;          /* temporary dir created by tst_tmpdir */

/* Global Variables */
char   *TCID = "watchdog_testapp_1";        /* test program identifier.  */
int     TST_TOTAL = 1;                      /* total number of tests in this file.  */

/*==================================================================================================
                                    GLOBALover 17KB  FUNCTION PROTOTYPES
==================================================================================================*/
void    cleanup(void);
void    setup(void);
int     main(int argc, char **argv);

/*==================================================================================================
                                    LOCAL FUNCTION PROTOTYPES
==================================================================================================*/

/*==================================================================================================
                                        GLOBAL FUNCTIONS
==================================================================================================*/

/*================================================================================================*/
/*===== cleanup =====*/
/**
@brief  Performs all one time clean up for this test on successful completion,
        premature exit or failure. Closes all temporary files,
        removes all temporary directories exits the test with
        appropriate return code by calling tst_exit(void) function.cleanup

@param  Input :      None.
        Output:      None.
    
@return Nothing
*/
/*================================================================================================*/
void cleanup(void)
{
        /* VTE : Actions needed to get a stable target environment */
        int     VT_rv = TFAIL;

        VT_rv = VT_watchdog_test1_cleanup();
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

@param  Input :      None.
        Output:      None.
    
@return On failure - Exits by calling cleanup().
        On success - returns 0.
*/
/*================================================================================================*/
void setup(void)
{
        int     VT_rv = TFAIL;

        /* VTE : Actions needed to prepare the test running */
        VT_rv = VT_watchdog_test1_setup();
        if (VT_rv != TPASS)
        {
                tst_brkm(TBROK, cleanup, "VT_nor_mtd_setup() Failed : error code = %d", VT_rv);
        }

        return;
}

/*================================================================================================*/
/*===== help =====*/
/**
@brief  Inform of the available options and the associated parameters

@param  Input :      None.
        Output:      None.
    
@return On failure - Exits by calling cleanup().
        On success - returns 0.
*/
/*================================================================================================*/
void help(void)
{
        printf("Options:\n\n");
        printf("-W     Use case number\n");
        printf("For one-watchdog configuration: Use case must be between 0 and 2\n");
        printf("For two-watchdog configuration: Use case must be between 0 and 4\n\n");
        printf("0  ->  Normal operation, all watchdog(s) enabled\n");
        printf("1  ->  All watchdog(s) enabled but interrupts disabled. System hangs\n");
        printf("2  ->  All watchog(s) enabled but watchdog 1 not serviced. It times out and system hangs\n");
        printf("3  ->  Only watchdog 2 enabled but not serviced. Watchdog times out. [N/A for MXC91131]\n");
        printf("4  ->  Both watchogs enabled but watchdog 2 not serviced. It times out and system hangs. [N/A for MXC91131]\n");
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
        int     VT_rv = TFAIL;

        char   *msg;
        int     use_case = 0;

        /* Parse user defined options */
        tst_resm(TINFO, "Parse options");
        tst_resm(TINFO, "For one-watchdog configuration: Use case must be between 0 and 2");
        tst_resm(TINFO, "For two-watchdog configuration: Use case must be between 0 and 4");
        tst_resm(TINFO, "0  ->  Normal operation, all watchdog(s) enabled");
        tst_resm(TINFO, "1  ->  All watchdog(s) enabled but interrupts disabled. System hangs");
        tst_resm(TINFO, "2  ->  All watchdog(s) enabled but watchdog 1 not serviced. It times out and system hangs");
        tst_resm(TINFO, "3  ->  Only watchdog 2 enabled but not serviced. Watchdog times out.  [N/A for MXC91131]");
        tst_resm(TINFO, "4  ->  Both watchogs enabled but watchdog 2 not serviced. It times out and system hangs.  [N/A for MXC91131]");

        msg = parse_opts(argc, argv, Watchdog_options, help);
        if (msg != (char *) NULL)
        {
                tst_brkm(TBROK, cleanup, "OPTION PARSING ERROR - %s", msg);
        }

        /* Check flags */
        if (flag_watchdog_number)
        {
                use_case = atoi(Watchdogopt);
                if ((use_case < 0) || (use_case > 4))
                {
                        tst_resm(TFAIL, "Bad use case number !! Choose 0-1-2-3-4. Refer to test description");
                        return VT_rv;
                }
        }

        /* Perform global test setup, call setup() function */
        setup();

        tst_resm(TINFO, "Use case number is : %d", use_case);

        /* Print test Assertion using tst_resm() function with argument TINFO */
        tst_resm(TINFO, "Testing if %s test case is OK", TCID);

        /* VTE : Execute test, print results and exit test scenario */
        VT_rv = VT_watchdog_test1(use_case);

        if (VT_rv == TPASS)
        {
                tst_resm(TPASS, "test case %s worked as expected", TCID);
        }
        else
        {
                tst_resm(TFAIL, "test case %s did NOT work as expected", TCID);
        }

        /* cleanup test allocated ressources */
        cleanup();

        return VT_rv;
}
