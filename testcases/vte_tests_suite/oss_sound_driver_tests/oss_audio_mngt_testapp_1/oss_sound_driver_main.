/*================================================================================================*/
/**
        @file   oss_sound_driver_main.c

        @brief  OSS audio simple open test.
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
D.Khoroshev/b00313           02/23/2006     TLSbo61805  Update according new SSI specifications
D.Simakov                    13/06/2006     TLSbo67022  Update HELP
====================================================================================================
Portability: ARM GCC 
==================================================================================================*/

/*==================================================================================================
Total Tests: 1

Test Name:   Simple open test 

Test Assertion
& Strategy:  Test to open more than once the same instance of the driver to ensure that the driver 
             does not allow multiple opening. It does it for each driver (dsp and dsp1)  
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
#if !defined(TRUE) && !defined(FALSE)
#define TRUE  1
#define FALSE 0
#endif

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
char *TCID     = "oss_testapp_mngt_1";        /* test program identifier.             */
int  TST_TOTAL = 1;                           /* total number of tests in this file.  */

int  Instflag = 0;        /* binary flags: opt or not   */
char *Instopt;            /* Device option arguments    */

option_t options[] = {
        { "N:", &Instflag, &Instopt  },        /* argument required          */
        { NULL, NULL, NULL }                   /* NULL required to end array */
};

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
        appropriate return code by calling tst_exit() function.cleanup

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
  
@return On failure - Exits by calling cleanup().
        On success - returns 0.
*/
/*================================================================================================*/
void help(void)
{
        printf("Switches \n\n");
        printf("  -N n        Number of instances the driver has\n");
}

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
        int VT_rv = TFAIL;
        VT_rv = VT_oss_sound_driver_setup();
        if (VT_rv != TPASS)
        {
                tst_brkm(TBROK , cleanup, "VT_setup() Failed : error code = %d", VT_rv);
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
        int Device = 0, inst_num = 2;
                
        /* parse options. */
        if ( (msg=parse_opts(argc, argv, options, help)) != NULL )
        {
                tst_brkm(TBROK, cleanup, "OPTION PARSING ERROR - %s", msg);
        }
        if (Instflag)
        {
                inst_num = atoi(Instopt);
                if (inst_num <= 0) inst_num = 2;
                tst_resm(TINFO, "Number of Instances to check = %d", Device);
        }
        
        /* perform global test setup, call setup() function. */
        setup();

        /* Print test Assertion using tst_resm() function with argument TINFO. */
        tst_resm(TINFO, "Testing if %s test case is OK", TCID);
        /* VTE : print results and exit test scenario */
        VT_rv = VT_oss_sound_driver_test(inst_num); /*with the parameters needed come from parse_opt()*/
        
        if (VT_rv == TPASS)
                tst_resm(TPASS, "%s test case worked as expected", TCID);
        else
                tst_resm(TFAIL, "%s test case did NOT work as expected", TCID);
                
        cleanup(); /** OR tst_exit(); */

        return VT_rv;
}
