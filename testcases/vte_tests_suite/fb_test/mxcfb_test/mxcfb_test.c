/*================================================================================================*/
/**
    @file   mxcfb_test.c

    @brief  C source file of the sleep test application that checks SLCDC driver by
            putting device to the various VESA blanking levels.
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

int                      fb_fd;         /* Framebuffer device file descriptor            */
int                      fb_fd_fg;         /* Framebuffer device file descriptor            */
struct fb_fix_screeninfo fb_info;       /* Framebuffer constant information              */
struct fb_fix_screeninfo fb_fg_info;       /* Framebuffer constant information              */
unsigned char            *fb_mem_ptr;   /* Pointer to the mapped momory of the fb device */
unsigned char            *fb_fg_mem_ptr;   /* Pointer to the mapped momory of the fb device */

static int iID = 0;


extern char *T_opt;
extern char *d_opt;
extern char *D_opt;
int get_modeinfo(struct fb_var_screeninfo *info);
int blank_test(int blank_mode);
void print_fbinfo(void);
unsigned char *draw_px(unsigned char *where, struct pixel *p);
BOOL vsync_test();
BOOL galpha_test();
BOOL pan_test();
BOOL colorkey_test();
BOOL ovpos_test();
BOOL draw_test();
int draw_pattern(int fd, unsigned char * pfb, int r, int g, int b );


/*
 * Draw test
 */
BOOL draw_test()
{

 tst_resm(TINFO, "draw a red screen in bg ground");
 if( TPASS != draw_pattern(fb_fd,fb_mem_ptr,255,0,0))
 {
   tst_resm(TINFO, "fail to draw patter on bg");
   return FALSE;
 }
 sleep(6);
 tst_resm(TINFO, "draw a green screen in bg ground");
 if( TPASS != draw_pattern(fb_fd,fb_mem_ptr,0,255,0))
 {
   tst_resm(TINFO, "fail to draw patter on bg");
   return FALSE;
 }
 sleep(6);
 return TRUE;
}

/*
 * overlay postion test
 */
BOOL ovpos_test()
{
 int x, y;
 struct mxcfb_pos pos; 
 struct fb_var_screeninfo mode_info;

 tst_resm(TINFO, "draw a red screen in fore ground");
 if( TPASS != draw_pattern(fb_fd_fg,fb_fg_mem_ptr,255,0,0))
 {
   tst_resm(TINFO, "fail to draw patter on fb1");
   return FALSE;
 }

 CALL_IOCTL(ioctl(fb_fd_fg, FBIOGET_VSCREENINFO, &mode_info));

 mode_info.xres_virtual = mode_info.xres;
 mode_info.yres_virtual = mode_info.yres;

 CALL_IOCTL(ioctl(fb_fd_fg, FBIOPUT_VSCREENINFO, &mode_info));

 for(x= 0; x < mode_info.xres; x += 8)
   for(y = 0; y < mode_info.yres; y += 8)
   {
     pos.x = 8 * ((x + 7)/8);
     pos.y = 8 * ((y + 7)/8);
     printf( "\r x: %d / y: %d", pos.x,pos.y);
     CALL_IOCTL(ioctl(fb_fd_fg, MXCFB_SET_OVERLAY_POS, &pos));
     usleep(100);
   }
   pos.x = 0;
   pos.y = 0;
   CALL_IOCTL(ioctl(fb_fd_fg, MXCFB_SET_OVERLAY_POS, &pos));
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
 tst_resm(TINFO, "ensure thr global alpha is 50");
 gbl_alpha.enable = 1;
 gbl_alpha.alpha = 50;
 CALL_IOCTL(ioctl(fb_fd, MXCFB_SET_GBL_ALPHA, &gbl_alpha));

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
   for (y = 0; y <= mode_info.yres; y += mode_info.yres / 2) 
   {
        mode_info.yoffset = y;
	printf("\r offset at %d", y);
        CALL_IOCTL(ioctl(fb_fd, FBIOPAN_DISPLAY, &mode_info));
	sleep(1);
  }
 mode_info.yres_virtual = old_yvres;
 CALL_IOCTL(ioctl(fb_fd, FBIOPUT_VSCREENINFO, &mode_info));
#endif
 tst_resm(TINFO,"test fb1 pan");
 CALL_IOCTL(ioctl(fb_fd_fg, FBIOGET_VSCREENINFO, &mode_info));
 old_yvres = mode_info.yres_virtual;
 mode_info.yres_virtual = mode_info.yres * 2;
 CALL_IOCTL(ioctl(fb_fd_fg, FBIOPUT_VSCREENINFO, &mode_info));
 
 /*remap the device*/ 
  munmap(fb_fg_mem_ptr, fb_fg_info.smem_len);
  CALL_IOCTL(ioctl(fb_fd_fg, FBIOGET_FSCREENINFO, &fb_fg_info));
  fb_fg_mem_ptr = mmap(NULL, fb_fg_info.smem_len, PROT_READ | PROT_WRITE, MAP_SHARED, fb_fd_fg, 0);
      if ((int)fb_fg_mem_ptr == -1)
        tst_brkm(TFAIL, cleanup, "Can't map framebuffer device into memory: %s\n", strerror(errno));
 tst_resm(TINFO, "draw a red screen in fore ground");

 if( TPASS != draw_pattern(fb_fd_fg,fb_fg_mem_ptr,255,0,0))
 {
   tst_resm(TINFO, "fail to draw patter on fb1");
   return FALSE;
 }
 for (y = 0; y <= mode_info.yres ; y += mode_info.yres / 2) 
 {
   mode_info.yoffset = y;
   printf("\r offset at %d", y);
   CALL_IOCTL(ioctl(fb_fd_fg, FBIOPAN_DISPLAY, &mode_info));
   sleep(1);
 }
 mode_info.yres_virtual = old_yvres;
 CALL_IOCTL(ioctl(fb_fd_fg, FBIOPUT_VSCREENINFO, &mode_info));
  munmap(fb_fg_mem_ptr, fb_fg_info.smem_len);
  CALL_IOCTL(ioctl(fb_fd_fg, FBIOGET_FSCREENINFO, &fb_fg_info));
  fb_fg_mem_ptr = mmap(NULL, fb_fg_info.smem_len, PROT_READ | PROT_WRITE, MAP_SHARED, fb_fd_fg, 0);
      if ((int)fb_fg_mem_ptr == -1)
        tst_brkm(TFAIL, cleanup, "Can't map framebuffer device into memory: %s\n", strerror(errno));
 tst_resm(TINFO, "draw a red screen in fore ground");
 return TRUE;
}


/*
 * color key test
 */
BOOL colorkey_test()
{
 struct mxcfb_gbl_alpha gbl_alpha;
 struct mxcfb_color_key key;
  
 tst_resm(TINFO, "draw a green screen in back ground");
  if( TPASS != draw_pattern(fb_fd,fb_mem_ptr,0,255,0))
 {
   tst_resm(TINFO, "fail to draw patter on fb0");
   return FALSE;
 }

 tst_resm(TINFO, "draw a red screen in fore ground");
 if( TPASS != draw_pattern(fb_fd_fg,fb_fg_mem_ptr,255,0,0))
 {
   tst_resm(TINFO, "fail to draw patter on fb1");
   return FALSE;
 }

  key.enable = 1;
  key.color_key = 0x00FF0000; // Red
  
  tst_resm(TINFO,"Color key enabled\n");
  tst_resm(TINFO,"Now the forground is green\n");
  CALL_IOCTL(ioctl(fb_fd, MXCFB_SET_CLR_KEY, &key));
  sleep(3);

  /*make fore ground opaque*/
  gbl_alpha.enable = 1;
  gbl_alpha.alpha = 128;
  CALL_IOCTL(ioctl(fb_fd, MXCFB_SET_GBL_ALPHA, &gbl_alpha));
  tst_resm(TINFO,"Now the forground is yellow\n");

  
  sleep(3);

  key.enable = 0;
  CALL_IOCTL(ioctl(fb_fd, MXCFB_SET_CLR_KEY, &key));
  tst_resm(TINFO,"Color key disabled\n");
  
  /* gbl_alpha.enable = 0; */
  gbl_alpha.alpha = 50;
  ioctl(fb_fd, MXCFB_SET_GBL_ALPHA, &gbl_alpha);
  tst_resm(TINFO,"Global alpha restore to 50\n");
  return TRUE;
}


/*
 * global alpha test
 */
BOOL galpha_test()
{
 int i;
 struct mxcfb_gbl_alpha gbl_alpha;
#if 1 
 tst_resm(TINFO, "draw a green screen in back ground"); 
 if( TPASS != draw_pattern(fb_fd,fb_mem_ptr,0,255,0))
 {
   tst_resm(TINFO, "fail to draw patter on fb0");
   return FALSE;
 }
#endif
#if 1
 tst_resm(TINFO, "draw a red screen in fore ground");
 if( TPASS != draw_pattern(fb_fd_fg,fb_fg_mem_ptr,255,0,0))
 {
   tst_resm(TINFO, "fail to draw patter on fb1");
   return FALSE;
 }
#endif
 /*now set alpha*/
 tst_resm(TINFO, "the overlay screen is change from red to yellow to green");
 gbl_alpha.enable = 1;
 for (i = 0; i < 0x100; i+=10) 
 {
     gbl_alpha.alpha = i;
     CALL_IOCTL(ioctl(fb_fd, MXCFB_SET_GBL_ALPHA, &gbl_alpha));
     sleep(1);
 }
  gbl_alpha.enable = 0;
  gbl_alpha.alpha = 0;
  CALL_IOCTL(ioctl(fb_fd, MXCFB_SET_GBL_ALPHA, &gbl_alpha));
  tst_resm(TINFO,"Global alpha disabled\n");

#if 1 
 tst_resm(TINFO, "clear back ground"); 
 if( TPASS != draw_pattern(fb_fd,fb_mem_ptr,0,0,0))
 {
   tst_resm(TINFO, "fail to draw patter on fb0");
   return FALSE;
 }
#endif
#if 1
 tst_resm(TINFO, "clear fore ground");
 if( TPASS != draw_pattern(fb_fd_fg,fb_fg_mem_ptr,0,0,0))
 {
   tst_resm(TINFO, "fail to draw patter on fb1");
   return FALSE;
 }
#endif

 return TRUE;
}

/*
 * VSYNC test
 */
BOOL vsync_test()
{
 int ifn = 120, ifrate = 0, i = 0, r = 0;
 clock_t stime, etime;
 long ldur = 0;  
 stime = clock();
 for (i = 0, r = 0; i < ifn; i++, r++)
 {
  r = r == 255 ? 0 : r;
  if( TPASS != draw_pattern(fb_fd, fb_mem_ptr, 0, r, 0))
  {
   tst_resm(TINFO, "fail to draw patter on fb0");
   return FALSE;
   }
  CALL_IOCTL(ioctl(fb_fd, MXCFB_WAIT_FOR_VSYNC, &i));
 }
 etime = clock();
 ldur = (etime - stime); 
 ifrate = 1000 * ldur/CLOCKS_PER_SEC;/*ms*/
 if(ifrate > 0)
   ifrate = ifn * 1000 / ifrate;
 tst_resm(TINFO,"the sw draw rate for BG is %d\n",ifrate);
 return TRUE;
}

/*===== VT_fb_setup =====*/
/**
@brief  assumes the pre-condition of the test case execution. Opens the framebuffer device,
        gets information into the fb_fix_screeninfo structure, and maps fb device into memory.

@param  Input:  None
        Output: None
  
@return On success - return TPASS
        On failure - return the error code
*/
int VT_fb_setup(void)
{
    int rv = TFAIL;
    if (T_opt)
    {
      iID = atoi(T_opt);
    }
    if(NULL == D_opt || NULL == d_opt)
      return rv;

    /* Open the framebuffer device */
    fb_fd = open(D_opt, O_RDWR);
    if (fb_fd < 0)
    {
        tst_brkm(TBROK, cleanup, "Cannot open fb0 framebuffer: %s", strerror(errno));
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

    if (iID == 3 || iID == 1 || iID == 4 || iID == 5)
    {
      /*global alpha test */
      fb_fd_fg = open(d_opt, O_RDWR);
      if ( fb_fd_fg < 0 )
        tst_brkm(TBROK, cleanup, "Cannot open fb1 framebuffer: %s", strerror(errno));
      
      CALL_IOCTL(ioctl(fb_fd_fg, FBIOBLANK, FB_BLANK_UNBLANK));
      sleep(3);
      if ((ioctl(fb_fd_fg, FBIOGET_FSCREENINFO, &fb_fg_info)) < 0)
        tst_brkm(TFAIL, cleanup, "Cannot get framebuffer fixed parameters due to ioctl error: %s",
                 strerror(errno));
      
      fb_fg_mem_ptr = mmap(NULL, fb_fg_info.smem_len, PROT_READ | PROT_WRITE, MAP_SHARED, fb_fd_fg, 0);
      if ((int)fb_fg_mem_ptr == -1)
        tst_brkm(TFAIL, cleanup, "Can't map framebuffer device into memory: %s\n", strerror(errno));
    }

    rv = TPASS;
    return rv;
}


/*===== VT_sleep_cleanup =====*/
/**
@brief  assumes the post-condition of the test case execution. Closes the framebuffer device.

@param  Input:  None
        Output: None
  
@return On success - return TPASS
        On failure - return the error code
*/
int VT_fb_cleanup(void)
{
    draw_pattern(fb_fd,fb_mem_ptr,0,0,0);
    munmap(fb_mem_ptr, fb_info.smem_len);
    close(fb_fd);
    if(iID == 3 || iID == 1 || iID == 4 || iID == 5)
    { 
    draw_pattern(fb_fd_fg,fb_fg_mem_ptr,0,0,0);
     munmap(fb_fg_mem_ptr, fb_fg_info.smem_len);
     close(fb_fd_fg);
    }

    return TPASS;
}

int VT_fb_test()
{
 int  rv = TPASS;
 switch(iID)
 {
  case 1:
       tst_resm(TINFO, "color key test");
       if (!colorkey_test())
       {
         rv = TFAIL; 
         tst_resm(TFAIL, "color key test FAIL");
       }else
         tst_resm(TPASS, "color key test ok"); 
       break;
  case 2:
       tst_resm(TINFO, "vsync test");
       if (!vsync_test())
       {
         rv = TFAIL; 
         tst_resm(TFAIL, "vsync test FAIL");
       }else{
         tst_resm(TPASS, "vsync test ok"); 
       }
       break;
  case 3:
       tst_resm(TINFO, "global alpha test");
       if (!galpha_test())
       {
         rv = TFAIL; 
         tst_resm(TFAIL, "alpha test FAIL");
       }else
         tst_resm(TPASS, "alpha test ok");
       break;
  case 4:
       tst_resm(TINFO, "pan test");
       if (!pan_test())
       {
         rv = TFAIL; 
         tst_resm(TFAIL, "pan test FAIL");
       }else
         tst_resm(TPASS, "pan test ok");
       break;
  case 5:
       tst_resm(TINFO, "overlay pos test");
       if (!ovpos_test())
       {
         rv = TFAIL; 
         tst_resm(TFAIL, "overlay test FAIL");
       }else
         tst_resm(TPASS, "overlay test ok");
       break;
  case 6:
       tst_resm(TINFO, "draw test test");
       if (!draw_test())
       {
         rv = TFAIL; 
         tst_resm(TFAIL, "draw test FAIL");
       }else
         tst_resm(TPASS, "draw test ok");
       break;
  default:
       break;
 }
 
 return rv;
}


int draw_pattern(int fd ,unsigned char * pfb, int r, int g, int b)
{
    struct pixel       px;       /* Store basic screen info and current pixel color      */
    int                size;     /* Screen size in pixels                                */
    int                i;
    unsigned char      *fb_wr_ptr = pfb; /* Pointer to the current pixel location */
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
        fb_wr_ptr = draw_px(fb_wr_ptr, &px);

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
