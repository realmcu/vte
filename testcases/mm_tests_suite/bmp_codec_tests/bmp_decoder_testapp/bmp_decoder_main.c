/*================================================================================================*/
/**
    @file   bmp_decoder_main.c

    @brief  LTP Motorola template.
*/
/*==================================================================================================

  Copyright (C) 2004, Freescale Semiconductor, Inc. All Rights Reserved
  THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
  BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
  Freescale Semiconductor, Inc.
     
====================================================================================================
Revision History:
                            Modification     Tracking
Author                          Date          Number    Description of Changes
-------------------------   ------------    ----------  -------------------------------------------
D.Simakov / smkd001c         12/07/2004     TLSbo40263   Initial version 
D.Simakov / smkd001c         02/03/2005     TLSbo47117   Update
D.Simakov / smkd001c         02/06/2005     TLSbo50899   Robustness test case was improved

====================================================================================================
Portability: Indicate if this module is portable to other compilers or platforms. 
             If not, indicate specific reasons why is it not portable.

==================================================================================================*/

/*==================================================================================================
Total Tests: TO BE COMPLETED

Test Name:   TO BE COMPLETED

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
#include "bmp_decoder_test.h"

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
/* Extern Global Variables */
extern int  Tst_count;               /* counter for tst_xxx routines.         */
extern char *TESTDIR;                /* temporary dir created by tst_tmpdir() */

/* Global Variables */
char *TCID     = "bmp_decoder_testapp"; /* test program identifier.          */
int  TST_TOTAL = 1;                  /* total number of tests in this file.   */

int testcase_flag = 0; 
int iter_flag = 0;
int cfg_flag = 0;
int rfile_flag = 0;
char * testcase_opt;   
char * iter_opt; 
char * cfg_opt;
char * rfile_opt;

option_t options[] = 
{
    { "T:",  &testcase_flag, &testcase_opt  }, 
    { "N:",  &iter_flag,     &iter_opt      }, 
    { "C:",  &cfg_flag,      &cfg_opt       }, 
    { "F:",  &rfile_flag,    &rfile_opt     },
    { NULL,  NULL,           NULL           }  
};

extern char * word_file;
extern char * word_file_with_bmp_hdr;

/*==================================================================================================
                                   GLOBAL FUNCTION PROTOTYPES
==================================================================================================*/
void cleanup();
void setup();
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
void cleanup()
{
	int VT_rv = TFAIL;
		
	VT_rv = VT_bmp_decoder_cleanup();
	if( VT_rv != TPASS )
	{
		tst_resm( TWARN, "VT_cleanup() Failed : error code = %d", VT_rv );
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
void help()
{
	printf("Switches (names may be abbreviated):\n\n");
  	printf("-T <test case id> Id of the test according to the test plan\n");
    // which???
  	printf("-C <config>       Name of config file\n");
  	printf("-N <iter>         Inform the iteration of the loop in case of an endurance/stress test\n");
    printf("-F <fname>        Fileanme of input file for the robustness test case\n");
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
void setup()
{
 	int VT_rv = TFAIL;

	VT_rv = VT_bmp_decoder_setup();
	if( VT_rv != TPASS )
	{
		tst_brkm( TBROK , cleanup, "VT_setup() Failed : error code = %d", VT_rv );
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
int main( int argc, char **argv )
{
	int VT_rv = TFAIL;
		
/* parse options. */
    char * msg;
    int testcase = NOMINAL_FUNCTIONALITY;
    int iter     = ITERATIONS;
    const char * cfg_file = NULL;
		
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
    if( cfg_flag )
    {
    	cfg_file = cfg_opt;
    }
    else if( ROBUSTNESS_1 != testcase && ROBUSTNESS_2 != testcase )
    {
        tst_brkm(TBROK, cleanup, "Required argument -C");
    }
    if( ROBUSTNESS_1 == testcase )
    {
        if( !rfile_flag )
            tst_brkm(TBROK, cleanup, "Required argument -F");
        else
        {
            word_file = strdup( rfile_opt );
            if( !word_file )
                tst_brkm(TBROK, cleanup, "Fatal error, can't allocate memory");
        }            
    }
    else if( ROBUSTNESS_2 == testcase )
    {
        if( !rfile_flag )
            tst_brkm(TBROK, cleanup, "Required argument -F");
        else
        {
            word_file_with_bmp_hdr = strdup( rfile_opt );
            if( !word_file_with_bmp_hdr )
                tst_brkm(TBROK, cleanup, "Fatal error, can't allocate memory");
        }                    
    }
    
    /* Check if the file is exist. */
    if( word_file || word_file_with_bmp_hdr )
    {    
        FILE * test = fopen( word_file ? word_file : word_file_with_bmp_hdr, "r" );
        if( !test )
        {
            tst_brkm(TBROK, cleanup, "Invalid arg -F: %s does not exist", word_file ? word_file : word_file_with_bmp_hdr );
        }
        fclose( test );
    }
    	
    /* perform global test setup, call setup() function. */
	setup();

    /* Print test Assertion using tst_resm() function with argument TINFO. */
	tst_resm( TINFO, "Testing if %s test case is OK", TCID );

    /* Test Case Body. */

    /* VTE : print results and exit test scenario */
	VT_rv = VT_bmp_decoder_test( testcase, iter, cfg_file ); /*with the parameters needed come from parse_opt())*/
	
	if( VT_rv == TPASS )
		tst_resm( TPASS, "%s test case worked as expected", TCID );
	else
		tst_resm( TFAIL, "%s test case did NOT work as expected", TCID );
		
		
	cleanup(); /** OR tst_exit(); */

    /* VTE */
	
	return VT_rv;
	
}
