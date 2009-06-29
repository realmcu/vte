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

        @brief  OSS audio control test main file.
====================================================================================================
Revision History:
                            Modification     Tracking
Author/core ID                  Date          Number    Description of Changes
-------------------------   ------------    ----------  -------------------------------------------
RB657C/gsch1c                20/07/2004     TLSbo40898  Initial version  of OSS sound driver test development
D.Simakov/smkd001c           25/07/2005     TLSbo52891  Test case asks final result to user (yes or no)
                                                        before printing PASS or FAIL status.      
A.Ozerov/b00320              07/11/2005     TLSbo56870  Compilation flags for SC55112 and MC13783
                                                        platforms
D.Khoroshev/b00313           02/02/2006     TLSbo61495  Code cleaning
D.Khoroshev/b00313           03/03/2006     TLSbo62323  Update according to the last MXC OSS specifications
A.Ozerov/b00320              20/07/2006     TLSbo70792  Code was cast to coding conventions.

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
char   *TCID = "oss_testapp_ctrl_0";    /* test program identifier.  */
int     TST_TOTAL = 1;  /* total number of tests in this file.  */

int     Deviceflag = 0,
        Incrementflag = 0,
        Fileflag = 0;       /* binary flags: opt or not */

char   *Deviceopt;      /* Device option arguments */
char   *Incrementopt;   /* Increment option arguments */
char   *Fileopt;        /* File option arguments */

option_t options[] = 
{
        {"D:", &Deviceflag, &Deviceopt},        /* argument required */
        {"N:", &Incrementflag, &Incrementopt},  /* argument required */
        {"F:", &Fileflag, &Fileopt},    /* argument required */
        {NULL, NULL, NULL}      /* NULL required to end array */
};

static char *audiofile = "ringout8k16M.wav";

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
    
@return None.
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

        /* Exit with appropriate return code. */
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
    
@return None.
*/
/*================================================================================================*/
void help(void)
{
        printf("Switches \n\n");
        printf("\t  -D n        Device to test\n");
        printf("\t  0   STDAC\n");
        printf("\t  1   CODEC(default)\n");
#ifdef CONFIG_MXC_MC13783_PMIC
        printf("\t  2   ADDER(not supported)\n\n");
#endif
        printf("\t  -N  Select the increment of volume and balance (default 5)\n");
        printf("\t      The number of increment impact of the length of the tests\n");
        printf("\t      that are run between 0 and 100\n");
        printf("\t  -F  Select the test file (default: ringout8k16M.wav)\n");
}

/*================================================================================================*/
/*===== setup =====*/
/**
@brief  Performs all one time setup for this test. This function is
        typically used to capture signals, create temporary dirs
        and temporary files that may be used in the course of this test.

@param  Input :      None.
        Output:      None.
    
@return None.
*/
/*================================================================================================*/
void setup(void)
{
        int     VT_rv = TFAIL;

        /* VTE : Actions needed to prepare the test running */
        VT_rv = VT_oss_sound_driver_setup();
        if (VT_rv != TPASS)
        {
                tst_brkm(TBROK, cleanup, "VT_setup() Failed : error code = %d", VT_rv);
        }
}

/*================================================================================================*/
/*===== ask_user =====*/
/**
@brief  Shows question specified with a parameter and reads answer. 

@param  Input :      question - text message.
        Output:      None.

@return TPASS or TFAIL depends of user's answer.
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
        int     Device = STDAC;
        int     Increment = INCREMENT;
        char   *File = audiofile;

        if ((msg = parse_opts(argc, argv, options, help)) != NULL)
        {
                tst_brkm(TBROK, cleanup, "OPTION PARSING ERROR - %s", msg);
        }
        if (Deviceflag)
        {
                Device = atoi(Deviceopt);
                tst_resm(TINFO, " Deviceflag = %d", Device);
        }
        if (Incrementflag)
        {
                Increment = atoi(Incrementopt);
                tst_resm(TINFO, " Incrementflag = %d", Increment);
        }
        if (Fileflag)
        {
                File = Fileopt;
        }

        if ( (Device < 0) || ( Device > 2) )
        {
                help();
                return VT_rv;
        }

        /* perform global test setup, call setup() function. */
        setup();

        /* Print test Assertion using tst_resm() function with argument TINFO. */
        tst_resm(TINFO, "Testing if %s test case is OK", TCID);

        /* VTE : print results and exit test scenario */
        VT_rv = VT_oss_sound_driver_test(Device, Increment, File);      /* with the parameters needed 
                                                                        * come from parse_opt()) */

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
