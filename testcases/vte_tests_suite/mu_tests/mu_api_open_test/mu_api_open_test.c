/*====================*/
/**
    @file   mu_api_open_test.c

    @brief  C source of mu_api_open_test test application that checks Messaging Unit
            driver open() and close() system calls.
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
Igor Semenchukov (smng001c)    23/08/2004     TLSbo40411   Initial version
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
#include <sys/types.h>          /* open()                       */
#include <sys/stat.h>           /* open()                       */
#include <fcntl.h>              /* open()                       */
#include <unistd.h>             /* close()                      */
#include <stdio.h>              /* sprintf()                    */
#include <sys/ioctl.h>          /* ioctl()                      */
#include <asm/arch/mxc_mu.h>    /* ioctl() command numbers      */

/* Harness Specific Include Files. */

#include "test.h"

/* Verification Test Environment Include Files */

#include "mu_api_open_test.h"

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


/*======================
                                       GLOBAL CONSTANTS
======================*/


/*======================
                                       GLOBAL VARIABLES
======================*/


/*======================
                                   LOCAL FUNCTION PROTOTYPES
======================*/


/*======================
                                       LOCAL FUNCTIONS
======================*/


/*====================*/
/*= VT_mu_api_open_setup =*/
/**
@brief  assumes the pre-condition of the test case execution

@param  None

@return On success - return TPASS
        On failure - return the error code
*/
/*====================*/
int VT_mu_api_open_setup(void)
{

    return TPASS;
}


/*====================*/
/*= VT_mu_api_open_cleanup =*/
/**
@brief  assumes the post-condition of the test case execution

@param  None

@return On success - return TPASS
        On failure - return the error code
*/
/*====================*/
int VT_mu_api_open_cleanup(void)
{

    return TPASS;
}


/*====================*/
/*= VT_mu_api_open_test =*/
/**
@brief  Open each MU device, spend some time while it is opened, then close the device

@param  Input :  None
        Output:  None

@return On success - return TPASS
        On failure - return the error code
*/
/*====================*/
int VT_mu_api_open_test(void)
{
    int  rv = TPASS,
         fd,
         ret,
         idx,
         open_mode = O_RDWR;
    char mu_dev[BUF_LEN];

    for (idx = 0; idx < NUM_DEVS; idx++)
    {
        sprintf(mu_dev, "%s/%d", mu_dir, idx);
        fd = open(mu_dev, open_mode);
        if (fd < 0)
        {
            printf("\tERROR: failed to open device %s: %s\n", mu_dev, strerror(errno));
            rv = TFAIL;
            continue;
        }
        else
            printf("Device %s is opened\n", mu_dev);

        /* Fill the time between open() and close() to get things more real */

        ret = ioctl(fd, SENDINT);
        if (ret < 0)
        {
            printf("\tAn ERROR occurs while device %s was being opened: %s\n", mu_dev,
                   strerror(errno));
            rv = TFAIL;
        }
        sleep(1);

        ret = close(fd);
        if (ret < 0)
        {
            printf("\tERROR: failed to close device %s: %s\n", mu_dev, strerror(errno));
            rv = TFAIL;
        }
        else
            printf("Device %s is closed\n", mu_dev);

        printf("\n");
    }
    return rv;
}

#ifdef __cplusplus
}

#endif
