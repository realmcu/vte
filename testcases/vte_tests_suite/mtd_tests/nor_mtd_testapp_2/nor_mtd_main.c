/***
**Copyright 2004-2009 Freescale Semiconductor, Inc. All Rights Reserved.
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
    @file   nor_mtd_main.c

    @brief  NOR MTD test 2 main function.
====================================================================================================
Revision History:
                            Modification     Tracking
Author                          Date          Number    Description of Changes
-------------------------   ------------    ----------  -------------------------------------------
V.Becker/rc023c          04/05/2004   TLSbo39142   Initial version 
V.Becker/rc023c          18/06/2004   TLSbo39142   Code reviewed 
E.Gromazina			07/07/2005	TLSbo50888	minor fixes
====================================================================================================
Portability:  ARM GCC  gnu compiler
==================================================================================================*/

/*==================================================================================================
Total Tests: 1

Test Executable Name:  nor_mtd_testapp_2

Test Strategy:  A test for error cases on NOR MTD flash memory device
=================================================================================================*/


#ifdef __cplusplus
extern "C"{
#endif

/*==================================================================================================
                                        INCLUDE FILES
==================================================================================================*/
/* Standard Include Files */
#include <errno.h>
    
/* Harness Specific Include Files. */
#include "test.h"
#include "usctest.h"

/* Verification Test Environment Include Files */
#include "nor_mtd_test_2.h"

/*==================================================================================================
                                        LOCAL MACROS
==================================================================================================*/

/*==================================================================================================
                                       LOCAL CONSTANTS
==================================================================================================*/

/*==================================================================================================
                                       LOCAL VARIABLES
==================================================================================================*/
  /* Binary flags : option is set or not */
  int flag_address = 0;
  int flag_dev = 0;

  /* Option arguments */
  char *Addressopt;
  char *dev_opt;

  char device_name[128];
/*==================================================================================================
                          LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
==================================================================================================*/
/* Options given to the NOR Flash test. Arguments are
   not required, we just need to know the length of the
   pattern that will be witten and read to/from NOR Flash :
   A : address until memory is locked. This address sets 
   also the offset where write and read operation start 
   D : device name  */
option_t address_options[] =
  {
    { "D:", &flag_dev, &dev_opt},			/* Device name */
    { "A:", &flag_address, &Addressopt },
    { NULL, NULL, NULL }
  };

/*==================================================================================================
                                       GLOBAL CONSTANTS
==================================================================================================*/


/*==================================================================================================
                                       GLOBAL VARIABLES
==================================================================================================*/
/* Extern Global Variables */
extern int  Tst_count;               /* counter for tst_xxx routines.         */
extern char *TESTDIR;                /* temporary dir created by tst_tmpdir() */

/* Global Variables */
char *TCID     = "nor_mtd_testapp_2"; /* test program identifier.          */
int  TST_TOTAL = 1;                  /* total number of tests in this file.   */
int flag_get_flash_information = 0;

/*==================================================================================================
                                   GLOBAL FUNCTION PROTOTYPES
==================================================================================================*/
void cleanup();
void setup();
int main(int argc, char **argv);

/*==================================================================================================
                                   LOCAL FUNCTION PROTOTYPES
==================================================================================================*/


/*==================================================================================================
                                       GLOBAL FUNCTIONS
==================================================================================================*/

/*================================================================================================*/
/*===== cleanup =====*/
/**
@brief  Performs all one time clean up for this test on successful
completion,  premature exit or  failure. Closes all temporary
files, removes all temporary directories exits the test with
appropriate return code by calling tst_exit() function.cleanup

@param  Input :      None.
        Output:      None.
  
@return Nothing
*/
/*================================================================================================*/
void cleanup()
{
  /* VTE : Actions needed to get a stable target environment */
  int VT_rv = TFAIL;
		
  VT_rv = VT_nor_mtd_test2_cleanup();
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
void setup()
{
  int VT_rv = TFAIL;
	
  /* VTE : Actions needed to prepare the test running */
  VT_rv = VT_nor_mtd_test2_setup();
  if (VT_rv != TPASS)
    {
      tst_brkm(TBROK , cleanup, "VT_nor_mtd_setup() Failed : error code = %d", VT_rv);
    }
    
  return;
}

/*================================================================================================*/
void help()
{
  printf("-D <device name>   \n");
  printf("-A <size of data in bytes  in NOR Flash>   \n");
  printf("\t\t\t Address until lock is done. Also offset for write and read opsize to write or read\n");
  printf("\t\t\t Warning ! Thsi size must be aligned on blocks multiple of 0x40000\n");
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
  int VT_rv = TFAIL;
  int Base = 16;
  char *msg;
  unsigned long size = ERASE_BLOCK_SIZE;

  /* Parse user defined options */
  msg=parse_opts(argc, argv, address_options, help);
  if (msg != (char*)NULL)
    {
      tst_brkm(TBROK, cleanup, "OPTION PARSING ERROR - %s", msg);
    }

  if (flag_dev)
  {
	  strcpy(device_name, dev_opt);
  }
  else
  {
	  strcpy(device_name, NOR_MTD_DRIVER );
  }

  /* Set address by checking options flags */
  if(flag_address)
  {
      /* Convert hex string starting with 0x into hex value */
      size = strtol(Addressopt, NULL, Base);
      if(size % ERASE_BLOCK_SIZE != 0)
	  {
	   tst_resm(TWARN, "Size must be multiple of 0x40000. Taking default size of 0x40000\n");
	   size = ERASE_BLOCK_SIZE;
	  }
  }  
  else
     {
      flag_get_flash_information = 1;
     }
     

  /* Perform global test setup, call setup() function. */
  setup();

  /* Print test Assertion using tst_resm() function with argument TINFO */
  tst_resm(TINFO, "Testing if %s test case is OK", TCID);

  /* VTE : Execute test, print results and exit test scenario */
  VT_rv = VT_nor_mtd_test2(size);
	
  if(VT_rv == TPASS)
    {
      tst_resm(TPASS, "test case %s worked as expected", TCID);
    }
  else
    {
      tst_resm(TFAIL, "test case %s did NOT work as expected", TCID);
    }
		
  /* cleanup test allocated ressources */	
  cleanup(); 

  return VT_rv;
}

#ifdef __cplusplus
}
#endif
