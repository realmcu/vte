/*
  Copyright (C) 2004, Freescale Semiconductor, Inc. All Rights Reserved
  THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
  BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
  Freescale Semiconductor, Inc.
*/

/**
@file TEMPLATE_test_X.h

@brief VTE C header template

Description of the file

@par Portability:
        Indicate if this module is portable to other compilers or platforms.
        If not, indicate specific reasons why is it not portable.
*/

/*======================== REVISION HISTORY ==================================

Author (core ID)      Date         CR Number    Description of Changes
-------------------   ----------   ----------   ------------------------------
Developer Name/ID     DD/MM/YYYY   TLSboXXXXX   BRIEF desc. of changes 
Delaspre/rc149c       07/12/2004   TLSbo40142   update copyrights with Freescale

=============================================================================*/

#ifndef TEMPLATE_TEST_X_H
#define TEMPLATE_TEST_X_H

#ifdef __cplusplus
extern "C"{
#endif

/*======================== INCLUDE FILES ====================================*/


/*======================== CONSTANTS ========================================*/


/*======================== DEFINES AND MACROS ===============================*/


/*======================== ENUMS ============================================*/

/** TEMPLATE_EX type */
typedef enum 
{
    TEMPLATE_EX_0 = 0,   /**< Example stuff 0. */
    TEMPLATE_EX_1        /**< Example stuff 1. */
} TEMPLATE_EX_T;


/*======================== STRUCTURES AND OTHER TYPEDEFS ====================*/


/*======================== GLOBAL VARIABLE DECLARATIONS =====================*/


/*======================== FUNCTION PROTOTYPES ==============================*/

void cleanup();

int VT_TEMPLATE_setup();
int VT_TEMPLATE_cleanup();

int VT_TEMPLATE_test_X(int test_num);



#ifdef __cplusplus
}
#endif

#endif  // TEMPLATE_TEST_X_H //
