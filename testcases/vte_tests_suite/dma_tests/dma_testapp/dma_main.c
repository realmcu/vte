/*
  Copyright (C) 2004, Freescale Semiconductor, Inc. All Rights Reserved
  THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
  BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
  Freescale Semiconductor, Inc.
*/

/**
@file dma_main.c

@par Portability:
        ARM GCC
*/

/*======== REVISION HISTORY ==========

Author (core ID)      Date         CR Number    Description of Changes
-------------------   ----------   ----------   ------------------------------
D.Simakov / b00236    31/08/2006   TLSbo76801   Initial version
=================*/

/*======================
                                        INCLUDE FILES
======================*/
/* Standard Include Files */
#include <stdio.h>

/* Harness Specific Include Files. */
#include "usctest.h"
#include "test.h"

/* Verification Test Environment Include Files */
#include "dma_test.h"

/*======================
                                   LOCAL FUNCTION PROTOTYPES
======================*/

void setup       ( void );
void cleanup     ( void );
int  main        ( int argc, char ** argv );
void GetOptions  ( int argc, char ** argv ); /* fills gTestappConfig */


/*======================
                                       GLOBAL VARIABLES
======================*/

char *          TCID                        = NULL;
int             TST_TOTAL                   = 1;
sTestappConfig  gTestappConfig;


/*======================
                                       LOCAL FUNCTIONS
======================*/

/*====================*/
/*====================*/
void help( void )
{
        printf( "Switches (names may be abbreviated):\n\n" );
        printf( "-T <test case id> Id of the test according to the test plan\n");
        printf( "                  0 - test 1d-1d linear DMA\n"
                "                  1 - test 2d-2d linear DMA\n"
                "                  2 - test 1d-2d linear DMA\n"
                "                  3 - test 2d-1d linear DMA\n"
                "                  4 - test DMA by your input\n"
                "                  5 - test Chain Buffer DMA\n"
                "                  6 - test 16 channel\n"
                "                  7 - test bus\n" );

        printf( "-C                User params string which must contain the following params\n"
                "                  srcmode dstmode srcport dstport X Y W count direction burstlength repeat\n"
                "                  for ex. -C\"0 0 0 0 0 0 0 1024 0 4 0\"" );
}


/*====================*/
/*====================*/
void setup( void )
{
        int rv = TFAIL;

        rv = VT_dma_setup();
        if( TPASS != rv )
        {
                tst_brkm( TBROK , cleanup, "VT_dma_setup() Failed : error code = %d", rv );
        }
}


/*====================*/
/*====================*/
void cleanup( void )
{
        int rv = TFAIL;

        rv = VT_dma_cleanup();
        if( TPASS != rv )
        {
                tst_resm( TWARN, "VT_dma_cleanup() Failed : error code = %d", rv );
        }

        tst_exit();
}


/*====================*/
/*====================*/
int main( int argc, char ** argv )
{
        int rv = TFAIL;

        /* Parse cmd line options and fill gTestappConfig. */
        GetOptions( argc, argv );

        /* Global test setup. */
        setup();

        tst_resm( TINFO, "Testing if %s test case is OK", TCID );
        fflush(stdout);

        /* Run test. */
        rv = VT_dma_test();

        /* Print the final test result. */
        if( rv == TPASS )
                tst_resm( TPASS, "%s test case worked as expected", TCID );
        else
                tst_resm( TFAIL, "%s test case did NOT work as expected", TCID );

        /* Global test cleanup.  */
        cleanup();

        return rv;
}

/*====================*/
/*====================*/
void GetOptions( int argc, char ** argv )
{
        /******************/
        /* Parse options. */
        /******************/

        int    testcaseFlag = 0;
        int    verboseFlag  = 0;
        int    cparamsFlag = 0;
        char * testcaseOpt;
        char * msg;

        option_t options[] =
        {
                { "T:",  &testcaseFlag,        &testcaseOpt                  },
                { "V",   &verboseFlag,         NULL                          },
                { "C:",  &cparamsFlag,         &gTestappConfig.mCustumParams },
                { NULL,  NULL,                 NULL                          }
        };

        extern void cleanup( void );
        if( (msg = parse_opts( argc, argv, options, help )) )
        {
                tst_brkm( TCONF, cleanup, "OPTION PARSING ERROR - %s", msg );
        }


        /*****************************************************/
        /* Fill up the gTestappConfig by the parsed options. */
        /*****************************************************/

        gTestappConfig.mTestCase = testcaseFlag ? atoi( testcaseOpt ) : 0;
        gTestappConfig.mVerbose  = verboseFlag;


        /**********************************************************/
        /* Check if all of the required arguments were presented. */
        /**********************************************************/

        if( gTestappConfig.mTestCase < 0 || gTestappConfig.mTestCase >= 7 )
        {
                help();
                tst_brkm( TCONF, cleanup, "INVALID ARGUMENT -T %d", gTestappConfig.mTestCase );
        }
        if( gTestappConfig.mTestCase == 5 && !gTestappConfig.mCustumParams )
        {
                tst_brkm( TCONF, cleanup, "ARGUMENT REQUIRED -C" );
        }
}
