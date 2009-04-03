/*================================================================================================*/
/**
        @file   spi_main.c

        @brief  SPI testapp main source file
*/
/*==================================================================================================

        Copyright (C) 2006, Freescale Semiconductor, Inc. All Rights Reserved
        THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
        BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
        Freescale Semiconductor, Inc.

====================================================================================================
Revision History:
                            Modification     Tracking
Author/core ID                  Date          Number    Description of Changes
-------------------------   ------------    ----------  -------------------------------------------
Tony THOMASSIN/RB595C        18/05/2004     TLSbo39490  SPI test development
V.Khalabuda/b00306           17/04/2006     TLSbo67010  Update version for linux-2.6.10-rel-L26_1_17
D.Kazachkov/b00316           30/08/2006     TLSbo67010  Update version for linux-2.6.10-rel-L26_1_18
D.Khoroshev/b00313           02/01/2006     TLSbo86657  Adaptation to new spi interface

====================================================================================================
Portability: ARM GCC

==================================================================================================*/

/*==================================================================================================
Total Tests: TO BE COMPLETED

Test Name:   spi_testapp_0

Test Assertion
& Strategy:  A brief description of the test Assertion and Strategy
            TO BE COMPLETED
==================================================================================================*/

/*==================================================================================================
                                        INCLUDE FILES
==================================================================================================*/
/* Verification Test Environment Include Files */
#include "spi_test_0.h"

/*==================================================================================================
                                        LOCAL MACROS
==================================================================================================*/
#define MC13783_REG_NUMBER 64

/*==================================================================================================
                            LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
==================================================================================================*/

/*==================================================================================================
                                        LOCAL CONSTANTS
==================================================================================================*/

/*==================================================================================================
                                        LOCAL VARIABLES
==================================================================================================*/
char    buffer[2 * BUFF_TEST_MAX_SIZE] = { 0 };
char    mc13783_message[2 * BUFF_TEST_MAX_SIZE] = { 0 };

/*==================================================================================================
                                        GLOBAL CONSTANTS
==================================================================================================*/

/*==================================================================================================
                                        GLOBAL VARIABLES
==================================================================================================*/
/* Global Variables */
char   *TCID = "spi_testapp_0";        /* test program identifier.            */
int     TST_TOTAL = 1;                 /* total number of tests in this file. */

char    device_filename[128];

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

/*================================================================================================*/
/*===== help =====*/
/**
@brief  Inform of the available options and the associated parameters

@param  Input :      None.
        Output:      None.

@return Nothing
*/
/*================================================================================================*/
void help(void)
{
        printf("====================================================\n");
        printf("SPI device driver options\n");
        printf("\t-D n, SPI number:  0 - MXC_SPI1, 1 - MXC_SPI2, 2- MXC_SPI3,  default is 0\n");
        printf("\t-N n, Number of iterations over spi_send_frame call, default is 1\n");
        printf("\t-V xxxxx, bytes to send\n");
        printf("\t-R n, number of bytes to send, from 2 to 32\n");
        printf("\nUsage: %s [-T n] [-D n] [-N n] [-R n] [-V xxx] \n", TCID);
        printf("\t./%s -D 0 -R 4 -V E6E0 \n", TCID);
        printf("\t./%s -D 1 -R 16 -V E6E00001E6E00000\n", TCID);
}

/*==================================================================================================
                                        LOCAL FUNCTIONS
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
void cleanup(void)
{
        /* VTE : Actions needed to get a stable target environment */
        int     VT_rv = TFAIL;

        VT_rv = VT_spi_cleanup();
        if (VT_rv != TPASS)
        {
                tst_resm(TWARN, "VT_spi_cleanup() Failed : error code = %d", VT_rv);
        }

        tst_exit();
}

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
        int     VT_rv = TFAIL;

        /* VTE : Actions needed to prepare the test running */
        VT_rv = VT_spi_setup();
        if (VT_rv != TPASS)
        {
                tst_brkm(TBROK, cleanup, "VT_spi_setup() Failed : error code = %d", VT_rv);
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

@param  Input :        argc - number of command line parameters.
        Output:        **argv - pointer to the array of the command line parameters.
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
        int     VT_rv = TFAIL;
        char   *msg;
        unsigned int test_type,
                spi_id,
                n_iterations, i,
                bytes;

        /* binary flags: opt or not */
        int     Tflag = 0,
                Dflag = 0,
                Nflag = 0,
                Rflag = 0,
                Vflag = 0;
        char   *Topt,
               *Dopt,
               *Nopt,
               *Ropt,
               *Vopt;

        option_t options[] =
        {
                {"T:", &Tflag, &Topt},        /* Testcase type - writing data or multi-client test */
                {"D:", &Dflag, &Dopt},        /* SPI interface - SPI1 or SPI2 or SP3 */
                {"N:", &Nflag, &Nopt},        /* Number of iterations */
                {"R:", &Rflag, &Ropt},        /* Number of bytes to send */
                {"V:", &Vflag, &Vopt},        /* Value in hexadecimal */
                {NULL,   NULL,  NULL}         /* NULL required to end array */
        };

        /* parse options. */
        if ((msg = parse_opts(argc, argv, options, &help)) != (char *) NULL)
        {
                tst_resm(TFAIL, "OPTION PARSING ERROR - %s", msg);
        }
        /* get option for test type */
        if (Tflag)
        {
                test_type = atoi(Topt);
                if ((test_type != 1) && (test_type != 2))
                {
                        tst_resm(TFAIL, "Invalid arg for -T: %s. OPTION VALUE OUT OF RANGE", Topt);
                        return TFAIL;
                }

                if ((test_type == 2))
                {
                        tst_resm(TFAIL, "Multi-client test is not supported");
                        return TFAIL;
                }

        }
//        else
                test_type = 1;

        /* get option for spi device to use */
        if (Dflag)
                spi_id = atoi(Dopt);
        else
                spi_id = 0;

        sprintf(device_filename, "/dev/" MXC_SPI_DEVS "%d", spi_id);

        /* get data to transfer */
        if (Vflag)
        {
                if (strncmp((char *) Vopt, "0x", 2))
                        strcpy(buffer, Vopt);
                else
                        strcpy(buffer, (char *) Vopt + 2);
        }
        else
        {
                strcpy(buffer, "E6E00001E6E00000E6E00001E6E00001");
        }

        /* get option for number of bytes to transfer */
        if (Rflag)
        {
                bytes = atoi(Ropt);
        }
        else
        {
                bytes = strlen(buffer);
        }

        if (bytes > BUFF_TEST_MAX_SIZE)
        {
                tst_resm(TFAIL, "Unrecognized parameter for buffer size -R: %s", Ropt);
                return TFAIL;
        }


        if (Nflag)
                n_iterations = atoi(Nopt);
        else
                n_iterations = 1;

        /* perform global test setup, call setup() function. */
        setup();

        /* Print test Assertion using tst_resm() function with argument TINFO. */
        tst_resm(TINFO, "Testing if %s test case is OK", TCID);

        tst_resm(TINFO,"Sending %d bytes", bytes);

        for(i=0;i<n_iterations;i++)
        {
                tst_resm(TINFO,"Executing data transfer test (%d)",i);
                VT_rv = VT_spi_buffer_test(bytes, buffer);
                if(VT_rv != TPASS)
                        break;
        }


        if (VT_rv == TPASS)
                tst_resm(TPASS, "%s test case worked as expected", TCID);
        else
                tst_resm(TFAIL, "%s test case did NOT work as expected", TCID);

        cleanup();        /** OR tst_exit(); */

        return VT_rv;
}
