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

        @brief  OSS audio multiple play test main file.
====================================================================================================
Revision History:
                            Modification     Tracking
Author/core ID                  Date          Number    Description of Changes
-------------------------   ------------    ----------  -------------------------------------------
RB657C/gsch1c                20/07/2004     TLSbo40898 Initial version  of OSS sound driver test development
RB657C/gsch1c                20/07/2004     TLSbo43102  Add a parameter to the application
D.Simakov/smkd001c           25/07/2005     TLSbo52891  Test case asks final result to 
                                                        user (yes or no) before printing PASS or FAIL status.      
I.Inkina/nknl001             11/01/2005     TLSbo61044  Clock master was added
D.Khoroshev/B00313           02/15/2006     TLSbo62323  Updates accoring to the last specifications
D.Simakov                    13/06/2006     TLSbo67022  STDAC <=> CODEC

====================================================================================================
Portability: ARM GCC
==================================================================================================*/

/*==================================================================================================
Total Tests: 1

Test Name:   OSS audio play multiple files

Test Assertion
& Strategy:  Two files should be played together using STDAC or CODEC device.
==================================================================================================*/

/*==================================================================================================
                                        INCLUDE FILES
==================================================================================================*/
/* Standard Include File1s */
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

/* Harness Specific Include File1s. */
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
char   *TCID = "oss_testapp_play_1";    /* test program identifier.  */
int     TST_TOTAL = 1;  /* total number of tests in this file.  */

int     File2flag = 0,
    Loopflag = 0,
    File1flag = 0,
    Deviceflag = 0;     /* binary flags: opt or not */
char   *File2opt;       /* File2 option arguments */
char   *Loopopt;        /* Loop option arguments */
char   *File1opt;       /* File1 option arguments */
char   *Deviceopt;

option_t options[] = {
        {"L:", &File2flag, &File2opt},  /* argument required */
        {"N:", &Loopflag, &Loopopt},    /* argument required */
        {"F:", &File1flag, &File1opt},  /* argument required */
        {"D:", &Deviceflag, &Deviceopt},        /* argument required */
        {NULL, NULL, NULL}      /* NULL required to end array */
};

static char *audiofile = "ringout.wav";

/*==================================================================================================
                                    GLOBAL FUNCTION PROTOTYPES
==================================================================================================*/
void    cleanup(void);
void    setup(void);
int     main(int argc, char **argv);

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
        int     VT_rv = TFAIL;

        VT_rv = VT_oss_sound_driver_cleanup();
        if (VT_rv != TPASS)
        {
                tst_resm(TWARN, "VT_cleanup() Failed : error code = %d", VT_rv);
        }
        tst_exit();
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
        printf("        Switches \n\n");
        printf("          -F        File1 Select the test file 1 (default ringtone)\n");
        printf("          -L        File2 Select the test file 2 (default ringtone)\n");
        printf("          -N        Number of loops\n");
        printf("          -D d      Index of the device to use (default StDAC)\n");
        printf("                        0: STDAC\n");
        printf("                        1: CODEC\n");
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
        int     VT_rv = TFAIL;

        VT_rv = VT_oss_sound_driver_setup();
        if (VT_rv != TPASS)
        {
                tst_brkm(TBROK, cleanup, "VT_setup() Failed : error code = %d", VT_rv);
        }
}

/*================================================================================================*/
/*===== setup =====*/
/**
@brief  Show the message and readback answer 

@param  Input :      question - text message.
        Output:      None.

@return On failure - TPASS, TFAIL or TRETR depending on user's answer.
*/
/*================================================================================================*/
int ask_user(char *question)
{
        unsigned char answer;
        int     ret = TRETR;

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
        fgetc(stdin);   /* Wipe CR character from stream */
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
    
@return On failure - Exits by calling cleanup().
        On success - exits with 0 exit value.*/
/*================================================================================================*/
int main(int argc, char **argv)
{
        int     VT_rv = TFAIL;
        char   *msg;
        char   *File2 = audiofile;
        int     Loop = 1;
        char   *File1 = audiofile;
        int     Device = 0;

        /* parse options. */
        if ((msg = parse_opts(argc, argv, options, help)) != NULL)
        {
                tst_brkm(TBROK, cleanup, "OPTION PARSING ERROR - %s", msg);
        }
        if (Loopflag)
        {
                Loop = atoi(Loopopt);
                tst_resm(TINFO, " Loopflag = %d", Loop);
        }
        if (File1flag)
        {
                File1 = File1opt;
        }
        if (File2flag)
        {
                File2 = File2opt;
        }
        if (Deviceflag)
        {
                Device = atoi(Deviceopt);
                if (Device == 0)
                {
                        tst_resm(TINFO, " First Device is STDAC\n");
                }
                else if (Device == 1)
                {
                        tst_resm(TINFO, " First Device is CODEC");
                        FILE   *fd_file = NULL;
                        int     channels;

                        fd_file = fopen(File1, "r");
                        if (!fd_file)
                        {
                                tst_resm(TFAIL, " file %s not found");
                                cleanup();
                        }
                        channels = get_audio_channels(fd_file);
                        fclose(fd_file);
                        if (channels == 2)
                        {
                                tst_brkm(TBROK, cleanup,
                                         "  ERROR: attempting to play stereo file on Voice CODEC device");
                        }

                        fd_file = fopen(File2, "r");
                        if (!fd_file)
                        {
                                tst_resm(TFAIL, " file %s not found");
                                cleanup();
                        }
                        channels = get_audio_channels(fd_file);
                        fclose(fd_file);
                        if (channels == 2)
                        {
                                tst_brkm(TBROK, cleanup,
                                         "  ERROR: attempting to play stereo file on Voice CODEC device");
                        }
                }
        }

        /* perform global test setup, call setup() function. */
        setup();

        /* Print test Assertion using tst_resm() function with argument TINFO. */
        tst_resm(TINFO, "Testing if %s test case is OK", TCID);

        /* VTE : print results and exit test scenario */
        VT_rv = VT_oss_sound_driver_test(Device, File1, File2, Loop);
        /* with the parameters needed come from parse_opt()) */

        if (VT_rv == TPASS)
        {
                if (ask_user("Did this test case work as expected") == TPASS)
                        tst_resm(TPASS, "%s test case worked as expected", TCID);
                else
                        tst_resm(TFAIL, "%s test case did NOT work as expected", TCID);
        }
        else
                tst_resm(TFAIL, "%s test case did NOT work as expected", TCID);

        cleanup(); /** OR tst_exit(); */

        return VT_rv;
}
