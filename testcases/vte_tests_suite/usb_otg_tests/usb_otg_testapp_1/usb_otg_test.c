/*===============================================================================================*/
/**
        @file   usb_otg_test.c

        @brief  source file for USB-OTG driver test.
*/
/*==================================================================================================

        Copyright (C) 2006, Freescale Semiconductor, Inc. All Rights Reserved
        THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
        BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
        Freescale Semiconductor, Inc.

====================================================================================================
Revision History:
                            Modification     Tracking
Author/core ID                  Date          Number    Description of Changes
-------------------------   ------------    ----------  --------------------------------------------
A.Ozerov/B00320              07/04/2006     TLSbo58840  Initial version
A.Ozerov/B00320              29/06/2006     TLSbo71035  Execution order of test cases was changed.
                                                        One test case was removed.
                                                        Some useless functions were removed.

====================================================================================================
Portability: ARM GCC

==================================================================================================*/

/*==================================================================================================
                                        INCLUDE FILES
==================================================================================================*/
/* Harness Specific Include Files. */
#include <test.h>

/* Verification Test Environment Include Files */
#include "usb_otg_test.h"

/*==================================================================================================
                                        LOCAL VARIABLES
==================================================================================================*/
extern char *TCID;
extern int fd;
extern int vflag;

int     rv;
int     i = 0;
int     count_1 = 1;
int     count_2 = 1;

char   *otg_strings[] = 
{
        "invalid_state",
        "otg_disabled",
        "otg_disable_tcd",
        "otg_disable_hcd",
        "otg_disable_pcd",
        "otg_disable_ocd",
        "otg_enable_ocd",
        "otg_enable_pcd",
        "otg_enable_hcd",
        "otg_enable_tcd",
        "otg_enabled",
        "peripheral_idle",
        "peripheral_dropped",
        "peripheral_wait",
        "peripheral_bus_reset",
        "peripheral_addressed",
        "peripheral_configured",
        "peripheral_discharge_vbus",
        "peripheral_suspended",
        "peripheral_wakeup_enabled",
        "peripheral_wakeup",
        "host_idle",
        "host_idle_dropped",
        "host_wait_vrise",
        "host_wait_vrise_overcurrent",
        "host_wait_port_connect",
        "host_port_connected",
        "host_bus_reset",
        "host_addressed",
        "host_configured",
        "host_vbus_err",
        "host_wait_dischrg",
        "host_wait_vfall",
        "terminator_state"
};

char   *otg_meta_states[] = 
{
        "m_a_idle",
        "m_a_wait_vrise",
        "m_a_wait_bcon",
        "m_a_host",
        "m_a_suspend",
        "m_a_peripheral",
        "m_a_wait_vfall",
        "m_a_vbus_err",
        "m_b_idle",
        "m_b_srp_init",
        "m_b_peripheral",
        "m_b_suspend",
        "m_b_wait_acon",
        "m_b_host",
        "m_b_suspended",
        "m_ph_disc",
        "m_ph_init",
        "m_ph_uart",
        "m_ph_aud",
        "m_ph_wait",
        "m_ph_exit",
        "m_cr_init",
        "m_cr_uart",
        "m_cr_aud",
        "m_cr_ack",
        "m_cr_wait",
        "m_cr_disc",
        "m_otg_init",
        "m_usb_accessory",
        "m_usb_factory",
        "m_unknown"
};

char    info_string[] = "new info";

/*==================================================================================================
                                            STRUCTURES
==================================================================================================*/
struct otg_firmware_info firmware_info;
struct otg_test test;
struct otg_admin_command command;
struct otg_status_update status_update;
struct otg_state state;

struct otg_state otg_states_mn[] = 
{
        {       /* 0 */
         invalid_state, /* .state */
         m_otg_init,    /* .meta */
         "invalid_state",       /* .name */
         0,     /* .tmout */
         0,     /* .reset */
         },
        {       /* 1 */
         otg_disabled,  /* .state */
         m_otg_init,    /* .meta */
         "otg_disabled",        /* .name */
         0,     /* .tmout */
         enable_otg_ | PCD_OK_ | TCD_OK_ | HCD_OK_,     /* .reset */
         /* .outputs */
         chrg_vbus_out_ | drv_vbus_out_ | charge_pump_out_ | dm_pullup_out_ |
         dp_pullup_out_ | loc_sof_out_ | hcd_rh_out_,
         },
        {       /* 2 */
         otg_disable_tcd,       /* .state */
         m_otg_init,    /* .meta */
         "otg_disable_tcd",     /* .name */
         0,     /* .tmout */
         TCD_OK_,       /* .reset */
         /* .outputs */
         tcd_init_out_,
         },
        {       /* 3 */
         otg_disable_hcd,       /* .state */
         m_otg_init,    /* .meta */
         "otg_disable_hcd",     /* .name */
         0,     /* .tmout */
         HCD_OK_,       /* .reset */
         /* .outputs */
         hcd_init_out_,
         },
        {       /* 4 */
         otg_disable_pcd,       /* .state */
         m_otg_init,    /* .meta */
         "otg_disable_pcd",     /* .name */
         0,     /* .tmout */
         PCD_OK_,       /* .reset */
         /* .outputs */
         pcd_init_out_,
         },
        {       /* 5 */
         otg_disable_ocd,       /* .state */
         m_otg_init,    /* .meta */
         "otg_disable_ocd",     /* .name */
         0,     /* .tmout */
         OCD_OK_,       /* .reset */
         /* .outputs */
         ocd_init_out_,
         },
        {       /* 6 */
         otg_enable_ocd,        /* .state */
         m_otg_init,    /* .meta */
         "otg_enable_ocd",      /* .name */
         0,     /* .tmout */
         OCD_OK_,       /* .reset */
         /* .outputs */
         ocd_init_out,
         },
        {       /* 7 */
         otg_enable_pcd,        /* .state */
         m_otg_init,    /* .meta */
         "otg_enable_pcd",      /* .name */
         0,     /* .tmout */
         PCD_OK_,       /* .reset */
         /* .outputs */
         pcd_init_out,
         },
        {       /* 8 */
         otg_enable_hcd,        /* .state */
         m_otg_init,    /* .meta */
         "otg_enable_hcd",      /* .name */
         0,     /* .tmout */
         HCD_OK_,       /* .reset */
         /* .outputs */
         hcd_init_out,
         },
        {       /* 9 */
         otg_enable_tcd,        /* .state */
         m_otg_init,    /* .meta */
         "otg_enable_tcd",      /* .name */
         0,     /* .tmout */
         TCD_OK_,       /* .reset */
         /* .outputs */
         tcd_init_out,
         },
        {       /* 10 */
         otg_enabled,   /* .state */
         m_otg_init,    /* .meta */
         "otg_enabled", /* .name */
         TST_ONE_SECOND,        /* .tmout */
         b_bus_drop_,   /* .reset */
         /* .outputs */
         dm_det_out_ | dp_det_out_,
         },
        {       /* 11 */
         peripheral_idle,       /* .state */
         m_b_idle,      /* .meta */
         "peripheral_idle",     /* .name */
         0,     /* .tmout */
         0,     /* .reset */
         /* .outputs */
         dp_pullup_out_ | hcd_rh_out | pcd_en_out_ | tcd_en_out_power |
         dischrg_vbus_out_ | dp_pulldown_out | dm_pulldown_out | hcd_en_out_,
         },
        {       /* 12 */
         peripheral_dropped,    /* .state */
         m_b_idle,      /* .meta */
         "peripheral_dropped",  /* .name */
         0,     /* .tmout */
         0,     /* .reset */
         },
        {       /* 13 */
         peripheral_wait,       /* .state */
         m_b_peripheral,        /* .meta */
         "peripheral_wait",     /* .name */
         0,     /* .tmout */
         BUS_RESET_,    /* .reset */
         /* .outputs */
         dp_pullup_out | pcd_en_out | dp_pulldown_out_ | dm_pulldown_out_ | hcd_en_out_,
         },
        {       /* 14 */
         peripheral_bus_reset,  /* .state */
         m_b_peripheral,        /* .meta */
         "peripheral_bus_reset",        /* .name */
         0,     /* .tmout */
         BUS_RESET_ | ADDRESSED_,       /* .reset */
         },
        {       /* 15 */
         peripheral_addressed,  /* .state */
         m_b_peripheral,        /* .meta */
         "peripheral_addressed",        /* .name */
         0,     /* .tmout */
         ADDRESSED_ | CONFIGURED_,      /* .reset */
         },
        {       /* 16 */
         peripheral_configured, /* .state */
         m_b_peripheral,        /* .meta */
         "peripheral_configured",       /* .name */
         0,     /* .tmout */
         CONFIGURED_ | BUS_SUSPENDED_,  /* .reset */
         },
        {       /* 17 */
         peripheral_discharge_vbus,     /* .state */
         m_b_peripheral,        /* .meta */
         "peripheral_discharge_vbus",   /* .name */
         TLDISC_DSCHRG, /* .tmout */
         0,     /* .reset */
         /* .outputs */
         dp_pullup_out_ | dischrg_vbus_out | pcd_en_out_ | hcd_en_out_,
         },
        {       /* 18 */
         peripheral_suspended,  /* .state */
         m_b_suspended, /* .meta */
         "peripheral_suspended",        /* .name */
         0,     /* .tmout */
         0,     /* .reset */
         },
        {       /* 19 */
         peripheral_wakeup_enabled,     /* .state */
         m_b_suspended, /* .meta */
         "peripheral_wakeup_enabled",   /* .name */
         0,     /* .tmout */
         remote_wakeup_cmd_,    /* .reset */
         /* .outputs */
         remote_wakeup_out_,
         },
        {       /* 20 */
         peripheral_wakeup,     /* .state */
         m_b_suspended, /* .meta */
         "peripheral_wakeup",   /* .name */
         0,     /* .tmout */
         0,     /* .reset */
         /* .outputs */
         remote_wakeup_out,
         },
        {       /* 21 */
         host_idle,     /* .state */
         m_a_idle,      /* .meta */
         "host_idle",   /* .name */
         0,     /* .tmout */
         0,     /* .reset */
         /* .outputs */
         chrg_vbus_out_ | hcd_en_out | hcd_rh_out_ | dp_pulldown_out |
         dm_pulldown_out | HUB_PORT_CONNECT_ | pcd_en_out_,
         },
        {       /* 22 */
         host_idle_dropped,     /* .state */
         m_a_idle,      /* .meta */
         "host_idle_dropped",   /* .name */
         0,     /* .tmout */
         0,     /* .reset */
         },
        {       /* 23 */
         host_wait_vrise,       /* .state */
         m_a_wait_vrise,        /* .meta */
         "host_wait_vrise",     /* .name */
         TA_WAIT_VRISE, /* .tmout */
         0,     /* .reset */
         /* .outputs */
         drv_vbus_out,
         },
        {       /* 24 */
         host_wait_vrise_overcurrent,   /* .state */
         m_a_wait_vrise,        /* .meta */
         "host_wait_vrise_overcurrent", /* .name */
         TST_ONE_SECOND,        /* .tmout */
         0,     /* .reset */
         },
        {       /* 25 */
         host_wait_port_connect,        /* .state */
         m_a_wait_bcon, /* .meta */
         "host_wait_port_connect",      /* .name */
         0,     /* .tmout */
         0,     /* .reset */
         /* .outputs */
         loc_sof_out_ | loc_suspend_out_ | dp_pullup_out_ | hcd_rh_out,
         },
        {       /* 26 */
         host_port_connected,   /* .state */
         m_a_host,      /* .meta */
         "host_port_connected", /* .name */
         TST_TWO_SECOND * 5,    /* .tmout */
         BUS_RESET_,    /* .reset */
         /* .outputs */
         loc_sof_out,
         },
        {       /* 27 */
         host_bus_reset,        /* .state */
         m_a_host,      /* .meta */
         "host_bus_reset",      /* .name */
         0,     /* .tmout */
         a_suspend_req_,        /* .reset */
         },
        {       /* 28 */
         host_addressed,        /* .state */
         m_a_host,      /* .meta */
         "host_addressed",      /* .name */
         0,     /* .tmout */
         0,     /* .reset */
         },
        {       /* 29 */
         host_configured,       /* .state */
         m_a_host,      /* .meta */
         "host_configured",     /* .name */
         TST_TWO_SECOND,        /* .tmout */
         CONFIGURED_ | HNP_CAPABLE | HNP_ENABLED,       /* .reset */
         },
        {       /* 30 */
         host_vbus_err, /* .state */
         m_a_vbus_err,  /* .meta */
         "host_vbus_err",       /* .name */
         0,     /* .tmout */
         clr_err_cmd_ | a_suspend_req_, /* .reset */
         /* .outputs */
         hcd_en_out | pcd_en_out_ | drv_vbus_out_ | charge_pump_out_ |
         dp_pullup_out_ | loc_sof_out_ | loc_suspend_out_ | dp_pulldown_out |
         dm_pulldown_out | hcd_rh_out_,
         },
        {       /* 31 */
         host_wait_dischrg,     /* .state */
         m_a_wait_vfall,        /* .meta */
         "host_wait_dischrg",   /* .name */
         0,     /* .tmout */
         0,     /* .reset */
         /* .outputs */
         drv_vbus_out_ | charge_pump_out_ | loc_sof_out_ | tcd_en_out_power |
         hcd_rh_out_ | dischrg_vbus_out,
         },
        {       /* 32 */
         host_wait_vfall,       /* .state */
         m_a_wait_vfall,        /* .meta */
         "host_wait_vfall",     /* .name */
         TST_ONE_SECOND,        /* .tmout */
         0,     /* .reset */
         /* .outputs */
         hcd_en_out_ | loc_suspend_out_ | pcd_en_out_ | chrg_vbus_out_,
         },
        {       /* 33 */
         terminator_state,      /* .state */
         m_otg_init,    /* .meta */
         "terminator_state",    /* .name */
         0,     /* .tmout */
         0,     /* .reset */
         /* .outputs */
         0,
         },

        {0, 0, "", 0, 0,},
};

struct otg_test otg_tests_mn[] = 
{
        /* 
        * This the initial state of the software when first loaded.
        * It is not possible to return to this state.
        */
        {       /* Initialize by sending the otg_enable signal. */
         0,     /* .test */
         invalid_state, /* .state */
         otg_disabled,  /* .target */
         enable_otg,    /* .test1 */
         },
        /* 
        * The USBOTG State Machine has been initialized but is inactive.
        * This state may have arrived at from either the invalid_state or
        * from the otg_disable state.
        */
        {       /* Initialize by sending the otg_enable signal. */
         1,     /* .test */
         otg_disabled,  /* .state */
         otg_enable_ocd,        /* .target */
         enable_otg,    /* .test1 */
         },
        /* 
        * The State Machine stops the device drivers and waits for them
        * to signal that they have finished de-initializing.
        */
        {       /* Wait for ok from de-initializing drivers. */
         2,     /* .test */
         otg_disable_tcd,       /* .state */
         otg_disable_hcd,       /* .target */
         TCD_OK,        /* .test1 */
         },
        /* 
        * The State Machine stops the device drivers and waits for them
        * to signal that they have finished de-initializing.
        */
        {       /* Wait for ok from de-initializing drivers. */
         3,     /* .test */
         otg_disable_hcd,       /* .state */
         otg_disable_pcd,       /* .target */
         HCD_OK,        /* .test1 */
         },
        /* 
        * The State Machine stops the device drivers and waits for them
        * to signal that they have finished de-initializing.
        */
        {       /* Wait for ok from de-initializing drivers. */
         4,     /* .test */
         otg_disable_pcd,       /* .state */
         otg_disable_ocd,       /* .target */
         PCD_OK,        /* .test1 */
         },
        /* 
        * The State Machine stops the device drivers and waits for them
        * to signal that they have finished de-initializing.
        */
        {       /* Wait for ok from de-initializing drivers. */
         5,     /* .test */
         otg_disable_ocd,       /* .state */
         otg_disabled,  /* .target */
         OCD_OK,        /* .test1 */
         },
        /* 
        * The State Machine starts the device drivers and waits for them
        * to signal that they have finished initializing.
        */
        {       /* Initialize by sending the otg_enable signal. */
         6,     /* .test */
         otg_disabled,  /* .state */
         otg_enable_ocd,        /* .target */
         enable_otg,    /* .test1 */
         },
        {       /* Wait for ok from initializing drivers. */
         7,     /* .test */
         otg_enable_ocd,        /* .state */
         otg_enable_pcd,        /* .target */
         OCD_OK,        /* .test1 */
         },
        /* 
        * The State Machine starts the device drivers and waits for them
        * to signal that they have finished initializing.
        */
        {       /* Wait for ok from initializing drivers. */
         8,     /* .test */
         otg_enable_pcd,        /* .state */
         otg_enable_hcd,        /* .target */
         PCD_OK,        /* .test1 */
         },
        /* 
        * The State Machine starts the device drivers and waits for them
        * to signal that they have finished initializing.
        */
        {       /* Wait for ok from initializing drivers. */
         9,     /* .test */
         otg_enable_hcd,        /* .state */
         otg_enable_tcd,        /* .target */
         HCD_OK,        /* .test1 */
         },
        /* 
        * The State Machine starts the device drivers and waits for them
        * to signal that they have finished initializing.
        */
        {       /* Wait for ok from initializing drivers. */
         10,    /* .test */
         otg_enable_tcd,        /* .state */
         otg_enabled,   /* .target */
         TCD_OK,        /* .test1 */
         },
        /* ! The State Machine has successfully started the device drivers and is waiting for an *
        * input event. Typically it will move from here to an idle state specific to the current *
        * conditions (peripheral_idle etc.) based on user request b_bus_drop. */
        {       /* Check for disable. */
         11,    /* .test */
         otg_enabled,   /* .state */
         otg_disable_tcd,       /* .target */
         enable_otg_,   /* .test1 */
         Tst_one_second,        /* .test2 */
         },
        {       /* Move to idle. */
         12,    /* .test */
         otg_enabled,   /* .state */
         peripheral_idle,       /* .target */
         ID_GND_,       /* .test1 */
         enable_otg,    /* .test2 */
         },
        {       /* Move to idle. */
         13,    /* .test */
         otg_enabled,   /* .state */
         host_idle,     /* .target */
         ID_GND,        /* .test1 */
         enable_otg,    /* .test2 */
         },
        /* ! USB Peripheral is idle. Waiting for Vbus to indicate that it has been plugged into a USB 
        * Host. */
        {       /* Check for disable (must be done for check for bus_drop.) */
         14,    /* .test */
         peripheral_idle,       /* .state */
         otg_enabled,   /* .target */
         ID_GND | enable_otg_,  /* .test1 */
         },
        {       /* Check for b_bus_drop */
         15,    /* .test */
         peripheral_idle,       /* .state */
         peripheral_dropped,    /* .target */
         b_bus_drop,    /* .test1 */
         },
        {       /* Move to peripheral mode when SESSION valid. */
         16,    /* .test */
         peripheral_idle,       /* .state */
         peripheral_wait,       /* .target */
         B_SESS_VLD,    /* .test1 */
         },
        /* ! USB Peripheral, user has dropped the bus. */
        {       /* Wait for b_bus_drop/ or enable_otg/ */
         17,    /* .test */
         peripheral_dropped,    /* .state */
         peripheral_idle,       /* .target */
         ID_GND | b_bus_drop_ | enable_otg_,    /* .test1 */
         },
        /* ! USB Peripheral, Vbus sensed, enabling pullup. The D+ pullup is enabled and we are *
        * waiting for a BUS_RESET to indicate that the USB Host has recognized that a USB Device is
        * * attached. */
        {       /* Move to idle if we loose any of these inputs. */
         18,    /* .test */
         peripheral_wait,       /* .state */
         peripheral_idle,       /* .target */
         ID_GND | enable_otg_ | B_SESS_VLD_ | b_bus_drop,       /* .test1 */
         },
        {       /* Move to next state if bus reset is seen. */
         19,    /* .test */
         peripheral_wait,       /* .state */
         peripheral_bus_reset,  /* .target */
         BUS_RESET,     /* .test1 */
         },
        /* ! USB Peripheral, waiting to be addressed. It is waiting to be enumerated and configured * 
        * by the USB Host. */
        {       /* Move to idle via discharge, if we loose any of these inputs. */
         20,    /* .test */
         peripheral_bus_reset,  /* .state */
         peripheral_discharge_vbus,     /* .target */
         ID_GND | enable_otg_ | B_SESS_VLD_ | b_bus_drop,       /* .test1 */
         },
        {       /* Progress if we are addressed. */
         21,    /* .test */
         peripheral_bus_reset,  /* .state */
         peripheral_addressed,  /* .target */
         ADDRESSED,     /* .test1 */
         },
        /* ! The State Machine in the configured state for a Traditional USB Device. This means that
        * * there is an active session, there is packet traffic with this device. */
        {       /* Move to idle via discharge, if we loose any of these inputs. */
         22,    /* .test */
         peripheral_addressed,  /* .state */
         peripheral_discharge_vbus,     /* .target */
         ID_GND | enable_otg_ | B_SESS_VLD_ | b_bus_drop,       /* .test1 */
         },
        {       /* Progress if we are configured. */
         23,    /* .test */
         peripheral_addressed,  /* .state */
         peripheral_configured, /* .target */
         CONFIGURED,    /* .test1 */
         },
        /* ! The State Machine in the configured state for a Traditional USB Device. This means that
        * * there is an active session, there is packet traffic with this device. */
        {       /* */
         24,    /* .test */
         peripheral_configured, /* .state */
         peripheral_suspended,  /* .target */
         BUS_SUSPENDED, /* .test1 */
         },
        {       /* Move to idle via discharge, if we loose any of these inputs. */
         25,    /* .test */
         peripheral_configured, /* .state */
         peripheral_discharge_vbus,     /* .target */
         ID_GND | enable_otg_ | B_SESS_VLD_ | b_bus_drop,       /* .test1 */
         },
        /* ! The State Machine in the discharge state for a Traditional USB Device. The device has *
        * been unplugged. The Vbus discharge resistor will be enabled for the TLDISC_DSCHRG time *
        * period. */
        {       /* Progress to idle on timeout. */
         26,    /* .test */
         peripheral_discharge_vbus,     /* .state */
         peripheral_idle,       /* .target */
         Tldisc_dschrg, /* .test1 */
         },
        /* ! The State Machine in the suspend state for a Traditional USB Device. */
        {       /* Move to idle via discharge, if we loose any of these inputs. */
         27,    /* .test */
         peripheral_suspended,  /* .state */
         peripheral_discharge_vbus,     /* .target */
         ID_GND | enable_otg_ | B_SESS_VLD_ | b_bus_drop,       /* .test1 */
         },
        {       /* Check for a resumed bus. */
         28,    /* .test */
         peripheral_suspended,  /* .state */
         peripheral_configured, /* .target */
         BUS_SUSPENDED_,        /* .test1 */
         },
        {       /* Is remote wakeup enabled? */
         29,    /* .test */
         peripheral_suspended,  /* .state */
         peripheral_wakeup_enabled,     /* .target */
         REMOTE_WAKEUP_ENABLED, /* .test1 */
         },
        /* ! The State Machine in the suspend state for a Traditional USB Device, prior to suspended
        * * the USB Host enabled Remote Wakeup by sending a set REMOTE WAKUP request. */
        {       /* Move to idle via discharge, if we loose any of these inputs. */
         30,    /* .test */
         peripheral_wakeup_enabled,     /* .state */
         peripheral_discharge_vbus,     /* .target */
         enable_otg_ | B_SESS_VLD_,     /* .test1 */
         },
        {       /* Check for a resumed bus. */
         31,    /* .test */
         peripheral_wakeup_enabled,     /* .state */
         peripheral_suspended,  /* .target */
         BUS_SUSPENDED_,        /* .test1 */
         },
        {       /* Remote wakeup requested? */
         32,    /* .test */
         peripheral_wakeup_enabled,     /* .state */
         peripheral_wakeup,     /* .target */
         remote_wakeup_cmd,     /* .test1 */
         },
        /* ! The State Machine in the wakeup state for a Traditional USB Device, The REMOTE WAKEUP *
        * procedure will be performed. */
        {       /* Automatic return. */
         33,    /* .test */
         peripheral_wakeup,     /* .state */
         peripheral_wakeup_enabled,     /* .target */
         AUTO | AUTO_,  /* .test1 */
         },
        /* ! A-Device idle state. An A-Plug is inserted in the Mini A-B Receptacle. This is the Host
        * * Only idle state. Waiting for user to allow the bus to be used. N.B. Reset all progress * 
        * indicator inputs here. */
        {       /* */
         34,    /* .test */
         host_idle,     /* .state */
         host_idle_dropped,     /* .target */
         a_bus_drop,    /* .test1 */
         },
        {       /* Check for ID change or disable (this must be before b_bus_drop test.) */
         35,    /* .test */
         host_idle,     /* .state */
         otg_enabled,   /* .target */
         ID_GND_ | enable_otg_, /* .test1 */
         },
        {       /* */
         36,    /* .test */
         host_idle,     /* .state */
         host_wait_vrise,       /* .target */
         AUTO | AUTO_,  /* .test1 */
         },
        /* ! A-Device idle state. An A-Plug is inserted in the Mini A-B Receptacle. This is the Host
        * * Only idle state. Waiting for user to allow the bus to be used. N.B. Reset all progress * 
        * indicator inputs here. */
        {       /* */
         37,    /* .test */
         host_idle_dropped,     /* .state */
         host_idle,     /* .target */
         a_bus_drop_ | ID_GND_ | enable_otg_,   /* .test1 */
         },
        /* ! First check if external charge pump is required.  We force a wait of Ta_wait_vrise *
        * here, arriving in host wait port connect to quickly causes system hangs occasionally. */
        {       /* */
         38,    /* .test */
         host_wait_vrise,       /* .state */
         host_idle,     /* .target */
         ID_GND_ | enable_otg_ | a_bus_drop,    /* .test1 */
         },
        {       /* */
         39,    /* .test */
         host_wait_vrise,       /* .state */
         host_wait_port_connect,        /* .target */
         VBUS_VLD,      /* .test1 */
         Ta_wait_vrise, /* .test2 */
         },
        {       /* */
         40,    /* .test */
         host_wait_vrise,       /* .state */
         host_wait_vrise_overcurrent,   /* .target */
         Ta_wait_vrise, /* .test1 */
         },
        /* ! A-Device Overcurrent condition (taking too long for Vbus to become valid.) */
        {       /* */
         41,    /* .test */
         host_wait_vrise_overcurrent,   /* .state */
         host_wait_port_connect,        /* .target */
         Tst_one_second,        /* .test1 */
         },
        /* ! Wait for a connection. A-Device wait for B-Device Connect. This is the normal route *
        * using the long debounce window. This state can optionally use the Ta_wait_bcon timeout *
        * (OTG mode) or wait forever (traditional USB Host mode.) Arriving in this state to quickly
        * * from wait vrise can cause a system hang. */
        {       /* Vbus error? */
         42,    /* .test */
         host_wait_port_connect,        /* .state */
         host_vbus_err, /* .target */
         VBUS_VLD_,     /* .test1 */
         },
        {       /* */
         43,    /* .test */
         host_wait_port_connect,        /* .state */
         host_wait_dischrg,     /* .target */
         ID_GND_ | enable_otg_ | a_bus_drop,    /* .test1 */
         },
        {       /* */
         44,    /* .test */
         host_wait_port_connect,        /* .state */
         host_port_connected,   /* .target */
         HUB_PORT_CONNECT,      /* .test1 */
         },
        /* ! A-Device host, the host controller driver has noticed a port status change, reset the *
        * bus and proceed. */
        {       /* */
         45,    /* .test */
         host_port_connected,   /* .state */
         host_wait_dischrg,     /* .target */
         ID_GND_ | enable_otg_ | a_bus_drop | HUB_PORT_CONNECT_ | Tst_two_second,       /* .test1 */
         },
        {       /* */
         46,    /* .test */
         host_port_connected,   /* .state */
         host_bus_reset,        /* .target */
         BUS_RESET,     /* .test1 */
         },
        {       /* */
         47,    /* .test */
         host_port_connected,   /* .state */
         host_addressed,        /* .target */
         ADDRESSED,     /* .test1 */
         },
        /* ! A-Device host, the bus has been reset, attempt to address the device. */
        {       /* */
         48,    /* .test */
         host_bus_reset,        /* .state */
         host_wait_dischrg,     /* .target */
         ID_GND_ | enable_otg_ | a_bus_drop,    /* .test1 */
         },
        {       /* */
         49,    /* .test */
         host_bus_reset,        /* .state */
         host_addressed,        /* .target */
         ADDRESSED,     /* .test1 */
         },
        /* ! A-Device host, the device has been addressed, attempt to enumerate, find the appropriate 
        * class driver and configure. */
        {       /* Lost B-Connect or user changed his mind? */
         50,    /* .test */
         host_addressed,        /* .state */
         host_wait_dischrg,     /* .target */
         ID_GND_ | enable_otg_ | a_bus_drop | HUB_PORT_CONNECT_,        /* .test1 */
         },
        {       /* Device configured, class driver loaded. */
         51,    /* .test */
         host_addressed,        /* .state */
         host_configured,       /* .target */
         CONFIGURED,    /* .test1 */
         },
        /* ! A-Device host, the enumerated device is supported, and has been configured.  N.B. *
        * Currently assuming HNP enable feature is automatically performed.  N.B. Currently forcing
        * * HNP_CAPABLE and HNP_ENABLED. */
        {       /* Vbus error? */
         52,    /* .test */
         host_configured,       /* .state */
         host_vbus_err, /* .target */
         VBUS_VLD_,     /* .test1 */
         },
        {       /* */
         53,    /* .test */
         host_configured,       /* .state */
         host_wait_dischrg,     /* .target */
         HUB_PORT_CONNECT_,     /* .test1 */
         },
        {       /* */
         54,    /* .test */
         host_configured,       /* .state */
         host_wait_dischrg,     /* .target */
         ID_GND_ | enable_otg_ | a_bus_drop,    /* .test1 */
         },
        /* ! A-Device Vbus error. The user must be informed and allowed to clear the problem. */
        {       /* */
         55,    /* .test */
         host_vbus_err, /* .state */
         host_wait_dischrg,     /* .target */
         ID_GND_ | clr_err_cmd | a_bus_drop,    /* .test1 */
         },
        /* ! */
        {       /* Normal finish, wait for a_sess_vld/ */
         56,    /* .test */
         host_wait_dischrg,     /* .state */
         host_wait_vfall,       /* .target */
         AUTO | AUTO_,  /* .test1 */
         },
        /* ! A-Device wait for Vbus to fall.  Currently reseting a_bus_req on entry, require *
        * explicit a_bus_req to proceed. */
        {       /* Normal finish, wait for a_sess_vld/ */
         57,    /* .test */
         host_wait_vfall,       /* .state */
         host_idle,     /* .target */
         A_SESS_VLD_ | Tst_one_second,  /* .test1 */
         },
        /* ! This is not an OTG State. It is used internally to mark the end of the list of states *
        * and inputs. */
        {       /* */
         58,    /* .test */
         terminator_state,      /* .state */
         invalid_state, /* .target */
         0,     /* .test1 */
         },
        {59, invalid_state,},

};

/*==================================================================================================
                                        LOCAL FUNCTIONS
==================================================================================================*/

/*================================================================================================*/
/*===== VT_usb_otg_test_cleanup =====*/
/** 
@brief  This function assumes the post-condition of the test case execution

@param  None.

@return None.
*/
/*================================================================================================*/
int VT_usb_otg_test_cleanup(void)
{
        if (fd != 0)
                close(fd);
        return TPASS;
}

/*================================================================================================*/
/*===== VT_usb_otg_test_setup =====*/
/**
@brief  This function assumes the pre-condition of the test case execution

@param  None.

@return On success - return TPASS.
        On failure - return the error code.
*/
/*================================================================================================*/
int VT_usb_otg_test_setup(void)
{
        fd = open("/proc/" USB_DEV, O_RDWR);

        if (fd < 0)
        {
                tst_resm(TFAIL, "setup() Failed open device");
                return TFAIL;
        }
        return TPASS;
}

/*================================================================================================*/
/*===== set_state =====*/
/**
@brief  This function sets a state of State Machine.

@param  num
        number of state.

@return On success - return TPASS.
        On failure - return the error code.
*/
/*================================================================================================*/
int set_state(int num)
{
        struct otg_state state1;

        rv = TPASS;

        memset(&firmware_info, 0x00, sizeof(struct otg_firmware_info));
        firmware_info.number_of_states = 36;
        firmware_info.number_of_tests = 60;
        strncpy(firmware_info.fw_name, info_string, strlen(info_string));
        ioctl(fd, OTGADMIN_SET_INFO, &firmware_info);

        tst_resm(TINFO, "=== set state(%d) ===", num);
        memcpy(&state, &otg_states_mn[num], sizeof(struct otg_state));
        if (ioctl(fd, OTGADMIN_SET_STATE, &state) < 0)
        {
                tst_resm(TFAIL, "set state ioctl - failed! errno: %d, error string: %s\n", errno,
                         strerror(errno));
                rv = TFAIL;
        }
        tst_resm(TINFO, "set state ioctl - [ ok ]");

        if (vflag)
        {
                tst_resm(TINFO, "state number: %d", state.state);
                tst_resm(TINFO, "meta: %s", otg_meta_states[state.meta]);
                tst_resm(TINFO, "state name: %s", state.name);
                tst_resm(TINFO, "state timeout: %d", state.tmout);
                tst_resm(TINFO, "state reset: %d", state.reset);
                tst_resm(TINFO, "state outputs: %d", state.outputs);
        }
        else
        {
                tst_resm(TINFO, "state name: %s", state.name);
        }

        memset(&firmware_info, 0x00, sizeof(struct otg_firmware_info));
        firmware_info.number_of_states = 36;
        firmware_info.number_of_tests = 60;
        strncpy(firmware_info.fw_name, info_string, strlen(info_string));
        ioctl(fd, OTGADMIN_SET_INFO, &firmware_info);

        printf("\n");
        tst_resm(TINFO, "=== get state ===");
        memset(&state1, 0x00, sizeof(struct otg_state));
        state1.state = state.state;
        if (ioctl(fd, OTGADMIN_GET_STATE, &state1) < 0)
        {
                tst_resm(TFAIL, "get state ioctl - failed! errno: %d, error string: %s\n", errno,
                         strerror(errno));
                rv = TFAIL;
        }
        tst_resm(TINFO, "get state ioctl - [ ok ]");

        if (vflag)
        {
                tst_resm(TINFO, "state number: %d", state1.state);
                tst_resm(TINFO, "meta: %s", otg_meta_states[state1.meta]);
                tst_resm(TINFO, "state name: %s", state1.name);
                tst_resm(TINFO, "state timeout: %d", state1.tmout);
                tst_resm(TINFO, "state reset: %d", state1.reset);
                tst_resm(TINFO, "state outputs: %d\n", state1.outputs);
        }
        else
        {
                tst_resm(TINFO, "state name: %s\n", state1.name);
        }

        if ((state1.state != state.state) || (state1.meta != state.meta)
            || (strcmp(state1.name, state.name)) || (state1.tmout != state.tmout)
            || (state1.reset != state.reset) || (state1.outputs != state.outputs))
        {
                tst_resm(TFAIL, "setted and getted states are not equal!\n");
                rv = TFAIL;
        }

        return rv;
}

/*================================================================================================*/
/*===== set_test =====*/
/**
@brief  This function tests transition from state to state.

@param  num
        number of start state.

@return On success - return TPASS.
        On failure - return the error code.
*/
/*================================================================================================*/
int set_test(int num)
{
        struct otg_test test1;
        memset(&test1, 0x0, sizeof(struct otg_test));
        rv = TPASS;

        memset(&firmware_info, 0x00, sizeof(struct otg_firmware_info));
        firmware_info.number_of_states = 36;
        firmware_info.number_of_tests = 61;
        strncpy(firmware_info.fw_name, info_string, strlen(info_string));
        ioctl(fd, OTGADMIN_SET_INFO, &firmware_info);

        tst_resm(TINFO, "=== set test(%d) ===", num);
        memcpy(&test, &otg_tests_mn[num], sizeof(struct otg_test));
        if (ioctl(fd, OTGADMIN_SET_TEST, &test) < 0)
        {
                tst_resm(TFAIL, "set test ioctl - failed! errno: %d, error string: %s\n", errno,
                         strerror(errno));
                rv = TFAIL;
        }
        tst_resm(TINFO, "set test ioctl - [ ok ]");
        if (vflag)
        {
                tst_resm(TINFO, "test: %d", test.test);
                tst_resm(TINFO, "state: %d", test.state);
                tst_resm(TINFO, "target: %s", otg_strings[test.target]);
                tst_resm(TINFO, "test1: %d", test.test1);
                tst_resm(TINFO, "test2: %d", test.test2);
                tst_resm(TINFO, "test3: %d", test.test3);
        }
        else
        {
                tst_resm(TINFO, "state: %s", otg_strings[test.state]);
                tst_resm(TINFO, "target: %s", otg_strings[test.target]);
        }

        memset(&firmware_info, 0x00, sizeof(struct otg_firmware_info));
        firmware_info.number_of_states = 36;
        firmware_info.number_of_tests = 61;
        strncpy(firmware_info.fw_name, info_string, strlen(info_string));
        ioctl(fd, OTGADMIN_SET_INFO, &firmware_info);

        printf("\n");
        tst_resm(TINFO, "=== get test ===");
        memset(&test1, 0x00, sizeof(struct otg_test));
        test1.test = test.test;
        if (ioctl(fd, OTGADMIN_GET_TEST, &test1) < 0)
        {
                tst_resm(TFAIL, "get test ioctl - failed! errno: %d, error string: %s\n", errno,
                         strerror(errno));
                rv = TFAIL;
        }
        if ((test1.target != test.target) || (test1.test != test.test)
            || (test1.state != test.state) || (test1.test1 != test.test1)
            || (test1.test2 != test.test2) || (test1.test3 != test1.test3))
        {
                tst_resm(TFAIL, "setted and getted tests are not equal!\n");
                rv = TFAIL;
        }
        tst_resm(TINFO, "get test ioctl - [ ok ]");
        if (vflag)
        {
                tst_resm(TINFO, "test: %d", test1.test);
                tst_resm(TINFO, "state: %d", test1.state);
                tst_resm(TINFO, "target: %s", otg_strings[test1.target]);
                tst_resm(TINFO, "test1: %d", test1.test1);
                tst_resm(TINFO, "test2: %d", test1.test2);
                tst_resm(TINFO, "test3: %d\n", test1.test3);
        }
        else
        {
                tst_resm(TINFO, "state: %s", otg_strings[test1.state]);
                tst_resm(TINFO, "target: %s\n", otg_strings[test1.target]);
        }

        return rv;
}

/*================================================================================================*/
/*===== VT_usb_otg_test =====*/
/**
@brief  USB-OTG test scenario.

@param  switch_fct
        Number of test case.

@return On success - return TPASS.
        On failure - return the error code.
*/
/*================================================================================================*/
int VT_usb_otg_test(int switch_fct)
{
        struct otg_admin_command command1;
        struct otg_firmware_info firmware_info1;

        rv = TPASS;

        switch (switch_fct)
        {
        case 0:
                printf("====================================================================\n");
                tst_resm(TINFO, "test the bus...");
                printf("====================================================================\n");
                /* enable OTG */
                if (ioctl(fd, OTGADMIN_ENABLE_OTG, 1) < 0)
                {
                        tst_resm(TFAIL, "enable OTG(true) - fail!");
                        rv = TFAIL;
                }
                tst_resm(TINFO, "enable OTG(true) - [ ok ]");

                tst_resm(TINFO, "=== get status ===");
                memset(&status_update, 0x00, sizeof(struct otg_status_update));
                if (ioctl(fd, OTGADMIN_STATUS, &status_update) < 0)
                {
                        tst_resm(TFAIL, "get status ioctl - failed! errno: %d, error string: %s\n",
                                 errno, strerror(errno));
                        rv = TFAIL;
                }
                tst_resm(TINFO, "state number: %d", status_update.state);
                tst_resm(TINFO, "state name: %s\n", status_update.state_name);
                if (status_update.state != 10)
                {
                        tst_resm(TFAIL, "state is wrong! Expected state: otg_enabled\n");
                        rv = TFAIL;
                }

                /* bus request */
                if (ioctl(fd, OTGADMIN_BUS_REQ, 1) < 0)
                {
                        tst_resm(TFAIL, "bus request(true) - fail!");
                        rv = TFAIL;
                }
                tst_resm(TINFO, "bus request(true) - [ ok ]");

                tst_resm(TINFO, "=== get status ===");
                memset(&status_update, 0x00, sizeof(struct otg_status_update));
                if (ioctl(fd, OTGADMIN_STATUS, &status_update) < 0)
                {
                        tst_resm(TFAIL, "get status ioctl - failed! errno: %d, error string: %s\n",
                                 errno, strerror(errno));
                        rv = TFAIL;
                }
                tst_resm(TINFO, "state number: %d", status_update.state);
                tst_resm(TINFO, "state name: %s\n", status_update.state_name);
                if ((status_update.state != 21) || (status_update.state != 11))
                {
                        tst_resm(TFAIL,
                                 "state is wrong! Expected state: host_idle or peripheral_idle\n");
                        rv = TFAIL;
                }

                /* A_device requests the bus */
                if (ioctl(fd, OTGADMIN_A_BUS_REQ, 1) < 0)
                {
                        tst_resm(TFAIL, "a_device bus request(true) - fail!");
                        rv = TFAIL;
                }
                tst_resm(TINFO, "a_device bus request(true) - [ ok ]");

                tst_resm(TINFO, "=== get status ===");
                memset(&status_update, 0x00, sizeof(struct otg_status_update));
                if (ioctl(fd, OTGADMIN_STATUS, &status_update) < 0)
                {
                        tst_resm(TFAIL, "get status ioctl - failed! errno: %d, error string: %s\n",
                                 errno, strerror(errno));
                        rv = TFAIL;
                }
                tst_resm(TINFO, "state number: %d", status_update.state);
                tst_resm(TINFO, "state name: %s\n", status_update.state_name);
                if (status_update.state != 21)
                {
                        tst_resm(TFAIL, "state is wrong! Expected state: host_idle\n");
                        rv = TFAIL;
                }

                /* A_device drops the bus */
                if (ioctl(fd, OTGADMIN_A_BUS_DROP, 1) < 0)
                {
                        tst_resm(TFAIL, "a_device bus drop(true) - fail!");
                        rv = TFAIL;
                }
                tst_resm(TINFO, "a_device bus drop(true) - [ ok ]");

                tst_resm(TINFO, "=== get status ===");
                memset(&status_update, 0x00, sizeof(struct otg_status_update));
                if (ioctl(fd, OTGADMIN_STATUS, &status_update) < 0)
                {
                        tst_resm(TFAIL, "get status ioctl - failed! errno: %d, error string: %s\n",
                                 errno, strerror(errno));
                        rv = TFAIL;
                }
                tst_resm(TINFO, "state number: %d", status_update.state);
                tst_resm(TINFO, "state name: %s\n", status_update.state_name);
                if (status_update.state != 22)
                {
                        tst_resm(TFAIL, "state is wrong! Expected state: host_idle_dropped\n");
                        rv = TFAIL;
                }

                /* B_device attempts to use HNP */
                if (ioctl(fd, OTGADMIN_B_HNP_CMD, 1) < 0)
                {
                        tst_resm(TFAIL, "b_device HNP(true) - fail!");
                        rv = TFAIL;
                }
                tst_resm(TINFO, "b_device HNP(true) - [ ok ]");

                tst_resm(TINFO, "=== get status ===");
                memset(&status_update, 0x00, sizeof(struct otg_status_update));
                if (ioctl(fd, OTGADMIN_STATUS, &status_update) < 0)
                {
                        tst_resm(TFAIL, "get status ioctl - failed! errno: %d, error string: %s\n",
                                 errno, strerror(errno));
                        rv = TFAIL;
                }
                tst_resm(TINFO, "state number: %d", status_update.state);
                tst_resm(TINFO, "state name: %s\n", status_update.state_name);
                if (status_update.state != 16)
                {
                        tst_resm(TFAIL, "state is wrong! Expected state: peripheral_configured\n");
                        rv = TFAIL;
                }

                /* B_device requests the bus */
                if (ioctl(fd, OTGADMIN_B_BUS_REQ, 1) < 0)
                {
                        tst_resm(TFAIL, "b_device bus request(true) - fail!");
                        rv = TFAIL;
                }
                tst_resm(TINFO, "a_device bus request(true) - [ ok ]");

                tst_resm(TINFO, "=== get status ===");
                memset(&status_update, 0x00, sizeof(struct otg_status_update));
                if (ioctl(fd, OTGADMIN_STATUS, &status_update) < 0)
                {
                        tst_resm(TFAIL, "get status ioctl - failed! errno: %d, error string: %s\n",
                                 errno, strerror(errno));
                        rv = TFAIL;
                }
                tst_resm(TINFO, "state number: %d", status_update.state);
                tst_resm(TINFO, "state name: %s\n", status_update.state_name);
                if (status_update.state != 11)
                {
                        tst_resm(TFAIL, "state is wrong! Expected state: peripheral_idle\n");
                        rv = TFAIL;
                }

                /* A_device suspends the bus */
                if (ioctl(fd, OTGADMIN_A_SUSPEND_REQ, 1) < 0)
                {
                        tst_resm(TFAIL, "a_device suspend the bus request(true) - fail!");
                        rv = TFAIL;
                }
                tst_resm(TINFO, "a_device suspend the bus request(true) - [ ok ]");

                tst_resm(TINFO, "=== get status ===");
                memset(&status_update, 0x00, sizeof(struct otg_status_update));
                if (ioctl(fd, OTGADMIN_STATUS, &status_update) < 0)
                {
                        tst_resm(TFAIL, "get status ioctl - failed! errno: %d, error string: %s\n",
                                 errno, strerror(errno));
                        rv = TFAIL;
                }
                tst_resm(TINFO, "state number: %d", status_update.state);
                tst_resm(TINFO, "state name: %s\n", status_update.state_name);
                if (status_update.state != 11)
                {
                        tst_resm(TFAIL, "state is wrong! Expected state: peripheral_idle\n");
                        rv = TFAIL;
                }

                /* B-device requests to perform SRP */
                if (ioctl(fd, OTGADMIN_B_SESS_REQ, 1) < 0)
                {
                        tst_resm(TFAIL, "b_device session request(true) - fail!");
                        rv = TFAIL;
                }
                tst_resm(TINFO, "b_device session request(true) - [ ok ]");

                tst_resm(TINFO, "=== get status ===");
                memset(&status_update, 0x00, sizeof(struct otg_status_update));
                if (ioctl(fd, OTGADMIN_STATUS, &status_update) < 0)
                {
                        tst_resm(TFAIL, "get status ioctl - failed! errno: %d, error string: %s\n",
                                 errno, strerror(errno));
                        rv = TFAIL;
                }
                tst_resm(TINFO, "state number: %d", status_update.state);
                tst_resm(TINFO, "state name: %s\n", status_update.state_name);
                if (status_update.state != 15)
                {
                        tst_resm(TFAIL, "state is wrong! Expected state: peripheral_addressed\n");
                        rv = TFAIL;
                }

                /* B_device suspends the bus */
                if (ioctl(fd, OTGADMIN_B_SUSPEND_REQ, 1) < 0)
                {
                        tst_resm(TFAIL, "b_device suspend the bus request(true) - fail!");
                        rv = TFAIL;
                }
                tst_resm(TINFO, "b_device suspend the bus request(true) - [ ok ]");

                tst_resm(TINFO, "=== get status ===");
                memset(&status_update, 0x00, sizeof(struct otg_status_update));
                if (ioctl(fd, OTGADMIN_STATUS, &status_update) < 0)
                {
                        tst_resm(TFAIL, "get status ioctl - failed! errno: %d, error string: %s\n",
                                 errno, strerror(errno));
                        rv = TFAIL;
                }
                tst_resm(TINFO, "state number: %d", status_update.state);
                tst_resm(TINFO, "state name: %s\n", status_update.state_name);
                if (status_update.state != 21)
                {
                        tst_resm(TFAIL, "state is wrong! Expected state: host_idle\n");
                        rv = TFAIL;
                }

                /* A_device requests the bus */
                if (ioctl(fd, OTGADMIN_A_BUS_REQ, 1) < 0)
                {
                        tst_resm(TFAIL, "a_device bus request(true) - fail!");
                        rv = TFAIL;
                }
                tst_resm(TINFO, "a_device bus request(true) - [ ok ]");

                tst_resm(TINFO, "=== get status ===");
                memset(&status_update, 0x00, sizeof(struct otg_status_update));
                if (ioctl(fd, OTGADMIN_STATUS, &status_update) < 0)
                {
                        tst_resm(TFAIL, "get status ioctl - failed! errno: %d, error string: %s\n",
                                 errno, strerror(errno));
                        rv = TFAIL;
                }
                tst_resm(TINFO, "state number: %d", status_update.state);
                tst_resm(TINFO, "state name: %s\n", status_update.state_name);
                if (status_update.state != 21)
                {
                        tst_resm(TFAIL, "state is wrong! Expected state: host_idle\n");
                        rv = TFAIL;
                }

                /* A-Device will send Remote Wakeup Enable Request */
                if (ioctl(fd, OTGADMIN_SET_REMOTE_WAKEUP_CMD, 1) < 0)
                {
                        tst_resm(TFAIL, "set remote wakeup(true) - fail!");
                        rv = TFAIL;
                }
                tst_resm(TINFO, "set remote wakeup(true) - [ ok ]");

                tst_resm(TINFO, "=== get status ===");
                memset(&status_update, 0x00, sizeof(struct otg_status_update));
                if (ioctl(fd, OTGADMIN_STATUS, &status_update) < 0)
                {
                        tst_resm(TFAIL, "get status ioctl - failed! errno: %d, error string: %s\n",
                                 errno, strerror(errno));
                        rv = TFAIL;
                }
                tst_resm(TINFO, "state number: %d", status_update.state);
                tst_resm(TINFO, "state name: %s\n", status_update.state_name);
                if (status_update.state != 19)
                {
                        tst_resm(TFAIL,
                                 "state is wrong! Expected state: peripheral_wakeup_enabled\n");
                        rv = TFAIL;
                }

                /* B-Device will perform Remote Wakeup */
                if (ioctl(fd, OTGADMIN_REMOTE_WAKEUP_CMD, 1) < 0)
                {
                        tst_resm(TFAIL, "remote wakeup command(true) - fail!");
                        rv = TFAIL;
                }
                tst_resm(TINFO, "remote wakeup command(true) - [ ok ]");

                tst_resm(TINFO, "=== get status ===");
                memset(&status_update, 0x00, sizeof(struct otg_status_update));
                if (ioctl(fd, OTGADMIN_STATUS, &status_update) < 0)
                {
                        tst_resm(TFAIL, "get status ioctl - failed! errno: %d, error string: %s\n",
                                 errno, strerror(errno));
                        rv = TFAIL;
                }
                tst_resm(TINFO, "state number: %d", status_update.state);
                tst_resm(TINFO, "state name: %s\n", status_update.state_name);
                if (status_update.state != 20)
                {
                        tst_resm(TFAIL, "state is wrong! Expected state: peripheral_wakeup\n");
                        rv = TFAIL;
                }

                /* A-Device will send Remote Wakeup Disable Request */
                if (ioctl(fd, OTGADMIN_RESET_REMOTE_WAKEUP_CMD, 1) < 0)
                {
                        tst_resm(TFAIL, "reset remote wakeup(true) - fail!");
                        rv = TFAIL;
                }
                tst_resm(TINFO, "reset remote wakeup(true) - [ ok ]");

                tst_resm(TINFO, "=== get status ===");
                memset(&status_update, 0x00, sizeof(struct otg_status_update));
                if (ioctl(fd, OTGADMIN_STATUS, &status_update) < 0)
                {
                        tst_resm(TFAIL, "get status ioctl - failed! errno: %d, error string: %s\n",
                                 errno, strerror(errno));
                        rv = TFAIL;
                }
                tst_resm(TINFO, "state number: %d", status_update.state);
                tst_resm(TINFO, "state name: %s\n", status_update.state_name);
                if (status_update.state != 21)
                {
                        tst_resm(TFAIL, "state is wrong! Expected state: host_idle\n");
                        rv = TFAIL;
                }

                /* A-Device will clears Vbus overcurrent error */
                if (ioctl(fd, OTGADMIN_CLR_ERR_CMD, 1) < 0)
                {
                        tst_resm(TFAIL, "a_device will clear Vbus overcurrent error(true) - fail!");
                        rv = TFAIL;
                }
                tst_resm(TINFO, "a_device will clear Vbus overcurrent error(true) - [ ok ]");

                tst_resm(TINFO, "=== get status ===");
                memset(&status_update, 0x00, sizeof(struct otg_status_update));
                if (ioctl(fd, OTGADMIN_STATUS, &status_update) < 0)
                {
                        tst_resm(TFAIL, "get status ioctl - failed! errno: %d, error string: %s\n",
                                 errno, strerror(errno));
                        rv = TFAIL;
                }
                tst_resm(TINFO, "state number: %d", status_update.state);
                tst_resm(TINFO, "state name: %s\n", status_update.state_name);
                if (status_update.state != 24)
                {
                        tst_resm(TFAIL,
                                 "state is wrong! Expected state: host_wait_vrise_overcurrent\n");
                        rv = TFAIL;
                }

                /* Application on Device requests bus be suspended */
                if (ioctl(fd, OTGADMIN_SUSPEND_REQ, 1) < 0)
                {
                        tst_resm(TFAIL, "bus suspend request(true) - fail!");
                        rv = TFAIL;
                }
                tst_resm(TINFO, "bus suspend request(true) - [ ok ]");

                tst_resm(TINFO, "=== get status ===");
                memset(&status_update, 0x00, sizeof(struct otg_status_update));
                if (ioctl(fd, OTGADMIN_STATUS, &status_update) < 0)
                {
                        tst_resm(TFAIL, "get status ioctl - failed! errno: %d, error string: %s\n",
                                 errno, strerror(errno));
                        rv = TFAIL;
                }
                tst_resm(TINFO, "state number: %d", status_update.state);
                tst_resm(TINFO, "state name: %s\n", status_update.state_name);
                if (status_update.state != 22)
                {
                        tst_resm(TFAIL, "state is wrong! Expected state: host_idle_dropped\n");
                        rv = TFAIL;
                }

                /* bus request */
                if (ioctl(fd, OTGADMIN_BUS_REQ, 1) < 0)
                {
                        tst_resm(TFAIL, "bus request(true) - fail!");
                        rv = TFAIL;
                }
                tst_resm(TINFO, "bus request(true) - [ ok ]");

                tst_resm(TINFO, "=== get status ===");
                memset(&status_update, 0x00, sizeof(struct otg_status_update));
                if (ioctl(fd, OTGADMIN_STATUS, &status_update) < 0)
                {
                        tst_resm(TFAIL, "get status ioctl - failed! errno: %d, error string: %s\n",
                                 errno, strerror(errno));
                        rv = TFAIL;
                }
                tst_resm(TINFO, "state number: %d", status_update.state);
                tst_resm(TINFO, "state name: %s\n", status_update.state_name);
                if ((status_update.state != 21) || (status_update.state != 11))
                {
                        tst_resm(TFAIL,
                                 "state is wrong! Expected state: host_idle or peripheral_idle\n");
                        rv = TFAIL;
                }

                /* A_device requests the bus */
                if (ioctl(fd, OTGADMIN_A_BUS_REQ, 1) < 0)
                {
                        tst_resm(TFAIL, "a_device bus request(true) - fail!");
                        rv = TFAIL;
                }
                tst_resm(TINFO, "a_device bus request(true) - [ ok ]");

                tst_resm(TINFO, "=== get status ===");
                memset(&status_update, 0x00, sizeof(struct otg_status_update));
                if (ioctl(fd, OTGADMIN_STATUS, &status_update) < 0)
                {
                        tst_resm(TFAIL, "get status ioctl - failed! errno: %d, error string: %s\n",
                                 errno, strerror(errno));
                        rv = TFAIL;
                }
                tst_resm(TINFO, "state number: %d", status_update.state);
                tst_resm(TINFO, "state name: %s\n", status_update.state_name);
                if (status_update.state != 21)
                {
                        tst_resm(TFAIL, "state is wrong! Expected state: host_idle\n");
                        rv = TFAIL;
                }

                /* Application on A-host wants external charge pump enabled */
                if (ioctl(fd, OTGADMIN_A_HPWR_REQ, 1) < 0)
                {
                        tst_resm(TFAIL, "external charge pump enable request(true) - fail!");
                        rv = TFAIL;
                }
                tst_resm(TINFO, "external charge pump enable request(true) - [ ok ]");

                tst_resm(TINFO, "=== get status ===");
                memset(&status_update, 0x00, sizeof(struct otg_status_update));
                if (ioctl(fd, OTGADMIN_STATUS, &status_update) < 0)
                {
                        tst_resm(TFAIL, "get status ioctl - failed! errno: %d, error string: %s\n",
                                 errno, strerror(errno));
                        rv = TFAIL;
                }
                tst_resm(TINFO, "state number: %d", status_update.state);
                tst_resm(TINFO, "state name: %s\n", status_update.state_name);
                if (status_update.state != 23)
                {
                        tst_resm(TFAIL, "state is wrong! Expected state: host_wait_vrise");
                        rv = TFAIL;
                }

                /* request to drop the bus */
                if (ioctl(fd, OTGADMIN_BUS_DROP, 1) < 0)
                {
                        tst_resm(TFAIL, "external charge pump enable request(true) - fail!");
                        rv = TFAIL;
                }
                tst_resm(TINFO, "external charge pump enable request(true) - [ ok ]");

                tst_resm(TINFO, "=== get status ===");
                memset(&status_update, 0x00, sizeof(struct otg_status_update));
                if (ioctl(fd, OTGADMIN_STATUS, &status_update) < 0)
                {
                        tst_resm(TFAIL, "get status ioctl - failed! errno: %d, error string: %s\n",
                                 errno, strerror(errno));
                        rv = TFAIL;
                }
                tst_resm(TINFO, "state number: %d", status_update.state);
                tst_resm(TINFO, "state name: %s\n", status_update.state_name);
                if (status_update.state != 1)
                {
                        tst_resm(TFAIL, "state is wrong! Expected state: otg_disabled\n");
                        rv = TFAIL;
                }

                break;

        case 1:
                printf("====================================================================\n");
                tst_resm(TINFO, "set/get various features...");
                printf("====================================================================\n");
                tst_resm(TINFO, "=== get status ===");
                memset(&status_update, 0x00, sizeof(struct otg_status_update));
                if (ioctl(fd, OTGADMIN_STATUS, &status_update) < 0)
                {
                        tst_resm(TFAIL, "get status ioctl - failed! errno: %d, error string: %s\n",
                                 errno, strerror(errno));
                        rv = TFAIL;
                }
                tst_resm(TINFO, "get status ioctl - [ ok ]\n");

                if (vflag)
                {
                        tst_resm(TINFO, "state number: %d", status_update.state);
                        tst_resm(TINFO, "meta number: %d", status_update.meta);
                        tst_resm(TINFO, "inputs number: %d", status_update.inputs);
                        tst_resm(TINFO, "outputs number: %d", status_update.outputs);
                        tst_resm(TINFO, "capabilities number: %d", status_update.capabilities);
                        tst_resm(TINFO, "firmware name: %s", status_update.fw_name);
                        tst_resm(TINFO, "state name: %s", status_update.state_name);
                        tst_resm(TINFO, "meta name: %s", status_update.meta_name);
                        tst_resm(TINFO, "function name: %s\n", status_update.function_name);
                }

                printf("====================================================================\n");
                tst_resm(TINFO, "=== set serial ===");
                memset(&command, 0x00, sizeof(struct otg_admin_command));
                command.n = 8;
                strcpy(command.string, "2278023");
                if (ioctl(fd, OTGADMIN_SET_SERIAL, &command) < 0)
                {
                        tst_resm(TFAIL, "set serial ioctl - failed! errno: %d, error string: %s\n",
                                 errno, strerror(errno));
                        rv = TFAIL;
                }
                tst_resm(TINFO, "set serial ioctl - [ ok ]\n");

                if (vflag)
                {
                        tst_resm(TINFO, "ioctl number: %d", command.n);
                        tst_resm(TINFO, "serial number: %s\n", command.string);
                }

                tst_resm(TINFO, "=== get serial ===");
                memset(&command1, 0x00, sizeof(struct otg_admin_command));
                if (ioctl(fd, OTGADMIN_GET_SERIAL, &command1) < 0)
                {
                        tst_resm(TFAIL, "get serial ioctl - failed! errno: %d, error string: %s\n",
                                 errno, strerror(errno));
                        rv = TFAIL;
                }
                tst_resm(TINFO, "get serial ioctl - [ ok ]\n");

                if (vflag)
                {
                        tst_resm(TINFO, "ioctl number: %d", command1.n);
                        tst_resm(TINFO, "serial number: %s\n", command1.string);
                }

                if ((command1.n != command.n) || (strcmp(command1.string, command.string)))
                {
                        tst_resm(TFAIL, "serial numbers are not equal!\n");
                        rv = TFAIL;
                }

                printf("====================================================================\n");
                tst_resm(TINFO, "=== set function ===");
                memset(&command, 0x00, sizeof(struct otg_admin_command));
                command.n = 0;
                strcpy(command.string, "mass");
                if (ioctl(fd, OTGADMIN_SET_FUNCTION, &command) < 0)
                {
                        tst_resm(TFAIL,
                                 "set function ioctl - failed! errno: %d, error string: %s\n",
                                 errno, strerror(errno));
                        rv = TFAIL;
                }
                tst_resm(TINFO, "set function ioctl - [ ok ]\n");

                tst_resm(TINFO, "=== get status ===");
                memset(&status_update, 0x00, sizeof(struct otg_status_update));
                if (ioctl(fd, OTGADMIN_STATUS, &status_update) < 0)
                {
                        tst_resm(TFAIL, "get status ioctl - failed! errno: %d, error string: %s\n",
                                 errno, strerror(errno));
                        rv = TFAIL;
                }
                tst_resm(TINFO, "function name: %s\n", status_update.function_name);

                tst_resm(TINFO, "=== set function ===");
                memset(&command, 0x00, sizeof(struct otg_admin_command));
                command.n = 0;
                strcpy(command.string, "hid");
                if (ioctl(fd, OTGADMIN_SET_FUNCTION, &command) < 0)
                {
                        tst_resm(TFAIL,
                                 "set function ioctl - failed! errno: %d, error string: %s\n",
                                 errno, strerror(errno));
                        rv = TFAIL;
                }
                tst_resm(TINFO, "set function ioctl - [ ok ]\n");

                if (vflag)
                {
                        tst_resm(TINFO, "ioctl number: %d", command.n);
                        tst_resm(TINFO, "function name: %s\n", command.string);
                }

                tst_resm(TINFO, "=== get status ===");
                memset(&status_update, 0x00, sizeof(struct otg_status_update));
                if (ioctl(fd, OTGADMIN_STATUS, &status_update) < 0)
                {
                        tst_resm(TFAIL, "get status ioctl - failed! errno: %d, error string: %s\n",
                                 errno, strerror(errno));
                        rv = TFAIL;
                }
                tst_resm(TINFO, "function name: %s\n", status_update.function_name);

                if (strcmp(status_update.function_name, command.string))
                {
                        tst_resm(TFAIL, "function was not setted!\n");
                        rv = TFAIL;
                }

                tst_resm(TINFO, "=== get function ===");
                for (i = 0; i < 8; i++)
                {
                        memset(&command1, 0x00, sizeof(struct otg_admin_command));
                        command1.n = i;
                        if (ioctl(fd, OTGADMIN_GET_FUNCTION, &command1) < 0)
                        {
                                tst_resm(TFAIL,
                                         "get function ioctl - failed! errno: %d, error string: %s\n",
                                         errno, strerror(errno));
                                rv = TFAIL;
                        }
                        if (!strcmp(command1.string, ""))
                        {
                                memset(&command1, 0x00, sizeof(struct otg_admin_command));
                                command1.n = i - 1;
                                if (ioctl(fd, OTGADMIN_GET_FUNCTION, &command1) < 0)
                                {
                                        tst_resm(TFAIL,
                                                 "get function ioctl - failed! errno: %d, error string: %s\n",
                                                 errno, strerror(errno));
                                        rv = TFAIL;
                                }
                                break;
                        }
                }
                tst_resm(TINFO, "get function ioctl - [ ok ]\n");

                if (vflag)
                {
                        tst_resm(TINFO, "ioctl number: %d", command1.n);
                        tst_resm(TINFO, "function name: %s\n", command1.string);
                }

                printf("====================================================================\n");
                tst_resm(TINFO, "=== set info ===");
                memset(&firmware_info, 0x00, sizeof(struct otg_firmware_info));
                firmware_info.number_of_states = 3;
                firmware_info.number_of_tests = 6;
                strncpy(firmware_info.fw_name, info_string, strlen(info_string));
                if (ioctl(fd, OTGADMIN_SET_INFO, &firmware_info) < 0)
                {
                        tst_resm(TFAIL, "set info ioctl - failed! errno: %d, error string: %s\n",
                                 errno, strerror(errno));
                        rv = TFAIL;
                }
                tst_resm(TINFO, "set info ioctl - [ ok ]\n");

                if (vflag)
                {
                        tst_resm(TINFO, "number of states: %d", firmware_info.number_of_states);
                        tst_resm(TINFO, "number of tests: %d", firmware_info.number_of_tests);
                        tst_resm(TINFO, "firmware name: %s\n", firmware_info.fw_name);
                }

                ioctl(fd, OTGADMIN_SET_INFO, &firmware_info);

                tst_resm(TINFO, "=== get info ===");
                memset(&firmware_info1, 0x00, sizeof(struct otg_firmware_info));
                if (ioctl(fd, OTGADMIN_GET_INFO, &firmware_info1) < 0)
                {
                        tst_resm(TFAIL, "get info ioctl - failed! errno: %d, error string: %s\n",
                                 errno, strerror(errno));
                        rv = TFAIL;
                }
                tst_resm(TINFO, "get info ioctl - [ ok ]\n");

                if (vflag)
                {
                        tst_resm(TINFO, "number of states: %d", firmware_info1.number_of_states);
                        tst_resm(TINFO, "number of tests: %d", firmware_info1.number_of_tests);
                        tst_resm(TINFO, "firmware name: %s\n", firmware_info1.fw_name);
                }

                if ((firmware_info1.number_of_states != firmware_info.number_of_states)
                    || (firmware_info1.number_of_tests != firmware_info.number_of_tests)
                    || (strcmp(firmware_info1.fw_name, firmware_info.fw_name)))
                {
                        tst_resm(TFAIL, "setted and getted info are not equal!\n");
                        rv = TFAIL;
                }

                break;
        case 2:
                printf("====================================================================\n");
                tst_resm(TINFO, "set/get state...");
                printf("====================================================================\n");
                for (i = 0; i < 34; i++)
                {
                        ASSERT(set_state(i));
                }

                break;
        case 3:
                printf("====================================================================\n");
                tst_resm(TINFO, "set/get test...");
                printf("====================================================================\n");
                for (i = 0; i < 59; i++)
                {
                        ASSERT(set_test(i));
                        printf
                            ("====================================================================\n");
                }

                break;
        default:
                tst_resm(TINFO, "unknown test case number: %d", switch_fct);
                return TFAIL;
        }

        return rv;
}
