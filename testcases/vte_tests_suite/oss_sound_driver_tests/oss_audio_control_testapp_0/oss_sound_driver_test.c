/*================================================================================================*/
/**
        @file   oss_sound_driver_test.c

        @brief  OSS audio control test scenario.
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
I.Inkina/nknl001             18/10/2005     TLSbo56551  Ask_user was added
A.Ozerov/b00320              07/11/2005     TLSbo56870  Compilation flags for SC55112 and MC13783
                                                        platforms
D.Khoroshev/b00313           03/03/2006     TLSbo62323  Update according to the last MXC OSS specifications
D.Simakov                    13/06/2006     TLSbo67022  STDAC <==> CODEC
A.Ozerov/b00320              20/07/2006     TLSbo70792  volume_test, balance_test and other functions were changed.
D.Simakov                    19/10/2006     TLSbo76144  dsp->adsp, dsp1->dsp
D.Simakov                    22/12/2005     TLSbo87096  Zeus compilation issue
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
                                        GLOBAL VARIABLES
==================================================================================================*/
static audio_settings current_driver_settings;  /* Current settings of the audio HW */

/*==================================================================================================
                                    LOCAL FUNCTION PROTOTYPES
==================================================================================================*/
unsigned short get_audio_bits(FILE * file);
unsigned short get_audio_channels(FILE * file);
int     get_audio_frq(FILE * file);
int     set_settings(int fd_audio, int fd_mixer, audio_settings settings_to_apply);
int     get_settings(int fd_audio, int fd_mixer, audio_settings * setttings);
int     set_audio_config(int fd_audio, int fd_mixer, FILE * file);
int     play_file(int fd_audio, FILE * file);
int     volume_test(int fd_audio, int fd_mixer, audio_settings audio_config,
                    test_parameters params);
int     balance_test(int fd_audio, int fd_mixer, audio_settings audio_config,
                     test_parameters params);
int     codec_filter_test(int fd_audio, int fd_mixer, audio_settings audio_config,
                          test_parameters params);

#ifdef CONFIG_MXC_MC13783_PMIC
int     adder_test(int fd_audio, int fd_mixer, audio_settings audio_config, test_parameters params);
#endif

int     volume_ioctl_test(int fd_audio, int fd_mixer, audio_settings audio_config);
void    detect_enter(int time_out);

/*==================================================================================================
                                        LOCAL FUNCTIONS
==================================================================================================*/
/*================================================================================================*/
/*===== get_audio_bits=====*/
/**
@brief  Parse the header of the test file to get the audio bits.

@param  file to parse
    
@return On success - return 0
        On failure - return the error code
*/
/*================================================================================================*/
unsigned short get_audio_bits(FILE * file)
{
        unsigned short ret;

        fseek(file, 34, SEEK_SET);
        fread(&ret, 2, 1, file);
        return ret;
}

/*================================================================================================*/
/*===== get_audio_channels=====*/
/**
@brief  Parse the header of the test file to get the audio channels.

@param  file to parse
    
@return On success - return 0
        On failure - return the error code
*/
/*================================================================================================*/
unsigned short get_audio_channels(FILE * file)
{
        unsigned short ret;

        fseek(file, 22, SEEK_SET);
        fread(&ret, 2, 1, file);
        return ret;
}

/*================================================================================================*/
/*===== equal_settings=====*/
/**
@brief  compares 2 audio_settings structure 

@param  the 2 audio_settings structure to compare
    
@return On success - return 0
        On failure - return err
*/
/*================================================================================================*/
unsigned int equal_settings(audio_settings config_1, audio_settings config_2)
{
        int     err = 0;

        if (config_1.volume != config_2.volume)
                err++;
        if (config_1.balance != config_2.balance)
                err++;
        if (config_1.channels != config_2.channels)
                err++;
        if (config_1.sample_rate != config_2.sample_rate)
                err++;
        if (config_1.format != config_2.format)
                err++;
        if (config_1.active_output != config_2.active_output)
                err++;
        if (config_1.active_input != config_2.active_input)
                err++;
        if (config_1.codec_filter != config_2.codec_filter)
                err++;

        return err;
}

/*================================================================================================*/
/*===== get_audio_frq=====*/
/**
@brief  Parse the header of the test file to get the frequency.

@param  file to parse
    
@return On success - return 0
        On failure - return the error code
*/
/*================================================================================================*/
int get_audio_frq(FILE * file)
{
        int     ret;

        fseek(file, 24, SEEK_SET);
        fread(&ret, 4, 1, file);
        return ret;
}

/*================================================================================================*/
/*===== set_settings=====*/
/**
@brief  apply the audio settings to the sound driver

@param  fd_audio: audio driver reference
                fd_mixer: audio mixer reference
                settings_to_apply: the settings to be set
    
@return On success - return 0
        On failure - return the error code
*/
/*================================================================================================*/
int set_settings(int fd_audio, int fd_mixer, audio_settings settings_to_apply)
{
        int     err = 0,
                ret = 0;

        /* for each settings, test if it has changed from the current configuration */

        /* Check if the format has been changed */
        if (settings_to_apply.format != current_driver_settings.format)
        {
                tst_resm(TINFO, "\t Updating format to 0x%x\n", settings_to_apply.format);
                current_driver_settings.format = settings_to_apply.format;
                ret = ioctl(fd_audio, SNDCTL_DSP_SETFMT, &settings_to_apply.format);
                if (settings_to_apply.format != current_driver_settings.format)
                {
                        tst_resm(TFAIL, "Error with IOCTL SNDCTL_DSP_SETFMT\n");
                        err++;
                }
        }

        /* Check if the sample_rate has been changed */
        if (settings_to_apply.sample_rate != current_driver_settings.sample_rate)
        {
                tst_resm(TINFO, "\t Updating sample rate to %d\n", settings_to_apply.sample_rate);
                current_driver_settings.sample_rate = settings_to_apply.sample_rate;
                ret = ioctl(fd_audio, SOUND_PCM_WRITE_RATE, &settings_to_apply.sample_rate);
                if (settings_to_apply.sample_rate != current_driver_settings.sample_rate)
                {
                        tst_resm(TFAIL, "Error with IOCTL SOUND_PCM_WRITE_RATE\n");
                        err++;
                }
        }

        /* Check if the channels used has been changed */
        if (settings_to_apply.channels != current_driver_settings.channels)
        {
                tst_resm(TINFO, "\t Updating channels to %d\n", settings_to_apply.channels);
                current_driver_settings.channels = settings_to_apply.channels;
                ret = ioctl(fd_audio, SOUND_PCM_WRITE_CHANNELS, &settings_to_apply.channels);
                if (settings_to_apply.channels != current_driver_settings.channels)
                {
                        tst_resm(TFAIL, "Error with IOCTL SOUND_PCM_WRITE_CHANNELS\n");
                        err++;
                }
        }

        /* Check if the active output has been activated/disactivated */
        if (settings_to_apply.active_output != current_driver_settings.active_output)
        {
                tst_resm(TINFO, "\t Updating active output to 0x%x\n", settings_to_apply.active_output);
                current_driver_settings.active_output = settings_to_apply.active_output;
                ret = ioctl(fd_mixer, SOUND_MIXER_WRITE_OUTSRC, &settings_to_apply.active_output);
                if (settings_to_apply.active_output != current_driver_settings.active_output)
                {
                        tst_resm(TFAIL, "Error with IOCTL SOUND_MIXER_WRITE_OUTSRC\n");
                        err++;
                }
        }

        /* Check if the volume has been changed */
        if (settings_to_apply.volume != current_driver_settings.volume)
        {
                tst_resm(TINFO, "\t Updating volume to %d\n", settings_to_apply.volume >> 8);
                current_driver_settings.volume = settings_to_apply.volume;
                /* for sure it is not useful to apply the volume to all connected device, just once
                * is enought!!! */

                /* if the earpiece is active, change the appropriate volume */
                if (settings_to_apply.active_output & SOUND_MASK_VOLUME)
                {
                        tst_resm(TINFO, "Updating volume for headset\n");
                        ret = ioctl(fd_mixer, SOUND_MIXER_WRITE_VOLUME, &settings_to_apply.volume);
                        if (settings_to_apply.volume != current_driver_settings.volume)
                        {
                                printf
                                    ("Error with IOCTL SOUND_MIXER_WRITE_VOLUME %d %d\n",
                                     settings_to_apply.volume, current_driver_settings.volume);
                                err++;
                        }
                }
                /* if the handsfree is active, change the appropriate volume */
                if (settings_to_apply.active_output & SOUND_MASK_SPEAKER)
                {
                        ret = ioctl(fd_mixer, SOUND_MIXER_WRITE_SPEAKER, &settings_to_apply.volume);
                        if (settings_to_apply.volume != current_driver_settings.volume)
                        {
                                tst_resm(TFAIL, "Error with IOCTL SOUND_MIXER_WRITE_SPEAKER\n");
                                err++;
                        }
                }
                /* if the headset is active, change the appropriate volume */
                if (settings_to_apply.active_output & SOUND_MASK_PCM)
                {
                        ret = ioctl(fd_mixer, SOUND_MIXER_WRITE_PCM, &settings_to_apply.volume);
                        if (settings_to_apply.volume != current_driver_settings.volume)
                        {
                                tst_resm(TFAIL, "Error with IOCTL SOUND_MIXER_WRITE_PCM\n");
                                err++;
                        }
                }

                /* if the lineout is active, change the appropriate volume */
                if (settings_to_apply.active_output & SOUND_MASK_PHONEOUT)
                {
                        ret =
                            ioctl(fd_mixer, SOUND_MIXER_WRITE_PHONEOUT, &settings_to_apply.volume);
                        if (settings_to_apply.volume != current_driver_settings.volume)
                        {
                                tst_resm(TFAIL, "Error with IOCTL SOUND_MIXER_WRITE_PHONEOUT\n");
                                err++;
                        }
                }
        }

#ifdef CONFIG_MXC_MC13783_PMIC
        /* Check if the balance has been changed */
        if (settings_to_apply.balance != current_driver_settings.balance)
        {
                tst_resm(TINFO, "\t Updating balance to %d\n", settings_to_apply.balance);
                current_driver_settings.balance = settings_to_apply.balance;
                ret = ioctl(fd_audio, SNDCTL_MC13783_WRITE_OUT_BALANCE, &settings_to_apply.balance);
                if (settings_to_apply.balance != current_driver_settings.balance)
                {
                        tst_resm(TFAIL, "Error with IOCTL SNDCTL_MC13783_WRITE_OUT_BALANCE\n");
                        err++;
                }
        }

        /* Check if the codec filter configuration has been changed */
        if (settings_to_apply.codec_filter != current_driver_settings.codec_filter)
        {
                tst_resm(TINFO, "\t Updating codec filter to 0x%x\n", settings_to_apply.codec_filter);
                current_driver_settings.codec_filter = settings_to_apply.codec_filter;
                ret =
                    ioctl(fd_audio, SNDCTL_MC13783_WRITE_CODEC_FILTER,
                          &settings_to_apply.codec_filter);
                if (settings_to_apply.codec_filter != current_driver_settings.codec_filter)
                {
                        tst_resm(TFAIL, "Error with IOCTL SNDCTL_MC13783_WRITE_CODEC_FILTER\n");
                        err++;
                }
        }
#endif
        tst_resm(TINFO, "End of set settings\n");

        return err;
}

/*================================================================================================*/
/*===== get_settings=====*/
/**
@brief  Get the audio settings from the audio driver

@param  fd_audio: audio driver reference
                fd_mixer: audio mixer reference
                settings_to_apply: the settings to be set
    
@return On success - return 0
        On failure - return the error code
*/
/*================================================================================================*/
int get_settings(int fd_audio, int fd_mixer, audio_settings * settings)
{
        int     err = 0;
        int     ret;

        tst_resm(TINFO, "Default Audio configuration when opening the audio driver\n");

#ifdef CONFIG_MXC_PMIC_SC55112
        /* Get the default balance */
        ret = ioctl(fd_audio, SNDCTL_PMIC_READ_OUT_BALANCE, &current_driver_settings.balance);
        tst_resm(TINFO, "\t Balance: %d\n", current_driver_settings.balance);
        settings->balance = current_driver_settings.balance;
#endif

#ifdef CONFIG_MXC_MC13783_PMIC
        /* Get the default balance */
        ret = ioctl(fd_audio, SNDCTL_MC13783_READ_OUT_BALANCE, &current_driver_settings.balance);
        tst_resm(TINFO, "\t Balance: %d\n", current_driver_settings.balance);
        settings->balance = current_driver_settings.balance;
#endif
        /* Get the default channels settings */
        ret = ioctl(fd_audio, SOUND_PCM_READ_CHANNELS, &current_driver_settings.channels);
        tst_resm(TINFO, "\t Channels: %d\n", current_driver_settings.channels);
        settings->channels = current_driver_settings.channels;

        /* Get the default sample rate */
        ret = ioctl(fd_audio, SOUND_PCM_READ_RATE, &current_driver_settings.sample_rate);
        tst_resm(TINFO, "\t Sample rate: %d\n", current_driver_settings.sample_rate);
        settings->sample_rate = current_driver_settings.sample_rate;

        /* Get the default active output */
        ret = ioctl(fd_mixer, SOUND_MIXER_READ_OUTSRC, &current_driver_settings.active_output);
        tst_resm(TINFO, "\t Active Output: %d\n", current_driver_settings.active_output);
        settings->active_output = current_driver_settings.active_output;

        /* Get the default active input */
        ret = ioctl(fd_mixer, SOUND_MIXER_READ_RECSRC, &current_driver_settings.active_input);
        tst_resm(TINFO, "\t Active Input: %d\n", current_driver_settings.active_input);
        settings->active_input = current_driver_settings.active_input;

        /* Get the default output volume */
        current_driver_settings.volume = 0x5050;
        settings->volume = current_driver_settings.volume;

        /* Get the default input gain */
#ifdef CONFIG_MXC_PMIC_SC55112
        /* Get the default codec filter configuration */
        ret = ioctl(fd_audio, SNDCTL_PMIC_READ_CODEC_FILTER, &current_driver_settings.codec_filter);
        tst_resm(TINFO, "\t Codec filter: %d\n", current_driver_settings.codec_filter);
        settings->codec_filter = current_driver_settings.codec_filter;
#endif

#ifdef CONFIG_MXC_MC13783_PMIC
        /* Get the default codec filter configuration */
        ret =
            ioctl(fd_audio, SNDCTL_MC13783_READ_CODEC_FILTER,
                  &current_driver_settings.codec_filter);
        tst_resm(TINFO, "\t Codec filter: %d\n", current_driver_settings.codec_filter);
        settings->codec_filter = current_driver_settings.codec_filter;
#endif
        /* Get the default format */
        ret = ioctl(fd_audio, SOUND_PCM_READ_BITS, &current_driver_settings.format);
        tst_resm(TINFO, "\t Format: %d\n\n", current_driver_settings.format);
        settings->format = current_driver_settings.format;

        return err;
}

/*================================================================================================*/
/*===== set_audio_config=====*/
/**
@brief  get the audio config for the file to be tested

@param  fd_audio: audio driver reference
                fd_mixer: audio mixer reference
                file: file to be tested
    
@return On success - return 0
        On failure - return the error code
*/
/*================================================================================================*/
int set_audio_config(int fd_audio, int fd_mixer, FILE * file)
{
        int     tmp,
                format,
                frequency,
                channels,
                ret = 0;
        audio_settings audio_set;

        ret += get_settings(fd_audio, fd_mixer, &audio_set);

        tmp = format = (int) get_audio_bits(file);
        audio_set.format = format;
        if (tmp != format)
                return -1;

        tmp = frequency = get_audio_frq(file);
        audio_set.sample_rate = frequency;
        if (tmp != frequency)
                return -1;

        tmp = channels = get_audio_channels(file);
        audio_set.channels = channels;
        if (tmp != channels)
                return -1;

        ret += set_settings(fd_audio, fd_mixer, audio_set);

        tst_resm(TINFO, "Configuration : %d %d %d\n", format, frequency, channels);
        return ret;
}

/*================================================================================================*/
/*===== play_file=====*/
/**
@brief  play the file

@param  fd_audio: audio driver reference
                file: file to be tested
    
@return On success - return 0
        On failure - return the error code
*/
/*================================================================================================*/
int play_file(int fd_audio, FILE * file)
{
        int     err = 0;
        int     n,
                nleft,
                nwritten;
        char    buf[BUF_SIZE],
               *p;

        /* Parsing wav header */
        fseek(file, 44, SEEK_SET);

        /* Reading the sample */
        while ((n = fread(buf, 1, sizeof(buf), file)) > 0)
        {
                nleft = n;
                p = buf;

                /* sending the sample... */
                while (nleft)
                {
                        if ((nwritten = write(fd_audio, p, nleft)) < 0)
                        {
                                perror("write error");
                                err++;
                        }
                        nleft -= nwritten;
                        p += nwritten;
                }
        }

        return err;
}

/*================================================================================================*/
/*===== volume_test=====*/
/**
@brief  volume test loop

@param  fd_audio: audio driver reference
                fd_mixer: audio mixer reference
                audio_config: config of the sound driver to use
                params: regroups the test parameters (file, increment)
    
@return On success - return 0
        On failure - return the error code
*/
/*================================================================================================*/
int volume_test(int fd_audio, int fd_mixer, audio_settings audio_config, test_parameters params)
{
        int     vol = 0,
            err = 0;

        audio_config.volume = (vol << 8) | vol;

        while (vol <= 100)
        {
                err += set_settings(fd_audio, fd_mixer, audio_config);
                err += play_file(fd_audio, params.fd_file);
                vol += params.increment;
                audio_config.volume = (vol << 8) | vol;
                tst_resm(TINFO, "Press enter to play @ new volume.....timeout 2s\n");
                detect_enter(2);
        }

        return err;
}

/*================================================================================================*/
/*===== balance_test=====*/
/**
@brief  Balance test loop

@param  fd_audio: audio driver reference
                fd_mixer: audio mixer reference
                audio_config: config of the sound driver to use
                params: regroups the test parameters (file, increment)
    
@return On success - return 0
        On failure - return the error code
*/
/*================================================================================================*/
int balance_test(int fd_audio, int fd_mixer, audio_settings audio_config, test_parameters params)
{
        int     err = 0;

        audio_config.balance = 0;
        audio_config.volume = (100 << 8) | 100;

        while (audio_config.balance <= 100)
        {
                err += set_settings(fd_audio, fd_mixer, audio_config);
                err += play_file(fd_audio, params.fd_file);
                audio_config.balance += params.increment;
                tst_resm(TINFO, "Press enter to play @ new balance.....timeout 2s\n");
                detect_enter(2);
        }

        return err;
}

/*================================================================================================*/
/*===== codec_filter_test=====*/
/**
@brief  Codec filter test loop: try all the configurations of the filter of the codec

@param  fd_audio: audio driver reference
                fd_mixer: audio mixer reference
                audio_config: config of the sound driver to use
                params: regroups the test parameters (file, increment)
    
@return On success - return 0
        On failure - return the error code
*/
/*================================================================================================*/
int codec_filter_test(int fd_audio, int fd_mixer, audio_settings audio_config,
                      test_parameters params)
{
        int     err = 0;

        tst_resm(TINFO, "\n filter test in high pass in\n");
#ifdef CONFIG_MXC_PMIC_SC55112
        audio_config.codec_filter = /*PMIC_CODEC_FILTER_HIGH_PASS_IN*/ 1 /*there is no such name in the bsp headers*/;
#endif

#ifdef CONFIG_MXC_MC13783_PMIC
        audio_config.codec_filter = MC13783_CODEC_FILTER_HIGH_PASS_IN;
#endif
        err += set_settings(fd_audio, fd_mixer, audio_config);
        err += play_file(fd_audio, params.fd_file);
        tst_resm(TINFO, "\n Look at the signal on the scope and press enter\n");
        detect_enter(2);

        tst_resm(TINFO, "\n filter test in high pass out \n");
#ifdef CONFIG_MXC_PMIC_SC55112
        audio_config.codec_filter = /*PMIC_CODEC_FILTER_HIGH_PASS_OUT*/ 2 /*there is no such name in the bsp headers*/;
#endif

#ifdef CONFIG_MXC_MC13783_PMIC
        audio_config.codec_filter = MC13783_CODEC_FILTER_HIGH_PASS_OUT;
#endif
        err += set_settings(fd_audio, fd_mixer, audio_config);
        err += play_file(fd_audio, params.fd_file);
        tst_resm(TINFO, "\n Look at the signal on the scope and press enter\n");
        detect_enter(2);

        tst_resm(TINFO, "\n filter test in fitler dithering \n");
#ifdef CONFIG_MXC_PMIC_SC55112
        audio_config.codec_filter = /*PMIC_CODEC_FILTER_DITHERING;*/ 4;
#endif

#ifdef CONFIG_MXC_MC13783_PMIC
        audio_config.codec_filter = MC13783_CODEC_FILTER_DITHERING;
#endif
        err += set_settings(fd_audio, fd_mixer, audio_config);
        err += play_file(fd_audio, params.fd_file);
        tst_resm(TINFO, "\n Look at the signal on the scope and press enter\n");
        detect_enter(2);

        return err;
}

/*================================================================================================*/
/*===== adder_test=====*/
/**
@brief  Adder test loop: try all the configurations of the adder

@param  fd_audio: audio driver reference
                fd_mixer: audio mixer reference
                audio_config: config of the sound driver to use
                params: regroups the test parameters (file, increment)
    
@return On success - return 0
        On failure - return the error code
*/
/*================================================================================================*/
#ifdef CONFIG_MXC_MC13783_PMIC
int adder_test(int fd_audio, int fd_mixer, audio_settings audio_config, test_parameters params)
{
        int     err = 0;

        audio_config.volume = (25 << 8) | 25;
        audio_config.balance = 50;

        tst_resm(TINFO, "Adder test in stereo mode\n");
        audio_config.adder = MC13783_AUDIO_ADDER_STEREO;
        err += set_settings(fd_audio, fd_mixer, audio_config);
        err += play_file(fd_audio, params.fd_file);
        tst_resm(TINFO, "Look at the signal on the scope and press enter\n");
        detect_enter(2);

        tst_resm(TINFO, "Adder test in stereo opposite mode\n");
        audio_config.adder = MC13783_AUDIO_ADDER_STEREO_OPPOSITE;
        err += set_settings(fd_audio, fd_mixer, audio_config);
        err += play_file(fd_audio, params.fd_file);
        tst_resm(TINFO, "Look at the signal on the scope and press enter\n");
        detect_enter(2);

        tst_resm(TINFO, "Adder test in mono mode\n");
        audio_config.adder = MC13783_AUDIO_ADDER_MONO;
        err += set_settings(fd_audio, fd_mixer, audio_config);
        err += play_file(fd_audio, params.fd_file);
        tst_resm(TINFO, "Look at the signal on the scope and press enter\n");
        detect_enter(2);

        tst_resm(TINFO, "Adder test in mono opposite mode\n");
        audio_config.adder = MC13783_AUDIO_ADDER_MONO_OPPOSITE;
        err += set_settings(fd_audio, fd_mixer, audio_config);
        err += play_file(fd_audio, params.fd_file);
        tst_resm(TINFO, "Look at the signal on the scope and press enter\n");
        detect_enter(2);

        return err;
}
#endif

/*================================================================================================*/
/*===== volume_ioctl_test=====*/
/**
@brief  Test the volume ioctl on different activated/disactivated device

@param  fd_audio: audio driver reference
                fd_mixer: audio mixer reference
                audio_config: config of the sound driver to use
                params: regroups the test parameters (file, increment)
    
@return On success - return 0
        On failure - return the error code
*/
/*================================================================================================*/
int volume_ioctl_test(int fd_audio, int fd_mixer, audio_settings audio_config)
{
        int     err = 0,
                ret = 0;

/*==================================================================================================
                                        FIRST TEST
==================================================================================================*/
        /* 2 outputs are active, set the volume on the 2 outputs at the same value */
        tst_resm(TINFO, "Activate VOLUME and PHONEOUT\n");
        audio_config.active_output = SOUND_MASK_VOLUME | SOUND_MASK_PHONEOUT;
        audio_config.volume = (25 << 8) | 25;
        audio_config.balance = 50;
        tst_resm(TINFO, "Volume set on both output to 25\n");
        set_settings(fd_audio, fd_mixer, audio_config);

        /* Then, set the volume on 1 output to another value = 75 and read of the volume */
        audio_config.volume = 75;
        tst_resm(TINFO, "Volume set on VOLUME @ 75\n");
        ret = ioctl(fd_mixer, SOUND_MIXER_WRITE_VOLUME, &audio_config.volume);
        if ((audio_config.volume & 0x00FF) != 75)
        {
                tst_resm(TFAIL, "Error with IOCTL SOUND_MIXER_WRITE_VOLUME\n");
                err++;
        }

/*==================================================================================================
                                        SECOND TEST
==================================================================================================*/
        /* Set the volume on an unactive output */
        tst_resm(TINFO, "Activate VOLUME\n");
        audio_config.active_output = SOUND_MASK_VOLUME;
        audio_config.volume = (50 << 8) | 50;
        tst_resm(TINFO, "Set volume @ 50 on VOLUME\n");
        set_settings(fd_audio, fd_mixer, audio_config);
        tst_resm(TINFO, "Read the volume on PHONEOUT\n");
        ret = ioctl(fd_mixer, SOUND_MIXER_WRITE_PHONEOUT, &audio_config.volume);
        if (ret >= 0)
        {
                tst_resm(TFAIL, "The IOCTL should have failed since PHONEOUT were disabled\n");
                err++;
        }
        else
                tst_resm(TINFO, "IOCTL failed since PHONEOUT were disabled\n");

        return err;
}

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
        int     rv = TFAIL;

        rv = TPASS;
        return rv;
}

/*================================================================================================*/
/*===== VT_cleanup =====*/
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

        rv = TPASS;
        return rv;
}

/*================================================================================================*/
/*===== detect_enter =====*/
/**
@brief  Detect enter key stroken from console 
                Used between each picture decode of the test suite.
                If enter is pressed, the next picture is decoded immediately.

@param  time_out: time out value to unlock the prompt
    
@return None
*/
/*================================================================================================*/
void detect_enter(int time_out)
{
        int     fd_console = 0; /* 0 is the video input */
        fd_set  fdset;
        struct timeval timeout;
        char    c;

        FD_ZERO(&fdset);
        FD_SET(fd_console, &fdset);
        timeout.tv_sec = time_out;      /* set timeout !=0 => blocking select */
        timeout.tv_usec = 0;
        if (select(fd_console + 1, &fdset, 0, 0, &timeout) > 0)
        {
                do
                {
                        read(fd_console, &c, 1);
                }
                while (c != 10);        // i.e. line-feed 
        }
}

/*================================================================================================*/
/*===== VT_oss_sound_driver_test =====*/
/**
@brief  Template test scenario X function

@param  None
    
@return On success - return TPASS
        On failure - return the error code
*/
/*================================================================================================*/
int VT_oss_sound_driver_test(int Device, int Increment, char *File)
{
        int     rv = TFAIL;
        int     ret = 0;

        /* Test launcher according to the test ID entered by user (default 0) */

        int     fd_audio,
                fd_mixer;
        int     err = 0,
            err_1 = 0;
        audio_settings audio_config;
        test_parameters params;

        tst_resm(TINFO, "OSS sound driver test suite \n");

        params.device = Device;
        params.increment = Increment;
        params.testfilename = File;

/*==================================================================================================
                                        TEST STDAC
==================================================================================================*/
        if (params.device == STDAC)
        {
                tst_resm(TINFO, "TEST SUITE on STDAC \n");

                if ((fd_audio = open(STDAC_DEV, O_WRONLY)) < 0)
                {
                        tst_resm(TFAIL, "Error opening %s", STDAC_DEV);
                        return 0;
                }

                if ((fd_mixer = open(MIXER_DEV, O_RDWR)) < 0)
                {
                        tst_resm(TFAIL, "Error opening %s", MIXER_DEV);
                        return 0;
                }

                /* Get the default audio settings to update the current settings structure */
                err += get_settings(fd_audio, fd_mixer, &audio_config);

                /* Set the settings (should not do anything) */
                err += set_settings(fd_audio, fd_mixer, audio_config);

                /* Open the test samples */
                if ((params.fd_file = fopen(params.testfilename, "r")) <= 0)
                {
                        err_1++;
                        goto _err_file_not_found;
                }

                /* Parse the header of the test file and update the audio configuration */
                if (set_audio_config(fd_audio, fd_mixer, params.fd_file) < 0)
                {
                        tst_resm(TFAIL, "Bad format for file 1\n");
                        err_1++;
                        goto _err_fmt;
                }

                /* Get the modified config for playing the test file correctly */
                err += get_settings(fd_audio, fd_mixer, &audio_config);

/*==================================================================================================
                                                    TEST IOCTL
==================================================================================================*/
                tst_resm(TINFO, "TEST IOCTL\n");
                err_1 = volume_ioctl_test(fd_audio, fd_mixer, audio_config);

                if (!err_1)
                        tst_resm(TINFO, "TEST IOCTL passed with success\n");
                else
                        tst_resm(TINFO, "Encountered %d errors\n", err_1);

                err += err_1;
                err_1 = 0;

/*==================================================================================================
                                            END OF TEST IOCTL
==================================================================================================*/

/*==================================================================================================
                                                TEST VOLUME
==================================================================================================*/
                /* Select the output device to activate */

                /* Earpiece: mono output */
                ret = ask_user("\n TEST VOLUME on PHONEOUT. Press 'Y' when ready, 'N' to skip");
                if (!ret)
                {
                        audio_config.active_output = SOUND_MASK_PHONEOUT;
                        err_1 += volume_test(fd_audio, fd_mixer, audio_config, params);
                }

                /* Earpiece and handsfree: 2 mono outputs */
                ret = ask_user("\n TEST VOLUME on PHONEOUT and HANDSFREE. Press 'Y' when ready, 'N' to skip");
                if (!ret)
                {
                        audio_config.active_output = SOUND_MASK_PHONEOUT | SOUND_MASK_SPEAKER;
                        err_1 += volume_test(fd_audio, fd_mixer, audio_config, params);
                }

                /* Headset and earpiece: 1 stereo and 1 mono outputs */
                ret = ask_user("\n TEST VOLUME on HEADSET and EARPIECE. Press 'Y' when ready, 'N' to skip");
                if (!ret)
                {
                        audio_config.active_output = SOUND_MASK_VOLUME | SOUND_MASK_PHONEOUT;
                        err_1 += volume_test(fd_audio, fd_mixer, audio_config, params);
                }

                /* Headset: stereo output */
                ret = ask_user("\n TEST VOLUME on HEADSET. Press 'Y' when ready, 'N' to skip");
                if (!ret)
                {
                        audio_config.active_output = SOUND_MASK_VOLUME;
                        err_1 += volume_test(fd_audio, fd_mixer, audio_config, params);
                }

                /* Headset and line out: 2 stereo outputs */
                ret = ask_user("\n TEST VOLUME on HEADSET and LINEOUT. Press 'Y' when ready, 'N' to skip");
                if (!ret)
                {
                        audio_config.active_output = SOUND_MASK_PCM | SOUND_MASK_VOLUME;
                        err_1 += volume_test(fd_audio, fd_mixer, audio_config, params);
                }

                if (!err_1)
                        tst_resm(TINFO, "TEST VOLUME passed with success\n");
                else
                        tst_resm(TINFO, "Encountered %d errors\n", err_1);

                err += err_1;
                err_1 = 0;

/*==================================================================================================
                                            END OF TEST VOLUME
==================================================================================================*/

/*==================================================================================================
                                                TEST BALANCE
==================================================================================================*/
                /* Select the output device to activate */
                ret = ask_user("\n TEST BALANCE on HEADSET. Press 'Y' when ready, 'N' to skip");
                if (!ret)
                {
                        audio_config.active_output = SOUND_MASK_VOLUME;
                        err_1 += balance_test(fd_audio, fd_mixer, audio_config, params);
                }

                ret = ask_user("\n TEST BALANCE on LINEOUT. Press 'Y' when ready, 'N' to skip");
                if (!ret)
                {
                        audio_config.active_output = SOUND_MASK_PCM;
                        err_1 += balance_test(fd_audio, fd_mixer, audio_config, params);
                }

                if (!err_1)
                        tst_resm(TINFO, "TEST BALANCE passed with success\n");
                else
                        tst_resm(TINFO, "Encountered %d errors\n", err_1);

                err += err_1;
                err_1 = 0;

/*==================================================================================================
                                        END OF TEST BALANCE
==================================================================================================*/
                fclose(params.fd_file);
                close(fd_audio);
                close(fd_mixer);

                if (!err)
                        tst_resm(TINFO, "All tests on STDAC passed with success");
                else
                        tst_resm(TFAIL, "Encountered %d errors", err);

        }

/*==================================================================================================
                                        END TEST STDAC
==================================================================================================*/

/*==================================================================================================
                                        TEST CODEC
==================================================================================================*/
        if (params.device == CODEC)
        {
                tst_resm(TINFO, "TEST SUITE on CODEC");
                if ((fd_audio = open(CODEC_DEV, O_RDWR)) < 0)
                {
                        tst_resm(TFAIL, "Error opening %s", CODEC_DEV);
                        return TFAIL;
                }

                if ((fd_mixer = open(MIXER_DEV, O_RDWR)) < 0)
                {
                        tst_resm(TFAIL, "Error opening %s", MIXER_DEV);
                        return TFAIL;
                }

                /* Get the default audio settings to update the current settings structure */
                err += get_settings(fd_audio, fd_mixer, &audio_config);

                /* Set the settings (should not do anything) */
                err += set_settings(fd_audio, fd_mixer, audio_config);

                /* Open the test samples */
                if ((params.fd_file = fopen(params.testfilename, "r")) <= 0)
                {
                        err_1++;
                        goto _err_file_not_found;
                }

                /* Parse the header of the test file and update the audio configuration */
                if (set_audio_config(fd_audio, fd_mixer, params.fd_file) < 0)
                {
                        tst_resm(TFAIL, "Bad format for file 1\n");
                        err_1++;
                        goto _err_fmt;
                }

                /* Get the modified config for playing the test file correctly */
                err += get_settings(fd_audio, fd_mixer, &audio_config);

/*==================================================================================================
                                                    TEST IOCTL
==================================================================================================*/
                tst_resm(TINFO, "TEST IOCTL\n");
                err_1 = volume_ioctl_test(fd_audio, fd_mixer, audio_config);

                if (!err_1)
                        tst_resm(TINFO, "TEST IOCTL passed with success\n");
                else
                        tst_resm(TINFO, "Encountered %d errors\n", err_1);

                err += err_1;
                err_1 = 0;

/*==================================================================================================
                                            END OF TEST IOCTL
==================================================================================================*/

/*==================================================================================================
                                                TEST VOLUME
==================================================================================================*/
                /* Select the output device to activate */

                /* Earpiece: mono output */
                ret = ask_user("\n TEST VOLUME on PHONEOUT. Press 'Y' when ready, 'N' to skip");
                if (!ret)
                {
                        audio_config.active_output = SOUND_MASK_PHONEOUT;
                        err_1 += volume_test(fd_audio, fd_mixer, audio_config, params);
                }

                /* Earpiece and handsfree: 2 mono outputs */
                ret = ask_user("\n TEST VOLUME on PHONEOUT and HANDSFREE. Press 'Y' when ready, 'N' to skip");
                if (!ret)
                {
                        audio_config.active_output = SOUND_MASK_PHONEOUT | SOUND_MASK_SPEAKER;
                        err_1 += volume_test(fd_audio, fd_mixer, audio_config, params);
                }

                /* Headset and earpiece: 1 stereo and 1 mono outputs */
                ret = ask_user("\n TEST VOLUME on HEADSET and EARPIECE. Press 'Y' when ready, 'N' to skip");
                if (!ret)
                {
                        audio_config.active_output = SOUND_MASK_VOLUME | SOUND_MASK_PHONEOUT;
                        err_1 += volume_test(fd_audio, fd_mixer, audio_config, params);
                }
                /* Headset: stereo output */
                ret = ask_user("\n TEST VOLUME on HEADSET. Press 'Y' when ready, 'N' to skip");
                if (!ret)
                {
                        audio_config.active_output = SOUND_MASK_VOLUME;
                        err_1 += volume_test(fd_audio, fd_mixer, audio_config, params);
                }
                /* Headset and line out: 2 stereo outputs */
                ret = ask_user("\n TEST VOLUME on HEADSET and LINEOUT. Press 'Y' when ready, 'N' to skip");
                if (!ret)
                {
                        audio_config.active_output = SOUND_MASK_PCM | SOUND_MASK_VOLUME;
                        err_1 += volume_test(fd_audio, fd_mixer, audio_config, params);
                }

                if (!err_1)
                        tst_resm(TINFO, "TEST VOLUME passed with success");
                else
                        tst_resm(TFAIL, "Encountered %d errors", err_1);

                err += err_1;
                err_1 = 0;
/*==================================================================================================
                                            END OF TEST VOLUME
==================================================================================================*/

/*==================================================================================================
                                            TEST BALANCE
==================================================================================================*/
                /* Select the output device to activate */
                ret = ask_user("\n TEST BALANCE on HEADSET. Press 'Y' when ready, 'N' to skip");
                if (!ret)
                {
                        audio_config.active_output = SOUND_MASK_VOLUME;
                        err_1 += balance_test(fd_audio, fd_mixer, audio_config, params);
                }

                ret = ask_user("\n TEST BALANCE on LINEOUT. Press 'Y' when ready, 'N' to skip");
                if (!ret)
                {
                        audio_config.active_output = SOUND_MASK_PCM;
                        err_1 += balance_test(fd_audio, fd_mixer, audio_config, params);
                }

                if (!err_1)
                        tst_resm(TINFO, "TEST BALANCE passed with success\n");
                else
                        tst_resm(TFAIL, "Encountered %d errors\n", err_1);

                err += err_1;
                err_1 = 0;
/*==================================================================================================
                                        END OF TEST BALANCE
==================================================================================================*/

/*==================================================================================================
                                            TEST FILTER
==================================================================================================*/
                ret = ask_user("\n TEST FILTER on HEADSET. Press 'Y' when ready, 'N' to skip");
                if (!ret)
                {
                        audio_config.active_output = SOUND_MASK_VOLUME;
                        err_1 += codec_filter_test(fd_audio, fd_mixer, audio_config, params);
                }
                if (!err_1)
                        tst_resm(TINFO, "TEST FILTER passed with success");
                else
                        tst_resm(TFAIL, "Encountered %d errors", err_1);

                err += err_1;
                err_1 = 0;
/*==================================================================================================
                                        END OF TEST FILTER
==================================================================================================*/

                fclose(params.fd_file);
                close(fd_audio);
                close(fd_mixer);

                if (!err_1)
                        tst_resm(TINFO, "All tests on CODEC passed with success");
                else
                        tst_resm(TFAIL, "Encountered %d errors", err_1);
        }

/*==================================================================================================
                                        END TEST CODEC
==================================================================================================*/

/*==================================================================================================
                                        TEST ADDER
==================================================================================================*/
#ifdef CONFIG_MXC_MC13783_PMIC
        if (params.device == ADDER)
        {
                tst_resm(TINFO, "NOT SUPPORTED");

                tst_resm(TINFO, "ADDER TEST on STDAC");

                if ((fd_audio = open(STDAC_DEV, O_WRONLY)) < 0)
                {
                        tst_resm(TFAIL, "Error opening %s", STDAC_DEV);
                        return TFAIL;
                }

                if ((fd_mixer = open(MIXER_DEV, O_RDWR)) < 0)
                {
                        tst_resm(TFAIL, "Error opening %s", MIXER_DEV);
                        return TFAIL;
                }

                /* Get the default audio settings to update the current settings structure */
                err += get_settings(fd_audio, fd_mixer, &audio_config);

                /* Set the settings (should not do anything) */
                err += set_settings(fd_audio, fd_mixer, audio_config);

                /* Open the test samples */
                if ((params.fd_file = fopen(params.testfilename, "r")) == 0)
                {
                        err_1++;
                        goto _err_file_not_found;
                }

                /* Parse the header of the test file and update the audio configuration */
                if (set_audio_config(fd_audio, fd_mixer, params.fd_file) < 0)
                {
                        tst_resm(TFAIL, "Bad format for file 1");
                        err_1++;
                        goto _err_fmt;
                }

                audio_config.volume = (75 << 8) | 75;
                audio_config.balance = 50;

                /* Play on a mono output device */
                tst_resm(TINFO, "TEST ADDER on PHONEOUT");
                audio_config.active_output = SOUND_MASK_PHONEOUT;
                err_1 += adder_test(fd_audio, fd_mixer, audio_config, params);

                /* Play on a stereo output device */
                tst_resm(TINFO, "TEST ADDER on HEADSET");
                audio_config.active_output = SOUND_MASK_VOLUME;
                err_1 += adder_test(fd_audio, fd_mixer, audio_config, params);

                /* Play on a mono and stereo output devices */
                tst_resm(TINFO, "TEST ADDER on HEADSET and PHONEOUT");
                audio_config.active_output = SOUND_MASK_PHONEOUT || SOUND_MASK_VOLUME;
                err_1 += adder_test(fd_audio, fd_mixer, audio_config, params);

                fclose(params.fd_file);
                close(fd_audio);
                close(fd_mixer);

                if (!err_1)
                        tst_resm(TINFO, "TEST ADDER passed with success");
                else
                        tst_resm(TFAIL, "Encountered %d errors", err_1);

                err += err_1;
                err_1 = 0;
        }
#endif
/*==================================================================================================
                                    END OF TEST ADDER
==================================================================================================*/
        if (!err)
        {
                rv = TPASS;
                tst_resm(TINFO, "All tests passed with success");
        }
        else
                tst_resm(TFAIL, "Encountered %d errors", err);

        return rv;

        _err_fmt:
        close(fd_audio);

        _err_file_not_found:
        tst_resm(TFAIL, "Error opening the file %s", params.testfilename);
        close(fd_audio);

        return rv;
}
