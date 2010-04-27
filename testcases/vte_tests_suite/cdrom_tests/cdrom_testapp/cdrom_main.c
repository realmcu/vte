/***
**Copyright (C) 2005-2009 Freescale Semiconductor, Inc. All Rights Reserved.
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
        @file   cdrom_driver_main.c

        @brief  Main file for ATA Disk driver test. 
*/
/*
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

#include "cdrom_testapp.h"

/*==================================================================================================
                                        GLOBAL VARIABLES
==================================================================================================*/
char   *TCID = "cdrom_driver_testapp";
int     TST_TOTAL = 1;  /* total number of tests in this file. */

/*==================================================================================================
                                    GLOBAL FUNCTION PROTOTYPES
==================================================================================================*/
void    cleanup(void);
void    setup(void);
void    help(void);
int     main(int argc, char **argv);

/*==================================================================================================
                                    GLOBAL FUNCTIONS
==================================================================================================*/

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
        int     rv = TFAIL;

        rv = VT_cdrom_driver_test_cleanup();
        if (rv != TPASS)
        {
                tst_resm(TWARN, "cdrom_test_cleanup() Failed : error code = %d", rv);
        }
        tst_exit();
}

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
        int     rv = TFAIL;

        rv = VT_cdrom_driver_test_setup();
        if (rv != TPASS)
        {
                tst_brkm(TBROK, cleanup, "setup() Failed : error code = %d", rv);
        }
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
        printf("\n===========================================================================\n");
        printf("CDROM driver option\n");
        printf("'-D device'                  Device for testing\n\n");
        printf("\t  '-T 0                    eject CD\n");
		printf("\t  '-T 1                    pendant eject CD\n");
		printf("\t  '-T 2                    get start and end tracks \n");
		printf("\t  '-T 3                    get all track's info\n");
		printf("\t  '-T 4 -V volume value    volume control setting\n");
		printf("\t  '-T 5                    Play CD\n");
		printf("\t  '-T 6                    pause CD\n");
        printf("\t  '-T 7'                   resume CD\n");
		printf("\t  '-T 8'                   stop CD \n");
		printf("\t  '-T 9'                   read sub channel info \n");
        printf("\n===========================================================================\n");
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
        // char c, *p ;
        // char *tmpstr;
        // char name[32];

        int     rv = TFAIL,
            volume=0,
            testcase = 0;
		

        /* parse options. */
        int tflag = 0,
            dflag = 0,
            vflag = 0;


        /* option arguments */
        char   *msg = 0;

        char   *topt = 0,
            *dopt = 0,
            *vopt = 0;

        option_t options[] = 
        {
                {"D:", &dflag, &dopt},
                {"T:", &tflag, &topt},
                {"V:", &vflag, &vopt},
                {NULL, NULL, NULL}
        };

        if ((msg = parse_opts(argc, argv, options, &help)) != NULL)
        {
                tst_resm(TWARN, "%s test didn't work as expected. Parse options error: %s", TCID,
                         msg);
                help();
                return rv;
        }

        /*  -------------------------------   T   -------------------------------  */
        if (tflag == 0 )
        {
                tst_resm(TFAIL, "Arg -T (test case number) is not set");
                help();
                return rv;
        }
        else
        {
                testcase = atoi(topt);
                if(testcase>9)
                {
                        tst_resm(TFAIL, "Invalid arg for -T: %d. option value out of range", testcase);
                        help();
                        return rv;
                }
        }

        /*  -------------------------------   D   -------------------------------  */
        if (dflag == 0 )
        {
                tst_resm(TFAIL, "Arg -D (device path) is not set");
                help();
                return rv;
        }

        
        

        /*  -------------------------------   V   -------------------------------  */
        if (vflag != 0)
        {
                volume= atoi(vopt);
                if(volume<0 ||volume >256)
                {
                        tst_resm(TFAIL, "Arg -V: possible values are 0 ~256 ");
                        help();
                        return rv;
                }                 
        }
        else
        {    
                if(testcase == 4) /* it is mandatory to set -F arg for testcase #1 */
                {
                        tst_resm(TFAIL, "Arg -V (dma flag) is not set");
                        help();
                        return rv;
                }
        }

        



        setup();

        rv = VT_cdrom_driver_test(dopt, testcase, volume);
        //rv = VT_cdrom_driver_test(dopt, testcase,*vopt);
        tst_resm(TINFO, "Testing %s %d test case is OK", TCID, testcase);
        if (rv == TPASS)
        {
                tst_resm(TPASS, "%s %d test case works as expected", TCID, testcase);
        }
        else
        {
                tst_resm(TFAIL, "%s %d test case didn't work as expected", TCID, testcase);
        }
 
        cleanup();
        return rv;
}
