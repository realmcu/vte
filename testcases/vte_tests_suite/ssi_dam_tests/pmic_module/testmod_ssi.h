/*================================================================================================*/
/**
        @file   testmod_ssi.c

        @brief  SSI DAM test module header file
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
J.Quintero/jqui1c            26/09/2005     xxx         Initial version
S.V-Guilhou/svan01c          04/10/2005     TLSbo55818  Add MXC91331
D.Khoroshev/b00313           29/11/2005     TLSbo56844  Add SC55112 support
D.Simakov                    06/06/2006     TLSbo67103  No sound on SSI 1 and SSI 2
D.Kardakov                   11/09/2006     TLSbo71015  Re-written for L26_21 release
====================================================================================================
Portability:  ARM GCC
==================================================================================================*/
/*==================================================================================================
                                        INCLUDE FILES
==================================================================================================*/
/*==================================================================================================
                                        GLOBAL MACROS
==================================================================================================*/
#ifndef __TESTMOD_H__
#define __TESTMOD_H__

#define SSI_IOCTL                                0x54

#define IOCTL_SSI_CONFIGURE_SSI                  0x0
#define IOCTL_SSI_GET_CONFIGURATION              0x1
#define IOCTL_SSI_CONFIGURE_AUDMUX               0x2
#define IOCTL_SSI_CONFIGURE_PMIC                 0x3

#define STEREO_DAC                               0x0
#define CODEC                                    0x1

#define PMIC_SLAVE                               0x0
#define PMIC_MASTER                              0x1

#define MAX_CHUNK_SIZE                           (1024 * 128)

#define DEVICE_NAME_SSI1                         "/dev/mxc_ssi1"
#define DEVICE_NAME_SSI2                         "/dev/mxc_ssi2"

/*==================================================================================================
                                 STRUCTURES AND OTHER TYPEDEFS
==================================================================================================*/
/* ! Wave file configuration. */
struct wave_config
{
        /*! SSI */
        unsigned short ssi;

        /*! Number of channels (1:MONO, 2:STEREO) */
        unsigned short num_channels;

        /*! Sample rate */
        unsigned long sample_rate;

        /*! Bits per sample (16-bits mode supported) */
        unsigned short bits_per_sample;

        /*! Sample size */
        unsigned long sample_size;

        /*! Requested mixing mode */
        unsigned long mix_enabled;

        /*! Used device */
        unsigned long dac_codec;

        /*! Used audio data  bus  (AUDIO_DATA_BUS_1 or AUDIO_DATA_BUS_2)*/
        PMIC_AUDIO_DATA_BUS dac_bus;

        /*! Source clock  (CLOCK_IN_CLIA or CLOCK_IN_CLIB for mc13783) */
        PMIC_AUDIO_CLOCK_IN_SOURCE pmic_clock_source;

        /*! clock provider (BUS_SLAVE_MODE - PMIC is slave, BUS_MASTER_MODE - PMIC is master) */
        PMIC_AUDIO_BUS_MODE master_slave;

        /*! ssi fifo to be used */
        int ssi_fifo;

};

#endif        /* __TESTMOD_H__ */
