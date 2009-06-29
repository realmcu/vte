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
        @file   tool_recorder.c

        @brief  OSS audio record test file.

====================================================================================================
Revision History:
                            Modification     Tracking
Author/core ID                  Date          Number    Description of Changes
-------------------------   ------------    ----------  -------------------------------------------
RB657C/gsch1c                29/09/2004     TLSbo43102  Initial version  of OSS sound driver test development
D.Simakov/smkd001c           06/10/2005     TLSbo53199  Syncronization was added. The code was formatted.
I.Inkina/nknl001             11/01/2005     TLSbo61044  Clock master was added
D.Khoroshev/b00313           03/03/2006     TLSbo62323  Updates according to last MXC OSS specification
D.Simakov/b00296             06/07/2006     TLSbo67022  Error in SNDCTL_CLK_SET_MASTER call fixed (PMIC master, SSI slave is 0 value)
D.Simakov                    19/10/2006     TLSbo76144  dsp->adsp, dsp1->dsp
D.Simakov                    07/12/2006     TLSbo76144  Updated with the new kernel
====================================================================================================
Portability: ARM GCC
==================================================================================================*/

/*==================================================================================================
                                        INCLUDE FILES
==================================================================================================*/
/* Standard Include Files */
#include <errno.h>
#include <pthread.h>

/* Harness Specific Include Files. */
#include <test.h>

/* Verification Test Environment Include Files */
#include "oss_sound_driver_test.h"
#include "../common.h"

/*==================================================================================================
                                        LOCAL FUNCTIONS
==================================================================================================*/
#define BUF_SIZE        4096
 // 32767
#define ARG_SRC         1
#define ARG_TYPE        2
#define ARG_BYTES       3
#define ARG_BITS        4
#define ARG_FREQ        5
#define ARG_CHAN        6

typedef struct _thrd_t
{
        int     drv_fd;
        int     sample_nb;
} thrd_t;

char    audio_buffer[4][BUF_SIZE];
int     not_reached;
char   *wr_ptr;
char   *rd_ptr;

pthread_mutex_t thread_flag_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t reached_flag_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t rw_mutex;
static int wait_for_write = 0;

/*================================================================================================*/
/**
@brief  assumes the pre-condition of the test case execution

@param  None
    
@return On success - return TPASS
        On failure - return the error code
*/
/*================================================================================================*/
int VT_oss_sound_driver_setup(void)
{
        int     rv = TFAIL;

        pthread_mutex_init(&rw_mutex, NULL);

        rv = TPASS;
        return rv;
}

/*================================================================================================*/
/**
@brief  assumes the post-condition of the test case execution

@param  None
    
@return On success - return TPASS
        On failure - return the error code
*/
/*================================================================================================*/
int VT_oss_sound_driver_cleanup(void)
{
        int     rv = TFAIL;

        pthread_mutex_destroy(&rw_mutex);

        rv = TPASS;
        return rv;
}

/*================================================================================================*/
/**
@brief  the recording thread

@param  loacal_arg - thread argument 

@return Nothing
*/
/*================================================================================================*/
void main1(thrd_t * local_arg)
{
        int     nread;
        char   *l_rd_ptr;
        int     fd_audio = local_arg->drv_fd;
        int     bytes = local_arg->sample_nb;

        bytes -= BUF_SIZE;

        tst_resm(TINFO, "Enter main1\n");

        // On lit l'échantillon...
        while (bytes > 0)
        {
                if (wait_for_write)
                        continue;

                l_rd_ptr = rd_ptr;

                pthread_mutex_lock(&rw_mutex);
                nread = read(fd_audio, l_rd_ptr, BUF_SIZE);
                pthread_mutex_unlock(&rw_mutex);

                if (nread < 0)
                        perror("write error");
                else
                        tst_resm(TINFO, "%d/%d read (%x).\n", nread, bytes, l_rd_ptr);

                bytes -= BUF_SIZE;

                /* increment reading pointer and maybe wrap */
                l_rd_ptr += BUF_SIZE;
                if (l_rd_ptr > &audio_buffer[3][0])
                        l_rd_ptr = &audio_buffer[0][0];

                rd_ptr = l_rd_ptr;
                wait_for_write = 1;
        }

        /* Exit condition for play thread */
        pthread_mutex_lock(&reached_flag_mutex);
        not_reached = 0;
        pthread_mutex_unlock(&reached_flag_mutex);
}

/*================================================================================================*/
/**
@brief  the playing thread

@param  loacal_arg - thread argument

@return Nothing
*/
/*================================================================================================*/
void main2(thrd_t * local_arg)
{
        int     ntot,
                nwritten;
        char   *l_wr_ptr;
        int     fd_audio = local_arg->drv_fd;
        int     cond;

        // On envoie l'échantillon...
        pthread_mutex_lock(&thread_flag_mutex);
        cond = not_reached;
        pthread_mutex_unlock(&thread_flag_mutex);

        tst_resm(TINFO, "Enter main2\n");
        ntot = 0;

        while (cond)
        {
                if (!wait_for_write)
                        continue;
                pthread_mutex_lock(&thread_flag_mutex);
                l_wr_ptr = wr_ptr;
                pthread_mutex_unlock(&thread_flag_mutex);

                pthread_mutex_lock(&rw_mutex);
                nwritten = write(fd_audio, l_wr_ptr, BUF_SIZE);
                pthread_mutex_unlock(&rw_mutex);

                if (nwritten < 0)
                        perror("write error");
                else
                {
                        ntot += BUF_SIZE;
                        tst_resm(TINFO, "%d/%d written (%x).\n", nwritten, ntot, l_wr_ptr);
                }

                /* Recompute writing pointer according to new reading pointer */
                pthread_mutex_lock(&thread_flag_mutex);
                wr_ptr += BUF_SIZE;
                if (wr_ptr > &audio_buffer[3][0])
                        wr_ptr = &audio_buffer[0][0];
                pthread_mutex_unlock(&thread_flag_mutex);

                pthread_mutex_lock(&reached_flag_mutex);
                cond = not_reached;
                pthread_mutex_unlock(&reached_flag_mutex);

                wait_for_write = 0;
        }
}

/*================================================================================================*/
/* ! * This function launches two threads. One for reading, one for writing
 * @param fd_audio the
 * reading device
 * @param fd_audio2 the writing device
 * @param toread the number of bytes to read
 * @return the number of errors encountered
 */
/*================================================================================================*/
int launch_full_duplex(int fd_audio, int fd_audio2, int toread)
{
        thrd_t  local_arg,
                local_arg2;
        pthread_t thread1,
                thread2;
        int     ret;

        /* Init variables */
        memset(audio_buffer, 0, 4 * BUF_SIZE);
        not_reached = 1;
        wr_ptr = &audio_buffer[0][0];
        rd_ptr = &audio_buffer[1][0];

        /* Initialize the mutex and condition variable. pthread_mutex_init (&thread_flag_mutex, NULL);*/

        tst_resm(TINFO, "Create threads 1 and 2 (read %d bytes) \n", toread);

        local_arg.drv_fd = fd_audio;
        local_arg.sample_nb = toread;
        ret = pthread_create(&thread1, NULL, (void *) &main1, (void *) &local_arg);

        local_arg2.drv_fd = fd_audio2;
        local_arg2.sample_nb = toread;
        ret = pthread_create(&thread2, NULL, (void *) &main2, (void *) &local_arg2);
        ret = pthread_join(thread1, NULL);
        ret = pthread_join(thread2, NULL);

        return ret;
}


/*================================================================================================*/
/**
@brief  executes test case.

@param  p_src - input source
        p_type - record mode
        p_bytes - number of bytes to record
        p_bits - recording bit depth
        p_speed - recording smaple rate
        p_chan - number of channels

@return Nothing
*/
/*================================================================================================*/
int VT_oss_sound_driver_test(int p_src, int p_type, int p_bytes, int p_bits, int p_speed,
                             int p_chan)
{
        int     VT_rv = TFAIL;
        int     tmp,
                frequency;
        int     stmp,
                channels,
                format;
        int     val,
                err = 0;
        int     fd_audio,
                fd_audio2,
                toread;

        /* nb of bytes to read */
        toread = p_bytes;

        /* Open driver according to scenario fd_audio will record, fd_audio will play */
        switch (p_type)
        {
        case (1):      /* use SSI1, in SSI registers directly */
                if ((fd_audio = open(CODEC_DEV, O_RDWR)) < 0)
                {
                        tst_resm(TFAIL, "Error opening %s", CODEC_DEV);
                        return VT_rv;
                }
                fd_audio2 = fd_audio;
                break;
        case (2):      /* use SSI2, in SSI registers directly */
                if ((fd_audio2 = open(CODEC_DEV, O_RDONLY)) < 0)
                {
                        tst_resm(TFAIL, "Error opening %s", CODEC_DEV);
                        return VT_rv;
                }
                if ((fd_audio = open(STDAC_DEV, O_WRONLY)) < 0)
                {
                        tst_resm(TFAIL, "Error opening %s", STDAC_DEV);
                        close(fd_audio2);
                        return VT_rv;
                }
                close(fd_audio2);
                if ((fd_audio2 = open(STDAC_DEV, O_WRONLY)) < 0)
                {
                        tst_resm(TFAIL, "Error opening %s", STDAC_DEV);
                        return VT_rv;
                }
                close(fd_audio);
                if ((fd_audio = open(CODEC_DEV, O_RDWR)) < 0)
                {
                        tst_resm(TFAIL, "Error opening %s", CODEC_DEV);
                        close(fd_audio2);
                        return VT_rv;
                }
                break;
        case (3):      /* use SSI1, full duplex in user space */
                if ((fd_audio = open(CODEC_DEV, O_RDWR)) < 0)
                {
                        tst_resm(TFAIL, "Error opening %s", CODEC_DEV);
                        return VT_rv;
                }
                fd_audio2 = fd_audio;
                break;
        case (4):      /* use SSI2, full duplex in user space */
                if ((fd_audio2 = open(CODEC_DEV, O_RDWR)) < 0)
                {
                        tst_resm(TFAIL, "Error opening %s", CODEC_DEV);
                        return VT_rv;
                }
                if ((fd_audio = open(STDAC_DEV, O_WRONLY)) < 0)
                {
                        tst_resm(TFAIL, "Error opening %s", STDAC_DEV);
                        return VT_rv;
                }
                close(fd_audio2);
                if ((fd_audio2 = open(STDAC_DEV, O_WRONLY)) < 0)
                {
                        tst_resm(TFAIL, "Error opening %s", STDAC_DEV);
                        return VT_rv;
                }
                close(fd_audio);
                if ((fd_audio = open(CODEC_DEV, O_RDWR)) < 0)
                {
                        tst_resm(TFAIL, "Error opening %s", CODEC_DEV);
                        close(fd_audio2);
                        return VT_rv;
                }
                break;
        case (5):      /* use SSI1 and SSI2, full duplex in user space */
                if ((fd_audio = open(CODEC_DEV, O_RDONLY)) < 0)
                {
                        tst_resm(TFAIL, "Error opening %s", CODEC_DEV);
                        return VT_rv;
                }

                if ((fd_audio2 = open(STDAC_DEV, O_WRONLY)) < 0)
                {
                        tst_resm(TFAIL, "Error opening %s", STDAC_DEV);
                        close(fd_audio);
                        return VT_rv;
                }
                break;
        case (6):      /* use SSI2 and SSI1, full duplex in user space */
                tst_resm(TFAIL, "Not implemented yet\n");
                return VT_rv;
        }

        /* ! set the input src */
        switch (p_src)
        {
        case 1:
                val = SOUND_MASK_PHONEIN;
                if (ioctl(fd_audio, SOUND_MIXER_WRITE_RECSRC, &val) < 0)
                {
                        err++;
                        tst_resm(TFAIL, "Error with IOCTL WRITE_RECSRC (val = %i)\n", val);
                }
                break;
        case 2:
                val = SOUND_MASK_MIC;
                if (ioctl(fd_audio, SOUND_MIXER_WRITE_RECSRC, &val) < 0)
                {
                        err++;
                        tst_resm(TFAIL, "Error with IOCTL WRITE_RECSRC (val = %i)\n", val);
                }
                break;
        case 3:
                val = SOUND_MASK_LINE;
                if (ioctl(fd_audio, SOUND_MIXER_WRITE_RECSRC, &val) < 0)
                {
                        err++;
                        tst_resm(TFAIL, "Error with IOCTL WRITE_RECSRC (val = %i)\n", val);
                }
                break;
        default:
                tst_resm(TINFO, "Unknown out src (%d), using the default one\n", p_src);
        }

        // On configure le device audio
        stmp = format = p_bits;
        if (ioctl(fd_audio, SNDCTL_DSP_SETFMT, &format) < 0)
        {
                err++;
                tst_resm(TFAIL, "Error with IOCTL (Format1 = %i)\n", format);
        }
        if (stmp != format)
                err++;
        stmp = format = p_bits;
        if (ioctl(fd_audio2, SNDCTL_DSP_SETFMT, &format) < 0)
        {
                err++;
                tst_resm(TFAIL, "Error with IOCTL (Format2 = %i)\n", format);
        }
        if (stmp != format)
                err++;

        tmp = frequency = p_speed;
        if (ioctl(fd_audio, SOUND_PCM_WRITE_RATE, &frequency) < 0)
        {
                err++;
                tst_resm(TFAIL, "Error with IOCTL (Frequence1 = %i)\n", frequency);
        }
        if (tmp != frequency)
                err++;
        tmp = frequency = p_speed;
        if (ioctl(fd_audio2, SOUND_PCM_WRITE_RATE, &frequency) < 0)
        {
                err++;
                tst_resm(TFAIL, "Error with IOCTL (Frequence2 = %i)\n", frequency);
        }
        if (tmp != frequency)
                err++;

        stmp = channels = p_chan;
        if (ioctl(fd_audio, SOUND_PCM_WRITE_CHANNELS, &channels) < 0)
        {
                err++;
                tst_resm(TFAIL, "Error with IOCTL (Channels1 = %i)\n", channels);
        }
        if (stmp != channels)
                err++;
        stmp = channels = p_chan;
        if (ioctl(fd_audio2, SOUND_PCM_WRITE_CHANNELS, &channels) < 0)
        {
                err++;
                tst_resm(TFAIL, "Error with IOCTL (Channels2 = %i)\n", channels);
        }
        if (stmp != channels)
                err++;

        tst_resm(TINFO, "Will rec the file with the configuration %i %i %i (err=%d)\n", format,
                 frequency, channels, err);
        if (err)
                goto _err_fmt;


        if (p_type <= 2)
        {
                /* Call the kernel driver loopback control */
                if (ioctl(fd_audio, SNDCTL_DBMX_HW_SSI_LOOPBACK, &toread) < 0)
                        err++;
        }
        else
        {
                /* Make a loopback at user space level */
                if (launch_full_duplex(fd_audio, fd_audio2, toread))
                        err++;
        }

        tst_resm(TINFO, "************** Closing the device\n");
        /* Close driver according to scenario */
        switch (p_type)
        {
        case (1):
        case (3):
                close(fd_audio);
                break;
        default:
                close(fd_audio);
                close(fd_audio2);
        }

        if (err)
                goto _end_err;
        VT_rv = TPASS;
        return VT_rv;

_err_fmt:
        close(fd_audio);
        close(fd_audio2);
_end_err:
        tst_resm(TFAIL, "Encountered %d errors\n", err);
        return VT_rv;
}
