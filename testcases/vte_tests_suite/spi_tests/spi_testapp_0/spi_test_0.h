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
        @file   spi_test_0.h

        @brief  Test scenario C header template.
====================================================================================================
Revision History:
                            Modification     Tracking
Author/core ID                  Date          Number    Description of Changes
-------------------------   ------------    ----------  -------------------------------------------
Tony THOMASSIN/RB595C        18/05/2004     TLSbo39490  SPI test development 
V.Khalabuda/b00306           17/04/2006     TLSbo67010  Update version for linux-2.6.10-rel-L26_1_17
D.Khoroshev/b00313           02/01/2006     TLSbo86657  Adaptation to new spi interface

==================================================================================================*/
#ifndef SPI_TEST_0_H
#define SPI_TEST_0_H

#ifdef __cplusplus
extern "C"{
#endif

/*==================================================================================================
                                        INCLUDE FILES
==================================================================================================*/
/* Standard Include Files */
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <ctype.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <pthread.h>

/* For bool type definition. hardware.h as alternative. */

/* Driver specific functions */
#include "spi_test_module.h"

/* Harness Specific Include Files. */
#include <test.h>
#include <usctest.h>

/*==================================================================================================
                                            CONSTANTS
==================================================================================================*/
#if !defined(TRUE) && !defined(FALSE)
#define TRUE  1
#define FALSE 0
#endif

//#define BUFF_TEST_MAX_SIZE 32

/*==================================================================================================
                                        DEFINES AND MACROS
==================================================================================================*/

/*==================================================================================================
                                            ENUMS
==================================================================================================*/
/** TEMPLATE_EX type */
typedef enum 
{
        TEMPLATE_EX_0 = 0,   /**< Example stuff 0. */
        TEMPLATE_EX_1        /**< Example stuff 1. */
} TEMPLATE_EX_T;

/*==================================================================================================
                                STRUCTURES AND OTHER TYPEDEFS
==================================================================================================*/

/*==================================================================================================
                                GLOBAL VARIABLE DECLARATIONS
==================================================================================================*/

/*==================================================================================================
                                    FUNCTION PROTOTYPES
==================================================================================================*/
int VT_spi_setup(void);
int VT_spi_cleanup(void);
int VT_spi_buffer_test(int bytes, char* buffer);
int VT_spi_multiclient_test(void);
int VT_check_data_integrity(char* buf1, char* buf2, int count);

#ifdef __cplusplus
}
#endif

#endif        /* SPI_TEST_0_H */
