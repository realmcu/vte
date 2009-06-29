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
        @file   cdrom_driver_testapp.h

        @brief  Header file for  CDROM driver test.

==================================================================================================*/
#ifndef CDROM_DRIVER_TESTAPP_H
#define CDROM_DRIVER_TESTAPP_H

#ifdef __cplusplus
extern "C"{ 
#endif

/*==================================================================================================
                                        INCLUDE FILES
==================================================================================================*/
#include <linux/types.h>

/*==================================================================================================
                                        DEFINES AND MACROS
==================================================================================================*/

/*==================================================================================================
                                    STRUCTURES AND TYPEDEFS
==================================================================================================*/




/*==================================================================================================
                                    GLOBAL VARIABLE DECLARATIONS
==================================================================================================*/

/*==================================================================================================
                                        FUNCTION PROTOTYPES
==================================================================================================*/


int VT_cdrom_driver_test_setup(void);
int VT_cdrom_driver_test_cleanup(void);
int VT_cdrom_driver_test(char *devname, int testcase, int volume);


#ifdef __cplusplus
} 
#endif

#endif /* CDROM_DRIVER_TESTAPP_H */
