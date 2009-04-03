/*================================================================================================*/
/**
        @file   dvfs_dptc_main.c

        @brief  DVFS/DPTC testapp main source file
*/
/*==================================================================================================

        Copyright (C) 2006, Freescale Semiconductor, Inc. All Rights Reserved
        THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
        BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
        Freescale Semiconductor, Inc.

====================================================================================================
Revision History:
                            Modification     Tracking
Author/core ID                  Date          Number    Description of Changes
-------------------------   ------------    ----------  -------------------------------------------
V.Khalabuda/b00306           22/11/2006     TLSbo83054  Initial version

====================================================================================================
Portability: ARM GCC

==================================================================================================*/

/*==================================================================================================
Total Tests: 6

Test Name:   dvfs_dptc_testapp

Test Assertion
& Strategy:  • Control DVFS/DPTC operation with features:
                - Enable/Disable DVFS/DPTC module
                - Update DVFS/DPTC table
                - Read currently loaded table
                - Enable/Disable reference circuits
             • On DVFS/DPTC interrupt, the driver updates the current IC voltage according to the DPTC controller measurements.
             • The sample module conforms to the WMSG Linux coding standards
==================================================================================================*/

/*==================================================================================================
                                        INCLUDE FILES
==================================================================================================*/
/* Verification Test Environment Include Files */
#include "dvfs_dptc_test.h"

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
int     case_flag = 0,  /* binary flags: opt or not */
        dev_flag = 0,
        arg_flag = 0;
char   *case_opt,       /* Id option arguments */
       *dev_opt,
       *arg_opt;

option_t options[] =
{
        {"T:", &case_flag, &case_opt},  /* argument required */
        {"D:", &dev_flag, &dev_opt},    /* Device name */
        {"A:", &arg_flag, &arg_opt},    /* Argument of test case */
        {NULL, NULL, NULL}              /* NULL required to end array */
};

/*==================================================================================================
                                        GLOBAL CONSTANTS
==================================================================================================*/

/*==================================================================================================
                                        GLOBAL VARIABLES
==================================================================================================*/
/* Extern Global Variables */
extern int      Tst_count;      /* counter for tst_xxx routines.  */
extern char    *TESTDIR;        /* temporary dir created by tst_tmpdir */

/* Global Variables */
char   *TCID = "dvfs_dptc_testapp";     /* test program identifier.  */
int     TST_TOTAL = 1;                  /* total number of tests in this file.  */

char    device_name[128];       /* Device name */
unsigned int    arg_case;       /* Argument of test case */

extern int test_case;

/*==================================================================================================
                                    GLOBAL FUNCTION PROTOTYPES
==================================================================================================*/

/*==================================================================================================
                                    LOCAL FUNCTION PROTOTYPES
==================================================================================================*/
void    setup(void);
void    help(void);
int     main(int argc, char **argv);

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

@param  Input :      None.
        Output:      None.
    
@return Nothing
*/
/*================================================================================================*/
void    cleanup(void)
{
        int     VT_rv = TFAIL;

        tst_resm(TINFO, "Close DVFS/DPTC device.");
        /* VTE : Actions needed to get a stable target environment */
        VT_rv = VT_dvfs_dptc_cleanup();
        if (VT_rv != TPASS)
        {
                tst_resm(TWARN, "VT_dvfs_dptc_cleanup() Failed : error code = %d", VT_rv);
        }

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
void    setup(void)
{
        int     VT_rv = TFAIL;

        tst_resm(TINFO, "Open DVFS/DPTC device...");
        /* VTE : Actions needed to prepare the test running */
        VT_rv = VT_dvfs_dptc_setup();
        if (VT_rv != TPASS)
        {
                tst_brkm(TBROK, cleanup, "VT_dvfs_dptc_setup() Failed : error code = %d", VT_rv);
        }

        return;
}

/*================================================================================================*/
/*===== help =====*/
/**
@brief  Prints usage information 

@param  Input : None.   
        Output: None. 

@return Nothing
*/
/*================================================================================================*/
void    help(void)
{
        printf("====================================================\n");
        printf("DVFS/DPTC testapp options\n");
        printf("\t-T n, Number of testcase, \tdefault is 0(\"INIT TEST\")\n");
        printf("\t-D X \t Device name\n");
        printf("\t-A x \t Argument of test case (hex)\n");
        printf("\nUsage: %s [-T n]\n", TCID);
}

/*================================================================================================*/
/*===== main =====*/
/**
@brief  Entry point to this test-case. It parses all the command line
        inputs, calls the global setup and executes the test. It logs
        the test status and results appropriately using the LTP API's
        On successful completion or premature failure, cleanup() func
        is called and test exits with an appropriate return code.

@param  Input :     argc - number of command line parameters.
        Output:     **argv - pointer to the array of the command line parameters.
                        Describe input arguments to this test-case
                    -l - Number of iteration
                    -v - Prints verbose output
                    -V - Prints the version number
    
@return On failure - Exits by calling cleanup().
        On success - exits with 0 exit value.
*/
/*================================================================================================*/
int     main(int argc, char **argv)
{
        int     VT_rv = TFAIL;

        /* parse options. */
        char   *msg;

        if ((msg = parse_opts(argc, argv, options, help)) != 0)
                tst_brkm(TBROK, cleanup, "OPTION PARSING ERROR - %s", msg);

        if (dev_flag)
        {
                strcpy(device_name, dev_opt);
        }
        else
        {
                strcpy(device_name, DPTC_DEVICE);
        }

        if (case_flag)
        {
                test_case = atoi(case_opt);
        }
        else
        {
                test_case = INIT_TEST;
        }

        if(arg_flag)
        {
                if(sscanf(arg_opt, "0x%x", &arg_case) != 1)
                {
                        tst_resm(TFAIL, "Cannot parse %s as argument, example: 0x01", arg_opt);
                        return TFAIL;
                }
        }
        else if (test_case == RC_TEST ||
                 test_case == WP_TEST ||
                 test_case == SW_TEST ||
                 test_case == WFI_TEST)
        {
                tst_resm(TFAIL, "-A must be specified");
                return TFAIL;
        }

        /* perform global test setup, call setup() function. */
        setup();

        /* Print test Assertion using tst_resm() function with argument TINFO. */
        tst_resm(TINFO, "Testing if %s %d test case is OK", TCID, test_case);

        /* VTE : print results and exit test scenario */
        VT_rv = VT_dvfs_dptc_test();

        if (VT_rv == TPASS)
                tst_resm(TPASS, "%s %d test case worked as expected", TCID, test_case);
        else
                tst_resm(TFAIL, "%s %d test case did NOT work as expected", TCID, test_case);

        cleanup();

        return  VT_rv;
}
