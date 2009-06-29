/***
**Copyright 2005-2009 Freescale Semiconductor, Inc. All Rights Reserved.
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
        @file   spi_test_module.h

        @brief  SPI test module header file
====================================================================================================
Revision History:
                            Modification     Tracking
Author/core ID                  Date          Number    Description of Changes
-------------------------   ------------    ----------  --------------------------------------------
V.Khalabuda/b00306           17/04/2006     TLSbo67010  Update version for linux-2.6.10-rel-L26_1_17
D.Kazachkov/b00316           30/05/2006     TLSbo67010  Update version for linux-2.6.10-rel-L26_1_18
V.Khalabuda/b00306           17/04/2006     TLSbo72876  Arrangement of device create using classes
D.Khoroshev/b00313           02/01/2006     TLSbo86657  Adaptation to linux 2.6.18 spi driver model

====================================================================================================
Portability:  ARM GCC
==================================================================================================*/
#ifndef SPI_TEST_H
#define SPI_TEST_H

#ifdef __cplusplus
extern "C"{
#endif

/*==================================================================================================
                                            CONSTANTS
==================================================================================================*/

/*==================================================================================================
                                        DEFINES AND MACROS
==================================================================================================*/
#define BURST_TEST_SIZE                                 0x4

#define BUFF_TEST_MAX_SIZE                              0x20

#define MXC_SPI_DEVS                                    "spi_test"
#define DEV_MXC_SPI                                     "spi_test"
#define SPI1 0
#define SPI2 1
#define SPI3 2

/*==================================================================================================
                                            ENUMS
==================================================================================================*/
/* Test IOCTLS */
typedef enum
{
        SPI_SEND_FRAME=0x10,
} SPI_TEST_IOCTL;

/*==================================================================================================
                                STRUCTURES AND OTHER TYPEDEFS
==================================================================================================*/

/*==================================================================================================
                                GLOBAL VARIABLE DECLARATIONS
==================================================================================================*/

/*==================================================================================================
                                    FUNCTION PROTOTYPES
==================================================================================================*/
#ifdef __cplusplus
}
#endif

#endif        /* SPI_TEST_H */
