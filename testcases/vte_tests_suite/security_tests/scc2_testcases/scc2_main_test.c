/*================================================================================================*/
/**
        @file   scc_test.c

        @brief  scc API test
*/
/*==================================================================================================

        Copyright 2006, Freescale Semiconductor, Inc. All Rights Reserved
        THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
        BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
        Freescale Semiconductor, Inc.

====================================================================================================
Revision History:
                             Modification    Tracking
Author/core ID                  Date          Number    Description of Changes
-------------------------   ------------    ----------  -------------------------------------------
A.Urusov/NONE                27/09/2005     TLSbo55835  Initial version
A.Urusov/NONE                17/11/2005     TLSbo58839  Help and some diagnose messages edited
A.Ozerov/NONE                03/01/2006     TLSbo61735  Mapping was removed

====================================================================================================
Total Tests: 1

Test Executable Name:  scc_test

Test Strategy: Examine the SCC driver common software operations
=================================================================================================*/

/*==================================================================================================
                                        INCLUDE FILES
==================================================================================================*/
/* Verification Test Environment Include Files */
#include "scc2_test.h"

/*==================================================================================================
                                       GLOBAL VARIABLES
==================================================================================================*/
/** Test program identifier.  */
char   *TCID = "scc2_test";
/** Total number of tests in this file.  */
int     TST_TOTAL = 2;
/** */
int     debug = FALSE;
/** */
int     superbrief_read_register = FALSE;
/** */
int     inject_crc_error = FALSE;
/** */
int     encrypt_only = FALSE;
/** */
int     plaintext_length = DEFAULT_PLAINTEXT_LENGTH;
/** Secret which is going to be encrypted and decrtyped. See also #init_plaintext */
uint8_t plaintext[4096] = 
        { 0xca, 0xbb, 0xad, 0xba,
          0xde, 0xad, 0xbe, 0xef,
          0xdb, 0xea, 0x11, 0xbe,
          'A', 'B', 'C', 'D',
          0x73, 0x1c, 0xab, 0xad,
          0xab, 0xad, 0xac, 0x24
};
/** */
scc_cypher_mode_t crypto_mode =  SCC_CYPHER_MODE_ECB;
/** */
int     byte_offset = 0;
/** */
int     encrypt_padding_allowance = DEFAULT_PADDING_ALLOWANCE;
/** */
int     decrypt_padding_allowance = 0;
/** */
scc_verify_t check_mode = SCC_VERIFY_MODE_NONE;
/** */
char    test_switch;
/** */
char   *test_to_run = "";
/** The SCC device */
int     scc_fd;
/** */
uint32_t timer_value = 0x5f0000;

/*==================================================================================================
                                   GLOBAL FUNCTION PROTOTYPES
==================================================================================================*/
void        help(void);
void        cleanup(void);
void        setup(void);
int         main(int argc, char **argv);

/*==================================================================================================
                                       LOCAL FUNCTIONS
==================================================================================================*/
/*===== cleanup =====*/
/**
@brief  Performs all one time clean up for this test on successful
                completion,  premature exit or  failure. Closes all temporary
                files, removes all temporary directories exits the test with
                appropriate return code by calling tst_exit() function.cleanup

@param
  
@return
*/
/*================================================================================================*/
void cleanup(void)
{
        int     VT_rv = TFAIL;

        VT_rv = VT_scc2_test_cleanup();
        if (VT_rv != TPASS)
        {
                tst_resm(TFAIL, "VT_scc_cleanup() failed : error code = %d", VT_rv);
        }
        else
        {
                tst_resm(TPASS, "VT_scc_cleanup() passed : code = %d", VT_rv);
        }

        /* Exit with appropriate return code. */
        tst_exit();
}

/*================================================================================================*/
/*===== setup =====*/
/**
@brief  Performs all one time setup for this test. This function is
        typically used to capture signals, create temporary dirs
        and temporary files that may be used in the course of this test.

@param
  
@return On failure - Exits by calling cleanup().
*/
/*================================================================================================*/
void setup(void)
{
        int     VT_rv = VT_scc2_test_setup();

        if (VT_rv == TFAIL)
        {
                tst_brkm(TBROK, cleanup, "VT_scc2_setup() failed : error code = %d", VT_rv);

        }
        else
        {
                tst_resm(TPASS, "VT_scc2_setup() passed: code = %d", VT_rv);
        }
}

/*================================================================================================*/
/*===== help =====*/
/**
@brief  Performs print of this test help.

@param

@return
*/
/*================================================================================================*/
void help(void)
{
        printf( "Usage: scc_test [-T][-R offset][-W offset:value][-L tests][-S+/-options]\n"
                "-T (set timer initial value to 44)\n"
                "-R offset (read value from offset)\n"
                "-W offset:value (write value into offset)\n" "-L tests,\n"
                "   where 'tests' can be:\n"
                "     a (test signal a software alarm to the SCC)\n"
                "     c (run AIC tests)\n" 
                "     C (display SCC configuration)\n"
                "     e (encryption and decryption tests)\n"
                "     m (start monitor security failure)\n"
                "     M (stop monitor security failure)\n"
                "     r (print all MXC91231 and SMN registers values)\n"
                "     s (print values and verify access of all 'always available' registers)\n"
                "     t (test the timer work)\n"
                "     z (zeroize Red and Black memories of the SCC)\n"
                "-S options to encryption and decryption tests,\n"
                "   where 'options' can be:\n"
                "  +/-c (corrupt ciphertext before decrypting)\n"
                "  +/-D (print debug messages in encryption/decryption tests)\n"
                "  +/-l value (set length of plaintext for cipher test, by default length = 24)\n"
                "  +/-m (set Cipher Block Chaining Mode / set Electronic Codebook Mode)\n"
                "  +/-o (change byte offset of ciphertext&recovered plaintext)\n"
                "  +/-p (change padding allowance of ciphertext buffer)\n"
                "  +/-Q (superbrief read register (quiet))\n"
                "  +/-v (turn on/off cipher verification mode)\n"
                "-H (display the help)\n");
}

/*================================================================================================*/
/*===== main =====*/
/**
@brief  Entry point to this test-case. It parses all the command line
        inputs, calls the global setup and executes the test. It logs
      	the test status and results appropriately using the LTP API's
        On successful completion or premature failure, cleanup() func
        is called and test exits with an appropriate return code.

@param  Input :      arg_count - number of command line parameters.
        Output:      *arg_list[] - pointers to the array of the command line parameters.

@return On failure - Exits by calling cleanup().
        On success - exits with 0 exit value.
*/

/*================================================================================================*/
int main(int arg_count, char *arg_list[]) 
{
        /* Declare and initialize variables */
        int             VT_rv = TPASS;
        int             argument_switch;
        unsigned        int offset;

        /* Perform plaintext initialization, call init_plaintext() function */
        init_plaintext();

        /* Perform global test setup, call setup() function */
        setup();

        /* Print test Assertion using tst_resm() function with argument TINFO */
        tst_resm(TINFO, "Testing if %s test case is OK", TCID);

        /* Process command line arguments - until we come up empty */
        while ((argument_switch = getopt(arg_count, arg_list, "L:R:MS:TW:H")) != EOF)
        {
                switch (argument_switch)
                {
                case 'L':      /* List tests to run */
                        test_to_run = optarg;
                        break;
                case 'R':      /* Read a specific register */
                {
                        uint32_t value;

                        if (optarg != 0)
                        {
                                /* get the offset */
                                if (sscanf(optarg, "%x", &offset) != 1)
                                {
                                        tst_resm(TINFO, "Improper use of -R %s\n", optarg);
                                }
                                else
                                {
                                        if (read_scc_register(offset, &value) == TPASS)
                                        {
                                                if (superbrief_read_register)
                                                {
                                                        tst_resm(TPASS,
                                                                 "Superbrief read register: Value %08X passed\n", value);
                                                }
                                                else
                                                {
                                                        tst_resm(TPASS,
                                                                 "Reading test register offset 0x%08X => 0X%08X passed\n",
                                                                 offset, value);
                                                }
                                        }
                                        else
                                        {
                                                VT_rv = TFAIL;
                                                tst_resm(TFAIL,
                                                         "Reading test register failed. Offset: 0x%08X",
                                                         offset);
                                        }
                                }
                        }
                        else
                        {
                                tst_resm(TINFO, "Test execution error. Options required!\n");
                        }
                }
                break;
                case 'S':      /* (re)set a switch */
                        switch (*(optarg + 1))
                        {
                        case 'c':
                        {
                                if (*optarg == '+')
                                {
                                        if (strlen(optarg) > 2)
                                        {
                                                if (sscanf
                                                    (optarg + 2, "%x", &inject_crc_error) != 1)
                                                {
                                                        tst_resm(TINFO,
                                                                 "Improper  hex value with -S+c\n");
                                                        inject_crc_error = 0x1959;
                                                }
                                        }
                                        else
                                                /* arbitrary change */
                                                inject_crc_error = 0x1959;
                                }
                                else
                                {
                                        inject_crc_error = 0;
                                }
                        }
                        break;
                        case 'D':
                                if (*optarg == '+')
                                {
                                        debug = TRUE;
                                }
                                else
                                {
                                        debug = FALSE;
                                }
                        break;
                        case 'e':
                                if (*optarg == '+')
                                {
                                        encrypt_only = TRUE;
                                }
                                else
                                {
                                        encrypt_only = FALSE;
                                }
                        break;
                        case 'l':
                                if (*optarg == '+')
                                {
                                        plaintext_length = atoi(optarg + 2);
                                        if (plaintext_length > sizeof(plaintext))
                                        {
                                                plaintext_length = sizeof(plaintext);
                                                tst_resm(TINFO,
                                                         "Plaintext size too large. Using %ld\n",
                                                         (long) plaintext_length);
                                        }
                                }
                                else
                                {
                                        plaintext_length = DEFAULT_PLAINTEXT_LENGTH;
                                }
                        break;
                        case 'm':      /* crypto mode */
                                if (*optarg == '+')
                                {
                                        crypto_mode =  SCC_CYPHER_MODE_CBC;
                                }
                                else
                                {
                                        crypto_mode =  SCC_CYPHER_MODE_ECB;
                                }
                        break;
                        case 'o':      /* byte offset */
                                if (*optarg == '+')
                                {
                                        byte_offset = atoi(optarg + 2);
                                }
                                else
                                {
                                        byte_offset = 0;
                                }
                        break;
                        case 'p':      /* ciphertext padding allowance */
                                if (*optarg == '+')
                                {
                                        encrypt_padding_allowance = atoi(optarg + 2);
                                }
                                else
                                {
                                        decrypt_padding_allowance = atoi(optarg + 2);
                                }
                        break;
                        case 'Q':      /* superbrief read register (quiet) */
                                if (*optarg == '+')
                                {
                                        superbrief_read_register = TRUE;
                                }
                                else
                                {
                                        superbrief_read_register = FALSE;
                                }
                        break;
                        case 'v':      /* verification */
                                if (*optarg == '+')
                                {
                                        check_mode = SCC_VERIFY_MODE_CCITT_CRC;
                                }
                                else
                                {
                                        check_mode = SCC_VERIFY_MODE_NONE;
                                }
                        break;
                        default:
                                tst_resm(TINFO, "Unknown switch %c\n", *(optarg + 1));
                        }
                break;
                case 'T':      /* change Timer Initial Value */
                        timer_value = 44;       /* code this up! */
                break;
                case 'W':      /* write a specific register */
                {
                        unsigned int value;

                        if (sscanf(optarg, "%x:%x", &offset, &value) != 2)
                        {
                                tst_resm(TINFO, "Improper use of -W (%s)\n", optarg);
                        }
                        else
                        {
                                if (write_scc_register(offset, value) == TPASS)
                                {
                                        if (read_scc_register(offset, &value) == TPASS)
                                        {
                                                tst_resm(TPASS,
                                                         "Reading test register offset 0x%08x => %08x passed\n",
                                                         offset, value);
                                        }
                                        else
                                        {
                                                VT_rv = TFAIL;
                                                tst_resm(TFAIL,
                                                         "Reading test register after write failed");
                                        }
                                }
                                else
                                {
                                        VT_rv = TFAIL;
                                        tst_resm(TFAIL,
                                                 "Writing test register offset 0x%08x <= %08x failed",
                                                 offset, value);
                                }
                        }
                }
                        test_to_run = "";
                break;
                case 'H':
                {
                        help(); /* Print the list of test execution parameters */
                        VT_rv = TPASS;
                }
                break;
                default:
                        help(); /* Print the list of test execution parameters */
                break;
                }
        }

        if (test_to_run != "")
        {
                VT_rv = VT_scc2_test();
        }

        if (VT_rv == TPASS)
        {
                tst_resm(TPASS, "Test case %s worked as expected", TCID);
        }
        else
        {
                tst_resm(TFAIL, "Test case %s did NOT work as expected", TCID);
        }

        /* cleanup test allocated ressources */
        cleanup();

        return VT_rv;

}
