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
        @file   pmic_audio_main.c

        @brief  Main file for PMIC audio driver test
====================================================================================================
Revision History:
                            Modification     Tracking
Author/core ID                  Date          Number    Description of Changes
-------------------------   ------------    ----------  -------------------------------------------
S.Bezrukov/SBAZR1C           31/08/2005     TLSbo52697  Initial version
A.Ozerov/b00320              08/08/2006     TLSbo73745  Review version(in accordance to L26_1_19 release).

====================================================================================================
Portability: ARM GCC
==================================================================================================*/

/*==================================================================================================
                                        INCLUDE FILES
==================================================================================================*/
/* Verification Test Environment Include Files */
#include "pmic_audio_test.h"

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
int     num_arg,
        n_num = 0;
char   *n_copt;

option_t options[] =
{
        {"T:", &n_num, &n_copt},        /* Test Number */
        {NULL, NULL, NULL}      /* NULL required to end array */
};

/*==================================================================================================
                                        GLOBAL CONSTANTS
==================================================================================================*/

/*==================================================================================================
                                        GLOBAL VARIABLES
==================================================================================================*/
/* Extern Global Variables */
extern int Tst_count;   /* counter for tst_xxx routines.  */
extern char *TESTDIR;   /* temporary dir created by tst_tmpdir(...) */

/* Global Variables */
char   *TCID = "pmic_testapp_audio";   /* test program identifier.  */
int     TST_TOTAL = 5;  /* total number of tests in this file.  */

/*==================================================================================================
                                    GLOBAL FUNCTION PROTOTYPES
==================================================================================================*/
void    setup(void);
void    help(void);
void    cleanup(void);
int     main(int argc, char **argv);

/*==================================================================================================
                                    LOCAL FUNCTION PROTOTYPES
==================================================================================================*/
/*================================================================================================*/
/*===== setup =====*/
/**
@brief Performs all one time setup for this test. This function is typically used to capture
       signals, create temporary dirs and temporary files that may be used in the course of this test.

@param None.

@return None.
*/
/*================================================================================================*/
void setup(void)
{
        int     VT_rv = TFAIL;

        /* VTE : Actions needed to prepare the test running */
        VT_rv = VT_pmic_audio_test_setup();
        if (VT_rv != TPASS)
        {
                tst_brkm(TBROK, cleanup, "VT_setup() Failed : error code = %d", VT_rv);
        }
        /* VTE */

        return;
}

/*================================================================================================*/
/*===== cleanup =====*/
/**
@brief This function performs all one time clean up for this test on successful completion,
       premature exit or failure. Closes all temporary files, removes all temporary directories exits
       the test with appropriate return code by calling tst_exit() function.

@param None.

@return None.
*/
/*================================================================================================*/
void cleanup(void)
{
        /* VTE : Actions needed to get a stable target environment */
        int     VT_rv = TFAIL;

        VT_rv = VT_pmic_audio_test_cleanup();
        if (VT_rv != TPASS)
        {
                tst_resm(TWARN, "VT_cleanup() Failed : error code = %d", VT_rv);
        }

        /* Exit with appropriate return code. */
        tst_exit();
}

/*================================================================================================*/
/*===== main =====*/
/**
@brief Entry point to this test-case. It parses all the command line inputs, calls the global
       setup and executes the test. It logs the test status and results appropriately using the LTP API's
       On successful completion or premature failure, cleanup() func is called and test exits with an
       appropriate return code.

@param Input : argc - number of command line parameters.
       Output: **argv - pointer to the array of the command line parameters.

@return On failure - Exits by calling cleanup().
        On success - exits with 0 exit value.
*/
/*================================================================================================*/
int main(int argc, char **argv)
{
        int     VT_rv = TFAIL;

        /* parse options. */
        char   *msg;

        if ((msg = parse_opts(argc, argv, options, &help)) != (char *) NULL)
        {
                tst_resm(TFAIL, "OPTION PARSING ERROR - %s", msg);
                return TFAIL;
        }

        if (n_num)
        {
                num_arg = atoi(n_copt);
                if (num_arg < 0 || num_arg > 4)
                {
                        tst_resm(TFAIL, "Invalid argument for -T: %s", n_copt);
                        help();
                        cleanup();
                }
        }
        else
        {
                tst_resm(TFAIL, "Required argument -T");
                help();
                cleanup();
        }

        setup();

        /* Print test Assertion using tst_resm() function with argument TINFO. */
        tst_resm(TINFO, "Testing if %s test case is OK", TCID);

        /* Test Case Body. */

        VT_rv = VT_pmic_audio_test(num_arg);

        if (VT_rv == TPASS)
        {
                tst_resm(TPASS, "test case %s %s worked as expected", TCID, n_copt);
        }

        else
        {
                tst_resm(TFAIL, "test case %s %s did NOT work as expected", TCID, n_copt);
        }

        /* cleanup test allocated ressources */
        cleanup();

        return VT_rv;
}

/*================================================================================================*/
/*===== help =====*/
/**
@brief  Print help information.

@param  None.

@return None.
*/
/*================================================================================================*/
void help(void)
{
#ifdef CONFIG_MXC_PMIC_SC55112
        printf("=================================================\n");
        printf("SC55112 audio driver test options\n");
        printf("\t  '-T 0'     Test OUTPUT functions\n");
        printf("\t  '-T 1'     Test INPUT functions\n");
        printf("\t  '-T 2'     Test SDAC functions\n");
        printf("\t  '-T 3'     Test CODEC functions\n");
        printf("\t  '-T 4'     Test BUS functions\n");
        printf("=================================================\n");
#endif

#ifdef CONFIG_MXC_PMIC_MC13783
        printf("=================================================\n");
        printf("MC13783 audio driver test options\n");
        printf("\t  '-T 0'     Test all MC13783 audio\n");
        printf("\t  '-T 1'     Test output audio functions of MC13783\n");
        printf("\t  '-T 2'     Test input audio functions of MC13783\n");
        printf("\t  '-T 3'     Test STDAC audio functions of MC13783\n");
        printf("\t  '-T 4'     Test VOICE CODEC audio functions of MC13783\n");
        printf("=================================================\n");
#endif
}
