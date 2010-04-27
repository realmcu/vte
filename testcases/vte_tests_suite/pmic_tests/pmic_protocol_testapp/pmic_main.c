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
        @file   pmic_main.c

        @brief  Main file of SC55112 Protocol driver test application.
====================================================================================================
Revision History:
                            Modification     Tracking
Author/core ID                  Date          Number    Description of Changes
-------------------------   ------------    ----------  -------------------------------------------
D.Khoroshev/b00313           07/20/2005     TLSbo52700   Initial version
D.Khoroshev/b00313           09/06/2005     TLSbo52700   Rework version
D.Khoroshev/b00313           12/06/2005     TLSbo58274   Removed test module
D.Khoroshev/b00313           02/15/2006     TLSbo59968   Returned test module for MC13783 support
D.Khoroshev/b00313           06/26/2006     TLSbo64239   Removed timeout for S_IT_U test
D.Khoroshev/b00313           07/25/2006     TLSbo64239   Added mc13783 legacy API support

====================================================================================================
Portability: ARM GCC
==================================================================================================*/

/*==================================================================================================
Total Tests: SC55112 Protocol

Test Name:   Read/Write test. Subscribe/Unubscribe event test. Sensors check test.

Test Assertion
& Strategy:  This test is used to test the SC55112 protocol driver.
==================================================================================================*/


/*==================================================================================================
                                        INCLUDE FILES
==================================================================================================*/
/* Standard Include Files */
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <pthread.h>

/* Verification Test Environment Include Files */
#include <test.h>
#include <usctest.h>

/* Harness Specific Include Files. */
#include "pmic_test_common.h"
#include "pmic_test.h"

/*==================================================================================================
                                        LOCAL MACROS
==================================================================================================*/
#define STD_COPIES 1

/*==================================================================================================
                          LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
==================================================================================================*/

/*==================================================================================================
                                       LOCAL CONSTANTS
==================================================================================================*/

/*==================================================================================================
                                       LOCAL VARIABLES
==================================================================================================*/
sig_atomic_t sig_count;
static struct sigaction sa, oldsa;
static char *ch_test_case=NULL;

/*==================================================================================================
                                       GLOBAL CONSTANTS
==================================================================================================*/


/*==================================================================================================
                                       GLOBAL VARIABLES
==================================================================================================*/
/* Global Variables */
char *TCID      = "pmic_protocol_testapp";    /* test program identifier.          */
int   TST_TOTAL = 6;                          /* total number of tests in this file.   */

int        threads_num = STD_COPIES;
int        *thr_result = NULL;
pthread_t  *thread = NULL;
test_param *params = NULL;
char       device_name[32];
char       *ifile_name = NULL;

/*==================================================================================================
                                   GLOBAL FUNCTION PROTOTYPES
==================================================================================================*/
void cleanup(void);
void setup(void);
int  main(int argc, char **argv);
void help(void);
void sig_hand(int sig);

/*==================================================================================================
                                   LOCAL FUNCTION PROTOTYPES
==================================================================================================*/


/*==================================================================================================
                                       GLOBAL FUNCTIONS
==================================================================================================*/

/*================================================================================================*/
/*===== cleanup =====*/
/**
@brief  Performs all one time clean up for this test on successful
        completion,  premature exit or  failure. Frees memory allocated for
        thread descriptors, thread results and params arrays. Exits the test
        with appropriate return code by calling tst_exit() function.

@param  Input :      None.
        Output:      None.

@return Nothing
*/
/*================================================================================================*/
void cleanup(void)
{
        int VT_rv = TFAIL;

        if (thr_result)
                free(thr_result);

        if (thread)
                free(thread);

        if (params)
                free(params);

        VT_rv = VT_pmic_cleanup();

        if (VT_rv != TPASS)
        {
                tst_resm(TWARN, "VT_cleanup() Failed : epmicor code = %d", VT_rv);
        }

        if (sigaction(SIGALRM, &oldsa, NULL))      /* Register it */
        {
                tst_resm(TFAIL, "Cannot install old signal handler: %s",
                strerror(errno));
        }

        tst_exit();
}

/*================================================================================================*/
/*===== setup =====*/
/**
@brief  Performs all one time setup for this test.
        This function is used to allocate memory for arrays thr_result, thread and params.

@param  Input :      None.
        Output:      None.

@return On failure - Exits by calling cleanup().
        On success - returns 0.
*/
/*================================================================================================*/
void setup(void)
{
        int VT_rv = TFAIL;

        if ((thr_result = malloc(sizeof(opt_params)*threads_num)) == NULL)
        {
                tst_brkm(TBROK , cleanup, "VT_setup() Failed : error code = %d", VT_rv);
        }
        if ((thread = malloc(sizeof(pthread_t)*threads_num)) == NULL)
        {
                tst_brkm(TBROK , cleanup, "VT_setup() Failed : error code = %d", VT_rv);
        }

        if ((params = malloc(sizeof(test_param)*threads_num)) == NULL)
        {
                tst_brkm(TBROK , cleanup, "VT_setup() Failed : error code = %d", VT_rv);
        }

        VT_rv = VT_pmic_setup();

        if (VT_rv != TPASS)
        {
                tst_brkm(TBROK , cleanup, "VT_setup() Failed : error code = %d", VT_rv);
        }

        sig_count = 0;                          /* Zero signal counter */
        memset(&sa, 0, sizeof(sa));
        sa.sa_handler = &sig_hand;              /* Assign a signal handler */

        if (sigaction(SIGALRM, &sa, &oldsa))    /* Register it */
        {
                tst_resm(TFAIL, "Cannot install signal handler: %s",
                strerror(errno));
        }
}

/*================================================================================================*/
/*===== main =====*/
/**
@brief Entry point to this test-case. It parses all the command line
        inputs, calls the global setup and executes the test. It logs
        the test status and results appropriately using the LTP API's
        On successful completion or premature failure, cleanup() func
        is called and test exits with an appropriate return code.

@param Input :      argc - number of command line parameters.
        Output:      **argv - pointer to the array of the command line parameters.
        Describe input arguments to this test-case
        -v - Print verbose output
        -N - Number of concurrent threads
        -R reg_number - Read from register reg_number
        -V <hexa value> used for write option
        -m <bitmask> bitmask used for writing register in hex form with prefix 0x(Optional)
        -W reg_number - Write to register reg_number
        -S event_number - Subscrube event event_number
        -U event_number - Unsubscrube event event_number
        -F Input data file name(Optional for some tests)
        -T  test_case - exec test case by prefix
           test cases prefixes:
                RW - Read/write test
                SU - Subscribe/unsubscribe test
                CA - Concurrent access test
                IP - Access with incorrect parametres test
                SC - Sensor check test
                RA - Random access test
                S_IT_U - Interrupt test

@return On failure - Exits by calling cleanup().
        On success - exits with 0 exit value.
*/
/*================================================================================================*/
int main(int argc, char **argv)
{
        int i;

        int VT_rv = TPASS;

        /* parse options. */
        int rflag=0, wflag=0, vflag=0, sflag=0, uflag=0, tflag=0, nflag = 0, mflag = 0, dflag = 0;

        /* binary flags: opt or not         */
        char *ch_reg_num=NULL, *ch_reg_value=NULL, *ch_event_num=NULL, *nb_threads=NULL, *ch_mask=NULL,
                *ch_dev=NULL;
        /* option arguments                 */
        unsigned int reg_num=0, reg_value=0, event_num=0, test_id=0, reg_mask=PMIC_ALL_BITS;
        /* option arguments                 */
        char *msg;

        memset(device_name, 0x0, 32);
        strncpy(device_name, PMIC_DEVICE, 31);
        device_name[31]='\0';

        /*        void *params;        */
        void* (*func)(void*) = NULL;

        option_t options[] =
        {
                { "R:", &rflag, &ch_reg_num    },        /* Read register opt                   */
                { "W:", &wflag, &ch_reg_num    },        /* Write register opt                  */
                { "V:", &vflag, &ch_reg_value  },        /* Value to write                      */
                { "S:", &sflag, &ch_event_num  },        /* Subscribe event opt                 */
                { "U:", &uflag, &ch_event_num  },        /* Unsubscribe event opt               */
                { "T:", &tflag, &ch_test_case  },        /* Execute test case                   */
                { "v",  &verbose_flag, NULL    },        /* Verbose flag                        */
                { "N:", &nflag, &nb_threads    },        /* Number of threads flag              */
                { "D:", &dflag, &ch_dev        },        /* Device name                         */
                { "m:",  &mflag, &ch_mask      },        /* Bitmask used for writing register in
                                                            hex form with prefix 0x.            */
                { NULL, NULL, NULL }                     /* NULL required to end array          */
        };

                /*{ "F:", &fflag, &ifile_name    },   */     /* Input file with data for some tests */
        if ((msg = parse_opts(argc, argv, options, &help)) != NULL)
        {
                tst_brkm(TFAIL, cleanup, "%s test application did NOT work as expected: %s", TCID, msg);
        }

        if (nflag)
        {
                threads_num = atoi(nb_threads);

        }
        else
        {
                threads_num = STD_COPIES;
        }

        if (rflag || wflag)
        {
                reg_num = atoi(ch_reg_num);
        }
        if (sflag || uflag)
        {
                event_num = atoi(ch_event_num);
        }
        if (vflag)
        {
                sscanf(ch_reg_value, "%x",&reg_value);
        }
        if (mflag)
        {
                sscanf(ch_mask, "%x",&reg_mask);
        }
        if (dflag)
        {
                strncpy(device_name, ch_dev, 31);
                device_name[31]='\0';
        }

        /* perform global test setup, call setup() function. */
        setup();

        /* Test Case Body. */

        if (tflag)
        {
                if (!strcmp((char*)ch_test_case, TEST_CASE_RW)) test_id = PMIC_PROTOCOL_RW;
                else if (!strcmp((char*)ch_test_case,TEST_CASE_SU)) test_id = PMIC_PROTOCOL_SU;
                else if (!strcmp((char*)ch_test_case,TEST_CASE_CA)) test_id = PMIC_PROTOCOL_CA;
                else if (!strcmp((char*)ch_test_case,TEST_CASE_IP)) test_id = PMIC_PROTOCOL_IP;
                else if (!strcmp((char*)ch_test_case,TEST_CASE_S_IT_U)) test_id = PMIC_PROTOCOL_S_IT_U;
                else
                {
                        help();
                        cleanup();
                }

                func = &VT_pmic_exec_test_case;
                for(i=0; i<threads_num; i++) {
                        params[i].test_id = test_id;
                        params[i].thread_num = i;
                }
        }
        else
        {
                        help();
                        cleanup();
                        /*
                if (rflag)
                {
                        optparams.operation = PMIC_READ_REG_T;
                        optparams.val1=reg_num;
                        optparams.val2=(unsigned int)&reg_value;
                        ch_test_case = strdup("R");
                }
                else if (wflag)
                {
                        optparams.operation = PMIC_WRITE_REG_T;
                        optparams.val1=reg_num;
                        optparams.val2=(unsigned int)reg_value;
                        optparams.mask=reg_mask;
                        ch_test_case = strdup("W");
                }
                else if (sflag)
                {
                        event.event=event_num;
                        optparams.operation = PMIC_SUBSCRIBE_T;
                        optparams.val1=2;
                        optparams.val2=(unsigned int)event_num;
                        ch_test_case = strdup("S");
                }
                else if (uflag)
                {
                        event.event=event_num;
                        optparams.operation = PMIC_UNSUBSCRIBE_T;
                        optparams.val1=2;
                        optparams.val2=(unsigned int)event_num;
                        ch_test_case = strdup("U");
                }
                else
                {
                        help();
                        tst_exit();
                }

                func = &VT_pmic_exec_opt;
                memcpy(params, (void*)(&optparams), sizeof optparams);*/
        }

        tst_resm(TINFO,"Testing if %s testcase is Ok.",ch_test_case);
        fflush(stdout);

        if(test_id != PMIC_PROTOCOL_S_IT_U)
        {
                alarm(4*threads_num<10 ? 4*threads_num : 10);
        }
        else
        {
                alarm(60);
        }
        sig_count=1;
        for (i = 0; i < threads_num; i++)
        {
                thr_result[i] = pthread_create(&(thread[i]), NULL, func, (void*)&params[i]);

                if (thr_result[i] != 0)
                {
                        if (verbose_flag)
                        {
                                tst_resm(TFAIL, "Thread %d starting failed", i);
                        }
                        VT_rv = TFAIL;
                }
                else
                {
                        if (verbose_flag)
                        {
                                tst_resm(TINFO, "Thread %d started", i);
                        }
                }
        }

        for (i = 0; i < threads_num; i++)
        {
                if (thr_result[i] == 0)
                {
                        pthread_join(thread[i], (void*)&(thr_result[i]));

                        if (verbose_flag) {
                                if (thr_result[i] == 0)
                                {
                                        tst_resm(TINFO, "Thread %d finished: OK", i);
                                }
                                else
                                {
                                        tst_resm(TFAIL, "Thread %d finished: FAILED", i);
                                }
                        }
                }
        }

        for (i = 0; i < threads_num; i++)
        {
                if(verbose_flag) {
                        tst_resm(TINFO,"\t+ thread %d finished with return code %d", i, thr_result[i]);
                }
                if (thr_result[i] != 0)
                {
                        VT_rv = thr_result[i];
                }
        }

        sig_count=0;

        /* Print test Assertion using tst_resm() */
        if (VT_rv == TPASS)
        {
                tst_resm(TPASS, "%s %s test worked as expected", TCID, ch_test_case);
        }
        else
        {
                tst_resm(TFAIL, "%s %s test did NOT work as expected", TCID, ch_test_case);
        }

        cleanup();
        return VT_rv;
}

/*================================================================================================*/
/*===== help =====*/
/**
@brief  Print information about test execution

@param  Input :      None.
        Output:      None.

@return Nothing
*/
/*================================================================================================*/
void help(void)
{
        printf("SC55112 Protocol driver test application option\n");
        printf("  -R <Reg>  Read a register\n");
        printf("  -W <Reg>  write a register\n");
        printf("  -V <hexa value> used for write option\n");
        printf("  -m <bitmask> bitmask used for writing register in hex form with prefix 0x(Optional).\n");
        printf("  -S <event_number> Subscribe event notification\n");
        printf("  -U <event_number> Unsubscribe event notification\n");
        printf("  -D <device_name> Name of device\n");
        printf("  -N <number of threads> Number of concurrent threads\n");
        printf("  -F Input data file name(Optional for some tests)\n");
        printf("  -T  test_case - exec test case by prefix\n");
        printf("  -v display additional information\n");
        printf("\tWith event_number value is:\n");
        printf(" 0 : EVENT_ADCDONEI completion of the 7 programmed A/D conversions in standard operation\n");
        printf(" 1 : EVENT_TSI touchscreen press\n");
        printf(" 2 : EVENT_1HZI interrupt is from the 1Hz output\n");
        printf(" 3 : EVENT_WHI A/D word read in ADC digital comparison mode exceeding the WHIGH[5:0] word.\n");
        printf(" 4 : EVENT_WLI A/D word read in ADC digital comparison mode reading below the WLOW[5:0] word.\n");
        printf(" 5 : EVENT_TODAI RTC_TOD = RTC_TODA; RTC_DAY = RTC_DAYA\n");
        printf(" 6 : EVENT_USB_44VI  occurs on rising and falling debounced edges of USBDET_4.4V.\n");
        printf(" 7 : EVENT_ONOFFI  ON/OFF button was pressed.\n");
        printf(" 8 : EVENT_ONOFF2I  ON/OFF2 button was pressed.\n");
        printf(" 9 : EVENT_USB_08VI  interrupt occurs on rising and falling edges of USBDET_0.8V.\n");
        printf(" 10: EVENT_MOBPORTI  interrupt occurs on rising and falling debounced edges of MOBPORTB (EXT B+).\n");
        printf(" 11: EVENT_PTTI  Interrupt linked to PTT_DET , to be debounced on both edges.\n");
        printf(" 12: EVENT_A1I  Triggered on debounced transition of A1_INT\n");
        printf(" 13: EVENT_PCI  Power Cut transition occurred when PCEN=1 and B+ was re-applied before\n"
                "\tthe Power Cut timer expired\n");
        printf(" 14: EVENT_WARMI  warm start to the MCU\n");
        printf(" 15: EVENT_EOLI  End of Life (low battery shut off)\n");
        printf(" 16: EVENT_CLKI  positive or negative edge of CLK_STAT\n");
        printf(" 17: EVENT_USB_20VI  interrupt occurs on rising and falling debounced edges of USBDET_2.0V.\n");
        printf(" 18: EVENT_AB_DETI  Interrupt generated from the de-bounced output of the USB ID-detect comparator.\n");
        printf(" 19: EVENT_ADCDONE2I  completion of the 7 programmed A/D conversions in standard operation.\n");
        printf(" 20: EVENT_SOFT_RESETI  Will be set (1) only if SYS_RST_MODE bits = 10\n"
                "\tand (2) the BATT_DET_IN/SYS_RESTART input was asserted for the minimum debounce time.\n");
        printf(" 21: EVENT_NB  Number of events.\n\n");
        printf("  -T test case 'RW', 'SU', 'CA', 'IP','S_IT_U'\n");
        printf("Examples :\n");
        printf("\t%s -W 18 -V 05F4DA\n",TCID);
        printf("\t%s -v -R 18\n",TCID);
        printf("\t%s -S 20\n",TCID);
        printf("\t%s -U 20\n",TCID);
        printf("\t%s -T SC\n",TCID);
}

/*================================================================================================*/
/*===== sig_hand =====*/
/**
@brief  Handler callback function

@param  Number of signal

@return Nothing
*/
/*================================================================================================*/
void sig_hand(int sig)
{
        if(sig_count)
        {
                tst_resm(TWARN,"Test hangs up. Time out expired. Exiting...");
                tst_resm(TFAIL, "%s %s test did NOT work as expected", TCID, ch_test_case);
                cleanup();
                exit(-1);
        }

        return;
}
