/***
 * **Copyright 2005-2009 Freescale Semiconductor, Inc. All Rights Reserved.
 * **
 * **The code contained herein is licensed under the GNU General Public
 * **License. You may obtain a copy of the GNU General Public License
 * **Version 2 or later at the following locations:
 * **
 * **http://www.opensource.org/licenses/gpl-license.html
 * **http://www.gnu.org/copyleft/gpl.html
 * **/
/*================================================================================================*/
/**
    @file   imx_fb.c

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

#include "test.h"

/* Verification Test Environment Include Files */


#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif


/*********************************************************************************/
/* Macro name:  CALL_IOCTL()                                                     */
/* Description: Macro checks for any error in function execution                 */
/*              based on the return value. In case of error, the function exits. */
/*********************************************************************************/
#define CALL_IOCTL(ioctl_cmd)\
{ \
    if( (ioctl_cmd) < 0 )\
    {\
        printf("Error! %s : %s fails #%d [File: %s, line: %d]", __FUNCTION__, "ioctl_", errno, __FILE__, __LINE__-2);\
        perror("ioctl"); \
        return FALSE;\
    }\
}
/*operation type*/
enum{eTP_SET = 10, eTP_GET, eTP_DRAW, eTP_INVALID};
/*operation name*/
enum{eTN_ALPHA, eTN_COLORKEY, eTN_PATTERN, eTN_INVALID};

enum{ePT_RED, ePT_GREEN, ePT_BLUE, ePT_INVALID};

typedef unsigned char BOOL;

typedef struct ALPHA_DATA{
int pc;
int value;
} sALPHA;

typedef struct COLORKEY_DATA{
int pc;
union{
unsigned char value[4];
struct sPIXEL{
unsigned char alpa;
unsigned char r;
unsigned char g;
unsigned char b;
}pixel;
}uValue;
} sCOLOR_KEY;

typedef struct DRAW_PATTERN{
int pc;
int value;
} sDRAW_PAT;

typedef struct sOP_ARRAY{
int op_tp;
int op_tn;
void * operants;
} sOP;

typedef BOOL (*ops)(void *);

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

static sOP m_op;
static sALPHA m_alpha;
static sCOLOR_KEY m_ck;
static sDRAW_PAT m_dr;

char *TCID = "imx_ipu";
int TST_TOTAL = 1;

int fb_fd;

void cleanup();
void help();
BOOL alpha_op(void *);
BOOL ck_op(void * );
BOOL draw_op(void *);

int draw_pattern(int fd ,unsigned char * pfb, int r, int g, int b);

unsigned char *draw_px(unsigned char *where, struct pixel *p);

ops m_ops[]={
     alpha_op,
     ck_op,
     draw_op,
     NULL
};


int parse_arg(int argc, char ** argv)
{
 int stage = 0;
 int pcn = 0;
 char fb_dev[255];

 memset(fb_dev,0, sizeof(fb_dev));
 if(argc < 2)
   return -1;

 sprintf(fb_dev, "/dev/fb%s", argv[argc - 1]);

  /*for imx system has only 3 fb*/
 if(strcmp(fb_dev,"/dev/fb0") == 0)
 system("echo 0 > /sys/class/graphics/fb0/blank");
 if(strcmp(fb_dev,"/dev/fb1") == 0)
 system("echo 0 > /sys/class/graphics/fb1/blank");
 if(strcmp(fb_dev,"/dev/fb2") == 0)
 system("echo 0 > /sys/class/graphics/fb2/blank");

 fb_fd = open(fb_dev, O_RDWR);
 if(fb_fd == -1)
 {
   printf("frame buffer device setting is error /dev/fb%s\n", argv[argc - 1]);
   return -1;
 }
 while(pcn < argc - 1)
 {
  switch(stage)
  {
   case 0:
      if(strncmp(argv[1],"SET",3) == 0)
        m_op.op_tp = eTP_SET;
      else if(strncmp(argv[1],"GET",3) == 0)
        m_op.op_tp = eTP_GET;
      else if(strncmp(argv[1],"DRAW",4) == 0)
        m_op.op_tp = eTP_DRAW;
       else
         return -1;
      break;
    case 1:
      if(strncmp(argv[2],"ALPHA",5) == 0)
      {
        m_op.op_tn = eTN_ALPHA;
	m_op.operants = &m_alpha;
	m_alpha.pc = 1;
      }else if(strncmp(argv[2],"COLORKEY",8) == 0)
      {
        m_op.op_tn = eTN_COLORKEY;
        m_op.operants = &m_ck;
	m_ck.pc = 4;
      }else if(strncmp(argv[2],"PATTERN",7) == 0)
      {
        m_op.op_tn = eTN_PATTERN;
        m_op.operants = &m_dr;
	m_dr.pc = 1;
      }else
         return -1;
      pcn = 3;
      break;
     default:
       if(m_op.op_tp == eTP_GET)
         break;
       switch(m_op.op_tn)
       {
        case eTN_ALPHA:
	  ((sALPHA *)(m_op.operants))->value = atoi(argv[pcn]);
	  break;
	case eTN_COLORKEY:
	   if( pcn >= ((sCOLOR_KEY *)(m_op.operants))->pc + 3)
	   {
	     break;
	   }
	   /*skip the alpha*/
	  ((sCOLOR_KEY *)(m_op.operants))->uValue.value[pcn - 2] = atoi(argv[pcn]);
	  break;
	case eTN_PATTERN:
	  if(0 == strcmp(argv[pcn],"RED"))
	  ((sDRAW_PAT *)(m_op.operants))->value = ePT_RED;
	  else if(0 == strcmp(argv[pcn],"GREEN"))
	  ((sDRAW_PAT *)(m_op.operants))->value = ePT_GREEN;
	  else
	  ((sDRAW_PAT *)(m_op.operants))->value = ePT_BLUE;
	   break;
	 break;
	default:
	 return -1;
       }
       pcn++;
  }
  stage++;
 }
 return 0;
}

BOOL draw_op(void * pr)
{
 int ret = TRUE;
 sDRAW_PAT * mp = (sDRAW_PAT *)pr;
 unsigned char * fb_mem_ptr; 
 struct fb_fix_screeninfo fb_info;       
 /* Get constant fb info */
 if ((ioctl(fb_fd, FBIOGET_FSCREENINFO, &fb_info)) < 0)
 {
    tst_brkm(TFAIL, cleanup, "Cannot get framebuffer fixed parameters due to ioctl error: %s",strerror(errno));
  }
 /* Map fb device file into memory */
 fb_mem_ptr = mmap(NULL, fb_info.smem_len, PROT_READ | PROT_WRITE, MAP_SHARED, fb_fd, 0);
 if ((int)fb_mem_ptr == -1)
 {
        tst_brkm(TFAIL, cleanup, "Can't map framebuffer device into memory: %s\n", strerror(errno));
 }

 switch(mp->value)
 {
  case ePT_RED:
   ret = draw_pattern(fb_fd,fb_mem_ptr,255,0,0);
  break;
  case ePT_GREEN:
   ret = draw_pattern(fb_fd,fb_mem_ptr,0,255,0);
  break;
  default:
  case ePT_BLUE:
   ret = draw_pattern(fb_fd,fb_mem_ptr,0,0,255);
  break;
 } 
 munmap(fb_mem_ptr, fb_info.smem_len);
 if(TPASS == ret )
  return TRUE;
 else
 return FALSE;
}

BOOL alpha_op(void * pr)
{
 struct mxcfb_gbl_alpha gbl_alpha;
 sALPHA * mp = (sALPHA *)pr; 
 gbl_alpha.enable = 1;
 gbl_alpha.alpha = mp->value;
 printf("alpha value %d \n", mp->value);
 CALL_IOCTL(ioctl(fb_fd, MXCFB_SET_GBL_ALPHA, &gbl_alpha));
 return TRUE; 
}

BOOL ck_op(void * pr)
{
 struct mxcfb_color_key key;
 sCOLOR_KEY * mp = (sCOLOR_KEY *) pr; 
 key.enable = 1;
 key.color_key = ((mp->uValue.pixel.r)<<16)|((mp->uValue.pixel.g)<<8)|(mp->uValue.pixel.b);
 CALL_IOCTL(ioctl(fb_fd, MXCFB_SET_CLR_KEY, &key));
 return TRUE;
}


int main(int argc, char ** argv)
{
  int ret = 0;

   ret = parse_arg(argc, argv);
   if(0 != ret){
      printf("parameter error\n");
      help();
      goto ERROR;
    }
  
   if ( 0 == m_ops[m_op.op_tn]((void* )(m_op.operants)))
     goto ERROR;
   
   close(fb_fd);
   return 0;
ERROR:
   if(fb_fd != -1)
   close(fb_fd);
   return -1;
}

void cleanup()
{
  tst_exit();
}

void help()
{
 printf(
  "USAGE: \n imx_fb [OPS] [OPN] <Values> <fb number> \r\n  \
   OPS: SET / GET / DRAW \r\n          \
   OPN: ALPHA / COLORKEY / PATTERN \r\n   \
   Values:               \r\n   \
   for ALPHA 1 integer.  \r\n   \
   for COLORKEY 3 interger \r\n \
   for PATTERN RED/GREEN/BLUE \r\n \
   fb number:           \r\n    \
   0 / 1 / 2  \r\n   \
  "
 );
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
