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

        @brief  OSS audio control header file.
====================================================================================================
Revision History:
                            Modification     Tracking
Author/core ID                  Date          Number    Description of Changes
-------------------------   ------------    ----------  -------------------------------------------
RB657C/gsch1c                20/07/2004     TLSbo40898  Initial version  of OSS sound driver test development
A.Ozerov/b00320              07/11/2005     TLSbo56870  Added a compilation flag allowing the same code to be compatible with both SC55112 and MC13783.
A.Ozerov/b00320              12/12/2005     TLSbo60058  Problem with set_mixer was fixed.
D.Khoroshev/b00313           03/03/2006     TLSbo62323  Update accordance to last MXC OSS specifications
A.Ozerov/b00320              13/11/2006     TLSbo81934  Adaptation with alsa-oss emulation.
D.Simakov                    07/12/2006     TLSbo76144  Updated with the new kernel
====================================================================================================

====================================================================================================
Portability: ARM GCC
==================================================================================================*/
#ifndef OSS_SOUND_DRIVER_TEST_H
#define OSS_SOUND_DRIVER_TEST_H

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

#ifdef CONFIG_MXC_PMIC_SC55112
#define MXC_DBG_CFG
//#include <mxc-oss-ctrls.h>
#endif

#ifdef CONFIG_MXC_MC13783_PMIC
//#include <asm/arch/audio_controls.h>
//#include <mxc-oss-ctrls.h>
#endif

#define CODEC_DEV "/dev/sound/adsp"
#define STDAC_DEV "/dev/sound/dsp"
#define MIXER_DEV "/dev/sound/mixer"

/*==================================================================================================
                                    FUNCTION PROTOTYPES
==================================================================================================*/
int VT_oss_sound_driver_setup(void);
int VT_oss_sound_driver_cleanup(void);
int VT_oss_sound_driver_test(int Device);

#ifdef __cplusplus
}
#endif

#endif        /* OSS_SOUND_DRIVER_TEST_H */
