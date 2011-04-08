/*
 * pxp_test - test application for the PxP DMA ENGINE lib
 *
 * Copyright (C) 2010 Freescale Semiconductor, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <string.h>

#include "pxp_lib.h"

#include "fsl_logo_480x360.h"

#define DBG_DEBUG		3
#define DBG_INFO		2
#define DBG_WARNING		1
#define DBG_ERR			0

static int debug_level = DBG_ERR;
#define dbg(flag, fmt, args...)	{ if(flag <= debug_level)  printf("%s:%d "fmt, __FILE__, __LINE__,##args); }

#define CONFIG_FPGA

/* there are three choices.
 * OUTPUT_DIRECT_TO_FB -- PxP outputs data directly to Framebuffer
 * OUTPUT_MEMCPY_TO_FB -- Copy data from PxP output buffer to Framebuffer
 * undefine BOTH       -- Save PxP output data into file
 * Note: Do not define _both_ !
 */
//#define OUTPUT_DIRECT_TO_FB	// FIXME: can't display on panel.
#define OUTPUT_MEMCPY_TO_FB

#if defined(OUTPUT_DIRECT_TO_FB) || defined(OUTPUT_MEMCPY_TO_FB)
#include <linux/mxcfb.h>

#define WAVEFORM_MODE_INIT	0x0	// Screen goes to white (clears)
#define WAVEFORM_MODE_DU	0x1	// Grey->white/grey->black
#define WAVEFORM_MODE_GC16	0x2	// High fidelity (flashing)
#define WAVEFORM_MODE_GC4	0x3	// Lower fidelity (texting, with potential for ghosting)
#endif

#define WIDTH	480
#define HEIGHT	360

/* Note: Must not more than 4, since we fetch the fixed phys mem on FPGA */
#define NR_THREAD	1

int fd_fb0 = 0;
unsigned short * fb0;
int g_fb0_size;

sigset_t sigset;
int quitflag;
unsigned int marker_val = 1;

struct test_pattern {
	int index;
	int rotate;
	int disp_left;
	int disp_top;
};
struct pthread_argument {
	struct test_pattern pattern;
	pthread_t tid;
};

static struct pthread_argument pthread_arg[NR_THREAD];

#if defined(OUTPUT_DIRECT_TO_FB) || defined(OUTPUT_MEMCPY_TO_FB)
static void copy_image_to_fb(int left, int top, int width, int height, uint *img_ptr, struct fb_var_screeninfo *screen_info)
{
	int i;
	uint *fb_ptr = (uint *)fb0;
	uint bytes_per_pixel;

	if ((width > screen_info->xres) || (height > screen_info->yres)) {
		dbg(DBG_ERR, "Bad image dimensions!\n");
		return;
	}

	bytes_per_pixel = screen_info->bits_per_pixel / 8;

	for (i = 0; i < height; i++) {
		memcpy(fb_ptr + ((i + top) * screen_info->xres + left) * bytes_per_pixel / 4,
			img_ptr + (i * width) * bytes_per_pixel /4,
			width * bytes_per_pixel);
	}
}

static void update_to_display(int left, int top, int width, int height, int wave_mode, int wait_for_complete)
{
	struct mxcfb_update_data upd_data;
	int retval;

	upd_data.update_mode = UPDATE_MODE_FULL;
	upd_data.waveform_mode = wave_mode;
	upd_data.update_region.left = left;
	upd_data.update_region.width = width;
	upd_data.update_region.top = top;
	upd_data.update_region.height = height;
	upd_data.temp = 24; //TEMP_USE_AMBIENT;

	if (wait_for_complete) {
		/* Get unique marker value */
		upd_data.update_marker = marker_val++;
	} else {
		upd_data.update_marker = 0;
	}

	retval = ioctl(fd_fb0, MXCFB_SEND_UPDATE, &upd_data);
	while (retval < 0 && quitflag == 0) {
		/* We have limited memory available for updates, so wait and
		 * then try again after some updates have completed */
		sleep(1);
		retval = ioctl(fd_fb0, MXCFB_SEND_UPDATE, &upd_data);
	}

//	if (wave_mode == WAVEFORM_MODE_AUTO)
//		dbg(DBG_INFO, "Waveform mode used = %d\n", upd_data.waveform_mode);

	if (wait_for_complete) {
		/* Wait for update to complete */
		retval = ioctl(fd_fb0, MXCFB_WAIT_FOR_UPDATE_COMPLETE, &upd_data.update_marker);
		if (retval < 0) {
			dbg(DBG_ERR, "Wait for update complete failed.  Error = 0x%x", retval);
		}
	}
}
#endif

int pxp_test(void *arg)
{
	int fd;
	struct pxp_config_data *pxp_conf = NULL;
	struct pxp_proc_data *proc_data = NULL;
	int ret = 0, i, n;
	char filename[128];
	struct test_pattern *pattern;
	struct pxp_mem_desc mem;
	struct pxp_mem_desc mem_o;
	pxp_chan_handle_t pxp_chan;
	int width, height;
#if defined(OUTPUT_DIRECT_TO_FB) || defined(OUTPUT_MEMCPY_TO_FB)
	struct fb_var_screeninfo var;
	struct fb_fix_screeninfo fix;
	char fb_device[] = "/dev/fb0";
#endif

	pattern = (struct test_pattern*)arg;

	dbg(DBG_DEBUG, "index = %d\n", pattern->index);
	dbg(DBG_DEBUG, "rotate = %d\n", pattern->rotate);

	ret = pxp_init();
	if (ret < 0) {
		dbg(DBG_ERR, "pxp init err\n");
		return -1;
	}

	ret = pxp_request_channel(&pxp_chan);
	if (ret < 0) {
		dbg(DBG_ERR, "pxp request channel err\n");
		goto err0;
	}
	dbg(DBG_INFO, "requested chan_id %d\n", pxp_chan.chan_id);
	/* Prepare the channel parameters */
	mem.size = WIDTH * HEIGHT * 2;
	mem_o.size = WIDTH * HEIGHT;
	ret = pxp_get_mem(&mem);
	if (ret < 0) {
		dbg(DBG_DEBUG, "get mem err\n");
		goto err1;
	}
	dbg(DBG_DEBUG, "mem.virt_uaddr %08x, mem.phys_addr %08x, mem.size %d\n",
				mem.virt_uaddr, mem.phys_addr, mem.size);
	ret = pxp_get_mem(&mem_o);
	if (ret < 0) {
		dbg(DBG_ERR, "get mem_o err\n");
		goto err2;
	}

	dbg(DBG_DEBUG, "mem_o.virt_uaddr %08x, mem_o.phys_addr %08x, mem_o.size %d\n",
				mem_o.virt_uaddr, mem_o.phys_addr, mem_o.size);

	for (i = 0; i < (WIDTH * HEIGHT * 2 / 4); i++) {
		*((unsigned int*)mem.virt_uaddr + i) = fb_480x360_2[i];
		if (i < 10)
			dbg(DBG_DEBUG, "[PxP In] 0x%08x 0x%08x\n",
				*((unsigned int *)mem.virt_uaddr + i),
				fb_480x360_2[i]);
	}

	/* Configure the channel */
	pxp_conf = malloc(sizeof (*pxp_conf));
	proc_data = &pxp_conf->proc_data;

	/* Initialize non-channel-specific PxP parameters */
	proc_data->srect.left = 0;
	proc_data->srect.top = 0;
	proc_data->drect.left = 0;
	proc_data->drect.top = 0;
	proc_data->srect.width = WIDTH;
	proc_data->srect.height = HEIGHT;
	proc_data->drect.width =  WIDTH;
	proc_data->drect.height = HEIGHT;
	proc_data->scaling = 0;
	proc_data->hflip = 0;
	proc_data->vflip = 0;
	proc_data->rotate = pattern->rotate;
	proc_data->bgcolor = 0;

	proc_data->overlay_state = 0;
	proc_data->lut_transform = PXP_LUT_NONE;//PXP_LUT_INVERT;

	/*
	 * Initialize S0 parameters
	 */
	pxp_conf->s0_param.pixel_fmt = PXP_PIX_FMT_RGB565;
	pxp_conf->s0_param.width = WIDTH;
	pxp_conf->s0_param.height = HEIGHT;
	pxp_conf->s0_param.color_key = -1;
	pxp_conf->s0_param.color_key_enable = false;
	pxp_conf->s0_param.paddr = mem.phys_addr;

	dbg(DBG_INFO, "pxp_test s0 paddr %08x\n", pxp_conf->s0_param.paddr);
	/*
	 * Initialize OL parameters
	 * No overlay will be used for PxP operation
	 */
	 for (i=0; i < 8; i++) {
		pxp_conf->ol_param[i].combine_enable = false;
		pxp_conf->ol_param[i].width = 0;
		pxp_conf->ol_param[i].height = 0;
		pxp_conf->ol_param[i].pixel_fmt = PXP_PIX_FMT_RGB565;
		pxp_conf->ol_param[i].color_key_enable = false;
		pxp_conf->ol_param[i].color_key = -1;
		pxp_conf->ol_param[i].global_alpha_enable = false;
		pxp_conf->ol_param[i].global_alpha = 0;
		pxp_conf->ol_param[i].local_alpha_enable = false;
	}

	/*
	 * Initialize Output channel parameters
	 * Output is Y-only greyscale
	 */
	pxp_conf->out_param.width = WIDTH;
	pxp_conf->out_param.height = HEIGHT;
	pxp_conf->out_param.pixel_fmt = PXP_PIX_FMT_GREY;
#if defined(OUTPUT_DIRECT_TO_FB) || defined(OUTPUT_MEMCPY_TO_FB)
	if ((fd_fb0 = open(fb_device, O_RDWR )) < 0) {
		dbg(DBG_ERR, "Unable to open frame buffer\n");
		goto err3;
	}
#endif
#ifdef	OUTPUT_DIRECT_TO_FB
	if (ioctl(fd_fb0, FBIOGET_FSCREENINFO, &fix) < 0) {
		dbg(DBG_ERR, "FBIOGET_FSCREENINFO error!\n");
		close(fd_fb0);
		goto err3;
	}
    #ifdef CONFIG_FPGA
	fix.smem_start = 0xB0000000;// FIXME
	pxp_conf->out_param.paddr = fix.smem_start - 0x70000000;
    #else
	pxp_conf->out_param.paddr = fix.smem_start;
    #endif
	dbg(DBG_DEBUG, "out addr (smem_start): 0x%08x\n", pxp_conf->out_param.paddr);
#else
	pxp_conf->out_param.paddr = mem_o.phys_addr;
#endif

	ret = pxp_config_channel(&pxp_chan, pxp_conf);
	if (ret < 0) {
		dbg(DBG_ERR, "pxp config channel err\n");
		goto err3;
	}

	ret = pxp_start_channel(&pxp_chan);
	if (ret < 0) {
		dbg(DBG_ERR, "pxp start channel err\n");
		goto err3;
	}

	ret = pxp_wait_for_completion(&pxp_chan, 3);
	if (ret < 0) {
		dbg(DBG_ERR, "pxp wait for completion err\n");
		goto err3;
	}

#if defined(OUTPUT_DIRECT_TO_FB) || defined(OUTPUT_MEMCPY_TO_FB)
	var.bits_per_pixel = 8;
	var.xres = 800;
	var.yres = 600;
	var.grayscale = GRAYSCALE_8BIT;
	var.yoffset = 0;
	var.rotate = FB_ROTATE_UR;
	var.activate = FB_ACTIVATE_FORCE;
	ret = ioctl(fd_fb0, FBIOPUT_VSCREENINFO, &var);
	if (ret < 0)
	{
		dbg(DBG_ERR, "FBIOPUT_VSCREENINFO error\n");
		goto err4;
	}

   #ifndef OUTPUT_DIRECT_TO_FB
	g_fb0_size = var.xres * var.yres * var.bits_per_pixel / 8;
	dbg(DBG_INFO, "g_fb0_size %d\n", g_fb0_size);

	/* Map the device to memory, */
	fb0 = (unsigned short *)mmap(0, g_fb0_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd_fb0, 0);
	if ((int)fb0 <= 0)
	{
		dbg(DBG_ERR, "\nError: failed to map framebuffer device 0 to memory.\n");
		goto err4;
	}
    #endif
#endif

#if !defined(OUTPUT_DIRECT_TO_FB) && !defined(OUTPUT_MEMCPY_TO_FB)
	sprintf(filename, "/pxp_test%d.yuv", pattern->index);
	fd = open(filename, O_RDWR|O_CREAT|O_TRUNC,
		S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
	if (fd < 0){
		dbg(DBG_ERR, "open file error.\n");
		ret = -1;
		goto err3;
	}

	n = write(fd, (void *)mem_o.virt_uaddr, mem_o.size);
	if (n != mem_o.size) {
		dbg(DBG_DEBUG, "n=%d\n", n);
		dbg(DBG_ERR, "write error\n");
		ret = -1;
		goto err4;
	}
#elif defined(OUTPUT_MEMCPY_TO_FB)
	if (pattern->rotate % 180) {
		width = HEIGHT;
		height = WIDTH;
	} else {
		width = WIDTH;
		height = HEIGHT;
	}
	copy_image_to_fb(pattern->disp_left, pattern->disp_top,
			 width, height, (void *)mem_o.virt_uaddr, &var);
#endif

#if defined(OUTPUT_DIRECT_TO_FB) || defined(OUTPUT_MEMCPY_TO_FB)
	dbg(DBG_INFO, "Update to display.\n");
	dbg(DBG_DEBUG, "w/h %d/%d\n", width ,height);
	update_to_display(pattern->disp_left, pattern->disp_top,
			  width, height, WAVEFORM_MODE_AUTO, true);
#endif

	dbg(DBG_INFO, "pxp_test FINISH test!!\n");
err4:
#if defined(OUTPUT_DIRECT_TO_FB) || defined(OUTPUT_MEMCPY_TO_FB)
	close(fd_fb0);
#else
	close(fd);
#endif
err3:
	pxp_put_mem(&mem_o);
err2:
	pxp_put_mem(&mem);
err1:
	free(pxp_conf);
	pxp_release_channel(&pxp_chan);
err0:
	pxp_uninit();

	return ret;
}

static int signal_thread(void *arg)
{
	int sig, err;

	pthread_sigmask(SIG_BLOCK, &sigset, NULL);

	while (1) {
		err = sigwait(&sigset, &sig);
		if (sig == SIGINT) {
			dbg(DBG_INFO, "Ctrl-C received\n");
		} else {
			dbg(DBG_ERR, "Unknown signal. Still exiting\n");
		}
		quitflag = 1;
		break;
	}

	return 0;
}

int main()
{
	pthread_t sigtid;
	int i;

	sigemptyset(&sigset);
	sigaddset(&sigset, SIGINT);
	pthread_sigmask(SIG_BLOCK, &sigset, NULL);
	pthread_create(&sigtid, NULL, (void *)&signal_thread, NULL);

	pthread_arg[0].pattern.index = 0;
	pthread_arg[0].pattern.rotate = 90;
	pthread_arg[0].pattern.disp_left = 0;
	pthread_arg[0].pattern.disp_top = 0;

	pthread_arg[1].pattern.index = 1;
	pthread_arg[1].pattern.rotate = 90;
	pthread_arg[1].pattern.disp_left = 400;
	pthread_arg[1].pattern.disp_top = 0;

	pthread_arg[2].pattern.index = 2;
	pthread_arg[2].pattern.rotate = 90;
	pthread_arg[2].pattern.disp_left = 100;
	pthread_arg[2].pattern.disp_top = 20;

	pthread_arg[3].pattern.index = 3;
	pthread_arg[3].pattern.rotate = 90;
	pthread_arg[3].pattern.disp_left = 300;
	pthread_arg[3].pattern.disp_top = 40;

	for (i = 0; i < NR_THREAD; i++) {
		pthread_create(&pthread_arg[i].tid, NULL,
			(void *)&pxp_test, (void *)&pthread_arg[i].pattern);
//		sleep(2);
	}
	for (i = 0; i < NR_THREAD; i++) {
		if (pthread_arg[i].tid != 0)
			pthread_join(pthread_arg[i].tid, NULL);
	}

	return 0;
}
