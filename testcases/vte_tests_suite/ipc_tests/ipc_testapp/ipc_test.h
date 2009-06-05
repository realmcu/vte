/*================================================================================================*/
/**
        @file   ipc_test.h

        @brief  Header file for IPC TTY test application. 
*/
/*==================================================================================================

        Copyright (C) 2006, Freescale Semiconductor, Inc. All Rights Reserved
        THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
        BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
        Freescale Semiconductor, Inc.

====================================================================================================
Revision History:
                            Modification     Tracking
Author/core ID                  Date          Number    Description of Changes
-------------------------   ------------    ----------  -------------------------------------------
I.Semenchukov/smng001c       31/03/2005     TLSbo47812  Initial version
S.Zavjalov/zvjs001c          24/06/2005     TLSbo50997  Linked list mode
A.Ozerov/b00320              26/04/2006     TLSbo61791  Performs a cast in accordance to coding standarts

====================================================================================================
Portability: ARM GCC

==================================================================================================*/
#ifndef IPC_TESTAPP_H
#define IPC_TESTAPP_H

#ifdef __cplusplus
extern "C"{
#endif

/*==================================================================================================
                                        INCLUDE FILES
==================================================================================================*/
#include <ipc_module.h>

/*==================================================================================================
                                        DEFINES AND MACROS
==================================================================================================*/
#if !defined(TRUE) && !defined(FALSE)
#define TRUE  1
#define FALSE 0
#endif

#define POWER_STATE_FILE                "/sys/devices/platform/mxc_mu.0/power/state"

#define DEVICE_PATH                     "/dev/mxc_ipc/"

/* MU channels */
typedef enum
{
        IPC_CHANNEL0 = 0,
        IPC_CHANNEL1,
        IPC_CHANNEL2,
/* SDMA DATA Channel */
        IPC_CHANNEL3,
/* SDMA Log Channel */
        IPC_CHANNEL4,
        IPC_CHANNEL5
} IPC_CHANNELS;

#define MAX_BUFFER_SIZE                 8192
#define TRANFER_SIZE                    8192

/* Max length of C-strings */
#define MAX_STR_LEN                     256
/* Length of short message */
#define SHORT_LEN                       4

/*==================================================================================================
                                            ENUMS
==================================================================================================*/
/* test case id names */
typedef enum 
{
        PACKET_DATA_EXCHANGE_1 = 0,
        PACKET_DATA_EXCHANGE_2,
        IOCTL_TEST,
        PWR_MGMT_TEST,
        ERROR_CHK_TEST,
        PAR_SHORT_MSG_TEST,
        PAR_DATA_LOG_TEST,
        PAR_ALL_TEST,
        LINKED_MODE_TEST,
        MSG_DATA_LOG_TEST
} IPC_TESTCASES;

/* Symbolic device names */
typedef enum
{
        SHORT_DEV1 = 0,
        SHORT_DEV2,
        SHORT_DEV3,
        PKT_DEV,
        LOG_DEV
} IPC_DEV_NAMES;

typedef enum
{
        ACTIVE_STATE,
        SUSP1_STATE,
        SUSP2_STATE,
        SUSP3_STATE
} DEV_PM_STATES;

/*==================================================================================================
                                            STRUCTURES
==================================================================================================*/
typedef struct
{
        unsigned int dev_number;
        unsigned int buf_size;
        int ltp_ret;
} dev_info_t;

/*==================================================================================================
                                        FUNCTION PROTOTYPES
==================================================================================================*/
void    cleanup(void);
int     VT_ipc_setup(void);
int     VT_ipc_cleanup(void);
int     VT_ipc_test(int test_id, int pkt_len, int log_len, int iter_nr, int nonblk, int quantity_seg);

#ifdef __cplusplus
}
#endif

#endif        /* IPC_TESTAPP_H */
