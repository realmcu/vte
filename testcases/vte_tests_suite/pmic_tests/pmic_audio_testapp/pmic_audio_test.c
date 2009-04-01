/*================================================================================================*/
/**
        @file   pmic_audio_test.c

        @brief  Source file for PMIC audio driver test
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
S.Bezrukov/SBAZR1C           31/08/2005     TLSbo52697  Initial version
A.Ozerov/b00320              08/08/2006     TLSbo73745  Review version(in accordance to L26_1_19 release).

====================================================================================================
Portability: ARM GCC
==================================================================================================*/

/*==================================================================================================
                                        INCLUDE FILES
==================================================================================================*/
/* Verification Test Environment Include Files */
#include "pmic_audio_test.h"

/*==================================================================================================
                                        GLOBAL VARIABLES
===================================================================================================*/
int     fd = 0;

/*==================================================================================================
                                        LOCAL FUNCTIONS
==================================================================================================*/
/*================================================================================================*/
/*===== VT_pmic_audio_test_setup =====*/
/**
@brief  This function assumes the pre-condition of the test case execution

@param  None.

@return On success - return PMIC_SUCCESS
        On failure - return the error code.
*/
/*================================================================================================*/
int VT_pmic_audio_test_setup(void)
{
        int     rv = TFAIL;

        fd = open("/dev/" PMIC_AUDIO_DEV, O_RDWR);
        if (fd < 0)
        {
                tst_resm(TFAIL, "VT_pmic_audio_test_setup() Failed open device");
                return TFAIL;
        }
        rv = TPASS;

        return rv;
}

/*================================================================================================*/
/*===== VT_pmic_audio_test_cleanup =====*/
/**
@brief  This function assumes the post-condition of the test case execution

@param  None.

@return None.
*/
/*================================================================================================*/
int VT_pmic_audio_test_cleanup(void)
{
        int     ret;

        ret = close(fd);

        if (ret < 0)
        {
                tst_resm(TINFO, "Unable to close file descriptor %d", fd);
                return TFAIL;
        }

        return TPASS;
}

/*================================================================================================*/
/*===== VT_pmic_audio_test =====*/
/**
@brief  PMIC Battery test scenario

@param  switch_fct
        Number test case.

@return On success - return PMIC_SUCCESS
        On failure - return the error code.
*/
/*================================================================================================*/
int VT_pmic_audio_test(int test_num)
{
#if defined( CONFIG_MXC_PMIC_SC55112 ) || defined( CONFIG_MXC_PMIC_MC13783 )
        switch (test_num)
        {
        case 0:
                tst_resm(TINFO, "Test case %d: OUTPUT", test_num);

                if (ioctl(fd, PMIC_AUDIO_TEST_OUTPUT, NULL) != 0)
                {
                        tst_resm(TFAIL, "Error in ioctl(PMIC_AUDIO_TEST_OUTPUT)");
                        return TFAIL;
                }
                break;

        case 1:
                tst_resm(TINFO, "Test case %d: INPUT", test_num);

                if ((ioctl(fd, PMIC_AUDIO_TEST_INPUT, NULL)) != 0)
                {
                        tst_resm(TFAIL, "Error in ioctl(PMIC_AUDIO_TEST_INPUT)");
                        return TFAIL;
                }
                break;

        case 2:
                tst_resm(TINFO, "Test case %d: SDAC", test_num);

                if (ioctl(fd, PMIC_AUDIO_TEST_SDAC, NULL) != 0)
                {
                        tst_resm(TFAIL, "Error in ioctl(PMIC_AUDIO_TEST_SDAC)");
                        return TFAIL;
                }

                break;

        case 3:
                tst_resm(TINFO, "Test case %d: CODEC", test_num);

                if (ioctl(fd, PMIC_AUDIO_TEST_CODEC, NULL) != 0)
                {
                        tst_resm(TFAIL, "Error in ioctl(PMIC_AUDIO_TEST_CODEC)");
                        return TFAIL;
                }
                break;

        case 4:
                tst_resm(TINFO, "Test case %d: BUS", test_num);

                if (ioctl(fd, PMIC_AUDIO_TEST_BUS, NULL) != 0)
                {
                        tst_resm(TFAIL, "Error in ioctl(PMIC_AUDIO_TEST_BUS)");
                        return TFAIL;
                }

                break;

        default:
                tst_resm(TFAIL, "Error: This test case has been broken");
                return TFAIL;
        }
#endif

#ifdef CONFIG_MXC_MC13783_LEGACY
        int     ret;

        switch (test_num)
        {
        case 0:
                tst_resm(TINFO, "Testing all MC13783 audio...");

                ret = ioctl(fd, INIT_AUDIO, NULL);
                if (ret != 0)
                {
                        tst_resm(TFAIL, "Initialization error! Error: %s", strerror(errno));
                        return TFAIL;
                }
                tst_resm(TINFO, "Initialization was performed successfully...");

                ret = ioctl(fd, CHECK_VOLUME, NULL);
                if (ret != 0)
                {
                        tst_resm(TFAIL, "Error with checking volume! Error: %s", strerror(errno));
                        return TFAIL;
                }
                tst_resm(TINFO, "Checking volume was performed successfully...");

                ret = ioctl(fd, CHECK_OUT, NULL);
                if (ret != 0)
                {
                        tst_resm(TFAIL, "Error with checking output! Error: %s", strerror(errno));
                        return TFAIL;
                }
                tst_resm(TINFO, "Checking output was performed successfully...");

                ret = ioctl(fd, CHECK_IN, NULL);
                if (ret != 0)
                {
                        tst_resm(TFAIL, "Error with checking input! Error: %s", strerror(errno));
                        return TFAIL;
                }
                tst_resm(TINFO, "Checking input was performed successfully...");

                ret = ioctl(fd, CHECK_STDAC, NULL);
                if (ret != 0)
                {
                        tst_resm(TFAIL, "Error with checking STDAC device! Error: %s",
                                 strerror(errno));
                        return TFAIL;
                }
                tst_resm(TINFO, "Checking STDAC device was performed successfully...");

                ret = ioctl(fd, CHECK_CODEC, NULL);
                if (ret != 0)
                {
                        tst_resm(TFAIL, "Error with checking VOICE CODEC device! Error: %s",
                                 strerror(errno));
                        return TFAIL;
                }
                tst_resm(TINFO, "Checking VOICE CODEC device was performed successfully...");

                ret = ioctl(fd, INIT_AUDIO, NULL);
                if (ret != 0)
                {
                        tst_resm(TFAIL, "Re-initialization error! Error: %s", strerror(errno));
                        return TFAIL;
                }
                tst_resm(TINFO, "Re-initialization was performed successfully...");
                break;

        case 1:
                tst_resm(TINFO, "Testing output audio functions of MC13783...");

                ret = ioctl(fd, INIT_AUDIO, NULL);
                if (ret != 0)
                {
                        tst_resm(TFAIL, "Initialization error! Error: %s", strerror(errno));
                        return TFAIL;
                }
                tst_resm(TINFO, "Initialization was performed successfully...");

                ret = ioctl(fd, CHECK_VOLUME, NULL);
                if (ret != 0)
                {
                        tst_resm(TFAIL, "Error with checking volume! Error: %s", strerror(errno));
                        return TFAIL;
                }
                tst_resm(TINFO, "Checking volume was performed successfully...");

                ret = ioctl(fd, CHECK_OUT, NULL);
                if (ret != 0)
                {
                        tst_resm(TFAIL, "Error with checking output! Error: %s", strerror(errno));
                        return TFAIL;
                }
                tst_resm(TINFO, "Checking output was performed successfully...");

                ret = ioctl(fd, INIT_AUDIO, NULL);
                if (ret != 0)
                {
                        tst_resm(TFAIL, "Re-initialization error! Error: %s", strerror(errno));
                        return TFAIL;
                }
                tst_resm(TINFO, "Re-initialization was performed successfully...");
                break;

        case 2:
                tst_resm(TINFO, "Testing input audio functions of MC13783...");

                ret = ioctl(fd, INIT_AUDIO, NULL);
                if (ret != 0)
                {
                        tst_resm(TFAIL, "Initialization error! Error: %s", strerror(errno));
                        return TFAIL;
                }
                tst_resm(TINFO, "Initialization was performed successfully...");

                ret = ioctl(fd, CHECK_IN, NULL);
                if (ret != 0)
                {
                        tst_resm(TFAIL, "Error with checking input! Error: %s", strerror(errno));
                        return TFAIL;
                }
                tst_resm(TINFO, "Checking input was performed successfully...");

                ret = ioctl(fd, INIT_AUDIO, NULL);
                if (ret != 0)
                {
                        tst_resm(TFAIL, "Re-initialization error! Error: %s", strerror(errno));
                        return TFAIL;
                }
                tst_resm(TINFO, "Re-initialization was performed successfully...");
                break;

        case 3:
                tst_resm(TINFO, "Testing STDAC audio functions of MC13783...");

                ret = ioctl(fd, INIT_AUDIO, NULL);
                if (ret != 0)
                {
                        tst_resm(TFAIL, "Initialization error! Error: %s", strerror(errno));
                        return TFAIL;
                }
                tst_resm(TINFO, "Initialization was performed successfully...");

                ret = ioctl(fd, CHECK_STDAC, NULL);
                if (ret != 0)
                {
                        tst_resm(TFAIL, "Error with checking STDAC device! Error: %s",
                                 strerror(errno));
                        return TFAIL;
                }
                tst_resm(TINFO, "Checking STDAC device was performed successfully...");

                ret = ioctl(fd, INIT_AUDIO, NULL);
                if (ret != 0)
                {
                        tst_resm(TFAIL, "Re-initialization error! Error: %s", strerror(errno));
                        return TFAIL;
                }
                tst_resm(TINFO, "Re-initialization was performed successfully...");
                break;

        case 4:
                tst_resm(TINFO, "Testing VOICE CODEC audio functions of MC13783...");

                ret = ioctl(fd, INIT_AUDIO, NULL);
                if (ret != 0)
                {
                        tst_resm(TFAIL, "Initialization error! Error: %s", strerror(errno));
                        return TFAIL;
                }
                tst_resm(TINFO, "Initialization was performed successfully...");

                ret = ioctl(fd, CHECK_CODEC, NULL);
                if (ret != 0)
                {
                        tst_resm(TFAIL, "Error with checking VOICE CODEC device! Error: %s",
                                 strerror(errno));
                        return TFAIL;
                }
                tst_resm(TINFO, "Checking VOICE CODEC device was performed successfully...");

                ret = ioctl(fd, INIT_AUDIO, NULL);
                if (ret != 0)
                {
                        tst_resm(TFAIL, "Re-initialization error! Error: %s", strerror(errno));
                        return TFAIL;
                }
                tst_resm(TINFO, "Re-initialization was performed successfully...");
                break;
        }
#endif
        return TPASS;
}
