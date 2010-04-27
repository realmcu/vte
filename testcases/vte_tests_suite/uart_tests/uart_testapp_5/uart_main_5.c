/***
**Copyright (C) 2005-2009 Freescale Semiconductor, Inc. All Rights Reserved.
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
    @file   uart_main.c

    @brief   UART test main function.
====================================================================================================
Revision History:
                            Modification     Tracking
Author                          Date          Number    Description of Changes
-------------------------   ------------    ----------  -------------------------------------------
I. Inkina / nknl001       07/04/2005     TLSbo48749   Initial version
I. Inkina / nknl001       07/04/2005     TLSbo49644   minor fix
I. Inkina / nknl001       08/06/2005     TLSbo51148   update options

====================================================================================================
Portability:  ARM GCC  gnu compiler
==================================================================================================*/

/*==================================================================================================
Total Tests: 1

Test Executable Name:  uart_testapp_5

Test Strategy:  A test for MXC and External UART serial drivers
=================================================================================================*/


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
#include "usctest.h"

/* Verification Test Environment Include Files */
#include "uart_test_5.h"

/*==================================================================================================
                                        LOCAL MACROS
==================================================================================================*/

/*==================================================================================================
                                       LOCAL VARIABLES
==================================================================================================*/
/* Binary flags : option is set or not */
int flag_s = 0;
char *s_opt;
 
/*==================================================================================================
                          LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
==================================================================================================*/
/* Options given to the UART test. Argument is required for speed option
  -S              detection name device
*/
option_t UART_options[] =
{
        { "s:", &flag_s, &s_opt },
        { NULL, NULL, NULL }
};

/*==================================================================================================
                                       LOCAL CONSTANTS
==================================================================================================*/

/*==================================================================================================
                                       GLOBAL CONSTANTS
==================================================================================================*/


/*==================================================================================================
                                       GLOBAL VARIABLES
==================================================================================================*/
/* Extern Global Variables */
extern int  Tst_count;               /* counter for tst_xxx routines.         */
extern char *TESTDIR;                /* temporary dir created by tst_tmpdir(void) */

/* Global Variables */
char *TCID     = "uart_testapp_5"; /* test program identifier.          */
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
        
        VT_rv = VT_mxc_uart_test5_cleanup();
        if (VT_rv != TPASS )
        {
                tst_resm(TWARN, "VT_cleanup() Failed : error code = %d", VT_rv);
        }
        
        /* Exit with appropriate return code. */
        tst_exit();
}

/*==================================================================================================
                                       LOCAL FUNCTIONS
==================================================================================================*/

/*================================================================================================*/
/*===== help =====*/
/**
@brief  Inform of the available options and the associated parameters

@param  Input :      None.
        Output:      None.

@return On failure - Exits by calling cleanup(void).
        On success - returns 0.
*/
/*================================================================================================*/
void help(void)
{
        printf("Options:\n\n");
        printf("-S <name source device>  : test case option-> testing of source device  \n");

}

/*================================================================================================*/
/*===== setup =====*/
/**
@brief  Performs all one time setup for this test. This function is
        typically used to capture signals, create temporary dirs
        and temporary files that may be used in the course of this test.

@param  Input :      UART_type
        Output:      None.

@return On failure - Exits by calling cleanup().
        On success - returns 0.
*/
/*================================================================================================*/
void setup(void)
{
        int VT_rv = TFAIL;
        
        /* VTE : Actions needed to prepare the test running */
        VT_rv = VT_mxc_uart_test5_setup();
        if (VT_rv != TPASS)
        {
                tst_brkm(TBROK , cleanup, "VT_mxc_uart_setup() Failed : error code = %d", VT_rv);
        }
        
        return;
}

/*================================================================================================*/
/*===== main =====*/
/**
@brief  Entry point to this test-case. It parses all the command line
        inputs, calls the global setup and executes the test. It logs
        the test status and results appropriately using the LTP API's
        On successful completion or premature failure, cleanup(void) func
        is called and test exits with an appropriate return code.

@param  Input :      argc - number of command line parameters.
        Output:      **argv - pointer to the array of the command line parameters.

@return On failure - Exits by calling cleanup(void).
        On success - exits with 0 exit value.
*/
/*================================================================================================*/
int main(int argc, char **argv)
{
        int VT_rv = TFAIL;
        char *msg;
        
        int type_1 = MXC_PORT_NUMBER;
        char *driver_name;
        
        
        driver_name=(char*)calloc(sizeof(char),15);
        
        
        /* Print test Assertion using tst_resm() function with argument TINFO. */
        if( NULL !=( msg=parse_opts(argc, argv, UART_options, help)))
        {
                free(driver_name);
                tst_brkm(TBROK,cleanup , "Option parsing error - %s" , msg);
        }
        
        if(flag_s)
        {
                sprintf(driver_name ,"%s",s_opt);
        }
        else  if(!flag_s )
        {
                free(driver_name);
                tst_brkm(TBROK,cleanup , "Option parsing error ");
        }
        
        
        
        /* perform global test setup, call setup() function. */
        setup();
        
        VT_rv =  VT_mxc_uart_test5(driver_name,type_1 );
        
        if(VT_rv == TPASS)
        {
                tst_resm(TPASS, "test case %s worked as expected", TCID);
        }
        else
        {
                tst_resm(TFAIL, "test case %s did NOT work as expected", TCID);
        }
        
        
        /* cleanup test allocated ressources */	
        cleanup(); 
        
        
        free(driver_name);
        
        return VT_rv;
}


#ifdef __cplusplus
}
#endif

