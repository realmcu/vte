/*====================*/
/**
        @file   scc_test.c

        @brief  scc API test
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
A.Urusov/NONE                27/09/2005     TLSbo55835  Initial version
A.Urusov/NONE                17/11/2005     TLSbo58839  The status variable is added into
                                                        read_scc_register and write_scc_register
                                                        routines
A.Urusov/NONE                18/11/2005     TLSbo58839  Decrypt failed error is fixed
D.Simakov                    13/11/2006     TLSbo80386  SCC : tests are unable to access registers for i.MX27ADS
A.Ozerov/b00320              23/11/2006     TLSbo80386  call of VT_scc_test_cleanup was removed.
======================
Total Tests: 1

Test Executable Name:  scc_test

Test Strategy: Examine the SCC driver common software operations
=====================

======================
                                        INCLUDE FILES
======================*/
#include "scc_test.h"

/*======================
                                        LOCAL FUNCTIONS
======================*/
/*= init_plaintext =*/
/**
@brief  Performs plaintext initialization.

@param

@return
*/
/*====================*/
void init_plaintext(void)
{
        int     i;

        /* Starting after precompiled values, fill up the rest */
        for (i = 24; i < sizeof(plaintext); i++)
        {
                plaintext[i] = i % 256;
        }
}

/*====================*/
/*= VT_scc_test_setup =*/
/**
@brief  Performs all one time setup for this test.

@param

@return On failure - TFAIL
        On success - TPASS
*/
/*====================*/
int VT_scc_test_setup(void)
{
        char    f_name[256] = "/dev/";
        int     VT_rv = TPASS;

        strcat(f_name, SCC_TEST_DEVICE_NAME);

        if ((scc_fd = open(f_name, O_RDWR)) < 0)
        {
                VT_rv = TFAIL;
                tst_resm(TFAIL, "Failed open device %s", SCC_TEST_DEVICE_NAME);
        }
        else
        {
                tst_resm(TPASS, "Device named %s is opened", SCC_TEST_DEVICE_NAME);
        }
        return VT_rv;
}

/*====================*/
/*= VT_scc_test_cleanup =*/
/**
@brief  Performs all one time clean up for this test.

@param

@return On failure - TFAIL
        On success - TPASS
*/
/*====================*/
int VT_scc_test_cleanup(void)
{
        int     VT_rv = TPASS;

        if (scc_fd > 0)
        {
                close(scc_fd);
                tst_resm(VT_rv, "Device named %s is closed", SCC_TEST_DEVICE_NAME);
        }
        else
        {
                VT_rv = TFAIL;
                tst_resm(VT_rv, "Failed close device %s", SCC_TEST_DEVICE_NAME);
        }
        return VT_rv;
}

/*=========================*/
/*= VT_scc_test =*/
/**
@brief  Performs loop through all of the requested tests, running each in turn.

@param

@return On failure - TFAIL
        On success - TPASS
*/
/*====================*/
int VT_scc_test(void)
{
        int     VT_rv = TPASS;

        while ((test_switch = *test_to_run++) != 0)
        {
                switch (test_switch)
                {
                case 'a':
                        tst_resm(TINFO, "Test signal a software alarm to the SCC");
                        VT_rv = set_software_alarm_test();
                        break;
                case 'c':
                        tst_resm(TINFO, "AIC tests");
                        VT_rv = run_aic_tests();
                        break;
                case 'C':
                        tst_resm(TINFO, "SCC configuration");
                        VT_rv = display_configuration_test();
                        break;
                case 'e':
                        tst_resm(TINFO, "Encryption and decryption tests");
                        VT_rv = run_cipher_tests();
                        break;
                case 'm':
                        tst_resm(TINFO, "Test start monitor security failure");
                        VT_rv = scc_test_monitor_security_failure();
                        break;
                case 'M':
                        tst_resm(TINFO, "Test stop monitor security failure");
                        scc_test_stop_monitoring_security_failure();
                        break;
                case 'r':
                        tst_resm(TINFO, "Print all SCM and SMN registers values");
                        VT_rv = dump_registers_test();
                        break;
                case 's':      /* the always available ones */
                        tst_resm(TINFO,
                                 "Print values and verify access of all 'always available' registers");
                        VT_rv = check_safe_registers_test();
                        break;
                case 't':
                        tst_resm(TINFO, "Test the timer work");
                        VT_rv = run_timer_tests(timer_value);
                        break;
                case 'z':
                        tst_resm(TINFO, "Zeroize Red and Black memories of the SCC");
                        VT_rv = run_zeroize_tests();
                        break;
                   case 'd':
                           tst_resm(TINFO,"Check the register safe after do some operation");
                           VT_rv=check_register_safe_after_operation();
                default:
                        tst_resm(TINFO, "Test switch %c unknown\n", test_switch);
                }
        }

        return VT_rv;
}

/*====================*/
/*= display_configuration =*/
/**
@brief  Configuration of SCC displaying.

@param

@return On failure - TFAIL
        On success - TPASS
*/
/*====================*/
int display_configuration_test(void)
{
        scc_configuration_access        *config;
        int                             VT_rv = TPASS;

        config = get_scc_configuration(scc_fd);

        if (NULL == config)
        {
                VT_rv = TFAIL;
                tst_resm(TFAIL, "Cannot display SCC Configuration");
        }
        else
        {
                tst_resm(TPASS, "SCMversion: %d, SMN version: %d, driver version %d.%d",
                         config->scm_version, config->smn_version,
                         config->driver_major_version, config->driver_minor_version);
                tst_resm(TPASS, "SCMblock size is %d bytes, Black has %d blocks, "
                         "Red has %d blocks", config->block_size,
                         config->black_ram_size, config->red_ram_size);
                free(config);
        }

        return VT_rv;
}

/*====================*/
/*= check_safe_registers =*/
/**
@brief  Performs print values and verify access of all 'always available' registers

@param

@return On failure - TFAIL
        On success - TPASS
*/
/*====================*/
int check_safe_registers_test(void)
{
        int             VT_rv = TPASS;
        uint32_t        value;

        if (read_scc_register(SCM_STATUS, &value) == TPASS)
        {
                tst_resm(TPASS, "(%08X) SCM Status     (0x%08x): ", SCM_STATUS, value);
                print_scm_status_register(value);
        }
        else
        {
                VT_rv = TFAIL;
                tst_resm(TFAIL, "Could not get (0x%08X) SCM Status Register", SCM_STATUS);
        }

        if (read_scc_register(SCM_ERROR_STATUS, &value) == TPASS)
        {
                tst_resm(TPASS, "(%08X) SCM Error Register     (0x%08x):", SCM_ERROR_STATUS, value);
                print_scc_error_status_register(value);
        }
        else
        {
                VT_rv = TFAIL;
                tst_resm(TFAIL, "Could not get (0x%08X) SCM Error Register", SCM_ERROR_STATUS);
        }

        if (read_scc_register(SCM_INTERRUPT_CTRL, &value) == TPASS)
        {
                tst_resm(TPASS, "(%08X) SCM Interrupt Control Register:  0x%08x\n",
                         SCM_INTERRUPT_CTRL, value);
        }
        else
        {
                VT_rv = TFAIL;
                tst_resm(TFAIL, "Could not get (0x%08X) SCM Interrupt Control Register",
                         SCM_INTERRUPT_CTRL);
        }

        if (read_scc_register(SCM_CONFIGURATION, &value) == TPASS)
        {
                tst_resm(TPASS, "(%08X) SCM Configuration Register:     0x%08x\n",
                         SCM_CONFIGURATION, value);
        }
        else
        {
                VT_rv = TFAIL;
                tst_resm(TFAIL, "Could not get (0x%08X) SCM Configuration Register",
                         SCM_CONFIGURATION);
        }

        tst_resm(TINFO, "");    /* Visually divide the halves */

        if (read_scc_register(SMN_STATUS, &value) == TPASS)
        {
                tst_resm(TPASS, "(%08X) SMN Status Register     (0x%08x): ", SMN_STATUS, value);
                print_smn_status_register(value);
        }
        else
        {
                VT_rv = TFAIL;
                tst_resm(TFAIL, "Could not get (0x%08X) SMN Status Register", SMN_STATUS);
        }

        if (read_scc_register(SMN_COMMAND, &value) == TPASS)
        {
                tst_resm(TPASS, "(%08X) SMN Command Register:     (0x%08x):", SMN_COMMAND, value);
                print_smn_command_register(value);
        }
        else
        {
                VT_rv = TFAIL;
                tst_resm(TFAIL, "Could not get (0x%08X) SMN Command Register", SMN_COMMAND);
        }

        if (read_scc_register(SMN_DEBUG_DETECT_STAT, &value) == TPASS)
        {
                tst_resm(TPASS, "(%08X) SMN Debug Detected Register:    (0x%08x):",
                         SMN_DEBUG_DETECT_STAT, value);
                print_scc_debug_detector_register(value);
        }
        else
        {
                VT_rv = TFAIL;
                tst_resm(TFAIL, "Could not get (0x%08X) SMN Debug Detected Register",
                         SMN_DEBUG_DETECT_STAT);
        }

        return VT_rv;
}

/*====================*/
/*= dump_registers =*/
/**
@brief  Performs print values of SCM and SMN registers

@param

@return On failure - TFAIL
        On success - TPASS
*/
/*====================*/
int dump_registers_test(void)
{
        int             VT_rv = TPASS;
        uint32_t        value;


        if (read_scc_register(SCM_RED_START, &value) == TPASS)
                tst_resm(TPASS, "(%08X) SCM Red Start Register:     0x%08x\n", SCM_RED_START,
                         value);
        else
        {
                VT_rv = TFAIL;
                tst_resm(TFAIL, "Could not get (0x%08X) SCM Red Start Register", SCM_RED_START);
        }

        if (read_scc_register(SCM_BLACK_START, &value) == TPASS)
                tst_resm(TPASS, "(%08X) SCM Black Start Register:     0x%08x\n", SCM_BLACK_START,
                         value);
        else
        {
                VT_rv = TFAIL;
                tst_resm(TFAIL, "Could not get (0x%08X) SCM Black Start Register", SCM_BLACK_START);
        }

        if (read_scc_register(SCM_LENGTH, &value) == TPASS)
                tst_resm(TPASS, "(%08X) SCM Length Register:     0x%08x\n", SCM_LENGTH, value);
        else
        {
                VT_rv = TFAIL;
                tst_resm(TFAIL, "Could not get (0x%08X) SCM Length Register", SCM_LENGTH);
        }

        if (read_scc_register(SCM_CONTROL, &value) == TPASS)
        {
                tst_resm(TPASS, "(%08X) SCM Control Register     (0x%08x): ", SCM_CONTROL, value);
                print_scm_control_register(value);
        }
        else
        {
                VT_rv = TFAIL;
                tst_resm(TFAIL, "Could not get (0x%08X) SCM Control Register", SCM_CONTROL);
        }

        if (read_scc_register(SCM_STATUS, &value) == TPASS)
        {
                tst_resm(TPASS, "(%08X) SCM Status     (0x%08x): ", SCM_STATUS, value);
                print_scm_status_register(value);
        }
        else
        {
                VT_rv = TFAIL;
                tst_resm(TFAIL, "Could not get (0x%08X) SCM Status", SCM_STATUS);
        }

        if (read_scc_register(SCM_ERROR_STATUS, &value) == TPASS)
        {
                tst_resm(TPASS, "(%08X) SCM Error Register     (0x%08x):", SCM_ERROR_STATUS, value);
                print_scc_error_status_register(value);
        }
        else
        {
                VT_rv = TFAIL;
                tst_resm(TFAIL, "Could not get (0x%08X) SCM Error Register", SCM_ERROR_STATUS);
        }

        if (read_scc_register(SCM_INTERRUPT_CTRL, &value) == TPASS)
        {
                tst_resm(TPASS, "(%08X) SCM Interrupt Control Register: 0x%08x\n",
                         SCM_INTERRUPT_CTRL, value);
        }
        else
        {
                VT_rv = TFAIL;
                tst_resm(TFAIL, "Could not get (0x%08X) SCM Interrupt Control Register",
                         SCM_INTERRUPT_CTRL);
        }

        if (read_scc_register(SCM_CONFIGURATION, &value) == TPASS)
        {
                tst_resm(TPASS, "(%08X) SCM Configuration Register:     0x%08x\n",
                         SCM_CONFIGURATION, value);
        }
        else
        {
                VT_rv = TFAIL;
                tst_resm(TFAIL, "Could not get (0x%08X) SCM Confuguration Register",
                         SCM_CONFIGURATION);
        }

        if (read_scc_register(SCM_INIT_VECTOR_0, &value) == TPASS)
        {
                tst_resm(TPASS, "(%08X) SCM Init Vector 0 Register:     0x%08x\n",
                         SCM_INIT_VECTOR_0, value);
        }
        else
        {
                VT_rv = TFAIL;
                tst_resm(TFAIL, "Could not get (0x%08X) SCM Init Vector 0 Register",
                         SCM_INIT_VECTOR_0);
        }

        if (read_scc_register(SCM_INIT_VECTOR_1, &value) == TPASS)
        {
                tst_resm(TPASS, "(%08X) SCM Init Vector 1 Register:     0x%08x\n",
                         SCM_INIT_VECTOR_1, value);
        }
        else
        {
                VT_rv = TFAIL;
                tst_resm(TFAIL, "Could not get (0x%08X) SCM Init Vector 1 Register",
                         SCM_INIT_VECTOR_1);
        }

        tst_resm(TINFO, "");    /* Visually divide the halves */

        if (read_scc_register(SMN_STATUS, &value) == TPASS)
        {
                tst_resm(TPASS, "(%08X) SMN Status Register     (0x%08x): ", SMN_STATUS, value);
                print_smn_status_register(value);
        }
        else
        {
                VT_rv = TFAIL;
                tst_resm(TFAIL, "Could not get (0x%08X) SMN Status Register", SMN_STATUS);
        }

        if (read_scc_register(SMN_COMMAND, &value) == TPASS)
        {
                tst_resm(TPASS, "(%08X) SMN Command Register:     (0x%08x):", SMN_COMMAND, value);
                print_smn_command_register(value);
        }
        else
        {
                VT_rv = TFAIL;
                tst_resm(TFAIL, "Could not get (0x%08X) SMN Command Register", SMN_COMMAND);
        }

        if (read_scc_register(SMN_SEQUENCE_START, &value) == TPASS)
        {
                tst_resm(TPASS, "(%08X) SMN Sequence Start Register:    0x%08x\n",
                         SMN_SEQUENCE_START, value);
        }
        else
        {
                VT_rv = TFAIL;
                tst_resm(TFAIL, "Could not get (0x%08X) SMN Sequence Start Register",
                         SMN_SEQUENCE_START);
        }

        if (read_scc_register(SMN_SEQUENCE_END, &value) == TPASS)
        {
                tst_resm(TPASS, "(%08X) SMN Sequence End Register:     0x%08x\n", SMN_SEQUENCE_END,
                         value);
        }
        else
        {
                VT_rv = TFAIL;
                tst_resm(TFAIL, "Could not get (0x%08X) SMN Sequence End Register",
                         SMN_SEQUENCE_END);
        }

        if (read_scc_register(SMN_SEQUENCE_CHECK, &value) == TPASS)
        {
                tst_resm(TPASS, "(%08X) SMN Sequence Check Register:    0x%08x\n",
                         SMN_SEQUENCE_CHECK, value);
        }
        else
        {
                VT_rv = TFAIL;
                tst_resm(TFAIL, "Could not get (0x%08X) SMN Sequence Check Register",
                         SMN_SEQUENCE_CHECK);
        }

        if (read_scc_register(SMN_BIT_COUNT, &value) == TPASS)
        {
                tst_resm(TPASS, "(%08X) SMN Bit Count Register:     0x%08x\n", SMN_BIT_COUNT,
                         value);
        }
        else
        {
                VT_rv = TFAIL;
                tst_resm(TFAIL, "Could not get (0x%08X) SMN Bit Count Register", SMN_BIT_COUNT);
        }

        if (read_scc_register(SMN_BITBANK_INC_SIZE, &value) == TPASS)
        {
                tst_resm(TPASS, "(%08X) SMN Bit Bank Inc Size Register: 0x%08x\n",
                         SMN_BITBANK_INC_SIZE, value);
        }
        else
        {
                VT_rv = TFAIL;
                tst_resm(TFAIL, "Could not get (0x%08X) SMN Bit Bank Inc Size Register",
                         SMN_BITBANK_INC_SIZE);
        }

        if (read_scc_register(SMN_COMPARE_SIZE, &value) == TPASS)
        {
                tst_resm(TPASS, "(%08X) SMN Compare  Size Register:     0x%08x\n", SMN_COMPARE_SIZE,
                         value);
        }
        else
        {
                VT_rv = TFAIL;
                tst_resm(TFAIL, "Could not get (0x%08X) SMN Compare Size Register",
                         SMN_COMPARE_SIZE);
        }

        if (read_scc_register(SMN_PLAINTEXT_CHECK, &value) == TPASS)
        {
                tst_resm(TPASS, "(%08X) SMN Plaintext Check Register:  0x%08x\n",
                         SMN_PLAINTEXT_CHECK, value);
        }
        else
        {
                VT_rv = TFAIL;
                tst_resm(TFAIL, "Could not get (0x%08X) SMN Plaintext Check Register",
                         SMN_PLAINTEXT_CHECK);
        }

        if (read_scc_register(SMN_CIPHERTEXT_CHECK, &value) == TPASS)
        {
                tst_resm(TPASS, "(%08X) SMN Ciphertext  Check Register: 0x%08x\n",
                         SMN_CIPHERTEXT_CHECK, value);
        }
        else
        {
                VT_rv = TFAIL;
                tst_resm(TFAIL, "Could not get (0x%08X) SMN Ciphertext Check Register",
                         SMN_CIPHERTEXT_CHECK);
        }

        if (read_scc_register(SMN_TIMER_IV, &value) == TPASS)
        {
                tst_resm(TPASS, "(%08X) SMN Timer Initial Value Register:     0x%08x\n",
                         SMN_TIMER_IV, value);
        }
        else
        {
                VT_rv = TFAIL;
                tst_resm(TFAIL, "Could not get (0x%08X) SMN Timer Initial Value Register",
                         SMN_TIMER_IV);
        }

        if (read_scc_register(SMN_TIMER_CONTROL, &value) == TPASS)
        {
                tst_resm(TPASS, "(%08X) SMN Timer Control Register     (0x%08x): ",
                         SMN_TIMER_CONTROL, value);
                print_smn_timer_control_register(value);
        }
        else
        {
                VT_rv = TFAIL;
                tst_resm(TFAIL, "Could not get (0x%08X) SMN Timer Control Register",
                         SMN_TIMER_CONTROL);
        }

        if (read_scc_register(SMN_DEBUG_DETECT_STAT, &value) == TPASS)
        {
                tst_resm(TPASS, "(%08X) SMN Debug Detected Register:   (0x%08x):",
                         SMN_DEBUG_DETECT_STAT, value);
                print_scc_debug_detector_register(value);
        }
        else
        {
                VT_rv = TFAIL;
                tst_resm(TFAIL, "Could not get (0x%08X) SMN Debug Detected Register",
                         SMN_DEBUG_DETECT_STAT);
        }

        if (read_scc_register(SMN_TIMER, &value) == TPASS)
        {
                tst_resm(TPASS, "(%08X) SMN Timer Register:     (0x%08x)\n", SMN_TIMER, value);
        }
        else
        {
                VT_rv = TFAIL;
                tst_resm(TFAIL, "Could not get (0x%08X) SMN Timer Register", SMN_TIMER);
        }

        return VT_rv;
}

/*====================*/
/*= run_cipher_tests =*/
/**
@brief  Performs Encryption and Decryption Tests

@param

@return On failure - TFAIL
        On success - TPASS
*/
/*====================*/
int run_cipher_tests(void)
{
        scc_encrypt_decrypt     cipher_control;
        uint32_t                value; /* value of various registers */
        int                     VT_rv = TPASS;
        uint8_t                 *ciphertext = NULL;
        uint8_t                 *new_plaintext = NULL;

        ciphertext = malloc(sizeof(plaintext)) + byte_offset;
        new_plaintext = malloc(sizeof(plaintext) + byte_offset);

        cipher_control.data_in = plaintext;
        cipher_control.data_in_length = plaintext_length;
        cipher_control.data_out = ciphertext;
        /* sufficient for block and padding */
        cipher_control.data_out_length = plaintext_length + encrypt_padding_allowance;

        cipher_control.init_vector[0] = 0;
        cipher_control.init_vector[1] = 0;

        cipher_control.crypto_mode = crypto_mode;
        cipher_control.check_mode = check_mode;
        cipher_control.wait_mode = SCC_ENCRYPT_POLL;

        /* clear these to make sure they are set by driver */
        if (write_scc_register(SCM_INIT_VECTOR_0, 0) == TPASS)
        {
                tst_resm(TPASS, "Writing 0 into SCM_INIT_VECTOR_0 (0x%08X) passed",
                         SCM_INIT_VECTOR_0);
        }
        else
        {
                VT_rv = TFAIL;
                tst_resm(TFAIL, "Writing 0 into SCM_INIT_VECTOR_0 (0x%08X) failed",
                         SCM_INIT_VECTOR_0);
        }
        if (write_scc_register(SCM_INIT_VECTOR_1, 0) == TPASS)
        {
                tst_resm(TPASS, "Writing 0 into SCM_INIT_VECTOR_1 (0x%08X) passed",
                         SCM_INIT_VECTOR_1);
        }
        else
        {
                VT_rv = TFAIL;
                tst_resm(TFAIL, "Writing 0 into SCM_INIT_VECTOR_1 (0x%08X) failed",
                         SCM_INIT_VECTOR_1);
        }

        /* Start Encryption */
        tst_resm(TINFO, "CIPHER: Start Encrypting....");
        if (ioctl(scc_fd, SCC_TEST_ENCRYPT, &cipher_control) != SCC_RET_OK)
        {
                VT_rv = TFAIL;
                tst_resm(TFAIL, "Encryption failed");
                print_scc_return_code(cipher_control.function_return_code);
        }
        else
        {
                if (debug)      /* used 'D' option */
                {
                        dump_cipher_control_block(&cipher_control);
                        print_ram_data((void *) ciphertext, SCM_BLACK_MEMORY, 32);

                        if (read_scc_register(SCM_STATUS, &value) == TPASS)
                        {
                                tst_resm(TPASS, "Reading SCM_STATUS (0x%08X) value: 0x%08X",
                                         SCM_STATUS, value);
                        }
                        else
                        {
                                VT_rv = TFAIL;
                                tst_resm(TFAIL, "Reading SCM_STATUS (0x%08X) value: 0x%08X",
                                         SCM_STATUS, value);
                        }
                        print_scm_status_register(value);

                        if (read_scc_register(SMN_STATUS, &value) == TPASS)
                        {
                                tst_resm(TPASS, "Reading SMN_STATUS (0x%08X) value: 0x%08X",
                                         SMN_STATUS, value);
                        }
                        else
                        {
                                VT_rv = TFAIL;
                                tst_resm(TFAIL, "Reading SMN_STATUS (0x%08X) value: 0x%08X",
                                         SMN_STATUS, value);
                        }
                        print_smn_status_register(value);

                        read_scc_register(SCM_ERROR_STATUS, &value);
                        if (read_scc_register(SCM_ERROR_STATUS, &value) == TPASS)
                        {
                                tst_resm(TPASS, "Reading SCM_ERROR_STATUS (0x%08X) value: 0x%08X",
                                         SCM_ERROR_STATUS, value);
                        }
                        else
                        {
                                VT_rv = TFAIL;
                                tst_resm(TFAIL, "Reading SCM_ERROR_STATUS (0x%08X) value: 0x%08X",
                                         SCM_ERROR_STATUS, value);
                        }
                        print_scc_error_status_register(value);
                }
                if (VT_rv == TPASS)
                {
                        tst_resm(TPASS, "***End of Encryption");
                }
                else
                {
                        tst_resm(TFAIL, "***Encryption not ended");
                }

                if (!encrypt_only)      /* not used 'e' option */
                {
                        /* wipe out any remnants of encryption */
                        run_zeroize_tests();

                        /* DECRYPTION TEST */

                        cipher_control.data_in = ciphertext;    /* feed ciphertext back in */
                        cipher_control.data_in_length = cipher_control.data_out_length;
                        cipher_control.data_out = new_plaintext;
                        cipher_control.data_out_length =
                            plaintext_length + decrypt_padding_allowance;

                        if (inject_crc_error)
                        {
                                ciphertext[rand() % cipher_control.data_in_length] ^= 1;
                        }

                        if (crypto_mode == SCC_CBC_MODE)
                        {
                                cipher_control.init_vector[0] = 0;
                                cipher_control.init_vector[1] = 0;
                        }

                        cipher_control.crypto_mode = crypto_mode;
                        cipher_control.check_mode = check_mode;
                        cipher_control.wait_mode = SCC_ENCRYPT_POLL;

                        /* clear these again to make sure they are set by driver */
                        if (write_scc_register(SCM_INIT_VECTOR_0, 0) == TPASS)
                        {
                                tst_resm(TPASS, "Writing 0 into SCM_INIT_VECTOR_0 (0x%08X) passed",
                                         SCM_INIT_VECTOR_0);
                        }
                        else
                        {
                                VT_rv = TFAIL;
                                tst_resm(TFAIL, "Writing 0 into SCM_INIT_VECTOR_0 (0x%08X) failed",
                                         SCM_INIT_VECTOR_0);
                        }
                        if (write_scc_register(SCM_INIT_VECTOR_1, 0) == TPASS)
                        {
                                tst_resm(TPASS, "Writing 0 into SCM_INIT_VECTOR_1 (0x%08X) passed",
                                         SCM_INIT_VECTOR_1);
                        }
                        else
                        {
                                VT_rv = TFAIL;
                                tst_resm(TFAIL, "Writing 0 into SCM_INIT_VECTOR_1 (0x%08X) failed",
                                         SCM_INIT_VECTOR_1);
                        }
                        if (write_scc_register(SCM_INTERRUPT_CTRL,
                                               SCM_INTERRUPT_CTRL_CLEAR_INTERRUPT
                                               | SCM_INTERRUPT_CTRL_MASK_INTERRUPTS) == TPASS)
                        {
                                tst_resm(TPASS,
                                         "Writing 0x%08X into SCM_INTERRUPT_CTRL (0x%08X) passed",
                                         SCM_INTERRUPT_CTRL_CLEAR_INTERRUPT |
                                         SCM_INTERRUPT_CTRL_MASK_INTERRUPTS, SCM_INTERRUPT_CTRL);
                        }
                        else
                        {
                                VT_rv = TFAIL;
                                tst_resm(TFAIL,
                                         "Writing 0x%08X into SCM_INTERRUPT_CTRL (0x%08X) failed",
                                         SCM_INTERRUPT_CTRL_CLEAR_INTERRUPT |
                                         SCM_INTERRUPT_CTRL_MASK_INTERRUPTS, SCM_INTERRUPT_CTRL);
                        }

                        /* Start Decryption */
                        tst_resm(TINFO, "CIPHER: Start Decrypting....");
                        if (ioctl(scc_fd, SCC_TEST_DECRYPT, &cipher_control) != SCC_RET_OK)
                        {
                                tst_resm(TFAIL, "Decryption failed");
                                print_scc_return_code(cipher_control.function_return_code);
                        }
                        else
                        {
                                int     bytes_to_check = plaintext_length;

                                if (debug)
                                {

                                        dump_cipher_control_block(&cipher_control);
                                        print_ram_data((void *) new_plaintext, SCM_RED_MEMORY, 10);

                                        if (read_scc_register(SCM_STATUS, &value) == TPASS)
                                        {
                                                tst_resm(TPASS,
                                                         "Reading SCM_STATUS (0x%08X) value: 0x%08X",
                                                         SCM_STATUS, value);
                                        }
                                        else
                                        {
                                                VT_rv = TFAIL;
                                                tst_resm(TFAIL,
                                                         "Reading SCM_STATUS (0x%08X) value: 0x%08X",
                                                         SCM_STATUS, value);
                                        }
                                        print_scm_status_register(value);

                                        if (read_scc_register(SMN_STATUS, &value) == TPASS)
                                        {
                                                tst_resm(TPASS,
                                                         "Reading SMN_STATUS (0x%08X) value: 0x%08X",
                                                         SMN_STATUS, value);
                                        }
                                        else
                                        {
                                                VT_rv = TFAIL;
                                                tst_resm(TFAIL,
                                                         "Reading SMN_STATUS (0x%08X) value: 0x%08X",
                                                         SMN_STATUS, value);
                                        }
                                        print_smn_status_register(value);

                                        read_scc_register(SCM_ERROR_STATUS, &value);
                                        if (read_scc_register(SCM_ERROR_STATUS, &value) == TPASS)
                                        {
                                                tst_resm(TPASS,
                                                         "Reading SCM_ERROR_STATUS (0x%08X) value: 0x%08X",
                                                         SCM_ERROR_STATUS, value);
                                        }
                                        else
                                        {
                                                VT_rv = TFAIL;
                                                tst_resm(TFAIL,
                                                         "Reading SCM_ERROR_STATUS (0x%08X) value: 0x%08X",
                                                         SCM_ERROR_STATUS, value);
                                        }
                                        print_scc_error_status_register(value);
                                }

                                if (cipher_control.data_out_length != plaintext_length)
                                {
                                        tst_resm(TINFO,
                                                 "Error:  input plaintext length (%d) and output "
                                                 "plaintext length (%ld) do not match.\n",
                                                 plaintext_length, cipher_control.data_out_length);
                                }
                                while (bytes_to_check--)
                                {
                                        if (plaintext[bytes_to_check] !=
                                            new_plaintext[bytes_to_check])
                                        {
                                                tst_resm(TPASS,
                                                         "Error:  input plaintext (0x%02x) and "
                                                         "output plaintext (0x%02x) do not match at "
                                                         "offset %d.\n", plaintext[bytes_to_check],
                                                         new_plaintext[bytes_to_check],
                                                         bytes_to_check);
                                        }
                                }
                                tst_resm(TPASS, "Successful end of Cipher Test");
                        }       /* else decrypt failed */
                }       /* encrypt_only */
        }       /* else encrypt failed */

        return VT_rv;
}

/*========================*/
/*= check_register_safe_after_operation =*/
/**
@brief  Check Register safe after do encryption and decryption

@param

@return On failure - TFAIL
        On success - TPASS
*/
/*====================*/
int check_register_safe_after_operation(void)
{
      int     VT_rv = TPASS;
      tst_resm(TINFO, "Do encryption and decryption once again and check the register safe");
      VT_rv = run_cipher_tests();
      VT_rv = check_safe_registers_test();

}


/*====================*/
/*= set_software_alarm_test =*/
/**
@brief  Performs test signal a software alarm to the SCC

@param

@return On failure - TFAIL
        On success - TPASS
*/
/*====================*/
int set_software_alarm_test(void)
{
        int     VT_rv = TPASS;

        if (ioctl(scc_fd, SCC_TEST_SET_ALARM, NULL) == SCC_RET_OK)
        {
                tst_resm(TPASS, "Test signal a software alarm to the SCC passed");
        }
        else
        {
                VT_rv = TFAIL;
                tst_resm(TFAIL, "Test signal a software alarm to the SCC failed");
        }
        return VT_rv;
}

/*====================*/
/*= run_timer_tests =*/
/**
@brief  Performs Timer tests

@param  Input :      timer_iv - timer initial value

@return On failure - TFAIL
        On success - TPASS
*/
/*====================*/
int run_timer_tests(uint32_t timer_iv)
{
        uint32_t        value;
        int             i;
        int             VT_rv = TPASS;

        if (write_scc_register(SMN_TIMER_IV, timer_iv) == TPASS)
        {
                tst_resm(TPASS, "Writing 0x%X into SMN_TIMER_IV:0x%08X passed", timer_iv,
                         SMN_TIMER_IV);
        }
        else
        {
                VT_rv = TFAIL;
                tst_resm(TFAIL, "Writing 0x%X into SMN_TIMER_IV:0x%08X failed", timer_iv,
                         SMN_TIMER_IV);
        }

        /* this operation should move the initial value to the timer reg */
        if (write_scc_register(SMN_TIMER_CONTROL, SMN_TIMER_LOAD_TIMER) == TPASS)
        {
                tst_resm(TPASS, "Loading timer using SMN_TIMER_CONTROL:0x%08X passed",
                         SMN_TIMER_CONTROL);
        }
        else
        {
                VT_rv = TFAIL;
                tst_resm(TFAIL, "Loading timer using SMN_TIMER_CONTROL:0x%08X failed",
                         SMN_TIMER_CONTROL);
        }

        if (read_scc_register(SMN_TIMER, &value) == TPASS)
        {
                tst_resm(TPASS, "Read SMN_TIMER value: 0x%X passed", value);
        }
        else
        {
                VT_rv = TFAIL;
                tst_resm(TFAIL, "Read SMN_TIMER value failed");
        }

        if (write_scc_register(SMN_TIMER_CONTROL, SMN_TIMER_START_TIMER) == TPASS)
        {
                tst_resm(TPASS, "Starting timer using SMN_TIMER_CONTROL: 0x%08X passed",
                         SMN_TIMER_CONTROL);
        }
        else
        {
                VT_rv = TFAIL;
                tst_resm(TFAIL, "Starting timer using SMN_TIMER_CONTROL: 0x%08X failed",
                         SMN_TIMER_CONTROL);
        }

        /*
         * Kill some time - Only if compiler doesn't optimize this away!
         */
        for (i = 0; i < 100000; i++);

        /* now stop the timer */
        if (read_scc_register(SMN_TIMER_CONTROL, &value) == TPASS)
        {
                tst_resm(TPASS, "Reading the SMN_TIMER_CONTROL value (0x%X) passed", value);
        }
        else
        {
                VT_rv = TFAIL;
                tst_resm(TFAIL, "Reading the SMN_TIMER_CONTROL value failed");
        }

        if (write_scc_register(SMN_TIMER_CONTROL, value & ~SMN_TIMER_STOP_MASK) == TPASS)
        {
                tst_resm(TPASS, "Stopping the timer passed");
        }
        else
        {
                VT_rv = TFAIL;
                tst_resm(TFAIL, "Stopping the timer failed");
        }

        /* and see how much time we ran off */
        if (read_scc_register(SMN_TIMER, &value) == TPASS)
        {
                tst_resm(TPASS, "Reading how much time run off passed. SMN_TIMER value: 0x%X",
                         value);
        }
        else
        {
                VT_rv = TFAIL;
                tst_resm(TFAIL, "Reading how much time run off failed.");
        }

        return VT_rv;
}

/*====================*/
/*= run_aic_tests =*/
/**
@brief  Performs AIC tests

@param

@return On failure - TFAIL
        On success - TPASS
*/
/*====================*/
int run_aic_tests(void)
{
        uint32_t        value;
        int             VT_rv = TPASS;


        if (write_scc_register(SMN_SEQUENCE_START, 4) == TFAIL)
        {
                VT_rv = TFAIL;
                tst_resm(TFAIL, "Writing value into SMN_SEQUENCE_START (0x%08X) failed",
                         SMN_SEQUENCE_START);
        }
        else
        {
                tst_resm(TPASS, "Writing value into SMN_SEQUENCE_START (0x%08X) passed",
                         SMN_SEQUENCE_START);
        }

        if (write_scc_register(SMN_SEQUENCE_END, 5) == TFAIL)
        {
                VT_rv = TFAIL;
                tst_resm(TFAIL, "Writing value into SMN_SEQUENCE_END (0x%08X) failed",
                         SMN_SEQUENCE_END);
        }
        else
        {
                tst_resm(TPASS, "Writing value into SMN_SEQUENCE_END (0x%08X) passed",
                         SMN_SEQUENCE_END);
        }

        if (write_scc_register(SMN_SEQUENCE_CHECK, 5) == TFAIL)
        {
                VT_rv = TFAIL;
                tst_resm(TFAIL, "Writing value into SMN_SEQUENCE_CHECK (0x%08X) failed",
                         SMN_SEQUENCE_CHECK);
        }
        else
        {
                tst_resm(TPASS, "Writing value into SMN_SEQUENCE_CHECK (0x%08X) passed",
                         SMN_SEQUENCE_CHECK);
        }

        if (read_scc_register(SMN_STATUS, &value) == TFAIL)
        {
                VT_rv = TFAIL;
                tst_resm(TFAIL, "Reading value from SMN_STATUS (0x%08X) failed", SMN_STATUS);
        }
        else
        {
                tst_resm(TPASS, "Reading value from SMN_STATUS (0x%08X) passed", SMN_STATUS);
        }

        if (VT_rv == TPASS)
        {
                print_smn_status_register(value);
        }

        return VT_rv;
}

/*====================*/
/*= run_zeroize_tests =*/
/**
@brief  Performs Zeroize tests

@param

@return On failure - TFAIL
        On success - TPASS
*/
/*====================*/
int run_zeroize_tests(void)
{
        int             VT_rv = TPASS;
        uint32_t        value;

        /* set up some known values */
        if (write_scc_register(SCM_RED_MEMORY, 42) == TPASS)
        {
                tst_resm(TPASS, "Writing value (42) into SCM_RED_MEMORY (0x%08X) passed",
                         SCM_RED_MEMORY);
        }
        else
        {
                VT_rv = TFAIL;
                tst_resm(TFAIL, "Writing value (42) into SCM_RED_MEMORY (0x%08X) failed",
                         SCM_RED_MEMORY);
        }
        if (write_scc_register(SCM_RED_MEMORY + 1020, 42) == TPASS)
        {
                tst_resm(TPASS, "Writing value (42) into SCM_RED_MEMORY+0x00001020 (0x%08X) passed",
                         SCM_RED_MEMORY + 0x00001020);
        }
        else
        {
                VT_rv = TFAIL;
                tst_resm(TFAIL, "Writing value (42) into SCM_RED_MEMORY+0x00001020 (0x%08X) failed",
                         SCM_RED_MEMORY + 0x00001020);
        }
        if (write_scc_register(SCM_BLACK_MEMORY, 42) == TPASS)
        {
                tst_resm(TPASS, "Writing value (42) into SCM_BLACK_MEMORY (0x%08X) passed",
                         SCM_BLACK_MEMORY);
        }
        else
        {
                VT_rv = TFAIL;
                tst_resm(TFAIL, "Writing value (42) into SCM_BLACK_MEMORY (0x%08X) failed",
                         SCM_BLACK_MEMORY);
        }
        if (write_scc_register(SCM_BLACK_MEMORY + 1020, 42) == TPASS)
        {
                tst_resm(TPASS,
                         "Writing value (42) into SCM_BLACK_MEMORY+0x00001020 (0x%08X) passed",
                         SCM_BLACK_MEMORY + 0x00001020);
        }
        else
        {
                VT_rv = TFAIL;
                tst_resm(TFAIL,
                         "Writing value (42) into SCM_BLACK_MEMORY+0x00001020 (0x%08X) failed",
                         SCM_BLACK_MEMORY + 0x00001020);
        }

        if (ioctl(scc_fd, SCC_TEST_ZEROIZE, &value) != SCC_RET_OK)
        {
                VT_rv = TFAIL;
                tst_resm(TFAIL, "Zeroize test failed. Function return code: %0X", value);
        }
        else
        {
                /* no errors, verify values are gone */
                if (read_scc_register(SCM_RED_MEMORY, &value) == TPASS)
                {
                        tst_resm(TPASS, "Reading value (42) from SCM_RED_MEMORY (0x%08X) passed",
                                 SCM_RED_MEMORY);
                        if (value == 42)
                        {
                                VT_rv = TFAIL;
                                tst_resm(TFAIL, "Zeroize failed at 0x%08X", SCM_RED_MEMORY);
                        }
                        else
                        {
                                tst_resm(TPASS, "Zeroize passed at 0x%08X", SCM_RED_MEMORY);
                        }
                }
                else
                {
                        VT_rv = TFAIL;
                        tst_resm(TFAIL, "Reading value (42) from SCM_RED_MEMORY (0x%08X) failed",
                                 SCM_RED_MEMORY);
                }
                if (read_scc_register(SCM_RED_MEMORY + 1020, &value) == TPASS)
                {
                        tst_resm(TPASS,
                                 "Reading value (42) from SCM_RED_MEMORY+0x00001020 (0x%08X) passed",
                                 SCM_RED_MEMORY + 0x00001020);
                        if (value == 42)
                        {
                                VT_rv = TFAIL;
                                tst_resm(TFAIL, "Zeroize failed at 0x%08X",
                                         SCM_RED_MEMORY + 0x00001020);
                        }
                        else
                        {
                                tst_resm(TPASS, "Zeroize passed at 0x%08X",
                                         SCM_RED_MEMORY + 0x00001020);
                        }
                }
                else
                {
                        VT_rv = TFAIL;
                        tst_resm(TFAIL,
                                 "Reading value (42) from SCM_RED_MEMORY+0x00001020 (0x%08X) failed",
                                 SCM_RED_MEMORY + 0x00001020);
                }
                if (read_scc_register(SCM_BLACK_MEMORY, &value) == TPASS)
                {
                        tst_resm(TPASS, "Reading value (42) from SCM_BLACK_MEMORY (0x%08X) passed",
                                 SCM_BLACK_MEMORY);
                        if (value == 42)
                        {
                                VT_rv = TFAIL;
                                tst_resm(TFAIL, "Zeroize failed at 0x%08X", SCM_BLACK_MEMORY);
                        }
                        else
                        {
                                tst_resm(TPASS, "Zeroize passed at 0x%08X", SCM_BLACK_MEMORY);
                        }
                }
                else
                {
                        VT_rv = TFAIL;
                        tst_resm(TFAIL, "Reading value (42) from SCM_BLACK_MEMORY (0x%08X) failed",
                                 SCM_BLACK_MEMORY);
                }
                if (read_scc_register(SCM_BLACK_MEMORY + 1020, &value) == TPASS)
                {
                        tst_resm(TPASS,
                                 "Reading value (42) from SCM_BLACK_MEMORY+0x00001020 (0x%08X) passed",
                                 SCM_BLACK_MEMORY + 0x00001020);
                        if (value == 42)
                        {
                                VT_rv = TFAIL;
                                tst_resm(TFAIL, "Zeroize failed at 0x%08X",
                                         SCM_BLACK_MEMORY + 0x00001020);
                        }
                        else
                        {
                                tst_resm(TPASS, "Zeroize passed at 0x%08X",
                                         SCM_BLACK_MEMORY + 0x00001020);
                        }
                }
                else
                {
                        VT_rv = TFAIL;
                        tst_resm(TFAIL,
                                 "Reading value (42) from SCM_BLACK_MEMORY+0x00001020 (0x%08X) failed",
                                 SCM_BLACK_MEMORY + 0x00001020);
                }
        }
        return VT_rv;
}

/*====================*/
/*= write_scc_resgister =*/
/**
@brief  Performs write data into SCC register

@param  Input :      reg   - the register to be written
                     value - the value to store

@return On failure - TFAIL
        On success - TPASS
*/
/*====================*/
int write_scc_register(uint32_t reg, uint32_t value)
{
        scc_reg_access  register_access;
        int             VT_rv = TPASS;
        int             status;

        register_access.reg_offset = reg;
        register_access.reg_data = value;

        status = ioctl(scc_fd, SCC_TEST_WRITE_REG, &register_access);
        if ( status != SCC_RET_OK)
        {
                VT_rv = TFAIL;
        }

        return VT_rv;
}

/*====================*/
/*= read_scc_resgister =*/
/**
@brief  Performs read data from SCC register

@param  Input :      reg -   the register to be read
                     value - the location for return value
        Output:      register_access.reg_data - the return value

@return On failure - TFAIL
        On success - TPASS
*/
/*====================*/
int read_scc_register(uint32_t reg, uint32_t * value)
{
        scc_reg_access  register_access;
        int             VT_rv = TPASS;
        int             status;

        register_access.reg_offset = reg;
        status = ioctl(scc_fd, SCC_TEST_READ_REG, &register_access);

        if (status == SCC_RET_OK)
        {
                *value = register_access.reg_data;
        }
        else
        {
                VT_rv = TFAIL;
        }

        return VT_rv;
}

/*====================*/
/*= get_scc_configuration =*/
/**
@brief  Performs get SCC configuration

@param  Input :      scc_fd - the SCC device file descriptor

@return              config - the location for SCC configuration data
*/
/*====================*/
scc_configuration_access *get_scc_configuration(int scc_fd)
{
        static scc_configuration_access         *config = NULL;

        if (NULL == config)
        {
                config = calloc(1, sizeof(scc_configuration_access));
                if (NULL != config)
                {
                        if (ioctl(scc_fd, SCC_TEST_GET_CONFIGURATION, config) != SCC_RET_OK)
                        {
                                tst_resm(TFAIL, "SCC_GET_CONFIGURATION failed");
                                free(config);
                        }
                        else
                        {
                                tst_resm(TPASS, "SCC_GET_CONFIGURATION passed");
                        }
                }
        }

        return config;
}

/*====================*/
/*= print_ram_data =*/
/**
@brief  Performs print eight words per line, starting at ram, as though they
        started at address, until count words have been printed

@param  Input :      ram    -  start
                     address - byte address
                     count   - word counter

@return
*/
/*====================*/
void print_ram_data(uint32_t * ram, uint32_t address, int count)
{
        int     i;

        for (i = 0; i < count; i++)
        {
                if (i % 8 == 0)
                {
                        printf("Byte address: %04X ", address + i * 4); /* print byte *address */
                }

                printf("Word: %08X ", *ram++);  /* print a word */

                if (i % 8 == 7)
                {
                        printf("\n");   /* end of line - do newline */
                }
                else if (i % 8 == 3)
                {
                        printf(" ");    /* add space in the middle */
                }
        }

        if (count % 8 != 0)     /* if didn't have mod8 words... */
        {
                printf("\n");
        }
}

/*====================*/
/*= print_smn_status_register =*/
/**
@brief Interpret the SMN Status register and print out the 'on' bits and State

@param  Input :      status -  the status register address

@return 0
*/
/*====================*/
int print_smn_status_register(uint32_t status)
{
        int             version_id;
        uint8_t         state;
        char            *state_name = "";

        version_id = (status & SMN_STATUS_VERSION_ID_MASK) >> SMN_STATUS_VERSION_ID_SHIFT;
        state      = (status & SMN_STATUS_STATE_MASK) >> SMN_STATUS_STATE_SHIFT;
        state_name = get_smn_state_name(state);

        printf( "Version %d %s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s, \nState: %s\n",
                 version_id,
                 (status & SMN_STATUS_CACHEABLE_ACCESS) ? ", CACHEABLE_ACCESS" : "",
                 (status & SMN_STATUS_ILLEGAL_MASTER) ? ", ILLEGAL_MASTER" : "",
                 (status & SMN_STATUS_SCAN_EXIT) ? ", SCAN_EXIT" : "",
                 (status & SMN_STATUS_UNALIGNED_ACCESS) ? ", UNALIGNED_ACCESS" : "",
                 (status & SMN_STATUS_BYTE_ACCESS) ? ", BYTE_ACCESS" : "",
                 (status & SMN_STATUS_ILLEGAL_ADDRESS) ? ", ILLEGAL_ADDRESS" : "",
                 (status & SMN_STATUS_USER_ACCESS) ? ", USER_ACCESS" : "",
                 (status & SMN_STATUS_DEFAULT_KEY) ? ", DEFAULT_KEY" : "",
                 (status & SMN_STATUS_BAD_KEY) ? ", BAD_KEY" : "",
                 (status & SMN_STATUS_ILLEGAL_ACCESS) ? ", ILLEGAL_ACCESS" : "",
                 (status & SMN_STATUS_SCM_ERROR) ? ", SCM_ERROR" : "",
                 (status & SMN_STATUS_SMN_STATUS_IRQ) ? ", SMN_IRQ" : "",
                 (status & SMN_STATUS_SOFTWARE_ALARM) ? ", SOFTWARE_ALARM" : "",
                 (status & SMN_STATUS_TIMER_ERROR) ? ", TIMER_ERROR" : "",
                 (status & SMN_STATUS_PC_ERROR) ? ", PC_ERROR" : "",
                 (status & SMN_STATUS_ASC_ERROR) ? ", ASC_ERROR" : "",
                 (status & SMN_STATUS_SECURITY_POLICY_ERROR) ? ", SECURITY_POLICY_ERROR" : "",
                 (status & SMN_STATUS_DEBUG_ACTIVE) ? ", DEBUG_ACTIVE" : "",
                 (status & SMN_STATUS_ZEROIZE_FAIL) ? ", ZEROIZE_FAIL" : "",
                 (status & SMN_STATUS_INTERNAL_BOOT) ? ", INTERNAL_BOOT" : "", state_name);

        return 0;
}

/*====================*/
/*= get_smn_state_name =*/
/**
@brief Interpret the SMN Status

@param  Input :      state -  the register status

@return char*
*/
/*====================*/
char   *get_smn_state_name(const uint8_t state)
{
        switch (state)
        {
        case SMN_STATE_START:
                return "START";
        case SMN_STATE_ZEROIZE_RAM:
                return "ZEROIZE";
        case SMN_STATE_HEALTH_CHECK:
                return "HEALTH CHECK";
        case SMN_STATE_FAIL:
                return "FAIL";
        case SMN_STATE_SECURE:
                return "SECURE";
        case SMN_STATE_NON_SECURE:
                return "NON-SECURE";
        default:
                return "UNKNOWN";
        }
}

/*====================*/
/*= print_scm_status_register =*/
/**
@brief  Interpret the SCM Status register and print out the 'on' bits

@param  Input :      status -  the register status

@return
*/
/*====================*/
void print_scm_status_register(uint32_t status)
{
        printf( "%s%s%s%s%s%s%s%s%s%s%s%s\n",
                 (status & SCM_STATUS_LENGTH_ERROR) ? ", LENGTH_ERROR" : "",
                 (status & SCM_STATUS_BLOCK_ACCESS_REMOVED) ? ", NO_BLOCK_ACCESS" : "",
                 (status & SCM_STATUS_CIPHERING_DONE) ? ", CIPHERING_DONE" : "",
                 (status & SCM_STATUS_ZEROIZING_DONE) ? ", ZEROIZING_DONE" : "",
                 (status & SCM_STATUS_INTERRUPT_STATUS) ? ", INTERRUPT_STATUS" : "",
                 (status & SCM_STATUS_INTERNAL_ERROR) ? ", INTERNAL_ERROR" : "",
                 (status & SCM_STATUS_BAD_SECRET_KEY) ? ", BAD_SECRET_KEY" : "",
                 (status & SCM_STATUS_ZEROIZE_FAILED) ? ", ZEROIZE_FAILED" : "",
                 (status & SCM_STATUS_SMN_BLOCKING_ACCESS) ? ", SMN_BLOCKING_ACCESS" : "",
                 (status & SCM_STATUS_CIPHERING) ? ", CIPHERING" : "",
                 (status & SCM_STATUS_ZEROIZING) ? ", ZEROIZING" : "",
                 (status & SCM_STATUS_BUSY) ? ", BUSY" : "");
}

/*====================*/
/*= print_scm_control_register =*/
/**
@brief  Interpret the SCM Control register and print its meaning

@param  Input :      control - SCM control register

@return
*/
/*====================*/
void print_scm_control_register(uint32_t control)
{
        printf( "%s%s%s\n",
                 ((control & SCM_CONTROL_CIPHER_MODE_MASK) == SCM_DECRYPT_MODE) ?
                 "DECRYPT " : "ENCRYPT ",
                 ((control & SCM_CONTROL_CHAINING_MODE_MASK) == SCM_CBC_MODE) ?
                 "CBC " : "ECB ", (control & SCM_CONTROL_START_CIPHER) ? "CipherStart" : "");
}

/*====================*/
/*= print_scc_error_status_register =*/
/**
@brief  Interpret the SCC Error Status register and print its meaning

@param  Input :      error - error register

@return
*/
/*====================*/
void print_scc_error_status_register(uint32_t error)
{
        printf(  "%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s\n",
                 "",
                 (error & SCM_ERR_CACHEABLE_ACCESS) ? ", CACHEABLE_ACCESS" : "",
                 (error & SCM_ERR_ILLEGAL_MASTER) ? ", ILLEGAL_MASTER" : "",
                 (error & SCM_ERR_UNALIGNED_ACCESS) ? ", UNALIGNED_ACCESS" : "",
                 (error & SCM_ERR_BYTE_ACCESS) ? ", BYTE_ACCESS" : "",
                 (error & SCM_ERR_ILLEGAL_ADDRESS) ? ", ILLEGAL_ADDRESS" : "",
                 (error & SCM_ERR_USER_ACCESS) ? ", USER_ACCESS" : "",
                 (error & SCM_ERR_SECRET_KEY_IN_USE) ? ", SECRET_KEY_IN_USE" : "",
                 (error & SCM_ERR_INTERNAL_ERROR) ? ", INTERNAL_ERROR" : "",
                 (error & SCM_ERR_BAD_SECRET_KEY) ? ", BAD_SECRET_KEY" : "",
                 (error & SCM_ERR_ZEROIZE_FAILED) ? ", ZEROIZE_FAILED" : "",
                 (error & SCM_ERR_SMN_BLOCKING_ACCESS) ? ", SMN_BLOCKING_ACCESS" : "",
                 (error & SCM_ERR_CIPHERING) ? ", CIPHERING" : "",
                 (error & SCM_ERR_ZEROIZING) ? ", ZEROIZING" : "",
                 (error & SCM_ERR_BUSY) ? ", BUSY" : "");
}

/*====================*/
/*= print_smn_command_register =*/
/**
@brief  Interpret the SMN Command register and print its meaning

@param  Input :      command - command register

@return
*/
/*====================*/
void print_smn_command_register(uint32_t command)
{
        if (command & SMN_COMMAND_ZEROS_MASK)
        {
                printf(" zeroes: 0x%08x", debug & SMN_COMMAND_ZEROS_MASK);
        }
        printf(  "%s%s%s%s\n",
                 (command & SMN_COMMAND_CLEAR_INTERRUPT) ? " CLEAR_INTERRUPT" : "",
                 (command & SMN_COMMAND_CLEAR_BIT_BANK) ? " CLEAR_BITBANK" : "",
                 (command & SMN_COMMAND_ENABLE_INTERRUPT) ? " ENABLE_INTERRUPT" : "",
                 (command & SMN_COMMAND_SET_SOFTWARE_ALARM) ? " SET_SOFWARE_ALARM" : "");
}

/*====================*/
/*= print_smn_timer_control_register =*/
/**
@brief  Interpret the SMN Timer Control register and print its meaning

@param  Input :      control - timer control register

@return
*/
/*====================*/
void print_smn_timer_control_register(uint32_t control)
{
        if (control & SMN_TIMER_CTRL_ZEROS_MASK)
        {
                printf(" zeroes: 0x%08x", debug & SMN_TIMER_CTRL_ZEROS_MASK);
        }

        printf(  "%s%s\n",
                 (control & SMN_TIMER_LOAD_TIMER) ? " LOAD_TIMER" : "",
                 (control & SMN_TIMER_START_TIMER) ? " START_TIMER" : "");
        return;
}

/*====================*/
/*= print_scc_debug_detector_register =*/
/**
@brief  generate human-readable interpretation of the SMN Debug Detector Register

@param  Input :      debug - debug detector register

@return
*/
/*====================*/
void print_scc_debug_detector_register(uint32_t debug)
{
        if (debug & SMN_DBG_ZEROS_MASK)
        {
                printf(" zeroes: 0x%08x", debug & SMN_DBG_ZEROS_MASK);
        }
        printf(  "%s%s%s%s%s%s%s%s%s%s%s%s\n",
                 (debug & SMN_DBG_D1) ? " D1" : "",
                 (debug & SMN_DBG_D2) ? " D2" : "",
                 (debug & SMN_DBG_D3) ? " D3" : "",
                 (debug & SMN_DBG_D4) ? " D4" : "",
                 (debug & SMN_DBG_D5) ? " D5" : "",
                 (debug & SMN_DBG_D6) ? " D6" : "",
                 (debug & SMN_DBG_D7) ? " D7" : "",
                 (debug & SMN_DBG_D8) ? " D8" : "",
                 (debug & SMN_DBG_D9) ? " D9" : "",
                 (debug & SMN_DBG_D10) ? " D10" : "",
                 (debug & SMN_DBG_D11) ? " D11" : "", (debug & SMN_DBG_D12) ? " D12" : "");
}

/*====================*/
/*= print_scc_return_code =*/
/**
@brief  Print an interpretation (the symbol name) of @c code

@param  Input :      code - the @c code

@return
*/
/*====================*/
void print_scc_return_code(scc_return_t code)
{
        char   *msg = NULL;

        switch (code)
        {
        case SCC_RET_OK:
                msg = "SCC_RET_OK";
                break;
        case SCC_RET_FAIL:
                msg = "SCC_RET_FAIL";
                break;
        case SCC_RET_VERIFICATION_FAILED:
                msg = "SCC_RET_VERIFICATION_FAILED";
                break;
        case SCC_RET_TOO_MANY_FUNCTIONS:
                msg = "SCC_RET_TOO_MANY_FUNCTIONS";
                break;
        case SCC_RET_BUSY:
                msg = "SCC_RET_BUSY";
                break;
        case SCC_RET_INSUFFICIENT_SPACE:
                msg = "SCC_RET_INSUFFICIENT_SPACE";
                break;
        default:
                break;
        }

        if (msg)
        {
                printf("SCC return code: %s\n", msg);
        }
        else
        {
                printf("SCC return code: %d\n", code);
        }
}

/*====================*/
/*= dump_cipher_control_block =*/
/**
@brief  Print out various values of the Cipher Control Block

@param  Input :      cipher_control

@return
*/
/*====================*/
void dump_cipher_control_block(scc_encrypt_decrypt * cipher_control)
{
        printf("data_out_length: %ld", (long) cipher_control->data_out_length);
        if (cipher_control->crypto_mode == SCC_CBC_MODE)
        {
                printf("init_vector: 0x%08x%08x", cipher_control->init_vector[0],
                       cipher_control->init_vector[1]);
        }
}

/*====================*/
/*= scc_test_monitor_security_failure =*/
/**
@brief  Registering a function to be called should a Security Failure be signaled by the SCC

@param

@return On failure - TFAIL
        On success - TPASS
*/
/*====================*/
int scc_test_monitor_security_failure(void)
{
        int     VT_rv = TPASS;

        if (ioctl(scc_fd, SCC_TEST_SET_MON_SEC_FAIL, NULL) == SCC_RET_OK)
        {
                tst_resm(TPASS,
                         "Registering a function to be called should a Security Failure be signaled by the SCC are passed");
        }
        else
        {
                VT_rv = TFAIL;
                tst_resm(TFAIL,
                         "Registering a function to be called should a Security Failure be signaled by the SCC are failed");
        }

        return VT_rv;
}

/*====================*/
/*= scc_test_stop_monitoring_security_failure =*/
/**
@brief  Unregistering a function to be called should a Security Failure be signaled by the SCC

@param

@return On failure - TFAIL
        On success - TPASS
*/
/*====================*/
int scc_test_stop_monitoring_security_failure(void)
{
        int     VT_rv = TPASS;

        if (ioctl(scc_fd, SCC_TEST_STOP_MON_SEC_FAIL, NULL) == SCC_RET_OK)
        {
                tst_resm(TPASS,
                         "Unregistering a finction to be called should a Security Failure be signaled by the SCC are passed");
        }
        else
        {
                VT_rv = TFAIL;
                tst_resm(TFAIL,
                         "Unregistering a finction to be called should a Security Failure be signaled by the SCC are failed");
        }

        return VT_rv;
}
