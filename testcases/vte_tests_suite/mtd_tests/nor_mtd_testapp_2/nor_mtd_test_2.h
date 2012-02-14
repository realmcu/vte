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
    @file   nor_mtd_test_2.h

    @brief  NOR MTD test header file
====================================================================================================
Revision History:
                            Modification     Tracking
Author (core ID)                Date          Number    Description of Changes
-------------------------   ------------    ----------  -------------------------------------------
V.Becker/rc023c              04/05/2004     TLSbo39142   Initial version
V.Becker/rc023c              18/06/2004     TLSbo39142   Code reviewed 
L.Delaspre/rc149c            03/08/2004     TLSbo40891   VTE 1.4 integration
V.Becker/rc023c              20/10/2004     TLSbo43924   Update after NOR Flash map change
L.Delaspre/rc149c            08/11/2004     TLSbo44472   Resolve compilation issue
C.GAGNERAUD/cgag1c           09/11/2004     TLSbo44474   Warnings fixup.
==================================================================================================*/

#ifndef NOR_MTD_TEST_2_H
#define NOR_MTD_TEST_2_H

#ifdef __cplusplus
extern "C"{
#endif

/*==================================================================================================
                                         INCLUDE FILES
==================================================================================================*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/ioctl.h>
#include <mtd/mtd-user.h>

/*==================================================================================================
                                           CONSTANTS
==================================================================================================*/


/*==================================================================================================
                                       DEFINES AND MACROS
==================================================================================================*/
#define NOR_MTD_DRIVER     "/dev/mtd/3"
#define ERASE_BLOCK_SIZE    0x40000
#define OFFSET1                   0x80
#define READ_SIZE                0x200
#define BUFFER_SIZE             0x200

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
int VT_nor_mtd_test2_setup();
int VT_nor_mtd_test2_cleanup();

int VT_nor_mtd_test2(unsigned long);


#ifdef __cplusplus
}
#endif

#endif  /* NOR_MTD_TEST_2_H */
