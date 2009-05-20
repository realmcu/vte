/*====================*/
/**
    @file   fbdraw_test.c

    @brief  C source file of the fbdraw test application that checks SLCDC driver by
            producing simple output to Sharp/Epson fb.
*/
/*======================

Copyright (C) 2004, Freescale Semiconductor, Inc. All Rights Reserved
THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
Freescale Semiconductor, Inc.

====================
Revision History:
                            Modification     Tracking
Author                      Date             Number      Description of Changes
-------------------------   ------------    ----------  -------------------------------------------
I. Semenchukov/smng001c      16/09/2004      TLSbo41672   Initial version
E.Gromazina                  15/08/2005      TLSbo53875   Test enhancement
E.Gromazina                  28/09/2005      TLSbo53875    Fix bag
E.Gromazina                  21/10/2005      TLSbo56740    Update to Epson LCD
E.Gromazina                  24/10/2005      TLSbo57589    Directing alpha value to the base framebuffer (fb0)
E.Gromazina                  21/11/2005      TLSbo57589    Fix bag
====================
Portability: arm, gcc, montavista

======================*/

#ifdef __cplusplus
extern "C"{
#endif

/*======================
                                        INCLUDE FILES
======================*/
/* Standard Include Files */
#include <sys/time.h>   /* setitimer()                     */
#include <sys/types.h>  /* open()                          */
#include <sys/stat.h>   /* open()                          */
#include <fcntl.h>      /* open()                          */
#include <unistd.h>     /* close()                         */
#include <stdio.h>      /* printf(), fgetc(), stdin        */
#include <errno.h>      /* errno                           */
#include <string.h>     /* strerror()                      */
#include <sys/ioctl.h>  /* ioctl()                         */
#include <sys/mman.h>   /* mmap(), munmap()                */
#include <termios.h>    /* tcsetattr(), tcgetattr()        */
#include <linux/fb.h>   /* framebuffer related information */
#include <asm/types.h>  /* __u16, __u32                    */

/* Harness Specific Include Files. */
#include "test.h"

/* Verification Test Environment Include Files */
#include "lcd_test.h"
#include "pict/lcd-color_pallet.h"
#include "pict/lcd-full_saturation.h"
#include "pict/lcd-grid_pattern.h"
#include "pict/lcd-shade_gradient.h"
#include "pict/lcd-flower.h"
#include "pict/lcd-color_pallet_176x220.h"
#ifndef MAD_TEST_MODIFY
#include "pict/test01.h"
#endif


/*======================
                                        LOCAL MACROS
======================*/


/*======================
                          LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
======================*/


/*======================
                                       LOCAL CONSTANTS
======================*/



/*======================
                                       LOCAL VARIABLES
======================*/

int    fb_fd, fb1_fd;         /* Framebuffer devices file descriptors            */

struct fb_fix_screeninfo fb_info, fb1_info;       /* Framebuffer constant information              */
unsigned char           *fb_mem_ptr = NULL;   /* Pointer to the mapped memory of the 1th fb device */
unsigned char           *fb1_mem_ptr = NULL;  /* Pointer to the mapped memory of the 2nd fb device */

/* Cursor 16 color cmap and its red, green and blue colors (taken from fbcmap.c)         */
static __u16 red16[] =
{
        0x0000, 0x0000, 0x0000, 0x0000, 0xaaaa, 0xaaaa, 0xaaaa, 0xaaaa,
                0x5555, 0x5555, 0x5555, 0x5555, 0xffff, 0xffff, 0xffff, 0xffff
};
static __u16 green16[] =
{
        0x0000, 0x0000, 0xaaaa, 0xaaaa, 0x0000, 0x0000, 0x5555, 0xaaaa,
                0x5555, 0x5555, 0xffff, 0xffff, 0x5555, 0x5555, 0xffff, 0xffff
};
static __u16 blue16[] =
{
        0x0000, 0xaaaa, 0x0000, 0xaaaa, 0x0000, 0xaaaa, 0x0000, 0xaaaa,
                0x5555, 0xffff, 0x5555, 0xffff, 0x5555, 0xffff, 0x5555, 0xffff
};

static struct fb_cmap curs_cmap =
{
        0, 16, red16, green16, blue16, NULL
};

int sig_count = 0;     /* Updated by signal handler                     */

/*======================
                                       GLOBAL CONSTANTS
======================*/


/*======================
                                       GLOBAL VARIABLES
======================*/

extern int O_flag;
extern int testcase_nb;
extern int bpp;
extern char fb_path[PATH_LEN];
extern char fb_path_1[PATH_LEN];

#ifndef MAD_TEST_MODIFY
extern int X_flag;
extern int wait_sec;
#endif

/*======================
                                   LOCAL FUNCTION PROTOTYPES
======================*/


/*======================
                                       LOCAL FUNCTIONS
======================*/


/*====================*/
/*= VT_fbdraw_setup =*/
/**
@brief  assumes the pre-condition of the test case execution. Opens the framebuffer device,
        gets information into the fb_fix_screeninfo structure, and maps fb device into memory.

@param  Input:  None
        Output: None

@return On success - return TPASS
        On failure - return the error code
*/
/*====================*/
int VT_lcd_setup(void)
{
        int rv = TFAIL;
        /* Open the framebuffer device 1*/
        fb_fd = open(fb_path, O_RDWR);
        if (fb_fd < 0)
        {
                tst_resm(TFAIL, "Cannot open LCD framebuffer 1: %s", strerror(errno));
                return rv;
        }
        /* Get constant 1th fb  info */
        if ((ioctl(fb_fd, FBIOGET_FSCREENINFO, &fb_info)) < 0)
        {
                tst_resm(TFAIL, "Cannot get LCD framebuffer fixed parameters due to ioctl error: %s", strerror(errno));
                return rv;
        }
        /* Map fb device file into memory */
        fb_mem_ptr = mmap(NULL, fb_info.smem_len, PROT_READ | PROT_WRITE, MAP_SHARED, fb_fd, 0);
        if ((int)fb_mem_ptr == -1)
        {
                tst_resm(TFAIL,"Can't map framebuffer device into memory: %s", strerror(errno));
                return rv;
        }
        /* Open the framebuffer device 2*/
#ifndef MAD_TEST_MODIFY
 if(0)
#else
 if(O_flag)
#endif
        {
                fb1_fd = open(fb_path_1, O_RDWR);
                if (fb1_fd < 0)
                {
                        tst_resm(TFAIL, "Cannot open LCD framebuffer 2: %s", strerror(errno));
                        return rv;
                }
    /* Get constant 2th fb info */
                if ((ioctl(fb1_fd, FBIOGET_FSCREENINFO, &fb1_info)) < 0)
                {
                        tst_resm(TFAIL, "Cannot get LCD framebuffer_2 fixed parameters due to ioctl error: %s",
                                strerror(errno));
                        return rv;
                }
                /* Map fb device file into memory */
                fb1_mem_ptr = mmap(NULL, fb1_info.smem_len, PROT_READ | PROT_WRITE, MAP_SHARED, fb1_fd, 0);
                if ((int)fb1_mem_ptr == -1)
                {
                        tst_resm(TFAIL,"Can't map framebuffer_2 device into memory: %s", strerror(errno));
                        return rv;
                }
        }

        rv = TPASS;
        return rv;
}


/*====================*/
/*= VT_fbdraw_cleanup =*/
/**
@brief  assumes the post-condition of the test case execution. Closes the framebuffer device.

@param  Input:  None
        Output: None

@return On success - return TPASS
        On failure - return the error code
*/
/*====================*/
int VT_lcd_cleanup(void)
{
        if (fb_mem_ptr)
                munmap(fb_mem_ptr, fb_info.smem_len);

        if (fb_fd)
                close(fb_fd);
#ifndef MAD_TEST_MODIFY
 if(0)
#else
        if(O_flag)
#endif
        {
                if (fb1_mem_ptr)
                        munmap(fb1_mem_ptr, fb1_info.smem_len);
                if (fb1_fd)
                        close(fb1_fd);
        }

        return TPASS;
}


/*====================*/
/*= VT_fbdraw_test =*/
/**
@brief  Prints framebuffer constant information. Gets current video mode information. Assigns
        corresponding values to the members of the pixel structure. Performs eight simple color
        tests and draws color picture. Asks user after each test if (s)he sees right screen.

@param  Input:  None
        Output: None

@return On success - return TPASS
        On failure - return the error code
*/
/*====================*/
int VT_lcd_test(void)
{
        int i;

        struct fb_var_screeninfo mode_info, mode_info1, origmode_info,mode_info_old;
        struct pixel  px, px1;

        int rv = TPASS;
        int screensize, screensize1;

        unsigned char *fb_wr_ptr = fb_mem_ptr;  /* Pointer to the current pixel location                */
        int size;                               /* Screen size in pixels                                         */

        /* for scroll test */
        struct itimerval   time_val,            /* Our timer values                                     */
                old_val;             /* Original timer values                           */

        struct sigaction   sa;
        int scroll_lines = 1,    /* How many lines are scrolled + 1                   */
                my_count = 0;        /* For comparison with signal counter           */

        /* scroll_test sighandler */

        if (testcase_nb == 4)
        {
                sig_count = 0;                          /* Zero signal counter                          */
                memset(&sa, 0, sizeof(sa));
                sa.sa_handler = &sig_hand;              /* Assign a signal handler                  */

                if (sigaction(SIGALRM, &sa, NULL))      /* Register it                                      */
                {
                        tst_resm(TFAIL, "Cannot install signal handler: %s",
                                strerror(errno));
                        return TFAIL;
                }
        }

        /* Print some fb information                 */
        print_fbinfo();

        tst_resm(TINFO, " Current video mode information for 1th FB");
        if (get_modeinfo(fb_fd,&mode_info))
        {
                rv = TFAIL;
                return rv;
        }

        /* Check that 1th fb configured at <BPP> */
        if (mode_info.bits_per_pixel != bpp)
        {
                tst_resm(TFAIL, "LCD display 1th framebuffer BPP has wrong value");
                tst_resm(TFAIL, "Your framebuffer configured to have %d bits per pixel", mode_info.bits_per_pixel);

                rv = TFAIL;
                return rv;
        }

        if (testcase_nb == 4)
        {
                /* Set scroling mode and change virtual Y resolution, so now we can change yoffset and perform scrolling   */

                origmode_info = mode_info ;

                mode_info.vmode |= FB_VMODE_YWRAP;
                mode_info.yres_virtual = fb_info.smem_len / (mode_info.xres * mode_info.bits_per_pixel / 8);
                if ((ioctl(fb_fd, FBIOPUT_VSCREENINFO, &mode_info)) < 0)
                {
                        tst_resm(TFAIL, "Cannot put LCD framebuffer current mode info due to ioctl error: %s", strerror(errno));
                        return TFAIL;
                }
                else /* If successful, print some important info */
                {
                        tst_resm(TINFO,"Current video mode information:");
                        print_modeinfo(&mode_info);
                }
        }

        /* Fill in the px struct */
        px.bpp = mode_info.bits_per_pixel / 8;
        px.xres = mode_info.xres;
        px.yres = mode_info.yres;
        px.r_field.offset = mode_info.red.offset;
        px.r_field.length = mode_info.red.length;
        px.g_field.offset = mode_info.green.offset;
        px.g_field.length = mode_info.green.length;
        px.b_field.offset = mode_info.blue.offset;
        px.b_field.length = mode_info.blue.length;
        px.t_field.offset = mode_info.transp.offset;
        px.t_field.length = mode_info.transp.length;
        px.trans = 0x00;
        px.line_length = fb_info.line_length / px.bpp;

#ifndef MAD_TEST_MODIFY
 if(0)
#else
        if(O_flag)
#endif
        {

                /* Store video mode parameters */
                if ((ioctl(fb1_fd, FBIOGET_VSCREENINFO, &mode_info_old)) < 0)
                {
                        tst_resm(TFAIL, "Cannot get LCD framebuffer current mode info due to ioctl error: %s",strerror(errno));
                        return rv;
                }

                tst_resm(TINFO, " Current video mode information for 2nd FB");
                if (get_modeinfo(fb1_fd,&mode_info1))
                {
                        rv = TFAIL;
                        return rv;
                }

                /* Check that 2nd fb configured at <BPP> */
                if (mode_info1.bits_per_pixel != bpp)
                {
                        tst_resm(TFAIL, "LCD display 2nd framebuffer BPP has wrong value");
                        tst_resm(TFAIL, "Your framebuffer configured to have %d bits per pixel", mode_info.bits_per_pixel);
                        rv = TFAIL;
                        return rv;
                }

                /* Fill in the px struct */
                px1.bpp = mode_info1.bits_per_pixel / 8;
                px1.xres = mode_info1.xres;
                px1.yres = mode_info1.yres;
                px1.r_field.offset = mode_info1.red.offset;
                px1.r_field.length = mode_info1.red.length;
                px1.g_field.offset = mode_info1.green.offset;
                px1.g_field.length = mode_info1.green.length;
                px1.b_field.offset = mode_info1.blue.offset;
                px1.b_field.length = mode_info1.blue.length;
                px1.t_field.offset = mode_info1.transp.offset;
                px1.t_field.length = mode_info1.transp.length;
                px1.trans = 0x00;
                px1.line_length = fb1_info.line_length /px1.bpp;
        }

        switch (testcase_nb)
        {
                /* color test */
        case 1:
                {
                        screensize = px.line_length * px.yres * px.bpp;

                        /* Draw black color surface */
                        tst_resm(TINFO,"Fill in the LCD by black color");
                        px.r_color = px.g_color = px.b_color = 0; /* Set color values */
                        if (colortest(&px))
                                rv = TFAIL;
                        /* Draw red color surface */
                        tst_resm(TINFO,"Fill in the LCD by red color");
                        px.r_color = 0xFF;
                        px.g_color = px.b_color = 0;
                        if (colortest(&px))
                                rv = TFAIL;
                        /* Draw green color surface */
                        tst_resm(TINFO,"Fill in the LCD by green color");
                        px.g_color = 0xFF;
                        px.r_color = px.b_color = 0;
                        if (colortest(&px))
                                rv = TFAIL;
                        /* Draw blue color surface */
                        px.b_color = 0xFF;
                        px.r_color = px.g_color = 0;
                        tst_resm(TINFO,"Fill in the LCD by blue color");
                        if (colortest(&px))
                                rv = TFAIL;
                        /* Draw yellow color surface */
                        tst_resm(TINFO,"Fill in the LCD by yellow color");
                        px.r_color = px.g_color = 0xFF;
                        px.b_color = 0;
                        if (colortest(&px))
                                rv = TFAIL;
                        /* Draw magenta color surface */
                        tst_resm(TINFO,"Fill in the LCD by magenta color");
                        px.r_color = px.b_color = 0xFF;
                        px.g_color = 0;
                        if (colortest(&px))
                                rv = TFAIL;
                        /* Draw cyan color surface */
                        tst_resm(TINFO,"Fill in the LCD by cyan color");
                        px.g_color = px.b_color = 0xFF;
                        px.r_color = 0;
                        if (colortest(&px))
                                rv = TFAIL;
                        /* Draw white color surface */
                        tst_resm(TINFO,"Fill in the LCD by white color");
                        px.r_color = px.g_color = px.b_color = 0xFF;
                        if (colortest(&px))
                                rv = TFAIL;
                        /* End of color tests */

                        memset(fb_mem_ptr, 0x00, screensize);

                        break;
                }

                /* Color test for picture */
        case 2:
                {
                        screensize = px.line_length * px.yres * px.bpp;

                        tst_resm(TINFO,"Clearing the LCD ");
                        sleep(1);
                        memset(fb_mem_ptr, 0x00, screensize);
                        sleep(1);

                        /* Draw  pictures */
                        tst_resm(TINFO,"Draw Color_pallet picture in the LCD");
#ifndef MAD_TEST_MODIFY
                        tst_resm(TINFO,"Draw picture size is x %d, y %d in the LCD\n", px.xres, px.yres);
#endif

                        if ((px.xres == COLOR_PALLET_WIDTH ) && (px.yres == COLOR_PALLET_HEIGHT))
                        {
                                if (picture_test(fb_mem_ptr,&px,COLOR_PALLET_pixel_data,COLOR_PALLET_WIDTH,COLOR_PALLET_HEIGHT,COLOR_PALLET_BYTES_PER_PIXEL))
                                        rv = TFAIL;
                        }
                        else if ((px.xres == COLOR_PALLET_176_220_WIDTH ) && (px.yres == COLOR_PALLET_176_220_HEIGHT))
                        {
                                if (picture_test(fb_mem_ptr,&px,COLOR_PALLET_176_220_pixel_data,COLOR_PALLET_176_220_WIDTH,COLOR_PALLET_176_220_HEIGHT,COLOR_PALLET_176_220_BYTES_PER_PIXEL))
                                        rv = TFAIL;
                        }else if((px.xres == COLOR_PALLET_HEIGHT) && (px.yres == COLOR_PALLET_WIDTH)){
                                if (picture_test(fb_mem_ptr,&px,COLOR_PALLET_pixel_data,px.xres,px.yres,3))
                                        rv = TFAIL;
   }else
                        {

#ifndef MAD_TEST_MODIFY
    //printf("%d, %c\n", sizeof(gImage_test01)/sizeof(char), COLOR_PALLET_pixel_data[0]);
    if (picture_test(fb_mem_ptr,&px,gImage_test01,test01_pic_xlen,test01_pic_ylen,3))

    rv = TFAIL;

#else
                                if (picture_test(fb_mem_ptr,&px,COLOR_PALLET_pixel_data,COLOR_PALLET_WIDTH,COLOR_PALLET_HEIGHT,COLOR_PALLET_BYTES_PER_PIXEL))
                                        rv = TFAIL;
                                //tst_resm(TFAIL,"LCD panel size is not suitable for COLOR_PALLET pictures!");
                                //return TFAIL;
#endif


                        }

                        memset(fb_mem_ptr, 0x00, screensize);

                        break;

                }

                /* HW cursor test  */
        case 3:
                {

                        /* Draw background blue color surface */
                        px.r_color = px.g_color = 0x00;
                        px.b_color = 0xFF;

                        /*  size = px.xres * px.yres; */
                        size = px.line_length * px.yres;
                        screensize = size * px.bpp;

                        for (i = 0; i < size; i++)
                                fb_wr_ptr = draw_px(fb_wr_ptr,&px);

                        /* Now test cursor operations */
                        if (cursor_test(&px))
                                rv = TFAIL;

                        memset(fb_mem_ptr, 0x00, screensize);

                        break;

                }

                /*Scroll test  */
        case 4:
                {
                        /* size = px.xres * px.yres; */
                        size = px.line_length * px.yres;
                        screensize = size * px.bpp;

                        /* Clear screen */
                        tst_resm(TINFO,"Clearing the LCD");
                        px.r_color = px.g_color = px.b_color = 0; /* Set color values */
                        fb_wr_ptr = fb_mem_ptr;

                        for (i = 0; i < size; i++)
                                fb_wr_ptr = draw_px(fb_wr_ptr,&px);

                        /* Draw flower picture */
                        tst_resm(TINFO,"Draw flower picture in the LCD");

                        if (picture_test(fb_mem_ptr,&px,flower_pic,pic_xlen,pic_ylen,3))
                                return rv;

                        tst_resm(TINFO,"Now scroll this picture...");
                        /* Sut up timer values */
                        time_val.it_value.tv_sec = 0;
                        time_val.it_value.tv_usec = 10000;
                        time_val.it_interval.tv_sec = 0;
                        time_val.it_interval.tv_usec = 10000;
                        /* Set up a timer */
                        if (setitimer(ITIMER_REAL, &time_val, &old_val))
                        {
                                tst_resm(TFAIL, "Cannot set timer, error is: %s",
                                        strerror(errno));
                                ioctl(fb_fd, FBIOPUT_VSCREENINFO, &origmode_info);
                                return TFAIL;
                        }
   mode_info.xoffset = 0;
                        /* Do full picture scrolling */
                        while (scroll_lines <= pic_ylen)
                        {
                                if (my_count != sig_count) /* If signal arrived */
                                {
                                        mode_info.yoffset = scroll_lines; /* Set appropriate Y axis position */
                                        if (ioctl(fb_fd, FBIOPAN_DISPLAY, &mode_info)) /* Scroll picture */
                                        {
                                                tst_resm(TFAIL, "Cannot scroll framebuffer contents due to ioctl error: %s", strerror(errno));
                                                setitimer(ITIMER_REAL, &old_val, NULL);
                                                ioctl(fb_fd, FBIOPUT_VSCREENINFO, &origmode_info);
                                                return TFAIL;
                                        }
                                        my_count = sig_count; /* Else we can't check signal arriving */
                                        scroll_lines++;
                                }
                        }
                        /* Restore timer and video mode parameters */
                        setitimer(ITIMER_REAL, &old_val, NULL);
                        ioctl(fb_fd, FBIOPUT_VSCREENINFO, &origmode_info);

                        if (ask_user())
                                rv = TFAIL;

                        memset(fb_mem_ptr, 0x00, screensize);

                        break;
                }

                /* Double FB test for different opasity */
        case 5:
                {
   ioctl(fb1_fd, FBIOBLANK, FB_BLANK_UNBLANK);
                        if (!fb1_mem_ptr)
                        {
                                tst_resm(TFAIL,"The second frame buffer wasn't defined!");
                                return TFAIL;
                        }

                        screensize = px.line_length * px.yres * px.bpp;
                        screensize1 = px1.line_length * px1.yres * px1.bpp;

                        tst_resm(TINFO,"Clearing the LCD ");
                        sleep(1);
                        memset(fb_mem_ptr, 0x00, screensize);
                        memset(fb1_mem_ptr, 0x00, screensize1);
                        sleep(1);

                        tst_resm(TINFO,"Draw Grid_pattern in the LCD");
                        if (picture_test(fb1_mem_ptr,&px1,GRID_PATTERN_pixel_data,GRID_PATTERN_WIDTH,GRID_PATTERN_HEIGHT,GRID_PATTERN_BYTES_PER_PIXEL))
                                rv = TFAIL;
                        if (mode_info.transp.length != 0)
                        {
                                /* Change transparency */
                                tst_resm(TINFO,"Draw Color_pallet in the LCD ...");
                                tst_resm(TINFO,"Opacity - 0 %");
                                px.trans = 0x00; /* Opacity - 0 %*/
                                if (picture_test(fb_mem_ptr,&px,COLOR_PALLET_pixel_data,COLOR_PALLET_WIDTH,COLOR_PALLET_HEIGHT,COLOR_PALLET_BYTES_PER_PIXEL))
                                {
                                        tst_resm(TFAIL,"Picture_test is failed!");
                                        return TFAIL;
                                }
                                rv = ask_user();

                                tst_resm(TINFO,"Opacity - 25 %");
                                px.trans = 0x3F; /* Opacity - 25 % */
                                if (picture_test(fb_mem_ptr,&px,COLOR_PALLET_pixel_data,COLOR_PALLET_WIDTH,COLOR_PALLET_HEIGHT,COLOR_PALLET_BYTES_PER_PIXEL))
                                {
                                        tst_resm(TFAIL,"Picture_test is failed!");
                                        return TFAIL;
                                }
                                rv |= ask_user();

                                tst_resm(TINFO,"Opacity - 50 %");
                                px.trans = 0x7F;  /* Opacity - 50 % */
                                if (picture_test(fb_mem_ptr,&px,COLOR_PALLET_pixel_data,COLOR_PALLET_WIDTH,COLOR_PALLET_HEIGHT,COLOR_PALLET_BYTES_PER_PIXEL))
                                {
                                        tst_resm(TFAIL,"Picture_test is failed!");
                                        return TFAIL;
                                }
                                rv |= ask_user();

                                tst_resm(TINFO,"Opacity - 75 %");
                                px.trans = 0xBD;  /* Opacity - 75 % */
                                if (picture_test(fb_mem_ptr,&px,COLOR_PALLET_pixel_data,COLOR_PALLET_WIDTH,COLOR_PALLET_HEIGHT,COLOR_PALLET_BYTES_PER_PIXEL))
                                {
                                        tst_resm(TFAIL,"Picture_test is failed!");
                                        return TFAIL;
                                }
                                rv |= ask_user();

                                tst_resm(TINFO,"Opacity - 100 %");
                                px.trans = 0xFF;  /* Opacity - 100 % */
                                if (picture_test(fb_mem_ptr,&px,COLOR_PALLET_pixel_data,COLOR_PALLET_WIDTH,COLOR_PALLET_HEIGHT,COLOR_PALLET_BYTES_PER_PIXEL))
                                {
                                        tst_resm(TFAIL,"Picture_test is failed!");
                                        return TFAIL;
                                }
                                rv |= ask_user();

                                memset(fb_mem_ptr, 0x00, screensize);
                                memset(fb1_mem_ptr, 0x00, screensize1);
                        }
                        else
                        {
                                tst_resm(TFAIL,"Transparency is not supported");
                                return TFAIL;
                        }

                        break;

                }

                /* Double FB test test */
        case 6:
                {
                        if (!fb1_mem_ptr)
                        {
                                tst_resm(TFAIL,"The second frame buffer wasn't defined!");
                                return TFAIL;
                        }

                        screensize = px.xres * px.yres * px.bpp;
                        screensize1 = px1.xres * px1.yres * px1.bpp;

                        tst_resm(TINFO,"Clearing the LCD ... ");
                        sleep(1);
                        memset(fb_mem_ptr, 0x00, screensize);
                        memset(fb1_mem_ptr, 0x00, screensize1);
                        sleep(1);

                        tst_resm(TINFO,"Draw Shade_gradient in the LCD");
                        if (picture_test(fb_mem_ptr,&px,SHADE_GRADIENT_pixel_data, SHADE_GRADIENT_WIDTH, SHADE_GRADIENT_HEIGHT, SHADE_GRADIENT_BYTES_PER_PIXEL))
                                rv = TFAIL;

                        tst_resm(TINFO,"Draw Full_saturation in the LCD");
                        if (picture_test(fb1_mem_ptr,&px1,FULL_SATURATION_pixel_data,FULL_SATURATION_WIDTH,FULL_SATURATION_HEIGHT, FULL_SATURATION_BYTES_PER_PIXEL))
                                rv = TFAIL;

                        rv = ask_user();

                        memset(fb_mem_ptr, 0x00, screensize);
                        memset(fb1_mem_ptr, 0x00, screensize1);

                        break;
                }

        default:
                tst_resm(TBROK, "You entered wrong testcase number");
        }

        return rv;
}

/*====================*/
/*= get_modeinfo =*/
/**
@brief  Gets video mode parameters. If successful, prints some of them.

@param  Input:  None
        Output: info  - pointer to the structure where video mode info will be stored

@return  0 on success
        -1 if cannot get mode info via ioctl()
*/
/*====================*/
int get_modeinfo(int fd, struct fb_var_screeninfo *info)
{
        if (!info)
        {
                tst_resm(TFAIL, "info isn't a valid pointer to 'struct fb_var_screeninfo' ");
                return -1;
        }
        /* Get video mode parameters */
        if ((ioctl(fd, FBIOGET_VSCREENINFO, info)) < 0)
        {
                tst_resm(TFAIL, "Cannot get LCD framebuffer current mode info due to ioctl error: %s",strerror(errno));
                return -1;
        }
        else /* If successful, print some important info */
        {
                print_modeinfo(info);
        }
        return 0;
};

/*====================*/
/*= colortest =*/
/**
@brief  Fills in the LCD by given color. Asks user if (s)he sees right color pattern.

@param  Input:  p - pointer to the structure holding color to be drawn and some screen parameters
        Output: None

@return 0 if test passed
        1 if test failed
*/
/*====================*/
int colortest(struct pixel *p)
{
        unsigned char *fb_wr_ptr = fb_mem_ptr;        /* Where we draw         */
        int size = p->line_length * p->yres; /* Screen size in pixels */
        int i;

        if (!p)
        {
                tst_resm(TFAIL, "p isn't a valid pointer to 'struct pixel' ");
                return -1;
        }
        if (!fb_wr_ptr)
        {
                tst_resm(TFAIL, "fb_wr_ptr isn't a valid pointer to 'unsigned char' ");
                return -1;
        }

        for (i = 0; i < size; i++)
                fb_wr_ptr = draw_px(fb_wr_ptr, p);

        return ask_user();
}

/*====================*/
/*= cursor_test =*/
/**
@brief  Prepares fb_cursor structure. Draws a cursor, then waits user input and moves a cursor
        accordingly. User can use the following keys: arrow up, down, left and right.
        Also temporarily changes terminal parameters when waits user input.

@param  Input:  p - pointer to the structure holding some screen parameters
        Output: None

@return 0 if test passed
        -1 if test failed
*/
/*====================*/
int cursor_test(struct pixel *p)
{
        unsigned char curs_move; /* In which direction the cursor will moved                     */
        struct  fb_cursor my_curs, old_curs;  /* This is needed in order to erase cursor in previous position */
        struct termios old_tset, new_tset;
        char  curs_image[CUR_WDTH * CUR_HGHT], curs_mask [CUR_WDTH * CUR_HGHT];

        if (!p)
        {
                tst_resm(TFAIL, "p isn't a valid pointer to 'struct pixel' ");
                return -1;
        }

        memset(&my_curs, 0, sizeof(struct fb_cursor));
        memcpy(curs_image, fb_mem_ptr, CUR_WDTH * CUR_HGHT);
        memset(curs_mask, 0xFF, CUR_WDTH * CUR_HGHT);

        /* Set initial cursor position; so we must set FB_CUR_SETPOS */
        my_curs.image.dx = 0;
        my_curs.image.dy = 0;
        my_curs.set = FB_CUR_SETPOS;
        /* Set cursor size; so we must set FB_CUR_SETSIZE */
        my_curs.image.width = CUR_WDTH;
        my_curs.image.height = CUR_HGHT;
        my_curs.set |= FB_CUR_SETSIZE;
        /* Set image depth and color map; so we must set FB_CUR_SETCMAP */
        my_curs.image.depth = 4;
        my_curs.image.cmap = curs_cmap;
        my_curs.set |= FB_CUR_SETCMAP;
        /* Set cursor image and mask; so we must set FB_CUR_SETSHAPE */
        my_curs.image.data = curs_image;
        my_curs.mask = curs_mask;
        my_curs.set |= FB_CUR_SETSHAPE;
        /* Enable cursor and set raster operation to inverted image; so we must set FB_CUR_SETCUR */
        my_curs.enable = 1;
        my_curs.rop = ROP_XOR;
        my_curs.set |= FB_CUR_SETIMAGE;
        /* Do it */
        if ((ioctl(fb_fd, FBIO_CURSOR, &my_curs)) < 0)
        {
                tst_resm(TFAIL, "Cannot enable cursor: %s", strerror(errno));
                return -1;
        }
        my_curs.set = FB_CUR_SETPOS; /* Now we change only cursor position */
        tst_resm(TINFO,"Now cursor must appear in top left position of the screen. Try to move it by pressing"
                " up, down, left and right arrow keys. Press 'Enter' to end this test");

        /* Change terminal parameters */
        tcgetattr(0, &old_tset);
        new_tset = old_tset;
        new_tset.c_lflag &= (~ICANON); /* No Enter  */
        new_tset.c_lflag &= (~ECHO);   /* No echo   */
        new_tset.c_lflag &= (~ISIG);   /* No CTRL+C */
        tcsetattr(0, TCSANOW, &new_tset);
        do
        {
                /* Arrows keycodes are 3-bytes codes; first two are 27 and 91 */
                if ( ((curs_move = getchar()) == 27) && ((curs_move = getchar()) == 91) )
                {
                        old_curs = my_curs;
                        curs_move = getchar();
                        switch (curs_move)
                        {
                        case 65: /* Move cursor up */
                                my_curs.image.dy -= CUR_HGHT;
                                if (my_curs.image.dy > (p->yres - CUR_HGHT - 1))
                                        my_curs.image.dy = p->yres - CUR_HGHT - 1 - (p->yres % CUR_HGHT);
                                break;

                        case 66: /* Move cursor down */
                                my_curs.image.dy += CUR_HGHT;
                                if (my_curs.image.dy > (p->yres - CUR_HGHT - 1))
                                        my_curs.image.dy = 0;
                                break;

                        case 67: /* Move cursor right */
                                my_curs.image.dx += CUR_WDTH;
                                if (my_curs.image.dx > (p->xres - CUR_WDTH - 1))
                                        my_curs.image.dx = 0;
                                break;

                        case 68: /* Move cursor left */
                                my_curs.image.dx -= CUR_WDTH;
                                if (my_curs.image.dx > (p->xres - CUR_WDTH - 1))
                                        my_curs.image.dx = p->xres - CUR_WDTH - 1 - (p->xres % CUR_WDTH);
                                break;

                        default:
                                continue;
                        }

                        /* If proper key was pressed, cursor position was changed - erase old cursor */
                        if ( (my_curs.image.dx != old_curs.image.dx) || (my_curs.image.dy != old_curs.image.dy) )
                        {
                                old_curs.enable = 0;
                                old_curs.rop = ROP_COPY;
                                if (ioctl(fb_fd, FBIO_CURSOR, &old_curs))
                                {
                                        tst_resm(TFAIL, "Cannot erase cursor: %s", strerror(errno));
                                        tcsetattr(0, TCSANOW, &old_tset);
                                        return -1;
                                }
                                /* Now draw cursor in new position */
                                if (ioctl(fb_fd, FBIO_CURSOR, &my_curs))
                                {
                                        tst_resm(TFAIL, "Cannot draw cursor: %s", strerror(errno));
                                        tcsetattr(0, TCSANOW, &old_tset);
                                        return -1;
                                }
                        }
                }
        }
        while (curs_move != 10); /* Press Enter to quit the cycle */
        tcsetattr(0, TCSANOW, &old_tset);
        /* Disable cursor */
        my_curs.enable = 0;
        my_curs.rop = ROP_COPY;
        if (ioctl(fb_fd, FBIO_CURSOR, &my_curs))
        {
                tst_resm(TFAIL, "Cannot disable cursor: %s", strerror(errno));
                tcsetattr(0, TCSANOW, &old_tset);
                return -1;
        }
        return ask_user();
}


/*====================*/
/*= picture_test =*/
/**
@brief  Draws picture, represented by constant array in the file. Asks user if (s)he sees right
        picture.

@param  Input:  p - pointer to the structure holding some screen parameters
        Output: None

@return 0 if test passed
        1 if test failed
*/
/*====================*/
int picture_test(unsigned char *fb_wr_ptr, struct pixel *p, const unsigned char* picture, int xlen, int ylen, int depth)
{
        int  i, j, index = 0;

        if (!p)
        {
                tst_resm(TFAIL, "p isn't a valid pointer to 'struct pixel' ");
                return -1;
        }
        if (!fb_wr_ptr)
        {
                tst_resm(TFAIL, "fb_wr_ptr isn't a valid pointer to 'unsigned char' ");
                return -1;
        }
        for (j = 0; j < ylen; j++)            /* Picture size by X axis */
        {
                for (i = 0; i < xlen; i++)        /* Picture size by Y axis */
                {
                        p->r_color = picture[index++];   /* Get next pixel colors  */
                        p->g_color = picture[index++];
                        p->b_color = picture[index++];
                        if (depth == 4)
                                p->trans =  picture[index++];

                        fb_wr_ptr = draw_px(fb_wr_ptr, p);  /* Draw it */
                }
                /* Set fb memory pointer to the beginning of next line */
                fb_wr_ptr += (p->line_length - xlen) * p->bpp;
        }

        if (testcase_nb == 4 || testcase_nb == 5 || testcase_nb == 6 )
                return 0;
        else
                return ask_user();
}
/*====================*/
/*= print_fbinfo =*/
/**
@brief  Prints some fb unchangeable parameters

@param  Input:  None
        Output: None

@return None
*/
/*====================*/
void print_fbinfo(void)
{
        tst_resm(TINFO,"Frame buffer device information:");
        tst_resm(TINFO,"ID string: %s", fb_info.id);
        tst_resm(TINFO,"FB memory (phys. addr.): Start address:%8lX  Length:%8X", fb_info.smem_start, fb_info.smem_len);
        switch (fb_info.type)
        {
        case FB_TYPE_PACKED_PIXELS:
                tst_resm(TINFO,"FB type: Packed Pixels");
                break;
        case FB_TYPE_PLANES:
                tst_resm(TINFO,"FB type: Non interleaved planes");
                break;
        case FB_TYPE_INTERLEAVED_PLANES:
                tst_resm(TINFO,"FB type: Interleaved planes,  interleave: %d", fb_info.type_aux);
                break;
        case FB_TYPE_TEXT:
                tst_resm(TINFO,"FB type: Text/attributes");
                break;
        case FB_TYPE_VGA_PLANES:
                tst_resm(TINFO,"FB type: EGA/VGA planes");
                break;
        default:
                tst_resm(TINFO,"FB type: Unknown type!");
        }

        switch (fb_info.visual)
        {
        case FB_VISUAL_MONO01:
                tst_resm(TINFO,"FB visual: Monochrome, 1=Black, 0=White");
                break;
        case FB_VISUAL_MONO10:
                tst_resm(TINFO,"FB visual: Monochrome, 0=Black, 1=White");
                break;
        case FB_VISUAL_TRUECOLOR:
                tst_resm(TINFO,"FB visual: True color");
                break;
        case FB_VISUAL_PSEUDOCOLOR:
                tst_resm(TINFO,"FB visual: Pseudo color (like atari)");
                break;
        case FB_VISUAL_DIRECTCOLOR:
                tst_resm(TINFO,"FB visual: Direct color");
                break;
        case FB_VISUAL_STATIC_PSEUDOCOLOR:
                tst_resm(TINFO,"FB visual: Pseudo color readonly");
                break;
        default:
                tst_resm(TINFO,"FB visual: Unknown type!");
        }
        tst_resm(TINFO,"Line length (bytes):       %4d", fb_info.line_length);
        tst_resm(TINFO,"X axis hw panning step:  %4d", fb_info.xpanstep);
        tst_resm(TINFO,"Y axis hw panning step:  %4d", fb_info.ypanstep);
        tst_resm(TINFO,"Y axis wrapping step:     %4d", fb_info.ywrapstep);
        tst_resm(TINFO,"Type of accel. avail.:     %4d", fb_info.accel);

        return;
}

/*====================*/
/*= print_modeinfo =*/
/**
@brief  Prints some fb video mode parameters from fb_var_screeninfo structure

@param  Input:  info - pointer to the structure holding video mode info
        Output: None

@return None
*/
/*====================*/
void print_modeinfo(struct fb_var_screeninfo *info)
{
        if (!info)
        {
                tst_resm(TFAIL, "info isn't a valid pointer to 'struct fb_var_screeninfo'");
                return;
        }
        tst_resm(TINFO,"Screen resolution: X visible:%4d  Y visible:%4d", info->xres, info->yres);
        tst_resm(TINFO,"X virtual:%4d  Y virtual:%4d", info->xres_virtual, info->yres_virtual);
        tst_resm(TINFO,"Offsets from virtual to visible: X: %4d Y: %4d", info->xoffset, info->yoffset);
        tst_resm(TINFO,"Bits per pixel: %4d", info->bits_per_pixel);
        tst_resm(TINFO,"Bitfields Red:");
        print_bfield(&(info->red));
        tst_resm(TINFO,"Bitfields Green:");
        print_bfield(&(info->green));
        tst_resm(TINFO,"Bitfields Blue:");
        print_bfield(&(info->blue));
        tst_resm(TINFO,"Bitfields Transp.:");
        print_bfield(&(info->transp));
        if (info->grayscale)
                tst_resm(TINFO,"Grayscale palette is used");
        if (info->nonstd)
                tst_resm(TINFO,"Non-standard pixel format is used");

        switch (info->sync)
        {
        case FB_SYNC_HOR_HIGH_ACT:
                tst_resm(TINFO,"Synchronization: Horisontal");
                break;

        case FB_SYNC_VERT_HIGH_ACT:
                tst_resm(TINFO,"Synchronization: Vertical");
                break;

        case FB_SYNC_EXT:
                tst_resm(TINFO,"Synchronization: External");
                break;

        case FB_SYNC_COMP_HIGH_ACT:
                tst_resm(TINFO,"Synchronization: Composite");
                break;

        case FB_SYNC_BROADCAST:
                tst_resm(TINFO,"Synchronization: Broadcast");
                break;

        case FB_SYNC_ON_GREEN:
                tst_resm(TINFO,"Synchronization: On green");
                break;

        default:
                tst_resm(TINFO,"Synchronization: Unknown type!");
        }

        switch (info->vmode)
        {
        case FB_VMODE_NONINTERLACED:
                tst_resm(TINFO,"Vertical mode: Noninterlaced");
                break;

        case FB_VMODE_INTERLACED:
                tst_resm(TINFO,"Vertical mode: Interlaced");
                break;

        case FB_VMODE_DOUBLE:
                tst_resm(TINFO,"Vertical mode: Double");
                break;

        default:
                tst_resm(TINFO,"Vertical mode: Unknown type!");
        }
        tst_resm(TINFO,"Clockwise rotation angle: %3d", info->rotate);
        tst_resm(TINFO,"");

        return;
}

/*====================*/
/*= print_bfield =*/
/**
@brief  Prints color info from given bit field structure

@param  Input:  field - pointer to the fb_bitfield structure
        Output: None

@return None
*/
/*====================*/
void print_bfield(struct fb_bitfield *field)
{
        if (!field)
        {
                tst_resm(TFAIL, "field isn't a valid pointer to 'struct fb_birfield' ");
                return;
        }
        if (field->msb_right)
                tst_resm(TINFO,"Field length:%2d , Field offset:%2d, most significant bit is right", field->length, field->offset);
        else
                tst_resm(TINFO,"Field length:%2d , Field offset:%2d, most significant bit is  left", field->length, field->offset);

        return;
}

/*====================*/
/*= draw_px =*/
/**
@brief  Computes byte values from given color values depending on color depth and draws one pixel

@param  Input:  where - pointer to the pixel that will be drawn
                p     - pointer to struct pixel that contains color values and screen color info
        Output: None

@return pointer to the next pixel that will be drawn
*/
/*====================*/
unsigned char *draw_px(unsigned char *where, struct pixel *p)
{
        __u32 value;

        if (!where)
        {
                tst_resm(TFAIL, "where isn't a valid pointer to 'unsigned char' ");
                return where;
        }
        if (!p)
        {
                tst_resm(TFAIL, "p isn't a valid pointer to 'struct pixel' ");
                return where;
        }

        /* Convert pixel color represented by 3 bytes to appropriate color depth */
        value = (p->r_color * (1 << p->r_field.length) / (1 << 8) ) << p->r_field.offset;
        value |= (p->g_color * (1 << p->g_field.length) / (1 << 8) ) << p->g_field.offset;
        value |= (p->b_color * (1 << p->b_field.length) / (1 << 8) ) << p->b_field.offset;

        if ( p->t_field.length != 0)
        {
                value |= (p->trans * (1 << p->t_field.length) / (1 << 8) ) << p->t_field.offset;
        }
        switch (p->bpp * 8)
        {
        case 12 ... 16:
                *where++ = *((unsigned char *)&value);
                *where++ = *((unsigned char *)&value + 1);
                break;

        case 24:
                *where++ = *((unsigned char *)&value);
                *where++ = *((unsigned char *)&value + 1);
                *where++ = *((unsigned char *)&value + 2);
                break;

        case 32:
                *where++ = *((unsigned char *)&value);
                *where++ = *((unsigned char *)&value + 1);
                *where++ = *((unsigned char *)&value + 2);
                *where++ = *((unsigned char *)&value + 3);
                break;

        default:
                break;
        }

        return where;
}

/*====================*/
/*= ask_user =*/
/**
@brief  Asks user to answer the question: is the drawn picture right?

@param  Input:  None
        Output: None

@return 1 - if user asks "No,  wrong"
        0 - if user asks "Yes, right"
*/
/*====================*/
int ask_user(void)
{
#ifndef MAD_TEST_MODIFY
  if (X_flag)
  {
              sleep(wait_sec);
  return 0;
  }
#endif

        unsigned char answer;
        int   ret = 2;

        do
        {
                tst_resm(TINFO,"Is the picture correct ?   [y/n] ");
                answer = fgetc(stdin);
                if (answer == 'Y' || answer == 'y')
                        ret = 0;
                else if (answer == 'N' || answer == 'n')
                        ret = 1;
        }
        while (ret == 2);
        fgetc(stdin);       /* Wipe CR character from stream */
        return ret;
}

/*====================*/
/*= sig_hand =*/
/**
@brief  Simply increments signal counter. All other work will be done by test function.

@param  Input:  None
        Output: None

@return None
*/
/*====================*/
void sig_hand(int sig)
{
        sig_count++;
        return;
}


#ifdef __cplusplus
}
#endif
