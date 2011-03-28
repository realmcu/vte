/***
 * **Copyright (C) 2008-2009 Freescale Semiconductor, Inc. All Rights Reserved.
 * **
 * **The code contained herein is licensed under the GNU General Public
 * **License. You may obtain a copy of the GNU General Public License
 * **Version 2 or later at the following locations:
 * **
 * **http://www.opensource.org/licenses/gpl-license.html
 * **http://www.gnu.org/copyleft/gpl.html
 * **/
/*================================================================================================*/
/**
    @file   rtc_main.c

    @brief  RTC test 2 main file
*/
/*
Revision History:
                     Modification     Tracking
Author                    Date          Number    Description of Changes
-------------------------   ------------    ----------  -------------------------------------------
Blake               12/29/2008
====================================================================================================
Portability:  ARM GCC  gnu compiler
==================================================================================================*/

/*==================================================================================================
Total Tests: 1

Test Name:   rtc_testapp_7

Test Assertion
& Strategy:  Get and set RTC time and date
==================================================================================================*/


#ifdef __cplusplus
extern "C"{
#endif

/*==================================================================================================
                                        INCLUDE FILES
==================================================================================================*/
/* Standard Include Files */
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
    
/* Harness Specific Include Files. */
#include "test.h"
#include "usctest.h"

/* Verification Test Environment Include Files */
#include "rtc_test_7.h"

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


/*==================================================================================================
                                       GLOBAL CONSTANTS
==================================================================================================*/


/*==================================================================================================
                                       GLOBAL VARIABLES
==================================================================================================*/
/* Extern Global Variables */
extern int  Tst_count;               /* counter for tst_xxx routines.         */
extern char *TESTDIR;                /* temporary dir created by tst_tmpdir() */

/* Global Variables */
char *TCID     = "rtc_testapp_7"; /* test program identifier.          */
int  TST_TOTAL = 1;                  /* total number of tests in this file.   */

/*==================================================================================================
                                   GLOBAL FUNCTION PROTOTYPES
==================================================================================================*/
void cleanup(void);
void setup(void);
int main(int argc, char **argv);

/*==================================================================================================
                                   LOCAL FUNCTION PROTOTYPES
==================================================================================================*/


/*==================================================================================================
                                       GLOBAL FUNCTIONS
==================================================================================================*/

/*================================================================================================*/
/*===== cleanup =====*/
/**
@brief  Performs all one time clean up for this test on successful
        completion,  premature exit or  failure. Closes all temporary
        files, removes all temporary directories exits the test with
        appropriate return code by calling tst_exit() function.cleanup

@param  Input :      None.
        Output:      None.
  
@return Nothing
*/
/*================================================================================================*/
void cleanup(void)
{
        /* VTE : Actions needed to get a stable target environment */
        int VT_rv = TFAIL;

        VT_rv = VT_rtc_test7_cleanup();
        if (VT_rv != TPASS)
        {
                tst_resm(TWARN, "VT_cleanup() Failed : error code = %d", VT_rv);
        }

        tst_exit();
}

/*==================================================================================================
                                       LOCAL FUNCTIONS
==================================================================================================*/

/*================================================================================================*/
/*===== setup =====*/
/**
@brief  Performs all one time setup for this test. This function is
        typically used to capture signals, create temporary dirs
        and temporary files that may be used in the course of this test.

@param  Input :      None.
        Output:      None.
  
@return On failure - Exits by calling cleanup().
        On success - returns 0.
*/
/*================================================================================================*/
void setup(void)
{
        int VT_rv = TFAIL;
 
        VT_rv = VT_rtc_test7_setup();
        if (VT_rv != TPASS)
        {
                tst_brkm(TBROK , cleanup, "VT_setup() Failed : error code = %d", VT_rv);
        }
    
        return;
}


/*================================================================================================*/
/*===== main =====*/
/**
@brief  Entry point to this test-case. It parses all the command line
        inputs, calls the global setup and executes the test. It logs
        the test status and results appropriately using the LTP API's
        On successful completion or premature failure, cleanup() func
        is called and test exits with an appropriate return code.

@param  Input :      argc - number of command line parameters.
        Output:      **argv - pointer to the array of the command line parameters.
        Describe input arguments to this test-case
        -l - Number of iteration
        -v - Prints verbose output
        -V - Prints the version number
  
@return On failure - Exits by calling cleanup().
        On success - exits with 0 exit value.
*/
/*================================================================================================*/
int main(int argc, char **argv)
{
        int VT_rv = TFAIL;
        int t_opt = 0;
        int o_opt = 0;
        int seconds = 0;
        char *msg;
        char *alarm_time;
        char *on_off;

        /*Parse options*/
        option_t options[]=
        {
            {"T:",&t_opt,&alarm_time},
            {"O:",&o_opt,&on_off},
            {NULL,NULL,NULL}
        };
        if ((msg = parse_opts(argc,argv,options,&help))!=(char*)NULL)
        {
            tst_resm(TFAIL,"option parsing error - %s",msg);
            return TFAIL;
        }

        if (o_opt)
        {
           if ( !strcmp(on_off,"on"))
           {
               if (t_opt)
               {
                   seconds = atoi(alarm_time);
               }
               else
               {
                   help();
                   return VT_rv;
               }
               setup();
               tst_resm(TINFO, "Launching %s test", TCID);
               VT_rv = VT_rtc_alarm_on(seconds);
               if(VT_rv == TPASS)
               {
                   tst_resm(TPASS, "%s test case worked as expected", TCID);
               }
               else
               {
                   tst_resm(TFAIL, "%s test case did NOT work as expected", TCID);
               }
               /* cleanup allocated test ressources */
               cleanup();
               return VT_rv;
           }
           else if (!strcmp(on_off,"off"))
           {
               setup();
               tst_resm(TINFO, "Launching %s test", TCID);
               VT_rv = VT_rtc_alarm_off();
               if(VT_rv == TPASS)
               {
                   tst_resm(TPASS, "%s test case worked as expected", TCID);
               }
               else
               {
                   tst_resm(TFAIL, "%s test case did NOT work as expected", TCID);
               }
               /* cleanup allocated test ressources */
               cleanup();
               return VT_rv;
           }
           else
           {
               help();
               return VT_rv;
           }
        }
        else
        {
            help();
        }
  
        return VT_rv;	
}

/*===== help =====*/
/** 
@brief  Print help information testapp execution.

@param  Input :      None. 
        Output:      None. 

@return None  
*/
/*================================================================================================*/
void help(void)
{
        printf("RTC driver option\n");
        printf("  -O on/off\t enable/disable alarm\n");
        printf("  -T seconds\t Set RTC alarm time in second\n");
}

#ifdef __cplusplus
}
#endif
