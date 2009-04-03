/*================================================================================================*/
/**
        @file   ata_driver_main.c

        @brief  Main file for ATA Disk driver test. 
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
A.Ozerov/b00320              10/09/2006     TLSbo76800  Initial version.
D.Kazachkov/b00316            6/12/2006     TLSbo80788  Parse opt fix

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

#include "libata_testapp.h"

/*==================================================================================================
                                        GLOBAL VARIABLES
==================================================================================================*/
char   *TCID = "ata_driver_testapp";
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

        rv = VT_ata_driver_test_cleanup();
        if (rv != TPASS)
        {
                tst_resm(TWARN, "pmic_battery_test_cleanup() Failed : error code = %d", rv);
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

        rv = VT_ata_driver_test_setup();
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
        printf("ATA Disk driver option\n");
        printf("'-B flag'                    flush buffer cache for device on exit(flag: 0,1)\n");
        printf("'-D device'                  Device for testing\n\n");
        printf("\t  '-T 0 -O flag'           get/set IDE 32-bit IO setting(mode: 0,1)\n");
        printf("\t  '-T 1'                   display drive geometry\n");
        printf("\t  '-T 2'                   display drive identification\n");
        printf("\t  '-T 3'                   perform reading the device timings\n");
        printf("\t  '-T 4 -X mode'           set IDE xfer mode(DANGEROUS)(mode: 0,1,15,16,39,71)");
		printf("\t  '-T 5'                   PIO mode read/write performance'");
		printf("\t  '-T 6'                   MDMA mode read/write performance");
		printf("\t  '-T 7'                   UDMA mode read/write performance");
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
            testcase = 0,
            xfermode = 0,
            dma_flag = 0,
            io32bit = 0,
            flush_buffer = 0;

        /* parse options. */
        int     tflag = 0,
            dflag = 0,
            xflag = 0,
            fflag = 0,
            iflag = 0,
            bflag = 0;

        /* option arguments */
        char   *msg = 0;

        char   *topt = 0,
            *dopt = 0,
            *xopt = 0,
            *fopt = 0,
            *iopt = 0,
            *bopt = 0;

        option_t options[] = 
        {
                {"D:", &dflag, &dopt},
                {"T:", &tflag, &topt},
                {"X:", &xflag, &xopt},
                {"F:", &fflag, &fopt},
                {"O:", &iflag, &iopt},
                {"B:", &bflag, &bopt},
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
                //if(testcase>>3)
                if(testcase>7)
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

        /*  -------------------------------   B   -------------------------------  */
        if (bflag != 0)
        {
                flush_buffer = atoi(bopt);
                if(flush_buffer>>1)
                {
                        tst_resm(TFAIL, "Arg -B: possible values are 0 and 1");
                        help();
                        return rv;
                }
        }

        /*  -------------------------------   O   -------------------------------  */
        if (iflag != 0)
        {
                io32bit = atoi(iopt);
                if(io32bit>>1)
                {
                        tst_resm(TFAIL, "Arg -O: possible values are 0 and 1");
                        help();
                        return rv;
                }
        }

        /*  -------------------------------   F   -------------------------------  */

        /*  -------------------------------   X   -------------------------------  */
        if (xflag != 0)
        {
                xfermode = atoi(xopt);
        }
        else
        {    
                if(testcase == 4) /* it is mandatory to set -X arg for testcase #7 */
                {
                        tst_resm(TFAIL, "Arg -X (xfer mode) is not set");
                        help();
                        return rv;
                }
        }



        setup();

        rv = VT_ata_driver_test(dopt, testcase, dma_flag, io32bit, xfermode, flush_buffer);

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
