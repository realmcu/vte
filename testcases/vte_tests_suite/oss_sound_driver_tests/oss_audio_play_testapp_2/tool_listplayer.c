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
        @file   tool_listplayer.c

        @brief  OSS driver test scenario
====================================================================================================
Revision History:
                            Modification     Tracking
Author/core ID                  Date          Number    Description of Changes
-------------------------   ------------    ----------  -------------------------------------------
RB657C/gsch1c                20/07/2004     TLSbo40898  Initial version  of OSS sound driver test development
I.Inkina/nknl001             11/01/2005     TLSbo61044  Clock master was added
A.Ozerov/B00320              17/02/2006     TLSbo62323  Index_ssi argument was deleted. Master mode
                                                        was removed. Checking if the audio file given
                                                        in argument is stereo or mono was added.
D.Khoroshev/b00313           04/14/2006     TLSbo67022  VTE 2.0 Integration 
D.Simakov                    13/06/2006     TLSbo67022  STDAC <==> CODEC
D.Simakov                    19/10/2006     TLSbo76144  dsp->adsp, dsp1->dsp
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
                                        LOCAL MACROS
==================================================================================================*/

/*==================================================================================================
                            LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
==================================================================================================*/
typedef struct _dbmx_cfg
{
        int reg;
        int val;
} dbmx_cfg;
        
/*==================================================================================================
                                        LOCAL CONSTANTS
==================================================================================================*/

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
                                        LOCAL FUNCTIONS
==================================================================================================*/

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
/*===== VT_oss_sound_driver_cleanup =====*/
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
int set_audio_config(int fd_audio, FILE* file, int device)
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
        if(device == 1 && channels == 2)
        {
                tst_resm(TFAIL, "For Voice CODEC device stereo mode is not supported!");
                return TFAIL;
        }

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

        fseek(file, 44, SEEK_SET);
        while ((n = fread (buf, 1, sizeof(buf), file)) > 0)
        {
                nleft = n;
                p = buf;

                while (nleft){
                        if ((nwritten = write (fd_audio, p, nleft)) < 0)
                        {
                                perror ("Device write error!");
                                err++;
                        }
                        nleft -= nwritten;
                        p += nwritten;
                }
        }
        write (fd_audio, NULL, 0);

        return err;
}

/*================================================================================================*/
/*===== get_next_file_name =====*/
void get_next_file_name(FILE *fd_list, char *file_name)
{
        int ret;
        *file_name = '\0';
        
        ret = fscanf(fd_list, "%s\n", file_name);
        tst_resm(TINFO, "Next file: %s\n", file_name);
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
int VT_oss_sound_driver_test(char *playlist, int file_loop, int list_loop, int device)
{
        FILE *fd_file, *fd_list;
        int fd_audio;
        int i, j;
        int err = 0;
        int ret = TFAIL;
        char file_name[256];
        
        fd_list = fopen (playlist, "r");
        if(fd_list == NULL)
        {
                tst_resm(TFAIL, "ERROR in playlist: cannot open file: %s",
                        strerror(errno));
                err++;
                goto _err_list_not_found;
        }
        
        /*CODEC*/
        if ((fd_audio = open (CODEC_DEV, O_RDWR)) < 0)
        {
                tst_resm(TFAIL, "Error opening %s", CODEC_DEV);
                err++;
                goto _err_drv_open2;
        }
        
        if ( device == 0 ) /*STDAC*/
        {
                close(fd_audio);                        
                if ((fd_audio = open (STDAC_DEV, O_WRONLY)) < 0)
                {
                        tst_resm(TFAIL, "Error opening %s", STDAC_DEV);
                        err++;
                        goto _err_drv_open2;
                }
        }
        
        for ( j=0; j<list_loop; j++ )
        {
                get_next_file_name(fd_list, file_name);
                
                while ( *file_name != '\0' )
                {
                        if ((fd_file = fopen (file_name, "r")) == NULL)
                        {
                                tst_resm(TFAIL, "Error at file opening. Skip.\n");
                                err++;
                                goto _next;
                        }

                        if ( set_audio_config(fd_audio, fd_file, device) < 0 )
                        {
                                tst_resm(TFAIL, "Unsupported format of file %s. Skip.\n", file_name);
                                       fclose (fd_file);
                                err++;
                                goto _next;
                        }

                        for ( i=0; i<file_loop; i++ )
                        {
                                err += play_file(fd_audio, fd_file);
                                sleep(2);
                        }

                        fclose (fd_file);
_next:
                        get_next_file_name(fd_list, file_name);
                }
                
                fseek(fd_list, 0, SEEK_SET);
        } 

        tst_resm(TINFO, "************** Closing the device\n");
        fclose(fd_list);
        close (fd_audio);

        if ( err ) goto _end_err;
        tst_resm(TINFO, "All tests passed with success\n");
        ret = TPASS;
        return ret;

_err_drv_open2:
        close(fd_audio);
_end_err:
_err_list_not_found:
        tst_resm(TFAIL, "Encountered %d errors\n", err);
        fclose(fd_list);
        return ret;
}
