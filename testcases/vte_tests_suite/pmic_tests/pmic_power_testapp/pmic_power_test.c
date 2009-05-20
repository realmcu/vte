/*====================*/
/**
        @file   pmic_power_test.c

        @brief  Test scenario C source for PMIC Power driver.
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
-------------------------   ------------    ----------  -------------------------------------------
S.Bezrukov/SBAZR1C           07/20/2005     TLSbo52698  Initial version
S.Bezrukov/SBAZR1C           09/06/2005     TLSbo52698  Rework version
N.Filinova/nfili1c           16/01/2006     TLSbo61037  Rework version
D.Khoroshev/b00313           07/17/2006     TLSbo64236  Added mc13783 legacy support
Pradeep K /b01016            10/19/2006     TLSboxxxx   Updated for pmic power
Rakesh S Joshi/R65956        01/03/2007     TLSbo88188   Updated for pmic power


====================
Portability:  ARM GCC
======================*/

/*======================
                                        INCLUDE FILES
======================*/
/* Harness Specific Include Files. */
#include <test.h>
#include <time.h>

/* Verification Test Environment Include Files */
#include "pmic_power_test.h"

/*======================
                                       GLOBAL VARIABLES
=======================*/
int fd = -1;
FILE * gInFile = NULL;
#define ARRAY_SIZE(x)   (sizeof(x) / sizeof(x[0]))

#ifdef CONFIG_MXC_PMIC_SC55112
static unsigned int regulator_on[]={
    SW_SW2A, REGU_VSIM, REGU_VVIB
};
static unsigned int regulator_off[]={
    SW_SW2A, REGU_VSIM, REGU_VVIB
};
#elif CONFIG_MXC_PMIC_MC13783

static unsigned int regulator_on[]={
    SW_SW3,         /*!< SW3 */
    SW_PLL,         /*!< PLL */
    REGU_VAUDIO,    /*!< VAUDIO */
    REGU_VIOHI,     /*!< VIOHI */
    REGU_VIOLO,     /*!< VIOLO */
    REGU_VDIG,      /*!< VDIG */
    REGU_VGEN,      /*!< VGEN */
    REGU_VRFDIG,    /*!< VRFDIG */
    REGU_VRFREF,    /*!< VRFREF */
    REGU_VRFCP,     /*!< VRFCP */
    REGU_VSIM,      /*!< VSIM */
    REGU_VESIM,     /*!< VESIM */
    REGU_VCAM,      /*!< VCAM */
    REGU_VRFBG,     /*!< VRFBG */
    REGU_VVIB,      /*!< VVIB */
    REGU_VRF1,      /*!< VRF1 */
    REGU_VRF2,      /*!< VRF2 */
    REGU_VMMC1,     /*!< VMMC1 or VMMC */
    REGU_VMMC2,     /*!< VMMC2 */
};
static unsigned int regulator_off[]={
    SW_SW3,         /*!< SW3 */
    SW_PLL,         /*!< PLL */
    REGU_VAUDIO,        /*!< VAUDIO */
    REGU_VCAM,      /*!< VCAM */
    REGU_VVIB,      /*!< VVIB */
};

#endif

/*======================
                                 LOCAL FUNCTIONS PROTOTYPES
======================*/
int  pmic_power_test_on(void);
int  pmic_power_test_off(void);
int  pmic_power_test_config(void);
int  check_reg_param(int aReg, int aInd, int aParam);
int  write_config_file(void);
char *pmic_error(int errcode);

/*======================
                                       LOCAL FUNCTIONS
======================*/

/*====================*/
/*= pmic_error =*/
/**
@brief  Returns string with error code short description

@param  errno - PMIC_STATUS error code.

@return On success - return pointer to string or NULL
*/
/*====================*/
char *pmic_error(int errcode)
{
        static char *pmic_errors[] =
        {
                "PMIC_SUCCESS",       /*!< The requested operation was successfully
                                           completed.                                     */
                "PMIC_ERROR",        /*!< The requested operation could not be completed
                                           due to an error.                               */
                "PMIC_PARAMETER_ERROR",      /*!< The requested operation failed because
                                                   one or more of the parameters was
                                                   invalid.                             */
                "PMIC_NOT_SUPPORTED",        /*!< The requested operation could not be
                                                   completed because the PMIC hardware
                                                   does not support it. */
                "PMIC_CLIENT_NBOVERFLOW",    /*!< The requested operation could not be
                                                   completed because there are too many
                                                   PMIC client requests */
                "PMIC_MALLOC_ERROR", /*!< Error in malloc function             */
                "PMIC_UNSUBSCRIBE_ERROR",    /*!< Error in un-subscribe event          */
                "PMIC_EVENT_NOT_SUBSCRIBED", /*!< Event occur and not subscribed       */
                "PMIC_EVENT_CALL_BACK",      /*!< Error - bad call back                */
        };
        if(errcode < 0)
                return NULL;
        if(errcode > -PMIC_EVENT_CALL_BACK)
                return strerror(errcode);

        return pmic_errors[errcode];
}

/*====================*/
/*= VT_pmic_power_test_setup =*/
/**
@brief  assumes the pre-condition of the test case execution

@param  None

@return On success - return TPASS
        On failure - return the error code
*/
/*====================*/
int VT_pmic_power_test_setup(void)
{
        int rv = TPASS;

        fd = open(PMIC_POWER_DEV_NAME, O_RDWR);

        if (fd < 0)
        {
                tst_resm(TFAIL, "VT_pmic_power_test_setup() : Failed open PMIC power device %s", PMIC_POWER_DEV_NAME);
                return TFAIL;

        }

        if((gTestConfig.mTestCase == CONFIG || gTestConfig.mTestCase == ERR_CONFIG_PARAMS )&&(!gTestConfig.mWriteConfig))
        {
                if((gInFile = fopen(gTestConfig.mCfgFile,"rt")) == NULL)
                {
                        tst_resm(TWARN, "VT_pmic_power_test_setup() : Failed open config file %s", gTestConfig.mCfgFile);
                        return TFAIL;
                }
        }

        return rv;
}

/*====================*/
/*= VT_pmic_power_cleanup =*/
/**
@brief  assumes the post-condition of the test case execution

@param  None

@return On success - return TPASS
        On failure - return the error code
*/
/*====================*/
int VT_pmic_power_test_cleanup(void)
{
        int ret;

        if(fd >= 0)
        {
                ret = close(fd);

                if (ret < 0)
                {
                        tst_resm(TWARN, "Unable to close file descriptor %d for PMIC Power Device", fd);
                        return TFAIL;
                }
        }

        if(gInFile) fclose(gInFile);

        return TPASS;
}

/*====================*/
/*= VT_pmic_power_test =*/
/**
@brief  Pmic power test scenario  function

@param

@return On success - return TPASS
        On failure - return TFAIL
*/
/*====================*/
int VT_pmic_power_test(void)
{
        switch(gTestConfig.mTestCase)
        {
                case ENABLE:
                        tst_resm(TINFO, "Test case %d: ENABLE A REGULATOR", gTestConfig.mTestCase);
                        if(pmic_power_test_on() != TPASS)
                        {
                                tst_resm(TWARN ,"Error in pmic_power_test_on()");
                                return TFAIL;
                        }
                        sleep(1);
                        if(pmic_power_test_off() != TPASS)
                        {
                                tst_resm(TWARN ,"Error in pmic_power_test_on()");
                                return TFAIL;
                        }
                break;

                case CONFIG:
                        tst_resm(TINFO, "Test case %d: CONFIG", gTestConfig.mTestCase);
                        if(pmic_power_test_config() != TPASS)
                        {
                                tst_resm(TWARN ,"Error in pmic_power_test_config()");
                                return TFAIL;
                        }
                break;

                case ERR_CONFIG_PARAMS:
                        tst_resm(TINFO, "Test case %d: ERROR PARAMETERS CONFIG", gTestConfig.mTestCase);

                        if(pmic_power_test_errconfig() != TPASS)
                        {
                                tst_resm(TWARN ,"Error in pmic_power_test_errconfig()");
                                return TFAIL;
                        }
                break;

                default:
                        tst_resm(TBROK , "Error: This test case has been broken");
                        return TBROK;
        }

        return TPASS;
}

/*====================*/
/*= pmic_power_test_on =*/
/**
@brief  Enable a PMIC regulator

@param  None

@return On success - return TPASS
        On failure - return the error code
*/
/*====================*/
int pmic_power_test_on(void)
{
        int regNum = 0;
        tst_resm(TINFO,"Regulators:");
        for(regNum = 0;  regNum < ARRAY_SIZE(regulator_on); regNum++)
        {
                tst_resm(TINFO,"Turn on regulator %d",  regulator_on[regNum] );
                if(ioctl(fd, PMIC_REGULATOR_ON, regulator_on[regNum]) < 0)
                {
                        tst_resm(TWARN,"Error PMIC_REGULATOR_ON ioctl : %s : errno = %d", pmic_error(errno), errno);
                        return TFAIL;
                }
        }
        return TPASS;
}

/*====================*/
/*= pmic_power_test_off =*/
/**
@brief  Diasable a PMIC regulator

@param  None

@return On success - return TPASS
        On failure - return the error code
*/
/*====================*/
int pmic_power_test_off(void)
{
        int regNum = 0;
        tst_resm(TINFO,"Regulators:");
        for(regNum = 0;  regNum < ARRAY_SIZE(regulator_off); regNum++)
        {
                tst_resm(TINFO,"Turn off regulator %d",  regulator_off[regNum] );
                if(ioctl(fd, PMIC_REGULATOR_OFF, regulator_off[regNum]) < 0)
                {
                        tst_resm(TWARN,"Error PMIC_REGULATOR_OFF ioctl : errno = %d", errno);
                        return TFAIL;
                }
        }
        return TPASS;
}

/*====================*/
/*= pmic_power_test_config =*/
/**
@brief  configure a PMIC regulator

@param  None

@return On success - return TPASS
        On failure - return the error code
*/
/*====================*/
int pmic_power_test_config(void)
{
    t_regulator_cfg_param set_cfg, get_cfg;
    gInFile = fopen(gTestConfig.mCfgFile,"r");
    int ret;
    while (!feof(gInFile)) {
        memset(&set_cfg, 0, sizeof(t_regulator_cfg_param));
#ifdef CONFIG_MXC_PMIC_SC55112
        ret = fscanf(gInFile, "%d %d %d %d %d %d\n",
                 (int *)&set_cfg.regulator,
                 (int *)&set_cfg.cfg.mode,
                 (int *)&set_cfg.cfg.voltage,
                 (int *)&set_cfg.cfg.voltage_lvs,
                 (int *)&set_cfg.cfg.voltage_stby,
                 (int *)&set_cfg.cfg.lp_mode);
#elif CONFIG_MXC_PMIC_MC13783
        ret = fscanf(gInFile, "%d %d %d %d %d %d %d %d %d %d %d\n",
                 (int *)&set_cfg.regulator,
                 (int *)&set_cfg.cfg.mode,
                 (int *)&set_cfg.cfg.stby_mode,
                 (int *)&set_cfg.cfg.voltage,
                 (int *)&set_cfg.cfg.voltage_lvs,
                 (int *)&set_cfg.cfg.voltage_stby,
                 (int *)&set_cfg.cfg.lp_mode,
                 (int *)&set_cfg.cfg.dvs_speed,
                 (int *)&set_cfg.cfg.panic_mode,
                 (int *)&set_cfg.cfg.softstart,
                 (int *)&set_cfg.cfg.factor);
#endif

        ret = ioctl(fd, PMIC_REGULATOR_SET_CONFIG, &set_cfg);
        if (ret != PMIC_SUCCESS) {
            printf("PMIC_REGULATOR_SET_CONFIG [%s]\n", pmic_error(errno));
            fclose(gInFile);
            return -1;
        }
        sleep(1);
        memset(&get_cfg, 0, sizeof(t_regulator_cfg_param));
        get_cfg.regulator = set_cfg.regulator;
        ret = ioctl(fd, PMIC_REGULATOR_GET_CONFIG, &get_cfg);
        if (ret != PMIC_SUCCESS) {
            printf("PMIC_REGULATOR_GET_CONFIG [%s]\n", pmic_error(errno));
            fclose(gInFile);
            return -1;
        }
#ifdef CONFIG_MXC_PMIC_SC55112
        if ((set_cfg.cfg.voltage.sw1 != get_cfg.cfg.voltage.sw1) || (set_cfg.cfg.lp_mode && (set_cfg.cfg.lp_mode != get_cfg.cfg.lp_mode))) {
            printf("Get, set Voltage %d ,%d, %d \n", set_cfg.cfg.voltage.sw1 , get_cfg.cfg.voltage.sw1,set_cfg.regulator);
            printf("REGULATOR %d : Test COMPARE FAILED\n", get_cfg.regulator);
            fclose(gInFile);
            return -1;
        }
#elif CONFIG_MXC_PMIC_MC13783
        if ((set_cfg.cfg.mode != get_cfg.cfg.mode) ||
            (set_cfg.cfg.stby_mode != get_cfg.cfg.stby_mode) ||
            (set_cfg.cfg.voltage.sw1a != get_cfg.cfg.voltage.sw1a) ||
            (set_cfg.cfg.voltage_lvs.sw1a !=
             get_cfg.cfg.voltage_lvs.sw1a)
            || (set_cfg.cfg.voltage_stby.sw1a !=
            get_cfg.cfg.voltage_stby.sw1a)
            || (set_cfg.cfg.lp_mode != get_cfg.cfg.lp_mode)
            || (set_cfg.cfg.dvs_speed != get_cfg.cfg.dvs_speed)
            || (set_cfg.cfg.panic_mode != get_cfg.cfg.panic_mode)
            || (set_cfg.cfg.softstart != get_cfg.cfg.softstart)
            || (set_cfg.cfg.factor != get_cfg.cfg.factor)) {
            printf("Test Compare FAILED\n");
            fclose(gInFile);
            return -1;
        }
#endif
        printf("REGULATOR %d : Test PASSED\n", get_cfg.regulator);

    }
    return 0;

}

/*====================*/
/*= pmic_power_test_errconfig =*/
/**
@brief  Error configuration of a PMIC regulator

@param  None

@return On success - return TPASS
        On failure - return the error code
*/
/*====================*/
int pmic_power_test_errconfig(void)
{
    t_regulator_cfg_param set_cfg, get_cfg;
    gInFile = fopen(gTestConfig.mCfgFile,"r");
    int ret;
    while (!feof(gInFile)) {
        memset(&set_cfg, 0, sizeof(t_regulator_cfg_param));
#ifdef CONFIG_MXC_PMIC_SC55112
        ret = fscanf(gInFile, "%d %d %d %d %d %d\n",
                 (int *)&set_cfg.regulator,
                 (int *)&set_cfg.cfg.mode,
                 (int *)&set_cfg.cfg.voltage,
                 (int *)&set_cfg.cfg.voltage_lvs,
                 (int *)&set_cfg.cfg.voltage_stby,
                 (int *)&set_cfg.cfg.lp_mode);
#elif CONFIG_MXC_PMIC_MC13783
        ret = fscanf(gInFile, "%d %d %d %d %d %d %d %d %d %d %d\n",
                 (int *)&set_cfg.regulator,
                 (int *)&set_cfg.cfg.mode,
                 (int *)&set_cfg.cfg.stby_mode,
                 (int *)&set_cfg.cfg.voltage,
                 (int *)&set_cfg.cfg.voltage_lvs,
                 (int *)&set_cfg.cfg.voltage_stby,
                 (int *)&set_cfg.cfg.lp_mode,
                 (int *)&set_cfg.cfg.dvs_speed,
                 (int *)&set_cfg.cfg.panic_mode,
                 (int *)&set_cfg.cfg.softstart,
                 (int *)&set_cfg.cfg.factor);
#endif

        ret = ioctl(fd, PMIC_REGULATOR_SET_CONFIG, &set_cfg);
        if (ret == PMIC_SUCCESS) {
            printf("PMIC_REGULATOR_SET_CONFIG [%s]\n", pmic_error(errno));
            fclose(gInFile);
            return -1;
        }
        sleep(1);
        memset(&get_cfg, 0, sizeof(t_regulator_cfg_param));
        get_cfg.regulator = set_cfg.regulator;
        printf("REGULATOR %d : Test PASSED\n", get_cfg.regulator);

    }
    return 0;
}
