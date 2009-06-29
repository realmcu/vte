/***
**Copyright 2007-2009 Freescale Semiconductor, Inc. All Rights Reserved.
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
    @file   uart_test_addon.h 

    @brief  UART test3 header file 
=========================================================================================
Revision History:
                            Modification     Tracking
Author (core ID)                Date          Number    Description of Changes
-------------------------   ------------    ----------  ---------------------------------
D.Kardakov                   04/06/2007    ENGR30041    Initial version
========================================================================================*/

#ifndef UART_TEST_ADDON_H
#define UART_TEST_ADDON_H

#ifdef __cplusplus
extern "C"{ 
#endif

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
//#define TESTCASE_SIMPLE 1
//#define TESTCASE_HW_FLOW 2
//#define TESTCASE_SW_FLOW 3
#define DATA_BUFFER_SIZE (16 * 1024)

/*======================================================================================
                                            ENUMS
    ======================================================================================*/


/*======================================================================================
                                STRUCTURES AND OTHER TYPEDEFS
    ======================================================================================*/


/*======================================================================================
                                GLOBAL VARIABLE DECLARATIONS
    ======================================================================================*/



/*======================================================================================
                                    FUNCTION PROTOTYPES
    ======================================================================================*/
extern int VT_uart_addon_setup(uart_config_t * uart_cfg, flow_ctrl_t flow_ctrl, int baud_rate);
extern int VT_uart_addon_cleanup(void);
extern int VT_uart_addon_test(int testcase);


#ifdef __cplusplus
} 
#endif

#endif                          /* UART_TEST_ADDON_H */
