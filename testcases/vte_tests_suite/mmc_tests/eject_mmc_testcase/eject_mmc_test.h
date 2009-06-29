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
@file eject_mmc_test.h

@brief VTE C header MMC/SD device input/eject testcase

Description of the file

@par Portability:
        Indicate if this module is portable to other compilers or platforms.
        If not, indicate specific reasons why is it not portable.*/

/*======================== REVISION HISTORY ==================================

Author (core ID)            Date           CR Number      Description of Changes
-------------------   ----------   ----------   ------------------------------
S.ZAVJALOV/zvjs001c   22/03/2005   tlsbo46706       Initial version
I.Inkina/nknl001           25/07/2005   TLSbo50891      Update open device  
E.Gromazina                14/10/2005    TLSbo56643     Update for the first MMC
=============================================================================*/

#ifndef EJECT_MMC_TEST_H
#define EJECT_MMC_TEST_H

#ifdef __cplusplus
extern "C"{ 
#endif

/*======================== INCLUDE FILES ====================================*/


/*======================== CONSTANTS ========================================*/


/*======================== DEFINES AND MACROS ===============================*/

#define SLEEP_TIME  20000       /* Wait for character with timeout 25ms */
/* 
* #define MMC_DEVICE_BLOCK1 "/dev/mmc/blk0/disc" #define MMC_DEVICE_BLOCK2 "/dev/mmc/blk1/disc" */
#ifndef TRUE
#define TRUE  1
#endif

#ifndef FALSE
#define FALSE 0
#endif

/*======================== ENUMS ============================================*/


/*======================== STRUCTURES AND OTHER TYPEDEFS ====================*/

typedef unsigned char BOOLEAN;
typedef struct par
{
        char    file_name_mmc_1[20];
        int     number_card;
        int     file_desc;
} param_mmc;

/*======================== GLOBAL VARIABLE DECLARATIONS =====================*/


/*======================== FUNCTION PROTOTYPES ==============================*/

void    cleanup(void);

int     VT_eject_mmc_setup(void);
int     VT_eject_mmc_cleanup(void);

int     VT_eject_mmc_test(param_mmc * par);

#ifdef __cplusplus
} 
#endif

#endif                          // EJECT_MMC_TEST_H //
