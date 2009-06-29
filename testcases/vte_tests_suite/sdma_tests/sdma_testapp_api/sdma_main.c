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
        @file   sdma_main.c

        @brief   SDMA API test main function.

====================================================================================================
Revision History:
                            Modification     Tracking
Author/core ID                  Date          Number    Description of Changes
-------------------------   ------------    ----------  -------------------------------------------
S.ZAVJALOV/----              13/07/2004     TLSbo40259  Initial version
S.ZAVJALOV/ZVJS001C          20/09/2004     TLSbo42070  Fixed case without "-a" argument given
A.Ozerov/B00320              10/02/2006     TLSbo61734  Code was cast to coding conventions
A.Ozerov/B00320              19/04/2007     ENGR29785   help and main functions were changed

====================================================================================================
Portability:  ARM GCC
==================================================================================================*/

/*==================================================================================================
                                        INCLUDE FILES
==================================================================================================*/
/* Standard Include Files */
#include <errno.h>

/* Harness Specific Include Files. */
#include <test.h>
#include <usctest.h>

/* Verification Test Environment Include Files */
#include "sdma_test.h"

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
extern int Tst_count;   /* counter for tst_xxx routines.  */
extern char *TESTDIR;   /* temporary dir created by tst_tmpdir */

/* Global Variables */
char   *TCID = "sdma_api";      /* test program identifier.  */
int     TST_TOTAL = 1;  /* total number of tests in this file.  */

int     test_num,
        channel,
        argument,
        n_num = 0,
        c_num = 0,
        a_num = 0;
char   *n_copt,
       *c_copt,
       *a_copt;

option_t options[] = 
{
        {"n:", &n_num, &n_copt},        /* Number of test */
        {"C:", &c_num, &c_copt},        /* Channel */
        {"a:", &a_num, &a_copt},        /* Argument */
        {NULL, NULL, NULL}      /* NULL required to end array */
};

/*==================================================================================================
                                    GLOBAL FUNCTION PROTOTYPES
==================================================================================================*/
void    cleanup(void);
void    setup(void);
int     main(int argc, char **argv);

/*==================================================================================================
                                    LOCAL FUNCTION PROTOTYPES
==================================================================================================*/
void help(void)
{
        printf("==========================================================================\n");
        printf("  -n x    Number of test case (1-3)\n");
        printf("  -C x    Channel (0-31)\n");
        printf("          if you use channel 0, channel will be chosen automatically\n");
        printf("  -a x    Arguments\n");
        printf("\nUsage: %s -n <num_of_test> -C [channel] -a [arguments]\n", TCID);
        printf("==========================================================================\n\n");
}

/*==================================================================================================
                                        GLOBAL FUNCTIONS
==================================================================================================*/

/*================================================================================================*/
/*===== cleanup =====*/
/**
@brief Performs all one time clean up for this test on successful completion, premature exit or
        failure. Closes all temporary files, removes all temporary directories exits the test with
        appropriate return code by calling tst_exit().

@param 
    
@return 
*/
/*================================================================================================*/
void cleanup(void)
{
        /* VTE : Actions needed to get a stable target environment */
        int     VT_rv = TFAIL;

        VT_rv = VT_sdma_test_cleanup();
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
        VT_rv = VT_sdma_test_setup();
        if (VT_rv != TPASS)
        {
                tst_brkm(TBROK, cleanup, "VT_sdma_setup() Failed : error code = %d", VT_rv);
        }
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

        if ((msg = parse_opts(argc, argv, options, &help)) != (char *) NULL)
                tst_brkm(TBROK, cleanup, "OPTION PARSING ERROR - %s", msg);;

        if (n_num)
        {
                test_num = atoi(n_copt);
                if (test_num < 1 || test_num > 3)
                {
                        help();
                        tst_brkm(TBROK, cleanup, "Invalid arg for -n: %s", n_copt);
                }
        }
        else
        {
                help();
                tst_brkm(TBROK, cleanup, "Required arg -n");
        }

        if (c_num)
        {
                channel = atoi(c_copt);
                if (channel < 0 || channel >= MAX_DMA_CHANNELS)
                {
                        help();
                        tst_brkm(TBROK, cleanup, "Invalid arg for -C: %s", c_copt);
                }
        }

        switch (test_num)
        {
        case MXC_SDMA_TEST_1:
                if (a_num)
                {
                        if (strcmp(a_copt, "SSI") == 0)
                                argument = SSI;
                        else if (strcmp(a_copt, "MMC") == 0)
                                argument = MMC;
                        else if (strcmp(a_copt, "EXT") == 0)
                                argument = EXT;
                        else if (strcmp(a_copt, "MEMORY") == 0)
                                argument = MEMORY;
                        else
                        {
                                help();
                                tst_brkm(TBROK, cleanup, "Invalid arg for -a: %s", a_copt);
                        }
                }
                else
                {
                        argument = MEMORY;
                }
                break;
        case MXC_SDMA_TEST_2:
                if (a_num)
                {
                        argument = atoi(a_copt);
                        if (argument < 1)
                        {
                                help();
                                tst_brkm(TBROK, cleanup, "Invalid arg for -a: %s", a_copt);
                        }
                }
                else
                {
                        argument = 32;
                }
                break;
        }

        /* perform global test setup, call setup() function. */
        setup();

        /* Print test Assertion using tst_resm() function with argument TINFO. */
        tst_resm(TINFO, "Testing if %s test case is OK", TCID);

        /* VTE : Execute test, print results and exit test scenario */
        VT_rv = VT_sdma_test();

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
