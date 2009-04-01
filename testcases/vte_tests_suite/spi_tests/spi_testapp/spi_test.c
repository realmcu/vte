/*================================================================================================*/
/**
        @file   spi_test.c

        @brief  SPI test scenario
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
S.Artyom/ID                  DD/MM/YYYY     TLSbo51450  BRIEF description of changes made 
I.Semenchukov/smng001c       14/07/2005     TLSbo52341  Fix bug that causes makes test to stuck
S.Bezrukov                   22/09/2005     TLSbo51450  Changed message in LTP function  
V.Khalabuda/b00306           17/04/2006     TLSbo67010  Update version for linux-2.6.10-rel-L26_1_17
D.Kazachkov/b00316           30/08/2006     TLSbo67010  Update version for linux-2.6.10-rel-L26_1_18
D.Khoroshev/b00313           02/01/2006     TLSbo86657  Adaptation to new spi interface

====================================================================================================
Portability: ARM GCC

==================================================================================================*/

/*==================================================================================================
                                        INCLUDE FILES
==================================================================================================*/
/* Verification Test Environment Include Files */
#include "spi_test.h"

/*==================================================================================================
                                        LOCAL MACROS
==================================================================================================*/

/*==================================================================================================
                                        LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
==================================================================================================*/

/*==================================================================================================
                                        LOCAL CONSTANTS
==================================================================================================*/

/*==================================================================================================
                                        LOCAL VARIABLES
==================================================================================================*/
static int spi_fd = -1;

/*==================================================================================================
                                        GLOBAL CONSTANTS
==================================================================================================*/

/*==================================================================================================
                                        GLOBAL VARIABLES
==================================================================================================*/
extern char device_filename[128];

/*==================================================================================================
                                        LOCAL FUNCTION PROTOTYPES
==================================================================================================*/

/*==================================================================================================
                                        LOCAL FUNCTIONS
==================================================================================================*/

/*================================================================================================*/
/*===== VT_camera_setup =====*/
/**
@brief  assumes the pre-condition of the test case execution

@param  None
    
@return On success - return TPASS
        On failure - return the error code
*/
/*================================================================================================*/
int VT_spi_setup(void)
{
        int     rv = TFAIL;

        spi_fd = open(device_filename, O_RDONLY);

        if (spi_fd == -1)
        {
                tst_resm(TBROK, "Can't open spi test device file\n");
                tst_resm(TBROK, device_filename);
                return rv;
        }

        rv = TPASS;
        return rv;
}

/*================================================================================================*/
/*===== VT_camera_cleanup =====*/
/**
@brief  assumes the post-condition of the test case execution

@param  None
    
@return On success - return TPASS
                On failure - return the error code
*/
/*================================================================================================*/
int VT_spi_cleanup(void)
{
        int     rv = TFAIL;

        if (spi_fd > 0)
                close(spi_fd);

        rv = TPASS;

        return rv;
}

/*================================================================================================*/
/*===== VT_camera_test =====*/
/* 
@brief camera test function

@param None

@return On success - return TPASS
        On failure - return the error code
*/
/*================================================================================================*/
int VT_spi_test(long arg)
{
        int     err = 0;

        err = ioctl(spi_fd, SPI_SEND_FRAME, arg);
        if (err == -1)
        {
                tst_resm(TWARN, "Error: operation is too fast");
        }
        return err;
}
