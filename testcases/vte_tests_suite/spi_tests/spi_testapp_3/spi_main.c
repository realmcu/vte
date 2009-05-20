/*====================*/
/**
    @file   spi_main.c

    @brief  LTP Motorola template.
*/
/*======================

    Copyright (C) 2004, Freescale Semiconductor, Inc. All Rights Reserved
  THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
  BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
  Freescale Semiconductor, Inc.

====================
Revision History:
                            Modification     Tracking
Author                          Date          Number    Description of Changes
-------------------------   ------------    ----------  -------------------------------------------
Tony THOMASSIN/RB595C        18/05/2004     TLSbo39490   SPI test development

====================
Portability: Indicate if this module is portable to other compilers or platforms.
             If not, indicate specific reasons why is it not portable.

======================*/

/*======================
Total Tests: TO BE COMPLETED

Test Name:   TO BE COMPLETED

Test Assertion
& Strategy:  A brief description of the test Assertion and Strategy
             TO BE COMPLETED
======================*/


#ifdef __cplusplus
extern "C"{
#endif

#ifndef bool
#define bool int
#endif

#ifndef true
#define true 1
#endif

#ifndef false
#define false 0
#endif

/*======================
                                        INCLUDE FILES
======================*/
/* Standard Include Files */
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

/* Harness Specific Include Files. */
#include "test.h"
#include "usctest.h"

/* Verification Test Environment Include Files */
#include "spi_test_3.h"

/*======================
                                        LOCAL MACROS
======================*/
#define READ_REGISTER_0 0x00000000
#define READ_REGISTER_1 0x02000000
/*======================
                          LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
======================*/


/*======================
                                       LOCAL CONSTANTS
======================*/
#if !defined(TRUE) && !defined(FALSE)
#define TRUE  1
#define FALSE 0
#endif

/*======================
                                       LOCAL VARIABLES
======================*/


/*======================
                                       GLOBAL CONSTANTS
======================*/
int Tflag = 0;
int Mflag = 0;
int Nflag = 0;
int Uflag = 0;
int Rflag = 0;
int Wflag = 0;

char *Topt,*Mopt,*Nopt,*Uopt,*Ropt,*Wopt;

option_t options[] = {
        { "T:", &Tflag, &Topt },
        { "M:", &Mflag, &Mopt },
        { "N:", &Nflag, &Nopt },
        { "U:", &Uflag, &Uopt },
        { "R:", &Rflag, &Ropt },
        { "W:", &Wflag, &Wopt },
        { NULL, NULL, NULL }
};

/*======================
                                       GLOBAL VARIABLES
======================*/
/* Extern Global Variables */
extern int  Tst_count;               /* counter for tst_xxx routines.         */
extern char *TESTDIR;                /* temporary dir created by tst_tmpdir(void) */

/* Global Variables */
char *TCID     = "spi_TestApp_3"; /* test program identifier.          */
int  TST_TOTAL = 1;                  /* total number of tests in this file.   */

/*======================
                                   GLOBAL FUNCTION PROTOTYPES
======================*/
void cleanup(void);
void setup(void);
int main(int argc, char **argv);

/*======================
                                   LOCAL FUNCTION PROTOTYPES
======================*/


/*======================
                                       GLOBAL FUNCTIONS
======================*/

/*====================*/
/*= cleanup =*/
/**
@brief  Performs all one time clean up for this test on successful
        completion,  premature exit or  failure. Closes all temporary
        files, removes all temporary directories exits the test with
        appropriate return code by calling tst_exit() function.cleanup

@param  Input :      None.
        Output:      None.

@return Nothing
*/
/*====================*/
void cleanup(void)
{
/* VTE : Actions needed to get a stable target environment */
        int VT_rv = TFAIL;

        VT_rv = VT_spi_cleanup();
        if (VT_rv != TPASS)
        {
                tst_resm(TWARN, "VT_cleanup() Failed : error code = %d", VT_rv);
        }
        /* VTE */

        /* Close all open file descriptors. */

        /** Insert code here. In case an unexpected failure occurs report it
        and exit setup(), the following lines of code will demonstrate
        this.

        if (close(fd) == -1)
        {
        tst_resm(TWARN, "close(%s) Failed, errno=%d : %s",
        fname, errno, strerror(errno));
        } */

        /* Remove all temporary directories used by this test. */

        /** Insert real code here */

        /* kill child processes if any. */

        /** Insert code here */

        /* Exit with appropriate return code. */

        tst_exit();
}

void help (void)
{
        printf("Options : \n\n");
        printf("   -T        card : 1 for MXC275-30 ADS , 2 for MXC275-30 EVB or 3 for i.300-30 EVB\n");
        printf("   -M        test        : 1 for multi-clients test, 2 for writing data\n");
        printf("   -N        mod         : 1 for SPI1 or 2 for SPI2\n");
        printf("   -U        0 or 1 for loopback\n");
        printf("   -R        Number of bytes to send from 1 to 32\n");
        printf("   -W        Value in hexadecimal\n\n");
        printf("Exemples :\n");
        printf("\t ./spi_testapp_3 -T 2 -M 1\n");
        printf("\t ./spi_testapp_3 -T 2 -M 2 -N 2 -U 0 -R 4 -W E6E00001 \n");
        printf("\t ./spi_testapp_3 -T 2 -M 2 -N 2 -U 1 -R 16 -W E6E00001E6E00000E6E00001E6E00000\n");
}

/*======================
                                       LOCAL FUNCTIONS
======================*/

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
void setup(void)
{
        int VT_rv = TFAIL;
        /* Capture signals. */

        /** Insert code here */

        /* Create temporary directories */

        /** Insert code here */

        /* Create temporary files. */

        /** Insert real code here. In case an unexpected failure occurs
        report it and exit setup(). Follow the code below for example.

        if ((fd = open(fname, O_RDWR|O_CREAT, 0700)) == -1 )
        {
        tst_brkm(TBROK, cleanup
        "Unable to open %s for read/write.  Error:%d, %s",
        fname, errno, strerror(errno));
        } */

        /* VTE : Actions needed to prepare the test running */
        VT_rv = VT_spi_setup();
        if (VT_rv != TPASS)
        {
                tst_brkm(TBROK , cleanup, "VT_setup() Failed : error code = %d", VT_rv);
        }
        /* VTE */

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
        int VT_rv = TFAIL;
        int card = 2;
        int test = 2;
        int module = 2;
        int loop = 1;
        int nbBytes = 4;
        char *msg;
        char message[1024];

        if ( (msg=parse_opts(argc,argv,options,&help)) != NULL )
                tst_brkm(TBROK, cleanup, "OPTION PARSING ERROR - %s", msg);

        if ( Tflag )
        {
                card = atoi(Topt);
                printf("Card = %d\n", card);
        }

        if ( Mflag )
        {
                test = atoi(Mopt);
                printf("Test = %d\n", test);
        }

        if ( Nflag )
        {
                module = atoi(Nopt);
                printf("Module = %d\n", module);
        }

        if ( Uflag )
        {
                loop = atoi(Uopt);
                printf("Loop = %d\n", loop);
        }

        if ( Rflag )
        {
                nbBytes = atoi(Ropt);
                printf("NbBytes = %d\n", nbBytes);
        }

        if ( Wflag )
        {
                sprintf(message, "%s", Wopt);
                printf("Message = %s\n", message);
        }

        /* perform global test setup, call setup() function. */
        setup();

        /* Print test Assertion using tst_resm() function with argument TINFO. */
        tst_resm(TINFO, "Testing if %s test case is OK", TCID);

        /* Test Case Body. */

        VT_rv = VT_spi_test_3(card, test, module, loop, nbBytes, (char *)message);

        if(VT_rv == TPASS)
                tst_resm(TPASS, "%s test case worked as expected", TCID);
        else
                tst_resm(TFAIL, "%s test case did NOT work as expected", TCID);



        cleanup(); /** OR tst_exit(); */
        /* VTE */

        return VT_rv;

}
