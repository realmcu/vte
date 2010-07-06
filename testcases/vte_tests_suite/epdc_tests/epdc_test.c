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
/*================================================================================================*/
/**
    @file   epdc_test.c
*/
#ifdef __cplusplus
extern "C"{
#endif

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
#include <time.h>
#include <linux/mxcfb.h>
#include <linux/pxp_dma.h>
/* Harness Specific Include Files. */
#include "test.h"

/* Verification Test Environment Include Files */
#include "mxcfb_test.h"


/*********************************************************************************/
/* Macro name:  CALL_IOCTL()                                                     */
/* Description: Macro checks for any error in function execution                 */
/*              based on the return value. In case of error, the function exits. */
/*********************************************************************************/
#define CALL_IOCTL(ioctl_cmd)\
{ \
    if( (ioctl_cmd) < 0 )\
    {\
        tst_resm( TFAIL, "%s : %s fails #%d [File: %s, line: %d]", __FUNCTION__, "ioctl_", errno, __FILE__, __LINE__-2);\
        perror("ioctl"); \
        return FALSE;\
    }\
}

#define MAX_WAIT 4

/*global virable*/
extern epdc_opt m_opt;

int fb_fd; /* Framebuffer device file descriptor            */
struct fb_fix_screeninfo fb_info; /* Framebuffer constant information */
unsigned char  *fb_mem_ptr;   /* Pointer to the mapped momory of the fb device */

int get_modeinfo(struct fb_var_screeninfo *info);
void print_fbinfo(void);
unsigned char *draw_px(unsigned char *where, struct pixel *p);
int draw_pattern(int fd, unsigned char * pfb, int r, int g, int b );
BOOL pan_test();
BOOL draw_test();

/*
 * Draw test
 */
BOOL draw_test()
{
 tst_resm(TINFO, "draw a green screen in bg ground");
 if( TPASS != draw_pattern(fb_fd,fb_mem_ptr,0,255,0))
 {
   tst_resm(TINFO, "fail to draw patter on bg");
   return FALSE;
 }
 return TRUE;
}

/*
 * pan test
 */
BOOL pan_test()
{
   int y, old_yvres;
   struct fb_var_screeninfo mode_info;
   struct mxcfb_gbl_alpha gbl_alpha;
/*x pan is not supported*/

#if 1
 tst_resm(TINFO,"test fb0 pan");
 CALL_IOCTL(ioctl(fb_fd, FBIOGET_VSCREENINFO, &mode_info));
 old_yvres = mode_info.yres_virtual;
 mode_info.yres_virtual = mode_info.yres * 2;
 CALL_IOCTL(ioctl(fb_fd, FBIOPUT_VSCREENINFO, &mode_info));

 /*remap the devices*/
  munmap(fb_mem_ptr, fb_info.smem_len);
  CALL_IOCTL(ioctl(fb_fd, FBIOGET_FSCREENINFO, &fb_info));
  fb_mem_ptr = mmap(NULL, fb_info.smem_len, PROT_READ | PROT_WRITE, MAP_SHARED, fb_fd, 0);
    if ((int)fb_mem_ptr == -1)
    {
        tst_brkm(TFAIL, cleanup, "Can't map framebuffer device into memory: %s\n", strerror(errno));
    }

 tst_resm(TINFO, "draw a green screen in back ground");
  if( TPASS != draw_pattern(fb_fd,fb_mem_ptr,0,255,0))
 {
   tst_resm(TINFO, "fail to draw patter on fb0");
   return FALSE;
 }
 sleep(5);
   for (y = 0; y <= mode_info.yres; y += mode_info.yres / 8)
   {
        mode_info.yoffset = y;
		printf("\r offset at %d", y);
        CALL_IOCTL(ioctl(fb_fd, FBIOPAN_DISPLAY, &mode_info));
		sleep(1);
  }
 mode_info.yres_virtual = old_yvres;
 CALL_IOCTL(ioctl(fb_fd, FBIOPUT_VSCREENINFO, &mode_info));
#endif
 return TRUE;
}

BOOL test_alt_update()
{
BOOL ret = FALSE
int fd_pxp;
int i= 0 ,j = 0;
int count = 100;
int update_maker = 0x112;
struct pxp_mem_desc mem;
#define PXP_DEVICE_NAME "/dev/pxp_device"
#define BUFFER_WIDTH 16
#define BUFFER_HEIGHT 16
#define PXP_BUFFER_SIZE (BUFFER_WIDTH*BUFFER_HEIGHT)
struct mxcfb_update_data im_update = {
  {0,0,BUFFER_WIDTH*2,BUFFER_HEIGHT*2},/*region round to 8*/
  257,/*waveform mode 0-255, 257 auto*/
  0, /*update mode 0(partial),1(Full)*/
  update_maker,/*update_maker assigned by user*/
  0x56,/*use ambient temperature set*/
  1,/*enable alt buffer*/
  {0,0,0,{0,0,0,0}}/*set this later*/
  };

/*step 1: set up update data*/
 fd_pxp = open(PXP_DEVICE_NAME, O_RDWR, 0);
 mem.size = PXP_BUFFER_SIZE;
 if (ioctl(fd_pxp, PXP_IOC_GET_PHYMEM, &mem) < 0)
 {
	mem.phys_addr = 0;
	mem.cpu_addr = 0;
	goto END;
 }
 im_update.alt_buffer_data.phys_addr = mem.phys_addr;
 im_update.alt_buffer_data.width = BUFFER_WIDTH;
 im_update.alt_buffer_data.height = BUFFER_HEIGHT;
 im_update.alt_buffer_data.alt_update_region.top = 0;
 im_update.alt_buffer_data.alt_update_region.left = 0;
 im_update.alt_buffer_data.alt_update_region.width = BUFFER_WIDTH;
 im_update.alt_buffer_data.alt_update_region.height = BUFFER_HEIGHT;
 for(i= 0; i < BUFFER_WIDTH; i++)
	for(j = 0; j < BUFFER_HEIGHT; j++)
	{
		mem.virt_uaddr[i*BUFFER_HEIGHT + j] = 128;
	}

/*step 2: start test*/
  /*partial update*/
  while(count--)
  {
	/*black and white alternative*/
	draw_pattern(fb_fd,fb_mem_ptr,255,255,255);
	CALL_IOCTL(ioctl(fb_fd, MXCFB_SEND_UPDATE, &im_update));
	while(ioctl(fb_fd, MXCFB_WAIT_FOR_UPDATE_COMPLETE, &update_maker)< 0)
    {
		wait_time++;
		if(wait_time > MAX_WAIT)
		{
		 printf("wait time exceed!!!\n");
		 break;
		}
	}
	wait_time = 0;
	printf("partial mode next update\n");
	draw_pattern(fb_fd,fb_mem_ptr,0,0,0);
  }
  /*full update*/
  count = 100;
  im_update.update_mode = 1;
  while(count--)
  {
	/*black and white alternative*/
	draw_pattern(fb_fd,fb_mem_ptr,255,255,255);
	CALL_IOCTL(ioctl(fb_fd, MXCFB_SEND_UPDATE, &im_update));
	while(ioctl(fb_fd, MXCFB_WAIT_FOR_UPDATE_COMPLETE, &update_maker)< 0)
	{
		wait_time++;
		if(wait_time > MAX_WAIT)
		{
		printf("wait time exceed!!!\n");
		break;
		}
	}
	wait_time = 0;
	printf("partial mode next update\n");
	draw_pattern(fb_fd,fb_mem_ptr,0,0,0);
  }

 /*step 4: clean up */
 if (ioctl(fd_pxp, PXP_IOC_PUT_PHYMEM, &mem) < 0)
 {
	mem.phys_addr = 0;
	mem.cpu_addr = 0;
	goto END;
 }
END:
 if(fd_pxp > 0)
	close(fd_pxp);
 ret = TRUE;
 return ret;
}

BOOL test_wait_update()
{
/*suppose you have set up the device before run this case*/
	/*step 1: set up update data*/
  int count = 100;
  int update_maker = 0x111;
  struct mxcfb_update_data im_update = {
  {0,0,16,16},/*region round to 8*/
  257,/*waveform mode 0-255, 257 auto*/
  0, /*update mode 0(partial),1(Full)*/
  update_maker,/*update_maker assigned by user*/
  0x56,/*use ambient temperature set*/
  0,/*do not use alt buffer*/
  {0,0,0,{0,0,0,0}}
  };
  /*step 2: update and wait finished*/
  while(count--)
  {
	/*black and white alternative*/
  draw_pattern(fb_fd,fb_mem_ptr,255,255,255);
  CALL_IOCTL(ioctl(fb_fd, MXCFB_SEND_UPDATE, &im_update));
  while(ioctl(fb_fd, MXCFB_WAIT_FOR_UPDATE_COMPLETE, &update_maker)< 0)
  {
     wait_time++;
	if(wait_time > MAX_WAIT)
	{
	  printf("wait time exceed!!!\n");
	  break;
	}
  }
  wait_time = 0;
  printf("partial mode next update\n");
  draw_pattern(fb_fd,fb_mem_ptr,0,0,0);
  }
  /*step 3: now using full update mode*/
  count = 100;
  im_update.update_mode = 1;
  while(count--)
  {
	/*black and white alternative*/
  draw_pattern(fb_fd,fb_mem_ptr,255,255,255);
  CALL_IOCTL(ioctl(fb_fd, MXCFB_SEND_UPDATE, &im_update));
  while(ioctl(fb_fd, MXCFB_WAIT_FOR_UPDATE_COMPLETE, &update_maker)< 0)
  {
     wait_time++;
	if(wait_time > MAX_WAIT)
	{
	  printf("full mode wait time exceed!!!\n");
	  break;
	}
  }
  wait_time = 0;
  printf("next update\n");
  draw_pattern(fb_fd,fb_mem_ptr,0,0,0);
  }
  return TRUE;
}

/*===== epdc_fb_setup =====*/
/**
@brief  assumes the pre-condition of the test case execution. Opens the framebuffer device,
        gets information into the fb_fix_screeninfo structure, and maps fb device into memory.

@param  Input:  None
        Output: None

@return On success - return TPASS
        On failure - return the error code
*/
int epdc_fb_setup(void)
{
    int rv = TFAIL;
    /* Open the framebuffer device */
    fb_fd = open(d_opt, O_RDWR);
    if (fb_fd < 0)
    {
        tst_brkm(TBROK, cleanup, "Cannot open framebuffer: %s", strerror(errno));
    }
    /* Get constant fb info */
    if ((ioctl(fb_fd, FBIOGET_FSCREENINFO, &fb_info)) < 0)
    {
        tst_brkm(TFAIL, cleanup, "Cannot get framebuffer fixed parameters due to ioctl error: %s",
                 strerror(errno));
    }
    /* Map fb device file into memory */
    fb_mem_ptr = mmap(NULL, fb_info.smem_len, PROT_READ | PROT_WRITE, MAP_SHARED, fb_fd, 0);
    if ((int)fb_mem_ptr == -1)
    {
        tst_brkm(TFAIL, cleanup, "Can't map framebuffer device into memory: %s\n", strerror(errno));
    }

	CALL_IOCTL(ioctl(fd, MXCFB_SET_WAVEFORM_MODES, &m_opt.waveform));

	if(m_opt.grayscale != -1)
	{
		struct fb_var_screeninfo mode_info;
		CALL_IOCTL(ioctl(fb_fd, FBIOGET_VSCREENINFO, &mode_info));
		mode_info.grayscale = m_opt.grayscale;
		CALL_IOCTL(ioctl(fd, FBIOPUT_VSCREENINFO, &mode_info));
		printf("set gray scale mode to %d\n", mode_info.grayscale);
	}
	if (m_opt.temp != -1)
	CALL_IOCTL(ioctl(fd, MXCFB_SET_TEMPERATURE, &m_opt.waveform));
	if (m_opt.au != -1)
	CALL_IOCTL(ioctl(fd, MXCFB_SET_AUTO_UPDATE_MODE, &m_opt.waveform));

    rv = TPASS;
    return rv;
}


/*===== epdc_fb_cleanup =====*/
/**
@brief  assumes the post-condition of the test case execution. Closes the framebuffer device.

@param  Input:  None
        Output: None

@return On success - return TPASS
        On failure - return the error code
*/
int epdc_fb_cleanup(void)
{
    draw_pattern(fb_fd,fb_mem_ptr,0,0,0);
    munmap(fb_mem_ptr, fb_info.smem_len);
    close(fb_fd);
    return TPASS;
}

int epdc_fb_test()
{
 int  rv = TPASS;
 switch(iID)
 {
  case 0:
       tst_resm(TINFO, "normal test");
  case 1:
       tst_resm(TINFO, "pan test");
       if (!pan_test())
       {
         rv = TFAIL;
         tst_resm(TFAIL, "pan test FAIL");
       }else
         tst_resm(TPASS, "pan test ok");
       break;
  case 2:
       tst_resm(TINFO, "draw test test");
       if (!draw_test())
       {
         rv = TFAIL;
         tst_resm(TFAIL, "draw test FAIL");
       }else
         tst_resm(TPASS, "draw test ok");
       break;
  case 3: /*wait update test*/
		if(!test_wait_update())
		{
          rv = TFAIL;
          tst_resm(TFAIL, "wait update FAIL");
		}else
          tst_resm(TPASS, "wait update ok");
		break;
  case 4: /*alt buffer overlay test*/
		if(!test_alt_update())
		{
          rv = TFAIL;
          tst_resm(TFAIL, "alt update FAIL");
		}else
          tst_resm(TPASS, "alt update ok");
		break;
  case 5:/*collision region update test*/
		break;
  case 6:/*max update region count test */
		break;
  case 7:/*1000 frames sequence region no collision frame rate test*/
		break;
  default:
		break;
 }
 return rv;
}


int draw_pattern(int fd ,unsigned char * pfb, int r, int g, int b)
{
    struct pixel  px;
    int           size;
    int           i;
    unsigned char      *fb_wr_ptr = pfb;
    int                act_mode;
    int                rv = TPASS;
    struct fb_fix_screeninfo fx_fb_info;       /* Framebuffer constant information              */
    struct fb_var_screeninfo mode_info;

    /* Print some fb information */
    if ((ioctl(fd, FBIOGET_VSCREENINFO, &mode_info)) < 0)
    {
       perror("ioctl");
       rv = TFAIL;
       return rv;
    }

    /* Change activation flag and apply it */
    act_mode = mode_info.activate;
    mode_info.activate = FB_ACTIVATE_NOW | FB_ACTIVATE_FORCE;
    if (ioctl(fd, FBIOPUT_VSCREENINFO, &mode_info))
    {
       perror("ioctl");
       rv = TFAIL;
       return rv;
    }


    CALL_IOCTL(ioctl(fd, FBIOGET_FSCREENINFO, &fx_fb_info));
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
    px.trans = 0x00;
    px.line_length = fx_fb_info.line_length / px.bpp;
    size = px.line_length * px.yres;

    /* Clear screen and fill it with some pattern */
    px.r_color = r;
    px.g_color = g;
    px.b_color = b; /* Set color values */
    for (i = 0; i < size; i++)
	{
		if(i % 10 == 0 && i!=0 )
		{
		 px.r_color = px.r_color - 5;
		 px.g_color = px.g_color - 5;
		 px.b_color = px.b_color - 5;
		}
        fb_wr_ptr = draw_px(fb_wr_ptr, &px);
	}
     /* Restore activation flag */
    #if 1
    mode_info.activate = act_mode;
    ioctl(fd, FBIOPUT_VSCREENINFO, &mode_info);
    #endif
    return rv;
}

/*===== draw_px =====*/
/**
@brief  Computes byte values from given color values depending on color depth and draws one pixel

@param  Input:  where - pointer to the pixel that will be drawn
                p     - pointer to struct pixel that contains color values and screen color info
        Output: None

@return pointer to the next pixel that will be drawn
*/
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

/*        if ( p->t_field.length != 0)
        {
               value |= (p->trans * (1 << p->t_field.length) / (1 << 8) ) << p->t_field.offset;
        }*/
        switch (p->bpp * 8)
        {
		case 8:
			*where++ = value;
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

#ifdef __cplusplus
}
#endif
