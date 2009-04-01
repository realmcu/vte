/*================================================================================================*/
/**
        @file   dio_sparse_test.h

        @brief  GPIO dio_sparse test header file.
*/
/*==================================================================================================

        Copyright (C) 2006, Freescale Semiconductor, Inc. All Rights Reserved
        THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
        BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
        Freescale Semiconductor, Inc.

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
