/*================================================================================================*/
/**
    @file   uart_test_4.c

    @brief Tests the ability of MXC and External drivers to manage flow control
*/
/*==================================================================================================

Copyright (C) 2004, Freescale Semiconductor, Inc. All Rights Reserved
THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
Freescale Semiconductor, Inc.
     
====================================================================================================
Revision History:
                            Modification     Tracking
Author                          Date          Number    Description of Changes
-------------------------   ------------    ----------  -------------------------------------------
I. Inkina / nknl001         07/04/2005     TLSbo49644   minor fix 
I. Inkina / nknl001         07/04/2005     TLSbo51148   update options 
I. Inkina / nknl001         22/07/2005     TLSbo52650   ending result of the test was fixed

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
#include "uart_test_4.h"

/*============================================================================
                                        LOCAL MACROS
==================================================================================================*/


/*==================================================================================================
                          LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
==================================================================================================*/
#define       MAX_BUFFER_SIZE    10
#define THS_INFO(fmt, args...) tst_resm(TINFO, "  [   S] " fmt, ##args)

/*==================================================================================================
                                       LOCAL CONSTANTS
==================================================================================================*/

/*==================================================================================================
                                       LOCAL VARIABLES
==================================================================================================*/
static param_drv  driver_thread[DRIVER_THREAD];
int ret_s=TPASS; 
/*==================================================================================================
                                       GLOBAL CONSTANTS
==================================================================================================*/


/*==================================================================================================
                                       GLOBAL VARIABLES
==================================================================================================*/

/*==================================================================================================
                                   LOCAL FUNCTION PROTOTYPES
==================================================================================================*/


/*==================================================================================================
                                       LOCAL FUNCTIONS
==================================================================================================*/
int set_param(void *ptr);
int mxc_uart_close(void *ptr);
int test_activate_modem_signals(void *ptr);
int test_different_block_mode_read_2(void* ptr);
int test_different_block_mode_read(void* ptr);
int test_different_block_mode_write(void* ptr);
int test_different_block_mode_read_a(void* ptr);

void* VT_th_signal(void *param);
void th_block_signals(void);

/*================================================================================================*/
/*===== VT_mxc_uart_test4 _setup =====*/
/**
@brief  assumes the pre-condition of the test case execution

@param    Input :  char UART_type
              Output :   None.

@return On success - return TPASS
        On failure - return the error code
*/
/*================================================================================================*/

int VT_mxc_uart_test4_setup(void)
{
        int rv = TFAIL;
        
        rv = TPASS;
        return rv;
}

/*================================================================================================*/
/*===== VT_mxc_uart_test4 _cleanup =====*/
/**
@brief  assumes the post-condition of the test case execution

@param  None

@return On success - return TPASS
        On failure - return the error code
*/
/*================================================================================================*/
int VT_mxc_uart_test4_cleanup(void)
{
        int rv = TFAIL;
        
        param_drv *driver_r = &driver_thread[0];
        param_drv *driver_w = &driver_thread[1];
        
        if(driver_w->file_desc)
        {
                rv = mxc_uart_close(driver_w);
                if(rv== TFAIL)  return rv;
        }
        
        if(driver_r->file_desc)
        {
                rv = mxc_uart_close(driver_r);
                if(rv== TFAIL)  return rv;
        }
        
        rv=TPASS;
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
/*===== VT_th_signal =====*/
/** 
@brief  get up signal
 
@param    Input :    
              Output : 
 
@return On success - 
        On failure - 
*/
/*================================================================================================*/
void* VT_th_signal(void *param)
{
        sigset_t mask;
        int signal;
        int error;
        int rv;
        
        sigemptyset(&mask);
        sigaddset(&mask, SIGINT);
        while(1)
        {
                sigwait(&mask, &signal);
                switch (signal)
                {
                case SIGINT:
                        error = pthread_cancel(driver_thread[1].tid);
                        if (error < 0)
                                tst_resm( TFAIL , "pthread_cancel(writer)");
                        error = pthread_cancel(driver_thread[0].tid);
                        if (error < 0)
                                tst_resm( TFAIL , "pthread_cancel(reader)");
                        rv = TBROK;
                        ret_s=TFAIL; 
                        pthread_exit(&rv);
                        break;
                default:
                        THS_INFO("Ignoring signal \"%s\"", sys_siglist[signal]);
                }
        }
}

/*================================================================================================*/
/*===== VT_mxc_uart_test4 =====*/
/**
@brief    test scenario of the  UART  functions 

@param  Input :
             Output :  .

@return On success - return TPASS
        On failure - return the error code
*/
/*================================================================================================*/
int VT_mxc_uart_test4(char* driver_name_1,char* driver_name_2,int testcase )
{
        
        int rv = TPASS;
        
        
        
        driver_thread[0].driver_name=driver_name_1;
        driver_thread[0].mode = O_RDWR | O_NONBLOCK;
        driver_thread[1].driver_name=driver_name_2;
        driver_thread[1].mode = O_RDWR;
        pthread_t thid_signal;
        
        th_block_signals();
        if( pthread_create(&thid_signal, NULL, VT_th_signal, NULL))
        {
                perror("pthread_create(signal)"); rv=TFAIL;
                return rv;
        }
        
        
        
        switch(testcase)
        {
        case  SET_LOCAL:
                
                if( pthread_create( &driver_thread[0].tid, NULL,(void *)&test_different_block_mode_read,
                        (void *) &driver_thread[0] ))
                {
                        tst_resm(TFAIL, "ERROR: cannot create thread " );
                        rv = TFAIL;
                }
                
                if (pthread_join(driver_thread[0].tid, (void*) &rv ))
                {
                        rv=TFAIL;
                        tst_resm(TINFO, "Thread 0 Fail");
                }
                
                
                driver_thread[0].mode = O_RDWR;
                
                if( pthread_create( &driver_thread[0].tid, NULL,(void *)&test_different_block_mode_read_2,
                        (void *) &driver_thread[0] ))
                {
                        tst_resm(TFAIL, "ERROR: cannot create thread " );
                        rv = TFAIL;
                }
                
                if( pthread_create( &driver_thread[1].tid, NULL,(void *)&test_different_block_mode_write,
                        (void *) &driver_thread[1] ))
                {
                        tst_resm(TFAIL, "ERROR: cannot create thread " );
                        rv = TFAIL;
                }
                
                if (pthread_join(driver_thread[0].tid, (void*) &rv ))
                {
                        rv=TFAIL;
                        tst_resm(TINFO, "Thread 0 Fail");
                }
                
                if (pthread_join(driver_thread[1].tid, (void*) &rv ))
                {
                        rv=TFAIL;
                        tst_resm(TINFO, "Thread 1 Fail");
                }
                
                break;
                
                
        case  SET_ACTIVE_SIGNAL:
                
                
                if( pthread_create( &driver_thread[0].tid, NULL,(void *)&test_different_block_mode_read_a,
                        (void *) &driver_thread[0] ))
                {
                        tst_resm(TFAIL, "ERROR: cannot create thread " );
                        rv = TFAIL;
                }
                
                if( pthread_create( &driver_thread[1].tid, NULL,(void *)&test_activate_modem_signals,
                        (void *) &driver_thread[1] ))
                {
                        tst_resm(TFAIL, "ERROR: cannot create thread " );
                        rv = TFAIL;
                }
                
                if (pthread_join(driver_thread[0].tid, (void*) &rv ))
                {
                        rv=TFAIL;
                        tst_resm(TINFO, "Thread 0 Fail");
                }
                
                if (pthread_join(driver_thread[1].tid, (void*) &rv ))
                {
                        rv=TFAIL;
                        tst_resm(TINFO, "Thread 1 Fail");
                }
                
                break;
                
        default:
                
                tst_resm(TINFO, " wrong  test case");
                
                break;
        }
        
        if(ret_s!=TPASS)rv=TFAIL; 
        
        /*    if(driver_thread[0].file_desc)
        {
        if(mxc_uart_close(&driver_thread[0]) == TFAIL) 
        rv=TFAIL; return rv;
        }
        
        if(driver_thread[1].file_desc)
        {
        if( mxc_uart_close(&driver_thread[1]) == TFAIL)
        rv=TFAIL; return rv;
        }
        */         
        return rv;
        
}
/*================================================================================================*/
/*===== test_different_block_mode_write =====*/
/**
@brief  open writer , set up TIOCMBIS and close writer

@param   struct param_drv

@return On success - return TPASS
        On failure - return the error code
*/
/*================================================================================================*/
int test_different_block_mode_write(void* ptr)
{
        int rv = TFAIL;
        unsigned int line_value = 0;
        /*  int j=0 , i=0;*/
        
        param_drv *driver_w = (param_drv*)ptr;
        
        
        if((driver_w->file_desc = open ( driver_w->driver_name, driver_w->mode ))<0)
        {
                tst_resm(TFAIL, "ERROR :Driver open  fails %d ", errno);
                return rv;
        }
        sleep(1);
        
        if( tcflush(driver_w->file_desc, TCIFLUSH)!=0)
        {
                tst_resm(TFAIL, "ERROR : Flush driver fails %d ", errno);
                return rv;
                
        }
        
        if(tcgetattr(driver_w->file_desc, &driver_w->termios_mxc_old)!=0)
        {
                tst_resm(TWARN, "Can not get atributs driver fails %d ", errno);
                return rv;
                
        }
        
        set_param( driver_w);
        
        line_value = RTS_AND_DTR;
        
        if(( ioctl ( driver_w->file_desc, TIOCMBIS, &line_value))<0)
        {
                tst_resm(TFAIL, "ERROR : ioctl TIOCMBIS fails  : %d", errno);
                return rv;
        }
        
        rv = TPASS;
        return rv;
        
}
/*================================================================================================*/
/*===== test_different_block_mode_read =====*/
/**
@brief   open reader  and set up CLOCAL

@param   struct param_drv

@return On success - return TPASS
        On failure - return the error code
*/
/*================================================================================================*/
int test_different_block_mode_read(void* ptr)
{
        int rv = TFAIL;
        /*  unsigned int line_value = 0;
        char read_buf[MAX_BUFFER_SIZE];*/
        
        param_drv *driver_r = (param_drv*)ptr;
        driver_r->termios_mxc.c_cflag |= ( B57600 | CS8 | CLOCAL | CREAD);
        
        
        if((driver_r->file_desc = open(driver_r->driver_name, driver_r->mode))<0)
        {
                tst_resm(TFAIL, "ERROR : Open  driver fails %d", errno);
                return rv;
        }
        
        sleep(1);
        
        if( tcflush(driver_r->file_desc, TCIFLUSH)!=0)
        {
                tst_resm(TFAIL, "ERROR : Flush driver fails %d ", errno);
                return rv;
                
        }
        
        if( tcgetattr(driver_r->file_desc, &driver_r->termios_mxc_old)!=0)
        {
                tst_resm(TWARN, "Can not get atributs driver fails %d ", errno);
                return rv;
                
        }
        
        
        if(( tcsetattr(driver_r->file_desc, TCSANOW, &driver_r->termios_mxc)) <0)
        {
                tst_resm(TFAIL, "ERROR : Fail to set  control parameters (termios) : %d", errno);
                return rv;
        }
        
        
        
        if((rv=mxc_uart_close(driver_r)) == TFAIL)  return rv;
        
        rv=TPASS;
        return rv;
}

/*================================================================================================*/
/*===== test_different_block_mode =====*/
/**
@brief  reopen, set up TIOCMWAIT

@param   struct param_drv

@return On success - return TPASS
        On failure - return the error code
*/
/*================================================================================================*/

int test_different_block_mode_read_2(void* ptr)
{
        int rv = TFAIL;
        unsigned int line_value = 0;
        unsigned int Serial_input_value = 0;
        /*  char read_buf[MAX_BUFFER_SIZE];*/
        
        param_drv *driver_r = (param_drv*)ptr;
        
        
        
        if((driver_r->file_desc = open(driver_r->driver_name, driver_r->mode))<0)
        {
                tst_resm(TFAIL, "ERROR : Open driver fails %d\n", errno);
                return rv;
        }
        
        tcflush(driver_r->file_desc, TCIFLUSH);
        
        
        /* Wait for any of the 4 serial inputs (DCD,RI,DSR,CTS) to change */
        Serial_input_value = TIOCM_RNG | TIOCM_DSR | TIOCM_CD | TIOCM_CTS;
        /* Loop until status of DCD, CTS, DSR or RI changes */
        do
        {
                tst_resm(TINFO," ioctl TIOCMIWAIT\n");
                
                /* Wait for a DSR signal status change */
                if(( ioctl(driver_r->file_desc, TIOCMIWAIT, TIOCM_DSR))<0)
                {
                        tst_resm(TFAIL, "ERROR : ioctl TIOCMIWAIT fails");
                        return rv;
                }
                /* Return current module control and status signals */
                
                ioctl(driver_r->file_desc, TIOCMGET, &line_value);
                tcflush(driver_r->file_desc, TCIFLUSH);
                
        }  
        while (!(line_value & Serial_input_value));
        
        
        rv = TPASS;
        return rv;
}


int test_different_block_mode_read_a(void* ptr)
{
        int rv = TFAIL;
        unsigned int line_value = 0;
        unsigned int Serial_input_value = 0;
        /*  char read_buf[MAX_BUFFER_SIZE];*/
        
        param_drv *driver_r = (param_drv*)ptr;
        
        driver_r->termios_mxc.c_cflag |= ( B57600 | CS8 | CLOCAL | CREAD);
        
        
        if((driver_r->file_desc = open(driver_r->driver_name, driver_r->mode))<0)
        {
                tst_resm(TFAIL, "ERROR : Open driver fails %d", errno);
                return rv;
        }
        sleep(1);
        
        
        if( tcflush(driver_r->file_desc, TCIFLUSH)!=0)
        {
                tst_resm(TFAIL, "ERROR : Flush driver fails %d", errno);
                return rv;
                
        }
        
        if( tcgetattr(driver_r->file_desc, &driver_r->termios_mxc_old)!=0)
        {
                tst_resm(TWARN, "Can not get atributs driver fails %d", errno);
                return rv;
                
        }
        
        
        if(( tcsetattr(driver_r->file_desc, TCSANOW, &driver_r->termios_mxc)) <0)
        {
                tst_resm(TFAIL, "ERROR : Fail to set control parameters (termios) : %d", errno);
                return rv;
        }
        
        
        
        /* Wait for any of the 4 serial inputs (DCD,RI,DSR,CTS) to change */
        Serial_input_value = TIOCM_RNG | TIOCM_DSR | TIOCM_CD | TIOCM_CTS;
        /* Loop until status of DCD, CTS, DSR or RI changes */
        do
        {
                tst_resm(TINFO," ioctl TIOCMIWAIT\n");
                
                /* Wait for a DSR signal status change */
                if(( ioctl(driver_r->file_desc, TIOCMIWAIT, TIOCM_DSR))<0)
                {
                        tst_resm(TFAIL, "ERROR : ioctl TIOCMIWAIT fails");
                        return rv;
                }
                
                ioctl(driver_r->file_desc, TIOCMGET, &line_value);
                tcflush(driver_r->file_desc, TCIFLUSH);
                
        }
        while (!(line_value & Serial_input_value));
        
        
        rv = TPASS;
        return rv;
}


/*================================================================================================*/
/*===== test_activate_modem_signals =====*/
/**
@brief    open writer, set   activate signals of the input

@param   struct_drv

@return On success - return TPASS
        On failure - return the error code
*/
/*================================================================================================*/
int test_activate_modem_signals(void *ptr)
{
        int rv = TFAIL;
        unsigned int line_value = 0;
        
        param_drv *driver_w = (param_drv*)ptr;
        
        
        if((driver_w->file_desc = open(driver_w->driver_name,  driver_w->mode ))<0)
        {
                tst_resm(TFAIL, "ERROR : Open driver fails %d", errno);
                return rv;
        }
        
        sleep(1);
        
        if( tcflush(driver_w->file_desc, TCIFLUSH)!=0)
        {
                tst_resm(TFAIL, "ERROR : Flush  driver fails %d", errno);
                return rv;
                
        }
        
        if(tcgetattr(driver_w->file_desc, &driver_w->termios_mxc_old)!=0)
        {
                tst_resm(TWARN, "Can not get atributs driver fails %d", errno);
                return rv;
                
        }
        
        set_param( driver_w);
        
        line_value = SET_ALL;
        
        if(( ioctl( driver_w->file_desc ,  TIOCMSET, &line_value)) <0)
        {
                tst_resm(TFAIL, "ERROR : ioctl TIOCMSET fails  : %d", errno);
                return rv;
        }
        
        line_value=TIOCM_DTR;
        if((ioctl( driver_w->file_desc , TIOCMBIC , &line_value))<0)
        {
                tst_resm(TFAIL, "ERROR : ioctl TIOCMBIC fails  : %d", errno);
                return rv;
        }
        
        line_value = RTS_AND_DTR;
        if(( ioctl ( driver_w->file_desc , TIOCMBIS, &line_value)) <0)
        {
                tst_resm(TFAIL, "ERROR : ioctl TIOCMBIS fails  : %d", errno);
                return rv;
        }
        
        if(( ioctl( driver_w->file_desc , TIOCMGET, &line_value)) <0)
        {
                tst_resm(TFAIL, "ERROR : ioctl TIOCMGET fails  : %d", errno);
                return rv;
        }
        
        
        
        
        rv = TPASS;
        return rv;
}

/*================================================================================================*/
/*===== set_param =====*/
/**
@brief  set UART parameters in struct termios 

@param  file descriptor

@return On success - return TPASS
        On failure - return the error code
*/
/*================================================================================================*/

int set_param(void *ptr)
{
        int rv = TFAIL;
        param_drv *driver_uart = (param_drv*)ptr;
        
        bzero(&driver_uart->termios_mxc, sizeof(struct termios));
        
        if(( rv = cfsetspeed( &driver_uart->termios_mxc ,B57600))) return rv;
        
        driver_uart->termios_mxc.c_cflag |= ( B57600 | CS8 | CLOCAL | CREAD);
        
        if(( tcsetattr(driver_uart->file_desc, TCSANOW, &driver_uart->termios_mxc)) <0)
        {
                tst_resm(TFAIL, "ERROR : Fail to set  control parameters (termios) : %d", errno);
                return rv;
        }
        
        rv = TPASS;
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
int mxc_uart_close(void* ptr )
{
        int rv = TFAIL;
        int ret = 0;
        
        param_drv *driver_uart = (param_drv*)ptr;
        
        if(( tcsetattr(driver_uart->file_desc, TCSANOW, &driver_uart->termios_mxc_old)) <0)
        {
                tst_resm(TFAIL, "ERROR : Fail to set control parameters (termios) : %d", errno);
                return rv;
        }
        
        /* Close MXC UART driver */
        if((ret = close(driver_uart->file_desc )) == -1)
        {
                tst_resm(TFAIL, "ERROR : Close  driver fails  : %d", errno);
                return rv;
        }
        driver_uart->file_desc=0;
        rv = TPASS;  
        
        return rv;
}

#ifdef __cplusplus
}
#endif
