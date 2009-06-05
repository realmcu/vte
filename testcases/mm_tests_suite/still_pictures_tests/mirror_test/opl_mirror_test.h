/*================================================================================================*/
/**
    @file   opl_mirror_test.h

    @brief  OPL image processing library mirroring functions tests header file
*/
/*==================================================================================================

  Copyright (C) 2004, Freescale Semiconductor, Inc. All Rights Reserved
  THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
  BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
  Freescale Semiconductor, Inc.
     
====================================================================================================
Revision History:
                            Modification     Tracking
Author (core ID)                Date          Number    Description of Changes
-------------------------   ------------    ----------  -------------------------------------------
Smirnov A.                    DD/MM/YYYY     TLSboXXXXX  BRIEF description of changes made 

==================================================================================================*/

#ifndef _OPL_MIRROR_TEST_H_ 
#define _OPL_MIRROR_TEST_H_

#ifdef __cplusplus
extern "C"{
#endif

/*==================================================================================================
                                         INCLUDE FILES
==================================================================================================*/

#include <stdio.h>
#include <stdlib.h> 
#include <string.h>
#include <ctype.h>
#include <math.h>


#include <pthread.h> 
#include <unistd.h>
#include <linux/fb.h>
#include <asm/ioctls.h> 
#include <asm/fcntl.h> 
#include <asm/mman.h> 
#include <sys/types.h>

#include "oplTypes.h"
#include "oplIP.h"
/* #include "opl_debug.h" */ 
 

/*==================================================================================================
                                           CONSTANTS
==================================================================================================*/


/*==================================================================================================
                                       DEFINES AND MACROS
==================================================================================================*/

#if !defined(TRUE) && !defined(FALSE)
#define TRUE  1
#define FALSE 0
#endif


#define ENDURANCE       1       /* functional test - any data - grey & RGB */
#define STRESS          2       /* stress test - overloaded environment   */
#define REENTRANCE      3       /* concurrency test - lib reentrancy      */
#define PROCESSED     1
#define PTHREADED     2
#define PREEMPTIVE      4       /* concurrency test - lib preemptivity    */
#define RESTITUTION     5       /* functional tets - even transformations */
	
#define ITERATIONS      1

#define NO_BMP		0
#define WR_BMP		1


#define _TRACE /* printf("***************We are in %s at line %d.******************\n", __FILE__, __LINE__);*/


/*==================================================================================================
                                             ENUMS
==================================================================================================*/

	

/*==================================================================================================
                                 STRUCTURES AND OTHER TYPEDEFS
==================================================================================================*/


/*==================================================================================================
                                 GLOBAL VARIABLE DECLARATIONS
==================================================================================================*/


/*==================================================================================================
                                     FUNCTION PROTOTYPES
==================================================================================================*/
int VT_opl_mirror_setup();
int VT_opl_mirror_cleanup();

int VT_opl_mirror_test(int Id, int Case, int Iter, int Writ, int r_flag, int R_flag);


#ifdef __cplusplus
}
#endif

#endif  /* _OPL_MIRROR_TEST_H_ */
