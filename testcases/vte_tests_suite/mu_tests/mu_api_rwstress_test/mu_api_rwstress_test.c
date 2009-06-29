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
    @file   mu_api_rwstress_test.c

    @brief  C source file of the mu_api_rwstress_test test that performs stress tests of Messaging
            Unit driver read() and write() system calls
====================================================================================================
Revision History:
                              Modification     Tracking
Author (Core ID)                  Date          Number    Description of Changes
---------------------------   ------------    ----------  ------------------------------------------
Igor Semenchukov (smng001c)    24/08/2004     TLSbo40411   Initial version
Igor Semenchukov (smng001c)    30/08/2004     TLSbo40411   Review after inspection
Igor Semenchukov (smng001c)    09/12/2004     TLSbo43804   Rework after heavy MU driver modification
Dmitriy Kazachkov (e1403c)     29/06/2006     TLSbo61895   Rework after MU message format changing
Sergey Yakubenko               04/10/2007     ENGR42513    Fixed address

====================================================================================================
Portability: Indicate if this module is portable to other compilers or platforms.
             If not, indicate specific reasons why is it not portable.

==================================================================================================*/

#ifdef __cplusplus
extern "C"{
#endif

/*==================================================================================================
                                        INCLUDE FILES
==================================================================================================*/

/* Standard Include Files */

#include <errno.h>
#include <sys/types.h>  /* open(), fork(), waitpid() */
#include <sys/wait.h>   /* waitpid() */
#include <sys/stat.h>   /* open() */
#include <fcntl.h>      /* open() */
#include <unistd.h>     /* close(), read(), write(), getopt(), fork() */
#include <stdio.h>      /* fprintf(), perror() */
#include <string.h>     /* strcpy(), strlen() */
#include <stdlib.h>     /* exit() */
//#include <linux/config.h>

/* Harness Specific Include Files. */

#include "test.h"

/* Verification Test Environment Include Files */

#include "mu_api_rwstress_test.h"

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
int count;      /* Number of read/write cycles each child performs */
int block;      /* If non-zero, non-blocking mode will be used     */

/*==================================================================================================
                                       GLOBAL CONSTANTS
==================================================================================================*/


/*==================================================================================================
                                       GLOBAL VARIABLES
==================================================================================================*/


/*==================================================================================================
                                   LOCAL FUNCTION PROTOTYPES
==================================================================================================*/
int  read_write(char *msg, int index);
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


        return TPASS;
}



/*================================================================================================*/
/*===== VT_mu_api_rwstress_setup =====*/
/**
@brief  assumes the pre-condition of the test case execution

@param  None

@return On success - return TPASS
        On failure - return the error code
*/
/*================================================================================================*/
int VT_mu_api_rwstress_setup(void)
{
    return TPASS;
}


/*================================================================================================*/
/*===== VT_mu_api_rwstress_cleanup =====*/
/**
@brief  assumes the post-condition of the test case execution

@param  None

@return On success - return TPASS
        On failure - return the error code
*/
/*================================================================================================*/
int VT_mu_api_rwstress_cleanup(void)
{
    return TPASS;
}


/*================================================================================================*/
/*===== VT_mu_api_rwstress_test =====*/
/**
@brief  Accepts user parametes. Spawns some children and waits when all of them complete their work.
        Checks children exit status.

@param  Input : msg       - message that will be written
                blk       - flag indicating use of non-blocking mode
                rw_count  - number of read/write cycles
                num_child - number of forks parent will perform
        Output: None

@return On success - return TPASS
        On failure - return the error code
*/
/*================================================================================================*/
int VT_mu_api_rwstress_test(char *msg, int blk, int rw_count, int num_child)
{
    int   rv = TPASS,
          ret;
    int   idx,
          kidx,
          status    = 0,
          child_err = 0;
    pid_t *pid;
    //int fd_0;
    //unsigned char testbuf[4];

    block = blk;
    count = rw_count;

#if defined(CONFIG_MACH_MXC27520EVB)
    /* Open in NON_BLOCK for dummy read */
    int fd_0;
    unsigned char testbuf[4];
    fd_0 = open("/dev/mxc_mu/0", O_RDWR | O_NONBLOCK);
    read(fd_0, testbuf, 4);
    close(fd_0);
#endif

    /* Allocate memory for pid[] array */

    if ( (pid =(pid_t *)malloc(num_child)) == NULL)
    {
        printf("\tERROR: can't allocate memory with malloc(): %s\n", strerror(errno));
        return TFAIL;
    }

    for (idx = 0; idx < num_child; idx++)       /* Fork children */
    {
        pid[idx] = fork();
        if(pid[idx] < 0)
        {
            printf("\tERROR: failed to fork child %d: %s\n", idx, strerror(errno));
            for (kidx = 0; kidx < idx; kidx++)
            {
                if (pid[kidx] > 0)
                    kill(pid[kidx], SIGTERM);
            }
            free(pid);
            return TFAIL;
        }

        /* Child does read and write operations from/to MU registers and then exits. */
        if (pid[idx] == 0)
            exit(read_write(msg, idx));
    }

    /* Parent waits while all children completes their work. On error children return -1 */

    for (idx = 0; idx < num_child; idx++)
    {
        if ( (ret = waitpid(pid[idx], &status, 0)) != pid[idx])
        {
            printf("\tERROR: failed to wait child %d\n", pid[idx]);
            child_err--;
        }
        if (status != TPASS)
            child_err--;
    }

    /* End of work */

    free(pid);
    if (child_err < 0)
        rv = TFAIL;
    return rv;
}

/*================================================================================================*/
/*===== read_write =====*/
/**
@brief  Tries to open device and waits if it busy. Then, do some read/write operations depending on
        parameters supplied by user and closes device.

@param  Input : msg   - message that will be written
                index - child index in the parent fork() cycle

@return On success - return 0
        On failure - return 1
*/
/*================================================================================================*/
int read_write(char *msg, int index)
{
    int  rv = TPASS,
         idx,
         i, j,
         mu_fd[NUM_REGS],                 /* Device file descriptors */
         open_flags;
    char mu_dev[32];
    unsigned int readreq, writereq;

#define WRITEREQ 1
#define READREQ  2
#if defined(CONFIG_MACH_MXC27520EVB) || defined(CONFIG_MACH_MXC30031ADS)
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
    int noverbose=0;

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

    address+=msglen*index;

    /* in fact we count 32-bit words  */
    msglen/=4;

    open_flags = block? O_RDWR|O_NONBLOCK : O_RDWR ;


    for (idx = 0; idx < NUM_REGS; idx++)
    {
        sprintf(mu_dev, "%s/%d", mu_dir, idx);

        /* Open the device */

        do
        {
/*           tst_resm(TINFO, "[child#%d] Open device %s (%d/%d)",index+1,mu_dev, noverbose+1, OPEN_TRY);  */
            mu_fd[idx] = open(mu_dev, open_flags);
            if ( (mu_fd[idx] < 0) && (errno == EBUSY) )
            {
                noverbose++;
                if (noverbose >OPEN_TRY) /* Break if too many attempts were made to avoid hanging */
                {
                    tst_resm(TFAIL, "[child#%d] Too many tries to open busy device %s. Exiting ...\n", index + 1, mu_dev);
                    return TFAIL;
                }
                sleep(1);   /* some pause */
                tst_resm(TINFO, "[child#%d]Device %s open conflict (%d/%d) ... waiting ....",index,mu_dev, noverbose, OPEN_TRY);               
            }
            else if (mu_fd[idx] < 0)  /* Break on another error */
            {
                tst_resm(TFAIL, "[child#%d] Fails to open device %s: %s\n", index + 1, mu_dev, strerror(errno));
                return TFAIL;
            }
        } while (mu_fd[idx] < 0);


    }

    for(j=0;j<count && rv==TPASS; j++)
    {
        /*
         * First test - write to registers
         */
//        tst_resm(TINFO, "Test %d pass %d", index, j);


        /* preparing a request */
        writereq = msglen<<16 | WRITEREQ;
        rv |= mu_write(mu_fd[0],(int*)&writereq);
        rv |= mu_write(mu_fd[1],(int*)&address);
        idx = 2;

        /* write a message */
        for(i=0;i<msglen;i++)
                rv |= mu_write(mu_fd[idx++%NUM_REGS],(int*)&bufw[i]);


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
                rv |= mu_read(mu_fd[idx++%NUM_REGS],(int*)&bufr[i]);


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
        if (memcmp(bufw, bufr, msglen))
        {
                tst_resm(TFAIL, "Messages are not the same");
                rv = TFAIL;
        }
    }


    for (idx = 0; idx < NUM_REGS; idx++)
    {
        sprintf(mu_dev, "%s/%d", mu_dir, idx);

        /* Close the device */
        if ( close(mu_fd[idx]) < 0)
        {
                tst_resm(TFAIL, "Can't close MU device %s: %s", mu_dev, strerror(errno));
                rv = TFAIL;
        }
    }

    tst_resm(TPASS, "Test %d passed %d times", index, count);

    return rv;
}


#ifdef __cplusplus
}
#endif
