/*====================*/
/**
        @file   oss_sound_driver_main.c

        @brief  OSS audio record test main file.
*/
/*======================

        Copyright (C) 2006, Freescale Semiconductor, Inc. All Rights Reserved
        THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
        BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
        Freescale Semiconductor, Inc.

====================
Revision History:
                            Modification     Tracking
Author/core ID                  Date          Number    Description of Changes
-------------------------   ------------    ----------  -------------------------------------------
RB657C/gsch1c                20/07/2004     TLSbo43102  Initial version  of OSS sound driver test development
D.Simakov/smkd001c           25/07/2005     TLSbo52891  Test case asks final result to
                                                        user (yes or no) before printing PASS or FAIL status.
D.Simakov/smkd001c           06/10/2005     TLSbo53199  Syncronization was added. The code was formatted.
I.Inkina/nknl001             11/01/2005     TLSbo61044  Clock master was added
D.Khoroshev/b00313           03/03/2006     TLSbo62323  Updates according to last MXC OSS specification
D.Simakov                    07/12/2006     TLSbo76144  Updated with the new kernel
====================
Portability: ARM GCC
======================*/

/*======================
Total Tests: 1

Test Name:   OSS audio record test in different spaces

Test Assertion
& Strategy:  Tries to record in different audio in kernel and user spaces

======================*/

/*======================
                                        INCLUDE FILES
======================*/
/* Standard Include Files */
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

/* Harness Specific Include Files. */
#include <test.h>
#include <usctest.h>

/* Verification Test Environment Include Files */
#include "oss_sound_driver_test.h"

/*======================
                                        LOCAL MACROS
======================*/

/*======================
                            LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
======================*/

/*======================
                                        LOCAL CONSTANTS
======================*/
#if !defined(TRUE) && !defined(FALSE)
#define TRUE  1
#define FALSE 0
#endif

/*======================
                                        LOCAL VARIABLES
======================*/

/*======================
                                        GLOBAL CONSTANTS
======================*/

/*======================
                                        GLOBAL VARIABLES
======================*/
/* Extern Global Variables */
extern int Tst_count;        /* counter for tst_xxx routines.  */
extern char *TESTDIR;        /* temporary dir created by tst_tmpdir(void) */

/* Global Variables */
char   *TCID = "oss_testapp_rec1";        /* test program identifier.  */
int     TST_TOTAL = 1;                    /* total number of tests in this file.  */

int     Chanflag = 0,
        ByteNumflag = 0,
        Sourceflag = 0,
        Speedflag = 0,
        SsiIndexflag = 0,
        Bitsflag = 0;       /* binary flags: opt or not */
char   *Chanopt;        /* Chan option arguments */
char   *Speedopt;       /* Chan option arguments */
char   *Bitsopt;        /* Chan option arguments */
char   *ByteNumopt;     /* ByteNum option arguments */
char   *SsiIndexopt;    /* SsiIndex option arguments */
char   *Sourceopt;      /* Source option arguments */

option_t options[] = {
        {"D:", &SsiIndexflag, &SsiIndexopt},    /* argument required */
        {"C:", &Chanflag, &Chanopt},    /* argument required */
        {"S:", &Speedflag, &Speedopt},  /* argument required */
        {"B:", &Bitsflag, &Bitsopt},    /* argument required */
        {"N:", &ByteNumflag, &ByteNumopt},      /* argument required */
        {"L:", &Sourceflag, &Sourceopt},        /* argument required */
        {NULL, NULL, NULL}      /* NULL required to end array */
};

/*======================
                                    GLOBAL FUNCTION PROTOTYPES
======================*/
void    cleanup(void);
void    setup(void);
int     main(int argc, char **argv);

/*======================
                                    LOCAL FUNCTION PROTOTYPES
======================*/

/*======================
                                        GLOBAL FUNCTIONS
======================*/

/*====================*/
/*= cleanup =*/
/**
@brief  Performs all one time clean up for this test on successful
        completion,  premature exit or  failure. Closes all temporary
        files, removes all temporary directories exits the test with
        appropriate return code by calling tst_exit() function.cleanup

@param  Input :      None.
        Output:      None.

@return Nothing
*/
/*====================*/
void cleanup(void)
{
        /* VTE : Actions needed to get a stable target environment */
        int     VT_rv = TFAIL;

        VT_rv = VT_oss_sound_driver_cleanup();
        if (VT_rv != TPASS)
        {
                tst_resm(TWARN, "VT_cleanup() Failed : error code = %d", VT_rv);
        }

        tst_exit();
}

/*======================
                                        LOCAL FUNCTIONS
======================*/


/*====================*/
/*= help =*/
/**
@brief  Inform of the available options and the associated parameters

@param  Input :      None.
        Output:      None.

@return On failure - Exits by calling cleanup().
        On success - returns 0.
*/
/*====================*/
void help(void)
{
        printf("Switches \n\n");
        printf("  -C   Recording channel number\n");
        printf("  -S   Recording sampling frequency\n");
        printf("  -B   Recording bit depth\n");
        printf("  -N   Number of bytes to record\n");
        printf("  -L s Select the input source\n");
        printf("       s: 1 for handset \n");
        printf("       s: 2 for headset \n");
        printf("       s: 3 for line in \n");
}

/*====================*/
/*= setup =*/
/**
@brief  Performs all one time setup for this test. This function is
        typically used to capture signals, create temporary dirs
        and temporary files that may be used in the course of this test.

@param  Input :      None.
        Output:      None.

@return On failure - Exits by calling cleanup().
        On success - returns 0.
*/
/*====================*/
void setup(void)
{
        int     VT_rv = TFAIL;

        /* VTE : Actions needed to prepare the test running */
        VT_rv = VT_oss_sound_driver_setup();
        if (VT_rv != TPASS)
        {
                tst_brkm(TBROK, cleanup, "VT_setup() Failed : error code = %d", VT_rv);
        }
        /* VTE */
        return;
}

/*====================*/
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

/*====================*/
/*= main =*/
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
/*====================*/
int main(int argc, char **argv)
{
        int     VT_rv = TFAIL;
        char   *msg;
        int     Bits = 16;
        int     Speed = 8000;
        int     Chan = 2;
        int     ByteNum = 16384;
        int     SsiIndex = 1;
        int     Source = 3;

        /* parse options. */
        if ((msg = parse_opts(argc, argv, options, help)) != NULL)
        {
                tst_brkm(TBROK, cleanup, "OPTION PARSING ERROR - %s", msg);
        }
        if (Chanflag)
        {
                Chan = atoi(Chanopt);
                tst_resm(TINFO, " Chanflag = %d", Chan);
        }
        if (ByteNumflag)
        {
                ByteNum = atoi(ByteNumopt);
                tst_resm(TINFO, " ByteNumflag = %d", ByteNum);
        }

        if (Speedflag)
        {
                Speed = atoi(Speedopt);
                tst_resm(TINFO, " Speedflag = %d", Speed);
        }
        if (Bitsflag)
        {
                Bits = atoi(Bitsopt);
                tst_resm(TINFO, " Bitsflag = %d", Bits);
        }
        if (SsiIndexflag)
        {
                SsiIndex = atoi(SsiIndexopt);
                tst_resm(TINFO, " SsiIndexflag = %d", SsiIndex);
        }
        if (Sourceflag)
        {
                Source = atoi(Sourceopt);
                tst_resm(TINFO, " Sourceflag = %d", Source);
        }

        /* perform global test setup, call setup() function. */
        setup();

        /* Print test Assertion using tst_resm() function with argument TINFO. */
        tst_resm(TINFO, "Testing if %s test case is OK", TCID);

        /* Test Case Body. */
        /* VTE : print results and exit test scenario */
        VT_rv = VT_oss_sound_driver_test(Source, 0, ByteNum, Bits, Speed, Chan); /* with the
                                                                                     * parameters
                                                                                     * needed
                                                                                     * come from
                                                                                     * parse_opt()) */
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
