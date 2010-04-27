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
    @file   uart_test_5.h

    @brief  UART test header file
====================================================================================================
Revision History:
                            Modification     Tracking
Author (core ID)                Date          Number    Description of Changes
-------------------------   ------------    ----------  -------------------------------------------
I.Inkina\nknl001           07/04/2005    TLSbo48749    
I. Inkina / nknl001       07/04/2005     TLSbo49644   minor fix
I. Inkina / nknl001       08/06/2005     TLSbo51148   update options  

==================================================================================================*/

#ifndef UART_TEST5_H
#define UART_TEST5_H

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
#include <linux/types.h>
#include <linux/serial_core.h>
#include <linux/serial.h>

#include <pthread.h>
#include <semaphore.h>

/*==================================================================================================
                                           CONSTANTS
==================================================================================================*/


/*==================================================================================================
                                       DEFINES AND MACROS
==================================================================================================*/
/* Drivers basename */
/* UART settings constants */
#define MXC_PORT_NUMBER          52

/*#define EXTERNAL_UART_PORT_TYPE  7*/

#define DRIVER_THREAD             2

/*==================================================================================================
                                             ENUMS
==================================================================================================*/


/*==================================================================================================
                                 STRUCTURES AND OTHER TYPEDEFS
==================================================================================================*/
typedef struct param
{
        int type;
        char *driver_name;
        pthread_t tid;           /* Thread id */
        int file_desc;
        struct serial_struct tty_mxc_serial;
        struct serial_struct tty_mxc_serial_old;

}param_drv;



/*==================================================================================================
                                 GLOBAL VARIABLE DECLARATIONS
==================================================================================================*/


/*==================================================================================================
                                     FUNCTION PROTOTYPES
==================================================================================================*/
int VT_mxc_uart_test5_setup(void);
int VT_mxc_uart_test5_cleanup(void);
int VT_mxc_uart_test5(char *driver_name_1,int type_1 );


#ifdef __cplusplus
}
#endif

#endif  /* UART_test5_H */
