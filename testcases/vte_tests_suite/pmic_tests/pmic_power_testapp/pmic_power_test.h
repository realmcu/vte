/*================================================================================================*/
/**
        @file   pmic_test_power.h

        @brief  Test scenario C header for PMIC Power driver.
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
S.Bezrukov/SBAZR1C           07/20/2005     TLSbo52698  Initial version
S.Bezrukov/SBAZR1C           09/06/2005     TLSbo52698  Rework  version
N.Filinova/nfili1c           16/01/2006     TLSbo61037  Rework version
D.Khoroshev/b00313           07/17/2006     TLSbo64236  Added mc13783 legacy support

==================================================================================================
Portability:  ARM GCC
==================================================================================================*/
#ifndef PMIC_TEST_POWER_H
#define PMIC_TEST_POWER_H

#ifdef __cplusplus
extern "C"{
#endif


/*==================================================================================================
                                         INCLUDE FILES
==================================================================================================*/
#include <sys/types.h>  /* open() */
#include <sys/stat.h>   /* open() */
#include <fcntl.h>      /* open() */
#include <sys/ioctl.h>  /* ioctl() */
#include <unistd.h>     /* close() */
#include <stdio.h>      /* sscanf() & perror() */
#include <stdlib.h>     /* atoi() */
#include <errno.h>
#include <linux/wait.h>

#include <linux/autoconf.h>


#include <linux/pmic_status.h>
#include <asm/arch-mxc/pmic_power.h>

/*==================================================================================================
                                       DEFINES AND MACROS
==================================================================================================*/
#if !defined(TRUE) && !defined(FALSE)
#define TRUE  1
#define FALSE 0
#endif


#define PMIC_POWER_DEV_NAME "/dev/pmic_power"
#define SC55112_PARAMS_NUM 6
#define MC13783_PARAMS_NUM 11
#define SC55112_REGS_NUM 10
#define MC13783_REGS_NUM 28

/*==================================================================================================
                                             ENUMS
==================================================================================================*/
/** Different test cases in the single application */
typedef enum
{
        ENABLE = 0,
        CONFIG,
        ERR_CONFIG_PARAMS,
} PMIC_POWER_TESTCASES;

/*==================================================================================================
                                 STRUCTURES AND OTHER TYPEDEFS
==================================================================================================*/
typedef struct
{
        int mTestCase;
        char mCfgFile[256];
        int mList;
        int mVerbose;
        int mWriteConfig;

} sTestConfig;

/*==================================================================================================
                                 GLOBAL VARIABLE DECLARATIONS
==================================================================================================*/
extern sTestConfig gTestConfig;

/*==================================================================================================
                                       LOCAL CONSTANTS
==================================================================================================*/

/*==================================================================================================
                                     FUNCTION PROTOTYPES
==================================================================================================*/
int VT_pmic_power_test_setup(void);
int VT_pmic_power_test_cleanup(void);
int VT_pmic_power_test(void);

#ifdef __cplusplus
}
#endif

#endif        /* PMIC_TEST_POWER_H */
