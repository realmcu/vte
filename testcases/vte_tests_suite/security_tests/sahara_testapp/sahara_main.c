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
        @file    sahara_main.c

        @brief   Securitty Sahara2 API test main function

====================================================================================================
Revision History:
                            Modification     Tracking
Author/Core ID                  Date          Number    Description of Changes
-------------------------   ------------    ----------  -------------------------------------------
A.Ozerov/NONE                10/10/2005     TLSbo55834  Initial version
A.Urusov/NONE                09/11/2005     TLSbo57926  New tests added
A.Ozerov/NONE                01/12/2005     TLSbo58662  Update for linux-2.6.10-rel-L26_1_14

====================================================================================================
Portability:  ARM GCC
==================================================================================================*/

/*==================================================================================================
                                        INCLUDE FILES
==================================================================================================*/
/* Verification Test Environment Include Files */
#include "sahara_test.h"

/*==================================================================================================
                                        GLOBAL VARIABLES
==================================================================================================*/
char    *TCID = "sahara_test";    /* test program identifier.  */
int     TST_TOTAL = 1;            /* total number of tests in this file.  */

SAHARA_TEST_IOCTL ctx_testcase = 0;

/*==================================================================================================
                                    GLOBAL FUNCTION PROTOTYPES
==================================================================================================*/
void    cleanup(void);
int     setup(void);
int     main(int argc, char **argv);

/*==================================================================================================
                                        GLOBAL VARIABLES
==================================================================================================*/

/*==================================================================================================
                                    LOCAL FUNCTION PROTOTYPES
==================================================================================================*/
void    help(void);

/*==================================================================================================
                                        LOCAL FUNCTIONS
==================================================================================================*/
/*================================================================================================*/
/*===== help =====*/
/** 
@brief  This function helps user to run a test case

@param  none

@return Nothing
*/
/*================================================================================================*/
void help(void)
{
        printf("\n===========================================\n");
        printf("Sahara2 driver options:\n");
        printf("\t  '-C 0'   Send multiple requests in non-blocking mode. Request a callback.\n");
        printf("\t  '-C 1'   Perform Cryptographic Hashing test.\n");
        printf("\t  '-C 2'   Calculate an HMAC in one operation.\n");
        printf("\t  '-C 3'   HMAC multi-step/chunking tests.\n");
        printf("\t  '-C 4'   Test the get results operation.\n");
        printf("\t  '-C 5'   Run tests on Symmetric cryptography routines.\n");
        printf("\t  '-C 6'   Test Key-Wrapping routines.\n");
        printf("\t  '-C 7'   Generating random data test\n");
        printf("\t  '-C 8'   Query the Platform Capabilities Object test.\n");
        printf("\t  '-C 9'   Verify/generate an authentication code and decrypting data test.\n");
        printf("\n===========================================\n");
}

/*==================================================================================================
                                        GLOBAL FUNCTIONS
==================================================================================================*/
/*================================================================================================*/
/*===== cleanup =====*/
/* 
@brief  Performs all one time clean up for this test on successful completion, premature exit or
        failure. Closes all temporary files, removes all temporary directories exits the test with
        appropriate return code by calling tst_exit()

@param  none

@return Nothing
*/
/*================================================================================================*/
void cleanup(void)
{
        int     VT_rv = TPASS;

        tst_resm(TINFO, "Close Sahara2 device.");

        VT_rv = VT_sahara_test_cleanup();

        if (TFAIL == VT_rv)
        {
                tst_resm(TWARN, "VT_sahara_cleanup() Failed : error code = %d", VT_rv);
        }

        tst_exit();
}

/*================================================================================================*/
/*===== setup =====*/
/**
@brief  Performs all one time setup for this test. This function is
        typically used to capture signals, create temporary dirs
        and temporary files that may be used in the course of this test

@param  none
    
@return On failure - Exits by calling cleanup()
        On success - return TPASS
*/
/*================================================================================================*/
int setup(void)
{
        int     VT_rv = TFAIL;

        tst_resm(TINFO, "Open Sahara2 device...");

        VT_rv = VT_sahara_test_setup();

        if (VT_rv != TPASS)
        {
                tst_brkm(TBROK, cleanup, "VT_sahara_setup() Failed : error code = %d", VT_rv);
        }

        return VT_rv;
}

/*================================================================================================*/
/*===== main =====*/
/* 
@param  argc The largest index of @c argv.

@param  argv Arguments of command invocation
 
@return On success - return TPASS 
        On failure - return TFAIL 
*/
/*================================================================================================*/
int main(int argc, char *argv[])
{
        int     VT_rv = TPASS;
        char    *msg;

        int     Cflag = 0;
        char    *Copt;

        option_t options[] = 
        {
                {"C:", &Cflag, &Copt},
                {NULL, NULL, NULL}
        };

        if ((msg = parse_opts(argc, argv, options, &help)) != (char *) NULL)
        {
                tst_resm(TCONF, "OPTION PARSING ERROR - %s", msg);
                return TFAIL;
        }
        if (Cflag)
        {
                ctx_testcase = atoi(Copt);
                if ((ctx_testcase < 0 || ctx_testcase > 9))
                {
                        tst_resm(TCONF, "Invalid arg for -C: %d OPTION VALUE OUT OF RANGE", ctx_testcase);
                        help();
                        return TFAIL;
                }
        }
        else
        {
                help();
                return TFAIL;
        }

        VT_rv = setup();

        VT_rv = VT_sahara_test(ctx_testcase);

        if (TPASS == VT_rv)
        {
                tst_resm(TPASS, "test case %s worked as expected", TCID);
        }
        else
        {
                tst_resm(TFAIL, "test case %s did NOT work as expected", TCID);
        }

        cleanup();

        return VT_rv;
}
