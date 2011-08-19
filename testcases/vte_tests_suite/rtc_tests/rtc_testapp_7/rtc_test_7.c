/***
 * **Copyright 2008-2010 Freescale Semiconductor, Inc. All Rights Reserved.
 * **
 * **The code contained herein is licensed under the GNU General Public
 * **License. You may obtain a copy of the GNU General Public License
 * **Version 2 or later at the following locations:
 * **
 * **http://www.opensource.org/licenses/gpl-license.html
 * **http://www.gnu.org/copyleft/gpl.html
 * **/
/*===========================================================================
        @file   rtc_test_7.c

        @brief  RTC wakeup test
=============================================================================
Revision History:
                       Modification     Tracking
Author                     Date          Number    Description of Changes
--------------------   ------------    ----------  -----------------------
Blake                   12/29/2008                   Initial version
Spring Zhang            01/18/2010        n/a    Delete unnecessary RTC off code
===============================================================================
Portability:  ARM GCC
=============================================================================*/

#ifdef __cplusplus
extern "C"{
#endif

/*=============================================================================
                              INCLUDE FILES
=============================================================================*/
/* Standard Include Files */
#include <errno.h>
    
/* Harness Specific Include Files. */
#include "test.h"

/* Verification Test Environment Include Files */
#include "rtc_test_7.h"

/*=============================================================================
                               LOCAL MACROS
=============================================================================*/
#if !defined(TRUE) && !defined(FALSE)
        #define TRUE  1
        #define FALSE 0
#endif  
#define PRECISION 2

/*=============================================================================
                          LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
=============================================================================*/
enum RTC_STATE
{
        RTC_TIME = 0,     /* Current time */	
        RTC_ALARM_TIME,   /* Current time of alarm */
        RTC_WKALARM_TIME, /* Current time of wake up alarm */
        RTC_TOTAL_STATES  /* Total count of the states */
};

/*=============================================================================
                                       LOCAL CONSTANTS
=============================================================================*/


/*=============================================================================
                                       LOCAL VARIABLES
=============================================================================*/
int               file_desc = -1; 
struct rtc_time   saved_time;    /* Saved time value */	
struct rtc_time   saved_alarm;   /* Saved alarms time value */
struct rtc_wkalrm saved_wkalarm; /* Saved wake up alarms time value */
int               saved_states[RTC_TOTAL_STATES] = {0}; /* Flags */
int               is_ok = 1;			

/*=============================================================================
                                       GLOBAL CONSTANTS
=============================================================================*/


/*=============================================================================
                                       GLOBAL VARIABLES
=============================================================================*/


/*=============================================================================
                                   LOCAL FUNCTION PROTOTYPES
=============================================================================*/
int ask_user(char *question);

/*===== ask_user =====*/
/**
@brief  Asks user to answer the question

@param  Input:  char * question
        Output: None
  
@return TPASS - if user asks "Y", yes
        TFAIL - if user asks "N", no
*/
int ask_user(char *question)
{
        unsigned char answer;
        int           ret = TRETR;

        do
        {
                tst_resm(TINFO, "%s [Y/N]", question);
                answer = fgetc(stdin);
                if (answer == 'Y' || answer == 'y')
                        ret = TPASS;
                else if (answer == 'N' || answer == 'n')
                ret = TFAIL;
        }
        while (ret == TRETR);

        /* Wipe CR character from stream */
        fgetc(stdin);      
        return ret;
}


/*===== VT_rtc_test6_setup =====*/
/**
@brief  assumes the pre-condition of the test case execution

@param  None
  
@return On success - return TPASS
        On failure - return the error code
*/
int VT_rtc_test7_setup(void)
{
        int rv = TFAIL;
        int i = 0;
        extern char* RTC_DRIVER_NAME[RTC_DEVICE_NUM];
        extern char* pdevice;

        if (!pdevice)
            do { 
                file_desc = open(RTC_DRIVER_NAME[i], O_RDONLY);
            } while (file_desc <= 0 && i++<RTC_DEVICE_NUM);
        else file_desc = open(pdevice, O_RDONLY);
        if (file_desc ==  -1)
        {
            tst_brkm(TBROK, cleanup, "ERROR : Open RTC driver fails");
            perror("cannot open RTC device");
        }
        else
        {
            tst_resm(TINFO, "Open RTC device successfully: %s \n",
                pdevice? pdevice: RTC_DRIVER_NAME[i]);
            rv = TPASS;
        }
    
        return rv;
}


/*===== VT_rtc_test7_cleanup =====*/
/**
@brief  assumes the post-condition of the test case execution

@param  None
  
@return On success - return TPASS
        On failure - return the error code
*/
int VT_rtc_test7_cleanup(void)
{
        int rv = TFAIL;
        int ret = 0;

        /* close RTC driver file descriptor */
        ret = close(file_desc);

        /* Close returns -1 in case of failure */
        if (ret == -1)
        {
                tst_resm(TWARN, "ERROR : Close RTC driver fails");
                perror("cannot close RTC device");
        }
        else
        {
                rv = TPASS;
        }

        return rv;
}


/*===== VT_rtc_test7 =====*/
/**
@brief  - Read and set time and date
        - Set alarm and read alarm settings
        - Enable alarm. Wait for alarm interrupt. Disable alarm

@param  int seconds - alarm time in second
  
@return On success - return TPASS
        On failure - return the error code
*/
int VT_rtc_alarm_on(int seconds)
{
        int rv = TFAIL;
        int retval;

        struct rtc_time rtc_tm = {0,0,0,0,0,0};

        tst_resm( TINFO, "RTC Driver Test 7" );
        tst_resm( TINFO, "RTC alarm" );



        /***************************************/
        /* Test for alarm                      */
        /***************************************/
        tst_resm( TINFO, "ALARM TEST: RTC_ALM_SET & RTC_ALM_READ" );
        /* Read the RTC time/date */
        tst_resm( TINFO, "  Read date/time..." );
        retval = ioctl( file_desc, RTC_RD_TIME, &rtc_tm );
        if( retval < 0 )
        {
                tst_resm(TFAIL, "    ioctl RTC_RD_TIME fails: %s \n", strerror(errno));
                return TFAIL;
        }

        tst_resm( TINFO, "    Current RTC date/time is %d-%d-%d, %02d:%02d:%02d.",
                rtc_tm.tm_mday, rtc_tm.tm_mon+1, rtc_tm.tm_year+1900,
                rtc_tm.tm_hour, rtc_tm.tm_min, rtc_tm.tm_sec );

        /* Set the alarm to 5 sec in the future, and check for rollover */
        tst_resm(TINFO, "  Set the alarm to %d seconds in the future..." ,seconds);
        rtc_tm.tm_sec += seconds;
        if (rtc_tm.tm_sec >= 60)
        {
                rtc_tm.tm_min += rtc_tm.tm_sec/60;
                rtc_tm.tm_sec %= 60;
        }
        if  (rtc_tm.tm_min >= 60)
        {
                rtc_tm.tm_hour += rtc_tm.tm_min/60;
                rtc_tm.tm_min %= 60;
        }
        if  (rtc_tm.tm_hour >= 24)
        {
                tst_brkm( TRETR, cleanup, "    Run test issue. Restart it." );
        }

        retval = ioctl( file_desc, RTC_ALM_SET, &rtc_tm );
        if( retval < 0 )
        {
                tst_resm(TFAIL, "    ioctl RTC_ALM_SET fails: %s \n", strerror(errno));
                is_ok = 0;
        }

        /* Enable alarm interrupts */
        tst_resm(TINFO, "    Enable alarm interrupts" );
        retval = ioctl( file_desc, RTC_AIE_ON, 0 );
        if( retval < 0 )
        {
                tst_resm(TFAIL, "    ioctl RTC_AIE_ON fails: %s \n", strerror(errno));
                return TFAIL;
        }

        tst_resm(TINFO, "  Waiting %d seconds for alarm......." ,seconds);
         
    
        /******** end of test for alarm *********/

       
        /***************************************/
        /* Final test result                   */
        /***************************************/
        
        rv = is_ok!=0?TPASS:TFAIL;
        return rv;
}

int  VT_rtc_alarm_off(void)
{
    int retval;

    /* Disable alarm interrupts */
    tst_resm(TINFO, "  Disable alarm interrupts" );
    retval = ioctl( file_desc, RTC_AIE_OFF, 0 );
    if( retval < 0 )
    {
        tst_resm(TFAIL, "    ioctl RTC_AIE_OFF fails: %s \n", strerror(errno));
        return TFAIL;
    }
    return TPASS;
}

#ifdef __cplusplus
}
#endif
