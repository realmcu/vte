/***
**Copyright 2004-2009 Freescale Semiconductor, Inc. All Rights Reserved.
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
    @file   lcd_power_main.c

    @brief  main file of the sleep test application that checks SLCDC driver by
            putting device to the various VESA blanking levels.
     
====================================================================================================
Revision History:
                            Modification     Tracking
Author                          Date          Number    Description of Changes
-------------------------   ------------    ----------  -------------------------------------------
I. Semenchukov/smng001c      21/09/2004     TLSbo41672   Initial version 
L.Delaspre/rc149c            15/12/2004     TLSbo44058   Invalid argument issue investigation
E.Gromazina 					19.08.2005	TLSbo53875	Renaming test

====================================================================================================
Portability: Indicate if this module is portable to other compilers or platforms. 
             If not, indicate specific reasons why is it not portable.

==================================================================================================*/

/*==================================================================================================
Total Tests:    1

Test Name:      lsd_testapp_power

Test Assertion
& Strategy:     Test checks sleeping capabilities of the Epson SLCD Controller Driver
                All operations performs via the framebuffer (FB) device file. After opening this
                file, its memory is mapped into the process memory area and the device is turned
                to the various blanking modes.
                The following FB ioctl calls are tested:
                    FBIOGET_FSCREENINFO
                    FBIOGET_VSCREENINFO
                    FBIOBLANK
                
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
#include "lcd_power_test.h"

/*==================================================================================================
                                        LOCAL MACROS
==================================================================================================*/


/*==================================================================================================
                          LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
==================================================================================================*/


/*==================================================================================================
                                       LOCAL CONSTANTS
==================================================================================================*/
#if !defined(TRUE) && !defined(FALSE)
#define TRUE  1
#define FALSE 0
#endif

/*==================================================================================================
                                       LOCAL VARIABLES
==================================================================================================*/
int      F_flag;        /* Option flag       */
char     *F_opt;        /* Option argument   */
option_t opts[] =
{
    { "F:", &F_flag, &F_opt },
    { NULL, NULL,    NULL   }
};
         


/*==================================================================================================
                                       GLOBAL CONSTANTS
==================================================================================================*/


/*==================================================================================================
                                       GLOBAL VARIABLES
==================================================================================================*/
/* Extern Global Variables */
extern int  Tst_count;               /* counter for tst_xxx routines.         */
extern char *TESTDIR;                /* temporary dir created by tst_tmpdir(...) */

/* Global Variables */
char *TCID     = "power";           /* test program identifier               */
int  TST_TOTAL = 1;                  /* total number of tests in this file    */

/*==================================================================================================
                                   GLOBAL FUNCTION PROTOTYPES
==================================================================================================*/
/*void cleanup();*/

/*==================================================================================================
                                   LOCAL FUNCTION PROTOTYPES
==================================================================================================*/
void help(void);
void setup(void);
int main(int argc, char **argv);


/*==================================================================================================
                                       GLOBAL FUNCTIONS
==================================================================================================*/

/*================================================================================================*/
/*===== cleanup =====*/
/**
@brief  Performs all one time clean up for this test on successful
        completion,  premature exit or  failure. Closes all temporary
        files, removes all temporary directories exits the test with
        appropriate return code by calling tst_exit(...) function.cleanup

@param  Input :      None.
        Output:      None.
  
@return Nothing
*/
/*================================================================================================*/
void cleanup(void)
{
        int VT_rv = TFAIL;
        
        /* VTE : Actions needed to get a stable target environment */
        VT_rv = VT_power_cleanup();
        if (VT_rv != TPASS)
        {
                tst_resm(TWARN, "VT_power_cleanup() Failed : error code = %d", VT_rv);
        }
        /* Exit with appropriate return code. */
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
        
        /* VTE : Actions needed to prepare the test running */
        VT_rv = VT_power_setup();
        if (VT_rv != TPASS)
        {
                tst_brkm(TBROK , cleanup, "VT_power_setup() Failed : error code = %d", VT_rv);
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

@param  Input :  argc - number of command line parameters.
                 **argv - pointer to the array of the command line parameters.
        Output:  None
  
@return On failure - Exits by calling cleanup().
        On success - exits with 0 exit value.
*/
/*================================================================================================*/
int main(int argc, char **argv)
{
        int  VT_rv = TFAIL;
        char *message;      /* From parse_opts() */
        
        if ( (message = parse_opts(argc, argv, opts, &help)) != NULL)
        {
                printf("An error occured while parsing options: %s\n", message);
                return VT_rv;
        }
        /* perform global test setup, call setup() function. */
        setup();
        /* Print test Assertion using tst_resm() function with argument TINFO. */
        tst_resm(TINFO, "Testing if %s test case is OK", TCID);
        VT_rv = VT_power_test();
        /* VTE : print results and exit test scenario */
        if(VT_rv == TPASS)
                tst_resm(TPASS, "%s test case worked as expected", TCID);
        else
                tst_resm(TFAIL, "%s test case did NOT work as expected", TCID);
        cleanup();
        
        return VT_rv;
}

/*================================================================================================*/
/*===== help =====*/
/**
@brief  Displays the program usage

@param  Input:  None
        Output: None
  
@return None
*/
/*================================================================================================*/
void help(void)
{
        printf("Usage: %s -F path_to_fb_device_file\n", TCID);
        return;
}
