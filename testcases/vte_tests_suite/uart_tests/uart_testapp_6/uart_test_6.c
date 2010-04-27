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
    @file   uart_test_6.c

    @brief Configure the serial interface using MXC UART and/or External UART low-level driver
              Read / Write test

====================================================================================================
Revision History:
                                
Author                            Date              CR Number         Description of Changes
---------------------  ------   ------------    -----------------------------------------------------
E.GROMAZINA                   25/04/2005     TLSbo48749       Initial version 
E.GROMAZINA                   16/05/2005     TLSbo49644       minor fix
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
#include <time.h>
#include <sys/time.h>
    
/* Harness Specific Include Files. */
#include "test.h"

/* Verification Test Environment Include Files */
#include "uart_test_6.h"

/*=================================================================================================
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
/* UART file descriptors */
int file_desc0 = 0;
int file_desc1 = 0;
char send_buf[MAX_BUFFER_SIZE];
struct termios term_old_1;
struct termios term_old_2;
struct serial_icounter_struct uart1_icounter_old;
struct serial_icounter_struct uart2_icounter_old;
pthread_t id_send, id_rec, id_sig;

/*==================================================================================================
                                       GLOBAL CONSTANTS
==================================================================================================*/


/*==================================================================================================
                                       GLOBAL VARIABLES
==================================================================================================*/

/*==================================================================================================
                                   LOCAL FUNCTION PROTOTYPES
==================================================================================================*/
int set_param1(param_t *p,int);
int set_param2(param_t *p,int);
void *send_fun(void *);
void *receiv_fun(void *);
void *signal_func(void *);
void th_block_signals(void);
void uart_initctrlchar(struct termios *termios);
int check_hardware(param_t *p);

/*==================================================================================================
                                       LOCAL FUNCTIONS
==================================================================================================*/


/*================================================================================================*/
/*===== VT_uart_test_setup =====*/
/**
@brief  assumes the pre-condition of the test case execution

@param  Input :  UART params
        Output :  None
  
@return On success - return TPASS
        On failure - return the error code
*/
/*================================================================================================*/
int VT_uart_test_setup(param_t *p)
{
        int rv = TFAIL;
        
        /* Open  UART1 driver and UART2 driver  */
        file_desc0 = open(p->UART1_drive, O_RDWR|O_NONBLOCK);
        file_desc1 = open(p->UART2_drive, O_RDWR|O_NONBLOCK);
        
        sleep(1);
        if ((file_desc0 == -1) || (file_desc1 == -1))
                tst_resm(TFAIL, "ERROR : Open UART driver fails : %d", errno);
        
        else
                rv = TPASS;
        
        /* Save initial icounter */
        tst_resm(TINFO, "Saving initial icounter...");
        if (ioctl(file_desc0, TIOCGICOUNT, &uart1_icounter_old) < 0)
        {
                tst_resm(TFAIL, "ERROR : icounter old for UART1  => failure.");
                rv = TFAIL;     
        }
        
        if (ioctl(file_desc1 , TIOCGICOUNT, &uart2_icounter_old) < 0)
        {
                tst_resm(TFAIL, "ERROR : icounter old for UART2 => Failure.");
                rv = TFAIL; 
        }
        
        
        return rv;
}

/*================================================================================================*/
/*===== VT_uart_test6_cleanup =====*/
/**
@brief  assumes the post-condition of the test case execution

@param  Output: TFAIL or TPASS
  
@return On success - return TPASS
        On failure - return TFAIL
*/
/*================================================================================================*/
int VT_uart_test_cleanup(void)
{
        
        int rv = TFAIL, retval;
        int ret = 0;
        int ret1 = 0;
        
        /* Close MXC UART driver */
        tst_resm(TINFO, "Close UART driver\n");
        
        /* Restore options for the ports   */
        
        retval = tcsetattr(file_desc0, 0, &term_old_1);
        if (retval <0)
        {
                tst_resm(TFAIL, "ERROR : Fail to restore UART parameters : %d", errno);
                return rv;
        }
        
        retval = tcsetattr(file_desc1, 0, &term_old_2);
        if (retval <0)
        {
                tst_resm(TFAIL, "ERROR : Fail to restore UART parameters : %d", errno);
                return rv;
        }  
        
        sleep(1);
        ret1 = close(file_desc0);
        ret = close(file_desc1);
        
        if ((ret1 == -1) || (ret == -1))
                tst_resm(TFAIL, "ERROR : Close MXC UART driver fails  : %d", errno);
        
        else
                rv = TPASS;
        
        return rv;
}

/*================================================================================================*/
/*===== VT_uart_test6 =====*/
/**
@brief  uart test configures and sets UART driver before sending/receiving
        characters on it

@param  Input :  parameters of UART 
        Output :  None

@return On success - return TPASS
        On failure - return the error code
*/
/*================================================================================================*/
int VT_uart_test6(param_t *par)
{
/* ioctls to be tested :
TIOCGSERIAL   : Get serial UART configuration
TIOCSSERIAL   : Set serial UART parameters
TIOCSERCONFIG : Set auto-configuration
TIOCSERGETLSR : Get Line Status Register info
TIOCMIWAIT    : Wait for serial input status change (DCD, DSR, RI, CTS)
TIOCGICOUNT   : Get counter of input serial line interrupts
TIOCSERGWILD   obsolete
TIOCSERSWILD   obsolete

Sets the serial control lines to the state described by 'mctrl:
values : TIOCM_DTR = 0x2, TIOCM_RTS = 0x4, TIOCM_LOOP = 0x8
TIOCMBIS      : Turns on RTS, DTR or loopback registers
TIOCMBIC      : Turns off RTS, DTR or loopback registers
TIOCMSET      : Turns all three values of RTS, DTR and loopback
registers off
 
TIOCSBRK      : Turn break on, unconditionally, that is start sending zero bits
TIOCCBRK      : Turn break off, unconditionally, that is stop sending zero bits
TIOCMGET      : Returns the current state of the serial control inputs
such as DTR or RTS lines
*/

        int i,j;
        int rv = TFAIL;
        int retval;
        int rfun1, rfun2;
        
        /*sending buffer preparation */
        j=49;
        for( i=0; i<MAX_BUFFER_SIZE; i++)
        {	
                send_buf[i] =(char)(j++);; 
                if(j > 58)
                        j=49;
        }
        send_buf[i-1] = '\0';
        
        bzero(&term_old_1,sizeof(&term_old_1));
        bzero(&term_old_2,sizeof(&term_old_2));
        
        /* Get the current options for the ports  */
        retval = tcgetattr(file_desc0, &term_old_1);
        if (retval <0)
        {
                tst_resm(TFAIL, "ERROR : Fail to get UART parameters : %d", errno);
                return rv;
        }	
        
        retval = tcgetattr(file_desc1, &term_old_2);
        if (retval <0)
        {
                tst_resm(TFAIL, "ERROR : Fail to get UART parameters : %d", errno);
                return rv;
        }	
        
        /*Set necessary configuration to UART1*/
        retval = set_param1(par,file_desc0);
        if (retval < 0)
        {
                tst_resm(TFAIL, "ERROR : set parameters of UART1");
                return rv;
        }
        /*Set necessary configuration to UART2*/
        retval = set_param2(par,file_desc1);
        if (retval < 0)
        {
                tst_resm(TFAIL, "ERROR : set parameters of UART2");
                return rv;
        }
        
        /* Suspend output transmission */
        retval = ioctl(file_desc1, TCXONC, TCOOFF);
        if (retval <0)
        {
                tst_resm(TFAIL, "ERROR : ioctl TCXONC fails : %d\n", errno);
                return rv;
        }
        
        /*Start transmitting thread*/
        tst_resm(TINFO, "Creating transmitting thread ");
        if(pthread_create( &id_send, NULL, &send_fun, (void *)par))
        {
                tst_resm(TFAIL,"ERROR : error creating sending thread" );	
                return rv;	
        }
        /*Start receiving thread*/
        tst_resm(TINFO, "Creating receiving thread ");
        if(pthread_create( &id_rec, NULL, &receiv_fun, (void *)par))
        {
                tst_resm(TFAIL,"ERROR : error creating receiving thread " );	
                return rv;	
        }
        
        th_block_signals();
        
        /* Start signal thread*/	
        tst_resm(TINFO, "Creating signal thread ");
        if(pthread_create(&id_sig, NULL, &signal_func, NULL))
        {
                tst_resm(TFAIL,"ERROR : error creating of signal thread " );	
                return rv;	
        }
        
        pthread_join(id_send,(void *)&rfun1);
        pthread_join(id_rec, (void *)&rfun2);
        if((rfun1 == TFAIL) || (rfun2 == TFAIL))
        {
                retval = check_hardware(par);
                return rv;
        }	
        
        retval = check_hardware(par);	 
        
        if(retval != TFAIL)
                rv = TPASS;
        return rv;
}


/*================================================================================================*/
/*===== set_param1 =====*/
/**
@brief  Set params for UART1

@param  Input :  input options, file descriptor UART1

@return On success - return TPASS
        On failure - return TFAIL
*/
/*================================================================================================*/
int set_param1(param_t *p,int d1)
{	
        int retval;
        int rv = TFAIL;
        struct termios term;
        speed_t speed = 0;
        unsigned int ioc_line = 0;
        
        /* Configure UART CTS/DTR to be ready to send/receive packets */
        ioc_line |= RTS_AND_DTR;
        
        retval = ioctl(d1, TIOCMBIS, &ioc_line);
        if (retval < 0)
        {
                tst_resm(TFAIL, "ERROR : ioctl TIOCMBIS fails : %d", errno);
                return rv;
        }
        
        bzero(&term, sizeof(term));
        
        /*set flow control*/
        
        speed = BAUD_DEF;
        retval = cfsetospeed(&term,speed);
        if (retval < 0)
        {
                tst_resm(TFAIL, "speed is failure ");
                return rv;
        }
        
        term.c_cflag &= ~CSIZE; 
        term.c_cflag |= CS7;    
        term.c_cflag &= ~PARENB;
        term.c_cflag &= ~CSTOPB;
        
        /*Set parity control*/
        if(p->parity_con != 'N')
        {
                term.c_cflag |= PARENB;
                term.c_cflag &= ~PARODD;
                
        }	
        
        /*Set control chars */
        uart_initctrlchar(&term);
        
        /* Set local read */
        tst_resm(TINFO, "Set local read");
        term.c_cflag |= (CLOCAL | CREAD);
        
        retval = tcsetattr(d1, 0, &term);
        if (retval <0)
        {
                tst_resm(TFAIL, "ERROR : Fail to set UART control parameters (termios) : %d", errno);
                return rv;
        }
        
        return TPASS;
}

/*================================================================================================*/
/*===== set_param2 =====*/
/**
@brief  Set params for UART2

@param  Input :  input options, file descriptor UART2

@return On success - return TPASS
        On failure - return TFAIL
*/
/*================================================================================================*/
int set_param2(param_t *p,int d1)
{	
        int retval;
        int rv = TFAIL;
        struct termios term;
        speed_t speed = 0;
        unsigned int ioc_line = 0;
        
        /* Configure UART CTS/DTR to be ready to send/receive packets */
        ioc_line |= RTS_AND_DTR;
        retval = ioctl(d1, TIOCMBIS, &ioc_line);
        if (retval < 0)
        {
                tst_resm(TFAIL, "ERROR : ioctl TIOCMBIS fails : %d", errno);
                return rv;
        }
        
        bzero(&term, sizeof(term));
        
        /*set flow control*/
        term.c_iflag |= (IXON | IXOFF | IXANY);
        
        speed = BAUD_DEF;
        retval = cfsetispeed(&term,speed);
        if (retval <0)
        {
                tst_resm(TFAIL, "input speed is failure ");
                return rv;
        }
        
        term.c_cflag &= ~CSIZE; 
        term.c_cflag |= CS7;
        term.c_cflag &= ~PARENB;
        term.c_cflag &= ~CSTOPB;
        
        /* set break control*/
        switch (p->break_con)
        {
        case 'D': 
                term.c_iflag &= ~IGNBRK;				
                break;
        case 'I':  
                term.c_iflag |= IGNBRK;
                break;
        }
        
        /*Set parity control*/
        switch (p->parity_con)
        {
        case 'D': term.c_cflag |= PARENB;
                term.c_cflag |= PARODD;
                term.c_cflag |= CSTOPB;
                term.c_iflag |= (INPCK | ISTRIP);;  
                term.c_iflag &= ~IGNPAR;								
                break;
        case 'I':  term.c_cflag |= PARENB;
                term.c_cflag |= PARODD;
                term.c_cflag |= CSTOPB;
                term.c_iflag |= IGNPAR;		
                break;
        }	
        
        /*Set control chars */
        uart_initctrlchar(&term);
        
        /* Set local read */
        term.c_cflag |= (CLOCAL | CREAD);
        
        retval = tcsetattr(d1, 0, &term);
        if (retval <0)
        {
                tst_resm(TFAIL, "ERROR : Fail to set UART control parameters (termios) : %d", errno);
                return rv;
        }
        
        return TPASS;
}


/*================================================================================================*/
/*===== send_fun =====*/
/**
@brief  Function for transmission

@param  Input :  input options

@return On success - return NULL
        On failure - return TFAIL
*/
/*================================================================================================*/
void *send_fun(void *p1)
{	
        int retval, rest;
        int i;
        param_t *p = (param_t *)p1;
        int rv = TFAIL;
        char break_buf ='B';
        
        th_block_signals();
        
        /* for break test */
        if(p->break_con != 'N')
        {	
                tst_resm(TINFO,"Write to sending UART for break test");
                for(i=0; i<ITER_SIZE; i++)
                {
                        
                        /* Turn on break status  */
                        retval = ioctl(file_desc0, TIOCSBRK, NULL);
                        if (retval <0)
                        {
                                tst_resm(TFAIL, "ERROR : ioctl TIOCSBRK fails : %d", errno);
                                return (void*) rv;
                        }
                        
                        /*		retval = tcdrain(file_desc0);
                        if (retval < 0)
                        {
                        tst_resm(TFAIL, "ERROR : error tcdrain : %d\n", errno);
                        return (void *) rv;
                        }
                        */			usleep(500000);
                        
                        /* Turn off break status */
                        retval = ioctl(file_desc0, TIOCCBRK, NULL);
                        if (retval <0)
                        {
                                tst_resm(TFAIL, "ERROR : ioctl TIOCCBRK fails : %d", errno);
                                return(void*) rv;
                        }
                        
                        /* Write buffer to sending UART */
                        retval = write(file_desc0, &break_buf, 1);
                        if (retval < 0)
                        {
                                tst_resm(TFAIL, "ERROR : first write to UART fails : %d", errno);
                                return (void *) rv;
                        }
                        
                }
                
        }
        
        /* Write buffer to sending UART */
        /* for parity test */
        if(p->parity_con != 'N')
        {	
                tst_resm(TINFO,"Write to sending UART for parity test");
                rest = MAX_BUFFER_SIZE;
                do 
                {  
                        
                        retval = write(file_desc0, &send_buf[MAX_BUFFER_SIZE - rest], rest);
                        if (retval < 0)
                        {
                                if (errno != EAGAIN)
                                {
                                        tst_resm(TFAIL,"ERROR: transfert failure after writing %d/%d bytes, error-%d ", MAX_BUFFER_SIZE - rest, MAX_BUFFER_SIZE,errno);
                                        pthread_exit((void*)rv);
                                }
                        }	
                        else if (retval != 0)
                        {
                                rest -= retval;
                        }
                        if(retval == 0)
                        {
                                break;
                        }
                        
                } while (rest > 0);
                
                tst_resm(TINFO,"send_buf  : %s", send_buf);
        }
        
        return NULL;
}


/*================================================================================================*/
/*===== receive_fun =====*/
/**
@brief  Function for receiving

@param  Input :  input options

@return On success - return NULL
        On failure - return TFAIL
*/
/*================================================================================================*/
void *receiv_fun(void *p1)
{
        int retval;
        int rest,beg;
        char receive_buf[MAX_BUFFER_SIZE];
        int rv = TFAIL;
        param_t *p = (param_t *)p1;
        struct serial_icounter_struct coun;
        struct timeval t_st,t_fin;
        
        th_block_signals();
        
        bzero(&coun, sizeof(coun));	
        
        /* Resume output transmission */
        retval = ioctl(file_desc1, TCXONC, TCOON);
        if (retval <0)
        {
                tst_resm(TFAIL, "ERROR : ioctl TCXONC fails : %d", errno);
                return (void *)rv;
        }

        /* Read pattern from receiving UART */
        tst_resm(TINFO,"Read file from receiving UART");
        if(p->parity_con != 'N')
        {
                rest = MAX_BUFFER_SIZE;
                beg = MAX_BUFFER_SIZE;
        }	
        if(p->break_con != 'N')
        {
                rest = ITER_SIZE*2 ;
                beg = ITER_SIZE*2;
        }
        
        gettimeofday(&t_st,NULL);
        do 
        {	
                retval = read(file_desc1, &receive_buf[beg - rest], rest);
                if (retval < 0)
                {
                        if (errno != EAGAIN)
                        {
                                tst_resm(TFAIL,"ERROR: receiver failure after reading %d/%d bytes ", beg - rest, beg);
                                pthread_exit((void*)rv);
                        }
                        
                }
                else if (retval != 0)
                        rest -= retval;
                
                if(retval == 0)
                {	
                        if(rest > 0)
                                tst_resm(TFAIL,"TINFO: receive %d bytes instead of %d",beg - rest,beg);
                        break;
                }
                
                gettimeofday(&t_fin,NULL);
                if((t_fin.tv_sec - t_st.tv_sec) > 6)
                        break;
        }while (rest > 0);		
        
        if(p->parity_con != 'N') 
                tst_resm(TINFO,"read_buf : %s", receive_buf);  
        
        return NULL;
}

/*================================================================================================*/
/* ====== signal_func ===== */
/**
@brief  Function for processing of signal

@param  

@return On success - return NULL
        On failure - return TFAIL
*/
/*================================================================================================*/
void *signal_func(void *p1)
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
                tst_resm(TINFO,"Got signal \"%s\"...", sys_siglist[signal]);
                switch (signal)
                {
                case SIGINT:
                        error = pthread_cancel(id_send);
                        if (error < 0)
                                perror("pthread_cancel(writer)");
                        error = pthread_cancel(id_rec);
                        if (error < 0)
                                perror("pthread_cancel(reader)");
                        rv = TFAIL;
                        pthread_exit((void*)rv);
                        break;
                default:
                        tst_resm(TINFO,"Ignoring signal \"%s\"", sys_siglist[signal]);
                }
        }
}


/* ============================================================================================= */
void th_block_signals(void)
{
        sigset_t mask;
        
        sigemptyset(&mask);
        sigaddset(&mask, SIGINT);
        pthread_sigmask(SIG_BLOCK, &mask, NULL);
}

/*================================================================================================*/
/* ====== uart_initctrlchar ===== */
/**
@brief  Function for set of control characters

@param  input:  termios struct

*/
/*================================================================================================*/
void uart_initctrlchar(struct termios *termios)
{
        termios->c_cc[VTIME] = 0;
        termios->c_cc[VMIN] = 1;
        
        termios->c_cc[VSTART] = _POSIX_VDISABLE;
        termios->c_cc[VSTOP] = _POSIX_VDISABLE;
        termios->c_cc[VINTR] = _POSIX_VDISABLE; //CINTR;
        termios->c_cc[VQUIT] = _POSIX_VDISABLE; //CQUIT;
        termios->c_cc[VERASE] = _POSIX_VDISABLE; //CERASE;
        termios->c_cc[VKILL] = _POSIX_VDISABLE; //CKILL;
        termios->c_cc[VEOF] = _POSIX_VDISABLE; //CEOF;
        termios->c_cc[VSWTC] = _POSIX_VDISABLE; //CSWTC;
        termios->c_cc[VSUSP] = _POSIX_VDISABLE; //CSUSP;
        termios->c_cc[VEOL] = _POSIX_VDISABLE; //CEOL;
        termios->c_cc[VREPRINT] = _POSIX_VDISABLE; //CREPRINT;
        termios->c_cc[VDISCARD] = _POSIX_VDISABLE; //CDISCARD;
        termios->c_cc[VWERASE] = _POSIX_VDISABLE; //CWERASE;
        termios->c_cc[VLNEXT] = _POSIX_VDISABLE; //CLNEXT;
        termios->c_cc[VEOL2] = _POSIX_VDISABLE; //CEOL;
}

/* ====== check_hardware ===== */
/*===============================================================================================*/
/*@brief  Function for check of hardware

@param  Input :  input options
        Output :  TPASS or TFAIL
@return On success - return TPASS
        On failure - return TFAIL
*/
/*================================================================================================*/
int check_hardware(param_t *p)
{
        int rv = TPASS;
        struct serial_icounter_struct uart1_icounter;
        struct serial_icounter_struct uart2_icounter;
        
        tst_resm(TINFO, "  Checking harware behaviour...");
        
        if (ioctl(file_desc0, TIOCGICOUNT, &uart1_icounter) < 0)
        {
                tst_resm(TFAIL, " TIOCGICOUNT   => Failure.");
                rv = TFAIL;     
        }
        
        if (ioctl(file_desc1 , TIOCGICOUNT, &uart2_icounter) < 0)
        {
                tst_resm(TFAIL, "  TIOCGICOUNT => Failure.");
                rv = TFAIL; 
        }
        
#define ICNT(who, what)  who##_icounter.what - who##_icounter_old.what
        
        tst_resm(TINFO, "/======================================\\");
        tst_resm(TINFO, "|       UART hardware summary          |");
        tst_resm(TINFO, "+============+============+============+");
        tst_resm(TINFO, "|            |    SRC     |    DST     |");
        tst_resm(TINFO, "+------------+------------+------------+");
        tst_resm(TINFO, "+------------+------------+------------+");
        tst_resm(TINFO, "|#cts        | %-10d | %-10d |", ICNT(uart1,cts), ICNT(uart2,cts));
        tst_resm(TINFO, "|#dsr        | %-10d | %-10d |", ICNT(uart1,dsr), ICNT(uart2,dsr));
        tst_resm(TINFO, "|#rng        | %-10d | %-10d |", ICNT(uart1,rng), ICNT(uart2,rng));
        tst_resm(TINFO, "|#dcd        | %-10d | %-10d |", ICNT(uart1,dcd), ICNT(uart2,dcd));
        tst_resm(TINFO, "|#rx         | %-10d | %-10d |", ICNT(uart1,rx), ICNT(uart2,rx));
        tst_resm(TINFO, "|#tx         | %-10d | %-10d |", ICNT(uart1,tx), ICNT(uart2,tx));
        tst_resm(TINFO, "|#frame err  | %-10d | %-10d |", ICNT(uart1,frame), ICNT(uart2,frame));
        tst_resm(TINFO, "|#overrun    | %-10d | %-10d |", ICNT(uart1,overrun), ICNT(uart2,overrun));
        tst_resm(TINFO, "|#parity err | %-10d | %-10d |", ICNT(uart1,parity), ICNT(uart2,parity));
        tst_resm(TINFO, "|#buf overrun| %-10d | %-10d |", ICNT(uart1,buf_overrun), ICNT(uart2,buf_overrun));
        tst_resm(TINFO, "|#break      | %-10d | %-10d |", ICNT(uart1,brk), ICNT(uart2,brk));
        tst_resm(TINFO, "\\------------+------------+------------/");
        
        if (p->parity_con != 'N')
        {
                if ((ICNT(uart2,parity) == 0) )
                {
                        tst_resm(TFAIL, "Error: number parity or frame errors is failed");
                        rv = TFAIL;
                }
        }
        
        if (p->break_con != 'N')
        {
                if (ICNT(uart2,brk) == 0 )
                {
                        tst_resm(TFAIL, "Error: number breaks is failed");
                        rv = TFAIL;
                }
        }
        
        return rv;
#undef ICNT
}


#ifdef __cplusplus
}
#endif
