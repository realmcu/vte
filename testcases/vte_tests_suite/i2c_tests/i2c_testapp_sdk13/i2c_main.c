/*====================*/
/**
        @file   i2c_main.c

        @brief  First I2C test main function.
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
-------------------------   ------------    ----------  --------------------------------------------
V.Khalabuda/hlbv001          21/07/2004     TLSbo40419  I2C test development
V.Khalabuda/hlbv001          28/02/2005     TLSbo45061  Review I2C test development
S.V-Guilhou/svan01c          20/09/2005     TLSbo53753  I/O errors
V.Khalabuda/b00306           04/07/2006     TLSbo68945  Update testapp for I2C_RDWR ioctl
                                                        and r/w from user space

====================
Portability:  ARM GCC
======================*/

/*======================
Total Tests: 1

Test Executable Name:  i2c_testapp_3

Test Assertion
& Strategy:  A test for MXC I2C
=====================*/

/*======================
                                        INCLUDE FILES
======================*/
/* Verification Test Environment Include Files */
#include "i2c_test.h"

/*======================
                                        LOCAL MACROS
======================*/

/*======================
                                        LOCAL VARIABLES
======================*/

/*======================
                            LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
======================*/

/*======================
                                        LOCAL CONSTANTS
======================*/

/*======================
                                        GLOBAL CONSTANTS
======================*/
int     Dflag = 0,
        Aflag = 0,
        Tflag = 0;
char   *Dopt,
       *Aopt,
       *Topt;
char    device_name[128];

option_t options[] =
{
        {"D:", &Dflag, &Dopt},
        {"A:", &Aflag, &Aopt},
        {"T:", &Tflag, &Topt},
        {NULL, NULL, NULL}
};

/*======================
                                        GLOBAL VARIABLES
======================*/
/* Extern Global Variables */
extern int   Tst_count;        /* counter for tst_xxx routines.  */
extern char *TESTDIR;          /* temporary dir created by tst_tmpdir */

/* Global Variables */
char   *TCID = "i2c_testapp_sdk13";        /* test program identifier.  */
int     TST_TOTAL = 1;                 /* total number of tests in this file.  */

I2C_TESTS i2c_testcase = 0;
char    device_name[128];
unsigned short addr;

/*======================
                                    GLOBAL FUNCTION PROTOTYPES
======================*/
void    cleanup(void);
int    setup(void);
int     main(int argc, char **argv);

/*======================
                                    LOCAL FUNCTION PROTOTYPES
======================*/
void    help(void);

/*======================
                                        GLOBAL FUNCTIONS
======================*/

/*====================*/
/*= cleanup =*/
/**
@brief  Performs all one time clean up for this test on successful completion,
        premature exit or  failure. Closes all temporary files,
        removes all temporary directories exits the test with
        appropriate return code by calling tst_exit() function.cleanup

@param  Input :      None.
        Output:      None.

@return Nothing
*/
/*====================*/
void cleanup(void)
{
        /* VTE : Actions needed to get a stable target environment */
        int     VT_rv = TFAIL;

        tst_resm(TINFO, "Close I2C bus device.");
        VT_rv = VT_i2c_cleanup();
        if (VT_rv != TPASS)
        {
                tst_resm(TWARN, "VT_cleanup() Failed : error code = %d", VT_rv);
        }

        /* Exit with appropriate return code. */
        tst_exit();
}

/*======================
                                        LOCAL FUNCTIONS
======================*/

/*====================*/
/*= help =*/
/**
@brief  Inform of the available options and the associated parameters

@param  Input :      None.
        Output:      None.

@return Nothing
*/
/*====================*/
void help(void)
{
        printf("============\n");
        printf("\t-D x        Device name, e.g. /dev/i2c/0        (def)\n");
        printf("\t-A x        Memory/Start address to access(def, hex)\n");
        printf("\t-T x        Testcase name\n");
        printf("\nUsage: %s [-D device_name] [-A base_address] [-T type_of_test]\n\n", TCID);
        printf("\t-D [/dev/i2c/0, /dev/i2c_module, /dev/test_i2c_module]\n");
        printf("\t-A [0xNN]\n");
        printf("\t-T [%d - %d]\n", TEST_I2C_RW, TEST_I2C_IOCTL);
}

/*====================*/
/*= setup =*/
/**
@brief  Performs all one time setup for this test. This function is
        typically used to capture signals, create temporary dirs
                                and temporary files that may be used in the course of this test.

@param  Input :      None.
        Output:      None.

@return On failure - Exits by calling cleanup().
        On success - returns 0.
*/
/*====================*/
int setup(void)
{
        int     VT_rv = TPASS;

        tst_resm(TINFO, "Open I2C bus device...");
        /* VTE : Actions needed to prepare the test running */
        VT_rv = VT_i2c_setup();
        if (VT_rv != TPASS)
        {
                tst_brkm(TBROK, cleanup, "VT_i2c_setup() Failed : error code = %d", VT_rv);
        }

        return VT_rv;
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
        int     VT_rv = TPASS;
  //int   r_value=TPASS;
        char   *msg;

        if ((msg = parse_opts(argc, argv, options, &help)) != (char *) NULL)
        {
                tst_resm(TFAIL, "OPTION PARSING ERROR - %s", msg);
                return TFAIL;
        }

        if (Dflag)
        {
                strcpy(device_name, Dopt);
        }
        else
        {
                strcpy(device_name, "/dev/"DEFAULT_I2C_BUS);
        }

        if (Tflag)
        {
                i2c_testcase = atoi(Topt);
                if (i2c_testcase < TEST_I2C_RW || i2c_testcase > TEST_I2C_IOCTL)
                {
                        tst_resm(TFAIL, "Invalid arg for -T: %s. OPTION VALUE OUT OF RANGE",
                                 Topt);
                        return TFAIL;
                }
        }
        else
        {
                tst_resm(TFAIL, "-T must be specified");
                return TFAIL;
        }

        if(Aflag)
        {
                if(sscanf(Aopt, "0x%x", &addr) != 1)
                {
                        tst_resm(TFAIL, "Cannot parse %s as addrs., example: 0x01", Aopt);
                        return TFAIL;
                }
        }
        else
        {
                tst_resm(TFAIL, "-A must be specified");
                return TFAIL;
        }

        /* perform global test setup, call setup() function. */
  printf("setup starts.....\n");
        VT_rv=setup();
  if(VT_rv!=TPASS)
   {
    printf("error in setup()!\n");
    tst_resm(TFAIL, "%s test case did NOT work as expected", TCID);
    return VT_rv;
   }
        printf("setup finished....\n");

        /* Print test Assertion using tst_resm() function with argument TINFO. */
        tst_resm(TINFO, "Testing if %s test case is OK", TCID);

        /* VTE : print results and exit test scenario */
        VT_rv = VT_i2c_test();

        if (VT_rv == TPASS)
                tst_resm(TPASS, "%s test case worked as expected", TCID);
        else
                tst_resm(TFAIL, "%s test case did NOT work as expected", TCID);

        cleanup();        /** OR tst_exit(); */

        return VT_rv;
}
