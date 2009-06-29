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
        @file  rw_mmc_test.h

        @brief Header file for MMC driver test scenario
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
#ifndef RW_MMC_TEST_H
#define RW_MMC_TEST_H

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

int VT_rw_mmc_setup(void);
int VT_rw_mmc_cleanup(void);

int VT_rw_mmc_test(void);

#ifdef __cplusplus
}
#endif

#endif        /* RW_MMC_TEST_H */
