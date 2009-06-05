/*================================================================================================*/
/**
        @file   dvfs_dptc_test.h

        @brief  Test scenario C header template.
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
V.Khalabuda/b00306           22/11/2006     TLSbo83054  Initial version

==================================================================================================*/
#ifndef DVFS_DPTC_TEST_H
#define DVFS_DPTC_TEST_H

#ifdef __cplusplus
extern "C"{
#endif

/*==================================================================================================
                                        INCLUDE FILES
==================================================================================================*/
/* Standard Include Files */
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <errno.h>

#include <unistd.h>
#include <termios.h>

/* Harness Specific Include Files. */
#include <test.h>
#include <usctest.h>

#include <asm/arch/pm_api.h>

/*==================================================================================================
                                            CONSTANTS
==================================================================================================*/
#if !defined(TRUE) && !defined(FALSE)
#define TRUE    1
#define FALSE   0
#endif

/*==================================================================================================
                                        DEFINES AND MACROS
==================================================================================================*/
#define DPTC_DEVICE     "/dev/dptc"
#define DVFS_DEVICE     "/dev/dvfs"
#define DVFS_DPTC_DEVICE        "/dev/dvfs_dptc"
#define DPTC_PROC_DEVICE        "/proc/dptc"

#define IN_BUFFER_SIZE  1024

#define SAFE_DELETE(p)  {if(p){free(p);p=0;}}

/*! Number of log buffer entries to be displayed.  */
#define LOG_NUM_OF_ROWS 10

#define MAX_TABLE_SIZE  8192

/*==================================================================================================
                                            ENUMS
==================================================================================================*/
/** DVFS_TEST_CASES type */
typedef enum 
{
        INIT_TEST = 0,  /**< Init test */
        RW_TEST,        /**< Reading and updating tables test*/
        RC_TEST,        /**< Enable and disable dptc reference circuit test */
        WP_TEST,        /**< Working point change tests */
        FREQ_TEST,      /**< Frequency command test */
        SW_TEST,        /**< SW general purpose bits test */
        WFI_TEST        /**< Wait-For-Interrupt state test */
} DVFS_TEST_CASES;

/*==================================================================================================
                                STRUCTURES AND OTHER TYPEDEFS
==================================================================================================*/
/* This structure holds the DVFS/DPTC log buffer entries read from the DPTC driver
 * after formatting them into text lines.
 */
typedef struct
{
        /* Points to the position in the buffer the next formatted log entry will be written to. */
        unsigned int    head;

        /* A table that holds the DPTC log buffer entries read from the
         * DPTC driver after formatting them to text lines. */
        char            table[LOG_NUM_OF_ROWS][80];
} log_table_s;

/*==================================================================================================
                                GLOBAL VARIABLE DECLARATIONS
==================================================================================================*/

/*==================================================================================================
                                    FUNCTION PROTOTYPES
==================================================================================================*/
int     VT_dvfs_dptc_setup(void);
int     VT_dvfs_dptc_cleanup(void);
int     VT_dvfs_dptc_test(void);

void    cleanup(void);

#ifdef __cplusplus
}
#endif

#endif          /* DVFS_DPTC_TEST_H */
