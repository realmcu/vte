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
    @file   uart_test_6.h

    @brief  UART test header file

====================================================================================================
Revision History:
                            				    
Author (core ID)             Date               CR Number         Description of Changes
------------------------   ------------    ----------------  -------------------------------------------
E.Gromazina                 25/04/2005       TLSbo48749       Initial version
E.GROMAZINA                 16/05/2005       TLSbo49644       minor fix

==================================================================================================*/

#ifndef UART_TEST6_H
#define UART_TEST6_H

#ifdef __cplusplus
extern "C"{
#endif

/*================================================================================================
                                         INCLUDE FILES
==================================================================================================*/
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <linux/types.h>
#include <linux/serial_core.h>
#include <linux/serial.h>

/*==================================================================================================
                                           CONSTANTS
==================================================================================================*/


/*==================================================================================================
                                       DEFINES AND MACROS
==================================================================================================*/
/* Drivers basename */
#define MXC_UART_TTY            "/dev/ttymxc/"

#define EXTERNAL_UART_TTS_0     "/dev/tts/0"
#define EXTERNAL_UART_TTS_1     "/dev/tts/1"

/* UART settings constants */
#define RTS_AND_DTR 	0x006
#define CTS_AND_RTS 	0x026

/* MXC port type defined in /include/linux/serial_core.h as PORT_MXC */
#define MXC_PORT_NUMBER 52

/* 16550 version 2 port type defined in /include/linux/serial_core.h as PORT_16650V2 */
#define EXTERNAL_UART_PORT_TYPE  7

/* Default parameters */
#define BAUD_DEF 		B115200
#define PARITY_DEF		'N' 
#define BREAK_DEF		'N' 

#define MAX_BUFFER_SIZE	512
#define ITER_SIZE			10

/*==================================================================================================
                                             ENUMS
==================================================================================================*/


/*==================================================================================================
                                 STRUCTURES AND OTHER TYPEDEFS
==================================================================================================*/
typedef struct param
{
        char UART1_drive[15];	
        char UART2_drive[15]; 
        char parity_con;
        char break_con;
        
}param_t;

/*==================================================================================================
                                 GLOBAL VARIABLE DECLARATIONS
==================================================================================================*/


/*==================================================================================================
                                     FUNCTION PROTOTYPES
==================================================================================================*/
int VT_uart_test_setup(param_t *);
int VT_uart_test_cleanup(void);
int VT_uart_test6(param_t *);


#ifdef __cplusplus
}
#endif

#endif  /* UART_TEST6_H */
