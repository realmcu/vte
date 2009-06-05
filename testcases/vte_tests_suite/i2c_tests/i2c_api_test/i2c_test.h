/*================================================================================================*/
/**
        @file   i2c_test.h

        @brief  I2C test header file
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
-------------------------   ------------    ----------  --------------------------------------------
V.Khalabuda/hlbv001          21/07/2004     TLSbo40419  I2C test development
V.Khalabuda/HLBV001          28/02/2005     TLSbo45061  I2C updates for L26.1.7
V.Khalabuda/b00306           04/07/2006     TLSbo68945  Update testapp for I2C_RDWR ioctl
                                                        and r/w from user space

==================================================================================================*/
#ifndef MXC_TEST1_H
#define MXC_TEST1_H

#ifdef __cplusplus
extern "C"{
#endif


/*==================================================================================================
                                        INCLUDE FILES
==================================================================================================*/
/* Standard Include Files */
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>    /* open()  */
#include <fcntl.h>        /* open()  */
#include <sys/ioctl.h>    /* ioctl() */
#include <linux/types.h>
#include <linux/i2c.h>  //added by devil
#include <linux/i2c-dev.h>
#include "i2c_smbus.h"

/* Harness Specific Include Files. */
#include <test.h>
#include <usctest.h>

/*==================================================================================================
                                        KERNEL VARIABLES
==================================================================================================*/
#if 0 //deleted by devil
/* Kernel variables for performs sending the I2C messages */
struct i2c_msg;
struct i2c_msg
{
        __u16   addr;   /* slave address */
        __u16   flags;
#define I2C_M_TEN       0x10    /* we have a ten bit chip address */
#define I2C_M_RD        0x01
#define I2C_M_NOSTART   0x4000
#define I2C_M_REV_DIR_ADDR        0x2000
#define I2C_M_IGNORE_NAK        0x1000
#define I2C_M_NO_RD_ACK                0x0800
        __u16   len;    /* msg length */
        __u8   *buf;    /* pointer to msg data */
};

#define MAX_I2C_MSG        2
#endif

#if 0  //deleted by devil
#define I2C_RETRIES        0x0701
#define I2C_TIMEOUT        0x0702
#define I2C_SLAVE          0x0703
#define I2C_RDWR           0x0707        /* Combined R/W transfer (one stop only) */
#endif

/*==================================================================================================
                                            CONSTANTS
==================================================================================================*/
#if !defined(TRUE) && !defined(FALSE)
#define TRUE  1
#define FALSE 0
#endif

/*==================================================================================================
                                        DEFINES AND MACROS
==================================================================================================*/
#define DEFAULT_I2C_BUS          "i2c-0"
#define BUFF_SIZE                32                /* one page is 256 byte */

#define TSKIP 64

/*==================================================================================================
                                            ENUMS
==================================================================================================*/
#if 0 //deleted by devil
/* Testcases of I2C */
typedef enum
{
        TEST_I2C_RW,
        TEST_I2C_IOCTL,
} I2C_TESTS;
#endif 

/*==================================================================================================
                                STRUCTURES AND OTHER TYPEDEFS
==================================================================================================*/

/*==================================================================================================
                                GLOBAL VARIABLE DECLARATIONS
==================================================================================================*/

/*==================================================================================================
                                    FUNCTION PROTOTYPES
==================================================================================================*/
void help(void);

int VT_i2c_setup(void);
int VT_i2c_cleanup(void);

int VT_i2c_test_capability(void * arg);
int VT_i2c_test_ioctl_slave(void * arg);
int VT_i2c_test_ioctl_10bit(void * arg);
int VT_i2c_test_ioctl_pec(void * arg);
int VT_i2c_test_ioctl_timeout(void * arg);
int VT_i2c_test_ioctl_retries(void * arg);
int VT_i2c_test_ioctl_readwrite(void * arg);
int VT_i2c_test_smbus(void * arg);

#ifdef __cplusplus
}
#endif

#endif        /* MXC_TEST1_H */
