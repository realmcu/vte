/*=========u:=======================================================================================*/
/**
    @file   drawrectangle_test_X.c

    @brief  Test scenario C source drawrectangle.
*/
/*==================================================================================================

  Copyright (C) 2004, Freescale Semiconductor, Inc. All Rights Reserved
  THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
  BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
  Freescale Semiconductor, Inc.
     
====================================================================================================
Revision History:
                            Modification     Tracking
Author                          Date          Number    Description of Changes
-------------------------   ------------    ----------  -------------------------------------------
Developer Name/ID            DD/MM/YYYY     TLSboXXXXX   BRIEF description of changes made 

====================================================================================================
Portability: Indicate if this module is portable to other compilers or platforms. 
             If not, indicate specific reasons why is it not portable.

==================================================================================================*/

#ifdef __cplusplus
extern "C"{
#endif

/*==================================================================================================
                                        INCLUDE FILES
==================================================================================================*/
/* Standard Include Files */
#include <errno.h>
#include <stdio.h>    
/* Harness Specific Include Files. */
#include "test.h"

/* Verification Test Environment Include Files */
#include "drawrectangle_test_X.h"

/*==================================================================================================
                                        LOCAL MACROS
==================================================================================================*/


/*==================================================================================================
                          LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
==================================================================================================*/


/*==================================================================================================
                                       LOCAL CONSTANTS
==================================================================================================*/


/*==================================================================================================
                                       LOCAL VARIABLES
==================================================================================================*/


/*==================================================================================================
                                       GLOBAL CONSTANTS
==================================================================================================*/


/*==================================================================================================
                                       GLOBAL VARIABLES
==================================================================================================*/


/*==================================================================================================
                                   LOCAL FUNCTION PROTOTYPES
==================================================================================================*/


/*==================================================================================================
                                       LOCAL FUNCTIONS
==================================================================================================*/


/*================================================================================================*/
/*===== VT_drawrectangle_setup =====*/
/**
@brief  assumes the pre-condition of the test case execution

@param  None
  
@return On success - return TPASS
        On failure - return the error code
*/
/*================================================================================================*/
int VT_drawrectangle_setup(void)
{
    int rv = TPASS;
    
    /** insert your code here */
    
    return rv;
}


/*================================================================================================*/
/*===== VT_drawrectangle_cleanup =====*/
/**
@brief  assumes the post-condition of the test case execution

@param  None
  
@return On success - return TPASS
        On failure - return the error code
*/
/*================================================================================================*/
int VT_drawrectangle_cleanup(void)
{
    int rv = TPASS;
    
    /** insert your code here */
    
    return rv;
}


/*================================================================================================*/
/*===== VT_drawrectangle_test_X =====*/
/**
@brief  drawrectangle test scenario X function

@param  None
  
@return On success - return TPASS
        On failure - return the error code
*/
/*================================================================================================*/
int VT_drawrectangle_test_X(int argc,char** argv)
{
    int rv = TPASS;
    /** insert your code here */
    rv=drawrectangle_main( argc, argv);
    return rv;
}
    
#ifdef __cplusplus
}
#endif
