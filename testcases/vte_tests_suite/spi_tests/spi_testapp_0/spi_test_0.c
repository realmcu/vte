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
        @file   spi_test_0.c

        @brief  Test scenario C source for spi.
====================================================================================================
Revision History:
                            Modification     Tracking
Author/core ID                  Date          Number    Description of Changes
-------------------------   ------------    ----------  -------------------------------------------
Tony THOMASSIN/RB595C        18/05/2004     TLSbo39490  SPI test development 
C. Gagneraud                 08/11/2004     TLSbo44474  Warning fixup.
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
#include "spi_test_0.h"

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
/*===== VT_spi_setup =====*/
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

        spi_fd = open(device_filename, O_RDWR);

        if (spi_fd == -1)
        {
                tst_resm(TBROK, "Can't open spi test device file %s\n",device_filename);
                return rv;
        }

        rv = TPASS;
        return rv;
}

/*================================================================================================*/
/*===== VT_spi_cleanup =====*/
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

        if (spi_fd >= 0)
                close(spi_fd);

        rv = TPASS;

        return rv;
}

/*================================================================================================*/
/*===== VT_spi_buffer_test =====*/
/**
@brief  spi test scenario function for writing data

@param  int   bytes
        char* buffer
    
@return On success - return TPASS
        On failure - return the error code
*/
/*================================================================================================*/
int VT_spi_buffer_test(int bytes, char *buffer)
{
        int     rv = TPASS;
        int     res = 0;
        char    tmp[2 * BUFF_TEST_MAX_SIZE];

        tst_resm(TINFO, "Data to be sent : %s", (char *) buffer);

        /* write data */
        res = write(spi_fd, buffer, bytes);
        if (res < 0)
        {
                tst_resm(TFAIL, "Failed when writing data to SPI: %d", errno);
                rv = TFAIL;
        }

        memset(tmp, 0, (2 * BUFF_TEST_MAX_SIZE));

        /* read data back */
        res = read(spi_fd, tmp, bytes);
        if (res < 0)
        {
                tst_resm(TFAIL, "Failed when reading data from SPI: %d", errno);
                rv = TFAIL;
        }

        res = VT_check_data_integrity(buffer, tmp, bytes);
        if (res != 0)
        {
                tst_resm(TFAIL, "Data corruption. Test did not work as expected");
                rv = TFAIL;
        }
        else
          tst_resm(TINFO, "Data received : %s", (char *) tmp);  /*b02578: TFAIL replaced by TINFO*/


        return rv;
}

/*================================================================================================*/
/*===== VT_spi_multiclient_test =====*/
/**
@brief  spi test scenario function for multi-client test

@param  None
    
@return On success - return TPASS
        On failure - return the error code
*/
/*================================================================================================*/
int VT_spi_multiclient_test(void)
{
        return -1;
}

/*================================================================================================*/
/*===== VT_check_data_integrity =====*/
/**
@brief  Check data integrity

@param  
    
@return On success - return TPASS
        On failure - return the error code
*/
/*================================================================================================*/
int VT_check_data_integrity(char *buf1, char *buf2, int count)
{
        int     rv = TPASS;
        int     i;

        for (i = 0; i < count; i++)
        {
                if (*buf1++ != *buf2++)
                {
                        buf1--; 
                        buf2--;
                        tst_resm(TFAIL, "Corrupted data at %d wbuf = %d rbuf = %d", i, *buf1, *buf2);
                        rv = TFAIL;
                }
        }

        return rv;
}
