/*================================================================================================*/
/**
        @file  perform_mmc_test.h

        @brief Header file for MMC driver test scenario
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
S.ZAVJALOV/zvjs001c          22/03/2005     TLSbo46706  Initial version
A.Ozerov/b00320              20/02/2006     TLSbo61899  Testapp was cast to coding standarts

====================================================================================================
Portability:  ARM GCC
==================================================================================================*/
#ifndef PERFORM_MMC_TEST_H
#define PERFORM_MMC_TEST_H

#ifdef __cplusplus
extern "C"{
#endif

/*==================================================================================================
                                        INCLUDE FILES
==================================================================================================*/
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <time.h>

/*==================================================================================================
                                    DEFINES AND MACROS
==================================================================================================*/
#define MMC_BLOCK_SIZE       0x200
#define MMC_SECTOR_SIZE      0x7D000

/*==================================================================================================
                                    FUNCTION PROTOTYPES
==================================================================================================*/
void cleanup(void);

int VT_perform_mmc_setup(void);
int VT_perform_mmc_cleanup(void);

int VT_perform_mmc_test(void);

#ifdef __cplusplus
}
#endif

#endif        /* PERFROM_MMC_TEST_H */
