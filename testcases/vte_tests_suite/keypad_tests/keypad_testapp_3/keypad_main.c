/*================================================================================================*/
/**
        @file   keypad_main.c

        @brief  keypad test 3 main function.
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
D. SIMAKOV                   04/05/2004     TLSbo39737   Draft version
A.Ozerov/NONE                10/01/2006     TLSbo61037   Update in accordance with linux-2.6.10-rel-L26_1_15

====================================================================================================
Portability: ARM GCC
==================================================================================================*/
/*==================================================================================================
Total Tests: 1

Test Name:   keypad_testapp_3

Test Assertion
& Strategy:  :  Read and display simultaneous key pressing in RAW mode
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
#include "keypad_test_3.h"

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
extern int  Tst_count;               /* counter for tst_xxx routines.         */
extern char *TESTDIR;                /* temporary dir created by tst_tmpdir(void) */

/* Global Variables */
char *TCID = "keypad_test_3";        /* test program identifier.          */
int  TST_TOTAL = 1;                  /* total number of tests in this file.   */

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
        /* VTE : Actions needed to get a stable target environment */
        int VT_rv = TFAIL;
                
        VT_rv = VT_keypad_test_3_cleanup();
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
        VT_rv = VT_keypad_test_3_setup();
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
    
@return On failure - Exits by calling cleanup().
        On success - exits with 0 exit value.
*/
/*================================================================================================*/
int main(int argc, char **argv)
{
        int VT_rv = TFAIL;
                
        /* perform global test setup, call setup() function. */
        setup();

        /* Print test Assertion using tst_resm() function with argument TINFO. */
        tst_resm(TINFO, "Testing if %s test case is OK", TCID);

        VT_rv = VT_keypad_test_3(); /*with the parameters needed come from parse_opt())*/
        
        if(VT_rv == TPASS)
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
