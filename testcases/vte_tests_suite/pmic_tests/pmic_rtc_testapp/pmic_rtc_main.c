/***
**Copyright (C) 2005-2009 Freescale Semiconductor, Inc. All Rights Reserved.
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
        @file   pmic_rtc_main.c

        @brief  main file of PMIC RTC driver test.
====================================================================================================
Revision History:
                            Modification     Tracking
Author/Core ID                  Date          Number    Description of Changes
-------------------------   ------------    ----------  -------------------------------------------
E.Gromazina/NONE             12/08/2005     TLSbo59968  PMIC RTC draft version
A.Ozerov/b00320              06/07/2006     TLSbo61903  Main function was changed.

====================================================================================================
Portability: ARM GCC
==================================================================================================*/

/*==================================================================================================
                                        INCLUDE FILES
==================================================================================================*/
/* Standard Include Files */
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

/* Harness Specific Include Files. */
#include <test.h>
#include <usctest.h>

/* Verification Test Environment Include Files */
#include "pmic_rtc_test.h"

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
/* Global Variables */
char   *TCID = "pmic_rtc_testapp";      /* test program identifier.  */
int     TST_TOTAL = 5;  /* total number of tests in this file.  */
char    dev_path[20];

/*==================================================================================================
                                    GLOBAL FUNCTION PROTOTYPES
==================================================================================================*/
int     main(int argc, char **argv);
void    help(void);
void    setup(void);
void    cleanup(void);

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

@return None
*/
/*================================================================================================*/
void setup(void)
{
        int     VT_rv = TFAIL;

        /* VTE : Actions needed to prepare the test running */
        VT_rv = VT_pmic_rtc_test_setup();
        if (VT_rv != TPASS)
        {
                tst_brkm(TBROK, cleanup, "VT_rtc_setup Failed : error code = %d", VT_rv);
        }

        return;
}

/*================================================================================================*/
/*===== cleanup =====*/
/** 
@brief  Performs all one time clean up for this test on successful 
        completion,  premature exit or  failure. Closes all temporary 
        files, removes all temporary directories exits the test with 
        appropriate return code by calling tst_exit(...) function.cleanup 

@param  Input :      None. 
        Output:      None. 
    
@return None. 
*/
/*================================================================================================*/
void cleanup(void)
{
        int     VT_rv = TFAIL;

        /* VTE : Actions needed to get a stable target environment */
        VT_rv = VT_pmic_rtc_test_cleanup();
        if (VT_rv != TPASS)
        {
                tst_resm(TWARN, "VT_rtc_test_cleanup() Failed : error code = %d", VT_rv);
        }
        /* Exit with appropriate return code. */
        tst_exit();
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

        /* parse options. */
        int     tflag = 0;      /* binary flags: opt or not */
        int     switch_fct = 0;
        char    *ch_test_case;   /* option arguments */
        char    *msg;

        option_t options[] = 
        {
                {"T:", &tflag, &ch_test_case},  /* argument required */
                {NULL, NULL, NULL}      /* NULL required to end array */
        };

        if ((msg = parse_opts(argc, argv, options, &help)) != NULL)
        {
                tst_brkm(TBROK, cleanup, "%s test case did NOT work as expected\n", VT_rv);
        }

        if (tflag)
        {
                sprintf(dev_path, "%s", PMIC_DEVICE_RTC);
                switch_fct = atoi(ch_test_case);

                /* Print test Assertion using tst_resm() function with argument TINFO. */
                tst_resm(TINFO, "Testing if %s test case is OK\n", TCID);

                /* perform global test setup, call setup() function. */
                setup();

                VT_rv = VT_pmic_rtc_test(switch_fct);

                if (VT_rv == TPASS)
                {
                        tst_resm(TPASS, "%s test case worked as expected\n", TCID);
                }
                else
                {
                        tst_resm(TFAIL, "%s test case did NOT work as expected\n", TCID);
                }

                cleanup();
        }
        else
        {
                /* VTE : print results and exit test scenario */
                help();
        }

        return VT_rv;
}

/*================================================================================================*/
/*===== help =====*/
/** 
@brief  Print help information testapp execution.

@param  Input :      None. 
        Output:      None. 

@return None  
*/
/*================================================================================================*/
void help(void)
{
        printf("PMIC RTC driver option\n");
        printf("  -T 0\t Test all PMIC rtc\n");
        printf("  -T 1\t Test time and date read and write RTC functions\n");
        printf("  -T 2\t Test alarm read and write RTC functions\n");
        printf("  -T 3\t Test Alarm IT function\n");
        printf("  -T 4\t Test Alarm IT poll function\n");
}
