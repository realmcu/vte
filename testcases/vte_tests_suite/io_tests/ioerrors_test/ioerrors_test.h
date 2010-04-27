/***
**Copyright (C) 2005-2009 Freescale Semiconductor, Inc. All Rights Reserved.
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
    @file   ioerrors_test.h

    @brief  GPIO ioerrors test scenario C header.
====================================================================================================
Revision History:
                            Modification     Tracking
Author (core ID)                Date          Number    Description of Changes
-------------------------   ------------    ----------  -------------------------------------------
Igor Semenchukov             10/06/2004     TLSbo39741   Initial version 

==================================================================================================*/

#ifndef IOERRORS_TEST_H
#define IOERRORS_TEST_H

/*==================================================================================================
                                         INCLUDE FILES
==================================================================================================*/


/*==================================================================================================
                                           CONSTANTS
==================================================================================================*/


/*==================================================================================================
                                       DEFINES AND MACROS
==================================================================================================*/


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
int VT_ioerrors_setup();
int VT_ioerrors_cleanup();

int VT_ioerrors_test(int argc, char** argv);

int main(int argc, char** argv);
void fillbuf(char *buf, int count, char value);
int runtest_f(int fd, char *buf, int offset, int count, int errnum, int testnum, char *msg);
int runtest_s(int fd, char *buf, int offset, int count, int testnum, char *msg);

#endif  /* IOERRORS_TEST_H */
