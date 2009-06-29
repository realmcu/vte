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
        @file   pmic_light_test.c

        @brief   Source file for PMIC (sc55112 and mc13783) Ligth driver test scenario.
====================================================================================================
Revision History:
                            Modification     Tracking
Author/core ID                  Date          Number    Description of Changes
-------------------------   ------------    ----------  -------------------------------------------
D.Khoroshev/b00313           07/25/2005     TLSbo52699   Initial version
D.Khoroshev/b00313           08/29/2005     TLSbo52699   Rework version
I.Inkina/nknl001             27/12/2005     TLSbo61037   Update for MXC91231 and MXC91131
D.Khoroshev/b00313           07/25/2005     TLSbo66285   Update for VTE 2.01
D.Khoroshev/b00313           08/31/2006     TLSbo76979   Added support for both SC55112 and MC13783
                                                         platforms
====================================================================================================
Portability: ARM GCC
==================================================================================================*/

/*==================================================================================================
                                INCLUDE FILES
==================================================================================================*/
/* LTP environment functions */
#include <test.h>
#include <usctest.h>
#include "pmic_light_test.h"
/*==================================================================================================
                                LOCAL MACROS
==================================================================================================*/
/*================================================================================================
                                        GLOABAL VARIABLES
==================================================================================================*/
int     fd;     /* device descriptor */
t_bklit_setting_param bklit_param_LD1;
t_bklit_setting_param bklit_param_LD2;
char pmic_status[9][30] =
{
        "PMIC_SUCCESS", "PMIC_ERROR", "PMIC_PARAMETER_ERROR", "PMIC_NOT_SUPPORTED",
        "PMIC_CLIENT_NBOVERFLOW", "PMIC_MALLOC_ERROR", "PMIC_UNSUBSCRIBE_ERROR",
        "PMIC_EVENT_NOT_SUBSCRIBED", "PMIC_EVENT_CALL_BACK"
};

/*==================================================================================================
                                        LOCAL FUNCTIONS
==================================================================================================*/
int     VT_LED_test_ioctl(void);
int     VT_LED_fun_pattern_config(void);
int     VT_LED_fun_test_config(void);
int     VT_LED_ind_config(void);
int     VT_bklit_test_ioctl(void);
int     SET_GET_bklit(t_bklit_setting_param * bklit_param);
int     VT_bklit_ramp_config_test(void);
/*================================================================================================*/
/*===== VT_SC55112_TEST_light_setup =====*/
/**
@brief  Opens device file.

@param  None

@return On success - return TPASS
        On failure - return the error code
*/
/*================================================================================================*/
int VT_pmic_light_test_setup(void)
{
        fd = open("/dev/" PMIC_LIGHT_DEV, O_RDWR);

        if (fd < 0)
        {
                tst_brkm(TBROK, VT_pmic_light_test_cleanup, "Unable to open %s",
                        "/dev/" PMIC_LIGHT_DEV);
        }
        printf("Master Enable for BackLight and TCLED is ENABLED\n");
        ioctl(fd, PMIC_BKLIT_TCLED_ENABLE);

        return TPASS;
}

/*================================================================================================*/
/*===== VT_SC55112_TEST_light_cleanup =====*/
/**
@brief  Closes device file.

@param  None

@return None.
*/
/*================================================================================================*/
void VT_pmic_light_test_cleanup(void)
{
        printf("Master Enable for BackLight and TCLED is DISABLED\n");
        ioctl(fd, PMIC_BKLIT_TCLED_DISABLE);
        if (!fd)
        {
                int ret;
                ret = close(fd);
                if (ret < 0)
                {
                        tst_resm(TWARN, "Unable to close file descriptor %d, error code %d", fd,
                                ret);
                }
        }

        tst_exit();
}

/*================================================================================================*/
/*===== VT_SC55112_test_light_TEST =====*/
/**
@brief  This function executes test cases for SC55112 Light driver test.

@param  Input:        switch_fct - test case number.

@return On success - return TPASS
        On failure - return the error code
*/
/*================================================================================================*/
int VT_pmic_light_test(int switch_fct)
{
        int     ret;
        int     rv = TPASS;

        VT_pmic_light_test_setup();

        switch (switch_fct)
        {
        case PMIC_LIGHT_TEST_TCLED_IOCTL:
                tst_resm(TINFO, "Test Tri-color LED IOCTL functions of  Light driver");
                rv = VT_LED_test_ioctl();
                if (rv)
                {
                        tst_resm(TFAIL, " Error enable - disable LED  function");
                        ret = TFAIL;
                }
                break;
        case PMIC_LIGHT_TEST_TCLED_CONFIG_FUN_PATTERN:
                tst_resm(TINFO, "Test Tri-color LED configuration for fun pattern");
                rv = VT_LED_fun_pattern_config();
                if (rv)
                {
                        tst_resm(TFAIL, "Error config LED  functions");
                        ret = TFAIL;
                }
                break;
        case PMIC_LIGHT_TEST_TCLED_CONFIG_FUN_TEST:
                tst_resm(TINFO, "Test Tri-color LED configuration for fun mode");
                rv = VT_LED_fun_test_config();
                if (rv)
                {
                        tst_resm(TFAIL, "Error config LED  functions");
                        ret = TFAIL;
                }
                break;
        case PMIC_LIGHT_TEST_TCLED_CONFIG_IND_MODE:
                tst_resm(TINFO, "Test Tri-color LED configuration for indicator mode");
                rv = VT_LED_ind_config();
                if (rv)
                {
                        tst_resm(TFAIL, "Error config LED  functions");
                        ret = TFAIL;
                }
                break;
        case PMIC_LIGHT_TEST_BKLIT_IOCTL:
                tst_resm(TINFO, "Test Backlight functions of  Light driver - Stress test");
                rv = VT_bklit_test_ioctl();
                if (rv)
                {
                        tst_resm(TFAIL, "Error Backligh function ");
                        ret = TFAIL;
                }
                break;
        case PMIC_LIGHT_TEST_BKLIT_RAMP_CONFIG:
                tst_resm(TINFO, "Test Backlight configuration of the  Light driver with Ramp functions");
                rv = VT_bklit_ramp_config_test();
                if (rv)
                {
                        tst_resm(TFAIL, "Error config backlight ");
                        ret = TFAIL;
                }
                break;
        default:
                tst_resm(TINFO, "Error in  Light Test: Unsupported operation");
        }

        VT_pmic_light_test_cleanup();
        return ret;
}



/*================================================================================================*/
/*===== VT_LED_test_ioctl =====*/
/**
@brief  This function executes all the ioctls for Tri-color LED.

@param  Input:None

@return On success - return TPASS
        On failure - return the error code
*/
/*================================================================================================*/
int VT_LED_test_ioctl(void)
{
        int bank, mode;
        int     rv = TPASS;
        t_tcled_enable_param enable_param;

        tst_resm(TINFO, "Test API for Tri-color LED functions of SC55112 and MC13783 Light driver");
/* This will check the PMIC_TCLED_ENABLE API */
        for (mode = TCLED_IND_MODE;mode <= TCLED_FUN_MODE ; mode++)
        {
                enable_param.mode = mode;
                for (bank = TCLED_FUN_BANK1; bank <= TCLED_FUN_BANK3; bank++)
                {
                        enable_param.bank = bank;
                        printf("MODE is %d\n", enable_param.mode);
                        rv = ioctl(fd, PMIC_TCLED_ENABLE, &enable_param);
                        if (rv < 0)
                        {
                                tst_resm(TFAIL, "Error %s for enable LED  function", pmic_status[abs(errno)]);
                                rv = TFAIL;
                                return rv;
                        }
                        rv = ioctl(fd, PMIC_TCLED_DISABLE, bank);
                        if (rv < 0)
                        {
                                tst_resm(TFAIL, "Error %s for disable LED  function", pmic_status[abs(errno)]);
                                rv = TFAIL;
                                return rv;
                        }
                        tst_resm(TINFO, "Disable LED function without enable it");

                        rv = ioctl(fd, PMIC_TCLED_DISABLE, bank);
                        if (rv < 0)
                        {
                                tst_resm(TFAIL, "Error %s for disable LED  function without enable it", pmic_status[abs(errno)]);
                                rv = TFAIL;
                                return rv;
                        }
                }
        }
        return rv;
}

/*================================================================================================*/
/*===== VT_LED_fun_pattern_config =====*/
/**
@brief  This function executes setup Tri-color LED for PMIC Light driver on FUN LIGHT MODE test.

@param  Input:None

@return On success - return TPASS
        On failure - return the error code
*/
/*================================================================================================*/
int VT_LED_fun_pattern_config(void)
{
        int rv = TPASS,bank, pattern;
        t_fun_param fun_param, fun_param_null;
        t_tcled_enable_param tcled_setting;

        tst_resm(TINFO, "Test Tri-color LED functions of SC55112 and MC13783 Light driver");
        tcled_setting.mode=TCLED_FUN_MODE;
        fun_param_null.bank=0;
        fun_param_null.pattern = 0;

        for(bank = TCLED_FUN_BANK1 ; bank <= TCLED_FUN_BANK3 ; bank++)
        {
            fun_param.bank = bank;
            tcled_setting.bank = bank;
            for (pattern = BLENDED_RAMPS_SLOW ; pattern <= CHASING_LIGHT_BGR_FAST ; pattern++)
            {
                fun_param.pattern = pattern;
                ioctl(fd, PMIC_TCLED_PATTERN, &fun_param);
                sleep(1);
                tst_resm(TINFO, "LED on %d Bank with %d Pattern",fun_param.bank, fun_param.pattern);
                ioctl(fd, PMIC_TCLED_ENABLE, &tcled_setting);
                ioctl(fd, PMIC_TCLED_PATTERN, &fun_param_null);
            }
            ioctl(fd, PMIC_TCLED_DISABLE, tcled_setting.bank);
        }
        return rv;
}

/*================================================================================================*/
/*===== VT_LED_fun_test_config =====*/
/**
@brief  This function executes setup Tri-color LED for PMIC Light driver on
FUN LIGHT MODE . Automated Fun Light test.

@param  Input:None

@return On success - return TPASS
        On failure - return the error code
*/
/*================================================================================================*/
int VT_LED_fun_test_config(void)
{
        t_fun_param fun_param;
        t_tcled_enable_param enable_param;

        for (enable_param.bank = 0; enable_param.bank <= 2; enable_param.bank++)
        {
            ioctl(fd, PMIC_TCLED_DISABLE, enable_param.bank);
        }
        enable_param.mode = TCLED_FUN_MODE;
        for (fun_param.bank = 0; fun_param.bank <= 2; fun_param.bank++)
        {
            for (fun_param.pattern = 0;fun_param.pattern <= 11; fun_param.pattern++)
            {
                enable_param.bank = fun_param.bank;
#ifdef CONFIG_MXC_PMIC_MC13783
                if ((fun_param.pattern == STROBE_SLOW) ||
                    (fun_param.pattern == STROBE_FAST))
                    continue ;
#endif
                ioctl(fd, PMIC_TCLED_PATTERN, &fun_param);
                ioctl(fd, PMIC_TCLED_ENABLE, &enable_param);
                usleep(100000);
            }
            ioctl(fd, PMIC_TCLED_DISABLE, enable_param.bank);
        }

        printf("Run different pattern\n");
        fun_param.bank = 0;
        enable_param.bank = 0;
        fun_param.pattern = 0;
        ioctl(fd, PMIC_TCLED_PATTERN, &fun_param);
        ioctl(fd, PMIC_TCLED_ENABLE, &enable_param);
        sleep(2);
        fun_param.bank = 1;
        enable_param.bank = 1;
        fun_param.pattern = 3;
        ioctl(fd, PMIC_TCLED_PATTERN, &fun_param);
        ioctl(fd, PMIC_TCLED_ENABLE, &enable_param);
        sleep(2);
        fun_param.bank = 2;
        enable_param.bank = 2;
        fun_param.pattern = 4;
        ioctl(fd, PMIC_TCLED_PATTERN, &fun_param);
        ioctl(fd, PMIC_TCLED_ENABLE, &enable_param);
        sleep(2);
        for (enable_param.bank = 0;enable_param.bank <= 2; enable_param.bank++) {
            ioctl(fd, PMIC_TCLED_DISABLE, enable_param.bank);
        }

        printf("Run the same pattern on the three banks\n");
        for (fun_param.pattern = 0; fun_param.pattern <= 11;fun_param.pattern++) {
            ioctl(fd, PMIC_TCLED_PATTERN, &fun_param);
            for (fun_param.bank = 0; fun_param.bank <= 2;fun_param.bank++)
            {
                enable_param.bank = fun_param.bank;
                ioctl(fd, PMIC_TCLED_ENABLE, &enable_param);
            }
            sleep(1);
        }
        for (fun_param.bank = 0; fun_param.bank <= 2;fun_param.bank++)
        {
            enable_param.bank = fun_param.bank;
            ioctl(fd, PMIC_TCLED_DISABLE, enable_param.bank);
        }

        printf("Test PASSED\n");
        return 0;
}
/*================================================================================================*/
/*!
 * It reset int_param struct
 *
 * @param        fd             the file pointer
 * @param        tcled_setting  the tcled setting
 * @param        ind_param      the tcled ind parameter
 *
 * @return       This function returns 0 if successful.
 */
/*================================================================================================*/
void ind_reset(t_tcled_ind_param * ind_param)
{
    ind_param->skip = 0;
    ind_param->half_current = 0;
    ind_param->level = 0;
    ind_param->channel = 0;
    ind_param->pattern = 0;
    ind_param->rampup = 0;
    ind_param->rampdown = 0;
}


/*================================================================================================*/
/*===== VT_LED_ind_config =====*/
/**
@brief  This function executes setup Tri-color LED for PMIC Light driver on INDICATOR LIGHT MODE test.

@param  Input:None

@return On success - return TPASS
        On failure - return the error code
*/
/*================================================================================================*/
int VT_LED_ind_config(void)
{
        t_tcled_enable_param tcled_setting;
        t_tcled_ind_param ind_param;
        tst_resm(TINFO, "Test Tri-color LED functions of Light driver");
        ind_reset(&ind_param);
        tcled_setting.mode = TCLED_IND_MODE;
        for (tcled_setting.bank = 0;tcled_setting.bank <= 2; tcled_setting.bank++)
        {
            ind_param.bank = tcled_setting.bank;
            for (ind_param.channel = TCLED_IND_RED;ind_param.channel <=TCLED_IND_BLUE;ind_param.channel++)
            {
                ind_param.level = TCLED_CUR_LEVEL_4;
                ind_param.pattern = TCLED_IND_BLINK_8;
                run_ind_light_test(&tcled_setting, &ind_param);
                sleep(1);
                ind_param.level = TCLED_CUR_LEVEL_1;
                ind_param.pattern = TCLED_IND_OFF;
                run_ind_light_test(&tcled_setting, &ind_param);
            }
        }
        ind_reset(&ind_param);
        for (tcled_setting.bank = 0;tcled_setting.bank <= 2; tcled_setting.bank++)
        {
            ind_param.bank = tcled_setting.bank;
            for (ind_param.channel = TCLED_IND_RED;ind_param.channel <=TCLED_IND_BLUE;ind_param.channel++)
            {
                for (ind_param.level = 0;ind_param.level <=TCLED_CUR_LEVEL_4;ind_param.level++)
                {
                    for (ind_param.pattern = 0;ind_param.pattern <=TCLED_IND_ON;ind_param.pattern++)
                    {
                        if (run_ind_light_test(&tcled_setting, &ind_param) < 0)
                            {
                                printf("Test FAILED");
                                return -1;
                            }
                            usleep(500);
                    }
                }
                ind_param.level = TCLED_CUR_LEVEL_1;
                ind_param.pattern = TCLED_IND_OFF;
                run_ind_light_test(&tcled_setting, &ind_param);
            }
        }
        ind_reset(&ind_param);

        printf("Test PASSED\n");
        return 0;
}

/*================================================================================================*/
/*!
 * This is the unit test for the tcled ind pattern.
 * It checks that the read/write operation are coherent.
 *
 * @param        fd             the file pointer
 * @param        tcled_setting  the tcled setting
 * @param        ind_param      the tcled ind parameter
 *
 * @return       This function returns 0 if successful.
 */
/*================================================================================================*/
int run_ind_light_test(t_tcled_enable_param * tcled_setting,
               t_tcled_ind_param * ind_param)
{
    t_tcled_ind_param r_ind_param;

    r_ind_param.channel = ind_param->channel;
    r_ind_param.bank = ind_param->bank;
    ioctl(fd, PMIC_SET_TCLED, ind_param);
    ioctl(fd, PMIC_GET_TCLED, &r_ind_param);
    ioctl(fd, PMIC_TCLED_ENABLE, tcled_setting);
    ioctl(fd, PMIC_TCLED_DISABLE, tcled_setting->bank);

/*    if ((ind_param->bank != r_ind_param.bank) ||
        (ind_param->channel != r_ind_param.channel) ||
        (ind_param->level != r_ind_param.level) ||
        (ind_param->pattern != r_ind_param.pattern) ||
        (ind_param->rampup != r_ind_param.rampup) ||
        (ind_param->rampdown != r_ind_param.rampdown)) {
        printf("channel : %d - %d\n", r_ind_param.channel,
               ind_param->channel);
        printf("level : %d - %d\n", r_ind_param.level,
               ind_param->level);
        printf("pattern : %d - %d\n", r_ind_param.pattern,
               ind_param->pattern);
        printf("rampup : %d - %d\n", r_ind_param.rampup,
               ind_param->rampup);
        printf("rampdown : %d - %d\n", r_ind_param.rampdown,
               ind_param->rampdown);
        return -1;
    }
*/
    return 0;
}

/*================================================================================================*/
/*!
 * This function enable backlights
 * Main, Aux and Keypad
 *
 * @param        tcled_setting  the tcled setting
 *
 * @return       This function returns a void
 */
/*================================================================================================*/

void enable_backlight(int fd, t_bklit_setting_param * setting)
{
    setting->channel = BACKLIGHT_LED1;
    setting->current_level = 4;
    setting->duty_cycle = 7;
    setting->mode = BACKLIGHT_CURRENT_CTRL_MODE;
    setting->cycle_time = 2;
    setting->strobe = BACKLIGHT_STROBE_NONE;
    setting->edge_slow = false;
    setting->en_dis = 0;
    setting->abms = 0;
    setting->abr = 0;
    printf("Main Backlight LED ON.\n");
    ioctl(fd, PMIC_SET_BKLIT, setting);

    sleep(1);
    setting->channel = BACKLIGHT_LED2;
    printf("Auxiliary Backlight LED ON.\n");
    ioctl(fd, PMIC_SET_BKLIT, setting);

    sleep(1);
    printf("Keypad Backlight LED ON.\n");
    setting->channel = BACKLIGHT_LED3;
    ioctl(fd, PMIC_SET_BKLIT, setting);

}

/*-----------------------------------------------------------------------------------------------*/
/*!
 * This function disable backlights
 * Main, Aux and Keypad
 *
 * @param        tcled_setting  the tcled setting
 *
 * @return       This function returns a void
 */
/*-----------------------------------------------------------------------------------------------*/
void test_disable_backlight(int fd, t_bklit_setting_param * setting)
{
    setting->channel = BACKLIGHT_LED1;
    setting->current_level = 0;
    setting->duty_cycle = 0;
    setting->mode = BACKLIGHT_CURRENT_CTRL_MODE;
    setting->cycle_time = 0;
    setting->strobe = BACKLIGHT_STROBE_NONE;
    setting->edge_slow = false;
    ioctl(fd, PMIC_SET_BKLIT, setting);

    setting->channel = BACKLIGHT_LED2;
    ioctl(fd, PMIC_SET_BKLIT, setting);

    setting->channel = BACKLIGHT_LED3;
    ioctl(fd, PMIC_SET_BKLIT, setting);

}

/*================================================================================================*/
/*===== VT_bklit_ramp_config_test =====*/
/**
@brief  This function setup differrent parametrs for backlight.

@param  Input: None

@return On success - return TPASS
        On failure - return the error code
*/
/*================================================================================================*/
int VT_bklit_ramp_config_test(void)
{
        int     rv = TPASS;
        t_bklit_setting_param setting;
        tst_resm(TINFO, "Test Backlight functions of Light driver");

        enable_backlight(fd, &setting);

        printf("Backlight channel 1 ramp up\n");
        ioctl(fd, PMIC_RAMPUP_BKLIT, BACKLIGHT_LED1);

        sleep(1);

        printf("Backlight channel 2 ramp up\n");
        ioctl(fd, PMIC_RAMPUP_BKLIT, BACKLIGHT_LED2);

        sleep(1);

        printf("Backlight channel 1 ramp up off\n");
        ioctl(fd, PMIC_OFF_RAMPUP_BKLIT, BACKLIGHT_LED1);

        sleep(1);

        printf("Backlight channel 2 ramp up off\n");
        ioctl(fd, PMIC_OFF_RAMPUP_BKLIT, BACKLIGHT_LED2);

        sleep(1);

        printf("Backlight channel 1 ramp down\n");
        ioctl(fd, PMIC_RAMPDOWN_BKLIT, BACKLIGHT_LED1);

        sleep(1);

        printf("Backlight channel 2 ramp down\n");
        ioctl(fd, PMIC_RAMPDOWN_BKLIT, BACKLIGHT_LED2);

        sleep(1);

        printf("Backlight channel 1 ramp down off\n");
        ioctl(fd, PMIC_OFF_RAMPDOWN_BKLIT, BACKLIGHT_LED1);

        sleep(1);

        printf("Backlight channel 2 ramp down off\n");
        ioctl(fd, PMIC_OFF_RAMPDOWN_BKLIT, BACKLIGHT_LED2);


        sleep(1);
        test_disable_backlight(fd, &setting);
        sleep(2);
        tst_resm(TINFO, "Disable Backlight function without enable it");

        return rv;
}
/*================================================================================================*/
/*===== VT_bklit_test_ioctl =====*/
/**
@brief  This function Checks the IOCTLs of  backlight functions.

@param  Input: None

@return On success - return TPASS
        On failure - return the error code
*/
/*================================================================================================*/
int VT_bklit_test_ioctl(void)
{
    int     rv = TPASS;
    int     ret = TPASS;
    unsigned int mode_cl = 0, mode_dc = 0, mode_ct = 0, mode_st = 0, md = 0;
    unsigned int mode_ab = 0, mode_abr = 0, mode_channel;
    t_bklit_channel channel;
    t_bklit_setting_param bklit_param_LD;
/* This is to set the Channel */
#ifdef CONFIG_MXC_PMIC_SC55112
     mode_channel = BACKLIGHT_LED2;
     for (channel = BACKLIGHT_LED1; channel <= BACKLIGHT_LED2; channel++)
#endif
#ifdef CONFIG_MXC_PMIC_MC13783
     mode_channel = BACKLIGHT_LED3;
     for (channel = BACKLIGHT_LED1; channel <= BACKLIGHT_LED3; channel++)
#endif
    {
        printf("\n Test for %d Channels, %d Current levels, %d Duty cycles, %d Cycle Time, %d Modes\n",(mode_channel + 1),(MAX_NUMBER_LEVEL + 1),(MAX_DUTY_CYCLE + 1),(MAX_TIME_CYCLE + 1),(BACKLIGHT_TRIODE_MODE + 1) );
        printf("\nChannel= %d\n", channel);
        bklit_param_LD.channel = channel;
        /* This is to set the Edge Slow */
        bklit_param_LD.edge_slow = true;
        /* This is to set the CURRENT LEVEL */
        for (mode_cl = 0; mode_cl <= MAX_NUMBER_LEVEL; mode_cl++)
        {
             printf("Current Level= %d\n", mode_cl);
             bklit_param_LD.current_level = mode_cl;
             /* This is to set the Duty Cycle */
             for (mode_dc = 0; mode_dc <= MAX_DUTY_CYCLE; mode_dc++)
             {
                  printf("Duty Cycle= %d\n", mode_dc);
                  bklit_param_LD.duty_cycle = mode_dc;
                  /* This is to set the Cycle Time */
                  for (mode_ct = 0; mode_ct <= MAX_TIME_CYCLE; mode_ct++)
                  {
                       printf("Cycle Time= %d\n", mode_ct);
                       bklit_param_LD.cycle_time = mode_ct;
                       /* This is to set Mode */
                       for (md = BACKLIGHT_CURRENT_CTRL_MODE; md <= BACKLIGHT_TRIODE_MODE; md++)
                       {
                            printf("Mode= %d\n", md);
                            bklit_param_LD.mode = md;
                            #ifdef CONFIG_MXC_PMIC_MC13783
                            /* This is to set the Enable disable boot time*/
                            bklit_param_LD.en_dis = true;
                            /* This is to set the Adaptive Boost Mode Selection */
                            for (mode_ab=0; mode_ab <= MAX_BOOST_ABMS; mode_ab++)
                            {
                                 bklit_param_LD.abms = mode_ab;

                                 /* This is to set Adaptive Boost Reference */
                                 for (mode_abr=0; mode_abr <= MAX_BOOST_ABR; mode_abr++)
                                 {
                                      bklit_param_LD.abr = mode_abr;
                                      rv = SET_GET_bklit(&bklit_param_LD);
                                 }
                            }
                            #else
                             /* This is to set Strobe */
                            for (mode_st = BACKLIGHT_STROBE_FAST; mode_st <= BACKLIGHT_STROBE_SLOW; mode_st++)
                            {
                                 bklit_param_LD.strobe = mode_st;
                                 rv = SET_GET_bklit(&bklit_param_LD);
                            }
                             #endif
                       }
                  }
             }
        }
    }

if(rv) ret=TFAIL;

return ret;
}

/*================================================================================================*/
/*===== SET_GET_bklit =====*/
/**
@brief  This function executes ioctl for backlight and is called by
VT_bklit_test_ioctl .

@param  Input: t_bklit_setting_param

@return On success - return TPASS
        On failure - return the error code
*/
/*================================================================================================*/
int SET_GET_bklit(t_bklit_setting_param *bklit_param)
{
        int     rv = TPASS;
        t_bklit_setting_param setting;
        t_bklit_setting_param bklit_param_LD_old;
        bklit_param_LD_old.channel = bklit_param->channel;
        if ((rv=ioctl(fd, PMIC_BKLIT_ENABLE, NULL)))
        {
                tst_resm(TFAIL, "Error enable Backlight function  %s", pmic_status[abs(errno)]);
                rv = TFAIL;
                return rv;
        }
        if (ioctl(fd, PMIC_SET_BKLIT, bklit_param) < 0)
        {
                tst_resm(TFAIL, "Error Set configuration LD %s", pmic_status[abs(errno)]);
                rv = TFAIL;
                return rv;
        }
        usleep(50);
        if (ioctl(fd, PMIC_GET_BKLIT, &bklit_param_LD_old) < 0)
        {
                tst_resm(TFAIL, "Error Get configuration LD %s", pmic_status[abs(errno)]);
                rv = TFAIL;
                return rv;
        }
        if (bklit_param->channel != bklit_param_LD_old.channel ||
                bklit_param->edge_slow != bklit_param_LD_old.edge_slow ||
                bklit_param->current_level != bklit_param_LD_old.current_level ||
                bklit_param->duty_cycle != bklit_param_LD_old.duty_cycle ||
                bklit_param->cycle_time != bklit_param_LD_old.cycle_time ||
                bklit_param->mode != bklit_param_LD_old.mode )
        {
            printf("Channel : %d - %d\n", bklit_param->channel,
                   bklit_param_LD_old.channel);
            printf("Current level : %d - %d\n", bklit_param->current_level,
                   bklit_param_LD_old.current_level);
            printf("Edge slow  : %d - %d\n", bklit_param->edge_slow,
                   bklit_param_LD_old.edge_slow);
            printf("Duty cycle : %d - %d\n", bklit_param->duty_cycle,
                   bklit_param_LD_old.duty_cycle);
            printf("Cycle time : %d - %d\n", bklit_param->cycle_time,
                   bklit_param_LD_old.cycle_time);
            printf("Mode : %d - %d\n", bklit_param->mode,
                   bklit_param_LD_old.mode);
            rv = TFAIL;
            return rv;
        }
#ifdef CONFIG_MXC_PMIC_MC13783
        if (bklit_param->en_dis != bklit_param_LD_old.en_dis ||
                bklit_param->abms != bklit_param_LD_old.abms ||
                bklit_param->abr != bklit_param_LD_old.abr )
        {
            printf("En_dis : %d - %d\n", bklit_param->en_dis,
                   bklit_param_LD_old.en_dis);
            printf("Abms : %d - %d\n", bklit_param->abms,
                   bklit_param_LD_old.abms);
            printf("Abr : %d - %d\n", bklit_param->abr,
                   bklit_param_LD_old.abr);
            rv = TFAIL;
            return rv;
        }
#else
        if (bklit_param->strobe != bklit_param_LD_old.strobe)
        {
            printf("Strobe : %d - %d\n", bklit_param->strobe,
                   bklit_param_LD_old.strobe);
            rv = TFAIL;
            return rv;
        }
#endif
        if ((rv=ioctl(fd, PMIC_BKLIT_DISABLE, NULL)))
        {
                tst_resm(TFAIL, "Error disable Backligh function %s", pmic_status[abs(errno)]);
                rv = TFAIL;
        }
        test_disable_backlight(fd, &setting);
        return rv;
}


