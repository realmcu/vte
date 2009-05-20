/*====================*/
/**
        @file   i2c_test.c

        @brief  Test scenario C source for i2c_testapp_3.
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
-------------------------   ------------    ----------  --------------------------------------------
V.Khalabuda/hlbv001          21/07/2004     TLSbo40419  I2C test development
L.Delaspre/rc149c            20/10/2004     TLSbo43922  I2C updates for L26.1.5 LSP
V.Khalabuda/hlbv001          28/02/2005     TLSbo45061  I2C updates for L26.1.7
S.V-Guilhou/svan01c          20/09/2005     TLSbo53753  I/O errors
V.Khalabuda/b00306           04/07/2006     TLSbo68945  Update testapp for I2C_RDWR ioctl
                                                        and r/w from user space

====================
Portability: ARM GCC

======================*/

/*======================
                                        INCLUDE FILES
======================*/
/* Verification Test Environment Include Files */
#include "i2c_test.h"

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
int     fd = 0;
struct  i2c_rdwr_ioctl_data work_queue;

extern I2C_TESTS i2c_testcase;
extern char device_name[128];
extern unsigned short addr;

/*======================
                                    LOCAL FUNCTION PROTOTYPES
======================*/

/*======================
                                        LOCAL FUNCTIONS
======================*/

/*====================*/
/*= VT_setup =*/
/**
@brief  assumes the pre-condition of the test case execution

@param  None

@return On success - return TPASS
        On failure - return the error code
*/
/*====================*/
int VT_i2c_setup(void)
{


        fd = open(device_name, O_RDWR);

        if ((fd) < 0)
        {
                tst_resm(TFAIL, "VT_i2c_setup() Failed open device");
    return TFAIL;
        }

        work_queue.msgs = (struct i2c_msg *) malloc(work_queue.nmsgs *sizeof(struct i2c_msg));
        if (!work_queue.msgs)
        {
                tst_resm(TFAIL, "VT_i2c_setup() Failed allocate memory");
    return TFAIL;
        }


        return TPASS;
}

/*====================*/
/*= VT_cleanup =*/
/**
@brief  assumes the post-condition of the test case execution

@param  None

@return On success - return TPASS
        On failure - return the error code
*/
/*====================*/
int VT_i2c_cleanup(void)
{
        int     VT_rv = TFAIL;

        if (fd > 0)
                close(fd);

        VT_rv = TPASS;

        return VT_rv;
}

/*====================*/
/*= VT_i2c_test =*/
/**
@brief  i2c test scenario function

@param  None

@return On success - return TPASS
        On failure - return the error code
*/
/*====================*/
int VT_i2c_test(void)
{
        int     VT_rv = TFAIL;

        switch (i2c_testcase)
        {
        case TEST_I2C_RW:
                tst_resm(TINFO, "Read/Write from I2C character device interface from user space");
                VT_rv = VT_i2c_test_rw();
                break;
        case TEST_I2C_IOCTL:
                tst_resm(TINFO, "Send the I2C messages using kernel ioctls with I2C_RDWR");
                VT_rv = VT_i2c_test_ioctl();
                break;
        default:
                tst_resm(TFAIL, "Operation not permitted");
                break;
        }
 //printf("VT_rv's value: %d\n", VT_rv);
        return VT_rv;
}

/*====================*/
/*= VT_i2c_test_rw =*/
/**
@brief  Read/Write from I2C character device interface from user space.
        reads bytes at the memory address (addr) from assigned slave address on the i2c device bus

@param  None

@return On success - return TPASS
        On failure - return the error code
*/
/*====================*/
int VT_i2c_test_rw(void)
{
        int     VT_rv = TPASS;
        //unsigned short size = BUFF_SIZE;
        //unsigned short idx;
       // char           buf[BUFF_SIZE];
        //long int mem_addr;
        char               cswap;

  int write_sign = -2;
  int read_sign = -2;
  int size = BUFF_SIZE;
  int idx;

  //charswap_buffer[3];
  char  buf_read[BUFF_SIZE*2];
  //char  buf[10];
  memset(buf_read,'a',BUFF_SIZE*2);
  buf_read[BUFF_SIZE*2-1]='\0';
  printf("buf_read content: %s\n", buf_read);
        union
        {
                unsigned short       mem_addr;
                char          bytes[2];
        } tmp;

 printf("addr's value: %d\n", addr);

  printf("i2c rw test starts......\n");
        if (ioctl(fd, I2C_SLAVE, 0x34) != 0)        /* set the pmic address */
        {
                tst_resm(TFAIL, "ERROR: with ioctl I2C_SLAVE. Errno: %d, Reason: %s", errno, strerror(errno));
                VT_rv = TFAIL;
        }

        if (ioctl(fd, I2C_TIMEOUT, 1) != 0)        /* set the timeout    */
        {
                tst_resm(TFAIL, "ERROR: with ioctl I2C_TIMEOUT. Errno: %d, Reason: %s", errno, strerror(errno));
                VT_rv = TFAIL;
        }

        if (ioctl(fd, I2C_RETRIES, 1) != 0)        /* set the retries    */
        {
                tst_resm(TFAIL, "ERROR: with ioctl I2C_RETRIES. Errno: %d, Reason: %s", errno, strerror(errno));
                VT_rv = TFAIL;
        }




 for(idx=0;idx<size-1;idx++)
  {
   tmp.mem_addr=addr+idx;
   cswap=tmp.bytes[0];
   tmp.bytes[0]=tmp.bytes[1];
   tmp.bytes[1]=cswap;


   write_sign=write(fd, tmp.bytes, 1);
   //buf[0]=0x07;
   //write_sign=write(fd,buf,1);
   if(write_sign<0)
    {
    printf("write size: %d \n",write_sign);
    return TFAIL;
    }

   read_sign=read(fd, buf_read+idx*2, 2);
   if(read_sign<0)
    {
    printf("read size: %d \n",read_sign);
    return TFAIL;
    }

  printf("the data readed from eeprom is: %x%x\n", buf_read[idx*2],buf_read[idx*2+1]);
  }
 buf_read[BUFF_SIZE*2-1]='\0';

 //printf("the data readed from eeprom is: %s\n", buf_read);

 printf("i2c rw test finished......\n");
        return VT_rv;
}

/*====================*/
/*= VT_i2c_test_ioctl =*/
/**
@brief  Send the I2C messages using kernel ioctls with I2C_RDWR

@param  None

@return On success - return TPASS
        On failure - return the error code
*/
/*====================*/
int VT_i2c_test_ioctl(void)
{
        int     VT_rv = TPASS;
        struct  i2c_rdwr_ioctl_data msg_queue;
        unsigned int   idx;

        msg_queue.nmsgs = MAX_I2C_MSG;

 int sign=-2;

        for (idx = 0; idx < work_queue.nmsgs; ++idx)
        {
                (work_queue.msgs[idx]).len   = 0;
                (work_queue.msgs[idx]).addr  = addr + idx;
                (work_queue.msgs[idx]).buf   = NULL;
        }

        if (ioctl(fd, I2C_TIMEOUT, 2) < 0)        /* set the timeout    */
        {
                tst_resm(TFAIL, "ERROR: with ioctl I2C_TIMEOUT. Errno: %d, Reason: %s", errno, strerror(errno));
                VT_rv = TFAIL;
        }

        if (ioctl(fd, I2C_RETRIES, 1) < 0)        /* set the retries    */
        {
                tst_resm(TFAIL, "ERROR: with ioctl I2C_RETRIES. Errno: %d, Reason: %s", errno, strerror(errno));
                VT_rv = TFAIL;
        }
 printf("i2c ioctl test starts......\n");

  sign=ioctl(fd, I2C_RDWR, (unsigned long)&work_queue);
  sleep(1);
        if (sign< 0)
        {
                tst_resm(TFAIL, "ERROR: with ioctl I2C_RDWR. Errno: %d, Reason: %s", errno, strerror(errno));
                VT_rv = TFAIL;
        }

 printf("i2c ioctl test finished......\n");

        return VT_rv;
}
