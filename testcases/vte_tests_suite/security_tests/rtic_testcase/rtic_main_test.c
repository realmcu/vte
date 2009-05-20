/*====================*/
/**
    @file   rtic_main.c

    @brief   rtic API test main function.
*/
/*======================

        Copyright 2004, Freescale Semiconductor, Inc. All Rights Reserved
        THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
        BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
        Freescale Semiconductor, Inc.

====================
Revision History:
                            Modification     Tracking
Author                          Date          Number    Description of Changes
-------------------------   ------------    ----------  -------------------------------------------
S.ZAVJALOV/zvjs001c          19/10/2004     TLSbo43475   Initial version
A.URUSOV                     13/09/2005     TLSbo55076   Fix compilation issue and warnings
A.URUSOV                     01/11/2005     TLSbo57063   Compile under L26.1.14
S.V-Guilhou/svan01c          08/11/2005     TLSbo53743   Bus error (bad string allocation)
======================*/

/*======================
Total Tests: 1

Test Executable Name:  rtic_test

Test Strategy: Examine the RTIC module functions
=====================*/


#ifdef __cplusplus
extern "C"{
#endif

/*======================
                                        INCLUDE FILES
======================*/
/* Standard Include Files */
#include <errno.h>

/* Harness Specific Include Files. */
#include "test.h"
#include "usctest.h"

/* Verification Test Environment Include Files */
#include "rtic_test.h"

/*======================
                                       GLOBAL VARIABLES
======================*/
/* Extern Global Variables */
extern int  Tst_count;               /* counter for tst_xxx routines.         */
extern char *TESTDIR;                /* temporary dir created by tst_tmpdir */

/* Global Variables */
char *TCID     = "rtic_test";      /* test program identifier.          */
int  TST_TOTAL = 1;                  /* total number of tests in this file.   */

char *data_file_path = 0;
rtic_test_param arg;
int runtime_mode = CASE_TEST_RTIC_ONETIME;

/*======================
                                   GLOBAL FUNCTION PROTOTYPES
======================*/
void cleanup(void);
void setup(void);
int main(int argc, char **argv);

/*======================
                                   LOCAL FUNCTION PROTOTYPES
======================*/

void help(void);

/*====================*/
/*= cleanup =*/
/**
@brief  Performs all one time clean up for this test on successful
        completion,  premature exit or  failure. Closes all temporary
        files, removes all temporary directories exits the test with
        appropriate return code by calling tst_exit() function.cleanup

@param  Input :
        Output:

@return
*/
/*====================*/
void cleanup(void)
{
    /* VTE : Actions needed to get a stable target environment */
        int VT_rv = TFAIL;

        VT_rv = VT_rtic_test_cleanup();
        if (VT_rv != TPASS)
        {
                tst_resm(TWARN, "VT_cleanup() Failed : error code = %d", VT_rv);
        }

    /* Exit with appropriate return code. */
        tst_exit();
}

/*======================
                                       LOCAL FUNCTIONS
======================*/

/*====================*/
/*= setup =*/
/**
@brief  Performs all one time setup for this test. This function is
        typically used to capture signals, create temporary dirs
        and temporary files that may be used in the course of this test.

@param  Input :
        Output:

@return On failure - Exits by calling cleanup().
        On success - returns 0.
*/
/*====================*/
void setup(void)
{
        int VT_rv = TFAIL;

        /* VTE : Actions needed to prepare the test running */
        VT_rv = VT_rtic_test_setup();
        if (VT_rv != TPASS)
        {
                tst_brkm(TBROK , cleanup, "VT_rtic_setup() Failed : error code = %d", VT_rv);
        }
}


/*====================*/
/*= main =*/
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
/*====================*/
int main(int argc, char **argv)
{
        int VT_rv = TFAIL;
        int f_num = 0, m_num = 0, r_num = 0, b_num=0, v_num=0, c_num=0, d_num=0;
        int path_length;
        char *f_copt, *m_copt, *r_copt, *b_copt;
        char *msg;

        if(1 == argc)
        {
                help();
                return TPASS;
        }

        option_t options[] = {
        { "F:", &f_num, &f_copt},                       /* Data file */
        { "M:", &m_num, &m_copt},                       /* Hashing mode */
        { "R:", &r_num, &r_copt},                       /* Interrupt */
        { "B:", &b_num, &b_copt},                       /* Memory block to be hashed */
        { "v", &v_num, NULL},                           /* Verbose mode */
        { "C", &c_num, NULL},                           /* Read the control register */
        { "D", &d_num, NULL},                           /* Read the fault address register */
        { NULL, NULL, NULL }                            /* NULL required to end array */
        };

        if ( (msg = parse_opts(argc, argv, options, &help)) != (char *) NULL )
        {
                tst_resm(TFAIL, "OPTION PARSING ERROR - %s", msg);
                return TFAIL;
        }

        if (b_num)
        {
                if (strcmp(b_copt, "A1") == 0 ) arg.block_memory = RTIC_A1;
                else if (strcmp(b_copt, "A2") == 0) arg.block_memory = RTIC_A2;
                else if (strcmp(b_copt, "B1") == 0) arg.block_memory = RTIC_B1;
                else if (strcmp(b_copt, "B2") == 0) arg.block_memory = RTIC_B2;
                else if (strcmp(b_copt, "C1") == 0) arg.block_memory = RTIC_C1;
                else if (strcmp(b_copt, "C2") == 0) arg.block_memory = RTIC_C2;
                else if (strcmp(b_copt, "D1") == 0) arg.block_memory = RTIC_D1;
                else if (strcmp(b_copt, "D2") == 0) arg.block_memory = RTIC_D2;
                else
                {
                        tst_resm(TFAIL, "OPTION PARSING ERROR - %s", b_copt);
                        return TFAIL;
                }
        }
        else
        {
                arg.block_memory = RTIC_A1;
        }

        if (m_num)
        {
                if (strcmp(m_copt, "ONETIME") == 0 )
                {
                        runtime_mode = CASE_TEST_RTIC_ONETIME;
                }
                else if (strcmp(m_copt, "RUNTIME") == 0)
                {
                        runtime_mode = CASE_TEST_RTIC_RUNTIME;
                }
                else
                {
                        tst_resm(TFAIL, "OPTION PARSING ERROR - %s", m_copt);
                        return TFAIL;
                }
        }

        if (f_num)
        {
                path_length = strlen(f_copt)+1;
                if (path_length > 0 )
                {
                        if (! (data_file_path = (char *)malloc(sizeof(char) * path_length)) )
                        {
                                tst_resm(TFAIL, "Cant allocate memory");
                                return TFAIL;
                        }
                }
                else
                {
                        tst_resm(TFAIL, "OPTION PARSING ERROR - %s", f_copt);
                        return TFAIL;
                }
                strcpy(data_file_path, f_copt);
        }

        if (r_num)
        {
                arg.interrupt = atoi(r_copt);
                if (arg.interrupt < 0)
                {
                        tst_resm(TFAIL, "Invalid arg for -R: %s", r_copt);
                        if (arg.interrupt != 0) free(data_file_path);
                        return TFAIL;
                }
        }
        else
        {
                arg.interrupt = 0;
        }

        if (v_num)
        {
                arg.verbose_mode = 1;
        }
        else
        {
                arg.verbose_mode = 0;
        }

        /* Reads the control register of the RTIC */
        if (c_num)
        {
                runtime_mode = CASE_TEST_RTIC_GET_CONTROL;
        }

        /* Reads the fault address register of the RTIC */
        if (d_num)
        {
                runtime_mode = CASE_TEST_RTIC_GET_FAULTADDRESS;
        }

        /* perform global test setup, call setup() function. */
        setup();

        /* Print test Assertion using tst_resm() function with argument TINFO. */
        tst_resm(TINFO, "Testing if %s test case is OK", TCID);

        /* VTE : Execute test, print results and exit test scenario */
        VT_rv = VT_rtic_test();

        if(VT_rv == TPASS)
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

#ifdef __cplusplus
}
#endif
