/*====================*/
/**
    @file   ioerrors_main.c

    @brief  Main file of GPIO ioerrors test.
*/
/*======================

  Copyright (C) 2004, Freescale Semiconductor, Inc. All Rights Reserved
  THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
  BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
  Freescale Semiconductor, Inc.

====================
Revision History:
                            Modification     Tracking
Author                          Date          Number    Description of Changes
-------------------------   ------------    ----------  -------------------------------------------
Igor Semenchukov             10/06/2004     TLSbo39741   Initial version

====================
Portability: Indicate if this module is portable to other compilers or platforms.
             If not, indicate specific reasons why is it not portable.

======================*/

/*======================
Total Tests:    15

Test Name:      ioerrors_test

Test Assertion
& Strategy:     The program generates error conditions and verifies the error
                code generated with the expected error value. The program also
                tests some of the boundary condtions. The size of test file created
                is filesize_in_blocks * 4k.
                Test blocks:
                    [1] Negative Offset
                    [2] Odd count of read and write
                    [3] Read beyond the file size
                    [4] Invalid file descriptor
                    [5] Out of range file descriptor
                    [6] Closed file descriptor
                    [7] Character device (/dev/null) read, write
                    [8] read, write to a mmaped file
                    [9] read, write to an unmaped file with munmap
                    [10] read from file not open for reading
                    [11] write to file not open for writing
                    [12] read, write with non-aligned buffer
                    [13] read, write buffer in read-only space
                    [14] read, write in non-existant space
                    [15] read, write for file with O_SYNC

======================*/



/*======================
                                        INCLUDE FILES
======================*/
/* Standard Include Files */
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#ifdef __cplusplus
extern "C"{
#endif


/* Harness Specific Include Files. */
#include "test.h"
#include "usctest.h"

#ifdef __cplusplus
}
#endif


/* Verification Test Environment Include Files */
#include "ioerrors_test.h"

/*======================
                                       LOCAL CONSTANTS
======================*/
#if !defined(TRUE) && !defined(FALSE)
#define TRUE  1
#define FALSE 0
#endif

/*======================
                                       GLOBAL VARIABLES
======================*/
/* Extern Global Variables */
extern int  Tst_count;               /* counter for tst_xxx routines.         */
extern char *TESTDIR;                /* temporary dir created by tst_tmpdir() */

/* Global Variables */
char *TCID     = "ioerrors_test";   /* test program identifier.          */
int  TST_TOTAL = 15;                        /* total number of tests in this file.   */

/*======================
                                   GLOBAL FUNCTION PROTOTYPES
======================*/
void cleanup();
void setup();
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
 completion,  premature exit or failure. Closes all temporary
 files, removes all temporary directories exits the test with
 appropriate return code by calling tst_exit() function.cleanup

@param  Input :      None.
        Output:      None.

@return Nothing
*/
/*====================*/
void cleanup()
{
    /* VTE : Actions needed to get a stable target environment */
    int VT_rv = TFAIL;

    VT_rv = VT_ioerrors_cleanup();
    if (VT_rv != TPASS)
    {
 tst_resm(TWARN, "VT_ioerrors_cleanup() Failed : error code = %d", VT_rv);
    }

    /* Exit with appropriate return code. */
    tst_exit();
}

/*======================
                                       LOCAL FUNCTIONS
======================*/

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
void setup()
{
    int VT_rv = TFAIL;

    /* VTE : Actions needed to prepare the test running */
    VT_rv = VT_ioerrors_setup();
    if (VT_rv != TPASS)
    {
 tst_brkm(TBROK , cleanup, "VT_GPIO_test3_setup() Failed : error code = %d", VT_rv);
    }

    return;
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
    -l - Number of iteration
    -v - Prints verbose output
    -V - Prints the version number

@return On failure - Exits by calling cleanup().
        On success - exits with 0 exit value.
*/
/*====================*/
int main(int argc, char **argv)
{
    int VT_rv = TFAIL;

    /* perform global test setup, call setup() function. */
    setup();

    /* Print test Assertion using tst_resm() function with argument TINFO. */
    tst_resm(TINFO, "Testing if %s test case is OK", TCID);

    VT_rv = VT_ioerrors_test(argc, argv);

    /* VTE : print results and exit test scenario */
    if(VT_rv == TPASS)
        tst_resm(TPASS, "%s test case worked as expected", TCID);
    else
        tst_resm(TFAIL, "%s test case did NOT work as expected", TCID);

    cleanup();
    return VT_rv;

}
