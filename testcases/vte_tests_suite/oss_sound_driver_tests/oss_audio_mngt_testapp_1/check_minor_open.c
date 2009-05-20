/*====================*/
/**
        @file   check_minor_open.c

        @brief  OSS audio simple open test.
*/
/*======================

        Copyright (C) 2006, Freescale Semiconductor, Inc. All Rights Reserved
        THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
        BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
        Freescale Semiconductor, Inc.

====================
Revision History:
                            Modification     Tracking
Author/core ID                  Date          Number    Description of Changes
-------------------------   ------------    ----------  -------------------------------------------
RB657C/gsch1c                20/07/2004     TLSbo40898  Initial version  of OSS sound driver test development
D.Khoroshev/b00313           02/23/2006     TLSbo61805  Update according new SSI specifications
D.Simakov                    19/10/2006     TLSbo76144  dsp->adsp, dsp1->dsp
D.Simakov                    07/12/2006     TLSbo76144  Updated with the new kernel
====================
Portability: ARM GCC

======================*/

/*======================
                                        INCLUDE FILES
======================*/
/* Standard Include Files */
#include <errno.h>

/* Harness Specific Include Files. */
#include <test.h>

/* Verification Test Environment Include Files */
#include "oss_sound_driver_test.h"
#include "../common.h"

/*======================
                                       LOCAL FUNCTIONS
======================*/
#define MAX_INSTANCES 4

/*====================*/
/*= VT_oss_sound_driver_setup =*/
/**
@brief  assumes the pre-condition of the test case execution

@param  None
@return On success - return TPASS              On failure - return the error code
*/
/*====================*/
int VT_oss_sound_driver_setup(void)
{
        return TPASS;
}

/*====================*/
/*= VT_oss_sound_driver_cleanup =*/
/**
@brief  assumes the post-condition of the test case execution

@param  None
@return On success - return TPASS              On failure - return the error code
*/
/*====================*/
int VT_oss_sound_driver_cleanup(void)
{
        return TPASS;
}

/*====================*/
/*= VT_oss_sound_driver_cleanup =*/
/**
@brief  Test program

@param  None
@return On success - return TPASS              On failure - return the error code
*/
/*====================*/
int VT_oss_sound_driver_test(int supported_inst)
{
        int VT_rv = TFAIL;
        int fd_audio[MAX_INSTANCES], i, j, tmp, err, toterr = 0;
        char drv_name[32];
        int open_flag;

        for ( i=0; i<MAX_INSTANCES; i++ )
                fd_audio[i] = -1;

        for ( i=0; i<=supported_inst; i++ )
        {
                if ( i == 0 )
                {
                        sprintf(drv_name,"/dev/sound/adsp");
                        open_flag = O_RDWR;
                }
                else
                {
                        sprintf(drv_name,"/dev/sound/dsp");
                        open_flag = O_WRONLY;
                }

                for ( j=0, err=0; j<3; j++ )
                {
                        if ((tmp = open (drv_name, open_flag)) < 0)
                        {
                                if ( i == supported_inst ) goto _end_loop;
                                /** did not succeeded to open the driver */
                                if ( j == 0 )
                                {
                                        tst_resm(TFAIL, "Error at first opening %s\n", drv_name);
                                        err++;
                                        break; // exit for this instance
                                }
                        }
                        else
                        {
                                if ( j > 0 )
                                {
                                        tst_resm(TFAIL, "Error : driver re-opened\n");
                                        close(tmp);
                                        err++;
                                        break; // exit for this instance
                                }

                                /** succeeded to open the driver */
                                else fd_audio[i] = tmp;
                        }
                }

                if ( !err ) tst_resm(TINFO, "Test passed successfully for %s\n", drv_name);

_end_loop:     toterr+=err;
        }

        for ( i=0; i<supported_inst; i++ )
        {
                if ( fd_audio[i] >= 0 ) close(fd_audio[i]); /* close the only opened instance */
        }

        if ( !toterr ) VT_rv = TPASS;

        return VT_rv;
}
