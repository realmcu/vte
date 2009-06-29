/***
**Copyright 2006-2009 Freescale Semiconductor, Inc. All Rights Reserved.
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
        @file   udma_test.h

        @brief  Unified DMA test H-file

====================================================================================================
Revision History:
                            Modification     Tracking
Author/core ID                  Date          Number    Description of Changes
-------------------------   ------------    ----------  -------------------------------------------
A.Ozerov/b00320              03/10/2006     TLSbo78550  Initial version.
A.Ozerov/b00320              01/11/2006     TLSbo81158  UDMA module was fixed for working with all platforms.

====================================================================================================
Portability:  ARM GCC
==================================================================================================*/
#ifndef __MXC_UDMA_TEST_H__
#define __MXC_UDMA_TEST_H__

#ifdef __cplusplus
extern "C"{
#endif

/*==================================================================================================
                                         INCLUDE FILES
==================================================================================================*/
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#include <errno.h>
#include <test.h>
#include <usctest.h>

#include "udma_test_module.h"

/*==================================================================================================
                                      DEFINES AND MACROS
==================================================================================================*/
#define CALL_IOCTL(ioctl_) {if((ioctl_) < 0) {tst_resm(TFAIL, "function %s failed. Error code: %d", __FUNCTION__, ioctl_); ret = TFAIL;} }

/*==================================================================================================
                                STRUCTURES AND OTHER TYPEDEFS
==================================================================================================*/

/*==================================================================================================
                                       GLOBAL VARIABLES
==================================================================================================*/

/*==================================================================================================
                                          FUNCTIONS
==================================================================================================*/
int VT_udma_test_setup(void);
int VT_udma_test(int test_num, int t_tranfer);
int VT_udma_test_cleanup(void);

#ifdef __cplusplus
}
#endif

#endif /* __MXC_UDMA_TEST_H__ */
