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
        @file   pmic_convity_main.c

        @brief  Main file for PMIC Connectivity driver test.
====================================================================================================
Revision History:
                            Modification     Tracking
Author/core ID                  Date          Number     Description of Changes
-------------------------   ------------    ----------   -------------------------------------------
V.Halabuda/hlbv001           16/08/2005     TLSbo52696   Initial version
V.Halabuda/hlbv001           02/09/2005     TLSbo58397   Update for linux-2.6.10-rel-L26_1_14
A.Ozerov/b00320              15/05/2006     TLSbo64237   Code was cast in accordance to coding conventions.
A.Ozerov/b00320              10/08/2006     TLSbo74269   Conditional compilation was added.
Rakesh S Joshi/r65956		 24/07/2007		ENGR00039319 RS-232 and CEA936 operating configuration updated.

====================================================================================================
Portability: ARM GCC
==================================================================================================*/

/*==================================================================================================
                                        INCLUDE FILES
==================================================================================================*/
/* Verification Test Environment Include Files */
#include "pmic_convity_test.h"

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
/* Extern Global Variables */
extern int  Tst_count;        /* counter for tst_count routines.       */
extern char *TESTDIR;         /* temporary dir created by tst_tmpdir   */

/* Global Variables */
char *TCID = "pmic_convity_testapp"; /* test program identifier.            */
int  TST_TOTAL = 1;                /* total number of tests in this file. */

PMIC_CONVITY_TEST_IOCTL convity_testcase = 0;        /* test case */
PMIC_CONVITY_MODE argument = 0;

/*==================================================================================================
                                    GLOBAL FUNCTION PROTOTYPES
==================================================================================================*/
void cleanup(void);
void setup(void);
int main(int argc, char **argv);

/*==================================================================================================
                                    LOCAL FUNCTION PROTOTYPES
==================================================================================================*/
void help(void);

/*==================================================================================================
                                        LOCAL FUNCTIONS
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

        tst_resm(TINFO, "Close PMIC Connectivity device.");
        /* VTE : Actions needed to get a stable target environment */
        VT_rv = VT_pmic_convity_test_cleanup();
        if(VT_rv != TPASS)
        {
                tst_resm(TFAIL, "VT_cleanup() Failed : error code = %d", VT_rv);
        }
        /* Exit with appropriate return code. */
        tst_exit();
}

/*==================================================================================================
                                        LOCAL FUNCTIONS
==================================================================================================*/
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

        tst_resm(TINFO, "Open PMIC Connectivity device...");
        /* VTE : Actions needed to prepare the test running */
        VT_rv = VT_pmic_convity_test_setup();
        if(VT_rv != TPASS)
        {
                tst_resm(TFAIL, "VT_setup() Failed : error code = %d", VT_rv);
        }
        return;
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
                     -A  argument - mode of connectivity

@return On failure - Exits by calling cleanup().
        On success - exits with 0 exit value.
*/
/*================================================================================================*/
int main(int argc, char **argv)
{
        int VT_rv = TFAIL;
        int t_num = 0;
        char *msg, *t_copt = NULL;

        char *a_copt=NULL;
        int a_num = 0;

        option_t options[] =
        {
                { "T:", &t_num, &t_copt},        /* Testcases numbers */
                { "A:", &a_num, &a_copt},        /* Argument for testcase */
                { NULL, NULL, NULL }             /* NULL required to end array */
        };

        if((msg = parse_opts(argc, argv, options, &help)) != (char *) NULL)
        {
                tst_resm(TFAIL, "OPTION PARSING ERROR - %s", msg);
                return TFAIL;
        }

        if(t_num)
        {
            convity_testcase = atoi(t_copt);
            printf("(convity_testcase= %d \n", convity_testcase);
#ifdef CONFIG_MXC_PMIC_SC55112
            if (convity_testcase < 0 || convity_testcase > 8)
            {
                tst_resm(TINFO, "-T 0, 1, 2, 3, 4, 5, 6, 7, 8.");
                tst_resm(TFAIL, "Invalid arg for -T: %s. OPTION VALUE OUT OF RANGE", t_copt);
                return TFAIL;
            }
#elif CONFIG_MXC_PMIC_MC13783
            if (convity_testcase < 0 || convity_testcase > 13)
            {
                tst_resm(TINFO, "-T 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13 .");
                tst_resm(TFAIL, "Invalid arg for -T: %s. OPTION VALUE OUT OF RANGE", t_copt);
                return TFAIL;
            }
#endif
            else
            {
                switch (convity_testcase)
                {
                case 0:
                        if(a_num)
                        {
#ifdef CONFIG_MXC_PMIC_SC55112
                            if(atoi(a_copt) > 1)
                            {
                                tst_resm(TINFO, "-A: 0, 1");
                                tst_resm(TFAIL, "Invalid arg for -A: %s. OPTION VALUE OUT OF RANGE", a_copt);
                                help();
                                return TFAIL;
                            }
#elif CONFIG_MXC_PMIC_MC13783
                            if(atoi(a_copt) > 7 || atoi(a_copt) == 1 )
                            {
                                tst_resm(TINFO, "-A: 0, 2 , 3, 4, 5, 6, 7.");
                                tst_resm(TFAIL, "Invalid arg for -A: %s. OPTION VALUE OUT OF RANGE", a_copt);
                                help();
                                return TFAIL;
                            }
#endif
                        }
                        else
                        {
                            tst_resm(TFAIL, "-A must be specified");
                            return TFAIL;
                        }
                        break;
               case 1: case 2: case 3: case 4: case 5: case 6: case 7:
                        argument = USB;
                        break;
               case 8:
#ifdef CONFIG_MXC_PMIC_SC55112
                        argument = RS232;
#elif CONFIG_MXC_PMIC_MC13783
                        argument = RS232_1;
#endif
                        break;
#ifdef CONFIG_MXC_PMIC_MC13783
               case 9:
                        argument = RS232_2;
                        break;
               case 10:
                        argument = CEA936_MONO;
                        break;
               case 11:
                        argument = CEA936_STEREO;
                        break;
               case 12:
                        argument = CEA936_TEST_LEFT;
                        break;
               case 13:
                        argument = CEA936_TEST_RIGHT;
                        break;
#endif
               }
            }
        }
        else
        {
                tst_resm(TFAIL, "-T must be specified");
                help();
                return TFAIL;
        }

        /* perform global test setup, call setup() function. */
        setup();

        /* Print test Assertion using tst_resm() function with argument TINFO. */
        tst_resm(TINFO, "Testing if %s test case is OK", TCID);

        /* VTE : Execute test, print results and exit test scenario */
        VT_rv = VT_pmic_convity_test();

        if(VT_rv == TPASS)
        {
                tst_resm(TPASS, "test case %s %s worked as expected", TCID, t_copt);
        }
        else
        {
                tst_resm(TFAIL, "test case %s %s did NOT work as expected", TCID, t_copt);
        }

        /* cleanup test allocated ressources */
        cleanup();

        return VT_rv;
}

/*================================================================================================*/
/*===== help =====*/
/**
@brief  This function prints help information.

@param  None.

@return None.
*/
/*================================================================================================*/
void help(void)
{
        printf("\n==============================================================\n");
        printf("PMIC Connectivity driver options\n");
        printf("\t -T N \t Testcase name\n");
        printf("\t -A N \t Testcase additional arguments\n");
        printf("\n Usage: %s [-T testcase_number] [-A testcase_mode]\n", TCID);
        printf("\t -T [0,1,2,3,4,5,6,7,8,9]\n");
        printf("\t -A [0-7 (for MC13783), and 0-1 (for SC55112)\n");
        printf("\t -T 0 -A [*]( PMIC_CONVITY_TEST_MODE ) \n");
        printf("\t -T 1 ( PMIC_CONVITY_TEST_RESET )\n");
        printf("\t -T 2 ( PMIC_CONVITY_TEST_CALLBACK )\n");
        printf("\t -T 3 ( PMIC_CONVITY_TEST_USB_SPEED )\n");
        printf("\t -T 4 ( PMIC_CONVITY_TEST_USB_POWER )\n");
        printf("\t -T 5 ( PMIC_CONVITY_TEST_USB_TRANSCEIVER_MODE )\n");
        printf("\t -T 6 ( PMIC_CONVITY_TEST_USB_OTG_DLP_DURATION )\n");
        printf("\t -T 7 ( PMIC_CONVITY_TEST_USB_OTG_CONFIG)\n");
#ifdef CONFIG_MXC_PMIC_SC55112
        printf("\t -T 8 ( PMIC_CONVITY_TEST_RS232)\n");
#endif
#ifdef CONFIG_MXC_PMIC_MC13783
        printf("\t -T 8 ( PMIC_CONVITY_TEST_RS232_1)\n");
        printf("\t -T 9 ( PMIC_CONVITY_TEST_RS232_2)\n");
        printf("\t -T 10 ( PMIC_CONVITY_TEST_CEA936_STEREO)\n");
        printf("\t -T 11 ( PMIC_CONVITY_TEST_CEA936_MONO)\n");
        printf("\t -T 12 ( PMIC_CONVITY_TEST_CEA936_TEST_LEFT)\n");
        printf("\t -T 13 ( PMIC_CONVITY_TEST_CEA936_TEST_RIGHT)\n");
#endif
        printf("\n==============================================================\n\n");
}
