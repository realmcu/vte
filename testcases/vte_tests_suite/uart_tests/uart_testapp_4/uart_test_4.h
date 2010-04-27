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
    @file   uart_test_4.h

    @brief  UART test header file
  
====================================================================================================
Revision History:
                            Modification     Tracking
Author (core ID)                Date          Number    Description of Changes
-------------------------   ------------    ----------  -------------------------------------------
I.Inkina\nknl001           07/04/2005    TLSbo49644      minor fix    

==================================================================================================*/

#ifndef UART_TEST4_H
#define UART_TEST4_H

#ifdef __cplusplus
extern "C"{
#endif

/*==================================================================================================
                                         INCLUDE FILES
==================================================================================================*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <linux/serial_core.h>
#include <linux/types.h>
#include <linux/serial.h>
#include <pthread.h>
#include <semaphore.h>

/*==================================================================================================
                                           CONSTANTS
==================================================================================================*/


/*==================================================================================================
                                       DEFINES AND MACROS
==================================================================================================*/
/* UART settings constants */
#define RTS_AND_DTR         0x006
#define SET_ALL 	                0x8006
#define CTS_AND_RTS        	0x026

/* Default parameters */

#define MXC_PORT_NUMBER     52

#define SET_LOCAL                      1
#define SET_ACTIVE_SIGNAL       2

#define DRIVER_THREAD             4
/*==================================================================================================
                                             ENUMS
==================================================================================================*/


/*==================================================================================================
                                 STRUCTURES AND OTHER TYPEDEFS
==================================================================================================*/
typedef struct param
{
        int mode;
        char *driver_name;
        pthread_t tid;           /* Thread id */
        int file_desc;
        struct termios termios_mxc_old;
        struct termios termios_mxc_new;
        struct termios termios_mxc;

}param_drv;



/*==================================================================================================
                                 GLOBAL VARIABLE DECLARATIONS
==================================================================================================*/


/*==================================================================================================
                                     FUNCTION PROTOTYPES
==================================================================================================*/
int VT_mxc_uart_test4_setup(void);
int VT_mxc_uart_test4_cleanup(void);
int VT_mxc_uart_test4(char* driver_name_1,char* driver_name_2,int testcase );


#ifdef __cplusplus
}
#endif

#endif  /* UART_test4_H */
