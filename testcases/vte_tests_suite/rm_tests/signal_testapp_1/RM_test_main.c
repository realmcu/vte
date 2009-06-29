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
    @file   RM_main.c

    @brief  Resorse Manager test main file.
====================================================================================================
Revision History:
                            Modification     Tracking
Author                          Date          Number    Description of Changes
-------------------------   ------------    ----------  -------------------------------------------
Dmitriy Kazachkov           10/06/2004      TLSbo39741   Initial version 
Khoroshev D.                10/27/2005      TLSbo56682   Rework version 

====================================================================================================
Portability: ARM GCC 
==================================================================================================*/

/*==================================================================================================
Total Tests: 1

Test Name:   Resource Manager Test

Test Assertion
& Strategy:  Test checks signal handling, blocking signals and waiting for a signal. 
==================================================================================================*/

/*==================================================================================================
                                        INCLUDE FILES
==================================================================================================*/
/* Verification Test Environment Include Files */
#include "signal_test_01.h"

/*==================================================================================================
                                        LOCAL VARIABLES
==================================================================================================*/
int verbose_flag=0;  /* verbose_mode */

option_t options[] =
{
        { "v", &verbose_flag, NULL },
        { NULL, NULL, NULL }
};

/*==================================================================================================
                                        GLOBAL VARIABLES
==================================================================================================*/
/* Extern Global Variables */
extern int  Tst_count;               /* counter for tst_xxx routines.         */
extern char *TESTDIR;                /* temporary dir created by tst_tmpdir( ) */

/* Global Variables */
char *TCID     = "signal_testapp_1"; /* test program identifier.          */
int  TST_TOTAL = 1;                  /* total number of tests in this file.   */

/*==================================================================================================
                                    LOCAL FUNCTION PROTOTYPES
==================================================================================================*/
void cleanup(void);
void setup(void);
int main(int argc, char **argv);

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

@param  
    
@return 
*/
/*================================================================================================*/
void cleanup(void)
{
        int VT_rv = TFAIL;
                
        VT_rv = VT_RM_cleanup();
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

@param  
*/
/*================================================================================================*/
void help(void)
{
        printf("Options:\n\n");
        printf("-v verbose mode\n");
}

/*================================================================================================*/
/*===== setup =====*/
/**
@brief  Performs all one time setup for this test. This function is
        typically used to capture signals, create temporary dirs
        and temporary files that may be used in the course of this test.

@param 
    
@return On failure - Exits by calling cleanup().
        On success - Returns nothing.
*/
/*================================================================================================*/
void setup(void)
{
        int VT_rv = TFAIL;
        VT_rv = VT_RM_setup();
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
                    **argv - pointer to the array of the command line parameters.
        Output:      NONE
@return On failure - Exits by calling cleanup().
        On success - exits with 0 exit value.*/
/*================================================================================================*/
int main(int argc, char **argv)
{
        int VT_rv = TFAIL;
        char *msg;
 
        msg=parse_opts(argc, argv, options, help);
        if (msg != (char *)NULL)
        {
                tst_brkm(TBROK, cleanup, "OPTION PARSING ERROR - %s", msg);
        }
        setup();

        tst_resm(TINFO, "Testing if %s test case is OK", TCID);

        VT_rv = VT_RM_signal_test(); 
        
        if(VT_rv == TPASS)
                tst_resm(TPASS, "%s test case worked as expected", TCID);
        else
                tst_resm(TFAIL, "%s test case did NOT work as expected", TCID);
                
        cleanup(); 
        return VT_rv;
}
