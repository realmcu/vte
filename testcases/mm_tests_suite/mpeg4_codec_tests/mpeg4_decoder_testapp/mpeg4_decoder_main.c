/*/
  Copyright (C) 2004, Freescale Semiconductor, Inc. All Rights Reserved
  THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
  BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
  Freescale Semiconductor, Inc.
*/

/**
@file   mpeg4_decoder_main.c

@brief VTE C main source template

Description of the file

@par Portability:
        Indicate if this module is portable to other compilers or platforms.
        If not, indicate specific reasons why is it not portable.
*/

/* REVISION HISTORY 

Author (core ID)      Date         CR Number    Description of Changes
-------------------   ----------   ----------   ------------------------------
Filinova Natalya      15/02/2005   TLSbo47115   Initial version
Filinova Natalya      28/02/2005   TLSbo47115   Initial version
Delaspre/rc149c                    TLSbo47115   update copyrights with Freescale
D.Simakov/smkd001c    16/12/2005   TLSbo59667   Advanced decode w skip fixed
*/

/*
Total Tests: 1

Test Name:   mpeg4_decoder_testapp

Test Assertion
& Strategy:  A brief description of the test Assertion and Strategy
             TO BE COMPLETED
*/


#ifdef __cplusplus
extern "C"{
#endif

/* INCLUDE FILES */
/* Standard Include Files */
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

/* Harness Specific Include Files. */
#include "test.h"
#include "usctest.h"

/* Verification Test Environment Include Files */
#include "mpeg4_decoder_test.h"
#include "mpeg4_dec_api.h"

/* LOCAL CONSTANTS */


/* LOCAL MACROS */

/* LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS) */


/* LOCAL VARIABLES */
int testcase_flag  0;   /* binary flags: opt or not       */
int iter_flag  0;    /* binary flags: opt or not     */
int output_flag  0;  /* flag for output data forming      */
int skip_mode_flag  0;  /* flag for decoding with skipping frames   */
int start_frame_flag  0;  /* flag for skipping frames from some frame */
int number_to_skip_flag  0;  /* flag for numer of frames to skip      */
int time_flag  0;            /* flag for displaying time of decoding     */
int input_fname_flag  0;  /* flag for name of input bitstream         */
int delay_flag  0;              /* flag for delay between displayed frames  */
int comm_flag  0;               /* flag for displaying comments */
int dir_flag  0;                /* flag for input directory with bitstreams */
int skip_at_flag  0;      /* skip to I or P frame. */

char * testcase_opt;        /* Id option arguments               */
char * iter_opt;            /* Iter option arguments             */
char * output_opt;          /* output data form option arguments */
char * skip_mode_opt;       /* skip mode option arguments        */
char * start_frame_opt;     /* start frame for skipping option arguments */
char * number_to_skip_opt;  /* number frames to skip option arguments     */
char * input_fname_opt;     /* input bitstream name option arguments     */
char * dir_opt;             /* input directory option arguments */


option_t options[] 
{
    { "T:", &testcase_flag,       &testcase_opt      }, /* argument required */
    { "N:", &iter_flag,           &iter_opt          }, /* argument required */
    { "O:", &output_flag,         &output_opt        }, /*output data form*/
    { "M:", &skip_mode_flag,      &skip_mode_opt     }, /*mode of skipping*/
    { "S:", &start_frame_flag,    &start_frame_opt   }, /*start of skipping from frame*/
    { "L:", &number_to_skip_flag, &number_to_skip_opt}, /*number frames to skip*/
    { "R" , &time_flag,           NULL               }, /*displaying decode time*/
    { "F:", &input_fname_flag,    &input_fname_opt   }, /*input bitstream name*/
    { "W" , &delay_flag,          NULL               }, /*delay beetween displaying frames*/
    { "C" , &comm_flag,           NULL               }, /*displaying comments of decode process*/
    { "D:", &dir_flag,            &dir_opt           }, /*input directory with bitstreams */
    { "s",  &skip_at_flag,  NULL               },
    { NULL, NULL,                 NULL               }  /* NULL required to end array */
};


/* GLOBAL CONSTANTS */


/* GLOBAL VARIABLES */
/* Extern Global Variables */
extern int  Tst_count;               /* counter for tst_xxx routines.         */
extern char *TESTDIR;                /* temporary dir created by tst_tmpdir() */

/* Global Variables */
char *TCID      "mpeg4_decoder_testapp";  /* test program identifier.              */
int  TST_TOTAL  1;                     /* total number of tests in this file.   */


/* LOCAL FUNCTION PROTOTYPES */
void cleanup();
void setup();
int main( int argc, char ** argv );

/* LOCAL FUNCTIONS */

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

void cleanup()
{
    /* VTE : Actions needed to get a stable target environment */
    int VT_rv  TFAIL;

    VT_rv  VT_mpeg4_decoder_cleanup();
    if( VT_rv ! TPASS )
    {
     tst_resm( TWARN, "VT_mpeg4_decoder_cleanup() Failed : error code  %d", VT_rv );
    }

    tst_exit();
}

/* help */
/**
@brief  Inform of the available options and the associated parameters

@param  Input :      None.
        Output:      None.

@return None.
*/

void help()
{
    printf( "Switches (names may be abbreviated):\n\n" );
    printf( "-T x \tTestcase Id of the test according to the test plan.\n" );
    printf( "-N n \tn iterations of the loop in case of an endurance/stress test.\n" );
    printf( "-O x \tOutput form x of output data.\n");
    printf( "-M x \tMode x of skipping frame: to skip to an INTRA Frame or specified number of frames.\n");
    printf( "-S x \tSkips beginning from x frame.\n");
    printf( "-L n \tNumber of frames to skip.\n");
    printf( "-R   \tDisplaying decoding time.\n");
    printf( "-F x \tx - input bitstream.\n");
    printf( "-W   \tMode of doing delay between frames.\n");
    printf( "-C   \tDisplaying comments of decode process.\n");
    printf( "-D x \tDirectory x (full path) where are input bistreams.\n");
    printf( "-s   \tSkip decoding until I frame. By default it will skip to until P frame.\n" );
}



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

void setup()
{
    int VT_rv  TFAIL;

    /* VTE : Actions needed to prepare the test running */
    VT_rv  VT_mpeg4_decoder_setup();
    if( VT_rv ! TPASS )
    {
     tst_brkm( TBROK , cleanup, "VT_mpeg4_decoder_setup() Failed : error code  %d", VT_rv );
    }
    /* VTE */

    return;
}



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
    -l - Number of iteration
    -v - Prints verbose output
    -V - Prints the version number

@return On failure - Exits by calling cleanup().
        On success - exits with 0 exit value.
*/

int main( int argc, char ** argv )
{

    int VT_rv  TFAIL;

    /* parse options. */

    char * msg;
    int testcase        NOMINAL_FUNCTIONALITY;    /* default nominaly functionality test       */
    int iter            DEFAULT_ITERATIONS;       /* default 10 iterations for decode process  */
    int output          WITHOUT_OUTPUT;           /* default without forming output data       */
    int skip_mode       SKIP_TO_INTRA;            /* default skipping to INTRA frame           */
    int start_frame     DEFAULT_START_FRAME;      /* default skipping from 1st frame           */
    int number_to_skip  DEFAULT_NUM_TO_SKIP;      /* default 5 frames to skip                  */
    //int time            DEFAULT_DISPLAY_TIME;     /* default no displaing decode time          */
    //int delay           DEFAULT_DELAY;            /* default no delay between showed frames    */
    //int comm            DEFAULT_COMM;             /* default no comments during decode process */
    char * input_fname  DEFAULT_INPUT_BITSTREAM;  /* default input bitstream name              */
    char * input_dir_path  DEFAULT_PATH;          /* default input directory path              */

    /* parse options. */

    if( NULL ! (msg  parse_opts( argc, argv, options, help )) )
    {
        tst_brkm( TBROK, cleanup, "OPTION PARSING ERROR - %s", msg );
    }

    if( testcase_flag )
    {
 testcase  atoi( testcase_opt );
    }

    if( iter_flag )
    {
        iter  atoi( iter_opt );
    }

    if(output_flag)
    {
 output  atoi(output_opt);
    }

    if(skip_mode_flag)
    {
 skip_mode  atoi(skip_mode_opt);
    }

    if(start_frame_flag)
    {
        start_frame  atoi(start_frame_opt);
    }

    if(number_to_skip_flag)
    {
        number_to_skip   atoi(number_to_skip_opt);
    }

    if(input_fname_flag)
    {
 input_fname  NULL;
        input_fname  input_fname_opt;
    }

    if(dir_flag)
    {
 input_dir_path  NULL;
 input_dir_path  dir_opt;
    }

    skip_at_flag  skip_at_flag ? MPEG4D_START_DECODE_AT_IFRAME : MPEG4D_START_DECODE_AT_PFRAME;

    /** LTP test harness provides a function called parse_opts() that
    may be used to parse standard options. For a list of standard
    option that are available refer on-line man page at the LTP
    web-site */

    /* perform global test setup, call setup() function. */
    setup();

    /* Print test Assertion using tst_resm() function with argument TINFO. */
    tst_resm( TINFO, "Testing if %s test case is OK", TCID );

    /* VTE : print results and exit test scenario */
    VT_rv  VT_mpeg4_decoder_test( testcase, iter, output, skip_mode,
                                   start_frame, number_to_skip,time_flag,
       input_fname,delay_flag,comm_flag,input_dir_path); /* with the parameters needed come from parse_opt()) */


    if( VT_rv  TPASS )
        tst_resm( TPASS, "%s test case worked as expected", TCID );
    else
        tst_resm( TFAIL, "%s test case did NOT work as expected", TCID );


    cleanup(); /** OR tst_exit(); */
    /* VTE */

    return VT_rv;
}
