/*===============================================================================================*/
/** 
    @file   uart3_main.c 

    @brief  First UART test main function. */
/*================================================================================================= 

    Copyright (C) 2007, Freescale Semiconductor, Inc. All Rights Reserved 
    THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT 
    BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF 
    Freescale Semiconductor, Inc. 

====================================================================================================
Revision History:
                            Modification     Tracking
Author                          Date          Number    Description of Changes
-------------------------   ------------    ----------  -------------------------------------------
C.GAGNERAUD/cgag1c           01/06/2005    TLSbo45060    Rework and improve UART test application
I.Inkina/nknl001             22/08/2005    TLSbo52626    Update Uart test 
E.Gromazina                  14/09/2005                         Min fix
D.Kardakov                   04/06/2007    ENGR30041     New transfer data test was added
D.Kardakov                   05/02/2007    ENGR30041     New options -J,  -Q,  -D were added.
====================================================================================================
Portability:  ARM GCC  gnu compiler
==================================================================================================*/

/*==================================================================================================
Total Tests:           1
Test Executable Name:  uart_testapp_3
Test Strategy:         
=================================================================================================*/


#ifdef __cplusplus
extern "C"{ 
#endif

/*==================================================================================================
                                        INCLUDE FILES
==================================================================================================*/
/* Standard Include Files */
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>

/* Harness Specific Include Files. */
#include "test.h"
#include "usctest.h"

/* Verification Test Environment Include Files */
#include "uart_test_3.h"
#include "uart_test_addon.h"
/*==================================================================================================
    LOCAL MACROS
    ==================================================================================================*/


/*==================================================================================================
    LOCAL VARIABLES
    ==================================================================================================*/

/* 
* Options flags and arguments
*/
int     uart_speed_flag = 0;
char   *uart_speed_arg = NULL;
int     uart_parity_flag = 0;
char   *uart_parity_arg = NULL;
int     uart_char_length_flag = 0;
char   *uart_char_length_arg = NULL;
int     uart_stop_bits_flag = 0;
char   *uart_stop_bits_arg = NULL;

/* 11 int uart_flow_ctrl_flag = 0; char *uart_flow_ctrl_arg = NULL; */
int     dev_source_flag = 0;
char   *dev_source_arg = NULL;
int     dev_dest_flag = 0;
char   *dev_dest_arg = NULL;
int     xfer_length_flag = 0;
char   *xfer_length_arg = NULL;
int     testcase_flag = 0;
char   *testcase_arg = NULL;
int     filename_flag = 0;
char   *filename_arg = NULL;
int     receive_to_ram_flag = 0;
int     save_to_file_flag = 0;
int     thread_sleep_flag = 0;
char   *thread_sleep_arg = NULL;

int     host_to_host_test_flag = 0;
int     exchange_direction = 0; // 0  - receiving, 1 - transmission
int     thread_sleep_time = 0;
char  * transmit_filename = NULL;
/*==================================================================================================
                            LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
==================================================================================================*/
/* 
*Command line options descriptor
*/
#define OPT_DESC(fmt, var) {fmt, &var##_flag, &var##_arg}
option_t UART_options[] = {
        OPT_DESC("B:", uart_speed),
        OPT_DESC("R:", uart_parity),
        OPT_DESC("C:", uart_char_length),
        OPT_DESC("S:", uart_stop_bits),
        OPT_DESC("s:", dev_source),
        OPT_DESC("d:", dev_dest),
        OPT_DESC("F:", filename),
        OPT_DESC("T:", testcase),
        OPT_DESC("Q:", thread_sleep),
        {"J", &receive_to_ram_flag, NULL},
        {"D", &save_to_file_flag, NULL},
        {NULL, NULL, NULL}
};

typedef struct
{
        char   *string;
        char   *desc;
        int     value;
} testcase_choice_t;

/*==================================================================================================
                                        LOCAL CONSTANTS
==================================================================================================*/


/*==================================================================================================
                                        GLOBAL CONSTANTS
==================================================================================================*/


/*==================================================================================================
                                        GLOBAL VARIABLES
==================================================================================================*/
/* Extern Global Variables */
extern int Tst_count;   /* counter for tst_xxx routines.  */
extern char *TESTDIR;   /* temporary dir created by tst_tmpdir(viod) */

/* Global Variables */
char   *TCID = "uart_testapp_3";        /* test program identifier.  */
int     TST_TOTAL = 1;  /* total number of tests in this file.  */



/*==================================================================================================
                                    GLOBAL FUNCTION PROTOTYPES
==================================================================================================*/
void    cleanup(void);
void    setup(void);
int     main(int argc, char **argv);

/*==================================================================================================
                                    LOCAL FUNCTION PROTOTYPES
==================================================================================================*/
static parity_type_t str2parity(char *str);

/* static flow_ctrl_t str2flowctrl(char *str); */
static int str2testcase(char *str);

/* */
static uart_config_t uart_src,
        uart_dst;
static int transfert_length;

static testcase_choice_t testcase_choices[] = {
        {"none", "Do a transfert without flow control", TESTCASE_SIMPLE},
        {"hard", "Do a transfert with HW flow control (RTS/CTS)", TESTCASE_HW_FLOW},
        {"soft", "Do a transfert with SW flow control (XON/XOFF)", TESTCASE_SW_FLOW}
};
static int testcase = -1;

/*==================================================================================================
                                        GLOBAL FUNCTIONS
==================================================================================================*/

/*================================================================================================*/
/*===== cleanup =====*/
/**
@brief  Performs all one time clean up for this test on successful
completion,  premature exit or  failure. Closes all temporary
files, removes all temporary directories exits the test with
appropriate return code by calling tst_exit(void) function.cleanup

@param  Input :      None.
Output:      None.
@return Nothing*/
/*================================================================================================*/
void cleanup(void)
{
        /* VTE : Actions needed to get a stable target environment */
        int     VT_rv = TFAIL;
        
        if (host_to_host_test_flag == 0)
                VT_rv = VT_uart_test3_cleanup();
        else 
                VT_rv = VT_uart_addon_cleanup();
        if (VT_rv != TPASS)
        {
                tst_resm(TWARN, "VT_uart_test3_cleanup() Failed : error code = %d", VT_rv);
        }
        /* Exit with appropriate return code. */
        tst_exit();
}

/*==================================================================================================
                                        LOCAL FUNCTIONS
==================================================================================================*/

/*================================================================================================*/
/*===== help =====*/
/**
@brief  Inform of the available options and the associated parameters

@param  Input :      None.
        Output:      None.
    
@return On failure - Exits by calling cleanup().
        On success - returns 0.*/
/*================================================================================================*/
void help(void)
{
        int     i;

        printf("\n"
               "Usage:\nuart_testapp_3 <OPTIONS> -T <TESTCASE>\n"
               "\n"
               "OPTIONS:\n"
               "  -B <Speed>           : Set UART baud rate to <Speed> bps\n"
               "  -R <paRity>          : Set UART parity to <paRity>, could be \"O\" \n"
               "                         \"E\" else none\n"
               "  -S <nb_Stop_bits>    : Set UART number of stop bits to <nb_stop_Bits>, could \n"
               "                         be \"1\" or \"2\"\n"
               "  -C <Char_length>    :  Set UART charater length to <Char_length>, could"
               "                         be \"7\" or \"8\"\n"
               "  -T <floW_ctrl_type>  : Set UART flow control type to <floW_ctrl_type>, could \n"
               "                         be \"hard\", \"soft\" or \"none\"\n"
               "  -s <Src_device>      : Use <Src_device> as UART to read from\n"
               "  -d <Dest_device>     : Use <Dest_device> as UART to write to\n" "\n"
               "  -F <filename>        : Set the file for transmission to another host \n"
               "  -J                   : Allows receiving file in RAM without file dumping, "
               "                       : Enables the compare of transmitted and received file\n"
               "  -D                   : Allows to save received file (Use with -J opt.)"
               "  -Q                   : Set the R/W threads sleep time");
        printf("Testcase:\n");
        for (i = 0; i < sizeof(testcase_choices) / sizeof(testcase_choice_t); i++)
        {
                printf("  %s: %s\n", testcase_choices[i].string, testcase_choices[i].desc);
        }
}

/*================================================================================================*/
/*===== setup =====*/
/**
@brief  Performs all one time setup for this test. This function is
typically used to capture signals, create temporary dirs
and temporary files that may be used in the course of this test.

@param  Input :      UART_type
Sending_UART
Receiving_UART
Output:      None.
    
@return On failure - Exits by calling tst_exit().
On success - returns 0.*/
/*================================================================================================*/
void setup(void)
{
        int     VT_rv = TFAIL;
        uart_config_t * uart;
        /* VTE : Actions needed to prepare the test running */
        if (host_to_host_test_flag == 0)
                VT_rv = VT_uart_test3_setup(&uart_src, &uart_dst, transfert_length, 
                                             uart_src.flow_ctrl, atoi(uart_speed_arg));
        else {
                uart = &uart_src;
                transmit_filename = filename_arg;
                VT_rv = VT_uart_addon_setup(uart, uart->flow_ctrl, atoi(uart_speed_arg));
        }
        /* If failure, setup() has done the corresponding cleanup job */
        if (VT_rv != TPASS)
        {
                tst_brkm(TBROK, NULL, "setup() Failed : error code = %d", VT_rv);
                tst_exit();
        }
        return;
}

/*================================================================================================*/
/*===== main =====*/
/**
@brief  Entry point to this test-case. It parses all the command line
inputs, calls the global setup and executes the test. It logs
the test status and results appropriately using the LTP API's
on successful completion or premature failure, cleanup(void) func
is called and test exits with an appropriate return code.

@param  Input :      argc - number of command line parameters.
Output:      **argv - pointer to the array of the command line parameters.
Describe input arguments to this test-case
-l - Number of iteration
-v - Prints verbose output
-V - Prints the version number
    
@return On failure - Exits by calling cleanup().
On success - exits with 0 exit value.*/
/*================================================================================================*/

#define check_option(opt, str)                                                          \
{                                                                                       \
        if (!opt##_flag || !opt##_arg)                                                        \
{                                                                                   \
        tst_brkm(TBROK, NULL, "Error while parsing command line options: %s", str);    \
        return TFAIL;                                                                     \
}                                                                                   \
}

int main(int argc, char **argv)
{
        int     VT_rv = TFAIL;
        char   *msg;

        uart_parity_arg = "N";

        /* Parse user defined options */
        msg = parse_opts(argc, argv, UART_options, help);
        /* non option argument is testcase */
        if (msg != (char *) NULL)
        {
                tst_brkm(TBROK, NULL, "Error when parsing arguments.");
                return TBROK;
        }

        /* Verify all mandatory option flag/arg */
        check_option(uart_speed, "Missing UART speed");
        check_option(uart_char_length, "Missing UART char length");
        check_option(uart_stop_bits, "Missing UART nb stop bits");
        check_option(testcase, "Missing testcase");
        if ((!dev_source_flag || !dev_source_arg) && (!dev_dest_flag || !dev_dest_arg)) {
                tst_brkm(TBROK, NULL, "Error while parsing command line options: %s", "Missing UART devices");
                return TFAIL;
        } 
        
        if (dev_source_flag && !dev_source_arg) {
                tst_brkm(TBROK, NULL, "Error while parsing command line options: %s", 
                                      "Missing UART devices source argument");
                return TFAIL;
        }
        
        if (dev_dest_flag && !dev_dest_arg) {
                tst_brkm(TBROK, NULL, "Error while parsing command line options: %s", 
                                      "Missing UART devices destination argument");
                return TFAIL;
        }
        
        if (!dev_source_flag || !dev_source_arg) {
                host_to_host_test_flag = 1;
                exchange_direction = 0; //receiving 
                uart_src.device = dev_dest_arg;
                if (thread_sleep_flag)
                        thread_sleep_time = atoi(thread_sleep_arg);

                if (thread_sleep_time < 0 || thread_sleep_time > 15) {
                        tst_resm(TBROK, "Wrong thread sleep time, try -P 0 up to 15", TCID);
                        return TFAIL;
                }

        } else if (!dev_dest_flag || !dev_dest_arg) {
                check_option(testcase, "Missing filename");
                host_to_host_test_flag = 1;
                exchange_direction = 1; //transmission
                uart_src.device = dev_source_arg;
                if (thread_sleep_flag)
                        thread_sleep_time = atoi(thread_sleep_arg);

                if (thread_sleep_time < 0 || thread_sleep_time > 15) {
                        tst_resm(TBROK, "Wrong thread sleep time, try -P 0 up to 15", TCID);
                        return TFAIL;
                }
        } else {
                host_to_host_test_flag = 0;
                exchange_direction = 0;
                uart_src.device = dev_source_arg;
                uart_dst.device = dev_dest_arg;
        }

        //check_option(dev_source, "Missing UART device source");
        //check_option(dev_dest, "Missing UART device destination");

        /* Init uart test config structs acording to option args */
        //uart_src.device = dev_source_arg;
        //uart_dst.device = dev_dest_arg;
        uart_src.baud_rate = atoi(uart_speed_arg);
        uart_dst.baud_rate = atoi(uart_speed_arg);
        uart_src.parity_type = str2parity(uart_parity_arg);
        uart_dst.parity_type = str2parity(uart_parity_arg);
        uart_src.char_length = atoi(uart_char_length_arg);
        uart_dst.char_length = atoi(uart_char_length_arg);
        uart_src.stop_bits = atoi(uart_stop_bits_arg);
        uart_dst.stop_bits = atoi(uart_stop_bits_arg);

        /* buffers are 4KB, transfert must be > 2 buffers length */
        transfert_length = MIN_TRANSFERT_LENGTH;
        testcase = str2testcase(testcase_arg);
        if (testcase < 0)
        {
                tst_brkm(TBROK, NULL, "Unknow testcase \"%s\"", testcase_arg);
                return TBROK;
        }


        switch (testcase)
        {
        case TESTCASE_HW_FLOW:
                uart_src.flow_ctrl = FLOW_CTRL_HARD;
                uart_dst.flow_ctrl = FLOW_CTRL_HARD;

                break;
        case TESTCASE_SW_FLOW:
                uart_src.flow_ctrl = FLOW_CTRL_SOFT;
                uart_dst.flow_ctrl = FLOW_CTRL_SOFT;

                break;

        default:
                uart_src.flow_ctrl = FLOW_CTRL_NONE;
                uart_dst.flow_ctrl = FLOW_CTRL_NONE;
                break;
        }

        /* perform global test setup, call setup() function. */
        setup();

        /* Print test Assertion using tst_resm() function with argument TINFO. */
        tst_resm(TINFO, "Testing if %s test case is OK", TCID);

        /* VTE : Execute test, print results and exit test scenario */
        if (host_to_host_test_flag == 0)
                VT_rv = VT_uart_test3(testcase);
        else
                VT_rv = VT_uart_addon_test(testcase);

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

        pthread_exit(&VT_rv);
}


static parity_type_t str2parity(char *str)
{
        if (strcmp("O", str) == 0)
                return PARITY_ODD;
        else if (strcmp("E", str) == 0)
                return PARITY_EVEN;
        else
                return PARITY_NONE;
}

static int str2testcase(char *str)
{
        int     i;

        for (i = 0; i < sizeof(testcase_choices) / sizeof(testcase_choice_t); i++)
        {
                if (strcmp(testcase_choices[i].string, str) == 0)
                        return testcase_choices[i].value;
        }
        return -1;
}

#ifdef __cplusplus
} 
#endif
