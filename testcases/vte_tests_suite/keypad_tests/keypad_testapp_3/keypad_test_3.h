/*================================================================================================*/
/**
        @file   keypad_test_3.h

        @brief  Test scenario C header template.
*/
/*==================================================================================================

        Copyright (C) 2006, Freescale Semiconductor, Inc. All Rights Reserved
        THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
        BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
        Freescale Semiconductor, Inc.
    
====================================================================================================
Revision History:
                            Modification     Tracking
Author/Core ID                  Date          Number    Description of Changes
-------------------------   ------------    ----------  -------------------------------------------
V. Becker/rc023c             30/04/2004     TLSbo38735   Initial version
V. Becker/rc023c             25/05/2004     TLSbo38735   Change file name
L. DELASPRE / rc149c         16/08/2004     TLSbo40891   VTE 1.4 integration 
C.Gagneraud cgag1c           08/11/2004     TLSbo44474   Fix #include issues.
A.Ozerov/NONE                10/01/2006     TLSbo61037   Update in accordance with linux-2.6.10-rel-L26_1_15

==================================================================================================*/
#ifndef KEYPAD_TEST_3_H
#define KEYPAD_TEST_3_H

#ifdef __cplusplus
extern "C"{
#endif

/*==================================================================================================
                                        INCLUDE FILES
==================================================================================================*/
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <linux/kd.h>
#include <sys/ioctl.h>
#include <asm/arch/mxc_keyb_ioctl.h>

/*==================================================================================================
                                            CONSTANTS
==================================================================================================*/

/*==================================================================================================
                                        DEFINES AND MACROS
==================================================================================================*/
#define KEYPAD_DRIVER "/dev/vc/0"

#if !defined(TRUE) && !defined(FALSE)
#define TRUE  1
#define FALSE 0
#endif

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
int VT_keypad_test_3_setup(void);
int VT_keypad_test_3_cleanup(void);

int VT_keypad_test_3(void);


#ifdef __cplusplus
}
#endif

#endif        /* KEYPAD_TEST_3_H */
