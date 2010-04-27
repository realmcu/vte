/***
**Copyright (C) 2004-2009 Freescale Semiconductor, Inc. All Rights Reserved.
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
    @file   lcd_power_test.c

    @brief  C source file of the sleep test application that checks SLCDC driver by
            putting device to the various VESA blanking levels.
====================================================================================================
Revision History:
                            Modification     Tracking
Author                          Date          Number    Description of Changes
-------------------------   ------------    ----------  -------------------------------------------
I. Semenchukov/smng001c      21/09/2004     TLSbo41672   Initial version 
L.Delaspre/rc149c            15/12/2004     TLSbo44058   Invalid argument issue investigation
E.Gromazina					19/08/2005	TLSbo53875	renaming test

====================================================================================================
Portability: Indicate if this module is portable to other compilers or platforms. 
             If not, indicate specific reasons why is it not portable.

==================================================================================================*/

#ifdef __cplusplus
extern "C"{
#endif

/*==================================================================================================
                                        INCLUDE FILES
==================================================================================================*/
/* Standard Include Files */
#include <sys/types.h>  /* open()                          */
#include <sys/stat.h>   /* open()                          */
#include <fcntl.h>      /* open()                          */
#include <unistd.h>     /* close()                         */
#include <stdio.h>      /* printf(), fgetc(), stdin        */
#include <errno.h>      /* errno                           */
#include <string.h>     /* strerror()                      */
#include <sys/ioctl.h>  /* ioctl()                         */
#include <sys/mman.h>   /* mmap(), munmap()                */
#include <linux/fb.h>   /* framebuffer related information */
#include <asm/types.h>
    
/* Harness Specific Include Files. */
#include "test.h"

/* Verification Test Environment Include Files */
#include "lcd_power_test.h"

/*==================================================================================================
                                        LOCAL MACROS
==================================================================================================*/


/*==================================================================================================
                          LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
==================================================================================================*/


/*==================================================================================================
                                       LOCAL CONSTANTS
==================================================================================================*/

/*==================================================================================================
                                       LOCAL VARIABLES
==================================================================================================*/
char                     fb_path[PATH_LEN] =
                            "/dev/fb0"; /* Path to the framebuffer device file           */
int                      fb_fd;         /* Framebuffer device file descriptor            */
struct fb_fix_screeninfo fb_info;       /* Framebuffer constant information              */
unsigned char            *fb_mem_ptr;   /* Pointer to the mapped momory of the fb device */

/*==================================================================================================
                                       GLOBAL CONSTANTS
==================================================================================================*/


/*==================================================================================================
                                       GLOBAL VARIABLES
==================================================================================================*/
extern char *F_opt;

/*==================================================================================================
                                   LOCAL FUNCTION PROTOTYPES
==================================================================================================*/
int get_modeinfo(struct fb_var_screeninfo *info);
int blank_test(int blank_mode);
void print_fbinfo(void);
unsigned char *draw_px(unsigned char *where, struct pixel *p);


/*==================================================================================================
                                       LOCAL FUNCTIONS
==================================================================================================*/


/*================================================================================================*/
/*===== VT_sleep_setup =====*/
/**
@brief  assumes the pre-condition of the test case execution. Opens the framebuffer device,
        gets information into the fb_fix_screeninfo structure, and maps fb device into memory.

@param  Input:  None
        Output: None
  
@return On success - return TPASS
        On failure - return the error code
*/
/*================================================================================================*/
int VT_power_setup(void)
{
    int rv = TFAIL;

    if (F_opt)
    {
        sprintf(fb_path, "%s", F_opt);
    }
    /* Open the framebuffer device */
    fb_fd = open(fb_path, O_RDWR);
    if (fb_fd < 0)
    {
        tst_brkm(TBROK, cleanup, "Cannot open LCD framebuffer: %s", strerror(errno));
    }
    /* Get constant fb info */
    if ((ioctl(fb_fd, FBIOGET_FSCREENINFO, &fb_info)) < 0)
    {
        tst_brkm(TFAIL, cleanup, "Cannot get LCD framebuffer fixed parameters due to ioctl error: %s",
                 strerror(errno));
    }
    /* Map fb device file into memory */
    fb_mem_ptr = mmap(NULL, fb_info.smem_len, PROT_READ | PROT_WRITE, MAP_SHARED, fb_fd, 0);
    if ((int)fb_mem_ptr == -1)
    {
        tst_brkm(TFAIL, cleanup, "Can't map framebuffer device into memory: %s\n", strerror(errno));
    }

    rv = TPASS;
    return rv;
}


/*================================================================================================*/
/*===== VT_sleep_cleanup =====*/
/**
@brief  assumes the post-condition of the test case execution. Closes the framebuffer device.

@param  Input:  None
        Output: None
  
@return On success - return TPASS
        On failure - return the error code
*/
/*================================================================================================*/
int VT_power_cleanup(void)
{
    munmap(fb_mem_ptr, fb_info.smem_len);
    close(fb_fd);

    return TPASS;
}


/*================================================================================================*/
/*===== VT_sleep_test =====*/
/**
@brief  Prints framebuffer constant information. Gets current video mode information. Assigns
        corresponding values to the members of the pixel structure. Applies three blanking modes
        and wakes up LCD after each mode.

@param  Input:  None
        Output: None
  
@return On success - return TPASS
        On failure - return the error code
*/
/*================================================================================================*/
int VT_power_test(void)
{
    struct
     fb_var_screeninfo mode_info;
    struct pixel       px;       /* Store basic screen info and current pixel color      */
    int                size;     /* Screen size in pixels                                */
    int                i;
    unsigned char      *fb_wr_ptr = fb_mem_ptr; /* Pointer to the current pixel location */
    int                act_mode;
    int                rv = TPASS;

    /* Print some fb information */
    print_fbinfo();
    if (get_modeinfo(&mode_info))
    {
        tst_brkm(TFAIL, cleanup, "Error: get_moreinfo");
    }
    /* Change activation flag and apply it */
    act_mode = mode_info.activate;
    mode_info.activate = FB_ACTIVATE_NOW | FB_ACTIVATE_FORCE;
    if (ioctl(fb_fd, FBIOPUT_VSCREENINFO, &mode_info))
    {
        tst_brkm(TFAIL, cleanup, "Cannot set LCD framebuffer parameters. Error is: %s",
                 strerror(errno));
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
    size = px.xres * px.yres;
    
    /* Clear screen and fill it with some pattern */
    tst_resm(TINFO, "Clearing the LCD...");
    px.r_color = px.g_color = px.b_color = 0; /* Set color values */
    for (i = 0; i < size; i++)
        fb_wr_ptr = draw_px(fb_wr_ptr, &px);
#ifdef MAD_TEST
    tst_resm(TINFO, "Fill in the LCD by green color");
    px.r_color = px.b_color = 0x00;
    px.g_color = 0xFF;
    fb_wr_ptr = fb_mem_ptr;
    for (i = 0; i < size; i++)
        fb_wr_ptr = draw_px(fb_wr_ptr, &px);
#else
    tst_resm(TINFO, "Fill in the LCD by yellow color");
    px.r_color = px.g_color = 0xFF;
    px.b_color = 0x0;
    fb_wr_ptr = fb_mem_ptr;
    for (i = 0; i < size; i++)
        fb_wr_ptr = draw_px(fb_wr_ptr, &px);
#endif    
    /* Standby mode. Spend some time before and after it */
    tst_resm(TINFO, "Go to the standby mode...");
    sleep(1);
    if (blank_test(VESA_VSYNC_SUSPEND + 1))
    {
        rv = TFAIL;
        tst_resm(TFAIL, "standby mode test KO");
    }
    else
    {
        tst_resm(TPASS, "standby mode test OK");
    }
    
    /* Suspend mode. Spend some time before and after it */
    tst_resm(TINFO, "Go to the suspend mode...");
    sleep(1);
    if (blank_test(VESA_HSYNC_SUSPEND + 1))
    {
        rv = TFAIL;
        tst_resm(TFAIL, "suspend mode test KO");
    }
    else
    {
        tst_resm(TPASS, "suspend mode test OK");
    }
    
    /* Poweroff mode. Spend some time before and after it */
    tst_resm(TINFO, "Go to the power off mode...");
    sleep(1);
    if (blank_test(VESA_POWERDOWN + 1))
    {
        rv = TFAIL;
        tst_resm(TFAIL, "power off mode test KO");
    }
    else
    {
        tst_resm(TPASS, "power off mode test OK");
    }
    
    /* Restore activation flag */
    mode_info.activate = act_mode;
    ioctl(fb_fd, FBIOPUT_VSCREENINFO, &mode_info);

    return rv;
}

/*================================================================================================*/
/*===== get_modeinfo =====*/
/**
@brief  Gets video mode parameters. If successful, prints some of them.

@param  Input:  None
        Output: info  - pointer to the structure where video mode info will be stored
  
@return  0 on success
        -1 if cannot get mode info via ioctl()
*/
/*================================================================================================*/
int get_modeinfo(struct fb_var_screeninfo *info)
{
    if (!info)
    {
        fprintf(stderr, "info isn't a valid pointer to 'struct fb_var_screeninfo'\n");
        return -1;
    }
    /* Get video mode parameters */
    if ((ioctl(fb_fd, FBIOGET_VSCREENINFO, info)) < 0)
    {
        tst_resm(TFAIL, "Cannot get LCD framebuffer current mode info due to ioctl error: %s",
            strerror(errno));
        return -1;
    }

    return 0;
};

/*================================================================================================*/
/*===== blank_test =====*/
/**
@brief  Sets blank mode. If successful, restores normal mode.

@param  Input:  blank_mode - which blank mode will be applied
        Output: None
  
@return  0 on success
        -1 if cannot set blank mode via ioctl()
*/
/*================================================================================================*/
int blank_test(int blank_mode)
{
    if (ioctl(fb_fd, FBIOBLANK, blank_mode))
    {
        tst_resm(TWARN, "Cannot turn LCD framebuffer into blank mode %d. Error is: %s",
                 blank_mode, strerror(errno));
        return -1;
    }
    sleep(3);
    tst_resm(TINFO, "Now turn on the LCD...");
    if (ioctl(fb_fd, FBIOBLANK, VESA_NO_BLANKING))
    {
        tst_resm(TWARN, "Cannot turn LCD framebuffer into some blank mode. Error is: %s",
                 strerror(errno));
        return -1;
    }
    sleep(2);
    
    return 0;
}
/*================================================================================================*/
/*===== print_fbinfo =====*/
/**
@brief  Prints some fb unchangeable parameters

@param  Input:  None
        Output: None
  
@return None
*/
/*================================================================================================*/
void print_fbinfo(void)
{
    printf("Frame buffer device information:\n");
    printf("  ID string:               %s\n", fb_info.id);
    printf("  FB memory (phys. addr.):\n");
    printf("    Start address:         %8lX\n", fb_info.smem_start);
    printf("    Length:                %8X\n", fb_info.smem_len);
    printf("  FB type:                 ");
    switch (fb_info.type)
    {
        case FB_TYPE_PACKED_PIXELS:
            printf("Packed Pixels\n");
            break;
        case FB_TYPE_PLANES:
            printf("Non interleaved planes\n");
            break;
        case FB_TYPE_INTERLEAVED_PLANES:
            printf("Interleaved planes, ");
            printf("interleave: %d\n", fb_info.type_aux);
            break;
        case FB_TYPE_TEXT:
            printf("Text/attributes\n");
            break;
        case FB_TYPE_VGA_PLANES:
            printf("EGA/VGA planes\n");
            break;
        default:
            printf("Unknown type!\n");
    }
    printf("  FB visual:               ");
    switch (fb_info.visual)
    {
        case FB_VISUAL_MONO01:
            printf("Monochrome, 1=Black, 0=White\n");
            break;
        case FB_VISUAL_MONO10:
            printf("Monochrome, 0=Black, 1=White\n");
            break;
        case FB_VISUAL_TRUECOLOR:
            printf("True color\n");
            break;
        case FB_VISUAL_PSEUDOCOLOR:
            printf("Pseudo color (like atari)\n");
            break;
        case FB_VISUAL_DIRECTCOLOR:
            printf("Direct color\n");
            break;
        case FB_VISUAL_STATIC_PSEUDOCOLOR:
            printf("Pseudo color readonly\n");
            break;
        default:
            printf("Unknown type!\n");
    }
    printf("  Line length (bytes):     %4d\n", fb_info.line_length);
    printf("  X axis hw panning step:  %4d\n", fb_info.xpanstep);
    printf("  Y axis hw panning step:  %4d\n", fb_info.ypanstep);
    printf("  Y axis wrapping step:    %4d\n", fb_info.ywrapstep);
    printf("  Type of accel. avail.:   %4d\n", fb_info.accel);
    printf("\n");

    return;
}

/*================================================================================================*/
/*===== draw_px =====*/
/**
@brief  Computes byte values from given color values depending on color depth and draws one pixel

@param  Input:  where - pointer to the pixel that will be drawn
                p     - pointer to struct pixel that contains color values and screen color info
        Output: None
  
@return pointer to the next pixel that will be drawn
*/
/*================================================================================================*/
unsigned char *draw_px(unsigned char *where, struct pixel *p)
{
#ifdef MAD_TEST
    __u32 value;

    if (!where)
    {
        fprintf(stderr, "where isn't a valid pointer to 'unsigned char'\n");
        return where;
    }
    if (!p)
    {
        fprintf(stderr, "p isn't a valid pointer to 'struct pixel'\n");
        return where;
    }
    /* Convert pixel color represented by 3 bytes to appropriate color depth */
    value = (p->r_color * (1 << p->r_field.length) / (1 << 8) ) << p->r_field.offset;
    value |= (p->g_color * (1 << p->g_field.length) / (1 << 8) ) << p->g_field.offset;
    value |= (p->b_color * (1 << p->b_field.length) / (1 << 8) ) << p->b_field.offset;
    switch (p->bpp * 8)
    {
        case 12 ... 16:
            *where++ = *((unsigned char *)&value + 1);
            *where++ = *((unsigned char *)&value);
            break;

        case 24:
            *where++ = *((unsigned char *)&value + 2);
            *where++ = *((unsigned char *)&value + 1);
            *where++ = *((unsigned char *)&value);
            break;

        case 32:
            /* Don't use transparency byte; this byte always equals 0 */
            *where++; 
            *where++ = *((unsigned char *)&value + 2);
            *where++ = *((unsigned char *)&value + 1);
            *where++ = *((unsigned char *)&value);
            break;

        default:
            break;
    }
    return where;
#else
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
        
/*        if ( p->t_field.length != 0)
        {
               value |= (p->trans * (1 << p->t_field.length) / (1 << 8) ) << p->t_field.offset;
        }*/
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
#endif
}

#ifdef __cplusplus
}
#endif
