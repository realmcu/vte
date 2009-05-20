/*====================*/
/**
        @file   pmic_light_main.c

        @brief  Main file for PMIC (sc55112 and mc13783) Light driver test.
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
D.Khoroshev/b00313           07/25/2005     TLSbo52699   Initial version
D.Khoroshev/b00313           08/29/2005     TLSbo52699   Rework version
I.Inkina/nknl001             27/12/2005     TLSbo61037   Update for MXC91131 and MXC91231
D.Khoroshev/b00313           07/25/2005     TLSbo66285   Update for VTE 2.01
D.Khoroshev/b00313           08/31/2006     TLSbo76979   Added support for both SC55112 and MC13783
                                                         platforms
====================
Portability: ARM GCC
======================*/

/*======================
Total Tests:    PMIC (SC55112 and MC13783) light

Test Name:      LED controls, LED configurate,  Fun Pattern controls ,Backlight controls,
                Backlight configurate, try to call functions, which not supported yet.
Test Assertion
& Strategy:     This test is using to test LED controls, Led configurate,  Fun Pattern controls ,
                Backlight controls, Backlight configurate,
                Try to call functions, which not supported yet.
======================*/

/*======================
                                        INCLUDE FILES
======================*/
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

/* Harness Specific Include Files. */
#include <test.h>
#include <usctest.h>

/* Verification Test Environment Include Files */
#include "pmic_light_test.h"

/*======================
                                        LOCAL MACROS
======================*/

/*======================
                                LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
======================*/

/*======================
                                        LOCAL CONSTANTS
======================*/

/*======================
                                        LOCAL VARIABLES
======================*/

/*======================
                                        GLOBAL CONSTANTS
======================*/

/*======================
                                        GLOBAL VARIABLES
======================*/
char   *TCID = "pmic_testapp_light";    /* test program identifier.  */
int     TST_TOTAL = 7;                  /* total number of tests in this file.  */

/*======================
                                GLOBAL FUNCTION PROTOTYPES
======================*/
int     main(int argc, char **argv);
void    help(void);

/*======================
                                        LOCAL FUNCTIONS
======================*/

/*====================*/
/*= main =*/
/**
@brief  Entry point to this test-case. It parses all the command line
        inputs, calls the global setup and executes the test. It logs
        the test status and results appropriately using the LTP API's
        On successful completion or premature failure test exits with
        an appropriate return code.

@param  Input :      argc - number of command line parameters.
        Output:      **argv - pointer to the array of the command line parameters.

        Describe input arguments to this test-case
        -T <test case>  specify test case. Where test case is
        0 - Test Led controls
        1 - Test Led configurate
        2 - Test Fun Pattern controls
        3 - Test Backlight controls
        4 - Test Backlight configurate
        6 - Try to call functions, which not supported by sc55112 yet.
        (Note: for mc13783 legacy only test 0, 2, 3 are available)

@return On failure - returns error code.
        On success - exits with 0 exit value.
*/
/*====================*/
int main(int argc, char **argv)
{
        int     VT_rv = TFAIL;

        /* parse options. */
        int     tflag = 0;                      /* binary flags: opt or not */
        char   *ch_test_case = 0;               /* option arguments */
        char   *msg = 0;
        int     type_case = 0;
        option_t options[] = {
                {"T:", &tflag, &ch_test_case},  /* argument required */
                {NULL, NULL, NULL}              /* NULL required to end array */
        };

        if ((msg = parse_opts(argc, argv, options, &help)) != NULL)
        {
                tst_resm(TBROK, "Parse options error.\n");
                return TFAIL;
        }

        if (tflag)
        {
                type_case = atoi(ch_test_case);

                tst_resm(TINFO, "Testing if %s %s test case is OK\n", TCID, ch_test_case);

                VT_rv = VT_pmic_light_test(type_case);
        }
        else
        {
                help();
                return TFAIL;
        }

        if (VT_rv == TPASS)
        {
                tst_resm(TPASS, "%s %s test case worked as expected\n", TCID, ch_test_case);
        }
        else
        {
                tst_resm(TFAIL, "%s %s test case did NOT work as expected\n", TCID, ch_test_case);
        }

        return VT_rv;
}

/*====================*/
/*= help =*/
/**
@brief  Inform of the available options and the associated parameters

@param  Input :      None.
        Output:      None.

@return On failure - None
        On success - None.
*/
/*====================*/
void help(void)
{
        printf("MC13783 and SC55112 PMIC light driver option\n");
        printf("  -T 0   Test TC Led IOCTLs\n");
        printf("  -T 1   Test TC Led config in FUN PATTERN\n");
        printf("  -T 2   Test TC LED config for Fun Controls\n");
        printf("  -T 3   Test TC LED for indicator Mode\n");
        printf("  -T 4   Test Backlight Functions - STRESS TEST\n");
        printf("  -T 5   Test Backlight controls with RAMP ENABLED\n");
}
