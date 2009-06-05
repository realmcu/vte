/*================================================================================================*/
/**
    @file   mu_api_rw_main.c

    @brief  Main file of the mu_api_rw_test test that checks Messaging Unit
            driver read() and write() system calls in both blocking and non-blocking modes.
*/
/*==================================================================================================

Copyright (C) 2004, Freescale Semiconductor, Inc. All Rights Reserved
  THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
  BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
  Freescale Semiconductor, Inc.
     
====================================================================================================
Revision History:
                              Modification     Tracking
Author (Core ID)                  Date          Number    Description of Changes
---------------------------   ------------    ----------  ------------------------------------------
Igor Semenchukov (smng001c)    23/08/2004     TLSbo40411   Initial version 
Igor Semenchukov (smng001c)    08/12/2004     TLSbo43804   Rework after heavy MU drv modification;
                                                           merge two test cases in the one 

====================================================================================================
Portability: Indicate if this module is portable to other compilers or platforms. 
             If not, indicate specific reasons why is it not portable.

==================================================================================================*/

/*==================================================================================================
Total Tests:    1

Test Name:      mu_api_rw_test

Test Assertion
& Strategy:     Checks Messaging Unit driver read() and write() functions in blocking and
                non-blocking modes. This mode is selected by a switch.
                Each device file is tested. Main test function sequentally opens a file, then reads
                message from it, then writes message to it, and finally closes a file.
                The following Messaging Unit driver functions are tested:
                    mxc_mu_read()
                    mxc_mu_write()
==================================================================================================*/

#ifdef __cplusplus
extern "C"{
#endif

/*==================================================================================================
                                        INCLUDE FILES
==================================================================================================*/

/* Standard Include Files */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

/* Harness Specific Include Files. */

#include "test.h"
#include "usctest.h"

/* Verification Test Environment Include Files */

#include "mu_api_rw_test.h"

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

/*
 * parse_opts() parameters. First option (msg) has flag -M and accepts message that will be written
 * to MU transmit registers. Second option (blk) indicates whether blocking read/write is used. It
 * has flag -O and, if it is set, uses non-blocking mode.
 */

int  msg_flag = 0;
int  blk_flag = 0;
char *msg_opt;

option_t test_opts[] = 
{
    { "M:", &msg_flag, &msg_opt },
    { "O",  &blk_flag, NULL     },
    { NULL, NULL,      NULL     },
};


/* Default message to write */
char def_msg[] = "aaaabbbbccccddddeeeeffffgggghhhhiiiijjjjkkkkllllmmmmnnnnooooppppqqqqrrrrssssttttuuuuvvvvwwwwxxxxyyyyzzzz0000111122223333444455556666777788889999"; 

/*==================================================================================================
                                       GLOBAL CONSTANTS
==================================================================================================*/


/*==================================================================================================
                                       GLOBAL VARIABLES
==================================================================================================*/

/* Extern Global Variables */

extern int  Tst_count;               /* counter for tst_xxx routines.         */
extern char *TESTDIR;                /* temporary dir created by tst_tmpdir(void) */

/* Global Variables */

char *TCID     = "mu_api_rw_test";  /* test program identifier.              */
int  TST_TOTAL = 1;                  /* total number of tests in this file    */

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
                                       GLOBAL FUNCTIONS
==================================================================================================*/

/*================================================================================================*/
/*===== cleanup =====*/
/**
@brief  Performs all one time clean up for this test on successful
completion,  premature exit or failure. Closes all temporary
files, removes all temporary directories exits the test with
appropriate return code by calling tst_exit(void) function.cleanup

@param  Input :      None.
        Output:      None.
  
@return Nothing
*/
/*================================================================================================*/
void cleanup(void)
{

    /* VTE : Actions needed to get a stable target environment */

    int VT_rv = TFAIL;
    VT_rv = VT_mu_api_rw_cleanup();
    if (VT_rv != TPASS)
    {
    tst_resm(TWARN, "VT_mu_api_rw_cleanup() Failed : error code = %d", VT_rv);
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
    /* VTE : Actions needed to prepare the test running */
    VT_rv = VT_mu_api_rw_setup();
    if (VT_rv != TPASS)
    {
    tst_brkm(TBROK , cleanup, "VT_mu_api_rw_setup() Failed : error code = %d", VT_rv);
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

@param  Input :  argc - number of command line parameters.
                 **argv - pointer to the array of the command line parameters.
        Output:  None

@return On failure - Exits by calling cleanup().
        On success - exits with 0 exit value.
*/
/*================================================================================================*/
int main(int argc, char **argv)
{
    int  i;
    char *err_str;             /* parse_opts() error string */
    char *msg   = def_msg;
    int  VT_rv  = TFAIL;
    if ( (err_str = parse_opts(argc, argv, test_opts, help)) != NULL )
    {
    tst_brkm(TBROK, cleanup, "OPTION PARSING ERROR - %s", err_str);
    }
    if (msg_flag)
    {
        if ( (strlen(msg_opt) > BUF_LEN) || (strlen(msg_opt) % REG_SIZE) )
        {
            fprintf(stderr, "\tERROR: message length must be <= %d and multiple of %d bytes\n",
                    BUF_LEN, REG_SIZE);
            help();
            tst_brkm(TBROK, cleanup, "OPTION PARSING ERROR");
        }
        else
            msg =  msg_opt;
    }

    printf("\tMessage will be used - ");
    for (i = 0; i < strlen(msg); i++)
        printf("%X", msg[i]);
    printf("\n");

    if (blk_flag)
        printf("\tNon-blocking mode will be used\n");
    else
        printf("\tBlocking mode will be used\n");

    /* perform global test setup, call setup() function. */

    setup();

    /* Print test Assertion using tst_resm() function with argument TINFO. */

    tst_resm(TINFO, "Testing if %s test case is OK\n", TCID);

    VT_rv = VT_mu_api_rw_test(msg, blk_flag);
    /* VTE : print results and exit test scenario */
    if(VT_rv == TPASS)
        tst_resm(TPASS, "%s test case worked as expected", TCID);
    else
        tst_resm(TFAIL, "%s test case did NOT work as expected", TCID);

    cleanup();

    return VT_rv;
}

/*================================================================================================*/
/*===== help =====*/
/**
@brief  Displays the program usage.

@param  Input:	None
    Output: None

@return	None
*/
/*================================================================================================*/
void help(void)
{
    fprintf(stderr, "Usage:\t%s [OPTION]\n\n", TCID);
    fprintf(stderr, "  -M msg\tuse msg when write to the MU transmit registers\n");
    fprintf(stderr, "\t\tASCII codes of message characters will be used\n");
    fprintf(stderr, "\t\tf.e. if message is 'QWER', the value '51574552' will be written\n");
    fprintf(stderr, "\t\tmsg must be multiple of %d bytes (%d characters)\n", REG_SIZE, REG_SIZE);
    fprintf(stderr, "  -O\t\tuse non-blocking read/write\n");
    return;
}

#ifdef __cplusplus
}
#endif
