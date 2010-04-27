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
    @file   dio_bio_test.h

    @brief  GPIO dio_bio test scenario C header.
====================================================================================================
Revision History:
                            Modification     Tracking
Author (core ID)                Date          Number    Description of Changes
-------------------------   ------------    ----------  -------------------------------------------
Igor Semenchukov             09/06/2004     TLSbo39741   Initial version 

==================================================================================================*/

#ifndef dio_bio_TEST_H
#define dio_bio_TEST_H

/*==================================================================================================
                                         INCLUDE FILES
==================================================================================================*/


/*==================================================================================================
                                           CONSTANTS
==================================================================================================*/


/*==================================================================================================
                                       DEFINES AND MACROS
==================================================================================================*/
#define	BUFSIZE	     4096
#define LEN          30
#define READ_DIRECT  1
#define WRITE_DIRECT 2
#define RDWR_DIRECT  3

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
int VT_dio_bio_setup();
int VT_dio_bio_cleanup();

int VT_dio_bio_test(int argc, char** argv);

int main(int argc, char** argv);

void prg_usage();
void fillbuf(char *buf, int count, char value);
int bufcmp(char *buf1, char *buf2, int bsize);
int runtest(int fd_r, int fd_w, int iter, off64_t offset, int action);

#endif  /* dio_bio_TEST_H */
