/*================================================================================================*/
/**
    @file   amr_decoder_main.c

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
F.GAFFIE/rb657c    21/07/2004     TLSbo40930  Initial version 

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
#include "amr_decoder_test.h"

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
char *TCID     = "amr_decoder_testapp"; /* test program identifier.          */
int  TST_TOTAL = 1;                  /* total number of tests in this file.   */

int Idflag = 0, Caseflag = 0, Iterflag = 0;       /* binary flags: opt or not */
char *Idopt;             	/* Id option arguments */
char *Caseopt;		/* Case option arguments */
char *Iteropt;		/* Iter option arguments */

option_t options[] = {
        { "T:", &Idflag, &Idopt  },          		/* argument required */
        { "C:", &Caseflag, &Caseopt },        	/* argument required */
        { "N:", &Iterflag, &Iteropt },        	/* argument required */
        { NULL, NULL, NULL }            			/* NULL required to end array */
};

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
    /* VTE : Actions needed to get a stable target environment */
		int VT_rv = TFAIL;
		
		VT_rv = VT_amr_decoder_cleanup();
		if (VT_rv != TPASS)
		{
			tst_resm(TWARN, "VT_cleanup() Failed : error code = %d", VT_rv);
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
	tst_resm(TINFO, "Switches (names may be abbreviated):\n");
  	tst_resm(TINFO, "- T Id	Id of the test according to the test plan");
  	tst_resm(TINFO, "-C case	If exist, the test case number associated with the test Id");
  	tst_resm(TINFO, "-N iter	Inform the iteration of the loop in case of an endurance/stress test");
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
		VT_rv = VT_amr_decoder_setup();
		if (VT_rv != TPASS)
		{
			tst_brkm(TBROK , cleanup, "VT_setup() Failed : error code = %d", VT_rv);
		}
		/* VTE */
    
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
	int VT_rv = TFAIL;
	char *msg;
	int Id = EXHAUSTIVE_DECODE;
	int Case = USING_THREAD;
	int Iter = ITERATIONS;
		
/* parse options. */
	if ( (msg=parse_opts(argc, argv, options, help)) != NULL ){
		tst_brkm(TBROK, cleanup, "OPTION PARSING ERROR - %s", msg);
	}
	if(Idflag){
		Id = atoi(Idopt);
		tst_resm(TINFO, " Idflag = %d", Id);
	}
	if(Caseflag){
		Case = atoi(Caseopt);
		tst_resm(TINFO, " Caseflag = %d", Case);
	}
	if(Iterflag){
		Iter = atoi(Iteropt);
		tst_resm(TINFO, " Iterflag = %d", Iter);
	}
	
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
	VT_rv = VT_amr_decoder_test(Id, Case, Iter); /*with the parameters needed come from parse_opt())*/
	
	if(VT_rv == TPASS)
		tst_resm(TPASS, "%s test case worked as expected", TCID);
	else
		tst_resm(TFAIL, "%s test case did NOT work as expected", TCID);
		
		
	cleanup(); /** OR tst_exit(); */

/* VTE */
	
	return VT_rv;
	
}
