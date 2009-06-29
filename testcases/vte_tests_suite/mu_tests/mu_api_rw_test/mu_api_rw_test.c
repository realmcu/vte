/***
**Copyright 2004-2009 Freescale Semiconductor, Inc. All Rights Reserved.
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
    @file   mu_api_rw_test.c

    @brief  C source of the mu_api_rw_test test that checks Messaging Unit
            driver read() and write() system calls in both blocking and non-blocking modes.
====================================================================================================
Revision History:
                              Modification     Tracking
Author (Core ID)                  Date          Number    Description of Changes
---------------------------   ------------    ----------  ------------------------------------------
Igor Semenchukov (smng001c)    23/08/2004     TLSbo40411   Initial version
Igor Semenchukov (smng001c)    30/08/2004     TLSbo40411   Review after inspection
Igor Semenchukov (smng001c)    09/12/2004     TLSbo43804   Rework after heavy MU drv modification;
                                                           merge two test cases in the one
Dmitriy Kazachkov (e1403c)     29/06/2006     TLSbo61895   Rework after MU message format changing
Sergey Yakubenko               04/10/2007     ENGR42513    Fixed adress

====================================================================================================
Portability: Indicate if this module is portable to other compilers or platforms.
             If not, indicate specific reasons why is it not portable.

==================================================================================================*/


/*==================================================================================================
                                        INCLUDE FILES
==================================================================================================*/
/* Standard Include Files */
#include <errno.h>
#include <sys/types.h>          /* open()                               */
#include <sys/stat.h>           /* open()                               */
#include <fcntl.h>              /* open()                               */
#include <unistd.h>             /* close(), read(), write(), getopt()   */
#include <stdio.h>              /* sprintf(), fprintf()                 */
#include <string.h>             /* strcpy(), strlen(), memset()         */
#include <stdlib.h>             /* atoi() */
//#include <linux/config.h>

#ifdef __cplusplus
extern "C"{
#endif

/* Harness Specific Include Files. */
#include "test.h"
#ifdef __cplusplus
}

#endif

/* Verification Test Environment Include Files */
#include "mu_api_rw_test.h"

/*==================================================================================================
                                        LOCAL MACROS
==================================================================================================*/


/*==================================================================================================
                          LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
==================================================================================================*/


/*==================================================================================================
                                       LOCAL CONSTANTS
==================================================================================================*/
const char *mu_dir = "/dev/mxc_mu";

/*==================================================================================================
                                       LOCAL VARIABLES
==================================================================================================*/


/*==================================================================================================
                                       GLOBAL CONSTANTS
==================================================================================================*/


/*==================================================================================================
                                       GLOBAL VARIABLES
==================================================================================================*/


/*==================================================================================================
                                   LOCAL FUNCTION PROTOTYPES
==================================================================================================*/
int mu_write(int dev, int* val);
int mu_read( int dev, int* val);
/*==================================================================================================
                                       LOCAL FUNCTIONS
==================================================================================================*/
int mu_write(int dev, int* val)
{
        int i, ret;
        unsigned char buf[4];

        for(i=3;i>=0;i--)
        {
                buf[i] = *val>>(i*8);
        }

        ret  = write(dev, buf, 4);

        if ( ret < 0)
        {
                tst_resm(TFAIL, "Write failed: %s.", strerror(errno));
                return TFAIL;
        }
        else if (ret != 4)
        {
                tst_resm(TFAIL, "Wrote %d bytes to MU instead of 4. Error is %s", ret, strerror(errno));
                return TFAIL;
        }

        tst_resm(TPASS, "Wrote %0x to dev %d with success", *val, dev );

        return TPASS;
}

int mu_read( int dev, int* val)
{
        int i,ret;
        unsigned char buf[4];

        ret = read(dev, buf, 4);
        *val = 0;

        if ( ret < 0)
        {
            if (errno == EAGAIN )
            {
                tst_resm(TFAIL, "No data ready for now.");
                return TFAIL;
            }
            else
            {
                tst_resm(TFAIL, "Read failed: %s.", strerror(errno));
                return TFAIL;
            }
        }
        else if (ret != 4)
        {
                tst_resm(TFAIL, "Read %d bytes from MU instead of %d. Error is %s", ret, 4, strerror(errno));
                return TFAIL;
        }

        for(i=3;i>=0;i--)
        {
                *val |= buf[i]<<(i*8);
        }

        tst_resm(TPASS, "Read %0x from device %d with success", *val, dev );

        return TPASS;
}


/*================================================================================================*/
/*===== VT_mu_api_rw_setup =====*/
/**
@brief  assumes the pre-condition of the test case execution

@param  None

@return On success - return TPASS
        On failure - return the error code
*/
/*================================================================================================*/
int VT_mu_api_rw_setup(void)
{

    return TPASS;
}


/*================================================================================================*/
/*===== VT_mu_api_rw_cleanup =====*/
/**
@brief  assumes the post-condition of the test case execution

@param  None

@return On success - return TPASS
        On failure - return the error code
*/
/*================================================================================================*/
int VT_mu_api_rw_cleanup(void)
{

    return TPASS;
}


/*================================================================================================*/
/*===== VT_mu_api_rw_test =====*/
/**
@brief  Sequentally opens each device in blocking mode, then reads from its register,
        then writes to its register, then closes the device.

@param  Input :  msg - pointer to the message that will be written to the MU transmit registers
                 blk - flag indicating use of non-blocking mode
        Output:  None

@return On success - return TPASS
        On failure - return the error code
*/
/*================================================================================================*/
int VT_mu_api_rw_test(char *msg, int blk)
{
    int  rv = TPASS,
         idx,
         i,
    //   fd_0,
         mu_fd[NUM_DEVS],                 /* Device file descriptors */
         open_flags;
    char mu_dev[32];
    //unsigned char testbuf[4];
    unsigned int readreq, writereq;

#define WRITEREQ 1
#define READREQ  2
#if defined(CONFIG_MACH_MXC27520EVB) || defined(CONFIG_MACH_MXC30020EVB) || defined(CONFIG_MACH_MXC30031ADS) 
       unsigned int address = 0x10000100;
#else
       unsigned int address = 0xff002800;
#endif

/*
* at the moment the address is the same for all platforms
* but probably one day you will have to use something like
* #ifdef CONFIG_ARCH_MXC91231
*       unsigned int address = 0xff002800;
* #endif
*/
    unsigned int bufw[BUF_LEN/4], bufr[BUF_LEN/4];
    int msglen;

    msglen = strlen(msg);
    if(msglen<4)
    {
        tst_resm(TFAIL, "The message %s is too short: %d bytes (must be at least 4)", msg, msglen);
        rv = TFAIL;
        return rv;
    }

    if(msglen>BUF_LEN)
        msglen=BUF_LEN;

    memcpy(bufw, msg, msglen);
    tst_resm(TINFO, "Writing  %d bytes to MU", msglen);

    /* in fact we count 32-bit words  */
    msglen/=4;

#if defined(CONFIG_MACH_MXC27520EVB)
    /* Open in NON_BLOCK for dummy read */
    int fd_0;
    unsigned char testbuf[4];
    fd_0 = open("/dev/mxc_mu/0", O_RDWR | O_NONBLOCK);
    read(fd_0, testbuf, 4);
    close(fd_0);
#endif

    open_flags = blk? O_RDWR|O_NONBLOCK : O_RDWR ;


    for (idx = 0; idx < NUM_DEVS; idx++)
    {
        sprintf(mu_dev, "%s/%d", mu_dir, idx);

        /* Open the device */
        tst_resm(TINFO, "Opening device %s", mu_dev);

        if ( (mu_fd[idx] = open(mu_dev, open_flags)) < 0)
        {
                tst_resm(TFAIL, "Can't open MU device %s: %s", mu_dev, strerror(errno));
                rv = TFAIL;
        }
        else
        {
                tst_resm(TPASS, "Opened - the handle is %d", mu_fd[idx]);
        }
    }

    if(rv == TPASS)
    {
        /*
         * First test - write to registers
         */

        tst_resm(TINFO, "Writing to MU device %s (%d bytes)",msg, msglen*4);

        /* preparing a request */
        writereq = msglen<<16 | WRITEREQ;
        rv |= mu_write(mu_fd[0],(int*)&writereq);
        rv |= mu_write(mu_fd[1],(int*)&address);
        idx = 2;

        /* write a message */

        for(i=0;i<msglen;i++)
        {        
            rv |= mu_write(mu_fd[idx++%NUM_DEVS],(int*)&bufw[i]);
        }
        /* now check the status */
        rv |= mu_read(mu_fd[0],(int*)&writereq);
        if( (writereq & 0xff) != WRITEREQ  || (writereq>>16) != msglen )
        {
                tst_resm(TFAIL, "Error writing message to MU: response is %x", writereq);
                tst_resm(TFAIL, "Expected response is %x",msglen<<16 | WRITEREQ);
                rv = TFAIL;
        }

        /*
         * Second test - read from registers
         */
        tst_resm(TINFO, "Reading from MU device %d bytes",  msglen*4);

        /* preparing a request */
        readreq = msglen<<16 | READREQ;
        rv |= mu_write(mu_fd[0],(int*)&readreq);
        rv |= mu_write(mu_fd[1],(int*)&address);

        /* get the safe mode status */
        rv |= mu_read(mu_fd[0],(int*)&readreq); 
    if( readreq!= 0)
        {
                tst_resm(TFAIL, "Error reading responce from MU:  %x", readreq);
                rv = TFAIL;
        }


        idx = 1;
        /* read a message */
        for(i=0;i<msglen;i++)
                rv |= mu_read(mu_fd[idx++%NUM_DEVS],(int*)&bufr[i]);


        /* now check the status */
        rv |= mu_read(mu_fd[0],(int*)&readreq);
        if( (readreq & 0xff) != READREQ  || (readreq>>16) != msglen)
        {
                tst_resm(TFAIL, "Error reading message from MU: response is %x", readreq);
                tst_resm(TFAIL, "Expected response is %x",msglen<<16 | READREQ);
                rv = TFAIL;
        }



        /*
         * Third test: compare buffers
         */
        tst_resm(TINFO, "Comparing messages");
        if (memcmp(bufw, bufr, msglen))
        {
                tst_resm(TFAIL, "Messages are not the same");
                rv = TFAIL;
        }
        else
        {
        	tst_resm(TPASS, "Messages are the same");
        }
    }


    for (idx = 0; idx < NUM_DEVS; idx++)
    {
        sprintf(mu_dev, "%s/%d", mu_dir, idx);

        /* Open the device */
        tst_resm(TINFO, "Closing device %s", mu_dev);
        if ( close(mu_fd[idx]) < 0)
        {
                tst_resm(TFAIL, "Can't close MU device %s: %s", mu_dev, strerror(errno));
                rv = TFAIL;
        }
    }


    return rv;
}
