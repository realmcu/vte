/*================================================================================================*/
/**
        @file   ipc_main.c

        @brief  Main file for Unified IPC driver test application.
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
I.Semenchukov/smng001c       31/03/2005     TLSbo47812  Initial version
V.Khalabuda/hlbv001          17/05/2005     TLSbo50460  Option behaviour
S.Zavjalov/zvjs001c          24/06/2005     TLSbo50997  Linked list mode
A.Ozerov/b00320              26/04/2006     TLSbo61791  Performs a cast in accordance to coding standarts

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
#include "ipc_test.h"

/*==================================================================================================
                                        DEFINES AND MACROS
==================================================================================================*/
#define DEF_PKT_LEN         8192
#define DEF_LOG_LEN         2048
#define MIN_BUF_ITER        0
#define NR_ITER             1

/*==================================================================================================
                                        LOCAL VARIABLES
==================================================================================================*/
static sig_atomic_t     sig_count;
static struct   sigaction sa;

int     id_flag;
int     pktsz_flag;
int     logsz_flag;
int     dev_flag;
int     iter_flag;
int     nonblk_flag;
int     qtty_flag;
char   *id_opt;
char   *pktsz_opt;
char   *logsz_opt;
char   *dev_opt;
char   *iter_opt;
char   *qtty_opt;

option_t options[] = 
{
        {"N:", &id_flag, &id_opt},              /* Test ID */
        {"S:", &pktsz_flag, &pktsz_opt},        /* Packet buffer size */
        {"L:", &logsz_flag, &logsz_opt},        /* Log buffer size */
        {"D:", &dev_flag, &dev_opt},            /* Device to use */
        {"C:", &iter_flag, &iter_opt},          /* Number of r/w iterations */
        {"Q:", &qtty_flag, &qtty_opt},          /* Segments quantity */
        {"O", &nonblk_flag, NULL},              /* Use non-block mode */
        {NULL, NULL, NULL},                     /* NULL required to end array */
};

/*==================================================================================================
                                        GLOBAL VARIABLES
==================================================================================================*/
/* Extern Global Variables */
extern int      Tst_count;        /* counter for tst_xxx routines              */
extern char    *TESTDIR;          /* temporary dir created by tst_tmpdir(void) */

/* Global Variables */
char   *TCID = "ipc_testapp";        /* test program name                  */
int     TST_TOTAL = 1;               /* total number of tests in this file */

/* Declaration the name and path to device */
char    dev_fname[MAX_STR_LEN];
char   *dev_path;
int     dev_num = -1;

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
/*===== help =====*/
/**
Description of the function
@brief  Describes the usage of the test application.
        Lists the needed arguments.

@param  Input : None.
        Output: None.

@return On failure - Exits by calling cleanup().
        On success - returns 0.
*/
/*================================================================================================*/
void help(void)
{
        printf("Usage: %s [options]\n", TCID);
        printf("\t-D <num>\tdevice number to use\n");
        printf("\t-S <pkt_size>\tsize of packet data buffer\n");
        printf("\t-L <log_size>\tsize of log buffer\n");
        printf("\t-C <count>\tnumber of read/write iterations\n");
        printf("\t-O\t\tuse non-block mode\n");
        printf("\t-N <testcase>\ttest case ID\n");
        printf("\t-Q <num>\tsegments quantity\n");
        printf("\n\tAvailable test case IDs are:\n");
        printf("\t\t0 - Packet data exchange in mode 1\n");
        printf("\t\t1 - Packet data exchange in mode 2\n");
        printf("\t\t2 - IPC ioctl() test\n");
        printf("\t\t3 - power management test\n");
        printf("\t\t4 - errors test\n");
        printf("\t\t5 - send/receive short messages in parallel\n");
        printf("\t\t6 - data and log transfer in parallel\n");
        printf("\t\t7 - parallel transfer of all kind of data\n");
        printf("\t\t8 - linked list mode test\n");
        printf("\t\t9 - send/receive short messages, data or receive log(depends on device)\n\n");

        return;
}

/*==================================================================================================
                                        LOCAL FUNCTIONS
==================================================================================================*/
void sig_hand(int sig);

/*================================================================================================*/
/*===== setup =====*/
/**
@brief  Performs all one time setup for this test. This function is
        typically used to capture signals, create temporary dirs
        and temporary files that may be used in the course of this test.

@param  Input : None.
        Output: None.

@return On failure - Exits by calling cleanup().
        On success - returns 0.
*/
/*================================================================================================*/
void setup(void)
{
        int     VT_rv = TFAIL;

        sig_count = 0;                        /* Zero signal counter        */
        memset(&sa, 0, sizeof(sa));
        sa.sa_handler = &sig_hand;            /* Assign a signal handler    */

        /* Register it    */
        if (sigaction(SIGALRM, &sa, NULL))
        {
                tst_resm(TFAIL, "Cannot install signal handler: %s",
                strerror(errno));
        }

        /* VTE : Actions needed to prepare the test running */
        VT_rv = VT_ipc_setup();
        if (VT_rv != TPASS)
        {
                tst_brkm(TBROK, cleanup, "VT_ipc_setup() Failed : error code = %d", VT_rv);
        }
}

/*================================================================================================*/
/*===== cleanup =====*/
/**
@brief  Performs all one time clean up for this test on successful
                completion,  premature exit or  failure. Closes all temporary
                files, removes all temporary directories exits the test with
                appropriate return code by calling tst_exit() function.cleanup

@param  Input : None.
        Output: None.

@return Nothing
*/
/*================================================================================================*/
void cleanup(void)
{
        int     VT_rv = TFAIL;

        sa.sa_handler = SIG_IGN;

        VT_rv = VT_ipc_cleanup();
        if (VT_rv != TPASS)
        {
                tst_resm(TWARN, "VT_ipc_cleanup() Failed : error code = %d", VT_rv);
        }

        /* Register it    */
        if (sigaction(SIGALRM,&sa, NULL))
        {
                tst_resm(TFAIL, "Cannot ignore alarm's signal: %s", strerror(errno));
        }
//        tst_exit();
}

/*================================================================================================*/
/*===== main =====*/
/**
@brief  Entry point to this test-case. It parses all the command line
        inputs, calls the global setup and executes the test. It logs
        the test status and results appropriately using the LTP API's
        On successful completion or premature failure, cleanup() func
        is called and test exits with an appropriate return code.

@param  Input : argc - number of command line parameters.
        Output: **argv - pointer to the array of the command line parameters.
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
        char   *err_msg;
        int     test_id = PACKET_DATA_EXCHANGE_1;
        int     pkt_len = DEF_PKT_LEN;
        int     log_len = DEF_LOG_LEN;
        int     iter_nr = NR_ITER;
        int     nonblk = FALSE;
        int     quantity_seg = 2;
        char   *p;

        /* parse options. */
        if ((err_msg = parse_opts(argc, argv, options, &help)) != (char *) NULL)
                tst_brkm(TBROK, cleanup, "OPTION PARSING ERROR - %s", err_msg);

        /* Argument handle */
        if (id_flag)
        {
                test_id = atoi(id_opt);
                if (test_id < PACKET_DATA_EXCHANGE_1 || test_id > MSG_DATA_LOG_TEST)
                {
                        help();
                        tst_brkm(TBROK, cleanup, "Invalid argument for -N: %s", id_opt);
                }
        }
        if (dev_flag)
        {
                if (strlen(dev_opt) > 2)        /* Indicate absulute path */
                {
                        for (p = dev_opt + strlen(dev_opt); *p != '/'; p--)
                        {
                        }
                        p++;
                        dev_path = dev_opt;
                }
                else        /* Put only number of channel */
                {
                        p = dev_opt;
                        dev_path = DEVICE_PATH;
                }
                if (!strcmp(p, MXC_IPC_DEV))        /* Module device parameter for -D */
                {
                        strcpy(dev_fname, "/dev/"MXC_IPC_DEV);
                }
                else
                {
                        dev_num = atoi(p);
                        *p = 0;
                        sprintf(dev_fname, "%s%d", dev_path, dev_num);
                        if (dev_num < IPC_CHANNEL0 || dev_num > IPC_CHANNEL5)
                        {
                                help();
                                tst_brkm(TBROK, cleanup,
                                         "Right values for -D (device number) from %d to %d)",
                                         IPC_CHANNEL0, IPC_CHANNEL5);
                        }
                }
        }
        else        /* Nothing parameters for -D (device), then using module device */
        {
                strcpy(dev_fname, "/dev/"MXC_IPC_DEV);
        }
        if (pktsz_flag)
        {
                if ((pkt_len = atoi(pktsz_opt)) <= MIN_BUF_ITER)
                {
                        help();
                        tst_brkm(TBROK, cleanup, "Invalid packet data size (must be greater than %d)",
                                 MIN_BUF_ITER);
                }
        }
        if (logsz_flag)
        {
                if ((log_len = atoi(logsz_opt)) <= MIN_BUF_ITER)
                {
                        help();
                        tst_brkm(TBROK, cleanup, "Invalid log buffer size (must be greater than %d)",
                                 MIN_BUF_ITER);
                }
        }
        if (iter_flag)
        {
                if ((iter_nr = atoi(iter_opt)) <= MIN_BUF_ITER)
                {
                        help();
                        tst_brkm(TBROK, cleanup,
                                 "Invalid number of iterations (must be greater than %d)",
                                 MIN_BUF_ITER);
                }
        }
        if (qtty_flag)
        {
                if ((quantity_seg = atoi(qtty_opt)) < 1)
                {
                        help();
                        tst_brkm(TBROK, cleanup, "Invalid quantity segments");
                }
        }
        if (nonblk_flag)
                nonblk = TRUE;

        alarm(1);
        /* perform global test setup, call setup() function. */
        setup();

        /* Print test Assertion using tst_resm() function with argument TINFO. */
        tst_resm(TINFO, "Testing if %s test case is OK", TCID);

        alarm(10);
        sig_count=1;
        /* VTE : Execute test, print results and exit test scenario */
        VT_rv = VT_ipc_test(test_id, pkt_len, log_len, iter_nr, nonblk, quantity_seg);

        /* VTE : print results and exit test scenario */
        if (VT_rv == TPASS)
                tst_resm(TPASS, "This test case worked as expected");
        else
                tst_resm(TFAIL, "This test case did NOT work as expected");

        cleanup();

        return VT_rv;
}

/*================================================================================================*/
/*===== timer_handler=====*/
/**  
@brief This is a timer handler.  
  
@param sig - signal number   
    
@return None  
*/
/*================================================================================================*/
void sig_hand(int sig)
{
        if (sig_count)
        {
                tst_resm(TWARN,"Test hangs up. Time out expired. Exiting after 10 sec waiting...");
                cleanup();
                tst_resm(TFAIL, "%s test did NOT work as expected", TCID);
                exit(-1);
        }
        return;
}
