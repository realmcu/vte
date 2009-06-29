/***
**Copyright 2005-2009 Freescale Semiconductor, Inc. All Rights Reserved.
**
**The code contained herein is licensed under the GNU General Public
**License. You may obtain a copy of the GNU General Public License
**Version 2 or later at the following locations:
**
**http://www.opensource.org/licenses/gpl-license.html
**http://www.gnu.org/copyleft/gpl.html
**/
/*==================================================================================================
Revision History:
                            Modification     Tracking
Author                          Date          Number    Description of Changes
-------------------------   ------------    ----------  -------------------------------------------
D.Simakov                    05/04/2007      ENGR37674  Initial version
====================================================================================================
Portability: ARM GCC
 
==================================================================================================*/

/*==================================================================================================  
                                        INCLUDE FILES
==================================================================================================*/
/* Standard Include Files */
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <assert.h>
    
/* Harness Specific Include Files. */
#include <usctest.h>

/* Verification Test Environment Include Files */
#include "systest_test.h"

/*==================================================================================================
                                       GLOBAL VARIABLES
==================================================================================================*/
char * TCID = NULL;                  /* Test program identifier.          */
int  TST_TOTAL = 1;                  /* Total number of tests in this file.   */

sTestappConfig gTestappConfig;

/*==================================================================================================
                                   FUNCTION PROTOTYPES
==================================================================================================*/
void Cleanup ( void );
void Setup   ( void );
void Help    ( void );


/*================================================================================================*/
/*================================================================================================*/
void Cleanup( void )
{
        int rv;
        if (TPASS != (rv = VT_systest_cleanup()))
        {
                tst_resm( TWARN, "VT_systest_cleanup() Failed : error code = %d", rv );
        }
        
        tst_exit();
}


/*================================================================================================*/
/*================================================================================================*/
void Setup( void )
{
        int rv;                
        if (TPASS != (rv = VT_systest_setup()))
        {
                tst_brkm( TBROK , Cleanup, "VT_systest_setup() Failed : error code = %d", rv );
        }         
}


/*================================================================================================*/
/*================================================================================================*/
void Help( void )
{
        printf("\nUsage: systest_testapp <keys>\n");
        printf("Keys: \n" );
        printf("-V       Verbose mode.\n");
        printf("-T <Num> Thread to execute. Num must be in range [0..3]:\n");
        printf("         0 - capture images;\n");
        printf("         1 - sahara works in background;\n");        
        printf("         2 - BP memory not firandly process;\n");        
        printf("-D <devname> V4L capture device name. Default value is /dev/video0 \n");
        printf("-M <dirname> FFS mounting point. Default value is ./output \n");
        printf("-F <devname> FFS device name to be mounted: SD/MMC card or USB mass storage. Default value is /dev/mmcblk0p1 (MMC card)\n");
}


/*================================================================================================*/
/*================================================================================================*/
int main( int argc, char ** argv )
{
        int rv = TFAIL;
        
        TCID = "systest";                
        
        /* parse options. */        

        // Thread to execute.
        int ttexFlag = 0; char * ttexOpt = 0;
        
        // V4L device name.
        int v4lDevFlag = 0; char * v4lDevOpt = 0;
        
        // FFS mount point.
        int mountPtFlag = 0; char * mountPtOpt = 0;
        
        // FFS device name
        int ffsDevFlag = 0; char * ffsDevOpt = 0;
        
        char * msg;
        
        option_t options[] = 
        {
                { "V",   &gTestappConfig.mVerbose,  NULL },                
                { "T:",  &ttexFlag,                 &ttexOpt },  // thread to execute
                { "D:",  &v4lDevFlag,               &v4lDevOpt}, // v4l device name
                { "M:",  &mountPtFlag,              &mountPtOpt }, // ffs mount point
                { "F:",  &ffsDevFlag,               &ffsDevOpt }, // ffs device name
                { NULL,  NULL,                      NULL }
        };
        
        /* parse options. */
        if (NULL != (msg = parse_opts(argc, argv, options, Help)))
        {
                tst_brkm( TBROK, Cleanup, "%s(): Option parsing error - %s", msg );
        }
        
        // Thread to execute.
        gTestappConfig.mThreadToExecute = -1;                
        if (ttexFlag)
        {
                if (!ttexOpt)
                {
                        tst_resm(TCONF, "%s(): -T argument is required. Ignored.", __FUNCTION__);                
                }
                else
                {        
                        gTestappConfig.mThreadToExecute = atoi(ttexOpt);
                        if(gTestappConfig.mThreadToExecute < 0 || gTestappConfig.mThreadToExecute > 3)
                        {
                                tst_resm( TCONF, "%s(): -T argument must be in range [0..3]. Ignored.", __FUNCTION__ );
                                gTestappConfig.mThreadToExecute = -1;
                        }
                }
        }        
        
        // V4L device name.
        if (v4lDevFlag)
        {        
                if (!v4lDevOpt)
                {
                        tst_resm(TCONF, "%s(): -D argument is required", __FUNCTION__);
                        Help();
                        return TCONF;                                                                        
                }
                
                strncpy(gTestappConfig.mV4LDevName, v4lDevOpt, MAX_STR_LEN);                
        }
        else 
        {
                // Set to default.
                strncpy(gTestappConfig.mV4LDevName, "/dev/video0", MAX_STR_LEN);                                
        }
        
        // Mount point.
        if (mountPtFlag)
        {
                if (!mountPtOpt)
                {
                        tst_resm(TCONF, "%s(): -M argument is required", __FUNCTION__);
                        Help();
                        return TCONF;
                }
                
                strncpy(gTestappConfig.mMountPoint, mountPtOpt, MAX_STR_LEN);
        }
        else 
        {
                // Set to default.
                strncpy(gTestappConfig.mMountPoint, "output", MAX_STR_LEN);
        }
        
        // Ffs device name.
        if (ffsDevFlag)
        {
                if (!ffsDevOpt)
                {
                        tst_resm(TCONF, "%s(): -F argument is required", __FUNCTION__);
                        Help();
                        return TCONF;
                }
                
                strncpy(gTestappConfig.mFFSDevName, ffsDevOpt, MAX_STR_LEN);
        }
        else 
        {
                // Set to default
                strncpy(gTestappConfig.mFFSDevName, "/dev/mmcblk0p1", MAX_STR_LEN);
        }
                                                                                         

        /* Perform global test setup, call Setup() function. */
        Setup();
        
        /* Print test Assertion using tst_resm() function with argument TINFO. */
        tst_resm( TINFO, "Testing if %s test case is OK", TCID );
        
        /* VTE : print results and exit test scenario. */
        rv = VT_systest_test();
        
        if (TPASS == rv)
                tst_resm( TPASS, "%s test case worked as expected", TCID );                 
        else
                tst_resm( TFAIL, "%s test case did NOT work as expected", TCID );
       
        Cleanup(); 
        
        return rv;
}
