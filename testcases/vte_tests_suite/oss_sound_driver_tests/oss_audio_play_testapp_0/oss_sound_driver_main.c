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
/*================================================================================================*/
/**
        @file   oss_sound_driver_main.c

        @brief  OSS audio play test main file.
====================================================================================================
Revision History:
                            Modification     Tracking
Author/core ID                  Date          Number    Description of Changes
-------------------------   ------------    ----------  -------------------------------------------
RB657C/gsch1c                20/07/2004     TLSbo40898  Initial version  of OSS sound driver test development
D.Simakov/smkd001c           25/07/2005     TLSbo52891  Test case asks final result to 
D.Khoroshev/B00313           02/08/2006     TLSbo61044  PMIC master mode is the only supported mode
D.Khoroshev/B00313           02/15/2006     TLSbo62323  Updates accoring last specifications
D.Simakov                    13/06/2006     TLSbo67022  STDAC <=> CODEC
====================================================================================================
Portability: ARM GCC
==================================================================================================*/

/*==================================================================================================
Total Tests: 1

Test Name:   OSS Audio Play Test

Test Assertion
& Strategy:  Play sample file using OSS driver
==================================================================================================*/

/*==================================================================================================
                                        INCLUDE FILES
==================================================================================================*/
/* Standard Include Files */
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
    
/* Harness Specific Include Files. */
#include <test.h>
#include <usctest.h>

/* Verification Test Environment Include Files */
#include "oss_sound_driver_test.h"

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
/* Global Variables */
char *TCID     = "oss_audio_play_testapp_0";        /* test program identifier.            */
int  TST_TOTAL = 1;                                 /* total number of tests in this file. */

int Deviceflag = 0, Loopflag = 0, Fileflag = 0;        /* binary flags: opt or not  */
char *Deviceopt;                                       /* Device option arguments   */
char *Loopopt;                                         /* Loop option arguments     */
char *Fileopt;                                         /* File option arguments     */

option_t options[] =
{
        { "N:", &Loopflag, &Loopopt  },            /* argument required */
        { "F:", &Fileflag, &Fileopt },             /* argument required */
        { "D:", &Deviceflag, &Deviceopt  },        /* argument required */
        { NULL, NULL, NULL }                       /* NULL required to end array */
};

static char * audiofile = "ringout.wav";

/*==================================================================================================
                                   GLOBAL FUNCTION PROTOTYPES
==================================================================================================*/
void cleanup(void);
void setup(void);
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
void cleanup(void)
{
        int VT_rv = TFAIL;
        
        VT_rv = VT_oss_sound_driver_cleanup();
        if (VT_rv != TPASS)
        {
                tst_resm(TWARN, "VT_cleanup() Failed : error code = %d", VT_rv);
        }
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
void help(void)
{
        printf("\tSwitches \n\n");
        printf("\t  -N n        Number of time to play the file\n");
        printf("\t  -F          Selects the test file (default ringtone)\n");
        printf("\t  -D d        Index of the device to use\n");
        printf("\t                0: STDAC\n");
        printf("\t                1: CODEC\n");
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
void setup(void)
{
        int VT_rv = TFAIL;
        VT_rv = VT_oss_sound_driver_setup();
        if (VT_rv != TPASS)
        {
                tst_brkm(TBROK , cleanup, "VT_setup() Failed : error code = %d", VT_rv);
        }
}

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
                                -h - shows full list of arguments

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
        int Loop= 1;
        char * File = audiofile;
        int Device = 0;
                
        /* parse options. */
        if ( (msg=parse_opts(argc, argv, options, help)) != NULL )
        {
                tst_brkm(TBROK, cleanup, "OPTION PARSING ERROR - %s", msg);
        }
        if(Loopflag)
        {
                Loop = atoi(Loopopt);
                tst_resm(TINFO, " Loopflag = %d", Loop);
        }
        if(Fileflag)
        {
                File = Fileopt;
        }
        if(Deviceflag)
        {
                Device = atoi(Deviceopt);
                if (Device == 1)
                {
                        tst_resm(TINFO, " Device is CODEC");
                        FILE *fd_file = NULL;
                        int channels;
 
                        fd_file = fopen(File, "r");

                        if(!fd_file)
                        {
                                tst_brkm(TBROK , cleanup, " file %s not found", File);
                        }

                        channels = get_audio_channels(fd_file);
                        fclose(fd_file);
                        if(channels == 2)
                        {
                                tst_brkm(TBROK , cleanup, "  ERROR: attempting to play stereo file on Voice CODEC device");                                
                        }
                }
                else if (Device == 0)
                {
                        tst_resm(TINFO, " Device is STDAC");
                }
                else 
                {
                        tst_resm(TCONF, " Unknown device device");
                        cleanup();
                }
        }

        setup();

        tst_resm(TINFO, "Testing if %s test case is OK", TCID);

        VT_rv = VT_oss_sound_driver_test(Device, Loop, File);
        
        if(VT_rv == TPASS)
        {
                if( ask_user( "Did this test case work as expected" ) == TPASS )
                            tst_resm(TPASS, "%s test case worked as expected", TCID);
                else
                        tst_resm(TFAIL, "%s test case did NOT work as expected", TCID);
        }

        cleanup();

        return VT_rv;
}
