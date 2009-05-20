/*====================*/
/**
    @file  dio_append_main.c

    @brief  Main file of GPIO dio_append test application.
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
Total Tests:    1

Test Name:      dio_append_test

Test Assertion
& Strategy:     Append zeroed data to a file using O_DIRECT while
                a 2nd process is doing buffered reads and check if the buffer
                reads always see zero.
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
#include "dio_append_test.h"

/*======================
                                        LOCAL MACROS
======================*/


/*======================
                          LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
======================*/


/*======================
                                       LOCAL CONSTANTS
======================*/
#if !defined(TRUE) && !defined(FALSE)
#define TRUE  1
#define FALSE 0
#endif

/*======================
                                       LOCAL VARIABLES
======================*/


/*======================
                                       GLOBAL CONSTANTS
======================*/


/*======================
                                       GLOBAL VARIABLES
======================*/
/* Extern Global Variables */
extern int  Tst_count;               /* counter for tst_xxx routines.         */
extern char *TESTDIR;                /* temporary dir created by tst_tmpdir() */

/* Global Variables */
char *TCID     = "dio_append_test";   /* test program identifier.          */
int  TST_TOTAL = 1;                  /* total number of tests in this file.   */

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

    VT_rv = VT_dio_append_cleanup();
    if (VT_rv != TPASS)
    {
 tst_resm(TWARN, "VT_dio_append_cleanup() Failed : error code = %d", VT_rv);
    }
    /* VTE */

    /* Close all open file descriptors. */

    /** Insert code here. In case an unexpected failure occurs report it
    and exit setup(), the following lines of code will demonstrate
    this.

    if (close(fd) == -1)
    {
       tst_resm(TWARN, "close(%s) Failed, errno=%d : %s",
           fname, errno, strerror(errno));
    } */

    /* Remove all temporary directories used by this test. */

    /** Insert real code here */

    /* kill child processes if any. */

    /** Insert code here */

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
    /* Capture signals. */

    /** Insert code here */

    /* Create temporary directories */

    /** Insert code here */

    /* Create temporary files. */

    /** Insert real code here. In case an unexpected failure occurs
    report it and exit setup(). Follow the code below for example.

    if ((fd = open(fname, O_RDWR|O_CREAT, 0700)) == -1 )
    {
        tst_brkm(TBROK, cleanup,
           "Unable to open %s for read/write.  Error:%d, %s",
           fname, errno, strerror(errno));
    } */

    /* VTE : Actions needed to prepare the test running */
    VT_rv = VT_dio_append_setup();
    if (VT_rv != TPASS)
    {
 tst_brkm(TBROK , cleanup, "VT_dio_append_setup() Failed : error code = %d", VT_rv);
    }
    /* VTE */

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

    /* parse options. */

    /** LTP test harness provides a function called parse_opts() that
    may be used to parse standard options. For a list of standard
    option that are available refer on-line man page at the LTP
    web-site */

    /* perform global test setup, call setup() function. */
    setup();

    /* Print test Assertion using tst_resm() function with argument TINFO. */
    tst_resm(TINFO, "Testing if %s test case is OK", TCID);

    /* Test Case Body. */

    /** Insert real code goes here. In case of unexpected failure, or
    failure not directly related to the entity being tested report
    using tst_brk() or tst_brkm() functions.

    if (something bad happened)
    {
        tst_brkm(TBROK, cleanup, "Unable to open test file %s", fname);
    } */

    /* VTE : print results and exit test scenario */
    VT_rv = VT_dio_append_test(argc,argv); /*with the parameters needed come from parse_opt())*/

    if(VT_rv == TPASS)
        tst_resm(TPASS, "%s test case worked as expected", TCID);
    else
        tst_resm(TFAIL, "%s test case did NOT work as expected", TCID);


    cleanup();
  /* VTE */

    return VT_rv;

}
