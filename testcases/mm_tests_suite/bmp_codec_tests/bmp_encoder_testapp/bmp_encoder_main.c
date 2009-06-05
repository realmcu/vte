/*
  Copyright (C) 2004, Freescale Semiconductor, Inc. All Rights Reserved
  THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
  BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
  Freescale Semiconductor, Inc.
*/
	
/**
@file bmp_encoder_main.c
	
@brief VTE C header template
	
@par Portability:
        Indicate if this module is portable to other compilers or platforms.
        If not, indicate specific reasons why is it not portable.
*/
	
/*======================== REVISION HISTORY ==================================
		
Author (core ID)      Date         CR Number    Description of Changes
-------------------   ----------   ----------   ------------------------------
D.Simakov / smkd001c  26/07/2004   TLSbo40263   Initial version 
D.Simakov / smkd001c  19/04/2005   TLSbo47117   Some new testcases were added
D.Simakov / smkd001c  27/07/2005   TLSbo52108   Color reduction test was added 
=============================================================================*/

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
#include <assert.h>
    
/* Harness Specific Include Files. */
#include "test.h"
#include "usctest.h"

/* Verification Test Environment Include Files */
#include "bmp_encoder_test.h"

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
extern char *TESTDIR;                /* temporary dir created by tst_tmpdir() */

/* Global Variables */
char * TCID = NULL;                   /* test program identifier.          */
int  TST_TOTAL = 1;                   /* total number of tests in this file.   */

int testcase_flag = 0; /* binary flags: opt or not */
int iter_flag = 0;     /* binary flags: opt or not */
int cfg_flag = 0;
char * testcase_opt;   /* Id option arguments */
char * iter_opt;       /* Iter option arguments */
char * cfg_opt;

option_t options[] = 
{
    { "T:",  &testcase_flag, &testcase_opt  }, 
    { "N:",  &iter_flag,     &iter_opt      }, 
    { "C:",  &cfg_flag,      &cfg_opt       }, 
    { NULL,  NULL,           NULL           }  
};

testapp_config_t testapp_cfg;

/*==================================================================================================
                                   GLOBAL FUNCTION PROTOTYPES
==================================================================================================*/
void cleanup();
void setup();
int main( int argc, char ** argv );

/*==================================================================================================
                                   LOCAL FUNCTION PROTOTYPES
==================================================================================================*/


/*==================================================================================================
                                       GLOBAL FUNCTIONS
==================================================================================================*/

/*================================================================================================*/
/*================================================================================================*/
void cleanup()
{
    int VT_rv = TFAIL;
		
    VT_rv = VT_bmp_encoder_cleanup();
    if( VT_rv != TPASS )
    {
	    tst_resm( TWARN, "VT_bmp_encoder_cleanup() Failed : error code = %d", VT_rv );
    }
    
    tst_exit();
}

/*==================================================================================================
                                       LOCAL FUNCTIONS
==================================================================================================*/


/*================================================================================================*/
/*================================================================================================*/
void help()
{
    printf( "Switches (names may be abbreviated):\n\n" );
    printf("-T <test case id> Id of the test according to the test plan\n");
    printf("-C <config>       Name of the config file\n");
    printf("-N <iter>         Inform the iteration of the loop in case of an endurance/stress test\n");    
}


/*================================================================================================*/
/*================================================================================================*/
void setup()
{
    int VT_rv = TFAIL;
	
    VT_rv = VT_bmp_encoder_setup();
    if( VT_rv != TPASS )
    {
    	tst_brkm( TBROK , cleanup, "VT_bmp_encoder_setup() Failed : error code = %d", VT_rv );
    }
    
    return;
}


/*================================================================================================*/
/*================================================================================================*/
int ask_user(char *question)
{
    unsigned char answer;
    int           ret = TRETR;                        
    do
    {
        tst_resm(TINFO, "%s [Y/N]", question);
        answer = fgetc(stdin);
        if (answer == 'Y' || answer == 'y')
            ret = TPASS;
        else if (answer == 'N' || answer == 'n')
            ret = TFAIL;
    }
    while (ret == TRETR);
        fgetc(stdin);       /* Wipe CR character from stream */
    return ret;
}

/*================================================================================================*/
/*================================================================================================*/
int main( int argc, char ** argv )
{
    int VT_rv = TFAIL;
		
    /* parse options. */
    
    char * msg;
    testapp_cfg.test_case = NOMINAL_FUNCTIONALITY;
    testapp_cfg.num_iter = DEFAULT_ITERATIONS;
    testapp_cfg.cfg_fname = NULL;
		
    /* parse options. */
    if( NULL != (msg = parse_opts( argc, argv, options, help )) )
    {
	    tst_brkm( TBROK, cleanup, "OPTION PARSING ERROR - %s", msg );
    }
    /* Test case */
    if( testcase_flag )
    {
	    testapp_cfg.test_case = atoi( testcase_opt );
    }
    /* iter  */
    if( iter_flag )
    {
	    testapp_cfg.num_iter = atoi( iter_opt );
    }
    /* config  */
    if( cfg_flag )
    {
	    testapp_cfg.cfg_fname = cfg_opt;
    }
    else 
    {
        tst_brkm( TBROK, cleanup, "Argument required -C" );
    }
    
    switch( testapp_cfg.test_case )
    {
        case NOMINAL_FUNCTIONALITY:
            TCID = strdup( "nominal" );
            break;
        case RE_ENTRANCE:
            TCID = strdup( "re-entrance" );
            break;
        case PRE_EMPTION:
            TCID = strdup( "pre-emption" );
            break;
        case ENDURANCE:
            TCID = strdup( "endurance" );
            break;
        case LOAD:
            TCID = strdup( "load" );                    
            break;
        case COLOR_REDUCTION:
            TCID = strdup( "color reduction" );            
            break;
        default:
            assert( !"unknown test case" );                                                        
    }

    /* perform global test setup, call setup() function. */
    setup();

    /* Print test Assertion using tst_resm() function with argument TINFO. */
    tst_resm( TINFO, "Testing if %s test case is OK", TCID );

    /* VTE : print results and exit test scenario */
    VT_rv = VT_bmp_encoder_test();

    /*	
    if( VT_rv == TPASS )
        tst_resm( TPASS, "%s test case worked as expected", TCID );
    else
        tst_resm( TFAIL, "%s test case did NOT work as expected", TCID );*/
        
    if( VT_rv == TPASS )
    {
        if( COLOR_REDUCTION == testapp_cfg.test_case )
        {
            if( ask_user( "Did this test case work as expected" ) == TPASS )
    		    tst_resm( TPASS, "%s test case worked as expected", TCID );
            else
                tst_resm( TFAIL, "%s test case did NOT work as expected", TCID );
        }
        else
            tst_resm( TPASS, "%s test case worked as expected", TCID );                    
    }
	else
		tst_resm(TFAIL, "%s test case did NOT work as expected", TCID);        
		
		
    if( TCID ) free( TCID );        
    cleanup(); 
	
    return VT_rv;	
}
