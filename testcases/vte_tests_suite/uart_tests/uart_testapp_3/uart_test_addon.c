/***
**Copyright (C) 2007-2009 Freescale Semiconductor, Inc. All Rights Reserved.
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
    @file   uart_test_addon.c 

    @brief Configure the serial interface using MXC UART and/or External UART low-level driver 
                Read / Write test 

====================================================================================================
Revision History:
                            Modification     Tracking
Author                          Date          Number    Description of Changes
-------------------------   ------------    ----------  -------------------------------------------
D.Kardakov                   04/06/2007    ENGR30041    Initial version
D.Kardakov                   05/02/2007    ENGR30041    Delay of the reading thread  was fixed.
====================================================================================================
Portability:  ARM GCC  gnu compiler
==================================================================================================*/


#ifdef __cplusplus
extern "C"{ 
#endif

/*==================================================================================================
                                        INCLUDE FILES
==================================================================================================*/
/* Standard Include Files */
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <errno.h>
#include <pthread.h>
#include <math.h>
#include <signal.h>
#include <linux/types.h>
#include <linux/serial.h>
#include <sys/time.h>   // timer usage for reading

/* Harness Specific Include Files. */
#include "test.h"

/* Verification Test Environment Include Files */
#include "uart_test_3.h"
#include "uart_test_addon.h"


/* 
* Global variable 
*/
//pthread_t thid_writer,
//        thid_reader,
//        thid_signal;

/* 
* Local typedef, macros, ...
*/
#define THR_INFO(fmt, args...) tst_resm(TINFO, "  [R   ] " fmt, ##args)
#define THR_WARN(fmt, args...) tst_resm(TWARN, "  [R   ] " fmt, ##args)
#define THW_INFO(fmt, args...) tst_resm(TINFO, "  [ W  ] " fmt, ##args)
#define THC_INFO(fmt, args...) tst_resm(TINFO, "  [  C ] " fmt, ##args)
#define THS_INFO(fmt, args...) tst_resm(TINFO, "  [   S] " fmt, ##args)
#define THR_FAIL(fmt, args...) tst_resm(TFAIL, "  [R   ] " fmt, ##args)
#define THW_FAIL(fmt, args...) tst_resm(TFAIL, "  [ W  ] " fmt, ##args)
#define THC_FAIL(fmt, args...) tst_resm(TFAIL, "  [  C ] " fmt, ##args)
#define THS_FAIL(fmt, args...) tst_resm(TFAIL, "  [   S] " fmt, ##args)

#define REC_BLOCK    4096
#define TRANS_BLOCK  1024
#define COMPARE_BUFFER 4096 
#define RW_SUCCESS  0xCC
#define NUM_SLEEP   4
/* 
* local variable 
*/
static int timer_running = 0; // 0 - is not running
static int cout_timer_sign = 0;
static int prev_cout_sign = 0;
static int all_bytes_transfered = 0;
static char start_message[] = "**Start header!";
static char stop_message [] = "**End header!";
static int sleep_counter = 1;

static pthread_mutex_t rw_mutex = PTHREAD_MUTEX_INITIALIZER;

extern int receive_to_ram_flag;
extern int save_to_file_flag;
extern int thread_sleep_flag;
extern int thread_sleep_time;
extern int thread_sleep_flag;
extern int exchange_direction; // 0  - receiving, 1 - transmission
extern int thread_sleep_time;

extern char *transmit_filename;
static char receive_filename[32];

pthread_t rw_thread,
        thid_signal;

/* Block all signals (except SIGINT) for da calling thread */
static void th_block_signals(void);

static int VT_check_hardware(int check_error, int check_break, int check_flow, int check_modem);
void      *VT_th_writer_addon(void *param);
void      *VT_th_reader_addon(void *param);
void      *VT_th_signal_addon(void *param);

/*==================================================================================================
                                        LOCAL MACROS
==================================================================================================*/


/*==================================================================================================
                            LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
==================================================================================================*/
typedef struct 
{
        char start_message [16];
        char transfer_fname [256];
        off_t file_size;
        char stop_message [16];
} transfer_header_t;

/*==================================================================================================
                                        LOCAL CONSTANTS
==================================================================================================*/
/*
* local function prototype
*/
//static int VT_check_hardware_addon(int check_error, int check_break, int check_flow, int check_modem);
static int CreateReceiveFname(char *Fname, char *ReceiveFname);
static int compare_data (char *Fname, int size);
static int alarm_handler(int stat);
static void run_timer(void);
static void timer_handler(int signum);
static void stop_timer (void);
static int check_timer(void);
static int check_header(transfer_header_t *h);
static int read_block_from_uart(int uart_fd, unsigned char *memPtr, int blockSize);
static int write_block_to_uart(int uart_fd, unsigned char *memPtr, int blockSize);
/*==================================================================================================
                                        LOCAL VARIABLES
==================================================================================================*/
  /** Source and destination UART to test */
static uart_test_t uart_test;
static flow_ctrl_t uart_flow_ctrl;
static int         uart_baud_rate;
//static int         uart_transfert_length;
int showmessage = 0;
int read_sleep = 0;

/*==================================================================================================
                                        GLOBAL CONSTANTS
==================================================================================================*/


/*==================================================================================================
                                        GLOBAL VARIABLES
==================================================================================================*/

/*==================================================================================================
                                    LOCAL FUNCTION PROTOTYPES
==================================================================================================*/

int     testcase_simple(void);
int     testcase_hw_flow(void);
int     testcase_sw_flow(void);


/*==================================================================================================
                                        LOCAL FUNCTIONS
==================================================================================================*/
void    timer_handler(int signum);

/*================================================================================================*/
/*===== VT_uart_addon_setup =====*/
/** 
* Assumes the test preconditions, open and configure source and destination UART.  Function  is used for data transfer test between two boards.
* 
* 
* @return */
/*================================================================================================*/

int VT_uart_addon_setup(uart_config_t * uart_cfg, flow_ctrl_t flow_ctrl, int baud_rate)
{
        int     rv = TFAIL;
        //int     bufidx;
        struct stat st;
        uart_flow_ctrl = flow_ctrl;
        uart_baud_rate = baud_rate;
        uart_test.flow_ctrl = uart_cfg->flow_ctrl;
        if (exchange_direction == 0)
                tst_resm(TINFO, "* Receiving file from %s port", uart_cfg->device);
        else 
                tst_resm(TINFO, "* Transmitting %s file to %s port", transmit_filename, uart_cfg->device);
        
        tst_resm(TINFO, "* %-20s: %d", "Speed", uart_baud_rate);
        tst_resm(TINFO, "* %-20s: %d", "Parity", uart_cfg->parity_type);
        tst_resm(TINFO, "* %-20s: %d", "Character length", uart_cfg->char_length);
        tst_resm(TINFO, "* %-20s: %d", "Number of stop bits", uart_cfg->stop_bits);
        tst_resm(TINFO, "* %-20s: %s", "Flow control",
                 uart_flow_ctrl == FLOW_CTRL_HARD ? "hard" :
                 uart_flow_ctrl == FLOW_CTRL_SOFT ? "soft" : "none");
        memset (&uart_test, 0, sizeof (uart_test_t));
        if (exchange_direction == 0) {
                tst_resm(TINFO, "Setting up destination UART...");
                if (VT_setup_uart(uart_cfg, &uart_test, "destination UART", 0) != TPASS)  // O_NONBLOCK
                        goto err_return;
                fcntl(uart_test.fd, F_SETFL, FNDELAY);
                tst_resm(TINFO, "Saving DST initial icounter...", uart_test.device);
                if (ioctl(uart_test.fd, TIOCGICOUNT, (unsigned long) &uart_test.icounter_saved) < 0)
                {
                        tst_resm(TWARN, "    => Failure.");
                        perror(uart_test.device);
                        goto err_restore;
                }

                if (!receive_to_ram_flag) {
                        uart_test.data = (unsigned char *) malloc(DATA_BUFFER_SIZE);
                        if (!uart_test.data) {
                                tst_brkm(TBROK, NULL, "Unable to allocate data buffer:");
                                perror("malloc()");
                                goto err_restore;
                        }
                }

        } else  {
                tst_resm(TINFO, "Setting up source UART...");
                if (VT_setup_uart(uart_cfg, &uart_test, "source UART", 0) != TPASS)  // O_NONBLOCK
                        goto err_return;
                fcntl(uart_test.fd, F_SETFL, FNDELAY);
                tst_resm(TINFO, "Saving SRC initial icounter...", uart_test.device);
                if (ioctl(uart_test.fd, TIOCGICOUNT, (unsigned long) &uart_test.icounter_saved) < 0) {
                        tst_resm(TWARN, "    => Failure.");
                        perror(uart_test.device);
                        goto err_restore;
                }
                
                if ( (uart_test.fpar.fd = open (transmit_filename, O_RDONLY)) < 0) {
                        tst_brkm(TBROK, NULL, "Unable to open file %s for transmitting: %s", 
                                 transmit_filename, strerror(errno));
                        uart_test.fpar.fd = 0;
                        goto err_restore;
                }
                
                if (fstat(uart_test.fpar.fd, &st) < 0) {
                        tst_brkm(TBROK, NULL, "Unable to get file attributes: %s", strerror(errno));
                        goto err_restore;
                }
                
                uart_test.data = (unsigned char *) malloc(DATA_BUFFER_SIZE);
                if (!uart_test.data) {
                        tst_brkm(TBROK, NULL, "Unable to allocate data buffer:");
                        perror("malloc()");
                        goto err_restore;
                }

                uart_test.fpar.file_size = st.st_size;
        }

        /* Flush it unconditionaly */
        tst_resm(TINFO, "  Flushing device %s...", uart_test.device);
        if (tcflush(uart_test.fd, TCIOFLUSH) != 0)
        {
                tst_resm(TWARN, " TCIOFLUSH   => Failure.");
                perror(uart_test.device);
                goto err_restore;
        }
        alarm_handler(1);

        tst_resm(TINFO, "Setup success.");
        rv = TPASS;
        return rv;

err_restore:
        if (uart_test.fpar.fd != 0) 
                close(uart_test.fpar.fd);
        VT_cleanup_uart(&uart_test);
err_return:
        tst_resm(TFAIL, "Setup failure.");
        return rv;
}



/*================================================================================================*/
/*===== VT_uart_addon_cleanup =====*/
/**
@brief  assumes the post-condition of the test case execution

@param  None

@return On success - return TPASS
On failure - return the error code*/
/*================================================================================================*/
int VT_uart_addon_cleanup(void)
{
        int     rv = TPASS;
        free(uart_test.data);
        if (uart_test.fpar.fd != 0) 
                close(uart_test.fpar.fd);
                
        if (exchange_direction == 0)
                tst_resm(TINFO, "Cleaning up destination UART...");
        else
                tst_resm(TINFO, "Cleaning up source UART...");
                
        if (VT_cleanup_uart(&uart_test) != TPASS)
                rv = TFAIL;

        if (rv != TPASS)
                tst_resm(TINFO, "Cleanup failure.");
        else
                tst_resm(TINFO, "Cleanup success.");

        return rv;
}

/*================================================================================================*/
/*===== VT_uart_addon_test =====*/
/**
@brief  main test function
@param  Input :   testcase type (soft/hard or none flow control)
Output :  None

@return On success - return TPASS
On failure - return the error code*/
/*================================================================================================*/
int VT_uart_addon_test(int testcase)
{
        int     rv = TPASS;
        int     error;
        int     thret_return;
        //int     bufidx;
        //int     nb_errors;
        //int     nb_sec;

        if (exchange_direction == 0)
                tst_resm(TINFO, "Testing receive from %s:", uart_test.device,
                                 uart_flow_ctrl == FLOW_CTRL_HARD ? "hardware" :
                                 uart_flow_ctrl == FLOW_CTRL_SOFT ? "software" : "???");
        else 
                tst_resm(TINFO, "Testing transmit from %s:", uart_test.device,
                                 uart_flow_ctrl == FLOW_CTRL_HARD ? "hardware" :
                                 uart_flow_ctrl == FLOW_CTRL_SOFT ? "software" : "???");
        tst_resm(TINFO, "    => testcase %d.", testcase);

        /* 
        * Lock reader and writer mutex, so they will wait on it before they begin 
        * their real job.
        */
        pthread_mutex_lock(&rw_mutex);
        
        /* 
        * Create the signal, reader or the writer threads
        */    
        if (exchange_direction == 0) {
                tst_resm(TINFO, "  Creating reader thread => [ R  ]...");
                error = pthread_create(&rw_thread, NULL, VT_th_reader_addon, NULL);
                if (error < 0) {
                        perror("pthread_create(reader)");
                        rv = TFAIL;
                        goto error_exit;
                }
        }
        else {
                tst_resm(TINFO, "  Creating writer thread => [W   ]...");
                error = pthread_create(&rw_thread, NULL, VT_th_writer_addon, NULL);
                if (error < 0)
                {
                        perror("pthread_create(writer)");
                        rv = TFAIL;
                        goto error_exit;
                }
        }
        th_block_signals();
        tst_resm(TINFO, "  Creating signal thread => [   S]...");
        error = pthread_create(&thid_signal, NULL, VT_th_signal_addon, NULL);
        if (error < 0)
        {
                perror("pthread_create(signal)");
                rv = TFAIL;
                goto error_exit_cancel_rw_thread;
        }

        
        if (exchange_direction == 0)
                tst_resm(TINFO, "  Signalig reader thread...");
        else 
                tst_resm(TINFO, "  Signalig writer thread...");
        
        pthread_mutex_unlock(&rw_mutex);

        if (exchange_direction == 0)
                tst_resm(TINFO, "  Waiting for reader thread...");
        else 
                tst_resm(TINFO, "  Waiting for writer thread...");
        /* 
        * wait for the reader or writer threads,
        * and check their status
        */
        error = pthread_join(rw_thread, (void **) &thret_return);
        if (error < 0)
        {
                if (exchange_direction == 0)
                        perror("pthread_join(reader)");
                else
                        perror("pthread_join(writer)");
                rv = TFAIL;
                goto error_exit;
        }
        if (thret_return != TPASS && thret_return != TWARN)
        {
                rv = thret_return;
                if (exchange_direction == 0)
                        tst_resm(TFAIL, "Reader has failed");
                else
                        tst_resm(TFAIL, "Writer has failed");
                goto error_exit;
        }

        /* 
        * Transfert is OK, now check for data errors
        */

       switch (testcase)
       {
       case TESTCASE_SIMPLE:
                error = VT_check_hardware(1, 1, 0, 1);
                break;

        case TESTCASE_SW_FLOW:
        case TESTCASE_HW_FLOW:
                error = VT_check_hardware(1, 1, 1, 1);
                break;

        default:
                rv = TBROK;
                break;
        }

        if (thret_return == TWARN)
                rv = TFAIL;

        return rv;

        error_exit_cancel_rw_thread:
        error = pthread_cancel(rw_thread);
        if (error < 0)
                perror("pthread_cancel()");

        error_exit:
        //VT_check_hardware(1, 1, 1, 1);
        return rv;
}

void   *VT_th_writer_addon(void *param)
{
        int     rv = TPASS;
        int     error,
                remaining,
                check_timer_val;
        
        int     i = 0,
                r_blocks_num = 0,
                last_block_size = 0,
                all_trans_tmp = 0;

        transfer_header_t trans_header;
        /* int bufidx; */
        int     percent = 0,
                last_percent = 0;
        unsigned char * MemPtr;
        memset (&trans_header, 0, sizeof (transfer_header_t));
        strcpy (trans_header.start_message, start_message);
        strcpy (trans_header.stop_message, stop_message);
        getcwd (trans_header.transfer_fname, 256);
        strcat (trans_header.transfer_fname, "/");
        strcat (trans_header.transfer_fname, transmit_filename);
        //strcpy (trans_header.transfer_fname, transmit_filename);
        trans_header.file_size = uart_test.fpar.file_size;
        th_block_signals();

        THW_INFO("Ready.");
        pthread_mutex_lock(&rw_mutex);
        pthread_mutex_unlock(&rw_mutex);

        THW_INFO("Writing data to source UART...");

        remaining = sizeof (transfer_header_t);
        
        MemPtr = (unsigned char *)&trans_header;
        do {
                error = write_block_to_uart(uart_test.fd, MemPtr, remaining);
                switch (error) {
                    case -1:
                            THW_FAIL("Header transmitting failure!: write() system error!");
                            rv = TFAIL;
                            goto th_exit;
                    case -2:
                            THW_FAIL("write_block_to_uart (): Invalid parameters!");
                            rv = TFAIL;
                            goto th_exit;
                    case RW_SUCCESS:
                            break;
                    default:
                        {
                            if (error != 0) {
                                    MemPtr += error;
                                    remaining -= error;
                            }
                                
                            check_timer_val = check_timer();
                            if (check_timer_val == -1) {
                                    THW_FAIL("Header transmitting failure. Timeout expired!");
                                    rv = TFAIL;
                                    goto th_exit;
                            }
                            
                            if (check_timer_val == -2) { 
                                    printf ("C"); //waiting
                                    fflush(stdout);
                            }
                        } //default
                }
        } while (error != RW_SUCCESS);
        
        THW_INFO ("Header was successfully transmitted!");
        r_blocks_num = uart_test.fpar.file_size / TRANS_BLOCK;
        last_block_size = uart_test.fpar.file_size % TRANS_BLOCK;
        if (last_block_size != 0) 
                r_blocks_num++;
    
        for (i = 0; i < r_blocks_num; i++ )
        {
                if (i == r_blocks_num - 1 && last_block_size != 0) {
                        remaining = last_block_size;
                        tst_resm (TINFO, "last_block_size = %d", last_block_size);
                        if (read (uart_test.fpar.fd, uart_test.data, last_block_size) != last_block_size) {
                                THW_FAIL("Error file reading!");
                                rv = TFAIL;
                                goto th_exit;
                        }
                } else  {
                        remaining = TRANS_BLOCK;
                        if (read (uart_test.fpar.fd, uart_test.data, TRANS_BLOCK) != TRANS_BLOCK) {
                                THW_FAIL("Error file reading!");
                                rv = TFAIL;
                                goto th_exit;
                        }
                }
                
                MemPtr = uart_test.data;
                
                do {
                        error = write_block_to_uart(uart_test.fd, MemPtr, remaining);
                        switch (error) {
                        case -1:
                                THW_FAIL("File transmitting failure! write() system error!");
                                rv = TFAIL;
                                goto th_exit;
                        case -2:
                                THW_FAIL("write_block_to_uart (): Invalid parameters!");
                                rv = TFAIL;
                                goto th_exit;
                        case RW_SUCCESS:
                                break;
                        default:
                            {
                                if (error != 0) {
                                        MemPtr += error;
                                        remaining -= error;
                                }
                                
                                check_timer_val = check_timer();
                                if (check_timer_val == -1) {
                                        THW_FAIL("File transmitting failure. Timeout expired!");
                                        rv = TFAIL;
                                        goto th_exit;
                                }
                            
                                if (check_timer_val == -2) {
                                        printf ("C"); //waiting
                                        fflush(stdout);
                                }
                            } //default
                        }
                } while (error != RW_SUCCESS);

                if (i == r_blocks_num - 1 && last_block_size != 0)
                        all_trans_tmp += last_block_size;
                else
                        all_trans_tmp += TRANS_BLOCK;

                percent = ( all_trans_tmp * 100 ) / uart_test.fpar.file_size;
                if (percent >= (last_percent + 10))
                {
                        THW_INFO("%3d%% (%d/%d)... ", percent, all_trans_tmp, uart_test.fpar.file_size);
                        last_percent = percent;
                }

                if (thread_sleep_flag && thread_sleep_time != 0 && sleep_counter <= NUM_SLEEP)
                {
                        if (percent > 100 * sleep_counter / (NUM_SLEEP + 1)) {
                                sleep_counter++;
                                THW_INFO("writing thread sleep %d sec!", thread_sleep_time);
                                sleep(thread_sleep_time);
                                THW_INFO("writing thread wake up!");        
                        }
                }

        }// for 
        all_bytes_transfered = uart_test.fpar.file_size + sizeof (transfer_header_t);
th_exit:

        THW_INFO("Closing transmitted file...");
        close(uart_test.fpar.fd);
        uart_test.fpar.fd = 0;
        THW_INFO("Exiting with return code = %d", rv);
        if (rv == TPASS) 
                sleep(4);
        pthread_exit((void *) rv);
}

void   *VT_th_reader_addon(void *param)
{
        int     rv = TPASS;
        int     error,
                remaining,
                check_timer_val;
        
        int     i = 0,
                r_blocks_num = 0,
                last_block_size = 0,
                all_rec_tmp = 0;
                
        transfer_header_t rec_header;
        
        int     percent = 0,
                last_percent = 0;
        unsigned char * MemPtr;// data_ptr;
        memset (&rec_header, 0, sizeof (transfer_header_t));

        th_block_signals();

        THR_INFO("Ready.");
        pthread_mutex_lock(&rw_mutex);
        pthread_mutex_unlock(&rw_mutex);

        THR_INFO("Reading data from destination UART...");

        remaining = sizeof (transfer_header_t);
        
        MemPtr = (unsigned char *)&rec_header;
        do {
                error = read_block_from_uart(uart_test.fd, MemPtr, remaining);
                switch (error) {
                    case -1:
                            THR_FAIL("Header transfert failure! read() ");
                            rv = TFAIL;
                            goto th_exit;
                    case -2:
                            THR_FAIL("read_block_from_uart (): Invalid parameters!");
                            rv = TFAIL;
                            goto th_exit;
                    case RW_SUCCESS:
                            break;
                    default:
                        {
                            if (error != 0) {
                                    MemPtr += error;
                                    remaining -= error;
                            }
                                
                            check_timer_val = check_timer();
                            if (check_timer_val == -1) {
                                    THR_FAIL("Header transfert failure. Timeout expired!");
                                    rv = TFAIL;
                                    goto th_exit;
                            }
                            
                            if (check_timer_val == -2) {
                                    printf ("C"); //waiting
                                    fflush (stdout);
                            }
                        } //default
                }
        } while (error != RW_SUCCESS);
        
        if (check_header(&rec_header) < 0) {
                THR_FAIL("Header checking is failed!");
                rv = TFAIL;
                goto th_exit;
        } 
        
        THR_INFO("Header was successfully received!");
        uart_test.fpar.file_size = rec_header.file_size;
        THR_INFO("Filename of transferred file %s", rec_header.transfer_fname);        
        if (receive_to_ram_flag == 0 || 
           (receive_to_ram_flag && save_to_file_flag)) {

                CreateReceiveFname(rec_header.transfer_fname, receive_filename);
                uart_test.fpar.fd = open(receive_filename, O_WRONLY | O_CREAT | O_TRUNC, 
                                                            S_IRWXU | S_IRWXG | S_IRWXO);
                if (uart_test.fpar.fd < 0) {
                        tst_brkm(TBROK, NULL, "Unable to open file %s for receiving: %s", 
                                               receive_filename, strerror(errno));
                        uart_test.fpar.fd = 0;
                        rv = TFAIL;
                        goto th_exit;
                }

                if (!receive_to_ram_flag)
                        THR_INFO("Dump file name: %s", receive_filename);
        }

        if (receive_to_ram_flag) {
                if (uart_test.fpar.file_size > 20 * 1024 * 1024) {
                        tst_brkm(TBROK, NULL, "File size is very large for keeping in RAM!");
                        rv = TFAIL;
                        goto err_rest;
                }
                
                uart_test.data = (unsigned char *) malloc(uart_test.fpar.file_size + 
                                                          uart_test.fpar.file_size % 4096);
                if (!uart_test.data) {
                        tst_brkm(TBROK, NULL, "Unable to allocate data buffer:");
                        perror("malloc()");
                        rv = TFAIL;
                        goto err_rest;
                }
                
                //THR_INFO("memory for file receiving allocated successfully!");
        }

        
        r_blocks_num = uart_test.fpar.file_size / REC_BLOCK;
        last_block_size = uart_test.fpar.file_size % REC_BLOCK;
        if (last_block_size != 0) 
                r_blocks_num++;
        
        MemPtr = uart_test.data;

        for (i = 0; i < r_blocks_num; i++ )
        {
                if (i == r_blocks_num - 1 && last_block_size != 0) {
                        remaining = last_block_size;
                        THR_INFO ("last_block_size = %d", last_block_size);
                } else 
                        remaining = REC_BLOCK;

                do {
                        error = read_block_from_uart(uart_test.fd, MemPtr, remaining);
                        switch (error) {
                        case -1:
                                THR_FAIL("File transfert failure! read() system error!");
                                rv = TFAIL;
                                goto err_rest;
                        case -2:
                                THR_FAIL("read_block_from_uart (): Invalid parameters!");
                                rv = TFAIL;
                                goto err_rest;
                        case RW_SUCCESS:
                                break;
                        default:
                            {
                                if (error != 0) {
                                        MemPtr += error;
                                        remaining -= error;
                                }
                                
                                check_timer_val = check_timer();
                                if (check_timer_val == -1) {
                                        THR_FAIL("File transfert failure. Timeout expired!");
                                        rv = TFAIL;
                                        goto err_rest;
                                }
                            
                                if (check_timer_val == -2) {
                                        printf ("C"); //waiting
                                        fflush(stdout);
                                }
                            } //default
                        }
                } while (error != RW_SUCCESS);
                
                if (i == r_blocks_num - 1 && last_block_size != 0) {
                        all_rec_tmp += last_block_size;
                        remaining = last_block_size;
                } else { 
                        all_rec_tmp += REC_BLOCK;
                        remaining = REC_BLOCK;
                } //else

                if (receive_to_ram_flag)
                        MemPtr = uart_test.data + all_rec_tmp;
                else {
                        MemPtr = uart_test.data;
                        if (write (uart_test.fpar.fd, uart_test.data, remaining) != remaining) {
                                THR_FAIL("Error file writing!");
                                rv = TFAIL;
                                goto err_rest;
                        }
                }

                percent = ( all_rec_tmp * 100 ) / uart_test.fpar.file_size;
                if (percent >= (last_percent + 10))
                {
                        THR_INFO("%3d%% (%d/%d)... ", percent, all_rec_tmp, uart_test.fpar.file_size);
                        last_percent = percent;
                }

                if (thread_sleep_flag && thread_sleep_time != 0 && sleep_counter <= NUM_SLEEP)
                {
                        if (percent > 100 * sleep_counter / (NUM_SLEEP + 1)) {
                                sleep_counter++;
                                THR_INFO("reading thread sleep %d sec!", thread_sleep_time);
                                sleep(thread_sleep_time);
                                THR_INFO("reading thread wake up!");        
                        }
                }
        } //for

        all_bytes_transfered = uart_test.fpar.file_size + sizeof (transfer_header_t);

        if (receive_to_ram_flag) {
                sleep(1);
                error = compare_data ((char*) rec_header.transfer_fname, all_rec_tmp);
                if (error != 0) {
                        rv = TWARN;
                        THR_WARN ("Files comparison failed (number of errors  = %d)", error);
                }
                else 
                        THR_INFO ("Files comparison successful!");
                
                if (save_to_file_flag) {
                        THR_INFO("Saving to file %s!", receive_filename);
                        if (write (uart_test.fpar.fd, uart_test.data, all_rec_tmp) 
                                                                     != all_rec_tmp) {
                                THR_FAIL("Error file writing!");
                                rv = TFAIL;
                                goto err_rest;
                        }
                }
        } //if 

err_rest:
        if (uart_test.fpar.fd != 0) {
                THR_INFO("Closing received file...");
                close(uart_test.fpar.fd);
                uart_test.fpar.fd = 0;
        }
        if (uart_test.data != NULL) {
                free(uart_test.data);
                uart_test.data = NULL;
        }
th_exit:
        THR_INFO("Exiting with return code = %d", rv);
        if (rv == TPASS || rv == TWARN) 
                sleep(1);
        pthread_exit((void *) rv);
}

void   *VT_th_signal_addon(void *param)
{
        sigset_t mask;
        int     signal;
        int     error;
        int     rv;

        sigemptyset(&mask);
        sigaddset(&mask, SIGINT);
        while (1)
        {
                sigwait(&mask, &signal);
                THS_INFO("Got signal \"%s\"...", sys_siglist[signal]);
                switch (signal)
                {
                case SIGINT:
                        error = pthread_cancel(rw_thread);
                        if (error < 0)
                                perror("pthread_cancel()");
                        rv = TBROK;
                        pthread_exit(&rv);
                        break;
                default:
                        THS_INFO("Ignoring signal \"%s\"", sys_siglist[signal]);
                }
        }
}

void th_block_signals(void)
{
        sigset_t mask;

        sigemptyset(&mask);
        sigaddset(&mask, SIGINT);
        pthread_sigmask(SIG_BLOCK, &mask, NULL);
}

int alarm_handler(int stat)
{
        struct sigaction old;
        struct sigaction new_action;
        memset(&new_action, 0, sizeof (struct sigaction));
        memset(&old, 0, sizeof (struct sigaction));
        if (stat != 0 && stat != 1)
                return -1;
        
        if (stat == 0) {
                sigaction (SIGALRM, NULL, &old);
                if (old.sa_handler != SIG_DFL) {
                        new_action.sa_handler = SIG_DFL;
                        sigaction (SIGALRM, &new_action, NULL);
                }
        } else {
                sigaction (SIGALRM, NULL, &old);
                if (old.sa_handler != timer_handler) {
                        new_action.sa_handler = timer_handler;
                        sigemptyset (&new_action.sa_mask);
                        sigaddset(&new_action.sa_mask, SIGINT);
                        sigaction (SIGALRM, &new_action, NULL);
                }
        }

        return 0;
}

/*================================================================================================*/
/*===== run_timer =====*/
/** 
@brief   

@param  None 
    
@return  
        */
/*================================================================================================*/
void run_timer(void)
{
        struct itimerval timer;
        long int msec;

        /* 
        * Configure the timer to expire. 
        * We are quit large on value since we have to take into account 
        * some "random" overhead
        */

        /* Worst case: 1 start bit + 8 data bits + 2 stop bits */
        /* We finally add 100% for overhead */
        msec = 3000; //3 sec
        timer.it_value.tv_sec = msec / 1000;
        timer.it_value.tv_usec = (msec - (timer.it_value.tv_sec * 1000)) * 1000;
        timer.it_interval.tv_sec = timer.it_value.tv_sec;
        timer.it_interval.tv_usec = timer.it_value.tv_usec;
        //tst_resm(TINFO, "  [   T] Timer is running!");

        /* Start timer...  */
        setitimer(ITIMER_REAL, &timer, NULL);
        timer_running = 1;
        cout_timer_sign = 0;
        prev_cout_sign = 0;
}

void stop_timer (void)
{
        struct itimerval timer;
        memset(&timer, 0, sizeof(struct itimerval));
        //tst_resm(TINFO, "  [   T] Timer is stoped!");
        //Stop timer...
        setitimer(ITIMER_REAL, &timer, NULL);
        timer_running = 0;
}

int check_timer(void)
{
        if (timer_running == 1) {

                if (cout_timer_sign == 10) //Timeout expired!
                        return -1;

                if (prev_cout_sign < cout_timer_sign) {   //For print 'C' on terminal (waiting)
                        prev_cout_sign = cout_timer_sign;
                        return -2;
                }
        }

        return 0;
}
int check_header(transfer_header_t *h)
{
        if ( strcmp (h->start_message, start_message) != 0 || 
             strcmp (h->stop_message, stop_message)   != 0 )
                return -1;
        else 
                return 0;
}
/*================================================================================================*/
/*===== timer_handler=====*/
/** 
@brief This is a timer handler. 

@param signum - signal number  
    
@return None */
/*================================================================================================*/
void timer_handler(int signum)
{
        //tst_resm(TINFO, "  [   T] Timeout expired!");
        cout_timer_sign++;
}

/*================================================================================================*/
/*===== read_block_from_uart=====*/
/*!
 * @brief  UART data block reading function.
 *
 * @param uart_fd [Input] The UART handle obtained from open()..
 *@param memPtr [Input] Pointer to the first element of an data array.
 * @param blockSize [Input] Size of reading block.
 *
 * @return 
 * @li RW_SUCCESS: If data block was read completely.
 * @li -2: memPtr is a null pointer, or uart_fd handle less or equal 0.
 * @li -1: read() system error.
 * @li Number of read bytes: If device is busy (errno == EAGAIN).
 */
/*================================================================================================*/

int read_block_from_uart(int uart_fd, unsigned char *memPtr, int blockSize)
{
        int Bytes_read = 0; 
        int error = 0;
        //tst_resm(TINFO, "memPtr = 0x%x", (unsigned long) memPtr);
        if (uart_fd == 0 || memPtr == NULL)
                return -2;
        if (blockSize == 0)
                return RW_SUCCESS;

        do {
                error = read(uart_fd, memPtr + Bytes_read, blockSize - Bytes_read);
                //if (error == 0) 
                //        tst_resm (TINFO, "error = %d", error);

                if (error < 0 && errno != EAGAIN)
                        return -1;

                else if ((error < 0 && errno == EAGAIN) || error == 0) {
                        if (timer_running == 0)
                                run_timer();
                        return Bytes_read;
                } else {
                        if (timer_running == 1)
                            stop_timer();

                        Bytes_read += error;
                }
                //tst_resm (TINFO, "error = %d", error);
        } while (blockSize - Bytes_read != 0);
        
        return RW_SUCCESS;
}
/*================================================================================================*/
/*===== write_block_to_uart=====*/
/*!
 * @brief  UART data block writing function.
 *
 * @param uart_fd [Input] The UART handle obtained from open()..
 *@param memPtr [Input] Pointer to the first element of an data array.
 * @param blockSize [Input] Size of  data array..
 *
 * @return 
 * @li RW_SUCCESS: If data block was transfered completely.
 * @li -2: memPtr is a null pointer, or uart_fd handle less or equal 0.
 * @li -1: write() system error.
 * @li Number of transfered bytes: If device is busy (errno == EAGAIN).
 */
/*================================================================================================*/
int write_block_to_uart(int uart_fd, unsigned char *memPtr, int blockSize)
{
        int Bytes_write = 0; 
        int error;
        if (uart_fd <= 0 || memPtr == NULL)
                return -2;
        if (blockSize == 0)
                return RW_SUCCESS;

        do {
                error = write(uart_fd, memPtr + Bytes_write, blockSize - Bytes_write);
        
                if (error < 0 && errno != EAGAIN)
                        return -1;
                
                else if ((error < 0 && errno == EAGAIN) || error == 0) {
                        if (timer_running == 0)
                                run_timer();

                        return Bytes_write;
                } else {
                        if (timer_running == 1)
                                stop_timer();

                        Bytes_write  += error;
                }

        } while (blockSize - Bytes_write != 0);

        return RW_SUCCESS;
}

int CreateReceiveFname(char *Fname, char *ReceiveFname)
{
        //char tmp1[] = "receive";
        //char *Ptr, *RPtr, *tmp1Ptr;
        //int FnameSize = strlen (Fname);
        if (ReceiveFname == NULL)
                return -1;
        strcpy (ReceiveFname, "_@received_file");
                return 0;
/*        if (Fname == NULL || strlen(Fname) == 0) {
                strcpy (ReceiveFname, "_@receive_file");
                return 0;
        }
        Ptr = Fname + FnameSize;
        RPtr = ReceiveFname;
        tmp1Ptr = tmp1;
        while (*Ptr != *Fname || *(Ptr - 1) != '/') 
                Ptr--;
    
        while (*Ptr != '.' || *Ptr != '\0') 
                *RPtr++ = *Ptr++;
    
        *RPtr++ = '_';
        
        while (*tmp1Ptr != '\0') 
                *RPtr++ = *tmp1Ptr++;
        
        if (*Ptr == '.') 
                while (*Ptr != '\0') *RPtr++ = *Ptr++;

        *RPtr = '\0'; //End of receive filename string

        return 0;*/
}

int MemCmp(unsigned char *Mem1, unsigned char *Mem2, int size)
{
    int nb_errors = 0, i;
    for (i = 0; i < size; i++)
            if (*(Mem1 + i) != *(Mem2 + i))
                    nb_errors ++;
    return nb_errors;
}

int compare_data (char *Fname, int size)
{
        int fd1, nb_errors  = 0, i, n_blocks = 0, 
        block_size = 0, last_block_size = 0,
        percent = 0; 
        struct stat st1;
        unsigned char *Rec_buff, *Trans_buff;

        if (!receive_to_ram_flag || uart_test.data == NULL)
                return -1;

        Rec_buff = uart_test.data;
        Trans_buff = (unsigned char *) malloc (COMPARE_BUFFER);

        if (Trans_buff == NULL)
                return -1;
    
        if ((fd1 = open(Fname, O_RDONLY)) < 0) {
                tst_resm(TFAIL, "Open file %s failes", Fname);
                return -1;
        }

        if (fstat(fd1, &st1) < 0) {
                tst_resm(TFAIL,"Unable to get file attributes: %s", Fname);
                return -1;
        }

        if (size != st1.st_size)
                return -2;

        n_blocks = st1.st_size / COMPARE_BUFFER;
        last_block_size = st1.st_size % COMPARE_BUFFER;
        if (last_block_size != 0)
                n_blocks++;
    
        for (i = 0; i < n_blocks; i++)
        {
                if (i == n_blocks - 1 && last_block_size != 0)
                        block_size = last_block_size;
                else
                        block_size = COMPARE_BUFFER;

                if ( (i * 100) / n_blocks > percent + 10) {
                        percent = (i * 100) / n_blocks;    
                        tst_resm (TINFO, "comparison: %3d%%", percent);
                } 

                if ( i == n_blocks - 1) {
                        percent = 100;
                        tst_resm (TINFO, "comparison: %3d%%", percent);
                }

                read(fd1, Trans_buff, block_size);
                nb_errors += MemCmp (Rec_buff, Trans_buff, block_size);
                if (nb_errors != 0 && showmessage == 0) {
                        tst_resm(TWARN, "First error %3d%% ", i * 100 / n_blocks);
                        showmessage = 1;
                }
                Rec_buff += block_size;
        }

        return nb_errors;
}

int VT_check_hardware(int check_error, int check_break, int check_flow, int check_modem)
{
        int     rv = TPASS;

        tst_resm(TINFO, "  Checking harware behaviour...");

        if (ioctl(uart_test.fd, TIOCGICOUNT, (unsigned long) &uart_test.icounter) < 0)
        {
                tst_resm(TWARN, "    => ioctl(src, TIOCGICOUNT) failed.");
                perror(uart_test.device);
                return TBROK;
        }

#define ICNT(who, what)  who##_test.icounter.what - who##_test.icounter_saved.what

        tst_resm(TINFO, "/=============================\\");
        tst_resm(TINFO, "|    UART hardware summary    |");
        tst_resm(TINFO, "+============+=================");
        tst_resm(TINFO, "|            | %-14s |", "SRC");
        tst_resm(TINFO, "+------------+-----------------");
        tst_resm(TINFO, "| device     | %-14s |", uart_test.device);
        if (exchange_direction)
        tst_resm(TINFO, "|#send bytes | %-14d |", all_bytes_transfered);
        else
        tst_resm(TINFO, "|#recv bytes | %-14d |", all_bytes_transfered);
        tst_resm(TINFO, "|#cts        | %-14d |", ICNT(uart, cts));
        tst_resm(TINFO, "|#dsr        | %-14d |", ICNT(uart, dsr));
        tst_resm(TINFO, "|#rng        | %-14d |", ICNT(uart, rng));
        tst_resm(TINFO, "|#dcd        | %-14d |", ICNT(uart, dcd));
        tst_resm(TINFO, "|#rx         | %-14d |", ICNT(uart, rx));
        tst_resm(TINFO, "|#tx         | %-14d |", ICNT(uart, tx));
        tst_resm(TINFO, "|#frame err  | %-14d |", ICNT(uart, frame));
        tst_resm(TINFO, "|#overrun    | %-14d |", ICNT(uart, overrun));
        tst_resm(TINFO, "|#parity err | %-14d |", ICNT(uart, parity));
        tst_resm(TINFO, "|#buf overrun| %-14d |", ICNT(uart, buf_overrun));
        tst_resm(TINFO, "|#break      | %-14d |", ICNT(uart, brk));
        tst_resm(TINFO, "\\------------+----------------/");

        if (exchange_direction) {
                if (ICNT(uart, tx) != all_bytes_transfered) {
                        tst_resm(TWARN, "  => Problem on TX counters.");
                        rv = TWARN;
                }
        }
        else {
                if (ICNT(uart, rx) != all_bytes_transfered) {
                        tst_resm(TWARN, "  => Problem on RX.");
                        rv = TWARN;
                }
        }

        if (check_error)
        {
                if ((ICNT(uart, frame) > 0) ||
                    (ICNT(uart, overrun) > 0) ||
                    (ICNT(uart, parity) > 0) ||
                    (ICNT(uart, buf_overrun) > 0))
                {
                        tst_resm(TWARN, "  => There are some hardware error(s).");
                        rv = TWARN;
                }
        }

        if (check_break)
        {
                if ((ICNT(uart, brk) > 0))
                {
                        tst_resm(TWARN, "  => Error(s): break found.");
                        rv = TWARN;
                }
        }

        if (check_flow)
        {
                if (uart_flow_ctrl == FLOW_CTRL_HARD)
                {
                        if (ICNT(uart, cts) == 0)
                        {
                                tst_resm(TWARN, "  => No hardware flow control activation.");
                                /* the next line is commented out because the feature is not supported */
/*                                
                                rv = TFAIL;
*/
                        }
                }

                if (uart_flow_ctrl == FLOW_CTRL_SOFT)
                {
                        if (ICNT(uart, tx) == 0)
                        {
                                tst_resm(TWARN, "  => No software flow control activation.");
                                rv = TWARN;
                        }
                        if (ICNT(uart, tx))
                        {
                                tst_resm(TWARN, "  => Software flow control activation problem.");
                                rv = TWARN;
                        }
                }
        }

        /* dsr, rng and dcd? */
        if (check_modem)
        {
        }

        return rv;
#undef ICNT
}

#ifdef __cplusplus
} 
#endif
