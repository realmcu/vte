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
        @file:   scc_test_module.h

        @brief:  scc module API test header file
====================================================================================================
Revision History:
                            Modification     Tracking
Author/core ID                  Date          Number    Description of Changes
-------------------------   ------------    ----------  -------------------------------------------
A.Urusov/NONE                29/09/2005     TLSbo55835  Initial version

==================================================================================================*/
#ifdef __cplusplus
extern "C"{
#endif

#ifndef SCC_TEST_MODULE_H
#define SCC_TEST_MODULE_H

/*#ifdef SCC_MODULE
#include "skdrv1st.h"
#endif
*/
#include <linux/mxc_scc_driver.h>

/*==================================================================================================
                                       DEFINES AND MACROS
==================================================================================================*/
/* Test interface definitions */
#define SCC_TEST_MAJOR_NODE 0
#define SCC_TEST_DEVICE_NAME "scc_test_module"

/* User/Driver interface definitions */

/* 
 * ioctlreturns Ioctl Error Codes.
 *
 * These are the values returned by #scc_ioctl and placed into @c
 * errno by @c ioctl. Porting opportunity.  These values were chosen to
 * match standard Linux values.
 */
#define IOCTL_SCC_OK             0      /**< ioctl completed successfully */
#define IOCTL_SCC_INVALID_CMD    ENOTTY /**< Invalid command passed */
#define IOCTL_SCC_IMPROPER_ADDR  EFAULT /**< Improper address/offset passed */
#define IOCTL_SCC_NO_MEMORY      ENOMEM /**< Insufficient memory to process */
#define IOCTL_SCC_FAILURE        ESPIPE /**< Generic 'SCC error' error */

/* Interface definitions between user and driver */

/* This is a porting opportunity.  It identifies the 'unique' byte value inserted into the IOCTL
 * number.  It really only has to be unique within the software used on a given device. */
#ifndef SCC_TEST_DRIVER_IOCTL_IDENTIFIER
#define SCC_TEST_DRIVER_IOCTL_IDENTIFIER 's'
#endif

/* Define SCC Driver Commands (Argument 2 of ioctl) */

/** ioctl cmd to return version and configuration information on driver and
 *  SCC */
#define SCC_TEST_GET_CONFIGURATION _IOW (SCC_TEST_DRIVER_IOCTL_IDENTIFIER, 0, \
         scc_configuration_access)
/** ioctl cmd to test the scc_read_register() function of the SCC driver */
#define SCC_TEST_READ_REG          _IOWR(SCC_TEST_DRIVER_IOCTL_IDENTIFIER, 1, \
         scc_reg_access)
/** ioctl cmd to test the scc_write_register() function of the SCC driver */
#define SCC_TEST_WRITE_REG         _IOWR(SCC_TEST_DRIVER_IOCTL_IDENTIFIER, 2, \
         scc_reg_access)
/** ioctl cmd to test the scc_crypt() function of the SCC driver */
#define SCC_TEST_ENCRYPT           _IOWR(SCC_TEST_DRIVER_IOCTL_IDENTIFIER, 3, \
         scc_encrypt_decrypt)
/** ioctl cmd to test the scc_crypt() function of the SCC driver */
#define SCC_TEST_DECRYPT           _IOWR(SCC_TEST_DRIVER_IOCTL_IDENTIFIER, 4, \
         scc_encrypt_decrypt)
/** ioctl cmd to test the scc_set_sw_alarm() function of the SCC driver */
#define SCC_TEST_SET_ALARM         _IO  (SCC_TEST_DRIVER_IOCTL_IDENTIFIER, 5)
/** ioctl cmd to test the scc_zeroize_memories() function of the SCC driver */
#define SCC_TEST_ZEROIZE           _IOWR(SCC_TEST_DRIVER_IOCTL_IDENTIFIER, 6, \
         scc_return_t)
/** ioctl cmd to test the scc_monitor_security_failure() function of the SCC driver */
#define SCC_TEST_SET_MON_SEC_FAIL  _IOW (SCC_TEST_DRIVER_IOCTL_IDENTIFIER, 7, \
         scc_return_t)
/** ioctl cmd to test the scc_stop_monitoring_security_failure() function of the SCC driver */
#define SCC_TEST_STOP_MON_SEC_FAIL _IO  (SCC_TEST_DRIVER_IOCTL_IDENTIFIER, 8)

/*==================================================================================================
                                 STRUCTURES AND OTHER TYPEDEFS
==================================================================================================*/
/* Special structs for argument 3 of ioctl */

/**
 * ioctl structure for retrieving driver & SCC version ids, used
 * with #SCC_TEST_GET_CONFIGURATION.
 */
typedef struct
{       /**< Major version of the SCC driver code  */
        int     driver_major_version;
        /**< Minor version of the SCC driver code  */
        int     driver_minor_version;
        int     scm_version;    /**< from Configuration register */
        int     smn_version;    /**< from SMN Status register */
        int     block_size;     /**< bytes in a block */
        int     black_ram_size; /**< number of blocks of Black RAM */
        int     red_ram_size;   /**< number of blocks of Red RAM */
} scc_configuration_access;

/**
 * ioctl structure for accessing SCC registers, used with
 * #SCC_TEST_READ_REG and #SCC_TEST_WRITE_REG.
 */
typedef struct
{
        uint32_t reg_offset;    /**< The register address from Memory Map */
        uint32_t reg_data;      /**< Data to/from the register */
        scc_return_t function_return_code;
                                       /**< Straight from SCC driver */
} scc_reg_access;

/**
 * ioctl structure for SCC encryption and decryption, used with
 * #SCC_TEST_ENCRYPT and #SCC_TEST_DECRYPT.
 */
typedef struct
{
        uint8_t *data_in;                /**< Starting text for cipher */
        unsigned long data_in_length;    /**< Number of bytes in data_in */
        uint8_t *data_out;               /**< Resulting text of cipher */
        unsigned long data_out_length;   /**< Number of bytes in data_out  */
        /** inform driver which type of cipher mode to use */
        scc_crypto_mode_t crypto_mode;
        scc_verify_t check_mode;         /**< none/padded or CRC/pad */
        uint32_t init_vector[2];      /**< Initial value of cipher function */
        /** Inform driver whether to poll or sleep while waiting for
        cipher completion */
        enum scc_encrypt_wait
        {
                SCC_ENCRYPT_SLEEP,
                                /**< Put process to sleep during operation */
                SCC_ENCRYPT_POLL/**< Make CPU monitor status for completion */
        } wait_mode;
        scc_return_t function_return_code;  /**< Straight from SCC driver */
        scc_verify_t verify_mode;    /**< Whether to add padding & CRC  */
} scc_encrypt_decrypt;

#endif                          /* scc_test_module.h */

#ifdef __cplusplus
}
#endif
