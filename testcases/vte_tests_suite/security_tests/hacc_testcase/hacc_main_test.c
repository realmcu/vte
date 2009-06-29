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
    @file   hacc_main.c

    @brief   hacc API test main function.
====================================================================================================
Revision History:
                            Modification     Tracking
Author                          Date          Number    Description of Changes
-------------------------   ------------    ----------  -------------------------------------------
S.ZAVJALOV/zvjs001c          10/08/2004     TLSbo40418   Initial version 
S.ZAVJALOV/zvjs001c          01/10/2004     TLSbo40649   Version after inspection
S.ZAVJALOV/zvjs001c          04/07/2005     TLSbo51629   Change hacc test strategy
A.URUSOV                     14/09/2005     TLSbo53754   Eliminated initialization from incompatible
                                                         pointer type in line 203
A.URUSOV                     18/10/2005     TLSbo57061   New test functions keys are added
====================================================================================================*/

/*==================================================================================================
Total Tests: 1

Test Executable Name:  hacc_test

Test Strategy: Examine the HAC module functions
=================================================================================================*/

/*==================================================================================================
                                        INCLUDE FILES
==================================================================================================*/
/* Standard Include Files */
#include <errno.h>

/* Harness Specific Include Files. */
#include "test.h"
#include "usctest.h"

/* Verification Test Environment Include Files */
#include "hacc_test.h"

/*==================================================================================================
                                       GLOBAL VARIABLES
==================================================================================================*/
/* Extern Global Variables */
extern int Tst_count;   /* Counter for tst_xxx routines */
extern char *TESTDIR;   /* Temporary dir created by tst_tmpdir */

/* Global Variables */
char   *TCID = "hacc_test";     /* Test program identifier */
int     TST_TOTAL = 1;  /* Total number of tests in this file */

int     contiguous_arrays = 0;  /* Indication of contiguous array and other test keys */
unsigned long data2hash;
int     data2hash_len = 0;
int     verbose_mode = 0;       /* Verbose mode flag */
short int stop_flag = 0;        /* Stop process flag */

/*==================================================================================================
                                   GLOBAL FUNCTION PROTOTYPES
==================================================================================================*/
void    cleanup(void);
void    setup(void);
int     main(int argc, char **argv);

/*==================================================================================================
                                   LOCAL FUNCTION PROTOTYPES
==================================================================================================*/

void    help(void);

/*==================================================================================================
                                       GLOBAL FUNCTIONS
==================================================================================================*/
/*===== cleanup =====*/
/**
@brief  Performs all one time clean up for this test on successful
        completion,  premature exit or  failure. Closes all temporary
        files, removes all temporary directories exits the test with
        appropriate return code by calling tst_exit() function.cleanup

@param
  
@return Nothing
*/
/*================================================================================================*/
void cleanup(void)
{
        /* VTE : Actions needed to get a stable target environment */
        int     VT_rv = TFAIL;

        VT_rv = VT_hacc_test_cleanup();
        if (VT_rv != TPASS)
        {
                tst_resm(TWARN, "VT_cleanup() Failed : error code = %d", VT_rv);
        }

        /* Exit with appropriate return code. */
        tst_exit();
}

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
        int     VT_rv = TFAIL;

        /* VTE : Actions needed to prepare the test running */
        VT_rv = VT_hacc_test_setup();
        if (VT_rv != TPASS)
        {
                tst_brkm(TBROK, cleanup, "VT_hacc_setup() Failed : error code = %d", VT_rv);
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

@param  Input :         argc - number of command line parameters.
        Output:         **argv - pointer to the array of the command line parameters.
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
        char   *msg,
               *s_copt,
               *l_copt;

        int     c_num = 0,
                l_num = 0,
                v_num = 0,
                s_num = 0,
                t_num = 0,
                r_num = 0,
                b_num = 0,
                d_num = 0;

#ifdef CONFIG_PM
        int     n_num = 0,
            m_num = 0;
#endif                          /* CONFIG_PM */



        option_t options[] = {
                {"S:", &s_num, &s_copt},        /* Start address of flash area */
                {"L:", &l_num, &l_copt},        /* Lenght of hashing data */
                {"C", &c_num, NULL},    /* Contiguous or non Contiguous */
                {"v", &v_num, NULL},    /* Verbose mode */
                {"T", &t_num, NULL},    /* Contiguous with Stop */
                {"R", &r_num, NULL},    /* Software reset of the entire HAC Module */
                {"B", &b_num, NULL},    /* HAC Module burst mode test */
                {"D", &d_num, NULL},    /* HAC Module burst read nature configure test */
#ifdef CONFIG_PM
                {"N", &n_num, NULL},    /* Suspends HAC Module */
                {"M", &m_num, NULL},    /* Resumes HAC Module */
#endif                          /* CONFIG_PM */
                {NULL, NULL, NULL}      /* NULL required to end array */
        };

        if ((msg = parse_opts(argc, argv, options, &help)) != (char *) NULL)
        {
                tst_resm(TFAIL, "OPTION PARSING ERROR - %s", msg);
                return TFAIL;
        }
        if (s_num)
        {
                data2hash = strtoul(s_copt, NULL, 16);
                if (data2hash <= 0)
                        tst_resm(TFAIL, "Invalid arg for -S: %s", s_copt);
        }
        else
                data2hash = 0xA0110000;
        if (l_num)
        {
                data2hash_len = strtoul(l_copt, NULL, 16);
                if (data2hash_len <= 0)
                {
                        tst_resm(TFAIL, "Invalid arg for -L: %s", l_copt);
                        return TFAIL;
                }
        }
        else
                data2hash_len = 1;

        if (c_num)
                contiguous_arrays = 1;
        if (v_num)
                verbose_mode = 1;
        if (t_num)
                stop_flag = 1;
        if (r_num)
                contiguous_arrays = 2;
        if (b_num)
                contiguous_arrays = 3;
        if (d_num)
                contiguous_arrays = 4;
#ifdef CONFIG_PM
        if (n_num)
                contiguous_arrays = 5;
        if (m_num)
                contiguous_arrays = 6;
#endif                          /* CONFIG_PM */

        /* perform global test setup, call setup() function. */
        setup();

        /* Print test Assertion using tst_resm() function with argument TINFO. */
        tst_resm(TINFO, "Testing if %s test case is OK", TCID);

        /* VTE : Execute test, print results and exit test scenario */
        VT_rv = VT_hacc_test();

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
