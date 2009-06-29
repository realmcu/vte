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
    @file   spi_test_2.h

    @brief  Test scenario C header template.
     
====================================================================================================
Revision History:
                            Modification     Tracking
Author (core ID)                Date          Number    Description of Changes
-------------------------   ------------    ----------  -------------------------------------------
Tony THOMASSIN/RB595C        18/05/2004     TLSbo39490  SPI test development 
I.Inkina/nknl001                      24/08/2005     TLsbo53757  SPI test improved
==================================================================================================*/

#ifndef SPI_TEST_0_H
#define SPI_TEST_0_H

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


struct s_list
{
        char  *adress;
        char  *value;
        int count;
        struct s_list *next;
};


/*==================================================================================================
                                 GLOBAL VARIABLE DECLARATIONS
==================================================================================================*/

/*==================================================================================================
                                     FUNCTION PROTOTYPES
==================================================================================================*/
int VT_spi_setup(int mod);
int VT_spi_cleanup(void);
int VT_spi_test_1(int mod, int card, struct s_list *root);

unsigned int read_mc13783_register (char address);
int write_mc13783_register(char address,unsigned int value);
int mc13783_commands(int reg_number,unsigned int reg_data);



#ifdef __cplusplus
}
#endif

#endif  // SPI_TEST_0_H //
