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
/*================================================================================================*/
/**
    @file   uart6_main.c

    @brief  First UART test main function.
====================================================================================================
Revision History:
                                
Author                       Date            CR Number        Description of Changes
-----------------   ----------   ------------    ---------- ----------------------------
E.Gromazina              24/04/2005         TLSbo48749     Initial version
I. Inkina / nknl001      08/06/2005         TLSbo51148     update options  

====================================================================================================
Portability:  ARM GCC  gnu compiler
==================================================================================================*/

/*==================================================================================================
Total Tests:  TO BE COMPLETED

Test Executable Name:  uart_testapp_6

Test Strategy:  A test for MXC UART and External UART serial drivers
=================================================================================================*/


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
#include "uart_test_6.h"

/*==================================================================================================
                                       LOCAL VARIABLES
==================================================================================================*/
/* Binary flags : option is set or not */
int flag_s = 0;
int flag_d = 0;
int flag_parity = 0;
int flag_break = 0;
  
/* Option arguments */
char *opt_s;
char *opt_d;
char *parityopt;
char *breakopt;
 

/*==================================================================================================
                          LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
==================================================================================================*/
/* Options given to the UART test. Argument is required for speed option
   -s              detection name source  device 
   -d              detection name destination  device 
   -T <D or I> : D - detection parity test, I - ignore parity test
   -B <D or I> : D - detection break test, I - ignore break test
 */
    
option_t options[] =
{
        { "s:", &flag_s, &opt_s },
        { "d:", &flag_d, &opt_d },
        { "T:", &flag_parity, &parityopt },
        { "B:", &flag_break, &breakopt },
        { NULL, NULL, NULL }
};

  
/*==================================================================================================
                                       GLOBAL VARIABLES
==================================================================================================*/
/* Extern Global Variables */
extern int  Tst_count;               /* counter for tst_xxx routines.         */
extern char *TESTDIR;                /* temporary dir created by tst_tmpdir(void) */

/* Global Variables */
char *TCID     = "uart_testapp_6"; /* test program identifier.          */
int  TST_TOTAL = 1;                  /* total number of tests in this file.   */
  
param_t *par = NULL;

/*==================================================================================================
                                   GLOBAL FUNCTION PROTOTYPES
==================================================================================================*/
void cleanup(void);
void def_param(param_t *);
void setup(void);
int main(int argc, char **argv);

/*==================================================================================================
                                       GLOBAL FUNCTIONS
==================================================================================================*/

/*================================================================================================*/
/*===== setup =====*/
/**
@brief  Performs all one time setup for this test. This function is
        typically used to capture signals, create temporary dirs
        and temporary files that may be used in the course of this test.

@param  Input :      UART_type
        Sending_UART
        Receiving_UART
        Output:      None.
  
@return On failure - Exits by calling cleanup(void).
        On success - returns 0.
*/
/*================================================================================================*/
void setup(void)
{
        int VT_rv = TFAIL;
        
        /* VTE : Actions needed to prepare the test running */
        VT_rv = VT_uart_test_setup(par);
        if (VT_rv != TPASS)
        {
                tst_brkm(TBROK , cleanup, "VT_mxc_uart_setup() Failed : error code = %d", VT_rv);
        }
        return;
}


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
        
        if(par)
        {
                free(par);
                par=NULL;
        }
        
        VT_rv = VT_uart_test_cleanup();
        
        if (VT_rv != TPASS)
        {
                tst_resm(TWARN, "VT_cleanup() Failed : error code = %d", VT_rv);
        }
        
        /* Exit with appropriate return code. */
        tst_exit();
}

/*======================== LOCAL FUNCTION ========================*/
/*===== help =====*/
/**
@brief  Inform of the available options and the associated parameters

@param  Input :      None.
        Output:      None.
  
@return None.
*/
/*================================================================================================*/
void help(void)
{	
        printf("Options:\n\n");
        
        printf("-s <name source device>  : test case option-> testing of source device  \n");
        printf("-d <name destination device>-> testing of destination device  \n");
        printf("-T <D or I> :  D - detection parity test, I - ignore parity test\n");
        printf("-B <D or I> :  D - detection break test, I - ignore break test\n");

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
            **argv - pointer to the array of the command line parameters.
 		Output: 				
@return On failure - Exits by calling cleanup().
        On success - exits with 0 exit value.
*/
/*================================================================================================*/
int main(int argc, char **argv)
{
        int VT_rv = TFAIL;
        char *msg;
        
        par = malloc( sizeof(param_t));
        def_param(par);

        /* Parse user defined options */
        
        if( NULL != (msg = parse_opts( argc, argv, options, help )) )
        {
                tst_resm(TFAIL, "OPTION PARSING ERROR - %s");
        }

        if(!(flag_s && flag_d))
        {
                tst_resm(TFAIL, "OPTION ERROR ");
                return TFAIL;
        }

        if(flag_s)
                sprintf(par->UART1_drive,"%s",opt_s);
        if(flag_d)
                sprintf(par->UART2_drive,"%s",opt_d);
                
        if(flag_parity)
        {
                
                par->parity_con = parityopt[0];
                switch (par->parity_con)
                {
                case 'D':  ;
                        break;
                case 'I':   ;
                        break;
                default:  tst_resm(TWARN, "WARN : parity opt is incorrect, the parameters by default will be established!"); 
                }		
        }
                
        if(flag_break)
        {
                par->break_con = breakopt[0];
                switch (par->break_con)
                {
                case 'D': 
                        break;
                case 'I':  
                        break;
                default:  tst_resm(TWARN, "WARN : break opt is incorrect, the parameters by default will be established!"); 
                }
        }		
 
        /* perform global test setup, call setup() function. */
        setup();
        
        /* Print test Assertion using tst_resm() function with argument TINFO. */
        tst_resm(TINFO, "Testing if %s test case is OK\n", TCID);
        
        /* VTE : Execute test, print results and exit test scenario */
        VT_rv = VT_uart_test6(par);
        
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
        
        return VT_rv;
}

/*==============================================================================================*/
/*=====  def_param =====*/
/**
@brief  Set of default parameters
  
*/
/*==============================================================================================*/
void def_param(param_t *p)
{	
        p->parity_con =  PARITY_DEF;
        p->break_con =  BREAK_DEF; 
        
}

#ifdef __cplusplus
}
#endif
