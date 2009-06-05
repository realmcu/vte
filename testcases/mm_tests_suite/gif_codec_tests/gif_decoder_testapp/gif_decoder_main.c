/*  Copyright (C) 2004, Freescale Semiconductor, Inc. All Rights Reserved
  THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
  BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
  Freescale Semiconductor, Inc.
*/

/**
@file gif_decoder_main.c

@brief VTE C main source template

Description of the file

@par Portability:
        Indicate if this module is portable to other compilers or platforms.
        If not, indicate specific reasons why is it not portable.
*/

/*======================== REVISION HISTORY ==================================

Author (core ID)      Date         CR Number    Description of Changes
-------------------   ----------   ----------   ------------------------------
E.Gromazina           25/02/2005   TLSbo47116   Initial version.
D.Simakov / smkd001c  07/04/2005   TLSbo47116   The endurance and load testcases 
                                                were added. Framebuffer's support 
                                                was added.
=============================================================================*/

/*==================================================================================================
Total Tests: TO BE COMPLETED

Test Name:   gif_decoder_testapp

Test Assertion
& Strategy:  A brief description of the test Assertion and Strategy
             TO BE COMPLETED
==================================================================================================*/


#ifdef __cplusplus
extern "C"{
#endif

/*==================================================================================================
                                        INCLUDE FILES
==================================================================================================*/
/* Standard Include Files */
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
    
/* Harness Specific Include Files. */
#include "test.h"
#include "usctest.h"

/* Verification Test Environment Include Files */
#include "gif_decoder_test.h"

/*======================== GLOBAL VARIABLES =================================*/

/* Extern Global Variables */
extern int  Tst_count;               /* counter for tst_xxx routines.         */
extern char *TESTDIR;                /* temporary dir created by tst_tmpdir() */

/* Global Variables */
char *TCID     = "gif_decoder_test"; /* test program identifier.          */
int  TST_TOTAL = 1;                  /* total number of tests in this file.   */

int testcase_flag = 0; // binary flags: opt or not 
int iter_flag = 0;     // binary flags: opt or not 
int cfg_flag = 0;
int delay_flag = 0;
char * testcase_opt;   // Id option arguments 
char * iter_opt;       //Iter option arguments 
char * cfg_opt;        // cfg file name
char * delay_opt;
int delay_value = 5;

option_t options[] = 
{
    { "T:",  &testcase_flag, &testcase_opt  }, // argument required 
    { "N:",  &iter_flag,     &iter_opt      }, // argument required 
    { "C:",  &cfg_flag,     &cfg_opt        },  // argument required 
    { "D:",  &delay_flag,    &delay_opt     }, 
    { NULL,  NULL,           NULL           }  // NULL required to end array 
};


/*======================== LOCAL FUNCTION PROTOTYPES ========================*/

void setup();
void help();
int main( int argc, char ** argv );


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
void cleanup()
{

    /* VTE : Actions needed to get a stable target environment */
    int VT_rv = TFAIL;
		
    VT_rv = VT_gif_decoder_cleanup();
    if( VT_rv != TPASS )
    {
	tst_resm( TWARN, "VT_gif_decoder_cleanup() Failed : error code = %d", VT_rv );
    }
    
    tst_exit();
}

/*======================== LOCAL FUNCTION ========================*/
/*===== help =====*/
/**
@brief  Inform of the available options and the associated parameters

@param  Input :      None.
        Output:      None.
  
@return None.
*/
/*================================================================================================*/
void help()
{
    printf( "Switches (names may be abbreviated):\n\n" );
    printf( "-T testcase Id of the test according to the test plan\n" );
    printf( "-N iter	 Inform the iteration of the loop in case of an endurance/stress test\n" );
    printf( "-C config   The cfg_file's name\n" );
    printf( "-D delay    A delay in seconds between decoding\n" );
}


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
void setup()
{
    int VT_rv = TFAIL;
	
    /* VTE : Actions needed to prepare the test running */
    VT_rv = VT_gif_decoder_setup();
    if( VT_rv != TPASS )
    {
	tst_brkm( TBROK , cleanup, "VT_gif_decoder_setup() Failed : error code = %d", VT_rv );
    }
    /* VTE */
    
    return;
}


/*===== main =====*/
/**
@brief  Entry point to this test-case. It parses all the command line
        inputs, calls the global setup and executes the test. It logs
	the test status and results appropriately using the LTP API's
	On successful completion or premature failure, cleanup() func
	is called and test exits with an appropriate return code.

@param  Input :      argc   - number of command line parameters.
        Output:      **argv - pointer to the array of the command line parameters.
			      Describe input arguments to this test-case
				-T - Test case
				-N - Number of iterations
				-C - Cfg file name
  
@return On failure - Exits by calling cleanup().
        On success - exits with 0 exit value.
*/
/*================================================================================================*/
int main( int argc, char ** argv )
{
    int VT_rv = TFAIL;
		
    /* parse options. */
    
    char * msg;
    int testcase = NOMINAL_FUNCTIONALITY;
    int iter     = DEFAULT_ITERATIONS;
    
    /* parse options. */
    if( NULL != (msg = parse_opts( argc, argv, options, help )) )
    {
	    tst_brkm( TBROK, cleanup, "OPTION PARSING ERROR - %s", msg );
    }
    if( testcase_flag )
    {
    	testcase = atoi( testcase_opt );
    }
    if( iter_flag )
    {
    	iter = atoi( iter_opt );
    }
    if( !cfg_flag )
    {
        tst_brkm( TBROK, cleanup, "Argument required -C" );
    }
    if( delay_flag )
    {
        delay_value = atoi( delay_opt );
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
    VT_rv = VT_gif_decoder_test( testcase, iter, cfg_opt ); /* with the parameters needed come from parse_opt()) */
	
    if( VT_rv == TPASS )
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
