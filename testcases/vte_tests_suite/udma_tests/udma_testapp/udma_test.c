/***
**Copyright 2006-2009 Freescale Semiconductor, Inc. All Rights Reserved.
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
        @file   udma_test.c

        @brief  Source file for Unified DMA driver test
====================================================================================================
Revision History:
                            Modification     Tracking
Author/core ID                  Date          Number    Description of Changes
-------------------------   ------------    ----------  -------------------------------------------
A.Ozerov/b00320              03/10/2006     TLSbo78550  Initial version.
A.Ozerov/b00320              01/11/2006     TLSbo81158  UDMA module was fixed for working with all platforms.
A.Ozerov/b00320              05/02/2007     TLSbo87473  One of testcases was removed.

====================================================================================================
Portability: ARM GCC
==================================================================================================*/

/*==================================================================================================
                                        INCLUDE FILES
==================================================================================================*/
/* Verification Test Environment Include Files */
#include "udma_test.h"

/*==================================================================================================
                                        GLOBAL VARIABLES 
===================================================================================================*/
int     fd = 0;

/*==================================================================================================
                                        LOCAL FUNCTIONS
==================================================================================================*/

/*================================================================================================*/
/*===== VT_udma_test_setup =====*/
/**
@brief  This function assumes the pre-condition of the test case execution
 
@param  None.
 
@return On success - return TPASS.
        On failure - return the error code.
*/
/*================================================================================================*/
int VT_udma_test_setup(void)
{
        int     rv = TFAIL;

        fd = open("/dev/" UDMA_NAME, O_RDWR);
        if (fd < 0)
        {
                tst_resm(TFAIL, "VT_udma_test_setup() Failed open device");
                return TFAIL;
        }
        rv = TPASS;

        return rv;
}

/*================================================================================================*/
/*===== VT_udma_test_cleanup =====*/
/** 
@brief  This function assumes the post-condition of the test case execution
 
@param  None.
 
@return None.
*/
/*================================================================================================*/
int VT_udma_test_cleanup(void)
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
/*===== VT_udma_test =====*/
/**
@brief  Unified DMA driver test scenario
 
@param  test_num
        Number test case.
 
@return On success - return TPASS.
        On failure - return the error code. 
*/
/*================================================================================================*/
int VT_udma_test(int test_num, int t_tranfer)
{
        int ret = TPASS;

        tst_resm(TINFO, "Testing udma...");
        switch(test_num)
        {
#ifdef CONFIG_OTHER_PLATFORM
        case 0:
                tst_resm(TINFO, "Config setting...");
                CALL_IOCTL(ioctl(fd, UDMA_IOC_SET_CONFIG, t_tranfer));
                break;
        case 1:
                tst_resm(TINFO, "Callback setting...");
                CALL_IOCTL(ioctl(fd, UDMA_IOC_SET_CALLBACK, NULL));
                break;
        case 2:
                tst_resm(TINFO, "Data transfer testing...");
                CALL_IOCTL(ioctl(fd, UDMA_IOC_DATA_TRANSFER, NULL));
                break;
#endif
#ifdef CONFIG_IMX27
        case 0:
                tst_resm(TINFO, "1d-1d memory testing...");
                CALL_IOCTL(ioctl(fd, UDMA_IOC_RAM2RAM, NULL));
                break;
        case 1:
                tst_resm(TINFO, "2d-2d memory testing...");
                CALL_IOCTL(ioctl(fd, UDMA_IOC_RAM2D2RAM2D, NULL));
                break;
        case 2:
                tst_resm(TINFO, "1d-2d memory testing...");
                CALL_IOCTL(ioctl(fd, UDMA_IOC_RAM2RAM2D, NULL));
                break;
        case 3:
                tst_resm(TINFO, "2d-1d memory testing...");
                CALL_IOCTL(ioctl(fd, UDMA_IOC_RAM2D2RAM, NULL));
                break;
        case 4:
                tst_resm(TINFO, "Chain of buffers testing(using HW channel)...");
                CALL_IOCTL(ioctl(fd, UDMA_IOC_HW_CHAINBUFFER, t_tranfer));
                break;
        case 5:
                tst_resm(TINFO, "Chain of buffers testing(using SW channel)...");
                CALL_IOCTL(ioctl(fd, UDMA_IOC_HW_CHAINBUFFER, t_tranfer));
                break;
#endif
        default:
                tst_resm(TWARN, "Wrong test case!");
        }

        return ret;
}
