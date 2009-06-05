/*================================================================================================*/
/**
    @file   hacc_test.h

    @brief  hacc API test header file
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
S.ZAVJALOV/zvjs001c          10/08/2004     TLSbo40418   Initial version 
S.ZAVJALOV/zvjs001c          01/10/2004     TLSbo40649   Version after inspection
A.URUSOV                     14/09/2005     TLSbo53754   Line 43 include path deleted
==================================================================================================*/

#ifndef HACC_TEST_H
#define HACC_TEST_H

#ifdef __cplusplus
extern  "C"
{
#endif

/*==================================================================================================
                                         INCLUDE FILES
==================================================================================================*/

#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#include "test.h"
#include "usctest.h"

#include "hacc_test_module.h"

/*==================================================================================================
                                     FUNCTION PROTOTYPES
==================================================================================================*/

void    help(void);
int     VT_hacc_test_setup(void);
int     VT_hacc_test_cleanup(void);
int     VT_hacc_test(void);

#ifdef __cplusplus
}
#endif

#endif /* HACC_TEST_H */
