/*================================================================================================*/
/**
        @file   pmic_audio_test.h

        @brief  Header file for PMIC audio driver test
*/
/*==================================================================================================

        Copyright (C) 2006, Freescale Semiconductor, Inc. All Rights Reserved
        THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
        BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
        Freescale Semiconductor, Inc.

====================================================================================================
Revision History:
                            Modification     Tracking
Author/core ID                  Date          Number    Description of Changes
-------------------------   ------------    ----------  -------------------------------------------
S.Bezrukov/SBAZR1C           31/08/2005     TLSbo52697  Initial version
A.Ozerov/b00320              08/08/2006     TLSbo73745  Review version(in accordance to L26_1_19 release).

====================================================================================================
Portability: ARM GCC
==================================================================================================*/
#ifndef PMIC_AUDIO_TEST_H
#define PMIC_AUDIO_TEST_H

#ifdef __cplusplus
extern "C"{
#endif


/*==================================================================================================
                                        INCLUDE FILES
==================================================================================================*/
#include <sys/types.h>        /* open() */
#include <sys/stat.h>        /* open() */
#include <fcntl.h>        /* open() */
#include <sys/ioctl.h>        /* ioctl() */
#include <unistd.h>        /* close() */
#include <stdio.h>        /* sscanf() & perror() */
#include <stdlib.h>        /* atoi() */
#include <test.h>
#include <usctest.h>
#include <errno.h>
#include <linux/wait.h>

#if defined( CONFIG_MXC_PMIC_SC55112 ) || defined( CONFIG_MXC_PMIC_MC13783 )
#include "pmic_audio_module.h"
#endif


/*==================================================================================================
                                        DEFINES AND MACROS
==================================================================================================*/
#if !defined(TRUE) && !defined(FALSE)
#define TRUE  1
#define FALSE 0
#endif

/*==================================================================================================
                                        LOCAL CONSTANTS
==================================================================================================*/

/*==================================================================================================
                                       FUNCTION PROTOTYPES
==================================================================================================*/
int VT_pmic_audio_test_setup(void);
int VT_pmic_audio_test_cleanup(void);
int VT_pmic_audio_test(int);

#ifdef __cplusplus
}
#endif

#endif  /* PMIC_AUDIO_TEST_H */
