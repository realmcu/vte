/*
  Copyright (C) 2004, Freescale Semiconductor, Inc. All Rights Reserved
  THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
  BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
  Freescale Semiconductor, Inc.
*/

/**
@file codec_main.c

@par Portability:
        ARM GCC
*/

/* REVISION HISTORY 

Author (core ID)      Date         CR Number    Description of Changes
-------------------   ----------   ----------   ------------------------------
D.Simakov/smkd001c    10/06/2005   TLSbo51185 Initial version
D.Simakov/smkd001c    23/06/2005   TLSbo51185   Working version
D.Simakov             10/05/2006   TLSbo66283   Phase2
*/

/*
                                        INCLUDE FILES
*/
/* Standard Include Files */
#include <stdio.h>

/* Harness Specific Include Files. */
#include "usctest.h"
#include "test.h"

/* Verification Test Environment Include Files */
#include "codec_test.h"


/*
                                       GLOBAL VARIABLES
*/

char *          TCID                         NULL;
int             TST_TOTAL                    1; /*LOAD + 1;*/
int             gNotSupportedTestCases[]     { -1 };
int             gNotSupportedTestCasesCount  sizeof(gNotSupportedTestCases)/sizeof(int);
sTestappConfig  gTestappConfig;


/*
                                       LOCAL FUNCTIONS
*/

/**/
/**/
void help( void )
{
        printf( "Switches (names may be abbreviated):\n\n" );
        printf( "-T <test case id> Id of the test according to the test plan\n");
        printf( "                  0 - Nominal functionality test\n"
                "                  1 - Robustness\n"
                "                  2 - Relocatability test\n"
                "                  3 - Re-entrance test\n"
                "                  4 - Pre-emption test\n"
                "                  5 - Endurance test\n"
                "                  6 - Load test\n" );
        printf( "-C <config>       Name of the config file\n" );
        printf( "-N <iter>         Inform the iteration of the loop in case of an endurance/stress test\n" );
        printf( "-V                Verbose mode\n" );
        printf( "-S                Slow bit-matching\n" );
}


/**/
/**/
void GetOptions( int argc, char ** argv )
{
        /******************/
        /* Parse options. */
        /******************/

        int    testcaseFlag         0;
        int    iterFlag             0;
        int    cfgFlag              0;
        int    verboseFlag          0;
        int    delayFlag            0;
        int    lcdFlag              0;
        char * testcaseOpt;
        char * iterOpt;
        char * cfgOpt;
        char * delayOpt;
        char * msg;

        option_t options[] 
        {
                { "T:",  &testcaseFlag,        &testcaseOpt },
                { "N:",  &iterFlag,            &iterOpt     },
                { "C:",  &cfgFlag,             &cfgOpt      },
                { "V",   &verboseFlag,         NULL         },
                { "D:",  &delayFlag,           &delayOpt    },
                { "L",   &lcdFlag,             NULL         },
                { NULL,  NULL,                 NULL         }
        };

        extern void cleanup( void );
        if( (msg  parse_opts( argc, argv, options, help )) )
        {
                tst_brkm( TCONF, cleanup, "OPTION PARSING ERROR - %s", msg );
        }


        /*****************************************************/
        /* Fill up the gTestappConfig by the parsed options. */
        /*****************************************************/

        gTestappConfig.mTestCase                  testcaseFlag ? atoi( testcaseOpt ) : NOMINAL_FUNCTIONALITY;
        gTestappConfig.mNumIter                   iterFlag ? atoi( iterOpt ) : DEFAULT_ITERATIONS;
        gTestappConfig.mConfigFilename            cfgFlag ? cfgOpt : NULL;
        gTestappConfig.mVerbose                   verboseFlag;
        gTestappConfig.mSlowBitMatching           FALSE;
        gTestappConfig.mDelay                     delayFlag ? atoi( delayOpt ) : 0;
        gTestappConfig.mDisableLCD                lcdFlag;


        /**********************************************************/
        /* Check if all of the required arguments were presented. */
        /**********************************************************/

        if( !gTestappConfig.mConfigFilename )
                tst_brkm( TCONF, cleanup, "Argument required -C" );


        /**********************************************/
        /* Select the TCID for extra test cases here. */
        /**********************************************/
}
