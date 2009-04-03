/*
  Copyright (C) 2004, Freescale Semiconductor, Inc. All Rights Reserved
  THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
  BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
  Freescale Semiconductor, Inc.
*/

/**
@file mu_sdma_ipc_test.h

@brief VTE C header file of mu_sdma_ipc_test application.

Description of the file

@par Portability:
        Indicate if this module is portable to other compilers or platforms.
        If not, indicate specific reasons why is it not portable.
*/

/*======================== REVISION HISTORY ====================================

Author (core ID)          Date         CR Number    Description of Changes
-----------------------   ----------   ----------   ----------------------------
I. Semenchukov/smng001c   09/03/2005   TLSbo47942   Initial version
Y. Batrakov               01/09/2006   TLSbo75877   Rework to write data
                                                    according to the MU protocol


=============================================================================*/

#ifndef MU_SDMA_IPC_TEST_H
#define MU_SDMA_IPC_TEST_H

#ifdef __cplusplus
extern "C"{
#endif

/*======================== INCLUDE FILES ====================================*/


/*======================== CONSTANTS ========================================*/
#if !defined(TRUE) && !defined(FALSE)
#define TRUE  1
#define FALSE 0
#endif

/*======================== DEFINES AND MACROS ===============================*/
#define DEF_NR_ITER         2
#define DEF_SDMA_BUFSZ      128
#define MU_BUFLEN           4
#define DEF_STR_LEN         256
#define DEF_SDMA_DEV        "/dev/sdma"
#define DEF_SDMA_NUM_DEVS   1
#define DEF_SDMA_FIRST_DEV  0
#define DEF_MU_DEV          "/dev/mxc_mu/"
#define DEF_MU_NUM_DEVS     4
#define DEF_MU_FIRST_DEV    0
#define SDMA_SIGN           "sdma"
#define CHAR_LEN            3
#define MAX_NUM_TEST_DEVS   4
/*======================== ENUMS ============================================*/


/*======================== STRUCTURES AND OTHER TYPEDEFS ====================*/
/* Handles all variables needed to communicate with one device and print results */
typedef struct
{
    char* dev_prefix;
    int   first_dev;
    int   num_devs;
    int   dev_handle[MAX_NUM_TEST_DEVS];
    char* wr_buf;
    char* rd_buf;
    int   buf_size;
    int   bytes_read;
    int   bytes_written;
    int   test_res;       /* LTP test result value (TPASS etc.) */
    int   thread_index;   /* Needed for proper thread ID output */
} dev_node_t;

/*======================== GLOBAL VARIABLE DECLARATIONS =====================*/


/*======================== FUNCTION PROTOTYPES ==============================*/

void cleanup();
int  VT_mu_sdma_ipc_setup();
int  VT_mu_sdma_ipc_cleanup();
int  VT_mu_sdma_ipc_test(int num_iter, dev_node_t* dev_nodes);

#ifdef __cplusplus
}
#endif

#endif  // MU_SDMA_IPC_TEST_H //
