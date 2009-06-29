/***
**Copyright 2005-2009 Freescale Semiconductor, Inc. All Rights Reserved.
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
    @file   uart_test_3.h 

    @brief  UART test3 header file 
=========================================================================================
Revision History:
                            Modification     Tracking
Author (core ID)                Date          Number    Description of Changes
-------------------------   ------------    ----------  ---------------------------------
C.GAGNERAUD/cgag1c           01/06/2005    TLSbo45060   Rework and improve UART test 
                                                        application
I.Inkina/nknl001             22/08/2005    TLSbo52626    Update Uart test
D.Kardakov                   04/06/2007    ENGR30041     New transfer data test was added
========================================================================================*/

#ifndef UART_TEST3_H
#define UART_TEST3_H

#ifdef __cplusplus
extern "C"{ 
#endif

#include <stdio.h>
#include <linux/types.h>
#include <linux/serial.h> 
/*======================================================================================
                                        INCLUDE FILES
    ======================================================================================*/
#include "uart_utils.h"

/*======================================================================================
                                            CONSTANTS
    ======================================================================================*/


/*======================================================================================
                                        DEFINES AND MACROS
    ======================================================================================*/
#define TESTCASE_SIMPLE 1
#define TESTCASE_HW_FLOW 2
#define TESTCASE_SW_FLOW 3

/* HW buffer is 16 bytes, driver buffer is 4KB * The transfert len should be strictly greater than
* the * rec+send buffers */

#define MIN_TRANSFERT_LENGTH (2.5*(4*1024+16))

/*======================================================================================
                                            ENUMS
    ======================================================================================*/


/*======================================================================================
                                STRUCTURES AND OTHER TYPEDEFS
    ======================================================================================*/
typedef struct
{
        int fd;
        off_t file_size;
} transmit_file_t;

typedef struct
{
        int     fd;
        char   *device;
        char   *desc;
        struct termios termios;
        struct termios termios_checked;
        struct termios termios_saved;
        struct serial_icounter_struct icounter;
        struct serial_icounter_struct icounter_saved;
        unsigned char *data;
        flow_ctrl_t flow_ctrl;
        int     baud_rate;
    transmit_file_t fpar;
} uart_test_t;

/*======================================================================================
                                GLOBAL VARIABLE DECLARATIONS
    ======================================================================================*/



/*======================================================================================
                                    FUNCTION PROTOTYPES
    ======================================================================================*/
extern int VT_uart_test3_setup(uart_config_t * src_cfg, uart_config_t * dst_cfg,
                               int transfert_length, flow_ctrl_t flow_type, int baud_rate);
extern int VT_uart_test3_cleanup(void);
extern int VT_uart_test3(int testcase);
extern int VT_setup_uart(uart_config_t * cfg, uart_test_t * uart, 
                         char *desc, int open_flag);
extern int VT_cleanup_uart(uart_test_t * uart);


#ifdef __cplusplus
} 
#endif

#endif                          /* UART_TEST3_H */
