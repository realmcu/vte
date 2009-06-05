#include "i2c_test.h"
//#include "i2c_smbus.h"


int fd=-1;

struct i2c_rdwr_ioctl_data msg_queue;   /*used in ioctl rw test. */


/*
global variables
*/

int     TST_TOTAL = 1;                 /* total number of tests in this file.  */
char* TCID="i2c_test";

extern int   Tst_count;        /* counter for tst_xxx routines.  */
extern char *TESTDIR;          /* temporary dir created by tst_tmpdir */
	


int    Dflag  = 0;
int    Nflag = 0;
int    Sflag = 0;
int	Rflag = 0;
char  *Dopt;
char   *Nopt;
char   *Sopt;
char    *Ropt;
		
char    device_name[128];

option_t options[] =
{
        {"d:", &Dflag, &Dopt},
        {"n:", &Nflag, &Nopt},
        {"s:", &Sflag, &Sopt},
        {"r:", &Rflag, &Ropt},
        {NULL, NULL, NULL}
};

void cleanup(void)
{
        /* VTE : Actions needed to get a stable target environment */
        int     VT_rv =-1;

	//printf("Close I2C bus device.");
        tst_resm(TINFO, "Close I2C bus device.");

	  if (fd > 0)
               VT_rv = close(fd);
	
        if (VT_rv !=0)
        {
		//printf("cleanup() Failed : error code = %d", VT_rv);
		tst_resm(TWARN, "cleanup() Failed : error code = %d", VT_rv);
        }

        /* Exit with appropriate return code. */
        tst_exit();
}


void help(void)
{
        printf("====================================================\n");
        printf("\t-d x        Device name, e.g. /dev/i2c-0        (def)\n");
        printf("\t-n x        test case number: 0-read/write; 1-ioctl(read/write);2-smbus read/write;\n 3-only read;4-only read using smbus;5-ioctl test\n");
        printf("\t-s x        slave address\n");
	 printf("\t-r x        register address\n");
        printf("\nUsage: %s [-d device_name] [-n case number] [-s slave address] [-r register address]\n\n", TCID);
        printf("\t-d [/dev/i2c/0, /dev/i2c_module, /dev/test_i2c_module]\n");
        printf("\t-n 0/1/2/3/4/5\n");
        printf("\t -s 0xNN\n");
	 printf("\t -r 0xNN\n");
}


/*
open i2c device
allocate memory for msg_queue
*/


int setup(void)
{
     

	//printf("Open I2C bus device...");
	tst_resm(TINFO, "Open I2C bus device...");
        /* VTE : Actions needed to prepare the test running */

	
        fd = open(device_name, O_RDWR);
		
        if ((fd) < 0)
        {
		//printf("VT_i2c_setup() Failed open device");
		tst_resm(TFAIL, "VT_i2c_setup() Failed open device");
				return 0;
        }

	msg_queue.nmsgs = MSG_LENTH;

        msg_queue.msgs = (struct i2c_msg *) malloc(msg_queue.nmsgs *sizeof(struct i2c_msg));
        if (!msg_queue.msgs)
        {
		//printf("VT_i2c_setup() Failed allocate memory");
		tst_resm(TFAIL, "VT_i2c_setup() Failed allocate memory");
				return 0;
        }
		 
        return 1;
}

/*
read/write test.

*/

int i2c_test_rw()
{
	char  buf[10];
	int write_result=-1;
	int read_result = -1;

	buf[0] = reg_addr;  
	buf[1] = 0x43; 
	buf[2] = 0x65;


	if (ioctl(fd, I2C_SLAVE, slave_addr) != 0)        /* set the slave address */
        {
		//printf("ERROR: with ioctl I2C_SLAVE. Errno: %d, Reason: %s",errno, strerror(errno));
		tst_resm(TFAIL, "ERROR: with ioctl I2C_SLAVE. Errno: %d, Reason: %s", errno, strerror(errno));
                return 0;
        }
	if (ioctl(fd, I2C_TIMEOUT, 1) != 0)        /* set the timeout    */
        {
		//printf("ERROR: with ioctl I2C_TIMEOUT. Errno: %d, Reason: %s", errno, strerror(errno));
		tst_resm(TFAIL, "ERROR: with ioctl I2C_TIMEOUT. Errno: %d, Reason: %s", errno, strerror(errno));
                return 0;
        }

	if (ioctl(fd, I2C_RETRIES, 1) != 0)        /* set the retries    */
        {
		//printf("ERROR: with ioctl I2C_RETRIES. Errno: %d, Reason: %s", errno, strerror(errno));
		tst_resm(TFAIL, "ERROR: with ioctl I2C_RETRIES. Errno: %d, Reason: %s", errno, strerror(errno));
                return 0;
        }

	if((write_result=write(fd, buf, 2))<0)    //if the device's register has one byte, use 2, if two bytes, use 3. now tsc2007 which has 
									  //one byte is supposed to use. so we use 2.
	{
		//printf("write operation fails!");
		printf("write data number: %d\n", write_result);
		tst_resm(TFAIL, "write operation fails!");
		return 0;
	}
	if((read_result=read(fd, buf, 1))<0)
	{
		//printf("read operation fails!");
		printf("read data number: %d\n",read_result);
		tst_resm(TFAIL, "read operation fails!");
		return 0;
	}
	printf("buf content after read: %x\n", buf[0]);

	return 1;

	
}

/*
read/write using ioctl.

*/

int i2c_test_ioctl_rw()
{
	int idx;
	__u8 buf[10*MSG_LENTH];
	unsigned long *funcs;
	
	if(ioctl(fd,I2C_FUNCS,funcs)==0)
	{

	for(idx=0; idx<MSG_LENTH; idx++)
	{
		(msg_queue.msgs[idx]).len = 1;
		(msg_queue.msgs[idx]).addr = slave_addr;
		(msg_queue.msgs[idx]).buf = &buf[idx*MSG_LENTH];
	}
	
	if (ioctl(fd, I2C_SLAVE, slave_addr) != 0)        /* set the salve address */
        {
		//printf("ERROR: with ioctl I2C_SLAVE. Errno: %d, Reason: %s", errno, strerror(errno));
		tst_resm(TFAIL, "ERROR: with ioctl I2C_SLAVE. Errno: %d, Reason: %s", errno, strerror(errno));
                return 0;
        }
	if (ioctl(fd, I2C_TIMEOUT, 1) != 0)        /* set the timeout    */
        {
		//printf("ERROR: with ioctl I2C_TIMEOUT. Errno: %d, Reason: %s", errno, strerror(errno));
		tst_resm(TFAIL, "ERROR: with ioctl I2C_TIMEOUT. Errno: %d, Reason: %s", errno, strerror(errno));
                return 0;
        }

	if (ioctl(fd, I2C_RETRIES, 1) != 0)        /* set the retries    */
        {
		//printf("ERROR: with ioctl I2C_RETRIES. Errno: %d, Reason: %s", errno, strerror(errno));
		tst_resm(TFAIL, "ERROR: with ioctl I2C_RETRIES. Errno: %d, Reason: %s", errno, strerror(errno));
                return 0;
        }

	if(ioctl(fd,I2C_RDWR,&msg_queue)<0)
	{
		//printf("ERROR: with ioctl I2C_RDWR. Errno: %d, Reason: %s", errno, strerror(errno));
		tst_resm(TFAIL, "ERROR: with ioctl I2C_RDWR. Errno: %d, Reason: %s", errno, strerror(errno));
                return 0;
	}
	return 1;
	}

	else
	{
		tst_resm(TWARN, "Only valid if the adapter has I2C_FUNC_I2C. ");
		return 0;
	}

	
}

/*
read/write using smbus.

*/

int i2c_test_smbus()
{
	 

	if (ioctl(fd, I2C_SLAVE, slave_addr) != 0)        /* set the eeprom address */
        {
                tst_resm(TFAIL, "ERROR: with ioctl I2C_SLAVE. Errno: %d, Reason: %s", errno, strerror(errno));
                return 0;
        }
	if (ioctl(fd, I2C_TIMEOUT, 1) != 0)        /* set the timeout    */
        {
                tst_resm(TFAIL, "ERROR: with ioctl I2C_TIMEOUT. Errno: %d, Reason: %s", errno, strerror(errno));
                return 0;
        }

	if (ioctl(fd, I2C_RETRIES, 1) != 0)        /* set the retries    */
        {
                tst_resm(TFAIL, "ERROR: with ioctl I2C_RETRIES. Errno: %d, Reason: %s", errno, strerror(errno));
                return 0;
        }

	if(i2c_smbus_write_word_data(fd,reg_addr,0x6543)<0)
	{
		tst_resm(TFAIL, "write though smbus fails!");
		return 0;
	}

	if(i2c_smbus_read_word_data(fd,reg_addr)<0)
	{
		tst_resm(TFAIL, "read though smbus fails!");
		return 0;
	}

	return 1;
	
}

/*
only read. since some slave device can't be written.

*/


int i2c_test_read()
{
	char buf[10];
	int read_result = -1;

	buf[0] = reg_addr;  
	buf[1] = 0x43; 
	buf[2] = 0x65;

	if (ioctl(fd, I2C_SLAVE, slave_addr) != 0)        /* set the slave address */
        {
                tst_resm(TFAIL, "ERROR: with ioctl I2C_SLAVE. Errno: %d, Reason: %s", errno, strerror(errno));
                return 0;
        }
	if (ioctl(fd, I2C_TIMEOUT, 1) != 0)        /* set the timeout    */
        {
                tst_resm(TFAIL, "ERROR: with ioctl I2C_TIMEOUT. Errno: %d, Reason: %s", errno, strerror(errno));
                return 0;
        }

	if (ioctl(fd, I2C_RETRIES, 1) != 0)        /* set the retries    */
        {
                tst_resm(TFAIL, "ERROR: with ioctl I2C_RETRIES. Errno: %d, Reason: %s", errno, strerror(errno));
                return 0;
        }

	if((read_result=read(fd, buf, 2))<0)
	{
		printf("read data number: %d\n", read_result);
		printf("buf content after read: %x  %x\n", buf[0], buf[1]);
		tst_resm(TFAIL, "read operation fails!");
		return 0;
	}
	printf("buf content after read: %x  %x\n", buf[0], buf[1]);
	return 1;
}

/*
read data using smbus

*/

int i2c_test_smbus_read()
{
	if (ioctl(fd, I2C_SLAVE, slave_addr) != 0)        /* set the eeprom address */
        {
                tst_resm(TFAIL, "ERROR: with ioctl I2C_SLAVE. Errno: %d, Reason: %s", errno, strerror(errno));
                return 0;
        }
	if (ioctl(fd, I2C_TIMEOUT, 1) != 0)        /* set the timeout    */
        {
                tst_resm(TFAIL, "ERROR: with ioctl I2C_TIMEOUT. Errno: %d, Reason: %s", errno, strerror(errno));
                return 0;
        }

	if (ioctl(fd, I2C_RETRIES, 1) != 0)        /* set the retries    */
        {
                tst_resm(TFAIL, "ERROR: with ioctl I2C_RETRIES. Errno: %d, Reason: %s", errno, strerror(errno));
                return 0;
        }

	if(i2c_smbus_read_word_data(fd,reg_addr)<0)
	{
		tst_resm(TFAIL, "read though smbus fails!");
		return 0;
	}
	return 1;
	
}

/*
ioctl api test.

*/

int i2c_test_ioctl()
{
	int  VT_rv =1;
	 if(i2c_test_ioctl_tenbit()==0)
	 {
	 	tst_resm(TFAIL, "i2c_test_ioctl_tenbit test fail!!!");
		VT_rv =0 ;
	 }
	 if(i2c_test_ioctl_pec()==0)
	 {
	 	tst_resm(TFAIL, "i2c_test_ioctl_pec test fail!!!");
		VT_rv =0 ;
	 }
	 if(i2c_test_ioctl_funcs()==0)
	 {
	 	tst_resm(TFAIL, "i2c_test_ioctl_funcs test fail!!!");
		VT_rv =0 ;
	 }
	 if(i2c_test_ioctl_retries()==0)
	 {
	 	tst_resm(TFAIL, "i2c_test_ioctl_retries test fail!!!");
		VT_rv =0 ;
	 }
	 if(i2c_test_ioctl_timeout()==0)
	 {
	 	tst_resm(TFAIL, "i2c_test_ioctl_timeout test fail!!!");
		VT_rv =0 ;
	 }

	 
	return VT_rv;

}

/*
I2C_TENBIT test
*/

int i2c_test_ioctl_tenbit()
{
	int VT_rv = 1;

	#ifdef I2C_FUNC_10BIT_ADDR
	if(ioctl(fd,I2C_TENBIT, 0)!=0)
	{
		tst_resm(TFAIL, "fail to set ten bit address ");	
		VT_rv = 0;
	}
	if(ioctl(fd,I2C_TENBIT, 1)!=0)
	{
		tst_resm(TFAIL, "fail to set seven bit address ");	
		VT_rv = 0;
	}
	#elif 
	tst_resm(TFAIL, " your request for setting bit address is invalid");
	VT_rv = 0;
	#endif
	return VT_rv;
}


/*
pec ioctl test

*/

int i2c_test_ioctl_pec()
{
	int VT_rv = 1;

	#ifdef I2C_FUNC_SMBUS_PEC
	if(ioctl(fd,I2C_PEC, 1)!=0)
	{
		tst_resm(TFAIL, "fail to enable SMBUS PEC ");	
		VT_rv = 0;
	}
	if(ioctl(fd,I2C_PEC, 0)!=0)
	{
		tst_resm(TFAIL, "fail to disnable SMBUS PEC ");	
		VT_rv = 0;
	}
	#elif 
	tst_resm(TFAIL, " your request for enable/disnable SMBUS PEC is  invalid");
	VT_rv = 0;
	#endif
	return VT_rv;
}

/*
funcs ioctl test
*/

int i2c_test_ioctl_funcs()
{
	int VT_rv = 1;

	unsigned long funcs;
	if(ioctl(fd,I2C_FUNCS,&funcs)!=0)
	{
		tst_resm(TFAIL, "fail to I2C_FUNCS ioctl ");	
		VT_rv = 0;	
	}
	return VT_rv;
	
}

/*

retry number setting ioctl test
*/


int i2c_test_ioctl_retries()
{
	int VT_rv = 1;
	if(ioctl(fd,I2C_RETRIES,2)<0)
	{
		tst_resm(TFAIL, "fail to I2C_RETRIES ioctl ");	
		VT_rv = 0;
	}
	return VT_rv;
}

/*
timeout setting ioctl test
*/

int i2c_test_ioctl_timeout()
{
	int VT_rv = 1;
	if(ioctl(fd,I2C_TIMEOUT,2)<0)
	{
		tst_resm(TFAIL, "fail to I2C_TIMEOUT ioctl ");	
		VT_rv = 0;
	}
	return VT_rv;
}

/*
select the function to excute from this function

*/

int i2c_test()
{
	int VT_rv = 1;
	switch (case_number)
        {
        case 0:
                tst_resm(TINFO, "Read/Write from I2C character device interface from user space");
                VT_rv = i2c_test_rw();
                break;
        case 1:
                tst_resm(TINFO, "Send the I2C messages using kernel ioctls with I2C_RDWR");
                VT_rv = i2c_test_ioctl_rw();
                break;
	case 2:
		  tst_resm(TINFO, "read/write using SMBUS");
		  VT_rv = i2c_test_smbus();
		  break;
	case 3:
		   tst_resm(TINFO, "only read ");
		   VT_rv = i2c_test_read();
		   break;
	case 4:
		   tst_resm(TINFO, "only read using SMBUS");
		   VT_rv = i2c_test_smbus_read();
		   break;
	case 5:
		   tst_resm(TINFO, "ioctl api test");
		   VT_rv = i2c_test_ioctl();
		   break;
		   
        default:
                tst_resm(TFAIL, "Operation not permitted");
                break;
        }
	return VT_rv;
}


int main(int argc, char **argv)
{
	int result=1;
	char * msg;

/*
parse the parameters
*/

	if ((msg = parse_opts(argc, argv, options, &help)) != (char *) NULL)
        {
                tst_resm(TFAIL, "OPTION PARSING ERROR - %s", msg);
                return TFAIL;
        }

	 if (Dflag)
        {
                strcpy(device_name, Dopt);
        }
        else
        {
                strcpy(device_name, DEFAULT_I2C_NAME);
        }

	if(Nflag)
	{
		case_number = atoi(Nopt);
		if(case_number<0 || case_number>5)
		{
			tst_resm(TINFO, "invalid parameter: -n %d ", Nopt);
			help();
			return 0;
		}
	}
	else
	{
		case_number = DEFAULT_CASE_NUMBER;
		tst_resm(TINFO, "-n is not point out, now we use the default case number: %d", DEFAULT_CASE_NUMBER);
	}



	if(Sflag)
        {
                if(sscanf(Sopt, "0x%x", &slave_addr) != 1)
                {
                        tst_resm(TFAIL, "Cannot parse %s as addrs., example: 0x01", Sopt);
                        return 0;
                }
		 printf("slave address: %x\n",slave_addr);
		 printf("slave address: %d\n",slave_addr);
        }

	else
	{
		slave_addr = DEFAULT_SLAVE_ADDR;
		tst_resm(TINFO, "-s is not point out, now we use the default slave address: %x", DEFAULT_SLAVE_ADDR);
	}

	if(Rflag)
        {
                if(sscanf(Ropt, "0x%x", &reg_addr) != 1)
                {
                        tst_resm(TFAIL, "Cannot parse %s as addrs., example: 0x01", Ropt);
                        return 0;
                }
		 printf("register address: %x\n",reg_addr);
        }
	else
	{
		reg_addr = DEFAULT_REG_ADDR;
		tst_resm(TINFO, "-r is not point out, now we use the default register address: %x", DEFAULT_REG_ADDR);
	}


/*
preparation
*/

	result=setup();
	if(result == 0)
	{
		tst_resm(TFAIL, "setup() fail!");
		printf("====================================");		
		tst_resm(TFAIL, "%s test case did NOT work as expected", TCID);
		printf("====================================");
		cleanup();
		return 0;
	}

/*
start test
*/


	result = i2c_test();
	if(result == 0)
	{
		
		printf("====================================\n");		
		tst_resm(TFAIL, "%s test case did NOT work as expected", TCID);
		printf("====================================\n");
		
	}
	else
	{
		
		printf("====================================\n");		
		tst_resm(TPASS, "%s test case works as expected", TCID);
		printf("====================================\n");
		
	}
	cleanup();
	
	return 0;
}
