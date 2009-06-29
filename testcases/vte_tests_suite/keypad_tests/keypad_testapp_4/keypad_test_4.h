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
        @file   keypad_test_4.h

        @brief  keypad test 4  header file
====================================================================================================
Revision History:
                            Modification     Tracking
Author/core ID                  Date          Number    Description of Changes
-------------------------   ------------    ----------  -------------------------------------------
V.BECKER/rc023c              09/04/2004     TLSbo38735  Initial version
V.BECKER/rc023c              15/04/2004     TLSbo38735  Correction after code inspection
V.BECKER/rc023c              25/05/2004     TLSbo38735  Change file name
D.SIMAKOV/smkd001c           31/05/2004     TLSbo39737  The configuration of row and column number
                                                        through test arguments is added
L.DELASPRE/rc149c            16/08/2004     TLSbo40891  VTE 1.4 integration
C.Gagneraud/cgag1c           08/11/2004     TLSbo44474  Fix #include issues.
A.Ozerov/NONE                10/01/2006     TLSbo61037  Update in accordance with linux-2.6.10-rel-L26_1_15
I.Inkina/nknl001             10/01/2006     TLSbo61037  Update in accordance with linux-2.6.10-rel-L26_1_15
A.Ozerov/B00320              15/02/2006     TLSbo61037  Device was changed and testapp was reworked accordingly

==================================================================================================*/
#ifndef KEYPAD_TEST4_H
#define KEYPAD_TEST4_H

#ifdef __cplusplus
extern "C"{
#endif

/*==================================================================================================
                                        INCLUDE FILES
==================================================================================================*/
#include <stdbool.h>
#include <errno.h>
#include <fcntl.h>

/*==================================================================================================
                                            CONSTANTS
==================================================================================================*/

#define CONFIG_MXC30030         1
#define CONFIG_MX27             2
#define CONFIG_PLATFORM         3

/*==================================================================================================
                                        DEFINES AND MACROS
==================================================================================================*/
#define KEYPAD_DRIVER_EVENT "/dev/input/event0"

#if !defined(TRUE) && !defined(FALSE)
#define TRUE  1
#define FALSE 0
#endif

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
int VT_keypad_test4_setup(void);
int VT_keypad_test4_cleanup(void);
int VT_keypad_test4(int tcase);

#ifdef __cplusplus
}
#endif

#endif        /* KEYPAD_TEST1_H */
