/*================================================================================================*/
/**
    @file   opl_mirror_main.c

    @brief  OPL image processing library mirroring functions tests main source file
*/
/*==================================================================================================

  Copyright (C) 2004, Freescale Semiconductor, Inc. All Rights Reserved
  THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
  BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
  Freescale Semiconductor, Inc.
     
====================================================================================================
Revision History:
                            Modification     Tracking
Author                          Date          Number    Description of Changes
-------------------------   ------------    ----------  -------------------------------------------
Smirnov, Kazachkov          10/9/2004     TLSbo41679   BRIEF description of changes made 

               

====================================================================================================
Portability: Indicate if this module is portable to other compilers or platforms. 
             If not, indicate specific reasons why is it not portable.

==================================================================================================*/

/*==================================================================================================
Total Tests: Ten cases for mirroring functions

Test Name:   OPL_MIRROR_TEST

Test Assertion
& Strategy:  A brief description of the test Assertion and Strategy
             
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
#include "opl_mirror_test.h"

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


/*==================================================================================================
                                       GLOBAL CONSTANTS
==================================================================================================*/


/*==================================================================================================
                                       GLOBAL VARIABLES
==================================================================================================*/
/* Extern Global Variables */
extern int  Tst_count;                  /* counter for tst_xxx routines.         */
extern char *TESTDIR;                   /* temporary dir created by tst_tmpdir() */

/* Global Variables */
char *TCID     = "opl_mirror_testapp";  /* test program identifier.              */
int  TST_TOTAL = 1;                     /* total number of tests in this file.   */

int Idflag = 0, Dflag = 0; Caseflag = 0, Iterflag = 0, Writflag = 0, Sflag=0;    /* binary flags: opt or not */ 
int Xflag=0, Yflag = 0;


int x_size=240;
int y_size=16;
int verbose_mode = 0;


int color_depth = 16;

char *Idopt;             	     /* Id option arguments   */
char *Caseopt;		             /* Case option arguments */
char *Iteropt;		             /* Iter option arguments */
char *Writopt; 
char *Sopt;
char *Dopt;
char *Xopt;
char *Yopt;

option_t options[] = {

    { "T:", &Idflag,   &Idopt   },   /* test ID			           */
    { "C:", &Caseflag, &Caseopt },   /* type of REENTRANCE test            */
    { "N:", &Iterflag, &Iteropt },   /* number of iterations for test func */
    { "W:", &Writflag, &Writopt },
    { "S:", &Sflag,    &Sopt    },
    { "D:", &Dflag,    &Dopt    },
    { "X:", &Xflag,    &Xopt    },
    { "Y:", &Yflag,    &Yopt    },
    { "V",  &verbose_mode, 0},
    { NULL, NULL,      NULL }        /* NULL required to end array         */
};      

/*==================================================================================================
                                   GLOBAL FUNCTION PROTOTYPES
==================================================================================================*/
void cleanup();
void setup();
int  main(int argc, char **argv);

/*==================================================================================================
                                   LOCAL FUNCTION PROTOTYPES
==================================================================================================*/

void help();

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
void cleanup()
{
/* VTE : Actions needed to get a stable target environment */
	int VT_rv = TFAIL;
	
	VT_rv = VT_opl_mirror_cleanup();
	
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
void setup()
{
    int VT_rv = TFAIL;

	VT_rv = VT_opl_mirror_setup();
	
		
    if (VT_rv != TPASS)
    {
		tst_brkm(TBROK , cleanup, "VT_setup() Failed : error code = %d", VT_rv);
    }
    
    return;
}

				       
				       
/*================================================================================================*/
/*===== help =====*/
/**
    @brief  Inform of the available options and the associated parameters
				       
    @param  Input :      None.
	    Output:      None.
					         
    @return On failure - Exits by calling cleanup().
	    On success - returns 0.
*/
/*================================================================================================*/
void help()
{
    printf("Options:\n\n");
    printf("-T 1..5	\n\tId of the test according to the test plan\n");
    printf("-C 1..2 \n\tThe test case number associated with the test \n");
    printf("-W \n\tWrite the output as a bmp file \n");
    printf("-N iter	\n\tThe number of iterations for endurance/stress test\n");
    printf("-S [m][M] \n\tDetermine wich operation perform \n");
    printf("-D depth \n\tColor depth (8,16,24,32) \n");
    printf("-X size \n\t width for 2,3,4 tests \n");
    printf("-Y size \n\t height for 2,3,4 tests \n");
    printf("-V verbose mode \n");


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
    
    char *err_msg;
    
    int Id   = ENDURANCE;
    int Case = PTHREADED;
    int Iter = ITERATIONS;
    int Writ = NO_BMP; 
    int r_flag =1, R_flag = 1;
	
    _TRACE
    	
    /* parse options. */

    if ( (err_msg = parse_opts(argc, argv, options, help)) != NULL )
	tst_brkm(TBROK, cleanup, "OPTION PARSING ERROR - %s", err_msg);
	
    if(Idflag)
    {
	    Id = atoi(Idopt);
	    printf(" Number of test type: %d", Id);
    }
    
    if(Caseflag)
    {
	    Case = atoi(Caseopt);
	    printf(" Type of REENTRANCE test, default is THREADED: %d", Case);
    }
    
    if(Iterflag)
    {
	    Iter = atoi(Iteropt);
	    printf(" Iterflag = %d", Iter);
    }    
    
    if (Writflag)
    {
	    Writ = atoi(Writopt);
	    printf(" Write BMP = %d", Writ);
    }

    if(Dflag)
    {
        color_depth = atoi(Dopt);	
        printf(" Color depth = %d", color_depth);
    }
    
    if(Xflag)
    {
        x_size = atoi(Xopt);	
        printf(" X size = %d", x_size);
    }

    if(Yflag)
    {
        y_size = atoi(Yopt);	
        printf(" Y size = %d", y_size);
    }

    if(Sflag)
    {
        r_flag = R_flag = 0;
        while(*Sopt)
        {
            if(*Sopt == 'm')
                r_flag = 1;
            if(*Sopt == 'M')
                R_flag = 1;
            Sopt++;
        }
        if((r_flag | R_flag) == 0)
        {
            r_flag = R_flag = 1;
        }

    }

    printf("\n");

    _TRACE

    /* perform global test setup, call setup() function. */
    setup();

    _TRACE

    /* Print test Assertion using tst_resm() function with argument TINFO. */
    tst_resm(TINFO, "Testing if %s test case is OK", TCID);

    /* Test Case Body. */

    /** Insert real code goes here. In case of unexpected failure, or 
    failure not directly related to the entity being tested report 
    using tst_brk() or tst_brkm() functions.

    if (something bad happened)
    {
        tst_brkm(TBROK, cleanup, "Unable to open test file %s", fname);
    } */

    /* VTE : print results and exit test scenario */
    
    _TRACE
    
    VT_rv = VT_opl_mirror_test(Id, Case, Iter, Writ, r_flag, R_flag);       /*with the parameters needed come from parse_opt())*/
	
    _TRACE
    
    if(VT_rv == TPASS)
        tst_resm(TPASS, "%s test case worked as expected", TCID);
    else
        tst_resm(TFAIL, "%s test case did NOT work as expected", TCID);
		
    _TRACE
		
    cleanup(); /** OR tst_exit(); */
		/* VTE */
	
	return VT_rv;
	
}
