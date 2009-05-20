/*====================*/
/**
        @file   udma_main.c

        @brief  Main file for Unified DMA driver test
*/
/*======================

        Copyright (C) 2006, Freescale Semiconductor, Inc. All Rights Reserved
        THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
        BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
        Freescale Semiconductor, Inc.

====================
Revision History:
                            Modification     Tracking
Author/core ID                  Date          Number    Description of Changes
-------------------------   ------------    ----------  -------------------------------------------
A.Ozerov/b00320              03/10/2006     TLSbo78550  Initial version.
A.Ozerov/b00320              01/11/2006     TLSbo81158  UDMA module was fixed for working with all platforms.
A.Ozerov/b00320              05/02/2007     TLSbo87473  One of testcases was removed.

====================
Portability: ARM GCC
======================*/

/*======================
                                        INCLUDE FILES
======================*/
/* Verification Test Environment Include Files */
#include "udma_test.h"

/*======================
                                    LOCAL FUNCTION PROTOTYPES
======================*/
void    setup(void);
void    help(void);
void    cleanup(void);
int     main(int argc, char **argv);

/*======================
                                        LOCAL VARIABLES
======================*/

/*======================
                                        GLOBAL VARIABLES
======================*/
/* Extern Global Variables */
extern int Tst_count;
extern char *TESTDIR;

/* Global Variables */
char   *TCID = "udma";
#ifdef CONFIG_OTHER_PLATFORM
int     TST_TOTAL = 3;
#else
int     TST_TOTAL = 6;
#endif

/*======================
                                       LOCAL FUNCTIONS
======================*/

/*====================*/
/*= setup =*/
/**
@brief Performs all one time setup for this test. This function is typically used to capture
       signals, create temporary dirs and temporary files that may be used in the course of this test.

@param None.

@return None.
*/
/*====================*/
void setup(void)
{
        int     VT_rv = TFAIL;

        VT_rv = VT_udma_test_setup();
        if (VT_rv != TPASS)
        {
                tst_brkm(TBROK, cleanup, "VT_setup() Failed : error code = %d", VT_rv);
        }
}

/*====================*/
/*= cleanup =*/
/**
@brief This function performs all one time clean up for this test on successful completion,
       premature exit or failure. Closes all temporary files, removes all temporary directories exits
       the test with appropriate return code by calling tst_exit() function.

@param None.

@return None.
*/
/*====================*/
void cleanup(void)
{
        int     VT_rv = TFAIL;

        VT_rv = VT_udma_test_cleanup();
        if (VT_rv != TPASS)
        {
                tst_resm(TWARN, "VT_cleanup() Failed : error code = %d", VT_rv);
        }

        tst_exit();
}

/*====================*/
/*= main =*/
/**
@brief Entry point to this test-case. It parses all the command line inputs, calls the global
       setup and executes the test. It logs the test status and results appropriately using the LTP API's
       On successful completion or premature failure, cleanup() func is called and test exits with an
       appropriate return code.

@param Input : argc - number of command line parameters.
       Output: **argv - pointer to the array of the command line parameters.

@return On failure - Exits by calling cleanup().
        On success - exits with 0 exit value.
*/
/*====================*/
int main(int argc, char **argv)
{
        int     VT_rv = TFAIL;

        char   *msg;
        int     ch_num = 0,
                n_num = 0,
                tnum = 0;
        char   *topt;

#ifdef CONFIG_OTHER_PLATFORM
        char   *nopt;
        int     nnum = 0;
#endif

        option_t options[] =
        {
                {"T:", &tnum, &topt},        /* Test case number */
#ifdef CONFIG_OTHER_PLATFORM
                {"N:", &nnum, &nopt},        /* Number of buffers */
#endif
                {NULL, NULL, NULL}           /* NULL required to end array */
        };

        if((msg = parse_opts(argc, argv, options, &help)) != (char *) NULL)
        {
                tst_resm(TFAIL, "OPTION PARSING ERROR - %s", msg);
                return TFAIL;
        }

        if(tnum)
        {
                ch_num = atoi(topt);

#ifdef CONFIG_OTHER_PLATFORM
                if(ch_num < 0 || ch_num > 3)
#else
                if(ch_num < 0 || ch_num > 5)
#endif
                {
                        tst_resm(TFAIL, "Invalid argument for -T: %s", topt);
                        help();
                        cleanup();
                }
#ifdef CONFIG_OTHER_PLATFORM
                if(ch_num == 3)
                {
                        if(!nnum)
                        {
                                tst_resm(TFAIL, "Required argument -N");
                                help();
                                cleanup();
                        }
                        else
                                n_num = atoi(nopt);
                        if(n_num < 2 || n_num > 8)
                        {
                                tst_resm(TFAIL, "Invalid argument for -N: %s", nopt);
                                help();
                                cleanup();
                        }
                }
#endif
        }
        else
        {
                tst_resm(TFAIL, "Required argument -T");
                help();
                cleanup();
        }

        setup();
        tst_resm(TINFO, "Testing if %s test case is OK", TCID);
        VT_rv = VT_udma_test(ch_num, n_num);

        if (VT_rv == TPASS)
        {
                tst_resm(TPASS, "test case %s %s worked as expected", TCID, topt);
        }
        else
        {
                tst_resm(TFAIL, "test case %s %s did NOT work as expected", TCID, topt);
        }

        cleanup();
        return VT_rv;
}

/*====================*/
/*= help =*/
/**
@brief  Print help information.

@param  None.

@return None.
*/
/*====================*/
void help(void)
{
        printf("=============\n");
        printf("Unified DMA driver test options\n");

#ifdef CONFIG_OTHER_PLATFORM
        printf("\t  '-T 0'     Config setting\n");
        printf("\t  '-T 1'     Callback function assigning\n");
        printf("\t  '-T 2'     Data transfer testing\n");
#endif
#ifdef CONFIG_IMX27
        printf("\t  '-T 0'     1d-1d memory testing\n");
        printf("\t  '-T 1'     2d-2d memory testing\n");
        printf("\t  '-T 2'     1d-2d memory testing\n");
        printf("\t  '-T 3'     2d-1d memory testing\n");
        printf("\t  '-T 4'     Chain of buffers testing(using HW channel)\n");
        printf("\t  '-T 5'     Chain of buffers testing(using SW channel)\n");
#endif
        printf("=============\n");
}
