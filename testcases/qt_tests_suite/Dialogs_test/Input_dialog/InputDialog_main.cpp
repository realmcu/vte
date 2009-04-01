/*================================================================================================*/
/**
    @file   InputDialog_main.cpp
    @brief  LTP Motorola template.
*/
/*==================================================================================================

  Copyright (C) 2004, Freescale Semiconductor, Inc. All Rights Reserved
  THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
  BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
  Freescale Semiconductor, Inc.
     
====================================================================================================
Revision History:
                            Modification     Tracking
Author                          Date          Number    Description of Changes
-------------------------   ------------    ----------  -------------------------------------------
Roman Holodov           17/05/2004      ?????????   Initial version 

====================================================================================================
Portability: Indicate if this module is portable to other compilers or platforms. 
             If not, indicate specific reasons why is it not portable.

==================================================================================================*/
extern "C"{
#include "test.h"
#include "usctest.h"
}

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <qapplication.h>
    
#include "InputDialog_test.h"

extern int  Tst_count;               /* counter for tst_xxx routines.         */
extern char *TESTDIR;                /* temporary dir created by tst_tmpdir() */

/* Global Variables */
char *TCID     = "InputDialog_test"; /* test program identifier.          */
int  TST_TOTAL = 1;                  /* total number of tests in this file.   */

void cleanup();
void setup();
int main(int argc, char **argv);

void cleanup()
{
	
		int VT_rv = VT_InputDialog_cleanup();
		if (VT_rv != TPASS)
		{
			tst_resm(TWARN, "VT_cleanup() Failed : error code = %d", VT_rv);
		}

    tst_exit();
}

void setup()
{
 	int VT_rv = TFAIL;

		VT_rv = VT_InputDialog_setup();
		if (VT_rv != TPASS)
		{
			tst_brkm(TBROK , cleanup, "VT_setup() Failed : error code = %d", VT_rv);
		}
    
    return;
}


int main(int argc, char **argv)
{

    QApplication app(argc,argv);

	int VT_rv = TFAIL;
		
    setup();

    tst_resm(TINFO, "Testing if %s test case is OK", TCID);

    VT_rv = VT_InputDialog_test(&app); /*with the parameters needed come from parse_opt())*/
	
    if(VT_rv == TPASS)
        tst_resm(TPASS, "%s test case worked as expected", TCID);
    else
        tst_resm(TFAIL, "%s test case did NOT work as expected", TCID);
		
		
    cleanup(); /** OR tst_exit(); */
		/* VTE */
	
	return VT_rv;

}
