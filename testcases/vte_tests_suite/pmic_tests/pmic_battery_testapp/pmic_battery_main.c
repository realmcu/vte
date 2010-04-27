/***
**Copyright (C) 2006-2009 Freescale Semiconductor, Inc. All Rights Reserved.
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
        @file   pmic_battery_main.c

        @brief  Main file for PMIC Battery driver test.
====================================================================================================
Revision History:
                            Modification     Tracking
Author/core ID                  Date          Number    Description of Changes
-------------------------   ------------    ----------  -------------------------------------------
A.Ozerov/NONE                18/01/2006     TLSbo61037  Initial version
A.Ozerov/b00320              12/07/2006     TLSbo64238  Changes for L26_1_19 release.
Pradeep K /b01016            09/25/2006     TLSboXXXX   Updated for PMIC API's

====================================================================================================
Portability: ARM GCC
==================================================================================================*/

/*==================================================================================================
                                        INCLUDE FILES
==================================================================================================*/
/* Standard Include Files */
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

/* Harness Specific Include Files. */
#include <test.h>
#include <usctest.h>

/* Verification Test Environment Include Files */
#include "pmic_battery_test.h"

/*==================================================================================================
                                        GLOBAL VARIABLES
==================================================================================================*/
char   *TCID = "pmic_battery_testapp";

int     fd;     /* PMIC test device descriptor */
int     TST_TOTAL = 5;  /* total number of tests in this file. */

/*==================================================================================================
                                    GLOBAL FUNCTION PROTOTYPES
==================================================================================================*/
void    cleanup(void);
void    setup(void);
int     main(int argc, char **argv);
void    help(void);

/*==================================================================================================
                                    GLOBAL FUNCTIONS
==================================================================================================*/
/*================================================================================================*/
/*===== cleanup =====*/
/**
@brief This function performs all one time clean up for this test on successful completion,
       premature exit or failure. Closes all temporary files, removes all temporary directories exits
       the test with appropriate return code by calling tst_exit() function.

@param None.

@return None.
*/
/*================================================================================================*/
void cleanup(void)
{
        int     rv = TFAIL;

        rv = VT_pmic_battery_test_cleanup();
        if (rv != TPASS)
        {
                tst_resm(TWARN, "pmic_battery_test_cleanup() Failed : error code = %d", rv);
        }
        tst_exit();
}

/*================================================================================================*/
/*===== setup =====*/
/**
@brief Performs all one time setup for this test. This function is typically used to capture
       signals, create temporary dirs and temporary files that may be used in the course of this test.

@param None.

@return None.
*/
/*================================================================================================*/
void setup(void)
{
        int     rv = TFAIL;

        rv = VT_pmic_battery_test_setup();
        if (rv != TPASS)
        {
                tst_brkm(TBROK, cleanup, "setup() Failed : error code = %d", rv);
        }
}

/*================================================================================================*/
/*===== main =====*/
/**
@brief Entry point to this test-case. It parses all the command line inputs, calls the global
       setup and executes the test. It logs the test status and results appropriately using the LTP API's
       On successful completion or premature failure, cleanup() func is called and test exits with an
       appropriate return code.

@param Input : argc - number of command line parameters.
       Output: **argv - pointer to the array of the command line parameters.

@return On failure - Exits by calling cleanup().
        On success - exits with 0 exit value.
*/
/*================================================================================================*/
int main(int argc, char **argv)
{
        int     rv = TFAIL;
        int     change_test_case = 0;
        FILE    *file;

//#ifdef CONFIG_MXC_PMIC_SC55112
//#ifdef CONFIG_MXC_MC13783_PMIC
        /* parse options. */
        int     tflag = 0;

        /* option arguments */
        char   *msg = 0;

        char    *conf_path,
                *ch_test_case = 0,
                *copt = 0;
        int     tcid_number = strlen(TCID),
                lengh_path = strlen(argv[0]),
                i = 0,
                cflag = 0;

        option_t options[] =
        {
                {"T:", &tflag, &ch_test_case},  /* argument required */
                {"C:", &cflag, &copt},          /* argument required */
                {NULL, NULL, NULL}              /* NULL required to end array */
        };

        if ((msg = parse_opts(argc, argv, options, &help)) != NULL)
        {
                tst_resm(TWARN, "%s test didn't work as expected. Parse options error: %s", TCID, msg);
                help();
                return rv;
        }

        if (tflag == 0)
        {
                /* Print results and exit test scenario */
                help();
                cleanup();
                return rv;
        }

        if (cflag == 0)
        {
                conf_path = malloc(strlen(argv[0]));
                strcat(conf_path, argv[0]);
                for (i = lengh_path - tcid_number; i <= lengh_path; i++)
                {
                        conf_path[i] = '\0';
                }

                strcat(conf_path, "Config");

                file = fopen(conf_path, "r");

                free(conf_path);
        }
        else
        {
                file = fopen(copt, "r");
        }

        if (file == NULL)
        {
                tst_resm(TFAIL, "Unable to open config file! Error code: %d", errno);
                help();
                return rv;
        }

        setup();

        change_test_case = atoi(ch_test_case);

        if (change_test_case >= 0 && change_test_case <= 10)
        {
                   int c_voltage;
                   PMIC_STATUS status;
                   if(status = ioctl(fd, PMIC_BATT_GET_BATTERY_VOLTAGE, &c_voltage)!=PMIC_SUCCESS)
			   tst_resm(TFAIL, "Error in pmic_batt_get_battery_voltage. Error code: %d", status);
		      else if(c_voltage==2400)
				{    
                                   printf("battery port's voltage:%d \n",c_voltage/1000);          
                                   tst_resm(TFAIL, "battery isn't linked to board,please connect battery to board!");
                            }

                else
                   {
                    //  status = ioctl(fd, PMIC_BATT_GET_BATTERY_VOLTAGE, &c_voltage);
                          if (c_voltage<=3400&&c_voltage>=2800)
                              printf("battery's energy is not enough!");
                          printf("battery voltage : %d v.\n", c_voltage/1000);
                      rv = VT_pmic_battery_test(change_test_case, file);
                       tst_resm(TINFO, "Testing %s_%s test case is OK", TCID, ch_test_case);
          
                 if (rv == PMIC_SUCCESS)
                 {
                        tst_resm(TPASS, "%s_%s test case working as expected", TCID, ch_test_case);
                 }
                 else
                 {
                        tst_resm(TFAIL, "%s_%s test case didn't work as expected", TCID, ch_test_case);
                 }
                   } 
        }
        else
        {
                rv = PMIC_ERROR;
                tst_resm(TFAIL, "Invalid arg for -T: %d. option value out of range", change_test_case);
                help();
        }
/*#else
        setup();
        change_test_case = 0;
        rv = VT_pmic_battery_test(change_test_case, file);
        tst_resm(TINFO, "Testing %s test case is OK", TCID);
        if (rv == PMIC_SUCCESS)
        {
                tst_resm(TPASS, "%s test case working as expected", TCID);
        }
        else
        {
                tst_resm(TFAIL, "%s test case didn't work as expected", TCID);
        }
#endif
*/
        cleanup();
        return rv;
}

/*================================================================================================*/
/*===== help =====*/
/**
@brief  Print help information.

@param  None.

@return None.
*/
/*================================================================================================*/
void help(void)
{
        printf("\n===========================================================================\n");
        printf("PMIC Battery Driver option\n");
//#ifdef CONFIG_MXC_PMIC_SC55112
//#ifdef CONFIG_MXC_MC13783_PMIC
        printf("\t  '-C file_name'   Open a configuration file\n");
        printf("\t  '-T 0'           Test charger management\n");
        printf("\t  '-T 1'           Test eol comparator\n");
        printf("\t  '-T 2'           Test led management\n");
        printf("\t  '-T 3'           Test reverse supply and unregulated modes\n");
        printf("\t  '-T 4'           Test get charger current\n");
        printf("\t  '-T 5'           Test get battery voltage\n");
        printf("\t  '-T 6'           Test get battery current\n");
        printf("\t  '-T 7'           Test get charger voltage\n");
/*#else
        printf("Call without any parameters\t  Check all of MC13783 Battery driver functions");
#endif
*/
        printf("\n===========================================================================\n");

}
