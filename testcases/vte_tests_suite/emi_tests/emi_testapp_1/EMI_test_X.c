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
    @file   EMI_test_X.c

    @brief  Test scenario C source template.
====================================================================================================
Revision History:
                            Modification     Tracking
Author                          Date          Number    Description of Changes
-------------------------   ------------    ----------  -------------------------------------------
Sergey Zavjalov[12~           10/06/2004     TLSbo39741   initial version 
====================================================================================================
Portability: Indicate if this module is portable to other compilers or platforms. 
             If not, indicate specific reasons why is it not portable.

==================================================================================================*/


/*==================================================================================================
                                        INCLUDE FILES
==================================================================================================*/
/* Standard Include Files */
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/ipc.h>
#ifdef _LINUX_
// defines struct msgbuf
#define __USE_GNU
#endif
#include <sys/msg.h>
#include <sys/types.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C"{
#endif
    
/* Harness Specific Include Files. */
#include "test.h"
#ifdef __cplusplus
}

#endif

/* Verification Test Environment Include Files */
#include "EMI_test_X.h"

/*==================================================================================================
                                        LOCAL MACROS
==================================================================================================*/

#define MAX_MSGS		8192
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
/*===== VT_EMI_setup =====*/
/**
@brief  assumes the pre-condition of the test case execution

@param  None
  
@return On success - return TPASS
        On failure - return the error code
*/
/*================================================================================================*/
int VT_EMI_setup(void)
{
    int rv = TPASS;
    
    /** insert your code here */
    
    return rv;
}


/*================================================================================================*/
/*===== VT_cleanup =====*/
/**
@brief  assumes the post-condition of the test case execution

@param  None
  
@return On success - return TPASS
        On failure - return the error code
*/
/*================================================================================================*/
int VT_EMI_cleanup(void)
{
    int rv = TPASS;
    
    /** insert your code here */
    
    return rv;
}


/*================================================================================================*/
/*===== VT_EMI_test_X =====*/
/**
@brief  Template test scenario X function

@param  None
  
@return On success - return TPASS
        On failure - return the error code
*/
/*================================================================================================*/
int VT_EMI_test_X(int argc, char** argv)
{
    int rv = TFAIL;
    int fd;
  
    if (argc != 2)
    {
		printf ("Open default device /dev/mtdblock/2\n");
		fd = open ("/dev/mtdblock/2", O_RDONLY);
    }
	else
	{
      fd = open (argv[1], O_RDONLY);
	  if (fd < 0) 
  	  {
		perror("Error open device");
		return TFAIL;
      }
	}
	
  close (fd);

    rv = TPASS;
    return rv;
}
