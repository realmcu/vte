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
        @file   pmic_adc_test.c

        @brief  Test scenario C source for PMIC(SC55112 and MC13783) ADC driver.

====================================================================================================
Revision History:
                            Modification     Tracking
Author/core id                  Date          Number     Description of Changes
-------------------------   ------------    ----------   -------------------------------------------
V. Halabuda/HLBV001          07/25/2005     TLSbo52694   Initial version
V. Halabuda/HLBV001          11/21/2005     TLSbo58395   Update for linux-2.6.10-rel-L26_1_14
E. Gromazina/NONE            12/27/2005     TLSbo59968   Update for MXC91231 and MXC91131
D. Khoroshev/b00313          05/18/2006     TLSbo64235   Added callback for comparator test and
                                                         removed timeout for touch screen tests
D. Khoroshev/b00313          07/06/2006     TLSbo64235   Added PMIC ADC test module
D. Khoroshev/b00313          07/26/2006     TLSbo64235   Added mc13783 legacy support
====================================================================================================
Portability: ARM GCC
==================================================================================================*/

/*==================================================================================================
                                        INCLUDE FILES
==================================================================================================*/
/* Verification Test Environment Include Files */
#include "pmic_adc_test.h"

/*==================================================================================================
                                      GLOBAL VARIABLES
==================================================================================================*/
extern char *TCID;
extern int adc_testcase;
extern char adc_device[128];
extern sig_atomic_t sig_count;
extern int verbose_flag;

/*==================================================================================================
                                       LOCAL VARIABLES
==================================================================================================*/
unsigned int argument;
int fd = 0;
#if defined(CONFIG_MXC_PMIC_SC55112) || defined( CONFIG_MXC_PMIC_MC13783 )
static t_adc_convert_param adc_config;
#endif

/*================================================================================================*/
static char *CHANNELS[]=
{
        "BATTERY_VOLTAGE",
        "BATTERY_CURRENT",
        "CHARGE_VOLTAGE",
        "CHARGE_CURRENT",
        "APPLICATION_SUPPLY",
        "TS_X_POS1",
        "TS_X_POS2",
        "TS_Y_POS1",
        "TS_Y_POS2",
        "GEN_PURPOSE_AD4",
        "GEN_PURPOSE_AD5",
        "GEN_PURPOSE_AD6",
        "GEN_PURPOSE_AD7",
        "GEN_PURPOSE_AD8",
        "GEN_PURPOSE_AD9",
        "GEN_PURPOSE_AD10",
        "GEN_PURPOSE_AD11",
        "USB_ID",
        "LICELL",
        "RAWEXTBPLUSSENSE",
        "MPBSENSE",
        "BATSENSE",
        "GND",
        "THERMISTOR",
        "DIE_TEMP"
};

/*==================================================================================================
                                        LOCAL FUNCTIONS
==================================================================================================*/

/*================================================================================================*/
/*===== kbhit =====*/
/**
@brief  Checks state of stdin input stream. This function wait for changing state of stdin with timeout
        SLEEP_TIME.
@param  Output: pnSleepTime - returns elapsed time in milliseconds.

@return Returns true if stream contains unread symbols, unless return false.
*/
/*================================================================================================*/
int kbhit(int *pnSleepTime)
{
        fd_set rset;
        struct timeval tv;
        int nSelect;

        FD_ZERO(&rset);
        FD_SET(fileno(stdin), &rset);

        tv.tv_sec = 0;
        tv.tv_usec = SLEEP_TIME;

        *pnSleepTime = 0;

        nSelect = select(fileno(stdin) + 1, &rset, NULL, NULL, &tv);

        if (nSelect == -1)
                return 0;

        /*  Calculate the elapsed time */
        *pnSleepTime = SLEEP_TIME;
        if (nSelect > 0)
                *pnSleepTime = tv.tv_usec;

        return nSelect > 0;
}

#if defined(CONFIG_MXC_PMIC_SC55112) || defined( CONFIG_MXC_PMIC_MC13783 )
/*================================================================================================*/
/*===== comparator_cb =====*/
/**
@brief  Checks state of stdin input stream. This function wait for changing state of stdin with timeout
        SLEEP_TIME.
@param  Output: pnSleepTime - returns elapsed time in milliseconds.

@return Returns true if stream contains unread symbols, unless return false.
*/
/*================================================================================================*/
void comparator_cb(t_comp_exception reason)
{
        switch(reason)
        {
        case GTWHIGH:
                tst_resm(TINFO, "Comparator callback called. Reason GTWHIGH");
                break;
        case LTWLOW:
                tst_resm(TINFO, "Comparator callback called. Reason GTWHIGH");
                break;
        default:
                tst_resm(TINFO, "Comparator callback called. Reason unknown");
        }
}
#endif

/*================================================================================================*/
/*===== VT_pmic_adc_test_setup =====*/
/**
@brief  assumes the pre-condition of the test case execution

@param  None

@return On success - return TPASS
        On failure - return TFAIL
*/
/*================================================================================================*/
int VT_pmic_adc_test_setup(void)
{
        int VT_rv = TPASS;

        if ((fd = open(adc_device, O_RDWR)) < 0)
        {
                tst_resm(TFAIL, "Failed open device");
                return TFAIL;
        }

        tst_resm(TINFO, "Initialize ADC");
        if (ioctl(fd, PMIC_ADC_INIT, NULL) < 0)
        {
                VT_rv = TFAIL;
                tst_resm(VT_rv, "Failed initializes ADC device, ERROR CODE is %s", strerror(errno));
        }

        return VT_rv;
}

/*================================================================================================*/
/*===== VT_pmic_adc_test_cleanup =====*/
/**
@brief  assumes the post-condition of the test case execution

@param  None

@return On success - return TPASS
        On failure - return TFAIL
*/
/*================================================================================================*/
int VT_pmic_adc_test_cleanup(void)
{
        int VT_rv = TPASS;

        tst_resm(TINFO, "Deinitialize ADC");
        if (ioctl(fd, PMIC_ADC_DEINIT, NULL) < 0)
        {
                VT_rv = TFAIL;
                tst_resm(VT_rv, "Failed deinitialize ADC device, ERROR CODE is %s", strerror(errno));
        }
        if (fd > 0)
                close (fd);

        return VT_rv;
}

/*================================================================================================*/
/*===== ask_user =====*/
/**
@brief  Asks user to answer the question and read the answer y/n?

@param  Input:  msg message to show
        Output: None

@return 1 - if user asks "No,  wrong"
        0 - if user asks "Yes, right"
*/
/*================================================================================================*/
int ask_user(char *msg)
{
        int retValue = TFAIL;
        unsigned char answer;
        int retKeyPress = 2;

        do
        {
                tst_resm(TINFO, msg);
                fflush(stdout);
                answer = fgetc(stdin);

                if(answer == 'Y' || answer == 'y')
                        retKeyPress = 0;
                else

                if(answer == 'N' || answer == 'n')
                        retKeyPress = 1;

        }
        while(retKeyPress == 2);

        fgetc(stdin);       /* Wipe CR character from stream */

        if(!retKeyPress)
                retValue = TPASS;

        return retValue;
}

/*================================================================================================*/
/*===== VT_pmic_adc_test =====*/
/**
@brief  SC55112 test scenario convert, touch, battery and comparator functions

@param  None

@return On success - return TPASS
        On failure - return the error code and TFAIL
*/
/*================================================================================================*/
int VT_pmic_adc_test(void)
{
        int trv;
        short buffer[4];
        int sleep_time;
        char ch=0;
        int i = 0;
        int VT_rv = TPASS;
        t_touch_screen samp;
        t_touch_mode mod = 0;

        memset(&samp,0,sizeof(t_touch_screen));
        switch (adc_testcase)
        {
        case PMIC_ADC_CONVERT_T:
            memset(adc_config.result, 0, sizeof(unsigned short)*16);
            for(i = 0; i < 24 ; i++)
            {
#ifdef CONFIG_MXC_PMIC_SC55112

            if (i == 0 || i == 2 || i == 4 || i == 15 || i == 16 || i == 23 || i == 24)
                    continue;
#else
            if (i == 9 || i == 19 || i == 20 || i == 21 || i == 22)
                    continue;
#endif
                   adc_config.channel = i;
                   tst_resm(TINFO, "Convert one channel (%s) ", CHANNELS[i]);
                   if (ioctl(fd, PMIC_ADC_CONVERT, &adc_config) < 0)
                   {
                           tst_resm(TFAIL, "Failed convert one channel (%s). ERROR CODE is %s", CHANNELS[i], strerror(errno));
                           VT_rv = TFAIL;
                   }
           }
           break;

        case PMIC_ADC_CONVERT_8X_T:

            for(i = 0; i < 24 ; i++)
            {
#ifdef CONFIG_MXC_PMIC_SC55112
            if (i == 0 || i == 2 || i == 4 || i == 15 || i == 16 || i == 23 || i == 24)
                    continue;
#else
            if (i == 9 || i == 19 || i == 20 || i == 21 || i == 22)
                    continue;

#endif
                        adc_config.channel = i;
                        tst_resm(TINFO, "Convert one channel (%s) eight samples", CHANNELS[i]);
                        if (ioctl(fd, PMIC_ADC_CONVERT_8X, &adc_config) < 0)
                        {
                                VT_rv = TFAIL;
                                tst_resm(VT_rv, "Failed convert one channel (%s) eight samples ADC device. ERROR CODE is %s",  CHANNELS[i], strerror(errno));
                        }
        }
                break;
        case PMIC_ADC_CONVERT_MULTICHANNEL_T:
                adc_config.channel = 0x000c;
                tst_resm(TINFO, "Convert multiple channels");
                if (ioctl(fd, PMIC_ADC_CONVERT_MULTICHANNEL, &adc_config) < 0)
                {
                        VT_rv = TFAIL;
                        tst_resm(VT_rv, "Failed convert multiple channels is 0x00ff ADC device. ERROR CODE is %s", strerror(errno));
                }
                break;
        case PMIC_ADC_SET_TOUCH_MODE_T:
                tst_resm(TINFO, "Set touch screen operation mode");
                for (i = TS_X_POSITION; i <= TS_NONE; i++)
                {
                        if (ioctl(fd, PMIC_ADC_SET_TOUCH_MODE, i) < 0)
                        {
                                VT_rv = TFAIL;
                                tst_resm(VT_rv, "Failed to set touch screen operation mode %s. ERROR CODE is %s",
                                                i, strerror(errno));
                        }
                        if (ioctl(fd, PMIC_ADC_GET_TOUCH_MODE, &mod) < 0)
                        {
                                VT_rv = TFAIL;
                                tst_resm(VT_rv, "Failed to get screen operation mode. ERROR CODE is %s",
                                                strerror(errno));
                        }
                        if (i != mod)
                        {
                                VT_rv = TFAIL;
                                tst_resm(VT_rv, "Touch screen operation mode was not set (set: %d, get: %d)",
                                                i, mod);
                        }

                }
                if (ioctl(fd, PMIC_ADC_SET_TOUCH_MODE, TS_X_POSITION) < 0)
                {
                        VT_rv = TFAIL;
                        tst_resm(VT_rv, "Failed to set touch screen operation mode %s. ERROR CODE is %s",
                                               TS_X_POSITION, strerror(errno));
                }
                break;
         case PMIC_ADC_GET_BATTERY_CURRENT_T:
                tst_resm(TINFO, "Get battery current level");
                if (ioctl(fd, PMIC_ADC_GET_BATTERY_CURRENT, &argument) < 0)
                {
                        VT_rv = TFAIL;
                        tst_resm(VT_rv, "Failed to get battery current level. ERROR CODE is %s",
                                        strerror(errno));
                }
                else if (verbose_flag)
                {
                        tst_resm(TINFO, "Battery current level is %x", argument);
                }
                break;

        case PMIC_ADC_GET_TOUCH_SAMPLE_T:
        {
                sig_count = 0; /* Disable time out exiting */
                tst_resm(TINFO, "Please, press on the touch screen");
                if (ioctl(fd, PMIC_ADC_GET_TOUCH_SAMPLE, &samp) < 0)
                {
                        VT_rv = TFAIL;
                        tst_resm(VT_rv, "Failed to get screen sample. ERROR CODE is %s", strerror(errno));
                }
                else if (verbose_flag)
                {
			tst_resm(TINFO,"X position   is %d", samp.x_position);
                        tst_resm(TINFO,"X position 1 is %d", samp.x_position1);
                        tst_resm(TINFO,"X position 2 is %d", samp.x_position2);
                        tst_resm(TINFO,"X position 3 is %d", samp.x_position3);
                        tst_resm(TINFO,"Y position   is %d", samp.y_position);
                        tst_resm(TINFO,"Y position 1 is %d", samp.y_position1);
                        tst_resm(TINFO,"Y position 2 is %d", samp.y_position2);
                        tst_resm(TINFO,"Y position 3 is %d", samp.y_position3);
                        tst_resm(TINFO,"contact_resistance is %d", samp.contact_resistance);
                }
                break;
        }
        case PMIC_ADC_ACTIVATE_COMPARATOR_T:
        {
                t_adc_comp_param comp;
                comp.wlow = 0x0f;
                comp.whigh = 0xf0;
                comp.callback = comparator_cb;
                tst_resm(TINFO, "Activate comparator");
                if (ioctl(fd, PMIC_ADC_ACTIVATE_COMPARATOR, &comp) < 0)
                {
                        VT_rv = TFAIL;
                        tst_resm(VT_rv, "Failed to activate comparator. ERROR CODE is %s", strerror(errno));
                }
                tst_resm(TINFO, "Deactive comparator ADC device");
                if (ioctl(fd, PMIC_ADC_DEACTIVE_COMPARATOR, NULL) < 0)
                {
                        VT_rv = TFAIL;
                        tst_resm(VT_rv, "Failed to deactive comparator. ERROR CODE is %s", strerror(errno));
                }
                break;
        }
        case TS_READ_T:
                tst_resm(TINFO, "Installing touch screen read interface");
                if (ioctl(fd, TOUCH_SCREEN_READ_INSTALL, NULL) < 0)
                {
                        VT_rv = TFAIL;
                        tst_resm(VT_rv, "Failed install touch screen read interface. ERROR CODE is %s",
                                        strerror(errno));
                }
                tst_resm(TINFO,"\nTouch the screen with a stylus to read touch screen value or press 'E' to exit.");
                sig_count = 0; /* Disable timeout */
                for (;;)
                {
                        trv = read(fd, buffer, sizeof buffer);
                        if(trv != 0 && (buffer[0] || buffer[1] || buffer[2] || buffer[3]))
                        {
                                tst_resm(TINFO, "Values:\t contact:%d x:%d y:%d pad:%d\tReturn:%d",
                                                buffer[0], buffer[1], buffer[2], buffer[3], trv);
                                if (trv < 0)
                                {
                                        tst_resm(TFAIL, "Not valid coordinates. Error code %s", strerror(trv));
                                        VT_rv  = TFAIL;
                                }
                                tst_resm(TINFO,"\nTouch the screen with a stylus to read touch screen value or press 'E' to exit.");
                        }
                        if (kbhit(&sleep_time))
                        {
                                ch = getchar();
                                if (ch == 'E' || ch == 'e')
                                        break;
                        }
                }

                tst_resm(TINFO, "Remove touch screen read interface");

                if (ioctl(fd, TOUCH_SCREEN_READ_UNINSTALL, NULL) < 0)
                {
                        VT_rv = TFAIL;
                        tst_resm(VT_rv, "Failed remove touch screen read interface. ERROR CODE is %s",
                                        strerror(errno));
                }
                VT_rv = ask_user("Does the test worked right [Y/N]?");
                break;
        }

        return VT_rv;
}

