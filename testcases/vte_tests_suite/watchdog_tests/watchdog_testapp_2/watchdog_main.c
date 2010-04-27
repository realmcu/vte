/***
**Copyright (C) 2006-2009 Freescale Semiconductor, Inc. All Rights Reserved.
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
V.Khalabuda/b00306           06/07/2006     TLSbo69495  Update for MX27 platform in accordance with changes in time.c file
Gamma Gao/b14842             12/24/2007                 Chang for MX37 platform(Kernel 2.6.22)
====================================================================================================
Portability:  ARM GCC
==================================================================================================*/

/*==================================================================================================
Total Tests: 1

Test Executable Name:  watchdog_testapp_2

Test Strategy:  Implementation watchdog features over ioctls of linux/watchdog.h and write operand
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
#include "watchdog_test_2.h"

/*==================================================================================================
                                        LOCAL MACROS
==================================================================================================*/


/*==================================================================================================
                                        LOCAL VARIABLES
==================================================================================================*/
/* Binary flags : option is set or not */
int     w_num = 0,
        s_num = 0,
        t_num = 0;

/* Option arguments */
char    *w_copt,
        *s_copt,
        *t_copt;

/*==================================================================================================
                            LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
==================================================================================================*/
/* Options given to the Watchdog test. Argument is required for number of watchdog to be tested. */
option_t Watchdog_options[] =
{
        {"T:", &t_num, &t_copt},        /* Timeout */
        {"S:", &s_num, &s_copt},        /* Sleep */
        {"W:", &w_num, &w_copt},        /* Watchdog use case */
        {NULL, NULL, NULL}              /* NULL required to end array */
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
char   *TCID = "watchdog_testapp_2";        /* test program identifier.  */
int     TST_TOTAL = 1;                      /* total number of tests in this file.  */

int timeout, sleep_sec, test;

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

        VT_rv = VT_watchdog_test2_cleanup();
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
        VT_rv = VT_watchdog_test2_setup();
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
        printf("====================================================\n");
        printf("WDOG driver options:\n");
        printf("\t-T N\ttimeout: value in seconds to cause wdt timeout/reset\n");
        printf("\t-S N\tsleep: value in seconds to service the wdt\n");
        printf("\t-W N\ttest: 0 - Service wdt with ioctl(), 1 - with write()\n");
        printf("Usage: %s -T <timeout> -S <sleep> -W <test>\n", TCID);
}

/*================================================================================================*/
/*===== main =====*/
/**
@brief  Entry point to this test-case. It parses all the command line inputs,
        calls the global setup and executes the test. It logs the test status and
        results appropriately using the LTP API's
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
        msg = parse_opts(argc, argv, Watchdog_options, help);
        if (msg != (char *) NULL)
        {
                tst_brkm(TBROK, cleanup, "OPTION PARSING ERROR - %s", msg);
        }

        /* Check flags */
        if (w_num)
        {
                use_case = atoi(w_copt);
                test=use_case;
                if ((use_case < 0) || (use_case > 4))
                {
                        tst_resm(TFAIL,
                                 "Bad use case number !! Choose 0-1-2-3-4. Refer to test description");
                        return VT_rv;
                }
        }

        if (t_num)
        {
                timeout = atoi(t_copt);
        }
        else
        {
                tst_resm(TFAIL, "-T must be specified");
                return TFAIL;
        }

        if (s_num)
        {
                sleep_sec = atoi(s_copt);
                if (sleep_sec <= 0)
                {
                        sleep_sec = 1;
                        tst_resm(TINFO, "Correct 0 or negative sleep time to %d seconds", sleep_sec);
                }
        }
        else
        {
                sleep_sec = 1;
                tst_resm(TINFO, "Correct 0 or negative sleep time to %d seconds", sleep_sec);
        }

        /* Perform global test setup, call setup() function */
        setup();

        tst_resm(TINFO, "Use case number is : %d", use_case);

        /* Print test Assertion using tst_resm() function with argument TINFO */
        tst_resm(TINFO, "Testing if %s test case is OK", TCID);

        /* VTE : Execute test, print results and exit test scenario */
        VT_rv = VT_watchdog_test2();

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
