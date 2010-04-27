/***
**Copyright (C) 2004-2009 Freescale Semiconductor, Inc. All Rights Reserved.
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
    @file   rtc_test_3.c

    @brief  RTC periodic interrupts

====================================================================================================
Revision History:
                            Modification     Tracking
Author                          Date          Number    Description of Changes
-------------------------   ------------    ----------  -------------------------------------------
V. BECKER / rc023c           02/04/2004     TLSbo38652   Initial version 
V. BECKER / rc023c           25/05/2004     TLSbo38652   Change file name
V. BECKER / rc023c           03/06/2004     TLSbo39890   IOCTLs not supported
D. Simakov                   04/06/2004     TLSbo39743   Code improvement
D. Simakov                   19/07/2004     TLSbo39743   An errors are corrected
L. Delaspre / rc149c         03/08/2004     TLSbo40891   VTE 1.4 integration
S. V-Guilhou / svan01c       14/09/2005     TLSbo53745   unsupported ioctl
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
#include <errno.h>
    
/* Harness Specific Include Files. */
#include "test.h"

/* Verification Test Environment Include Files */
#include "rtc_test_3.h"

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
int file_desc = -1;
unsigned long saved_periodic_rate = 0;

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
/*===== VT_rtc_test3_setup =====*/
/**
@brief  assumes the pre-condition of the test case execution

@param  None
  
@return On success - return TPASS
        On failure - return the error code
*/
/*================================================================================================*/
int VT_rtc_test3_setup(void)
{
        int rv = TPASS;
        int retval;

        file_desc = open( RTC_DRIVER_NAME, O_RDONLY );
        if( file_desc == -1 )
        {
                tst_resm(TFAIL, "Open RTC driver fails: %s \n", strerror(errno));
                return TFAIL;
        }

        /*==============================================================*/
        /* RTC_IRQP_READ & RTC_IRQP_SET:                                */
        /*==============================================================*/
        
        retval = ioctl( file_desc, RTC_IRQP_READ, &saved_periodic_rate );
        if( retval < 0 )
        {
                tst_resm(TWARN, "ioctl RTC_IRQP_READ not SUPPORTED by platform" );
                perror( "ioctl fails for RTC_IRQP_READ" );
        }
        else
                tst_resm(TINFO, "Store periodic IRQ rate = %ld Hz.", saved_periodic_rate );

        
    return rv;
}


/*================================================================================================*/
/*===== VT__rtc_test3_cleanup =====*/
/**
@brief  assumes the post-condition of the test case execution

@param  None
  
@return On success - return TPASS
        On failure - return the error code
*/
/*================================================================================================*/
int VT_rtc_test3_cleanup(void)
{
        int rv = TFAIL;
        int ret = 0;
        
        /*==============================================================*/
        /* RTC_IRQP_READ & RTC_IRQP_SET                                 */
        /*==============================================================*/
        if( saved_periodic_rate )
        {
                ret = ioctl( file_desc, RTC_IRQP_SET, saved_periodic_rate );
                if( ret < 0 )
                {
                        tst_resm(TWARN, "ioctl RTC_IRQP_SET not SUPPORTED by platform" );
                        perror( "ioctl fails for RTC_IRQP_SET" );
                }
                else
                        tst_resm(TINFO, "Restore periodic IRQ rate = %ld Hz.", saved_periodic_rate );

        }
       
        /* close RTC driver file descriptor */
        ret = close(file_desc);
        
        /* Close returns -1 in case of failure */
        if (ret == -1)
        {
                tst_resm(TFAIL, "Close RTC driver fails: %s \n", strerror(errno));
        }
        else
        {
                rv = TPASS;
        } 

        return rv;
}


/*================================================================================================*/
/*===== VT_rtc_test3 =====*/
/**
@brief  Read and set periodic interrupt rate
                Enable and disable periodic interrupts

@param  None
  
@return On success - return TPASS
        On failure - return the error code
*/
/*================================================================================================*/
int VT_rtc_test3(void)
{
        int rv = TPASS;
        int retval = 0;
        /* --- variables used for the RTC_IRQP_READ & RTC_IRQP_SET tests*/
        int i;
        unsigned long Periodic_Rate = 0, data;
        unsigned long Periodic_Rate_2 = 0;
        
        tst_resm(TINFO, "RTC Driver Test 3");
        tst_resm(TINFO, "RTC periodic interrupts");
        
        /*==============================================================*/
        /* Test Read and Set Periodic Interrupts                        */
        /* RTC_IRQP_READ & RTC_IRQP_SET                                 */
        /*==============================================================*/
        
        /* Read periodic IRQ rate */
        
        tst_resm(TINFO, "Read periodic interrupt rate");
        retval = ioctl(file_desc, RTC_IRQP_READ, &Periodic_Rate);
        if (retval <0)
        {
                tst_resm(TWARN, "ioctl RTC_IRQP_READ not SUPPORTED by platform");
                perror("ioctl fails for RTC_IRQP_READ");

        }
        tst_resm(TINFO, "Periodic IRQ rate was %ld Hz.", Periodic_Rate);
    
        tst_resm(TINFO, "Counting 20 interrupts at:");
        
        
        /******** test for the periodic interrupt 2HZ~ 512HZ *********/
        
        /* The frequencies 128Hz, 256Hz, ... 8192Hz are only allowed for root. */
        
        for( Periodic_Rate = 2; Periodic_Rate <= 512; Periodic_Rate *=2 )
        {
        
                /* set the rtc_freq */
                tst_resm(TINFO, "Set RTC frequency to : %ld Hz", Periodic_Rate);                
                retval = ioctl( file_desc, RTC_IRQP_SET, Periodic_Rate );
                if( retval < 0 )
                {
                        tst_resm(TWARN, "ioctl RTC_IRQP_SET not SUPPORTED by platform" );
                        perror( "ioctl fails for RTC_IRQP_SET" );

                }

                retval = ioctl( file_desc, RTC_IRQP_READ, &Periodic_Rate_2 );
                if( retval < 0 ) 
                {
               
                        /* it's impossible */
               
                        tst_resm(TWARN, "ioctl RTC_IRQP_READ not SUPPORTED by platform" );
                        perror( "ioctl fails for RTC_IRQP_READ" );
                }
                else
                {
                        if( Periodic_Rate != Periodic_Rate_2 )
                        {
                                tst_resm( TFAIL, "Exposed and real values of periodic rate are not equal\n" );
                                return TFAIL;
                        }
                }                
                               
                /* Enable periodic interrupts in RTCISR[bit 7 ...bit 15] */
                
                tst_resm(TINFO, "Enable periodic interrupts" );
                retval = ioctl( file_desc, RTC_PIE_ON, 0 );
                if( retval < 0 )
                {
                        tst_resm(TWARN, "ioctl RTC_PIE_ON not SUPPORTED by platform" );
                        perror( "ioctl fails for RTC_PIE_ON" );
                }
                               
                for( i = 1; i < 21; i++ )
                {
                        /* This read blocks */
                        retval = read( file_desc, &data, sizeof(unsigned long) );
                        if( retval < 0 )
                        {
                                tst_resm( TFAIL, "ERROR : read fails \n" );
                                perror( "read" );
                                return rv;
                        }
                }
                
                /* Disable periodic interrupts */
                
                tst_resm(TINFO, "Disable periodic interrupts" );
                retval = ioctl( file_desc, RTC_PIE_OFF, 0 );
                if( retval < 0 )
                {
                        tst_resm(TWARN, "ioctl RTC_PIE_OFF not SUPPORTED by platform" );
                        perror( "ioctl fails for RTC_PIE_OFF");
                }
        
        }
        
                
        /*==============================================================*/
        /* END of Test Read and Set Periodic Interrupts                 */
        /*==============================================================*/
                
        
        return rv;
}

#ifdef __cplusplus
}
#endif

