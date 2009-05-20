/*/
  Copyright (C) 2004, Freescale Semiconductor, Inc. All Rights Reserved
  THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
  BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
  Freescale Semiconductor, Inc.
*/

/**
@file mu_sdma_ipc_test.c

@brief VTE C source file of mu_sdma_ipc_test application.

Description of the file

@par Portability:
        Indicate if this module is portable to other compilers or platforms.
        If not, indicate specific reasons why is it not portable.
*/

/*======== REVISION HISTORY ========

Author (core ID)          Date         CR Number    Description of Changes
-----------------------   ----------   ----------   ----------------------------
I. Semenchukov/smng001c   09/03/2005   TLSbo47942   Initial version
Y. Batrakov               01/09/2006   TLSbo75877   Rework to write data
                                                    according to the MU protocol

==================*/

#ifdef __cplusplus
extern "C"{
#endif

/*======== INCLUDE FILES ========*/
/* Standard Include Files */

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <time.h>
#include <errno.h>
#include <pthread.h>
#include <string.h>
//#include <linux/config.h>

#include <mxc_sdma_tty.h>

/* Harness Specific Include Files. */

#include "test.h"

/* Verification Test Environment Include Files */

#include "mu_sdma_ipc_test.h"

/*======== LOCAL CONSTANTS ==========*/
const char* log_fname = "/tmp/mu_sdma_ipc_test.log";

/*======== LOCAL MACROS =========*/


/*======== LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS) ===*/

/*======== LOCAL VARIABLES ==========*/
FILE* stdlog = NULL;      /* Log file contains more verbose info about test results */
int   iter;
struct termios init_parm;

/*======== GLOBAL CONSTANTS =========*/


/*======== GLOBAL VARIABLES =========*/


/*======== LOCAL FUNCTION PROTOTYPES ========*/
void* ipc_thread_func(void* thread_arg);
int   dev_node_init(dev_node_t* node, int is_sdma);
int   dev_node_cleanup(dev_node_t* node, int is_sdma);
int   dev_node_write(dev_node_t* node, int is_sdma);
int   dev_node_read(dev_node_t* node, int is_sdma);
int   mu_write(int dev, int* val);
int   mu_read( int dev, int* val);

/*======== LOCAL FUNCTIONS ==========*/


/*======== GLOBAL FUNCTIONS =========*/

/*= VT_mu_sdma_ipc_setup =*/
/**
Description of the function
@brief  assumes the pre-condition of the test case execution
@pre None
@post None
@param  Input : None.
        Output: None.
@return On success - return TPASS
        On failure - return the error code
@bug No bug
@deprecated Not applicable
@todo Nothing
*/
int VT_mu_sdma_ipc_setup(void)
{
    int rv = TFAIL;

    if ( (stdlog = fopen(log_fname, "w")) == NULL )
        tst_resm(TCONF, "Cannot open log file %s for writing", log_fname);
    else
        rv = TPASS;

    return rv;
}

/*= VT_mu_sdma_ipc_cleanup =*/
/**
Description of the function
@brief  assumes the post-condition of the test case execution
@pre None
@post None
@param  Input : None.
        Output: None.
@return On success - return TPASS
        On failure - return the error code
@bug No bug
@deprecated Not applicable
@todo Nothing
*/
int VT_mu_sdma_ipc_cleanup(void)
{
    int rv = TPASS;

    if (stdlog)
        fclose(stdlog);

    return rv;
}

/*= VT_mu_sdma_ipc_test =*/
/**
@brief  Initializes variables and creates two test threads, then waits
        when they complete their work.

@param  num_iter     - number of write/read cycles
        sdma_bufsize - SDMA buffer size
        mu_fname     - MU device file name
        sdma_fname   - SDMA device file name

@return On success - return TPASS
        On failure - return the error code
*/
int VT_mu_sdma_ipc_test(int num_iter, dev_node_t* dev_node)
{
    /* mu device related defines */
    #define WRITEREQ 1
    #define READREQ  2

    pthread_t  th_id[2];
    int        rv = TFAIL;
    int     fd_0;
    unsigned char testbuf[4];

#if defined(CONFIG_MACH_MXC27520EVB)
  /* Open in NON_BLOCK for dummy read */
  fd_0 = open("/dev/mxc_mu/0", O_RDWR | O_NONBLOCK);
  read(fd_0, testbuf, 4);
  close(fd_0);
#endif

    iter = num_iter;

    /* Initialize structure members */

    dev_node[0].test_res = dev_node[1].test_res = TFAIL;
    dev_node[0].thread_index = 0;
    dev_node[1].thread_index = 1;

    if ( (pthread_create(&th_id[0], NULL, ipc_thread_func, (void*)&dev_node[0]) != 0) ||
         (pthread_create(&th_id[1], NULL, ipc_thread_func, (void*)&dev_node[1]) != 0) )
        tst_resm(TWARN, "Can't create thread");

    pthread_join(th_id[0], NULL);
    pthread_join(th_id[1], NULL);

    /* Test PASSED if all threads finish with successful result */

    if (dev_node[0].test_res == TPASS && dev_node[1].test_res == TPASS)
        rv = TPASS;

    return rv;
}

/*= ipc_thread_func =*/
/**
@brief  Calls device init function, then calls write and read functions 'iter' times,
        then calls device cleanup function. Tracks device test status.

@param  thread_arg - void pointer to thread argument

@return NULL
*/
void* ipc_thread_func(void* thread_arg)
{
    dev_node_t *node;

    if (!thread_arg)
        tst_resm(TWARN, "Thread %d: invalid argument", node->thread_index);
    else
    {
        int is_sdma = 0;
        int result;
        int index;

        node = (dev_node_t*)thread_arg;
        if (strstr(node->dev_prefix, SDMA_SIGN) != NULL) /* Will perform additional ops for SDMA device */
            is_sdma = 1;

        result = dev_node_init(node, is_sdma);

        /* Main write/read cycle */

        for (index = 0; (index < iter) && result; ++index)
        {
            int buf_idx;

            tst_resm(TINFO, "Thread %d: iteration #%d is started", node->thread_index, index + 1);
            fprintf(stdlog, "Thread %d: iteration #%d is started\n", node->thread_index, index + 1);
            result = dev_node_write(node, is_sdma);
            if (result)
                result = dev_node_read(node, is_sdma);

            if (!is_sdma)
                continue;

            /* Compare actual buffer sizes at first, then, if they are equal, compare, their contents */

            if (node->bytes_written != node->bytes_read)
            {
                tst_resm(TWARN, "Thread %d: SDMA buffers' length don't match", node->thread_index);
                result = FALSE;
            }
            for (buf_idx = 0; (buf_idx < node->bytes_read) && result; ++buf_idx)
            {
                if (node->wr_buf[buf_idx] != node->rd_buf[buf_idx])
                {
                    tst_resm(TWARN, "Thread %d: SDMA buffers don't match at offset %d: read: %2X, write: %2X",
                             node->thread_index, buf_idx, node->rd_buf[buf_idx], node->wr_buf[buf_idx]);
                    result = FALSE;
                    break;
                }
            }
        } /* for (index...) */

        node->test_res = (result == TRUE) ? TPASS : TFAIL;

        dev_node_cleanup(node, is_sdma);
    }    /* if (!thread_arg) ... else ... */

    return NULL;
}

/*= dev_node_init =*/
/**
@brief  Opens a device file and allocates space for read/write buffers.
        Also for SDMA device sets its TTY mode and turns on loopback mode.
        Writes appropriate info to log file.

@param  node    - pointer to the device node structure
        is_sdma - flag which is set when device is an SDMA device

@return On success - return TRUE
        On failure - return FALSE
*/
int dev_node_init(dev_node_t* node, int is_sdma)
{
    int ret = TRUE;
    int i;
    char device[256];

    if (!node)
    {
        tst_resm(TWARN, "dev_node_init(): invalid parameter");
        ret = FALSE;
    }
    else
    {
        for (i = 0; i < node->num_devs; i++)
        {
            sprintf(device, "%s%d", node->dev_prefix, i + node->first_dev);

            if ( (node->dev_handle[i] = open(device, O_RDWR)) < 0 )
            {
                tst_resm(TFAIL, "Can't open device %s: %s", device, strerror(errno));
                ret = FALSE;
            }

            if (!ret)
            {
                /* No need to continue */
                return ret;
            }
        }

        if ( (node->wr_buf = (char*)malloc(node->buf_size * 2)) == NULL )
        {
            tst_resm(TFAIL, "Can't allocate buffer for devices %s?: %s", node->dev_prefix, strerror(errno));
            ret = FALSE;
        }
        else
        {
            node->rd_buf = node->wr_buf + node->buf_size;
        }
    }

    if (ret && is_sdma)
    {
        struct termios t_parm;

        for (i = 0; i < node->num_devs; i++)
        {
            if (tcgetattr(node->dev_handle[i], &init_parm) < 0)
            {
                tst_resm(TWARN, "Can't get terminal attrs for %s%d: %s", node->dev_prefix, node->first_dev + i, strerror(errno));
                ret = FALSE;
            }
            else
            {
                t_parm = init_parm;
                t_parm.c_cflag = t_parm.c_lflag = t_parm.c_iflag = t_parm.c_oflag = 0;
                if (tcsetattr(node->dev_handle[i], TCSANOW, &t_parm) < 0)
                {
                    tst_resm(TWARN, "Can't set terminal attrs for %s%d: %s", node->dev_prefix, node->first_dev + i, strerror(errno));
                    ret = FALSE;
                }
                else
                    fprintf(stdlog, "=== Device %s%d: TTY parameters are set ===\n", node->dev_prefix, node->first_dev + i);
            }
        }
    }

    if (ret && is_sdma)
    {
        for (i = 0; i < node->num_devs; i++)
        {
            if (ioctl(node->dev_handle[i], TIOCLPBACK, &is_sdma) < 0)
            {
                tst_resm(TWARN, "Can't turn on SDMA loopback mode for %s%d: %s", node->dev_prefix, node->first_dev + i, strerror(errno));
                ret = FALSE;
            }
            else
                fprintf(stdlog, "=== Device %s%d: loopback mode is turned on ===\n", node->dev_prefix, node->first_dev + i);
        }
    }

    return ret;
}

/*= dev_node_cleanup =*/
/**
@brief  Closes a device file and deallocates read/write buffers' space.
        Also for SDMA device restores its TTY mode and loopback mode.
        Writes appropriate info to log file.

@param  node    - pointer to the device node structure
        is_sdma - flag which is set when device is an SDMA device

@return On success - return TRUE
        On failure - return FALSE
*/
int dev_node_cleanup(dev_node_t* node, int is_sdma)
{
    int ret = TRUE;
    int i;

    if (!node)
    {
        tst_resm(TWARN, "dev_node_cleanup(): invalid parameter");
        ret = FALSE;
    }

    if (ret && node->wr_buf)
        free(node->wr_buf);

    if (ret && node->dev_handle[0] >= 0)
    {
        if (is_sdma)
        {
            for (i = 0; i < node->num_devs; i++)
            {
                int arg = !is_sdma;
                if (ioctl(node->dev_handle[i], TIOCLPBACK, &arg) < 0)
                {
                    tst_resm(TWARN, "Can't turn off SDMA loopback mode for %s%d: %s", node->dev_prefix, node->first_dev + i, strerror(errno));
                    ret = FALSE;
                }
                else
                    fprintf(stdlog, "=== Device %s%d: loopback mode is turned off ===\n", node->dev_prefix, node->first_dev + i);

                if (tcsetattr(node->dev_handle[i], TCSANOW, &init_parm) < 0)
                {
                    tst_resm(TWARN, "Can't restore terminal attrs for %s%d: %s", node->dev_prefix, node->first_dev + i, strerror(errno));
                    ret = FALSE;
                }
                else
                    fprintf(stdlog, "=== Device %s%d: TTY parameters are restored ===\n", node->dev_prefix, node->first_dev + i);
            }
        }

        for (i = 0; i < node->num_devs; i++)
        {
            if (close(node->dev_handle[i]) < 0)
            {
                tst_resm(TWARN, "Can't close device %s%d: %s", node->dev_prefix, node->first_dev + i, strerror(errno));
                ret = FALSE;
            }
            else
            {
                fprintf(stdlog, "=== Device %s%d is closed ===\n", node->dev_prefix, node->first_dev + i);
            }
        }
    }

    return ret;
}

/*= dev_node_write =*/
/**
@brief  Fills write buffer by random values. Writes buffer contents to the device.
        Writes appropriate info to log file.

@param  node - pointer to the device node structure
        is_sdma - flag indicating if the device is SDMA device

@return On success - return TRUE
        On failure - return FALSE
*/
int dev_node_write(dev_node_t* node, int is_sdma)
{
    int ret = TRUE;

    if (!node)
    {
        tst_resm(TWARN, "dev_node_write(): invalid parameter");
        ret = FALSE;
    }

    if (ret)
    {
        int index;
        int rand_val;
        int msglen, writereq, rv;
#if defined(CONFIG_MACH_MXC27520EVB)
       unsigned int address = 0x10000100;
#else
       unsigned int address = 0xff002800;
#endif
        unsigned int seed = (unsigned int)time(NULL);

        srand(seed);
        for (index = 0; index < node->buf_size; ++index)
        {
            rand_val = rand();
            node->wr_buf[index] = ((char*)&rand_val)[0];
        }

        if (!is_sdma) /* MU device */
        {
            /* Look mu_api_rw test for details */

            /* preparing a request */
            msglen = node->buf_size / 4;          /* in fact we count 32-bit words  */
            writereq = msglen<<16 | WRITEREQ;
            rv |= mu_write(node->dev_handle[0],&writereq);
            rv |= mu_write(node->dev_handle[1],&address);

            /* writing a message to the registers, beginning from the 2nd */
            for(index = 0; index < msglen; index++)
                rv |= mu_write(node->dev_handle[(index+2) % node->num_devs], (int*) &node->wr_buf[index]);

            /* now check the status */
            rv |= mu_read(node->dev_handle[0], &writereq);
            if( (writereq & 0xff) != WRITEREQ  || (writereq>>16) != msglen )
            {
                tst_resm(TFAIL, "Error writing message to MU: response is %x", writereq);
                tst_resm(TFAIL, "Expected response is %x",msglen<<16 | WRITEREQ);
                rv = TFAIL;
            }
            else
            {
                tst_resm(TPASS, "Wrote to MU registers with success");
            }
        }
        else
        {
            for (index = 0; index < node->num_devs; index++)
            {
                if ( (node->bytes_written = write(node->dev_handle[index], node->wr_buf, node->buf_size)) < 0 )
                {
                    tst_resm(TWARN, "Can't write to SDMA device %s%d: %s", node->dev_prefix, index, strerror(errno));
                    ret = FALSE;
                }
                else
                {
                    fprintf(stdlog, "=== Device %s%d, iteration #%d: %d bytes are written ===\n",
                            node->dev_prefix, node->first_dev + index, iter + 1, node->bytes_written);
                }
            }
        }
    }

    return ret;
}

/*= dev_node_read =*/
/**
@brief  Reads information from the device file to the read buffer.
        Writes appropriate info to log file.

@param  node - pointer to the device node structure
        is_sdma - flag indicating if the device is SDMA device

@return On success - return TRUE
        On failure - return FALSE
*/
int dev_node_read(dev_node_t* node, int is_sdma)
{
    int ret = TRUE;
    int msglen, readreq, rv;
#if defined(CONFIG_MACH_MXC27520EVB)
       unsigned int address = 0x10000100;
#else
       unsigned int address = 0xff002800;
#endif
    int index;

    if (!node)
    {
        tst_resm(TWARN, "dev_node_read(): invalid parameter");
        ret = FALSE;
    }
    if (ret)
    {
        if (!is_sdma)
        {
            /* preparing a request */
            readreq = msglen<<16 | READREQ;
            rv |= mu_write(node->dev_handle[0], &readreq);
            rv |= mu_write(node->dev_handle[1], &address);

            /* get the safe mode status */
            rv |= mu_read(node->dev_handle[0], &readreq);
            if( readreq!= 0)
            {
                tst_resm(TFAIL, "Error reading response from MU:  %x", readreq);
                rv = TFAIL;
            }

            /* read a message starting from the first register */
            for(index = 0 ; index < msglen; index++)
                rv |= mu_read(node->dev_handle[(index+1) % node->num_devs], (int*) &node->rd_buf[index]);


            /* now check the status */
            rv |= mu_read(node->dev_handle[0], &readreq);
            if( (readreq & 0xff) != READREQ  || (readreq>>16) != msglen)
            {
                tst_resm(TFAIL, "Error reading message from MU: response is %x", readreq);
                tst_resm(TFAIL, "Expected response is %x",msglen<<16 | READREQ);
                rv = TFAIL;
            }
        }
        else
        {
            for (index = 0; index < node->num_devs; index++)
            {
                if ( (node->bytes_read = read(node->dev_handle[index], node->rd_buf, node->buf_size)) < 0 )
                {
                    tst_resm(TWARN, "Can't read from the device %s%d: %s", node->dev_prefix, node->first_dev + index, strerror(errno));
                    ret = FALSE;
                }
                else
                {
                    fprintf(stdlog, "=== Device %s%d, iteration #%d: %d bytes are read ===\n",
                            node->dev_prefix, node->first_dev + index, iter + 1, node->bytes_read);
                }
            }
        }
    }

    return ret;
}

/*= dev_node_read =*/
/**
@brief  Reverts bytes in 4-byte word and writes it to the MU device.

@param  dev - MU device file descriptor
        val - pointer to 4-byte word to write

@return On success - return TRUE
        On failure - return FALSE
*/
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

/*= dev_node_read =*/
/**
@brief  Reads 4-byte word from MU device and reverts it.

@param  dev - MU device file descriptor
        val - pointer to 4-byte word to read

@return On success - return TRUE
        On failure - return FALSE
*/
int mu_read(int dev, int* val)
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

#ifdef __cplusplus
}
#endif

