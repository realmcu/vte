/***
**Copyright 2006-2009 Freescale Semiconductor, Inc. All Rights Reserved.
**
**The code contained herein is licensed under the GNU General Public
**License. You may obtain a copy of the GNU General Public License
**Version 2 or later at the following locations:
**
**http://www.opensource.org/licenses/gpl-license.html
**http://www.gnu.org/copyleft/gpl.html
**/
/*===============================================================================================*/
/**
        @file   pmic_battery_test.c

        @brief  Source file for PMIC Battery driver test scenario.
====================================================================================================
Revision History:
                            Modification     Tracking
Author/core ID                  Date          Number    Description of Changes
-------------------------   ------------    ----------  -------------------------------------------
A.Ozerov/NONE                18/01/2006     TLSbo61037  Initial version
A.Ozerov/b00320              12/07/2006     TLSbo64238  Changes for L26_1_19 release.
Pradeep K/b01016             09/25/2006     TLSboXXXX   Updated for PMIC API's

====================================================================================================
Portability: ARM GCC
==================================================================================================*/

/*==================================================================================================
                                        INCLUDE FILES
==================================================================================================*/
/* Harness Specific Include Files. */
#include <test.h>

/* Verification Test Environment Include Files */
#include "pmic_battery_test.h"

/*==================================================================================================
                                        GLOBAL VARIABLES
==================================================================================================*/
extern char *TCID;
extern int fd;

/*==================================================================================================
                                        LOCAL FUNCTIONS
==================================================================================================*/
/*================================================================================================*/
/*===== VT_pmic_read_param =====*/
/**
@brief  This function reads parameters from file and initializes structures.

@param  tcharg
        pointer to the structure t_charger_setting.

@param  teol
        pointer to the structure t_eol_setting.

@return None.
*/
/*================================================================================================*/
//#ifdef CONFIG_MXC_PMIC_SC55112
//#ifdef CONFIG_MXC_MC13783_PMIC
void VT_pmic_read_param(t_charger_setting * tcharg, t_eol_setting * teol,FILE * file)
{
        int     voltage, current, threshold;

        if(file != NULL)
        {
                fscanf(file, "%d %d %d", &voltage, &current, &threshold);
                tcharg->c_voltage = (unsigned char) voltage;
                tcharg->c_current = (unsigned char) current;
                teol->typical = (unsigned char) threshold;
				//tcrl= contrl;
			//thred = setthreshold;
        }
        else
        {
                tst_resm(TFAIL, "Unable to read a config file!");
                VT_pmic_battery_test_cleanup();
        }
}//#endif
/*================================================================================================*/
/*===== VT_pmic_battery_test_cleanup =====*/
/**
@brief  This function assumes the post-condition of the test case execution

@param  None.

@return None.
*/
/*================================================================================================*/
int VT_pmic_battery_test_cleanup(void)
{
        if (fd != 0)
                close(fd);
        return TPASS;
}

/*================================================================================================*/
/*===== VT_pmic_battery_test_setup =====*/
/**
@brief  This function assumes the pre-condition of the test case execution

@param  None.

@return On success - return PMIC_SUCCESS
        On failure - return the error code.
*/
/*================================================================================================*/
int VT_pmic_battery_test_setup(void)
{
        fd = open("/dev/" PMIC_BATTERY_DEV, O_RDWR);

        if (fd < 0)
        {
                tst_resm(TFAIL, "setup() Failed open device");
                return TFAIL;
        }
        return TPASS;
}

/*================================================================================================*/
/*===== VT_pmic_battery_test =====*/
/**
@brief  PMIC Battery test scenario

@param  switch_fct
        Number test case.

@return On success - return PMIC_SUCCESS
        On failure - return the error code.
*/
/*================================================================================================*/
int VT_pmic_battery_test(int switch_fct, FILE * file)
{
        PMIC_STATUS status;
        int     rv1 = PMIC_SUCCESS;
        int     rv2;
	int c_current, c_voltage;

//#ifdef CONFIG_MXC_PMIC_SC55112
//#ifdef CONFIG_MXC_MC13783_PMIC
        int     i = 0;
        t_charger_setting tset1[CONFIG_PARAM_NUMBER];
        t_eol_setting     tset2[CONFIG_PARAM_NUMBER];
		//t_control     tset3[3];
		//int             tset4[3];
        memset(&tset1[i], 0x00, sizeof(t_charger_setting) * CONFIG_PARAM_NUMBER);
        memset(&tset2[i], 0x00, sizeof(t_eol_setting) * CONFIG_PARAM_NUMBER);
	// memset(&tset3[i],0x00,sizeof(t_control) *CONFIG_PARAM_NUMBER);
	// memset(&tset4[i],0x00,sizeof(int) *CONFIG_PARAM_NUMBER);

        for (i = 0; i < CONFIG_PARAM_NUMBER; i++) 
                 VT_pmic_read_param(&tset1[i], &tset2[i],file);
       t_control tset3[]={CONTROL_HARDWARE,CONTROL_BPFET_LOW,CONTROL_BPFET_HIGH};
        int tset4[]={0,1,2};

        if (switch_fct == 0)
        {
                rv1 = PMIC_SUCCESS;
                tst_resm(TINFO, "Test PMIC Battery API\n");
                tst_resm(TINFO, "Testing main charger...\n");

                for (i = 0; i < CONFIG_PARAM_NUMBER; i++)
                {
                        tst_resm(TINFO, "Test enable charger control function of PMIC Battery driver. (%d, %d, %d)", tset1[i].c_voltage, tset1[i].c_current, tset2[i].typical);

                        tset1[i].chgr = BATT_MAIN_CHGR;
                        status = ioctl(fd, PMIC_BATT_CHARGER_CONTROL, &tset1[i]);
                        if (status != PMIC_SUCCESS)
                        {
                                rv1 = status;
                                tst_resm(TFAIL, "Error in pmic_batt_enable_charger(for main charger). (%d, %d, %d). Error code: %d", tset1[i].c_voltage, tset1[i].c_current, tset2[i].typical, status);
                        }

                        tset1[i].on = FALSE;
                        tst_resm(TINFO, "Test disable charger control function of PMIC Battery driver. (%d, %d, %d)", tset1[i].c_voltage, tset1[i].c_current, tset2[i].typical);

                        status = ioctl(fd, PMIC_BATT_CHARGER_CONTROL, &tset1[i]);

                        if (status != PMIC_SUCCESS)
                        {
                                rv1 = status;
                                tst_resm(TFAIL, "Error in pmic_batt_disable_charger(for main charger). (%d, %d, %d). Error code: %d", tset1[i].c_voltage, tset1[i].c_current, tset2[i].typical, status);
                        }

                        tst_resm(TINFO, "Test disable charger control function of PMIC Battery driver(without enable). (%d, %d, %d)", tset1[i].c_voltage, tset1[i].c_current, tset2[i].typical);

                        status = ioctl(fd, PMIC_BATT_CHARGER_CONTROL, &tset1[i]);

                        if (status != PMIC_SUCCESS)
                        {
                                rv1 = status;
                                tst_resm(TFAIL, "Error in pmic_batt_disable_charger when it calls without pmic_battery_enable_charger(for main charger). (%d, %d, %d). Error code: %d", tset1[i].c_voltage, tset1[i].c_current, tset2[i].typical, status);
                        }

                        tset1[i].on = TRUE;

                        tst_resm(TINFO, "Test set charger function of PMIC Battery driver. (%d, %d, %d)", tset1[i].c_voltage, tset1[i].c_current, tset2[i].typical);

                        status = ioctl(fd, PMIC_BATT_SET_CHARGER, &tset1[i]);

                        if (status != PMIC_SUCCESS)
                        {
                                rv1 = status;
                                tst_resm(TFAIL, "Error in pmic_batt_set_charger(for main charger). (%d, %d, %d). Error code: %d", tset1[i].c_voltage, tset1[i].c_current, tset2[i].typical, status);
                        }

                        tst_resm(TINFO, "Test get charger function of PMIC Battery driver. (%d, %d, %d)", tset1[i].c_voltage, tset1[i].c_current, tset2[i].typical);

                        status = ioctl(fd, PMIC_BATT_GET_CHARGER, &tset1[i]);

                        if (status != PMIC_SUCCESS)
                        {
                                rv1 = status;
                                tst_resm(TFAIL, "Error in pmic_batt_get_charger_setting(for main charger). (%d, %d, %d). Error code: %d", tset1[i].c_voltage, tset1[i].c_current, tset2[i].typical, status);
                        }
                        printf("\n");
                }

                tst_resm(TINFO, "Testing cell charger...\n");

                fseek(file, 0, SEEK_SET);

                for (i = 0; i < CONFIG_PARAM_NUMBER; i++)
                        VT_pmic_read_param(&tset1[i], &tset2[i],file);

                for (i = 0; i < CONFIG_PARAM_NUMBER; i++)
                {
                        tset1[i].chgr = BATT_CELL_CHGR;

                        tset1[i].on = TRUE;
                        tst_resm(TINFO, "Test enable charger function of PMIC Battery driver. (%d, %d, %d)", tset1[i].c_voltage, tset1[i].c_current, tset2[i].typical);

                        status = ioctl(fd, PMIC_BATT_CHARGER_CONTROL, &tset1[i]);

                        if (status != PMIC_SUCCESS)
                        {
                                rv2 = status;
                                tst_resm(TFAIL, "Error in pmic_batt_enable_charger(for cell charger). (%d, %d, %d). Error code: %d", tset1[i].c_voltage, tset1[i].c_current, tset2[i].typical, status);
                        }

                        tset1[i].on = FALSE;
                        tst_resm(TINFO, "Test disable charger function of PMIC Battery driver. (%d, %d, %d)", tset1[i].c_voltage, tset1[i].c_current, tset2[i].typical);

                        status = ioctl(fd, PMIC_BATT_CHARGER_CONTROL, &tset1[i]);

                        if (status != PMIC_SUCCESS)
                        {
                                rv2 = status;
                                tst_resm(TFAIL, "Error in pmic_batt_disable_charger(for cell charger). (%d, %d, %d). Error code: %d", tset1[i].c_voltage, tset1[i].c_current, tset2[i].typical, status);
                        }

                        tst_resm(TINFO, "Test disable charger function of PMIC Battery driver. (%d, %d, %d)", tset1[i].c_voltage, tset1[i].c_current, tset2[i].typical);

                        status = ioctl(fd, PMIC_BATT_CHARGER_CONTROL, &tset1[i]);

                        if (status != PMIC_SUCCESS)
                        {
                                rv2 = status;
                                tst_resm(TFAIL, "Error in pmic_batt_disable_charger when it calls without pmic_batt_enable_charger(for cell charger). (%d, %d, %d). Error code: %d", tset1[i].c_voltage, tset1[i].c_current, tset2[i].typical, status);
                        }

                        tset1[i].on = TRUE;

                        tst_resm(TINFO, "Test set charger function of PMIC Battery driver. (%d, %d, %d)", tset1[i].c_voltage, tset1[i].c_current, tset2[i].typical);

                        status = ioctl(fd, PMIC_BATT_SET_CHARGER, &tset1[i]);

                        if (status != PMIC_SUCCESS)
                        {
                                rv2 = status;
                                tst_resm(TFAIL, "Error in pmic_batt_set_charger(for cell charger). (%d, %d, %d). Error code: %d", tset1[i].c_voltage, tset1[i].c_current, tset2[i].typical, status);
                        }

                        tst_resm(TINFO, "Test get charger function of PMIC Battery driver. (%d, %d, %d)", tset1[i].c_voltage, tset1[i].c_current, tset2[i].typical);

                        status = ioctl(fd, PMIC_BATT_GET_CHARGER, &tset1[i]);

                        if (status != PMIC_SUCCESS)
                        {
                                rv2 = status;
                                tst_resm(TFAIL, "Error in pmic_batt_get_charger_setting(for cell charger). (%d, %d, %d). Error code: %d", tset1[i].c_voltage, tset1[i].c_current, tset2[i].typical, status);
                        }
                        printf("\n");
                }
                return rv1 && rv2;
        }

        fseek(file, 0, SEEK_SET);

        for (i = 0; i < CONFIG_PARAM_NUMBER; i++)
                VT_pmic_read_param(&tset1[i],&tset2[i],file);

        switch (switch_fct)
        {
        case 1:
                  	
        	        for (i = 0; i < CONFIG_PARAM_NUMBER; i++) {
                        tst_resm(TINFO, "Test enable eol function of PMIC Battery driver. (%d, %d, %d)", tset1[i].c_voltage, tset1[i].c_current, tset2[i].typical);

                        rv1 = PMIC_SUCCESS;
                        tset2[i].enable = TRUE;

                        status = ioctl(fd, PMIC_BATT_EOL_CONTROL, &tset2[i]);

                        if (status != PMIC_SUCCESS)
                        {
                                rv1 = status;
                                tst_resm(TFAIL, "Error in pmic_batt_enable_eol. (%d, %d, %d). Error code: %d", tset1[i].c_voltage, tset1[i].c_current, tset2[i].typical, status);
                        }

                        tset2[i].enable = FALSE;
                        tst_resm(TINFO, "Test disable eol function of PMIC Battery driver. (%d, %d, %d)", tset1[i].c_voltage, tset1[i].c_current, tset2[i].typical);
                        status = ioctl(fd, PMIC_BATT_EOL_CONTROL, &tset2[i]);
                        if (status != PMIC_SUCCESS)
                        {
                                rv1 = status;
                                tst_resm(TFAIL, "Error in pmic_batt_disable_eol. (%d, %d, %d). Error code: %d", tset1[i].c_voltage, tset1[i].c_current, tset2[i].typical, status);
                        }

                        tst_resm(TINFO, "Test disable eol function of PMIC Battery driver. (%d, %d, %d)", tset1[i].c_voltage, tset1[i].c_current, tset2[i].typical);
                        status = ioctl(fd, PMIC_BATT_EOL_CONTROL, &tset2[i]);
                        if (status != PMIC_SUCCESS)
                        {
                                rv1 = status;
                                tst_resm(TFAIL, "Error in pmic_batt_disable_eol when it calls without enable eol. (%d, %d, %d). Error code: %d", tset1[i].c_voltage, tset1[i].c_current, tset2[i].typical, status);
                        }

                        printf("\n");
		}
                break;
           
         case 2:
        	for (i = 0; i < CONFIG_PARAM_NUMBER; i++) {
                        tst_resm(TINFO, "Test led control function of PMIC Battery driver(TRUE). (%d, %d, %d)", tset1[i].c_voltage, tset1[i].c_current, tset2[i].typical);

                        rv1 = PMIC_SUCCESS;
                        status = ioctl(fd, PMIC_BATT_LED_CONTROL, TRUE);
                        if (status != PMIC_SUCCESS)
                        {
                                rv1 = status;
                                tst_resm(TFAIL, "Error in pmic_batt_led_control with on=true. (%d, %d, %d). Error code: %d", tset1[i].c_voltage, tset1[i].c_current, tset2[i].typical, status);
                        }

                        tst_resm(TINFO, "Test led control function of PMIC Battery driver(FALSE). (%d, %d, %d)", tset1[i].c_voltage, tset1[i].c_current, tset2[i].typical);

                        status = ioctl(fd, PMIC_BATT_LED_CONTROL, FALSE);
                        if (status != PMIC_SUCCESS)
                        {
                                rv1 = status;
                                tst_resm(TFAIL, "Error in pmic_batt_led_control with on=false. (%d, %d, %d). Error code: %d", tset1[i].c_voltage, tset1[i].c_current, tset2[i].typical, status);
                        }

                        printf("\n");
		}
                break;
        case 3:
        	for (i = 0; i < CONFIG_PARAM_NUMBER; i++) {
                        tst_resm(TINFO, "Test reverse supply setting function of PMIC Battery driver(TRUE). (%d, %d, %d)", tset1[i].c_voltage, tset1[i].c_current, tset2[i].typical);

                        rv1 = PMIC_SUCCESS;
                        status = ioctl(fd, PMIC_BATT_REV_SUPP_CONTROL, TRUE);
                        if (status != PMIC_SUCCESS)
                        {
                                rv1 = status;
                                tst_resm(TFAIL, "Error in pmic_batt_set_reverse_supply with enable=true. (%d, %d, %d). Error code: %d", tset1[i].c_voltage, tset1[i].c_current, tset2[i].typical, status);
                        }

                        tst_resm(TINFO, "Test reverse supply setting function of PMIC Battery driver(FALSE). (%d, %d, %d)", tset1[i].c_voltage, tset1[i].c_current, tset2[i].typical);
                        status = ioctl(fd, PMIC_BATT_REV_SUPP_CONTROL, FALSE);
                        if (status != PMIC_SUCCESS)
                        {
                                rv1 = status;
                                tst_resm(TFAIL, "Error in pmic_batt_set_reverse_supply with enable=false. (%d, %d, %d). Error code: %d", tset1[i].c_voltage, tset1[i].c_current, tset2[i].typical, status);
                        }

                        tst_resm(TINFO, "Test unregulatored function of PMIC Battery driver(TRUE). (%d, %d, %d)", tset1[i].c_voltage, tset1[i].c_current, tset2[i].typical);
                        status = ioctl(fd, PMIC_BATT_UNREG_CONTROL, TRUE);
                        if (status != PMIC_SUCCESS)
                        {
                                rv1 = status;
                                tst_resm(TFAIL, "Error in pmic_batt_set_unregulated with enable=true. (%d, %d, %d). Error code: %d", tset1[i].c_voltage, tset1[i].c_current, tset2[i].typical, status);
                        }

                        tst_resm(TINFO, "Test unregulatored function of PMIC Battery driver(FALSE). (%d, %d, %d)", tset1[i].c_voltage, tset1[i].c_current, tset2[i].typical);
                        status = ioctl(fd, PMIC_BATT_UNREG_CONTROL, FALSE);
                        if (status != PMIC_SUCCESS)
                        {
                                rv1 = status;
                                tst_resm(TFAIL, "Error in pmic_batt_set_unregulated with enable=false. (%d, %d, %d). Error code: %d", tset1[i].c_voltage, tset1[i].c_current, tset2[i].typical, status);
                        }

                        printf("\n");
		}
                break;
	case 4:
		tst_resm(TINFO, "Test get charger current function of PMIC Battery");
		status = ioctl(fd, PMIC_BATT_GET_CHARGER_CURRENT, &c_current);
		if (status != PMIC_SUCCESS) {
			rv1 = status;
			tst_resm(TFAIL, "Error in pmic_batt_get_charger_current. Error code: %d", status);
		}
		printf("charger current : %d.\n", c_current);

		break;

	case 5:
		tst_resm(TINFO, "Test get battery voltage function of PMIC Battery");
		status = ioctl(fd, PMIC_BATT_GET_BATTERY_VOLTAGE, &c_voltage);
		if (status != PMIC_SUCCESS) {
			rv1 = status;
			tst_resm(TFAIL, "Error in pmic_batt_get_battery_voltage. Error code: %d", status);
		}
		printf("battery voltage : %d.\n", c_voltage);

		break;

	case 6:
		tst_resm(TINFO, "Test get battery current function of PMIC Battery");
		status = ioctl(fd, PMIC_BATT_GET_BATTERY_CURRENT, &c_current);
		if (status != PMIC_SUCCESS) {
			rv1 = status;
			tst_resm(TFAIL, "Error in pmic_batt_get_battery_current. Error code: %d", status);
		}
		printf("battery current : %d.\n", c_current);

		break;

	case 7:
		tst_resm(TINFO, "Test get charger voltage function of PMIC Battery");
		status = ioctl(fd, PMIC_BATT_GET_CHARGER_VOLTAGE, &c_voltage);
		if (status != PMIC_SUCCESS) {
			rv1 = status;
			tst_resm(TFAIL, "Error in pmic_batt_get_charger_voltage. Error code: %d", status);
		}
		printf("charger voltage : %d.\n", c_voltage);

		break;

       case 8:
		for(i=0;i<3;i++){
		  tst_resm(TINFO, "Test set out control  function of PMIC Battery");
		  status=ioctl(fd,PMIC_BATT_SET_OUT_CONTROL,tset3[i]);
		  if (status !=PMIC_SUCCESS){
			rv1 = status;
			tst_resm(TFAIL,"Error in pmic_batt_set_out_control. Error code: %d",status);
               }
		printf("output control value: %d.\n",tset3[i]);
	      }

		break;

	case 9:
		for(i=0;i<3;i++){
			tst_resm(TINFO, "Test set threshold function of PMIC Battery");
			status=ioctl(fd,PMIC_BATT_SET_THRESHOLD,&tset4[i]);
			if(status !=PMIC_SUCCESS){
				rv1=status;
				tst_resm(TFAIL,"Error in pmic_batt_set_threshold. Error code: %d",status);
				}
			printf("number of threshold is: %d.\n",tset4[i]);
			}

		break;

	case 10:
		tst_resm(TINFO, "Test get battery temperature function of PMIC Battery");
		status = ioctl(fd,  PMIC_BATT_GET_BATTERY_TEMPERATURE, &c_current);
		if (status != PMIC_SUCCESS) {
			rv1 = status;
			tst_resm(TFAIL, "Error in pmic_batt_get_battery_temperature. Error code: %d", status);
		}
		printf("battery temperature : %d.\n", c_current);

		break;

	

	}
        return rv1;

	
/*#else
        status = ioctl(fd, MC13783_BATTERY_INIT, NULL);

        if(status != PMIC_SUCCESS)
        {
                tst_resm(TFAIL, "Error in INIT ioctl. Error code: %d, error string: %s", status, strerror(errno));
                rv1 = status;
        }
        else
        tst_resm(TINFO, "INIT ioctl passed");

        status = ioctl(fd, MC13783_BATTERY_CHECK, NULL);

        if(status != PMIC_SUCCESS)
        {
                tst_resm(TFAIL, "Error in CHECK ioctl. Error code: %d, error string: %s", status, strerror(errno));
                rv2 = status;
        }
        else
        tst_resm(TINFO, "CHECK ioctl passed");
        printf("\n");

        return rv1 && rv2;
#endif
*/
}
