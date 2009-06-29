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

        @brief  Test scenario C source template.
====================================================================================================
Revision History:
                            Modification     Tracking
Author/core ID                  Date          Number    Description of Changes
-------------------------   ------------    ----------  -------------------------------------------
RB657C/gsch1c                20/07/2004     TLSbo40898  Initial version  of OSS sound driver test development
I.Inkina/nknl001             18/10/2005     TLSbo53864  The error management was added
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

/* Harness Specific Include Files. */
#include <test.h>

/* Verification Test Environment Include Files */
#include "oss_sound_driver_test.h"
#include "../common.h"

/*==================================================================================================
                                        LOCAL FUNCTIONS
==================================================================================================*/
#define BUF_SIZE 32767

/*================================================================================================*/
/*===== VT_oss_sound_driver_setup =====*/
/**
@brief  assumes the pre-condition of the test case execution

@param  None
    
@return On success - return TPASS
        On failure - return the error code
*/
/*================================================================================================*/
int VT_oss_sound_driver_setup(void)
{
        return TPASS;
}

/*================================================================================================*/
/*===== VT_oss_sound_driver_cleanup =====*/
/**
@brief  assumes the post-condition of the test case execution

@param  None
    
@return On success - return TPASS
        On failure - return the error code
*/
/*================================================================================================*/
int VT_oss_sound_driver_cleanup(void)
{
        return TPASS;
}

/*================================================================================================*/
int write_wav_header(FILE * fd_file, int *totalread, unsigned short *chan,
                     int *frequency, unsigned short *bits)
{
        int     rv = TFAIL;
        char    riff[] = { 'R', 'I', 'F', 'F' };
        char    wave_fmt[] = { 'W', 'A', 'V', 'E', 'f', 'm', 't', ' ' };
        char    data[] = { 'd', 'a', 't', 'a' };
        int     file_size = *totalread + 44;
        int     byte_rate = *frequency * (*bits / 8);
        int     temp;

        if (fseek(fd_file, 0, SEEK_SET) < 0)
                goto er;
        if (fwrite(riff, 4, 1, fd_file) <= 0)
                goto er;        /* pos 0 */
        if (fwrite(&file_size, 4, 1, fd_file) <= 0)     /* pos 4 */
                goto er;
        if (fwrite(wave_fmt, 8, 1, fd_file) <= 0)       /* pos 8 */
                goto er;
        temp = 0x00000010;
        if (fwrite(&temp, 4, 1, fd_file) <= 0)  /* pos 16 */
                goto er;
        temp = 0x0001;
        if (fwrite(&temp, 2, 1, fd_file) <= 0)  /* pos 20 */
                goto er;
        if (fwrite(chan, 2, 1, fd_file) <= 0)   /* pos 22 */
                goto er;
        if (fwrite(frequency, 4, 1, fd_file) <= 0)      /* pos 24 */
                goto er;
        if (fwrite(&byte_rate, 4, 1, fd_file) == 0)     /* pos 28 */
                goto er;
        temp = 0x0004;
        if (fwrite(&temp, 2, 1, fd_file) == 0)  /* pos 32 */
                goto er;
        if (fwrite(bits, 2, 1, fd_file) == 0)   /* pos 34 */
                goto er;
        if (fwrite(data, 4, 1, fd_file) == 0)   /* pos 36 */
                goto er;
        if (fwrite(totalread, 4, 1, fd_file) == 0)      /* pos 40 */
                goto er;

        rv = TPASS;

er:     return rv;
}

/*================================================================================================*/
/*===== VT_oss_sound_driver_test =====*/
/**
@brief  Test program

@param  None
    
@return On success - return TPASS
        On failure - return the error code
*/
/*================================================================================================*/
int VT_oss_sound_driver_test(char *file1, int source, int toread, int chan,
                             int speed, int bits)
{
        int     ret = TFAIL;
        int     VT_rv = TPASS;
        int     fd_audio,
                n,
                totalread;
        FILE   *fd_file;
        unsigned short buf[BUF_SIZE];
        int     tmp,
                frequency;
        int     stmp,
                channels,
                format;
        int     err = 0;
        unsigned short l_us1,
                l_us2;

        fd_file = NULL;
        fd_audio = -1;

        if ((fd_audio = open("/dev/sound/dsp", O_RDONLY)) < 0)
        {
                tst_resm(TFAIL, "Error opening %s: %s", CODEC_DEV, strerror(errno));
                return ret;
        }
        tst_resm(TINFO, "File descriptor fd_audio = %x", fd_audio);

        fd_file = fopen(file1, "w+");
        if (fd_file == NULL)
        {
                tst_resm(TFAIL, "Error opening  file %s: %s", file1, strerror(errno));
                err++;
                VT_rv = TFAIL;
                goto _err_file_not_found;
        }

        stmp = format = bits;
        if (ioctl(fd_audio, SNDCTL_DSP_SETFMT, &format) < 0)
        {
                err++;
                tst_resm(TFAIL, "Error with IOCTL (Format = %i)\n", format);
        }
        if (stmp != format)
                err++;

        tmp = frequency = speed;
        if (ioctl(fd_audio, SOUND_PCM_WRITE_RATE, &frequency) < 0)
        {
                err++;
                tst_resm(TFAIL, "Error with IOCTL (Frequence = %i)\n", frequency);
        }
        if (tmp != frequency)
                err++;

        stmp = channels = chan;
        if (ioctl(fd_audio, SOUND_PCM_WRITE_CHANNELS, &channels) < 0)
        {
                err++;
                tst_resm(TFAIL, "Error with IOCTL (Channels = %i)\n", channels);
        }
        if (stmp != channels)
                err++;

        tst_resm(TINFO, "Will rec the file with the configuration %i %i %i (err=%d)\n", format,
                 frequency, channels, err);
        if (err)
        {
                VT_rv = TFAIL;
                goto _err_exit;
        }

        tst_resm(TINFO, "File descriptor fd_audio = %x", fd_audio);

        /* ! set the output src MUST BE DONE AFTER CHANNEL SELECTION */
        switch (source)
        {
        case 1:
                tmp = SOUND_MASK_PHONEIN;
                if (ioctl(fd_audio, SOUND_MIXER_WRITE_RECSRC, &tmp) < 0)
                {
                        err++;
                        tst_resm(TFAIL, "Error with IOCTL WRITE_RECSRC (val = %i)\n", tmp);
                }
                break;
        case 2:
                tmp = SOUND_MASK_MIC;
                if (ioctl(fd_audio, SOUND_MIXER_WRITE_RECSRC, &tmp) < 0)
                {
                        err++;
                        tst_resm(TFAIL, "Error with IOCTL WRITE_RECSRC (val = %i)\n", tmp);
                }
                break;
        case 3:
                tmp = SOUND_MASK_LINE;
                if (ioctl(fd_audio, SOUND_MIXER_WRITE_RECSRC, &tmp) < 0)
                {
                        err++;
                        tst_resm(TFAIL, "Error with IOCTL WRITE_RECSRC (val = %i)\n", tmp);
                }
                break;
        default:
                tst_resm(TINFO, "Unknown out src (%d), using the default one\n", source);
        }

        stmp = channels = chan;
        if (ioctl(fd_audio, SOUND_PCM_WRITE_CHANNELS, &channels) < 0)
        {
                err++;
                tst_resm(TFAIL, "Error with IOCTL (Channels = %i)\n", channels);
        }
        if (stmp != channels)
                err++;

        tst_resm(TINFO, "File descriptor fd_audio = %x", fd_audio);

        // On passe l'entete WAV, qu'on remplira plus tard
        totalread = 0;
        memset(buf, 0, sizeof(buf));
        n = fwrite(buf, 44, 1, fd_file);
        if (n <= 0)
        {
                tst_resm(TFAIL, "Error writing");
                VT_rv = TFAIL;
                goto _err_exit;
        }
        while (toread > 0)
        {
                // Lecture de l'chantillon
                if (toread > (int) sizeof(buf))
                {
                        if ((n = read(fd_audio, buf, sizeof(buf))) < 0)
                        {
                                tst_resm(TFAIL, "Error reading audio data");
                                VT_rv = TFAIL;
                                goto _err_exit;
                        }
                }
                else
                {
                        if ((n = read(fd_audio, buf, toread)) < 0)
                        {
                                tst_resm(TFAIL, "Error reading audio data ");
                                VT_rv = TFAIL;
                                goto _err_exit;
                        }
                }

                tst_resm(TINFO, "Read %d bytes from the device", n);
                totalread += n;
                toread -= n;

                // On envoit l'chantillon...
                if ((n = fwrite(buf, 1, n, fd_file)) < 0)
                {
                        tst_resm(TFAIL, "Error writing audio data in /dev/audio");
                        VT_rv = TFAIL;
                        goto _err_exit;

                }
        }

        // On fini d'ecrire le fichier
        l_us1 = channels;
        l_us2 = format;
        if (write_wav_header(fd_file, &totalread, &l_us1, &frequency, &l_us2) < 0)
        {
                tst_resm(TFAIL, "Error data write ");
                VT_rv = TFAIL;
                goto _err_exit;
        }
        tst_resm(TINFO, "Data read = %08x\n", totalread);

_err_exit:
        tst_resm(TINFO, "The devices will close\n");
        if (fd_file)
                fclose(fd_file);
        if (fd_audio)
                close(fd_audio);

        if (err)
        {
                VT_rv = TFAIL;
                goto _end_err;
        }

        return VT_rv;
#if 0
        _err_fmt: if ( ssi_index == 2 ) close (fd_audio2); if ( fd_audio) close (fd_audio);
#endif
      _end_err:
        tst_resm(TFAIL, "Encountered %d errors\n", err);
        return VT_rv;

_err_file_not_found:
        tst_resm(TFAIL, "Error opening the file %s \n", file1);
        if (fd_audio)
                close(fd_audio);
        return VT_rv;
}
