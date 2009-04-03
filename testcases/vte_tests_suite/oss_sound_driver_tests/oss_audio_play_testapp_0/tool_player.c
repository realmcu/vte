/*================================================================================================*/
/**
        @file   tool_player.c

        @brief  OSS Audio Play Test scenario source file.
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
RB657C/gsch1c                20/07/2004     TLSbo40898  Initial version  of OSS sound driver test development
D.Khoroshev/B00313           02/10/2006     TLSbo62323  Update according to the latest specifications
D.Simakov                    13/06/2006     TLSbo67022  STDAC <=> CODEC
D.Simakov                    19/10/2006     TLSbo76144  dsp->adsp, dsp1->dsp
====================================================================================================
Portability: ARM GCC
==================================================================================================*/

/*==================================================================================================
                                        INCLUDE FILES
==================================================================================================*/
/* Standard Include Files */
#include <errno.h>
    
/* Verification Test Environment Include Files */
#include <test.h>

/* Harness Specific Include Files. */
#include "oss_sound_driver_test.h"
#include "../common.h"

/*==================================================================================================
                                       LOCAL FUNCTIONS
==================================================================================================*/
extern int Portflag;

/*==================================================================================================
                                       LOCAL FUNCTIONS
==================================================================================================*/
#define BUF_SIZE 4096

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
/*===== get_audio_bits =====*/
unsigned short get_audio_bits(FILE* file)
{
        unsigned short ret;
        fseek(file, 34, SEEK_SET);
        fread(&ret, 2, 1, file);
        return ret;
}

/*================================================================================================*/
/*===== get_audio_channels =====*/
unsigned short get_audio_channels(FILE* file)
{
        unsigned short ret;
        fseek(file, 22, SEEK_SET);
        fread(&ret, 2, 1, file);
        return ret;
}

/*================================================================================================*/
/*===== get_audio_frq =====*/
int get_audio_frq(FILE* file)
{
        int ret;
        fseek(file, 24, SEEK_SET);
        fread(&ret, 4, 1, file);
        return ret;
}

/*================================================================================================*/
/*===== set_audio_config =====*/
int set_audio_config(int fd_audio, FILE* file)
{
        int tmp, format, frequency, channels;
        int VT_rv = TPASS;

        tmp = format = (int) get_audio_bits(file);
        if (ioctl(fd_audio, SNDCTL_DSP_SETFMT, &format) < 0)
        {
                tst_resm(TINFO,
                        "Error in SNDCTL_DSP_SETFMT ioctl call (arg is %d, returned %d). %s", 
                        tmp, format, strerror(errno));
                VT_rv = TFAIL;
        }
        else if ( tmp != format )
        {
                tst_resm(TINFO,
                        "Format was not set (SNDCTL_DSP_SETFMT, arg is %d, returned %d). %s", 
                        tmp, format);
                VT_rv = TFAIL;
        }

        tmp = frequency = get_audio_frq(file);
        if (ioctl(fd_audio, SOUND_PCM_WRITE_RATE, &frequency)< 0)
        {
                tst_resm(TINFO,
                        "Error in SOUND_PCM_WRITE_RATE ioctl call (arg is %d, returned %d). %s", 
                        tmp, frequency, strerror(errno));
                VT_rv = TFAIL;
        }
        else if ( tmp != frequency )
        {
                tst_resm(TINFO,
                        "Format was not set (SOUND_PCM_WRITE_RATE, arg is %d, returned %d). %s",
                        tmp, frequency);
                VT_rv = TFAIL;
        }

        tmp = channels = get_audio_channels(file);
        if (ioctl(fd_audio, SNDCTL_DSP_CHANNELS, &channels) < 0)
        {
                tst_resm(TINFO,
                        "Error in SNDCTL_DSP_CHANNELS ioctl call (arg is %d, returned %d). %s",
                        tmp, channels, strerror(errno));
                VT_rv = TFAIL;
        }
        else if ( tmp != channels )
        {
                tst_resm(TINFO,
                        "Format was not set (SNDCTL_DSP_CHANNELS, arg is %d, returned %d). %s",
                        tmp, channels);
                VT_rv = TFAIL;
        }

        tst_resm(TINFO, "Configuration : format %d, frequency %d, channels %d\n", format, frequency, channels);

        return VT_rv;
}

/*================================================================================================*/
/*===== play_file =====*/
int play_file(int fd_audio, FILE* file)
{
        int err = 0;
        int n, nleft, nwritten;
        char buf[BUF_SIZE], *p;

        /* On passe l'entete WAV */
        fseek(file, 44, SEEK_SET);

        /* Lecture de l'échantillon */
        while ((n = fread (buf, 1, sizeof(buf), file)) > 0)
        {
                nleft = n;
                p = buf;

                /* On envoie l'échantillon... */
                while (nleft)
                {
                        if ((nwritten = write (fd_audio, p, nleft)) < 0)
                        {
                                perror ("write error");
                                err++;
                        }
                        else
                                printf ("%d/%d written.\n", nwritten, nleft);
                
                        nleft -= nwritten;
                        p += nwritten;
                }
        }

        /* Flush output */
        write (fd_audio, NULL, 0);

        return err;
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
int VT_oss_sound_driver_test(int device, int loop, char *file)
{
        FILE *fd_file = NULL;
        int fd_audio = -1;
        int i;
        int err = 0;
        char rep[32];

        if ((fd_file = fopen (file, "r")) <= 0) {
                err++;
                goto _err_file_not_found;
        }

        printf("DEBUG:in test");
        /* Open driver in WR mode for stdac, R/W for codec */
        if ( device == 1 ) /*CODEC*/
        {
                if ((fd_audio = open (CODEC_DEV, O_WRONLY)) < 0)
                {
                        tst_resm(TFAIL, "Error opening %s (SSI1)", CODEC_DEV);
                        err++;
                        goto _err_drv_open;
                }
        }
        else if ( device == 0 ) /*STDAC*/
        {
                if ((fd_audio = open (STDAC_DEV, O_WRONLY)) < 0)
                {
                        tst_resm(TFAIL, "Error opening %s (SSI2)", STDAC_DEV);
                        err++;
                        goto _err_drv_open;
                }
        }
        else
        {
                goto _err_drv_open;
        }
        
        if ( set_audio_config(fd_audio, fd_file) != TPASS )
        {
                tst_resm(TFAIL, "Bad format for file 1. ");
                tst_resm(TINFO, "Do you want to continue ? (y/n) : ");
                scanf("%s", rep);
                err++;
                if ( rep[0] != 'y' ) goto _err_fmt;
        }

        /* read the file many times if desired */
        for ( i=0; i<loop; i++ )
        {
                err += play_file(fd_audio, fd_file);
        }

        tst_resm(TINFO, "************** Closing the device\n");
        fclose (fd_file);
        close (fd_audio);

        if ( err ) goto _end_err;
        return TPASS;

_err_drv_open:
        if ( fd_audio >= 0 ) close(fd_audio);
_err_fmt:
        if ( fd_file ) fclose(fd_file);
_end_err:
        tst_resm(TFAIL, "Encountered %d errors\n", err);
        return TFAIL;

_err_file_not_found:
        tst_resm(TFAIL, "Error opening the file %s \n", file);
        return TFAIL;
}
