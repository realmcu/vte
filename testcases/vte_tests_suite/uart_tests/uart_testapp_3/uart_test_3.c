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
    @file   uart_test_3.c 

    @brief Configure the serial interface using MXC UART and/or External UART low-level driver 
                Read / Write test 

====================================================================================================
Revision History:
                            Modification     Tracking
Author                          Date          Number    Description of Changes
-------------------------   ------------    ----------  -------------------------------------------
C.GAGNERAUD/cgag1c           01/06/2005    TLSbo45060    Rework and improve UART test application
I.Inkina/nknl001             22/08/2005    TLSbo52626    Update Uart test 
E.Gromazina                  14/09/2005                         min fix  
E.Gromazina                  13/10/2005    TLSbo56650    replace the FAIL by a WARN when verifying         
                                                        the CTS/RTS activation"
Pradeep K /b01016            05/08/2007	   ENGR11834    Fix for Soft and Hard flow control issue			
Dmitriy Kardakov                                        New transfer data test
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
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
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


/* 
* Global variable 
*/
pthread_t thid_writer,
        thid_reader,
        thid_signal;

/* 
* Local typedef, macros, ...
*/
#define THR_INFO(fmt, args...) tst_resm(TINFO, "  [R   ] " fmt, ##args)
#define THW_INFO(fmt, args...) tst_resm(TINFO, "  [ W  ] " fmt, ##args)
#define THC_INFO(fmt, args...) tst_resm(TINFO, "  [  C ] " fmt, ##args)
#define THS_INFO(fmt, args...) tst_resm(TINFO, "  [   S] " fmt, ##args)
#define THR_FAIL(fmt, args...) tst_resm(TFAIL, "  [R   ] " fmt, ##args)
#define THW_FAIL(fmt, args...) tst_resm(TFAIL, "  [ W  ] " fmt, ##args)
#define THC_FAIL(fmt, args...) tst_resm(TFAIL, "  [  C ] " fmt, ##args)
#define THS_FAIL(fmt, args...) tst_resm(TFAIL, "  [   S] " fmt, ##args)

/* 
* local variable 
*/
static int cout_timer_sign = 0;
static unsigned long nb_read = 0;

/* static int reader_sleep = 0; */
/* static int reading = 0; */
static pthread_mutex_t read_mutex = PTHREAD_MUTEX_INITIALIZER;
static unsigned long nb_write = 0;

/* static int writing = 0; */
static pthread_mutex_t write_mutex = PTHREAD_MUTEX_INITIALIZER;

/* 
* local function prototype
*/
/* Block all signals (except SIGINT) for da calling thread */
static void th_block_signals(void);


pthread_t thid_writer,
        thid_reader,
        thid_signal;

//int     VT_setup_uart(uart_config_t * cfg, uart_test_t * uart, char *desc, int open_flag);
//int     VT_cleanup_uart(uart_test_t * uart);

int     VT_check_hardware(int check_error, int check_break, int check_flow, int check_modem);

void   *VT_th_writer(void *param);
void   *VT_th_reader(void *param);
void   *VT_th_signal(void *param);

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
  /** Source and destination UART to test */
static uart_test_t src_uart,
        dst_uart;
static flow_ctrl_t uart_flow_ctrl;
static int uart_transfert_length;
static int uart_baud_rate;


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
int     run_timer(void);
void    timer_handler(int signum);

/*================================================================================================*/
/*===== VT_uart_test4_setup =====*/
/** 
* Assumes the test preconditions, open and configure source and destination UART.
* 
* 
* @return */
/*================================================================================================*/
int VT_uart_test3_setup(uart_config_t * src_cfg, uart_config_t * dst_cfg,
                        int transfert_length, flow_ctrl_t flow_ctrl, int baud_rate)
{
        int     rv = TFAIL;
        int     bufidx;

        uart_transfert_length = transfert_length;
        uart_flow_ctrl = flow_ctrl;
        uart_baud_rate = baud_rate;
        src_uart.flow_ctrl = src_cfg->flow_ctrl;
        dst_uart.flow_ctrl = dst_cfg->flow_ctrl;

        /* Print a summary of test conditions. */
        tst_resm(TINFO, "* Testing transfert from %s to %s:", src_cfg->device, dst_cfg->device);
        tst_resm(TINFO, "* %-20s: %d", "Speed", uart_baud_rate);
        tst_resm(TINFO, "* %-20s: %d", "Parity", src_cfg->parity_type);
        tst_resm(TINFO, "* %-20s: %d", "Character length", src_cfg->char_length);
        tst_resm(TINFO, "* %-20s: %d", "Number of stop bits", src_cfg->stop_bits);
        tst_resm(TINFO, "* %-20s: %d", "Transfert length", uart_transfert_length);
        tst_resm(TINFO, "* %-20s: %s", "Flow control",
                 uart_flow_ctrl == FLOW_CTRL_HARD ? "hard" :
                 uart_flow_ctrl == FLOW_CTRL_SOFT ? "soft" : "none");

        tst_resm(TINFO, "Setting up source UART...");
        if (VT_setup_uart(src_cfg, &src_uart, "source UART", 0) != TPASS)       // O_NONBLOCK
        {
                goto err_return;
        }

        tst_resm(TINFO, "Setting up destination UART...");
        if (VT_setup_uart(dst_cfg, &dst_uart, "destination UART", 0) != TPASS)  // O_NONBLOCK
        {
                goto err_restore_src;
        }

        /* Save initial icounter */
        tst_resm(TINFO, "Saving SRC initial icounter...", src_uart.device);
        if (ioctl(src_uart.fd, TIOCGICOUNT, (unsigned long) &src_uart.icounter_saved) < 0)
        {
                tst_resm(TWARN, "    => Failure.");
                perror(src_uart.device);
                goto err_restore_both;
        }
        tst_resm(TINFO, "Saving DST initial icounter...", dst_uart.device);
        if (ioctl(dst_uart.fd, TIOCGICOUNT, (unsigned long) &dst_uart.icounter_saved) < 0)
        {
                tst_resm(TWARN, "    => Failure.");
                perror(dst_uart.device);
                goto err_restore_both;
        }

        tst_resm(TINFO, "Allocating data buffers (%d bytes each)...", uart_transfert_length);
        src_uart.data = calloc(uart_transfert_length, 1);
        if (!src_uart.data)
        {
                tst_brkm(TBROK, NULL, "Unable to allocate %d bytes for sending:",
                         uart_transfert_length);
                perror("calloc()");
                goto err_restore_both;
        }
        dst_uart.data = calloc(uart_transfert_length, 1);
        if (!dst_uart.data)
        {
                tst_brkm(TBROK, NULL, "Unable to allocate %d bytes for receiving:",
                         uart_transfert_length);
                perror("calloc()");
                free(src_uart.data);
                goto err_restore_both;
        }

        tst_resm(TINFO, "Preparing %d bytes of random data...", uart_transfert_length);
        for (bufidx = 0; bufidx < uart_transfert_length; bufidx++)
        {
                src_uart.data[bufidx] =
                    (unsigned char) (random() * ((double) 128 / (double) RAND_MAX));
                /* In soft flow control mode, don't generate control cahracters, * => restrict to *
                * ascii printable characters: 31<X<127 */
                if (uart_flow_ctrl == FLOW_CTRL_SOFT)
                {
                        src_uart.data[bufidx] &= 0x7F;  /* <127 */
                        if (src_uart.data[bufidx] <= 31)
                                src_uart.data[bufidx] += 32;    /* >31 and <127 */
                }
        }

        /* All's good! */
        tst_resm(TINFO, "Setup success.");
        rv = TPASS;
        return rv;

        /* If there's some errors, exit properly. */
      err_restore_both:
        VT_cleanup_uart(&dst_uart);
      err_restore_src:
        VT_cleanup_uart(&src_uart);
      err_return:
        tst_resm(TFAIL, "Setup failure.");
        return rv;
}



/*================================================================================================*/
/*===== VT_mxc_uart_test4_cleanup =====*/
/**
@brief  assumes the post-condition of the test case execution

@param  None

@return On success - return TPASS
On failure - return the error code*/
/*================================================================================================*/
int VT_uart_test3_cleanup(void)
{
        int     rv = TPASS;

        free(src_uart.data);
        free(dst_uart.data);

        tst_resm(TINFO, "Cleaning up source UART...");
        if (VT_cleanup_uart(&src_uart) != TPASS)
        {
                rv = TFAIL;
        }

        tst_resm(TINFO, "Cleaning up destination UART...");
        if (VT_cleanup_uart(&dst_uart) != TPASS)
        {
                rv = TFAIL;
        }

        if (rv != TPASS)
        {
                tst_resm(TINFO, "Cleanup failure.");
        }
        else
        {
                tst_resm(TINFO, "Cleanup success.");
        }

        return rv;
}

/*================================================================================================*/
/*===== VT_mxc_uart_test4 =====*/
/**
@brief  mxc_uart test configures and sets UART driver before sending/receiving
characters on it

@param  Input :   int  baud rate
int  size_to_write
char UART_type
Output :  None

@return On success - return TPASS
On failure - return the error code*/
/*================================================================================================*/
int VT_uart_test3(int testcase)
{
        int     rv = TPASS;
        int     error;
        int     thret_writer,
                thret_reader;
        int     bufidx;
        int     nb_errors;
        int     nb_sec;


        tst_resm(TINFO, "Testing transfert from %s to %s:",
                 src_uart.device,
                 dst_uart.device,
                 uart_flow_ctrl == FLOW_CTRL_HARD ? "hardware" :
                 uart_flow_ctrl == FLOW_CTRL_SOFT ? "software" : "???");

        tst_resm(TINFO, "    => testcase %d.", testcase);

        /* 
        * Lock reader and writer mutex, so they will wait on it before they begin 
        * their real job.
        */
        pthread_mutex_lock(&read_mutex);
        pthread_mutex_lock(&write_mutex);

        /* 
        * Create the signal, reader and the writer threads
        */
        tst_resm(TINFO, "  Creating reader thread => [ R  ]...");
        error = pthread_create(&thid_reader, NULL, VT_th_reader, NULL);
        if (error < 0)
        {
                perror("pthread_create(reader)");
                rv = TFAIL;
                goto error_exit;
        }
        tst_resm(TINFO, "  Creating writer thread => [W   ]...");
        error = pthread_create(&thid_writer, NULL, VT_th_writer, NULL);
        if (error < 0)
        {
                perror("pthread_create(writer)");
                rv = TFAIL;
                goto error_exit_cancel_reader;
        }
        th_block_signals();
        tst_resm(TINFO, "  Creating signal thread => [   S]...");
        error = pthread_create(&thid_signal, NULL, VT_th_signal, NULL);
        if (error < 0)
        {
                perror("pthread_create(signal)");
                rv = TFAIL;
                goto error_exit_cancel_writer;
        }

        switch (testcase)
        {
        case TESTCASE_SIMPLE:
                /* 
                * "Signal" reader then writer thread
                */
                tst_resm(TINFO, "  Signalig reader and writer threads...");
                pthread_mutex_unlock(&read_mutex);
                pthread_mutex_unlock(&write_mutex);
                break;

        case TESTCASE_HW_FLOW:
  		tst_resm(TINFO, "  Signalig reader and writer threads...");
                pthread_mutex_unlock(&read_mutex);
                pthread_mutex_unlock(&write_mutex);
	#if 0		
                /* 
                * "Signal" the writer thread
                */
                tst_resm(TINFO, "  Signalig writer threads...");
                pthread_mutex_unlock(&write_mutex);

                /* 
                * Wait, to allow the writer to fill reader and writer buffers
                */
                nb_sec = 1 + uart_transfert_length / uart_baud_rate;
                tst_resm(TINFO, "  Waiting %d before signaling reader...", nb_sec);
                sleep(nb_sec);

                /* 
                * "Signal" the reader thread
                */
                pthread_mutex_unlock(&read_mutex);
	#endif		
                break;

        case TESTCASE_SW_FLOW:
                /* 
                * "Signal" the writer thread
                */
                tst_resm(TINFO, "  Signalig writer threads...");
		pthread_mutex_unlock(&read_mutex);
                pthread_mutex_unlock(&write_mutex);

	#if 0   /* 
                * Wait, to allow the writer to fill reader and writer buffers
                */
                nb_sec = 1 + uart_transfert_length / uart_baud_rate;
                tst_resm(TINFO, "  Waiting %d before signaling reader...", nb_sec);
                sleep(nb_sec);

                /* 
                * "Signal" the reader thread
                */
                pthread_mutex_unlock(&read_mutex);
                break;

        default:
                rv = TBROK;
	#endif		
                break;
        }


        /* 
        * wait for the reader and the writer threads,
        * and check their status
        */
        tst_resm(TINFO, "  Waiting for writer thread...");
        error = pthread_join(thid_writer, (void **) &thret_writer);
        if (error < 0)
        {
                perror("pthread_join(writer)");
                rv = TFAIL;
                goto error_exit_cancel_reader;
        }
        if (thret_writer != TPASS)
        {
                rv = thret_writer;
                tst_resm(TFAIL, "Writer has failed");
                goto error_exit_cancel_reader;
        }
        tst_resm(TINFO, "  Waiting for reader thread...");
        error = pthread_join(thid_reader, (void **) &thret_reader);
        if (error < 0)
        {
                perror("pthread_join(reader)");
                rv = TFAIL;
        }
        if (thret_reader != TPASS)
        {
                tst_resm(TFAIL, "Reader has failed");
                rv = thret_reader;
                goto error_exit;
        }

        /* 
        * Transfert is OK, now check for data errors
        */
        nb_errors = 0;
        tst_resm(TINFO, "  Comparing data...");
        for (bufidx = 0; bufidx < uart_transfert_length; bufidx++)
        {
                if (src_uart.data[bufidx] != dst_uart.data[bufidx])
                {
                        nb_errors++;
                        if (nb_errors < 20)
                        {
                                tst_resm(TFAIL,
                                         "Corruption found @ %d: send 0x%02X, receive 0x%02X",
                                         bufidx, src_uart.data[bufidx], dst_uart.data[bufidx]);
                                rv = TFAIL;
                        }
                }
        }
        tst_resm(TINFO, "    => %d error(s) found.", nb_errors);

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

        if (rv == TPASS && error != TPASS)
                rv = error;

        return rv;

        error_exit_cancel_writer:
        error = pthread_cancel(thid_writer);
        if (error < 0)
        {
                perror("pthread_cancel(writer)");
        }

        error_exit_cancel_reader:
        error = pthread_cancel(thid_reader);
        if (error < 0)
        {
                perror("pthread_cancel(reader)");
        }

        error_exit:
        VT_check_hardware(1, 1, 1, 1);
        return rv;
}


int VT_setup_uart(uart_config_t * cfg, uart_test_t * uart, char *desc, int open_flag)
{

        uart->desc = desc;
        uart->device = cfg->device;

        /* Fill in new attibutes: */
        tst_resm(TINFO, "  Building new attributes...");
        if (uart_fill_termios(cfg, &uart->termios) != 0)
        {
                tst_resm(TWARN, " 11 => Oups! Something goes wrong...");
                goto err_fail;
        }

        /* ignore parity and frame error */
        uart->termios.c_iflag |= INPCK | IGNPAR;

        /* CLOCAL: * If this bit is set, it indicates that the terminal is connected "locally" * and
        * * that the modem status lines (such as carrier detect) should be * ignored. * CREAD: * If * 
        * this bit is set, input can be read from the terminal. Otherwise, * input is discarded *
        * when it arrives. */
        uart->termios.c_cflag |= CLOCAL | CREAD;
          
        /* Open device twice: * - first with O_NONBLOCK in case CLOCAL is not set * - second open is
        * * the "normal" way */

        /* Open first time */
        tst_resm(TINFO, "  Opening device %s...", uart->device);
        uart->fd = open(uart->device, O_RDWR | O_NOCTTY | O_NONBLOCK);
        if (uart->fd < 0)
        {
                tst_resm(TWARN, "    => Oups! Something goes wrong");
                perror(uart->device);
                goto err_fail;
        }

        /* Flush it unconditionaly */
        tst_resm(TINFO, "  Flushing device %s...", uart->device);
        if (tcflush(uart->fd, TCIOFLUSH) != 0)
        {
                tst_resm(TWARN, "    => Failure.");
                perror(uart->device);
                goto err_close_uart;
        }

        /* Save attribute */
        tst_resm(TINFO, "  Saving attributes...");
        if (tcgetattr(uart->fd, &uart->termios_saved) != 0)
        {
                tst_resm(TWARN, "    => Oups! Something goes wrong");
                perror(uart->device);
                goto err_close_uart;
        }

        /* Set new attributes */
        tst_resm(TINFO, "  Setting new attributes...");
        if (tcsetattr(uart->fd, TCSANOW, &uart->termios) != 0)
        {
                tst_resm(TWARN, "    => Oups! Something goes wrong");
                perror(uart->device);
                goto err_restore_uart;
        }

        /* now, CLOCAL is set, re-open it without O_NONBLOCK */
        tst_resm(TINFO, "  Re-opening device %s...", uart->device);
        close(uart->fd);
        uart->fd = open(uart->device, O_RDWR | O_NOCTTY | open_flag);
        if (uart->fd < 0)
        {
                tst_resm(TWARN, "    => Oups! Something goes wrong");
                perror(uart->device);
                goto err_fail;
        }

        /* Recheck new attributes */
        tst_resm(TINFO, "  Checking new attributes...");
        if (tcgetattr(uart->fd, &uart->termios_checked) != 0)
        {
                tst_resm(TWARN, "    => Oups! Something goes wrong");
                perror(uart->device);
                goto err_restore_uart;
        }

        if (uart_cmp_termios_flags(&uart->termios, &uart->termios_checked) != 0)
        {
                tst_resm(TWARN, "    => Attributes not correctly set: ");
                uart_dump_termios(&uart->termios, "termios set via tcsetattr()");
                uart_dump_termios(&uart->termios_checked, "termios get via tcgetattr()");
                goto err_restore_uart;
        }

        /* Return TPASS on success */
        return TPASS;

        /* On error, do the job to exit the cleanest way */

        err_restore_uart:
        if (tcsetattr(uart->fd, TCSAFLUSH, &uart->termios_saved) != 0)
        {
                tst_resm(TWARN, "  Unable to restore attributes:");
                perror(uart->device);
        }

        err_close_uart:
        if (close(uart->fd))
        {
                tst_resm(TWARN, "  Unable to close device:");
                perror(uart->device);
        }

        err_fail:
        tst_resm(TFAIL, "Setup failure.");

        return TFAIL;

}

int VT_cleanup_uart(uart_test_t * uart)
{
        int     rv = TPASS;

        if (uart->fd > 0)
        {
                /* Flush it unconditionaly */
                if (tcflush(uart->fd, TCIOFLUSH) != 0)
                {
                        tst_resm(TWARN, "Unable to flush device");
                        perror(uart->device);
                        rv = TBROK;
                }

                if (tcsetattr(uart->fd, TCSANOW, &uart->termios_saved) != 0)
                {
                        tst_resm(TWARN, "Unable to restore UART attributes:");
                        perror(uart->device);
                        rv = TBROK;
                }

                if (close(uart->fd))
                {
                        tst_resm(TWARN, "Unable to close UART device:");
                        perror(uart->device);
                        rv = TBROK;
                }
        }

        return rv;

}

void   *VT_th_writer(void *param)
{
        int     rv = TFAIL;
        int     error,
                remaining;
        int     percent,
                last_percent = 0;

        th_block_signals();

        THW_INFO("Ready.");
        pthread_mutex_lock(&write_mutex);
        pthread_mutex_unlock(&write_mutex);



        THW_INFO("Writing data to source UART...");
        remaining = uart_transfert_length;
        do
        {
                error = write(src_uart.fd,
                              &src_uart.data[uart_transfert_length - remaining], remaining);
                if (error < 0)
                {
                        if (errno != EAGAIN)
                        {
                                THW_FAIL("Transfert failure after writing %d/%d bytes on %s",
                                         uart_transfert_length - remaining,
                                         uart_transfert_length, src_uart.device);
                                perror(src_uart.device);
                                rv = TFAIL;
                                goto th_exit;
                        }
                }
                else if (error != 0)
                {
                        pthread_mutex_lock(&write_mutex);
                        nb_write += error;
                        pthread_mutex_unlock(&write_mutex);
                        remaining -= error;
                        percent =
                            ((uart_transfert_length - remaining) * 100) / uart_transfert_length;
                        if (percent >= (last_percent + 10))
                        {
                                THW_INFO("%3d%% (%d/%d)...",
                                         percent,
                                         uart_transfert_length - remaining, uart_transfert_length);
                                last_percent = percent;
                        }
                }
        }
        while (remaining > 0);

        THW_INFO("Draining source UART...");
        error = tcdrain(src_uart.fd);
        if (error != 0)
        {
                THW_FAIL("tcdrain(src_uart) failed.");
                perror(src_uart.device);
                rv = TFAIL;
                goto th_exit;
        }

        rv = TPASS;

        th_exit:
        /* 
        * if (rv != TPASS) { error = pthread_cancel(thid_reader); if (error < 0) {
        * perror("pthread_cancel(reader)"); } } */
        THW_INFO("Exiting with return code = %d", rv);
        pthread_exit((void *) rv);
}

void   *VT_th_reader(void *param)
{
        int     rv = 0;
        int     error,
                remaining;
        int     xfer_errors = 0;

        /* int bufidx; */
        int     percent,
                last_percent = 0;

        th_block_signals();

        THR_INFO("Ready.");
        pthread_mutex_lock(&read_mutex);
        pthread_mutex_unlock(&read_mutex);

        THR_INFO("Reading data from destination UART...");

        remaining = uart_transfert_length;
        run_timer();
        do
        {
                error = read(dst_uart.fd,
                             &dst_uart.data[uart_transfert_length - remaining], remaining);
                if (error < 0)
                {
                        if (errno != EAGAIN)
                        {
                                THR_FAIL("Transfert failure after reading %d/%d bytes on %s:",
                                         uart_transfert_length - remaining,
                                         uart_transfert_length, dst_uart.device);
                                perror(dst_uart.device);
                                rv = TFAIL;
                                goto th_exit;
                        }
                }
                else if (error != 0)
                {
                        pthread_mutex_lock(&read_mutex);
                        nb_read += error;
                        pthread_mutex_unlock(&read_mutex);
                        remaining -= error;
                        percent =
                            ((uart_transfert_length - remaining) * 100) / uart_transfert_length;
                        if (percent >= (last_percent + 10))
                        {
                                THR_INFO("%3d%% (%d/%d)... (%d errors)",
                                         percent,
                                         uart_transfert_length - remaining,
                                         uart_transfert_length, xfer_errors);
                                last_percent = percent;
                        }
                }
                /* 
                * pthread_mutex_lock(&read_mutex); if (reader_sleep > 0) {
                * sleep(random_max(reader_sleep)); } pthread_mutex_unlock(&read_mutex); */
        }
        while (remaining > 0 && cout_timer_sign == 0);
        rv = TPASS;
	printf("Remaining bytes=%d\n", remaining);

        th_exit:

        if (rv != TPASS)
        {
                error = pthread_cancel(thid_writer);
                if (error < 0)
                {
                        perror("pthread_cancel(writer)");
                }
        }
        THR_INFO("Exiting with return code = %d", rv);
        pthread_exit((void *) rv);
}

void   *VT_th_signal(void *param)
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
                        error = pthread_cancel(thid_writer);
                        if (error < 0)
                                perror("pthread_cancel(writer)");
                        error = pthread_cancel(thid_reader);
                        if (error < 0)
                                perror("pthread_cancel(reader)");
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

int VT_check_hardware(int check_error, int check_break, int check_flow, int check_modem)
{
        int     rv = TPASS;

        tst_resm(TINFO, "  Checking harware behaviour...");

        if (ioctl(src_uart.fd, TIOCGICOUNT, (unsigned long) &src_uart.icounter) < 0)
        {
                tst_resm(TWARN, "    => ioctl(src, TIOCGICOUNT) failed.");
                perror(src_uart.device);
                return TBROK;
        }

        if (ioctl(dst_uart.fd, TIOCGICOUNT, (unsigned long) &dst_uart.icounter) < 0)
        {
                tst_resm(TWARN, "    => ioctl(dst, TIOCGICOUNT) failed.");
                perror(dst_uart.device);
                return TBROK;
        }

#define ICNT(who, what)  who##_uart.icounter.what - who##_uart.icounter_saved.what

        tst_resm(TINFO, "/==============================================\\");
        tst_resm(TINFO, "|         UART hardware summary                |");
        tst_resm(TINFO, "+============+================+================+");
        tst_resm(TINFO, "|            | %-14s | %-14s |", "SRC", "DST");
        tst_resm(TINFO, "+------------+----------------+----------------+");
        tst_resm(TINFO, "| device     | %-14s | %-14s |", src_uart.device, dst_uart.device);
        tst_resm(TINFO, "|#send bytes | %-14d | %-14d |", nb_write, 0);
        tst_resm(TINFO, "|#recv bytes | %-14d | %-14d |", 0, nb_read);
        tst_resm(TINFO, "|#cts        | %-14d | %-14d |", ICNT(src, cts), ICNT(dst, cts));
        tst_resm(TINFO, "|#dsr        | %-14d | %-14d |", ICNT(src, dsr), ICNT(dst, dsr));
        tst_resm(TINFO, "|#rng        | %-14d | %-14d |", ICNT(src, rng), ICNT(dst, rng));
        tst_resm(TINFO, "|#dcd        | %-14d | %-14d |", ICNT(src, dcd), ICNT(dst, dcd));
        tst_resm(TINFO, "|#rx         | %-14d | %-14d |", ICNT(src, rx), ICNT(dst, rx));
        tst_resm(TINFO, "|#tx         | %-14d | %-14d |", ICNT(src, tx), ICNT(dst, tx));
        tst_resm(TINFO, "|#frame err  | %-14d | %-14d |", ICNT(src, frame), ICNT(dst, frame));
        tst_resm(TINFO, "|#overrun    | %-14d | %-14d |", ICNT(src, overrun), ICNT(dst, overrun));
        tst_resm(TINFO, "|#parity err | %-14d | %-14d |", ICNT(src, parity), ICNT(dst, parity));
        tst_resm(TINFO, "|#buf overrun| %-14d | %-14d |", ICNT(src, buf_overrun),
                 ICNT(dst, buf_overrun));
        tst_resm(TINFO, "|#break      | %-14d | %-14d |", ICNT(src, brk), ICNT(dst, brk));
        tst_resm(TINFO, "\\------------+----------------+----------------/");

        if ((ICNT(src, tx) != uart_transfert_length) || (ICNT(dst, rx) != uart_transfert_length))
        {
                tst_resm(TFAIL, "  => Problem on RX/TX counters.");
                rv = TBROK;
        }

        if (check_error)
        {
                if ((ICNT(src, frame) > 0) ||
                    (ICNT(dst, frame) > 0) ||
                    (ICNT(src, overrun) > 0) ||
                    (ICNT(dst, overrun) > 0) ||
                    (ICNT(src, parity) > 0) ||
                    (ICNT(dst, parity) > 0) ||
                    (ICNT(src, buf_overrun) > 0) || (ICNT(dst, buf_overrun) > 0))
                {
                        tst_resm(TFAIL, "  => There are some hardware error(s).");
                        rv = TFAIL;
                }
        }

        if (check_break)
        {
                if ((ICNT(src, brk) > 0) || (ICNT(dst, brk) > 0))
                {
                        tst_resm(TFAIL, "  => Error(s): break found.");
                        rv = TFAIL;
                }
        }

        if (check_flow)
        {
/* These counts cannot be maintained by the driver as we are using hardware driven hardware flow control (i.e: the hardware controls the RTS and CTS lines)*/
#if 0		
                if (uart_flow_ctrl == FLOW_CTRL_HARD)
                {
                        if (ICNT(src, cts) == 0)
                        {
                                tst_resm(TWARN, "  => No hardware flow control activation.");
                                /* the next line is commented out because the feature is not supported */
/*                                
                                rv = TFAIL;
*/				
                        }
                }
#endif
                if (uart_flow_ctrl == FLOW_CTRL_SOFT)
                {
                        if (ICNT(src, tx) == 0 || ICNT(dst, rx) == 0)
                        {
                                tst_resm(TFAIL, "  => No software flow control activation.");
                                rv = TFAIL;
                        }
                        if (ICNT(src, tx) != ICNT(dst, rx))
                        {
                                tst_resm(TFAIL, "  => Software flow control activation problem.");
                                rv = TFAIL;
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


/*================================================================================================*/
/*===== run_timer_for_reader =====*/
/** 
@brief   

@param  None 
    
@return On success - return TPASS 
        On failure - return the error code */
/*================================================================================================*/
int run_timer(void)
{
        struct sigaction sa;
        struct itimerval timer;
        long int msec;

        /* Install the timer handler... */
        memset(&sa, 0, sizeof(sa));
        sa.sa_handler = &timer_handler;

        sigaction(SIGALRM, &sa, NULL);

        /* 
        * Configure the timer to expire. 
        * We are quit large on value since we have to take into account 
        * some "random" overhead
        */

        /* Worst case: 1 start bit + 8 data bits + 2 stop bits */
        /* We finally add 100% for overhead */
        msec =
            (long
             int) ((float) ((uart_transfert_length * (1 + 8 + 2) * 1000) / (float) uart_baud_rate) *
                   2);
        timer.it_value.tv_sec = msec / 1000;
        timer.it_value.tv_usec = (msec - (timer.it_value.tv_sec * 1000)) * 1000;
        timer.it_interval.tv_sec = timer.it_value.tv_sec;
        timer.it_interval.tv_usec = timer.it_value.tv_usec;
        tst_resm(TINFO, "  [   T] Setting timeout to %ld.%06ld s",
                 timer.it_value.tv_sec, timer.it_value.tv_usec);

        /* Start timer...  */
        setitimer(ITIMER_REAL, &timer, NULL);


        return TPASS;
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
        int     error;

        tst_resm(TINFO, "  [   T] Timeout expired!");
        error = pthread_cancel(thid_reader);
        if (error < 0)
                perror("pthread_cancel(reader)");
        error = pthread_cancel(thid_writer);
        if (error < 0)
                perror("pthread_cancel(writer)");
        cout_timer_sign++;

}



#ifdef __cplusplus
} 
#endif
