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
    @file   epdc_main.c

    @brief  main file for epdc frame buffer test.
*/

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
#include "epdc_test.h"

/* LOCAL MACROS */


/* LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS) */


/* LOCAL CONSTANTS */

/* LOCAL VARIABLES */

int      T_flag;        /* test case */
char      *T_opt;        /* test case */
int      d_flag;        /* fisrt fb to open */
char     *d_opt;        /* fb name  */
int      f_flag;        /* waveform setting*/
char     *f_opt;
int      g_flag;        /* pixel format*/
char     *g_opt;
int      t_flag;        /* tempture setting*/
char     *t_opt;
int      u_flag;        /* auto update*/
char      *u_opt;        /* auto update*/
int      s_flag;        /* send update setting*/
char     *s_opt;
int      R_flag;        /* Rotation */
char     *R_opt;

option_t opts[] =
{
    { "T:", &T_flag, &T_opt},
    { "d:", &d_flag, &d_opt},
    { "F:", &F_flag, &F_opt},
    { "g:", &g_flag, &g_opt},
    { "H:", &H_flag, &H_opt},
    { "u:", &u_flag, &u_opt},
    { "s:", &s_flag, &s_opt},
    { "R:", &R_flag, &R_opt},
    { NULL, NULL,    NULL}
};


/* GLOBAL VARIABLES */
epdc_opts m_opts;


/* Extern Global Variables */
extern int  Tst_count; /* counter for tst_xxx routines.*/
extern char *TESTDIR;  /* temporary dir created by tst_tmpdir(...) */
/* Global Variables */
char *TCID = "epdcfb"; /* test program identifier*/
int  TST_TOTAL = 6;  /* total number of tests in this file*/

/* GLOBAL FUNCTION PROTOTYPES */

/* LOCAL FUNCTION PROTOTYPES */
static void help(void);
static void setup(void);
static void cleanup();

/* help */
void help(void)
{
        printf("Usage:\n
		[-T <int> : special test\n
			0(setting framebuffer):
			1(pan test):
			2(draw test):\n
			3(wait update test):
			4(alt buffer overlay test):\n
			5(collision region update test)]:\n
			6(max update region count test)]\n
			7(1000 frames sequence region no collision frame rate test)]\n
		[-F <string> : wave form 0,1,3,2,2,2]\n
		[-g <int> : grayscale 0(normal):1(inverion)]\n
		[-H <int> : tempture ]\n
		-d /dev/fb0: fb device\n
		[-u <int> : auto update mode 0(partial)/1(full)]\n
		[-s <string>: send update with format only in partial update]\n
		[-r <int>: Rotation 0/1/2/3]\n
		");
        return;
}
/* cleanup */
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
  VT_rv = epdc_fb_cleanup();
  if (VT_rv != TPASS)
  {
   tst_resm(TWARN, "epdc_fb_cleanup() Failed : error code = %d", VT_rv);
  }
  /* Exit with appropriate return code. */
  tst_exit();
}

/* LOCAL FUNCTIONS */

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
		memset(m_opt, 0, sizeof(m_opt));
		m_opt.Tid = T_flag?atoi(T_opt):0;
		if(d_flag)
		  strcpy(m_opt.dev,d_opt);
		else
		  strcpy(m_opt.dev,"/dev/fb");
  /* perform global test setup, call setup() function. */
        setup();
 /* Print test Assertion using tst_resm() function with argument TINFO. */
        tst_resm(TINFO, "Testing if %s test case is OK", TCID);
        VT_rv = epdc_fb_test();
        /* VTE : print results and exit test scenario */
        if(VT_rv == TPASS)
          tst_resm(TPASS, "%s test case worked as expected", TCID);
        else
          tst_resm(TFAIL, "%s test case did NOT work as expected", TCID);
        cleanup();

        return VT_rv;
}
