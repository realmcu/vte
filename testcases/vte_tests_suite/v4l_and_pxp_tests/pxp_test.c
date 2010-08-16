/*
 * pxp_test - test application for the MXS PxP
 *
 * Copyright (C) 2009-2010 Freescale Semiconductor, Inc.
 * Copyright 2008-2009 Embedded Alley Solutions
 * Matt Porter <mporter@embeddedalley.com>
 * 
 * This program is free software, see the COPYING file
 */

#include <errno.h>
#include <fcntl.h>
#include <getopt.h>
#include <stdio.h>  
#include <stdlib.h> 
#include <string.h>
#include <unistd.h>

#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/ioctl.h>

#include <asm/types.h>

#include <linux/fb.h>
#include <linux/videodev2.h>

#define DEFAULT_WIDTH	320
#define DEFAULT_HEIGHT	240

#define	V4L2_BUF_NUM	2

int g_num_buffers;

struct buffer {
	void *start;
	struct v4l2_buffer buf;
};

struct pxp_control {
	int fmt_idx;
	char *vdevfile;
	int vfd;
	char *s0_infile;
	char *s1_infile;
	unsigned int out_addr;
	char *outfile;
	int outfile_state;
	struct buffer *buffers;
	int global_alpha;
	int global_alpha_val;
	int colorkey;
	unsigned int colorkey_val;
	int hflip;
	int vflip;
	int rotate;
	int rotate_pass;
	struct v4l2_rect s0;
	int dst_state;
	struct v4l2_rect dst;
	int wait;
	int screen_w, screen_h;
	int en_ov_crop;
};

struct pxp_video_format
{
	char *name;
	unsigned int bpp;
	unsigned int bpp_demon;
	unsigned int fourcc;
	enum v4l2_colorspace colorspace;
};

static struct pxp_video_format pxp_video_formats[] = {
	{
		.name = "24-bit RGB",
		.bpp = 4,
		.bpp_demon = 1,
		.fourcc = V4L2_PIX_FMT_RGB24,
		.colorspace = V4L2_COLORSPACE_SRGB,
	}, {
		.name = "16-bit RGB 5:6:5",
		.bpp = 2,
		.bpp_demon = 1,
		.fourcc = V4L2_PIX_FMT_RGB565,
		.colorspace = V4L2_COLORSPACE_SRGB,
	}, {
		.name = "16-bit RGB 5:5:5",
		.bpp = 2,
		.bpp_demon = 1,
		.fourcc = V4L2_PIX_FMT_RGB555,
		.colorspace = V4L2_COLORSPACE_SRGB,
	}, {
		.name = "YUV 4:2:0 Planar",
		.bpp = 3,
		.bpp_demon = 2,
		.fourcc = V4L2_PIX_FMT_YUV420,
		.colorspace = V4L2_COLORSPACE_JPEG,
	}, {
		.name = "YUV 4:2:2 Planar",
		.bpp = 2,
		.bpp_demon = 1,
		.fourcc = V4L2_PIX_FMT_YUV422P,
		.colorspace = V4L2_COLORSPACE_JPEG,
	},
};

#define VERSION	"1.0"
#define MAX_LEN 512
#define DEFAULT_OUTFILE "out.pxp"
#define DEFAULT_V4L_DEVICE "/dev/video0"

#define PXP_RES		0
#define PXP_DST		1
#define PXP_HFLIP	2
#define PXP_VFLIP	3
#define PXP_WIDTH	4
#define PXP_HEIGHT	5


static void usage(char *bin)
{
	printf("Usage: %s [-a <n>] [-k 0xHHHHHHHH] [-o <outfile>] [-sx <width>] [-sy <height>] [-hf] [-vf] [-r <D>] [-res <x>:<y>] [-w <n>] [-dst ...] <s0_in> <s1_in>\n", bin);
}

static void help(char *bin)
{
	printf("pxp_qa - PxP QA test, v%s\n", VERSION);
     
	usage(bin);
    
	printf("\nPossible options:\n");
	printf("\t-a n\t\tset global alpha\n");
	printf("\t-h    \tprint help information\n");
	printf("\t-hf   \tflip image horizontally\n");
	printf("\t-k 0xHHHHHHHH   \tSet colorkey\n");
	printf("\t-dst <x>:<y>:<w>:<h>  \tset destination window\n");
	printf("\t-sx <width> \twidth of the LCD screen\n");
	printf("\t-sy <height> \theight of the LCD screen\n");
	printf("\t-o <outfile>  \tset outfile for virtual buffer\n");
	printf("\t-r   \trotate image\n");
	printf("\t-res <w>:<h>  \tinput resolution\n");
	printf("\t-vf   \tflip image vertically\n");
	printf("\t-w n   \twait n seconds before exiting\n");
	printf("\t-c    \tcorp input\n");
	printf("\t-?    \tprint this usage information\n");
}

static struct pxp_control *pxp_init(int argc, char **argv)
{
	struct pxp_control *pxp;
	int opt;
	int long_index = 0;
	int tfd;
	char buf[8];

	/* Disable screen blanking */
	tfd = open("/dev/tty0", O_RDWR);
	sprintf(buf, "%s", "\033[9;0]");
	write(tfd, buf, 7);
	close(tfd);

	pxp = calloc(1, sizeof(struct pxp_control));
	if (!pxp) {
		perror("failed to allocate PxP control object");
		return NULL;
	}

	pxp->buffers = calloc(V4L2_BUF_NUM, sizeof(*pxp->buffers));

	if (!pxp->buffers) {
		perror("insufficient buffer memory");
		return NULL;
	}

	/* Init pxp control struct */
	pxp->s0_infile = argv[argc-2];
	pxp->s1_infile = strcmp(argv[argc-1],"BLANK")==0?NULL:argv[argc-1];
	pxp->outfile = calloc(1, MAX_LEN);
	pxp->outfile_state = 0;
	pxp->vdevfile = calloc(1, MAX_LEN);
	strcpy(pxp->outfile, DEFAULT_OUTFILE);
	strcpy(pxp->vdevfile, DEFAULT_V4L_DEVICE);
	pxp->screen_w = pxp->s0.width = DEFAULT_WIDTH;
	pxp->screen_h = pxp->s0.height = DEFAULT_HEIGHT;
	pxp->fmt_idx = 4;	/* YUV422 */
	pxp->wait = 1;

	static const char *opt_string = "a:hk:o:ir:w:f:c?";

	static const struct option long_opts[] = {
		{ "dst", required_argument, NULL, PXP_DST},
		{ "hf", no_argument, NULL, PXP_HFLIP},
		{ "res", required_argument, NULL, PXP_RES},
		{ "vf", no_argument, NULL, PXP_VFLIP},
		{ "sx", required_argument, NULL, PXP_WIDTH},
		{ "sy", required_argument, NULL, PXP_HEIGHT},
		{ NULL, no_argument, NULL, 0 }
	};

	for (;;) {
		opt = getopt_long_only(argc, argv, opt_string,
					long_opts, &long_index);
		if (opt == -1)
			break;
		switch(opt) {
			case PXP_WIDTH:
				pxp->screen_w = pxp->s0.width = atoi(optarg);
				break;
			case PXP_HEIGHT:
				pxp->screen_h = pxp->s0.height = atoi(optarg);
				break;
			case PXP_RES:
				pxp->s0.width = atoi(strtok(optarg, ":"));
				pxp->s0.height = atoi(strtok(NULL, ":"));
				break;
			case PXP_DST:
				pxp->dst_state = 1;
				pxp->dst.left = atoi(strtok(optarg, ":"));
				pxp->dst.top = atoi(strtok(NULL, ":"));
				pxp->dst.width = atoi(strtok(NULL, ":"));
				pxp->dst.height = atoi(strtok(NULL, ":"));
				break;
			case PXP_HFLIP:
				pxp->hflip = 1;
				break;
			case PXP_VFLIP:
				pxp->vflip = 1;
				break;
			case 'a':
				pxp->global_alpha = 1;
				pxp->global_alpha_val = atoi(optarg);
				break;
			case 'h':
				usage(argv[0]);
				goto error;
			case 'k':
				pxp->colorkey = 1;
				pxp->colorkey_val = strtoul(optarg, NULL, 16);
				break;
			case 'o':
				pxp->outfile_state = 1;
				strncpy(pxp->outfile, optarg, MAX_LEN);
				break;
			case 'r':
				pxp->rotate = atoi(optarg);
				if ((pxp->rotate == 90) || (pxp->rotate == 270))
					pxp->rotate_pass = 1; 
				break;
			case 'w':
				pxp->wait = atoi(optarg);
				break;
			case 'c':
			        pxp->en_ov_crop = 1;
			       break;
			case 'f':
			        pxp->fmt_idx = atoi(optarg);
			       break;
			case '?':
				help(argv[0]);
				goto error;
			default:
				usage(argv[0]);
		}
	}

	if ((optind == argc) || (2 != argc-optind)) {
		usage(argv[0]);
		goto error;
	}

	if ((pxp->rotate != 0) && (pxp->rotate != 90) &&
	    (pxp->rotate != 180) && (pxp->rotate != 270)) {
		printf("Rotation must be 0, 90, 180, or 270 degrees\n");
		goto error;
	}

	return pxp;

error:
	if (pxp)
		free(pxp);
	return NULL;
}

static int pxp_check_capabilities(struct pxp_control *pxp)
{
        struct v4l2_capability cap;

        if (ioctl(pxp->vfd, VIDIOC_QUERYCAP, &cap) < 0) {
		perror("VIDIOC_QUERYCAP");
		return 1;
        }

	if (!(cap.capabilities &
		(V4L2_CAP_VIDEO_OUTPUT | V4L2_CAP_VIDEO_OUTPUT_OVERLAY))) {
		perror("video output overlay not detected\n");
		return 1;
	}

	if (!(cap.capabilities & V4L2_CAP_STREAMING)) {
		perror("streaming support not detected\n");
		return 1;
	}

	return 0;
}

static int pxp_config_output(struct pxp_control *pxp, int pass)
{
	struct v4l2_output output;
	struct v4l2_format format;
	int out_idx = 0;

	if (!pass)
		out_idx = 1;

	if (ioctl(pxp->vfd, VIDIOC_S_OUTPUT, &out_idx) < 0) {
		perror("failed to set output");
		return 1;
	}

	output.index = out_idx;

	if (ioctl(pxp->vfd, VIDIOC_ENUMOUTPUT, &output) >= 0) {
		pxp->out_addr = output.reserved[0];
		printf ("V4L output %d (0x%08x): %s\n",
			output.index, output.reserved[0], output.name);
	} else {
		perror("VIDIOC_ENUMOUTPUT");
		return 1;
	}

	format.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
	format.fmt.pix.width = pxp->s0.width;
	format.fmt.pix.height = pxp->s0.height;
	format.fmt.pix.pixelformat = pxp_video_formats[pxp->fmt_idx].fourcc;
	if (ioctl(pxp->vfd, VIDIOC_S_FMT, &format) < 0) {
		perror("VIDIOC_S_FMT output");
		return 1;
	}

	printf ("Video input format: %dx%d %s\n", pxp->s0.width, pxp->s0.height,
		pxp_video_formats[pxp->fmt_idx].name);

	return 0;
}

static int pxp_config_buffer(struct pxp_control *pxp, int b)
{
	struct v4l2_requestbuffers req;
        int ibcnt = V4L2_BUF_NUM;
	int i =0;
	/*
	 * Hardcoded to two buffers for now
	 */
	req.count = ibcnt; /*enlarge the buffer*/
	req.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
	req.memory = V4L2_MEMORY_MMAP;

	if (ioctl(pxp->vfd, VIDIOC_REQBUFS, &req) < 0) {
		perror("VIDIOC_REQBUFS");
		return 1;
	}

	if (req.count < ibcnt) {
		perror("insufficient buffer control memory");
		return 1;
	}
        
	/*add more buffer prepared code*/
	for(i = 0; i < ibcnt ; i++) 
	{
	pxp->buffers[i].buf.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
	pxp->buffers[i].buf.memory = V4L2_MEMORY_MMAP;
	pxp->buffers[i].buf.index = i;

	if (ioctl(pxp->vfd, VIDIOC_QUERYBUF, &pxp->buffers[i].buf) < 0) {
		perror("VIDIOC_QUERYBUF");
		return 1;
	   }   

	pxp->buffers[i].start =
		mmap(NULL /* start anywhere */,
				pxp->buffers[i].buf.length,
				PROT_READ | PROT_WRITE,
				MAP_SHARED,
				pxp->vfd,
				pxp->buffers[i].buf.m.offset);

	if (pxp->buffers[i].start == MAP_FAILED) {
		perror("failed to mmap pxp buffer");
		return 1;
	   }
    }
	return 0;
}

static int pxp_read_infiles(struct pxp_control *pxp)
{
	int fd; 
	int bpp = pxp_video_formats[pxp->fmt_idx].bpp;
	int s0_size = pxp->s0.width * pxp->s0.height * bpp / pxp_video_formats[pxp->fmt_idx].bpp_demon;
	int ffd;
	struct fb_var_screeninfo var;
	int fb_size;
	char *fb;
	int n;
	if ((fd = open(pxp->s0_infile, O_RDWR, 0)) < 0) {
		perror("s0 data open failed");
		return 1;
	}

	n = read(fd, pxp->buffers[0].start, s0_size);
	if (n != s0_size) {
		perror("error reading s0 data");
		close(fd);
		return 1;
	}

	close(fd);
	if ((ffd = open("/dev/fb0", O_RDWR, 0)) < 0) {
		perror("fb device open failed");
		return 1;
	}

	if (ioctl(ffd, FBIOGET_VSCREENINFO, &var)) {
		perror("FBIOGET_VSCREENINFO");
		return 1;
	}

	fb_size = var.xres * var.yres * (var.bits_per_pixel >> 3);

	fb = mmap(NULL /* start anywhere */,
			fb_size,
			PROT_WRITE,
			MAP_SHARED,
			ffd,
			0);

	if (fb == MAP_FAILED) {
		perror("failed to mmap output buffer");
		return 1;
	}
        /* Hake may not use sencodary input*/
	if (pxp->s1_infile){
	 if ((fd = open(pxp->s1_infile, O_RDWR, 0)) < 0) {
		perror("s1 data open failed");
		return 1;
	 }

	 n = read(fd, fb, fb_size);
	 if (n != (fb_size)) {
		perror("error reading s1 data into fb");
		close(fd);
		return 1;
	 } 

	 close(fd);
	 }
	close(ffd);

	return 0;
}

static int pxp_config_windows(struct pxp_control *pxp, int pass)
{
	struct v4l2_framebuffer fb;
	struct v4l2_format format;
	struct v4l2_crop crop;

	/* Set FB overlay options */
	if (!pass)
		fb.flags = V4L2_FBUF_FLAG_OVERLAY;
	else
		fb.flags = 0;
	if (pxp->global_alpha)
		fb.flags |= V4L2_FBUF_FLAG_GLOBAL_ALPHA;
	if (pxp->colorkey)
		fb.flags |= V4L2_FBUF_FLAG_CHROMAKEY;
	if (ioctl(pxp->vfd, VIDIOC_S_FBUF, &fb) < 0) {
		perror("VIDIOC_S_FBUF");
		return 1;
	}

	/* Set overlay source window */
	memset(&format, 0, sizeof(struct v4l2_format));
	format.type = V4L2_BUF_TYPE_VIDEO_OUTPUT_OVERLAY;
	format.fmt.win.global_alpha = pxp->global_alpha_val;
	format.fmt.win.chromakey = pxp->colorkey_val;
	if (pxp->en_ov_crop)
	{
	/*codec need 16 pixels offset*/
	printf("crop offset set for output overlay\n");
	format.fmt.win.w.left = 16;
	format.fmt.win.w.top = 16;
	format.fmt.win.w.width = pxp->s0.width - 32;
	format.fmt.win.w.height = pxp->s0.height - 32;
	}else{
	format.fmt.win.w.left = 0;
	format.fmt.win.w.top = 0;
	format.fmt.win.w.width = pxp->s0.width;
	format.fmt.win.w.height = pxp->s0.height;
	}
	if (ioctl(pxp->vfd, VIDIOC_S_FMT, &format) < 0) {
		perror("VIDIOC_S_FMT output overlay");
		return 1;
	}

	/* Set cropping window */
	crop.type = V4L2_BUF_TYPE_VIDEO_OUTPUT_OVERLAY;
	if (!pass) {
		if (pxp->dst_state) {
			crop.c.left = pxp->dst.left;
			crop.c.top = pxp->dst.top;
			crop.c.width = pxp->dst.width;
			crop.c.height = pxp->dst.height;
		}else{
		crop.c.left = 0;
		crop.c.top = 0;
		crop.c.width = pxp->s0.width;
		crop.c.height = pxp->s0.height;
		}
	} else {
		if (pxp->dst_state) {
			crop.c.left = pxp->dst.left;
			crop.c.top = pxp->dst.top;
			crop.c.width = pxp->dst.width;
			crop.c.height = pxp->dst.height;
		} else {
			if (pxp->rotate_pass) {
				int scale = 16 * pxp->screen_h / pxp->screen_w;
				crop.c.left = 0;
				crop.c.top = 0;
				crop.c.width = pxp->screen_w * scale/16;
				crop.c.height = pxp->screen_h * scale/16;
			} else {
				crop.c.left = 0;
				crop.c.top = 0;
				crop.c.width = pxp->s0.width;
				crop.c.height = pxp->s0.height;
			}
		}
	}
	printf("crop.c.l/t/w/h = %d/%d/%d/%d\n", crop.c.left,
	       crop.c.top, crop.c.width, crop.c.height);
	if (ioctl(pxp->vfd, VIDIOC_S_CROP, &crop) < 0) {
		perror("VIDIOC_S_CROP");
		return 1;
	}

	return 0;
}

static int pxp_config_controls(struct pxp_control *pxp)
{
	struct v4l2_control vc;

	/* Horizontal flip */
	if (pxp->hflip)
		vc.value = 1;
	else
		vc.value = 0;
	vc.id = V4L2_CID_HFLIP;
	if (ioctl(pxp->vfd, VIDIOC_S_CTRL, &vc) < 0) {
		perror("VIDIOC_S_CTRL");
		return 1;
	}

	/* Vertical flip */
	if (pxp->vflip) 
		vc.value = 1;
	else
		vc.value = 0;
	vc.id = V4L2_CID_VFLIP;
	if (ioctl(pxp->vfd, VIDIOC_S_CTRL, &vc) < 0) {
		perror("VIDIOC_S_CTRL");
		return 1;
	}

	/* Rotation */
	vc.id = V4L2_CID_PRIVATE_BASE;
	vc.value = pxp->rotate;
	if (ioctl(pxp->vfd, VIDIOC_S_CTRL, &vc) < 0) {
		perror("VIDIOC_S_CTRL");
		return 1;
	}
	/* Set background color */
	vc.id = V4L2_CID_PRIVATE_BASE + 1;
	vc.value = 0x0;
	if (ioctl(pxp->vfd, VIDIOC_S_CTRL, &vc) < 0) {
		perror("VIDIOC_S_CTRL");
		return 1;
	}

	return 0;
}

static int pxp_start(struct pxp_control *pxp, int pass)
{
	enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
        int i = 0, cnt = 0;
	int fd;
	int bpp = pxp_video_formats[pxp->fmt_idx].bpp;
	int s0_size = pxp->s0.width * pxp->s0.height * bpp / pxp_video_formats[pxp->fmt_idx].bpp_demon;
	/* Queue buffer */
	/*Hake add more buffer support*/
	if ((fd = open(pxp->s0_infile, O_RDWR, 0)) < 0) {
		perror("s0 data open failed");
		return 1;
	}
	printf("prepare buffers\n");
        #if 1
	for(i = 0; i < V4L2_BUF_NUM; i++){
        /*prepare 2 buffers */
        struct v4l2_buffer buf;
	buf.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = i;
	if (ioctl(pxp->vfd, VIDIOC_QUERYBUF, &buf) < 0) {
	  printf("VIDIOC_QUERYBUF failed\n");
	  break;
	}
	pxp->buffers[i].buf.index = i;
	if (ioctl(pxp->vfd, VIDIOC_QBUF, &pxp->buffers[i].buf) < 0) {
		perror("VIDIOC_QBUF 1");
		return 1;
	   }
	}
	#endif
	/* Enable PxP */
        if (ioctl(pxp->vfd, VIDIOC_STREAMON, &type) < 0) {
               perror("VIDIOC_STREAMON");
               return 1;
        }
	printf("PxP processing: start...");
	do
	{
	struct v4l2_buffer buf;
	buf.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
	buf.memory = V4L2_MEMORY_MMAP;
	if (ioctl(pxp->vfd, VIDIOC_DQBUF, &buf) < 0) {
		perror("VIDIOC_DQBUF");
		return 1;
	}
	/*
	 printf("\ndisplay buff %d\n",buf.index);
	 */
	cnt = read(fd, pxp->buffers[buf.index].start, s0_size);
	if(cnt < s0_size)
	   break;
	printf("\nread cnt=%d (%d), at buf %d\n",cnt, s0_size, buf.index);
        pxp->buffers[buf.index].buf.index = buf.index;
	if (ioctl(pxp->vfd, VIDIOC_QBUF, &pxp->buffers[buf.index].buf) < 0) {
		perror("VIDIOC_QBUF 2");
		return 1;
	    }
	}while(cnt == s0_size );


        close(fd);
	return 0;
}

static int pxp_stop(struct pxp_control *pxp, int pass)
{
	enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_OUTPUT;

        #if 0
	/*Hake no use here*/
	/* Query buffer */
	if (ioctl(pxp->vfd, VIDIOC_DQBUF, &pxp->buffers[pass].buf) < 0) {
		perror("VIDIOC_DQBUF");
		return 1;
	}
        #endif
	printf("complete\n");

	if (((pxp->rotate_pass) && pass) || !pxp->rotate_pass)
		sleep(pxp->wait);

	/* Disable PxP */
        if (ioctl(pxp->vfd, VIDIOC_STREAMOFF, &type) < 0) {
               perror("VIDIOC_STREAMOFF");
               return 1;
        }

	if (!pass)
	{
	 int i = 0;
	 for(i = 0; i < V4L2_BUF_NUM ; i++)
          munmap(pxp->buffers[i].start, pxp->buffers[i].buf.length);
        }
	printf("pxp stopped\n");
	return 0;
}

static int pxp_write_outfile(struct pxp_control *pxp)
{
	int fd, ffd, mfd;
	struct fb_var_screeninfo var;
	char *out_buf;
	size_t out_buf_size;
	int n;
	char fmt_buf[32];

	if ((ffd = open("/dev/fb0", O_RDWR, 0)) < 0) {
		perror("fb device open failed");
		return 1;
	}

	if (ioctl(ffd, FBIOGET_VSCREENINFO, &var)) {
		perror("FBIOGET_VSCREENINFO");
		return 1;
	}

	out_buf_size = var.xres * var.yres * (var.bits_per_pixel >> 3);

	close(ffd);

	if ((mfd = open("/dev/mem", O_RDWR, 0)) < 0) {
		perror("mem device open failed");
		return 1;
	}

	out_buf = mmap(NULL /* start anywhere */,
			out_buf_size,
			PROT_READ,
			MAP_SHARED,
			mfd,
			pxp->out_addr);

	if (out_buf == MAP_FAILED) {
		perror("failed to mmap output buffer");
		return 1;
	}

	if ((fd = open(pxp->outfile,
			O_CREAT|O_WRONLY,
			S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH)) < 0) {
		perror("outfile open failed");
		return 1;
	}

	n = write(fd, out_buf, out_buf_size);
	if (n != out_buf_size) {
		perror("failed to write entire output buffer");
		return 1;
	}

	close(fd);

	if (var.bits_per_pixel == 16)
		strcpy(fmt_buf, "RGB565");
	else
		strcpy(fmt_buf, "RGB24 (32-bit unpacked)");

	printf("Virtual output buffer: %dx%d %s\n",
		var.xres, var.yres, fmt_buf);

	return 0;
}

static int pxp_rotate_pass(struct pxp_control *pxp)
{
	int mfd;
	char *out_buf;
	int w;
	int bpp;
	int s0_size;
 
 	bpp = pxp_video_formats[pxp->fmt_idx].bpp;
	s0_size = pxp->s0.width * pxp->s0.height * bpp / pxp_video_formats[pxp->fmt_idx].bpp_demon;

	if (pxp_config_output(pxp, 1))
		return 1;

	if (pxp_config_buffer(pxp, 1))
		return 1;

	if ((mfd = open("/dev/mem", O_RDWR, 0)) < 0) {
		perror("mem device open failed");
		return 1;
	}

	out_buf = mmap(NULL /* start anywhere */,
			s0_size,
			PROT_READ,
			MAP_SHARED,
			mfd,
			pxp->out_addr);

	if (out_buf == MAP_FAILED) {
		perror("failed to mmap output buffer");
		return 1;
	}

	memcpy(pxp->buffers[1].start, out_buf, s0_size);

	if (pxp_config_windows(pxp, 1))
		return 1;

	if (pxp_config_controls(pxp))
		return 1;

	if (pxp_start(pxp, 1))
		return 1;

	if (pxp_stop(pxp, 1))
		return 1;

	close(mfd);

	return 0;
}

static void pxp_cleanup(struct pxp_control *pxp)
{
	close(pxp->vfd);
	free(pxp->vdevfile);
	if (pxp->outfile)
		free(pxp->outfile);
	free(pxp->buffers);
	free(pxp);
}

int main(int argc, char **argv)
{
	struct pxp_control *pxp;

	if (!(pxp = pxp_init(argc, argv)))
		return 1;

        if ((pxp->vfd = open(pxp->vdevfile, O_RDWR, 0)) < 0) {
		perror("video device open failed");
		return 1;
	}

	if (pxp_check_capabilities(pxp))
		return 1;

	if (!pxp->rotate_pass)
	{
	if (pxp_config_output(pxp, 0))
		return 1;

	if (pxp_config_buffer(pxp, 0))
		return 1;

	if (pxp_read_infiles(pxp))
		return 1;

	if (pxp_config_windows(pxp, 0))
		return 1;

	if (pxp_config_controls(pxp))
		return 1;

	if (pxp_start(pxp, 0))
		return 1;

	if (pxp_stop(pxp, 0))
		return 1;
	}else{
	  if (pxp_rotate_pass(pxp))
		return 1;
	}
	if (pxp->outfile_state)
		if (pxp_write_outfile(pxp))
			return 1;

	pxp_cleanup(pxp);

	return 0;
}

