/*/
  Copyright (C) 2004, Freescale Semiconductor, Inc. All Rights Reserved
  THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
  BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
  Freescale Semiconductor, Inc.
*/

/**
@file mu_sdma_ipc_main.c

@brief VTE C main source file of mu_sdma_ipc_test application.

Description of the file

@par Portability:
        Indicate if this module is portable to other compilers or platforms.
        If not, indicate specific reasons why is it not portable.
*/

/*===== REVISION HISTORY =======

Author (core ID)          Date         CR Number    Description of Changes
-----------------------   ----------   ----------   ----------------------------
I. Semenchukov/smng001c   09/03/2005   TLSbo47942   Initial version
Y. Batrakov               09/01/2006   TLSbo75877   Rework to write data
                                                    according to the MU protocol


==================*/

/*================
Total Tests: 1

Test Name:   mu_sdma_ipc_test

Test Assertion
& Strategy:  Test creates two threads. One of them communicates with MU via
             on of its device files, second communicates with SDMA controller.
             SDMA is turned into loopback mode. Each thread opens the device
             file, performs some subsequent read/write cycles via this device,
             then closes the device file.
             Test fails if one of listed operations is failed:
                open a device
                close a device
                read from device
                write to device
                set operating mode of device (for SDMA)
                buffers contents match for write and read buffers after write
                   and read operation

=================*/

#ifdef __cplusplus
extern "C"{
#endif

/*======== INCLUDE FILES ========*/
/* Standard Include Files */
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

/* Harness Specific Include Files. */

#include "test.h"
#include "usctest.h"

/* Verification Test Environment Include Files */

#include "mu_sdma_ipc_test.h"

/*======== LOCAL CONSTANTS ==========*/


/*======== LOCAL MACROS =========*/


/*======== LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS) ===*/


/*======== LOCAL VARIABLES ==========*/
/* parse_opts() four parameters. First parameter defines number of write/read
 * cycles (default is 2). Second parameter defines SDMA buffer size (default
 * is 128). Third parameter defines SDMA device number (0 for /dev/sdma0 and
 * so on). Forth parametes defines the number of SDMA devices to write (default
 * is 1).
 * All parameters require arguments.
 */

int   iter_flag;
int   bufsz_flag;
int   sddev_flag;
int   sddev_num_flag;
char* iter_opt;
char* bufsz_opt;
char* sddev_num_opt;
char* sddev_opt;

option_t options[] =
{
    { "I:",  &iter_flag,       &iter_opt  },       /* Number of iterations           */
    { "B:",  &bufsz_flag,      &bufsz_opt },       /* Size of the SDMA buffer        */
    { "S:",  &sddev_flag,      &sddev_opt },       /* SDMA device number             */
    { "N:",  &sddev_num_flag,  &sddev_num_opt },   /* Number of SDMA devices to test */
    { NULL,  NULL,             NULL       },       /* NULL required to end array     */
};

dev_node_t dev_node[2];

/*======== GLOBAL CONSTANTS =========*/


/*======== GLOBAL VARIABLES =========*/
/* Extern Global Variables */

extern int   Tst_count;          /* counter for tst_xxx routines             */
extern char* TESTDIR;            /* temporary dir created by tst_tmpdir(void)*/

/* Global Variables */

char* TCID      = "mu_sdma_ipc_test";/* test program name                    */
int   TST_TOTAL = 1;                 /* total number of tests in this file   */

/*======== LOCAL FUNCTION PROTOTYPES ========*/
void setup(void);
void help(void);
int  main(int argc, char** argv);

/*======== LOCAL FUNCTIONS ==========*/

/*= setup =*/
/**
Description of the function
@brief  Performs all one time setup for this test. This function is
        typically used to capture signals, create temporary dirs
        and temporary files that may be used in the course of this test.
@pre None
@post None
@param  Input : None.
        Output: None.
@return On failure - Exits by calling cleanup().
        On success - returns 0.
@bug No bug
@deprecated Not applicable
@todo Nothing
*/
void setup(void)
{
    int VT_rv = TFAIL;

    VT_rv = VT_mu_sdma_ipc_setup();
    if (VT_rv != TPASS)
    {
        tst_brkm(TBROK , cleanup, "VT_mu_sdma_ipc_setup() Failed : error code = %d", VT_rv);
    }

    return;
}

/*= help =*/
/**
Description of the function
@brief  Describe the usage of the test application
        List the needed arguments
@pre None
@post None
@param  Input : None.
        Output: None.
@return On failure - Exits by calling cleanup().
        On success - returns 0.
@bug No bug
@deprecated Not applicable
@todo Nothing
*/
void help(void)
{
    printf("Usage : %s [options]\n"
           "\t-B <buf_size> - SDMA transfer buffer size\n"
           "\t-I <iter_num> - number of read/write cycles\n"
           "\t-S <sdma_dev> - SDMA device number to use\n"
           "\t-N <sdma_num> - number of SDMA devices to test", TCID);

    return;
}

/*= main =*/
/**
@brief  Entry point to this test-case. It parses all the command line
        inputs, calls the global setup and executes the test. It logs
        the test status and results appropriately using the LTP API's
        On successful completion or premature failure, cleanup() func
        is called and test exits with an appropriate return code.
@pre  None
@post None
@param  Input : argc - number of command line parameters.
        Output: **argv - pointer to the array of the command line parameters.
                Describe input arguments to this test-case
                    -l - Number of iteration
                    -v - Prints verbose output
                    -V - Prints the version number
@return On failure - Exits by calling cleanup().
        On success - exits with 0 exit value.
@bug No bug
@deprecated Not applicable
@todo Nothing
*/
int main(int argc, char **argv)
{
    int VT_rv = TFAIL;
    char* err_msg;
    int num_iter = DEF_NR_ITER;
    int sdma_bufsz = DEF_SDMA_BUFSZ;

    /* parse options. */

    if ( (err_msg = parse_opts(argc, argv, options, &help)) != NULL )
        tst_brkm(TBROK, cleanup, "OPTION PARSING ERROR - %s", err_msg);

    /* Argument handle */

    if (iter_flag)
    {
        num_iter = atoi(iter_opt);
        if (num_iter < 1)
            tst_brkm(TBROK, cleanup, "Number of iterations must be > 0");
    }
    if (bufsz_flag)
    {
        sdma_bufsz = atoi(bufsz_opt);
        if (sdma_bufsz <= 0)
            tst_brkm(TBROK, cleanup, "SDMA buffer size must be > 0");
    }

    dev_node[0].dev_prefix = DEF_SDMA_DEV;
    dev_node[0].first_dev  = sddev_flag     ? atoi(sddev_opt)     : DEF_SDMA_FIRST_DEV;
    dev_node[0].num_devs   = sddev_num_flag ? atoi(sddev_num_opt) : DEF_SDMA_NUM_DEVS;
    dev_node[0].buf_size   = sdma_bufsz;

    dev_node[1].dev_prefix = DEF_MU_DEV;
    dev_node[1].first_dev  = DEF_MU_FIRST_DEV;
    dev_node[1].num_devs   = DEF_MU_NUM_DEVS;
    dev_node[1].buf_size   = MU_BUFLEN;

    /* perform global test setup, call setup() function. */

    setup();

    /* Print test Assertion using tst_resm() function with argument TINFO. */

    tst_resm(TINFO, "Testing if %s test case is OK", TCID);
    VT_rv = VT_mu_sdma_ipc_test(num_iter, dev_node);

    /* VTE : print results and exit test scenario */

    if(VT_rv == TPASS)
        tst_resm(TPASS, "This test case worked as expected");
    else
        tst_resm(TFAIL, "This test case did NOT work as expected");

    cleanup();

    return VT_rv;
}

/*======== GLOBAL FUNCTIONS =========*/

/*= cleanup =*/
/**
Description of the function
@brief  Performs all one time clean up for this test on successful
        completion,  premature exit or  failure. Closes all temporary
        files, removes all temporary directories exits the test with
        appropriate return code by calling tst_exit() function.cleanup
@pre None
@post None
@param  Input : None.
        Output: None.
@return Nothing
@bug No bug
@deprecated Not applicable
@todo Nothing
*/
void cleanup(void)
{
    int VT_rv = TFAIL;

    VT_rv = VT_mu_sdma_ipc_cleanup();
    if (VT_rv != TPASS)
        tst_resm(TWARN, "VT_mu_sdma_ipc_cleanup() Failed : error code = %d", VT_rv);
    tst_exit();
}

#ifdef __cplusplus
}
#endif
