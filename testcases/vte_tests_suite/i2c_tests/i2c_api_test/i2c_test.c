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
        @file   i2c_test.c

        @brief  Test scenario C source for i2c_testapp_3.
====================================================================================================
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

====================================================================================================
Portability: ARM GCC

==================================================================================================*/

/*==================================================================================================
                                        INCLUDE FILES
==================================================================================================*/
/* Verification Test Environment Include Files */
#include "i2c_test.h"

#define I2C_FUNC_SMBUS_PEC		0x00000008

/*==================================================================================================
                                        LOCAL MACROS
==================================================================================================*/

/*==================================================================================================
                            LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
==================================================================================================*/

/*==================================================================================================
                                        LOCAL CONSTANTS
==================================================================================================*/

/*==================================================================================================
                                        LOCAL VARIABLES
==================================================================================================*/

/*==================================================================================================
                                        GLOBAL CONSTANTS
==================================================================================================*/

/*==================================================================================================
                                        GLOBAL VARIABLES
==================================================================================================*/
int     fd = 0;
struct  i2c_rdwr_ioctl_data work_queue;

extern char device_name[128];
extern unsigned short slave_addr;

/*==================================================================================================
                                    LOCAL FUNCTION PROTOTYPES
==================================================================================================*/

/*==================================================================================================
                                        LOCAL FUNCTIONS
==================================================================================================*/

/*================================================================================================*/
/*===== VT_setup =====*/
/**
@brief  assumes the pre-condition of the test case execution

@param  None
    
@return On success - return TPASS
        On failure - return the error code
*/
/*================================================================================================*/
int VT_i2c_setup(void)
{
        int     VT_rv = TFAIL;

        fd = open(device_name, O_RDWR);
        if ((fd) < 0)
        {
                tst_resm(TFAIL, "VT_i2c_setup() Failed open device");
        }

        work_queue.msgs = (struct i2c_msg *) malloc(work_queue.nmsgs *sizeof(struct i2c_msg));
        if (!work_queue.msgs)
        {
                tst_resm(TFAIL, "VT_i2c_setup() Failed allocate memory");
        }


        VT_rv = TPASS;
        return VT_rv;
}

/*================================================================================================*/
/*===== VT_cleanup =====*/
/**
@brief  assumes the post-condition of the test case execution

@param  None
    
@return On success - return TPASS
        On failure - return the error code
*/
/*================================================================================================*/
int VT_i2c_cleanup(void)
{
        int     VT_rv = TFAIL;

        if (fd > 0)
                close(fd);

        VT_rv = TPASS;

        return VT_rv;
}

struct i2c_func_list{
	char * func_name;
	unsigned long func_num;
};

void print_result(int rst)
{
	if(rst==TPASS) tst_resm(TPASS,"-PASS: The case is passed!");
	else if(rst==TFAIL) tst_resm(TFAIL,"-FAIL: The case is failed!");
	else tst_resm(TINFO,"-SKIP: The case is skipped!");
}

int VT_i2c_test_capability(void * arg)
{
	int VT_rv=TPASS;
	unsigned long funcs;
	int i;
	
	//set the parameter is invalid
	arg=NULL;
	
	struct i2c_func_list i2cfl[]=
	{
			{"I2C_FUNC_I2C",I2C_FUNC_I2C},
			{"I2C_FUNC_10BIT_ADDR",I2C_FUNC_10BIT_ADDR},
			{"I2C_FUNC_PROTOCOL_MANGLING",I2C_FUNC_PROTOCOL_MANGLING},
			{"I2C_FUNC_SMBUS_PEC",I2C_FUNC_SMBUS_PEC},
			{"I2C_FUNC_SMBUS_BLOCK_PROC_CALL",I2C_FUNC_SMBUS_BLOCK_PROC_CALL},
			{"I2C_FUNC_SMBUS_QUICK",I2C_FUNC_SMBUS_QUICK},
			{"I2C_FUNC_SMBUS_READ_BYTE",I2C_FUNC_SMBUS_READ_BYTE},
			{"I2C_FUNC_SMBUS_WRITE_BYTE",I2C_FUNC_SMBUS_WRITE_BYTE},
			{"I2C_FUNC_SMBUS_READ_BYTE_DATA",I2C_FUNC_SMBUS_READ_BYTE_DATA},
			{"I2C_FUNC_SMBUS_WRITE_BYTE_DATA",I2C_FUNC_SMBUS_WRITE_BYTE_DATA},
			{"I2C_FUNC_SMBUS_READ_WORD_DATA",I2C_FUNC_SMBUS_READ_WORD_DATA},
			{"I2C_FUNC_SMBUS_WRITE_WORD_DATA",I2C_FUNC_SMBUS_WRITE_WORD_DATA},
			{"I2C_FUNC_SMBUS_PROC_CALL",I2C_FUNC_SMBUS_PROC_CALL},
			{"I2C_FUNC_SMBUS_READ_BLOCK_DATA",I2C_FUNC_SMBUS_READ_BLOCK_DATA},
			{"I2C_FUNC_SMBUS_WRITE_BLOCK_DATA",I2C_FUNC_SMBUS_WRITE_BLOCK_DATA},
			{"I2C_FUNC_SMBUS_READ_I2C_BLOCK",I2C_FUNC_SMBUS_READ_I2C_BLOCK},
			{"I2C_FUNC_SMBUS_WRITE_I2C_BLOCK",I2C_FUNC_SMBUS_WRITE_I2C_BLOCK},
			{"I2C_FUNC_SMBUS_READ_I2C_BLOCK_2",I2C_FUNC_SMBUS_READ_I2C_BLOCK_2},
			{"I2C_FUNC_SMBUS_WRITE_I2C_BLOCK_2",I2C_FUNC_SMBUS_WRITE_I2C_BLOCK_2},
			{"I2C_FUNC_SMBUS_BYTE",I2C_FUNC_SMBUS_BYTE},
			{"I2C_FUNC_SMBUS_BYTE_DATA",I2C_FUNC_SMBUS_BYTE_DATA},
			{"I2C_FUNC_SMBUS_WORD_DATA",I2C_FUNC_SMBUS_WORD_DATA},
			{"I2C_FUNC_SMBUS_BLOCK_DATA",I2C_FUNC_SMBUS_BLOCK_DATA},
			{"I2C_FUNC_SMBUS_I2C_BLOCK",I2C_FUNC_SMBUS_I2C_BLOCK},
			{"I2C_FUNC_SMBUS_I2C_BLOCK_2",I2C_FUNC_SMBUS_I2C_BLOCK_2},
			{"I2C_FUNC_SMBUS_EMUL",I2C_FUNC_SMBUS_EMUL},
			{NULL,NULL}
	};
	
	if(ioctl(fd,I2C_FUNCS,&funcs)<0){
		tst_resm(TFAIL, "------ERROR: with ioctl I2C_FUNCS. Error : %d, Reason: %s",errno,strerror(errno));
		VT_rv=TFAIL;
	}

	tst_resm(TINFO,"---Supported:");
	for(i=0;i2cfl[i].func_name;i++) 
		if(funcs&i2cfl[i].func_num) tst_resm(TINFO,"------%s",i2cfl[i].func_name);
		
	tst_resm(TINFO,"---Unsupported:");
	for(i=0;i2cfl[i].func_name;i++)
		if(!(funcs&i2cfl[i].func_num)) tst_resm(TINFO,"------%s",i2cfl[i].func_name);

	print_result(VT_rv);

	return VT_rv;	
}

int VT_i2c_test_ioctl_slave(void * arg)
{
	unsigned long funcs=0;
	int VT_rv=TPASS;

	//set the parameter is invalid.
	arg=NULL;

	if(ioctl(fd,I2C_FUNCS,&funcs)<0){
		tst_resm(TFAIL, "------ERROR: with ioctl I2C_FUNCS. Error : %d, Reason: %s",errno,strerror(errno));
		VT_rv=TFAIL;
	}

	//set non-10bit mode.
	if(funcs&I2C_FUNC_10BIT_ADDR) 
		ioctl(fd,I2C_TENBIT,0);

	//test slave addr
	if(ioctl(fd,I2C_SLAVE,slave_addr)<0){
		tst_resm(TFAIL,"------ERROR: can not set slave addr=0x%x.Errno: %d, Reason: %s",slave_addr, errno, strerror(errno));
		VT_rv=TFAIL;
	}

	//double set slave addr=slave_addr,it should be failed
	if(ioctl(fd,I2C_SLAVE,slave_addr)==0){
		tst_resm(TFAIL,"------ERROR: double set slave addr=0x%x passed,it should return fail.Errno: %d, Reason: %s",slave_addr, errno, strerror(errno));
		VT_rv=TFAIL;
	}

	//test slave addr=0x7f with non-10bit mode
	if(ioctl(fd,I2C_SLAVE,0x7f)==0){
		tst_resm(TFAIL,"------ERROR: set an invalid slave addr=0x7f,it should return fail.Errno: %d, Reason: %s", errno, strerror(errno));
		VT_rv=TFAIL;
	}

	//set 10bit mode if it is supported.
	if(funcs&I2C_FUNC_10BIT_ADDR){
		ioctl(fd,I2C_TENBIT,1);

		//test slave addr=0x3fe
		if(ioctl(fd,I2C_SLAVE,0x3fe)<0){
			tst_resm(TFAIL,"------ERROR: can not set slave addr=0x3fe.Errno: %d, Reason: %s", errno, strerror(errno));
			VT_rv=TFAIL;
		}

		//double set slave addr=0x3fe,it should be failed
		if(ioctl(fd,I2C_SLAVE,0x3fe)==0){
			tst_resm(TFAIL,"------ERROR: double set slave addr=0x3fe passed,it should return fail.Errno: %d, Reason: %s", errno, strerror(errno));
			VT_rv=TFAIL;
		}

		//test slave addr=0x3ff with 10bit mode
		if(ioctl(fd,I2C_SLAVE,0x3ff)==0){
			tst_resm(TFAIL,"------ERROR: set an invalid slave addr=0x3ff,it should return fail. Errno: %d, Reason: %s", errno, strerror(errno));
			VT_rv=TFAIL;
		}

		//test ioctl I2C_SLAVE_FORCE
		if(ioctl(fd,I2C_SLAVE_FORCE,0x7e)<0){
			tst_resm(TFAIL,"------ERROR: with IOCTL I2C_SLAVE_FORCE.Errno: %d, Reason: %s", errno, strerror(errno));
			VT_rv=TFAIL;
		}	
	}

	//restore the test environment.
	//restore the i2c addr mode.
	if(funcs&I2C_FUNC_10BIT_ADDR) 
		ioctl(fd,I2C_TENBIT,0);

	print_result(VT_rv);
	
	return VT_rv;
}

int VT_i2c_test_ioctl_10bit(void * arg)
{
	unsigned long funcs=0;
	int VT_rv=TPASS;

	//set the parameter is invalid.
	arg=NULL;

	if(ioctl(fd,I2C_FUNCS,&funcs)<0){
		tst_resm(TFAIL, "------ERROR: with ioctl I2C_FUNCS. Error : %d, Reason: %s",errno,strerror(errno));
		VT_rv=TFAIL;
	}

	//test 10bit mode.
	if(funcs&I2C_FUNC_10BIT_ADDR){
		//test 10bit mode set
		if(ioctl(fd,I2C_TENBIT,1)<0){
			tst_resm(TFAIL,"------ERROR: Set 10bit mode failed!Errno: %d, Reason: %s", errno, strerror(errno));
			VT_rv=TFAIL;
		}
		//test non-10bit mode set
		if(ioctl(fd,I2C_TENBIT,0)<0){
			tst_resm(TFAIL,"------ERROR: Set non-10bit mode failed!Errno: %d, Reason: %s", errno, strerror(errno));
			VT_rv=TFAIL;
		}
	}
	else{
		tst_resm(TSKIP,"------ERROR: 10bit mode is not supported!");
		VT_rv=TSKIP;
	}

	print_result(VT_rv);

	return VT_rv;
}

int VT_i2c_test_ioctl_pec(void * arg)
{
	unsigned long funcs=0;
	int VT_rv=TPASS;

	//set the parameter is invalid.
	arg=NULL;

	if(ioctl(fd,I2C_FUNCS,&funcs)<0){
		tst_resm(TFAIL, "------ERROR: with ioctl I2C_FUNCS. Error : %d, Reason: %s",errno,strerror(errno));
		VT_rv=TFAIL;
	}

	//set pec mode.
	if(funcs&I2C_FUNC_SMBUS_PEC){
		//test pec mode set
		if(ioctl(fd,I2C_PEC,1)<0){
			tst_resm(TFAIL,"------ERROR: Set pec mode failed!Errno: %d, Reason: %s", errno, strerror(errno));
			VT_rv=TFAIL;
		}
		//test non-pec mode set
		if(ioctl(fd,I2C_PEC,0)<0){
			tst_resm(TFAIL,"------ERROR: Set non-pec mode failed!Errno: %d, Reason: %s", errno, strerror(errno));
			VT_rv=TFAIL;
		}
	}
	else{
		tst_resm(TSKIP,"------SKIP: pec mode is not supported!");
		VT_rv=TSKIP;
	}

	print_result(VT_rv);

	return VT_rv;
}

int VT_i2c_test_ioctl_timeout(void * arg)
{
	int VT_rv=TPASS;

	//set the parameter is invalid.
	arg=NULL;

	//test timeout=65535
	if(ioctl(fd,I2C_TIMEOUT,65535)<0){
		tst_resm(TFAIL,"------ERROR: set timeout=65535 failed!Errno: %d, Reason: %s", errno, strerror(errno));
		VT_rv=TFAIL;
	}

	//test timeout=0
	if(ioctl(fd,I2C_TIMEOUT,0)<0){
		tst_resm(TFAIL,"------ERROR: set timeout=0 failed! Errno: %d, Reason: %s", errno, strerror(errno));
		VT_rv=TFAIL;
	}

	print_result(VT_rv);

	return VT_rv;
}

int VT_i2c_test_ioctl_retries(void * arg)
{
	int VT_rv=TPASS;

	//set the parameter is invalid.
	arg=NULL;

	//test retries=65535
	if(ioctl(fd,I2C_RETRIES,65535)<0){
		tst_resm(TFAIL,"------ERROR: set retries=65535 failed!");
		VT_rv=TFAIL;
	}

	//test retries=0
	if(ioctl(fd,I2C_RETRIES,0)<0){
		tst_resm(TFAIL,"------ERROR: set retries=0 failed!");
		VT_rv=TFAIL;
	}

	print_result(VT_rv);

	return VT_rv;
}

int VT_i2c_test_ioctl_readwrite(void * arg)
{
	int VT_rv=TPASS;
	struct i2c_rdwr_ioctl_data msg_queue;
	int idx;

	//set the parameter is invalid
	arg=NULL;

	msg_queue.nmsgs = 2;

	for(idx=0;idx<msg_queue.nmsgs;++idx){
		(work_queue.msgs[idx]).len = 0;
		(work_queue.msgs[idx]).addr = slave_addr + idx;
		(work_queue.msgs[idx]).buf = NULL;
	}

	if (ioctl(fd, I2C_TIMEOUT, 2) < 0)        /* set the timeout    */
        {
                tst_resm(TFAIL, "------ERROR: with ioctl I2C_TIMEOUT. Errno: %d, Reason: %s", errno, strerror(errno));
                VT_rv = TFAIL;
        }

        if (ioctl(fd, I2C_RETRIES, 1) < 0)        /* set the retries    */
        {
                tst_resm(TFAIL, "------ERROR: with ioctl I2C_RETRIES. Errno: %d, Reason: %s", errno, strerror(errno));
                VT_rv = TFAIL;
        }
 
        if (ioctl(fd, I2C_RDWR, (unsigned long)&work_queue) < 0)
        {
                tst_resm(TFAIL, "------ERROR: with ioctl I2C_RDWR. Errno: %d, Reason: %s", errno, strerror(errno));
                VT_rv = TFAIL;
        }

	print_result(VT_rv);

	return VT_rv;
}

int VT_i2c_test_smbus(void * arg)
{
	int VT_rv=TPASS;
	unsigned long funcs;
	//int res;

	__u8 test_regedit=0x10; //command
	__u8 test_data_byte=0x10;        //value
	__u16 test_data_word=0x10;     //regedit 
	__u8 test_data_block[32]={0x10};
	
	unsigned long type=(unsigned long)arg;

	if (ioctl(fd, I2C_SLAVE_FORCE, slave_addr) < 0)       
        {
                tst_resm(TFAIL, "------ERROR: with ioctl I2C_SLAVE. Errno: %d, Reason: %s", errno, strerror(errno));
                VT_rv = TFAIL;
        }

	if (ioctl(fd, I2C_TIMEOUT, 1) < 0)        /* set the timeout    */
        {
                tst_resm(TFAIL, "------ERROR: with ioctl I2C_TIMEOUT. Errno: %d, Reason: %s", errno, strerror(errno));
                VT_rv = TFAIL;
        }

        if (ioctl(fd, I2C_RETRIES, 1) < 0)        /* set the retries    */
        {
                tst_resm(TFAIL, "------ERROR: with ioctl I2C_RETRIES. Errno: %d, Reason: %s", errno, strerror(errno));
                VT_rv = TFAIL;
        }
#if 0
	if(ioctl(fd,I2C_PEC,0)<0){
		tst_resm(TFAIL,"------ERROR: Set pec mode failed!Errno: %d, Reason: %s", errno, strerror(errno));
		VT_rv=TFAIL;
	}
#endif 
	if(ioctl(fd,I2C_FUNCS,&funcs)<0){
		tst_resm(TFAIL, "------ERROR: with ioctl I2C_FUNCS. Error : %d, Reason: %s",errno,strerror(errno));
		VT_rv=TFAIL;
	}

	if(!(funcs&(unsigned long)type)){
		tst_resm(TINFO, "------SKIP: the smbus r/w mode is not supported.");
		return TSKIP;
	}

	switch(type){
		case I2C_FUNC_SMBUS_QUICK:
			if(i2c_smbus_write_quick(fd, test_data_byte)<0){
				tst_resm(TFAIL,"------ERROR: using i2c_smbus_write_quick() to write data failed!Error : %d, Reason: %s",errno,strerror(errno));
				return TFAIL;
			}
			return TPASS;			

		case I2C_FUNC_SMBUS_READ_BYTE:
			if(i2c_smbus_read_byte(fd)<0){
				tst_resm(TFAIL,"------ERROR: using i2c_smbus_read_byte() to read data failed!Error : %d, Reason: %s",errno,strerror(errno));
				return TFAIL;
			}
			return TPASS;

		case I2C_FUNC_SMBUS_WRITE_BYTE:
			if(i2c_smbus_write_byte(fd,test_data_byte)<0){
				tst_resm(TFAIL,"------ERROR: using i2c_smbus_write_byte() to write data failed!Error : %d, Reason: %s",errno,strerror(errno));
				return TFAIL;
			}
			return TPASS;

		case I2C_FUNC_SMBUS_READ_BYTE_DATA:
			if(i2c_smbus_read_byte_data(fd,test_regedit)<0){
				tst_resm(TFAIL,"------ERROR: using i2c_smbus_read_byte_data() to read data failed!Error : %d, Reason: %s",errno,strerror(errno));
				return TFAIL;
			}
			return TPASS;

		case I2C_FUNC_SMBUS_WRITE_BYTE_DATA:
			if(i2c_smbus_write_byte_data(fd,test_regedit,test_data_byte)<0){
				tst_resm(TFAIL,"------ERROR: using i2c_smbus_write_byte_data() to write data failed!Error : %d, Reason: %s",errno,strerror(errno));
				return TFAIL;
			}
			return TPASS;

		case I2C_FUNC_SMBUS_READ_WORD_DATA:
			if(i2c_smbus_read_word_data(fd,test_regedit)<0){
				tst_resm(TFAIL,"------ERROR: using i2c_smbus_read_word_data() to read data failed!Error : %d, Reason: %s",errno,strerror(errno));
				return TFAIL;
			}
			return TPASS;

		case I2C_FUNC_SMBUS_WRITE_WORD_DATA:
			if(i2c_smbus_write_word_data(fd,test_regedit,test_data_word)<0){
				tst_resm(TFAIL,"------ERROR: using i2c_smbus_write_word_data() to write data failed!Error : %d, Reason: %s",errno,strerror(errno));
				return TFAIL;
			}
			return TPASS;

		case I2C_FUNC_SMBUS_PROC_CALL:
			if(i2c_smbus_process_call(fd,test_regedit,test_data_word)<0){
				tst_resm(TFAIL,"------ERROR: using i2c_smbus_process_call() to write data failed!Error : %d, Reason: %s",errno,strerror(errno));
				return TFAIL;
			}
			return TPASS;			

		case I2C_FUNC_SMBUS_READ_BLOCK_DATA:
			if(i2c_smbus_read_block_data(fd,test_regedit,NULL)<0){
				tst_resm(TFAIL,"------ERROR: using i2c_smbus_read_block_data() to read data failed!Error : %d, Reason: %s",errno,strerror(errno));
				return TFAIL;
			}
			return TPASS;

		case I2C_FUNC_SMBUS_WRITE_BLOCK_DATA:
			if(i2c_smbus_write_block_data(fd,test_regedit,0,test_data_block)<0){
				tst_resm(TFAIL,"------ERROR: using i2c_smbus_write_block_data() to write data failed!Error : %d, Reason: %s",errno,strerror(errno));
				return TFAIL;
			}
			return TPASS;

		case I2C_FUNC_SMBUS_READ_I2C_BLOCK:
			if(i2c_smbus_read_i2c_block_data(fd,test_regedit,0,NULL)<0){
				tst_resm(TFAIL,"------ERROR: using i2c_smbus_read_i2c_block_data() to read data failed!Error : %d, Reason: %s",errno,strerror(errno));
				return TFAIL;
			}
			return TPASS;

		case I2C_FUNC_SMBUS_WRITE_I2C_BLOCK:
			if(i2c_smbus_write_i2c_block_data(fd,test_regedit,0,test_data_block)<0){
				tst_resm(TFAIL,"------ERROR: using i2c_smbus_write_i2c_block_data() to write data failed!Error : %d, Reason: %s",errno,strerror(errno));
				return TFAIL;
			}
			return TPASS;
			
		case I2C_FUNC_SMBUS_BLOCK_PROC_CALL:
			if(i2c_smbus_block_process_call(fd,test_regedit,0,test_data_block)<0){
				tst_resm(TFAIL,"------ERROR: using i2c_smbus_write_i2c_block_data() to write data failed!Error : %d, Reason: %s",errno,strerror(errno));
				return TFAIL;
			}
			return TPASS;

		default:
			tst_resm(TFAIL,"------ERROR: the type value is override!");
			return TFAIL;
	}

	print_result(VT_rv);
	
	return VT_rv;
}
