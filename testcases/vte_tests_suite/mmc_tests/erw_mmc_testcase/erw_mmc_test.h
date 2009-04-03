/*
  Copyright (C) 2004, Freescale Semiconductor, Inc. All Rights Reserved
  THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
  BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
  Freescale Semiconductor, Inc.
*/

/**
@file erw_mmc_test.h

@brief VTE C header MMC/SD testcase extracting event during read/write process

Description of the file

@par Portability:
        Indicate if this module is portable to other compilers or platforms.
        If not, indicate specific reasons why is it not portable.
*/

/*======================== REVISION HISTORY ==================================

Author (core ID)      Date         CR Number    Description of Changes
-------------------   ----------   ----------   ------------------------------
S.ZAVJALOV/zvjs001c   04/04/2005   tlsbo45047   Initial version

=============================================================================*/

#ifndef ERW_MMC_TEST_H
#define ERW_MMC_TEST_H

#ifdef __cplusplus
extern "C"{
#endif

/*======================== INCLUDE FILES ====================================*/


/*======================== CONSTANTS ========================================*/


/*======================== DEFINES AND MACROS ===============================*/

#define MMC_DEVICE_BLOCK1 "/dev/mmc/blk0/disc"
#define MMC_DEVICE_BLOCK2 "/dev/mmc/blk1/disc"

#define SLEEP_TIME  20000

#ifndef TRUE
#define TRUE  1
#endif

#ifndef FALSE
#define FALSE 0
#endif

/*======================== ENUMS ============================================*/


/*======================== STRUCTURES AND OTHER TYPEDEFS ====================*/

typedef unsigned char BOOLEAN;

/*======================== GLOBAL VARIABLE DECLARATIONS =====================*/


/*======================== FUNCTION PROTOTYPES ==============================*/

void cleanup();

int VT_erw_mmc_setup();
int VT_erw_mmc_cleanup();

int VT_erw_mmc_test();

#ifdef __cplusplus
}
#endif

#endif  // ERW_MMC_TEST_H //
