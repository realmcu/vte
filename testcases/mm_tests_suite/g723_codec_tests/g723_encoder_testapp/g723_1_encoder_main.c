/*  Copyright (C) 2004, Freescale Semiconductor, Inc. All Rights Reserved
  THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
  BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
  Freescale Semiconductor, Inc.
*/

/**
@file g723_1_encoder_main.c

@brief VTE C main source template

Description of the file

@par Portability:
        Indicate if this module is portable to other compilers or platforms.
        If not, indicate specific reasons why is it not portable.
*/

/* REVISION HISTORY 

Author (core ID)      Date         CR Number    Description of Changes
-------------------   ----------   ----------   ------------------------------
I.Inkina/nknl001         27/12/2004        TLSbo47117   BRIEF desc. of changes
S. V-Guilhou/svan01c  26/05/2005   TLSbo50534   P4 Codecs Campaign (add traces)

*/

/*
Total Tests: TO BE COMPLETED

Test Name:   TO BE COMPLETED

Test Assertion
& Strategy:  A brief description of the test Assertion and Strategy
             TO BE COMPLETED
*/


#ifdef __cplusplus
extern "C"{
#endif

/*
                                        INCLUDE FILES
*/
/* Standard Include Files */
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

/* Harness Specific Include Files. */
#include "test.h"
#include "usctest.h"

/* Verification Test Environment Include Files */
#include "g723_1_encoder_test.h"

/*
                                        LOCAL MACROS
*/


/*
                          LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
*/


/*
                                       LOCAL CONSTANTS
*/

/*
                                       LOCAL VARIABLES
*/


/*
                                       GLOBAL CONSTANTS
*/


/*
                                       GLOBAL VARIABLES
*/
/* Extern Global Variables */
extern int  Tst_count;               /* counter for tst_xxx routines.         */
extern char *TESTDIR;                /* temporary dir created by tst_tmpdir() */

/* Global Variables */
char *TCID      "g723_1_encoder_testapp"; /* test program identifier.          */
int  TST_TOTAL  1;                     /* total number of tests in this file.   */

extern int test_case;

int testcase_flag  0, list_flag  0; /* binary flags: opt or not */
int number_flag0;
int iter_flag  0;          /* binary flags: opt or not */
char * testcase_opt;        /* Id option arguments */
char * iter_opt;            /* Iter option arguments */
char *number_opt;
char *list_opt; /* Case option arguments */

option_t options[] 
{
    { "T:",  &testcase_flag, &testcase_opt  }, /* argument required */
    { "I:",  &iter_flag,     &iter_opt      }, /*number of the iteration */
    { "L:", &list_flag, &list_opt },          /* argument default list file name*/
    { "N:", &number_flag, &number_opt },       /*write output in the file*/
    { NULL,  NULL,           NULL           }  /* NULL required to end array */
};

/*
                                   GLOBAL FUNCTION PROTOTYPES
*/
void cleanup();
void setup();
int main( int argc, char ** argv );

/*
                                   LOCAL FUNCTION PROTOTYPES
*/


/*
                                       GLOBAL FUNCTIONS
*/

/**/
/* cleanup */
/**
@brief  Performs all one time clean up for this test on successful
 completion,  premature exit or  failure. Closes all temporary
 files, removes all temporary directories exits the test with
 appropriate return code by calling tst_exit() function.cleanup

@param  Input :      None.
        Output:      None.

@return Nothing
*/
/**/
void cleanup()
{
    /* VTE : Actions needed to get a stable target environment */
    int VT_rv  TFAIL;

    VT_rv  VT_g723_1_encoder_cleanup();
    if( VT_rv ! TPASS )
    {
 tst_resm( TWARN, "VT_g723_1_encoder_cleanup() Failed : error code  %d", VT_rv );
    }

    tst_exit();
}

/*
                                       LOCAL FUNCTIONS
*/

/**/
/* help */
/**
@brief  Inform of the available options and the associated parameters

@param  Input :      None.
        Output:      None.

@return None.
*/
/**/
void help()
{
    printf( "Switches (names may be abbreviated):\n\n" );
    printf( "-T testcase Id of the test according to the test plan\n" );
    printf( "-I iter  Inform the iteration of the loop in case of an endurance/stress test\n" );
    printf("-N set the write to the files \n");
    printf("-L set the file list \n");
}
/**/
/* setup */
/**
@brief  Performs all one time setup for this test. This function is
        typically used to capture signals, create temporary dirs
 and temporary files that may be used in the course of this test.

@param  Input :      None.
        Output:      None.

@return On failure - Exits by calling cleanup().
        On success - returns 0.
*/
/**/
void setup()
{
    int VT_rv  TFAIL;

    /* VTE : Actions needed to prepare the test running */
    VT_rv  VT_g723_1_encoder_setup();
    if( VT_rv ! TPASS )
    {
 tst_brkm( TBROK , cleanup, "VT_g723_1_encoder_setup() Failed : error code  %d", VT_rv );
    }
    /* VTE */

    return;
}


/**/
/* main */
/**
@brief  Entry point to this test-case. It parses all the command line
        inputs, calls the global setup and executes the test. It logs
 the test status and results appropriately using the LTP API's
 On successful completion or premature failure, cleanup() func
 is called and test exits with an appropriate return code.

@param  Input :      argc   - number of command line parameters.
        Output:      **argv - pointer to the array of the command line parameters.
         Describe input arguments to this test-case
            -T- test number
    -l - Number of iteration
            - L argument default list file name
            - N  write output in the file

@return On failure - Exits by calling cleanup().
        On success - exits with 0 exit value.
*/
/**/
int main( int argc, char ** argv )
{

    /* ZOB */ tst_resm(TINFO, "--> main()", TCID);

    int VT_rv  TFAIL;

    /* parse options. */

    char * msg;
    int testcase  NOMINAL_FUNCTIONALITY;
    int iter      DEFAULT_ITERATIONS;
    int number 0;
    test_case  testcase;

    /* parse options. */
    if( NULL ! (msg  parse_opts( argc, argv, options, help )) )
    {
 tst_brkm( TBROK, cleanup, "OPTION PARSING ERROR - %s", msg );
    }
    if( testcase_flag )
    {
 testcase  atoi( testcase_opt );
 tst_resm(TINFO , "testcase  %d", testcase );
    test_case  testcase;
    }
    if( iter_flag )
    {
 iter  atoi( iter_opt );
 tst_resm(TINFO, "Iterflag  %d", iter );
    }
    if( number_flag )
    {
        number  atoi( number_opt );
        tst_resm(TINFO, "numberflag  %d", number );
    }
    if (list_flag)
    {
        tst_resm(TINFO, "List of files will be taken from %s", list_opt);
    }

    /** LTP test harness provides a function called parse_opts() that
    may be used to parse standard options. For a list of standard
    option that are available refer on-line man page at the LTP
    web-site */

    /* perform global test setup, call setup() function. */
    setup();

    /* Print test Assertion using tst_resm() function with argument TINFO. */
    tst_resm( TINFO, "Testing if %s test case is OK", TCID );
    /* VTE : print results and exit test scenario */
    VT_rv  VT_g723_1_encoder_test( testcase,  iter, list_opt ,number);
     /* with the parameters needed come from parse_opt()) */

    if( VT_rv  TPASS )
        tst_resm( TPASS, "%s test case worked as expected", TCID );
    else
        tst_resm( TFAIL, "%s test case did NOT work as expected", TCID );


    cleanup(); /** OR tst_exit(); */
    /* VTE */

    return VT_rv;
}




#ifdef __cplusplus
}
#endif

