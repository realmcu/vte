/***
**Copyright 2005-2009 Freescale Semiconductor, Inc. All Rights Reserved.
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
        @file   spi_test.h

        @brief  Test scenario C header template
====================================================================================================
Revision History:
                            Modification     Tracking
Author/core ID                  Date          Number    Description of Changes
-------------------------   ------------    ----------  -------------------------------------------
A.Smirnov/ID                 16/06/2005     TLSbo51450  BRIEF description of changes made 
V.Khalabuda/b00306           17/04/2006     TLSbo67010  Update version for linux-2.6.10-rel-L26_1_17
D.Khoroshev/b00313           02/01/2006     TLSbo86657  Adaptation to new spi interface

==================================================================================================*/
#ifndef _SPI_TEST_H_
#define _SPI_TEST_H_

#ifdef __cplusplus
extern "C"{
#endif

/*==================================================================================================
                                        INCLUDE FILES
==================================================================================================*/
/* Standard Include Files */
#include <stdio.h>
#include <errno.h>
#include <ctype.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <pthread.h>

#include <test.h>
#include <usctest.h>

#include "spi_test_module.h"

/* Harness Specific Include Files. */

/*==================================================================================================
                                            CONSTANTS
==================================================================================================*/

/*==================================================================================================
                                        DEFINES AND MACROS
==================================================================================================*/
#define MAX_THREAD_NUM    1000

#define DEFAULT_ITER_NUM  10

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
int     VT_spi_setup(void);
int     VT_spi_cleanup(void);
int     VT_spi_test(long arg);

#ifdef __cplusplus
}
#endif

#endif        /* _SPI_TEST_H_ */
