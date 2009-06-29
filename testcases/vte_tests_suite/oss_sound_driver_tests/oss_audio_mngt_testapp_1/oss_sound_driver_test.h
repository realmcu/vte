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

        @brief  OSS audio simple open test header file.
====================================================================================================
Revision History:
                            Modification     Tracking
Author/core ID                  Date          Number    Description of Changes
-------------------------   ------------    ----------  -------------------------------------------
RB657C/gsch1c                20/07/2004     TLSbo40898  Initial version  of OSS sound driver test development
D.Khoroshev/b00313           02/23/2006     TLSbo61805  Update according new SSI specifications
D.Simakov                    22/12/2005     TLSbo87096  Zeus compilation issue
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

#ifdef CONFIG_MXC_PMIC_SC55112
#define MXC_DBG_CFG
//#include <mxc-oss-ctrls.h>        /* header for usefull oss and mc13783 ioctl plus mask parameters */
#endif

#ifdef CONFIG_MXC_MC13783_PMIC
#include <asm/arch/audio_controls.h>
#endif

#define CODEC_DEV "/dev/sound/adsp"
#define STDAC_DEV "/dev/sound/dsp"
#define MIXER_DEV "/dev/sound/mixer"

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
int VT_oss_sound_driver_test(int inst_num);
int VT_oss_sound_driver_setup();
int VT_oss_sound_driver_cleanup();

#ifdef __cplusplus
}
#endif

#endif        /* oss_sound_driver_TEST_H */
