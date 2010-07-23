/***
**Copyright (C) 2010 Freescale Semiconductor, Inc. All Rights Reserved.
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
#include <unistd.h>
#include <sys/time.h>
#include <pthread.h>
#define _GNU_SOURCE
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>

/* Harness Specific Include Files. */
#include "test.h"

/* Verification Test Environment Include Files */
#include "epdc_test.h"
#ifdef USE_PIC
#include "soc_blk.c"
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
        tst_resm( TFAIL, "%s : %s fails #%d [File: %s, line: %d]", __FUNCTION__, "ioctl_", errno, __FILE__, __LINE__);\
        perror("ioctl"); \
        return FALSE;\
    }\
}

#define PXP_DEVICE_NAME "/dev/pxp_device"
#define BUFFER_WIDTH 256
#define BUFFER_HEIGHT 256
#define PXP_BUFFER_SIZE (BUFFER_WIDTH*BUFFER_HEIGHT)
#define MAX_WAIT 4
/*update interval in us*/
#define UPDATE_INTERVAL 1000
/*global virables*/
extern epdc_opts m_opt;

/*internal virables*/
int fb_fd; /* Framebuffer device file descriptor            */
struct fb_fix_screeninfo fb_info; /* Framebuffer constant information */
unsigned char  *fb_mem_ptr;   /* Pointer to the mapped momory of the fb device */
sigset_t sigset;
static int quitflag = 0;
unsigned long last_t = 0;
/*local functions*/
int get_modeinfo(struct fb_var_screeninfo *info);
void print_fbinfo(void);
unsigned char *draw_px(unsigned char *where, struct pixel *p);
int draw_pattern(int fd, unsigned char * pfb, int r, int g, int b );
BOOL pan_test();
BOOL draw_test();

inline unsigned char  BOUND255(short a)
{
	return (a > 255) ? 255: (a < 0)? 0: a;
}
/*
 * Draw test
 */
BOOL draw_test()
{
	int wait_time = 0;
	int update_marker = 0x101;
   struct fb_var_screeninfo mode_info;
  struct mxcfb_update_data im_update = {
  {0,0,16,16},/*region round to 8*/
  257,/*waveform mode 0-255, 257 auto*/
  0, /*update mode 0(partial),1(Full)*/
  update_marker,/*update_marker assigned by user*/
  TEMP_USE_AMBIENT,/*use ambient temperature set*/
  0,/*do not use alt buffer*/
  {0,0,0,{0,0,0,0}}
  };
	CALL_IOCTL(ioctl(fb_fd, FBIOGET_VSCREENINFO, &mode_info));
	im_update.update_region.width = mode_info.xres;
	im_update.update_region.height = mode_info.yres;
	tst_resm(TINFO, "draw a white in bg ground");
	if( TPASS != draw_pattern(fb_fd,fb_mem_ptr,255,255,255))
	{
		tst_resm(TINFO, "fail to draw patter on bg");
		return FALSE;
	}
	printf("updating the screen now\n");
	CALL_IOCTL(ioctl(fb_fd, MXCFB_SEND_UPDATE, &im_update));
	while(ioctl(fb_fd, MXCFB_WAIT_FOR_UPDATE_COMPLETE, &update_marker)< 0)
	{
		wait_time++;
		if(wait_time > MAX_WAIT)
		{
			printf("full mode wait time exceed!!!\n");
			return FALSE;
		}
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

 tst_resm(TINFO, "draw a white screen in back ground");
  if( TPASS != draw_pattern(fb_fd,fb_mem_ptr,255,255,255))
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
 /*reset pan postion*/
 mode_info.yoffset = 0;
 CALL_IOCTL(ioctl(fb_fd, FBIOPAN_DISPLAY, &mode_info));
 mode_info.yres_virtual = old_yvres;
 CALL_IOCTL(ioctl(fb_fd, FBIOPUT_VSCREENINFO, &mode_info));
#endif
 return TRUE;
}

static BOOL update_once(void * p_update)
{
 /*step 1: set up update data*/
  int  wait_time = 0;
  int count = 1;
  unsigned long st = 0i,et = 0;
  struct timeval tv;
  struct timezone tz;
  struct mxcfb_update_data *  p_im_update = (struct mxcfb_update_data *)p_update;
   /*do not use alt buffer*/
   p_im_update->use_alt_buffer = 0;
	 /*
	 printf("l = %d t= %d w = %d h = %d\n",
	 p_im_update->update_region.left,
	 p_im_update->update_region.top,
	 p_im_update->update_region.width,
	 p_im_update->update_region.height);
	 */
#if 0
  /*step 2: update and wait finished*/
  while(count--)
  {
	/*black and white alternative*/
  CALL_IOCTL(ioctl(fb_fd, MXCFB_SEND_UPDATE, p_im_update));
  while(ioctl(fb_fd, MXCFB_WAIT_FOR_UPDATE_COMPLETE, &(p_im_update->update_marker))< 0)
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
  }
#endif
  /*step 3: now using full update mode*/
  count = 1;
  p_im_update->update_mode = 1;
  gettimeofday(&tv, &tz);
  st = tv.tv_usec + tv.tv_sec * 1000000;
  while(count--)
  {
	/*black and white alternative*/
  CALL_IOCTL(ioctl(fb_fd, MXCFB_SEND_UPDATE, p_im_update));
  while(ioctl(fb_fd, MXCFB_WAIT_FOR_UPDATE_COMPLETE, &(p_im_update->update_marker))< 0)
  {
     wait_time++;
	if(wait_time > MAX_WAIT)
	{
	  printf("full mode wait time exceed!!!\n");
	  break;
	}
  }
  gettimeofday(&tv, &tz);
  et = tv.tv_usec + tv.tv_sec * 1000000;
  if(et > st)
		last_t += (et - st);
	else
		printf("WARNING!!! gettimeof day wrong");
  wait_time = 0;
  /*printf("next update\n");*/
  }
  return TRUE;
}

static BOOL single_update(void * p_update)
{
	/*step 1: set up update data*/
  int  wait_time = 0;
  int count = 10;
	pid_t tid = syscall(SYS_gettid);
  struct mxcfb_update_data *  p_im_update = (struct mxcfb_update_data *)p_update;
	/*do not use alt buffer*/
	p_im_update->use_alt_buffer = 0;
	printf("process %d runing at t= %d, l = %d, w= %d, h = %d\n",tid, p_im_update->update_region.top,p_im_update->update_region.left,
	p_im_update->update_region.width,p_im_update->update_region.height);

  /*step 2: update and wait finished*/
  while(count--)
  {
		/*black and white alternative*/
#if 1
		CALL_IOCTL(ioctl(fb_fd, MXCFB_SEND_UPDATE, p_im_update));
		while(ioctl(fb_fd, MXCFB_WAIT_FOR_UPDATE_COMPLETE, &(p_im_update->update_marker))< 0)
		{
			wait_time++;
			if(wait_time > MAX_WAIT)
			{
				printf("wait time exceed!!!\n");
				break;
			}
		}
#endif
		usleep(100);
		printf("process %d update\n",tid);
		if(quitflag)
			goto QUIT;
		wait_time = 0;
  }
  /*step 3: now using full update mode*/
  count = 10;
  p_im_update->update_mode = 1;
  while(count--)
  {
		/*black and white alternative*/
	#if 1
		CALL_IOCTL(ioctl(fb_fd, MXCFB_SEND_UPDATE, p_im_update));
		while(ioctl(fb_fd, MXCFB_WAIT_FOR_UPDATE_COMPLETE, &(p_im_update->update_marker))< 0)
		{
			wait_time++;
			if(wait_time > MAX_WAIT)
			{
				printf("full mode wait time exceed!!!\n");
				break;
			}
		}
	#endif
		usleep(100);
		printf("process %d full update\n",tid);
		if(quitflag)
			goto QUIT;
		wait_time = 0;
  }
QUIT:
	printf("process %d quit\n", tid);
  return TRUE;
}

static void draw_thread(int *pon)
{
	while(*pon == 1)
	{
		if(quitflag)break;
		draw_pattern(fb_fd,fb_mem_ptr,255,255,255);
		usleep(UPDATE_INTERVAL);
		draw_pattern(fb_fd,fb_mem_ptr,0,0,0);
		usleep(UPDATE_INTERVAL);
	}
	printf("draw quit\n");
}

static int signal_thread(void *arg)
{
     int sig, err;
     pthread_sigmask(SIG_BLOCK, &sigset, NULL);
     while (1) {
         err = sigwait(&sigset, &sig);
         if (sig == SIGINT) {
             printf("Ctrl-C received\n");
         } else {
             printf("Unknown signal. Still exiting\n");
         }
         quitflag = 1;
         break;
     }
     return 0;
}

BOOL test_rate_update()
{
	#define FRAME_CNT 1000
	struct mxcfb_update_data im_update[2];
	int i = 0;
	struct fb_var_screeninfo mode_info;
	CALL_IOCTL(ioctl(fb_fd, FBIOGET_VSCREENINFO, &mode_info));
	while(i++ < FRAME_CNT)
	{
    int x,y,w,h;
		x = 0;
		y = 0;
		w = (mode_info.xres>>1)&(~0x03);
		h = mode_info.yres;
		im_update[0].update_region.top = y;
		im_update[0].update_region.left = x;
		im_update[0].update_region.width = w;
		im_update[0].update_region.height = h;
		im_update[0].waveform_mode = 257;
		im_update[0].update_marker = 0x300;
		im_update[0].temp = 24;
		draw_pattern(fb_fd,fb_mem_ptr,255,255,255);
		update_once(&(im_update[0]));
		draw_pattern(fb_fd,fb_mem_ptr,128,128,128);
		x = (mode_info.xres>>1)&(~0x03);
		y = 0;
		w = (mode_info.xres>>1)&(~0x03);
		h = mode_info.yres;
		im_update[1].update_region.top = y;
		im_update[1].update_region.left = x;
		im_update[1].update_region.width = w;
		im_update[1].update_region.height = h;
		im_update[1].waveform_mode = 257;
		im_update[1].update_marker = 0x301;
		im_update[1].temp = 24;
		update_once(&(im_update[1]));
	}
    if(last_t > 0)
		printf("total update fps is:%f\n",(float)((FRAME_CNT*1000000.0f)/last_t));
    return TRUE;
}

BOOL test_max_update()
{
   int state = 1;
	 int ret = 0;
   int i = 0, j = 0;
	 int id= 0;
#define MAX_CNT_X 4
#define MAX_CNT_Y 4
   struct mxcfb_update_data im_update[MAX_CNT_X * MAX_CNT_Y];
   pthread_t sigtid,drawid,updates_id[MAX_CNT_X * MAX_CNT_Y];
   sigemptyset(&sigset);
   sigaddset(&sigset, SIGINT);
   pthread_sigmask(SIG_BLOCK, &sigset, NULL);
   pthread_create(&sigtid, NULL, (void *)&signal_thread, NULL);
   pthread_create(&drawid, NULL, (void *)&draw_thread, &state);
   if(state)
   {
		int cn = 0;
		struct fb_var_screeninfo mode_info;
		CALL_IOCTL(ioctl(fb_fd, FBIOGET_VSCREENINFO, &mode_info));
		memset(im_update, 0, sizeof(im_update));
		for(i = 0 ; i < MAX_CNT_X; i++)
			for(j = 0; j < MAX_CNT_Y; j++)
			{
			int x,y,w,h;
			x = i*(mode_info.xres/MAX_CNT_X);
			y = j*(mode_info.yres/MAX_CNT_Y);
			w = (mode_info.xres/MAX_CNT_X)&(~0x03);
			h = (mode_info.yres/MAX_CNT_Y)&(~0x03);
			im_update[cn].update_region.top = y;
			im_update[cn].update_region.left = x;
			im_update[cn].update_region.width = w;
			im_update[cn].update_region.height = h;
			im_update[cn].waveform_mode = 257;
			im_update[cn].update_marker = i + 0x200;
			im_update[cn].temp = 24;
			pthread_create(&(updates_id[id++]), NULL,
				(void *)&single_update, (void *)&(im_update[cn]));
			printf("carete pid %d\n", updates_id[i]);
			cn++;
			}
		for (i = 0; i < (MAX_CNT_X * MAX_CNT_Y); i++)
		{
			int pret = 0;
			if (updates_id[i] != 0)
				pthread_join(updates_id[i], (void **)&(pret));
			ret = ((BOOL)pret)?0:ret + 1;
			printf("%d return with %s\n", updates_id[i], ret == 0 ? "OK":"FAIL");
		}
		state = 0;
		//pthread_join(sigtid,NULL);
		pthread_join(drawid,NULL);
   }
   state = 0;
	 return ret == 0? TRUE: FALSE;
}

BOOL test_collision_update()
{
 BOOL ret = FALSE;
int fd_pxp;
int  wait_time = 0;
int i= 0 ,j = 0;
int count = 100;
int update_marker = 0x113;
struct pxp_mem_desc mem;
struct fb_var_screeninfo mode_info;
struct mxcfb_update_data im_update = {
  {0,0,BUFFER_WIDTH,BUFFER_HEIGHT},/*region round to 8*/
  257,/*waveform mode 0-255, 257 auto*/
  0, /*update mode 0(partial),1(Full)*/
  update_marker,/*update_marker assigned by user*/
  TEMP_USE_AMBIENT,/*use ambient temperature set*/
  1,/*enable alt buffer*/
  {0,0,0,{0,0,0,0}}/*set this later*/
  };

/*step 1: set up update data*/
	CALL_IOCTL(ioctl(fb_fd, FBIOGET_VSCREENINFO, &mode_info));
	fd_pxp = open(PXP_DEVICE_NAME, O_RDWR, 0);
	mem.size = m_opt.su == 1? (m_opt.update.alt_buffer_data.width * m_opt.update.alt_buffer_data.height) :PXP_BUFFER_SIZE;
	if (ioctl(fd_pxp, PXP_IOC_GET_PHYMEM, &mem) < 0)
	{
		mem.cpu_addr = 0;
		goto END;
	}
	#if 1
	{
		int fd_mem = open("/dev/mem",O_RDWR);
		if(fd_mem < 0)
		{
			printf("open mem device error\n");
			goto END;
		}
		mem.virt_uaddr = (unsigned long)mmap(NULL, mem.size, PROT_READ | PROT_WRITE,
				      MAP_SHARED, fd_mem, mem.phys_addr);/* + 0x70000000);*/
		close(fd_mem);
	}
#else
 mem.virt_uaddr = (unsigned long)mmap(NULL, mem.size, PROT_READ | PROT_WRITE,
				      MAP_SHARED, fd_pxp, mem.phys_addr);/* + 0x70000000);*/
#endif
	if(mem.virt_uaddr == 0)
	{
		printf("virtual address error!\n");
		goto END;
	}
	if(m_opt.su == 1)
	{
		memcpy(&im_update,&m_opt.update, sizeof(struct mxcfb_update_data));
	}else{
		im_update.alt_buffer_data.width = BUFFER_WIDTH;
		im_update.alt_buffer_data.height = BUFFER_HEIGHT;
		im_update.alt_buffer_data.alt_update_region.top = 0;
		im_update.alt_buffer_data.alt_update_region.left = 0;
		im_update.alt_buffer_data.alt_update_region.width =  BUFFER_WIDTH;
		im_update.alt_buffer_data.alt_update_region.height = BUFFER_HEIGHT;
 }
 im_update.alt_buffer_data.phys_addr = mem.phys_addr;
 for(i= 0; i < BUFFER_HEIGHT; i++)
	for(j = 0; j < BUFFER_WIDTH; j++)
	{
		((unsigned char *)(mem.virt_uaddr))[i*BUFFER_HEIGHT + j] = 128;
	}

/*step 2: start test*/
  /*partial update*/
  while(count--)
  {
	/*black and white alternative*/
	draw_pattern(fb_fd,fb_mem_ptr,255,255,255);
	CALL_IOCTL(ioctl(fb_fd, MXCFB_SEND_UPDATE, &im_update));
	while(ioctl(fb_fd, MXCFB_WAIT_FOR_UPDATE_COMPLETE, &update_marker)< 0)
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
	/*shift the update position a bit*/
	if(im_update.update_region.top + im_update.update_region.height < mode_info.yres - 5)
	im_update.update_region.top += 5;
	else
		im_update.update_region.top = 0;
	if(im_update.update_region.left + im_update.update_region.width < mode_info.yres - 5)
		im_update.update_region.left += 5;
	else
		im_update.update_region.left = 0;
	/*change the content*/
	for(i= 0; i < BUFFER_WIDTH; i++)
		for(j = 0; j < BUFFER_HEIGHT; j++)
		{
			((unsigned char *)mem.virt_uaddr)[i*BUFFER_HEIGHT + j] = random()&0xff;
		}
  }
  /*full update*/
  count = 100;
  im_update.update_mode = 1;
  while(count--)
  {
	/*black and white alternative*/
	draw_pattern(fb_fd,fb_mem_ptr,255,255,255);
	CALL_IOCTL(ioctl(fb_fd, MXCFB_SEND_UPDATE, &im_update));
	while(ioctl(fb_fd, MXCFB_WAIT_FOR_UPDATE_COMPLETE, &update_marker)< 0)
	{
		wait_time++;
		if(wait_time > MAX_WAIT)
		{
		printf("wait time exceed!!!\n");
		break;
		}
	}
	wait_time = 0;
	printf("full mode next update\n");
	/*shift the update position a bit*/
	if(im_update.update_region.top + im_update.update_region.height < mode_info.yres - 5)
	im_update.update_region.top += 5;
	else
	  im_update.update_region.top = 0;
	if(im_update.update_region.left + im_update.update_region.width < mode_info.yres - 5)
	im_update.update_region.left += 5;
	else
	  im_update.update_region.left = 0;
	/*change the content*/
	for(i= 0; i < BUFFER_WIDTH; i++)
		for(j = 0; j < BUFFER_HEIGHT; j++)
		{
			((unsigned char *)mem.virt_uaddr)[i*BUFFER_HEIGHT + j] = random()&0xff;
		}
  }

 /*step 4: clean up */
 if (ioctl(fd_pxp, PXP_IOC_PUT_PHYMEM, &mem) < 0)
 {
	mem.phys_addr = 0;
	mem.cpu_addr = 0;
	goto END;
 }
 ret = TRUE;
END:
 if(fd_pxp > 0)
	close(fd_pxp);
 return ret;
}

BOOL test_alt_update()
{
BOOL ret = FALSE;
int  wait_time = 0;
int fd_pxp;
int i= 0 ,j = 0;
int count = 100;
int update_marker = 0x112;
struct pxp_mem_desc mem;
struct mxcfb_update_data im_update = {
  {0,0,BUFFER_WIDTH,BUFFER_HEIGHT},/*region round to 8*/
  257,/*waveform mode 0-255, 257 auto*/
  0, /*update mode 0(partial),1(Full)*/
  update_marker,/*update_marker assigned by user*/
  TEMP_USE_AMBIENT,/*use ambient temperature set*/
  1,/*enable alt buffer*/
  {0,0,0,{0,0,0,0}}/*set this later*/
  };
 printf("start alt update test\n");
/*step 1: set up update data*/
 fd_pxp = open(PXP_DEVICE_NAME, O_RDWR, 0);
 if(fd_pxp < 0)
 {
	 printf("open pxp devices fialed\n");
	 return FALSE;
 }
 mem.size = m_opt.su == 1? (m_opt.update.alt_buffer_data.width * m_opt.update.alt_buffer_data.height) :PXP_BUFFER_SIZE;
 printf("try to get memory size %d\n",mem.size);
 if (ioctl(fd_pxp, PXP_IOC_GET_PHYMEM, &mem) < 0)
 {
	mem.phys_addr = 0;
	mem.cpu_addr = 0;
	printf("get memory failed");
	goto END;
 }
#if 1
{
	int fd_mem = open("/dev/mem",O_RDWR);
	if(fd_mem < 0)
	{
		printf("open mem device error\n");
		goto END;
	}
	mem.virt_uaddr = (unsigned long)mmap(NULL, mem.size, PROT_READ | PROT_WRITE,
				      MAP_SHARED, fd_mem, mem.phys_addr);/* + 0x70000000);*/
	close(fd_mem);
}
#else
 mem.virt_uaddr = (unsigned long)mmap(NULL, mem.size, PROT_READ | PROT_WRITE,
				      MAP_SHARED, fd_pxp, mem.phys_addr);/* + 0x70000000);*/
#endif
if(mem.virt_uaddr == 0)
 {
	 printf("virtual address error!\n");
	 goto END;
 }
 printf("success get memory at %x\n",mem.virt_uaddr);
 if(m_opt.su == 1)
 {
  memcpy(&im_update,&m_opt.update, sizeof(struct mxcfb_update_data));
 }else{
 im_update.alt_buffer_data.width = BUFFER_WIDTH;
 im_update.alt_buffer_data.height = BUFFER_HEIGHT;
 im_update.alt_buffer_data.alt_update_region.top = 0;
 im_update.alt_buffer_data.alt_update_region.left = 0;
 im_update.alt_buffer_data.alt_update_region.width =  BUFFER_WIDTH;
 im_update.alt_buffer_data.alt_update_region.height = BUFFER_HEIGHT;
 }
 im_update.alt_buffer_data.phys_addr = mem.phys_addr;
 for(i= 0; i < BUFFER_HEIGHT; i++)
	for(j = 0; j < BUFFER_WIDTH; j++)
	{
		((unsigned char*)mem.virt_uaddr)[i*BUFFER_HEIGHT + j] = 128;
	}
 printf("start test\n");
/*step 2: start test*/
  /*partial update*/
  while(count--)
  {
	/*black and white alternative*/
	if(count & 0x01)
		draw_pattern(fb_fd,fb_mem_ptr,255,255,255);
	else
		draw_pattern(fb_fd,fb_mem_ptr,0,0,0);
	CALL_IOCTL(ioctl(fb_fd, MXCFB_SEND_UPDATE, &im_update));
	while(ioctl(fb_fd, MXCFB_WAIT_FOR_UPDATE_COMPLETE, &update_marker)< 0)
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
  }
  /*full update*/
  count = 100;
  im_update.update_mode = 1;
  while(count--)
  {
	/*black and white alternative*/
	if(count & 0x01)
		draw_pattern(fb_fd,fb_mem_ptr,255,255,255);
	else
		draw_pattern(fb_fd,fb_mem_ptr,0,0,0);
	CALL_IOCTL(ioctl(fb_fd, MXCFB_SEND_UPDATE, &im_update));
	while(ioctl(fb_fd, MXCFB_WAIT_FOR_UPDATE_COMPLETE, &update_marker)< 0)
	{
		wait_time++;
		if(wait_time > MAX_WAIT)
		{
		printf("wait time exceed!!!\n");
		break;
		}
	}
	wait_time = 0;
	printf("full mode next update\n");
  }

 /*step 4: clean up */
 if (ioctl(fd_pxp, PXP_IOC_PUT_PHYMEM, &mem) < 0)
 {
	mem.phys_addr = 0;
	mem.cpu_addr = 0;
	goto END;
 }
 ret = TRUE;
END:
 if(fd_pxp > 0)
	close(fd_pxp);
 return ret;
}

BOOL full_update()
{
  /*step 1: set up update data*/
  int  wait_time = 0;
  int update_marker = 0x001;
  struct mxcfb_update_data im_update = {
  {0,0,16,16},/*region round to 8*/
  257,/*waveform mode 0-255, 257 auto*/
  0, /*update mode 0(partial),1(Full)*/
  update_marker,/*update_marker assigned by user*/
  TEMP_USE_AMBIENT,/*use ambient temperature set*/
  0,/*do not use alt buffer*/
  {0,0,0,{0,0,0,0}}
  };
  struct fb_var_screeninfo mode_info;
  CALL_IOCTL(ioctl(fb_fd, FBIOGET_VSCREENINFO, &mode_info));
  im_update.update_region.width = mode_info.xres;
  im_update.update_region.height = mode_info.yres;
  draw_pattern(fb_fd,fb_mem_ptr,255,255,255);
  if(m_opt.su == 1)
  {
   memcpy(&im_update,&m_opt.update, sizeof(struct mxcfb_update_data));
  }
   /*do not use alt buffer*/
   im_update.use_alt_buffer = 0;
  /*step 2: update and wait finished*/
  CALL_IOCTL(ioctl(fb_fd, MXCFB_SEND_UPDATE, &im_update));
  while(ioctl(fb_fd, MXCFB_WAIT_FOR_UPDATE_COMPLETE, &update_marker)< 0)
  {
     wait_time++;
	if(wait_time > MAX_WAIT)
	{
	  printf("wait time exceed!!!\n");
	  break;
	}
  }
  wait_time = 0;
	return TRUE;
}

BOOL test_wait_update()
{
/*suppose you have set up the device before run this case*/
	/*step 1: set up update data*/
  int count = 100;
  int  wait_time = 0;
  int update_marker = 0x111;
  struct mxcfb_update_data im_update = {
  {0,0,16,16},/*region round to 8*/
  257,/*waveform mode 0-255, 257 auto*/
  0, /*update mode 0(partial),1(Full)*/
  update_marker,/*update_marker assigned by user*/
  TEMP_USE_AMBIENT,/*use ambient temperature set*/
  0,/*do not use alt buffer*/
  {0,0,0,{0,0,0,0}}
  };
  if(m_opt.su == 1)
  {
   memcpy(&im_update,&m_opt.update, sizeof(struct mxcfb_update_data));
  }
   /*do not use alt buffer*/
   im_update.use_alt_buffer = 0;
  /*step 2: update and wait finished*/
  while(count--)
  {
	/*black and white alternative*/
	if(count & 0x01)
		draw_pattern(fb_fd,fb_mem_ptr,255,255,255);
	else
		draw_pattern(fb_fd,fb_mem_ptr,0,0,0);
  CALL_IOCTL(ioctl(fb_fd, MXCFB_SEND_UPDATE, &im_update));
  while(ioctl(fb_fd, MXCFB_WAIT_FOR_UPDATE_COMPLETE, &update_marker)< 0)
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
	if(count & 0x01)
		draw_pattern(fb_fd,fb_mem_ptr,255,255,255);
	else
		draw_pattern(fb_fd,fb_mem_ptr,0,0,0);
	CALL_IOCTL(ioctl(fb_fd, MXCFB_SEND_UPDATE, &im_update));
	while(ioctl(fb_fd, MXCFB_WAIT_FOR_UPDATE_COMPLETE, &update_marker)< 0)
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
    fb_fd = open(m_opt.dev, O_RDWR);
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
    /*keep system on*/
		{
			int tfd = open("/dev/tty0", O_RDWR);
			write(tfd, "\033[9;0]", 7);
			close(tfd);
		}

	CALL_IOCTL(ioctl(fb_fd, MXCFB_SET_WAVEFORM_MODES, &m_opt.waveform));

	if(m_opt.grayscale != -1)
	{
		struct fb_var_screeninfo mode_info;
		CALL_IOCTL(ioctl(fb_fd, FBIOGET_VSCREENINFO, &mode_info));
		mode_info.bits_per_pixel = 8;
		/*mode_info.grayscale = GRAYSCALE_8BIT;*/
		mode_info.rotate = m_opt.rot;
		mode_info.grayscale = m_opt.grayscale;
		mode_info.yoffset = 0;
		mode_info.activate = FB_ACTIVATE_FORCE;
		CALL_IOCTL(ioctl(fb_fd, FBIOPUT_VSCREENINFO, &mode_info));
		printf("set gray scale mode to %d\n", mode_info.grayscale);
	}
	if (m_opt.temp != -1)
	CALL_IOCTL(ioctl(fb_fd, MXCFB_SET_TEMPERATURE, &m_opt.waveform));
	if (m_opt.au != -1)
	CALL_IOCTL(ioctl(fb_fd, MXCFB_SET_AUTO_UPDATE_MODE, &m_opt.au));

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
 switch(m_opt.Tid)
 {
  case 0:
       tst_resm(TINFO, "normal test");
       if (!full_update())
       {
         rv = TFAIL;
         tst_resm(TFAIL, "normal test FAIL");
       }else
         tst_resm(TPASS, "normal test ok");

       break;
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
		if(!test_collision_update())
		{
          rv = TFAIL;
          tst_resm(TFAIL, "collision update FAIL");
		}else
          tst_resm(TPASS, "collision update ok");
		break;
  case 6:/*max update region count test */
		if(!test_max_update())
		{
          rv = TFAIL;
          tst_resm(TFAIL, "max update FAIL");
		}else
          tst_resm(TPASS, "max update ok");
		break;
  case 7:/*1000 frames sequence region no collision frame rate test*/
		if(!test_rate_update())
		{
          rv = TFAIL;
          tst_resm(TFAIL, "max update FAIL");
		}else
		break;
  default:
		break;
 }
 return rv;
}

#ifdef USE_PIC
int draw_pattern(int fd ,unsigned char * pfb, int r, int g, int b)
{
		struct pixel  px;
    int           size;
    int           i,j;
    unsigned char      *fb_wr_ptr = pfb;
		unsigned char * pdata;
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

    for(j = 0; j < px.yres; j++)
		{
			for(i = 0 ; i < px.xres; i++)
			{
				unsigned long l = j * gimp_image.width + i;
				if( i < gimp_image.width && j < gimp_image.height)
				{
					px.r_color = gimp_image.pixel_data[l * 3];
					px.g_color = gimp_image.pixel_data[l * 3 + 1];
					px.b_color = gimp_image.pixel_data[l * 3 + 2];
					fb_wr_ptr = draw_px(fb_wr_ptr, &px);
				}else{
					if(i %2 == 0)
					{
						px.r_color = 0;
						px.g_color = 0;
						px.b_color = 0; /* Set color values */
					}else{
						px.r_color = 255;
						px.g_color = 255;
						px.b_color = 255; /* Set color values */
					}
					fb_wr_ptr = draw_px(fb_wr_ptr, &px);
				}
			}
		}
	/* Restore activation flag */
    mode_info.activate = act_mode;
    ioctl(fd, FBIOPUT_VSCREENINFO, &mode_info);
    return rv;

}
#else
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
			fb_wr_ptr = draw_px(fb_wr_ptr, &px);
		}
	/* Restore activation flag */
	#if 1
    mode_info.activate = act_mode;
    ioctl(fd, FBIOPUT_VSCREENINFO, &mode_info);
    #endif
    return rv;
}
#endif
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
	if(p->r_field.offset == 0 &&  p->g_field.offset == 0 && p->b_field.offset == 0)
	{
		/*gray scale image*/
		 #define PR (unsigned char)(0.257*256)
		 #define PG (unsigned char)(0.504*256)
		 #define PB (unsigned char)(0.098*256)
		 unsigned char r = p->r_color;
		 unsigned char g = p->g_color;
		 unsigned char b = p->b_color;
		 value = (unsigned char)BOUND255(((PR * r + PG * g + PB * b)>>8) + 16);
	}else{
	value = (p->r_color * (1 << p->r_field.length) / (1 << 8) ) << p->r_field.offset;
	value |= (p->g_color * (1 << p->g_field.length) / (1 << 8) ) << p->g_field.offset;
	value |= (p->b_color * (1 << p->b_field.length) / (1 << 8) ) << p->b_field.offset;
	}
	switch (p->bpp * 8)
	{
		case 8:
			/*fix me*/
			*where++ = value;
			break;
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
