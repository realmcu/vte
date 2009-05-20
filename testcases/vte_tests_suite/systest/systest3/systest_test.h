/*==================================================================================================

        Copyright (C) 2007, Freescale Semiconductor, Inc. All Rights Reserved
        THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
        BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
        Freescale Semiconductor, Inc.

====================================================================================================
Revision History:
                            Modification     Tracking
Author (core ID)                Date          Number    Description of Changes
-------------------------   ------------    ----------  -------------------------------------------
D.Khoroshev                  06/05/2007     ENGR37697   Initial version
==================================================================================================*/

#ifndef __SYSTEST_TEST_3_H__
#define __SYSTEST_TEST_3_H__

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
        char         mV4LDevOName[MAX_STR_LEN]; /*!< V4L output device name. */
        char         mMountPoint[MAX_STR_LEN];  /*!< FFS mount point. */
        char         mFFSDevName[MAX_STR_LEN];  /*!< FFS device name. */
        int          mEncryptOnly;
        int          mInjectCRCError;
} sTestappConfig;

/*==================================================================================================
                                     FUNCTION PROTOTYPES
==================================================================================================*/
int VT_systest_setup   ( void );
int VT_systest_cleanup ( void );
int VT_systest_test    ( void );

#endif //__SYSTEST_TEST_3_H__
