/*================================================================================================*/
/**
    @file   opl_chain_main.c

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
D.SIMAKOV/smkd001c           11/07/2004     TLSbo41679  Initial version 

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
#include "opl_chain_test.h"

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
char *TCID     = "opl_chain_testapp"; /* test program identifier.          */
int  TST_TOTAL = 1;                   /* total number of tests in this file.   */

int op_seq_flag = 0; /* binary flags: opt or not */
char *op_seq;        /* Id option arguments */
int fmt_flag = 0;
char * fmt;

option_t options[] = 
{
    { "S:", &op_seq_flag, &op_seq  },	/* opl operations sequense */
    { "D:", &fmt_flag, &fmt },        	/* color depth */
    { NULL, NULL, NULL } 		/* NULL required to end array */
};

/*==================================================================================================
                                   GLOBAL FUNCTION PROTOTYPES
==================================================================================================*/
void cleanup();
void setup();
int main( int argc, char **argv );
char* generate_sequence( int len );

/*==================================================================================================
                                   LOCAL FUNCTION PROTOTYPES
==================================================================================================*/


/*==================================================================================================
                                       GLOBAL FUNCTIONS
==================================================================================================*/

void help()
{
    printf("Options:\n\n");
    printf("-S {[m][M][r][R]} \n\tDetermine wich operation perform \n");
    printf("-D depth \n\tColor depth (8,16,24,32) \n");

}

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
	
    VT_rv = VT_opl_chain_cleanup();
    if ( VT_rv != TPASS )
    {
	tst_resm( TWARN, "VT_opl_chain_cleanup() Failed : error code = %d", VT_rv );
    }
    
    /* VTE */
		
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
void setup()
{
    int VT_rv = TFAIL;

    /* VTE : Actions needed to prepare the test running */
    VT_rv = VT_opl_chain_setup();
    if( VT_rv != TPASS )
    {
	tst_brkm( TBROK , cleanup, "VT_TEMPLATE_sequp() Failed : error code = %d", VT_rv );
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
		    		-S - Sets an OPL operations sequense
				-D - Sets color depth of an input image
  
@return On failure - Exits by calling cleanup().
        On success - exits with 0 exit value.
*/
/*================================================================================================*/
int main(int argc, char **argv)
{
    int VT_rv = TFAIL;
		
    char * msg;
    char * rnd_op_seq = 0; 
    char * def_op_seq = "rmRM";
    char * op_seq_ref;
	
    int color_depth = 16;
		
    /* parse options. */
    if ( ( msg = parse_opts( argc, argv, options, help ) ) != NULL )
    {
        tst_brkm( TBROK, cleanup, "OPTION PARSING ERROR - %s", msg );
    }
    if( op_seq_flag )
    {
        if( isdigit(*op_seq) )
        {
	    rnd_op_seq = generate_sequence( atoi( op_seq ) );
	    op_seq_ref = rnd_op_seq;
	}
	else
	{
	    op_seq_ref = op_seq;
	}
    }
    else
	op_seq_ref = def_op_seq;
    if( fmt_flag )
    {
	color_depth = atoi( fmt );
    }
	
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
    VT_rv = VT_opl_chain_test( color_depth, op_seq_ref ); /*with the parameters needed come from parse_opt())*/
	
    if( VT_rv == TPASS )
        tst_resm( TPASS, "%s test case worked as expected", TCID );
    else
        tst_resm( TFAIL, "%s test case did NOT work as expected", TCID );
		
		
    if( rnd_op_seq )
	free( rnd_op_seq );
		
    cleanup(); /** OR tst_exit(); */
    /* VTE */
	
    return VT_rv;	
}

/* auxiliary function - check the parity */
int aindex(char* m)
{
   int flip = 0, sign_x = 0, sign_y = 0;
   int i;
    
   for( i = 0; m[i]; i++ )
    {
        switch( m[i] )
        {
            case 'R':
                flip = !flip;
                sign_x = !sign_x;
                sign_x ^= sign_y ^= sign_x ^= sign_y;
                break;
		
            case 'r':
                flip = !flip;
                sign_y = !sign_y;
                sign_x ^= sign_y ^= sign_x ^= sign_y;
                break;

            case 'M':
                sign_x = !sign_x;
                break;
		
            case 'm':
                sign_y = !sign_y;    		
        }
    }

   return( flip * 4 + sign_x * 2 + sign_y );
}
/*================================================================================================*/
/*===== generate_sequence =====*/
/**  author: D/Kazachkov (e1403c)
@brief  This function generates the sequense of operations, the picture
	integrity is guaranteed.

@param  Input :      len - the length of the sequence
        Output:      the sequence (can be 1 or 2 operations longer)
  
@return a pointer to allocated buffer - free() is needed
*/
/*================================================================================================*/

/* generate the squence */
char* generate_sequence( int len )
{
    static char ops[] = "RrMm";
    static char add_on[][8] = {"","m","M","RR","Rm","r","R","RM"};
    char* mem = (char*)malloc( len+3 );
    int i, j;
   
    for( i = 0; i < len; i++ )
    {
        j = rand() % 4;
        mem[i] = ops[j];
    }
    mem[i]=0;
    strcat( mem, add_on[aindex( mem )] );
    return mem;
}