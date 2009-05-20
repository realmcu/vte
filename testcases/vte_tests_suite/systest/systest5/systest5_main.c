/*======================

        Copyright (C) 2007, Freescale Semiconductor, Inc. All Rights Reserved
        THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
        BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
        Freescale Semiconductor, Inc.

====================
Revision History:
                            Modification     Tracking
Author                          Date          Number    Description of Changes
-------------------------   ------------    ----------  -------------------------------------------
D.Kardakov                    13/06/2007     ENGR37681    Initial version
====================
Portability: ARM GCC

======================*/

/*======================
                                        INCLUDE FILES
======================*/
/* Standard Include Files */
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <assert.h>

/* Harness Specific Include Files. */
#include <usctest.h>

/* Verification Test Environment Include Files */
#include "systest5_test.h"

/*======================
                                       GLOBAL VARIABLES
======================*/
char * TCID = NULL;                  /* Test program identifier.          */
int  TST_TOTAL = 1;                  /* Total number of tests in this file.   */

sTestappConfig gTestappConfig;

/*======================
                                   FUNCTION PROTOTYPES
======================*/
void Cleanup ( void );
void Setup   ( void );
void Help    ( void );


/*====================*/
/*====================*/
void Cleanup( void )
{
        int rv;
        if (TPASS != (rv = VT_systest_cleanup()))
        {
                tst_resm( TWARN, "VT_systest_cleanup() Failed : error code = %d", rv );
        }

        tst_exit();
}


/*====================*/
/*====================*/
void Setup( void )
{
        int rv;
        if (TPASS != (rv = VT_systest_setup()))
        {
                tst_brkm( TBROK , Cleanup, "VT_systest_setup() Failed : error code = %d", rv );
        }
}


/*====================*/
/*====================*/
void Help( void )
{
        printf("\nUsage: systest_testapp <keys>\n");
        printf("Keys: \n" );
        printf("-V       Verbose mode.\n");
        printf("-T <Num> Thread to execute. Num must be in range [0..3]:\n");
        printf("         0 - VPU enc/dec thread;\n");
        printf("         1 - BP/AP communication over SDMA;\n");
}


/*====================*/
/*====================*/
int main( int argc, char ** argv )
{
        int rv = TFAIL;

        TCID = "systest";

        /* parse options. */

        // Thread to execute.
        int ttexFlag = 0; char * ttexOpt = 0;

        char * msg;

        option_t options[] =
        {
                { "V",   &gTestappConfig.mVerbose,  NULL },
                { "T:",  &ttexFlag,                 &ttexOpt },  // thread to execute
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
