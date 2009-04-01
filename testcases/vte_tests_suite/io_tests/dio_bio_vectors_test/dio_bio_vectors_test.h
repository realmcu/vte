/*================================================================================================*/
/**
    @file   dio_bio_vectors_test.h

    @brief  dio_bio_vectors test scenario C header.
*/
/*==================================================================================================

  Copyright (C) 2004, Freescale Semiconductor, Inc. All Rights Reserved
  THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
  BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
  Freescale Semiconductor, Inc.
     
====================================================================================================
Revision History:
                            Modification     Tracking
Author (core ID)                Date          Number    Description of Changes
-------------------------   ------------    ----------  -------------------------------------------
Igor Semenchukov             09/06/2004     TLSbo39741   Initial version 

==================================================================================================*/

#ifndef dio_bio_vectors_test_H
#define dio_bio_vectors_test_H

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
struct iovec;

/*==================================================================================================
                                 GLOBAL VARIABLE DECLARATIONS
==================================================================================================*/


/*==================================================================================================
                                     FUNCTION PROTOTYPES
==================================================================================================*/
int VT_dio_bio_vectors_setup();
int VT_dio_bio_vectors_cleanup();
int VT_dio_bio_vectors_test(int argc, char** argv);

int _main(int argc, char** argv);

void prg_usage();
void fillbuf(char *buf, int count, char value);
void vfillbuf(struct iovec *iv, int vcnt, char value);
int bufcmp(char *buf1, char *buf2, int bsize);
int vbufcmp(struct iovec *iv1, struct iovec *iv2, int vcnt);
int forkchldrn(int **pidlst, int numchld, int action, int (*chldfunc)());
int waitchldrn(int **pidlst, int numchld);
int killchldrn(int **pidlst, int numchld, int sig);
int runtest(int fd_r, int fd_w, int childnum, int action);
int child_function(int childnum, int action);

#endif  /* dio_bio_vectors_test_H */
