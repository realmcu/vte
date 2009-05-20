/*====================*/
/**
    @file   mu_api_err_test.c

    @brief  C source of the mu_api_err_test application that generates variuos errors
            and checks proper Messaging Unit driver reactions.
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
Igor Semenchukov (smng001c)    25/08/2004     TLSbo40411   Initial version
Igor Semenchukov (smng001c)    30/08/2004     TLSbo40411   Review after inspection
Igor Semenchukov (smng001c)    09/12/2004     TLSbo43804   Rework after heavy MU driver modification
Igor Semenchukov (smng001c)    11/01/2005     TLSbo43806   Fix wrong verdict management errors
Dmitriy Kazachkov (e1403c)     29/06/2006     TLSbo61895   Rework after MU message format changing
Yury Batrakov (NONE)           20/09/2006     TLSbo75877   Fixed ioctl error case (#6)

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

/* Standard Include Files */

#include <errno.h>
#include <stdio.h>              /* fprintf()               */
#include <stdlib.h>             /* system(), malloc()      */
#include <string.h>             /* memset()                */
#include <sys/types.h>          /* open(), stat(), mknod() */
#include <sys/stat.h>           /* open(), stat(), mknod() */
#include <fcntl.h>              /* open(), stat(), mknod() */
#include <unistd.h>             /* close(), mknod()        */
#include <sys/ioctl.h>          /* ioctl()                 */

/* Harness Specific Include Files. */

#include "test.h"

/* Verification Test Environment Include Files */

#include "mu_api_err_test.h"

/*======================
                                        LOCAL MACROS
======================*/


/*======================
                          LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
======================*/


/*======================
                                       LOCAL CONSTANTS
======================*/
const char *mu_dir  = "/dev/mxc_mu";

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
/*= VT_mu_api_err_setup =*/
/**
@brief  assumes the pre-condition of the test case execution

@param  None

@return On success - return TPASS
        On failure - return the error code
*/
/*====================*/
int VT_mu_api_err_setup(void)
{
    return TPASS;
}


/*====================*/
/*= VT_mu_api_err_cleanup =*/
/**
@brief  assumes the post-condition of the test case execution

@param  None

@return On success - return TPASS
        On failure - return the error code
*/
/*====================*/
int VT_mu_api_err_cleanup(void)
{
    return TPASS;
}


/*====================*/
/*= VT_mu_api_err_test =*/
/**
@brief  Generate six errors and checks proper response of Messaging Unit driver system calls.

@param  Input :  None
        Output:  None

@return On success - return TPASS
        On failure - return the error code
*/
/*====================*/
int VT_mu_api_err_test(void)
{
    int         rv = TPASS,                 /* test status               */
                ret,                        /* syscalls return status    */
                major,                      /* test device major ...     */
                minor,                      /* ... and minor numbers     */
                fd,                         /* file descriptors used ... */
                fd1,                        /* ... in this test          */
                open_mode = O_RDWR,
                big_cnt = BIG_LEN;          /* big message length        */
    struct stat f_stat;
    char        mu_device[BUF_LEN],
                msg[BAD_CNT] = "\1\0\1\0\0",     /* non-aligned message       */
                *big_msg     = NULL;


    /*
     * FIRST TEST - try to open invalid device
     */

    printf("\n==  Running FIRST test  ==\n");

    /* Get MU driver major number since it may be a dynamic number */

    sprintf(mu_device, "%s/0", mu_dir);
    if ( (ret = stat(mu_device, &f_stat)) < 0)
    {
        printf("\tERROR: can't stat() device file %s: %s\n", mu_device, strerror(errno));
        return TFAIL;
    }
    major = f_stat.st_rdev & 0xFF00;

    /* Change minor number, so we will open invalid device file. Create device file */

    minor = NR_DEVS + 1;
    sprintf(mu_device, "%s/%d", mu_dir, minor);
    if ( (ret = mknod(mu_device, S_IFCHR | S_IRUSR | S_IWUSR, major | minor)) < 0)
    {
        printf("\tERROR: can't create device file %s: %s\n", mu_device, strerror(errno));
        return TFAIL;
    }
    fd = open(mu_device, open_mode);      /* Try to open file */
    if ( (fd < 0) && (errno == ENODEV) )  /* Proper behaviour */
    {
        tst_resm(TPASS, "Attempt to open an invalid device file is failed as expected. "
                 "Error is: %s", strerror(errno));
    }
    else
    {
        if (fd < 0) /* Error messages that confuses the user */
        {
            tst_resm(TWARN, "Although attempt to open an invalid device is failed, "
                     "it is produced wrong error message: %s!", strerror(errno));
        }
        else /* Misbehaviour */
        {
            tst_resm(TFAIL, "An invalid device file is opened!");
            close(fd);
            rv = TFAIL;
        }
    }
    remove(mu_device);

    /*
     * SECOND TEST - try to open the same device file two times at once.
     * Since device file opening operation allocates a particular channel,
     * the device must be busy
     */

    printf("\n==  Running SECOND test  ==\n");
    sprintf(mu_device, "%s/0", mu_dir);

    if ( (fd = open(mu_device, open_mode)) < 0)    /* Open a device first time */
    {
        printf("\tERROR: can't open device file: %s. Exiting...\n", strerror(errno));
        rv = TFAIL;
        return rv;
    }
    else
    {
        sleep(2);
        fd1 = open(mu_device, open_mode);     /* Try to open a device second time */
        if ( (fd1 < 0) && (errno == EBUSY) )  /* Proper behaviour */
        {
            tst_resm(TPASS, "Attempt to open a device file twice is failed as expected. "
                     "Error is: %s", strerror(errno));
        }
        else
        {
            if (fd1 < 0) /* Error messages that confuses the user */
            {
                tst_resm(TWARN, "Although attempt to open a device file twice is failed, "
                         "it is produced wrong error message: %s!", strerror(errno));
            }
            else /* Misbehaviour */
            {
                tst_resm(TFAIL, "Opened a device file twice at once!");
                close(fd1);
                rv = TFAIL;
            }
        }
    }   /* fd will be released after last test because all remained tests use it */

    /*
     * THIRD TEST - try to write message that is non-aligned to the register size
     */

    printf("\n==  Running THIRD test  ==\n");
    ret = write(fd, msg, strlen(msg));
    printf("ret = %d\n", ret);
    if ( (ret < 0) && (errno == EFAULT || errno==EINVAL) ) /* Proper behaviuor */
    {
        tst_resm(TPASS, "Attempt to write non-aligned message is failed as expected. "
                 "Error is: %s", strerror(errno));
    }
    else
    {
        if (ret < 0) /* Error messages that confuses the user */
        {
            tst_resm(TWARN, "Although attempt to write non-aligned message is failed, "
                     "it is produced wrong error message: %s!", strerror(errno));
        }
        else /* Misbehaviour */
        {
            tst_resm(TFAIL, "Wrote non-aligned message!");
            rv = TFAIL;
        }
    }

    /*
     * FOURTH TEST - try to read message that is non-aligned to the register size
     */

    printf("\n==  Running FORTH test  ==\n");
    memset(msg, 0, BAD_CNT + 1);
    ret = read(fd, msg, BAD_CNT);
    printf("ret = %d\n", ret);
    if ( (ret < 0) && (errno == EFAULT || errno==EINVAL) )
    {
        tst_resm(TPASS, "Attempt to read non-aligned message is failed as expected. "
                 "Error is: %s", strerror(errno));
    }
    else
    {
        if (ret < 0) /* Error messages that confuses the user */
        {
            tst_resm(TWARN, "Although attempt to write non-aligned message is failed, "
                     "it is produced wrong error message: %s!", strerror(errno));
        }
        else /* Misbehaviour */
        {
            tst_resm(TFAIL, "Read non-aligned message!");
            rv = TFAIL;
        }
    }

    /*
     * FIFTH TEST - try to write big amount of bytes and see what happened
     */

    printf("\n==  Running FIFTH test  ==\n");
    if ( (big_msg = (char *)malloc(big_cnt)) == NULL)
    {
        rv = TFAIL;
        printf("\tERROR: can't allocate memory for buffer big_msg: %s\n", strerror(errno));
    }
    else
    {
        memset(big_msg, 0xAA, big_cnt);
        ret = write(fd, big_msg, big_cnt);
        printf("ret = %d\n", ret);
        if (ret < 0)    /* Some error occured */
        {
            rv = TFAIL;
            tst_resm(TFAIL, "Failed to write big message: %s!", strerror(errno));
        }
        else
            tst_resm(TPASS, "Successfully wrote big message or part of it: %d bytes", ret);
        free(big_msg);
    }

    /*
     * SIXTH TEST - invalid ioctl() command. We send 10, since mxc_mu.h in
     * <kernel_tree>/include/asm/arch reserved numbers from 1 to 9
     */

    printf("\n==  Running SIXTH test  ==\n");
    ret = ioctl(fd, 10);
    if ( (ret < 0) && (errno == ENOTTY || errno==EINVAL) ) /* Proper behaviuor */
    {
        tst_resm(TPASS, "Attempt to call ioctl() with invalid cmd is failed as expected. "
                 "Error is: %s", strerror(errno));
    }
    else
    {
        if (ret < 0) /* Error messages that confuses the user */
        {
            tst_resm(TWARN, "Although attempt to call ioctl() with invalid cmd is failed, "
                     "it is produced wrong error message: %s!", strerror(errno));
        }
        else /* Misbehaviour */
        {
            tst_resm(TFAIL, "Processed unknown cmd, returned value = %d!", ret);
            rv = TFAIL;
        }
    }
    sleep(3);
    close(fd);

    return rv;
}

#ifdef __cplusplus
}
#endif
