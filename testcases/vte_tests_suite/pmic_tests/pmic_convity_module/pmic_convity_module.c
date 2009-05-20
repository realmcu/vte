/*====================*/
/**
        @file   pmic_convity_module.c

        @brief  PMIC CONNECTIVITY dirver API
*/
/*======================

        Copyright (C) 2006, Freescale Semiconductor, Inc. All Rights Reserved
        THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
        BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
        Freescale Semiconductor, Inc.

====================
Revision History:
                            Modification     Tracking
Author/core ID                  Date          Number     Description of Changes
-------------------------   ------------    ----------   -------------------------------------------
V.Khalabuda/hlbv001          13/08/2005     TLSbo52695   Initial version
V.Khalabuda/hlbv001          02/09/2005     TLSbo58397   Update for linux-2.6.10-rel-L26_1_14
A.Ozerov/b00320              15/05/2006     TLSbo64237   4, 6, 9, 12, 14 testcases were changed.
A.Ozerov/b00320              05/07/2006     TLSbo64237   12, 14 testcases were fixed.

====================
Portability:    ARM GCC

======================*/

/*======================
                                        INCLUDE FILES
======================*/
//#include <linux/config.h>
#include <linux/module.h>
#include <linux/device.h>      /* Added on 05/03/06 by RAKESH S JOSHI */
#include <linux/kernel.h>
#include <linux/version.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <asm/uaccess.h>
#include <linux/autoconf.h>

/* Driver specific include files */
#include <asm/arch-mxc/pmic_convity.h>        /* For PMIC Connectivity driver interface. */
#include <linux/pmic_status.h>
#include <linux/pmic_external.h>
#include "pmic_convity_module.h"

#define DEBUG
#ifndef TRACEMSG
#ifdef DEBUG
#define TRACEMSG(fmt,args...) printk( fmt, ##args )
#else
#define TRACEMSG(args...)
#endif /* DEBUG */
#endif /* TRACEMSG */

/*======================
                                        DEFINES AND MACROS
======================*/

/*======================
                                       GLOBAL VARIABLES
======================*/

static struct class *pmic_convity_class;    /* added on 05/03/06 RAKESH S JOSHI */


/*======================
                                    FUNCTION PROTOTYPES
======================*/
int usbCallback(const PMIC_CONVITY_EVENTS event)
{
        return 0;
}

/*====================*/
/*====================*/
/*= VT_pmic_convity_test_reset =*/
/**
@brief  Implementation of the PMIC Connectivity reset with call pmic_convity_reset()

@param  PMIC_CONVITY_HANDLE handle

@return On success - return pass
        On failure - return the error code of last error
*/
/*====================*/
PMIC_STATUS VT_pmic_convity_test_reset(PMIC_CONVITY_HANDLE handle)
{
        PMIC_STATUS status = PMIC_SUCCESS;
        TRACEMSG(KERN_INFO "\n >  Try to reset:\n");
        pmic_convity_reset(handle);
        TRACEMSG(KERN_INFO "\n Reset PMIC Convity handle. PASSED\n");
        return status;
}

/*====================*/
/*= VT_pmic_convity_test_mode =*/
/**
@brief  Implementation of the PMIC Connectivity with call pmic_convity_set_mode() and
                pmic_convity_get_mode() to set mode (USB, RS232) after initially opening with mode (RS232, USB)

@param  PMIC_CONVITY_MODE mode
        PMIC_CONVITY_HANDLE handle

@return On success - return pass
        On failure - return the error code of last error
*/
/*====================*/
PMIC_STATUS VT_pmic_convity_test_mode(PMIC_CONVITY_HANDLE handle, PMIC_CONVITY_MODE mode)
{
        PMIC_CONVITY_MODE mod;
        PMIC_STATUS status = PMIC_SUCCESS;

        TRACEMSG(KERN_INFO
                 "\n  > Try SET and GET operating mode for the PMIC connectivity hardware:\n");

        TRACEMSG("\nGOT mode is : %d\n", mode);
        memset(&mod, 0, sizeof(PMIC_CONVITY_MODE));
        memset(&mode, 0, sizeof(PMIC_CONVITY_MODE));
        pmic_convity_set_mode(handle, mode);
        memset(&mod, 1, sizeof(PMIC_CONVITY_MODE));
        pmic_convity_get_mode(handle, &mod);
        if (mode == mod)
        {
                TRACEMSG("\n Getting the %d mode after setting with %d mode.PASSED\n",mod,mode);
        }
        else
        {
                TRACEMSG("\n Getting the %d mode after setting with %d mode.FAILED\n",mod,mode);
                status = PMIC_ERROR;
        }

        return status;
}

/*====================*/
/*= VT_pmic_convity_test_callback =*/
/**
@brief  Implementation of the PMIC Connectivity with call pmic_convity_set_callback(),
                pmic_convity_get_callback() and pmic_convity_clear_callback

@param  PMIC_CONVITY_HANDLE handle

@return On success - return pass
        On failure - return the error code of last error
*/
/*====================*/
PMIC_STATUS VT_pmic_convity_test_callback(PMIC_CONVITY_HANDLE handle)
{
        int     mask = 1;
        PMIC_CONVITY_CALLBACK pCallback = (PMIC_CONVITY_CALLBACK) NULL;
        PMIC_CONVITY_CALLBACK uCallback = (PMIC_CONVITY_CALLBACK) usbCallback;
        PMIC_CONVITY_EVENTS eventMask = 0;
        PMIC_STATUS status = PMIC_SUCCESS;

        TRACEMSG(" Deregistering the existing callback function and list of events...");
        pmic_convity_clear_callback(handle);

        for (mask = USB_DETECT_4V4_RISE; mask <= USB_DM_HI; mask *= 2)
        {
                /* Register a callback. The specific event isn't important. */
#ifdef CONFIG_MXC_SC55112_PMIC
        if (mask==USB_DETECT_NON_USB_ACCESSORY||mask==USB_DETECT_FACTORY_MODE)
            continue;
#endif
                TRACEMSG
                    ("\n Testing pmic_convity_set_callback(handle,usbCallback,USB_DETECT_...%d\n", mask);
                pmic_convity_set_callback(handle, uCallback, mask);

                /* Now test retrieving the callback function pointer and event mask. */
                TRACEMSG("\n Testing pmic_convity_get_callback(handle, )...\n");

                pmic_convity_get_callback(handle, &pCallback, &eventMask);

                if ((pCallback == uCallback) && (eventMask == mask))
                {
                        TRACEMSG
                            ("\n Retrived the callback function pointer and event mask succesful. PASSED\n");
                }
                else
                {
                        TRACEMSG
                            ("\n Retrieving the callback function pointer and event mask brake up. FAILED\n");
                        status = PMIC_ERROR;
                        return status;
                }
                /* Now test clearing the callback. */
                TRACEMSG("\n Testing pmic_convity_clear_callback(handle)...\n");

                pmic_convity_clear_callback(handle);

        }

        return status;
}

/*====================*/
/*= VT_pmic_convity_test_usb_speed =*/
/**
@brief  Implementation of the PMIC Connectivity with call pmic_convity_usb_set_speed() and
                pmic_convity_usb_get_speed() for get speed and mode settings.

@param  PMIC_CONVITY_HANDLE handle

@return On success - return pass
        On failure - return the error code of last error
*/
/*====================*/
PMIC_STATUS VT_pmic_convity_test_usb_speed(PMIC_CONVITY_HANDLE handle)
{
        PMIC_CONVITY_USB_SPEED speed_s ,speed = 0;
        PMIC_CONVITY_USB_MODE mode_s, mode = 0;
        PMIC_STATUS status = PMIC_SUCCESS;

        /* First set the USB transceiver's speed and mode. */
        TRACEMSG(KERN_INFO
                 "\n Testing pmic_convity_usb_set_speed(handle, USB_LOW_SPEED, USB_HOST)...\n");
        for (speed_s=USB_LOW_SPEED; speed_s<=USB_FULL_SPEED ; speed_s++)
        {
            for (mode_s=USB_HOST; mode_s<=USB_PERIPHERAL ; mode_s++)
            {
                pmic_convity_usb_set_speed(handle, speed_s);
                TRACEMSG(KERN_INFO "\n Testing pmic_convity_usb_get_speed(handle, &speed, &mode)...\n");
                pmic_convity_usb_get_speed(handle, &speed, &mode);
                if (speed == speed_s)
                {
                     TRACEMSG
                         ("\n The USB transceiver's speed:USB_LOW_SPEED and mode:USB_HOST. PASSED\n");
                }
                else
                {
                    TRACEMSG("\n The USB transceiver's speed and mode NOT correct. FAILED\n");
                    TRACEMSG("\n Written speed =%d, mode=%d, but read speed= %d,  mode=%d \n", speed_s, mode_s, speed, mode);
                    return PMIC_ERROR;
                }
            }
        }
        return status;
}

/*====================*/
/*= VT_pmic_convity_test_usb_power_source =*/
/**
@brief  Implementation of the PMIC Connectivity with call pmic_convity_usb_set_power_source() and
                pmic_convity_usb_get_power_source() to retrieve the current configuration.

@param  PMIC_CONVITY_HANDLE handle

@return On success - return pass
        On failure - return the error code of last error
*/
/*====================*/
PMIC_STATUS VT_pmic_convity_test_usb_power_source(PMIC_CONVITY_HANDLE handle)
{
        PMIC_CONVITY_USB_POWER_IN pwrin_s, pwrin = 0;
        PMIC_CONVITY_USB_POWER_OUT pwrout_s, pwrout = 0;
        PMIC_STATUS status = PMIC_SUCCESS;

        /* Set a valid power supply configuration. */
        for (pwrin_s=USB_POWER_INTERNAL_BOOST; pwrin_s<=USB_POWER_INTERNAL; pwrin_s++ )
        {
            for (pwrout_s=USB_POWER_2V775; pwrout_s<=USB_POWER_3V3; pwrout_s++)
            {
#ifdef CONFIG_MXC_PMIC_SC55112
                if(pwrin_s==USB_POWER_INTERNAL||pwrout_s==USB_POWER_2V775)
                    continue;
#endif
                pmic_convity_usb_set_power_source(handle,pwrin_s,pwrout_s);

                /* Now test retrieval of the power supply configuration. */
                pmic_convity_usb_get_power_source(handle, &pwrin, &pwrout);
                if ((pwrin == pwrin_s) && (pwrout == pwrout_s))
                {
                    TRACEMSG
                        ("\n Retrived power supply configuration in:USB_POWER_VBUS and out:USB_POWER_3V3. PASSED\n");
                }
                else
                {
                    TRACEMSG("\n Retrived power supply configuration NOT correctly. FAILED\n");
                    return PMIC_ERROR;
                }
            }
        }
        return status;
}

/*====================*/
/*= VT_pmic_convity_test_usb_xcvr =*/
/**
@brief  Implementation of the PMIC Connectivity with call pmic_convity_usb_set_xcvr() and
                pmic_convity_usb_get_xcvr() to retrieve the current configuration.

@param  PMIC_CONVITY_HANDLE handle

@return On success - return pass
        On failure - return the error code of last error
*/
/*====================*/
PMIC_STATUS VT_pmic_convity_test_usb_xcvr(PMIC_CONVITY_HANDLE handle)
{
        PMIC_CONVITY_USB_TRANSCEIVER_MODE mode = 0;
        PMIC_CONVITY_USB_TRANSCEIVER_MODE i = 0;
        PMIC_STATUS status = PMIC_SUCCESS;

        TRACEMSG(KERN_INFO "\n  > Try to SET and GET the USB transceiver's operating mode:\n");

        for (i = USB_OTG_SRP_DLP_START; i <= USB_OTG_SRP_DLP_STOP; i++)
        {
                TRACEMSG(KERN_INFO "\n Testing pmic_convity_usb_set_xcvr(handle, %d) ...\n", i);
                pmic_convity_usb_set_xcvr(handle, i);

                /* Now test retrieval of the USB transceiver configuration. */
                TRACEMSG(KERN_INFO "\n Testing pmic_convity_usb_get_xcvr(handle, &mode) ...\n");
                (pmic_convity_usb_get_xcvr(handle, &mode));
                if (mode == i)
                {
                        TRACEMSG
                            ("\n Retrieved of the USB transceiver configuration successful. PASSED\n");
                }
                else
                {
                        TRACEMSG
                            ("\n Retrieved of the USB transceiver configuration NOT correctly. FAILED\n");
                         return PMIC_ERROR;
                }
        }

        for (i = USB_SINGLE_ENDED_UNIDIR; i <= USB_SINGLE_ENDED_LOW; i++)
        {
                TRACEMSG(KERN_INFO "\n Testing pmic_convity_usb_set_xcvr(handle, %d) ...\n", i);
                (pmic_convity_usb_set_xcvr(handle, i));

                /* Now test retrieval of the USB transceiver configuration. */
                TRACEMSG(KERN_INFO "\n Testing pmic_convity_usb_get_xcvr(handle, &mode) ...\n");
                (pmic_convity_usb_get_xcvr(handle, &mode));
                if (mode == i)
                {
                        TRACEMSG
                            ("\n Retrieved of the USB transceiver configuration successful. PASSED\n");
                }
                else
                {
                        TRACEMSG
                            ("\n Retrieved of the USB transceiver configuration NOT correctly. FAILED\n");
                         return PMIC_ERROR;
                }
        }

        TRACEMSG(KERN_INFO "\n Testing pmic_convity_usb_set_xcvr(handle, %d) ...\n",
                 USB_DIFFERENTIAL_UNIDIR_RX);
        pmic_convity_usb_set_xcvr(handle, USB_DIFFERENTIAL_UNIDIR_RX);

        /* Now test retrieval of the USB transceiver configuration. */
        TRACEMSG(KERN_INFO "\n Testing pmic_convity_usb_get_xcvr(handle, &mode) ...\n");
        (pmic_convity_usb_get_xcvr(handle, &mode));
        if (mode == USB_DIFFERENTIAL_UNIDIR_RX)
        {
                TRACEMSG("\n Retrieved of the USB transceiver configuration successful. PASSED\n");
        }
        else
        {
                TRACEMSG
                    ("\n Retrieved of the USB transceiver configuration NOT correctly. FAILED\n");
                 return PMIC_ERROR;
        }

        for (i = USB_SUSPEND_ON; i <= USB_SUSPEND_OFF; i++)
        {
                TRACEMSG(KERN_INFO "\n Testing pmic_convity_usb_set_xcvr(handle, %d) ...\n", i);
                (pmic_convity_usb_set_xcvr(handle, i));
                TRACEMSG("\n Setting a USB transceiver configuration successful. PASSED\n");

                /* Now test retrieval of the USB transceiver configuration. */
                TRACEMSG(KERN_INFO "\n Testing pmic_convity_usb_get_xcvr(handle, &mode) ...\n");
                (pmic_convity_usb_get_xcvr(handle, &mode));
                if (mode == i)
                {
                        TRACEMSG
                            ("\n Retrieved of the USB transceiver configuration successful. PASSED\n");
                }
                else
                {
                        TRACEMSG
                            ("\n Retrieved of the USB transceiver configuration NOT correctly. FAILED\n");
                         return PMIC_ERROR;
                }
        }

#ifdef CONFIG_MXC_PMIC_MC13783
        for (i = USB_DIFFERENTIAL_UNIDIR_TX; i <= USB_DIFFERENTIAL_UNIDIR; i++)
        {
                TRACEMSG(KERN_INFO "\n Testing pmic_convity_usb_set_xcvr(handle, %d) ...\n", i);
                (pmic_convity_usb_set_xcvr(handle, i));

                /* Now test retrieval of the USB transceiver configuration. */
                TRACEMSG(KERN_INFO "\n Testing pmic_convity_usb_get_xcvr(handle, &mode) ...\n");
                (pmic_convity_usb_get_xcvr(handle, &mode));
                if (mode == i)
                {
                        TRACEMSG
                            ("\n Retrieved of the USB transceiver configuration successful. PASSED\n");
                }
                else
                {
                        TRACEMSG
                            ("\n Retrieved of the USB transceiver configuration NOT correctly. FAILED\n");
                         return PMIC_ERROR;
                }
        }

        TRACEMSG(KERN_INFO "\n Testing pmic_convity_usb_set_xcvr(handle, %d) ...\n", USB_DIFFERENTIAL_BIDIR);
        pmic_convity_usb_set_xcvr(handle, USB_DIFFERENTIAL_BIDIR);

        /* Now test retrieval of the USB transceiver configuration. */
        TRACEMSG(KERN_INFO "\n Testing pmic_convity_usb_get_xcvr(handle, &mode) ...\n");
        pmic_convity_usb_get_xcvr(handle, &mode);
        if (mode == USB_DIFFERENTIAL_BIDIR)
        {
                TRACEMSG
                    ("\n Retrieved of the USB transceiver configuration successful. PASSED\n");
        }
        else
        {
                TRACEMSG
                    ("\n Retrieved of the USB transceiver configuration NOT correctly. FAILED\n");
                 return PMIC_ERROR;
        }
        TRACEMSG(KERN_INFO "\n Testing pmic_convity_usb_set_xcvr(handle, %d) ...\n", USB_DIFFERENTIAL_BIDIR);
        pmic_convity_usb_set_xcvr(handle, USB_TRANSCEIVER_OFF);

        /* Now test retrieval of the USB transceiver configuration. */
        TRACEMSG(KERN_INFO "\n Testing pmic_convity_usb_get_xcvr(handle, &mode) ...\n");
        pmic_convity_usb_get_xcvr(handle, &mode);
        if (mode == USB_TRANSCEIVER_OFF)
        {
                TRACEMSG
                    ("\n Retrieved of the USB transceiver configuration successful. PASSED\n");
        }
        else
        {
                TRACEMSG
                    ("\n Retrieved of the USB transceiver configuration NOT correctly. FAILED\n");
                 return PMIC_ERROR;
        }
#endif

        return status;
}

/*====================*/
/*= VT_pmic_convity_test_usb_otg_dlp_duration =*/
/**
@brief  Implementation of the PMIC Connectivity with call pmic_convity_usb_otg_set_dlp_duration() and
                pmic_convity_usb_otg_get_dlp_duration() to retrieve the current configuration.

@param  PMIC_CONVITY_HANDLE handle

@return On success - return pass
        On failure - return the error code of last error
*/
/*====================*/
PMIC_STATUS VT_pmic_convity_test_usb_otg_dlp_duration(PMIC_CONVITY_HANDLE handle)
{
        unsigned int setDlpDuration;
        unsigned int getDlpDuration = 0;
        PMIC_STATUS status = PMIC_SUCCESS;

        TRACEMSG(KERN_INFO
                 "\n  > Try to get the current Data Line Pulse duration (in milliseconds) for the USB OTG Session Request Protocol:\n");

        /* Try all valid DLP durations that are supported by PMIC. */
        for (setDlpDuration = 0; setDlpDuration <= 7; setDlpDuration++)
        {
#ifdef CONFIG_MXC_PMIC_SC55112
                pmic_convity_usb_otg_set_dlp_duration(handle, setDlpDuration);
                pmic_convity_usb_otg_get_dlp_duration(handle, &getDlpDuration);
                if (getDlpDuration == setDlpDuration)
                {
                        TRACEMSG("\n Retrieved of the DLP durations correctly set= %d and get =%d. PASSED\n",getDlpDuration,setDlpDuration);
                }
                else
                {
                        TRACEMSG("\n Retrieved of the DLP durations NOT correctly set= %d and get =%d. FAILED\n",getDlpDuration,setDlpDuration);
                        return PMIC_ERROR;
                }
#endif
/* MC13783 is not supported , So just check for PMIC_NOT_SUPPORTED */
#ifdef CONFIG_MXC_PMIC_MC13783
                if (PMIC_NOT_SUPPORTED==pmic_convity_usb_otg_set_dlp_duration(handle,setDlpDuration))
                {
                     TRACEMSG("\n DLP durations Not supported on MC13783. So cannot be set\n");
                }
                else
                {
                     TRACEMSG("\n Driver Returned Error. Driver FAILED Set\n");
                     return PMIC_ERROR;
                }
                if(PMIC_NOT_SUPPORTED==pmic_convity_usb_otg_get_dlp_duration(handle,&getDlpDuration))
                {
                     TRACEMSG("\n DLP durations Not supported on MC13783.So cannot get\n");
                }
                else
                {
                     TRACEMSG("\n Driver Returned Error. Driver FAILED to Get\n");
                     return PMIC_ERROR;
                }
#endif
        }
        return status;
}

/*====================*/
/*= VT_pmic_convity_test_usb_otg_config =*/
/**
@brief  Implementation of the PMIC Connectivity with call pmic_convity_usb_otg_set_config(),
                pmic_convity_usb_otg_clear_config() and pmic_convity_usb_otg_get_config()
                to retrieve the current configuration.

@param  PMIC_CONVITY_HANDLE handle

@return On success - return pass
        On failure - return the error code of last error
*/
/*====================*/
PMIC_STATUS VT_pmic_convity_test_usb_otg_config(PMIC_CONVITY_HANDLE handle)
{
        PMIC_CONVITY_USB_OTG_CONFIG config = 0;
        PMIC_CONVITY_USB_OTG_CONFIG i = 0;
        PMIC_STATUS status = PMIC_SUCCESS;

        TRACEMSG(KERN_INFO
                 "\n  > Try to SET, GET and CLEAR the USB On-The-Go (OTG) configuration:\n");

#ifdef CONFIG_MXC_SC55112_PMIC
        for (i = USB_OTG_SE0CONN; i <= USB_VBUS_CURRENT_LIMIT_LOW; i *= 2)
        {
                TRACEMSG(KERN_INFO "\n Try to SET a valid USB OTG configuration...\n");
                pmic_convity_usb_otg_set_config(handle, i);

                TRACEMSG(KERN_INFO
                         "\n Testing pmic_convity_usb_otg_get_config(handle, &config)...\n");
                (pmic_convity_usb_otg_get_config(handle, &config));
                TRACEMSG("\n config: 0x%x, i: 0x%x\n", (config & i), i);
                if ( config & i )
                {
                        TRACEMSG("\n Retrieved USB OTG configuration correctly. PASSED\n");
                }
                else
                {
                        TRACEMSG("\n Retrieved USB OTG configuration NOT correctly. FAILED\n");
                        return PMIC_ERROR;
                }
                TRACEMSG(KERN_INFO "\n Try to CLEAR the USB OTG configuration...\n");
                (pmic_convity_usb_otg_clear_config(handle, i));
        }
#endif

#ifdef CONFIG_MXC_PMIC_MC13783
        for (i = USB_VBUS_CURRENT_LIMIT_LOW_10MS; i <= USB_VBUS_CURRENT_LIMIT_LOW_60MS; i *= 2)
        {
                TRACEMSG(KERN_INFO "\n Try to SET a valid USB OTG configuration...\n");
                pmic_convity_usb_otg_set_config(handle, i);

                TRACEMSG(KERN_INFO
                         "\n Testing pmic_convity_usb_otg_get_config(handle, &config)...\n");
                (pmic_convity_usb_otg_get_config(handle, &config));
                TRACEMSG("\n config: 0x%x, i: 0x%x\n", (config & i), i);
                if (config & i)
                {
                        TRACEMSG("\n Retrieved USB OTG configuration correctly. PASSED\n");
                }
                else
                {
                        TRACEMSG("\n Retrieved USB OTG configuration NOT correctly. FAILED\n");
                        return PMIC_ERROR;
                }
                TRACEMSG(KERN_INFO "\n Try to CLEAR the USB OTG configuration...\n");
                pmic_convity_usb_otg_clear_config(handle, i);
        }
#endif
        /* Set another valid USB OTG configuration. */
        for (i = USB_VBUS_PULLDOWN; i <= USBXCVREN; i *= 2)
        {
                TRACEMSG(KERN_INFO "\n Try to SET a valid USB OTG configuration...\n");
                (pmic_convity_usb_otg_set_config(handle, i));

                TRACEMSG(KERN_INFO
                         "\n Testing pmic_convity_usb_otg_get_config(handle, &config)...\n");
                (pmic_convity_usb_otg_get_config(handle, &config));
                TRACEMSG("\n config: 0x%x, i: 0x%x\n", (config & i), i);
                if (config & i)
                {
                        TRACEMSG("\n Retrieved USB OTG configuration correctly. PASSED\n");
                }
                else
                {
                        TRACEMSG("\n Retrieved USB OTG configuration NOT correctly. FAILED\n");
                        return PMIC_ERROR;
                }
                TRACEMSG(KERN_INFO "\n Try to CLEAR the USB OTG configuration...\n");
                pmic_convity_usb_otg_clear_config(handle, i);
        }
        return status;
}

/*====================*/
/*= VT_pmic_convity_test_rs232_config =*/
/**
@brief  Implementation of the PMIC Connectivity with call pmic_convity_rs232_set_config() and
                pmic_convity_rs232_get_config() - Functions for controlling RS-232 serial connectivity

@param  PMIC_CONVITY_HANDLE handle

@return On success - return pass
        On failure - return the error code of last error
*/
/*====================*/
PMIC_STATUS VT_pmic_convity_test_rs232_config(PMIC_CONVITY_HANDLE handle)
{
        PMIC_CONVITY_RS232_INTERNAL cfgInternal_s, cfgInternal = 0;
        PMIC_CONVITY_RS232_EXTERNAL cfgExternal_s, cfgExternal = 0;
        PMIC_STATUS status = PMIC_SUCCESS;

        TRACEMSG(KERN_INFO
                 "\n  > Try to SET and GET the connectivity interface's current RS-232 operating configuration:\n");

        for(cfgInternal_s = RS232_TX_USE0VM_RX_UDATVP; cfgInternal_s <= RS232_TX_RX_INTERNAL_DEFAULT; cfgInternal_s++)
        {
#ifdef CONFIG_MXC_PMIC_SC55112
            if (cfgInternal_s == RS232_TX_USE0VM_RX_UDATVP || cfgInternal_s == RS232_TX_UDATVP_RX_URXVM)
                continue;
#endif
#ifdef CONFIG_MXC_PMIC_MC13783
            if (cfgInternal_s == RS232_TX_UTXDI_RX_URXDO)
                continue;
#endif
            for(cfgExternal_s = RS232_TX_UDM_RX_UDP; cfgExternal_s <= RS232_TX_RX_EXTERNAL_DEFAULT; cfgExternal_s++)
            {
#ifdef CONFIG_MXC_PMIC_MC13783
                if (cfgExternal_s == RS232_TX_UDP_RX_UDM )
                    continue;
#endif
                TRACEMSG(KERN_INFO "\n Calling pmic_convity_rs232_set_config...\n");
                pmic_convity_rs232_set_config(handle, cfgInternal_s, cfgExternal_s);

                /* Now test the retrieval of the RS-232 configuration. */
                TRACEMSG(KERN_INFO
                     "Calling pmic_convity_rs232_get_config(handle, &cfgInternal, &cfgExternal) ...");
                (pmic_convity_rs232_get_config(handle, &cfgInternal, &cfgExternal));
                if ((cfgInternal == cfgInternal_s) && (cfgExternal == cfgExternal))
                {
                    TRACEMSG("\n Retrieved the RS-232 configuration correctly. PASSED\n");
                }
                else
                {
                    TRACEMSG("\n Retrieved the RS-232 configuration NOT correctly. FAILED\n");
                    return PMIC_ERROR;
                }
            }
        }
        return status;
}

/*====================*/
/*= VT_pmic_convity_test_cea936_exit_signal =*/
/**
@brief  Implementation of the PMIC Connectivity CEA936 exit signal
                with call pmic_convity_cea936_exit_signal()

@param  PMIC_CONVITY_HANDLE handle

@return On success - return pass
        On failure - return the error code of last error
*/
/*====================*/
PMIC_STATUS VT_pmic_convity_test_cea936_exit_signal(PMIC_CONVITY_HANDLE handle)
{
        PMIC_CONVITY_CEA936_EXIT_SIGNAL i = 0;
        PMIC_STATUS status;
        PMIC_STATUS rv = PMIC_SUCCESS;

        TRACEMSG(KERN_INFO
                 "\n  > Signal the attached device to exit the current CEA-936 operating mode:\n");

        for (i = CEA936_UID_NO_PULLDOWN; i <= CEA936_UDMPULSE; i++)
        {
                /* Call pmic_convity_cea936_exit_signal() with an exit signal Expect * *
                * PMIC_NOT_SUPPORTED. */
                TRACEMSG(KERN_INFO "\n Testing pmic_convity_cea936_exit_signal(handle, %d)...\n",
                         i);
                status = pmic_convity_cea936_exit_signal(handle, i);

#ifdef CONFIG_MXC_SC55112_PMIC
                if (status == PMIC_NOT_SUPPORTED)
                {
                        TRACEMSG("\n Attached device to exit the current CEA-936 NOT SUPPORTED. PASSED\n");
                }
                else
                {
                        TRACEMSG("\n Attached device to exit the current CEA-936 FAILED\n");
                        rv = PMIC_ERROR;
                }
#else
                if (status != PMIC_SUCCESS)
                {
                        TRACEMSG("\n Attached device to exit the current CEA-936 FAILED\n");
                        rv = PMIC_ERROR;
                }
                else
                {
                        TRACEMSG("\n Attached device to exit the current CEA-936 PASSED\n");
                }
#endif
        }

        return rv;
}

/*====================*/
/*= VT_pmic_convity_test_open =*/
/**
@brief  Implementation of the PMIC Connectivity pmic_convity_open() API unit tests.

@param  char mode

@return On success - return pass
        On failure - return the error code of last error
*/
/*====================*/
PMIC_STATUS VT_pmic_convity_test_open(PMIC_CONVITY_HANDLE * handle, PMIC_CONVITY_MODE mode)
{
        PMIC_STATUS status;

        TRACEMSG(KERN_INFO "\n  > Try to open device handle in mode\n");

        /* Try to acquire the connectivity device handle in mode, expect PMIC_SUCCESS.  */
        status = pmic_convity_open(handle, mode);
        if (status != PMIC_SUCCESS)
                TRACEMSG("\n Opening break up. FAILED\n");

        return status;
}

/*====================*/
/*= VT_pmic_convity_test_close =*/
/**
@brief  Implementation of the PMIC Connectivity pmic_convity_close() API unit tests.

@param  PMIC_CONVITY_HANDLE handle

@return On success - return pass
        On failure - return the error code of last error
*/
/*====================*/
PMIC_STATUS VT_pmic_convity_test_close(PMIC_CONVITY_HANDLE handle)
{
        PMIC_STATUS status;

        TRACEMSG(KERN_INFO "\n  > Try to close device handle\n");

        /* Try closing the handle, expect PMIC_SUCCESS. */
        status = pmic_convity_close(handle);
        if (status != PMIC_SUCCESS)
            TRACEMSG("\n Closing break up. FAILED\n");

        return status;
}

/*====================*/
static int pmic_test_open(struct inode *inode, struct file *filp)
{
        return 0;
}

static ssize_t pmic_test_read(struct file *file, char *buf, size_t count, loff_t * ppos)
{
        return 0;
}

static ssize_t pmic_test_write(struct file *filp, const char *buf, size_t count, loff_t * ppos)
{
        return 0;
}

/*====================*/
static int pmic_test_ioctl(struct inode *inode, struct file *file, unsigned int cmd,
                         unsigned long arg)
{
        pmic_covity_test_param *comp_param;
        PMIC_STATUS status = PMIC_ERROR;

        TRACEMSG(KERN_INFO "\n pmic_convity_module : pmic_test_ioctl()\n");
        switch (cmd)
        {
        case PMIC_CONVITY_TEST_OPEN:
                if ((comp_param = kmalloc(sizeof(pmic_covity_test_param), GFP_KERNEL)) == NULL)
                {
                        return -ENOMEM;
                }
                if (copy_from_user(comp_param, (pmic_covity_test_param *) arg,
                                   sizeof(pmic_covity_test_param)))
                {
                        kfree(comp_param);
                        return -EFAULT;
                }
                printk ("\n Test MODE =%d\n", comp_param->mode);
                status=VT_pmic_convity_test_open(&comp_param->handle, comp_param->mode);
                if (status != PMIC_SUCCESS )
                {
                    printk ("\n Test Failedwith status =%d\n", status);
                    return -EFAULT;
                }
                if (copy_to_user((pmic_covity_test_param *) arg, comp_param,
                                 sizeof(pmic_covity_test_param)))
                {
                        return -EFAULT;
                }
                kfree(comp_param);
                break;
        case PMIC_CONVITY_TEST_CLOSE:
                if ((comp_param = kmalloc(sizeof(pmic_covity_test_param), GFP_KERNEL)) == NULL)
                {
                        return -ENOMEM;
                }
                if (copy_from_user(comp_param, (pmic_covity_test_param *) arg,
                                   sizeof(pmic_covity_test_param)))
                {
                        kfree(comp_param);
                        return -EFAULT;
                }
                status=VT_pmic_convity_test_close(comp_param->handle);
                if (status != PMIC_SUCCESS )
                {
                    printk ("\n Test Failedwith status =%d\n", status);
                    return -EFAULT;
                }
                break;
        case PMIC_CONVITY_TEST_MODE:
                if ((comp_param = kmalloc(sizeof(pmic_covity_test_param), GFP_KERNEL)) == NULL)
                {
                        return -ENOMEM;
                }
                if (copy_from_user(comp_param, (pmic_covity_test_param *) arg,
                                   sizeof(pmic_covity_test_param)))
                {
                        kfree(comp_param);
                        return -EFAULT;
                }
                status = VT_pmic_convity_test_mode(comp_param->handle, comp_param->mode);
                if (status != PMIC_SUCCESS )
                {
                    printk ("\n Test Failed with status =%d\n", status);
                    return -EFAULT;
                }
                if (copy_to_user((pmic_covity_test_param *) arg, comp_param,
                                 sizeof(pmic_covity_test_param)))
                {
                        return -EFAULT;
                }
                kfree(comp_param);
                break;
        case PMIC_CONVITY_TEST_CALLBACK:
                if ((comp_param = kmalloc(sizeof(pmic_covity_test_param), GFP_KERNEL)) == NULL)
                {
                        return -ENOMEM;
                }
                if (copy_from_user(comp_param, (pmic_covity_test_param *) arg,
                                   sizeof(pmic_covity_test_param)))
                {
                        kfree(comp_param);
                        return -EFAULT;
                }
                status = VT_pmic_convity_test_callback(comp_param->handle);
                if (status != PMIC_SUCCESS )
                {
                    printk ("\n Test Failed with status =%d\n", status);
                    return -EFAULT;
                }
                if (copy_to_user((pmic_covity_test_param *) arg, comp_param,
                                 sizeof(pmic_covity_test_param)))
                {
                        return -EFAULT;
                }
                kfree(comp_param);
                break;
        case PMIC_CONVITY_TEST_USB_SPEED:
                if ((comp_param = kmalloc(sizeof(pmic_covity_test_param), GFP_KERNEL)) == NULL)
                {
                        return -ENOMEM;
                }
                if (copy_from_user(comp_param, (pmic_covity_test_param *) arg,
                                   sizeof(pmic_covity_test_param)))
                {
                        kfree(comp_param);
                        return -EFAULT;
                }
                status = VT_pmic_convity_test_usb_speed(comp_param->handle);
                if (status != PMIC_SUCCESS )
                {
                    printk ("\n Test Failed with status =%d\n", status);
                    return -EFAULT;
                }
                if (copy_to_user((pmic_covity_test_param *) arg, comp_param,
                                 sizeof(pmic_covity_test_param)))
                {
                        return -EFAULT;
                }
                kfree(comp_param);
                break;
        case PMIC_CONVITY_TEST_USB_POWER:
                if ((comp_param = kmalloc(sizeof(pmic_covity_test_param), GFP_KERNEL)) == NULL)
                {
                        return -ENOMEM;
                }
                if (copy_from_user(comp_param, (pmic_covity_test_param *) arg,
                                   sizeof(pmic_covity_test_param)))
                {
                        kfree(comp_param);
                        return -EFAULT;
                }
                status = VT_pmic_convity_test_usb_power_source(comp_param->handle);
                if (status != PMIC_SUCCESS )
                {
                    printk ("\n Test Failed with status =%d\n", status);
                    return -EFAULT;
                }
                if (copy_to_user((pmic_covity_test_param *) arg, comp_param,
                                 sizeof(pmic_covity_test_param)))
                {
                        return -EFAULT;
                }
                kfree(comp_param);
                break;
        case PMIC_CONVITY_TEST_USB_TRANSCEIVER_MODE:
                if ((comp_param = kmalloc(sizeof(pmic_covity_test_param), GFP_KERNEL)) == NULL)
                {
                        return -ENOMEM;
                }
                if (copy_from_user(comp_param, (pmic_covity_test_param *) arg,
                                   sizeof(pmic_covity_test_param)))
                {
                        kfree(comp_param);
                        return -EFAULT;
                }
                status = VT_pmic_convity_test_usb_xcvr(comp_param->handle);
                if (status != PMIC_SUCCESS )
                {
                    printk ("\n Test Failed with status =%d\n", status);
                    return -EFAULT;
                }
                if (copy_to_user((pmic_covity_test_param *) arg, comp_param,
                                 sizeof(pmic_covity_test_param)))
                {
                        return -EFAULT;
                }
                kfree(comp_param);
                break;
        case PMIC_CONVITY_TEST_USB_OTG_DLP_DURATION:
                if ((comp_param = kmalloc(sizeof(pmic_covity_test_param), GFP_KERNEL)) == NULL)
                {
                        return -ENOMEM;
                }
                if (copy_from_user(comp_param, (pmic_covity_test_param *) arg,
                                   sizeof(pmic_covity_test_param)))
                {
                        kfree(comp_param);
                        return -EFAULT;
                }
                status = VT_pmic_convity_test_usb_otg_dlp_duration(comp_param->handle);
                if (status != PMIC_SUCCESS )
                {
                    printk ("\n Test Failed with status =%d\n", status);
                    return -EFAULT;
                }
                if (copy_to_user((pmic_covity_test_param *) arg, comp_param,
                                 sizeof(pmic_covity_test_param)))
                {
                        return -EFAULT;
                }
                kfree(comp_param);
                break;
        case PMIC_CONVITY_TEST_USB_OTG_CONFIG:
                if ((comp_param = kmalloc(sizeof(pmic_covity_test_param), GFP_KERNEL)) == NULL)
                {
                        return -ENOMEM;
                }
                if (copy_from_user(comp_param, (pmic_covity_test_param *) arg,
                                   sizeof(pmic_covity_test_param)))
                {
                        kfree(comp_param);
                        return -EFAULT;
                }
                status = VT_pmic_convity_test_usb_otg_config(comp_param->handle);
                if (status != PMIC_SUCCESS )
                {
                    printk ("\n Test Failed with status =%d\n", status);
                    return -EFAULT;
                }
                if (copy_to_user((pmic_covity_test_param *) arg, comp_param,
                                 sizeof(pmic_covity_test_param)))
                {
                        return -EFAULT;
                }
                kfree(comp_param);
                break;
        case PMIC_CONVITY_TEST_RS232:
                if ((comp_param = kmalloc(sizeof(pmic_covity_test_param), GFP_KERNEL)) == NULL)
                {
                        return -ENOMEM;
                }
                if (copy_from_user(comp_param, (pmic_covity_test_param *) arg,
                                   sizeof(pmic_covity_test_param)))
                {
                        kfree(comp_param);
                        return -EFAULT;
                }
                status = VT_pmic_convity_test_rs232_config(comp_param->handle);
                if (status != PMIC_SUCCESS )
                {
                    printk ("\n Test Failed with status =%d\n", status);
                    return -EFAULT;
                }
                if (copy_to_user((pmic_covity_test_param *) arg, comp_param,
                                 sizeof(pmic_covity_test_param)))
                {
                        return -EFAULT;
                }
                kfree(comp_param);
                break;
        case PMIC_CONVITY_TEST_CEA936:
                if ((comp_param = kmalloc(sizeof(pmic_covity_test_param), GFP_KERNEL)) == NULL)
                {
                        return -ENOMEM;
                }
                if (copy_from_user(comp_param, (pmic_covity_test_param *) arg,
                                   sizeof(pmic_covity_test_param)))
                {
                        kfree(comp_param);
                        return -EFAULT;
                }
                status = VT_pmic_convity_test_cea936_exit_signal(comp_param->handle);
                if (status != PMIC_SUCCESS )
                {
                    printk ("Test Failed with status =%d\n", status);
                    return -EFAULT;
                }
                break;
        case PMIC_CONVITY_TEST_RESET:
                if ((comp_param = kmalloc(sizeof(pmic_covity_test_param), GFP_KERNEL)) == NULL)
                {
                        return -ENOMEM;
                }
                if (copy_from_user(comp_param, (pmic_covity_test_param *) arg,
                                   sizeof(pmic_covity_test_param)))
                {
                        kfree(comp_param);
                        return -EFAULT;
                }
                status = VT_pmic_convity_test_reset(comp_param->handle);
                if (status != PMIC_SUCCESS )
                {
                    printk ("Test Failed with status =%d\n", status);
                    return -EFAULT;
                }
                break;
        default:
                printk("\n Connectivity TEST DRV: Uncknown CMD=%d\n", cmd);
                return 0;
                break;
        }
        return 0;
}

/*====================*/
static int pmic_test_release(struct inode *inode, struct file *filp)
{
        return 0;
}

/*======================
                                GLOBAL VARIABLE DECLARATIONS
======================*/
static struct file_operations pmic_test_fops =
{
        owner:THIS_MODULE,
        open:pmic_test_open,
        release:pmic_test_release,
        read:pmic_test_read,
        write:pmic_test_write,
        ioctl:pmic_test_ioctl,
};

/*====================*/
static int __init pmic_test_init(void)
{
        int     res;

        TRACEMSG(KERN_INFO "PMIC Connectivity Test: creating virtual device\n");
        res = register_chrdev(231, PMIC_CONVITY_DEV, &pmic_test_fops);

        if (res < 0)
        {
                TRACEMSG(KERN_INFO "PMIC Connectivity Test: unable to register the device\n");
                return res;
        }

        pmic_convity_class = class_create(THIS_MODULE, PMIC_CONVITY_DEV);
        if (IS_ERR(pmic_convity_class))
        {
                printk(KERN_ALERT "class simple created failed\n");
                goto err_out;
        }

        if (IS_ERR(class_device_create(pmic_convity_class,NULL,MKDEV(231, 0), NULL, PMIC_CONVITY_DEV)))
        {
                printk(KERN_ALERT "class simple add failed\n");
                goto err_out;
        }

        //---devfs_mk_cdev(MKDEV(231, 0), S_IFCHR | S_IRUGO | S_IWUGO, PMIC_CONVITY_DEV);
        return 0;

        err_out:
        printk(KERN_ERR "PMIC_CONVITY : error creating convity test module class.\n");
        class_device_destroy(pmic_convity_class , MKDEV(231, 0));
        class_destroy(pmic_convity_class);
        unregister_chrdev(231, PMIC_CONVITY_DEV);
        return -1;
}

/*====================*/
static void __exit pmic_test_exit(void)
{
        unregister_chrdev(231, PMIC_CONVITY_DEV);
        class_device_destroy(pmic_convity_class ,MKDEV(231, 0));
        class_destroy(pmic_convity_class);
        //---devfs_remove(PMIC_CONVITY_DEV);
        TRACEMSG(KERN_INFO "PMIC Connectivity Test: removing virtual device\n");
}

/*====================*/

module_init(pmic_test_init);
module_exit(pmic_test_exit);

MODULE_DESCRIPTION("Test Module for PMIC Connectivity driver");
MODULE_AUTHOR("FreeScale");
MODULE_LICENSE("GPL");
