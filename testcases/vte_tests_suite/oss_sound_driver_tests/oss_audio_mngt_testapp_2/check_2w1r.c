/*================================================================================================*/
/**
        @file   check_2w1r.c

        @brief  OSS access management test.
*/
/*==================================================================================================

        Copyright (C) 2006, Freescale Semiconductor, Inc. All Rights Reserved
        THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
        BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
        Freescale Semiconductor, Inc.

====================================================================================================
Revision History:
                            Modification     Tracking
Author                          Date          Number    Description of Changes
-------------------------   ------------    ----------  -------------------------------------------
RB657C /gsch1c               07/20/2004     TLSbo40898  Initial version  of OSS sound driver test
                                                        development
D.Khoroshev/b00313           01/31/2006     TLSbo61785  Reworked version
D.Simakov                    19/10/2006     TLSbo76144  dsp->adsp, dsp1->dsp
D.Khoroshev/b00313           11/13/2006     TLSbo81934  Opening quotas were changed

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
#include "oss_sound_driver_test.h"

/*==================================================================================================
                                       LOCAL MACROS
==================================================================================================*/
#define MAX_INSTANCES 2

/*==================================================================================================
                                       LOCAL FUNCTIONS
==================================================================================================*/

/*================================================================================================*/
/*===== VT_oss_sound_driver_test_device =====*/
/**
@brief  This functions tries to open device 2 times in different modes(O_RDWR, O_WRONLY, O_RDONLY) 
        Result is stored in second parameter.        
        
@param  Input:  test_id - test identifier
        Output: results - state of attempts to open device.
                            
@return On success - return TPASS
        On failure - return the error code
*/
/*================================================================================================*/
static void VT_oss_sound_driver_test_device(char *device, int results[9][2])
{
        int fd_audio[MAX_INSTANCES], i, j, k;
        int mode[3] = { O_RDWR|O_NONBLOCK, O_WRONLY|O_NONBLOCK, O_RDONLY|O_NONBLOCK };

        for ( i=0; i<MAX_INSTANCES; i++ )
                fd_audio[i] = -1;

        for ( i=0; i<9; i++ ) 
        {
                results[i][0]=0;
                results[i][1]=0;
        }

        for(i=0; i<3; i++) 
        {
                for(j=0; j<3; j++)
                {
                        if ((fd_audio[0] = open (device, mode[i])) < 0)
                        {
                                results[i*3+j][0]=fd_audio[0];
                        }

                        if ((fd_audio[1] = open (device, mode[j])) < 0)
                        {
                                results[i*3+j][1]=fd_audio[1];
                        }

                        for ( k=0; k<MAX_INSTANCES; k++ )
                                if ( fd_audio[k] >= 0 )
                                {
                                        close(fd_audio[k]);
                                        fd_audio[k] = -1;
                                }
                }
        }

}

/*==================================================================================================
                                       GLOBAL FUNCTIONS
==================================================================================================*/

/*================================================================================================*/
/*===== VT_oss_sound_driver_setup =====*/
/**
@brief  assumes the pre-condition of the test case execution

@param  None        
@return On success - return TPASS              On failure - return the error code
*/
/*================================================================================================*/
int VT_oss_sound_driver_setup(void)
{
        return TPASS;
}

/*================================================================================================*/
/*===== VT_oss_sound_driver_cleanup =====*/
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
/*===== VT_oss_sound_driver_test =====*/
/**
@brief  This functions checks access politics for Stereo DAC, Voice CODEC and Mixer devices.

@param  test_id - test identifier        
@return On success - return TPASS
        On failure - return the error code
*/
/*================================================================================================*/
int VT_oss_sound_driver_test(int test_id)
{
        int VT_rv = TPASS;
        int results[9][2], i;
        char *op_name[3] = { "O_RDWR", "O_WRONLY", "O_RDONLY" };

        switch(test_id)
        {
                case 0:
                        /*        Voice CODEC        */
                        VT_oss_sound_driver_test_device(STDAC_DEV, results);
                        for(i=0; i<9; i++)
                        {
                                if(results[i][0] < 0)
                                {
                                        tst_resm(TINFO, "Voice %s wasn't opened once in %s mode", STDAC_DEV, op_name[i/3]);
                                        VT_rv=TFAIL;
                                }
                                else if(!results[i][1] && i != 5 && i != 7) /* O_WRONLY - O_RDONLY and vice versa */
                                {
                                        tst_resm(TINFO, "%s was opened second time.  [%s - %s]", STDAC_DEV, op_name[i/3], op_name[i%3]);
                                        VT_rv=TFAIL;
                                }
                        }
                        break;
                case 1:
                        /*        Stereo DAC        */
                        VT_oss_sound_driver_test_device(CODEC_DEV, results);
                        for(i=0; i<9; i++)
                        {

                                if(!results[i][0])        
                                {        
                                        if(i > 5)
                                        {
                                                tst_resm(TINFO, "%s was opened in %s mode", CODEC_DEV, op_name[i/3]);
                                                VT_rv=TFAIL;
                                        } 
                                }
                                else
                                {
                                        if(i >= 3 && i <= 5)
                                        {
                                                tst_resm(TINFO, "%s wasn't opened in %s mode", CODEC_DEV, op_name[i/3]);
                                                VT_rv=TFAIL;
                                        }
                                }

                                if(!results[i][1])
                                {
                                        if(i % 3 != 1 && (i != 6 && i != 7)) 
                                        /* O_RDONLY - O_RDWR,
                                        O_RDONLY - O_WRONLY 
                                        O_RDWR is converted to O_WRONLY when opening in half duplex mode
                                        */
                                        {
                                                tst_resm(TINFO, "%s was opened in %s mode[%s - %s]", CODEC_DEV, op_name[i%3], op_name[i/3], op_name[i%3]);
                                                VT_rv=TFAIL;
                                        }
                                }
                                else
                                {
                                        if(i % 3 == 1 && results[i][0] < 0)
                                        {
                                                tst_resm(TINFO, "Stereo DAC wasn't opened in %s mode[%s - %s]", op_name[i%3], op_name[i/3], op_name[i%3]);

                                                VT_rv=TFAIL;
                                        }
                                }
                                
                                if(!results[i][0] && !results[i][1])
                                {
                                        tst_resm(TINFO, "Stereo DAC was opened second time.  [%s - %s]", op_name[i/3], op_name[i%3]);
                                               VT_rv=TFAIL; 
                                }
                        }
                        break;
                case 2:
                        /*        Mixer        */
                        VT_oss_sound_driver_test_device(MIXER_DEV, results);
                        for(i=0; i<9; i++)
                        {
                                if(results[i][0] < 0)
                                {
                                        tst_resm(TINFO, "Mixer wasn't opened in %s mode", op_name[i/3]);
                                        VT_rv=TFAIL; 
                                }
                                if(results[i][1] <0)
                                {
                                        tst_resm(TINFO, "Mixer wasn't opened second time. [%s - %s]", op_name[i/3], op_name[i%3]);
                                        VT_rv=TFAIL;
                                }
                        }                        
        }

        return VT_rv;
}
