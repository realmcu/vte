/*====================*/
/**
        @file   keypad_main.c

        @brief  keypad test 4 main function.
*/
/*======================

        Copyright (C) 2006, Freescale Semiconductor, Inc. All Rights Reserved
        THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
        BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
        Freescale Semiconductor, Inc.

====================
Revision History:
                            Modification     Tracking
Author/core ID                  Date          Number    Description of Changes
-------------------------   ------------    ----------  -------------------------------------------
V.BECKER/rc023c              30/03/2004     TLSbo38735  Initial version
V.BECKER/rc023c              15/04/2004     TLSbo38735  Correction after code inspection
V.BECKER/rc023c              30/04/2004     TLSbo38735  Change file name
D.SIMAKOV/smkd001c           31/05/2004     TLSbo39737  The configuration of row and column
                                                        number through test arguments is added
A.Ozerov/NONE                10/01/2006     TLSbo61037  Update in accordance with linux-2.6.10-rel-L26_1_15
A.Ozerov/B00320              15/02/2006     TLSbo61037  Device was changed and testapp was reworked accordingly

====================
Portability:  ARM GCC
======================*/

/*======================
Total Tests: 1

Test Executable Name:  keypad_testapp_4

=====================*/

/*======================
                                        INCLUDE FILES
======================*/
/* Standard Include Files */
#include <stdio.h>
#include <errno.h>

/* Harness Specific Include Files. */
#include <test.h>
#include <usctest.h>

/* Verification Test Environment Include Files */
#include "keypad_test_4.h"

/*======================
                                        LOCAL MACROS
======================*/

/*======================
                                        LOCAL VARIABLES
======================*/

/*======================
                            LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
======================*/

/*======================
                                        LOCAL CONSTANTS
======================*/

/*======================
                                        GLOBAL CONSTANTS
======================*/

/*======================
                                        GLOBAL VARIABLES
======================*/
/* Extern Global Variables */
extern int  Tst_count;               /* counter for tst_xxx routines.         */
extern char *TESTDIR;                /* temporary dir created by tst_tmpdir(void) */

/* Global Variables */
char *TCID     = "keypad_testapp_4"; /* test program identifier.          */
int  TST_TOTAL = 1;                  /* total number of tests in this file.   */

/*======================
                                    GLOBAL FUNCTION PROTOTYPES
======================*/
void cleanup(void);
void setup(void);
int main(int argc, char **argv);

/*======================
                                    LOCAL FUNCTION PROTOTYPES
======================*/


/*======================
                                        GLOBAL FUNCTIONS
======================*/
/*====================*/
/*= cleanup =*/
/**
@brief  Performs all one time clean up for this test on successful
                                completion,  premature exit or  failure. Closes all temporary
                                files, removes all temporary directories exits the test with
                                appropriate return code by calling tst_exit() function.cleanup

@param  Input :      None.
        Output:      None.

@return Nothing
*/
/*====================*/
void cleanup(void)
{
        int VT_rv = TFAIL;

        VT_rv = VT_keypad_test4_cleanup();
        if (VT_rv != TPASS)
        {
                tst_resm(TWARN, "VT_keypad_test4_cleanup() Failed : error code = %d", VT_rv);
        }

        tst_exit();
}

/*======================
                                        LOCAL FUNCTIONS
======================*/
/*====================*/
/*= help =*/
/**
@brief  Inform of the available options and the associated parameters

@param  Input :      None.
        Output:      None.

@return On failure - Exits by calling cleanup().
        On success - returns 0.
*/
/*====================*/
void help(void)
{
        printf("\n============\n");
        printf("Keypad driver options:\n");
        printf("\t '-C' Number of a test case\n");
        printf("Usage: %s [-C]\n\n", TCID);
}

/*====================*/
/*= setup =*/
/**
@brief  Performs all one time setup for this test. This function is
        typically used to capture signals, create temporary dirs
                                and temporary files that may be used in the course of this test.

@param  Input :      None.
        Output:      None.

@return On failure - Exits by calling cleanup().
        On success - returns 0.
*/
/*====================*/
void setup(void)
{
        int VT_rv = TFAIL;

        VT_rv = VT_keypad_test4_setup();
        if (VT_rv != TPASS)
        {
                tst_brkm(TBROK , cleanup, "VT_keypad_test4_setup() Failed : error code = %d", VT_rv);
        }
}

/*====================*/
/*= main =*/
/**
@brief  Entry point to this test-case. It parses all the command line
        inputs, calls the global setup and executes the test. It logs
            the test status and results appropriately using the LTP API's
        On successful completion or premature failure, cleanup() func
        is called and test exits with an appropriate return code.

@param  Input :      argc - number of command line parameters.
        Output:      **argv - pointer to the array of the command line parameters.
                                Describe input arguments to this test-case
                                -l   - Number of iteration
                                -v   - Prints verbose output
                                -V   - Prints the version number
                                -r   - Number of rows of the keypad matrix
                                -c   - Number of columns of the keypad matrix

@return On failure - Exits by calling cleanup().
        On success - exits with 0 exit value.
*/
/*====================*/
int main(int argc, char **argv)
{
        int VT_rv = TFAIL;
        int testcase = 0;
        char *msg;

        int     Cflag = 0;
        char    *Copt;

        option_t options[] =
        {
                {"C:", &Cflag, &Copt},
                {NULL, NULL, NULL}
        };

        setup();
        tst_resm(TINFO, "Testing if %s test case is OK", TCID);

        /* Parsing of the command line */
        tst_resm(TINFO, "Parse options");
        msg = parse_opts(argc, argv, options, help);

        if (msg != (char *)NULL)
        {
                tst_brkm(TBROK, cleanup, "OPTION PARSING ERROR - %s", msg);
        }

        if(Cflag)
        {
                testcase = atoi(Copt);
                if ((testcase < 0 || testcase > 1))
                {
                        tst_resm(TCONF, "Invalid arg for -C: %d OPTION VALUE OUT OF RANGE", testcase);
                        help();
                        return TFAIL;
                }
        }
        else
        {
                help();
                return TFAIL;
        }

        VT_rv = VT_keypad_test4(testcase);


        if(VT_rv == TPASS)
        {
                tst_resm(TPASS, "test case %s worked as expected", TCID);
        }
        else
        {
                tst_resm(TFAIL, "test case %s did NOT work as expected", TCID);
        }

        cleanup();

        return VT_rv;
}
