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
        @file   sahara_module.c

        @brief  Sahara module implementation.
====================================================================================================
Revision History:
                            Modification     Tracking
Author/Core ID                  Date          Number    Description of Changes
-------------------------   ------------    ----------  -------------------------------------------
A.Ozerov/NONE                01/12/2005     TLSbo58662  Initial version for linux-2.6.10-rel-L26_1_14
A.Ozerov/NONE                19/12/2005     TLSbo58662  Version for linux-2.6.10-rel-L26_1_15
A.Ozerov/NONE                31/01/2006     TLSbo61952  Problem with fsl_shw_register_user was fixed
D.Simakov/smkd001c           21/09/2006     TLSbo76069  fsl_shw* kernel api can not work with a static
                                                        data.
====================================================================================================
Portability:  ARM GCC
==================================================================================================*/

/*==================================================================================================
Total Tests: 1

Test Executable Name:  sahara_module.ko

=================================================================================================*/

/*==================================================================================================
                                        INCLUDE FILES
==================================================================================================*/
/* Standard Include Files */
//#include <linux/config.h>
#include <linux/module.h>
#include <linux/device.h>      /* Added on 05/03/06 by RAKESH S JOSHI */
#include <linux/kernel.h>
#include <linux/version.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <asm/uaccess.h>

/* Verification Test Environment Include Files */
#include "sahara_module.h"

/*==================================================================================================
                                        DEFINES AND MACROS
==================================================================================================*/
#ifdef DEBUG
#define TRACEMSG(fmt,args...)  printk(fmt,##args)
#else
#define TRACEMSG(fmt,args...)
#endif

/*==================================================================================================
                                       GLOBAL VARIABLES
==================================================================================================*/

static struct class *sahara_class;    /* added on 05/03/06 RAKESH S JOSHI */

/*==================================================================================================
                                            STRUCTURES
==================================================================================================*/
/*================================ For sahara_callback_test ======================================*/
struct rand_test
{
        unsigned initiated;
        unsigned result_received;
        uint8_t random[RAND_SIZE];
};

/*=================================== For sahara_hash_test =======================================*/
typedef struct
{
        unsigned long opId;
        unsigned long plainTextLen;
        unsigned char *pPlaintext;
        unsigned long digestLen;
        unsigned char *pDigest;
        char    testDesc[30];
} HASHTESTTYPE;

/*=================================== For sahara_hmac1_test ======================================*/
typedef struct
{
        unsigned long opId;
        uint32_t flags;                 /* hmac flags */
        unsigned long key_length;
        unsigned char *key;
        unsigned long message_length;
        unsigned char *message;
        unsigned long digest_length;    /* digest length, or 0 */
        unsigned char *digest;
        unsigned long context_length;   /* opad/ipad lengths, or 0 */
        char *opad;
        char *ipad;
        char    testDesc[34];
} HMACTESTTYPE;

/*=================================== For sahara_sym_test ========================================*/
typedef struct
{
        unsigned long opId;
        unsigned long KeyLen;
        unsigned char *pKey;
        unsigned char *pIV_CTR;          /* IV for CBC, CTR value for CTR, S-Box+ptrs for ARC4 */
        unsigned long  ivLen;
        fsl_shw_ctr_mod_t modulus;       /* for CTR mode only */
        unsigned long TextLen;
        unsigned char *pInputText;
        fsl_shw_return_t result;         /* expected return code */
        unsigned char *pOutputText;      /* expected value */
        unsigned long outputTextLen;
        unsigned char *pOutputIV_CTR;    /* IV for CBC, CTR value for CTR, S-Box+ for ARC4 */
        unsigned long outputIVLen;
        char    testDesc[50];
} SYMTESTTYPE;

/*======================================= For anything ===========================================*/
/* VT_show_capabilities stuffs */
struct interpret
{
        unsigned symbol;
        char   *name;
};

typedef struct
{
        uint8_t id;                      /* which string to use */
        fsl_shw_hash_alg_t algorithm;    /* which algorithm to use */
        uint8_t sequence[7];             /* sequence of block sizes */
        uint8_t *result;                 /* compare test results with this */
        uint8_t digest_length;           /* length of digest */
} TEST;

struct interpret keyalgs[] =
{
        {FSL_KEY_ALG_AES, "AES"},
        {FSL_KEY_ALG_DES, "DES"},
        {FSL_KEY_ALG_TDES, "3DES"},
        {FSL_KEY_ALG_ARC4, "ARC4"}
};

struct interpret symmodes[] =
{
        {FSL_SYM_MODE_STREAM, "Stream"},
        {FSL_SYM_MODE_ECB, "ECB"},
        {FSL_SYM_MODE_CBC, "CBC"},
        {FSL_SYM_MODE_CTR, "CTR"}
};

struct interpret hashalgs[] =
{
        {FSL_HASH_ALG_MD5, "MD5"},
        {FSL_HASH_ALG_SHA1, "SHA1"},
        {FSL_HASH_ALG_SHA224, "SHA224"},
        {FSL_HASH_ALG_SHA256, "SHA256"}
};        /* End of VT_show_capabilities stuffs */


/*==================================================================================================
                                            ARRAYS
==================================================================================================*/
/* this result came from the vl test. It is the "golden" multi-step case */
uint8_t actual_vl[] =
{
        0x6D, 0x6D, 0x99, 0x63, 0x9E, 0x1E, 0xF3, 0x32,
        0xDD, 0xA6, 0x7F, 0x49, 0xCD, 0x60, 0xA7, 0xDC
};

/* this result came from running a single shot on this data */
uint8_t actual_lc[] =
{
        0x0E, 0x28, 0x15, 0xCA, 0xEB, 0x3D, 0x93, 0x59,
        0x10, 0x98, 0x05, 0x62, 0x43, 0x66, 0x96, 0x2F,
        0x8E, 0x4B, 0xCA, 0x41
};

/* this result came from running a single shot on this data */
uint8_t actual_sw[] =
{
        0xF9, 0x0B, 0x50, 0x64, 0x45, 0x00, 0xE9, 0x3E,
        0x91, 0x54, 0xF3, 0x50, 0x35, 0xB0, 0x19, 0x4A,
        0x02, 0x38, 0x6A, 0x46, 0x8D, 0xB2, 0x9C, 0x80,
        0x54, 0x9F, 0xF9, 0xD3, 0x19, 0xDD, 0xEC, 0x5B
};

uint32_t my_key[] =
{
        0x0b0b0b0b, 0x0b0b0b0b, 0x0b0b0b0b, 0x0b0b0b0b
};

uint8_t my_msg[3][530] =
{
        /* from mdha_md5_hmac_chunk_data.vl with endian reversal and converting hex to character *
        * (e.g., 0x34 to '4') */
        "0123456711234567212345673123456741234567512345676123456771234567"
            "0123456711234567212345673123456741234567512345676123456771234567"
            "0123456711234567212345673123456741234567512345676123456771234567"
            "0123456711234567212345673123456741234567512345676123456771234567",

        /* Lewis Carrol, Jabberwocky, Through the Looking-Glass and What Alice Found There, 1872 */
        "'Twas brillig, and the slithy toves\nDid gyre and gimble in the w"
            "abe:\nAll mimsy were the borogoves,\nAnd the mome raths outgrabe.\n"
            "\n\"Beware the Jabberwock, my son!\nThe jaws that bite, the claws t"
            "hat catch!\nBeware the Jubjub bird, and shun\nThe frumious Banders" "natch!\"",

        /* Star Wars, New Hope, Episode IV, 1977 */
        "Episode IV\nIt is a period of civil war.\nRebel spaceships, striki"
            "ng\nfrom a hidden base, have won\ntheir first victory against\nthe "
            "evil Galactic Empire.\nDuring the battle, Rebel\nspies managed to "
            "steal secret\nplans to the Empire's\nultimate weapon, the DEATH\nST"
            "AR, an armored space\nstation with enough power to\ndestroy an ent"
            "ire planet.\nPursued by the Empire's\nsinister agents, Princess\nLe"
            "ia races home aboard her\nstarship, custodian of the\nstolen plans"
            " that can save her\npeople and restore\nfreedom to the galaxy...."
};

TEST    test[] =
{
        {0, FSL_HASH_ALG_MD5, {1, 2, 0}, actual_vl, 16},
        {0, FSL_HASH_ALG_MD5, {2, 1, 0}, actual_vl, 16},
        {0, FSL_HASH_ALG_MD5, {3, 0}, actual_vl, 16},
        {1, FSL_HASH_ALG_SHA1, {1, 2, 0}, actual_lc, 20},
        {2, FSL_HASH_ALG_SHA256, {3, 2, 1, 0}, actual_sw, 32}
};      /* End of VT_run_hmac2 stuffs */

/*======================================= For anything ===========================================*/
/* VT_run_wrap stuffs */
static const unsigned char cleartext1[] =
{
        0x00, 0x04, 0x01, 0x08, 0x02, 0x0c, 0x03, 0x10,
        0x04, 0x14, 0x05, 0x18, 0x06, 0x1c, 0x07, 0x20,
        0x08, 0x24, 0x09, 0x28, 0x0a, 0x2c, 0x0b, 0x30,
        0x0c, 0x34, 0x0d, 0x38, 0x0e, 0x3c, 0x0f, 0x40,
        0x10, 0x44, 0x11, 0x48, 0x12, 0x4c, 0x13, 0x50,
        0x14, 0x54, 0x15, 0x58, 0x16, 0x5c, 0x17, 0x60,
        0x18, 0x64, 0x19, 0x68, 0x1a, 0x6c, 0x1b, 0x70,
        0x1c, 0x74, 0x1d, 0x78, 0x1e, 0x7c, 0x1f, 0x80
};

/* ! Key for Known Answer test */
static const unsigned char known_key[] =
{
        0x2C, 0x82, 0x96, 0xE0, 0x2E, 0x5F, 0x5C, 0x19,
        0xAA, 0x29, 0xA6, 0xCF, 0x97, 0x05, 0x5C, 0xD2,
        0xA8, 0xEC, 0xE4, 0x1D, 0xAC, 0x47, 0x7B, 0x6F
};

/* ! Plaintext for Known Answer test */
static unsigned char known_plaintext[] =
{
        0x0F, 0xEB, 0x9B, 0x5C, 0x22, 0x0B, 0xA5, 0x13,
        0x5D, 0x0F, 0x55, 0x06, 0xC7, 0xD6, 0x75, 0xAF,
        0x76, 0x20, 0x1A, 0x91, 0x78, 0x31, 0x75, 0x94,
        0x67, 0xB7, 0x3D, 0x23, 0x90, 0xEC, 0x4E, 0x4F,
        0x84, 0x55, 0xB0, 0xED, 0x4B, 0x81, 0x70, 0x85,
        0x1D, 0xB1, 0xD5, 0x48, 0x85, 0x7A, 0x13, 0x40,
        0x76, 0x74, 0x7C, 0x92, 0x97, 0x75, 0xB3, 0x14,
        0xA7, 0xE5, 0x02, 0x4F, 0xB4, 0x2F, 0x1E, 0x03
};

/* ! Result of encrypting cleartext in ECB mode. */
static const unsigned char known_ciphertext[] =
{
        0x79, 0xA2, 0xCB, 0x79, 0x43, 0x66, 0x07, 0x61,
        0xB6, 0x53, 0xEF, 0x8B, 0xD9, 0x6C, 0xD6, 0x45,
        0xF4, 0xEF, 0x29, 0x4F, 0x37, 0x07, 0x5F, 0x2B,
        0xBE, 0xE9, 0x5E, 0x6A, 0x27, 0x29, 0x02, 0x45,
        0x22, 0x7F, 0x80, 0x16, 0xFF, 0x21, 0x83, 0xB4,
        0x19, 0x80, 0xED, 0x66, 0x37, 0x28, 0x8E, 0xA0,
        0xEA, 0x7F, 0x3A, 0x96, 0x37, 0xE2, 0x07, 0x24,
        0x87, 0x42, 0x35, 0xCB, 0x11, 0xDE, 0xEB, 0xC3
};

/*================================ For sahara_callback_test ======================================*/
/* SHA1 test vectors from FIPS PUB 180-1 */
static const unsigned char sha1padplaintext1[] = "abc";

static const unsigned char sha1paddigest1_1[] =
{
        0xa9, 0x99, 0x3e, 0x36, 0x47,
        0x06, 0x81, 0x6a, 0xba, 0x3e,
        0x25, 0x71, 0x78, 0x50, 0xc2,
        0x6c, 0x9c, 0xd0, 0xd8, 0x9d
};

static const unsigned char sha1padplaintext2[] =
    "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq";

static const unsigned char sha1paddigest2[] =
{
        0x84, 0x98, 0x3e, 0x44, 0x1c,
        0x3b, 0xd2, 0x6e, 0xba, 0xae,
        0x4a, 0xa1, 0xf9, 0x51, 0x29,
        0xe5, 0xe5, 0x46, 0x70, 0xf1
};

/* SHA1 test vector after Sahara VL tests */
static const unsigned char sha1padplaintext3[] =
    "0123456711234567212345673123456741234567512345676123456771234567"
    "0123456711234567212345673123456741234567512345676123456771234567"
    "0123456711234567212345673123456741234567512345676123456771234567"
    "0123456711234567212345673123456741234567512345676123456771234567"
    "0123456711234567212345673123456741234567512345676123456771234567"
    "0123456711234567212345673123456741234567512345676123456771234567"
    "0123456711234567212345673123456741234567512345676123456771234567"
    "0123456711234567212345673123456741234567512345676123456771234567"
    "0123456711234567212345673123456741234567512345676123456771234567"
    "0123456711234567212345673123456741234567512345676123456771234567"
    "0123456711234567212345673123456741234567512345676123456771234567"
    "0123456711234567212345673123456741234567512345676123456771234567"
    "0123456711234567212345673123456741234567512345676123456771234567"
    "0123456711234567212345673123456741234567512345676123456771234567"
    "0123456711234567212345673123456741234567512345676123456771234567"
    "0123456711234567212345673123456741234567512345676123456771234567";

static const unsigned char sha1paddigest3[] =
{
        0xcb, 0xbc, 0xe2, 0x86, 0xc8, 0x70, 0x15, 0x60,
        0x78, 0x9f, 0x43, 0xf7, 0xb9, 0x88, 0xc9, 0xec,
        0xbb, 0x8d, 0x8d, 0xe4
};

/* SHA256 test vectors from from NIST */
static const unsigned char sha256padplaintext1_1[] = "abc";

static const unsigned char sha256paddigest1_1[] =
{
        0xba, 0x78, 0x16, 0xbf, 0x8f, 0x01, 0xcf, 0xea,
        0x41, 0x41, 0x40, 0xde, 0x5d, 0xae, 0x22, 0x23,
        0xb0, 0x03, 0x61, 0xa3, 0x96, 0x17, 0x7a, 0x9c,
        0xb4, 0x10, 0xff, 0x61, 0xf2, 0x00, 0x15, 0xad
};
static const unsigned char sha256padplaintext2[] =
    "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq";

static const unsigned char sha256paddigest2[] =
{
        0x24, 0x8d, 0x6a, 0x61, 0xd2, 0x06, 0x38, 0xb8,
        0xe5, 0xc0, 0x26, 0x93, 0x0c, 0x3e, 0x60, 0x39,
        0xa3, 0x3c, 0xe4, 0x59, 0x64, 0xff, 0x21, 0x67,
        0xf6, 0xec, 0xed, 0xd4, 0x19, 0xdb, 0x06, 0xc1
};

/* MD5 test vectors from RFC1321 */
static const unsigned char md5padplaintext1_1[] = "a";
static const unsigned char md5padplaintext1_2[] =       /* 64 */
{
        'w', 'h', 'a', 't', ' ', 'd', 'o', ' ',
        'y', 'a', ' ', 'w', 'a', 'n', 't', ' ',
        'f', 'o', 'r', ' ', 'n', 'o', 't', 'h',
        'i', 'n', 'g', '?', 0x80, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0xe0, 0x02, 0, 0, 0, 0, 0, 0
};

static const unsigned char md5paddigest1_1[] =
{
        0x0c, 0xc1, 0x75, 0xb9, 0xc0, 0xf1, 0xb6, 0xa8,
        0x31, 0xc3, 0x99, 0xe2, 0x69, 0x77, 0x26, 0x61
};

static const unsigned char md5padplaintext2[] = "abc";

static const unsigned char md5paddigest2[] =
{
        0x90, 0x01, 0x50, 0x98, 0x3c, 0xd2, 0x4f, 0xb0,
        0xd6, 0x96, 0x3f, 0x7d, 0x28, 0xe1, 0x7f, 0x72
};

static const unsigned char md5padplaintext3[] = "message digest";

static const unsigned char md5paddigest3[] =
{
        0xf9, 0x6b, 0x69, 0x7d, 0x7c, 0xb7, 0x93, 0x8d,
        0x52, 0x5a, 0x2f, 0x31, 0xaa, 0xf1, 0x61, 0xd0
};

static const unsigned char md5padplaintext4[] = "abcdefghijklmnopqrstuvwxyz";

static const unsigned char md5paddigest4[] =
{
        0xc3, 0xfc, 0xd3, 0xd7, 0x61, 0x92, 0xe4, 0x00,
        0x7d, 0xfb, 0x49, 0x6c, 0xca, 0x67, 0xe1, 0x3b
};

static const unsigned char md5padplainttext5[] =
    "0123456711234567212345673123456741234567512345676123456771234567"
    "0123456711234567212345673123456741234567512345676123456771234567"
    "0123456711234567212345673123456741234567512345676123456771234567"
    "0123456711234567212345673123456741234567512345676123456771234567"
    "0123456711234567212345673123456741234567512345676123456771234567"
    "0123456711234567212345673123456741234567512345676123456771234567"
    "0123456711234567212345673123456741234567512345676123456771234567"
    "0123456711234567212345673123456741234567512345676123456771234567"
    "0123456711234567212345673123456741234567512345676123456771234567"
    "0123456711234567212345673123456741234567512345676123456771234567"
    "0123456711234567212345673123456741234567512345676123456771234567"
    "0123456711234567212345673123456741234567512345676123456771234567"
    "0123456711234567212345673123456741234567512345676123456771234567"
    "0123456711234567212345673123456741234567512345676123456771234567"
    "0123456711234567212345673123456741234567512345676123456771234567"
    "0123456711234567212345673123456741234567512345676123456771234567";

static const unsigned char md5paddigest5[] =
{
        0xc2, 0xaa, 0xfe, 0xd0, 0x82, 0x58, 0xac, 0x2c,
        0x0e, 0xd0, 0x1e, 0x2f, 0xde, 0xde, 0x95, 0x9c
};

static const unsigned char md5nopadplaintext[] =
{
        'a', 0x80, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        8, 0, 0, 0, 0, 0, 0, 0
};

static const unsigned char md5nopaddigest[] =
{
        0x0c, 0xc1, 0x75, 0xb9, 0xc0, 0xf1, 0xb6, 0xa8,
        0x31, 0xc3, 0x99, 0xe2, 0x69, 0x77, 0x26, 0x61
};

static const unsigned char sha1nopadplaintext[] =
{
        'a', 'b', 'c', 0x80, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 24
};

static const unsigned char sha1nopaddigest[] =
{
        0xa9, 0x99, 0x3e, 0x36, 0x47,
        0x06, 0x81, 0x6a, 0xba, 0x3e,
        0x25, 0x71, 0x78, 0x50, 0xc2,
        0x6c, 0x9c, 0xd0, 0xd8, 0x9d
};

static const unsigned char sha256nopadplaintext[] =
{
        'a', 'b', 'c', 0x80, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 24
};

static const unsigned char sha256nopaddigest[] =
{
        0xba, 0x78, 0x16, 0xbf, 0x8f, 0x01, 0xcf, 0xea,
        0x41, 0x41, 0x40, 0xde, 0x5d, 0xae, 0x22, 0x23,
        0xb0, 0x03, 0x61, 0xa3, 0x96, 0x17, 0x7a, 0x9c,
        0xb4, 0x10, 0xff, 0x61, 0xf2, 0x00, 0x15, 0xad
};

static const unsigned char sha1paddigest1_2[] = /* 20 */
{
        0xef, 0xfc, 0xdf, 0x6a, 0xe5, 0xeb, 0x2f, 0xa2, 0xd2, 0x74,
        0x16, 0xd5, 0xf1, 0x84, 0xdf, 0x9c, 0x25, 0x9a, 0x7c, 0x79
};

static HASHTESTTYPE hashTest[ /* NUM_HASHTESTS */ ] =
{
        /* SHA-1 PAD Test 1 */
        {
                DPD_SHA_LDCTX_IDGS_HASH_PAD_ULCTX,
                3,
                (unsigned char *) sha1padplaintext1,
                20,
                (unsigned char *) sha1paddigest1_1,
                "SHA-1 HASH PAD TEST 1"
        },

        /* SHA-1 PAD Test 2 */
        {
                DPD_SHA_LDCTX_IDGS_HASH_PAD_ULCTX,
                56,
                (unsigned char *) sha1padplaintext2,
                20,
                (unsigned char *) sha1paddigest2,
                "SHA-1 HASH PAD TEST 2"
        },

        /* SHA HASH PAD TEST 3 */
        {
                DPD_SHA_LDCTX_IDGS_HASH_PAD_ULCTX,
                1024,
                (unsigned char *) sha1padplaintext3,
                20,
                (unsigned char *) sha1paddigest3,
                "SHA-1 HASH PAD TEST 3"
        },

        /* SHA-256 PAD Test 1 */
        {
                DPD_SHA256_LDCTX_IDGS_HASH_PAD_ULCTX,
                3,
                (unsigned char *) sha256padplaintext1_1,
                32,
                (unsigned char *) sha256paddigest1_1,
                "SHA-256 HASH PAD TEST 1"
        },

        /* SHA-256 PAD Test 2 */
        {
                DPD_SHA256_LDCTX_IDGS_HASH_PAD_ULCTX,
                56,
                (unsigned char *) sha256padplaintext2,
                32,
                (unsigned char *) sha256paddigest2,
                "SHA-256 HASH PAD TEST 2"
        },

        /* MD5 PAD Test 1 */
        {
                DPD_MD5_LDCTX_IDGS_HASH_PAD_ULCTX,
                1,
                (unsigned char *) md5padplaintext1_1,
                16,
                (unsigned char *) md5paddigest1_1,
                "MD5 HASH PAD TEST 1"
        },

        /* MD5 PAD Test 2 */
        {
                DPD_MD5_LDCTX_IDGS_HASH_PAD_ULCTX,
                3,
                (unsigned char *) md5padplaintext2,
                16,
                (unsigned char *) md5paddigest2,
                "MD5 HASH PAD TEST 2"
        },

        /* MD5 PAD Test 3 */
        {
                DPD_MD5_LDCTX_IDGS_HASH_PAD_ULCTX,
                14,
                (unsigned char *) md5padplaintext3,
                16,
                (unsigned char *) md5paddigest3,
                "MD5 HASH PAD TEST 3"
        },

        /* MD5 PAD Test 4 */
        {
                DPD_MD5_LDCTX_IDGS_HASH_PAD_ULCTX,
                26,
                (unsigned char *) md5padplaintext4,
                16,
                (unsigned char *) md5paddigest4,
                "MD5 HASH PAD TEST 4"
        },

        {
                DPD_MD5_LDCTX_IDGS_HASH_PAD_ULCTX,
                1024,
                (unsigned char *) md5padplainttext5,
                16,
                (unsigned char *) md5paddigest5,
                "MD5 HASH PAD TEST 5"
        },

        /* MD5 No PAD Test */
        {
                DPD_MD5_LDCTX_IDGS_HASH_ULCTX,
                64,
                (unsigned char *) md5nopadplaintext,
                16,
                (unsigned char *) md5nopaddigest,
                "MD5 HASH No PAD TEST"
        },

        /* SHA-1 No PAD Test */
        {
                DPD_SHA_LDCTX_IDGS_HASH_ULCTX,
                64,
                (unsigned char *) sha1nopadplaintext,
                20,
                (unsigned char *) sha1nopaddigest,
                "SHA-1 HASH No PAD TEST"
        },

        /* SHA-256 No PAD Test */
        {
                DPD_SHA256_LDCTX_IDGS_HASH_ULCTX,
                64,
                (unsigned char *) sha256nopadplaintext,
                32,
                (unsigned char *) sha256nopaddigest,
                "SHA-256 HASH No PAD TEST"
        },
};

/*=================================== For sahara_hmac1_test ======================================*/
static const unsigned char sha1testkey1[] =
{
        0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B,
        0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B,
        0x0B, 0x0B, 0x0B, 0x0B
};

static const unsigned char sha1testmessage1[] =
{
        0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
        0x31, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
        0x32, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
        0x33, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
        0x34, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
        0x35, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
        0x36, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
        0x37, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37
};

static const unsigned char sha1testdigest1[] =
{
        0xFB, 0x6D, 0xCB, 0xAF, 0x78, 0x92, 0x6E, 0x99,
        0xFB, 0x13, 0xB0, 0x14, 0x28, 0x6E, 0xA3, 0xFA,
        0xFA, 0x65, 0x9B, 0xE1
};

static const unsigned char sha1testipad1[] =
{
        0x06, 0x4C, 0x66, 0x2A, 0xCC, 0x6E, 0xD1, 0xCC,
        0x6C, 0xFA, 0x9A, 0xB0, 0x04, 0x2F, 0x13, 0x7F,
        0xCE, 0xA5, 0x70, 0xEB, 0x40, 0x00, 0x00, 0x00
};

static const unsigned char sha1testopad1[] =
{
        0xD6, 0x2A, 0xCC, 0xCC, 0x7E, 0x6A, 0x98, 0xB3,
        0xDF, 0x01, 0x5B, 0x02, 0xD8, 0x85, 0xC3, 0xE0,
        0x9B, 0xD1, 0x84, 0xCB, 0x40, 0x00, 0x00, 0x00
};

static const unsigned char md5testkey1[] =
{
        0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B,
        0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B,
        0x0B, 0x0B, 0x0B, 0x0B
};

static const unsigned char md5testmessage1[] =
{
        0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
        0x31, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
        0x32, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
        0x33, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
        0x34, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
        0x35, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
        0x36, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
        0x37, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37
};

static const unsigned char md5testdigest1[] =
{
        0x79, 0xD3, 0x12, 0x94, 0x76, 0xC6, 0x67, 0xB5,
        0xD2, 0x4B, 0x05, 0x32, 0x0F, 0x85, 0x24, 0xE0
};

static const unsigned char md5testipad1[] =
{
        0x2A, 0x2F, 0x00, 0x9E, 0xFB, 0x9C, 0x79, 0xD2,
        0x1B, 0xE7, 0xDB, 0xE8, 0x64, 0xEA, 0xC8, 0xA9,
        0x00, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00
};

static const unsigned char md5testopad1[] =
{
        0xC2, 0xC2, 0xA3, 0x9B, 0xD8, 0x1C, 0x6B, 0xBD,
        0x74, 0x33, 0x4B, 0x4D, 0xBD, 0xBD, 0x21, 0xF0,
        0x00, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x00
};

static const unsigned char sha256testkey1[] =
{
        0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B,
        0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B, 0x0B,
        0x0B, 0x0B, 0x0B, 0x0B
};

static const unsigned char sha256testmessage1[] =
{
        0x48, 0x69, 0x20, 0x54, 0x68, 0x65, 0x72, 0x65
};

static const unsigned char sha256testdigest1[] =
{
        0xB0, 0x34, 0x4C, 0x61, 0xD8, 0xDB, 0x38, 0x53,
        0x5C, 0xA8, 0xAF, 0xCE, 0xAF, 0x0B, 0xF1, 0x2B,
        0x88, 0x1D, 0xC2, 0x00, 0xC9, 0x83, 0x3D, 0xA7,
        0x26, 0xE9, 0x37, 0x6C, 0x2E, 0x32, 0xCF, 0xF7
};

static const unsigned char sha256testipad1[] =
{
        0x2B, 0xB2, 0x18, 0x04, 0x23, 0xB9, 0x5B, 0xF9,
        0xB4, 0xE8, 0x25, 0x8C, 0xFA, 0xB5, 0xE6, 0x54,
        0x11, 0xF2, 0x92, 0x1E, 0x4F, 0xEB, 0x78, 0xEE,
        0x98, 0x90, 0xE5, 0xFE, 0x64, 0xB7, 0x80, 0x36,
        0x40, 0x00, 0x00, 0x00
};

static const unsigned char sha256testopad1[] =
{
        0x27, 0xE7, 0x73, 0x9F, 0xD9, 0x56, 0x25, 0x83,
        0x56, 0xD6, 0x66, 0xE2, 0x5F, 0x81, 0x0D, 0xE8,
        0xEC, 0x5E, 0x4F, 0x8A, 0x55, 0x3D, 0x4F, 0xB8,
        0x3C, 0xFF, 0x20, 0xBA, 0x10, 0x23, 0x4B, 0x40,
        0x40, 0x00, 0x00, 0x00
};

/* HMAC-SHA1 test vector from RFC2202 */
static const unsigned char sha1padkeydata1[] = "Jefe";  /* 4 */

/* HMAC-MD5 test vector from RFC2202 */

static const unsigned char md5padkeydata1[] = "Jefe";   /* 4 */

/* Plaintext padded manually for no pad test */
static const unsigned char md5paddigest1_2[] =  /* 16 */
{
        0x75, 0x0c, 0x78, 0x3e, 0x6a, 0xb0, 0xb5, 0x03,
        0xea, 0xa8, 0x6e, 0x31, 0x0a, 0x5d, 0xb7, 0x38
};

/* HMAC-SHA256 test vector from draft-ietf-ipsec-ciph-sha-256-01.txt */
static const unsigned char sha256padkeydata1[] =        /* 32 */
{
        0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
        0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10,
        0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18,
        0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20
};

/* Plaintext padded manually for no pad test */
static const unsigned char sha256padplaintext1_2[] =    /* 64 */
{
        'a', 'b', 'c', 0x80, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0x02, 0x18
};
static const unsigned char sha256paddigest1_2[] =       /* 32 */
{
        0xa2, 0x1b, 0x1f, 0x5d, 0x4c, 0xf4, 0xf7, 0x3a,
        0x4d, 0xd9, 0x39, 0x75, 0x0f, 0x7a, 0x06, 0x6a,
        0x7f, 0x98, 0xcc, 0x13, 0x1c, 0xb1, 0x6a, 0x66,
        0x92, 0x75, 0x90, 0x21, 0xcf, 0xab, 0x81, 0x81
};
static const HMACTESTTYPE hmactests[] =
{
        /* test digest can be generated starting with precomputes */
        {       /* SHA-1 HMAC 64 20byte */
                DPD_SHA_LDCTX_HMAC_PAD_ULCTX,
                FSL_HMAC_FLAGS_FINALIZE,       /* hmac flags */
                20,    /* key len */
                (unsigned char*)sha1testkey1,
                64,    /* msg len */
                (unsigned char*)sha1testmessage1,
                20,    /* digest len */
                (unsigned char*)sha1testdigest1,
                24,    /* pad len */
                (char*)sha1testipad1,
                (char*)sha1testopad1,
                "SHA-1 HMAC 64 20byte Flags IFP"
        },

        {       /* MD5 HMAC 64 20byte */
                DPD_MD5_LDCTX_HMAC_PAD_ULCTX,
                FSL_HMAC_FLAGS_FINALIZE,       /* hmac flags */
                20,    /* key len */
                (unsigned char*)md5testkey1,
                64,    /* msg len */
                (unsigned char*)md5testmessage1,
                16,    /* digest len */
                (unsigned char*)md5testdigest1,
                24,    /* pad len */
                (char*)md5testipad1,
                (char*)md5testopad1,
                "MD5 HMAC 64 20byte Flags IFP"
        },

        {       /* SHA-256 HMAC 64 20byte */
                DPD_SHA256_LDCTX_HMAC_PAD_ULCTX,
                FSL_HMAC_FLAGS_FINALIZE | FSL_HMAC_FLAGS_SAVE, /* hmac flags */
                20,    /* key len */
                (unsigned char*)sha256testkey1,
                8,     /* msg len */
                (unsigned char*)sha256testmessage1,
                32,    /* digest len */
                (unsigned char*)sha256testdigest1,
                36,    /* pad len */
                (char*)sha256testipad1,
                (char*)sha256testopad1,
                "SHA-256 HMAC 64 20byte Flags ISFP"
        },

        /* test digest can be generated starting with the key */
        {       /* MD5 HMAC PAD Test 1 */
                DPD_MD5_LDCTX_HMAC_PAD_ULCTX,
                FSL_HMAC_FLAGS_FINALIZE | FSL_HMAC_FLAGS_INIT, /* hmac flags */
                4,
                (unsigned char *) md5padkeydata1,
                28,
                (unsigned char *) md5padplaintext1_2,
                16,
                (unsigned char *) md5paddigest1_2,
                0,
                NULL,
                NULL,
                "MD5 HMAC PAD TEST 1 Flags IF"
        },

        {       /* SHA-1 HMAC PAD Test 1 */
                DPD_SHA_LDCTX_HMAC_PAD_ULCTX,
                FSL_HMAC_FLAGS_FINALIZE | FSL_HMAC_FLAGS_INIT, /* hmac flags */
                4,
                (unsigned char *) sha1padkeydata1,
                28,
                (unsigned char *) md5padplaintext1_2,
                20,
                (unsigned char *) sha1paddigest1_2,
                0,
                NULL,
                NULL,
                "SHA-1 HMAC PAD TEST 1 Flags IF"
        },

        {       /* SHA-256 HMAC PAD Test 1 */
                DPD_SHA256_LDCTX_HMAC_PAD_ULCTX,
                FSL_HMAC_FLAGS_FINALIZE | FSL_HMAC_FLAGS_INIT, /* hmac flags */
                32,
                (unsigned char *) sha256padkeydata1,
                3,
                (unsigned char *) sha256padplaintext1_2,
                32,
                (unsigned char *) sha256paddigest1_2,
                0,
                NULL,
                NULL,
                "SHA-256 HMAC PAD TEST 1 Flags IF"
        },

        /* test that adding the save flag does not hurt the results */
        {       /* MD5 HMAC PAD Test 1 */
                DPD_MD5_LDCTX_HMAC_PAD_ULCTX,
                FSL_HMAC_FLAGS_FINALIZE | FSL_HMAC_FLAGS_INIT | FSL_HMAC_FLAGS_SAVE,
                4,
                (unsigned char *) md5padkeydata1,
                28,
                (unsigned char *) md5padplaintext1_2,
                16,
                (unsigned char *) md5paddigest1_2,
                0,
                NULL,
                NULL,
                "MD5 HMAC PAD TEST 1 Flags ISF"
        },

        {       /* SHA-1 HMAC PAD Test 1 */
                DPD_SHA_LDCTX_HMAC_PAD_ULCTX,
                FSL_HMAC_FLAGS_FINALIZE | FSL_HMAC_FLAGS_INIT | FSL_HMAC_FLAGS_SAVE,
                4,
                (unsigned char *) sha1padkeydata1,
                28,
                (unsigned char *) md5padplaintext1_2,
                20,
                (unsigned char *) sha1paddigest1_2,
                0,
                NULL,
                NULL,
                "SHA-1 HMAC PAD TEST 1 Flags ISF"
        },

        {       /* SHA-256 HMAC PAD Test 1 */
                DPD_SHA256_LDCTX_HMAC_PAD_ULCTX,
                FSL_HMAC_FLAGS_FINALIZE | FSL_HMAC_FLAGS_INIT | FSL_HMAC_FLAGS_SAVE,
                32,
                (unsigned char *) sha256padkeydata1,
                3,
                (unsigned char *) sha256padplaintext1_2,
                32,
                (unsigned char *) sha256paddigest1_2,
                0,
                NULL,
                NULL,
                "SHA-256 HMAC PAD TEST 1 Flags ISF"
        }
};

/*=================================== For sahara_sym_test ========================================*/
static unsigned char symtestkey1[] =
{
        0x2B, 0x7E, 0x15, 0x16, 0x28, 0xAE, 0xD2, 0xA6,
        0xAB, 0xF7, 0x15, 0x88, 0x09, 0xCF, 0x4F, 0x3C
};

static unsigned char symtestctr1[] =
{
        0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7,
        0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0xFF
};

static unsigned char symtestinput1[] =
{
        0x6B, 0xC1, 0xBE, 0xE2, 0x2E, 0x40, 0x9F, 0x96,
        0xE9, 0x3D, 0x7E, 0x11, 0x73, 0x93, 0x17, 0x2A,
        0xAE, 0x2D, 0x8A, 0x57, 0x1E, 0x03, 0xAC, 0x9C,
        0x9E, 0xB7, 0x6F, 0xAC, 0x45, 0xAF, 0x8E, 0x51,
        0x30, 0xC8, 0x1C, 0x46, 0xA3, 0x5C, 0xE4, 0x11,
        0xE5, 0xFB, 0xC1, 0x19, 0x1A, 0x0A, 0x52, 0xEF
};

static unsigned char symtestoutput1[] =
{
        0x87, 0x4D, 0x61, 0x91, 0xB6, 0x20, 0xE3, 0x26,
        0x1B, 0xEF, 0x68, 0x64, 0x99, 0x0D, 0xB6, 0xCE,
        0x98, 0x06, 0xF6, 0x6B, 0x79, 0x70, 0xFD, 0xFF,
        0x86, 0x17, 0x18, 0x7B, 0xB9, 0xFF, 0xFD, 0xFF,
        0x5A, 0xE4, 0xDF, 0x3E, 0xDB, 0xD5, 0xD3, 0x5E,
        0x5B, 0x4F, 0x09, 0x02, 0x0D, 0xB0, 0x3E, 0xAB
};

#if 0
static unsigned char symtestkey2[] =
{
        0x17, 0x10, 0xE3, 0x0D, 0x2C, 0xD9, 0xFB, 0x66,
        0xD2, 0x61, 0x59, 0x22, 0x48, 0x0D, 0xEA, 0xBC
};
#endif

#if 0
static unsigned char symtestinput2[] =
{
        0xF3, 0x94, 0x62, 0x33, 0x8D, 0xB0, 0x87, 0xB5,
        0x1E, 0x92, 0xA9, 0x49, 0x37, 0x38, 0x99, 0x3B,
        0x91, 0x76, 0x54, 0x86, 0xB3, 0xA2, 0x51, 0x1C,
        0x29, 0x47, 0x63, 0x88, 0xB7, 0x64, 0x50, 0x2D,
        0x5C, 0x33, 0xB0, 0xBF, 0x76, 0xD4, 0x32, 0x57,
        0xE1, 0xFD, 0xCE, 0xA9, 0x79, 0x03, 0x1B, 0xC4,
        0xB2, 0x42, 0x3E, 0xF0, 0xD0, 0x6E, 0xA4, 0xF1,
        0x6A, 0x9B, 0x11, 0x6D, 0xA7, 0x9F, 0x23, 0x99,
        0x0F, 0xAB, 0xDD, 0x03, 0x35, 0x9D, 0x57, 0x87,
        0x2F, 0x03, 0x1E, 0x5F, 0x13, 0x42, 0x72, 0x46,
        0x6A, 0xE5, 0xE8, 0x9C, 0xEC, 0x77, 0x88, 0xA9,
        0x4E, 0xF5, 0x0E, 0xB9, 0x93, 0x47, 0x54, 0x47,
        0x9D, 0xB6, 0x91, 0x00, 0x72, 0xD9, 0x69, 0xBB,
        0xF0, 0xF5, 0x85, 0x3E, 0x62, 0x44, 0xB7, 0xE0,
        0xBB, 0x14, 0x45, 0xEA, 0xDA, 0x49, 0x7B, 0xD5,
        0xAD, 0x27, 0x12, 0x1E, 0x83, 0xE5, 0x88, 0xF5,
        0x76, 0x04, 0x0B, 0x76, 0x2D, 0xD1, 0xF2, 0xA1,
        0xE7, 0x28, 0x8A, 0xD2, 0x1D, 0xCE, 0x1A, 0xEF,
        0x7C, 0x7B, 0xE1, 0xF6, 0xC6, 0xE6, 0x15, 0x3D,
        0x34, 0xF9, 0x70, 0x03, 0xDA, 0x7B, 0x7C, 0x9A,
        0xDB, 0x3D, 0x22, 0xDB, 0xBB, 0x44, 0x97, 0x1C,
        0xAF, 0xDD, 0x4B, 0x60, 0x4F, 0x1F, 0xE3, 0x07,
        0x5E, 0xBE, 0xDF, 0x91, 0x33, 0xCB, 0x03, 0xE1,
        0x69, 0x32, 0x11, 0x88, 0x51, 0x82, 0x05, 0x66,
        0xEB, 0x02, 0x47, 0x5E, 0xCC, 0x68, 0x11, 0x49,
        0xBA, 0x56, 0x7D, 0xE3, 0x5D, 0xE8, 0x75, 0xEF,
        0xE8, 0x7A, 0xFC, 0x44, 0xFB, 0xEB, 0x0D, 0xFC,
        0xAA, 0x88, 0x77, 0x85, 0xF7, 0xEB, 0x0D, 0xBE,
        0x98, 0xEA, 0x81, 0xE2, 0x69, 0xEE, 0x32, 0x7D,
        0x51, 0xC7, 0x6E, 0x0F, 0x07, 0x5C, 0x46, 0xAE,
        0x7C, 0x44, 0x8C, 0x53, 0x54, 0xB2, 0x10, 0xFE,
        0x30, 0xB0, 0xBC, 0x8B, 0x38, 0x22, 0x9B, 0x42,
        0xB0, 0x88, 0x70, 0x09, 0xEF, 0xFE, 0xE5, 0x47,
        0x96, 0x60, 0x04, 0x50, 0x5F, 0x1D, 0xE8, 0x7E,
        0x51, 0xA8, 0x79, 0xB5, 0xC5, 0x03, 0x05, 0x90,
        0x05, 0x54, 0x91, 0xE0, 0xD3, 0x05, 0xCC, 0xC9,
        0xD6, 0x63, 0x4C, 0x26, 0x17, 0x37, 0x2F, 0x69,
        0x85, 0x47, 0xBB, 0xC9, 0xD3, 0x49, 0x05, 0xD0,
        0x76, 0x2A, 0x47, 0x20, 0x36, 0x39, 0xF8, 0x93,
        0x12, 0x16, 0x41, 0x85, 0xE2, 0xEE, 0xD3, 0x63,
        0x86, 0xFB, 0xE3, 0x47, 0xED, 0xAF, 0x28, 0xE2,
        0xEF, 0x9A, 0xAB, 0x58, 0x2A, 0x75, 0x5B, 0x52,
        0xD6, 0x47, 0x12, 0xF9, 0xDE, 0x28, 0x14, 0x20,
        0x14, 0x90, 0xAC, 0x33, 0xD7, 0xB1, 0x00, 0x55,
        0x15, 0xC9, 0x9F, 0x33, 0xDC, 0xF6, 0x04, 0xE6,
        0x7F, 0x70, 0x80, 0x90, 0x7F, 0xAF, 0xC4, 0xE4,
        0x30, 0x72, 0x8E, 0x67, 0x53, 0x1A, 0x93, 0x86,
        0xA1, 0x55, 0x4D, 0x58, 0x7B, 0x95, 0xB3, 0x1E,
        0xAE, 0x3E, 0x7E, 0x67, 0xA1, 0x16, 0x0D, 0xE2,
        0xB6, 0xD7, 0x80, 0xBC, 0x49, 0x7D, 0x32, 0xA3,
        0x1B, 0x1A, 0x07, 0x40, 0x7E, 0xD4, 0xCD, 0x52,
        0x28, 0xF0, 0x35, 0x1D, 0xED, 0x5C, 0x6C, 0x76,
        0x57, 0xC1, 0x19, 0x1A, 0x55, 0x8A, 0xA5, 0x7C,
        0xEE, 0xD6, 0x8A, 0xE1, 0x54, 0xDC, 0xAB, 0xE0,
        0x08, 0xA1, 0x5F, 0x18, 0xA4, 0x92, 0x33, 0x41,
        0xEF, 0xE3, 0x0D, 0x61, 0xA9, 0x41, 0xBE, 0x4C,
        0xEB, 0xB4, 0x9E, 0x3A, 0x64, 0x4D, 0x49, 0x90,
        0x5C, 0x6C, 0x13, 0xBB, 0xC5, 0x42, 0x52, 0x29,
        0x3F, 0x66, 0x14, 0x39, 0x60, 0x06, 0x4A, 0x4E,
        0x15, 0xA8, 0x18, 0xBC, 0x7F, 0xF2, 0x57, 0xC6,
        0x1F, 0x6F, 0xD6, 0x6B, 0x98, 0xCD, 0x89, 0x33,
        0x07, 0x8E, 0x27, 0xBD, 0x1A, 0x9B, 0x5F, 0x3D,
        0xE1, 0xBA, 0x38, 0xA3, 0xA4, 0x5C, 0xAF, 0xA9,
        0x8E, 0xB2, 0x30, 0x80, 0x99, 0x9C, 0xFB, 0x45,
        0x7D, 0x42, 0x22, 0x0D, 0x10, 0xF1, 0x13, 0xB1,
        0xD2, 0x2B, 0x6D, 0x14, 0x28, 0x4E, 0x22, 0x1A,
        0xE5, 0xEC, 0x77, 0x12, 0xBC, 0x37, 0x1C, 0xBC,
        0x2A, 0x6C, 0xC4, 0xB3, 0x76, 0xDE, 0x87, 0x5D,
        0x6C, 0x75, 0x76, 0x35, 0x41, 0x1A, 0xA6, 0x91,
        0x78, 0x29, 0x24, 0xA3, 0x1B, 0x37, 0x04, 0xF1,
        0x1E, 0x41, 0x16, 0xF6, 0x4B, 0xB2, 0x5D, 0x28,
        0x8E, 0x37, 0xE1, 0x16, 0xF1, 0xD3, 0xEF, 0xDE,
        0x27, 0x49, 0x65, 0xB1, 0xFC, 0x1F, 0x1F, 0x8E,
        0x8B, 0x67, 0x20, 0x07, 0x7A, 0xAC, 0x85, 0x2F,
        0x30, 0xF3, 0xEE, 0x7C, 0x4C, 0x63, 0x5A, 0xC4,
        0x3A, 0x6D, 0x28, 0x20, 0x3B, 0x0D, 0x41, 0xD0,
        0xC1, 0xF6, 0x15, 0x08, 0x68, 0x4D, 0x70, 0x9F,
        0x75, 0xBB, 0xCE, 0x93, 0x22, 0x7B, 0x40, 0x75,
        0x9E, 0x39, 0x75, 0x83, 0x13, 0x53, 0x12, 0xA3,
        0x82, 0x63, 0xD0, 0x01, 0xDD, 0x93, 0xA0, 0x72,
        0x28, 0xAE, 0x46, 0x76, 0x05, 0x6F, 0xA4, 0xEE,
        0x77, 0xF4, 0x36, 0x53, 0x46, 0xE8, 0xE2, 0x9F,
        0xBF, 0x3D, 0xB6, 0xA6, 0x4B, 0x06, 0x9A, 0x0E,
        0x96, 0x61, 0xAE, 0xA1, 0xB8, 0xE7, 0x4A, 0x39,
        0x20, 0x98, 0x50, 0xF2, 0xA8, 0xC2, 0xDD, 0xE3,
        0xAD, 0xD9, 0xF3, 0x0C, 0x76, 0xB2, 0x39, 0xBF,
        0xC4, 0x25, 0x55, 0x38, 0xF5, 0x77, 0x23, 0x84,
        0x7B, 0xAB, 0x2B, 0xA0, 0x06, 0x05, 0x93, 0xD5,
        0x44, 0xD4, 0x23, 0x2B, 0x82, 0xFE, 0x57, 0x1A,
        0x0A, 0x27, 0x3E, 0x38, 0x95, 0x0F, 0x21, 0x22,
        0xB4, 0x0D, 0x8D, 0x41, 0x70, 0x1B, 0xF2, 0xBF,
        0x10, 0x7B, 0x46, 0x57, 0x57, 0x5B, 0xE8, 0x2E,
        0x09, 0x7D, 0x45, 0x85, 0x19, 0x54, 0x63, 0x66,
        0x54, 0x95, 0xE2, 0x0D, 0xE2, 0xAA, 0x93, 0x49,
        0x71, 0x09, 0x32, 0x83, 0x6C, 0xDB, 0x66, 0xB0,
        0x0B, 0x03, 0x9C, 0xD4, 0x94, 0xD4, 0xCD, 0x62,
        0xBB, 0xA0, 0xD7, 0x20, 0x4D, 0x69, 0x6D, 0xD6,
        0x32, 0xD1, 0x46, 0x75, 0xF0, 0xAA, 0xA2, 0xEE,
        0xB1, 0x26, 0xB0, 0x7A, 0xF5, 0x1E, 0xF6, 0x81,
        0xF8, 0x6E, 0x5D, 0xE1, 0x00, 0xD2, 0xE0, 0xC7,
        0x7E, 0x48, 0x91, 0xCF, 0x55, 0x5E, 0xF6, 0xAE,
        0x1B, 0x83, 0x67, 0x16, 0xAF, 0xAD, 0x76, 0x07,
        0x07, 0x68, 0x0B, 0x56, 0x70, 0xBB, 0x31, 0x91,
        0x41, 0xDD, 0x53, 0xFC, 0x33, 0x2F, 0xD5, 0xED,
        0x4F, 0x74, 0xC2, 0x23, 0xC0, 0xCA, 0x9B, 0x6D,
        0x62, 0x48, 0xDA, 0x4B, 0x63, 0xC6, 0x50, 0xBD,
        0xD8, 0xCB, 0xDC, 0x03, 0x96, 0x0A, 0xBD, 0x7A,
        0x22, 0x6D, 0xE0, 0x5F, 0x1E, 0x3A, 0x79, 0x9B,
        0x02, 0x1E, 0x53, 0x60, 0x76, 0xBD, 0x0E, 0xC4,
        0x32, 0xB7, 0xCF, 0x27, 0xA8, 0x86, 0x87, 0x73,
        0x67, 0x46, 0x5B, 0x20, 0x7B, 0xD2, 0x59, 0x0D,
        0xB1, 0x2F, 0xFD, 0xF8, 0x0B, 0xBE, 0xAF, 0xD0,
        0x42, 0x34, 0xC1, 0x84, 0xB5, 0xBD, 0x19, 0xA4,
        0x8C, 0x5E, 0x0B, 0x77, 0x74, 0xEF, 0x8C, 0xC4,
        0xCA, 0xC3, 0x50, 0x09, 0x8A, 0x5A, 0xDB, 0x52,
        0xDD, 0x2D, 0x3D, 0x71, 0x99, 0xFC, 0x77, 0xC2,
        0x94, 0xA0, 0x27, 0x46, 0x16, 0xCB, 0x9E, 0x30,
        0x4C, 0xC3, 0xE8, 0xBF, 0x15, 0x81, 0xE9, 0x83,
        0xF2, 0x27, 0x21, 0xCF, 0x87, 0x5B, 0x33, 0x8B,
        0x6C, 0x6A, 0xC8, 0x28, 0xC4, 0xAE, 0xE2, 0xE1,
        0x55, 0x43, 0x32, 0x14, 0x85, 0x5D, 0x9A, 0xC4,
        0x22, 0x66, 0x61, 0x3B, 0x2F, 0x30, 0x3F, 0xBC,
        0xA8, 0x4E, 0xC8, 0x3D, 0x8B, 0x0B, 0x67, 0x31,
        0xB7, 0xF4, 0x2B, 0x12, 0x4C, 0x64, 0x1E, 0xF6,
        0x99, 0xBC, 0x02, 0xDD, 0xBF, 0x69, 0x45, 0xB0,
        0xF3, 0x94, 0x62, 0x33, 0x8D, 0xB0, 0x87, 0xB5,
        0x1E, 0x92, 0xA9, 0x49, 0x37, 0x38, 0x99, 0x3B
};
#endif

static unsigned char symtestkey3[] =
{
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

static unsigned char symtestinput3[] =
{
        0x66, 0xe9, 0x4b, 0xd4, 0xef, 0x8a, 0x2c, 0x3b,
        0x88, 0x4c, 0xfa, 0x59, 0xca, 0x34, 0x2b, 0x2e,
        0xf7, 0x95, 0xbd, 0x4a, 0x52, 0xe2, 0x9e, 0xd7,
        0x13, 0xd3, 0x13, 0xfa, 0x20, 0xe9, 0x8d, 0xbc,
        0x10, 0xa5, 0x82, 0xed, 0x9b, 0xd1, 0x5d, 0xbe,
        0x6a, 0x6e, 0xe4, 0xb1, 0x58, 0xd5, 0x79, 0x0e,
        0x75, 0x4b, 0x44, 0xfa, 0x7b, 0xae, 0x0b, 0xb2,
        0xf4, 0x82, 0xf2, 0xf2, 0x63, 0xa4, 0x24, 0x00
};

static unsigned char symtestoutput3[] =
{
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x66, 0xe9, 0x4b, 0xd4, 0xef, 0x8a, 0x2c, 0x3b,
        0x88, 0x4c, 0xfa, 0x59, 0xca, 0x34, 0x2b, 0x2e,
        0xf7, 0x95, 0xbd, 0x4a, 0x52, 0xe2, 0x9e, 0xd7,
        0x13, 0xd3, 0x13, 0xfa, 0x20, 0xe9, 0x8d, 0xbc
};

static unsigned char symtestkey4[] =
{
        0xCC, 0xB4, 0x8F, 0xD3, 0xF9, 0x0B, 0xF2, 0x96
};

static unsigned char symtestinput4[] =
{
        0xCA, 0xCE, 0x0F, 0x3A, 0x37, 0xF6, 0xDD, 0x9A,
        0xEB, 0x04, 0x30, 0x23, 0x6C, 0xF2, 0x85, 0xC5,
        0xC2, 0x7D, 0x6C, 0xCF, 0x81, 0x08, 0x5D, 0x52,
        0x0A, 0x23, 0x5D, 0x67, 0xCF, 0x82, 0x27, 0x3E,
        0xAC, 0x91, 0xD7, 0xD9, 0x67, 0xD0, 0x4B, 0x7A,
        0x97, 0xE2, 0x62, 0xCE, 0xBA, 0x51, 0xAF, 0x90,
        0x9F, 0xEC, 0x7D, 0x46, 0xB6, 0xFB, 0x7C, 0xD1,
        0x35, 0x00, 0xC8, 0xFD, 0x06, 0xB1, 0xA5, 0x54,
        0xCB, 0x6A, 0x1A, 0x19, 0x6A, 0xA1, 0x67, 0x65,
        0xFD, 0xEF, 0x29, 0x8B, 0x70, 0xA8, 0xE4, 0x34,
        0x25, 0x62, 0xFE, 0x0F, 0xBA, 0x8B, 0xF6, 0xCA,
        0xDC, 0x1E, 0x64, 0x9F, 0x83, 0x1D, 0xDF, 0xDF,
        0x50, 0xE9, 0xCA, 0xA7, 0x41, 0xFF, 0xEC, 0xD3,
        0x6B, 0xD4, 0xAB, 0x10, 0x48, 0x02, 0x93, 0xB9,
        0x29, 0x31, 0x32, 0x62, 0x92, 0xF6, 0x8F, 0x14,
        0xF8, 0x1B, 0x3C, 0x56, 0xB8, 0xD0, 0xE1, 0xF7,
        0x75, 0xCD, 0xB1, 0x74, 0xD3, 0xCB, 0x1D, 0x9A,
        0x07, 0xB3, 0xD8, 0xB6, 0x4C, 0x71, 0x18, 0xAD,
        0xC8, 0x12, 0xEF, 0x03, 0x0F, 0x7F, 0xCE, 0xB1,
        0xFF, 0x86, 0xC3, 0x85, 0x57, 0xBF, 0x6B, 0xB8,
        0x3F, 0xAD, 0xAC, 0x74, 0xF5, 0x9C, 0x51, 0xD5,
        0xB1, 0x13, 0xBE, 0x04, 0x9D, 0x70, 0x43, 0x2D,
        0x09, 0x5F, 0xD7, 0x2D, 0x9D, 0x3D, 0xB5, 0x70,
        0xFA, 0x89, 0x2A, 0x7B, 0x44, 0x7A, 0xB0, 0x6A
};

static unsigned char symtestoutput4[] =
{
        0x4F, 0x0D, 0x4C, 0xDC, 0x80, 0xC0, 0x5C, 0xED,
        0xA0, 0xF4, 0xF6, 0xC0, 0x9B, 0x3A, 0x2B, 0x9E,
        0x1F, 0xA1, 0x40, 0x38, 0x46, 0x1A, 0xA3, 0x86,
        0x52, 0x40, 0x3A, 0xC9, 0xB9, 0x94, 0x6E, 0xC4,
        0x99, 0xC0, 0x07, 0x18, 0xF2, 0x10, 0x81, 0xBB,
        0xC6, 0x04, 0xE8, 0x1D, 0x6E, 0x72, 0xAB, 0xA3,
        0x14, 0xA2, 0x1B, 0x7E, 0x19, 0x6E, 0xA1, 0xB5,
        0x49, 0xED, 0x8A, 0xDE, 0xCB, 0x2D, 0xC2, 0xED,
        0x01, 0x56, 0xC5, 0x8A, 0xF8, 0x66, 0x3F, 0xDF,
        0x21, 0x2C, 0x09, 0x5A, 0xAA, 0xF2, 0x2B, 0x91,
        0x9C, 0x83, 0xA1, 0xA8, 0x16, 0x01, 0xB9, 0xAE,
        0xCC, 0xEA, 0xA2, 0x1B, 0xF0, 0xB3, 0xBB, 0xDD,
        0x17, 0xA1, 0xDC, 0x74, 0xD4, 0x2C, 0x6E, 0x07,
        0x45, 0x02, 0x53, 0xCB, 0x8A, 0x1A, 0xBA, 0x1B,
        0xB2, 0xE1, 0xF4, 0xD4, 0x9C, 0x1B, 0xCF, 0xE9,
        0xFD, 0xF8, 0x5B, 0x71, 0x5E, 0x54, 0x02, 0x03,
        0xC3, 0x28, 0xF9, 0xEE, 0xA2, 0x35, 0xEE, 0xA5,
        0x00, 0xF3, 0x8A, 0x32, 0x0D, 0x83, 0xDF, 0x7B,
        0x6B, 0x00, 0x6C, 0xD6, 0x42, 0x39, 0x3F, 0xA7,
        0x84, 0x01, 0x03, 0x7D, 0x71, 0x62, 0xFB, 0x75,
        0xB5, 0xC9, 0x31, 0x2B, 0xB3, 0xEB, 0xC8, 0x87,
        0xB8, 0xD4, 0xA9, 0xE4, 0x5D, 0x8E, 0xA6, 0x53,
        0xC3, 0x8A, 0xFB, 0xAB, 0x46, 0x84, 0xFE, 0x5E,
        0x95, 0xC6, 0x92, 0x42, 0x6E, 0xFC, 0x60, 0xA1
};

static unsigned char symtestkey5[] =
{
        0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
        0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01
};

static unsigned char symtestinput5[] =
{
        0x95, 0xF8, 0xA5, 0xE5, 0xDD, 0x31, 0xD9, 0x00,
        0xDD, 0x7F, 0x12, 0x1C, 0xA5, 0x01, 0x56, 0x19,
        0x2E, 0x86, 0x53, 0x10, 0x4F, 0x38, 0x34, 0xEA,
        0x4B, 0xD3, 0x88, 0xFF, 0x6C, 0xD8, 0x1D, 0x4F,
        0x20, 0xB9, 0xE7, 0x67, 0xB2, 0xFB, 0x14, 0x56,
        0x55, 0x57, 0x93, 0x80, 0xD7, 0x71, 0x38, 0xEF,
        0x6C, 0xC5, 0xDE, 0xFA, 0xAF, 0x04, 0x51, 0x2F,
        0x0D, 0x9F, 0x27, 0x9B, 0xA5, 0xD8, 0x72, 0x60
};

static unsigned char symtestoutput5[] =
{
        0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

#if 0
static unsigned char symtestkey6[] =
{
        0x65, 0xA4, 0xD7, 0xD2
};

static unsigned char symtestinput6[] =
{
        0x46, 0x73, 0xF7, 0x2F, 0x53, 0x9F, 0x20, 0x69,
        0xD0, 0xA7, 0x00, 0xD3, 0x0F, 0x12, 0xA3, 0xB1,
        0xD8, 0x88, 0x17, 0xC3, 0x49, 0xE1, 0xC4, 0x83,
        0x44, 0xD7, 0xD3, 0xF2, 0x4E, 0xA1, 0x22, 0x2A,
        0xEE, 0x67, 0xA7, 0x1F, 0x6A, 0xD5, 0xE9, 0xDD,
        0xF5, 0xB3, 0x47, 0xAD, 0x2A, 0xC6, 0xB7, 0x4F,
        0xE9, 0xEB, 0xEC, 0xE3, 0x74, 0xB3, 0x43, 0x91,
        0xDA, 0x56, 0x87, 0x85, 0x8F, 0x66, 0x00, 0x66,
        0x4B, 0x0E, 0xB9, 0xCD, 0xFB, 0x28, 0x8B, 0xE7,
        0xFA, 0x2A, 0xC4, 0xC8, 0x37, 0x10, 0x0C, 0x95,
        0x99, 0x42, 0x50, 0x13, 0x55, 0xB9, 0x62, 0xBC,
        0x53, 0x49, 0x0E, 0xED
};

static unsigned char symtestoutput6[] =
{
        0xC9, 0x99, 0xB8, 0x6F, 0x59, 0x52, 0xAD, 0xB5,
        0x22, 0x83, 0xA1, 0x76, 0x48, 0xCD, 0x1E, 0x24,
        0x69, 0xEE, 0x45, 0xBB, 0x12, 0x5F, 0x0B, 0x83,
        0x5A, 0x00, 0x80, 0x19, 0xCF, 0x9B, 0x45, 0xD2,
        0xBF, 0x8A, 0x44, 0x48, 0x68, 0x21, 0x25, 0x45,
        0x9C, 0x2B, 0xD8, 0x1A, 0x4B, 0xE8, 0x25, 0xCB,
        0xF4, 0x9C, 0xF4, 0xFE, 0xE9, 0x9B, 0x72, 0x89,
        0x69, 0x61, 0xB3, 0x0B, 0x15, 0xDC, 0xF4, 0xDC,
        0x2B, 0x6E, 0xDD, 0x2C, 0x43, 0x31, 0xA4, 0xE5,
        0x1A, 0x56, 0x89, 0xA5, 0xC9, 0xCD, 0x83, 0x3E,
        0x62, 0x79, 0xA2, 0xA8, 0xA9, 0x04, 0x05, 0xB1,
        0xE8, 0xA3, 0xEF, 0x3F, 0x86, 0xC1, 0xC7, 0xCE,
        0xE0, 0xA6, 0x32, 0x58, 0xCC, 0x2C, 0xD0, 0x5D,
        0x9C, 0x2D, 0xE5, 0x1C, 0xEC, 0x17, 0xFA, 0x4E,
        0xB0, 0x27, 0xD5, 0xE1, 0x5C, 0xAE, 0x10, 0x28,
        0x5B, 0x35, 0x41, 0xDE, 0x31, 0xB9, 0xDA, 0xE2,
        0x98, 0x74, 0x99, 0xAC, 0xDF, 0x75, 0x9E, 0xEB,
        0xA7, 0x84, 0x11, 0x5E, 0xCB, 0xE6, 0x02, 0xC0,
        0xBF, 0x45, 0x6F, 0xA5, 0x20, 0x0E, 0xCA, 0xD9,
        0x2F, 0xF7, 0x3C, 0xCF, 0x0D, 0x00, 0x60, 0xAA,
        0xD2, 0xC8, 0x72, 0x26, 0x54, 0x19, 0x94, 0x70,
        0xBB, 0x83, 0x62, 0x9A, 0x42, 0x4D, 0x5F, 0x01,
        0x92, 0xDC, 0xCD, 0xE3, 0xA4, 0x0C, 0xBC, 0x57,
        0x73, 0x07, 0x85, 0x47, 0x8E, 0x16, 0xE4, 0x0B,
        0x7E, 0x96, 0xED, 0x2E, 0x23, 0x8B, 0xD8, 0x8A,
        0x12, 0x7D, 0xD6, 0xF2, 0x8F, 0x1B, 0xF6, 0xFC,
        0x3A, 0x33, 0x4F, 0x77, 0x79, 0x4C, 0x91, 0xBD,
        0x52, 0x36, 0xC9, 0xD1, 0x13, 0x6D, 0xA1, 0x66,
        0x9B, 0xF1, 0x06, 0xB3, 0xF8, 0xFB, 0x48, 0xF0,
        0xC5, 0x46, 0x90, 0x63, 0xB8, 0x1F, 0xB2, 0x8C,
        0xBA, 0xD7, 0x69, 0x34, 0x29, 0xE9, 0xEE, 0xC6,
        0x81, 0xAF, 0x3B, 0x30, 0xE7, 0x6E, 0x71, 0xF9,
        0x0F, 0xFD, 0x1A, 0x39, 0xA0, 0xF3, 0x18, 0x1E,
        0x56, 0x43, 0x55, 0x53, 0x40, 0x03, 0xD3, 0xC4,
        0x9D, 0x88, 0xDD, 0x2B, 0x4B, 0x67, 0x93, 0x59,
        0x1D, 0x7C, 0x7F, 0x51, 0xF4, 0x44, 0x61, 0x21,
        0xDB, 0x68, 0xAB, 0x7A, 0x78, 0x14, 0x3E, 0xFE,
        0xBE, 0xAD, 0x7B, 0x89, 0x5A, 0x37, 0x15, 0xB4,
        0xF5, 0xB5, 0x09, 0xD4, 0x3D, 0x82, 0x76, 0xFF,
        0x6C, 0x25, 0x50, 0xB6, 0x80, 0x9F, 0xC2, 0x97,
        0x49, 0x22, 0xEA, 0x6B, 0xB7, 0x08, 0x95, 0x87,
        0x4A, 0x24, 0x64, 0xC3, 0x0A, 0x8D, 0x2A, 0x6A,
        0x5D, 0xC8, 0x4D, 0x00
};
#endif

static unsigned char symtestinput7[] =
{
        0x85, 0x55, 0x45, 0x44, 0x40, 0xDA, 0x44, 0xC2,
        0x6D, 0x1F, 0x33, 0xDA, 0xB4, 0xD7, 0x31, 0xF4,
        0x6A, 0x2D, 0x4D, 0x7C, 0x6C, 0x69, 0x56, 0x25,
        0xAE, 0xA7, 0x0D, 0x62, 0xA0, 0x68, 0x9B, 0xC6,
        0x40, 0x56, 0xDB, 0x49, 0xEF, 0x1B, 0x58, 0x1D,
        0xDC, 0x97, 0x88, 0x21, 0x2F, 0x5C, 0x00, 0x00
};

static unsigned char symtestoutput7[] =
{
        0xF8, 0x1A, 0x56, 0xE1, 0x4F, 0x80, 0x0C, 0xA4,
        0x90, 0x0F, 0x85, 0x4D, 0xBD, 0x65, 0x8D, 0xD5,
        0x7E, 0x3B, 0xD2, 0xFC, 0x05, 0x25, 0x9E, 0x2A,
        0xD6, 0x17, 0x1C, 0x52, 0xE8, 0xBD, 0xF2, 0x3E,
        0x2F, 0x70, 0x8F, 0x58, 0x43, 0x75, 0x1D, 0xFB,
        0xB4, 0x40, 0xCE, 0x78, 0xED, 0xDD, 0x00, 0x00
};

static unsigned char symtestkey8[] =
{
        0x65, 0xA4, 0xD7, 0xD2
};

static unsigned char symtestinput8[] =
{
        0x46, 0x73, 0xF7, 0x2F, 0x53, 0x9F, 0x20, 0x69,
        0xD0, 0xA7, 0x00, 0xD3, 0x0F, 0x12, 0xA3, 0xB1,
        0xD8, 0x88, 0x17, 0xC3, 0x49, 0xE1, 0xC4, 0x83,
        0x44, 0xD7, 0xD3, 0xF2, 0x4E, 0xA1, 0x22, 0x2A,
        0xEE, 0x67, 0xA7, 0x1F, 0x6A, 0xD5, 0xE9, 0xDD,
        0xF5, 0xB3, 0x47, 0xAD, 0x2A, 0xC6, 0xB7, 0x4F,
        0xE9, 0xEB, 0xEC, 0xE3, 0x74, 0xB3, 0x43, 0x91,
        0xDA, 0x56, 0x87, 0x85, 0x8F, 0x66, 0x00, 0x66,
        0x4B, 0x0E, 0xB9, 0xCD, 0xFB, 0x28, 0x8B, 0xE7,
        0xFA, 0x2A, 0xC4, 0xC8, 0x37, 0x10, 0x0C, 0x95,
        0x99, 0x42, 0x50, 0x13, 0x55, 0xB9, 0x62, 0xBC,
        0x53, 0x49, 0x0E, 0xED
};

static unsigned char symtestoutput8[] =
{
        0xC9, 0x99, 0xB8, 0x6F, 0x59, 0x52, 0xAD, 0xB5,
        0x22, 0x83, 0xA1, 0x76, 0x48, 0xCD, 0x1E, 0x24,
        0x69, 0xEE, 0x45, 0xBB, 0x12, 0x5F, 0x0B, 0x83,
        0x5A, 0x00, 0x80, 0x19, 0xCF, 0x9B, 0x45, 0xD2,
        0xBF, 0x8A, 0x44, 0x48, 0x68, 0x21, 0x25, 0x45,
        0x9C, 0x2B, 0xD8, 0x1A, 0x4B, 0xE8, 0x25, 0xCB,
        0xF4, 0x9C, 0xF4, 0xFE, 0xE9, 0x9B, 0x72, 0x89,
        0x69, 0x61, 0xB3, 0x0B, 0x15, 0xDC, 0xF4, 0xDC,
        0x2B, 0x6E, 0xDD, 0x2C, 0x43, 0x31, 0xA4, 0xE5,
        0x1A, 0x56, 0x89, 0xA5, 0xC9, 0xCD, 0x83, 0x3E,
        0x62, 0x79, 0xCD, 0xED, 0xE0, 0xA7, 0x57, 0x09,
        0x10, 0x1D, 0x71, 0xEC
};

static unsigned char symtestsboxout8[] =
{
        0x65, 0x38, 0xA2, 0xA8, 0xA9, 0x04, 0x05, 0xB1,
        0xE8, 0xA3, 0xEF, 0x3F, 0x86, 0xC1, 0xC7, 0xCE,
        0xE0, 0xA6, 0x32, 0x58, 0xCC, 0x2C, 0xD0, 0x5D,
        0x9C, 0x2D, 0xE5, 0x1C, 0xEC, 0x17, 0xFA, 0x4E,
        0xB0, 0x27, 0xD5, 0xE1, 0x5C, 0xAE, 0x10, 0x28,
        0x5B, 0x35, 0x41, 0xDE, 0x31, 0xB9, 0xDA, 0xE2,
        0x98, 0x74, 0x99, 0xAC, 0xDF, 0x75, 0x9E, 0xEB,
        0xA7, 0x84, 0x11, 0x5E, 0xCB, 0xE6, 0x02, 0xC0,
        0xBF, 0x45, 0x6F, 0xA5, 0x20, 0x0E, 0xCA, 0xD9,
        0x2F, 0xF7, 0x3C, 0xCF, 0x0D, 0x00, 0x60, 0xAA,
        0xD2, 0xC8, 0x72, 0x26, 0x54, 0x19, 0x94, 0x70,
        0xBB, 0x83, 0x62, 0x9A, 0x42, 0x4D, 0x5F, 0x01,
        0x92, 0xDC, 0xCD, 0xE3, 0xA4, 0x0C, 0xBC, 0x57,
        0x73, 0x07, 0x85, 0x47, 0x8E, 0x16, 0xE4, 0x0B,
        0x7E, 0x96, 0xED, 0x2E, 0x23, 0x8B, 0xD8, 0x8A,
        0x12, 0x7D, 0xD6, 0xF2, 0x8F, 0x1B, 0xF6, 0xFC,
        0x3A, 0x33, 0x4F, 0x77, 0x79, 0x4C, 0x91, 0xBD,
        0x52, 0x36, 0xC9, 0xD1, 0x13, 0x6D, 0xA1, 0x66,
        0x9B, 0xF1, 0x06, 0xB3, 0xF8, 0xFB, 0x48, 0xF0,
        0xC5, 0x46, 0x90, 0x63, 0xB8, 0x1F, 0xB2, 0x8C,
        0xBA, 0xD7, 0x69, 0x34, 0x29, 0xE9, 0xEE, 0xC6,
        0x81, 0xAF, 0x3B, 0x30, 0xE7, 0x6E, 0x71, 0xF9,
        0x0F, 0xFD, 0x1A, 0x39, 0xA0, 0xF3, 0x18, 0x1E,
        0x56, 0x43, 0x55, 0x53, 0x40, 0x03, 0xD3, 0xC4,
        0x9D, 0x88, 0xDD, 0x2B, 0x4B, 0x67, 0x93, 0x59,
        0x1D, 0x7C, 0x7F, 0x51, 0xF4, 0x44, 0x61, 0x21,
        0xDB, 0x68, 0xAB, 0x7A, 0x78, 0x14, 0x3E, 0xFE,
        0xBE, 0xAD, 0x7B, 0x89, 0x5A, 0x37, 0x15, 0xB4,
        0xF5, 0xB5, 0x09, 0xD4, 0x3D, 0x82, 0x76, 0xFF,
        0x6C, 0x25, 0x50, 0xB6, 0x80, 0x9F, 0xC2, 0x97,
        0x49, 0x22, 0xEA, 0x6B, 0xB7, 0x08, 0x95, 0x87,
        0x4A, 0x24, 0x64, 0xC3, 0x0A, 0x8D, 0x2A, 0x6A,
        0x5D, 0xC8, 0x4D
};

#if 0
static unsigned char symtestoutput2[] =
{
        0xAE, 0x2D, 0x35, 0xC9, 0xB9, 0x03, 0x0E, 0xD0,
        0x96, 0xF5, 0xE7, 0xEE, 0x42, 0xA8, 0x11, 0x84,
        0x2E, 0x1B, 0x95, 0xA1, 0x2B, 0xAF, 0x3C, 0xBB,
        0xB1, 0x1B, 0x31, 0xFA, 0x19, 0x68, 0xAA, 0x18,
        0x32, 0x94, 0x4A, 0xAB, 0x08, 0x3F, 0xC4, 0x8E,
        0xB1, 0xB6, 0x94, 0x06, 0x5C, 0x9A, 0x60, 0x4B,
        0xDA, 0x84, 0xFD, 0xDD, 0x28, 0xB8, 0x20, 0x46,
        0x30, 0xC8, 0xD2, 0x6C, 0xA3, 0x88, 0x86, 0x29,
        0xE7, 0x89, 0xC1, 0x05, 0x19, 0x32, 0x81, 0xEC,
        0xBA, 0x73, 0x7B, 0x82, 0xDD, 0x2F, 0x4C, 0xC0,
        0xAD, 0xD1, 0x0B, 0x2F, 0x1C, 0x88, 0xBD, 0x74,
        0x2E, 0xD3, 0x5B, 0x82, 0xD6, 0x32, 0x43, 0x9A,
        0x59, 0x56, 0x9C, 0x0C, 0x5D, 0x8F, 0x22, 0x74,
        0x62, 0x17, 0xA2, 0x33, 0x37, 0x09, 0x44, 0x12,
        0xF9, 0x40, 0xA2, 0x47, 0x06, 0x9E, 0xF7, 0xFD,
        0xBA, 0x5F, 0x68, 0x02, 0x91, 0x30, 0x84, 0x76,
        0x1F, 0x4D, 0xDA, 0x41, 0xC7, 0x84, 0x8A, 0x04,
        0xE5, 0x87, 0xDC, 0x87, 0xEB, 0xD9, 0xAE, 0x4C,
        0xDD, 0xDE, 0x84, 0xB8, 0x95, 0x42, 0x6B, 0xB8,
        0x03, 0x90, 0x06, 0x72, 0xA6, 0x67, 0xD6, 0xD6,
        0xD8, 0xF7, 0x52, 0x9E, 0x2F, 0x39, 0x1B, 0xDE,
        0x48, 0xD0, 0xD5, 0x1B, 0xE4, 0x75, 0xF8, 0x8E,
        0xFE, 0xBA, 0x6B, 0xFC, 0x35, 0x24, 0x07, 0x42,
        0x31, 0x07, 0x1C, 0x7C, 0x1E, 0xEF, 0x65, 0x15,
        0xF7, 0x87, 0x41, 0x1B, 0x7F, 0x02, 0x28, 0x32,
        0xA5, 0x52, 0xBD, 0xA3, 0xC4, 0xB6, 0x25, 0xFB,
        0x56, 0xB7, 0xE5, 0x96, 0xB4, 0x95, 0xBB, 0xCE,
        0x30, 0x59, 0xC9, 0x23, 0x4A, 0x2C, 0xFC, 0x1F,
        0x3E, 0xB8, 0x30, 0x6A, 0x0A, 0x37, 0x0C, 0x8B,
        0x2F, 0xC9, 0x9C, 0x6A, 0x92, 0x00, 0x22, 0x03,
        0xB3, 0xDF, 0xE2, 0xAE, 0x0D, 0xBA, 0x96, 0xF7,
        0x85, 0xC5, 0x82, 0x38, 0x07, 0x80, 0x64, 0xF4,
        0x86, 0xBC, 0xBD, 0xD8, 0x6E, 0xEC, 0x59, 0x07,
        0x5F, 0x64, 0xFC, 0xA6, 0x43, 0x1F, 0xC8, 0x7C,
        0x6A, 0xFF, 0x85, 0x8A, 0x17, 0x7E, 0x52, 0x22,
        0x76, 0x35, 0xB8, 0x6B, 0x13, 0xBC, 0x2F, 0x64,
        0x19, 0xC8, 0x22, 0x4C, 0x98, 0x0C, 0x06, 0xFB,
        0xB7, 0x8E, 0x05, 0x93, 0x42, 0xC2, 0x2C, 0x1D,
        0x00, 0x2B, 0xED, 0xF4, 0x0E, 0xF4, 0x43, 0x2C,
        0xDF, 0x5A, 0xCC, 0x12, 0x8C, 0xD1, 0xA6, 0xA9,
        0x07, 0x1F, 0xAD, 0xF0, 0xD4, 0x2C, 0xEB, 0xD7,
        0xD3, 0x3E, 0x2F, 0x8D, 0xAB, 0xBB, 0xCE, 0x58,
        0xCE, 0xC0, 0xDD, 0x64, 0xAB, 0x3F, 0xA4, 0x68,
        0x86, 0xDE, 0x10, 0x54, 0x4C, 0xC5, 0x80, 0xDA,
        0xDD, 0x91, 0xA6, 0x3E, 0xAF, 0x1A, 0x74, 0x6E,
        0x3C, 0x3C, 0x64, 0x54, 0xEA, 0x83, 0x02, 0x9B,
        0x82, 0x35, 0xD3, 0x3E, 0x17, 0x50, 0x5F, 0xB5,
        0x5C, 0x0F, 0xA3, 0xDB, 0xAD, 0x3F, 0x81, 0xDC,
        0x4E, 0xBF, 0x0C, 0xD0, 0x2D, 0x21, 0x99, 0x94,
        0x48, 0x8A, 0x3E, 0xD6, 0x6F, 0x08, 0x9B, 0x9A,
        0x3A, 0x60, 0xEF, 0x91, 0x5D, 0x86, 0xF1, 0xDE,
        0xD0, 0x98, 0x18, 0xAC, 0xB2, 0xBC, 0x8A, 0xA6,
        0x6F, 0x96, 0xF1, 0x04, 0x26, 0x8B, 0x9C, 0x67,
        0x96, 0x15, 0x35, 0x35, 0x88, 0x55, 0x84, 0xAF,
        0xBB, 0xB2, 0x2C, 0x4C, 0x28, 0xB9, 0xEE, 0x05,
        0xCB, 0x65, 0xAF, 0x59, 0x2E, 0xD1, 0x1C, 0x0B,
        0x28, 0xA3, 0x90, 0x6F, 0x78, 0x9D, 0xB4, 0x0D,
        0x43, 0x57, 0x83, 0x4E, 0x39, 0x87, 0x66, 0xB1,
        0x3F, 0x5B, 0xF4, 0x9D, 0xE7, 0x7A, 0xBA, 0x6D,
        0x54, 0x57, 0x2F, 0x0E, 0xA6, 0xAA, 0x3B, 0xB7,
        0xC3, 0x96, 0xB9, 0x47, 0x72, 0xCC, 0xF5, 0xBF,
        0x19, 0xD1, 0x18, 0xE0, 0x59, 0x95, 0x9C, 0x39,
        0x18, 0x41, 0x37, 0xBD, 0x37, 0x95, 0x56, 0xAF,
        0xBA, 0x9C, 0x9D, 0xF7, 0x03, 0x06, 0x4D, 0x76,
        0xB3, 0x87, 0x12, 0x69, 0x32, 0x95, 0x61, 0x89,
        0x0A, 0x9D, 0xEC, 0xD2, 0x2B, 0xF2, 0x47, 0xCB,
        0x37, 0x26, 0x3E, 0x51, 0xDA, 0x56, 0x5C, 0x39,
        0xA3, 0x27, 0xF0, 0xEE, 0xEE, 0x26, 0xF9, 0x9F,
        0x8F, 0xC3, 0xC8, 0x40, 0x92, 0xCE, 0xE2, 0xD8,
        0x76, 0xF3, 0x79, 0xD2, 0x4B, 0xD9, 0x51, 0xCA,
        0xCE, 0x11, 0x45, 0xBC, 0x9B, 0x28, 0x64, 0x05,
        0x14, 0x74, 0x56, 0x22, 0x0E, 0xAF, 0xF9, 0xB9,
        0x5F, 0xCA, 0x95, 0x74, 0xFB, 0x08, 0x45, 0x45,
        0x88, 0xE9, 0x39, 0x55, 0x2A, 0x48, 0x6F, 0xC0,
        0xA5, 0xF3, 0x02, 0x85, 0xA5, 0x0D, 0xF2, 0x98,
        0x52, 0x2A, 0x46, 0x19, 0x15, 0xD1, 0xCA, 0xEC,
        0x03, 0x69, 0x9B, 0xFE, 0x43, 0xC2, 0xCD, 0x1E,
        0xAC, 0xA7, 0x2F, 0xD4, 0xAC, 0x8C, 0x95, 0x88,
        0xCA, 0xE8, 0x87, 0x2E, 0x86, 0xEB, 0x21, 0x90,
        0xC2, 0xA8, 0x4F, 0xF5, 0x76, 0x34, 0xFE, 0xE1,
        0x91, 0xCE, 0x5B, 0xE2, 0x07, 0x25, 0x00, 0x17,
        0x09, 0x9D, 0x1F, 0x4B, 0x03, 0xEF, 0x5B, 0x9C,
        0x3C, 0x74, 0x9F, 0xD1, 0x05, 0xEB, 0x64, 0x96,
        0xD9, 0xA3, 0xC6, 0x26, 0x5A, 0xE5, 0x6C, 0x84,
        0x27, 0x92, 0x55, 0x6C, 0xB2, 0x04, 0x1C, 0xDD,
        0xA1, 0xE0, 0x77, 0xE9, 0xA2, 0x18, 0x86, 0x41,
        0x0F, 0x75, 0x09, 0x3F, 0x04, 0x93, 0xFC, 0x5B,
        0x57, 0xC3, 0x98, 0x04, 0xE8, 0xA0, 0x39, 0x92,
        0xB4, 0xD2, 0x09, 0xAA, 0xD8, 0xE6, 0xD0, 0xA0,
        0xBB, 0x5C, 0x3A, 0x63, 0xAE, 0xBC, 0xBA, 0xCC,
        0xC5, 0xEE, 0xF6, 0x90, 0xC7, 0x82, 0xFA, 0xC6,
        0x5A, 0x33, 0x71, 0x12, 0xE6, 0x1C, 0x57, 0xFA,
        0x38, 0x23, 0xE2, 0x6C, 0x14, 0x4B, 0xBC, 0x0E,
        0x09, 0x96, 0x9E, 0xC4, 0xA5, 0x91, 0x0E, 0xBF,
        0xCA, 0x05, 0xA6, 0xE1, 0x1E, 0xC6, 0xC3, 0x1B,
        0x69, 0x44, 0xCC, 0x8D, 0x7B, 0x1C, 0x63, 0xED,
        0x80, 0x63, 0x44, 0x01, 0xCB, 0xB0, 0x93, 0x4F,
        0x5D, 0xA8, 0x61, 0x5E, 0x73, 0x51, 0xA8, 0x29,
        0x61, 0x41, 0xB6, 0xBC, 0x7E, 0x00, 0x2C, 0xB8,
        0xDF, 0xBF, 0x8A, 0x5C, 0xD9, 0xC8, 0x4B, 0x31,
        0x95, 0x44, 0x86, 0x4C, 0xC1, 0xA6, 0xFF, 0x31,
        0x73, 0x9F, 0x1A, 0xB4, 0x02, 0x9E, 0x37, 0x09,
        0x97, 0x55, 0xD5, 0x39, 0xA1, 0x1A, 0x2E, 0x21,
        0x1F, 0x91, 0xE6, 0x91, 0x1A, 0xD1, 0x79, 0xC7,
        0xF6, 0x57, 0xC7, 0xC1, 0x73, 0xFB, 0xE4, 0x91,
        0x61, 0x38, 0xC0, 0x66, 0xF5, 0x05, 0x0D, 0xCE,
        0x99, 0x82, 0x78, 0x37, 0x63, 0x0D, 0x00, 0x04,
        0x4C, 0xCB, 0xFA, 0x32, 0x0B, 0xD0, 0xDE, 0x7C,
        0x34, 0x58, 0xCC, 0xCD, 0x2D, 0x27, 0xA2, 0x41,
        0x7A, 0x8D, 0x82, 0x3A, 0x0C, 0x0E, 0x0F, 0xE9,
        0x58, 0x90, 0x46, 0x44, 0x7A, 0xA0, 0xE1, 0x6B,
        0x21, 0x33, 0xCB, 0x73, 0xAE, 0x50, 0x0C, 0xBB,
        0xF3, 0x6C, 0x0B, 0xB7, 0xF8, 0x7D, 0xAD, 0x7C,
        0x11, 0xF9, 0x5A, 0x15, 0x1E, 0xB4, 0xBE, 0xFE,
        0x38, 0x65, 0x9B, 0xE6, 0xD1, 0x65, 0x73, 0x9D,
        0x76, 0x6F, 0x03, 0xF2, 0x0C, 0x71, 0x72, 0xC5,
        0xB5, 0x1C, 0x4F, 0xE1, 0xE5, 0x2E, 0xD1, 0x56,
        0xE2, 0xED, 0xAE, 0x40, 0x1E, 0x97, 0x23, 0x2C,
        0x58, 0x96, 0x8B, 0xA0, 0xE2, 0x45, 0xC6, 0x00,
        0xC9, 0xAC, 0x2A, 0xAF, 0x0B, 0x7E, 0xBA, 0x84,
        0xF1, 0xF3, 0x9D, 0x8E, 0x1E, 0xEE, 0xA8, 0xF5,
        0x91, 0x3F, 0xF9, 0x93, 0x70, 0x91, 0xCD, 0x09,
        0x84, 0xB2, 0x11, 0xFB, 0x58, 0x76, 0x70, 0x36,
        0x4F, 0xD9, 0x12, 0x57, 0xD1, 0xAA, 0x1B, 0xAE,
        0xE2, 0xE3, 0xA3, 0x7D, 0x6F, 0xA4, 0xE1, 0x4C,
        0x2D, 0xE9, 0x22, 0xEF, 0x9B, 0x80, 0x95, 0x32,
        0xAE, 0x2D, 0x35, 0xC9, 0xB9, 0x03, 0x0E, 0xD0,
        0x96, 0xF5, 0xE7, 0xEE, 0x42, 0xA8, 0x11, 0x84
};
#endif
static unsigned char symtestsbox7[] =
{
        0x65, 0x38, 0xA2, 0xA8, 0xA9, 0x04, 0x05, 0xB1,
        0xE8, 0xA3, 0xEF, 0x3F, 0x86, 0xC1, 0xC7, 0xCE,
        0xE0, 0xA6, 0x32, 0x58, 0xCC, 0x2C, 0xD0, 0x5D,
        0x9C, 0x2D, 0xE5, 0x1C, 0xEC, 0x17, 0xFA, 0x4E,
        0xB0, 0x27, 0xD5, 0xE1, 0x5C, 0xAE, 0x10, 0x28,
        0x5B, 0x35, 0x41, 0xDE, 0x31, 0xB9, 0xDA, 0xE2,
        0x98, 0x74, 0x99, 0xAC, 0xDF, 0x75, 0x9E, 0xEB,
        0xA7, 0x84, 0x11, 0x5E, 0xCB, 0xE6, 0x02, 0xC0,
        0xBF, 0x45, 0x6F, 0xA5, 0x20, 0x0E, 0xCA, 0xD9,
        0x2F, 0xF7, 0x3C, 0xCF, 0x0D, 0x00, 0x60, 0xAA,
        0xD2, 0xC8, 0x72, 0x26, 0x54, 0x19, 0x94, 0x70,
        0xBB, 0x83, 0x62, 0x9A, 0x42, 0x4D, 0x5F, 0x01,
        0x92, 0xDC, 0xCD, 0xE3, 0xA4, 0x0C, 0xBC, 0x57,
        0x73, 0x07, 0x85, 0x47, 0x8E, 0x16, 0xE4, 0x0B,
        0x7E, 0x96, 0xED, 0x2E, 0x23, 0x8B, 0xD8, 0x8A,
        0x12, 0x7D, 0xD6, 0xF2, 0x8F, 0x1B, 0xF6, 0xFC,
        0x3A, 0x33, 0x4F, 0x77, 0x79, 0x4C, 0x91, 0xBD,
        0x52, 0x36, 0xC9, 0xD1, 0x13, 0x6D, 0xA1, 0x66,
        0x9B, 0xF1, 0x06, 0xB3, 0xF8, 0xFB, 0x48, 0xF0,
        0xC5, 0x46, 0x90, 0x63, 0xB8, 0x1F, 0xB2, 0x8C,
        0xBA, 0xD7, 0x69, 0x34, 0x29, 0xE9, 0xEE, 0xC6,
        0x81, 0xAF, 0x3B, 0x30, 0xE7, 0x6E, 0x71, 0xF9,
        0x0F, 0xFD, 0x1A, 0x39, 0xA0, 0xF3, 0x18, 0x1E,
        0x56, 0x43, 0x55, 0x53, 0x40, 0x03, 0xD3, 0xC4,
        0x9D, 0x88, 0xDD, 0x2B, 0x4B, 0x67, 0x93, 0x59,
        0x1D, 0x7C, 0x7F, 0x51, 0xF4, 0x44, 0x61, 0x21,
        0xDB, 0x68, 0xAB, 0x7A, 0x78, 0x14, 0x3E, 0xFE,
        0xBE, 0xAD, 0x7B, 0x89, 0x5A, 0x37, 0x15, 0xB4,
        0xF5, 0xB5, 0x09, 0xD4, 0x3D, 0x82, 0x76, 0xFF,
        0x6C, 0x25, 0x50, 0xB6, 0x80, 0x9F, 0xC2, 0x97,
        0x49, 0x22, 0xEA, 0x6B, 0xB7, 0x08, 0x95, 0x87,
        0x4A, 0x24, 0x64, 0xC3, 0x0A, 0x8D, 0x2A, 0x6A,
        0x5D, 0xC8, 0x4D, 0x00
};

static SYMTESTTYPE symTest[] =
{
        {       /* AES CTR Test */
                DPD_AES_CTR_ENC,
                16,    /* key len */
                symtestkey1,   /* key */
                symtestctr1,   /* Initial Counter */
                sizeof(symtestctr1),
                FSL_CTR_MOD_72,        /* ctr mod */
                48,    /* data len */
                symtestinput1,
                FSL_RETURN_OK_S,
                symtestoutput1,
                sizeof(symtestoutput1),
                NULL,
                0,
                "AES CTR Encrypt Test"
        },
#if 0 // a wrong data
        {       /* AES ECB Test */
                DPD_AES_ECB_ENC,
                16,    /* key len */
                symtestkey2,   /* key */
                NULL,  /* iv */
                0,
                0,     /* ctr mod */
                1024,  /* data len */
                symtestinput2,
                FSL_RETURN_OK_S,
                symtestoutput2,
                sizeof(symtestoutput2),
                NULL,
                0,
                "AES ECB Encrypt Test"
        },
#endif

        {       /* AES CBC Test */
                DPD_AES_CBC_DEC,
                16,    /* key len */
                symtestkey3,   /* key */
                NULL,  /* iv */
                0,
                0,     /* ctr mod */
                64,    /* data len */
                symtestinput3,
                FSL_RETURN_OK_S,
                symtestoutput3,
                sizeof(symtestoutput3),
                NULL,
                0,
                "AES CBC Decrypt Test"
        },

        {       /* DES ECB Test */
                DPD_DES_ECB_DEC,
                8,     /* key len */
                symtestkey4,   /* key */
                NULL,  /* iv */
                0,
                0,     /* ctr mod */
                192,   /* data len */
                symtestinput4,
                FSL_RETURN_OK_S,
                symtestoutput4,
                sizeof(symtestoutput4),
                NULL,
                0,
                "DES ECB Decrypt Test"
        },

        {       /* DES Key Parity Test */
                DPD_DES_KEY_PARITY,
                8,     /* key len */
                "abcdefgh",    /* key */
                NULL,  /* iv */
                0,
                0,     /* ctr mod */
                192,   /* data len */
                symtestinput4,
                FSL_RETURN_BAD_KEY_PARITY_S,
                symtestoutput4,
                sizeof(symtestoutput4),
                NULL,
                0,
                "DES ECB Key Parity Test"
        },

        {       /* 3DES ECB Test */
                DPD_3DES_ECB_DEC,
                16,    /* key len */
                symtestkey5,   /* key */
                NULL,  /* iv */
                0,
                0,     /* ctr mod */
                64,    /* data len */
                symtestinput5,
                FSL_RETURN_OK_S,
                symtestoutput5,
                sizeof(symtestoutput5),
                NULL,
                0,
                "3DES ECB Decrypt Test"
        },

        {       /* 3DES Bad Key Length Test */
                DPD_3DES_ECB_DEC,
                6,     /* key len */
                symtestkey5,   /* key */
                NULL,  /* iv */
                0,
                0,     /* ctr mod */
                64,    /* data len */
                symtestinput5,
                FSL_RETURN_BAD_KEY_LENGTH_S,
                symtestoutput5,
                sizeof(symtestoutput5),
                NULL,
                0,
                "3DES Bad Key Length Test"
        },

        {       /* 3DES Bad Data Length Test */
                DPD_3DES_ECB_DEC,
                16,    /* key len */
                symtestkey5,   /* key */
                NULL,  /* iv */
                0,
                0,     /* ctr mod */
                63,    /* data len */
                symtestinput5,
                FSL_RETURN_BAD_DATA_LENGTH_S,
                symtestoutput5,
                sizeof(symtestoutput5),
                NULL,
                0,
                "3DES Bad Data Length Test"
        },
#if 0 // a wrong data
        {       /* ARC4 'encrypt' */
                DPD_ARC4_ENC_KEY,
                4,     /* key len */
                symtestkey6,   /* key */
                NULL,  /* iv */
                0,
                0,     /* ctr mod */
                92,    /* data len */
                symtestinput6,
                FSL_RETURN_OK_S,
                symtestoutput6,
                sizeof(symtestoutput6),
                NULL,
                0,
                "ARC4 from Key Test"
        },
#endif

        {       /* ARC4 'encrypt' */
                DPD_ARC4_ENC_KEY,
                0,     /* key len */
                NULL,  /* key */
                symtestsbox7,  /* sbox+ptrs */
                sizeof(symtestsbox7),
                0,     /* ctr mod */
                46,    /* data len */
                symtestinput7,
                FSL_RETURN_OK_S,
                symtestoutput7,
                sizeof(symtestoutput7),
                NULL,
                0,
                "ARC4 from Sbox Test"
        },

        {       /* ARC4 'encrypt' */
                DPD_ARC4_ENC_KEY,
                4,     /* key len */
                symtestkey8,   /* key */
                NULL,  /* iv */
                0,
                0,     /* ctr mod */
                92,    /* data len */
                symtestinput8,
                FSL_RETURN_OK_S,
                symtestoutput8,
                sizeof(symtestoutput8),
                symtestsboxout8,
                sizeof(symtestsboxout8),
                "ARC4 from Key Test & check SBox"
        }
};

/*==================================================================================================
                                            CONSTANTS
==================================================================================================*/
static const unsigned NUM_HASHTESTS = sizeof(hashTest) / sizeof(HASHTESTTYPE);
static const unsigned NUM_SYMTESTS = sizeof(symTest) / sizeof(SYMTESTTYPE);
static const unsigned NUM_HMACTESTS = sizeof(hmactests) / sizeof(HMACTESTTYPE);

/*==================================================================================================
                                        LOCAL VARIABLES
==================================================================================================*/
/*================================ For sahara_callback_test ======================================*/
static unsigned char callback_complete;
static unsigned callback_passed;
static unsigned callback_test_count;
//struct rand_test tests[RESULTS_SIZE];
//static fsl_shw_result_t results[RESULTS_SIZE];  /* place to put results */
struct rand_test * tests;
fsl_shw_result_t * results;

/*==================================================================================================
                                      FUNCTION PROTOTYPES
==================================================================================================*/
int compare_result(const uint8_t * model, const uint8_t * result,
                   uint32_t length, const uint8_t * name);

/*==================================================================================================
                                        LOCAL FUNCTIONS
==================================================================================================*/
static int sahara_test_open(struct inode *inode, struct file *filp)
{
        return 0;
}

static ssize_t sahara_test_read(struct file *file, char *buf, size_t count, loff_t * ppos)
{
        return 0;
}

static ssize_t sahara_test_write(struct file *filp, const char *buf, size_t count, loff_t * ppos)
{
        return 0;
}

static int sahara_test_release(struct inode *inode, struct file *filp)
{
        return 0;
}

/*================================================================================================*/
/*===== fsl_error_string =====*/
/**
@brief  Error code interpreteur

@param  code The error code to interpret. @return The associated English interpretation of the code.

@return Return a (non-allocated) string containing an interpretation of an FSL SHW API error code.
*/
/*================================================================================================*/
char   *fsl_error_string(fsl_shw_return_t code)
{
        char   *str;

        switch (code)
        {
        case FSL_RETURN_OK_S:
                str = "No error";
                break;
        case FSL_RETURN_ERROR_S:
                str = "Error";
                break;
        case FSL_RETURN_NO_RESOURCE_S:
                str = "No resource";
                break;
        case FSL_RETURN_BAD_ALGORITHM_S:
                str = "Bad algorithm";
                break;
        case FSL_RETURN_BAD_MODE_S:
                str = "Bad mode";
                break;
        case FSL_RETURN_BAD_FLAG_S:
                str = "Bad flag";
                break;
        case FSL_RETURN_BAD_KEY_LENGTH_S:
                str = "Bad key length";
                break;
        case FSL_RETURN_BAD_KEY_PARITY_S:
                str = "Bad key parity";
                break;
        case FSL_RETURN_BAD_DATA_LENGTH_S:
                str = "Bad data length";
                break;
        case FSL_RETURN_AUTH_FAILED_S:
                str = "Authentication failed";
                break;
        case FSL_RETURN_MEMORY_ERROR_S:
                str = "Memory error";
                break;
        case FSL_RETURN_INTERNAL_ERROR_S:
                str = "Internal error";
                break;

        default:
                str = "unknown value";
                break;
        }
        return str;
}

/*================================================================================================*/
/*===== VT_register_user =====*/
/*================================================================================================*/
int VT_register_user(fsl_shw_uco_t * my_ctx)
{
        fsl_shw_return_t code;
        int     VT_rv = 0;

        /* Set Results Pool size to 10 */
        fsl_shw_uco_init(my_ctx, 10);

        /* Tell hw API that we are here */
        code = fsl_shw_register_user(my_ctx);

        if (FSL_RETURN_OK_S != code)
        {
                VT_rv = 1;
                TRACEMSG("VT_sahara_test_setup: fsl_shw_register_user() failed with error: %s\n",
                         fsl_error_string(code));
        }
        else
        {
                TRACEMSG("VT_sahara_test_setup: User registry: %s\n", fsl_error_string(code));

                /* Set my private value in ctx */
                fsl_shw_uco_set_reference(my_ctx, SET_REFERENCE);
                fsl_shw_uco_set_flags(my_ctx, FSL_UCO_BLOCKING_MODE);
                TRACEMSG("VT_sahara_test_setup: Ready to rock\n");
        }
        return VT_rv;
}

/*================================================================================================*/
/*===== VT_deregister_user =====*/
/*================================================================================================*/
int VT_deregister_user(fsl_shw_uco_t * my_ctx)
{
        fsl_shw_return_t code;
        int     VT_rv = 0;

        code = fsl_shw_deregister_user(my_ctx);
        if (FSL_RETURN_OK_S != code)
        {
                VT_rv = 1;
                TRACEMSG
                    ("VT_sahara_test_cleanup: fsl_shw_deregister_user() failed with error: %s\n",
                     fsl_error_string(code));
        }
        return VT_rv;
}

/*==================================================================================================*/
static int check_one(uint8_t random[RAND_SIZE])
{
        if ((random[0] == 0) && (random[1] == 0) &&
            (random[2] == 0) && (random[3] == 0) &&
            (random[4] == 0) && (random[5] == 0) &&
            (random[RAND_SIZE / 3] == 0) && (random[RAND_SIZE / 2] == 0) &&
            (random[RAND_SIZE / 2] == 0) && (random[RAND_SIZE - 1] == 0))
        {
                return 1;       /* failed */
        }
        else
        {
                return 0;       /* passed */
        }
}

/*==================================================================================================*/
/*===== test_callback1 =====*/
/**
@param  uco User Context Object

@return Nothing
*/
/*==================================================================================================*/
static void test_callback1(fsl_shw_uco_t * uco)
{
        fsl_shw_return_t code;  /* value returned from API call */
        unsigned int loop;      /* number of iterations in verify loop */
        unsigned passed = 1;    /* boolean */
        unsigned  * actual = kmalloc(sizeof(unsigned), GFP_KERNEL);  /* number of results actually received */

        /* Request results */
        code = fsl_shw_get_results(uco, RESULTS_SIZE, results, actual);

        /* check that results request worked */
        if (code != FSL_RETURN_OK_S)
        {
                passed = 0;
                TRACEMSG("   fsl_shw_get_results() returned %s\n", fsl_error_string(code));
        }
        else
        {
                TRACEMSG("   %d results expected", RESULTS_SIZE);
                /* report how many results received */
                if (actual > 0)
                {
                        TRACEMSG(", %d received\n", *actual);
                }
                else
                {
                        TRACEMSG(", none received\n");
                        passed = 0;
                }
        }

        if (passed)
        {
                /* loop over each results received */
                for (loop = 0; loop < *actual; ++loop)
                {
                        /* check user reference value is good */
                        unsigned testno = fsl_shw_ro_get_reference(results + loop);

                        if ((testno >= RESULTS_SIZE) || (!tests[testno].initiated)
                            || tests[testno].result_received)
                        {
                                passed = 0;
                                if (testno >= RESULTS_SIZE)
                                {
                                        TRACEMSG("   reference our of range: %d >= %d\n",
                                                 testno, RESULTS_SIZE);
                                }
                                if (!tests[testno].initiated)
                                {
                                        TRACEMSG("   test case %d never initiated\n", testno);
                                }
                                if (tests[testno].result_received)
                                {
                                        TRACEMSG("   results alreedy received for test case %d\n",
                                                 testno);
                                }
                        }
                        else
                        {
                                /* mark that a test result was received for this ref */
                                tests[testno].result_received = 1;
                                /* verify that result is okay for this ref */
                                if ((code =
                                     fsl_shw_ro_get_status(results + loop)) != FSL_RETURN_OK_S)
                                {
                                        TRACEMSG("   result %d(%d) returned error %s\n",
                                                 testno, loop, fsl_error_string(code));
                                        passed = 0;
                                }
                                /* check that a random number was likely recevied */
                                if (check_one(tests[testno].random))
                                {
                                        TRACEMSG("   result %d values are not good\n", testno);
                                        passed = 0;
                                }
                        }
                }       /* for each result received */
                /* check that every request was serviced succesfully */
                for (loop = 0; loop < RESULTS_SIZE; loop++)
                {
                        if (!tests[loop].result_received)
                        {
                                TRACEMSG("   result never received for test %d\n", loop);
                                passed = 0;
                        }
                }
        }

        /* pass back if passed or failed */
        if (passed == 0)
        {
                /* callback detected an error, the test has failed */
                callback_complete = 2;
        }
        else
        {
                /* no errors detected, test has passed */
                callback_complete = 1;
        }
        kfree(actual);
}

/*==================================================================================================*/
/*===== test_callback2 =====*/
/**
@param  uco User Context Object

@return Nothing
*/
/*==================================================================================================*/
static void test_callback2(fsl_shw_uco_t * uco)
{
        fsl_shw_return_t code;  /* value returned from API call */
        unsigned int loop;      /* number of iterations in verify loop */
        unsigned  * actual = kmalloc(sizeof(unsigned), GFP_KERNEL);  /* number of results actually received */

        /* Request results */
        code = fsl_shw_get_results(uco, RESULTS_SIZE, results, actual);
        ++callback_test_count;

        /* check that results request worked */
        if (code != FSL_RETURN_OK_S)
        {
                callback_passed = 0;
                TRACEMSG("   fsl_shw_get_results() returned %s\n", fsl_error_string(code));
        }

        if (code == FSL_RETURN_OK_S)
        {
                /* loop over each result received */
                for (loop = 0; loop < *actual; ++loop)
                {
                        unsigned testno = fsl_shw_ro_get_reference(results + loop);

                        if ((testno >= RESULTS_SIZE) || (!tests[testno].initiated) ||
                            tests[testno].result_received)
                        {
                                callback_passed = 0;
                                if (testno >= RESULTS_SIZE)
                                {
                                        TRACEMSG("   reference our of range: %d >= %d\n",
                                                 testno, RESULTS_SIZE);
                                }
                                if (!tests[testno].initiated)
                                {
                                        TRACEMSG("   test case %d never initiated\n", testno);
                                }
                                if (tests[testno].result_received)
                                {
                                        TRACEMSG("   results alreedy received for test case %d\n",
                                                 testno);
                                }
                        }
                        else
                        {
                                /* mark that a test result was received for this ref */
                                tests[testno].result_received = 1;
                                /* verify that result is okay for this ref */
                                if ((code =
                                     fsl_shw_ro_get_status(results + loop)) != FSL_RETURN_OK_S)
                                {
                                        TRACEMSG("   result %d(%d) returned error %s\n",
                                                 testno, loop, fsl_error_string(code));
                                        callback_passed = 0;
                                }
                                /* check that a random number was likely recevied */
                                if (check_one(tests[testno].random))
                                {
                                        TRACEMSG("   result %d values are not good\n", testno);
                                        callback_passed = 0;
                                }
                        }
                }       /* for each result received */
        }
        if (callback_test_count == RESULTS_SIZE)
        {
                if (callback_passed)
                {
                        callback_complete = 1;
                }
                else
                {
                        callback_complete = 2;
                        callback_passed = 1;    /* reset for next test run */
                }
        }

        kfree(actual);
}

/*==================================================================================================*/
/*===== VT_run_callback =====*/
/**
@brief  Tests the get results operation

@param  my_ctx User context to use

@return On success - return 0 On failure - return -1
*/
/*==================================================================================================*/
int VT_run_callback(fsl_shw_uco_t * my_ctx)
{
        fsl_shw_return_t code;  /* value returned from API call */
        unsigned int loop;      /* number of iterations in verify loop */
        unsigned passed = 1;    /* boolean */
        unsigned total; /* how many results actually found */
        int     VT_rv = 0;

        TRACEMSG("Callback Tests\n");

        /* clears random data to zeros; sets initiated and result_received FALSE */
        tests = kmalloc(sizeof(struct rand_test)*RESULTS_SIZE, GFP_KERNEL);
        results = kmalloc(sizeof(fsl_shw_result_t)*RESULTS_SIZE, GFP_KERNEL);
        memset(tests, 0, sizeof(struct rand_test)*RESULTS_SIZE);
        memset(results, 0, sizeof(fsl_shw_result_t)*RESULTS_SIZE);

        callback_complete = 0;
        fsl_shw_uco_clear_flags(my_ctx, FSL_UCO_BLOCKING_MODE);


        /********************************************************************
        * send multiple requests in non-blocking mode. request a callback
        * only on the last service request. The callback should be able to
        * obtain all of the results in a single "get results" call
        *******************************************************************/
        TRACEMSG("   Callback Test 1: Get block of results\n");

        /* This loop will have to be fixed when pool size is enforced. */
        for (loop = 0; loop < RESULTS_SIZE; loop++)
        {
                /* on last time through loop, request callback be performed */
                if (loop == (RESULTS_SIZE - 1))
                {
                        fsl_shw_uco_set_callback(my_ctx, test_callback1);
                        fsl_shw_uco_set_flags(my_ctx, FSL_UCO_CALLBACK_MODE);
                }

                /* use loop value as user reference */
                fsl_shw_uco_set_reference(my_ctx, loop);
                /* mark as being intialized */
                tests[loop].initiated = 1;

                /* perform a descriptor chain generating function */
                code = fsl_shw_get_random(my_ctx, sizeof(uint8_t)*RAND_SIZE, tests[loop].random);
                if (code != FSL_RETURN_OK_S)
                {
                        /* mark test as failed */
                        passed = 0;
                        /* un-initialize the entry here (must be initialized before the * interrupt *
                        * goes off, so it is initilzed before the descriptor * chain generating *
                        * function above */
                        tests[loop].initiated = 0;
                        TRACEMSG("   fsl_shw_random(), call %d returned %s\n", loop,
                                 fsl_error_string(code));
                }
        }

        if (passed)
        {
                uint32_t loop_count = 0;

                /* stay in test until we get pass/fail feedback from callback */
                while ((!callback_complete) && (++loop_count < 100))
                {
                        usleep(10);
                }

                /* test status as set by the callback routine */
                if (callback_complete != 1)
                {
                        if (callback_complete == 0)
                        {
                                TRACEMSG("   Callback never called\n");
                        }
                        passed = 0;
                }
        }
        if (passed)
        {
                TRACEMSG("Test 1 passed\n");
        }
        else
        {
                TRACEMSG("Test 1 failed\n");
                VT_rv = -1;
        }

        /***************************************************************
        * send a callback request along with each service request
        **************************************************************/
        if (!passed)
        {
                TRACEMSG("   Callback Test 2 not performed\n");
        }
        else
        {
                TRACEMSG("   Callback Test 2: callback on each result\n");

                /* clear results area again */
                memset(tests, 0, sizeof(struct rand_test)*RESULTS_SIZE);
                memset(results, 0, sizeof(fsl_shw_result_t)*RESULTS_SIZE);
                callback_complete = 0;

                fsl_shw_uco_set_callback(my_ctx, test_callback2);

                /* now set up the callback variables */
                callback_passed = 1;
                callback_test_count = 0;

                /* This loop will have to be fixed when pool size is enforced. */
                for (loop = 0; loop < RESULTS_SIZE; loop++)
                {
                        fsl_shw_uco_set_reference(my_ctx, loop);
                        tests[loop].initiated = 1;

                        code =
                            fsl_shw_get_random(my_ctx, sizeof(uint8_t)*RAND_SIZE,
                                               tests[loop].random);

                        if (code != FSL_RETURN_OK_S)
                        {
                                passed = 0;
                                tests[loop].initiated = 0;
                                TRACEMSG("   fsl_shw_random(), call %d returned %s\n", loop,
                                         fsl_error_string(code));
                        }
                }

                if (passed)
                {
                        total = 0;
                        /* stay in test until we get pass/fail feedback */
                        while ((!callback_complete) && (++total != 0x64))
                        {
                                usleep(10);
                        }

                        /* test status as set by the callback routine */
                        if (callback_complete != 1)
                        {
                                if (callback_complete == 0)
                                {
                                        TRACEMSG("   Callback never called\n");
                                }
                                passed = 0;
                        }

                        total = RESULTS_SIZE;

                        /* check that every request was serviced succesfully */
                        for (loop = 0; loop < RESULTS_SIZE; ++loop)
                        {
                                if (!tests[loop].result_received)
                                {
                                        TRACEMSG("   result never received for test %d\n", loop);
                                        passed = 0;
                                        --total;
                                }
                        }
                        TRACEMSG("   %d results expected, %d received\n", RESULTS_SIZE, total);
                }
                else
                {
                        /* service request failure */
                        passed = 0;
                }

                if (passed)
                {
                        TRACEMSG("Test 2 passed\n");
                }
                else
                {
                        TRACEMSG("Test 2 failed\n");
                        VT_rv = -1;
                }
        }

        /* set uco back back to its default */
        fsl_shw_uco_set_flags(my_ctx, FSL_UCO_BLOCKING_MODE);
        fsl_shw_uco_clear_flags(my_ctx, FSL_UCO_CALLBACK_MODE);
        fsl_shw_uco_set_callback(my_ctx, NULL);

        kfree(tests);
        kfree(results);

        return VT_rv;
}


/*==================================================================================================*/
/*===== run_hash_test =====*/
/**
@brief  All hashing tests

@param  user_ctx User context

@param  alg Cryptographic algorithm

@param  pad The HASH flag finalize

@param  test Type of test

@return On success - return 1 On failure - return 0
*/
/*==================================================================================================*/
static int run_hash_test(fsl_shw_uco_t * user_ctx, fsl_shw_hash_alg_t alg,
                         unsigned pad, HASHTESTTYPE * test1)
{

        int     failed = 1;
        fsl_shw_return_t code;
        fsl_shw_hco_t * hash_ctx = (fsl_shw_hco_t*)kmalloc(sizeof(fsl_shw_hco_t), GFP_KERNEL);
        uint8_t * digest = (uint8_t*)kmalloc(256*sizeof(uint8_t), GFP_KERNEL);

        HASHTESTTYPE * test = kmalloc(sizeof(HASHTESTTYPE), GFP_KERNEL);
        memcpy(test,test1,sizeof(HASHTESTTYPE));
        test->pPlaintext = kmalloc(test->plainTextLen,GFP_KERNEL);
        memcpy(test->pPlaintext,test1->pPlaintext,test->plainTextLen);
        test->pDigest = kmalloc(test->digestLen,GFP_KERNEL);
        memcpy(test->pDigest,test1->pDigest,test->digestLen);

        TRACEMSG("%s\n", test->testDesc);
        memset(hash_ctx, 0x00, sizeof(hash_ctx));

        fsl_shw_hco_init(hash_ctx, alg);
        fsl_shw_hco_set_flags(hash_ctx, FSL_HASH_FLAGS_INIT);

        memset(digest, 0xff, test->digestLen);      /* clear out result */

        /*
        * How to run this and other multi-step tests when user ctx
        * is flagged for non-blocking??
        */
        if (test->plainTextLen > PARTIAL_SIZE)
        {

                fsl_shw_hco_set_flags(hash_ctx, FSL_HASH_FLAGS_SAVE);

                code = fsl_shw_hash(user_ctx, hash_ctx, test->pPlaintext, PARTIAL_SIZE, NULL, 0);

                if (code != FSL_RETURN_OK_S)
                {
                        TRACEMSG("Test failed; first fsl_shw_hash() return %s\n",
                                 fsl_error_string(code));
                }
                else
                {
                        fsl_shw_hco_t * final_ctx = (fsl_shw_hco_t*)kmalloc(sizeof(fsl_shw_hco_t), GFP_KERNEL);
                        uint32_t * msglen = (uint32_t*)kmalloc(sizeof(uint32_t),GFP_KERNEL);
                        uint8_t * context = (uint8_t*)kmalloc(32*sizeof(uint8_t),GFP_KERNEL);    /* enough for largest ctx */

                        /* Set up new context to do rest of the hash */
                        fsl_shw_hco_init(final_ctx, alg);
                        fsl_shw_hco_set_flags(final_ctx, FSL_HASH_FLAGS_LOAD);
                        /* Perhaps turn on SAVE as well, and extract digest/len from context for * *
                        * * * comparison? */

                        /* Move hash context from one object to the other. */
                        fsl_shw_hco_get_digest(hash_ctx, context, 32, msglen);
                        fsl_shw_hco_set_digest(final_ctx, context, *msglen);

                        if (pad)
                        {
                                fsl_shw_hco_set_flags(final_ctx, FSL_HASH_FLAGS_FINALIZE);
                        }

                        code = fsl_shw_hash(user_ctx, final_ctx,
                                            test->pPlaintext + PARTIAL_SIZE,
                                            test->plainTextLen - PARTIAL_SIZE, digest,
                                            test->digestLen);
                        kfree(final_ctx);
                        kfree(msglen);
                        kfree(context);
                }
        }
        else
        {
                /* do one-shot hash */
                if (pad)
                {
                        fsl_shw_hco_set_flags(hash_ctx, FSL_HASH_FLAGS_FINALIZE);
                }
                code = fsl_shw_hash(user_ctx, hash_ctx, test->pPlaintext,
                                    test->plainTextLen, digest, test->digestLen);
        }
        if (code != FSL_RETURN_OK_S)
        {
                TRACEMSG("Test failed; fsl_shw_hash() return  %s\n", fsl_error_string(code));
        }
        else
        {
                if (!compare_result(test->pDigest, digest, test->digestLen, "digest"))
                {
                        failed = 0;
                        TRACEMSG("%s: Passed\n", test->testDesc);
                }
                else
                {
                        TRACEMSG("%s: Failed\n", test->testDesc);
                }
        }
        kfree(digest);
        kfree(hash_ctx);
        kfree(test->pPlaintext);
        kfree(test->pDigest);
        kfree(test);

        return failed;
}

/*==================================================================================================*/
/*===== VT_run_hash =====*/
/**
@brief  Test code to perform Cryptographic Hashing

@param  my_ctx User context to use

@return On success - return 0 On failure - return -1
*/
/*==================================================================================================*/
int VT_run_hash(fsl_shw_uco_t * my_ctx)
{
        unsigned testno;
        int     VT_rv = 0;
        unsigned passed_count = 0;
        unsigned failed_count = 0;

        TRACEMSG("canned test count: %d\n", NUM_HASHTESTS);

        for (testno = 0; testno < NUM_HASHTESTS; testno++)
        {
                int     passed;

                switch (hashTest[testno].opId)
                {
                case DPD_MD5_LDCTX_IDGS_HASH_PAD_ULCTX:
                case DPD_MD5_LDCTX_IDGS_HASH_PAD_ULCTX_CMP:
                        passed = run_hash_test(my_ctx, FSL_HASH_ALG_MD5, 1, hashTest + testno);
                        break;
                case DPD_MD5_LDCTX_IDGS_HASH_ULCTX:
                case DPD_MD5_LDCTX_IDGS_HASH_ULCTX_CMP:
                        passed = run_hash_test(my_ctx, FSL_HASH_ALG_MD5, 0, hashTest + testno);
                        break;
                case DPD_SHA_LDCTX_IDGS_HASH_PAD_ULCTX:
                case DPD_SHA_LDCTX_IDGS_HASH_PAD_ULCTX_CMP:
                        passed = run_hash_test(my_ctx, FSL_HASH_ALG_SHA1, 1, hashTest + testno);
                        break;
                case DPD_SHA_LDCTX_IDGS_HASH_ULCTX:
                case DPD_SHA_LDCTX_IDGS_HASH_ULCTX_CMP:
                        passed = run_hash_test(my_ctx, FSL_HASH_ALG_SHA1, 0, hashTest + testno);
                        break;
                case DPD_SHA224_LDCTX_IDGS_HASH_PAD_ULCTX:
                case DPD_SHA224_LDCTX_IDGS_HASH_PAD_ULCTX_CMP:
                        passed = run_hash_test(my_ctx, FSL_HASH_ALG_SHA224, 1, hashTest + testno);
                        break;
                case DPD_SHA224_LDCTX_IDGS_HASH_ULCTX:
                case DPD_SHA224_LDCTX_IDGS_HASH_ULCTX_CMP:
                        passed = run_hash_test(my_ctx, FSL_HASH_ALG_SHA224, 0, hashTest + testno);
                        break;
                case DPD_SHA256_LDCTX_IDGS_HASH_PAD_ULCTX:
                case DPD_SHA256_LDCTX_IDGS_HASH_PAD_ULCTX_CMP:
                        passed = run_hash_test(my_ctx, FSL_HASH_ALG_SHA256, 1, hashTest + testno);
                        break;
                case DPD_SHA256_LDCTX_IDGS_HASH_ULCTX:
                case DPD_SHA256_LDCTX_IDGS_HASH_ULCTX_CMP:
                        passed = run_hash_test(my_ctx, FSL_HASH_ALG_SHA256, 0, hashTest + testno);
                        break;
                default:
                        TRACEMSG("Unknown test type: 0x%lx\n", hashTest[testno].opId);
                        continue;
                }
                if (passed == 0)
                {
                        passed_count++;
                }
                else
                {
                        failed_count++;
                }
        }

        TRACEMSG("Hash: %d tests passed, %d tests failed\n", passed_count, failed_count);
        if (failed_count != 0)
        {
                VT_rv = -1;
        }

        return VT_rv;
}

/*==================================================================================================*/
/*===== run_hmac1_test =====*/
/**
@brief  All HMAC tests

@param  user_ctx User context

@param  alg Hashing algorithm

@param  test Type of test

@return On success - return 1 On failure - return 0
*/
/*==================================================================================================*/
static int run_hmac1_test(fsl_shw_uco_t * my_ctx, fsl_shw_hash_alg_t algorithm,
                          const HMACTESTTYPE * test1)
{
        unsigned char * hmac = kmalloc(32*sizeof(unsigned char), GFP_KERNEL); /* The completed MAC - large enough for anything */
        fsl_shw_hmco_t * hmac_ctx = kmalloc(sizeof(fsl_shw_hmco_t), GFP_KERNEL);
        fsl_shw_sko_t * key_info = kmalloc(sizeof(fsl_shw_sko_t), GFP_KERNEL);
        fsl_shw_return_t code;
        unsigned passed = 0;

        HMACTESTTYPE * test = kmalloc(sizeof(HMACTESTTYPE), GFP_KERNEL);
        memcpy(test,test1,sizeof(HMACTESTTYPE));
        test->key = kmalloc(sizeof(unsigned char)*test1->key_length, GFP_KERNEL);
        memcpy(test->key,test1->key,sizeof(unsigned char)*test1->key_length);
        test->message = kmalloc(sizeof(unsigned char)*test1->message_length, GFP_KERNEL);
        memcpy(test->message,test1->message, sizeof(unsigned char)*test1->message_length);
        test->digest = kmalloc(sizeof(unsigned char)*test1->digest_length, GFP_KERNEL);
        memcpy(test->digest,test1->digest,sizeof(unsigned char)*test1->digest_length);
        if(test1->context_length)
        {
                test->ipad = kmalloc(sizeof(char)*test1->context_length, GFP_KERNEL);
                test->opad = kmalloc(sizeof(char)*test1->context_length, GFP_KERNEL);
                memcpy(test->ipad,test1->ipad,sizeof(char)*test1->context_length);
                memcpy(test->opad,test1->opad,sizeof(char)*test1->context_length);
        }

        memset(key_info, 0x8a, sizeof(fsl_shw_sko_t));
        memset(hmac_ctx, 0x4f, sizeof(fsl_shw_hmco_t));


        TRACEMSG("Starting %s:\n", test->testDesc);

        /* Initialize the objects */
        fsl_shw_hmco_init(hmac_ctx, algorithm);

        fsl_shw_sko_init(key_info, FSL_KEY_ALG_HMAC);

        /* Store key in the key object */
        fsl_shw_sko_set_key(key_info, test->key, test->key_length);

        /* Do pre-compute or not... */
        if (test->context_length != 0)
        {
                /* Calculate the IPAD and OPAD from the key */
                code = fsl_shw_hmac_precompute(my_ctx, key_info, hmac_ctx);
                if (code != FSL_RETURN_OK_S)
                {
                        TRACEMSG("Test failed: fsl_shw_hmac_precompute() returned %d %s\n",
                                 code, fsl_error_string(code));
                }
                else
                {
                        memset(hmac, 0, sizeof(hmac));
                        /* Finish the HMAC by running the message through */
                        /* INIT and PRECOMPUTES_PRESENT are set in precompute call */
                        fsl_shw_hmco_set_flags(hmac_ctx, test->flags);

                        code = fsl_shw_hmac(my_ctx, key_info, hmac_ctx, test->message,
                                            test->message_length, hmac, test->digest_length);
                        if (code != FSL_RETURN_OK_S)
                        {
                                TRACEMSG("Test failed: fsl_shw_hmac() returned %d %s\n",
                                         code, fsl_error_string(code));
                        }
                }
        }
        else
        {
                /* No pre-compute */
                memset(hmac, 0, sizeof(hmac));
                /* Set up for one-shot HMAC of all data... */
                fsl_shw_hmco_set_flags(hmac_ctx, test->flags);
                /* Compute HMAC from start to finish */
                code = fsl_shw_hmac(my_ctx, key_info, hmac_ctx, test->message,
                                    test->message_length, hmac, test->digest_length);
                if (code != FSL_RETURN_OK_S)
                {
                        TRACEMSG("Test failed: fsl_shw_hmac() returned %d %s\n",
                                 code, fsl_error_string(code));
                }
        }

        /* Verify HMAC */
        if (code == FSL_RETURN_OK_S)
        {
                if (!compare_result(test->digest, hmac, test->digest_length, "HMAC"))
                {
                        passed = 1;
                }
                else
                {
                        TRACEMSG("key: %s, msg: %s\n", test->key, test->message);
                }
        }

        kfree(hmac);
        kfree(hmac_ctx);
        kfree(key_info);
        kfree(test->key);
        kfree(test->message);
        kfree(test->digest);
        if(test1->context_length)
        {
                kfree(test->opad);
                kfree(test->ipad);
        }
        kfree(test);
        return passed;
}

/*==================================================================================================*/
/**
@brief  Sample code to calculate an HMAC in one operation

@param  my_ctx User context to use

@return On success - return 0 On failure - return -1
*/
/*==================================================================================================*/
int VT_run_hmac1(fsl_shw_uco_t * my_ctx)
{
        unsigned testno;

        unsigned passed_count = 0;
        unsigned failed_count = 0;
        int     passed;
        int     VT_rv = 0;

        TRACEMSG("canned HMAC test count: %d\n", NUM_HMACTESTS);

        for (testno = 0; testno < NUM_HMACTESTS; testno++)
        {
                switch (hmactests[testno].opId)
                {
                case DPD_SHA224_LDCTX_HMAC_ULCTX:
                case DPD_SHA224_LDCTX_HMAC_ULCTX_CMP:
                case DPD_SHA256_LDCTX_HMAC_ULCTX:
                case DPD_SHA256_LDCTX_HMAC_ULCTX_CMP:
                case DPD_SHA_LDCTX_HMAC_ULCTX:
                case DPD_SHA_LDCTX_HMAC_ULCTX_CMP:
                case DPD_MD5_LDCTX_HMAC_ULCTX:
                case DPD_MD5_LDCTX_HMAC_ULCTX_CMP:
                        TRACEMSG("Skipping test %s - User Prepad is not supported\n",
                                 hmactests[testno].testDesc);
                        continue;
                        break;

                case DPD_SHA224_LDCTX_HMAC_PAD_ULCTX:
                case DPD_SHA224_LDCTX_HMAC_PAD_ULCTX_CMP:
                        passed = run_hmac1_test(my_ctx, FSL_HASH_ALG_SHA224, hmactests + testno);
                        break;

                case DPD_SHA256_LDCTX_HMAC_PAD_ULCTX:
                case DPD_SHA256_LDCTX_HMAC_PAD_ULCTX_CMP:
                        passed = run_hmac1_test(my_ctx, FSL_HASH_ALG_SHA256, hmactests + testno);
                        break;

                case DPD_SHA_LDCTX_HMAC_PAD_ULCTX:
                case DPD_SHA_LDCTX_HMAC_PAD_ULCTX_CMP:
                        passed = run_hmac1_test(my_ctx, FSL_HASH_ALG_SHA1, hmactests + testno);
                        break;

                case DPD_MD5_LDCTX_HMAC_PAD_ULCTX:
                case DPD_MD5_LDCTX_HMAC_PAD_ULCTX_CMP:
                        passed = run_hmac1_test(my_ctx, FSL_HASH_ALG_MD5, hmactests + testno);
                        break;

                default:
                        continue;
                        break;
                }       /* switch */

                if (passed)
                {
                        TRACEMSG("%s: Passed\n", hmactests[testno].testDesc);
                        ++passed_count;
                }
                else
                {
                        TRACEMSG("%s: Failed\n", hmactests[testno].testDesc);
                        ++failed_count;
                }

        }       /* for testno ... */

        TRACEMSG("HMAC: %d tests passed, %d tests failed\n", passed_count, failed_count);

        if (0 != failed_count)
        {
                VT_rv = -1;
        }

        return VT_rv;
}

/*==================================================================================================*/
/**
@brief  Tests the get results operation

@param  my_ctx User context to use

@return On success - return 0 On failure - return -1
*/
/*==================================================================================================*/
int VT_run_result(fsl_shw_uco_t * my_ctx)
{
        fsl_shw_return_t code;  /* value returned from API call */
        unsigned actual;        /* number of results actually received */
        fsl_shw_result_t * results;//[RESULTS_SIZE]; /* place to put results */
        uint32_t loop;  /* number of iterations in verify loop */
        unsigned int launched_count = 0;
        unsigned int received_count = 0;
        struct rand_test * tests;//[NUM_REQUESTS];
        unsigned passed = 1;    /* boolean */
        int     VT_rv = 0;

        results = kmalloc(sizeof(fsl_shw_result_t)*RESULTS_SIZE,GFP_KERNEL);
        tests = kmalloc(sizeof(struct rand_test)*NUM_REQUESTS,GFP_KERNEL);

        TRACEMSG("Test: GET RESULTS\n");
        /* clears random data to zeros; sets initiated and result_received FALSE */
        memset(tests, 0, sizeof(struct rand_test)*NUM_REQUESTS);

        fsl_shw_uco_clear_flags(my_ctx, FSL_UCO_BLOCKING_MODE);

        /* This loop will have to be fixed when pool size is enforced. */

        for (loop = 0; loop < NUM_REQUESTS; loop++)
        {
                fsl_shw_uco_set_reference(my_ctx, loop);
                code = fsl_shw_get_random(my_ctx, sizeof(tests[loop].random), tests[loop].random);
                if (code != FSL_RETURN_OK_S)
                {
                        TRACEMSG("fsl_shw_random(), on call %d returned %s\n", loop,
                                 fsl_error_string(code));
                        passed = 0;
                }
                else
                {
                        launched_count++;
                        tests[loop].initiated = 1;
                }
        }

        loop = 0;
        /* Now go retrieve the results */
        while ((received_count < launched_count) && (loop++ < (1000 * NUM_REQUESTS)))
        {
                unsigned i;

                usleep(5);      /* microseconds */

                /* Now try for some results. */
                code = fsl_shw_get_results(my_ctx, RESULTS_SIZE, results, &actual);
                if (code != FSL_RETURN_OK_S)
                {
                        passed = 0;
                        TRACEMSG("fsl_shw_get_results() returned %s\n", fsl_error_string(code));
                        break;  /* get out of while() */
                }
                else
                {
                        if (actual > 0)
                        {
                                TRACEMSG("%d results received\n", actual);
                        }
                        /* and loop over each result received. */
                        for (i = 0; i < actual; i++)
                        {
                                unsigned testno = fsl_shw_ro_get_reference(results + i);

                                received_count++;
                                if ((testno >= NUM_REQUESTS) || (!tests[testno].initiated) ||
                                    tests[testno].result_received)
                                {
                                        passed = 0;
                                        TRACEMSG("result for bad reference %d received\n", testno);
                                }
                                else
                                {
                                        tests[testno].result_received = 1;
                                        if ((code =
                                             fsl_shw_ro_get_status(results + i)) != FSL_RETURN_OK_S)
                                        {
                                                TRACEMSG("result %d(%d) returned error %s\n",
                                                         testno, i, fsl_error_string(code));
                                                passed = 0;
                                        }
                                        if (check_one(tests[testno].random))
                                        {
                                                TRACEMSG("result %d values are not good\n", testno);
                                                passed = 0;
                                        }
                                }
                        }       /* for each result received */
                }
        }

        for (loop = 0; loop < NUM_REQUESTS; loop++)
        {
                if (!tests[loop].result_received)
                {
                        TRACEMSG("result never received for test %d\n", loop);
                        passed = 0;
                }
        }

        if (passed)
        {
                TRACEMSG("GET RESULTS: Passed\n");
        }
        else
        {
                TRACEMSG("GET RESULTS: Failed\n");
                VT_rv = -1;
        }

        fsl_shw_uco_set_flags(my_ctx, FSL_UCO_BLOCKING_MODE);

        kfree(results);
        kfree(tests);

        return VT_rv;
}

/*================================================================================================*/
/*===== run_symmetric_test =====*/
/**
@brief  All symmetric tests

@param  user_ctx User context

@param  test Type of test

@return On success - return 1 On failure - return 0
*/
/*================================================================================================*/
int run_symmetric_test(fsl_shw_uco_t * my_ctx, SYMTESTTYPE * test1)
{
        fsl_shw_return_t code;
        fsl_shw_sko_t * key_info;
        fsl_shw_scco_t * sym_ctx;
        fsl_shw_key_alg_t algorithm;
        fsl_shw_sym_mode_t mode;
        unsigned encrypt;
        unsigned blocksize;
        unsigned contextsize;
        unsigned char * outputText;
        unsigned char * output_iv;
        int     ignore_des_key_parity = 1;
        int     passed = 0;

        SYMTESTTYPE * test = kmalloc(sizeof(SYMTESTTYPE),GFP_KERNEL);
        memcpy(test,test1,sizeof(SYMTESTTYPE));
        if(test1->pKey)
        {
                test->pKey = kmalloc(test->KeyLen,GFP_KERNEL);
                memcpy(test->pKey,test1->pKey,test->KeyLen);
        }
        if(test1->pIV_CTR)
        {
                test->pIV_CTR = kmalloc(test->ivLen,GFP_KERNEL);
                memcpy(test->pIV_CTR,test1->pIV_CTR,test->ivLen);
        }
        test->pInputText = kmalloc(test->TextLen,GFP_KERNEL);
        memcpy(test->pInputText,test1->pInputText,test->TextLen);
        test->pOutputText = kmalloc(test->outputTextLen,GFP_KERNEL);
        memcpy(test->pOutputText,test1->pOutputText,test->outputTextLen);
        if(test1->pOutputIV_CTR)
        {
                test->pOutputIV_CTR = kmalloc(test->outputIVLen,GFP_KERNEL);
                memcpy(test->pOutputIV_CTR,test1->pOutputIV_CTR,test->outputIVLen);
        }

        key_info = kmalloc(sizeof(fsl_shw_sko_t),GFP_KERNEL);
        sym_ctx = kmalloc(sizeof(fsl_shw_scco_t),GFP_KERNEL);
        outputText = kmalloc(test->TextLen,GFP_KERNEL);
        output_iv = kmalloc(259,GFP_KERNEL);

        memset(key_info, 0xcc, sizeof(fsl_shw_sko_t));
        memset(sym_ctx, 0x42, sizeof(fsl_shw_scco_t));

        switch (test->opId)
        {
        case DPD_AES_CTR_ENC:
                algorithm = FSL_KEY_ALG_AES;
                blocksize = 16;
                contextsize = 16;
                mode = FSL_SYM_MODE_CTR;
                encrypt = 1;
                break;
        case DPD_AES_CBC_ENC:
                algorithm = FSL_KEY_ALG_AES;
                blocksize = 16;
                contextsize = 16;
                mode = FSL_SYM_MODE_CBC;
                encrypt = 1;
                break;
        case DPD_AES_ECB_ENC:
                algorithm = FSL_KEY_ALG_AES;
                blocksize = 16;
                contextsize = 16;
                mode = FSL_SYM_MODE_ECB;
                encrypt = 1;
                break;
        case DPD_AES_CTR_DEC:
                algorithm = FSL_KEY_ALG_AES;
                blocksize = 16;
                contextsize = 16;
                mode = FSL_SYM_MODE_CTR;
                encrypt = 0;
                break;
        case DPD_AES_CBC_DEC:
                algorithm = FSL_KEY_ALG_AES;
                blocksize = 16;
                contextsize = 16;
                mode = FSL_SYM_MODE_CBC;
                encrypt = 0;
                break;
        case DPD_AES_ECB_DEC:
                algorithm = FSL_KEY_ALG_AES;
                blocksize = 16;
                contextsize = 16;
                mode = FSL_SYM_MODE_ECB;
                encrypt = 0;
                break;
        case DPD_DES_CBC_ENC:
                algorithm = FSL_KEY_ALG_DES;
                blocksize = 8;
                contextsize = 8;
                mode = FSL_SYM_MODE_CBC;
                encrypt = 1;
                break;
        case DPD_DES_ECB_ENC:
                algorithm = FSL_KEY_ALG_DES;
                blocksize = 8;
                contextsize = 8;
                mode = FSL_SYM_MODE_ECB;
                encrypt = 1;
                break;
        case DPD_DES_CBC_DEC:
                algorithm = FSL_KEY_ALG_DES;
                blocksize = 8;
                contextsize = 8;
                mode = FSL_SYM_MODE_CBC;
                encrypt = 0;
                break;
        case DPD_DES_ECB_DEC:
                algorithm = FSL_KEY_ALG_DES;
                blocksize = 8;
                contextsize = 8;
                mode = FSL_SYM_MODE_ECB;
                encrypt = 0;
                break;
        case DPD_DES_KEY_PARITY:
                ignore_des_key_parity = 0;
                algorithm = FSL_KEY_ALG_DES;
                blocksize = 8;
                contextsize = 8;
                mode = FSL_SYM_MODE_ECB;
                encrypt = 0;
                break;
        case DPD_3DES_CBC_ENC:
                algorithm = FSL_KEY_ALG_TDES;
                blocksize = 8;
                contextsize = 8;
                mode = FSL_SYM_MODE_CBC;
                encrypt = 1;
                break;
        case DPD_3DES_ECB_ENC:
                algorithm = FSL_KEY_ALG_TDES;
                blocksize = 8;
                contextsize = 8;
                mode = FSL_SYM_MODE_ECB;
                encrypt = 1;
                break;
        case DPD_3DES_CBC_DEC:
                algorithm = FSL_KEY_ALG_TDES;
                blocksize = 8;
                contextsize = 8;
                mode = FSL_SYM_MODE_CBC;
                encrypt = 0;
                break;
        case DPD_3DES_ECB_DEC:
                algorithm = FSL_KEY_ALG_TDES;
                blocksize = 8;
                contextsize = 8;
                mode = FSL_SYM_MODE_ECB;
                encrypt = 0;
                break;
        case DPD_3DES_KEY_PARITY:
                ignore_des_key_parity = 0;
                algorithm = FSL_KEY_ALG_TDES;
                blocksize = 8;
                contextsize = 8;
                mode = FSL_SYM_MODE_ECB;
                encrypt = 0;
                break;
        case DPD_ARC4_ENC_KEY:
                algorithm = FSL_KEY_ALG_ARC4;
                blocksize = 0;
                contextsize = 259;
                mode = FSL_SYM_MODE_STREAM;
                encrypt = 1;
                break;
        case DPD_ARC4_ENC_SBOX:
                algorithm = FSL_KEY_ALG_ARC4;
                blocksize = 16; /* just for grins */
                contextsize = 259;
                mode = FSL_SYM_MODE_STREAM;
                encrypt = 1;
                break;
        default:
                algorithm = -1;
                mode = -1;
                encrypt = 0;
                blocksize = 0;
                contextsize = 0;
                break;
        }

        /* Initialize crypto objects */
        fsl_shw_sko_init(key_info, algorithm);
        fsl_shw_scco_init(sym_ctx, algorithm, mode);

        /* Insert the key into the key object */
        fsl_shw_sko_set_key(key_info, test->pKey, test->KeyLen);

        /* Set DES 'ignore parity' for certain cases. */
        if (ignore_des_key_parity && ((algorithm == FSL_KEY_ALG_DES)
                                      || (algorithm == FSL_KEY_ALG_TDES)))
        {
                fsl_shw_sko_set_flags(key_info, FSL_SKO_KEY_IGNORE_PARITY);
        }

        /* Set up context */
        if ((mode == FSL_SYM_MODE_CTR) || (mode == FSL_SYM_MODE_CBC) ||
            algorithm == FSL_KEY_ALG_ARC4)
        {
                if (test->pIV_CTR)
                {
                        fsl_shw_scco_set_flags(sym_ctx, FSL_SYM_CTX_LOAD);
                        if (mode == FSL_SYM_MODE_CTR)
                        {
                                fsl_shw_scco_set_counter_info(sym_ctx, test->pIV_CTR,
                                                              test->modulus);
                        }
                        else
                        {       /* CBC or ARC4 */
                                fsl_shw_scco_set_context(sym_ctx, test->pIV_CTR);
                        }
                }
                else
                {
                        fsl_shw_scco_set_flags(sym_ctx, FSL_SYM_CTX_INIT);
                }
        }
        /* If comparing output context ... have API save it */
        if (test->pOutputIV_CTR != NULL)
        {
                fsl_shw_scco_set_flags(sym_ctx, FSL_SYM_CTX_SAVE);
        }

        if (encrypt)
        {
                code = fsl_shw_symmetric_encrypt(my_ctx, key_info,
                                                 sym_ctx, test->TextLen,
                                                 test->pInputText, outputText);
        }
        else
        {
                code = fsl_shw_symmetric_decrypt(my_ctx, key_info,
                                                 sym_ctx, test->TextLen,
                                                 test->pInputText, outputText);
        }

        /* If OK is correct result, and that was returned, check data. */
        if ((code == test->result) && (code == FSL_RETURN_OK_S))
        {
                passed = 1;     /* assume for now */

                if (compare_result(test->pOutputText, outputText,
                                   test->TextLen, encrypt ? "ciphertext" : "plaintext"))
                {
                        passed = 0;     /* failed check */
                }

                /* Check context out if test has value to compare */
                if (test->pOutputIV_CTR != NULL)
                {
                        char   *fieldname;

                        if ((algorithm == FSL_KEY_ALG_ARC4) || (mode == FSL_SYM_MODE_CBC))
                        {
                                /* Extract the new IV from in the sym_ctx */
                                fsl_shw_scco_get_context(sym_ctx, output_iv);
                        }
                        else if (mode == FSL_SYM_MODE_CTR)
                        {
                                fsl_shw_ctr_mod_t mod;

                                fsl_shw_scco_get_counter_info(sym_ctx, output_iv, &mod);
                        }
                        /* and now compare */
                        if (algorithm == FSL_KEY_ALG_ARC4)
                        {
                                fieldname = "SBOX+pointers";
                        }
                        else if (mode == FSL_SYM_MODE_CBC)
                        {
                                fieldname = "IV";
                        }
                        else
                        {
                                fieldname = "CTR";
                        }
                        if (compare_result(test->pOutputIV_CTR, output_iv, contextsize, fieldname))
                        {
                                passed = 0;     /* failed */
                        }
                }
        }
        else
        {       /* OK was not expected or received */
                if (code == test->result)
                {
                        passed = 1;
                }
                else
                {
                        passed = 0;
                        TRACEMSG("fsl_shw_symmetric_%s() returned \"%s\" instead of "
                                 "\"%s\"\n", encrypt ? "encrypt" : "decrypt",
                                 fsl_error_string(code), fsl_error_string(test->result));
                }
        }

        if (passed)
        {
                TRACEMSG("%s: Passed\n", test->testDesc);
        }
        else
        {
                TRACEMSG("%s: Failed\n", test->testDesc);
        }

        if(test->pKey) kfree(test->pKey);
        if(test->pIV_CTR) kfree(test->pIV_CTR);
        kfree(test->pInputText);
        kfree(test->pOutputText);
        if(test->pOutputIV_CTR) kfree(test->pOutputIV_CTR);
        kfree(test);

        kfree(key_info);
        kfree(sym_ctx);
        kfree(outputText);
        kfree(output_iv);

        return passed;
}

/*=================================================================================================*/
/*===== VT_run_symmetric =====*/
/**
@brief  Run a series of tests on Symmetric cryptography routines.

@param  my_ctx User context to use

@return On success - return 0 On failure - return -1
*/
/*=================================================================================================*/
int VT_run_symmetric(fsl_shw_uco_t * my_ctx)
{
        int     passed_count = 0;
        int     failed_count = 0;
        unsigned testno;
        int     VT_rv = 0;

        for (testno = 0; testno < NUM_SYMTESTS; testno++)
        {
                if (run_symmetric_test(my_ctx, symTest + testno))
                {
                        passed_count++;
                }
                else
                {
                        failed_count++;
                }
        }

        TRACEMSG("symmetric: %d tests passed, %d tests failed\n", passed_count, failed_count);

        if (0 != failed_count)
        {
                VT_rv = -1;
        }

        return VT_rv;
}

/*================================================================================================*/
/*===== VT_run_gen_encrypt =====*/
/**
@brief  This function generates an authentication code and encrypt data

@param  my_ctx
        User context

@return Nothing
*/
/*================================================================================================*/
void VT_run_gen_encrypt(fsl_shw_uco_t * my_ctx)
{
        uint8_t pt[] =
        {
                0x20, 0x21, 0x22, 0x23
        };

        uint8_t addl_data[] =
        {
                0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07
        };

        uint8_t nonce[] =
        {
                0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x15
        };

        uint8_t key[] =
        {
                0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47,
                0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f
        };

        unsigned t = 4; /* length of MAC */
        unsigned q = 8; /* size of payload size ... 15-nonce_size */
        uint8_t ct[sizeof(pt) + 4];

        fsl_shw_sko_t ccm_key;
        fsl_shw_acco_t auth_object;
        fsl_shw_return_t code;

        fsl_shw_sko_init(&ccm_key, FSL_KEY_ALG_AES);
        fsl_shw_sko_set_key(&ccm_key, key, sizeof(key));

        fsl_shw_ccm_nist_format_ctr_and_iv(&auth_object, t, sizeof(addl_data),
                                           q, nonce, sizeof(pt) + 1);

        nonce[sizeof(nonce) - 1] += 1;

        /* Update with changed nonce and payload length */
        fsl_shw_ccm_nist_update_ctr_and_iv(&auth_object, nonce, sizeof(pt));

        /* Generate the MAC and encrypt the data */
        code = fsl_shw_gen_encrypt(my_ctx, &auth_object, &ccm_key, NULL /* key */ ,
                                   sizeof(addl_data), addl_data, sizeof(pt), pt,
                                   ct, ct + sizeof(pt));

        TRACEMSG("VT_run_gen_encrypt: gen code %d", code);
}

/*================================================================================================*/
/*===== VT_run_auth_decrypt =====*/
/**
@brief  This function verify an authentication code and decrypts data

@param  my_ctx
        User context

@return Nothing
*/
/*================================================================================================*/
void VT_run_auth_decrypt(fsl_shw_uco_t * my_ctx)
{
        /* ct includes the MAC as last four octets */
        uint8_t ct[] = {
                0x71, 0x62, 0x01, 0x5b, 0x4d, 0xac, 0x25, 0x5d
        };

        uint8_t addl_data[] = {
                0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07
        };

        uint8_t nonce[] = {
                0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16
        };

        uint8_t key[] = {
                0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47,
                0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f
        };

        unsigned t = 4; /* Length of MAC */
        unsigned q = 15 - sizeof(nonce);        /* size of payload size ... */
        uint8_t pt[sizeof(ct) - 4];

        fsl_shw_sko_t ccm_key;
        fsl_shw_acco_t auth_object;
        fsl_shw_return_t code;

        fsl_shw_sko_init(&ccm_key, FSL_KEY_ALG_AES);
        fsl_shw_sko_set_key(&ccm_key, key, sizeof(key));

        fsl_shw_ccm_nist_format_ctr_and_iv(&auth_object, t, sizeof(addl_data),
                                           q, nonce, sizeof(pt));

        /* Update with changed nonce and payload length */
        fsl_shw_ccm_nist_update_ctr_and_iv(&auth_object, nonce, sizeof(pt));

        /* Authenticate the MAC at end of cleartext and decrypt the data */
        code = fsl_shw_auth_decrypt(my_ctx, &auth_object, &ccm_key, NULL /* key */ ,
                                    sizeof(addl_data), addl_data, sizeof(pt), ct,
                                    ct + sizeof(pt), pt);

        TRACEMSG("VT_run_auth_decrypt: auth code %d", code);
}

/*================================================================================================*/
/*===== VT_show_capabilities =====*/
/**
@brief  Query the Platform Capabilities Object test

@param  my_ctx
        User context

@return On success - return 0
        On failure - return -1
*/
/*================================================================================================*/
int VT_show_capabilities(fsl_shw_uco_t * my_ctx)
{
        fsl_shw_pco_t *cap = fsl_shw_get_capabilities(my_ctx);
        fsl_shw_key_alg_t *capkeyalg;
        fsl_shw_sym_mode_t *capsymmode;
        fsl_shw_hash_alg_t *caphashalg;
        unsigned count;
        unsigned i;
        unsigned j;
        int     VT_rv = 0;

        if (cap == NULL)
        {
                TRACEMSG("VT_show_capabilities: Get capabilities failed!\n");
                VT_rv = -1;
        }
        else
        {

                TRACEMSG("Symmetric algorithms:\n");
                fsl_shw_pco_get_sym_algorithms(cap, &capkeyalg, &count);
                for (i = 0; i < count; i++)
                {
                        for (j = 0; j < sizeof(keyalgs) / sizeof(struct interpret); j++)
                        {
                                if (capkeyalg[i] == keyalgs[j].symbol)
                                {
                                        TRACEMSG("   %s\n", keyalgs[j].name);
                                        break;
                                }
                        }
                }

                TRACEMSG("Symmetric modes:\n");
                fsl_shw_pco_get_sym_modes(cap, &capsymmode, &count);
                for (i = 0; i < count; i++)
                {
                        for (j = 0; j < sizeof(symmodes) / sizeof(struct interpret); j++)
                        {
                                if (capsymmode[i] == symmodes[j].symbol)
                                {
                                        TRACEMSG("   %s\n", symmodes[j].name);
                                        break;
                                }
                        }
                }

                for (j = 0; j < sizeof(keyalgs) / sizeof(struct interpret); j++)
                {
                        TRACEMSG("Modes supported for %s:\n", keyalgs[j].name);
                        for (i = 0; i < sizeof(symmodes) / sizeof(struct interpret); i++)
                        {
                                if (fsl_shw_pco_check_sym_supported(cap, keyalgs[j].symbol,
                                                                    symmodes[i].symbol))
                                {
                                        TRACEMSG("   %s\n", symmodes[i].name);
                                }
                        }
                }

                TRACEMSG("Hash algorithms:\n");
                fsl_shw_pco_get_hash_algorithms(cap, &caphashalg, &count);
                for (i = 0; i < count; i++)
                {
                        for (j = 0; j < sizeof(hashalgs) / sizeof(struct interpret); j++)
                        {
                                if (caphashalg[i] == hashalgs[j].symbol)
                                {
                                        TRACEMSG("   %s\n", hashalgs[j].name);
                                        break;
                                }
                        }
                }
        }
        return VT_rv;
}

/*================================================================================================*/
/*===== VT_run_hmac2 =====*/
/**
@brief  This function calculates an HMAC in init/update/final operations

@param  my_ctx
        User context

@return On success - return 0
        On failure - return -1
*/
/*================================================================================================*/
int VT_run_hmac2(fsl_shw_uco_t * my_ctx)
{
        fsl_shw_return_t code;
        uint8_t *msg_ptr = NULL;
        uint8_t *msg = 0;
        uint32_t msg_len = 0;
        uint32_t msg_segment;
        uint8_t sequence_loop;
        uint8_t test_loop;
        uint8_t *result;
        fsl_shw_sko_t *key;
        fsl_shw_hmco_t *hco;
        int     VT_rv = 0;
        uint32_t * my_key1 = kmalloc(sizeof(my_key),GFP_KERNEL);
        memcpy(my_key1,my_key,sizeof(my_key));

        result = (uint8_t *) kmalloc(sizeof((int) RESULT_SIZE), GFP_KERNEL);
        if (result == NULL)
        {
                return -ENOMEM;
        }
        key = (fsl_shw_sko_t *) kmalloc(sizeof(fsl_shw_sko_t), GFP_KERNEL);
        if (key == NULL)
        {
                return -ENOMEM;
        }
        hco = (fsl_shw_hmco_t *) kmalloc(sizeof(fsl_shw_hmco_t), GFP_KERNEL);
        if (hco == NULL)
        {
                return -ENOMEM;
        }

        TRACEMSG("VT_run_hmac2: HMAC Multi-step / chunking Tests\n");

        /**** Get the precomputes *****/

        /* set up key information */
        fsl_shw_sko_init(key, FSL_KEY_ALG_HMAC);
        fsl_shw_sko_set_key(key, (uint8_t *) my_key1, 16);

        for (test_loop = 0; test_loop < NUMBER_OF_TESTS; ++test_loop)
        {
                /* set hmac information needed for precompute */
                fsl_shw_hmco_init(hco, test[test_loop].algorithm);

                /* This function sets the INIT and PRECOMPUTES_PRESENT flags */
                code = fsl_shw_hmac_precompute(my_ctx, key, hco);

                if (code != FSL_RETURN_OK_S)
                {
                        TRACEMSG("VT_run_hmac2: Test %d failed: precompute not obtained: %s\n",
                                 test_loop, fsl_error_string(code));
                        VT_rv = -1;
                }

                /**** Chunk through bulk of message ****/
                if (code == FSL_RETURN_OK_S)
                {
                        msg = kmalloc(strlen(my_msg[test[test_loop].id])+1,GFP_KERNEL);
                        memcpy(msg,my_msg[test[test_loop].id],strlen(my_msg[test[test_loop].id])+1);
                        msg_ptr = msg;
                        msg_len = strlen(msg_ptr);
                        msg_segment = 64 * test[test_loop].sequence[0];

                        /* set SAVE flag (INIT already set) */
                        fsl_shw_hmco_set_flags(hco, FSL_HMAC_FLAGS_SAVE);
                        sequence_loop = 1;

                        /* chunk thorugh the message */
                        while (msg_segment != 0)
                        {
                                code = fsl_shw_hmac(my_ctx, NULL,
                                                    hco, msg_ptr, msg_segment, NULL, 0);

                                if (code != FSL_RETURN_OK_S)
                                {
                                        TRACEMSG("VT_run_hmac2: Test %d failed: chunk %d bad: %s\n",
                                                 test_loop, sequence_loop, fsl_error_string(code));
                                        VT_rv = -1;
                                        break;
                                }
                                else
                                {
                                        /* been started, so clear INIT flag */
                                        fsl_shw_hmco_clear_flags(hco, FSL_HMAC_FLAGS_INIT);
                                        /* LOAD previous context (SAVE still set) */
                                        fsl_shw_hmco_set_flags(hco, FSL_HMAC_FLAGS_LOAD);

                                        msg_len -= msg_segment;
                                        msg_ptr += msg_segment;

                                        msg_segment =
                                            64 * test[test_loop].sequence[sequence_loop++];

                                        /* check that segment array isn't bad */
                                        if (msg_segment > msg_len)
                                        {
                                                msg_segment = 0;
                                                //msg_ptr = NULL;
                                                TRACEMSG
                                                    ("VT_run_hmac2: Test %d Sequence Array is bad\n",
                                                     test_loop);
                                                VT_rv = -1;
                                        }
                                }
                        }
                        kfree(msg);
                }

                /*** finish off this message ***/
                if ((code == FSL_RETURN_OK_S) && (msg_ptr != NULL))
                {
                        /* this is the last chunk, so set FINIALIZE flag */
                        fsl_shw_hmco_set_flags(hco, FSL_HMAC_FLAGS_FINALIZE);
                        fsl_shw_hmco_clear_flags(hco, FSL_HMAC_FLAGS_SAVE);

                /***** last chunk of message ****/
                        code = fsl_shw_hmac(my_ctx, NULL, hco, msg_ptr,
                                            msg_len, result, test[test_loop].digest_length);

                        if (code != FSL_RETURN_OK_S)
                        {
                                TRACEMSG("VT_run_hmac2: Test %d failed: final chunk bad: %s\n",
                                         test_loop, fsl_error_string(code));
                                VT_rv = -1;
                        }
                }

                if (code == FSL_RETURN_OK_S)
                {
                        if (compare_result(test[test_loop].result, result,
                                           test[test_loop].digest_length, "HMAC"))
                        {
                                TRACEMSG("VT_run_hmac2: Chunking HMAC Test %d Failed\n", test_loop);
                                VT_rv = -1;
                        }
                        else
                        {
                                TRACEMSG("VT_run_hmac2: Chunking HMAC Test %d Passed\n", test_loop);
                        }
                }
                else
                {
                        TRACEMSG("VT_run_hmac2: Chunking HMAC Test %d Failed\n", test_loop);
                        VT_rv = -1;
                }
        }

        kfree(key);
        kfree(hco);
        kfree(result);
        kfree(my_key1);
        return VT_rv;
}

/*================================================================================================*/
/*===== VT_run_random =====*/
/**
@brief  This function tests generating random data function

@param  my_ctx
        User context

@return On success - return 0
        On failure - return -1
*/
/*================================================================================================*/
int VT_run_random(fsl_shw_uco_t * my_ctx)
{
        int     VT_rv = 0;
        uint8_t random[24];
        fsl_shw_return_t code;

        memset(random, INIT_VAL, sizeof(random));
        TRACEMSG("Generating random data\n");

        /* Get some octets of random data */
        code = fsl_shw_get_random(my_ctx, sizeof(random), random);
        if (FSL_RETURN_OK_S != code)
        {
                TRACEMSG("VT_run_random: fsl_shw_get_random() failed, error %s\n",
                         fsl_error_string(code));
                VT_rv = -1;
        }
        else
        {
                if ((random[0] == INIT_VAL) && (random[1] == INIT_VAL) &&
                    (random[2] == INIT_VAL) && (random[3] == INIT_VAL) &&
                    (random[4] == INIT_VAL) && (random[5] == INIT_VAL) &&
                    (random[8] == INIT_VAL) && (random[12] == INIT_VAL) &&
                    (random[12] == INIT_VAL) && (random[20] == INIT_VAL))
                {
                        TRACEMSG("VT_run_random: Failed: Random number not random enough\n");
                        VT_rv = -1;
                }
                else
                {
                        TRACEMSG("VT_run_random: Random: passed\n");
                }
        }
        return VT_rv;
}

/*================================================================================================*/
static int create_key(fsl_shw_uco_t * my_ctx, fsl_shw_sko_t * key_info,
                      fsl_shw_scco_t * sym_ctx, const uint8_t * plaintext,
                      uint32_t len, uint8_t * encrypt_output)
{
        int     passed = 0;
        fsl_shw_return_t code;
        uint8_t *decrypt_output = kmalloc(2 * len, GFP_KERNEL);

        if (decrypt_output != NULL)
        {
                code = fsl_shw_establish_key(my_ctx, key_info, FSL_KEY_WRAP_CREATE, NULL);
                if (code != FSL_RETURN_OK_S)
                {
                        TRACEMSG("fsl_shw_establish_key(CREATE) returned: %s\n",
                                 fsl_error_string(code));
                }
                else
                {
                        memset(encrypt_output, 0, len);
                        /* Encrypt an arbitrary vector.  Try to decrypt to same value. */
                        code = fsl_shw_symmetric_encrypt(my_ctx, key_info, sym_ctx, len,
                                                         plaintext, encrypt_output);
                        if (code != FSL_RETURN_OK_S)
                        {
                                TRACEMSG("fsl_shw_symmetric_encrypt() returned: %s\n",
                                         fsl_error_string(code));
                        }
                        else
                        {
                                memset(decrypt_output, 0, len);
                                code = fsl_shw_symmetric_decrypt(my_ctx, key_info, sym_ctx,
                                                                 len, encrypt_output,
                                                                 decrypt_output);
                                if (code != FSL_RETURN_OK_S)
                                {
                                        TRACEMSG("fsl_shw_symmetric_decrypt() returned: %s\n",
                                                 fsl_error_string(code));
                                }
                                else
                                {
                                        if (!compare_result(plaintext, decrypt_output, len,
                                                            "decrypted plaintext"))
                                        {
                                                passed = 1;
                                        }
                                }
                        }
                }
        }
        else
        {
                return -ENOMEM;
        }

        if (decrypt_output)
        {
                kfree(decrypt_output);
        }

        return passed;
}

/*================================================================================================*/
static int extract_reestablish_key(fsl_shw_uco_t * my_ctx,
                                   uint32_t ownerid, uint32_t handle,
                                   fsl_shw_scco_t * sym_ctx,
                                   const uint8_t * ciphertext,
                                   const uint8_t * expected_plaintext, uint32_t len)
{
        int     passed = 0;
        uint8_t *blob = NULL;
        uint32_t blob_length;
        fsl_shw_sko_t old_key_info;     /* existing, established key info */
        fsl_shw_sko_t new_key_info;     /* to-be-unwrapped key info */
        fsl_shw_return_t code;
        uint8_t *decrypt_output = kmalloc(2 * len, GFP_KERNEL);

        fsl_shw_sko_init(&old_key_info, FSL_KEY_ALG_TDES);
        fsl_shw_sko_set_key_length(&old_key_info, SECRET_KEY_SIZE);
        fsl_shw_sko_set_established_info(&old_key_info, ownerid, handle);

        fsl_shw_sko_calculate_wrapped_size(&old_key_info, &blob_length);
        blob = kmalloc(blob_length, GFP_KERNEL);
        if (blob == NULL)
        {
                return -ENOMEM;
        }

        if (decrypt_output != NULL)
        {
                memset(blob, 0, blob_length);
                code = fsl_shw_extract_key(my_ctx, &old_key_info, blob);
                if (code != FSL_RETURN_OK_S)
                {
                        fsl_shw_return_t err_err;

                        TRACEMSG("fsl_shw_extract_key() returned: %s\n", fsl_error_string(code));
                        err_err = fsl_shw_release_key(my_ctx, &old_key_info);
                        if (err_err != 0)
                        {
                                TRACEMSG("Warning: could not release key with handle 0x%x: %s\n",
                                         handle, fsl_error_string(err_err));
                        }
                }
                else
                {
                        fsl_shw_sko_init(&new_key_info, FSL_KEY_ALG_TDES);
                        fsl_shw_sko_set_key_length(&new_key_info, SECRET_KEY_SIZE);
                        fsl_shw_sko_set_flags(&new_key_info, FSL_SKO_KEY_IGNORE_PARITY);
                        fsl_shw_sko_set_user_id(&new_key_info, KEY_OWNER_ID);

                        code = fsl_shw_establish_key(my_ctx, &new_key_info,
                                                     FSL_KEY_WRAP_UNWRAP, blob);
                        if (code != FSL_RETURN_OK_S)
                        {
                                TRACEMSG("fsl_shw_establish_key(UNWRAP) returned: %s\n",
                                         fsl_error_string(code));
                        }
                        else
                        {
                                fsl_shw_return_t err_err;

                                /* Try to decrypt what had been done with previous incarnation. */
                                memset(decrypt_output, 0, len);
                                code = fsl_shw_symmetric_decrypt(my_ctx, &new_key_info,
                                                                 sym_ctx, len, ciphertext,
                                                                 decrypt_output);
                                if (code != FSL_RETURN_OK_S)
                                {
                                        TRACEMSG("fsl_shw_symmetric_decrypt() returned: %s\n",
                                                 fsl_error_string(code));
                                }
                                else
                                {
                                        if (!compare_result(expected_plaintext, decrypt_output,
                                                            len, "decrypted plaintext"))
                                        {
                                                passed = 1;
                                        }
                                }

                                err_err = fsl_shw_release_key(my_ctx, &new_key_info);
                                if (err_err != 0)
                                {
                                        TRACEMSG("Warning: could not release key with handle 0x%x: "
                                                 "%s\n", handle, fsl_error_string(err_err));
                                }
                        }
                }
        }
        else
        {
                return -ENOMEM;
        }

        if (decrypt_output)
        {
                kfree(decrypt_output);
        }
        if (blob)
        {
                kfree(blob);
        }

        return passed;
}

/*================================================================================================*/
/*===== VT_run_wrap =====*/
/**
@brief  Test Key-Wrapping routines.

@param  my_ctx User context

@return On success - return 0 On failure - return -1
*/
/*================================================================================================*/
int VT_run_wrap(fsl_shw_uco_t * my_ctx, uint32_t * total_passed_count,
                uint32_t * total_failed_count)
{
        uint8_t *blob = NULL;
        uint8_t *encrypt_input;
        uint8_t *decrypt_input;
        uint8_t *encrypt_output;
        uint8_t *decrypt_output;
        uint32_t blob_length;
        uint32_t handle;
        int     passed_count = 0;
        int     failed_count = 0;
        int     passed = 0;
        int     VT_rv = 0;
        fsl_shw_sko_t key_info;
        fsl_shw_return_t code;
        fsl_shw_scco_t *sym_ctx;

        sym_ctx = kmalloc(sizeof(*sym_ctx), GFP_KERNEL);
        encrypt_input = kmalloc(sizeof(known_plaintext), GFP_KERNEL);
        decrypt_input = kmalloc(sizeof(known_plaintext), GFP_KERNEL);
        encrypt_output = kmalloc(2 * sizeof(known_plaintext), GFP_KERNEL);
        decrypt_output = kmalloc(2 * sizeof(known_plaintext), GFP_KERNEL);

        if ((encrypt_output == NULL) || (decrypt_output == NULL)
            || (encrypt_input == NULL) || (decrypt_input == NULL) || (sym_ctx == NULL))
        {
                TRACEMSG("Memory allocation problems. Skipping wrapped Key Tests\n");
                *total_failed_count += 4;
                VT_rv = -1;
                return -ENOMEM;
        }
        else
        {
                memcpy(encrypt_input, known_plaintext, sizeof(known_plaintext));
                memcpy(decrypt_input, known_ciphertext, sizeof(known_plaintext));

                /* for test, fill with garbage */
                memset(&key_info, 0x2d, sizeof(key_info));
                memset(sym_ctx, 0x51, sizeof(*sym_ctx));

                fsl_shw_sko_init(&key_info, FSL_KEY_ALG_TDES);
                fsl_shw_sko_set_key_length(&key_info, SECRET_KEY_SIZE);
                fsl_shw_sko_set_flags(&key_info, FSL_SKO_KEY_IGNORE_PARITY);

                fsl_shw_sko_set_user_id(&key_info, KEY_OWNER_ID);

                fsl_shw_scco_init(sym_ctx, FSL_KEY_ALG_TDES, FSL_SYM_MODE_ECB);

                TRACEMSG("Secret Key Test 1: Generate and use a RED key\n");

                passed = create_key(my_ctx, &key_info, sym_ctx,
                                    encrypt_input, sizeof(known_plaintext), encrypt_output);


                TRACEMSG("Secret Key Test 1: %s\n\n", passed ? "passed" : "failed");
                if (passed)
                {
                        passed_count++;
                }
                else
                {
                        failed_count++;
                }

                TRACEMSG("Secret Key Test 2: Extract and Re-establish RED key\n");

                fsl_shw_sko_get_established_info(&key_info, &handle);
                passed = extract_reestablish_key(my_ctx, KEY_OWNER_ID, handle,
                                                 sym_ctx, encrypt_output,
                                                 encrypt_input, sizeof(known_plaintext));

                TRACEMSG("Secret Key Test 2: %s\n\n", passed ? "passed" : "failed");
                if (passed)
                {
                        passed_count++;
                }
                else
                {
                        failed_count++;
                }

                /* fill with garbage */
                memset(&key_info, 0x2c, sizeof(key_info));
                memset(sym_ctx, 0x51, sizeof(sym_ctx));

                fsl_shw_sko_init(&key_info, FSL_KEY_ALG_TDES);
                fsl_shw_sko_set_key_length(&key_info, KNOWN_KEY_SIZE);
                fsl_shw_sko_set_flags(&key_info, FSL_SKO_KEY_IGNORE_PARITY);
                fsl_shw_sko_set_user_id(&key_info, KEY_OWNER_ID);

                fsl_shw_scco_init(sym_ctx, FSL_KEY_ALG_TDES, FSL_SYM_MODE_ECB);

                TRACEMSG("Secret Key Test 3: Establish a known RED key\n");
                passed = 0;

                code = fsl_shw_establish_key(my_ctx, &key_info, FSL_KEY_WRAP_ACCEPT, known_key);
                if (code != FSL_RETURN_OK_S)
                {
                        TRACEMSG("fsl_shw_establish_key(ACCEPT) returned: %s\n",
                                 fsl_error_string(code));
                        VT_rv = -1;
                }
                else
                {
                        memset(encrypt_output, 0, sizeof(known_plaintext));
                        code = fsl_shw_symmetric_encrypt(my_ctx, &key_info, sym_ctx,
                                                         sizeof(known_plaintext),
                                                         encrypt_input, encrypt_output);
                        if (code != FSL_RETURN_OK_S)
                        {
                                TRACEMSG("fsl_shw_symmetric_encrypt() returned: %s\n",
                                         fsl_error_string(code));
                                VT_rv = -1;
                        }
                        else
                        {
                                if (!compare_result(known_ciphertext, encrypt_output,
                                                    sizeof(known_plaintext),
                                                    "encrypted ciphertext"))
                                {
                                        passed = 1;
                                }
                        }
                }

                TRACEMSG("Secret Key Test 3: %s\n\n", passed ? "passed" : "failed");
                if (passed)
                {
                        passed_count++;
                }
                else
                {
                        failed_count++;
                }

                TRACEMSG("Secret Key Test 4: Re-establish known RED key\n");
                passed = 0;

                fsl_shw_sko_calculate_wrapped_size(&key_info, &blob_length);
                blob = kmalloc(blob_length, GFP_KERNEL);
                if (blob == NULL)
                {
                        TRACEMSG("Allocation failed; aborting test\n");
                        VT_rv = -1;
                        return -ENOMEM;
                }
                else
                {

                        code = fsl_shw_extract_key(my_ctx, &key_info, blob);
                        if (code != FSL_RETURN_OK_S)
                        {
                                fsl_shw_return_t err_err;

                                TRACEMSG("fsl_shw_extract_key() returned: %s\n",
                                         fsl_error_string(code));
                                VT_rv = -1;
                                err_err = fsl_shw_release_key(my_ctx, &key_info);
                                if (err_err != 0)
                                {
                                        TRACEMSG("Warning: could not release key with handle 0x%x: "
                                                 "%s\n", handle, fsl_error_string(err_err));
                                        VT_rv = -1;
                                }
                        }
                        else
                        {
                                fsl_shw_sko_init(&key_info, FSL_KEY_ALG_TDES);
                                fsl_shw_sko_set_key_length(&key_info, KNOWN_KEY_SIZE);
                                fsl_shw_sko_set_flags(&key_info, FSL_SKO_KEY_IGNORE_PARITY);
                                fsl_shw_sko_set_user_id(&key_info, KEY_OWNER_ID);

                                code = fsl_shw_establish_key(my_ctx, &key_info,
                                                             FSL_KEY_WRAP_UNWRAP, blob);
                                if (code != FSL_RETURN_OK_S)
                                {
                                        TRACEMSG("fsl_shw_establish_key(UNWRAP) returned: %s\n",
                                                 fsl_error_string(code));
                                        VT_rv = -1;
                                }
                                else
                                {
                                        /* Try to decrypt what had been done with previous
                                        * incarnation. */
                                        memset(decrypt_output, 0, sizeof(known_plaintext));
                                        code = fsl_shw_symmetric_decrypt(my_ctx, &key_info,
                                                                         sym_ctx,
                                                                         sizeof(known_plaintext),
                                                                         decrypt_input,
                                                                         decrypt_output);
                                        if (code != FSL_RETURN_OK_S)
                                        {
                                                TRACEMSG
                                                    ("fsl_shw_symmetric_decrypt() returned: %s\n",
                                                     fsl_error_string(code));
                                                VT_rv = -1;
                                        }
                                        else
                                        {
                                                if (!compare_result(known_plaintext, decrypt_output,
                                                                    sizeof(known_plaintext),
                                                                    "decrypted plaintext"))
                                                {
                                                        passed = 1;
                                                }
                                        }
                                        fsl_shw_release_key(my_ctx, &key_info);
                                }
                        }
                }

                TRACEMSG("Secret Key Test 4: %s\n\n", passed ? "passed" : "failed");
                if (passed)
                {
                        passed_count++;
                }
                else
                {
                        failed_count++;
                }

                TRACEMSG("wrap: %d tests passed, %d tests failed\n", passed_count, failed_count);

                *total_passed_count += passed_count;
                *total_failed_count += failed_count;
        }

        if (encrypt_input)
        {
                kfree(encrypt_input);
        }
        if (decrypt_input)
        {
                kfree(decrypt_input);
        }
        if (encrypt_output)
        {
                kfree(encrypt_output);
        }
        if (decrypt_output)
        {
                kfree(decrypt_output);
        }
        if (blob)
        {
                kfree(blob);
        }

        return VT_rv;
}

/*================================================================================================*/
/*===== compare_result =====*/
/**
@brief  Compare two strings. Print debug information if they do not compare.

@param  model The correct version of @a result.

@param  result An output string from a test.

@param  length The number of bytes of @a model, @a result to compare.

@param  name A NUL-terminated string which is the name of the string being compared.

@return If the strings compare - 0 Otherwise - -1
*/
/*================================================================================================*/
int compare_result(const uint8_t * model, const uint8_t * result,
                   uint32_t length, const uint8_t * name)
{
        int     VT_rv = -1;
        uint32_t i;

        for (i = 0; i < length; i++)
        {
                if (model[i] != result[i])
                {
                        break;
                }
        }
        if (i != length)
        {
                /* Try to dump relevant portions of data */
                uint32_t start_offset = (i > 10) ? (i - 4) : 0;
                uint32_t dump_count = (length > MAX_DUMP) ? MAX_DUMP : length;

                if ((start_offset + dump_count) > length)
                {
                        dump_count = length - start_offset;
                }

                TRACEMSG("Compare_result: Comparison of %s differs at offset %u of %u", name,
                         i, length);

                TRACEMSG("\nCompare_result: Good: (%d) ", start_offset);
                for (i = start_offset; i < (dump_count + start_offset); i++)
                {
                        TRACEMSG("%02x ", model[i]);
                }
                TRACEMSG("\nCompare_result: Bad:  (%d) ", start_offset);
                for (i = start_offset; i < (dump_count + start_offset); i++)
                {
                        TRACEMSG("%02x ", result[i]);
                }
                TRACEMSG("\n");
        }
        else
        {
                VT_rv = 0;
        }
        return VT_rv;
}

/*================================================================================================*/
static int sahara_test_ioctl(struct inode *inode, struct file *file, unsigned int cmd,
                             unsigned long arg)
{
        int     passed_count = 0;
        int     failed_count = 0;
        fsl_shw_uco_t *ctx;
        int error = 0;

        switch (cmd)
        {
        case SAHARA_TEST_RUN_CALLBACK:
                if ((ctx = kmalloc(sizeof(fsl_shw_uco_t), GFP_KERNEL)) == NULL)
                {
                        return -ENOMEM;
                }
                if (copy_from_user(ctx, (fsl_shw_uco_t *) arg, sizeof(fsl_shw_uco_t)))
                {
                        kfree(ctx);
                        return -EFAULT;
                }
                CHECK_ERROR(VT_run_callback(ctx));
                kfree(ctx);
                break;
        case SAHARA_TEST_RUN_HASH:
                if ((ctx = kmalloc(sizeof(fsl_shw_uco_t), GFP_KERNEL)) == NULL)
                {
                        return -ENOMEM;
                }
                if (copy_from_user(ctx, (fsl_shw_uco_t *) arg, sizeof(fsl_shw_uco_t)))
                {
                        kfree(ctx);
                        return -EFAULT;
                }
                CHECK_ERROR(VT_run_hash(ctx));
                kfree(ctx);
                break;
        case SAHARA_TEST_RUN_HMAC1:
                if ((ctx = kmalloc(sizeof(fsl_shw_uco_t), GFP_KERNEL)) == NULL)
                {
                        return -ENOMEM;
                }
                if (copy_from_user(ctx, (fsl_shw_uco_t *) arg, sizeof(fsl_shw_uco_t)))
                {
                        kfree(ctx);
                        return -EFAULT;
                }
                CHECK_ERROR(VT_run_hmac1(ctx));
                kfree(ctx);
                break;
        case SAHARA_TEST_RUN_HMAC2:
                if ((ctx = kmalloc(sizeof(fsl_shw_uco_t), GFP_KERNEL)) == NULL)
                {
                        return -ENOMEM;
                }
                if (copy_from_user(ctx, (fsl_shw_uco_t *) arg, sizeof(fsl_shw_uco_t)))
                {
                        kfree(ctx);
                        return -EFAULT;
                }
                CHECK_ERROR(VT_run_hmac2(ctx));
                kfree(ctx);
                break;
        case SAHARA_TEST_RUN_RESULT:
                if ((ctx = kmalloc(sizeof(fsl_shw_uco_t), GFP_KERNEL)) == NULL)
                {
                        return -ENOMEM;
                }
                if (copy_from_user(ctx, (fsl_shw_uco_t *) arg, sizeof(fsl_shw_uco_t)))
                {
                        kfree(ctx);
                        return -EFAULT;
                }
                CHECK_ERROR(VT_run_result(ctx));
                kfree(ctx);
                break;
        case SAHARA_TEST_RUN_SYMMETRIC:
                if ((ctx = kmalloc(sizeof(fsl_shw_uco_t), GFP_KERNEL)) == NULL)
                {
                        return -ENOMEM;
                }
                if (copy_from_user(ctx, (fsl_shw_uco_t *) arg, sizeof(fsl_shw_uco_t)))
                {
                        kfree(ctx);
                        return -EFAULT;
                }
                CHECK_ERROR(VT_run_symmetric(ctx));
                kfree(ctx);
                break;
        case SAHARA_TEST_RUN_WRAP:
                if ((ctx = kmalloc(sizeof(fsl_shw_uco_t), GFP_KERNEL)) == NULL)
                {
                        return -ENOMEM;
                }
                if (copy_from_user(ctx, (fsl_shw_uco_t *) arg, sizeof(fsl_shw_uco_t)))
                {
                        kfree(ctx);
                        return -EFAULT;
                }
                CHECK_ERROR(VT_run_wrap(ctx, &passed_count, &failed_count));
                kfree(ctx);
                break;
        case SAHARA_TEST_RUN_RANDOM:
                if ((ctx = kmalloc(sizeof(fsl_shw_uco_t), GFP_KERNEL)) == NULL)
                {
                        return -ENOMEM;
                }
                if (copy_from_user(ctx, (fsl_shw_uco_t *) arg, sizeof(fsl_shw_uco_t)))
                {
                        kfree(ctx);
                        return -EFAULT;
                }
                CHECK_ERROR(VT_run_random(ctx));
                kfree(ctx);
                break;
        case SAHARA_TEST_SHOW_CAPABILITIES:
                if ((ctx = kmalloc(sizeof(fsl_shw_uco_t), GFP_KERNEL)) == NULL)
                {
                        return -ENOMEM;
                }
                if (copy_from_user(ctx, (fsl_shw_uco_t *) arg, sizeof(fsl_shw_uco_t)))
                {
                        kfree(ctx);
                        return -EFAULT;
                }
                CHECK_ERROR(VT_show_capabilities(ctx));
                kfree(ctx);
                break;
        case SAHARA_TEST_RUN_ENCRYPT_DECRYPT:
                if ((ctx = kmalloc(sizeof(fsl_shw_uco_t), GFP_KERNEL)) == NULL)
                {
                        return -ENOMEM;
                }
                if (copy_from_user(ctx, (fsl_shw_uco_t *) arg, sizeof(fsl_shw_uco_t)))
                {
                        kfree(ctx);
                        return -EFAULT;
                }
                VT_run_gen_encrypt(ctx);
                VT_run_auth_decrypt(ctx);
                kfree(ctx);
                break;
        case SAHARA_TEST_REGISTER_USER:
                if ((ctx = kmalloc(sizeof(fsl_shw_uco_t), GFP_KERNEL)) == NULL)
                {
                        return -ENOMEM;
                }
                if (copy_from_user(ctx, (fsl_shw_uco_t *) arg, sizeof(fsl_shw_uco_t)))
                {
                        kfree(ctx);
                        return -EFAULT;
                }
                CHECK_ERROR(VT_register_user(ctx));
                if (copy_to_user((fsl_shw_uco_t *) arg, ctx, sizeof(fsl_shw_uco_t)))
                {
                        kfree(ctx);
                        return -EFAULT;
                }
                kfree(ctx);
                break;
        case SAHARA_TEST_DEREGISTER_USER:
                if ((ctx = kmalloc(sizeof(fsl_shw_uco_t), GFP_KERNEL)) == NULL)
                {
                        return -ENOMEM;
                }
                if (copy_from_user(ctx, (fsl_shw_uco_t *) arg, sizeof(fsl_shw_uco_t)))
                {
                        kfree(ctx);
                        return -EFAULT;
                }
                CHECK_ERROR(VT_deregister_user(ctx));
                kfree(ctx);
                break;
        }
        return error;
}

/*==================================================================================================
                                GLOBAL VARIABLE DECLARATIONS
==================================================================================================*/
static struct file_operations sahara_test_fops =
{
        owner:        THIS_MODULE,
        open:         sahara_test_open,
        release:      sahara_test_release,
        read:         sahara_test_read,
        write:        sahara_test_write,
        unlocked_ioctl:        sahara_test_ioctl,
};

/*================================================================================================*/
static int __init sahara_test_init(void)
{
        int     res;

        TRACEMSG("----------------------------------------------------------------------------\n");
        TRACEMSG(KERN_INFO "Sahara Test: creating virtual device\n");
        res = register_chrdev(231, SAHARA_TEST_DEVICE, &sahara_test_fops);

        if (res < 0)
        {
                TRACEMSG(KERN_INFO "Sahara Test: unable to register the device\n");
                return res;
        }

        sahara_class = class_create(THIS_MODULE, "sahara_test");
        if (IS_ERR(sahara_class))
        {
                printk(KERN_ALERT "class simple created failed\n");
                goto err_out;
        }
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,28))
		if (IS_ERR(device_create(sahara_class, NULL,
                MKDEV(231, 0),"sahara_test")))
#else
        if (IS_ERR(device_create(sahara_class, NULL,
                MKDEV(231, 0),NULL,"sahara_test")))
#endif
        {
                printk(KERN_ALERT "class simple add failed\n");
                goto err_out;
        }

        //---devfs_mk_cdev(MKDEV(231, 0), S_IFCHR | S_IRUGO | S_IWUGO, SAHARA_TEST_DEVICE);

        return 0;

        err_out:
        printk(KERN_ERR "SAHARA : error creating sahara test module class.\n");
        device_destroy(sahara_class, MKDEV(231, 0));
        class_destroy(sahara_class);
        unregister_chrdev(231, SAHARA_TEST_DEVICE);
        return -1;


}

/*================================================================================================*/
static void __exit sahara_test_exit(void)
{
        unregister_chrdev(231, SAHARA_TEST_DEVICE);
        //---devfs_remove(SAHARA_TEST_DEVICE);
        device_destroy(sahara_class, MKDEV(231, 0));
        class_destroy(sahara_class);
        TRACEMSG(KERN_INFO "Sahara Test: removing virtual device\n");
}

/*================================================================================================*/
module_init(sahara_test_init);
module_exit(sahara_test_exit);

MODULE_DESCRIPTION("Test Module for SAHARA driver");
MODULE_LICENSE("GPL");
