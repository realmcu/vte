/*================================================================================================*/
/**
    @file   mxcfb_main.c

    @brief  main file for frame buffer test.
*/

#ifdef __cplusplus
extern "C"{
#endif

/* Standard Include Files */
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

/* Harness Specific Include Files. */
#include "test.h"
#include "usctest.h"

/* Verification Test Environment Include Files */
#include "mxcfb_test.h"

int      T_flag;        /* fisrt fb to open */
char     *T_opt;        /* fb name  */
int      d_flag;
char     *d_opt;
int      D_flag;
char     *D_opt;

option_t opts[] =
{
    { "T:", &T_flag, &T_opt},
    { "d:", &d_flag, &d_opt},
    { "D:", &D_flag, &D_opt},
    { NULL, NULL,    NULL}
};





/* Extern Global Variables */
extern int  Tst_count;               /* counter for tst_xxx routines.         */
extern char *TESTDIR;                /* temporary dir created by tst_tmpdir(...) */

/* Global Variables */
char *TCID     = "mxcfb";           /* test program identifier               */
int  TST_TOTAL = 1;                  /* total number of tests in this file    */

/*void cleanup();*/

void help(void);
void setup(void);
int main(int argc, char **argv);


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
void cleanup(void)
{
        int VT_rv = TFAIL;

        /* VTE : Actions needed to get a stable target environment */
        VT_rv = VT_fb_cleanup();
        if (VT_rv != TPASS)
        {
                tst_resm(TWARN, "VT_fb_cleanup() Failed : error code = %d", VT_rv);
        }
        /* Exit with appropriate return code. */
        tst_exit();
}

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
void setup(void)
{
    int VT_rv = TFAIL;
    /* VTE : Actions needed to prepare the test running */
    VT_rv = VT_fb_setup();
    if (VT_rv != TPASS)
    {
      tst_brkm(TBROK , cleanup, "VT_fb_setup() Failed : error code = %d", VT_rv);
    }

    return;
}


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
        VT_rv = VT_fb_test();
        /* VTE : print results and exit test scenario */
        if(VT_rv == TPASS)
                tst_resm(TPASS, "%s test case worked as expected", TCID);
        else
                tst_resm(TFAIL, "%s test case did NOT work as expected", TCID);
        cleanup();

        return VT_rv;
}

/*===== help =====*/
/**
@brief  Displays the program usage

@param  Input:  None
        Output: None

@return None
*/
void help(void)
{
        printf("Usage: -T <testcase ID> -D <fb background> -d <fb fore ground> \n");
        printf("ID: 1 color key test \n");
        printf("ID: 2 vsync test \n");
        printf("ID: 3 global alpha  test \n");
        printf("ID: 4 pan test \n");
        printf("ID: 5 overlay position  test \n");
        printf("ID: 6 draw color test \n");
        return;
}
