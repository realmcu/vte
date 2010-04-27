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
    @file   uart_test_5.c

    @brief Tests the ability of MXC and External drivers to manage flow control
====================================================================================================
Revision History:
                            Modification     Tracking
Author                          Date          Number    Description of Changes
-------------------------   ------------    ----------  -------------------------------------------
I. Inkina / nknl001       07/04/2005     TLSbo48749   Initial version
I. Inkina / nknl001       07/04/2005     TLSbo49644    minor fix
I. Inkina / nknl001       07/04/2005     TLSbo52650    update code
E.Gromazina               03/10/2005     TLSbo52626   verification of invariance of the UART interrupts, base address and register offsets
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
#include <errno.h>

/* Harness Specific Include Files. */
#include "test.h"

/* Verification Test Environment Include Files */
#include "uart_test_5.h"

 #include <linux/capability.h>
 #include <linux/types.h>
/*============================================================================
                                        LOCAL MACROS
==================================================================================================*/

#define THS_INFO(fmt, args...) tst_resm(TINFO, "  [   S] " fmt, ##args)

/*==================================================================================================
                          LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
==================================================================================================*/


/*==================================================================================================
                                       LOCAL CONSTANTS
==================================================================================================*/

/*==================================================================================================
                                       LOCAL VARIABLES
==================================================================================================*/

/*==================================================================================================
                                       GLOBAL CONSTANTS
==================================================================================================*/


/*==================================================================================================
                                       GLOBAL VARIABLES
==================================================================================================*/
static param_drv  driver_thread[DRIVER_THREAD];

/*==================================================================================================
                                   LOCAL FUNCTION PROTOTYPES
==================================================================================================*/


/*==================================================================================================
                                       LOCAL FUNCTIONS
==================================================================================================*/
int uart_close(void* ptr);
int uart_open(void *ptr);
int config_test(void *ptr);
void print_uart_config(param_drv * uart_conf, int fl);

/*================================================================================================*/
/*===== VT_mxc_uart_test5 _setup =====*/
/**
@brief  assumes the pre-condition of the test case execution

@param  None
@return On success - return TPASS
        On failure - return the error code
*/
/*================================================================================================*/

int VT_mxc_uart_test5_setup(void)
{

        int rv = TFAIL;

        rv = TPASS;
        return rv;
}

/*================================================================================================*/
/*===== VT_mxc_uart_test5 _cleanup =====*/
/**
@brief  assumes the post-condition of the test case execution

@param  None

@return On success - return TPASS
        On failure - return the error code
*/
/*================================================================================================*/
int VT_mxc_uart_test5_cleanup(void)
{
        int rv = TFAIL;
        rv = TPASS;
        param_drv *driver_r = &driver_thread[0];

        if(driver_r->file_desc)
        {
                rv = uart_close(driver_r);
                if(rv== TFAIL)  return rv;
        }
        return rv;
}

/*================================================================================================*/
/*=====uart_open =====*/
/**
@brief  open UART

@param    Input :  driver_ptr
          Output :   None.

@return On success - return TPASS
        On failure - return the error code
*/
/*================================================================================================*/
int uart_open(void *ptr )
{
        int rv = TFAIL;
        param_drv *uart_driver=( param_drv*)ptr;

        /* Open MXC UART1 driver which is used for test purposes */

        uart_driver->file_desc   = open(uart_driver->driver_name, O_RDWR | O_NONBLOCK);

        sleep(1);

        if ( uart_driver->file_desc == -1)
        {
                tst_resm(TFAIL, "ERROR : Open UART driver fails : %d", errno);
        }
        else
                rv = TPASS;

        return rv;
}
/*================================================================================================*/
/*===== th_block_signals =====*/
/**
@brief  set up signals

@param    Input :
          Output :

@return On success -
        On failure -
*/
/*================================================================================================*/
void th_block_signals(void)
{
        sigset_t mask;

        sigemptyset(&mask);
        sigaddset(&mask, SIGINT);
        pthread_sigmask(SIG_BLOCK, &mask, NULL);
}


/*================================================================================================*/
void* VT_th_signal(void *param)
{
        sigset_t mask;
        int signal;
        int rv;

        sigemptyset(&mask);
        sigaddset(&mask, SIGINT);
        while(1)
        {
                sigwait(&mask, &signal);
                THS_INFO("Got signal \"%s\"...", sys_siglist[signal]);
                switch (signal)
                {
                case SIGINT:
                        if( pthread_cancel(driver_thread[0].tid))
                                tst_resm(TFAIL ,"pthread_cancel(reader)");
                        rv = TBROK;
                        pthread_exit(&rv);
                        break;
                default:
                        THS_INFO("Ignoring signal \"%s\"", sys_siglist[signal]);
                }
        }
}

/*================================================================================================*/
/*===== VT_mxc_uart_test5 =====*/
/**
@brief  mxc_uart get and set UART parameters

@param  Input :  the driver name for configuring
        Output : None .

@return On success - return TPASS
        On failure - return the error code
*/
/*================================================================================================*/
int VT_mxc_uart_test5(char *driver_name_1, int type_1 )

{
        int rv = TPASS;

        pthread_t thid_signal;
        driver_thread[0].driver_name=driver_name_1;
        driver_thread[0].type = type_1;

        th_block_signals();

        if( pthread_create(&thid_signal, NULL, VT_th_signal, NULL))
        {
                perror("pthread_create(signal)");
                rv = TFAIL;
                return rv;
        }

        /* Open  UART driver which is used for test purposes */
        /* VTE : Execute test, print results and exit test scenario */

        if (pthread_create
            (&driver_thread[0].tid, NULL, (void *) &config_test, (void *) &driver_thread[0]))
        {
                tst_resm(TFAIL, "ERROR: cannot create thread " );
                rv = TFAIL;
        }

        /* Wait finished thread*/
        if(pthread_join(driver_thread[0].tid, (void*) &rv ))
        {
                rv=TFAIL;
                tst_resm(TINFO, "Thread Fail");
        }

        if(driver_thread[0].file_desc)
        {
                if( uart_close(&driver_thread[0]) == TFAIL)
                        rv=TFAIL;
        }

        return rv;

}

/*================================================================================================*/
/*===== config_test =====*/
/**
@brief  mxc_uart get and set UART parameters

@param  Input :  the driver name for configuring
        Output : None .

@return On success - return TPASS
        On failure - return the error code
*/
/*================================================================================================*/

int config_test(void *ptr)
{
        int     rv = TPASS;
        int     ret;
	//pid_t pid;
	//cap_t cap;

        // unsigned int line_value = 0;

        param_drv *uart_driver = (param_drv *) ptr;


        /* ioctls to be tested : TIOCGSERIAL : Get serial UART configuration TIOCSSERIAL : Set serial
        * UART parameters */

        //uart_driver->file_desc = open(uart_driver->driver_name, O_RDWR | O_NONBLOCK);
        uart_driver->file_desc = open(uart_driver->driver_name, O_RDWR);

        sleep(1);
        if (uart_driver->file_desc == -1)
        {
                tst_resm(TFAIL, "ERROR : Open UART driver fails : %d", errno);
                rv = TFAIL;
                return rv;
        }

        if (ioctl(uart_driver->file_desc, TIOCGSERIAL, &uart_driver->tty_mxc_serial_old))
        {
                tst_resm(TFAIL, "ERROR : ioctl TIOCGSERIAL fails  : %d", errno);
                rv = TFAIL;
                return rv;
        }

        print_uart_config(uart_driver, 1);

        uart_driver->tty_mxc_serial = uart_driver->tty_mxc_serial_old;

        /*uart_driver->tty_mxc_serial.type = 52; */
        uart_driver->tty_mxc_serial.baud_base = 115200;
        uart_driver->tty_mxc_serial.xmit_fifo_size = 16;
	/*uart_driver->tty_mxc_serial.custom_divisor = 1;*/
	/*custom_divisor cannot be changed */
	uart_driver->tty_mxc_serial.close_delay = 10;


        // Set MXC Tortolla UART port configuration with tty_mxc_serial structure
        ret = ioctl(uart_driver->file_desc, TIOCSSERIAL, &uart_driver->tty_mxc_serial);
        if (ret < 0)
        {
                tst_resm(TFAIL, "ERROR : ioctl TIOCSSERIAL fails  : %d", errno);
                rv = TFAIL;
        }

        // Get new MXC UART port configuration
sleep(3);
        ret = ioctl(uart_driver->file_desc, TIOCGSERIAL, &uart_driver->tty_mxc_serial);
        if (ret < 0)
        {
                tst_resm(TFAIL, "ERROR : ioctl TIOCGSERIAL fails  : %d", errno);
                rv = TFAIL;
                goto mm;
        }

        print_uart_config(uart_driver, 0);

        if (uart_driver->tty_mxc_serial.irq != uart_driver->tty_mxc_serial_old.irq ||
            uart_driver->tty_mxc_serial.iomem_base != uart_driver->tty_mxc_serial_old.iomem_base ||
            uart_driver->tty_mxc_serial.iomap_base != uart_driver->tty_mxc_serial_old.iomap_base ||
            uart_driver->tty_mxc_serial.iomem_reg_shift !=
            uart_driver->tty_mxc_serial_old.iomem_reg_shift
            || uart_driver->tty_mxc_serial.port != uart_driver->tty_mxc_serial_old.port
            || uart_driver->tty_mxc_serial.port_high != uart_driver->tty_mxc_serial_old.port_high)
        {
                tst_resm(TFAIL,
                         "ERROR : values of irq or iomem_base or iomap_base or iomem_reg_shift or port or port_high are modified! ");
                rv = TFAIL;
        }


        if (uart_driver->tty_mxc_serial.xmit_fifo_size != 16
        /*    || uart_driver->tty_mxc_serial.custom_divisor != 1*/
            || uart_driver->tty_mxc_serial.baud_base != 115200
            || uart_driver->tty_mxc_serial.close_delay != 10)
        {
               tst_resm(TFAIL, "ERROR : Serial UART parameters not the same as configured");
                tst_resm(TINFO, "Check your configuration");
                rv = TFAIL;
                goto mm;
        }

mm:

uart_driver->tty_mxc_serial_old.baud_base = 115200;
ret = ioctl(uart_driver->file_desc, TIOCSSERIAL, &uart_driver->tty_mxc_serial_old);
        if (ret < 0)
        {
                tst_resm(TFAIL, "ERROR : ioctl TIOCSSERIAL fails  : %d", errno);
                rv = TFAIL;
printf("fail\n");
                //return rv;
        }
        ret = ioctl(uart_driver->file_desc, TIOCGSERIAL, &uart_driver->tty_mxc_serial_old);
        if (ret < 0)
        {
                tst_resm(TFAIL, "ERROR : ioctl TIOCGSERIAL fails  : %d", errno);
                rv = TFAIL;
        }



        return rv;

}

/*================================================================================================*/
/*===== mxc_uart_close =====*/
/**
@brief  close UART

@param  None

@return On success - return TPASS
        On failure - return the error code
*/
/*================================================================================================*/
int uart_close(void *ptr)
{
        int rv = TFAIL;
        int ret = 0;
        param_drv *uart_driver=( param_drv*)ptr;


        /* Close MXC UART driver */
        tst_resm(TINFO, "Close UART driver");
        if(uart_driver->file_desc > 0) { ret = close(uart_driver->file_desc);uart_driver->file_desc = -1 ;}

        if (ret == -1)
        {
                tst_resm(TFAIL, "ERROR : Close MXC UART driver fails  : %d", errno);
        }
        else
        {
                rv = TPASS;
        }

        return rv;
}

void print_uart_config(param_drv * uart_conf, int fl)
{
        if (fl)
        {
                tst_resm(TINFO, "/=================================\\");
                tst_resm(TINFO, "|       UART configure old         |");
                tst_resm(TINFO, "+============+====================+");
                tst_resm(TINFO, "| device            | %-10s |", uart_conf->driver_name);
                tst_resm(TINFO, "+------------+--------------------+");
                tst_resm(TINFO, "|  baud_base        | %-10d |",
                         uart_conf->tty_mxc_serial_old.baud_base);
                // tst_resm(TINFO, "| port type | %-10d |", uart_conf->tty_mxc_serial_old.type);
                tst_resm(TINFO, "|  xmit_fifo_size   | %-10d |",
                         uart_conf->tty_mxc_serial_old.xmit_fifo_size);
                tst_resm(TINFO, "|  custom_divisor   | %-10d |",
                         uart_conf->tty_mxc_serial_old.custom_divisor);
                tst_resm(TINFO, "|  close_delay      | %-10d |",
                         uart_conf->tty_mxc_serial_old.close_delay);
                tst_resm(TINFO, "\\------------+-------------------/");
                tst_resm(TINFO, "|  port type        | %-10d |",
                         uart_conf->tty_mxc_serial_old.type);
                tst_resm(TINFO, "|  irq port         | %-10d |", uart_conf->tty_mxc_serial_old.irq);
                tst_resm(TINFO, "|  iomem_base       | %-10d |",
                         uart_conf->tty_mxc_serial_old.iomem_base);
                tst_resm(TINFO, "|  iomap_base       | %-10lu |",
                         uart_conf->tty_mxc_serial_old.iomap_base);
                tst_resm(TINFO, "|  iomem_reg_shift  | %-10d |",
                         uart_conf->tty_mxc_serial_old.iomem_reg_shift);
                tst_resm(TINFO, "|  port             | %-10u |",
                         uart_conf->tty_mxc_serial_old.port);
                tst_resm(TINFO, "|  port_high        | %-10d |",
                         uart_conf->tty_mxc_serial_old.port_high);
                tst_resm(TINFO, "\\------------+-------------------/");
        }
        else
        {
                tst_resm(TINFO, "/=================================\\");
                tst_resm(TINFO, "|       UART configure new        |");
                tst_resm(TINFO, "+============+====================+");
                tst_resm(TINFO, "| device            | %-10s |", uart_conf->driver_name);
                tst_resm(TINFO, "+------------+--------------------+");
                tst_resm(TINFO, "|  baud_base        | %-10d |",
                         uart_conf->tty_mxc_serial.baud_base);
                // tst_resm(TINFO, "| port type | %-10d |", uart_conf->tty_mxc_serial.type);
                tst_resm(TINFO, "|  xmit_fifo_size   | %-10d |",
                         uart_conf->tty_mxc_serial.xmit_fifo_size);
                tst_resm(TINFO, "|  custom_divisor   | %-10d |",
                         uart_conf->tty_mxc_serial.custom_divisor);
                tst_resm(TINFO, "|  close_delay      | %-10d |",
                         uart_conf->tty_mxc_serial.close_delay);
                tst_resm(TINFO, "\\------------+-------------------/");
                tst_resm(TINFO, "|  port type        | %-10d |", uart_conf->tty_mxc_serial.type);
                tst_resm(TINFO, "|  irq port         | %-10d |", uart_conf->tty_mxc_serial.irq);
                tst_resm(TINFO, "|  iomem_base       | %-10d |",
                         uart_conf->tty_mxc_serial.iomem_base);
                tst_resm(TINFO, "|  iomap_base       | %-10lu |",
                         uart_conf->tty_mxc_serial.iomap_base);
                tst_resm(TINFO, "|  iomem_reg_shift  | %-10d |",
                         uart_conf->tty_mxc_serial.iomem_reg_shift);
                tst_resm(TINFO, "|  port             | %-10u |", uart_conf->tty_mxc_serial.port);
                tst_resm(TINFO, "|  port_high        | %-10d |",
                         uart_conf->tty_mxc_serial.port_high);
                tst_resm(TINFO, "\\------------+-------------------/");
        }

}

#ifdef __cplusplus
}
#endif
