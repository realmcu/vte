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
    @file   lcd_power_test.h

    @brief  C header file of the sleep test application that checks SLCDC driver by
            putting device to the various VESA blanking levels.     
====================================================================================================
Revision History:
                            Modification     Tracking
Author (core ID)                Date          Number    Description of Changes
-------------------------   ------------    ----------  -------------------------------------------
I. Semenchukov/smng001c      21/09/2004     TLSbo41672   Initial version 
L.Delaspre/rc149c            15/12/2004     TLSbo44058   Invalid argument issue investigation
E.Gromazina					19.08.2005	TLSbo53875	renaming test

==================================================================================================*/

#ifndef SLEEP_TEST_H
#define SLEEP_TEST_H

#ifdef __cplusplus
extern "C"{
#endif

/*==================================================================================================
                                         INCLUDE FILES
==================================================================================================*/
#include <linux/fb.h>   /* framebuffer related information */

/*==================================================================================================
                                           CONSTANTS
==================================================================================================*/


/*==================================================================================================
                                       DEFINES AND MACROS
==================================================================================================*/
#define PATH_LEN  128
/*==================================================================================================
                                             ENUMS
==================================================================================================*/


/*==================================================================================================
                                 STRUCTURES AND OTHER TYPEDEFS
==================================================================================================*/
/* This structure contains information taken from fb_var_screeninfo struct */
struct px_field
{
    int offset;                 /* Bitfield offset */
    int length;                 /* Bitfield length */
};

/* This structure is used when particular pixel is being written into fb memory */
struct pixel
{
    int bpp;                    /* Color depth in bytes            */
    int xres;                   /* X resolution in pixels          */
    int yres;                   /* Y resolution in pixels          */
    unsigned char   r_color;    /* Red color value to be written   */
    struct px_field r_field;    
    unsigned char   g_color;    /* Green color value to be written */
    struct px_field g_field;
    unsigned char   b_color;    /* Blue color value to be written  */
    struct px_field b_field;
};

/*==================================================================================================
                                 GLOBAL VARIABLE DECLARATIONS
==================================================================================================*/


/*==================================================================================================
                                     FUNCTION PROTOTYPES
==================================================================================================*/
int VT_power_setup();
int VT_power_cleanup();
int VT_power_test(void);

void cleanup();


#ifdef __cplusplus
}
#endif

#endif  /* SLEEP_TEST_H */
