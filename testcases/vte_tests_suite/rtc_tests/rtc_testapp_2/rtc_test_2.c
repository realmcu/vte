/***
**Copyright (C) 2004-2011 Freescale Semiconductor, Inc. All Rights Reserved.
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
        @file   rtc_test_2.c

        @brief  RTC time and date test

====================================================================================================
Revision History:
                            Modification     Tracking
Author                          Date          Number    Description of Changes
-------------------------   ------------    ----------  -------------------------------------------
V. BECKER / rc023c           01/04/2004     TLSbo38652   Initial version 
V. BECKER / rc023c           25/05/2004     TLSbo38652   Change in file name
V. BECKER / rc023c           03/06/2004     TLSbo39891   Correct second value line 325
D. Simakov                   04/06/2004     TLSbo39743   Code improvement 
D. Simakov                   19/07/2004     TLSbo39743   An errors are correted
L. DELASPRE / rc149c         02/08/2004     TLSbo40891   VTE 1.4 integration
C. Gagneraud cgag1c          08/11/2004     TLSbo44474   warnings fixup.
L. DELASPRE / rc149c         08/12/2004     TLSbo40142   Update copyrights
L. DELASPRE / rc149c         14/12/2004     TLSbo45058   Update printed message
E.Gromazina				     08/06/2005	    TLSbo50973	 Update for automation test
S. V-Guilhou / svan01c       13/09/2005     TLSbo53745   Unsupported ioctls
A.Ozerov/b00320              11/12/2006     TLSbo84161   Minor changes.

====================================================================================================
Portability:  ARM GCC
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
#include "rtc_test_2.h"

/*==================================================================================================
                                        LOCAL MACROS
==================================================================================================*/
#if !defined(TRUE) && !defined(FALSE)
        #define TRUE  1
        #define FALSE 0
#endif  
#define PRECISION 2

/*==================================================================================================
                          LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
==================================================================================================*/
enum RTC_STATE
{
        RTC_TIME = 0,     /* Current time */	
        RTC_ALARM_TIME,   /* Current time of alarm */
        RTC_WKALARM_TIME, /* Current time of wake up alarm */
        RTC_TOTAL_STATES  /* Total count of the states */
};

/*==================================================================================================
                                       LOCAL CONSTANTS
==================================================================================================*/


/*==================================================================================================
                                       LOCAL VARIABLES
==================================================================================================*/
int               file_desc = -1; 
struct rtc_time   saved_time;    /* Saved time value */	
struct rtc_time   saved_alarm;   /* Saved alarms time value */
struct rtc_wkalrm saved_wkalarm; /* Saved wake up alarms time value */
int               saved_states[RTC_TOTAL_STATES] = {0}; /* Flags */
int               is_ok = 1;			

/*==================================================================================================
                                       GLOBAL CONSTANTS
==================================================================================================*/


/*==================================================================================================
                                       GLOBAL VARIABLES
==================================================================================================*/
extern char * pdevice;

/*==================================================================================================
                                   LOCAL FUNCTION PROTOTYPES
==================================================================================================*/
int save_rtc_state( enum RTC_STATE rtc_state );
int restore_rtc_state( enum RTC_STATE rtc_state );
int rtc_time_equal( const struct rtc_time * rtc_tm_1, const struct rtc_time * rtc_time_2, int alarm );
int ask_user(char *question);

/*==================================================================================================
                                       LOCAL FUNCTIONS
==================================================================================================*/

/*================================================================================================*/
/*===== save_rtc_state =====*/
/**
@brief  Saves the current rtc state

@param  rtc_state - state which is necessary for saving
  
@return On success - return 1 (TRUE)
        On failure - return 0 (FALSE)
*/
/*================================================================================================*/
int save_rtc_state( enum RTC_STATE rtc_state )
{
        int retval;

        if( file_desc == -1 )
        {
                fprintf( stderr, "device is not ready\n" );
                return 0;
        }

        switch( rtc_state )
        {
                case RTC_TIME:
                        retval = ioctl( file_desc, RTC_RD_TIME, &saved_time );
                        if( retval < 0 )
                        {
                                tst_resm(TFAIL, "    ioctl RTC_RD_TIME fails: %s \n", strerror(errno));	
                                return 0;
                        }
                        saved_states[RTC_TIME] = 1;
                        break;

                case RTC_ALARM_TIME:
                        retval = ioctl( file_desc, RTC_ALM_READ, &saved_alarm );
                        if( retval < 0 )
                        {	
                                tst_resm(TFAIL, "    ioctl RTC_ALM_READ fails: %s \n", strerror(errno));
                                return 0;
                        }
                        saved_states[RTC_ALARM_TIME] = 1;
                        break;

                case RTC_WKALARM_TIME:
                        retval = ioctl( file_desc, RTC_WKALM_RD, &saved_wkalarm );
                        if( retval < 0 )
                        {
                                tst_resm(TFAIL, "    ioctl RTC_WKALM_RD fails: %s \n", strerror(errno));
                                return 0;
                        }
                        saved_states[RTC_WKALARM_TIME] = 1;
                        break;

                default:
                        fprintf( stderr, "unknown rtc state\n" );
                        return 0;
                break;
        }

        return 1;
}

/*================================================================================================*/
/*===== restore_rtc_state =====*/
/**
@brief  Restores the saved rtc_state

@param  rtc_state - state which is necessary for restoring
  
@return On success - return 1 (TRUE)
        On failure - return 0 (FALSE)
*/
/*================================================================================================*/
int restore_rtc_state( enum RTC_STATE rtc_state )
{	
        int retval;

        if( file_desc == -1 )
        {
                fprintf( stderr, "device is not ready\n" );
                return 0;
        }

        switch( rtc_state )
        {
                case RTC_TIME:
                        if( saved_states[RTC_TIME] != 0 )
                        {
                                retval = ioctl( file_desc, RTC_SET_TIME, &saved_time );
                                if( retval < 0 )
                                {
                                        tst_resm(TFAIL, "    ioctl RTC_SET_TIME fails: %s \n", strerror(errno));
                                        return 0;
                                }
                                saved_states[RTC_TIME] = 0;
                        }
                        break;

                case RTC_ALARM_TIME:
                        if( saved_states[RTC_ALARM_TIME] != 0 )
                        {
                                retval = ioctl( file_desc, RTC_ALM_SET, &saved_alarm );
                                if( retval < 0 )
                                {
                                        tst_resm(TFAIL, "    ioctl RTC_ALM_READ fails: %s \n", strerror(errno));
                                        return 0;
                                }
                                saved_states[RTC_ALARM_TIME] = 0;
                        }
                        break;

                case RTC_WKALARM_TIME:
                        if( saved_states[RTC_WKALARM_TIME] != 0 )
                        {
                                retval = ioctl( file_desc, RTC_WKALM_SET, &saved_wkalarm );
                                if( retval < 0 )
                                {
                                        tst_resm(TFAIL, "    ioctl RTC_WKALM_RD fails: %s \n", strerror(errno));
                                        return 0;
                                }
                                saved_states[RTC_WKALARM_TIME] = 0;
                        }
                        break;

                default:
                        fprintf( stderr, "unknown rtc state\n" );
                        return 0;
                break;
        }

        return 1;
}

/*================================================================================================*/
/*===== rtc_time_equal =====*/
/**
@brief  Compares two rtc_time values on equality

@param  rtc_tm_1 - the 1st compared argument
@param  rtc_tm_2 - the 2nd compared argument
  
@return returns 1 (TRUE)  if both values are equal
        returns 0 (FALSE) if rtc_tm_1 and rtc_tm_2 are not equal
        returns 0 (FALSE) if even one of arguments is invalid 
*/
/*================================================================================================*/
int rtc_time_equal( const struct rtc_time * rtc_tm_1, const struct rtc_time * rtc_tm_2, int alarm )
{
        int eq = 0;

        if( (rtc_tm_1 == 0) || (rtc_tm_2 == 0 ) ) 
        {
                return FALSE;
        }

        //printf( "-----------------------------------\n" );
        //printf( "   RTC_SET_TIME   RTC_RD_TIME\n" );

        if((rtc_tm_2->tm_sec - rtc_tm_1->tm_sec) > PRECISION)
                eq += 1;
        /*	printf( "sec:\t%d,\t\t%d\n", rtc_tm_1->tm_sec, rtc_tm_2->tm_sec );*/

        eq += ( rtc_tm_1->tm_min   != rtc_tm_2->tm_min   );
        /*	printf( "min:\t%d,\t\t%d\n", rtc_tm_1->tm_min, rtc_tm_2->tm_min );*/

        eq += ( rtc_tm_1->tm_hour  != rtc_tm_2->tm_hour  );
        /*	printf( "hour:\t%d,\t\t%d\n", rtc_tm_1->tm_hour, rtc_tm_2->tm_hour );*/

        eq += ( ( rtc_tm_1->tm_mday  != rtc_tm_2->tm_mday  ) && (alarm == FALSE) );
        /*	printf( "day:\t%d,\t\t%d\n", rtc_tm_1->tm_mday, rtc_tm_2->tm_mday );*/

        eq += ( ( rtc_tm_1->tm_mon   != rtc_tm_2->tm_mon   ) && (alarm == FALSE) );
        /*	printf( "mon:\t%d,\t\t%d\n", rtc_tm_1->tm_mon, rtc_tm_2->tm_mon );*/

        eq += ( ( rtc_tm_1->tm_year  != rtc_tm_2->tm_year  ) && (alarm == FALSE) );
        /*	printf( "year:\t%d,\t\t%d\n", rtc_tm_1->tm_year, rtc_tm_2->tm_year );*/

        /*eq += ( rtc_tm_1->tm_wday  != rtc_tm_2->tm_wday  );
        printf( "wday:\t%d,\t\t%d\n", rtc_tm_1->tm_wday, rtc_tm_2->tm_wday );*/

        /*eq += ( rtc_tm_1->tm_yday  != rtc_tm_2->tm_yday  );
        printf( "yday:\t%d\t\t%d\n", rtc_tm_1->tm_yday, rtc_tm_2->tm_yday );*/

        /*eq += ( rtc_tm_1->tm_isdst != rtc_tm_2->tm_isdst );
        printf( "isdist:\t%d,\t\t%d\n", rtc_tm_1->tm_isdst, rtc_tm_2->tm_isdst );*/

        /*printf( "-----------------------------------\n" );*/

        return (eq == 0) ? TRUE : FALSE;
}

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

        // Wipe CR character from stream 
        fgetc(stdin);      
        return ret;
}


/*================================================================================================*/
/*===== VT_rtc_test2_setup =====*/
/**
@brief  assumes the pre-condition of the test case execution

@param  None
  
@return On success - return TPASS
        On failure - return the error code
*/
/*================================================================================================*/
int VT_rtc_test2_setup(void)
{
        int rv = TFAIL;
				if(pdevice == NULL)
        	file_desc = open (RTC_DRIVER_NAME, O_RDONLY);
				else
        	file_desc = open (pdevice, O_RDONLY);

        if (file_desc ==  -1)
        {
                tst_brkm(TBROK, cleanup, "ERROR : Open RTC driver fails");
                perror("cannot open /dev/misc/rtc");
        }
        else
        {
                rv = TPASS;
        }
    
        return rv;
}


/*================================================================================================*/
/*===== VT_rtc_test2_cleanup =====*/
/**
@brief  assumes the post-condition of the test case execution

@param  None
  
@return On success - return TPASS
        On failure - return the error code
*/
/*================================================================================================*/
int VT_rtc_test2_cleanup(void)
{
        int rv = TFAIL;
        int ret = 0;
        int i;

        for( i = 0; i < RTC_TOTAL_STATES; ++i )
        {
                if( saved_states[i] != 0 )
                {
                        restore_rtc_state( i );
                }
        }

        /* close RTC driver file descriptor */
        ret = close(file_desc);

        /* Close returns -1 in case of failure */
        if (ret == -1)
        {
                tst_resm(TWARN, "ERROR : Close RTC driver fails");
                perror("cannot close /dev/misc/rtc");
        }
        else
        {
                rv = TPASS;
        }

        return rv;
}


/*================================================================================================*/
/*===== VT_rtc_test2 =====*/
/**
@brief  - Read and set time and date
        - Set alarm and read alarm settings
        - Enable alarm. Wait for alarm interrupt. Disable alarm
        - Test stop watch interrupts

@param  int watch
  
@return On success - return TPASS
        On failure - return the error code
*/
/*================================================================================================*/
int VT_rtc_test2(void)
{
        int rv = TFAIL;
        int retval, irqcount = 0;
        unsigned long data = 0;

        struct rtc_time rtc_tm = {0,0,0,0,0,0};
        struct rtc_time rtc_tm_2 = {0,0,0,0,0,0};

        tst_resm( TINFO, "RTC Driver Test 2" );
        tst_resm( TINFO, "RTC date, time and alarm" );


        /***************************************/
        /* TEST Read and Set the RTC time/date */
        /***************************************/

        if( !save_rtc_state( RTC_TIME ) )
        {
                is_ok = 0;
        }

        tst_resm( TINFO, "SET TIME TEST: RTC_SET_TIME & RTC_RD_TIME" );
        tst_resm( TINFO, "  Read RTC date/time..." );

        retval = ioctl( file_desc, RTC_RD_TIME, &rtc_tm );

        if( retval < 0 )
        {
                tst_resm(TFAIL, "ioctl RTC_RD_TIME fails: %s \n", strerror(errno));
                is_ok = 0;
                return rv;
        }

        tst_resm( TINFO, "    Current RTC date/time is %d-%d-%d, %02d:%02d:%02d.",
                rtc_tm.tm_mday, rtc_tm.tm_mon+1, rtc_tm.tm_year+1900,
                rtc_tm.tm_hour, rtc_tm.tm_min, rtc_tm.tm_sec );

        /* Set the RTC time/date to the date of the crash ! */
        tst_resm(TINFO, "  Set new time and date..." );
        rtc_tm.tm_mday = 25;
        rtc_tm.tm_mon  = 3-1;
        rtc_tm.tm_year = 2008-1900; /* the reference year for tm structure is 1900 */
        rtc_tm.tm_hour = 10;
        rtc_tm.tm_min  = 58;
        rtc_tm.tm_sec  = 30;


        retval = ioctl( file_desc, RTC_SET_TIME, &rtc_tm );
        if( retval < 0 )
        {
                tst_brkm(TFAIL, cleanup, "    ioctl RTC_SET_TIME fails: %s \n", strerror(errno));
        }
        tst_resm( TINFO, "    Set Current RTC date/time to %d-%d-%d, %02d:%02d:%02d.",
                rtc_tm.tm_mday, rtc_tm.tm_mon+1, rtc_tm.tm_year+1900,
                rtc_tm.tm_hour, rtc_tm.tm_min, rtc_tm.tm_sec );

        sleep(1);

        retval = ioctl( file_desc, RTC_RD_TIME, &rtc_tm_2 );
        if( retval < 0 )
        {
                /* it's impossible */
                tst_resm(TFAIL, "    ioctl RTC_RD_TIME fails: %s \n", strerror(errno));
                is_ok = TFAIL;
        }
        tst_resm( TINFO, "    New current RTC date/time is %d-%d-%d, %02d:%02d:%02d.",
                rtc_tm_2.tm_mday, rtc_tm_2.tm_mon+1, rtc_tm_2.tm_year+1900,
                rtc_tm_2.tm_hour, rtc_tm_2.tm_min, rtc_tm_2.tm_sec );
    
        if(rtc_time_equal( &rtc_tm, &rtc_tm_2, FALSE ) == TRUE)
        {
                tst_resm( TPASS, "    Read and Set RTC time OK");
        }
        else
        {
                tst_resm( TFAIL, "    Set RTC time not available");
                is_ok = 0;
        }
    
        /******* end of test Read and Set RTC time ********/


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
        tst_resm(TINFO, "  Set the alarm to 5 seconds in the future..." );
        rtc_tm.tm_sec += 5;
        if (rtc_tm.tm_sec >= 60)
        {
                rtc_tm.tm_sec %= 60;
                rtc_tm.tm_min += 1;
        }
        if  (rtc_tm.tm_min >= 60)
        {
                rtc_tm.tm_min %= 60;
                rtc_tm.tm_hour += 1;
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

        retval = ioctl( file_desc, RTC_ALM_READ, &rtc_tm_2  );	
        if( retval < 0 )
        {
                /* it's impossible */
                tst_resm(TFAIL, "    ioctl RTC_ALM_READ fails: %s \n", strerror(errno));
                is_ok = 0;
        }
        else
        {
/*		tst_resm( TINFO, "Alarm is now set to %d-%d-%d, %02d:%02d:%02d.",
                  rtc_tm_2.tm_mday, rtc_tm_2.tm_mon+1, rtc_tm_2.tm_year+1900,
                  rtc_tm_2.tm_hour, rtc_tm_2.tm_min, rtc_tm_2.tm_sec );
*/
        /* Spring@2008.9.24
         * Set TINFO instead of TWARN, for TWARN will cause return value is non-zero */
        tst_resm( TINFO, "    Alarm is now set to %02d:%02d:%02d.\n\tWARNING: !! No date handled with Alarm service, only hours !!",
                rtc_tm_2.tm_hour, rtc_tm_2.tm_min, rtc_tm_2.tm_sec );
        }
        /* Enable alarm interrupts */
        tst_resm(TINFO, "    Enable alarm interrupts" );
        retval = ioctl( file_desc, RTC_AIE_ON, 0 );
        if( retval < 0 )
        {
                tst_resm(TFAIL, "    ioctl RTC_AIE_ON fails: %s \n", strerror(errno));
                return TFAIL;
        }

        tst_resm(TINFO, "  Waiting 5 seconds for alarm......." );

        /* This blocks until the alarm ring causes an interrupt */
        retval = read( file_desc, &data, sizeof(unsigned long) );
        if( retval < 0 )
        {
                tst_resm(TFAIL, "    read fails: %s \n", strerror(errno));
                return TFAIL;
        }
        else
        {
                irqcount++;
                tst_resm(TPASS, "  Test ALARM OK : Alarm rang." );
        }

        /* Disable alarm interrupts */
        tst_resm(TINFO, "  Disable alarm interrupts" );
        retval = ioctl( file_desc, RTC_AIE_OFF, 0 );
        if( retval < 0 )
        {
                tst_resm(TFAIL, "    ioctl RTC_AIE_OFF fails: %s \n", strerror(errno));
                return TFAIL;
        }

        /******** end of test for alarm *********/


        /***************************************/
        /* Start test the StopWatch            */
        /***************************************/
       /*
	 tst_resm( TINFO, "" );
        tst_resm( TINFO, "STOPWATCH TEST: RTC_WKALM_SET & RTC_WKALM_RD" );
        tst_resm( TINFO, "   RTC_WIE_ON & RTC_WIE_OFF not supported by MXC platforms" );
        */
        /* Read the RTC time/date */
        /*
        tst_resm( TINFO, "  Read RTC date/time..." );
        retval = ioctl( file_desc, RTC_RD_TIME, &rtc_tm );
        if( retval < 0 )
        {
                tst_resm(TFAIL, "    ioctl RTC_RD_TIME fails: %s \n", strerror(errno));
                return TFAIL;
        }

        tst_resm( TINFO, "    Current RTC date/time is %d-%d-%d, %02d:%02d:%02d.",
                rtc_tm.tm_mday, rtc_tm.tm_mon+1, rtc_tm.tm_year+1900,
                rtc_tm.tm_hour, rtc_tm.tm_min, rtc_tm.tm_sec );

        tst_resm( TINFO, "  Set wake up alarm to 1min 5sec.\n\tNote that Root privileges are needed to use it" );
        */
        /* Set wake up alarm service with values below */
       /*
	 wakeup_alarm.enabled = WAKEUP_ALARM_ENABLED;
        wakeup_alarm.pending = WKALM_NOT_PENDING;
        wakeup_alarm.time.tm_year = rtc_tm.tm_year;
        wakeup_alarm.time.tm_mon = rtc_tm.tm_mon;
        wakeup_alarm.time.tm_mday = rtc_tm.tm_mday;
        wakeup_alarm.time.tm_sec = rtc_tm.tm_sec+5;
        wakeup_alarm.time.tm_min = rtc_tm.tm_min+1;
        wakeup_alarm.time.tm_hour = rtc_tm.tm_hour;

        if( !save_rtc_state( RTC_WKALARM_TIME ) )
        {
                is_ok = 0;
        }

        tst_resm( TINFO, "    (RTC_WKALM_SET) Set Wake up alarm time to %d-%d-%d, %02d:%02d:%02d.",
                wakeup_alarm.time.tm_mday, wakeup_alarm.time.tm_mon+1, wakeup_alarm.time.tm_year+1900,
                wakeup_alarm.time.tm_hour, wakeup_alarm.time.tm_min, wakeup_alarm.time.tm_sec );
        retval = ioctl( file_desc, RTC_WKALM_SET, &wakeup_alarm );
        if( retval < 0 )
        {
                tst_resm(TFAIL, "    ioctl RTC_WKALM_SET fails: %s \n", strerror(errno));
                return TFAIL;
        }

        tst_resm( TINFO, "    (RTC_WKALM_RD) Read wake up alarm service" );
        retval = ioctl(file_desc, RTC_WKALM_RD, &wakeup_alarm_2 );
        if( retval < 0 )
        {
                tst_resm(TFAIL, "    ioctl RTC_WKALM_RD fails: %s \n", strerror(errno));
                return TFAIL;
        }
        tst_resm( TINFO, "    Wake up alarm time is now set to %d-%d-%d, %02d:%02d:%02d.",
                wakeup_alarm_2.time.tm_mday, wakeup_alarm_2.time.tm_mon+1, wakeup_alarm_2.time.tm_year+1900,
                wakeup_alarm_2.time.tm_hour, wakeup_alarm_2.time.tm_min, wakeup_alarm_2.time.tm_sec );

        if(rtc_time_equal( &wakeup_alarm.time, &wakeup_alarm_2.time, FALSE ) == TRUE)
        {
                tst_resm( TPASS, "    Read and Set RTC wake-up alarm OK");
        }
        else
        {
                tst_resm( TFAIL, "    Set RTC wake-up alarm not available");
                is_ok = 0;
        }
        */
        /* STOP WATCH INTERRUPTS NOT SUPPORTED BY MXC PLATFORM - refer to TLSbo54508 */
        /* Enable Stop Watch interrupts */
        /*
        tst_resm( TINFO, "Enable stop watch interrupts" );
        retval = ioctl( file_desc, RTC_WIE_ON, 0 );
        if( retval < 0 )
        {
                perror("ioctl RTC_WIE_ON");
                tst_resm(TWARN, "RTC_WIE_ON: enable stop watch interrupts not supported yet");
        }
        else
        {
                irqcount++;
                tst_resm(TPASS, "Test STOPWATCH OK : Stopwatch rang." );
        }
        */
        /* Disable WatchStop interrupts */
        /*
        tst_resm(TINFO, "Disable stopwatch interrupts" );
        retval = ioctl( file_desc, RTC_WIE_OFF, 0 );
        if( retval < 0 )
        {
                perror("ioctl RTC_WIE_OFF");
                tst_resm(TWARN, "RTC_WIE_OFF : disable stop watch interrupts not supported yet");
        }
        */

        /******** end of test for stop watch *********/

        /***************************************/
        /* Restore the initial date & time     */
        /***************************************/
        
        if( !restore_rtc_state( RTC_TIME ) )
        {
                is_ok = 0;
        }
        
        retval = ioctl( file_desc, RTC_RD_TIME, &rtc_tm );

        if( retval < 0 )
        {
                tst_resm(TFAIL, "    ioctl RTC_RD_TIME fails: %s \n", strerror(errno));
                is_ok = 0;
                return rv;
        }

        tst_resm( TINFO, "    Current RTC date/time is %d-%d-%d, %02d:%02d:%02d.",
                rtc_tm.tm_mday, rtc_tm.tm_mon+1, rtc_tm.tm_year+1900,
                rtc_tm.tm_hour, rtc_tm.tm_min, rtc_tm.tm_sec );


        /***************************************/
        /* Final test result                   */
        /***************************************/
        
        rv = is_ok!=0?TPASS:TFAIL;
        return rv;
}

#ifdef __cplusplus
}
#endif
