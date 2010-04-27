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
        @file   rw_mmc_main.c

        @brief  Main file for MMC driver test. 
====================================================================================================
Revision History:
                            Modification     Tracking
Author/core ID                  Date          Number    Description of Changes
-------------------------   ------------    ----------  -------------------------------------------
S.ZAVJALOV/zvjs001c          22/03/2005     TLSbo46706  Initial version
A.Ozerov/b00320              20/02/2006     TLSbo61899  Testapp was cast to coding standarts

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
#include "rw_mmc_test.h"

/*==================================================================================================
                                        LOCAL VARIABLES
==================================================================================================*/
int     o_num = 0,
        t_num = 0,
        c_num = 0,
        vb_mode = 0,
        d_num = 0;
char   *t_copt,
       *o_copt,
       *c_copt,
       *device_name;
unsigned long block_size = 0,
              block_count = 0,
              offset_address = 0;

option_t options[] = 
{
        {"D:", &d_num, &device_name},   /* MMC device name */
        {"O:", &o_num, &o_copt},        /* Offset */
        {"C:", &c_num, &c_copt},        /* Block count */
        {"T:", &t_num, &t_copt},        /* Block type */
        {"V", &vb_mode, NULL},  /* Verbose mode */
        {NULL, NULL, NULL}      /* NULL required to end array */
};

/*==================================================================================================
                                        GLOBAL VARIABLES
==================================================================================================*/
/* Extern Global Variables */
extern int   Tst_count; /* counter for tst_xxx routines */
extern char *TESTDIR;   /* temporary dir created by tst_tmpdir */

/* Global Variables */
char   *TCID = "rw_mmc_test";   /* test program name */
int     TST_TOTAL = 1;  /* total number of tests in this file */

/*==================================================================================================
                                    GLOBAL FUNCTION PROTOTYPES
==================================================================================================*/
void    setup(void);
void    help(void);
int     main(int argc, char **argv);

/*==================================================================================================
                                        GLOBAL FUNCTIONS
==================================================================================================*/
/*================================================================================================*/
/*===== setup =====*/
/**
@brief  Performs all one time setup for this test. This function is typically used to capture
        signals, create temporary dirs and temporary files that may be used in the course of this test

@param  None

@return Nothing
*/
/*================================================================================================*/
void setup(void)
{
        int     VT_rv = TFAIL;

        VT_rv = VT_rw_mmc_setup();
        if (VT_rv != TPASS)
        {
                tst_brkm(TBROK, cleanup, "VT_rw_mmc_setup() Failed : error code = %d", VT_rv);
        }
}

/*================================================================================================*/
/*===== cleanup =====*/
/**
@brief  This function performs all one time clean up for this test on successful completion,
        premature exit or failure. Closes all temporary files, removes all temporary directories exits
        the test with appropriate return code by calling tst_exit() function.

@param  None

@return Nothing
*/
/*================================================================================================*/
void cleanup(void)
{
        int     VT_rv = TFAIL;

        VT_rv = VT_rw_mmc_cleanup();
        if (VT_rv != TPASS)
        {
                tst_resm(TWARN, "VT_rw_mmc_cleanup() Failed : error code = %d", VT_rv);
        }
        tst_exit();
}

/*================================================================================================*/
/*===== help =====*/
/**
@brief  Print help information

@param  None

@return Nothing
*/
/*================================================================================================*/
void help(void)
{
        printf("Usage : %s -D <MMC block device> -O <offset> -C <block count> -T <block type> -V\n",
               TCID);
        return;
}

/*================================================================================================*/
/*===== main =====*/
/**
@brief  Entry point to this test-case. It parses all the command line inputs, calls the global
        setup and executes the test. It logs the test status and results appropriately using the LTP API's
        On successful completion or premature failure, cleanup() func is called and test exits with an
        appropriate return code

@param  Input : argc - number of command line parameters
        Output: **argv - pointer to the array of the command line parameters
        
@return On failure - Exits by calling cleanup()
        On success - exits with 0 exit value
*/
/*================================================================================================*/
int main(int argc, char **argv)
{
        int     VT_rv = TFAIL;

        char   *msg;

        if ((msg = parse_opts(argc, argv, options, &help)) != (char *) NULL)
        {
                tst_brkm(TBROK, cleanup, "OPTION PARSING ERROR - %s", msg);
        }

        if (!d_num)
        {
                tst_brkm(TBROK, cleanup, "Required argument -D");
        }

        if (c_num)
        {
                block_count = strtol(c_copt, NULL, 16);
                if (block_count <= 0)
                {
                        tst_brkm(TBROK, cleanup, "Invalid arg for -C: %s", c_copt);
                        return TFAIL;
                }
        }
        else
        {
                tst_brkm(TBROK, cleanup, "Required argument -C");
        }

        if (o_num)
        {
                offset_address = strtol(o_copt, NULL, 16);
                if (offset_address < 0)
                {
                        tst_brkm(TBROK, cleanup, "Invalid arg for -O: %s", o_copt);
                        return TFAIL;
                }
        }

        if (t_num)
        {
                if (strcmp(t_copt, "BLOCK") == 0)
                        block_size = MMC_BLOCK_SIZE;
                else if (strcmp(t_copt, "SECTOR") == 0)
                        block_size = MMC_SECTOR_SIZE;
                else if (strcmp(t_copt, "BYTE") == 0)
                        block_size = 1;
                else
                        tst_brkm(TBROK, cleanup, "Invalid arg for -T: %s", t_copt);
        }
        else
        {
                tst_brkm(TBROK, cleanup, "Required argument -T");
        }

        setup();
        tst_resm(TINFO, "Testing if %s test case is OK", TCID);
        VT_rv = VT_rw_mmc_test();

        if (VT_rv == TPASS)
        {
                tst_resm(TPASS, "This test case works as expected");
        }
        else
        {
                tst_resm(TFAIL, "This test case does NOT work as expected");
        }

        cleanup();
        return VT_rv;
}
