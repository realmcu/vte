/*================================================================================================*/
/**
    @file   v4l_output_main.c

    @brief  v4l output main source file.*/
/*==================================================================================================

    Copyright (C) 2005, Freescale Semiconductor, Inc. All Rights Reserved
    THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
    BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
    Freescale Semiconductor, Inc.
    
====================================================================================================
Revision History:
                            Modification     Tracking
Author                          Date          Number    Description of Changes
-------------------------   ------------    ----------  -------------------------------------------
Smirnov Artyom/ID            07/05/05       TLSbo49894   BRIEF description of changes made 
Bezrukov.S/SBAZR1C           08/17/2005     TLSbo53919   Remove the Brightness feature
KHOROSHEV.D                  09/29/2005     TLSbo55077   Rewiev version
Kazachkov/e1403c             03/14/2006     TLSbo58742	 -S option causes seg fault  - fixed
====================================================================================================
Portability: ARM GCC 
==================================================================================================*/

/*==================================================================================================
Total Tests: 

Test Name: v4l output test scenario   

Test Assertion
& Strategy:  Read dump file with picture, convert it to necessary format, crop, rotate picture and 
display it to the screen(write to another dump file).
==================================================================================================*/


#ifdef __cplusplus
extern "C"{ 
#endif

/*==================================================================================================
                                        INCLUDE FILES
==================================================================================================*/

/* Verification Test Environment Include Files */
#include "v4l_output_test.h"

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

/* Global Variables */
char   *TCID = "v4l_output_testapp";    /* test program identifier.  */
int     TST_TOTAL = 1;  /* total number of tests in this file.  */

                                        /* binary flags: opt or not */

int     Cflag = 0,
    dflag = 0,
    Rflag = 0,
    Xflag = 0,
    Yflag = 0,
    Fflag = 0;
 
int     Oflag = 0,
    oflag = 0,
    Pflag = 0,
    Sflag = 0,
    Uflag = 0;
/*hake add offset test in crop*/
int Bflag = 0;

char   *Copt,
       *dopt,
       *Ropt,
       *Xopt,
       *Yopt,
       *Fopt,
       *Oopt,
       *oopt,
       *Bopt,
       *Uopt,
       *Sopt;

/* opt values */

option_t options[] = {
        {"C:", &Cflag, &Copt},
        {"B:", &Bflag, &Bopt},
        {"d:", &dflag, &dopt},
        {"R:", &Rflag, &Ropt},
        {"S:", &Sflag, &Sopt},
        {"X:", &Xflag, &Xopt},
        {"Y:", &Yflag, &Yopt},
        {"F:", &Fflag, &Fopt},
        {"O:", &Oflag, &Oopt},
        {"o:", &oflag, &oopt},
        {NULL, NULL, NULL}      /* NULL required to end array */
};


            /* global structure to exchange parameters between functios */
params  p;


/*==================================================================================================
                                    GLOBAL FUNCTION PROTOTYPES
==================================================================================================*/

void    cleanup(void);
void    setup(void);
int     main(int argc, char **argv);

/*==================================================================================================
                                    LOCAL FUNCTION PROTOTYPES
==================================================================================================*/


/*==================================================================================================
                                        GLOBAL FUNCTIONS
==================================================================================================*/


void help(void)
{
        printf("^[[36m^[[40mUsage: %s [-C <number>][-X <x_ratio>] [-Y <y_ratio>] [-R <rotation>]\n", TCID);
        printf("          [-S <number_of_buffers>] [-F <dump file>] [-o <output_file>]\n"); 
        printf("          [-O <output_format>] [-d <video_dev>]^[[m\n\n");
        printf("\tWhere: C specifies testcase number: 1 - file to file, 2 - file to display\n");
        printf("\t\tx_ratio, y_ratio - ratios are in percents\n");
        printf("\t\trotation - number 0 to 7\n");
        printf("\t\tnumber_of_buffers - specify number of video buffers\n");
        printf("\t\tdump file - input dump file\n");
        printf("\t\toutput_file - in case -C 1 output file\n");
        printf("\t\toutput_format - format of output file\n");
        printf("\t\tvideo_dev - device file\n");
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
    
@return Nothing*/
/*================================================================================================*/
void cleanup(void)
{
        /* VTE : Actions needed to get a stable target environment */

        int     VT_rv = TFAIL;

        VT_rv = VT_v4l_output_cleanup();

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
        On success - returns 0.*/
/*================================================================================================*/
void setup(void)
{
        int     VT_rv = TFAIL;

        VT_rv = VT_v4l_output_setup();

        if (VT_rv != TPASS)
        {
                tst_brkm(TBROK, cleanup, "VT_setup() Failed : error code = %d", VT_rv);
        }

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
    
@return On failure - Exits by calling cleanup().
        On success - exits with 0 exit value.*/
/*================================================================================================*/
int main(int argc, char **argv)
{
        int     VT_rv = TFAIL;

        char   *msg;

        memset(&p, 0x0, sizeof(params));

        /* parse options. */

        if ((msg = parse_opts(argc, argv, options, help)) != NULL)
        {
                tst_brkm(TBROK, tst_exit, "OPTION PARSING ERROR - %s", msg);
                goto final;
        }

        if (Cflag)
        {
                p.test_type = atoi(Copt);

                if ((p.test_type < 1) || (p.test_type > 2))
                {
                        tst_resm(TBROK, "You entered wrong testcase type, try -C 1 or 2", TCID);
                        return VT_rv;
                }

                /* if (p.test_type == 1) { tst_resm(TBROK, "File to file processing not supported *
                * currently", TCID); goto final; } */
        }

        if (dflag)
                p.v4l_dev_file = dopt;
        else
                p.v4l_dev_file = V4L_OUTPUT_DEVICE;

        if (Rflag)
        {
                p.nb_rot = atoi(Ropt);
                if (p.nb_rot < 0 || p.nb_rot > 7)
                        p.nb_rot = 0;
        }
        else
                p.nb_rot = 0;

        if (Pflag)
        {       /* cropping tested through resizing */
                p.crop_on = 1;
                /* hardcode the cropping rectangle for now */
                p.x_size_crop = 30;
                p.y_size_crop = 40;
        }

	if (Bflag)
	{
	    /*cropping tested through offset */
	    p.crop_on = 2;
	    /*hardcode to offset 10 by default*/
	    p.x_offset = 10;
	    p.y_offset = 10;
	    p.x_size_crop = 320;
	    p.y_size_crop = 240;
	    if(Bopt)
	     sscanf(Bopt,"%d,%d,%d,%d",&p.x_offset,&p.y_offset,&p.x_size_crop,&p.y_size_crop);
	}

        if (Xflag)
                p.x_ratio = atoi(Xopt);
        else    /* ratios are in percents, so to upsize try ratio > 100 */
                p.x_ratio = 100;

        if (Yflag)
                p.y_ratio = atoi(Yopt);
        else
                p.y_ratio = 100;

        if (Fflag)
        {
                p.input_file = Fopt;
        }
        else
        {
                p.input_file = INPUT_FILE;
        }

        if (Oflag)
        {
                p.output_fmt = Oopt;
        }
        else
        {
                p.output_fmt = "BGR24";
        }

        if (oflag)
                p.output_file = oopt;
        else
                p.output_file = OUTPUT_FILE;

        if (Sflag)
        {
                p.nb_buffers = atoi(Sopt);
                if (p.nb_buffers < 1)
                        p.nb_buffers = 1;
                if (p.nb_buffers > MAX_BUFF_NUM)
                        p.nb_buffers = MAX_BUFF_NUM;
        }
        else
                p.nb_buffers = 4;
        /* perform global test setup, call setup() function. */
        setup();

        /* Print test Assertion using tst_resm() function with argument TINFO. */

        tst_resm(TINFO, "Testing if %s test case is OK\n", TCID);

        VT_rv = VT_v4l_output_test();

        if (VT_rv == TPASS)
                tst_resm(TPASS, "%s test case worked as expected", TCID);
        else
                tst_resm(TFAIL, "%s test case did NOT work as expected", TCID);

        cleanup();
    final:

        return VT_rv;
}
