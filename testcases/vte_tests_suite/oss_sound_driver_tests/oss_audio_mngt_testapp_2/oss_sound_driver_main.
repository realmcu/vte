/*================================================================================================*/
/**
        @file   oss_sound_driver_main.c

        @brief  OSS audio management test main file.
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
RB657C/gsch1c                20/07/2004     TLSbo40898  Initial version  of OSS sound driver test development
D.Khoroshev/b00313           01/31/2006     TLSbo61785  Reworked version
D.Khoroshev/b00313           11/13/2006     TLSbo81934  Opening quotas were changed, help function fixed

====================================================================================================
Portability: ARM GCC
==================================================================================================*/

/*==================================================================================================
Total Tests: 3

Test Name:   OSS audio management test

Test Assertion
& Strategy:  Check access management politics.
==================================================================================================*/

/*==================================================================================================
                                        INCLUDE FILES
==================================================================================================*/
/* Standard Include Files */
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

/* Verification Test Environment Include Files */
#include "oss_sound_driver_test.h"

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
char *TCID     = "oss_testapp_mngt_2"; /* test program identifier.          */
int  TST_TOTAL = 3;                  /* total number of tests in this file.   */

/*==================================================================================================
                                   GLOBAL FUNCTION PROTOTYPES
==================================================================================================*/
void cleanup(void);
void setup(void);
int main(int argc, char **argv);

/*==================================================================================================
                                   LOCAL FUNCTION PROTOTYPES
==================================================================================================*/


/*==================================================================================================
                                       GLOBAL FUNCTIONS
==================================================================================================*/

/*================================================================================================*/
/*===== cleanup =====*/
/**
@brief  Performs all one time clean up for this test on successful
        completion,  premature exit or  failure. Closes all temporary
        files, removes all temporary directories exits the test with
        appropriate return code by calling tst_exit() function.

@param  Input :      None.
        Output:      None.
  
@return Nothing
*/
/*================================================================================================*/
void cleanup(void)
{
        int VT_rv = TFAIL;
        
        VT_rv = VT_oss_sound_driver_cleanup();
        if (VT_rv != TPASS)
        {
                tst_resm(TWARN, "VT_cleanup() Failed : error code = %d", VT_rv);
        }
        tst_exit();
}

/*==================================================================================================
                                       LOCAL FUNCTIONS
==================================================================================================*/

/*================================================================================================*/
/*===== help =====*/
/**
@brief  Inform of the available options and the associated parameters

@param  Input :      None.
        Output:      None.
  
@return Nothing
*/
/*================================================================================================*/
void help(void)
{
        printf("\tUsage: %s -D <device number>\n\n", TCID);
        printf("\t\t'-D 0' - test Stereo Dac device'\n");
        printf("\t\t'-D 1' - test Voice CODEC device'\n");
        printf("\t\t'-D 2' - test Mixer device'\n");
}

/*================================================================================================*/
/*===== setup =====*/
/**
@brief  Performs all one time setup for this test. This function is
        typically used to capture signals, create temporary dirs
        and temporary files that may be used in the course of this test.

@param  Input :      None.
        Output:      None.
  
@return Nothing
*/
/*================================================================================================*/
void setup(void)
{
        int VT_rv = TFAIL;

        VT_rv = VT_oss_sound_driver_setup();
        if (VT_rv != TPASS)
        {
                tst_brkm(TBROK , cleanup, "VT_setup() Failed : error code = %d", VT_rv);
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

                                -Id - Id of the test according to the test plan
                                -Case N - If exist, the test case number associated with the test Id
                                -Iter - Inform the iteration of the loop in case of an endurance/stress test

@return On failure - Exits by calling cleanup().
        On success - exits with 0 exit value.
*/
/*================================================================================================*/
int main(int argc, char **argv)
{
        int VT_rv = TFAIL;
        char *msg;

        /* parse options. */
        int tflag=0, tid;                 /* binary flags: opt or not */
        char *ch_test_case;

        option_t options[] =
        {
                { "D:", &tflag, &ch_test_case },       /* Test case */
                { NULL, NULL, NULL }                   /* NULL required to end array */
        };

        if ( (msg=parse_opts(argc, argv, options, &help)) != NULL )
        {
                tst_brkm(TBROK, tst_exit, "%s: Invalid arguments\n", TCID);
        }

        tid=atoi(ch_test_case);
        if(tid < 0 || tid > 2)
        {
                tst_resm(TFAIL, "Invalid testcase number: %d", tid);
                cleanup();
        }

        setup();
        tst_resm(TINFO, "Testing if %s %d test case is OK", TCID, tid);

        VT_rv = VT_oss_sound_driver_test(tid);

        if(VT_rv == TPASS)
                tst_resm(TPASS, "%s %d test case worked as expected", TCID, tid);
        else
                tst_resm(TFAIL, "%s %d test case did NOT work as expected", TCID, tid);

        cleanup();

        return VT_rv;
}
