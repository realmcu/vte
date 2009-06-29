/***
**Copyright 2004-2009 Freescale Semiconductor, Inc. All Rights Reserved.
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
    @file   hacc_test.h

    @brief  hacc API test header file
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
