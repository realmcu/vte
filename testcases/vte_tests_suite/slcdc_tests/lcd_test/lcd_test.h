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
    @file   lcd_test.h

    @brief  C header file of the fbdraw test application that checks SLCDC driver by
            producing simple output to Epson fb.

====================================================================================================
Revision History:
                                            Modification     Tracking
Author (core ID)                         Date          Number         Description of Changes
-------------------------   ------------    ----------  -------------------------------------------
I. Semenchukov/smng001c      16/09/2004     TLSbo41672   Initial version 
E.Gromazina                          22/08/2005     TLSbo53875   Test enhancement
E.Gromazina                          21/10/2005     TLSbo56740    Update to Epson LCD
==================================================================================================*/

#ifndef _LCD_TEST_H_
#define _LCD_TEST_H_

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
#define PATH_LEN  50
#define CUR_WDTH  6
#define CUR_HGHT  10
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
int           VT_lcd_setup();
int           VT_lcd_cleanup();
int           VT_lcd_test(void);
int           get_modeinfo(int fd, struct fb_var_screeninfo *info);
int           colortest(struct pixel *p);
int           cursor_test(struct pixel *p);
int           picture_test(unsigned char *fb_wr_ptr, struct pixel *p, const unsigned char* picture, int xlen, int ylen, int depth);
void          print_fbinfo(void);
void          print_modeinfo(struct fb_var_screeninfo *info);
void          print_bfield(struct fb_bitfield *field);
unsigned char *draw_px(unsigned char *where, struct pixel *p);
int           ask_user(void);
void          sig_hand(int sig);
void          help(void);
        
#ifdef __cplusplus
}
#endif

#endif  /* _LCD_TEST_H_ */
