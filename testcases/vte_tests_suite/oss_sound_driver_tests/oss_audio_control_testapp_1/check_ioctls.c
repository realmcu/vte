/*================================================================================================*/
/**
        @file   check_ioctls.c

        @brief  OSS audio mixer controls test.
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
-------------------------   ------------    ----------  --------------------------------------------
Franois GAFFIE/
RB657C Guillaume SCHMUCK/
gsch1c                      20/07/2004      TLSbo40898  Initial version  of OSS sound driver test 
                                                        development.
D.Simakov/smkd001c          01/08/2005      TLSBo53192  IOCTL's checks was fixed         
A.Ozerov/b00320             26/10/2005      TLSbo56870  MC13783 tests was changed to SC55112 tests
A.Ozerov/b00320             12/12/2005      TLSbo60058  Fix set_mixer function
D.Khoroshev/b00313          02/03/2005      TLSbo61495  Added ioctl return values checking
D.Khoroshev/b00313          03/03/2006      TLSbo62323  Update accordance to last MXC OSS specifications
D.Khoroshev/b00313          03/14/2006      TLSbo62673  Reworked version
D.Simakov                   13/06/2006      TLSbo67022  STDAC <==> CODEC
D.Simakov                   19/10/2006      TLSbo76144  dsp->adsp, dsp1->dsp
A.Ozerov/b00320             13/11/2006      TLSbo81934  Adaptation with alsa-oss emulation
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

/*================================================================================================*/
char *AFMT[] =
{ 
        "AFMT_MU_LAW",
        "AFMT_A_LAW",
        "AFMT_IMA_ADPCM",
        "AFMT_U8",
        "AFMT_S16_LE",
        "AFMT_S16_BE",
        "AFMT_S8",
        "AFMT_U16_LE",
        "AFMT_U16_BE",
        "AFMT_MPEG",
        "AFMT_AC3"
};

char *CAP[] =
{
        "DSP_CAP_REVISION",
        "DSP_CAP_DUPLEX",
        "DSP_CAP_REALTIME",
        "DSP_CAP_BATCH",
        "DSP_CAP_COPROC",
        "DSP_CAP_TRIGGER",
        "DSP_CAP_MMAP",
        "DSP_CAP_MULTI",
        "DSP_CAP_BIND"
};

char *MIXER_DEVS[] =
{
        "SOUND_MIXER_VOLUME",
        "SOUND_MIXER_BASS",
        "SOUND_MIXER_TREBLE",
        "SOUND_MIXER_SYNTH",
        "SOUND_MIXER_PCM",
        "SOUND_MIXER_SPEAKER",
        "SOUND_MIXER_LINE",
        "SOUND_MIXER_MIC",
        "SOUND_MIXER_CD",
        "SOUND_MIXER_IMIX",
        "SOUND_MIXER_ALTPCM",
        "SOUND_MIXER_RECLEV",
        "SOUND_MIXER_IGAIN",
        "SOUND_MIXER_OGAIN",
        "SOUND_MIXER_LINE1",
        "SOUND_MIXER_LINE2",
        "SOUND_MIXER_LINE3",
        "SOUND_MIXER_DIGITAL1",
        "SOUND_MIXER_DIGITAL2",
        "SOUND_MIXER_DIGITAL3",
        "SOUND_MIXER_PHONEIN",
        "SOUND_MIXER_PHONEOUT",
        "SOUND_MIXER_VIDEO",
        "SOUND_MIXER_RADIO",
        "SOUND_MIXER_MONITOR"
};

/*==================================================================================================
                                        LOCAL FUNCTIONS
==================================================================================================*/
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
        return TPASS;
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
        return TPASS;
}

/*================================================================================================*/
/**
@brief  

@param  int fd_mixer
        int fd_audio
        int device
    
@return 
*/
/*================================================================================================*/
int set_input_bias(int fd_mixer, int fd_audio, int device)
{
        int     val = device;
        int     err = 0, nb_err=0;

        if (ioctl(fd_mixer, SOUND_MIXER_WRITE_RECSRC, &val) < 0)
        {
                tst_resm(TFAIL, "%s: Error in SOUND_MIXER_WRITE_RECSRC for device 0x%x. Returned error code '%s'",
                                __FUNCTION__, device, strerror(errno));        
                nb_err++;
        }
        if (ioctl(fd_mixer, SOUND_MIXER_READ_RECSRC, &val) < 0)
        {
                tst_resm(TFAIL, "%s: Error in SOUND_MIXER_READ_RECSRC for device 0x%x. Returned error code '%s'",
                                __FUNCTION__, device, strerror(errno));
                nb_err++;
        }
        if (val != device && nb_err == 0)
        {
                tst_resm(TFAIL, "%s: Failed to set input device 0x%x (SOUND_MIXER_WRITE_RECSRC). Value write: 0x%x, read: 0x%x", __FUNCTION__, device, device, val);
                nb_err++;
        }

        err += nb_err;
        nb_err = 0;
        val = 1;
#ifdef CONFIG_MXC_PMIC_SC55112
        /* Not Supported 
        if (ioctl(fd_audio, SNDCTL_PMIC_WRITE_IN_BIAS, &val) < 0)
        {        
                tst_resm(TFAIL, "%s: Error in SNDCTL_PMIC_WRITE_IN_BIAS for device 0x%x. Returned error code '%s'",
                                __FUNCTION__, device, strerror(errno));
                nb_err++;
        }
        if (ioctl(fd_audio, SNDCTL_PMIC_READ_IN_BIAS, &val) < 0)
        {
                tst_resm(TFAIL, "%s: Error in SNDCTL_PMIC_READ_IN_BIAS for device 0x%x. Returned error code '%s'",
                                __FUNCTION__, device, strerror(errno));
                nb_err++;

        }
                err++;
        if (val != 1 && nb_err == 0)
        {
                tst_resm(TFAIL, "%s: Failed to set input bias (SNDCTL_PMIC_WRITE_IN_BIAS) for device 0x%x. Value write: 0x%x, read: 0x%x",
                                __FUNCTION__, device, device, val);
                nb_err++;
        }*/
#endif

#ifdef CONFIG_MXC_MC13783_PMIC
        /* NOT SUPPORTED
        if (ioctl(fd_audio, SNDCTL_MC13783_WRITE_IN_BIAS, &val) < 0)
        {
                tst_resm(TFAIL, "%s: Error in SNDCTL_MC13783_WRITE_IN_BIAS for device 0x%x. Returned error code '%s'",
                                __FUNCTION__, device, strerror(errno));
                nb_err++;
        }

                err++;
        if (ioctl(fd_audio, SNDCTL_MC13783_READ_IN_BIAS, &val) < 0)
        {
                tst_resm(TFAIL, "%s: Error in SNDCTL_MC13783_READ_IN_BIAS for device 0x%x. Returned error code '%s'",
                                __FUNCTION__, device, strerror(errno));
                nb_err++;
        }

        if (val != 1 && nb_err == 0)
        {
                tst_resm(TFAIL, "%s: Failed to set input bias (SNDCTL_MC13783_WRITE_IN_BIAS) for device 0x%x. Value write: 0x%x, read: 0x%x",
                                __FUNCTION__, device, device, val);
                nb_err++;
        }
        */
#endif
        err += nb_err;

        return err;
}

/*================================================================================================*/
/**
@brief  

@param  int fd_mixer
        int curr_device

@return 
*/
/*================================================================================================*/
int set_output_device(int fd_mixer, int device)
{
        int     val = device;
        int     err = 0;

        if (ioctl(fd_mixer, SOUND_MIXER_WRITE_OUTSRC, &val) < 0)
        {
                tst_resm(TFAIL, "%s: Error in SOUND_MIXER_WRITE_OUTSRC for device 0x%x. Returned error code '%s'",
                                __FUNCTION__, device, strerror(errno));
                err++;
        }

        if (ioctl(fd_mixer, SOUND_MIXER_READ_OUTSRC, &val) < 0)
        {
                tst_resm(TFAIL, "%s: Error in SOUND_MIXER_READ_OUTSRC for device 0x%x. Returned error code '%s'",
                                __FUNCTION__, device, strerror(errno));
                err++;
        }

        if (!err && !(val & device))
        {
                tst_resm(TFAIL, "%s: Failed to set output device 0x%x (SOUND_MIXER_WRITE_OUTSRC). Value write: 0x%x, read: 0x%x",
                                __FUNCTION__, device, device, val);
                err++;
        }

        return err;
}

/*================================================================================================*/
/**
@brief  

@param  int fd_audio

@return 
*/
/*================================================================================================*/
int set_bits(int fd_audio)
{
        int     ret = 0,
                err = 0;
        int     val;
        int     refval = AFMT_S16_LE;

        val = refval;

        ret = ioctl(fd_audio, SOUND_PCM_WRITE_BITS, &val);
        if (ret < 0)
        {
                tst_resm(TFAIL, "%s: Error in IOCTL SOUND_PCM_WRITE_BITS 0x%08x. Returned error code '%s'",
                                __FUNCTION__, AFMT_U8, strerror(errno));
                err++;
        }

        ret = ioctl(fd_audio, SOUND_PCM_READ_BITS, &val);
        if (ret < 0)
        {
                tst_resm(TFAIL, "%s: Error in IOCTL SOUND_PCM_READ_BITS. Returned error code '%s'",
                                __FUNCTION__, strerror(errno));
                err++;
        }
        if (refval != val && err == 0)
        {
                tst_resm(TFAIL, "%s: Failed to set bits (IOCTL SOUND_PCM_WRITE_BITS). set: 0x%08x, get: 0x%08x",
                                __FUNCTION__, refval, val);
                err++;
        }

        if (err)
        {
                tst_resm(TFAIL, "Bitrate was not set to 8-bit. %d-bit supported", val);
        }

        return err;
}

/*================================================================================================*/
/**
@brief  

@param  int fd_audio
        int max_channel

@return 
*/
/*================================================================================================*/
int set_channels(int fd_audio, int max_channel)
{
        int     ret = 0,
                err = 0, nb_err = 0;
        int     val = 0;

        ret = ioctl(fd_audio, SOUND_PCM_WRITE_CHANNELS, &max_channel);
        if(ret < 0)
        {
                tst_resm(TFAIL, "%s: Error in IOCTL SNDCTL_DSP_STEREO %d. Returned error code '%s'",
                                __FUNCTION__, max_channel, strerror(errno));
                nb_err++;
        }

        ret = ioctl(fd_audio, SOUND_PCM_READ_CHANNELS, &val);
        if(ret < 0)
        {
                tst_resm(TFAIL, "%s: Error in IOCTL SOUND_PCM_READ_CHANNELS. Returned error code '%s'",
                                __FUNCTION__, strerror(errno));
                nb_err++;
        }
        tst_resm(TINFO, "Value of channels number. set: %d, get:%d", max_channel, val);

        if(val != max_channel)
        {
                tst_resm(TFAIL, "%s: Failed to set number of channels(IOCTL SNDCTL_DSP_STEREO) set:%d, get:%d",
                                __FUNCTION__, max_channel, val);
                nb_err++;

        }
        err += nb_err;

        return err;
}

/*================================================================================================*/
/**
@brief  

@param  None

@return 
*/
/*================================================================================================*/
int test_channels(void)
{
        int     err = 0;
        int     fd_audio;
        int     val = 0;
        int     ret = 0;

        tst_resm(TINFO, "");
        tst_resm(TINFO, "Setting channels for VOICE CODEC...");
        if ((fd_audio = open(CODEC_DEV, O_RDWR)) < 0)
        {
                tst_resm(TFAIL, "%s: Error opening %s", __FUNCTION__, CODEC_DEV);
                err++;
                return err;
        }

        ret = ioctl(fd_audio, SOUND_PCM_READ_CHANNELS, &val);
        if(ret < 0)
        {
                tst_resm(TFAIL, "%s: Error in IOCTL SOUND_PCM_READ_CHANNELS. Returned error code '%s'",
                                __FUNCTION__, strerror(errno));
                err++;
        }
        tst_resm(TINFO, "First value of channels number: %d", val);

        err += set_channels(fd_audio, 1);
        err += set_channels(fd_audio, 2);
        close(fd_audio);

        tst_resm(TINFO, "");
        tst_resm(TINFO, "Setting channels for STEREO DAC...");
        if ((fd_audio = open(STDAC_DEV, O_WRONLY)) < 0)
        {
                tst_resm(TFAIL, "%s: Error opening %s", __FUNCTION__, STDAC_DEV);
                err++;
                return err;
        }

        ret = ioctl(fd_audio, SOUND_PCM_READ_CHANNELS, &val);
        if(ret < 0)
        {
                tst_resm(TFAIL, "%s: Error in IOCTL SOUND_PCM_READ_CHANNELS. Returned error code '%s'",
                                __FUNCTION__, strerror(errno));
                err++;
        }
        tst_resm(TINFO, "First value of channels number: %d", val);

        err += set_channels(fd_audio, 1);
        err += set_channels(fd_audio, 2);
        close(fd_audio);
        
        return err;
}

/*================================================================================================*/
/**
@brief  

@param  int fd_audio

@return 
*/
/*================================================================================================*/
int set_speed_stdac(int fd_audio)
{
        int     ret = 0,
                err = 0,
                nb_err = 0;
        int     val,
                i,
                mem;

        int     supported_speeds[] = 
                {
                        8000, 11025, 12000, 16000, 22050, 24000, 32000, 44100, 48000
#ifdef CONFIG_MXC_MC13783_PMIC
                        , 64000, 96000
#endif
                };


        int     nb_supported_speeds = sizeof(supported_speeds) / sizeof(int);

        tst_resm(TINFO, "");
        tst_resm(TINFO, "Testing supported speed on stdac device");
        for (i = 0; i < nb_supported_speeds; i++)
        {
                nb_err = 0;
                mem = val = supported_speeds[i];
                ret = ioctl(fd_audio, SOUND_PCM_WRITE_RATE, &val);
                if (ret < 0)
                {
                        tst_resm(TFAIL, "%s: Error in IOCTL SOUND_PCM_WRITE_RATE (%d). Returned error code '%s'",
                                        __FUNCTION__, mem, strerror(errno));
                        nb_err++;
                }
                ret = ioctl(fd_audio, SOUND_PCM_READ_RATE, &val);
                if (ret < 0)
                {
                        tst_resm(TFAIL, "%s: Error in IOCTL SOUND_PCM_READ_RATE. Returned error code '%s'",
                                        __FUNCTION__, strerror(errno));
                        nb_err++;
                }
                        
                if (!nb_err && val != mem)
                {
                        tst_resm(TFAIL, "%s: Failed to set speed rate to %d, returned value %d.",
                                        __FUNCTION__, mem, val, strerror(errno));
                        nb_err++;
                }
                if(!nb_err)
                {
                        tst_resm(TINFO, "%s: IOCTL SNDCTL_DSP_SPEED(%d, returned value: %d)", __FUNCTION__, mem, val);
                }
                err += nb_err;
        }

        return err;
}

/*================================================================================================*/
/**
@brief  

@param  int fd_audio

@return 
*/
/*================================================================================================*/
int set_speed_codec(int fd_audio)
{
        int     ret = 0,
                    err = 0,
                nb_err = 0;
        int     val,
                i,
                mem;
        int     supported_speeds[] = 
                {
                        8000, 16000
                };

        int     nb_supported_speeds = sizeof(supported_speeds) / sizeof(int);

        tst_resm(TINFO, "");
        tst_resm(TINFO, "Testing supported speed on codec device");

        for (i = 0; i < nb_supported_speeds; i++)
        {
                nb_err = 0;
                mem = val = supported_speeds[i];
                ret = ioctl(fd_audio, SOUND_PCM_WRITE_RATE, &val);
                if (ret < 0)
                {
                        tst_resm(TFAIL, "%s: Error in IOCTL SOUND_PCM_WRITE_RATE (%d). Returned error code '%s'",
                                        __FUNCTION__, mem, strerror(errno));
                        nb_err++;
                }
                ret = ioctl(fd_audio, SOUND_PCM_READ_RATE, &val);
                if (ret < 0)
                {
                        tst_resm(TFAIL, "%s: Error in IOCTL SOUND_PCM_READ_RATE. Returned error code '%s'",
                                        __FUNCTION__, strerror(errno));
                        nb_err++;
                }

                if (!nb_err && val != mem)
                {
                        tst_resm(TFAIL, "%s: Failed to set speed rate to %d, returned value %d.",
                                        __FUNCTION__, mem, val);
                        nb_err++;
                }
                if(!nb_err)
                {
                        tst_resm(TINFO, "%s: IOCTL SOUND_PCM_WRITE_RATE is Ok (%d)", __FUNCTION__, val);
                }
                err += nb_err;
        }

        return err;
}

/*================================================================================================*/
/**
@brief  

@param  int fd_audio
        int device

@return 
*/
/*================================================================================================*/
int set_speed(int fd_audio, int device)
{
        int     err = 0;

        if (device == 1)
        {
                err += set_speed_codec(fd_audio);
        }
        else
        {
                err += set_speed_stdac(fd_audio);
        }

        return err;
}

/*================================================================================================*/
/**
@brief  

@param  int fd_audio
        int device

@return 
*/
/*================================================================================================*/
int check_audio_ioctls(int fd_audio, int device)
{
        int     err = 0, blk_size = 0;
        int     i = AFMT_QUERY;
        int     j = 0, k = 0;

        err += set_bits(fd_audio);
        err += set_speed(fd_audio, device);

        tst_resm(TINFO, "");
        if (ioctl(fd_audio, SNDCTL_DSP_RESET, NULL) != 0)
        {
                tst_resm(TFAIL, "Error in IOCTL SNDCTL_DSP_RESET. Error code: %s", strerror(errno));
                err++;
        }
        else
                tst_resm(TINFO, "IOCTL SNDCTL_DSP_RESET is OK");

        if (ioctl(fd_audio, SNDCTL_DSP_SYNC, NULL) != 0)
        {
                tst_resm(TFAIL, "Error in IOCTL SNDCTL_DSP_SYNC. Error code: %s", strerror(errno));
                err++;
        }
        else
                tst_resm(TINFO, "IOCTL SNDCTL_DSP_SYNC is OK");

        if (ioctl(fd_audio, SNDCTL_DSP_GETBLKSIZE, &blk_size) != 0)
        {
                tst_resm(TFAIL, "Error in IOCTL SNDCTL_DSP_GETBLKSIZE. Error code: %s", strerror(errno));
                err++;
        }
        else
                tst_resm(TINFO, "IOCTL SNDCTL_DSP_GETBLKSIZE is OK, block size: %d", blk_size);

        if (ioctl(fd_audio, SNDCTL_DSP_SETFMT, &i) != 0)
        {
                tst_resm(TFAIL, "Error in IOCTL SNDCTL_DSP_SETFMT(%d). Error code: %s", i,  strerror(errno));
                err++;
        }
        else
        {
                tst_resm(TINFO, "IOCTL SNDCTL_DSP_SETFMT is OK. Current format: %d", i);
        }

        i = AFMT_U8;
        do
        {
                j = i;
                if (ioctl(fd_audio, SNDCTL_DSP_SETFMT, &i) != 0)
                {
                        tst_resm(TFAIL, "Error in IOCTL SNDCTL_DSP_SETFMT(%d). Error code: %s", i,  strerror(errno));
                        err++;
                }
                else
                {
                        tst_resm(TINFO, "IOCTL SNDCTL_DSP_SETFMT is OK. Set: %d, got: %d", j, i);
                }
                i <<= 1;

        }while(i <= AFMT_U16_BE);

        if (ioctl(fd_audio, SOUND_PCM_READ_BITS, &i) != 0)
        {
                tst_resm(TFAIL, "Error in IOCTL SOUND_PCM_READ_BITS. Error code: %s", strerror(errno));
                err++;
        }
        else
        {
                tst_resm(TINFO, "IOCTL SOUND_PCM_READ_BITS is OK. got format: %d", i);
        }

        for (i = 1; i <= 128; i++)
        {
                j = i;
                if (ioctl(fd_audio, SNDCTL_DSP_CHANNELS, &i) != 0)
                {
                        tst_resm(TFAIL, "Error in IOCTL SNDCTL_DSP_CHANNELS(%d). Error code: %s", i,  strerror(errno));
                        err++;
                }
                else
                {
                        tst_resm(TINFO, "IOCTL SNDCTL_DSP_CHANNELS is OK. Set: %d, got: %d", j, i);
                }
        }

        if (ioctl(fd_audio, SOUND_PCM_READ_CHANNELS, &i) != 0)
        {
                tst_resm(TFAIL, "Error in IOCTL SOUND_PCM_READ_CHANNELS. Error code: %s", strerror(errno));
                err++;
        }
        else
        {
                tst_resm(TINFO, "IOCTL SOUND_PCM_READ_CHANNELS is OK. got channel: %d", i);
        }

        if (ioctl(fd_audio, SNDCTL_DSP_POST, NULL) != 0)
        {
                tst_resm(TFAIL, "Error in IOCTL SNDCTL_DSP_POST. Error code: %s", strerror(errno));
                err++;
        }
        else
        {
                tst_resm(TINFO, "IOCTL SNDCTL_DSP_POST is OK");
        }

        if (ioctl(fd_audio, SNDCTL_DSP_GETFMTS, &i) != 0)
        {
                tst_resm(TFAIL, "Error in IOCTL SNDCTL_DSP_GETFMTS. Error code: %s", strerror(errno));
                err++;
        }
        else
        {
                tst_resm(TINFO, "IOCTL SNDCTL_DSP_GETFMTS is OK. Got value: 0x%x", i);
                j = AFMT_MU_LAW;
                k = 0;
                while(j <= AFMT_AC3)
                {
                        if(i&j)
                        {
                                tst_resm(TINFO, "Format %s was set", AFMT[k]);
                        }
                        j <<= 1;
                        k++;
                }
        }

        if (ioctl(fd_audio, SNDCTL_DSP_NONBLOCK, NULL) != 0)
        {
                tst_resm(TFAIL, "Error in IOCTL SNDCTL_DSP_NONBLOCK. Error code: %s", strerror(errno));
                err++;
        }
        else
        {
                tst_resm(TINFO, "IOCTL SNDCTL_DSP_NONBLOCK is Ok");
        }

        if (ioctl(fd_audio, SNDCTL_DSP_GETCAPS, &i) != 0)
        {
                tst_resm(TFAIL, "Error in IOCTL SNDCTL_DSP_GETCAPS. Error code: %s", strerror(errno));
                err++;
        }
        else
        {
                tst_resm(TINFO, "IOCTL SNDCTL_DSP_GETCAPS is Ok. Got value: 0x%x", i);
                j = DSP_CAP_REVISION;
                k = 0;
                while(j <= DSP_CAP_BIND)
                {
                        if(i&j)
                        {
                                tst_resm(TINFO, "Capability %s was set", CAP[k]);
                        }
                        j <<= 1;
                        k++;
                }
        }

        if (device == 1)
        {
                i = PCM_ENABLE_OUTPUT;
                if (ioctl(fd_audio, SNDCTL_DSP_SETTRIGGER, &i) != 0)
                {
                        tst_resm(TFAIL, "Error in IOCTL SNDCTL_DSP_SETTRIGGER. Error code: %s", strerror(errno));
                        err++;
                }
                else
                {
                        tst_resm(TINFO, "IOCTL SNDCTL_DSP_SETTRIGGER is Ok(PCM_ENABLE_OUTPUT)");
                        if (ioctl(fd_audio, SNDCTL_DSP_GETTRIGGER, &j) != 0)
                        {
                                tst_resm(TFAIL, "Error in IOCTL SNDCTL_DSP_GETTRIGGER. Error code: %s", strerror(errno));
                                err++;
                        }
                }

                i = PCM_ENABLE_INPUT;
                if (ioctl(fd_audio, SNDCTL_DSP_SETTRIGGER, &i) != 0)
                {
                        tst_resm(TFAIL, "Error in IOCTL SNDCTL_DSP_SETTRIGGER. Error code: %s", strerror(errno));
                        err++;
                }
                else
                {
                        tst_resm(TINFO, "IOCTL SNDCTL_DSP_SETTRIGGER is Ok(PCM_ENABLE_INPUT)");
                        if (ioctl(fd_audio, SNDCTL_DSP_GETTRIGGER, &j) != 0)
                        {
                                tst_resm(TFAIL, "Error in IOCTL SNDCTL_DSP_GETTRIGGER. Error code: %s", strerror(errno));
                                err++;
                        }
                        else
                        {
                                if (i != j)
                                        tst_resm(TINFO, "Set and got trigger values aren't equal: (%d, %d)", i, j);
                                else
                                        tst_resm(TINFO, "Set and got trigger values are equal: (%d, %d)", i, j);
                        }
                }
        }
        else
        {
                i = PCM_ENABLE_INPUT;
                if (ioctl(fd_audio, SNDCTL_DSP_SETTRIGGER, &i) != 0)
                {
                        tst_resm(TFAIL, "Error in IOCTL SNDCTL_DSP_SETTRIGGER. Error code: %s", strerror(errno));
                        err++;
                }
                else
                {
                        tst_resm(TINFO, "IOCTL SNDCTL_DSP_SETTRIGGER is Ok(PCM_ENABLE_INPUT)");
                        if (ioctl(fd_audio, SNDCTL_DSP_GETTRIGGER, &j) != 0)
                        {
                                tst_resm(TFAIL, "Error in IOCTL SNDCTL_DSP_GETTRIGGER. Error code: %s", strerror(errno));
                                err++;
                        }
                        else
                        {
                                if (i != j)
                                        tst_resm(TINFO, "Set and got trigger values aren't equal: (%d, %d)", i, j);
                                else
                                        tst_resm(TINFO, "Set and got trigger values are equal: (%d, %d)", i, j);
                        }
                }
        }

        if (ioctl(fd_audio, SNDCTL_DSP_GETODELAY, &i) != 0)
        {
                tst_resm(TFAIL, "Error in IOCTL SNDCTL_DSP_GETODELAY. Error code: %s", strerror(errno));
                err++;
        }
        else
        {
                tst_resm(TINFO, "IOCTL SNDCTL_DSP_GETODELAY is OK. Delay: %d", i);
        }

        tst_resm(TINFO, "");

        if (!err)
                tst_resm(TINFO, "No Error in AUDIO IOCTLs");

        return err;
}

/*================================================================================================*/
/**
@brief  

@param  int fd_mixer

@return 
*/
/*================================================================================================*/
int check_mixer_ioctls(int fd_mixer)
{
        int     err = 0;
        int     val = 0, i = 0, j = 0;

        tst_resm(TINFO, "Testing the mixer ioctls...");

        if (ioctl(fd_mixer, OSS_GETVERSION, &val) < 0)
        {
                tst_resm(TFAIL, "%s: Error in IOCTL OSS_GETVERSION. Returned error code '%s'",
                                 __FUNCTION__, strerror(errno));
                err++;
        }
        else
                tst_resm(TINFO, "%s: IOCTL OSS_GETVERSION is Ok. Version: %d", __FUNCTION__, val);

        if (ioctl(fd_mixer, SOUND_MIXER_READ_DEVMASK, &val) < 0)
        {
                tst_resm(TFAIL, "%s: Error in IOCTL SOUND_MIXER_READ_DEVMASK. Returned error code '%s'",
                                 __FUNCTION__, strerror(errno));
                err++;
        }
        else
        {
                tst_resm(TINFO, "%s: Ioctl SOUND_MIXER_READ_DEVMASK is Ok. Devmask: 0x%x", __FUNCTION__, val);
                j = SOUND_MIXER_VOLUME;
                while(j <= SOUND_MIXER_MONITOR)
                {
                        if(val&j)
                        {
                                tst_resm(TINFO, "Mixer device %s was set", MIXER_DEVS[j]);
                        }
                        j++;
                }
        }

        if (ioctl(fd_mixer, SOUND_MIXER_READ_RECMASK, &val) < 0)
        {
                tst_resm(TFAIL, "%s: Error in IOCTL SOUND_MIXER_READ_RECMASK. Returned error code '%s'",
                                 __FUNCTION__, strerror(errno));
                err++;
        }
        else
        {
                tst_resm(TINFO, "%s: Ioctl SOUND_MIXER_READ_RECMASK is Ok. Recmask: 0x%x", __FUNCTION__, val);
                j = SOUND_MIXER_VOLUME;
                while(j <= SOUND_MIXER_MONITOR)
                {
                        if(val&j)
                        {
                                tst_resm(TINFO, "Rec device %s was set", MIXER_DEVS[j]);
                        }
                        j++;
                }
        }

        if (ioctl(fd_mixer, SOUND_MIXER_READ_STEREODEVS, &val) < 0)
        {
                tst_resm(TFAIL, "%s: Error in IOCTL SOUND_MIXER_READ_STEREODEVS. Returned error code '%s'",
                                __FUNCTION__, strerror(errno));
                err++;
        }
        else
        {
                tst_resm(TINFO, "%s: SOUND_MIXER_READ_STEREODEVS Supported: SOUND_MASK_VOLUME %d, SOUND_MASK_PHONEIN %d, SOUND_MASK_PCM %d",
                 __FUNCTION__, SOUND_MASK_VOLUME & val ? 1 : 0,
                 SOUND_MASK_PHONEIN & val ? 1 : 0,
                 SOUND_MASK_PCM & val ? 1 : 0);
        }

        if (ioctl(fd_mixer, SOUND_MIXER_READ_RECSRC, &val) < 0)
        {
                tst_resm(TFAIL, "%s: Error in IOCTL SOUND_MIXER_READ_RECSRC. Returned error code '%s'",
                                __FUNCTION__, strerror(errno));
                err++;
        }
        else
        {
                tst_resm(TINFO, "%s: SOUND_MIXER_READ_RECSRC Supported: SOUND_MASK_LINE %d, SOUND_MASK_PHONEIN %d, SOUND_MASK_MIC %d",
                                __FUNCTION__, SOUND_MASK_LINE & val ? 1 : 0,
                                SOUND_MASK_PHONEIN & val ? 1 : 0,
                                SOUND_MASK_MIC & val ? 1 : 0);
        }

        if (ioctl(fd_mixer, SOUND_MIXER_READ_CAPS, &val) < 0)
        {
                tst_resm(TFAIL, "%s: Error in IOCTL SOUND_MIXER_READ_CAPS. Returned error code '%s'",
                                __FUNCTION__, strerror(errno));
                err++;
        }
        else if (val & SOUND_CAP_EXCL_INPUT)
        {
                tst_resm(TINFO, "%s: Support of SOUND_CAP_EXCL_INPUT(IOCTL SOUND_MIXER_READ_CAPS)", __FUNCTION__);
        }

        for (val = 0; val <= 100; val += 50)
        {
                i = val;
                if (ioctl(fd_mixer, SIOC_IN, &i) != 0)
                {
                        tst_resm(TFAIL, "Error in IOCTL SIOC_IN. Error code: %s", strerror(errno));
                        err++;
                }
                else
                {
                        tst_resm(TINFO, "IOCTL SIOC_IN is OK. Set volume: %d", i & val);
                        if (ioctl(fd_mixer, SIOC_OUT, &j) != 0)
                        {
                                tst_resm(TFAIL, "Error in IOCTL SIOC_OUT. Error code: %s", strerror(errno));
                                err++;
                        }
                        else
                        {
                                tst_resm(TINFO, "IOCTL SIOC_OUT is OK. Get volume: %d", j & val);
                                if ((j&val) != val)
                                {
                                        tst_resm(TFAIL, "Set and got volumes aren't equal: (%d, %d)", val, j & val);
                                        err++;
                                }
                        }
                }
        }
        
        if (!err)
                tst_resm(TINFO, "No Error in MIXER IOCTLs");

        return err;
}

/*================================================================================================*/
/**
@brief  Test program

@param  None
    
@return On success - return TPASS
        On failure - return the error code
*/
/*================================================================================================*/
int VT_oss_sound_driver_test(int Device)
{
        int     VT_rv = TFAIL;
        int     fd_audio,
                fd_mixer;
        int     err, nb_err;

        tst_resm(TINFO, "Hi... ");
        err = 0;
        nb_err = 0;

        err += test_channels();

        if (Device == 0) /*STDAC*/
        {
                /* ! Using the stDac for the tests */
                if ((fd_audio = open(STDAC_DEV, O_WRONLY)) < 0)
                {
                        tst_resm(TFAIL, "Error opening %s. %s", STDAC_DEV,
                                 strerror(errno));
                }
                else
                {
                        err += check_audio_ioctls(fd_audio, Device);
                }
        }
        else /*CODEC*/
        {
                /* ! Using the Codec for the tests */
                if ((fd_audio = open(CODEC_DEV, O_RDWR)) < 0)
                {
                        tst_resm(TFAIL, "Error opening %s. %s",CODEC_DEV,
                                 strerror(errno));
                }
                else
                {
                        err += check_audio_ioctls(fd_audio, Device);
                }
        }

        if ((fd_mixer = open(MIXER_DEV, O_RDWR)) < 0)
        {
                tst_resm(TFAIL, "Error opening %s. %s", MIXER_DEV, strerror(errno));
        }
        else
        {
                err += check_mixer_ioctls(fd_mixer);
        }

#ifdef NOT_SUPPORTED
        nb_err = set_input_bias(fd_mixer, fd_audio, SOUND_MASK_PHONEIN);
        if (nb_err)
        {
                tst_resm(TINFO, "Error in IOCTL SNDCTL_PMIC_WRITE_IN_BIAS 1");
                err += nb_err;
        }
        nb_err = set_input_bias(fd_mixer, fd_audio, SOUND_MASK_LINE);
        if (nb_err)
        {
                tst_resm(TINFO, "Error in IOCTL SNDCTL_PMIC_WRITE_IN_BIAS 2");
                err += nb_err;
        }
        nb_err = set_input_bias(fd_mixer, fd_audio, SOUND_MASK_MIC);
        if (nb_err)
        {
                tst_resm(TINFO, "Error in IOCTL SNDCTL_PMIC_WRITE_IN_BIAS 3");
                err += nb_err;
        }
#endif
        close(fd_audio);
        close(fd_mixer);
        
        if (!err)
                VT_rv = TPASS;

        return VT_rv;
}
