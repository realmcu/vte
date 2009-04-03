/*================================================================================================*/
/**
        @file   oss_sound_driver_test.h

        @brief  OSS audio play testapp header file.
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
D.Khoroshev/B00313           02/08/2006     TLSbo62323  Updates according new specifications
D.Khoroshev/B00313           02/15/2006     TLSbo62323  Updates accoring to the last SSI specifications
D.Simakov                    07/12/2006     TLSbo76144  Updated with the new kernel
================================================================================================= */
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
#include <asm-arm/arch-mxc/audio_controls.h>

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
unsigned short get_audio_channels(FILE* file);

int VT_oss_sound_driver_test(int device, int loop, char *file);
int VT_oss_sound_driver_setup();
int VT_oss_sound_driver_cleanup();

#ifdef __cplusplus
}
#endif

#endif        /* oss_sound_driver_TEST_H */
