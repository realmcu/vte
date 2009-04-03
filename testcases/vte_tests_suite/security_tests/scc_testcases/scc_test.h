/*================================================================================================*/
/**
        @file:   scc_test.h

        @brief:  SCC API test header file
*/
/*==================================================================================================

        Copyright 2006, Freescale Semiconductor, Inc. All Rights Reserved
        THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
        BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
        Freescale Semiconductor, Inc.

====================================================================================================
Revision History:
                            Modification     Tracking
Author/core ID                  Date          Number    Description of Changes
-------------------------   ------------    ----------  -------------------------------------------
A.Urusov/NONE                27/09/2005     TLSbo55835  Initial version
A.Ozerov/NONE                03/01/2006     TLSbo61735  Mapping was removed
Y.Batrakov/NONE              20/09/2006     TLSbo78563  Fixed compilation for imx27 platform

==================================================================================================*/
#ifdef __cplusplus
extern "C"{
#endif

#ifndef SCC_TEST_H
#define SCC_TEST_H

/*==================================================================================================
                                         INCLUDE FILES
==================================================================================================*/
/* Standard Include Files */
#include <fcntl.h>
#include <stdint.h>
#include <sys/ioctl.h>
#include <sys/mman.h>

/* Harness Specific Include Files. */
#include "test.h"
#include "usctest.h"

#include "scc_test_module.h"

/*==================================================================================================
                                       DEFINES AND MACROS
==================================================================================================*/
#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

/** Normal number of bytes to encrypt */
#define DEFAULT_PLAINTEXT_LENGTH 24

/** Normal number of bytes of padding to add to ciphertext storage */
#define DEFAULT_PADDING_ALLOWANCE 10

/*==================================================================================================
                                 GLOBAL VARIABLE DECLARATIONS
==================================================================================================*/
/** Test program identifier.  */
extern char *TCID;
/** Total number of tests in this file.  */
extern int TST_TOTAL;
/** */
extern int debug;
/** */
extern uint8_t plaintext[4096];
/** */
extern char test_switch;
/** */
extern char *test_to_run;
/** The SCC device */
extern int scc_fd;
/** */
extern uint32_t timer_value;
/** */
extern int byte_offset;
/** */
extern int plaintext_length;
/** */
extern int encrypt_padding_allowance;
/** */
extern scc_crypto_mode_t crypto_mode;
/** */
extern scc_verify_t check_mode;
/** */
extern int encrypt_only;
/** */
extern int decrypt_padding_allowance;
/** */
extern int inject_crc_error;
/** */
extern int superbrief_read_register;

/*==================================================================================================
                                     GLOBAL FUNCTION PROTOTYPES
==================================================================================================*/
void    help(void);

int     VT_scc_test_setup(void);
int     VT_scc_test_cleanup(void);
int     VT_scc_test(void);

/** test routines */
int     display_configuration_test(void);
int     check_safe_registers_test(void);
int     dump_registers_test(void);
int     run_aic_tests(void);
int     run_cipher_tests(void);
int     run_timer_tests(uint32_t);
int     run_zeroize_tests(void);
int     set_software_alarm_test(void);
int     scc_test_monitor_security_failure(void);
int     scc_test_stop_monitoring_security_failure(void);

/** utility print functions */
void    print_ram_data(uint32_t * ram, uint32_t address, int count);
int     print_smn_status_register(uint32_t);
void    print_scm_control_register(uint32_t);
void    print_scm_status_register(uint32_t);
void    print_scc_error_status_register(uint32_t);
char   *get_smn_state_name(const uint8_t);
void    print_smn_timer_control_register(const uint32_t);
void    print_smn_command_register(const uint32_t);
void    print_scc_debug_detector_register(uint32_t);
void    print_scc_return_code(const scc_return_t);
void    dump_cipher_control_block(scc_encrypt_decrypt *);

/** utility register access functions */
int     write_scc_register(uint32_t address, uint32_t value);
int     read_scc_register(uint32_t address, uint32_t * value);

/** miscellaneous utility functions */
scc_configuration_access *get_scc_configuration(int);
void    init_plaintext(void);

#endif          /* scc_test.h */

#ifdef __cplusplus
}
#endif
