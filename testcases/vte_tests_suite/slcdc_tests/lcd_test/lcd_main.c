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
@file   lcd_main.c

@brief  main file of the fbdraw test application that checks SLCDC driver by
producing simple output to Epson fb.

====================================================================================================
Revision History:
Modification     Tracking
Author                          Date          Number    Description of Changes
-------------------------   ------------    ----------  -------------------------------------------
I. Semenchukov/smng001c      16/09/2004     TLSbo41672   Initial version 
Artyom Smirnov                1/07/2005     TLSbo51716   Tests reorganization
E.Gromazina                  12/08/2005     TLSbo53875	Test enhancement

====================================================================================================
Portability: Indicate if this module is portable to other compilers or platforms. 
If not, indicate specific reasons why is it not portable.

==================================================================================================*/

/*==================================================================================================
Total Tests:    4
====================================================================================================
Test Name:      fbdraw_test

Test Assertion
& Strategy:     Test performs simple checks of the Epson SLCD Controller Driver functionality
by drawing base eight color patterns and one flower picture to the LCD.
All operations performs via the framebuffer (FB) device file. After opening this
file, its memory is mapped into the process memory area and then pixel color bytes
is written into this memory. The test is color depth independent: all color values
initially stored in three bytes (r, g, b), and then, if necessary, conversion is
performed.
The following FB ioctl calls are tested:
FBIOGET_FSCREENINFO
FBIOGET_VSCREENINFO

==================================================================================================*/
/*==================================================================================================
Test Name:      cursor_test

Test Assertion
& Strategy:     Test checks cursor operations of the Epson SLCD Controller Driver.
All operations performs via the framebuffer (FB) device file. After opening this
file, its memory is mapped into the process memory area and various cursor
operations are performed.
The following FB ioctl calls are tested:
FBIOGET_FSCREENINFO
FBIOGET_VSCREENINFO
FBIO_CURSOR

==================================================================================================*/
/*==================================================================================================
Test Name:      scroll_test

Test Assertion
& Strategy:     Test performs simple checks of the Epson SLCD Controller Driver functionality
by scrolling contents of the flower picture drawn on the LCD.
All operations performs via the framebuffer (FB) device file. After opening this
file, its memory is mapped into the process memory area and then pixel color bytes
is written into this memory. The test is color depth independent: all color values
initially stored in three bytes (r, g, b), and then, if necessary, conversion is
performed.
The following FB ioctl calls are tested:
FBIOGET_FSCREENINFO
FBIOGET_VSCREENINFO
FBIOPAN_DISPLAY

==================================================================================================*/
/*
* former Sharp LCD test 
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
#include "lcd_test.h"

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
int T_flag = 0;	/* Option flags      */
int B_flag = 0;
int O_flag = 0;
int P_flag = 0;        
int X_flag = 0;
int N_flag = 0;
int R_flag = 0;


char *T_opt;	 /* Option arguments   */
char *B_opt;
char *O_opt;
char *P_opt;       
char *X_opt;  
char *N_opt;
char *R_opt;


option_t opts[] =
{
        { "T:", &T_flag, &T_opt },
        { "B:", &B_flag, &B_opt },
        { "O:", &O_flag, &O_opt },
        { "D:", &P_flag, &P_opt },
        { "X:", &X_flag, &X_opt },
        { "N:", &N_flag, &N_opt },
        { "R:", &R_flag, &R_opt },
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
char *TCID     = "lcd_testapp";           /* test program identifier               */
int  TST_TOTAL = 3;                  /* total number of tests in this file    */

int testcase_nb;
char fb_path[PATH_LEN];
char fb_path_1[PATH_LEN];
int bpp;
int wait_sec;
int run_times = 1;

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
        VT_rv = VT_lcd_cleanup();
        if (VT_rv != TPASS)
        {
                tst_resm(TWARN, "VT_fbdraw_cleanup() Failed : error code = %d", VT_rv);
        }
        /* Exit with appropriate return code. */
	if (!O_flag)
 	       tst_exit();		
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
        VT_rv = VT_lcd_setup();
        if (VT_rv != TPASS)
        {
                tst_brkm(TBROK , cleanup, "VT_lcd_setup() Failed : error code = %d", VT_rv);
        }
        
        return;
}


/*================================================================================================*/
/*===== main =====*/
/**
@brief  Entry point to this test-case. It parses all the command line
inputs, calls the global setup and executes the test. It logs
the test status and results appropriately using the LTP API's
On successful completion or premature failure, cleanup(...) func
is called and test exits with an appropriate return code.

@param  Input : argc - number of command line parameters.
                **argv - pointer to the array of the command line parameters.
        Output:  None
 
@return On failure - Exits by calling cleanup(...).
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

	if (X_flag){
                wait_sec = atoi(X_opt);
                tst_resm(TINFO, "Disable user enquiry\n");
	}
	if (N_flag)
                run_times = atoi(N_opt);	
        
        if (T_flag)
                testcase_nb = atoi(T_opt);
        else
                testcase_nb = 1;

        if (B_flag)
                sprintf(fb_path, "%s", B_opt);
        else
        {
                tst_resm(TINFO, "Enter path_to_fb_device file value");
                help();
                return VT_rv;
        }
        if (O_flag)
                sprintf(fb_path_1, "%s", O_opt);
        if (P_flag)
        {
                bpp = atoi(P_opt);
        } 
        else 
        {
                bpp = 16; 
        }
        if ((bpp != 16) && (bpp != 24) && (bpp != 32))
        {
                tst_resm(TINFO, "You entered wrong BPP value");
                help();
                return VT_rv;
        }
	int i, j;
	int rc = 0;
	int tn = 1;
	if (R_flag)
		tn = atoi(R_opt);

       tst_resm(TINFO, "Testing if %s test case is OK", TCID);	        
       tst_resm(TINFO, "Execute time is %d", run_times);
       tst_resm(TINFO, "Repeat time is %d", tn);
	   
        /* perform global test setup, call setup() function. */

	if (R_flag && O_flag)
	{
		for (i = 0; i < run_times; i++)
		{	
			if (i%2 == 1)
	                sprintf(fb_path, "%s", B_opt);
			else
	                sprintf(fb_path, "%s", O_opt);

			setup();
			for (j = 0; j < tn; j++)
			{
			        VT_rv = VT_lcd_test();
			       tst_resm(TINFO, "Current rv is %d", VT_rv);
				  rc = rc | VT_rv;
			       tst_resm(TINFO, "Current rc is %d", rc);

				 if (rc)
				 {
			      		tst_resm(TINFO, "Error!!! Current time is %d", i);	
					break;
				 }
			}
			cleanup();
		}
		if(rc == TPASS)
	                tst_resm(TPASS, "%s test case worked as expected", TCID);
	        else
	                tst_resm(TFAIL, "%s test case did NOT work as expected", TCID);
  	       tst_exit();

	}
	else{
		setup();

		for (i = 0; i < run_times; i++)
		{	        
		      // tst_resm(TINFO, "Current time is %d", i+1);
		        /* Print test Assertion using tst_resm() function with argument TINFO. */
		        VT_rv = VT_lcd_test();

			 if (VT_rv)
			 {
		      		tst_resm(TINFO, "Error!!! Current time is %d", i);	
				break;
			 }
		}
		if(VT_rv == TPASS)
	                tst_resm(TPASS, "%s test case worked as expected", TCID);
	        else
	                tst_resm(TFAIL, "%s test case did NOT work as expected", TCID);
		cleanup();

	}
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
        printf("Usage: %s -T <testcase_number> \n", TCID);
        printf("testcase_number: \n");
        printf("\t	1 - color test \n");
        printf("\t	2 - lcd test \n"); 
        printf("\t	3 - HW cursor test \n"); 
        printf("\t	4 - scroll test \n");
        printf("\t	5 - sharp test \n");
        printf("\t	6 - sharp test \n");
        printf("Usage: %s -B <path to fb device file> \n", TCID);
        printf("Usage: %s -O <path to 2nd fb device file (only for 5 or 6 testcase> \n", TCID);
        printf("Usage: %s -D <number of bits per pixel - 16, 24 or 32> \n", TCID);
        return;
}

#ifdef __cplusplus
}
#endif
