/*====================*/
/**
        @file   sdma_test.c

        @brief  SDMA API test
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
S.ZAVJALOV/----              13/07/2004     TLSbo40259  Initial version
S.ZAVJALOV/ZVJS001           20/09/2004     TLSbo42212  Fixed return error code
I.Inkina/nknl001             02/08/2005     TLsbo49843  warnings was fixed
A.Ozerov/B00320              10/02/2006     TLSbo61734  Code was cast to coding conventions
A.Ozerov/b00320              11/12/2006     TLSbo84161  Minor changes.

====================
Portability:  ARM GCC
======================*/

/*======================
                                        INCLUDE FILES
======================*/
/* Standard Include Files */
#include <errno.h>

/* Verification Test Environment Include Files */
#include "sdma_test.h"

/*======================
                                        GLOBAL VARIABLES
======================*/
extern char *TCID;
extern int test_num;
extern int channel;
extern int argument;

int     fd = 0;
test_param *arg = 0;

/*====================*/
/*= VT_sdma_test =*/
/**
@brief  SDMA Driver test scenario

@param  none

@return On success - return TPASS
        On failure - return the error code
*/
/*====================*/
int VT_sdma_test(void)
{
        int     VT_rv = TFAIL,
            err;

        arg->channel = channel;
        arg->param = argument;

        if (test_num == MXC_SDMA_TEST_4)
        {
                err = ioctl(fd, MXC_SDMA_TEST_1, arg);
                if ((err < 0) && (errno == EBUSY))
                        VT_rv = TPASS;
        }
        else
        {
                err = ioctl(fd, test_num, arg);
                if (err == 0)
                        VT_rv = TPASS;
        }


        return VT_rv;
}

/*====================*/
/*= VT_sdma_test_setup =*/
/**
@brief  This function assumes the pre-condition of the test case execution

@param  none

@return On success - return TPASS
        On failure - return TFAIL
*/
/*====================*/
int VT_sdma_test_setup(void)
{
        int     VT_rv = TFAIL;

        if ((fd = open(SDMA_TEST_DEVICE, O_RDWR)) < 0)
        {
                tst_brkm(TBROK, (void *) VT_sdma_test_cleanup,
                         "VT_sdma_test_setup() Failed open device");
        }

        if (!(arg = malloc(sizeof(test_param))))
        {
                tst_brkm(TBROK, (void *) VT_sdma_test_cleanup,
                         "VT_sdma_test_setup () Failed allocate memory");
        }

        VT_rv = TPASS;
        return VT_rv;
}

/*====================*/
/*= VT_sdma_test_cleanup =*/
/**
@brief  This function assumes the post-condition of the test case execution

@param  none

@return On success - return TPASS
        On failure - return TFAIL
*/
/*====================*/
int VT_sdma_test_cleanup(void)
{
        if (fd > 0)
                close(fd);
        if (arg != 0)
                free(arg);

        return TPASS;
}
