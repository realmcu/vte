#ifndef _I2C_TEST
#define _I2C_TEST


#ifdef __cplusplus
extern "C"{
#endif

#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>    /* open()  */
#include <fcntl.h>        /* open()  */
#include <sys/ioctl.h>    /* ioctl() */
#include <linux/types.h>
#include <linux/i2c.h> 
#include <linux/i2c-dev.h>




/* Harness Specific Include Files. */
#include <test.h>
#include <usctest.h>

#include "i2c_smbus.h"

#define DEFAULT_I2C_NAME  "/dev/i2c-0"
#define DEFAULT_CASE_NUMBER  0
#define DEFAULT_SLAVE_ADDR  0x50
#define DEFAULT_REG_ADDR  0x01
#define MSG_LENTH 2

int case_number = 0;
__u8 slave_addr = 0;
__u8 reg_addr = 0;






void cleanup(void);
void help(void);
int setup(void);
int i2c_test();











int i2c_test_rw();
int i2c_test_ioctl_rw();
int i2c_test_smbus();
int i2c_test_smbus_read();
int i2c_test_read();
int i2c_test_ioctl();

int i2c_test_ioctl_tenbit();
int i2c_test_ioctl_pec();
int i2c_test_ioctl_funcs();
int i2c_test_ioctl_retries();
int i2c_test_ioctl_timeout();


#ifdef __cplusplus
}
#endif

#endif
