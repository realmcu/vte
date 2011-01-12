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
    @file   rtc_test_4.c

    @brief  RTC error test cases.
====================================================================================================
Revision History:
                            Modification     Tracking
Author                          Date          Number    Description of Changes
-------------------------   ------------    ----------  -------------------------------------------
V. BECKER/rc023c             08/04/2004     TLSbo38652   Initial version 
D. SIMAKOV                   02/06/2004     TLSbo39743   This test is full-automatic now
V. BECKER/rc023c             03/06/2004     TLSbo39685   RTC_PIE_OFF not supported
L. DELASPRE/rc149c           23/06/2004     TLSbo39941   VTE 1.3 integration
D. SIMAKOV                   19/07/2004     TLSbo39743   An errors are corrected 
L. DELASPRE/rc149c           03/08/2004     TLSbo40891   VTE 1.4 integration
L. DELASPRE/rc149c           13/04/2005     TLSbo48760   VTE 1.9 integration
S.ZAVJALOV/zvjs001c          26/05/2005     TLSbo49951   Removed wrong test. Repair "negative size of buffer" test
E.Gromazina                  21/11/2005     TLSbo58720   update to confirm RTC_IRQP_SET  
==================================================================================================== 
Portability:  ARM   GCC  Montavista 
==================================================================================================*/
#ifdef __cplusplus
extern "C"{
#endif

/*==================================================================================================
                                        INCLUDE FILES
==================================================================================================*/
/* Standard Include Files */
#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <execinfo.h>
#include <limits.h>
#include <ucontext.h>
#include <sys/types.h>
#include <setjmp.h>    
/* Harness Specific Include Files. */
#include "test.h"

/* Verification Test Environment Include Files */
#include "rtc_test_4.h"

/*==================================================================================================
                                        LOCAL MACROS
==================================================================================================*/
#if !defined(TRUE) && !defined(FALSE)
        #define TRUE 1
        #define FALSE 0
#endif    

/*==================================================================================================
                          LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
==================================================================================================*/


/*==================================================================================================
                                       LOCAL CONSTANTS
==================================================================================================*/
#define BAD_RTC_IOCTL 0x20
#define arrsizeof(x) (sizeof(x)/sizeof(x[0]))

/*==================================================================================================
                                       LOCAL VARIABLES
==================================================================================================*/
int file_desc = -1;
int is_ok = TRUE;
jmp_buf   jmpbuf;
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


/*==================================================================================================
                                       LOCAL FUNCTIONS
==================================================================================================*/


/*================================================================================================*/
/*===== VT_rtc_test4_setup =====*/
/**
@brief  assumes the pre-condition of the test case execution

@param  None
  
@return On success - return TPASS
        On failure - return the error code
*/
/*================================================================================================*/
int VT_rtc_test4_setup(void)
{
        int rv = TFAIL;
        int file_desc1 = -1;
        int is_passed = FALSE;
        
        /* Open RTC driver file descriptor */
				if(pdevice == NULL)
        	file_desc = open (RTC_DRIVER_NAME, O_RDONLY | O_NONBLOCK);
				else
        	file_desc = open (pdevice, O_RDONLY | O_NONBLOCK);

        if (file_desc ==  -1)
        {
                tst_resm(TFAIL,"ERROR : Open RTC driver fails");
                perror("cannot open /dev/misc/rtc");                
                return rv;
        }
        
        /* Opening driver a second time returns busy */
        tst_resm(TINFO, "Repeatedly open driver must return the -EBUSY" );
        
				if(pdevice == NULL)
        	file_desc1 = open (RTC_DRIVER_NAME, O_RDONLY);
				else
        	file_desc1 = open (pdevice, O_RDONLY | O_NONBLOCK);

        if (file_desc1 == -1)
        {      
                is_passed = ( errno == EBUSY ) ? TRUE : FALSE;                
        }

        if( is_passed )
        {
                tst_resm( TPASS, "Test is working as expected\n" );
                rv = TPASS;
        }
        else
        {
                tst_brkm( TFAIL, cleanup, "Test is not working as expected\n" );
                rv = TFAIL;
        }
        
        is_ok = is_ok && is_passed;

        return rv;
}


/*================================================================================================*/
/*===== VT__rtc_test4_cleanup =====*/
/**
@brief  assumes the post-condition of the test case execution

@param  None
  
@return On success - return TPASS
        On failure - return the error code
*/
/*================================================================================================*/
int VT_rtc_test4_cleanup(void)
{
        int rv = TFAIL;
        int ret = 0;
        
        /* close RTC driver file descriptor */
        ret = close(file_desc);
        /* Close returns -1 in case of failure */
        if (ret == -1)
        {
                tst_resm(TFAIL, "ERROR : Close RTC driver fails \n");                
                perror("cannot close /dev/misc/rtc");
        }
        else
        {
                rv = TPASS;
        }

        return rv;
}

//
//
//
char *getProcessName (void)
{
  char buffer [128];
  FILE *fp;

  sprintf (buffer, "/proc/%d/stat", getpid ());

  if ((fp = fopen (buffer, "r")))
  {
    if (fgets (buffer, sizeof (buffer), fp))
    {
      char *s;

      if ((s = index (buffer, ')')))
      {
        *s = '\0';

        if ((s = index (buffer, '(')))
          return ++s;
      }
    }
  }

  return NULL;
}

//
//
//
static void sighandlerPrint (FILE *fp, int signo, int code, ucontext_t *context, void *bt [], int bt_size)
{
  char *processName;
  time_t ttime = time (NULL);

#ifdef __arm__
  fprintf (fp, "%s", ctime (&ttime));
  fprintf (fp, "PID=%d (%s)\n", getpid (), (processName = getProcessName ()) ? processName : "unknown");
  fprintf (fp, "signo=%d/%s\n", signo, strsignal (signo));
  fprintf (fp, "code=%d (not always applicable)\n", code);
  fprintf (fp, "\nContext: 0x%08lx\n", (unsigned long) context);
  fprintf (fp, "    trap_no: 0x%08lx   error_code: 0x%08lx   oldmask: 0x%08lx\n"
               "   arm_r0: 0x%08lx  arm_r1: 0x%08lx  arm_r2: 0x%08lx  arm_r3: 0x%08lx\n"
               "   arm_r4: 0x%08lx  arm_r5: 0x%08lx  arm_r6: 0x%08lx  arm_r7: 0x%08lx\n"
               "  arm_r8:  0x%8lx  arm_r9: 0x%08lx  arm_r10: 0x%08lx   arm_fp: 0x%08lx\n"
               "  arm_ip: 0x%08lx   arm_sp: 0x%08lx   arm_lr: 0x%08lx  arm_pc: 0x%08lx\n"
               "  arm_cpsr: 0x%08lx   fault_address: 0x%08lx\n",
               context->uc_mcontext.trap_no,
							 context->uc_mcontext.error_code,
							 context->uc_mcontext.oldmask,
							 context->uc_mcontext.arm_r0,
               context->uc_mcontext.arm_r1,
							 context->uc_mcontext.arm_r2,
							 context->uc_mcontext.arm_r3,
							 context->uc_mcontext.arm_r4,
               context->uc_mcontext.arm_r5,  
							 context->uc_mcontext.arm_r6,
							 context->uc_mcontext.arm_r7,
							 context->uc_mcontext.arm_r8,
               context->uc_mcontext.arm_r9,
							 context->uc_mcontext.arm_r10,
							 context->uc_mcontext.arm_fp,
							 context->uc_mcontext.arm_ip,
               context->uc_mcontext.arm_sp,  
							 context->uc_mcontext.arm_lr,
							 context->uc_mcontext.arm_pc,
							 context->uc_mcontext.arm_cpsr,
							 context->uc_mcontext.fault_address
  );

  fprintf (fp, "\n%d elements in backtrace\n", bt_size);
  fflush (fp);
#endif
  backtrace_symbols_fd (bt, bt_size, fileno (fp));
}



static void sighandlerABRT (int signo, struct siginfo *si, void *ctx)
{
  void *bt [128];
  int bt_size;

  bt_size = backtrace (bt, arrsizeof (bt));

  sighandlerPrint (stderr, signo, si->si_code, (ucontext_t *) ctx, bt, bt_size);

  longjmp(jmpbuf,1);
  /*exit (1);*/
}


int installSignalHandlers (void)
{
  struct sigaction sa;

  sigemptyset (&sa.sa_mask);
  sigaddset (&sa.sa_mask, SIGABRT);

  sa.sa_flags = SA_ONESHOT | SA_SIGINFO;
  sa.sa_sigaction = sighandlerABRT;

  if (sigaction (SIGABRT, &sa, NULL))
  {
    fprintf (stderr, "sigaction failed, line %d, %d/%s\n", __LINE__, errno, strerror (errno));
    exit (1);
  }
  printf("abort signal installed\n");
  return 1;
}


/*================================================================================================*/
/*===== VT_rtc_test4 =====*/
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
int VT_rtc_test4(void)
{
        int rv = TFAIL;
        int retval = 0;
        unsigned long data =0;
        long buffer_size = 0;
        struct rtc_time rtc_tm;
//        struct rtc_wkalrm wakeup_alarm;
        int is_passed = FALSE;
				int result;

        tst_resm(TINFO, "RTC Driver error cases test2");

 				installSignalHandlers ();
        /**************************/
        /* Do a non-blocking read */
        /**************************/

        is_passed = 0;
        tst_resm(TINFO, "Do a non-blocking read  (errno should be -EAGAIN)" );
        retval = read( file_desc, &data, sizeof(unsigned long) );
        if( retval < 0 )
        {
                is_passed = TRUE;
                tst_resm( TINFO, "Errno: %d, Reason: %s", errno, strerror(errno) );
        }
        if( is_passed )
        {
                tst_resm( TPASS, "It's working as expected\n" );
        }
        else
        {
                tst_resm( TFAIL, "It's not working as expected\n" );
        }
        is_ok = is_ok && is_passed;


        /*******************************************/
        /*  Pass negative size of buffer in read() */
        /*******************************************/
        is_passed = 0;
        tst_resm(TINFO, "Pass negative size of buffer in read()" );
        result = setjmp(jmpbuf);

				if(result == 0)
				{
					buffer_size = -2;
					retval = read( file_desc, &data, buffer_size );
          if( retval < 0 )
        	{                
                is_passed = TRUE;
                tst_resm( TINFO, "Errno: %d, Reason: %s", errno, strerror(errno) );
        	}
        	if( is_passed )
        	{
                tst_resm( TPASS, "It's working as expected\n" );
        	}
        	else
        	{
                tst_resm( TFAIL, "It's not working as expected\n" );
        	}
				}else{
                is_passed = TRUE;
                tst_resm( TPASS, "It's working as expected\n" );
				}
        is_ok = is_ok && is_passed;
        
        
        /*************************************************/
        /* Set date/time with a wrong rtc_time structure */
        /*************************************************/
        
        tst_resm(TINFO, "Set date/time with wrong values of rtc_time structure" );
        rtc_tm.tm_mday = 32;
        rtc_tm.tm_mon  = 8;
        rtc_tm.tm_year = 1976;
        rtc_tm.tm_hour = 8;
        rtc_tm.tm_min  = 15;
        rtc_tm.tm_sec  = 47;
        
        /* Set wrong day */
        is_passed = FALSE;
        tst_resm(TINFO, "Set wrong day -32- (errno should be -EINVAL)" );
        retval = ioctl( file_desc, RTC_SET_TIME, &rtc_tm );
        if(retval < 0)
        {                
                is_passed = TRUE; 
                tst_resm( TINFO, "Errno: %d, Reason: %s", errno, strerror(errno) );
        }
        if( is_passed )
        {
                tst_resm( TPASS, "It's working as expected\n" );
        }
        else
        {
                tst_resm( TFAIL, "It's not working as expected\n" );
        }
        is_ok = is_ok && is_passed;
        
        /* Set wrong month */
        is_passed = FALSE;
        tst_resm(TINFO, "Set wrong month -13- (errno should be -EINVAL)" );
        rtc_tm.tm_mday = 18;
        rtc_tm.tm_mon  = 13;
        
        retval = ioctl( file_desc, RTC_SET_TIME, &rtc_tm );
        if( retval < 0 )
        {
                is_passed = TRUE;
                tst_resm( TINFO, "Errno: %d, Reason: %s", errno, strerror(errno) );
        }
        if( is_passed )
        {
                tst_resm( TPASS, "It's working as expected\n" );
        }
        else
        {
                tst_resm( TFAIL, "It's not working as expected\n" );
        }
        is_ok = is_ok && is_passed;
        
        /* Set wrong year */
        is_passed = FALSE;                
        tst_resm(TINFO, "Set wrong year -1969- (errno should be -EINVAL)" );
        rtc_tm.tm_mon  = 8;
        rtc_tm.tm_year = 69;
        
        retval = ioctl( file_desc, RTC_SET_TIME, &rtc_tm );
         if( retval < 0 )
        {                
                is_passed = TRUE;
                tst_resm( TINFO, "Errno: %d, Reason: %s", errno, strerror(errno) );
        }
        if( is_passed )
        {
                tst_resm( TPASS, "It's working as expected\n" );
        }
        else
        {
                tst_resm( TFAIL, "It's not working as expected\n" );
        }
        is_ok = is_ok && is_passed;
        
        /* Set wrong hour */
        is_passed = FALSE;                
        tst_resm(TINFO, "Set wrong hour -25- (errno should be -EINVAL)" );
        rtc_tm.tm_year = 1976;
        rtc_tm.tm_hour = 25;
        
        retval = ioctl( file_desc, RTC_SET_TIME, &rtc_tm );
        if( retval < 0 )
        {                
                is_passed = TRUE;
                tst_resm( TINFO, "Errno: %d, Reason: %s", errno, strerror(errno) );
        }
        if( is_passed )
        {
                tst_resm( TPASS, "It's working as expected\n" );
        }
        else
        {
                tst_resm( TFAIL, "It's not working as expected\n" );
        }
        is_ok = is_ok && is_passed;
        
        /* Set wrong minute */
        is_passed = 0;
        tst_resm(TINFO, "Set wrong minute -61- (errno should be -EINVAL)" );
        rtc_tm.tm_hour = 23;
        rtc_tm.tm_min  = 61;
        
        retval = ioctl( file_desc, RTC_SET_TIME, &rtc_tm );
        if( retval < 0 )
        {                
                is_passed = TRUE;
                tst_resm( TINFO, "Errno: %d, Reason: %s", errno, strerror(errno) );
        }
        if( is_passed )
        {
                tst_resm( TPASS, "It's working as expected\n" );
        }
        else
        {
                tst_resm( TFAIL, "It's not working as expected\n" );
        }
        is_ok = is_ok && is_passed;
        
        /* Set wrong second */
        is_passed = FALSE;                
        tst_resm(TINFO, "Set wrong second -61- (errno should be -EINVAL)");
        rtc_tm.tm_min = 59;
        rtc_tm.tm_sec = 61;
        
        retval = ioctl( file_desc, RTC_SET_TIME, &rtc_tm );        
        if( retval < 0 )
        {                
                is_passed = TRUE;
                tst_resm( TINFO, "Errno: %d, Reason: %s", errno, strerror(errno) );
        }
        if( is_passed )
        {
                tst_resm( TPASS, "It's working as expected\n" );
        }
        else
        {
                tst_resm( TFAIL, "It's not working as expected\n" );
        }
        is_ok = is_ok && is_passed;
        

        /*********************************************************/
        /* Set the rtc_freq to 1024 Hz . It should not be allowed */
        /*********************************************************/

        is_passed = FALSE;                
        tst_resm(TINFO, "Set wrong frequency: more than 512 Hz (errno should be -EINVAL)");
        retval = ioctl( file_desc, RTC_IRQP_SET, BAD_FREQUENCY2 );
        if( retval < 0 )
        {                
                is_passed = TRUE;
                tst_resm( TINFO, "Errno: %d, Reason: %s", errno, strerror(errno) );
        }
        if( is_passed )
        {
                tst_resm( TPASS, "It's working as expected\n" );
        }
        else
        {
                tst_resm( TFAIL, "It's not working as expected\n" );
        }
        is_ok = is_ok && is_passed;

        /************************************************************************/
        /* Set the rtc_freq to a value not divisible by 2. It should not be allowed */
        /************************************************************************/

        is_passed = FALSE;                
        tst_resm(TINFO, "Set wrong frequency (not divisible by 2) -33 Hz-. It should not be allowed" );
        retval = ioctl( file_desc, RTC_IRQP_SET, 33 );
        if( retval < 0 )
        {                
                is_passed = TRUE;
                tst_resm( TINFO, "Errno: %d, Reason: %s", errno, strerror(errno) );
        }
        if( is_passed )
        {
                tst_resm( TPASS, "It's working as expected\n" );
        }
        else
        {
                tst_resm( TFAIL, "It's not working as expected\n" );
        }
        is_ok = is_ok && is_passed;
        
        /**********************************/
        /* Use a bad ioctl command number */
        /**********************************/
        
        is_passed = 0;                
        tst_resm(TINFO, "Use a bad IOCTL command number (errno should be -EINVAL)" );
        retval = ioctl( file_desc, BAD_RTC_IOCTL, 0 );
        if( retval < 0 )
        {                
                is_passed = TRUE;
                tst_resm( TINFO, "Errno: %d, Reason: %s", errno, strerror(errno) );
        }
        if( is_passed )
        {
                tst_resm( TPASS, "It's working as expected\n" );
        }
        else
        {
                tst_resm( TFAIL, "It's not working as expected\n" );
        }
        is_ok = is_ok && is_passed;
                
        
        if( is_ok )
        {
                rv = TPASS;
        }
        else
        {
                rv = TFAIL;
        }
        
        return rv;
}

#ifdef __cplusplus
}
#endif
