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
D.Simakov                    06/06/2007      ENGR37682   Initial version
==================================================================================================*/

#ifndef __SYSTEST3_TEST_H__
#define __SYSTEST3_TEST_H__

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

#define FIELDS_LENGTH            0x0010

#define X_SIZE_OFFSET            0x0010
#define Y_SIZE_OFFSET            0x0020
#define RESERVED_AREA_OFFSET     0x0030
#define RAW_DATA_OFFSET          0x0100


/*==================================================================================================
                                             ENUMS
==================================================================================================*/

/*==================================================================================================
                                 STRUCTURES AND OTHER TYPEDEFS
==================================================================================================*/
typedef struct
{
        int          mThreadToExecute;            /*!< Number of thread to execute. */
        int          mVerbose;                    /*!< Verbose mode. */
        char         mV4LDevName[MAX_STR_LEN];    /*!< V4L device name. */
        char         mVideoFileName[MAX_STR_LEN]; /*!< Video stream to play. */
        int          mVideoFileDesc;              /*!< Video stream file descriptor. */
        int          mWidth;                      /*!< Frame width. */
        int          mHeight;                     /*!< Frame height. */
        char         mMountPoint[MAX_STR_LEN];    /*!< FFS mount point. */
        char         mFFSDevName[MAX_STR_LEN];    /*!< FFS device name. */
} sTestappConfig;

typedef struct
{
        char    image_fmt[FIELDS_LENGTH];
        char    image_width[FIELDS_LENGTH];
        char    image_height[FIELDS_LENGTH];
        char    reserved_area[RAW_DATA_OFFSET - RESERVED_AREA_OFFSET];

} dump_file_header;

/*==================================================================================================
                                     FUNCTION PROTOTYPES
==================================================================================================*/
int VT_systest_setup   ( void );
int VT_systest_cleanup ( void );
int VT_systest_test    ( void );

#endif //__SYSTEST2_TEST_H__
