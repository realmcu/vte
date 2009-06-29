/***
**Copyright 2005-2009 Freescale Semiconductor, Inc. All Rights Reserved.
**
**The code contained herein is licensed under the GNU General Public
**License. You may obtain a copy of the GNU General Public License
**Version 2 or later at the following locations:
**
**http://www.opensource.org/licenses/gpl-license.html
**http://www.gnu.org/copyleft/gpl.html
**/
/*================================================================================================*/
/** 
        @file   ipu_pf_main.c 
 
        @brief  LTP Freescale Test V4L/IPU. 
====================================================================================================
Revision History:
                            Modification     Tracking
Author                          Date          Number    Description of Changes
-------------------------   ------------    ----------  -------------------------------------------
A.Smirnov                    26/05/2005     TLSbo49894   Initial version 
D.Simakov / smkd001c         21/09/2005     TLSbo55077   Re-written
E.Gromazina/NONE             10/01/2006     TLSbo61481   Clean the LCD after testing
D.Kardakov                   30/08/2006     TLSbo75997   The new option "-N" was added

====================================================================================================
Portability: ARM GCC
 
==================================================================================================*/

/*================================================================================================== 
Total Tests: 5
 
Test Name:   ipu_pf_testapp
==================================================================================================*/

/*==================================================================================================  
                                        INCLUDE FILES
==================================================================================================*/
/* Standard Include Files */
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <assert.h>
    
/* Harness Specific Include Files. */
#include <usctest.h>

/* Verification Test Environment Include Files */
#include "ipu_pf_test.h"

/*==================================================================================================
                                       GLOBAL VARIABLES
==================================================================================================*/
extern int  Tst_count;               /* counter for tst_xxx routines.             */
extern char *TESTDIR;                /* temporary dir created by tst_tmpdir(void) */

char * TCID = NULL;                   /* test program identifier.          */
int  TST_TOTAL = 5;                   /* total number of tests in this file.   */

sTestappConfig gTestappConfig;

/*==================================================================================================
                                   GLOBAL FUNCTION PROTOTYPES
==================================================================================================*/
void cleanup( void );
void setup( void );
int main( int argc, char ** argv );

/*==================================================================================================
                                   LOCAL FUNCTION PROTOTYPES
==================================================================================================*/
void help( void );
int AskUser( char * question );

/*================================================================================================*/
/*===== cleanup =====*/
/** 
@brief  Performs all one time clean up for this test on successful 
        completion,  premature exit or  failure. Closes all temporary 
        files, removes all temporary directories exits the test with 
        appropriate return code by calling tst_exit() function.cleanup 

@param  Input :      None. 
        Output:      None. 
     
@return None 
*/
/*================================================================================================*/
void cleanup( void )
{
        int rv = TFAIL;
        
        rv = VT_ipu_pf_cleanup();
        if( rv != TPASS )
        {
                tst_resm( TWARN, "VT_ipu_pf_cleanup() Failed : error code = %d", rv );
        }
        
        tst_exit();
}

/*================================================================================================*/
/*===== setup =====*/
/** 
@brief  Performs all one time setup for this test. This function is 
        typically used to capture signals, create temporary dirs 
        and temporary files that may be used in the course of this test. 
 
@param  Input :      None. 
        Output:      None. 
     
@return None 
*/
/*================================================================================================*/
void setup( void )
{
        int rv = TFAIL;
        
        rv = VT_ipu_pf_setup();
        if( rv != TPASS )
        {
                tst_brkm( TBROK , cleanup, "VT_ipu_pf_setup() Failed : error code = %d", rv );
        }
        
        return;
}

/*================================================================================================*/
/*===== help =====*/
/**  
@brief  Inform of the available options and the associated parameters  
  
@param  Input :      None.  
        Output:      None.  
    
@returns None  
*/
/*================================================================================================*/
void help( void )
{
        printf( "Switches (names may be abbreviated):\n\n" );
        printf( "-T <test case id> Id of the test according to the test plan:\n" );
        printf( "                        0 - disable all\n" );
        printf( "                        1 - MPEG4 deblock (default)\n" );
        printf( "                        2 - MPEG4 dering\n" );
        printf( "                        3 - MPEG4 deblock dering\n" );
        printf( "                        4 - H.264 deblock\n" );
        printf( "                        5 - H.264 deblock with pause and resume\n" );
        printf( "-F <filename>     Name of an input yuv420 file\n" );
        printf( "-O <filename>     Name of an output yuv420 file\n" );
        printf( "-N                Number of image filtering\n");
        printf( "-V                Verbose mode\n" );
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
                                -T  test_case - exec test case by prefix 
     
@return On failure - Exits by calling cleanup(). 
        On success - exits with 0 exit value. 
*/
/*================================================================================================*/
int main( int argc, char ** argv )
{
        int rv = TFAIL;
        
        /* parse options. */
        int    testcaseFlag     = 0;
        int    inpFileFlag      = 0;
        int    outFileFlag      = 0;
        int    numFilteringFlag = 0;
        int    numH264PauseRowFlag = 0;
        char * testcaseOpt;
        char * inpFileOpt;
        char * outFileOpt;
        char * numFilteringOpt;
	char * numH264PauseRowOpt;
        char * msg;
        
        option_t options[] = 
        {
                { "T:",  &testcaseFlag,             &testcaseOpt     },
                { "F:",  &inpFileFlag,              &inpFileOpt      },
                { "O:",  &outFileFlag,              &outFileOpt      },
                { "V",   &gTestappConfig.mVerbose,  NULL             },
                { "N:",  &numFilteringFlag,         &numFilteringOpt },
                { "R:",  &numH264PauseRowFlag,      &numH264PauseRowOpt},
                { NULL,  NULL,                      NULL             }
        };
        
        /* parse options. */
        if( NULL != ( msg = parse_opts( argc, argv, options, help ) ) )
        {
                tst_brkm( TBROK, cleanup, "OPTION PARSING ERROR - %s", msg );
        }
        
        /* Fill up the gTestappConfig by the parsed options. */
        gTestappConfig.mTestCase = testcaseFlag ? atoi(testcaseOpt) : 1;
        gTestappConfig.mInputFileName = inpFileFlag ? inpFileOpt : NULL;
        gTestappConfig.mOutputFileName = outFileFlag ? outFileOpt : NULL;
        gTestappConfig.mNumFilterCycle = numFilteringFlag ? atoi(numFilteringOpt) : 1;
	gTestappConfig.mH264PauseRow = numH264PauseRowFlag ? atoi(numH264PauseRowOpt) : 0;
        
        /* Check if all of the required arguments were presented. */
        if( !gTestappConfig.mInputFileName )
                tst_brkm( TCONF, cleanup, "Argument required -F" );
        if( !gTestappConfig.mOutputFileName )
                tst_brkm( TCONF, cleanup, "Argument required -O" );
        
        /* Select test case name */
        switch( gTestappConfig.mTestCase )
        {
        case PF_DISABLE_ALL:
                TCID = strdup( "PF_DISABLE_ALL" );
                break;
        case PF_MPEG4_DEBLOCK:
                TCID = strdup( "PF_MPEG4_DEBLOCK" );
                break;
        case PF_MPEG4_DERING:
                TCID = strdup( "PF_MPEG4_DERING" );
                break;
        case PF_MPEG4_DEBLOCK_DERING:
                TCID = strdup( "PF_MPEG4_DEBLOCK_DERING" );
                break;
        case PF_H264_DEBLOCK:
                TCID = strdup( "PF_H264_DEBLOCK" );
                break;
	case  5:
	        TCID = strdup("PF_H264_DEBLOCK_RESUME");
		break;
        default:
                assert( !"unknown test case" );
        }

        /* perform global test setup, call setup() function. */
        setup();
        
        /* Print test Assertion using tst_resm() function with argument TINFO. */
        tst_resm( TINFO, "Testing if %s test case is OK", TCID );
        
        /* VTE : print results and exit test scenario. */
        rv = VT_ipu_pf_test();
        
        if( TPASS == rv )
        {
                if( AskUser( "Did this test case work as expected ?" ) == 1 )
                        tst_resm( TPASS, "%s test case worked as expected", TCID );
                else
                        tst_resm( TFAIL, "%s test case did NOT work as expected", TCID );
        }
        else
                tst_resm( TFAIL, "%s test case did NOT work as expected", TCID );

        if( TCID ) 
                free( TCID );
        cleanup(); 
        
        return rv;
}

/*================================================================================================*/
/*===== ask_user =====*/
/** 
@brief  Asks user to answer the question: Did this test case work as expected? 
 
@param  Input: string of question   
        Output: None 
  
@return 0 - if user asks "No,  wrong" 
        1 - if user asks "Yes, right" 
*/
/*================================================================================================*/
int AskUser( char * question )
{
        unsigned char answer;
        int ret = 2;
        do
        {
                tst_resm(TINFO, "%s [Y/N]", question);
                answer = fgetc(stdin);
                if (answer == 'Y' || answer == 'y')
                        ret = 1;
                else if (answer == 'N' || answer == 'n')
                        ret = 0;
        }
        while (ret == 2);
        fgetc(stdin);       /* Wipe CR character from stream */
        return ret;
}
