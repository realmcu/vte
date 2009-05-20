/*====================*/
/**
        @file   fec_main.c

        @brief  Fast Ethernet Controller test

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
D.Khoroshev/b00313           10/05/2006     TLSbo76803  Initial version

====================
Portability: ARM GCC
======================*/

/*======================
Total Tests: 1

Test Name:   Fast Ethernet Controller test

Test Assertion
& Strategy:  This code is used to test the i.mx27 FEC driver
======================*/

/*======================
                                        INCLUDE FILES
======================*/
/* Harness Specific Include Files. */
#include <usctest.h>

/* Verification Test Environment Include Files */
#include "fec_test.h"

/*======================
                                        LOCAL MACROS
======================*/

/*======================
                          LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
======================*/

/*======================
                                       LOCAL CONSTANTS
======================*/

/*======================
                                       LOCAL VARIABLES
======================*/

/*======================
                                       GLOBAL CONSTANTS
======================*/

/*======================
                                       GLOBAL VARIABLES
======================*/
/* Global Variables */
char *TCID     = "fec_testapp";    /* test program identifier.          */
int  TST_TOTAL = T_NB;                   /* total number of tests in this file.   */

sTestConfig gTestConfig;

/*======================
                                   GLOBAL FUNCTION PROTOTYPES
======================*/
int main(int argc, char **argv);
void help(void);
void cleanup(void);
void setup(void);

/*======================
                                       LOCAL FUNCTIONS
======================*/

/*====================*/
/*= setup =*/
/**
Description of the function

@brief  Performs all one time setup for this test. This function is
        typically used to capture signals, create temporary dirs
        and temporary files that may be used in the course of this test.

@param  Input : None.
        Output: None.

@return Nothing
*/
/*====================*/
void setup(void)
{
        int VT_rv = TFAIL;

        VT_rv = VT_fec_test_setup();
        if (VT_rv != TPASS)
        {
                tst_brkm(TBROK , cleanup, "VT_setup() Failed : error code = %d", VT_rv);
        }
}

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

        VT_rv = VT_fec_test_cleanup();
        if (VT_rv != TPASS)
        {
                tst_resm(TWARN, "VT_cleanup() Failed : error code = %d", VT_rv);
        }

        /* Exit with appropriate return code. */
        tst_exit();
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

@return On failure - Exits by calling cleanup().
        On success - exits with 0 exit value.
*/
/*====================*/
int main(int argc, char **argv)
{
        int VT_rv = TFAIL;
        /* parse options. */
        int Tflag = 0,
#ifdef DEBUG
            Eflag = 0,
#endif
            vflag = 0,
            Dflag = 0,
            Hflag = 0,
            bflag = 0;
        char *Topt,
#ifdef DEBUG
             *Eopt,
#endif
             *Dopt,
             *Hopt,
             *msg;

        option_t options[] = {
                                {"T:", &Tflag, &Topt}, /* Test Number */
                                {"v",  &vflag,  NULL}, /* Verbouse flag */
#ifdef DEBUG
                                {"E:", &Eflag, &Eopt}, /* Use ethtool(0) and/or MII(1) interface. Both - 2 */
#endif
                                {"D:", &Dflag, &Dopt}, /* interface name, e.g. eth0 */
                                {"H:", &Hflag, &Hopt}, /* ping ip address */
                                {"b",  &bflag,  NULL}, /* send ping to default hardcoded address */
                                {NULL,  NULL,   NULL}  /* NULL required to end array */
                             };



        if ( (msg = parse_opts(argc, argv, options, &help)) != (char *) NULL )
        {
            tst_resm(TFAIL, "OPTION PARSING ERROR - %s", msg);
            return TFAIL;
        }

        fill_defconfig();
        gTestConfig.mTestCase = Tflag ? atoi(Topt) : 1; /*Default test case no. 1*/

        if (gTestConfig.mTestCase < T_SHOW_INFO || gTestConfig.mTestCase >= T_NB)
        {
                tst_resm(TBROK, "Invalid argument for -T: %s",Topt);
                help();
                return TFAIL;
        }

        memset(gTestConfig.ifname, 0, sizeof gTestConfig.ifname);
        if (Dflag)
                strncpy(gTestConfig.ifname, Dopt, 31);
        else
                strcpy(gTestConfig.ifname, "eth0");

#ifdef DEBUG
        gTestConfig.useif = Eflag ? atoi(Eopt) : IF_ETHTOOL | IF_MII | IF_IFF;
        if (gTestConfig.useif < 0 || gTestConfig.useif > 7)
        {
                tst_resm(TBROK, "Invalid argument for -E: %s", Eopt);
                help();
                return TFAIL;
        }
#endif

        gTestConfig.input_ip = 1;
        if (Hflag)
        {
                strncpy(gTestConfig.dc.def_test_ping_addr, Hopt, IPADDR_STRLEN - 1);
                gTestConfig.input_ip = 0;
        }
        else
        {
                if (bflag)
                {
                        gTestConfig.input_ip = 0;
                }
        }

        gTestConfig.mVerbose = vflag;

        setup();

        /* Print test Assertion using tst_resm() function with argument TINFO. */
        tst_resm(TINFO, "Testing if %s test case is OK", TCID);

        /* Test Case Body. */
        VT_rv = VT_fec_test();

        if(VT_rv == TPASS)
        {
                tst_resm(TPASS, "%s test case worked as expected", TCID);
        }
        else
        {
                tst_resm(TFAIL, "%s test case did NOT work as expected", TCID);
        }

        /* Cleanup test allocated ressources */
        cleanup();

        return VT_rv;
}

/*====================*/
/*= help =*/
/**
@brief  Inform of the available options and the associated parameters

@param  None.

@return Nothing.
*/
/*====================*/
void help(void)
{
        printf("  i.mx27 FEC driver test\n");
        printf("  Usage : -T <test_num>. To put test case number\n");
        printf("                  0 : Show information about current ethernet interface(eth0 by defaults)\n");
        printf("                  1 : Send ping signal to specified host\n");
        printf("                  2 : Change main interface parameters(such as ip address, MTU size, etc)\n");
        printf("                  4 : Shut down and rise up current interface\n");
        printf("                  5 : Rise up current ethernet interface\n");
        printf("                  6 : Shut down current ethernet interface\n");
        printf("          -v Verbouse flag.\n");
        printf("          -H <ip_address> set ip address to ping for ping test\n");
        printf("                (if not set address will be prompted during test execution)\n");
        printf("          -b Send ping to default hardcoded address(For debug purpose only)\n");
        printf("          -D <iface_name> Choose interface name e.g. eth0, eth1\n");
        printf("                (default: eth0)\n\n");
}
