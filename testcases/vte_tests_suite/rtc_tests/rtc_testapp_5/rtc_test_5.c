/***
**Copyright (C) 2005-2011 Freescale Semiconductor, Inc. All Rights Reserved.
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
    @file   rtc_test_5.c

    @brief  RTC test

====================================================================================================
Revision History:
                Modification     Tracking
Author             Date          Number         Description of Changes
-------------   ------------    -----------------------------------------------------
E.Gromazina      27/06/2005        TLSbo49951          Initial version
S. V-Guilhou     07/11/2005        TLSbo56422          EPOCH_SET not supported
Spring Zhang     13/04/2011        n/a                 Attempt RTC devices

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
#include <time.h>
#include <poll.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <sys/wait.h>

#define __USE_GNU
#include<sched.h>
#include<ctype.h>
#include<string.h>

#include <linux/mxc_srtc.h>

/* Verification Test Environment Include Files */
#include "rtc_test_5.h"

/*==================================================================================================
                                        LOCAL MACROS
==================================================================================================*/
#if !defined(TRUE) && !defined(FALSE)
    #define TRUE  1
    #define FALSE 0
#endif  

/*==================================================================================================
                          LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
==================================================================================================*/


/*==================================================================================================
                                       LOCAL CONSTANTS
==================================================================================================*/


/*==================================================================================================
                                       LOCAL VARIABLES
==================================================================================================*/
int file_desc = -1; 
int is_ok = 1;                

/*==================================================================================================
                                       GLOBAL CONSTANTS
==================================================================================================*/


/*==================================================================================================
                                       GLOBAL VARIABLES
==================================================================================================*/
extern char * pdevice;
extern char * RTC_DRIVER_NAME[];

/*==================================================================================================
                                   LOCAL FUNCTION PROTOTYPES
==================================================================================================*/


/*==================================================================================================
                                       LOCAL FUNCTIONS
==================================================================================================*/

/*================================================================================================*/
/*===== VT_rtc_test5_setup =====*/
/**
@brief  assumes the pre-condition of the test case execution

@param  None
  
@return On success - return TPASS
        On failure - return the error code
*/
/*================================================================================================*/
int VT_rtc_test5_setup(void)
{
        int rv = TFAIL;
        int i = 0;

        if (pdevice == NULL)
            do {
                file_desc = open(RTC_DRIVER_NAME[i], O_RDONLY);
            } while (file_desc <= 0 && i++<RTC_DEVICE_NUM);
        else 
            file_desc = open(pdevice, O_RDONLY);

        if (file_desc ==  -1)
            tst_brkm(TBROK, cleanup, "ERROR : Open RTC driver fails");
        else {
            tst_resm(TINFO, "Open RTC device successfully: %s \n",
                pdevice? pdevice: RTC_DRIVER_NAME[i]);
            rv = TPASS;
        }

        return rv;
}

/*================================================================================================*/
/*===== VT_rtc_test5_cleanup =====*/
/**
@brief  assumes the post-condition of the test case execution

@param  None
  
@return On success - return TPASS
        On failure - return the error code
*/
/*================================================================================================*/
int VT_rtc_test5_cleanup(void)
{
        int rv = TFAIL;
        int ret = 0;
        
        /* close RTC driver file descriptor */
        ret = close(file_desc);
        
        if (ret == -1)
                tst_resm(TWARN, "ERROR : Close RTC driver fails");
        else
                rv = TPASS;
                
        return rv;
}


/*================================================================================================*/
/*===== VT_rtc_test5 =====*/
/**
@brief  - Read and set eroch
                - poll test
                - fasync test

@param  No
  
@return On success - return TPASS
        On failure - return the error code
*/
/*================================================================================================*/
int VT_rtc_test5(int sw_t)
{
        int rv = TFAIL;
        int retval;
        unsigned long ep = 0;
        struct rtc_time rtc_tm = {0,0,0,0,0,0};
        struct pollfd fds;
        int flags;

        switch (sw_t)
        {
                
                case 0: /*---Not supported by MXC platforms(MX31)---*/
                        /***********************************/
                        /* TEST Read and Set the RTC epoch */
                        /***********************************/
                        tst_resm(TINFO, "  Test RTC_EPOCH_READ");
                        tst_resm(TINFO, "  Do not test RTC_EPOCH_SET: not supported by MXC platforms\n");

                        /* Read the RTC epoch */
                        tst_resm( TINFO, "  Read the RTC epoch..." );
                        retval = ioctl( file_desc, RTC_EPOCH_READ, &ep);
                        if( retval < 0 )
                            {
                                tst_resm( TFAIL, "    ioctl RTC_EPOCH_READ fails" );
                                tst_resm( TINFO, "    Errno: %d, Reason: %s", errno, strerror(errno) );
                                is_ok = 0;
                        }
                        else
                        {
                                tst_resm( TINFO, "    ioctl RTC_EPOCH_READ worked as expected, epoch is %lu",ep);
                        }
                
                        /* Set the RTC epoch (the value must be >= 1900) */
                        
                        /*-------- NOT SUPPORTED by MCX platforms
                        
                        tst_resm( TINFO, "Set the RTC epoch to %lu...", ep );
                        retval = ioctl( file_desc, RTC_EPOCH_SET, &ep);
                        if( retval < 0 )
                        {
                                tst_resm( TFAIL, "    ioctl RTC_EPOCH_SET fails" );
                                tst_resm( TINFO, "    Errno: %d, Reason: %s", errno, strerror(errno) );
                                is_ok = 0;
                        }
                        else
                        {
                                tst_resm( TINFO, "    ioctl RTC_EPOCH_SET worked as expected");
                        }
                        
                        ---------*/
                
                        /* Set the RTC epoch */
                        
                        /*-------- NOT SUPPORTED by MCX platforms
                        
                        ep=10000;
                        tst_resm( TINFO, "Set the RTC epoch to %lu...", ep );
                        retval = ioctl( file_desc, RTC_EPOCH_SET, &ep);
                        if( retval < 0 )
                        {
                                tst_resm( TFAIL, "    ioctl RTC_EPOCH_SET fails" );
                                tst_resm( TINFO, "    Errno: %d, Reason: %s", errno, strerror(errno) );
                                is_ok = 0;
                        }
                        else
                        {
                                tst_resm( TINFO, "    ioctl RTC_EPOCH_SET worked as expected");
                        }
                        
                        ---------*/
                break;
                        
                case 1:
                        /**************************/
                        /* TEST poll() for RTC    */
                        /**************************/

                        retval = ioctl( file_desc, RTC_RD_TIME, &rtc_tm );
                        if( retval < 0 )
                        {
                                tst_resm( TFAIL, "    ioctl RTC_RD_TIME fails" );
                                tst_resm( TINFO, "    Errno: %d, Reason: %s", errno, strerror(errno) );
                                is_ok = 0;
                                //return TFAIL;
                        }
                        else
                        {
                                tst_resm( TINFO, "    ioctl RTC_RD_TIME worked as expected");
                        }
         
                        /* Set the alarm to 3 sec in the future, and check for roll() */
                        tst_resm( TINFO, "Set the alarm to 3 sec in the future..." );
                        rtc_tm.tm_sec += 3;
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
                                tst_brkm( TRETR, cleanup, "Run test issue. Restart it." );
                        }
        
                        retval = ioctl( file_desc, RTC_ALM_SET, &rtc_tm );
                        if( retval < 0 )
                           {
                                tst_resm( TFAIL, "    ioctl RTC_ALM_SET fails" );
                                tst_resm( TINFO, "    Errno: %d, Reason: %s", errno, strerror(errno) );
                                is_ok = 0;
                                //return TFAIL;
                        }
                        else
                        {
                                tst_resm( TINFO, "    ioctl RTC_ALM_SET worked as expected");
                        }
                        
                        fds.fd = file_desc;
                        fds.events = POLLIN;
                        fds.revents = 0;
                        retval = poll(&fds,1,0);

                        /* Enable alarm interrupts */
                        tst_resm( TINFO, "Enable alarm interrupts..." );
                        retval = ioctl( file_desc, RTC_AIE_ON, 0 );
                        if( retval < 0 )
                        {
                                tst_resm( TFAIL, "ioctl RTC_AIE_ON fails" );
                                tst_resm( TINFO, "    Errno: %d, Reason: %s", errno, strerror(errno) );
                                is_ok = 0;
                        }
                        else
                        {
                                tst_resm( TINFO, "    ioctl RTC_AIE_ON worked as expected");
                        }
                        
                        tst_resm(TINFO, "Waiting 3 seconds ...." );
                        do 
                        {
                                retval = poll(&fds,1,0);
                        } while (retval == 0);        
                        
                        tst_resm( TINFO, "    poll() worked as expected");

                        /* Disable alarm interrupts */
                        tst_resm( TINFO, "Disable alarm interrupts..." );
                        retval = ioctl( file_desc, RTC_AIE_OFF, 0 );
                        if( retval < 0 )
                        {
                                tst_resm( TFAIL, "ioctl RTC_AIE_OFF fails" );
                                tst_resm( TINFO, "    Errno: %d, Reason: %s", errno, strerror(errno) );
                                is_ok = 0;
                        }
                        else
                        {
                                tst_resm( TINFO, "    ioctl RTC_AIE_OFF worked as expected");
                        }
                        
                break;
                case 2:

                        /**************************/
                        /* TEST fasync() for RTC  */
                        /**************************/

                        flags = fcntl (file_desc, F_GETFL);
                        if (flags < 0) 
                        {
                                tst_resm( TFAIL, "getflags fails" );
                                tst_resm( TINFO, "    Errno: %d, Reason: %s", errno, strerror(errno) );
                                is_ok = 0;
                        }

                        flags |= FASYNC;
        
                        if (fcntl (file_desc, F_SETFL, flags) == -1) 
                        {
                                tst_resm( TFAIL, "Cannot set asynchronous for RTC" );
                                tst_resm( TINFO, "    Errno: %d, Reason: %s", errno, strerror(errno) );
                                is_ok = 0;
                        }
                        else
                        {
                                tst_resm( TINFO, "fasync test worked as expected");
                        }
                break;
				case 3:
				{
					struct timeval ctime;				    
        			struct rtc_time rtc_tm;
					pid_t pid;
					cpu_set_t mask;
					CPU_ZERO(&mask);
					CPU_SET(0, &mask);
        			if (sched_setaffinity(0, sizeof(mask), &mask) == -1)
        			{
                		tst_resm(TFAIL,"warning: could not set CPU affinity, continuing...");
						break;
        			} 
                    retval = ioctl( file_desc, RTC_READ_TIME_47BIT, &ctime );
					if (retval)
					{
                        tst_resm( TFAIL, "Cannot use RTC_READ_TIME_47BIT for RTC" );
                        tst_resm( TINFO, "Errno: %d, Reason: %s", errno, strerror(errno) );
						is_ok = 0;
						break;
					}
                    tst_resm( TINFO, "RTC_READ_TIME_47BIT worked as expected");
					retval = ioctl( file_desc, RTC_RD_TIME, &rtc_tm );
					if (retval < 0)
					{
						tst_resm(TFAIL, "ioctl RTC_RD_TIME fails: %s \n", strerror(errno));	
						exit(1);
					}
					pid = fork();
					if (!pid) {/*CHILD*/
						/*wait parent to execute first*/
						setpriority(PRIO_PROCESS, 0, 15);
						sleep(1);
                    	tst_resm( TINFO, "child procee to set time" );
        				retval = ioctl( file_desc, RTC_SET_TIME, &rtc_tm );
						if (retval < 0)
						{
							tst_resm(TFAIL, "ioctl RTC_SET_TIME fails: %s \n", strerror(errno));	
                            exit(1);
						}
						retval = ioctl( file_desc, RTC_RD_TIME, &rtc_tm );
						if (retval < 0)
						{
						tst_resm(TFAIL, "ioctl RTC_RD_TIME fails: %s \n", strerror(errno));	
						exit(1);
						}
                    	tst_resm( TINFO, "child procee to quit" );
						exit(0);
					}
					/*parent*/
                    tst_resm( TINFO, "parent procee to check child write" );
                    retval |= ioctl( file_desc, RTC_WAIT_TIME_SET, &ctime );
					if (retval)
					{
                        tst_resm( TFAIL, "Cannot use RTC_WAIT_TIME_SET for RTC" );
                        tst_resm( TINFO, "Errno: %d, Reason: %s", errno, strerror(errno) );
						is_ok = 0;
						break;
					}
                    tst_resm( TINFO, "parent procee to quit" );
					wait(&retval);
					if(retval==0)
                    	tst_resm( TINFO, "RTC_WAIT_TIME_SET worked as expected");
					else{
						is_ok = 0;
                    	tst_resm( TINFO, "RTC_WAIT_TIME_SET failed");
					}
					break;
				}
        }        

        rv = is_ok!=0?TPASS:TFAIL;
        return rv;
}


#ifdef __cplusplus
}
#endif
