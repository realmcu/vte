/*================================================================================================*/
/**
        @file   pmic_adc_main.c

        @brief  LTP Freescale Test PMIC(SC55112 and MC13783) ADC driver.
*/
/*==================================================================================================

        Copyright (C) 2004, Freescale Semiconductor, Inc. All Rights Reserved
        THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
        BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
        Freescale Semiconductor, Inc.

====================================================================================================
Revision History:
                            Modification     Tracking
Author                          Date          Number     Description of Changes
-------------------------   ------------    ----------   -------------------------------------------
V. Halabuda/HLBV001          07/25/2005     TLSbo52694   Initial version
V. HALABUDA/HLBV001          11/21/2005     TLSbo58395   Update for linux-2.6.10-rel-L26_1_14
E. Gromazina/NONE            12/27/2005     TLSbo59968   Update for MXC91231 and MXC91131
D. Khoroshev/b00313          07/06/2006     TLSbo64235   Added PMIC ADC test module
D. Khoroshev/b00313          07/26/2006     TLSbo64235   Added mc13783 legacy support
====================================================================================================
Portability: ARM GCC

==================================================================================================*/

/*==================================================================================================
Total Tests: 10

Test Name:   Convert/Touch/Battery/Comparator tests

Test Assertion
& Strategy:  This test is used to test the PMIC protocol.
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
#include "ts_test.h"

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
static struct sigaction sa;
static char *t_copt = NULL;

/*==================================================================================================
                                        GLOBAL CONSTANTS
==================================================================================================*/


/*==================================================================================================
                                        GLOBAL VARIABLES
==================================================================================================*/
sig_atomic_t sig_count;
char *TCID = "ts_testapp1";  /* test program identifier.              */
int  TST_TOTAL = 10;            /* total number of tests in this file.   */
char adc_device[128];
int adc_testcase = 0;                /* test case */
int verbose_flag = 0;
int mode_set=0;
char* m_copt;
int mode=0;
/*==================================================================================================
                                    GLOBAL FUNCTION PROTOTYPES
==================================================================================================*/
void cleanup(void);
void setup(void);
int main(int argc, char **argv);
void help(void);

/*==================================================================================================
                                    LOCAL FUNCTION PROTOTYPES
==================================================================================================*/
void sig_hand(int sig);

/*================================================================================================*/
/*===== cleanup =====*/
/**
@brief  Performs all one time clean up for this test on successful
                                completion,  premature exit or  failure. Closes all temporary
                                files, removes all temporary directories exits the test with
                                appropriate return code by calling tst_exit() function.cleanup

@param  Input :      None.
        Output:      None.

@return None
*/
/*================================================================================================*/
void cleanup(void)
{
        /* VTE : Actions needed to get a stable target environment */
        int VT_rv = TFAIL;

        sa.sa_handler = SIG_IGN;

        VT_rv = VT_pmic_adc_test_cleanup();
        if (VT_rv != TPASS)
        {
                tst_resm(TWARN, "VT_cleanup() Failed : error code = %d", VT_rv);
        }
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

@return None
*/
/*================================================================================================*/
void setup(void)
{
        int VT_rv = TFAIL;

        sig_count = 0;                        /* Zero signal counter        */
        memset(&sa, 0, sizeof(sa));
        sa.sa_handler = &sig_hand;            /* Assign a signal handler    */

        /* Register it    */
        if (sigaction(SIGALRM, &sa, NULL))
        {
                tst_resm(TFAIL, "Cannot install signal handler: %s",
                strerror(errno));
        }

        /* VTE : Actions needed to prepare the test running */
        VT_rv = VT_pmic_adc_test_setup();
        if (VT_rv != TPASS)
        {
                tst_resm(TBROK, "VT_setup() Failed : error code = %d", VT_rv);
                cleanup();
                exit(-1);
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
                                -T  test_case - exec test case by prefix

@return On failure - Exits by calling cleanup().
        On success - exits with 0 exit value.
*/
/*================================================================================================*/
int main(int argc, char **argv)
{
        int  VT_rv = TFAIL;
        int  t_num = 0,
             d_num = 0,
             mode_get;
        char *msg,
             *d_copt = NULL;

        option_t options[] =
        {
                { "D:", &d_num, &d_copt},        /* Device name */
                { "T:", &t_num, &t_copt},        /* Testcases numbers */
                { "v", &verbose_flag },          /* Verbose flag */
                { "m:", &mode_set,&m_copt},  
                { NULL, NULL, NULL }             /* NULL required to end array */
        };

        if ( (msg = parse_opts(argc, argv, options, &help)) != (char *) NULL )
        {
                tst_resm(TFAIL, "OPTION PARSING ERROR - %s", msg);
                return TFAIL;
        }

        if (d_num)
        {
                strncpy(adc_device, d_copt, 127);
        }
        else
        {
                strncpy(adc_device, PMIC_ADC_DEV, 127);
        }
        if (t_num)
        {
                if (strcmp(t_copt, "ADC_CONVERT") == 0) adc_testcase = PMIC_ADC_CONVERT_T;
#if defined( CONFIG_MXC_PMIC_SC55112 ) || defined( CONFIG_MXC_PMIC_MC13783 )
                else if (strcmp(t_copt, "ADC_CONVERT_8X") == 0) adc_testcase = PMIC_ADC_CONVERT_8X_T;
                else if (strcmp(t_copt, "ADC_CONVERT_MULTI") == 0) adc_testcase = PMIC_ADC_CONVERT_MULTICHANNEL_T;
                else if (strcmp(t_copt, "ADC_SET_TOUCH") == 0) adc_testcase = PMIC_ADC_SET_TOUCH_MODE_T;
                else if (strcmp(t_copt, "ADC_BATTERY") == 0) adc_testcase = PMIC_ADC_GET_BATTERY_CURRENT_T;
#endif
                else if (strcmp(t_copt, "ADC_GET_TOUCH_SAMPLE") == 0) adc_testcase = PMIC_ADC_GET_TOUCH_SAMPLE_T;
                else if (strcmp(t_copt, "ADC_COMPARATOR") == 0) adc_testcase = PMIC_ADC_ACTIVATE_COMPARATOR_T;
                else if (strcmp(t_copt, "TS_SET_GET_MODE_T") == 0) adc_testcase = TS_SET_GET_MODE_T;
		  else if (strcmp(t_copt, "TS_GET_TOUCH_SAMPLE_T") == 0) adc_testcase = TS_GET_TOUCH_SAMPLE_T;
                else if (strcmp(t_copt, "TS_READ") == 0) adc_testcase = TS_READ_T;
                else
                {
                        tst_resm(TFAIL, "Unknown testcase!");
                        help();
                        return TFAIL;
                }
        }
        else
        {
                tst_resm(TFAIL, "-T must be specified");
                return TFAIL;
        }
     if (mode_set)
        {
                mode_get= atoi(m_copt);
		  mode=mode_get;
                if (( mode< 0) || ( mode > 5))
                {
                        tst_resm(TFAIL,
                                 "Bad use case number !! Choose 0-5. Refer to test description");
                        return TFAIL;
                }
        }

        alarm(1);        /* To avoid hanging in setup. */
        /* perform global test setup, call setup() function. */
        setup();

        /* Print test Assertion using tst_resm() function with argument TINFO. */
        tst_resm(TINFO, "Testing if %s test case is OK", TCID);

        alarm(30);
        sig_count=1;
        /* VTE : Execute test, print results and exit test scenario */
        VT_rv = VT_pmic_adc_test();

        if(VT_rv == TPASS)
        {
                tst_resm(TPASS, "test case %s %s worked as expected", TCID, t_copt);
        }
        else
        {
                tst_resm(TFAIL, "test case %s %s did NOT work as expected", TCID, t_copt);
        }

        /* cleanup test allocated ressources */
        cleanup();

        return VT_rv;
}

/*================================================================================================*/
/*===== help =====*/
/**
@brief  Inform of the available options and the associated parameters

@param  Input :      None.
        Output:      None.

@returns None
*/
/*================================================================================================*/
void help(void)
{
        printf("====================================================\n");
        printf("PMIC ADC driver options\n");
        printf("\t-D name \t Device name\n");
        printf("\t-T name \t Testcase name\n");
        printf("\nUsage: %s [-D device_name] [-T testcase_name]\n", TCID);
        printf("\tcommon testcases:\n");
        //printf("\t -T ADC_CONVERT\n");
        //printf("\t -T ADC_GET_TOUCH_SAMPLE\n");
        //printf("\t -T ADC_COMPARATOR\n");
        //printf("\t -T TS_READ\n\n");
        //printf("\tSC55112 specific testcases:\n");
        //printf("\t -T ADC_CONVERT_8X\n");
        //printf("\t -T ADC_CONVERT_MULTI\n");
        //printf("\t -T ADC_SET_TOUCH\n");
        //printf("\t -T ADC_BATTERY\n");
        printf("\t -T TS_SET_GET_MODE_T\n");
	 printf("\t -T TS_GET_TOUCH_SAMPLE_T -m mode\t mode is 0~5,default is 0\n");
}

/*================================================================================================*/
/*===== timer_handler=====*/
/**
@brief This is a timer handler.

@param sig - signal number

@return None
*/
/*================================================================================================*/
void sig_hand(int sig)
{
        if (sig_count)
        {
                tst_resm(TWARN,"Test hangs up. Exiting after timeout...");
                cleanup();
                tst_resm(TFAIL, "%s %s test did NOT work as expected", TCID, t_copt);
                exit(-1);
        }
        return;
}
