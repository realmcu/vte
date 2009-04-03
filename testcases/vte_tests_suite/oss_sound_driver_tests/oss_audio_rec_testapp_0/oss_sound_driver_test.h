/*================================================================================================*/
/**
        @file   oss_sound_driver_test.h

        @brief  OSS audio record test header file.
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
RB657C/gsch1c                20/07/2004     TLSbo40898  Initial version  of OSS sound driver test development
D.Khoroshev/B00313           02/22/2006     TLSbo62323  Code cleaning
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
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>
#include <sys/ioctl.h>
#include <linux/soundcard.h>
#include <linux/autoconf.h>

//#include <mxc_audio_mc13783_ioctls.h>
#ifdef CONFIG_MXC_PMIC_SC55112
#define MXC_DBG_CFG
//#include <mxc-oss-ctrls.h>        /* header for usefull oss and mc13783 ioctl plus mask parameters */
#endif

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
int VT_oss_sound_driver_test(char * file1, int source, int toread, int chan, int speed, int bits);
int VT_oss_sound_driver_setup(void);
int VT_oss_sound_driver_cleanup(void);

#ifdef __cplusplus
}
#endif

#endif        /* oss_sound_driver_TEST_H */
