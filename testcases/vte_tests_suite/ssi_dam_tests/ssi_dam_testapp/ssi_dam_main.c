/***
**Copyright 2004-2009 Freescale Semiconductor, Inc. All Rights Reserved.
**
**The code contained herein is licensed under the GNU General Public
**License. You may obtain a copy of the GNU General Public License
**Version 2 or later at the following locations:
**
**http://www.opensource.org/licenses/gpl-license.html
**http://www.gnu.org/copyleft/gpl.html
**/
/**
@file ssi_dam_main.c

@par Portability:
        ARM GCC
*/

/*======================== REVISION HISTORY ==================================

Author (core ID)      Date         CR Number    Description of Changes
-------------------   ----------   ----------   ------------------------------
Tony THOMASSIN/RB595C 29/07/2004   TLSbo41151   SSI/DAM test development 
V.Halabuda/hlbv001    05/08/2005   TLSbo53363   update for linux-2.6.10-rel-1.12.arm
I.Inkina/nknl001      11/08/2005   TLSbo53878   parse options were update   
I.Inkina/nknl001      22/09/2005   TLSbo55818   update for VTE_1.13  
D.Khoroshev/b00313    12/02/2005   TLSbo56844   update for VTE_1.14
D.Simakov             05/06/2006   TLSbo67103   Re-written
D.Kardakov            11/09/2006   TLSbo71015   update for L26_21 release 
=============================================================================*/


/*==================================================================================================
                                        INCLUDE FILES
==================================================================================================*/

/* Standard Include Files */
#include <stdio.h>
#include <assert.h>
#include <asm/arch/pmic_audio.h>    
/* Harness Specific Include Files. */
#include "usctest.h" 
#include "test.h"

/* Verification Test Environment Include Files */
#include "ssi_dam_test.h"


/*==================================================================================================
                                       GLOBAL VARIABLES
==================================================================================================*/

char *          TCID                        = "ssi_dam";
int             TST_TOTAL                   = 1; 
sTestappConfig  gTestappConfig; 


/*==================================================================================================
                                   LOCAL FUNCTION PROTOTYPES
==================================================================================================*/

void Help        ( void );
void Setup       ( void );
void Cleanup     ( void );
int  AskUser     ( const char * msg );


/*==================================================================================================
                                       LOCAL FUNCTIONS
==================================================================================================*/

/*================================================================================================*/
/*================================================================================================*/
void Help( void )
{
        printf("Custom option : \n");
        printf("  -T       Test : 0, 1, 2 or 3 \n");
        printf("            0 = Play a sample from SSI1-STEREODAC\n\n");
        printf("            1 = Play a sample from SSI1-CODEC\n\n");
        printf("            2 = Play a sample from SSI2-STEREODAC\n\n");
        printf("            3 = Play a sample from SSI2-CODEC\n\n");        
        printf("  -S       Sample to play : <file.wav>\n");
        printf("  -C       Num of bytes to write (default 512)\n");
} 


/*================================================================================================*/
/*================================================================================================*/
void Setup( void )
{               
        int rv = VT_ssi_dam_setup();
        if( TPASS != rv )
        {
                tst_brkm( TBROK , Cleanup, "VT_codec_setup() Failed : error code = %d", rv );
        }
}


/*================================================================================================*/
/*================================================================================================*/
void Cleanup( void )
{        
        int rv = VT_ssi_dam_cleanup();
        if( TPASS != rv )
        {
                tst_resm( TWARN, "VT_codec_cleanup() Failed : error code = %d", rv );
        }
        
        tst_exit();
}


/*================================================================================================*/
/*================================================================================================*/
int AskUser( const char * msg )
{
        unsigned char answer;
        int           ret = 2;

        do
        {
                printf("%s\n", msg);
                answer = fgetc(stdin);
                if (answer == 'Y' || answer == 'y')
                        ret = TPASS;
                else if (answer == 'N' || answer == 'n')
                        ret = TFAIL;
        }
        while (ret == 2);

        fgetc(stdin);  

        return ret;
}


/*================================================================================================*/
/*================================================================================================*/
int main( int argc, char ** argv )
{   
        int rv = TFAIL;
        

        /******************/
        /* Parse options. */
        /******************/
        
        int    testcaseFlag     = 0;                                
        int    sndNameFlag      = 0;
        int    writeChunkSzFlag = 0;
        char * testcaseOpt      = 0;
        char * writeChunkSzOpt  = 0;
        char * msg;
        
        option_t options[] =
        {
                { "T:",  &testcaseFlag,     &testcaseOpt             },
                { "S:",  &sndNameFlag,      &gTestappConfig.mSndName },
                { "C:",  &writeChunkSzFlag, &writeChunkSzOpt         },
                { NULL,  NULL,              NULL                     }
        };
                
        if( (msg = parse_opts( argc, argv, options, Help )) )
        {
                tst_brkm( TCONF, Cleanup, "OPTION PARSING ERROR - %s", msg );
        }
        
        gTestappConfig.mTestCase = testcaseFlag ? atoi( testcaseOpt ) : 1;        
        gTestappConfig.mWriteChunkSz = writeChunkSzFlag ? atoi( writeChunkSzOpt ) : 512;          
                                 
        
        /**********************************************************/
        /* Check if all of the required arguments were presented. */
        /**********************************************************/        
        
        if( !testcaseFlag || !sndNameFlag )
        {
                tst_brkm( TCONF, Cleanup, "Argument required..." );
                Help();
        }
                        
        tst_resm( TINFO, "Testing if %s test case is OK", TCID );        
        
        /**********************/
        /* Global test setup. */
        /**********************/
        
        Setup();                
        

        /*************/
        /* Run test. */
        /*************/
                
        rv = VT_ssi_dam_test();
        
        
        /********************************/
        /* Print the final test result. */
        /********************************/
        
        if( TPASS == rv )
        {
                if( AskUser("Did this test case work as expected? [y/n]") == TPASS )
                        tst_resm( TPASS, "%s test case worked as expected", TCID );
                else
                        tst_resm( TFAIL, "%s test case did NOT work as expected", TCID );
                
        }
        else
                tst_resm( TFAIL, "%s test case did NOT work as expected", TCID );
        
        
        /************************/
        /* Global test cleanup. */
        /************************/
        
        Cleanup(); 
        
        return rv;
} 
