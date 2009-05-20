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
F.GAFFIE/rb657c       03/05/2004   TLSbo39336   Initial version
D.Simakov/smkd001c    07/02/2005   TLSbo47179   Bad dependancies in the mm tests
                                                application build process
D.Simakov/smkd001c    25/10/2005   TLSbo59191   Improved
D.Simakov/smkd001c    24/01/2006   TLSbo61035   Centralization of common features
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

char *          TCID              NULL;
int             TST_TOTAL         1; /*LOAD + 1;*/
sTestappConfig  gTestappConfig;
int             gNotSupportedTestCases[]  { -1 };
int             gNotSupportedTestCasesCount  sizeof(gNotSupportedTestCases)/sizeof(int);


/*
                                       LOCAL FUNCTIONS
*/

/**/
/**/
void help()
{
        printf( "Switches (names may be abbreviated):\n\n" );
        printf( "-T <test case id> Id of the test according to the test plan\n");
        printf( "                  0 - Nominal functionality test\n"
                "                  1 - Thumb encoding\n"
                "                  2 - Relocatability test\n"
                "                  3 - Re-entrance test\n"
                "                  4 - Pre-emption test\n"
                "                  5 - Endurance test\n"
                "                  6 - Load test\n" );
        printf( "-C <config>       Name of the config file\n" );
        printf( "-N <iter>         Inform the iteration of the loop in case of an endurance/stress test\n" );
        printf( "-V                Verbose mode\n" );
        printf( "-F                Frame-level API\n" );
        printf( "-S                Slow bit-matching\n" );
}


/**/
/**/
void GetOptions( int argc, char ** argv )
{

        /* parse options. */
        int    testcaseFlag         0;
        int    iterFlag             0;
        int    cfgFlag              0;
        int    verboseFlag          0;
        int    slowBitmatchingFlag  0;
        int    frameLevelApiFlag    0;
        char * testcaseOpt;
        char * iterOpt;
        char * cfgOpt;
        char * msg;

        option_t options[] 
        {
                { "T:",  &testcaseFlag,        &testcaseOpt },
                { "N:",  &iterFlag,            &iterOpt     },
                { "C:",  &cfgFlag,             &cfgOpt      },
                { "V",   &verboseFlag,         NULL         },
                { "S",   &slowBitmatchingFlag, NULL         },
                { "F",   &frameLevelApiFlag,   NULL         },
                { NULL,  NULL,                 NULL         }
        };

        /* parse options. */
        extern void cleanup( void );
        if( (msg  parse_opts( argc, argv, options, help )) )
        {
                tst_brkm( TCONF, cleanup, "OPTION PARSING ERROR - %s", msg );
        }

        /* Fill the gTestappConfig by the parsed options. */
        gTestappConfig.mTestCase                  testcaseFlag ? atoi(testcaseOpt) : NOMINAL_FUNCTIONALITY;
        gTestappConfig.mNumIter                   iterFlag ? atoi(iterOpt) : DEFAULT_ITERATIONS;
        gTestappConfig.mConfigFilename            cfgFlag ? cfgOpt : NULL;
        gTestappConfig.mVerbose                   verboseFlag;
        gTestappConfig.mSlowBitMatching           slowBitmatchingFlag;
        gTestappConfig.mFrameLevelApi             frameLevelApiFlag;

        /* Check if all of the required arguments were presented */
        if( !gTestappConfig.mConfigFilename )
                tst_brkm( TCONF, cleanup, "Argument required -C" );

        /* Select the TCID for extra test cases here: */
        if( THUMB_ENCODING  gTestappConfig.mTestCase )
                TCID  "thumb";
}


