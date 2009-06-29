/***
**Copyright 2005-2009 Freescale Semiconductor, Inc. All Rights Reserved.
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
        @file   sahara_module.h 

        @brief  Sahara module header file

====================================================================================================
Revision History:
                            Modification     Tracking
Author/Core ID                  Date          Number    Description of Changes
-------------------------   ------------    ----------  -------------------------------------------
A.Ozerov/NONE                01/12/2005     TLSbo58662  Initial version for linux-2.6.10-rel-L26_1_14
A.Ozerov/NONE                19/12/2005     TLSbo58662  Version for linux-2.6.10-rel-L26_1_15
D.Simakov/smkd001c           21/09/2006     TLSbo76069  fsl_shw* kernel api can not work with a static
                                                        data.                                                        
====================================================================================================
Portability:  ARM GCC
==================================================================================================*/
#ifndef SAHARA_MODULE_H
#define SAHARA_MODULE_H

#ifdef __cplusplus
extern "C"{
#endif

#define MODVERSIONS
#define LINUX_KERNEL

#include <fsl_shw.h>
/* #include <api_tests.h> */
#define usleep mdelay

#include <sahara.h>

/*==================================================================================================
                                        DEFINES AND MACROS
==================================================================================================*/
#define SET_REFERENCE 42

/* Does this belong here? */
#ifndef SAHARA_TEST_DEVICE
#define SAHARA_TEST_DEVICE "sahara_test"
#endif

#define CHECK_ERROR(a) \
{       error = a; \
        if( error != 0 ) \
        { \
        printk(KERN_WARNING "Error in sahara test: "#a); \
        } \
}        

/*================================ For sahara_callback_test ======================================*/
/* ! The maximum number of results to request at once */
#define RESULTS_SIZE 10

/* ! The number of bytes of random data to request.  Needs to be greater than five. */
#define RAND_SIZE 24

/*=================================== For sahara_hmac1_test ======================================*/
typedef enum
{
        DPD_SHA256_LDCTX_HMAC_ULCTX = 0x4A00,
        DPD_MD5_LDCTX_HMAC_ULCTX,
        DPD_SHA_LDCTX_HMAC_ULCTX,
        DPD_SHA256_LDCTX_HMAC_PAD_ULCTX,
        DPD_MD5_LDCTX_HMAC_PAD_ULCTX,
        DPD_SHA_LDCTX_HMAC_PAD_ULCTX,
        DPD_SHA224_LDCTX_HMAC_ULCTX,
        DPD_SHA224_LDCTX_HMAC_PAD_ULCTX,
        DPD_SHA256_LDCTX_HMAC_ULCTX_CMP,
        DPD_MD5_LDCTX_HMAC_ULCTX_CMP,
        DPD_SHA_LDCTX_HMAC_ULCTX_CMP,
        DPD_SHA256_LDCTX_HMAC_PAD_ULCTX_CMP,
        DPD_MD5_LDCTX_HMAC_PAD_ULCTX_CMP,
        DPD_SHA_LDCTX_HMAC_PAD_ULCTX_CMP,
        DPD_SHA224_LDCTX_HMAC_ULCTX_CMP,
        DPD_SHA224_LDCTX_HMAC_PAD_ULCTX_CMP
}
DPD_HASH_LDCTX_HMAC_ULCTX_GROUP;

/*=================================== For sahara_hash_test =======================================*/
#define PARTIAL_SIZE 128 

typedef enum
{
        DPD_SHA256_LDCTX_HASH_ULCTX = 0x4400,
        DPD_MD5_LDCTX_HASH_ULCTX,
        DPD_SHA_LDCTX_HASH_ULCTX,
        DPD_SHA256_LDCTX_IDGS_HASH_ULCTX,
        DPD_MD5_LDCTX_IDGS_HASH_ULCTX,
        DPD_SHA_LDCTX_IDGS_HASH_ULCTX,
        DPD_SHA256_CONT_HASH_ULCTX,
        DPD_MD5_CONT_HASH_ULCTX,
        DPD_SHA_CONT_HASH_ULCTX,
        DPD_SHA224_LDCTX_HASH_ULCTX,
        DPD_SHA224_LDCTX_IDGS_HASH_ULCTX,
        DPD_SHA224_CONT_HASH_ULCTX,
        DPD_SHA256_LDCTX_HASH_ULCTX_CMP,
        DPD_MD5_LDCTX_HASH_ULCTX_CMP,
        DPD_SHA_LDCTX_HASH_ULCTX_CMP,
        DPD_SHA256_LDCTX_IDGS_HASH_ULCTX_CMP,
        DPD_MD5_LDCTX_IDGS_HASH_ULCTX_CMP,
        DPD_SHA_LDCTX_IDGS_HASH_ULCTX_CMP,
        DPD_SHA224_LDCTX_HASH_ULCTX_CMP,
        DPD_SHA224_LDCTX_IDGS_HASH_ULCTX_CMP
}
DPD_HASH_LDCTX_HASH_ULCTX_GROUP;

typedef enum
{
        DPD_SHA256_LDCTX_HASH_PAD_ULCTX = 0x4500,
        DPD_MD5_LDCTX_HASH_PAD_ULCTX,
        DPD_SHA_LDCTX_HASH_PAD_ULCTX,
        DPD_SHA256_LDCTX_IDGS_HASH_PAD_ULCTX,
        DPD_MD5_LDCTX_IDGS_HASH_PAD_ULCTX,
        DPD_SHA_LDCTX_IDGS_HASH_PAD_ULCTX,
        DPD_SHA224_LDCTX_HASH_PAD_ULCTX,
        DPD_SHA224_LDCTX_IDGS_HASH_PAD_ULCTX,
        DPD_SHA256_LDCTX_HASH_PAD_ULCTX_CMP,
        DPD_MD5_LDCTX_HASH_PAD_ULCTX_CMP,
        DPD_SHA_LDCTX_HASH_PAD_ULCTX_CMP,
        DPD_SHA256_LDCTX_IDGS_HASH_PAD_ULCTX_CMP,
        DPD_MD5_LDCTX_IDGS_HASH_PAD_ULCTX_CMP,
        DPD_SHA_LDCTX_IDGS_HASH_PAD_ULCTX_CMP,
        DPD_SHA224_LDCTX_HASH_PAD_ULCTX_CMP,
        DPD_SHA224_LDCTX_IDGS_HASH_PAD_ULCTX_CMP
}
DPD_HASH_LDCTX_HASH_PAD_ULCTX_GROUP;
 
/*=================================== For sahara_results_test ====================================*/
/* ! The maximum number of results to request at once */
#define RESULTS_SIZE 10

/* ! The number of requests to make */
#define NUM_REQUESTS 20

/* ! The number of bytes of random data to request.  Needs to be greater than five. */
#define RAND_SIZE 24

/*======================================= For anything ===========================================*/
/* VT_run_hmac2 stuffs */
#define NUMBER_OF_TESTS sizeof(test)/sizeof(TEST)
#define RESULT_SIZE 36

/* VT_run_random defines */
#define INIT_VAL 0x42

/* VT_compare_result defines */
#define MAX_DUMP 16

/* VT_sahara_test_setup defines */
#define KEY_OWNER_ID 0x42

#define SECRET_KEY_SIZE 16
#define KNOWN_KEY_SIZE 24

#define UNIQUE_KEY_ALG FSL_KEY_ALG_AES
#define UNIQUE_KEY_MODE FSL_SYM_MODE_ECB

/*==================================================================================================
                                            ENUMS
==================================================================================================*/
/*=================================== For sahara_sym_test ========================================*/ 
typedef enum
{
        DPD_AES_CTR_ENC,
        DPD_AES_CBC_ENC,
        DPD_AES_ECB_ENC,
        DPD_AES_CTR_DEC,
        DPD_AES_CBC_DEC,
        DPD_AES_ECB_DEC,
        DPD_DES_CBC_ENC,
        DPD_DES_ECB_ENC,
        DPD_DES_CBC_DEC,
        DPD_DES_ECB_DEC,
        DPD_DES_KEY_PARITY,
        DPD_3DES_CBC_ENC,
        DPD_3DES_ECB_ENC,
        DPD_3DES_CBC_DEC,
        DPD_3DES_ECB_DEC,
        DPD_3DES_KEY_PARITY,
        DPD_ARC4_ENC_KEY,
        DPD_ARC4_ENC_SBOX
} test_type;

/*============================================ IOCTL =============================================*/
typedef enum
{
        SAHARA_TEST_RUN_CALLBACK,
        SAHARA_TEST_RUN_HASH,
        SAHARA_TEST_RUN_HMAC1,
        SAHARA_TEST_RUN_HMAC2,
        SAHARA_TEST_RUN_RESULT,
        SAHARA_TEST_RUN_SYMMETRIC,
        SAHARA_TEST_RUN_WRAP,
        SAHARA_TEST_RUN_RANDOM,
        SAHARA_TEST_SHOW_CAPABILITIES,
        SAHARA_TEST_RUN_ENCRYPT_DECRYPT,
        SAHARA_TEST_REGISTER_USER,
        SAHARA_TEST_DEREGISTER_USER
} SAHARA_TEST_IOCTL;

#ifdef __cplusplus
}
#endif

#endif /* SAHARA_MODULE_H */
