/*====================*/
/**
        @file   spi_main.c

        @brief  SPI testapp main source file
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
S.Artyom/ID                  16/06/2005     TLSbo51450  Initial version
V.Khalabuda/b00306           17/04/2006     TLSbo67010  Update version for linux-2.6.10-rel-L26_1_17
D.Kazachkov/b00316           30/08/2006     TLSbo67010  Update version for linux-2.6.10-rel-L26_1_18
D.Khoroshev/b00313           02/01/2006     TLSbo86657  Adaptation to new spi interface

====================
Portability: ARM GCC

======================*/

/*======================
Total Tests: 2

Test Name:   spi_testapp

Test Assertion
& Strategy:  Multiple write to send frames through SPI.

======================*/

/*======================
                                                INCLUDE FILES
======================*/
#include "spi_test.h"

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
pthread_t thread_id[MAX_THREAD_NUM];

int     err_check = 0;

/*======================
                                                GLOBAL CONSTANTS
======================*/

/*======================
                                                GLOBAL VARIABLES
======================*/
/* Global Variables */
char   *TCID = "spi_testapp";        /* test program identifier. */
int     TST_TOTAL = 1;               /* total number of tests in this file. */

char    device_filename[128];

/*======================
                                GLOBAL FUNCTION PROTOTYPES
======================*/
void    cleanup(void);
void    setup(void);
int     main(int argc, char **argv);

/*======================
                                LOCAL FUNCTION PROTOTYPES
======================*/
int     thread_func(void *);

/*======================
                                GLOBAL FUNCTIONS
======================*/
/*====================*/
/*= help =*/
/**
@brief  Inform of the available options and the associated parameters

@param  Input :      None.
        Output:      None.

@return Nothing
*/
/*====================*/
void help(void)
{
        printf("============\n");
        printf("SPI device driver options\n");
        printf("\t-T n, Number of threads to run,                      default is 1\n");
        printf("\t-D n, SPI number:  0 - SPI1, 1 - SPI2,               default is 0\n");
//      printf("\t-S n, Slave select number: 0-3,                      default is 2\n");
        printf("\t-N n, Number of iterations over spi_send_frame call, default is 10\n");
        printf("\nUsage: %s [-T n] [-I n] [-S n] [-N n] [-D x]\n", TCID);
}

/*======================
                                        LOCAL FUNCTIONS
======================*/
/*====================*/
/*= cleanup =*/
/**
@brief  Performs all one time clean up for this test on successful
        completion,  premature exit or  failure. Closes all temporary
        files, removes all temporary directories exits the test with
        appropriate return code by calling tst_exit() function.cleanup

@param  Input :          None.
        Output:          None.

@return Nothing
*/
/*====================*/
void cleanup(void)
{
        int     VT_rv = TFAIL;

        VT_rv = VT_spi_cleanup();
        if (VT_rv != TPASS)
        {
                tst_resm(TWARN, "VT_cleanup() Failed : error code = %d", VT_rv);
        }

        tst_exit();
}

/*====================*/
/*= setup =*/
/**
@brief  Performs all one time setup for this test. This function is
        typically used to capture signals, create temporary dirs
        and temporary files that may be used in the course of this test.

@param  Input :          None.
        Output:          None.

@return On failure - Exits by calling cleanup().
                On success - returns 0.
*/
/*====================*/
void setup(void)
{
        int     VT_rv = TFAIL;

        VT_rv = VT_spi_setup();

        if (VT_rv != TPASS)
        {
                tst_brkm(TBROK, cleanup, "VT_setup() Failed : error code = %d", VT_rv);
        }

        return;
}

/*====================*/
/*= main =*/
/**
@brief  Entry point to this test-case. It parses all the command line
        inputs, calls the global setup and executes the test. It logs
        the test status and results appropriately using the LTP API's
        On successful completion or premature failure, cleanup() func
        is called and test exits with an appropriate return code.

@param  Input :        argc - number of command line parameters.
        Output:        **argv - pointer to the array of the command line parameters.
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
        int     i,
                thread_num,
                iterations,
                spi_num,
                err;

        int     VT_rv = TFAIL;
        char   *msg;

        /* binary flags: opt or not */
        int     Tflag = 0,
                Dflag = 0,
          //   Sflag = 0,
                Nflag = 0;
        char   *Topt,
               *Dopt,
         //   *Sopt,
               *Nopt;

        option_t options[] =
        {
                {"T:", &Tflag, &Topt},        /* Testcase type - mono or multithreaded */
                {"D:", &Dflag, &Dopt},        /* SPI interface - SPI1 or SPI2 */
           //   {"S:", &Sflag, &Sopt},        /* Slave select type - 0 - 3 */
                {"N:", &Nflag, &Nopt},        /* Number of threads */
                {NULL,   NULL,  NULL}         /* NULL required to end array */
        };

        /* parse options. */
        if ((msg = parse_opts(argc, argv, options, help)) != NULL)
        {
                tst_brkm(TBROK, cleanup, "OPTION PARSING ERROR - %s", msg);
        }

        if (Tflag)
                thread_num = atoi(Topt);
        else
                thread_num = 1;
        if (thread_num >= MAX_THREAD_NUM)
                thread_num = MAX_THREAD_NUM;

/*//        memset(&param, 0x0, sizeof(params));

        if (Dflag)
      //          param.spi_num = (module_nb_t) atoi(Dopt);
        {
                spi_num = atoi (Dopt);
                if (spi_num < 0 || spi_num > 2)
                        tst_brkm(TBROK, cleanup, "Invalid spi number");
        }
        else
        {
                spi_num = 0;
        }*/
        if (Dflag)
                spi_num = atoi (Dopt);
        else
                spi_num = 0;

        sprintf(device_filename, "/dev/" MXC_SPI_DEVS "%d", spi_num);
#if 0
        if (Sflag)
                param.slave_select = (slave_select_t) atoi(Sopt);
        else
                param.slave_select = SS_2;
#endif
        if (Nflag)
                iterations = atoi(Nopt);
        else
                iterations = DEFAULT_ITER_NUM;

        /* perform global test setup, call setup() function. */
        setup();
        printf("Sleep 5s\n");
        sleep(5);

        /* Print test Assertion using tst_resm() function with argument TINFO. */
        // tst_resm(TINFO, "\nTesting if %s test case is OK\n", TCID);

        for (i = 0; i < thread_num; i++)
        {
                err =
                    pthread_create(&thread_id[i], NULL, (void *) thread_func, (void *) iterations);
                if (err != 0)
                        tst_brkm(TBROK, cleanup, "\nError: can't create thread\n");
        }

        for (i = 0; i < thread_num; i++)
        {
                pthread_join(thread_id[i], NULL);
        }

        if (err_check == 0)
        {
                VT_rv = TPASS;
        }
        if (VT_rv == TPASS)
        {
                tst_resm(TPASS, "\n%s test case worked as expected\n", TCID);
        }
        else
        {
                tst_resm(TFAIL, "\n%s test case did NOT work as expected\n", TCID);
        }
        cleanup();        /** OR tst_exit(); */

        return VT_rv;
}

/*====================*/
/*= thread_func =*/
/**
@brief  Performs SPI_SEND_FRAME ioctl

@param  Input :        None.
        Output:        None.

@return Always success
*/
/*====================*/
int thread_func(void *arg)
{
        int     err = 0;

        err = VT_spi_test((long)arg);
        if (err == 0)
        {
                tst_resm(TINFO,
                         "Thread with PID: %d called SPI_SEND_FRAME ioctl with success result\n",
                         (int) getpid());
        }
        else
        {
                tst_resm(TFAIL,
                         "Thread with PID: %d called SPI_SEND_FRAME ioctl with fail result\n",
                         (int) getpid());
                err_check |= 0x1;
        }
        return 0;
}
