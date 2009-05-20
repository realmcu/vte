/*================================================================================================*/
/**
    @file   multicast_test.h

    @brief  Network multicast test header file
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
V. ZAVJALOV/----	        10/06/2004      TLSbo39738     Initial version

==================================================================================================*/

#ifndef multicast_test_H
#define multicast_test_H

#ifdef __cplusplus
extern "C"{
#endif

/*==================================================================================================
                                         INCLUDE FILES
==================================================================================================*/
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <errno.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/utsname.h>
#include <string.h>
#include <termios.h>

/*==================================================================================================
                                           CONSTANTS
==================================================================================================*/


/*==================================================================================================
                                       DEFINES AND MACROS
==================================================================================================*/
#define SLEEP_TIME  20000  /* Wait for character with timeout 25ms */
#define MAX_LENGTH_MESSAGE 1024

#define TRUE  1
#define FALSE 0

/*==================================================================================================
                                             ENUMS
==================================================================================================*/


/*==================================================================================================
                                 STRUCTURES AND OTHER TYPEDEFS
==================================================================================================*/

typedef unsigned char BOOLEAN;

/*==================================================================================================
                                 GLOBAL VARIABLE DECLARATIONS
==================================================================================================*/

/*==================================================================================================
                                     FUNCTION PROTOTYPES
==================================================================================================*/
void set_terminal(void);
void restore_terminal(void);
BOOLEAN kbhit(int *pnSleepTime);

int VT_multicast_test_setup();
int VT_multicast_test_cleanup();

int VT_multicast_test(int argc, char **argv);


#ifdef __cplusplus
}
#endif

#endif  /* multicast_test_H */
