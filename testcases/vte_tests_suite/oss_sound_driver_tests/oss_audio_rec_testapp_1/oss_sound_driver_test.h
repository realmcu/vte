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
        @file   oss_sound_driver_test.h

        @brief  OSS audio record test header file.
====================================================================================================
Revision History:
                            Modification     Tracking
Author/core ID                  Date          Number    Description of Changes
-------------------------   ------------    ----------  -------------------------------------------
RB657C/gsch1c                20/07/2004     TLSbo43102  Initial version  of OSS sound driver test development
D.Simakov/smkd001c           06/10/2005     TLSbo53199  Syncronization was added. The code was formatted.
I.Inkina/nknl001             11/01/2005     TLSbo61044  Clock master was added
D.Khoroshev/b00313           03/02/2006     TLSbo62323  Removed Clock Master option
D.Simakov                    13/06/2006     TLSbo67022  Exclude #include <asm/arch/audio_controls.h>
D.Simakov                    07/12/2006     TLSbo76144  Updated with the new kernel
==================================================================================================*/
#ifndef oss_sound_driver_TEST_H
#define oss_sound_driver_TEST_H

#ifdef __cplusplus
extern "C"{
#endif

/*==================================================================================================
                                        INCLUDE FILES
==================================================================================================*/
#include <stdlib.h>
#include <signal.h>
#include <stdio.h>
#include <math.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>
#include <sys/ioctl.h>
#include <linux/soundcard.h>
#include <linux/autoconf.h>

typedef struct _mxc_cfg 
{
        int reg;        /*!< The register address (data sheet value) */
        int val;        /*!< The value to write, or returned by read */
} mxc_cfg;

/*! debug only */
#define SNDCTL_MXC_HW_SSI_CFG_W       _SIOW('Z', 0, mxc_cfg)
/*! debug only */
#define SNDCTL_MXC_HW_SSI_CFG_R       _SIOR('Z', 1, mxc_cfg)
/*! debug only */
#define SNDCTL_MXC_HW_SEL_OUT_DAC     _SIO('Z', 2)
/*! debug only */
#define SNDCTL_MXC_HW_SEL_OUT_CODEC   _SIO('Z', 3)
/*! debug only */
#define SNDCTL_MXC_HW_SEL_IN_CODEC    _SIO('Z', 14)
/*! debug only */
#define SNDCTL_MC13783_READ_OUT_MIXER   _SIOR('Z', 4, int)
/*! debug only */
#define SNDCTL_MC13783_WRITE_OUT_MIXER  _SIOWR('Z', 5, int)
/*! debug only */
#define SNDCTL_MXC_HW_WRITE_REG       _SIOW('Z', 100, mxc_cfg)
/*! debug only */
#define SNDCTL_MXC_HW_READ_REG        _SIOR('Z', 101, mxc_cfg)
/*! debug only */
#define SNDCTL_MXC_HW_WRITE_SSI_STX0  _SIOW('Z', 102, int)
/*! debug only */
#define SNDCTL_DBMX_HW_SSI_LOOPBACK   _SIOW('Z', 103, int)
/*! debug only */
#define SNDCTL_DBMX_HW_SSI_FIFO_FULL  _SIO('Z', 104)

/*==================================================================================================
                                            CONSTANTS
==================================================================================================*/

/*==================================================================================================
                                        DEFINES AND MACROS
==================================================================================================*/

/*==================================================================================================
                                            ENUMS
==================================================================================================*/

/*==================================================================================================
                                STRUCTURES AND OTHER TYPEDEFS
==================================================================================================*/

/*==================================================================================================
                                GLOBAL VARIABLE DECLARATIONS
==================================================================================================*/

/*==================================================================================================
                                    FUNCTION PROTOTYPES
==================================================================================================*/
int VT_oss_sound_driver_test(int p_src, int p_type, int p_bytes, int p_bits, int p_speed,
                             int p_chan);
int VT_oss_sound_driver_setup(void);
int VT_oss_sound_driver_cleanup(void);

#ifdef __cplusplus
}
#endif

#endif        /* oss_sound_driver_TEST_H */
