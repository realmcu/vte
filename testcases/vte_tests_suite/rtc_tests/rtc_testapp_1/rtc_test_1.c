/*================================================================================================*/
/**
    @file   rtc_test_1.c

    @brief  Update RTC interrupts
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
V. BECKER / rc023c           01/04/2004     TLSbo38652   Initial version 
V. BECKER / rc023c           25/05/2004     TLSbo38652   Change file name 
L.Delaspre/rc149c            13/04/2005     TLSbo48760   VTE 1.9 integration 
====================================================================================================
Portability:  ARM GCC  gnu compiler
==================================================================================================*/
#ifdef __cplusplus
extern "C"{
#endif

/*==================================================================================================
                                        INCLUDE FILES
==================================================================================================*/
/* Standard Include Files */
#include <errno.h>
    
/* Harness Specific Include Files. */
#include "test.h"

/* Verification Test Environment Include Files */
#include "rtc_test_1.h"

/*==================================================================================================
                                        LOCAL MACROS
==================================================================================================*/


/*==================================================================================================
                          LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
==================================================================================================*/


/*==================================================================================================
                                       LOCAL CONSTANTS
==================================================================================================*/


/*==================================================================================================
                                       LOCAL VARIABLES
==================================================================================================*/
int file_desc = 0;

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
/*===== VT_rtc_test1_setup =====*/
/**
@brief  assumes the pre-condition of the test case execution

@param  None
  
@return On success - return TPASS
        On failure - return the error code
*/
/*================================================================================================*/
int VT_rtc_test1_setup(void)
{
        int rv = TFAIL;
    
        /* Open RTC driver file descriptor */
        file_desc = open (RTC_DRIVER_NAME, O_RDONLY);
        /* Open returns -1 in case of failure */
        if (file_desc == -1)
        {
                tst_resm(TFAIL, "Open RTC driver fails: %s \n", strerror(errno));
        }
        else
        {
                rv = TPASS;
        }
  
        return rv;
}


/*================================================================================================*/
/*===== VT_rtc_test1_cleanup =====*/
/**
@brief  assumes the post-condition of the test case execution

@param  None
  
@return On success - return TPASS
        On failure - return the error code
*/
/*================================================================================================*/
int VT_rtc_test1_cleanup(void)
{
        int rv = TFAIL;
        int ret = 0;

        /* close RTC driver file descriptor */
        ret = close(file_desc);

        /* Close returns -1 in case of failure */
        if (ret == -1)
        {
                tst_resm(TFAIL, "ERROR : Close RTC driver fails: %s \n", strerror(errno));
        }
        else
        {
                rv = TPASS;
        }

        return rv;
}


/*================================================================================================*/
/*===== VT_rtc_test1 =====*/
/**
@brief  - Turn on RTC interrupts
        - Update interrupts by reading from device
        - Use read to interrupt select function
        - Turn off interrupts		

@param  None
  
@return On success - return TPASS
        On failure - return the error code
*/
/*================================================================================================*/
int VT_rtc_test1(void)
{
        int rv = TFAIL;
        int i = 0;
        int retval = 0;
        int irqcount = 0;
        unsigned long data = 0;

        tst_resm(TINFO, "RTC Driver Test 1: Update RTC interrupts");

        /* Turn on update interrupts (one per second) */
        tst_resm(TINFO, "Turn on update of interrupts...");
        retval = ioctl(file_desc, RTC_UIE_ON, 0);
        if (retval < 0)
        {
                tst_resm(TFAIL, "    ioctl fails for RTC_UIE_ON : %s \n", strerror(errno));
                return rv;
        }

        tst_resm(TINFO, "Counting 5 update (1/sec) interrupts from reading /dev/rtc0...");

        for( i = 1; i <6; i++)
        {
                /* This read will block */
                retval = read(file_desc, &data, sizeof(unsigned long));
                if (retval < 0)
                {
                        tst_resm(TFAIL, "    read fails : %s \n", strerror(errno));
                        return rv;
                }

                tst_resm(TINFO, " %d",i);
                irqcount++;
        }

        tst_resm(TINFO, "Again, from using select(2) on /dev/rtc0...");

        for (i=1; i<6; i++)
        {
                /* 5 second timeout on select */
                struct timeval tv = {5, 0};
                fd_set readfds;

                FD_ZERO(&readfds);
                FD_SET(file_desc, &readfds);

                /* The select will wait until an RTC interrupt happens. */
                retval = select(file_desc+1, &readfds, NULL, NULL, &tv);
                if (retval <0)
                {
                        tst_resm(TFAIL, "    select fails: %s \n", strerror(errno));
                        return rv;
                }

                /* This read won't block unlike the select-less case above. */
                retval = read(file_desc, &data, sizeof(unsigned long));
                if (retval <0)
                {
                        tst_resm(TFAIL, "    read fails: %s \n", strerror(errno));
                        return rv;
                }
                tst_resm(TINFO, " %d",i);
                irqcount++;
        }

        /* Turn off update interrupts */
        tst_resm(TINFO, "Turn off update of interrupts...");
        retval = ioctl(file_desc, RTC_UIE_OFF, 0);
        if (retval <0)
        {
                tst_resm(TFAIL, "    ioctl fails for RTC_UIE_OFF: %s \n", strerror(errno));
                return rv;
        }

        rv = TPASS;
        return rv;
}

#ifdef __cplusplus
}
#endif
