/*
  Copyright (C) 2004, Freescale Semiconductor, Inc. All Rights Reserved
  THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
  BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
  Freescale Semiconductor, Inc.
*/

/**
@file common_codec_main.c

@par Portability:
        ARM GCC
*/

/*======================== REVISION HISTORY ==================================

Author (core ID)      Date         CR Number    Description of Changes
-------------------   ----------   ----------   ------------------------------
D.Simakov / smkd001c  24/01/2006   TLSbo61035   Initial version
=============================================================================*/


/*==================================================================================================
                                        INCLUDE FILES
==================================================================================================*/

/* Standard Include Files */
#include <stdio.h>
#include <assert.h>
    
/* Harness Specific Include Files. */
#include "test.h"

/* Verification Test Environment Include Files */
#include "codec_test.h"


/*==================================================================================================
                                       GLOBAL VARIABLES
==================================================================================================*/

extern char * TCID;         
extern int    gNotSupportedTestCases[];
extern int    gNotSupportedTestCasesCount;


/*==================================================================================================
                                   LOCAL FUNCTION PROTOTYPES
==================================================================================================*/

void setup       ( void );
void cleanup     ( void );
int  main        ( int argc, char ** argv );
void GetOptions  ( int argc, char ** argv ); /* fills gTestappConfig */


/*==================================================================================================
                                       LOCAL FUNCTIONS
==================================================================================================*/

/*================================================================================================*/
/*================================================================================================*/
void setup( void )
{
        int rv = TFAIL;
        
        rv = VT_codec_setup();
        if( TPASS != rv )
        {
                tst_brkm( TBROK , cleanup, "VT_codec_setup() Failed : error code = %d", rv );
        }
}


/*================================================================================================*/
/*================================================================================================*/
void cleanup( void )
{
        int rv = TFAIL;
        
        rv = VT_codec_cleanup();
        if( TPASS != rv )
        {
                tst_resm( TWARN, "VT_codec_cleanup() Failed : error code = %d", rv );
        }
        
        tst_exit();
}


/*================================================================================================*/
/*================================================================================================*/
int main( int argc, char ** argv )
{   
        int rv = TFAIL;
        
        /* Parse cmd line options and fill gTestappConfig. */
        GetOptions( argc, argv );   
        
        /* Select test case name */
        switch( gTestappConfig.mTestCase )
        {
        case NOMINAL_FUNCTIONALITY:
                TCID = "nominal";
                break;                
        case RELOCATABILITY:
                TCID = "relocatability";
                break;                        
        case RE_ENTRANCE:
                TCID = "re-entrance";
                break;
        case PRE_EMPTION:
                TCID = "pre-emption";
                break;        
        case ENDURANCE:
                TCID = "endurance";
                break;                                    
        case LOAD:
                TCID = "load";
                break;
        default:
                if( !TCID )
                        tst_brkm( TBROK, cleanup, "Unknown test case (-T %d)", gTestappConfig.mTestCase );
        }

        /* Check if the selected test case is allowed. */
        int i;        
        for( i = 0; i < gNotSupportedTestCasesCount; ++i )
        {
                if( gNotSupportedTestCases[i] == gTestappConfig.mTestCase )
                        tst_brkm( TBROK, cleanup, "%s test case (-T %d) is not supported", TCID, gTestappConfig.mTestCase );
        }
        
        /* Global test setup. */
        setup();
        
        tst_resm( TINFO, "Testing if %s test case is OK", TCID );
        fflush(stdout);
        
        /* Run test. */
        rv = VT_codec_test();
        
        /* Print the final test result. */
        if( rv == TPASS )
                tst_resm( TPASS, "%s test case worked as expected", TCID );
        else
                tst_resm( TFAIL, "%s test case did NOT work as expected", TCID );
        
        /* Global test cleanup.  */
        cleanup(); 
        
        return rv;
}
