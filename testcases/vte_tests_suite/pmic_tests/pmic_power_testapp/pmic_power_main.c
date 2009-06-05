/*================================================================================================*/
/**
        @file   pmic_power_main.c

        @brief  PMIC power test
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
S.Bezrukov/SBAZR1C           07/20/2005     TLSbo52698  Initial version
S.Bezrukov/SBAZR1C           09/06/2005     TLSbo52698  Rework  version
N.Filinova/nfili1c           16/01/2006     TLSbo61037  Rework version

====================================================================================================
Portability: ARM GCC
==================================================================================================*/

/*==================================================================================================
Total Tests: 1

Test Name:   PMIC power test

Test Assertion
& Strategy:  This code is used to test the PMIC power
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
#include "pmic_power_test.h"

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
/* Global Variables */
char *TCID     = "pmic_power_testapp";    /* test program identifier.          */
int  TST_TOTAL = 1;                  /* total number of tests in this file.   */

sTestConfig gTestConfig;

/*==================================================================================================
                                   GLOBAL FUNCTION PROTOTYPES
==================================================================================================*/
int main(int argc, char **argv);
void help(void);
void cleanup(void);
void setup(void);

/*==================================================================================================
                                       LOCAL FUNCTIONS
==================================================================================================*/

/*================================================================================================*/
/*===== setup =====*/
/**
Description of the function

@brief  Performs all one time setup for this test. This function is
        typically used to capture signals, create temporary dirs
        and temporary files that may be used in the course of this test.

@param  Input : None.
        Output: None.

@return Nothing
*/
/*================================================================================================*/
void setup(void)
{
        int VT_rv = TFAIL;

        /* VTE : Actions needed to prepare the test running */
        VT_rv = VT_pmic_power_test_setup();
        if (VT_rv != TPASS)
        {
                tst_brkm(TBROK , cleanup, "VT_setup() Failed : error code = %d", VT_rv);
        }
}

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
void cleanup(void)
{
        /* VTE : Actions needed to get a stable target environment */
        int VT_rv = TFAIL;

        VT_rv = VT_pmic_power_test_cleanup();
        if (VT_rv != TPASS)
        {
                tst_resm(TWARN, "VT_cleanup() Failed : error code = %d", VT_rv);
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

@return On failure - Exits by calling cleanup().
        On success - exits with 0 exit value.
*/
/*================================================================================================*/
int main(int argc, char **argv)
{
        int VT_rv = TFAIL;
        /* parse options. */
        int Tflag = 0,
            Cflag = 0,
            lflag = 0,
            vflag = 0,
            wflag = 0;
        char *Topt,
             *Copt,
             *msg;
        char* cfgFile = NULL;

        option_t options[] = {
                                {"T:", &Tflag, &Topt}, /* Test Number */
                                {"C:", &Cflag, &Copt}, /* Path to config file */
                                {"l",  &lflag,  NULL}, /* To list config settings */
                                {"v",  &vflag,  NULL}, /* Verbouse flag */
                                {"w",  &wflag,  NULL}, /* To create and write new config file */
                                {NULL,  NULL,   NULL}  /* NULL required to end array */
                             };



        if ( (msg = parse_opts(argc, argv, options, &help)) != (char *) NULL )
        {
            help();
            return TFAIL;
        }

        gTestConfig.mTestCase = Tflag ? atoi(Topt) : 0; /*Default test case no. 1*/

        if (gTestConfig.mTestCase < ENABLE || gTestConfig.mTestCase > ERR_CONFIG_PARAMS)
        {
                tst_resm(TBROK, "Invalid argument for -T: %s",Topt);
                help();
                return TFAIL;
        }



        gTestConfig.mWriteConfig = wflag;

        if(gTestConfig.mTestCase == CONFIG)
        {
#ifndef CONFIG_MXC_PMIC_MC13783
                cfgFile = "SC55112_pmic_power.cfg";
#else
                cfgFile = "MC13783_pmic_power.cfg";
#endif
        }
        else
        {
                if(gTestConfig.mTestCase == ERR_CONFIG_PARAMS)
                {
#ifndef CONFIG_MXC_PMIC_MC13783
                        cfgFile = "SC55112_err_params.cfg";
#else
                        cfgFile = "MC13783_err_params.cfg";
#endif
                }
                else gTestConfig.mWriteConfig = 0;
        }

        if(Cflag)
                strcpy(gTestConfig.mCfgFile,Copt);
        else
                strcpy(gTestConfig.mCfgFile,"./");

        if(gTestConfig.mCfgFile[strlen(gTestConfig.mCfgFile)-1] != '/')
                sprintf(gTestConfig.mCfgFile,"%s%c%s",gTestConfig.mCfgFile,'/',cfgFile);
        else
                sprintf(gTestConfig.mCfgFile,"%s%s",gTestConfig.mCfgFile,cfgFile);

        gTestConfig.mList = lflag;
        gTestConfig.mVerbose = vflag;

        setup();

        /* Print test Assertion using tst_resm() function with argument TINFO. */
        tst_resm(TINFO, "Testing if %s test case is OK", TCID);

        /* Test Case Body. */

        VT_rv = VT_pmic_power_test();

        if(VT_rv == TPASS)
        {
                tst_resm(TPASS, "%s test case worked as expected", TCID);
        }
        else
        {
                tst_resm(TFAIL, "%s test case did NOT work as expected", TCID);
        }

        /* Cleanup test allocated ressources */
        cleanup();

        return VT_rv;
}

/*================================================================================================*/
/*===== help =====*/
/**
@brief  Inform of the available options and the associated parameters

@param  None.

@return Nothing.
*/
/*================================================================================================*/
void help(void)
{
        printf("******************************************************************************************\n");
        printf("***  Usage : -T <test_num>. To put test case number.                                   ***\n");
        printf("***  PMIC power driver options:                                                        ***\n");
        printf("***          -T <0> : Test Enable and Disable PMIC regulator                           ***\n");
        printf("***          -T <1> : Test Config a PMIC regulator                                     ***\n");
        printf("***          -T <2> : Test Error Parametrs Config a PMIC regulator                     ***\n");
        printf("***          -C <path>. To put path to config file                                     ***\n");
        printf("***          -l To do pause after each configuration setting.To use only with -v flag  ***\n");
        printf("***          -v Verbouse flag                                                          ***\n");
        printf("***          -w To create and write new config file                                    ***\n");
        printf("******************************************************************************************\n");
}
