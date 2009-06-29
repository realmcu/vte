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
/**
@file lock_mmc_test.h

@brief VTE C header MMC/SD device read/write testcase with locking switch

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

#ifndef LOCK_MMC_TEST_H
#define LOCK_MMC_TEST_H

#ifdef __cplusplus
extern "C"{
#endif

/*======================== INCLUDE FILES ====================================*/


/*======================== CONSTANTS ========================================*/


/*======================== DEFINES AND MACROS ===============================*/

#define READ_BLOCK	0x80
#define WRITE_BLOCK	0x80

/*======================== ENUMS ============================================*/


/*======================== STRUCTURES AND OTHER TYPEDEFS ====================*/


/*======================== GLOBAL VARIABLE DECLARATIONS =====================*/


/*======================== FUNCTION PROTOTYPES ==============================*/

void cleanup();

int VT_lock_mmc_setup();
int VT_lock_mmc_cleanup();

int VT_lock_mmc_test();

#ifdef __cplusplus
}
#endif

#endif  // LOCK_MMC_TEST_H //

