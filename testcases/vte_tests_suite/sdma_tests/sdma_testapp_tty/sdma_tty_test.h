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
    @file   sdma_tty_test.h

    @brief  SDMA TTY test header file
====================================================================================================
Revision History:
                            Modification     Tracking
Author (core ID)                Date          Number    Description of Changes
-------------------------   ------------    ----------  -------------------------------------------
V. ZAVJALOV/----             19/07/2004     TLSbo40259  Initial version
S.ZAVJALOV/ZVJS001           20/09/2004     TLSbo41740  Device name changed
E.Gromazina                  31/10/2005     TLSbo56685  Fix bag
==================================================================================================*/
#ifndef sdma_tty_test_H
#define sdma_tty_test_H

#ifdef __cplusplus
extern "C"{
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

#include "asm/arch/mxc_sdma_tty.h"
/*#include "asm-arm/arch-MXC91131/mxc_sdma_tty.h"*/

/*==================================================================================================
                                            CONSTANTS
==================================================================================================*/


/*==================================================================================================
                                        DEFINES AND MACROS
==================================================================================================*/

#define SDMA_TTY_DEVICE "/dev/sdma0"

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

void help(void);

int VT_sdma_tty_test_setup(void);
int VT_sdma_tty_test_cleanup(void);

int VT_sdma_tty_test(void);

#ifdef __cplusplus
}
#endif

#endif  /* sdma_tty_test_H */
