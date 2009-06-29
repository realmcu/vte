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
    @file   spi_test_1.h

    @brief  Test scenario C header template.

====================================================================================================
Revision History:
                            Modification     Tracking
Author (core ID)                Date          Number    Description of Changes
-------------------------   ------------    ----------  -------------------------------------------
Tony THOMASSIN/RB595C        18/05/2004     TLSbo39490  SPI test development 
Irina Inkina/nknl001         19/05/2005     Tlsbo52848  Code improved  
Irina Inkina/nknl001         15/08/2005     Tlsbo53514   Number of the iterations were added 

==================================================================================================*/

#ifndef SPI_TEST_1_H
#define SPI_TEST_1_H

#ifdef __cplusplus
extern "C"{
#endif

/*==================================================================================================
                                         INCLUDE FILES
==================================================================================================*/
#include <spi.h>

/*==================================================================================================
                                           CONSTANTS
==================================================================================================*/


/*==================================================================================================
                                       DEFINES AND MACROS
==================================================================================================*/
#define ITERATION         64

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
int VT_spi_setup(int mod);
int VT_spi_cleanup(void);
int VT_spi_test_1(int mod,  int card, int  iteration);



#ifdef __cplusplus
}
#endif

#endif  // SPI_TEST_1_H //
