/***
 *Copyright 2010 Freescale Semiconductor, Inc. All Rights Reserved.
 *This program is free software; you can redistribute it and/or modify
 *it under the terms of the GNU General Public License as published by
 *the Free Software Foundation; either version 2 of the License, or
 *(at your option) any later version.
 *This program is distributed in the hope that it will be useful,
 *but WITHOUT ANY WARRANTY; without even the implied warranty of
 *MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *GNU General Public License for more details.
 *You should have received a copy of the GNU General Public License along
 *with this program; if not, write to the Free Software Foundation, Inc.,
 *51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
**/

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
