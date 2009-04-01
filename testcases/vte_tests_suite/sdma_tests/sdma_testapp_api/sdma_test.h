/*================================================================================================*/
/**
        @file   sdma_test.h

        @brief  SDMA API test header file
*/
/*==================================================================================================

        Copyright (C) 2006, Freescale Semiconductor, Inc. All Rights Reserved
        THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
        BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
        Freescale Semiconductor, Inc.

====================================================================================================
Revision History:
                            Modification     Tracking
Author/Core ID                  Date          Number    Description of Changes
-------------------------   ------------    ----------  -------------------------------------------
S.ZAVJALOV/----              13/07/2004     TLSbo40259  Initial version
L.DELASPRE/rc149c            13/07/2004     TLSbo40891  VTE 1.4 integration
S.ZAVJALOV/ZVJS001           20/09/2004     TLSbo42212  Fixed return error code
C.GAGNERAUD/CGAG1C           26/10/2004     TLSbo43815  Update sdma_periphT
A.Ozerov/B00320              10/02/2006     TLSbo61734  Code was cast to coding conventions
A.Ozerov/B00320              19/04/2007     ENGR29785   MAX_DMA_CHANNELS definition was added

==================================================================================================*/
#ifndef SDMA_TEST_H
#define SDMA_TEST_H

#ifdef __cplusplus
extern "C"{
#endif

/*==================================================================================================
                                        INCLUDE FILES
==================================================================================================*/
/* Standard Include Files */
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/ioctl.h>

/* Harness Specific Include Files. */
#include <test.h>
#include <usctest.h>

/*==================================================================================================
                                            CONSTANTS
==================================================================================================*/

/*==================================================================================================
                                        DEFINES AND MACROS
==================================================================================================*/
#define MXC_SDMA_TEST_1 1
#define MXC_SDMA_TEST_2 2
#define MXC_SDMA_TEST_3 3
#define MXC_SDMA_TEST_4 4

#define SDMA_TEST_DEVICE "/dev/sdma_test_module"
#define MAX_DMA_CHANNELS 32

/*==================================================================================================
                                            ENUMS
==================================================================================================*/
typedef enum 
{
        SSI,        /*!< MCU domain SSI */
        SSI_SP,     /*!< Shared SSI */
        MMC,        /*!< MMC */
        SDHC,       /*!< SDHC */
        UART,       /*!< MCU domain UART */
        UART_SP,    /*!< Shared UART */
        FIRI,       /*!< FIRI */
        CSPI,       /*!< MCU domain CSPI */
        CSPI_SP,    /*!< Shared CSPI */
        SIM,        /*!< SIM */
        ATA,        /*!< ATA */
        CCM,        /*!< CCM */
        EXT,        /*!< External peripheral */
        MSHC,       /*!< Memory Stick Host Controller */
        DSP,        /*!< DSP */
        MEMORY      /*!< Memory */
} sdma_periphT;

/*==================================================================================================
                                STRUCTURES AND OTHER TYPEDEFS
==================================================================================================*/
typedef struct 
{
        unsigned int channel;
        unsigned int param;
} test_param;

/*==================================================================================================
                                GLOBAL VARIABLE DECLARATIONS
==================================================================================================*/

/*==================================================================================================
                                    FUNCTION PROTOTYPES
==================================================================================================*/
void help(void);

int VT_sdma_test_setup(void);
int VT_sdma_test_cleanup(void);

int VT_sdma_test(void);

#ifdef __cplusplus
}
#endif

#endif        /* SDMA_TEST_H */
