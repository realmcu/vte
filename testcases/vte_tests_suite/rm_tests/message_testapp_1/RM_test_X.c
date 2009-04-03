/*================================================================================================*/
/**
    @file   RM_test_X.c

    @brief  Test scenario C source template.
*/
/*==================================================================================================

  Copyright (C) 2004, Freescale Semiconductor, Inc. All Rights Reserved
  THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
  BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
  Freescale Semiconductor, Inc.
     
====================================================================================================
Revision History:
                            Modification     Tracking
Author                          Date          Number    Description of Changes
-------------------------   ------------    ----------  -------------------------------------------
Dmitriy Kazachkov           10/06/2004     TLSbo39741   initial version 
====================================================================================================
Portability: Indicate if this module is portable to other compilers or platforms. 
             If not, indicate specific reasons why is it not portable.

==================================================================================================*/


/*==================================================================================================
                                        INCLUDE FILES
==================================================================================================*/
/* Standard Include Files */
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/ipc.h>
#ifdef _LINUX_
// defines struct msgbuf
#define __USE_GNU
#endif
#include <sys/msg.h>
#include <sys/types.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C"{
#endif
    
/* Harness Specific Include Files. */
#include "test.h"
#ifdef __cplusplus
}

#endif

/* Verification Test Environment Include Files */
#include "RM_test_X.h"

/*==================================================================================================
                                        LOCAL MACROS
==================================================================================================*/

#define MAX_MSGS		8192
/*==================================================================================================
                          LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
==================================================================================================*/


/*==================================================================================================
                                       LOCAL CONSTANTS
==================================================================================================*/


/*==================================================================================================
                                       LOCAL VARIABLES
==================================================================================================*/


/*==================================================================================================
                                       GLOBAL CONSTANTS
==================================================================================================*/


/*==================================================================================================
                                       GLOBAL VARIABLES
==================================================================================================*/


/*==================================================================================================
                                   LOCAL FUNCTION PROTOTYPES
==================================================================================================*/


/*==================================================================================================
                                       LOCAL FUNCTIONS
==================================================================================================*/


/*================================================================================================*/
/*===== VT_RM_setup =====*/
/**
@brief  assumes the pre-condition of the test case execution

@param  None
  
@return On success - return TPASS
        On failure - return the error code
*/
/*================================================================================================*/
int VT_RM_setup(void)
{
    int rv = TPASS;
    
    /** insert your code here */
    
    return rv;
}


/*================================================================================================*/
/*===== VT_cleanup =====*/
/**
@brief  assumes the post-condition of the test case execution

@param  None
  
@return On success - return TPASS
        On failure - return the error code
*/
/*================================================================================================*/
int VT_RM_cleanup(void)
{
    int rv = TPASS;
    
    /** insert your code here */
    
    return rv;
}


/*================================================================================================*/
/*===== VT_RM_test_X =====*/
/**
@brief  Template test scenario X function

@param  None
  
@return On success - return TPASS
        On failure - return the error code
*/
/*================================================================================================*/
int VT_RM_test_X(int verbose)
{
    int rv = TFAIL;

	struct msqid_ds info;	/* Message queue info */
	struct msgbuf buf;	/* Message queue buffer */
	int	msqid;		/* Message queue identifier */
    int     mode = 0777;    /* Default mode bits */
	size_t 	max_bytes;	/* Num bytes sent to message queue */
	size_t 	msg_size;	/* Num bytes sent to message queue */
	unsigned long	bytes_sent;	/* Num bytes sent to message queue */
	
        /* 
         * Obtain a unique message queue identifier using msgget() 
         */
        if ((msqid = msgget (IPC_PRIVATE, IPC_CREAT|mode)) < 0)
        {
                tst_resm(TFAIL, "ERROR :msgget failed");
                	      return(rv);
	}    

        if (verbose)
                printf ("\tCreated message queue: %d\n\n", msqid);

	/*
	 * Determine message queue limits
	 *
	 * Determine the maximum number of bytes that the message
	 * queue will hold.  Then determine the message size 
	 * (Max num of bytes per queue / maximum num of messages per queue)
	 */
	if (msgctl (msqid, IPC_STAT, &info) < 0)
	{
	      tst_resm(TFAIL, "ERROR : msgctl (IPC_STAT) failed");
	      return(rv);
	}

	max_bytes = info.msg_qbytes;

	msg_size  = (size_t)((max_bytes + MAX_MSGS - 1) / MAX_MSGS);

	if (verbose) 
	{
		printf ("\tMax num of bytes per queue:  %ld\n", (long)max_bytes);
		printf ("\tMax messages per queue:      %d\n",  MAX_MSGS);
		printf ("\tCorresponding message size:  %ld\n\n", (long)msg_size);
	}
	

	/*
	 * Fill up the message queue
	 * 
	 * Send bytes to the message queue until it fills up
	 */

	buf.mtype = 1L;

	bytes_sent = 0;
	while (bytes_sent < max_bytes - msg_size) 
	{
		if (msgsnd (msqid, &buf, msg_size, 0) < 0)
		{
		      tst_resm(TFAIL, "ERROR : msgsnd failed");
		      return(rv);
		}
		bytes_sent += msg_size;
		if (verbose) 
		{
			printf ("\r\tBytes sent: %ld", (long)bytes_sent);
			fflush(stdout);
		}
	}
	if (verbose) puts ("\n");


	/*
	 * Remove the message queue
	 */
	if (msgctl (msqid, IPC_RMID, 0) < 0)
	{
	      tst_resm(TFAIL, "ERROR : msgctl (IPC_RMID) failed");
	      return(rv);
	}
	
	if (verbose)
		printf ("\n\tRemoved message queue: %d\n", msqid);

	/* Program completed successfully -- exit */
	
	rv = TPASS;
    
    return rv;
}

