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
        @file   i2c_main.c

        @brief  First I2C test main function.
====================================================================================================
Revision History:
                            Modification     Tracking
Author/core ID                  Date          Number    Description of Changes
-------------------------   ------------    ----------  --------------------------------------------
V.Khalabuda/hlbv001          21/07/2004     TLSbo40419  I2C test development
V.Khalabuda/hlbv001          28/02/2005     TLSbo45061  Review I2C test development
S.V-Guilhou/svan01c          20/09/2005     TLSbo53753  I/O errors
V.Khalabuda/b00306           04/07/2006     TLSbo68945  Update testapp for I2C_RDWR ioctl
                                                        and r/w from user space

====================================================================================================
Portability:  ARM GCC
==================================================================================================*/

/*==================================================================================================
Total Tests: 1

Test Executable Name:  i2c_testapp_3

Test Assertion
& Strategy:  A test for MXC I2C
=================================================================================================*/

/*==================================================================================================
                                        INCLUDE FILES
==================================================================================================*/
/* Verification Test Environment Include Files */
#include "i2c_test.h"

/*==================================================================================================
                                        LOCAL MACROS
==================================================================================================*/

/*==================================================================================================
                                        LOCAL VARIABLES
==================================================================================================*/

/*==================================================================================================
                            LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
==================================================================================================*/

/*==================================================================================================
                                        LOCAL CONSTANTS
==================================================================================================*/

/*==================================================================================================
                                        GLOBAL CONSTANTS
==================================================================================================*/
int     Dflag = 0,
        Aflag = 0,
        Xflag = 0;
char   *Dopt,
       *Aopt,
       *Xopt;
char    device_name[128];

option_t options[] =
{
        {"D:", &Dflag, &Dopt},
        {"A:", &Aflag, &Aopt},
        {"T:", &Xflag, &Xopt},
        {NULL, NULL, NULL}
};

//added by devil
struct vte_function_table {
	char * descriptions;   //case description
	int depth;  //log depth
	unsigned long usrdata; // user data
	unsigned long uniqueID; //Unique ID
	int (*testproc) (void *); //point of testcase function
};

/*==================================================================================================
                                        GLOBAL VARIABLES
==================================================================================================*/
/* Extern Global Variables */
extern int   Tst_count;        /* counter for tst_xxx routines.  */
extern char *TESTDIR;          /* temporary dir created by tst_tmpdir */

/* Global Variables */
char   *TCID = "i2c_api_test";        /* test program identifier.  */
int     TST_TOTAL = 1;                 /* total number of tests in this file.  */

char    device_name[128];
char    selected_cases[128];
unsigned int slave_addr;


/*==================================================================================================
                                    GLOBAL FUNCTION PROTOTYPES
==================================================================================================*/
void    cleanup(void);
void    setup(void);
int     main(int argc, char **argv);

/*==================================================================================================
                                    LOCAL FUNCTION PROTOTYPES
==================================================================================================*/
void    help(void);
int parse_selected_case(unsigned long i,char * str);

/*==================================================================================================
                                        GLOBAL FUNCTIONS
==================================================================================================*/

/*================================================================================================*/
/*===== cleanup =====*/
/**
@brief  Performs all one time clean up for this test on successful completion,
        premature exit or  failure. Closes all temporary files,
        removes all temporary directories exits the test with
        appropriate return code by calling tst_exit() function.cleanup

@param  Input :      None.
        Output:      None.
    
@return Nothing
*/
/*================================================================================================*/
void cleanup(void)
{
        /* VTE : Actions needed to get a stable target environment */
        int     VT_rv = TFAIL;

        tst_resm(TINFO, "Close I2C bus device.");
        VT_rv = VT_i2c_cleanup();
        if (VT_rv != TPASS)
        {
                tst_resm(TWARN, "VT_cleanup() Failed : error code = %d", VT_rv);
        }

        /* Exit with appropriate return code. */
        tst_exit();
}

/*==================================================================================================
                                        LOCAL FUNCTIONS
==================================================================================================*/

int parse_selected_case(unsigned long i,char * str)
{
	char tmp[128];
	char casenum[10]="";
	char * ptmp;

	strcpy(tmp,str);
	ptmp=tmp;
	

	if(strcmp(tmp,"ALL")) return TRUE;
	
	while(*ptmp){
		while((*ptmp)<='0'||(*ptmp)>='9') ptmp++;

		strcpy(casenum,"");
		while((*ptmp)>='0'&&(*ptmp)<='9') strcat(casenum,*ptmp++);

		if(atoi(casenum)<i){
			while((*ptmp)==' ') ptmp++;
			if((*ptmp)=='-'){
				strcpy(casenum,"");
				while((*ptmp)>='0'&&(*ptmp)<='9') strcat(casenum,*ptmp++);
				if(atoi(casenum)>=i) return TRUE;
			}
			continue;
		}
		else if(atoi(casenum)>i) continue;
		else return TRUE;
	}
	return FALSE;
}

/*================================================================================================*/
/*===== help =====*/
/**
@brief  Inform of the available options and the associated parameters

@param  Input :      None.
        Output:      None.
    
@return Nothing
*/
/*================================================================================================*/
void help(void)
{
#if 0
        printf("====================================================\n");
        printf("\t-D x        Device name, e.g. /dev/i2c/0        (def)\n");
        printf("\t-A x        Memory/Start address to access(def, hex)\n");
        printf("\t-T x        Testcase name\n");
        printf("\nUsage: %s [-D device_name] [-A base_address] [-T type_of_test]\n\n", TCID);
        printf("\t-D [/dev/i2c/0, /dev/i2c_module, /dev/test_i2c_module]\n");
        printf("\t-A [0xNN]\n");
        printf("\t-T [%d - %d]\n", TEST_I2C_RW, TEST_I2C_IOCTL);
#endif
}

/*================================================================================================*/
/*===== setup =====*/
/**
@brief  Performs all one time setup for this test. This function is
        typically used to capture signals, create temporary dirs
                                and temporary files that may be used in the course of this test.

@param  Input :      None.
        Output:      None.
    
@return On failure - Exits by calling cleanup().
        On success - returns 0.
*/
/*================================================================================================*/
void setup(void)
{
        int     VT_rv = TFAIL;

        tst_resm(TINFO, "Open I2C bus device...");
        /* VTE : Actions needed to prepare the test running */
        VT_rv = VT_i2c_setup();
        if (VT_rv != TPASS)
        {
                tst_brkm(TBROK, cleanup, "VT_i2c_setup() Failed : error code = %d", VT_rv);
        }

        return;
}

/*================================================================================================*/
/*===== main =====*/
/**
@brief  Entry point to this test-case. It parses all the command line
        inputs, calls the global setup and executes the test. It logs
            the test status and results appropriately using the LTP API's
        On successful completion or premature failure, cleanup() func
        is called and test exits with an appropriate return code.

@param  Input :      argc - number of command line parameters.
        Output:      **argv - pointer to the array of the command line parameters.
                                Describe input arguments to this test-case
                                -l - Number of iteration
                                -v - Prints verbose output
                                -V - Prints the version number
    
@return On failure - Exits by calling cleanup().
        On success - exits with 0 exit value.
*/
/*================================================================================================*/
int main(int argc, char **argv)
{
       int     VT_rv = TFAIL;
       char   *msg;
	int idx;
	int cpass,cfail,cskip;
	char *space="-";
	char prefix_space[20];
	int i;

	struct vte_function_table i2c_vft[]=
	{
				{"Start I2C API Test",0,0,0,NULL},
				{"Part1:I2C IOCTL Test",1,0,0,NULL},
				{"Test I2C Capability.",2,0,1001,VT_i2c_test_capability},
				{"Test I2C IOCTL I2C_SLAVE&I2C_SLAVE_FORCE",2,0,1002,VT_i2c_test_ioctl_slave},
				{"Test I2C IOCTL I2C_TENBIT",2,0,1003,VT_i2c_test_ioctl_10bit},
				{"Test I2C IOCTL I2C_PEC",2,0,1004,VT_i2c_test_ioctl_pec},
				{"Test I2C IOCTL I2C_TIMEOUT",2,0,1005,VT_i2c_test_ioctl_timeout},
				{"Test I2C IOCTL I2C_RETRIES",2,0,1006,VT_i2c_test_ioctl_retries},
				{"Test I2C IOCTL I2C_RDWR",2,0,1007,VT_i2c_test_ioctl_readwrite},
#if 0
				{"Part2:I2C SMBUS Test",1,0,0,NULL},
				{"Test i2c_smbus_write_quick()",2,I2C_FUNC_SMBUS_QUICK,2001,VT_i2c_test_smbus},
				{"Test i2c_smbus_read_byte()",2,I2C_FUNC_SMBUS_READ_BYTE,2002,VT_i2c_test_smbus},
				{"Test i2c_smbus_write_byte()",2,I2C_FUNC_SMBUS_WRITE_BYTE,2003,VT_i2c_test_smbus},
				{"Test i2c_smbus_read_byte_data()",2,I2C_FUNC_SMBUS_READ_BYTE_DATA,2004,VT_i2c_test_smbus},
				{"Test i2c_smbus_write_byte_data()",2,I2C_FUNC_SMBUS_WRITE_BYTE_DATA,2005,VT_i2c_test_smbus},
				{"Test i2c_smbus_read_word_data()",2,I2C_FUNC_SMBUS_READ_WORD_DATA,2006,VT_i2c_test_smbus},
				{"Test i2c_smbus_write_word_data()",2,I2C_FUNC_SMBUS_WRITE_WORD_DATA,2007,VT_i2c_test_smbus},
				{"Test i2c_smbus_process_call()",2,I2C_FUNC_SMBUS_PROC_CALL,2008,VT_i2c_test_smbus},
				{"Test i2c_smbus_read_block_data()",2,I2C_FUNC_SMBUS_READ_BLOCK_DATA,2009,VT_i2c_test_smbus},
				{"Test i2c_smbus_write_block_data()",2,I2C_FUNC_SMBUS_WRITE_BLOCK_DATA,2010,VT_i2c_test_smbus},
				{"Test i2c_smbus_read_i2c_block_data()",2,I2C_FUNC_SMBUS_READ_I2C_BLOCK,2011,VT_i2c_test_smbus},
				{"Test i2c_smbus_write_i2c_block_data()",2,I2C_FUNC_SMBUS_WRITE_I2C_BLOCK,2012,VT_i2c_test_smbus},
				{"Test i2c_smbus_block_process_call()",2,I2C_FUNC_SMBUS_BLOCK_PROC_CALL,2013,VT_i2c_test_smbus},
				{"Finish I2C API Test",0,0,0,NULL},
				{NULL,0,0,0,NULL}
#endif
	};

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
                strcpy(device_name, "/dev/"DEFAULT_I2C_BUS);
        }

	if(Xflag)
	{
		strcpy(selected_cases, Xopt);
	}
	else
	{
		strcpy(selected_cases,"ALL");
	}

	if(Aflag)
        {
                if(sscanf(Aopt, "0x%x", &slave_addr) != 1)
                {
                        tst_resm(TFAIL, "Cannot parse %s as addrs., example: 0x01", Aopt);
                        return TFAIL;
                }
        }
        else
        {
                tst_resm(TFAIL, "-A must be specified");
                return TFAIL;
        }

        /* perform global test setup, call setup() function. */
        setup();

        /* Print test Assertion using tst_resm() function with argument TINFO. */
        tst_resm(TINFO, "Testing if %s test case is OK", TCID);

	cpass=cfail=cskip=0;

	for(idx=0;i2c_vft[idx].descriptions;idx++)
	{
		//if(parse_selected_case(i2c_vft[idx].uniqueID,selected_cases)==TRUE){
			//print the description
			strcpy(prefix_space,"");
			for(i=0;i<=i2c_vft[idx].depth;i++)	strcat(prefix_space,space);			

			tst_resm(TINFO,"%s%s",prefix_space,i2c_vft[idx].descriptions);
		#if 1
			//run the test proc
			if(i2c_vft[idx].testproc!=NULL)
				VT_rv=i2c_vft[idx].testproc((void *)i2c_vft[idx].usrdata);
		
			if(VT_rv==TPASS) cpass++;
			else if(VT_rv==TFAIL) cfail++;
			else cskip++;
		#endif
		//}

	}

	tst_resm(TINFO,"Test Results:");
	tst_resm(TINFO,"Pass:%d",cpass);
	tst_resm(TINFO,"Fail:%d",cfail);
	tst_resm(TINFO,"Skip:%d",cskip);
	tst_resm(TINFO,"Total:%d",cpass+cfail+cskip);

        cleanup();        /** OR tst_exit(); */

        return VT_rv;
}
