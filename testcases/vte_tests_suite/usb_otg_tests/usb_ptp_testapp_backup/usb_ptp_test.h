/*================================================================================================*/
/**
        @file   usb_HID_test.h

        @brief  header file for USB-HID driver test.
*/
/* Portability: ARM GCC

==================================================================================================*/
#ifndef USB_OTG_TEST_H
#define USB_PTP_TEST_H

#ifdef __cplusplus
extern "C"{
#endif

/*==================================================================================================
                                        DEFINES AND MACROS
==================================================================================================*/
//#define        USB_DEV "otg_message"

//#define        OTG_APPLICATION
//#define        OTG_LINUX

#define        ASSERT(a) if(a != TPASS)rv = TFAIL;

/*==================================================================================================
                                        INCLUDE FILES
==================================================================================================*/
#include <sys/types.h>
#include <asm/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/hiddev.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <linux/input.h>
#include<..//usbstillimage.h>   //????



/*==================================================================================================
                                        DEFINES AND MACROS
==================================================================================================*/

#define USB_PTP_DEVICE       "/dev/ptp_usb"



/*==================================================================================================
                                    FUNCTION PROTOTYPES
==================================================================================================*/
int VT_usb_ptp_test_cleanup(void);
int VT_usb_ptp_test_setup(void);
int VT_usb_ptp_test(int switch_fct);

#ifdef __cplusplus
}
#endif

#endif        /* USB_ptp_TEST_H */

