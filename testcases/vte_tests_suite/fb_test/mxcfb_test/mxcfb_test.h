/***
**Copyright (C) 2005-2009 Freescale Semiconductor, Inc. All Rights Reserved.
**
**The code contained herein is licensed under the GNU General Public
**License. You may obtain a copy of the GNU General Public License
**Version 2 or later at the following locations:
**
**http://www.opensource.org/licenses/gpl-license.html
**http://www.gnu.org/copyleft/gpl.html
**/
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

#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif

typedef unsigned char BOOL;
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
    unsigned char   trans;    /* Transparency value   */
    struct px_field t_field;
   int line_length;            /* length of a line in px  */
};

/*==================================================================================================
                                 GLOBAL VARIABLE DECLARATIONS
==================================================================================================*/


/*==================================================================================================
                                     FUNCTION PROTOTYPES
==================================================================================================*/
int VT_fb_setup();
int VT_fb_cleanup();
int VT_fb_test(void);

void cleanup();


#ifdef __cplusplus
}
#endif

#endif  /* SLEEP_TEST_H */
