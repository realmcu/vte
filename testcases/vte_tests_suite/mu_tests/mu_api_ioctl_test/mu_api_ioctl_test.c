/*====================*/
/**
    @file   mu_api_ioctl_test.c

    @brief  C source of the mu_api_ioctl_test test application that checks Messaging Unit driver
    ioctl() system call.
*/
/*======================

Copyright (C) 2004, Freescale Semiconductor, Inc. All Rights Reserved
THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
Freescale Semiconductor, Inc.

====================
Revision History:
                              Modification     Tracking
Author (Core ID)                  Date          Number    Description of Changes
---------------------------   ------------    ----------  ------------------------------------------
Igor Semenchukov (smng001c)    24/08/2004     TLSbo40411   Initial version
Igor Semenchukov (smng001c)    30/08/2004     TLSbo40411   Review after inspection
Igor Semenchukov (smng001c)    08/12/2004     TLSbo43804   Rework after heavy MU driver modification

====================
Portability: Indicate if this module is portable to other compilers or platforms.
             If not, indicate specific reasons why is it not portable.

======================*/

#ifdef __cplusplus
extern "C"{
#endif

/*======================
                                        INCLUDE FILES
======================*/
#define __KERNEL__

/* Standard Include Files */

#include <errno.h>
#include <sys/ioctl.h>          /* ioctl()           */
#include <sys/types.h>          /* open()            */
#include <sys/stat.h>           /* open()            */
#include <fcntl.h>              /* open()            */
#include <unistd.h>             /* close(), getopt() */
#include <stdio.h>              /* sprintf(), fprintf() */
#include <string.h>             /* strlen()          */
#include <asm/arch/mxc_mu.h>    /* ioctl() numbers   */

/* Harness Specific Include Files. */

#include "test.h"

/* Verification Test Environment Include Files */

#include "mu_api_ioctl_test.h"

/*======================
                                        LOCAL MACROS
======================*/


/*======================
                          LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
======================*/


/*======================
                                       LOCAL CONSTANTS
======================*/
const char *mu_dir = "/dev/mxc_mu";

/*======================
                                       LOCAL VARIABLES
======================*/
int fd;                     /* MU device file descriptor */

/*======================
                                       GLOBAL CONSTANTS
======================*/


/*======================
                                       GLOBAL VARIABLES
======================*/


/*======================
                                   LOCAL FUNCTION PROTOTYPES
======================*/
int send_ioctl(char *diag_msg, int cmd);

/*======================
                                       LOCAL FUNCTIONS
======================*/

/*====================*/
/*= VT_mu_api_ioctl_setup =*/
/**
@brief  assumes the pre-condition of the test case execution

@param  None

@return On success - return TPASS
        On failure - return the error code
*/
/*====================*/
int VT_mu_api_ioctl_setup(void)
{
    return TPASS;
}

/*====================*/
/*= VT_mu_api_ioctl_cleanup =*/
/**
@brief  assumes the post-condition of the test case execution

@param  None

@return On success - return TPASS
        On failure - return the error code
*/
/*====================*/
int VT_mu_api_ioctl_cleanup(void)
{
    return TPASS;
}

/*====================*/
/*= VT_mu_api_ioctl_test =*/
/**
@brief  Checks seven ioctl() commands presented in the Messaging Unit Driver

@param  Input :  None
        Output:  None

@return On success - return TPASS
        On failure - return the error code
*/
/*====================*/
int VT_mu_api_ioctl_test(void)
{
    int rv = TPASS,             /* test status */
        idx,                    /* cycle index */
        open_flags = O_RDWR;
    char mu_device[BUF_LEN];


    /*
     * Test each MU device since there are ioctl() cmds that do different work
     * on different devices
     */

    for (idx = 0; idx < NUM_DEVS; idx++)
    {
        sprintf(mu_device, "%s/%d", mu_dir, idx);
        if ( (fd = open(mu_device, open_flags)) < 0 )
        {
            printf("\tERROR: can't open device %s: %s\n", mu_device, strerror(errno));
            rv = TFAIL;
            break;
        }
        else
            printf("==  Device %s is opened  ==\n", mu_device);

        /*
         * These three ioctl's are common for all devices, so we need to test them only once
         */

        if (idx == 0)
        {
            if (send_ioctl("Sending NMI to the DSP core...\t", SENDNMI) == FALSE)
                rv = TFAIL;

            if (send_ioctl("Resetting DSP core...\t\t", SENDDSPRESET) == FALSE)
                rv = TFAIL;

            if (send_ioctl("Resetting the whole MU device...", SENDMURESET) == FALSE)
                rv = TFAIL;
        }

        /*
         * Remaining ioctl's will be tested with each device
         */

        if (send_ioctl("Sending GP interrupt...\t\t", SENDINT) == FALSE)
            rv = TFAIL;
        if (send_ioctl("Enabling RX interrupt...\t", RXENABLE) == FALSE)
            rv = TFAIL;
        if (send_ioctl("Enabling TX interrupt...\t", TXENABLE) == FALSE)
            rv = TFAIL;
        if (send_ioctl("Enabling GP interrupt...\t", GPENABLE) == FALSE)
            rv = TFAIL;

        close(fd);
        printf("==  Device %s is closed  ==\n\n", mu_device);
    }

    return rv;
}

/*====================*/
/*= send_ioctl =*/
/**
@brief  Prints a message concerning ioctl cmd that will be sent, sends this cmd and prints results.

@param  Input :  diag_msg - pointer to the diagnostic message
                 cmd      - ioctl command
        Output:  None

@return On success - return TRUE
        On failure - return FALSE
*/
/*====================*/
int send_ioctl(char *diag_msg, int cmd)
{
    int ret = TRUE;

    if (diag_msg)
        printf(diag_msg);
    if ( ioctl(fd, cmd) < 0 )
    {
        printf("ERROR: %s\n", strerror(errno));
        ret = FALSE;
    }
    else
        printf("SUCCESS\n");

    return ret;
}

#ifdef __cplusplus
}
#endif
