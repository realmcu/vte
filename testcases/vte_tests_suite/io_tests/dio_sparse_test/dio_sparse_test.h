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
        @file   dio_sparse_test.h

        @brief  GPIO dio_sparse test header file.
====================================================================================================
Revision History:
                            Modification     Tracking
Author                          Date          Number    Description of Changes
-------------------------   ------------    ----------  -------------------------------------------
Igor Semenchukov             10/06/2004     TLSbo39741  Initial version
A.Ozerov/b00320              11/12/2006     TLSbo84161  Minor changes.

==================================================================================================*/

#ifndef dio_sparse_test_H
#define dio_sparse_test_H

/*==================================================================================================
                                        INCLUDE FILES
==================================================================================================*/


/*==================================================================================================
                                            CONSTANTS
==================================================================================================*/


/*==================================================================================================
                                        DEFINES AND MACROS
==================================================================================================*/
#define NUM_CHILDREN 8

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
int     VT_dio_sparse_setup(void);
int     VT_dio_sparse_cleanup(void);

int     VT_dio_sparse_test(int argc, char **argv);

int     main(int argc, char **argv);
int     usage(void);
long long scale_by_kmg(long long value, char scale);
int     dirty_freeblocks(int size);
int     dio_sparse(char *filename, int align, int writesize, int filesize);
int     read_sparse(char *filename, int filesize);
void    sig_term_func(int i, siginfo_t * si, void *p);
unsigned char *check_zero(unsigned char *buf, int size);

#endif                          /* dio_sparse_test_H */
