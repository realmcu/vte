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
        @file   ipc_module.h

        @brief  Header file for IPC dirver API
====================================================================================================
Revision History:
                            Modification     Tracking
Author/core ID                  Date          Number    Description of Changes
-------------------------   ------------    ----------  --------------------------------------------
A.Ozerov/b00320              26/04/2006     TLSbo61791  Initial version
V.Khalabuda/b00306           07/04/2006     TLSbo63489  Update version for linux-2.6.10-rel-L26_1_19

====================================================================================================
Portability: ARM GCC

==================================================================================================*/
#ifndef IPC_TEST_H
#define IPC_TEST_H

#ifdef __cplusplus
extern "C"{
#endif

/*==================================================================================================
                                        INCLUDE FILES
==================================================================================================*/
#include <asm/arch/mxc_ipc.h>

/*=================================================================================================
                                        DEFINES AND MACROS
==================================================================================================*/
#define MXC_IPC_DEV             "test_ipc"

#define DEBUG 1

#if DEBUG
#define DPRINTK(fmt, args...) printk("%s: " fmt, __FUNCTION__ , ## args)
#else
#define DPRINTK(fmt, args...)
#endif

#define OPEN_CLOSE_TEST                         0x0
#define PACKET_DATA_LOOPBACK                    0x1
#define SHORT_MSG_LOOPBACK                      0x2
#define PACKET_DATA_CONT_LOOPBACK               0x5
#define PACKET_DATA_LINK_LOOPBACK               0x6
#define LOGGING_LOOPBACK                        0x4

#define IPCTEST_MAJOR           0
//#define MAX_BUFFER_SIZE         512

/*==================================================================================================
                                            STRUCTURES
==================================================================================================*/
struct ioctl_args
{
        int                   iterations;
        unsigned short        channel;
        unsigned short        vchannel;
        unsigned short        bytes;
        unsigned short        message;
};

#ifdef __cplusplus
}
#endif

#endif        /* IPC_TEST_H */
