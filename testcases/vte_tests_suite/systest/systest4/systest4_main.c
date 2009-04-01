/*================================================================================================== 
 
        Copyright (C) 2007, Freescale Semiconductor, Inc. All Rights Reserved 
        THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT 
        BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF 
        Freescale Semiconductor, Inc. 

====================================================================================================
Revision History:
                            Modification     Tracking
Author/Core ID                  Date          Number     Description of Changes
-------------------------   ------------    -----------  -------------------------------------------
A.Ozerov/b00320              29/05/2007      ENGR37685   Initial version
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
#include "systest4_test.h"

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
        if (TPASS != (rv = VT_systest2_cleanup()))
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
        if (TPASS != (rv = VT_systest2_setup()))
        {
                tst_brkm( TBROK , Cleanup, "VT_systest_setup() Failed : error code = %d", rv );
        }         
}


/*================================================================================================*/
/*================================================================================================*/
void Help( void )
{
        printf("\nUsage: systest2_testapp <keys>\n");
        printf("If you use keys, you must use both of them\n");
        printf("Keys: \n" );
        printf("-D1 <devname> MMC/SD 1 device name. Default value is /dev/mmcblk0p1 \n");
        printf("-D2 <devname> MMC/SD 2 device name. Default value is /dev/mmcblk1p1 \n\n");
}

/*================================================================================================*/
/*================================================================================================*/
int main( int argc, char ** argv )
{
        int rv = TFAIL;
        
        TCID = "systest";                
        
        /* parse options */        

        /* MMC/SD 1 device name */
        int MMC1DevFlag = 0; char * MMC1DevOpt = 0;
        
        /* MMC/SD 2 device name */
        int MMC2DevFlag = 0; char * MMC2DevOpt = 0;

        char * msg;
        
        option_t options[] = 
        {
                { "d:", &MMC1DevFlag, &MMC1DevOpt}, /* MMC/SD 1 device name */
                { "D:", &MMC2DevFlag, &MMC2DevOpt}, /* MMC/SD 2 device name */
                { NULL, NULL, NULL }
        };
        
        /* parse options */
        if(NULL != (msg = parse_opts(argc, argv, options, Help)))
        {
                tst_brkm( TBROK, Cleanup, "%s(): Option parsing error - %s", msg );
        }
        
        /* Thread to execute */
        gTestappConfig.mThreadToExecute = -1;
 
        /* MMC/SD 1 device name */
        if(MMC1DevFlag)
        {        
                if(MMC1DevOpt == 0 || MMC2DevFlag == 0 || MMC2DevOpt == 0)
                {
                        tst_resm(TCONF, "%s(): -D1 and -D2 arguments are both required", __FUNCTION__);
                        Help();
                        return TCONF;                                                                        
                }
                strncpy(gTestappConfig.mmc1DevName, MMC1DevOpt, MAX_STR_LEN);
                strncpy(gTestappConfig.mmc2DevName, MMC2DevOpt, MAX_STR_LEN);
        }
        else 
        {
                /* Set to default */
                strncpy(gTestappConfig.mmc1DevName, "/dev/mmcblk0p1", MAX_STR_LEN);
                strncpy(gTestappConfig.mmc2DevName, "/dev/mmcblk1p1", MAX_STR_LEN);
        }
        
        /* Perform global test setup, call Setup() function. */
        Setup();
        
        /* Print test Assertion using tst_resm() function with argument TINFO. */
        tst_resm( TINFO, "Testing if %s test case is OK", TCID );
        
        /* VTE : print results and exit test scenario. */
        rv = VT_systest2_test();
        
        if (TPASS == rv)
                tst_resm( TPASS, "%s test case worked as expected", TCID );                 
        else
                tst_resm( TFAIL, "%s test case did NOT work as expected", TCID );
       
        Cleanup(); 
        
        return rv;
}
