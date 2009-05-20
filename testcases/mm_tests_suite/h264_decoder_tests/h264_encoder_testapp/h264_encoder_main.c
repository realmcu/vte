/*
  Copyright (C) 2004, Freescale Semiconductor, Inc. All Rights Reserved
  THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
  BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
  Freescale Semiconductor, Inc.
*/

/**
@file h264_encoder_main.c

@brief VTE C header template

@par Portability:
        Indicate if this module is portable to other compilers or platforms.
        If not, indicate specific reasons why is it not portable.
*/

/* REVISION HISTORY 

Author (core ID)      Date         CR Number    Description of Changes
-------------------   ----------   ----------   ------------------------------
I.Inkina/nknl001      1/07/2005    TLSbo52105   Initial version
D.Simakov/smkd001c    22/08/2005   TLSbo53252   Re-written
*/

#ifdef __cplusplus
extern "C"{
#endif

/*
                                        INCLUDE FILES
*/
/* Standard Include Files */
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <assert.h>

/* Harness Specific Include Files. */
#include "usctest.h"

/* Verification Test Environment Include Files */
#include "h264_encoder_test.h"

/*
                                        LOCAL MACROS
*/


/*
                          LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
*/

/*
                                       LOCAL CONSTANTS
*/

/*
                                       LOCAL VARIABLES
*/


/*
                                       GLOBAL CONSTANTS
*/


/*
                                       GLOBAL VARIABLES
*/
/* Extern Global Variables */
extern int  Tst_count;               /* counter for tst_xxx routines.         */
extern char *TESTDIR;                /* temporary dir created by tst_tmpdir() */

/* Global Variables */
char * TCID  NULL;                   /* test program identifier.          */
int  TST_TOTAL  1;                   /* total number of tests in this file.   */

sTestappConfig gTestappConfig;

/*
                                   GLOBAL FUNCTION PROTOTYPES
*/
void cleanup();
void setup();
int main( int argc, char ** argv );

/*
                                   LOCAL FUNCTION PROTOTYPES
*/


/*
                                       GLOBAL FUNCTIONS
*/

/**/
/**/
void cleanup()
{
    int rv  TFAIL;

    rv  VT_h264_encoder_cleanup();
    if( rv ! TPASS )
    {
     tst_resm( TWARN, "VT_h264_encoder_cleanup() Failed : error code  %d", rv );
    }

    tst_exit();
}

/*
                                       LOCAL FUNCTIONS
*/


/**/
/**/
void help()
{
    printf( "Switches (names may be abbreviated):\n\n" );
    printf("-T <test case id> Id of the test according to the test plan\n");
    printf(                   "\t\t\t 0 - Nominal functionality\n"
                              "\t\t\t 1 - Relocatability\n"
                              "\t\t\t 2 - Re-entrance\n"
                              "\t\t\t 3 - Pre-emption\n"
                              "\t\t\t 4 - Endurance\n"
                              "\t\t\t 5 - Load\n" );
    printf("-C <config>       Name of the config file\n");
    printf("-N <iter>         Inform the iteration of the loop in case of an endurance/stress test\n");
    printf("-V                Verbose mode\n");

}


/**/
/**/
void setup()
{
    int rv  TFAIL;

    rv  VT_h264_encoder_setup();
    if( rv ! TPASS )
    {
    tst_brkm( TBROK , cleanup, "VT_h264_encoder_setup() Failed : error code  %d", rv );
    }

    return;
}


/**/
/**/
int main( int argc, char ** argv )
{
    int rv  TFAIL;

    /* parse options. */
    int    testcaseFlag  0;
    int    iterFlag      0;
    int    cfgFlag       0;
    int    obanFlag      0;
    int    verboseFlag   0;
    char * testcaseOpt;
    char * iterOpt;
    char * cfgOpt;
    char * obanOpt;
    char * msg;
    //int    isTestCaseAuto  TRUE; /*+ AskUser();*/

    option_t options[] 
    {
        { "T:",  &testcaseFlag, &testcaseOpt  },
        { "N:",  &iterFlag,     &iterOpt      },
        { "C:",  &cfgFlag,      &cfgOpt       },
        { "O:",  &obanFlag,     &obanOpt      },
        { "V",   &verboseFlag,  NULL          },
        { NULL,  NULL,          NULL          }
    };

    /* parse options. */
    if( NULL ! (msg  parse_opts( argc, argv, options, help )) )
    {
     tst_brkm( TBROK, cleanup, "OPTION PARSING ERROR - %s", msg );
    }

    /* Fill the gTestappConfig by the parsed options */
    gTestappConfig.mTestCase  testcaseFlag ? atoi(testcaseOpt) : NOMINAL_FUNCTIONALITY;
    gTestappConfig.mNumIter  iterFlag ? atoi(iterOpt) : DEFAULT_ITERATIONS;
    gTestappConfig.mConfigFilename  cfgFlag ? cfgOpt : NULL;
    gTestappConfig.mOutputBan  obanFlag ? atoi(obanOpt) : 0;
    gTestappConfig.mVerbose  verboseFlag;

    /* Check if all of the required arguments were presented */
    if( !gTestappConfig.mConfigFilename )
        tst_brkm( TBROK, cleanup, "Argument required -C" );

    /* Select test case name */
    switch( gTestappConfig.mTestCase )
    {
        case NOMINAL_FUNCTIONALITY:
            TCID  strdup( "nominal" );
            break;
        case RELOCATABILITY:
            TCID  strdup( "relocatability" );
            break;
        case RE_ENTRANCE:
            TCID  strdup( "re-entrance" );
            break;
        case PRE_EMPTION:
            TCID  strdup( "pre-emption" );
            break;
        case ENDURANCE:
            TCID  strdup( "endurance" );
            break;
        case LOAD:
            TCID  strdup( "load" );
            break;
        default:
            assert( !"unknown test case" );
    }

    /* perform global test setup, call setup() function. */
    setup();

    /* Print test Assertion using tst_resm() function with argument TINFO. */
    tst_resm( TINFO, "Testing if %s test case is OK", TCID );

    /* VTE : print results and exit test scenario */
    rv  VT_h264_encoder_test();

    if( rv  TPASS )
        tst_resm( TPASS, "%s test case worked as expected", TCID );
    else
        tst_resm( TFAIL, "%s test case did NOT work as expected", TCID );

    if( TCID ) free( TCID );
    cleanup();

    return rv;
}

#ifdef __cplusplus
}
#endif
