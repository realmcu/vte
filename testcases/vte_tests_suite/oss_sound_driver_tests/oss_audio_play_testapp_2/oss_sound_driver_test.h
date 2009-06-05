/*================================================================================================*/
/**
        @file   oss_sound_driver_test.h

        @brief  Header file for OSS driver test scenario
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
I.Inkina/nknl001             11/01/2005     TLSbo61044  Clock master was added
A.Ozerov/B00320              17/02/2006     TLSbo62323  Index_ssi argument was deleted. Master mode
                                                        was removed. Checking if the audio file given 
                                                        in argument is stereo or mono was added.
D.Simakov                    07/12/2006     TLSbo76144  Updated with the new kernel                                                        
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

/*==================================================================================================
                                            CONSTANTS
==================================================================================================*/

/*==================================================================================================
                                        DEFINES AND MACROS
==================================================================================================*/
#if !defined(TRUE) && !defined(FALSE)
#define TRUE  1
#define FALSE 0
#endif

#define SNDCTL_DBMX_HW_SSI_CFG_W _SIOW('Z', 0, dbmx_cfg)
#define SNDCTL_DBMX_HW_SSI_CFG_R _SIOR('Z', 1, dbmx_cfg)

#define BUF_SIZE 4096

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
int VT_oss_sound_driver_test(char *playlist, int file_loop, int list_loop, int device);
int VT_oss_sound_driver_setup(void);
int VT_oss_sound_driver_cleanup(void);

#ifdef __cplusplus
}
#endif

#endif        /* OSS_SOUND_DRIVER_TEST_H */
