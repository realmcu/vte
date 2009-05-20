/*====================*/
/**
        @file usb_main.c

        @brief main file for USB-OTG driver test
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
-------------------------   ------------    ----------  --------------------------------------------
A.Ozerov/B00320              07/04/2006     TLSbo58840  Initial version
A.Ozerov/B00320              29/06/2006     TLSbo71035  Help function was changed.

====================
Portability: ARM GCC

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
#include "usb_otg_test.h"

/*======================
                                    GLOBAL VARIABLES
======================*/
char   *TCID = "usb_otg_testapp_1";

int     fd;     /* PMIC test device descriptor */
int     TST_TOTAL = 0;  /* total number of tests in this file. */
int     vflag = 0;      /* verbose flag */

/*======================
                                GLOBAL FUNCTION PROTOTYPES
======================*/
void    cleanup(void);
void    setup(void);
int     main(int argc, char **argv);
void    help(void);

/*======================
                                    GLOBAL FUNCTIONS
======================*/
/*====================*/
/*= cleanup =*/
/**
@brief This function performs all one time clean up for this test on successful completion,
        premature exit or failure. Closes all temporary files, removes all temporary directories exits
        the test with appropriate return code by calling tst_exit() function.

@param None.

@return None.
*/
/*====================*/
void cleanup(void)
{
        int     rv = TFAIL;

        rv = VT_usb_otg_test_cleanup();
        if (rv != TPASS)
        {
                tst_resm(TWARN, "usb_otg_test_cleanup() Failed : error code = %d", rv);
        }

        tst_exit();
}

/*====================*/
/*= setup =*/
/**
@brief Performs all one time setup for this test. This function is typically used to capture
        signals, create temporary dirs and temporary files that may be used in the course of this test.

@param None.

@return None.
*/
/*====================*/
void setup(void)
{
        int     rv = TFAIL;

        rv = VT_usb_otg_test_setup();
        if (rv != TPASS)
        {
                tst_brkm(TBROK, cleanup, "setup() Failed : error code = %d", rv);
        }
}

/*====================*/
/*= main =*/
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
/*====================*/
int main(int argc, char **argv)
{
        int     rv = TFAIL;
        int     tflag = 0,
            vflg = 0,
            opt = 0;

        char   *msg = 0,
            *topt = 0;

        option_t options[] =
        {
                {"T:", &tflag, &topt},
                {"v", &vflg, NULL},
                {NULL, NULL, NULL}      /* NULL required to end array */
        };

        if ((msg = parse_opts(argc, argv, options, &help)) != NULL)
        {
                tst_resm(TWARN, "%s test didn't work as expected. Parse options error: %s", TCID,
                         msg);
                help();
                return rv;
        }

        if (vflg != 0)
        {
                vflag = vflg;
        }

        if (tflag == 0)
        {
                help();
                cleanup();
                return rv;
        }

        opt = atoi(topt);
        if (opt < 0 || opt > 4)
        {
                help();
                cleanup();
                return rv;
        }

        setup();

        rv = VT_usb_otg_test(opt);
        tst_resm(TINFO, "Testing %s_%d test case is OK", TCID, opt);
        if (rv == TPASS)
        {
                tst_resm(TPASS, "%s_%d test case work as expected", TCID, opt);
        }
        else
        {
                tst_resm(TFAIL, "%s_%d test case didn't work as expected", TCID, opt);
        }
        cleanup();

        return rv;
}

/*====================*/
/*= help =*/
/**
@brief  Print help information.

@param  None.

@return None.
*/
/*====================*/
void help(void)
{
        printf("\n===============\n");
        printf("USB-OTG driver option\n");
        printf("\t  '-T 0' test the bus\n");
        printf("\t  '-T 1' set/get status, serial, function and info\n");
        printf("\t  '-T 2' set/get state\n");
        printf("\t  '-T 3' set/get test");
        printf("\nUsage: %s [-T type operation]\n", TCID);
        printf("\n===============\n");
}
