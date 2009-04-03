/*/
  Copyright (C) 2004, Freescale Semiconductor, Inc. All Rights Reserved
  THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
  BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
  Freescale Semiconductor, Inc.
*/

/**
@file TEMPLATE_test_X.c

@brief VTE C source template

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

#ifdef __cplusplus
extern "C"{
#endif

/*======================== INCLUDE FILES ====================================*/
/* Standard Include Files */
#include <errno.h>
    
/* Harness Specific Include Files. */
#include "test.h"

/* Verification Test Environment Include Files */
#include "TEMPLATE_test_X.h"

/*======================== LOCAL CONSTANTS ==================================*/


/*======================== LOCAL MACROS =====================================*/


/*======================== LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS) =======*/


/*======================== LOCAL VARIABLES ==================================*/


/*======================== GLOBAL CONSTANTS =================================*/


/*======================== GLOBAL VARIABLES =================================*/


/*======================== LOCAL FUNCTION PROTOTYPES ========================*/


/*======================== LOCAL FUNCTIONS ==================================*/


/*======================== GLOBAL FUNCTIONS =================================*/

/*===== VT_TEMPLATE_setup =====*/
/**
Description of the function
@brief  assumes the pre-condition of the test case execution
@pre None
@post None
@param  Input : None.
        Output: None.
@return On success - return TPASS
        On failure - return the error code
@bug No bug
@deprecated Not applicable
@todo Nothing
*/
int VT_TEMPLATE_setup(void)
{
    int rv = TFAIL;
    
    /** insert your code here */
    
    rv = TPASS;
    return rv;
}


/*===== VT_TEMPLATE_cleanup =====*/
/**
Description of the function
@brief  assumes the post-condition of the test case execution
@pre None
@post None
@param  Input : None.
        Output: None.
@return On success - return TPASS
        On failure - return the error code
@bug No bug
@deprecated Not applicable
@todo Nothing
*/
int VT_TEMPLATE_cleanup(void)
{
    int rv = TFAIL;
    
    /** insert your code here */
    
    rv = TPASS;
    return rv;
}


/*===== VT_TEMPLATE_test_X =====*/
/**
@brief  Template test scenario X function

@param  None
  
@return On success - return TPASS
        On failure - return the error code
*/
int VT_TEMPLATE_test_X(int test_num)
{
    int rv = TFAIL;
    
    rv = VT_TEMPLATE_setup();
    if (rv != TPASS)
    {
        tst_brkm(TBROK , cleanup, "VT_TEMPLATE_setup() Failed : error code = %d", rv);
    }

    /** replace this following example code by ours */
    switch (test_num)
    {
        case 1:
            tst_resm(TPASS, "Test case %d: -n %d",test_num,test_num);
            rv = TPASS;
        break;
        
        case 2:
            tst_resm(TFAIL, "Test case %d: -n %d",test_num,test_num);
            rv = TFAIL;
        break;
        
        default:
            tst_brkm(TBROK , cleanup, "Error: This test case has been broken");
    }
    /** replace this previous example code by ours */
    
    return rv;
}



#ifdef __cplusplus
}
#endif
