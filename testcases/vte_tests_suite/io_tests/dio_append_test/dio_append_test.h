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
        @file   dio_append_test.h

        @brief  GPIO dio_append test header file.
====================================================================================================
Revision History:
                            Modification     Tracking
Author/core ID                Date          Number    Description of Changes
-------------------------   ------------    ----------  -------------------------------------------
Igor Semenchukov             10/06/2004     TLSbo39741  Initial version 
A.Ozerov/b00320              11/12/2006     TLSbo84161  Minor changes.

==================================================================================================*/

#ifndef DIO_APPEND_TEST_H
#define DIO_APPEND_TEST_H

/*==================================================================================================
                                        INCLUDE FILES
==================================================================================================*/


/*==================================================================================================
                                            CONSTANTS
==================================================================================================*/


/*==================================================================================================
                                        DEFINES AND MACROS
==================================================================================================*/
#define BUFSIZE 4096
#define NUM_CHILDREN 8
#define MEMSIZE 65536

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
int     VT_dio_append_setup(void);
int     VT_dio_append_cleanup(void);

int     VT_dio_append_test(int argc, char **argv);

int     main(int argc, char **argv);
int     read_eof(char *filename, int childnum);
unsigned char *check_zero(unsigned char *buf, int size);
int     append_to_file(char *filename);

#endif                          /* DIO_APPEND_TEST_H */
