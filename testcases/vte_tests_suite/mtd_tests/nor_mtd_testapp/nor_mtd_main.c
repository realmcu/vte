/*====================*/
/**
        @file   nor_mtd_main.c

        @brief    MTD test main function.
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
S.ZAVJALOV/ZVJS001C          27/07/2004      TLSbo40261  Initial version
E.Gromazina                  07/07/2005      TLSbo50888  minor fixes
A.Ozerov/b00320              19/04/2006      TLSbo61865  Cast to coding conversions

====================
Portability:  ARM GCC
======================*/

/*======================
                                        INCLUDE FILES
======================*/
/* Standard Include Files */
#include <errno.h>

/* Harness Specific Include Files. */
#include <test.h>
#include <usctest.h>

/* Verification Test Environment Include Files */
#include "nor_mtd_test.h"

/*======================
                                        GLOBAL VARIABLES
======================*/
/* Extern Global Variables */
extern int Tst_count;   /* counter for tst_xxx routines.  */
extern char *TESTDIR;   /* temporary dir created by tst_tmpdir(void) */

/* Global Variables */
char   *TCID = "nor_mtd";      /* test program identifier.  */
int     TST_TOTAL = 1;  /* total number of tests in this file.  */

int     d_num = 0,
        a_num = 0,
        l_num = 0,
        v_num = 0,
        f_num=0,
        t_num = 0;
char   *d_copt,
       *a_copt,
       *l_copt,
       *f_copt,
       *t_copt;

char    device_name[128];
long    addr_offset = -1,
        length_tmem = -1;
int fullPageFlag=1;
option_t options[] =
{
        {"D:", &d_num, &d_copt},        /* Device name                */
        {"A:", &a_num, &a_copt},        /* Start address              */
        {"L:", &l_num, &l_copt},        /* Length test memory         */
        {"V",  &v_num,    NULL},        /* Verbose mode               */
        {"T:", &t_num, &t_copt},        /* Test number                */
        {"F:", &f_num, &f_copt},        //full page or not 1 full page, 0 half page
        {NULL,   NULL,    NULL}         /* NULL required to end array */
};

/*======================
                                    GLOBAL FUNCTION PROTOTYPES
======================*/
void    cleanup(void);
void    setup(void);
int     main(int argc, char **argv);
void    help(void);

/*======================
                                        GLOBAL FUNCTIONS
======================*/
/*====================*/
/*= help =*/
/**
@brief  Print help information.

@param  None

@return None
*/
/*====================*/
void help(void)
{
        printf("============\n");
        printf("  -T      Testcase: <RDRW>|<WRNER>\n");
        printf("  -V      Verbose mode\n");
        printf("  -D x    Device name\n");
        printf("  -A x    Start test address (hex)\n");
        printf("  -L x    Length test memory (hex)\n");
  printf("  -F x    full page for performance or half page performance 1: full page and 0: half page");
        printf("\nUsage: %s -T <testcase> [-V] [-D device_name] [-A start_address] [-L length_memory] [-F flag]\n\n", TCID);
}

/*======================
                                        LOCAL FUNCTIONS
======================*/
/*= cleanup =*/
/**
@brief  Performs all one time clean up for this test on successful
        completion,  premature exit or  failure. Closes all temporary
        files, removes all temporary directories exits the test with
        appropriate return code by calling tst_exit() function.cleanup

@param  None

@return None
*/
/*====================*/
void cleanup(void)
{
        /* VTE : Actions needed to get a stable target environment */
        int     VT_rv = TFAIL;

        VT_rv = VT_nor_mtd_test_cleanup();
        if (VT_rv != TPASS)
        {
                tst_resm(TWARN, "VT_nor_mtd_cleanup() Failed : error code = %d", VT_rv);
        }

        /* Exit with appropriate return code. */
        tst_exit();
}

/*====================*/
/*= setup =*/
/**
@brief  Performs all one time setup for this test. This function is
        typically used to capture signals, create temporary dirs
        and temporary files that may be used in the course of this test.

@param  None

@return On failure - Exits by calling cleanup().
        On success - returns 0.
*/
/*====================*/
void setup(void)
{
        int     VT_rv = TFAIL;

        /* VTE : Actions needed to prepare the test running */
        VT_rv = VT_nor_mtd_test_setup();
        if (VT_rv != TPASS)
        {
                tst_brkm(TBROK, cleanup, "VT_nor_mtd_setup() Failed : error code = %d", VT_rv);
        }

        return;
}

/*====================*/
/*= main =*/
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
/*====================*/
int main(int argc, char **argv)
{
        int     VT_rv = TFAIL;
        char   *msg;

        if ((msg = parse_opts(argc, argv, options, &help)) != (char *) NULL)
        {
                tst_resm(TFAIL, "OPTION PARSING ERROR - %s", msg);
                return TFAIL;
        }

        if (!t_num)
        {
                tst_resm(TFAIL, "-T - this option is requered");
                return TFAIL;
        }

        if (d_num)
        {
                strcpy(device_name, d_copt);
        }
        else
        {
                strcpy(device_name, NOR_MTD_DEVICE);
        }

        if (a_num)
        {
                addr_offset = -1;
                sscanf(a_copt, "0x%lx", &addr_offset);
                if (addr_offset < 0)
                {
                        tst_resm(TFAIL, "Invalid arg for -A: %s", a_copt);
                        return TFAIL;
                }
        }
        else
        {
                addr_offset = ADDR_OFFSET;
        }

  if(f_num)
  {
   fullPageFlag=atoi(f_copt);
   if(fullPageFlag>0)
   {
    fullPageFlag=1;
   }
   else
   {
    fullPageFlag=0;
   }
  }
  else
  {
   fullPageFlag=1;
  }

        if (l_num)
        {
                length_tmem = -1;
                sscanf(l_copt, "0x%lx", &length_tmem);
                if (length_tmem < 0)
                {
                        tst_resm(TFAIL, "Invalid arg for -L: %s", l_copt);
                        return TFAIL;
                }
        }
        else
        {
                length_tmem = LENGTH_TEST_MEM;
        }

        /* perform global test setup, call setup() function. */
        setup();

        /* Print test Assertion using tst_resm() function with argument TINFO. */
        tst_resm(TINFO, "Testing if %s test case is OK", TCID);

        /* VTE : Execute test, print results and exit test scenario */
        /* Get info test */
        if (strcmp(t_copt, TEST_CASE_INFO) == 0)
        {
                VT_rv = VT_nor_mtd_test_info();
        }
        else
        {
                /* Try write into not erase memory */
                if (strcmp(t_copt, TEST_CASE_WRNER) == 0)
                {
                        VT_rv = VT_nor_mtd_test_wrner();
                }
                else
                {
                        /* Read Write Erase test */
                        if (strcmp(t_copt, TEST_CASE_RDRW) == 0)
                        {
                                VT_rv = VT_nor_mtd_test_rdrw();
                        }
                        else
                        {

         if(strcmp(t_copt,TEST_CASE_GETBADBLOCK)==0)
                            {
                           VT_rv=VT_nor_mtd_test_regionInfo();
      VT_rv=VT_nor_mtd_test_badblk();
      if(VT_rv==TPASS)
      {
       tst_resm(TPASS,"test case %s worked as expected",TCID);
      }

     }
        else
        {
       if(strcmp(t_copt,TEST_CASE_PERFORMACE)==0)
       {
       VT_rv=VT_nor_mtd_test_perform();

       if(VT_rv==TPASS)
       {
        tst_resm(TPASS,"test case %s worked as expected",TCID);
       }

      }
      else
      {
       if(strcmp(t_copt,TEST_CASE_THRDRWE)==0)
       {
        VT_rv=VT_nor_mtd_test_thrdrwe();
        if(VT_rv==TPASS)
        {
         tst_resm(TPASS,"test case %s worked as expected",TCID);
        }
       }
       else
       {

        if(strcmp(t_copt,TEST_CASE_THRDRWONEPAGE)==0)
        {
         VT_rv=VT_nor_mtd_test_thrdrwonepage();
         if(VT_rv==TPASS)
          {
          tst_resm(TPASS,"test case %s worked as expected",TCID);
         }
        }
        else
        {
         tst_resm(TFAIL,"Invalid arg for -T: %s",t_copt);
          VT_rv=TFAIL;
        }

       }
      }
        }
                        }

                }

        }

        if (VT_rv == TPASS)
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
