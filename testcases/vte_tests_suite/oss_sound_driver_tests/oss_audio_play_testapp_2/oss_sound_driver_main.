/*================================================================================================*/
/**
        @file   oss_sound_driver_main.c

        @brief  OSS test main function
*/
/*==================================================================================================

        Copyright (C) 2006, Freescale Semiconductor, Inc. All Rights Reserved
        THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
        BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
        Freescale Semiconductor, Inc.

====================================================================================================
Revision History:
                            Modification     Tracking
Author/core ID                  Date          Number    Description of Changes
-------------------------   ------------    ----------  -------------------------------------------
RB657C/gsch1c                20/07/2004     TLSbo40898  Initial version  of OSS sound driver test development
D.Simakov/smkd001c           25/07/2005     TLSbo52891  Test case asks final result to 
                                                        user (yes or no) before printing PASS or FAIL status.      
I.Inkina/nknl001             11/01/2005     TLSbo61044  Clock master was added
A.Ozerov/B00320              17/02/2006     TLSbo62323  Index_ssi argument was deleted. Master mode
                                                        was removed. Checking if the audio file given
                                                        in argument is stereo or mono was added.
D.Khoroshe/b00313            04/14/2006     TLSbo67022  VTE 2.0 Integration 
D.Simakov                    13/06/2006     TLSbo67022  Update help
D.Simakov                    19/10/2006     TLSbo76144  dsp->adsp, dsp1->dsp
====================================================================================================
Portability: ARM GCC
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
char   *TCID = "oss_testapp_play_2";     /* test program identifier.  */
int     TST_TOTAL = 1;  /* total number of tests in this file.  */

int     Loopflag = 0,
        Playlistflag = 0,
        FileLoopflag = 0,
        Deviceflag = 0;     /* binary flags: opt or not */
char   *Loopopt;        /* Loop option arguments */
char   *Playlistopt;    /* File option arguments */
char   *FileLoopopt;    /* File option arguments */
char   *Deviceopt;

option_t options[] = {
        {"L:", &Playlistflag, &Playlistopt},    /* argument required */
        {"F:", &FileLoopflag, &FileLoopopt},    /* argument required */
        {"N:", &Loopflag, &Loopopt},    /* argument required */
        {"D:", &Deviceflag, &Deviceopt},        /* argument required */
        {NULL, NULL, NULL}      /* NULL required to end array */
};

static char default_playlist[] = "playlist.txt";

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
        appropriate return code by calling tst_exit(void) function.cleanup

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
        printf("Switches \n\n");
        printf("  -L        Playlist text file\n");
        printf("  -N n      Number of time to play a file inside the playlist\n");
        printf("  -F f      Number of time to the whole playlist\n");
        printf("  -D d      Index of the device to use \n");
        printf("                0: STDAC\n");
        printf("                1: CODEC\n");
}

/*================================================================================================*/
/*===== setup =====*/
/**
@brief  Performs all one time setup for this test. This function is
        typically used to capture signals, create temporary dirs
        and temporary files that may be used in the course of this test.
    
@param  Input :      None.
        Output:      None.
    
@return On failure - Exits by calling cleanup(void).
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
@brief  Show user text message and read answer.

@param  Input :      question - message string.

@return TPASS or TFAIL. 
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
                                Describe input arguments to this test-case
                                -l - Number of iteration
                                -v - Prints verbose output
                                -V - Prints the version number

                                -Id - Id of the test according to the test plan
                                -Case N - If exist, the test case number associated with the test Id
                                -Iter - Inform the iteration of the loop in case of an endurance/stress test
    
@return On failure - Exits by calling cleanup().
        On success - exits with 0 exit value.
*/
/*================================================================================================*/
int main(int argc, char **argv)
{
        int     VT_rv = TFAIL;
        char   *msg;
        int     Loop = 1;
        int     FileLoop = 1;
        int     Device = 0;
        char   *PlayList;

        if ((msg = parse_opts(argc, argv, options, help)) != NULL)
        {
                tst_brkm(TBROK, cleanup, "OPTION PARSING ERROR - %s", msg);
        }
        if (Playlistflag)
        {
                tst_resm(TINFO, " Playlist field given \n");
                PlayList = Playlistopt;
        }
        else
        {
                tst_resm(TINFO, "default playlist.txt \n");
                PlayList = default_playlist;
        }
        if (FileLoopflag)
        {
                FileLoop = atoi(FileLoopopt);
                tst_resm(TINFO, " FileLoopflag = %d \n", FileLoop);
        }

        if (Loopflag)
        {
                Loop = atoi(Loopopt);
                tst_resm(TINFO, " Loopflag = %d \n", Loop);
        }
        if (Deviceflag)
        {
                Device = atoi(Deviceopt);
                if(Device != 1 && Device != 0) 
                        Device = 0;                
        }

        setup();

        if (Device == 0)
                tst_resm(TINFO, " Device is STDAC\n");
        else
                tst_resm(TINFO, " Device is CODEC\n");

        tst_resm(TINFO, "Testing if %s test case is OK", TCID);
        VT_rv = VT_oss_sound_driver_test(PlayList, FileLoop, Loop, Device);

        if (VT_rv == TPASS)
        {
                if (ask_user("Did this test case work as expected") == TPASS)
                        tst_resm(TPASS, "%s test case worked as expected", TCID);
                else
                        tst_resm(TFAIL, "%s test case did NOT work as expected", TCID);
        }
        else
                tst_resm(TFAIL, "%s test case did NOT work as expected", TCID);

        cleanup();

        return VT_rv;
}
