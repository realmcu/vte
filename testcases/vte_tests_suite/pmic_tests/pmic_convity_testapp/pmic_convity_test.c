/*================================================================================================*/
/**
        @file   pmic_convity_test.c

        @brief  Source file for PMIC Connectivity driver test.
*/
/*==================================================================================================

        Copyright (C) 2006, Freescale Semiconductor, Inc. All Rights Reserved
        THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
        BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
        Freescale Semiconductor, Inc.

====================================================================================================
Revision History:
                            Modification     Tracking
Author/core ID                  Date          Number     Description of Changes
-------------------------   ------------    ----------   -------------------------------------------
V.Halabuda/hlbv001           16/08/2005     TLSbo52696   Initial version
V.Halabuda/hlbv001           02/09/2005     TLSbo58397   Update for linux-2.6.10-rel-L26_1_14
A.Ozerov/b00320              15/05/2006     TLSbo64237   Code was cast in accordance to coding conventions.
A.Ozerov/b00320              10/08/2006     TLSbo74269   Test MC13783 connectivity driver was added.
Rakesh S Joshi/r65956        24/07/2007     ENGR00039319 RS-232 and CEA936 operating configuration updated.

====================================================================================================
Portability: ARM GCC
==================================================================================================*/

/*==================================================================================================
                                        INCLUDE FILES
==================================================================================================*/
/* Verification Test Environment Include Files */
#include "pmic_convity_test.h"

/*================================================================================================*/
extern char *TCID;

extern PMIC_CONVITY_TEST_IOCTL convity_testcase;
extern PMIC_CONVITY_MODE argument;

int fd = 0;

static pmic_covity_test_param convity_config;

static char *IOCTLS[]=
{
        "PMIC_CONVITY_TEST_MODE",
        "PMIC_CONVITY_TEST_RESET",
        "PMIC_CONVITY_TEST_CALLBACK",
        "PMIC_CONVITY_TEST_USB_SPEED",
        "PMIC_CONVITY_TEST_USB_POWER",
        "PMIC_CONVITY_TEST_USB_TRANSCEIVER_MODE",
        "PMIC_CONVITY_TEST_USB_OTG_DLP_DURATION",
        "PMIC_CONVITY_TEST_USB_OTG_CONFIG",
        "PMIC_CONVITY_TEST_RS232",
        "PMIC_CONVITY_TEST_CEA936"
        "PMIC_CONVITY_TEST_OPEN",
        "PMIC_CONVITY_TEST_CLOSE",
};

/*==================================================================================================
                                        LOCAL FUNCTIONS
==================================================================================================*/
/*================================================================================================*/
/*===== VT_pmic_convity_test_setup =====*/
/**
@brief  assumes the pre-condition of the test case execution

@param  None

@return On success - return TPASS
        On failure - return TFAIL
*/
/*================================================================================================*/
int VT_pmic_convity_test_setup(void)
{
        int VT_rv = TPASS;
        char f_name[256]="/dev/";

        strcat(f_name, PMIC_CONVITY_DEV);

        if((fd = open(f_name, O_RDWR)) < 0)
        {
                VT_rv = TFAIL;
                tst_resm(VT_rv, "VT_pmic_convity_test_setup() Failed open device");
        		VT_rv = VT_pmic_convity_test_cleanup();
		        if(VT_rv != TPASS)
        		{
                	tst_resm(TFAIL, "VT_cleanup() Failed : error code = %d", VT_rv);
        		}
        }
        return VT_rv;
}

/*================================================================================================*/
/*===== VT_pmic_convity_test_cleanup =====*/
/**
@brief  assumes the post-condition of the test case execution

@param  None

@return On success - return TPASS
        On failure - return the error code
*/
/*================================================================================================*/
int VT_pmic_convity_test_cleanup(void)
{
        if(fd > 0)
        {
                close(fd);
                return TPASS;
        }
        else return TFAIL;
}

/*================================================================================================*/
/*===== VT_pmic_convity_test =====*/
/**
@brief  PMIC test scenario connectivity function

@param  None

@return On success - return TPASS
        On failure - return TFAIL
*/
/*================================================================================================*/
int VT_pmic_convity_test(void)
{
        int status = 0;
        int VT_rv = TPASS;

        PMIC_CONVITY_HANDLE handle = 0;
        PMIC_CONVITY_MODE mode;

        mode = argument;
        convity_config.mode = mode;
        convity_config.handle = handle;

        /*if(convity_testcase >= PMIC_CONVITY_TEST_OPEN )*/
        if(convity_testcase >= 14 )
        {
            VT_rv = TFAIL;
            tst_resm(VT_rv, "\n\nInvalid Option\n\n");
            return VT_rv;
        }else
		{
			if (convity_testcase == 9 ){
				convity_testcase =8;
			}else
				if ( convity_testcase == 10 || convity_testcase == 11 || convity_testcase == 12 || convity_testcase == 13 )
        		{
					convity_testcase=9;
				}
            if((status = ioctl(fd, PMIC_CONVITY_TEST_OPEN, &convity_config)) != PMIC_SUCCESS)
            {
                    VT_rv = TFAIL;
                    tst_resm(VT_rv, "Failed opened PMIC Connectivity device, ERROR CODE is \"%d\"", status);
                    return VT_rv;
            }
            tst_resm(TINFO, "Connectivity operation mode - %s", IOCTLS[convity_testcase]);
            if((status = ioctl(fd, convity_testcase, &convity_config)) != PMIC_SUCCESS)
            {
                    VT_rv = TFAIL;
                    tst_resm(VT_rv, "Failed connectivity operation mode, ERROR CODE is \"%d\"", status);
                    return VT_rv;
            }
            if(convity_testcase != PMIC_CONVITY_TEST_RESET )
            {
                if((status = ioctl(fd, PMIC_CONVITY_TEST_CLOSE, &convity_config)) != PMIC_SUCCESS)
                {
                    VT_rv = TFAIL;
                    tst_resm(VT_rv, "Failed closed PMIC Connectivity device, ERROR CODE is \"%d\"", status);
                    return VT_rv;
                }
            }
        }

        return VT_rv;
}
