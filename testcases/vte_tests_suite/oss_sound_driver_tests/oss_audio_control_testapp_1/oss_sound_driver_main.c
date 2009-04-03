/*================================================================================================*/
/**
        @file   oss_sound_driver_main.c

        @brief  OSS audio control test main file.
*/
/*==================================================================================================

        Copyright (C) 2006, Freescale Semiconductor, Inc. All Rights Reserved
        THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
        BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
        Freescale Semiconductor, Inc.

====================================================================================================
Revision History:
                            Modification     Tracking
Author/Core ID                  Date          Number    Description of Changes
-------------------------   ------------    ----------  -------------------------------------------
RB657C/gsch1c                20/07/2004     TLSbo40898  Initial version of OSS sound driver test development
A.Ozerov/b00320              12/12/2005     TLSbo60058  Problem with set_mixer was fixed.
D.Khoroshev/b00313           03/03/2006     TLSbo62323  Update accordance to last MXC OSS specifications
D.Simakov                    22/12/2005     TLSbo87096  Zeus compilation issue
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
#include "oss_sound_driver_test.h"

/*==================================================================================================
                                            LOCAL CONSTANTS
==================================================================================================*/
#if !defined(TRUE) && !defined(FALSE)
#define TRUE  1
#define FALSE 0
#endif

/*==================================================================================================
                                            GLOBAL VARIABLES
==================================================================================================*/
/* Global Variables */
char   *TCID = "oss_audio_control_testapp_1";   /* test program identifier.  */
int     TST_TOTAL = 1;  /* total number of tests in this file.  */

int     Deviceflag = 0; /* binary flags: opt or not */
char   *Deviceopt;      /* Device option arguments */

option_t options[] = {
        {"D:", &Deviceflag, &Deviceopt},        /* argument required */
        {NULL, NULL, NULL}      /* NULL required to end array */
};

/*==================================================================================================
                                        GLOBAL FUNCTION PROTOTYPES
==================================================================================================*/
void    cleanup(void);
void    setup(void);
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
void cleanup(void)
{
        /* VTE : Actions needed to get a stable target environment */
        int     VT_rv = TFAIL;

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
        printf("  -D n        Device to test\n");
        printf("  0 - STDAC\n");
        printf("  1 - CODEC (default)\n");
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
        int     VT_rv = TFAIL;

        VT_rv = VT_oss_sound_driver_setup();

        if (VT_rv != TPASS)
        {
                tst_brkm(TBROK, cleanup, "VT_setup() Failed : error code = %d", VT_rv);
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
        int     VT_rv = TFAIL;
        char   *msg;
        int     Device = 0;

        /* parse options. */
        if ((msg = parse_opts(argc, argv, options, help)) != NULL)
        {
                tst_brkm(TBROK, cleanup, "OPTION PARSING ERROR - %s", msg);
        }

        if (Deviceflag)
        {
                Device = atoi(Deviceopt);
                tst_resm(TINFO, " Deviceflag = %d", Device);
        }

        /* perform global test setup, call setup() function. */
        setup();

        /* Print test Assertion using tst_resm() function with argument TINFO. */
        tst_resm(TINFO, "Testing if %s test case is OK", TCID);

        VT_rv = VT_oss_sound_driver_test(Device);       /* with the parameters needed come from
                                                        * parse_opt()) */
        if (VT_rv == TPASS)
        {
                tst_resm(TPASS, "%s test case worked as expected", TCID);
        }
        else
        {
                tst_resm(TFAIL, "%s test case did NOT work as expected", TCID);
        }

        cleanup();

        return VT_rv;
}
