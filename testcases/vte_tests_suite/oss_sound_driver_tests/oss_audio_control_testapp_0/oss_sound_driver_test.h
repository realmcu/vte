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

        @brief  OSS audio control test header.
====================================================================================================
Revision History:
                            Modification     Tracking
Author/core ID                  Date          Number    Description of Changes
-------------------------   ------------    ----------  -------------------------------------------
RB657C/gsch1c                20/07/2004     TLSbo40898  Initial version  of OSS sound driver test development.
A.Ozerov/b00320              07/11/2005     TLSbo56870  Compilation flag for SC55112 and MC13783
                                                        platforms
D.Khoroshev/b00313           03/03/2006     TLSbo62323  Update according to the last MXC OSS specifications
D.Simakov                    13/06/2006     TLSbo67022  Exclude #include <mxc_audio_mc13783_ioctls.h>  
A.Ozerov/b00320              20/07/2006     TLSbo70792  Variable adder was added in the structure audio_settings.
D.Simakov                    07/12/2006     TLSbo76144  Updated with the new kernel
D.Simakov                    22/12/2005     TLSbo87096  Zeus compilation issue
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
#include <linux/autoconf.h>

#ifdef CONFIG_MXC_PMIC_SC55112
#define __KERNEL__
#include <mxc-alsa-pmic.h> /* header for usefull oss and mc13783 ioctl plus mask parameters */
#undef __KERNEL__
#endif

#ifdef CONFIG_MXC_MC13783_PMIC
#include <asm-arm/arch-mxc/audio_controls.h>
#endif

/*==================================================================================================
                                            CONSTANTS
==================================================================================================*/

/*==================================================================================================
                                        DEFINES AND MACROS
==================================================================================================*/
/* List all the test Id / Case */
#define STDAC                                0
#define CODEC                                1
#define ADDER                                2

#ifndef FALSE
#define FALSE                                0
#endif
#ifndef TRUE
#define TRUE                                 1
#endif

#define INCREMENT                            10

#define BUF_SIZE                             4096
#define DECODER_THREAD                       2        /* max number of decoding thread */

/*==================================================================================================
                                            ENUMS
==================================================================================================*/

/*==================================================================================================
                                STRUCTURES AND OTHER TYPEDEFS
==================================================================================================*/
/* All parameters for the test, usefull when using threads, should be the own thread datas */
typedef struct
{
        int     device;
        int     increment;
        char   *testfilename;   /* test file name */
        FILE   *fd_file;        /* test file pointer */
} test_parameters;

/* structure that groups all audio settings */
typedef struct
{
        unsigned int volume;
        unsigned int balance;
        unsigned int adder;
        unsigned int channels;
        unsigned int sample_rate;
        unsigned int format;
        int     active_output;
        int     active_input;
        int     codec_filter;
} audio_settings;

/*==================================================================================================
                                GLOBAL VARIABLE DECLARATIONS
==================================================================================================*/

/*==================================================================================================
                                    FUNCTION PROTOTYPES
==================================================================================================*/
int     VT_oss_sound_driver_setup(void);
int     VT_oss_sound_driver_cleanup(void);
int     VT_oss_sound_driver_test(int Device, int Increment, char *File);
int     ask_user(char *question);

#ifdef __cplusplus
} 
#endif

#endif        /* OSS_SOUND_DRIVER_TEST_H */
