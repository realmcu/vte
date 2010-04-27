/***
**Copyright (C) 2005-2009 Freescale Semiconductor, Inc. All Rights Reserved.
**
**The code contained herein is licensed under the GNU General Public
**License. You may obtain a copy of the GNU General Public License
**Version 2 or later at the following locations:
**
**http://www.opensource.org/licenses/gpl-license.html
**http://www.gnu.org/copyleft/gpl.html
**/
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
