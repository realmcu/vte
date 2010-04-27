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
    @file   sdma_tty_main.c

    @brief   SDMA TTY test main function.
====================================================================================================
Revision History:
                            Modification     Tracking
Author                          Date          Number    Description of Changes
-------------------------   ------------    ----------  -------------------------------------------
V. ZAVJALOV/----             19/07/2004     TLSbo40259  Initial version
E.Gromazina                  31/10/2005     TLSbo56685  Fix bag
====================================================================================================
Portability:  ARM GCC  gnu compiler
==================================================================================================*/

/*==================================================================================================
Total Tests: 1

Test Executable Name:  sdma_tty

Test Strategy:
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
#include "sdma_tty_test.h"

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
extern char *TESTDIR;                /* temporary dir created by tst_tmpdir(void) */

/* Global Variables */
char *TCID     = "sdma_tty";              /* test program identifier.          */
int  TST_TOTAL = 1;                  /* total number of tests in this file.   */

int num_param, r_buf_len_param, n_num=0, r_num=0, s_num=0;
char *n_copt, *r_copt, *s_copt, *string_param;

option_t options[] = {
        { "s:", &s_num, &s_copt },                        /* String */
        { "r:", &r_num, &r_copt},                        /* Read buffer length */
        { "n:", &n_num, &n_copt},                         /* Number of string */
        { NULL, NULL, NULL }                    /* NULL required to end array */
};

/*==================================================================================================
                                    GLOBAL FUNCTION PROTOTYPES
==================================================================================================*/
void cleanup(void);
void setup(void);
int main(int argc, char **argv);
void help(void);

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
    
@return Nothing*/
/*================================================================================================*/
void cleanup(void)
{
    /* VTE : Actions needed to get a stable target environment */
        int VT_rv = TFAIL;
                
        VT_rv = VT_sdma_tty_test_cleanup();
        if (VT_rv != TPASS)
        {
                tst_resm(TWARN, "VT_sdma_tty_cleanup() Failed : error code = %d", VT_rv);
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
        On success - returns 0.*/
/*================================================================================================*/
void setup(void)
{
        int VT_rv = TFAIL;
        
    /* VTE : Actions needed to prepare the test running */
        VT_rv = VT_sdma_tty_test_setup();
        if (VT_rv != TPASS)
        {
                tst_brkm(TBROK , cleanup, "VT_sdma_tty_setup() Failed : error code = %d", VT_rv);
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
        On success - exits with 0 exit value.*/
/*================================================================================================*/
int main(int argc, char **argv)
{
        int VT_rv = TFAIL;
        char *msg;

        if ( (msg = parse_opts(argc, argv, options, &help)) != (char *) NULL )
                tst_brkm(TBROK, cleanup, "OPTION PARSING ERROR - %s", msg);;

        if (n_num)
        {
            num_param = atoi( n_copt );
            if (num_param < 1)
                  tst_brkm(TBROK, cleanup, "Invalid arg for -n: %s", n_copt);
        }
        else
            tst_brkm(TBROK, cleanup, "Required arg -n");        

        if (r_num)
        {
            r_buf_len_param = atoi(r_copt);
            if (r_buf_len_param < 0)
              tst_brkm(TBROK, cleanup, "Invalid arg for -r: %s", r_copt);
        }
        else
           tst_brkm(TBROK, cleanup, "Required arg -r");

        if (s_num)
           string_param = s_copt;
        else
           tst_brkm(TBROK, cleanup, "Required arg -s");
        
        /* perform global test setup, call setup() function. */
        setup();

        /* Print test Assertion using tst_resm() function with argument TINFO. */
        tst_resm(TINFO, "Testing if %s test case is OK", TCID);

        /* VTE : Execute test, print results and exit test scenario */
        VT_rv = VT_sdma_tty_test();
        
        if(VT_rv == TPASS)
            tst_resm(TPASS, "test case %s worked as expected", TCID);
        else
            tst_resm(TFAIL, "test case %s did NOT work as expected", TCID);
                
        /* cleanup test allocated ressources */        
        cleanup(); 

        return VT_rv;
}

#ifdef __cplusplus
}
#endif
