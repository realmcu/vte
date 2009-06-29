/***
**Copyright 2007-2009 Freescale Semiconductor, Inc. All Rights Reserved.
**
**The code contained herein is licensed under the GNU General Public
**License. You may obtain a copy of the GNU General Public License
**Version 2 or later at the following locations:
**
**http://www.opensource.org/licenses/gpl-license.html
**http://www.gnu.org/copyleft/gpl.html
**/
/*=================================================================================================
Revision History:
                            Modification     Tracking
Author (core ID)                Date          Number    Description of Changes
-------------------------   ------------    ----------  -------------------------------------------
D.Simakov                    10/04/2007      ENGR37676   Initial version
==================================================================================================*/

#ifndef __SYSTEST2_TEST_H__
#define __SYSTEST2_TEST_H__

/*==================================================================================================
                                         INCLUDE FILES
==================================================================================================*/

/* Harness Specific Include Files.*/
#include <test.h>
#include <usctest.h>

/*==================================================================================================
                                       DEFINES AND MACROS
==================================================================================================*/
#if !defined(TRUE) && !defined(FALSE)
    #define TRUE  1
    #define FALSE 0
#endif

#define MAX_STR_LEN 80

/*==================================================================================================
                                             ENUMS
==================================================================================================*/

/*==================================================================================================
                                 STRUCTURES AND OTHER TYPEDEFS
==================================================================================================*/
typedef struct
{
        int          mThreadToExecute;          /*!< Number of thread to execute. */
        int          mVerbose;                  /*!< Verbose mode. */        
        char         mV4LDevName[MAX_STR_LEN];  /*!< V4L device name. */
        char         mMountPoint[MAX_STR_LEN];  /*!< FFS mount point. */
        char         mFFSDevName[MAX_STR_LEN];  /*!< FFS device name. */
} sTestappConfig;

/*==================================================================================================
                                     FUNCTION PROTOTYPES
==================================================================================================*/
int VT_systest_setup   ( void );
int VT_systest_cleanup ( void );
int VT_systest_test    ( void );

#endif //__SYSTEST2_TEST_H__
