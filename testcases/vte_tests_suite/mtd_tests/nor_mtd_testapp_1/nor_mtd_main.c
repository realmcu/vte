/*================================================================================================*/
/**
        @file   nor_mtd_main.c

        @brief  NOR MTD test 1 main function.
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
V.Becker/rc023c              04/05/2004     TLSbo39142  Initial version 
V.Becker/rc023c              18/06/2004     TLSbo39142  Code reviewed 
S.ZAVJALOV/zvjs001c          22/09/2004     TLSbo42376  Print MTD Flash info without any key
E.Gromazina/NONE             07/07/2005     TLSbo50888  minor fixes
A.Urusov/NONE                13/02/2006     TLSbo61868  Warnings fixup, code formatting

====================================================================================================
Portability:  ARM GCC
==================================================================================================*/

/*==================================================================================================
Total Tests: 1

Test Executable Name:  nor_mtd_testapp_1

Test Strategy:  Test of basic operations on the NOR MTD Flash device driver
=================================================================================================*/

/*==================================================================================================
                                        INCLUDE FILES
==================================================================================================*/
/* Standard Include Files */
#include <errno.h>

/* Harness Specific Include Files. */
#include <test.h>
#include <usctest.h>

/* Verification Test Environment Include Files */
#include "nor_mtd_test_1.h"

/*==================================================================================================
                                        LOCAL VARIABLES
==================================================================================================*/
/* Binary flags : option is set or not */
int     flag_size_to_write = 0;
int     flag_dev = 0;

/* Option arguments */
char    *Sizeopt;
char    *dev_opt;
char    device_name[128];

/*==================================================================================================
                            LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
==================================================================================================*/
/* Options given to the NOR Flash test.
 * Arguments are not required, we just need to know the length
 * of the pattern that will be witten and read to/from NOR Flash :
 * T : size to write or read when NOR Flash is Read only
 */
option_t write_options[] = {
        {"D:", &flag_dev, &dev_opt},        /* Device name */
        {"T:", &flag_size_to_write, &Sizeopt},
        {NULL, NULL, NULL}
};

/*==================================================================================================
                                        GLOBAL VARIABLES
==================================================================================================*/
/* Extern Global Variables */
extern int Tst_count;   /* counter for tst_xxx routines.  */
extern char *TESTDIR;   /* temporary dir created by tst_tmpdir(void) */

/* Global Variables */
char    *TCID = "nor_mtd_testapp_1";     /* test program identifier.  */
int     TST_TOTAL = 1;  /* total number of tests in this file.  */
int     flag_get_flash_information = 0;

/*==================================================================================================
                                    GLOBALover 17KB  FUNCTION PROTOTYPES
==================================================================================================*/
void    cleanup(void);
void    setup(void);
int     main(int argc, char **argv);

/*==================================================================================================
                                        GLOBAL FUNCTIONS
==================================================================================================*/
/*===== cleanup =====*/
/**
@brief  Performs all one time clean up for this test on successful
        completion, premature exit or failure. Closes all temporary
        files, removes all temporary directories exits the test with
        appropriate return code by calling tst_exit(void) function.cleanup

@param
    
@return
*/
/*================================================================================================*/
void cleanup(void)
{
        /* VTE : Actions needed to get a stable target environment */
        int     VT_rv = VT_nor_mtd_test1_cleanup();

        if (TFAIL == VT_rv)
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

@param 
    
@return On failure - Exits by calling cleanup().
        On success - returns 0.
*/
/*================================================================================================*/
void setup(void)
{
        /* VTE : Actions needed to prepare the test running */
        int     VT_rv = VT_nor_mtd_test1_setup();

        if (TFAIL == VT_rv)
        {
                tst_brkm(TBROK, cleanup, "VT_nor_mtd_setup() Failed : error code = %d", VT_rv);
        }

        return;
}

/*================================================================================================*/
/*===== help =====*/
/**
@brief  Displays help message to user
            Usage :  nor_mtd_testapp_1 -h

@param  none

@return Nothing
*/
/*================================================================================================*/
void help(void)
{
        printf("-T <size to write or read>   size of data in bytes to program in NOR Flash\n");
        printf("\t\t\t or size of data in bytes to read when NOR Flash is read only\n");
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
    
@return On failure - Exits by calling cleanup().
        On success - exits with 0 exit value.
*/
/*================================================================================================*/
int main(int argc, char **argv)
{
        int     VT_rv = TFAIL;
        int     Base = 16;
        char    *msg;
        unsigned long size = SIZE_WRITE_BASE;

        /* Parse user defined options */
        msg = parse_opts(argc, argv, write_options, help);
        if (msg != (char *) NULL)
        {
                tst_brkm(TBROK, cleanup, "OPTION PARSING ERROR - %s", msg);
        }

        if (flag_dev)
        {
                strcpy(device_name, dev_opt);
        }
        else
        {
                strcpy(device_name, NOR_MTD_DRIVER);
        }

        /* Set program mode by checking options flags */
        if (flag_size_to_write)
        {
                /* Convert hex string starting with 0x into hex value */
                size = strtol(Sizeopt, NULL, Base);
                if (size > FLASH_SIZE)
                {
                        tst_resm(TWARN, "Size to write bigger than Flash size -> "
                                 "truncating size to 0x02000000");
                        size = FLASH_SIZE - 1;
                }
        }
        else
        {
                flag_get_flash_information = 1;
        }

        /* Perform global test setup, call setup() function */
        setup();

        /* Print test Assertion using tst_resm() function with argument TINFO */
        tst_resm(TINFO, "Testing if %s test case is OK", TCID);

        /* VTE : Execute test, print results and exit test scenario */
        VT_rv = VT_nor_mtd_test1(size);

        if (VT_rv == TPASS)
        {
                tst_resm(TPASS, "test case %s worked as expected", TCID);
        }
        else
        {
                tst_resm(TFAIL, "test case %s did NOT work as expected", TCID);
        }

        /* cleanup test allocated ressources */
        cleanup();

        return VT_rv;
}
