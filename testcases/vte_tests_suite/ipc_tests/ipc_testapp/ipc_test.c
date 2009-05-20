/*===================*/
/**
        @file   ipc_test.c

        @brief  Source file for Unified IPC test application.
*/
/*======================

        Copyright (C) 2006, Freescale Semiconductor, Inc. All Rights Reserved
        THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
        BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
        Freescale Semiconductor, Inc.

====================
Revision History:
                            Modification     Tracking
Author/core ID                  Date          Number    Description of Changes
-------------------------   ------------    ----------  -------------------------------------------
I.Semenchukov/smng001c       31/03/2005     TLSbo47812  Initial version
V.Khalabuda/hlbv001          17/05/2005     TLSbo50460  Option behaviour
S.Zavjalov/zvjs001c          24/06/2005     TLSbo50997  Linked list mode
A.Ozerov/b00320              26/04/2006     TLSbo61791  Performs a cast in accordance to coding standarts
Olivier Davard/b02578        09/21/2006     TLSbo61860  Update to get working tests
A.Ozerov/b00320              11/12/2006     TLSbo84161  Minor changes.

====================
Portability: ARM GCC

======================*/

/*======================
                                        INCLUDE FILES
======================*/
/* Standard Include Files */
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>
#include <asm/arch/mxc_sdma_tty.h>

/* Harness Specific Include Files. */
#include <test.h>

/* Verification Test Environment Include Files */
#include "ipc_test.h"

/*======================
                                        DEFINES AND MACROS
======================*/
const char pm_file[] = POWER_STATE_FILE;

/*======================
                                        GLOBAL VARIABLES
======================*/
unsigned int cycles;    /* Number of write/read cycles in appropriate tests */
int     nonblock_mode;

extern char dev_fname[MAX_STR_LEN];
extern char *dev_path;
extern int dev_num;

/*======================
                                    LOCAL FUNCTION PROTOTYPES
======================*/
int     VT_ipc_dev_exch(unsigned int dev_id, unsigned int buf_size);
void    VT_ipc_rnd_fill_buffer(char *buf, unsigned int size);
int     VT_ipc_goto_pmode(unsigned int pmstate);
int     VT_ipc_invalid_dev_open(void);
int     VT_ipc_dev_open_twice(unsigned int dev_id);
int     VT_ipc_threads_tests(dev_info_t * dev_info);
int     VT_ipc_packet_data_loopback(int pkt_len, int quantity_seg);
int     VT_ipc_packet_data_cont_loopback(int pkt_len, int quantity_seg);
int     VT_ipc_lkd_tst(int pkt_len, int quantity_seg);

void   *VT_ipc_thread_func(void *arg);
int     VT_ipc_ioctl_test(unsigned int dev_id);
int     VT_ipc_pm_test(void);
int     VT_ipc_errors_test(unsigned int dev_id);

/*======================
                                        LOCAL FUNCTIONS
======================*/

/*====================*/
/*= VT_ipc_lkd_tst =*/
/**
@brief  Performs some write/read operations with IPC device defined by number

@param  pkt_len - length of package
        quantity_seg - segments quantity

@return On success - return TPASS
        On failure - return the error code
*/
/*====================*/
int VT_ipc_lkd_tst(int pkt_len, int quantity_seg)
{
        struct ioctl_args args;
        int     ret = TPASS;
        int     dev_h = 0;

        // char dev_fname[] = "/dev/test_ipc";

        tst_resm(TINFO, "Using device %s", dev_fname);
        if ((dev_h = open(dev_fname, O_RDWR)) < 0)
        {
                tst_resm(TWARN, "Cannot open device %s: %s", dev_fname, strerror(errno));
                ret = TFAIL;
        }

        memset(&args, 0, sizeof(struct ioctl_args));
        args.bytes = pkt_len;
        args.iterations = quantity_seg;
        args.vchannel = 0;

        if (ioctl(dev_h, PACKET_DATA_LINK_LOOPBACK, (unsigned long) &args) < 0)
        {
                tst_resm(TFAIL, "Cannot perform writing, error: %s", strerror(errno));
                ret = TFAIL;
        }

        close(dev_h);

        return ret;
}

/*====================*/
/*= VT_ipc_packet_data_loopback =*/
/**
@brief  Performs some write/read operations with IPC device defined by number

@param  pkt_len - length of package
        quantity_seg - segments quantity

@return On success - return TPASS
        On failure - return the error code
*/
/*====================*/
int VT_ipc_packet_data_loopback(int pkt_len, int quantity_seg)
{
        struct ioctl_args args;
        int     ret = TPASS;
        int     dev_h = 0;

        // char dev_fname[] = "/dev/test_ipc";

        tst_resm(TINFO, "Using device %s", dev_fname);
        if ((dev_h = open(dev_fname, O_RDWR)) < 0)
        {
                tst_resm(TWARN, "Cannot open device %s: %s", dev_fname, strerror(errno));
                ret = TFAIL;
        }

        memset(&args, 0, sizeof(struct ioctl_args));
        args.bytes = pkt_len;
        args.iterations = quantity_seg;
        args.vchannel = 0;

        if (ioctl(dev_h, PACKET_DATA_LOOPBACK, (unsigned long) &args) < 0)
        {
                tst_resm(TFAIL, "Cannot perform writing, error: %s", strerror(errno));
                ret = TFAIL;
        }

        close(dev_h);

        return ret;
}

/*====================*/
/*= VT_ipc_packet_data_write_ex_cont_loopback =*/
/**
@brief  Performs some write/read operations with IPC device defined by number

@param  pkt_len - length of package
        quantity_seg - segments quantity

@return On success - return TPASS
        On failure - return the error code
*/
/*====================*/
int VT_ipc_packet_data_cont_loopback(int pkt_len, int quantity_seg)
{
        struct ioctl_args args;
        int     ret = TPASS;
        int     dev_h = 0;

        // char dev_fname[] = "/dev/test_ipc";

        tst_resm(TINFO, "Using device %s", dev_fname);
        if ((dev_h = open(dev_fname, O_RDWR)) < 0)
        {
                tst_resm(TWARN, "Cannot open device %s: %s", dev_fname, strerror(errno));
                ret = TFAIL;
        }

        memset(&args, 0, sizeof(struct ioctl_args));
        args.bytes = pkt_len;
        args.iterations = quantity_seg;
        args.vchannel = 0;

        // printf("VT_ipc_packet_data_cont_loopback: length = %d, iterations = %d\n", args.bytes,
        // args.iterations);

        if (ioctl(dev_h, PACKET_DATA_CONT_LOOPBACK, (unsigned long) &args) < 0)
        {
                tst_resm(TFAIL, "Cannot perform writing, error: %s", strerror(errno));
                ret = TFAIL;
        }

        close(dev_h);

        return ret;
}

/*====================*/
/*= VT_ipc_dev_exch =*/
/**
@brief  Performs some write/read operations with IPC device defined by number

@param  dev_id   - IPC device number
        buf_size - read/write buffer size

@return On success - return TPASS
        On failure - return the error code
*/
/*====================*/
int VT_ipc_dev_exch(unsigned int dev_id, unsigned int buf_size)
{
        // char dev_fname[MAX_STR_LEN];
        int     dev_h = -1;
        int     i;
        int     open_mode = O_RDWR;
        char   *buffer = NULL;
        int     b_wr = 0;
        int     b_rd = 0;
        int     ret = TPASS;

        if ((dev_id == IPC_CHANNEL4) || (dev_id == IPC_CHANNEL5))
                open_mode = O_RDONLY;

        open_mode |= nonblock_mode;
        sprintf(dev_fname, "%s%d", dev_path, dev_id);

        tst_resm(TINFO, "Using device %s", dev_fname);
        if ((dev_h = open(dev_fname, open_mode)) < 0)
        {
                tst_resm(TWARN, "Cannot open device %s: %s", dev_fname, strerror(errno));
                ret = TFAIL;
        }
        // printf("dv_h open = %d\n", dev_h);

        if (ret == TPASS)
        {
                if (!(buffer = (char *) malloc(buf_size)))
                {
                        tst_resm(TWARN, "Cannot allocate space for buffer: %s", strerror(errno));
                        ret = TFAIL;
                }
        }

        if (ret == TPASS)
        {
                tst_resm(TINFO, "Performing %d write/read cycles, buffer size is %d bytes", cycles,
                         buf_size);

                for (i = 0; (i < cycles) && (ret == TPASS); ++i)
                {
                        int     wr;
                        int     rd;

                        if ((dev_id != IPC_CHANNEL4) && (dev_id != IPC_CHANNEL5))       /* Write
                                                                                        * isn't
                                                                                        * allowed */
                        {
                                VT_ipc_rnd_fill_buffer(buffer, buf_size);

                                // printf("start writing\n");

                                if ((wr = write(dev_h, buffer, buf_size)) < 0)
                                {
                                        /* Generally this isn't an error */
                                        if (!(nonblock_mode && errno == EAGAIN))
                                        {
                                                tst_resm(TWARN,
                                                         "Cannot write to device %s on cycle %d: %s",
                                                         dev_fname, i + 1, strerror(errno));
                                                ret = TFAIL;
                                        }
                                }
                                else
                                        b_wr += wr;
                        }

                        // printf("end writing\n");

                        if (ret == TPASS)
                        {

                                if ((rd = read(dev_h, buffer, 4)) < 0)
                                {
                                        /* Generally this isn't an error */
                                        if (!(nonblock_mode && errno == EAGAIN))
                                        {
                                                tst_resm(TWARN,
                                                         "Cannot read from device %s on cycle %d: %s",
                                                         dev_fname, i + 1, strerror(errno));
                                                ret = TFAIL;
                                        }
                                }
                                else
                                        b_rd += rd;
                        }
                        // printf("end reading\n");
                }
        }

        if (ret == TPASS)
                tst_resm(TINFO, "Wrote %d and read %d bytes from %s", b_wr, b_rd, dev_fname);
        if (buffer)
                free(buffer);
        if (dev_h >= 0)
        {
                // printf("VT_ipc_dev_exch: close device dev_h =%d\n", dev_h);
                close(dev_h);
        }

        // printf("VT_ipc_dev_exch: ret = %d\n", ret);
        return ret;
}

/*====================*/
/*= VT_ipc_rnd_fill_buffer =*/
/**
@brief  Fills memory that buf points to by random values.

@param  buf      - pointer to area to be filled
        buf_size - buffer size

@return None
*/
/*====================*/
void VT_ipc_rnd_fill_buffer(char *buf, unsigned int size)
{
        int     i;

        /* int randval; */

        assert(buf);
        /* srand((unsigned int) time(NULL)); for (i = 0; i < size; ++i) { randval = rand(); buf[i] =
        * ((char *) (&randval))[0]; } */

        for (i = 0; i < size; i++)
        {
                buf[i] = (char) i;
        }

}

/*====================*/
/*= VT_ipc_ioctl_test =*/
/**
@brief  Sends IPC ioctl() commands to the given device file

@param  dev_id - IPC device number

@return On success - return TPASS
        On failure - return the error code
*/
/*====================*/
int VT_ipc_ioctl_test(unsigned int dev_id)
{
        // char dev_fname[MAX_STR_LEN];
        int     dev_h;
        int     arg;
        int     ret = TPASS;

        sprintf(dev_fname, "%s%d", dev_path, dev_id);
        tst_resm(TINFO, "Using device %s", dev_fname);
        if ((dev_h = open(dev_fname, O_RDONLY)) < 0)
        {
                ret = TFAIL;
                tst_resm(TWARN, "Cannot open device %s: %s", dev_fname, strerror(errno));
        }

        if (ret == TPASS)
        {
                arg = 1;        /* Turn loopback mode on */
                // printf("VT_ipc_ioctl_test: Turn loopback mode on");
                if (ioctl(dev_h, TIOCLPBACK, &arg) < 0)
                {
                        ret = TFAIL;
                        tst_resm(TWARN, "Failed to turn on loopback mode for device %s", dev_fname);
                }
        }
        if (ret == TPASS)
        {
                arg = 0;        /* Turn loopback mode off */
                // printf("VT_ipc_ioctl_test: Turn loopback mode off");
                if (ioctl(dev_h, TIOCLPBACK, &arg) < 0)
                {
                        ret = TFAIL;
                        tst_resm(TWARN, "Failed ro turn off loopback mode for device %s",
                                 dev_fname);
                }
        }
        if (ret == TPASS)
        {
                tst_resm(TINFO, "Turning loopback mode on and off is successful");
                if (ioctl(dev_h, TIOCGETD, &arg) < 0)
                {
                        ret = TFAIL;
                        tst_resm(TWARN, "Failed to get TTY line discipline for device %s",
                                 dev_fname);
                }
        }
        if (ret == TPASS)
        {
                int     old_val = arg;  /* In order to restore TTY line discipline after check */

                arg = N_PPP;
                if (ioctl(dev_h, TIOCSETD, &arg) < 0)
                {
                        ret = TFAIL;
                        tst_resm(TWARN, "Failed to set TTY line discipline for device %s",
                                 dev_fname);
                }
                ioctl(dev_h, TIOCSETD, &old_val);
        }
        if (ret == TPASS)
                tst_resm(TINFO, "Getting and setting TTY line discipline is successful");

        if (dev_h != -1)
                close(dev_h);
        return ret;
}

/*====================*/
/*= VT_ipc_pm_test =*/
/**
@brief  Puts IPC device into various power modes, then restores active state

@param  None

@return On success - return TPASS
        On failure - return the error code
*/
/*====================*/
int VT_ipc_pm_test(void)
{
        int     state = ACTIVE_STATE;
        int     ret = TPASS;

        tst_resm(TINFO, "Using power mode state device %s", pm_file);
        if ((ret = VT_ipc_goto_pmode(state)) != TPASS)
                tst_resm(TWARN, "Cannot put device to state %d", state);
        if (ret == TPASS)
        {
                state = SUSP1_STATE;
                ret = VT_ipc_goto_pmode(state);
                if (ret != TPASS)
                        tst_resm(TWARN, "Cannot put device to state %d", state);
        }
        if (ret == TPASS)
        {
                state = SUSP2_STATE;
                ret = VT_ipc_goto_pmode(state);
                if (ret != TPASS)
                        tst_resm(TWARN, "Cannot put device to state %d", state);
        }
        if (ret == TPASS)
        {
                state = SUSP3_STATE;
                ret = VT_ipc_goto_pmode(state);
                if (ret != TPASS)
                        tst_resm(TWARN, "Cannot put device to state %d", state);
        }
        if (ret == TPASS)
        {
                state = ACTIVE_STATE;
                ret = VT_ipc_goto_pmode(state);
                if (ret != TPASS)
                        tst_resm(TWARN, "Cannot put device to state %d", state);
                else
                        tst_resm(TINFO,
                                 "Successfully put IPC device to all power modes supported by Linux");
        }

        return ret;
}

/*====================*/
/*= VT_ipc_goto_pmode =*/
/**
@brief  Opens device power state mode and writes new state to it, then
        reads state from it. If state values are identical, operation
        was performed successfully.

@param  pmstate    - number that represents power mode

@return On success - return TPASS
        On failure - return the error code
*/
/*====================*/
int VT_ipc_goto_pmode(unsigned int pmstate)
{
        FILE   *fp;
        char    rstate;
        char    wstate = '0' + pmstate;
        int     ret = TPASS;

        if ((fp = fopen(pm_file, "w+")) < 0)
        {
                ret = TFAIL;
                tst_resm(TWARN, "Cannot open PM state file %s: %s", pm_file, strerror(errno));
        }
        else
        {
                fwrite((void *) &wstate, 1, sizeof(char), fp);
                rewind(fp);
                sleep(2);
                fread((void *) &rstate, 1, sizeof(char), fp);
                if (rstate != wstate)
                        ret = TFAIL;
                fclose(fp);
        }

        return ret;
}

/*====================*/
/*= VT_ipc_errors_test =*/
/**
@brief  Tries to generate some error conditions and checks proper device driver reaction

@param  dev_id - IPC device number

@return On success - return TPASS
        On failure - return the error code
*/
/*====================*/
int VT_ipc_errors_test(unsigned int dev_id)
{
        int     ret = TPASS;

        // printf("TFAIL = %d\n", TFAIL);

        /* FIRST TEST for error: try to open an invalid device */
        if (VT_ipc_invalid_dev_open() != TPASS)
                ret = TFAIL;
        /* SECOND TEST for error: try to open device twice */
        if (VT_ipc_dev_open_twice(dev_id) != TPASS)
                ret = TFAIL;
        /* THIRD TEST for error: try to write/read short msgs with improper length */
        /* if (dev_id <= IPC_CHANNEL2) { if (VT_ipc_dev_exch(dev_id, SHORT_LEN + 1) == TFAIL)
        *
        * tst_resm(TINFO, "Got expected result: Cannot write a long msg on IPC Channels over MU");
        * else ret = TFAIL; } else { if (VT_ipc_dev_exch(dev_id, SHORT_LEN + 1) != TPASS) ret =
        * TFAIL; } */
        return ret;
}

/*====================*/
/*= VT_ipc_invalid_dev_open =*/
/**
@brief  Creates an invalid entry in /dev, then tries to open it.

@param  None

@return On success - return TPASS
        On failure - return the error code
*/
/*====================*/
int VT_ipc_invalid_dev_open(void)
{
        // char dev_fname[MAX_STR_LEN];
        struct stat dev_stat;
        int     dev_h;
        dev_t   major;
        dev_t   minor = IPC_CHANNEL5;
        int     ret = TPASS;

        sprintf(dev_fname, "%s%d", dev_path, (int) minor);
        /* We need device major number in order to create a new one */
        if (stat(dev_fname, &dev_stat) < 0)
        {
                tst_resm(TWARN, "Cannot stat() device file %s: %s", dev_fname, strerror(errno));
                ret = TFAIL;
        }
        else
        {
                major = dev_stat.st_rdev & 0xFF00;
                /* Change minor number, so we will open invalid device file. Create device file */
                ++minor;

                sprintf(dev_fname, "%s%d", dev_path, (int) minor);
                if (mknod(dev_fname, S_IFCHR | S_IRUSR | S_IWUSR, major | minor) < 0)
                {
                        tst_resm(TWARN, "Cannot create device file %s: %s", dev_fname,
                                 strerror(errno));
                        ret = TFAIL;
                }
                else
                {
                        tst_resm(TINFO, "Using device %s as invalid device", dev_fname);
                        if ((dev_h = open(dev_fname, O_RDONLY | nonblock_mode)) < 0)
                                tst_resm(TINFO,
                                         "Got expected result: cannot open invalid device %s: %s",
                                         dev_fname, strerror(errno));
                        else
                        {
                                tst_resm(TWARN,
                                         "Got UNexpected result: invalid device %s is opened",
                                         dev_fname);
                                close(dev_h);
                                ret = TFAIL;
                        }
                        unlink(dev_fname);
                }
        }

        return ret;
}

/*====================*/
/*= VT_ipc_dev_open_twice =*/
/**
@brief  Tries to open valid IPC device twice. It shouldn't been allowed according to documentation.

@param  dev_id - IPC device number

@return On success - return TPASS
        On failure - return the error code
*/
/*====================*/
int VT_ipc_dev_open_twice(unsigned int dev_id)
{
        // char dev_fname[MAX_STR_LEN];
        int     dev_h;
        int     dev_h2;
        int     ret = TPASS;

        sprintf(dev_fname, "%s%d", dev_path, dev_id);
        tst_resm(TINFO, "Trying to open device %s twice", dev_fname);
        if ((dev_h = open(dev_fname, O_RDONLY | nonblock_mode)) < 0)
        {
                tst_resm(TWARN, "Cannot open device %s: %s", dev_fname, strerror(errno));
                ret = TFAIL;
        }
        else
        {
                if ((dev_h2 = open(dev_fname, O_WRONLY | nonblock_mode)) < 0)
                        tst_resm(TINFO,
                                 "Got expected result: can't open device %s that has been already opened: %s",
                                 dev_fname, strerror(errno));
                else
                {
                        tst_resm(TWARN, "Got UNexpected result: device %s is opened twice",
                                 dev_fname);
                        close(dev_h2);
                        ret = TFAIL;
                }
                close(dev_h);
        }

        return ret;
}

/*====================*/
/*= VT_ipc_thread_func =*/
/**
@brief  Performs write/read operations with the device file defined by dev_info_t structure

@param  arg - void pointer to the dev_info_t structure containing device number, buffer size
                and error value

@return Nothing
*/
/*====================*/
void   *VT_ipc_thread_func(void *arg)
{
        dev_info_t *devp;

        // if (dev_path == NULL)
        // dev_path = DEVICE_PATH;
        assert(arg);
        devp = (dev_info_t *) arg;

        /* If this equals zero, it means that this test scenario shouldn't comm. w/this device */
        if (devp->buf_size)
        {
                tst_resm(TINFO, "Thread %u uses device %s%u", devp->dev_number, dev_path,
                         devp->dev_number);
                devp->ltp_ret = VT_ipc_dev_exch(devp->dev_number, devp->buf_size);
        }
        else
                devp->ltp_ret = TPASS;

        return NULL;
}

/*====================*/
/*= VT_ipc_threads_tests =*/
/**
@brief  Creates threads, each of which will communicate to its own device.

@param  dev_info - pointer to array of dev_info_t structures

@return On success - return TPASS
        On failure - return the error code
*/
/*====================*/
int VT_ipc_threads_tests(dev_info_t * dev_info)
{
        int     i;
        pthread_t th_id[IPC_CHANNEL5 + 1];
        int     th_ret[IPC_CHANNEL5 + 1];
        int     ret = TPASS;

        assert(dev_info);
        for (i = 0; i < IPC_CHANNEL3 + 1; i++)
        {
                if ((th_ret[i] =
                     pthread_create(&th_id[i], NULL, VT_ipc_thread_func,
                                    (void *) &dev_info[i])) != 0)
                {
                        ret = TFAIL;
                        tst_resm(TWARN, "Thread %d creation is failed: %s", i, strerror(errno));
                }
        }

        for (i = 0; i < IPC_CHANNEL3 + 1; i++)
        {
                if (!th_ret[i])
                {
                        if (pthread_join(th_id[i], NULL))
                        {
                                ret = TFAIL;
                                tst_resm(TWARN, "Waiting for thread %d termination is failed: %s",
                                         i, strerror(errno));
                        }
                        else if (dev_info[i].ltp_ret != TPASS)
                                ret = dev_info[i].ltp_ret;
                }
        }

        return ret;
}

/*====================*/
/*= VT_ipc_setup =*/
/**
@brief  assumes the pre-condition of the test case execution

@param  Input : None.
        Output: None.

@return On success - return TPASS
        On failure - return the error code
*/
/*====================*/
int VT_ipc_setup(void)
{
        int     VT_rv = TFAIL;

        VT_rv = TPASS;
        return VT_rv;
}

/*====================*/
/*= VT_ipc_cleanup =*/
/**
@brief  assumes the post-condition of the test case execution

@param  Input : None.
        Output: None.

@return On success - return TPASS
        On failure - return the error code
*/
/*====================*/
int VT_ipc_cleanup(void)
{
        return TPASS;
}

/*====================*/
/*= VT_ipc_test =*/
/**
@brief  Fills dev_info_t array by given parameters and runs appropriate test scenario

@param  test_id, dev_num, pkt_len, log_len, iter_nr, nonblk, quantity_seg.

@return On success - return TPASS
        On failure - return the error code
*/
/*====================*/
int VT_ipc_test(int test_id, int pkt_len, int log_len, int iter_nr, int nonblk, int quantity_seg)
{
        int     i;
        int     rv = TFAIL;
        dev_info_t info[IPC_CHANNEL5 + 1] = { {0} };
        unsigned int buf_sizes[IPC_CHANNEL5 + 1] =
            { SHORT_LEN, SHORT_LEN, SHORT_LEN, pkt_len, log_len, log_len };

        // printf("enter VT_ipc_test\n");

        nonblock_mode = nonblk ? O_NONBLOCK : 0;
        cycles = iter_nr;

        for (i = 0; i < IPC_CHANNEL5 + 1; i++)
        {
                info[i].dev_number = i;

                /* If buffer size equals zero, thread function skips its work and simply returns */
                if ((((test_id == PACKET_DATA_EXCHANGE_1) || (test_id == PACKET_DATA_EXCHANGE_2)
                      || (test_id == MSG_DATA_LOG_TEST)
                      || (test_id == LINKED_MODE_TEST)) && i == dev_num)
                    || (test_id == PAR_SHORT_MSG_TEST && i <= SHORT_DEV2)
                    || (test_id == PAR_DATA_LOG_TEST && i >= PKT_DEV) || (test_id == PAR_ALL_TEST))
                {
                        info[i].buf_size = buf_sizes[i];
                        // printf("VT_ipc_test: buf_size[%d] = %d\n", i, info[i].buf_size);

                }
        }

        // printf("VT_ipc_test: test_id = %d\n", test_id);

        switch (test_id)
        {
        case IOCTL_TEST:
                rv = VT_ipc_ioctl_test(dev_num);
                break;
        case PWR_MGMT_TEST:
                rv = VT_ipc_pm_test();
                break;
        case ERROR_CHK_TEST:
                rv = VT_ipc_errors_test(dev_num);
                break;
        case MSG_DATA_LOG_TEST:
                rv = VT_ipc_dev_exch(dev_num, info[dev_num].buf_size);
                break;
        case PACKET_DATA_EXCHANGE_1:
                rv = VT_ipc_packet_data_cont_loopback(pkt_len, quantity_seg);
                break;
        case PACKET_DATA_EXCHANGE_2:
                rv = VT_ipc_packet_data_loopback(pkt_len, quantity_seg);
                break;
        case PAR_SHORT_MSG_TEST:
        case PAR_DATA_LOG_TEST:
        case PAR_ALL_TEST:
                rv = VT_ipc_threads_tests(info);        /* The difference lies in the zero buffer
                                                        * sizes */
                break;
        case LINKED_MODE_TEST:
                rv = VT_ipc_lkd_tst(pkt_len, quantity_seg);
                break;
        default:
                tst_brkm(TBROK, cleanup, "ERROR: wrong test case");
        }
        // printf("end VT_ipc_test\n");

        return rv;
}
