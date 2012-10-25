/*
* V4L2 video capture example
*
* This program can be used and distributed without restrictions.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <getopt.h>		/* getopt_long() */
#include <fcntl.h>		/* low-level i/o */
#include <unistd.h>
#include <errno.h>
#include <malloc.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <asm/types.h>		/* for videodev2.h */
#include <linux/videodev2.h>
#include <linux/fb.h>
#define CLEAR(x) memset (&(x), 0, sizeof (x))
typedef enum {
	IO_METHOD_READ,
	IO_METHOD_MMAP,
	IO_METHOD_USERPTR,
} io_method;
struct buffer {
	void *start;
	size_t length;
};
static struct fb_var_screeninfo gScreenInfo;
static char *dev_name = NULL;
static char *fb_name = NULL;
static io_method io = IO_METHOD_MMAP;
static int fd = -1;
static int gFdFB = -1;
struct buffer *buffers = NULL;
static unsigned int n_buffers = 0;
static int gFBPixFormat = V4L2_PIX_FMT_RGB565;
static char gFBPixFmtName[40] = "RGB565";
static unsigned char * gpFB = NULL;
static unsigned long gVideoBufferSize;
static struct v4l2_format gFormat;
static unsigned long gCount = 1000;

static void errno_exit(const char *s)
{
	fprintf(stderr, "%s error %d, %s\n", s, errno, strerror(errno));
	exit(EXIT_FAILURE);
}

static int xioctl(int fd, int request, void *arg)
{
	int r;
	do
		r = ioctl(fd, request, arg);
	while (-1 == r && EINTR == errno);
	return r;
}

static int detect_fb_fmt(void)
{
        memset (&gScreenInfo, 0, sizeof (gScreenInfo));
        if(ioctl(gFdFB, FBIOGET_VSCREENINFO, &gScreenInfo))
        {
                printf("Unable to read FB information\n");
                return 1;
        }
        
        switch(gScreenInfo.bits_per_pixel)
        {    
                case 8:
                       
                        if ((gScreenInfo.red.offset == 5) && (gScreenInfo.red.length == 3) &&
                            (gScreenInfo.green.offset == 2) && (gScreenInfo.green.length == 3) &&
                            (gScreenInfo.blue.offset == 0) && (gScreenInfo.blue.length == 2))
                        {         
                                gFBPixFormat = V4L2_PIX_FMT_RGB332;
                                strcpy(gFBPixFmtName,"RGB332");
                        }
                
                        break;
                
                case 16:
                
                        if ((gScreenInfo.red.offset == 11) && (gScreenInfo.red.length == 5) &&
                            (gScreenInfo.green.offset == 5) && (gScreenInfo.green.length == 6) &&           
                            (gScreenInfo.blue.offset == 0) && (gScreenInfo.blue.length == 5))   
                        {          
                                gFBPixFormat = V4L2_PIX_FMT_RGB565;          
                                strcpy(gFBPixFmtName,"RGB565");
                                break;
                        }        
                        
                        if ((gScreenInfo.red.offset == 11) && (gScreenInfo.red.length == 5) &&      
                            (gScreenInfo.green.offset == 6) && (gScreenInfo.green.length == 5) &&           
                            (gScreenInfo.blue.offset == 1) && (gScreenInfo.blue.length == 5))   
                        {          
                                gFBPixFormat = V4L2_PIX_FMT_RGB555;          
                                strcpy(gFBPixFmtName,"RGB555");          
                                break;        
                        }

                        if ((gScreenInfo.red.offset == 0) && (gScreenInfo.red.length == 5) &&       
                            (gScreenInfo.green.offset == 5) && (gScreenInfo.green.length == 6) &&           
                            (gScreenInfo.blue.offset == 11) && (gScreenInfo.blue.length == 5))  
                        {          
                                gFBPixFormat = V4L2_PIX_FMT_RGB565X;          
                                strcpy(gFBPixFmtName,"RGB565X");          
                                break;        
                        }        
                        
                        if ((gScreenInfo.red.offset == 0) && (gScreenInfo.red.length == 5) &&       
                            (gScreenInfo.green.offset == 6) && (gScreenInfo.green.length == 5) &&           
                            (gScreenInfo.blue.offset == 11) && (gScreenInfo.blue.length == 5))  
                        {          
                                gFBPixFormat = V4L2_PIX_FMT_RGB555X;          
                                strcpy(gFBPixFmtName,"RGB555X");          
                                break;        
                        }
                        
                        strcpy(gFBPixFmtName,"Unknown");        
                        printf("Unsupported FB format\n");
                        
                        printf("\t Resolution = %d bpp\n", gScreenInfo.bits_per_pixel); 
                        printf("\t Color\toffset\tlength\n");   
                        printf("\t Red\t%d\t%d\n", gScreenInfo.red.offset, gScreenInfo.red.length);     
                        printf("\t Green\t%d\t%d\n", gScreenInfo.green.offset, gScreenInfo.green.length);
                        printf("\t Blue\t%d\t%d\n", gScreenInfo.blue.offset, gScreenInfo.blue.length);

                        return 1;

                case 24:        
                
                        if ((gScreenInfo.red.offset == 16) && (gScreenInfo.red.length == 8) &&
                            (gScreenInfo.green.offset == 8) && (gScreenInfo.green.length == 8) &&
                            (gScreenInfo.blue.offset == 0) && (gScreenInfo.blue.length == 8))
                        {
                                gFBPixFormat = V4L2_PIX_FMT_BGR24;
                                strcpy(gFBPixFmtName,"BGR24");
                                break;
                        }
                        
                        if ((gScreenInfo.red.offset == 0) && (gScreenInfo.red.length == 8) &&       
                            (gScreenInfo.green.offset == 8) && (gScreenInfo.green.length == 8) &&           
                            (gScreenInfo.blue.offset == 16) && (gScreenInfo.blue.length == 8))  
                        {          
                                gFBPixFormat = V4L2_PIX_FMT_RGB24;          
                                strcpy(gFBPixFmtName,"RGB24");          
                                break;        
                        }

                        printf("Unsupported FB format\n");

                        printf("\t Resolution = %d bpp\n", gScreenInfo.bits_per_pixel); 
                        printf("\t Color\toffset\tlength\n");   
                        printf("\t Red\t%d\t%d\n", gScreenInfo.red.offset, gScreenInfo.red.length);     
                        printf("\t Green\t%d\t%d\n", gScreenInfo.green.offset, gScreenInfo.green.length);
                        printf("\t Blue\t%d\t%d\n", gScreenInfo.blue.offset, gScreenInfo.blue.length);

                        return 1;

                case 32:        
                
                        if ((gScreenInfo.red.offset == 16) && (gScreenInfo.red.length == 8) &&              
                            (gScreenInfo.green.offset == 8) && (gScreenInfo.green.length == 8) &&           
                            (gScreenInfo.blue.offset == 0) && (gScreenInfo.blue.length == 8))        
                        {         
                                gFBPixFormat = V4L2_PIX_FMT_BGR32;          
                                strcpy(gFBPixFmtName,"BGR32");
                                break;
                        }
                        
                        if ((gScreenInfo.red.offset == 0) && (gScreenInfo.red.length == 8) &&       
                            (gScreenInfo.green.offset == 8) && (gScreenInfo.green.length == 8) &&           
                            (gScreenInfo.blue.offset == 16) && (gScreenInfo.blue.length == 8))  
                        {          
                                gFBPixFormat = V4L2_PIX_FMT_RGB32;          
                                strcpy(gFBPixFmtName,"RGB32");          
                                break;        
                        }

                        printf("Unsupported FB format\n");
                        printf("\t Resolution = %d bpp\n", gScreenInfo.bits_per_pixel); 
                        printf("\t Color\toffset\tlength\n");   
                        printf("\t Red\t%d\t%d\n", gScreenInfo.red.offset, gScreenInfo.red.length);     
                        printf("\t Green\t%d\t%d\n", gScreenInfo.green.offset, gScreenInfo.green.length);
                        printf("\t Blue\t%d\t%d\n", gScreenInfo.blue.offset, gScreenInfo.blue.length);

                        return 1;
                default:
                        printf("Unsupported FB format\n");
						printf("\t Resolution = %d bpp\n", gScreenInfo.bits_per_pixel); 
                        printf("\t Color\toffset\tlength\n");   
                        printf("\t Red\t%d\t%d\n", gScreenInfo.red.offset, gScreenInfo.red.length);     
                        printf("\t Green\t%d\t%d\n", gScreenInfo.green.offset, gScreenInfo.green.length);
                        printf("\t Blue\t%d\t%d\n", gScreenInfo.blue.offset, gScreenInfo.blue.length);

                return 1;

        }
        
        return 0;
}


static int open_out_device(void)
{
	struct fb_var_screeninfo var;
	printf("open framebuffer %s\n", fb_name);
        /* Open FrameBuffer Device */

        if((gFdFB = open(fb_name, O_RDWR)) < 0)
        {
                printf("Unable to open frame buffer \n");
                return 1;
        }

		if (ioctl(gFdFB, FBIOGET_VSCREENINFO, &var) < 0) {
			printf("FBIOPUT_VSCREENINFO failed\n");
			return 1;
		}

		var.xres_virtual = var.xres;
		var.yres_virtual = 2 * var.yres;
		if (ioctl(gFdFB, FBIOPUT_VSCREENINFO, &var) < 0) {
			printf("FBIOPUT_VSCREENINFO failed\n");
			return 1;
		}
       
        if(detect_fb_fmt())
                return 1;
        gVideoBufferSize = gScreenInfo.xres * gScreenInfo.yres * gScreenInfo.bits_per_pixel / 8;

        if((gpFB = mmap(0, gVideoBufferSize, PROT_READ | PROT_WRITE, MAP_SHARED, gFdFB, 0)) < 0)
        {
                printf("Error: failed to map framebuffer device to memory.\n");
                return 1;
        }
        
        return 0;
}



static void display_to_fb (unsigned char * aStart, int aLength)
{

        int dstWidth = gScreenInfo.xres * gScreenInfo.bits_per_pixel / 8;
        int srcWidth = gFormat.fmt.pix.width * gScreenInfo.bits_per_pixel / 8;

        unsigned char *pDst = gpFB;
        unsigned char *pSrc = (unsigned char *)aStart;
        
        int i = 0; 
		int width = srcWidth < dstWidth ? srcWidth : dstWidth;
		while(i < 2 * gFormat.fmt.pix.width * gFormat.fmt.pix.height)
        {
                memcpy(pDst, pSrc, width);
				pDst += dstWidth;
				pSrc += srcWidth;
				i += 2 * gFormat.fmt.pix.width;
        }
}


static void process_image(const void *p, int aLength)
{
	printf(".");
	display_to_fb( (unsigned char *)p, aLength);
}

static int read_frame(void)
{
	struct v4l2_buffer buf;
	unsigned int i;
	switch (io) {
	case IO_METHOD_READ:
		if (-1 == read(fd, buffers[0].start, buffers[0].length)) {
			switch (errno) {
			case EAGAIN:
				return 0;
			case EIO:
/* Could ignore EIO, see spec. */
/* fall through */
			default:
				errno_exit("read");
			}
		}
		process_image(buffers[0].start, buffers[0].length);
		break;
	case IO_METHOD_MMAP:
		CLEAR(buf);
		buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf.memory = V4L2_MEMORY_MMAP;
		if (-1 == xioctl(fd, VIDIOC_DQBUF, &buf)) {
			switch (errno) {
			case EAGAIN:
				printf("Error retry...\n");
				return 0;
			case EIO:
/* Could ignore EIO, see spec. */
/* fall through */
			default:
				errno_exit("VIDIOC_DQBUF");
			}
		}
		printf("process...\n");
		assert(buf.index < n_buffers);
		process_image(buffers[buf.index].start, buffers[buf.index].length);
		if (-1 == xioctl(fd, VIDIOC_QBUF, &buf))
			errno_exit("VIDIOC_QBUF");
		break;
	case IO_METHOD_USERPTR:
		CLEAR(buf);
		buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf.memory = V4L2_MEMORY_USERPTR;
		if (-1 == xioctl(fd, VIDIOC_DQBUF, &buf)) {
			switch (errno) {
			case EAGAIN:
				return 0;
			case EIO:
/* Could ignore EIO, see spec. */
/* fall through */
			default:
				errno_exit("VIDIOC_DQBUF");
			}
		}
		for (i = 0; i < n_buffers; ++i)
			if (buf.m.userptr == (unsigned long)buffers[i].start
			    && buf.length == buffers[i].length)
				break;
		assert(i < n_buffers);
		process_image((void *)buf.m.userptr,buffers[i].length);
		if (-1 == xioctl(fd, VIDIOC_QBUF, &buf))
			errno_exit("VIDIOC_QBUF");
		break;
	  default:
		printf("process default ...\n");
	  	break;
	}
	return 1;
}

static void mainloop(void)
{
	//gCount = 1000;
	while (gCount-- > 0) {
			fd_set fds;
			struct timeval tv;
			int r;
			FD_ZERO(&fds);
			FD_SET(fd, &fds);
/* Timeout. */
			tv.tv_sec = 2;
			tv.tv_usec = 0;
			r = select(fd + 1, &fds, NULL, NULL, &tv);
			if (-1 == r) {
				if (EINTR == errno)
					continue;
				errno_exit("select");
			}
			if (0 == r) {
				fprintf(stderr, "select timeout\n");
				exit(EXIT_FAILURE);
			}
			if (read_frame())
				break;
	}
}

static void stop_capturing(void)
{
	enum v4l2_buf_type type;
	switch (io) {
	case IO_METHOD_READ:
/* Nothing to do. */
		break;
	case IO_METHOD_MMAP:
	case IO_METHOD_USERPTR:
		type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		if (-1 == xioctl(fd, VIDIOC_STREAMOFF, &type))
			errno_exit("VIDIOC_STREAMOFF");
		break;
	}
}

static void start_capturing(void)
{
	unsigned int i;
	enum v4l2_buf_type type;
	printf("start capturing\n");
	switch (io) {
	case IO_METHOD_READ:
/* Nothing to do. */
		break;
	case IO_METHOD_MMAP:
		for (i = 0; i < n_buffers; ++i) {
			struct v4l2_buffer buf;
			CLEAR(buf);
			buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
			buf.memory = V4L2_MEMORY_MMAP;
			buf.index = i;
			if (-1 == xioctl(fd, VIDIOC_QBUF, &buf))
				errno_exit("VIDIOC_QBUF");
		}
		type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		if (-1 == xioctl(fd, VIDIOC_STREAMON, &type))
			errno_exit("VIDIOC_STREAMON");
		break;
	case IO_METHOD_USERPTR:
		for (i = 0; i < n_buffers; ++i) {
			struct v4l2_buffer buf;
			CLEAR(buf);
			buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
			buf.memory = V4L2_MEMORY_USERPTR;
			buf.index = i;
			buf.m.userptr = (unsigned long)buffers[i].start;
			buf.length = buffers[i].length;
			if (-1 == xioctl(fd, VIDIOC_QBUF, &buf))
				errno_exit("VIDIOC_QBUF");
		}
		type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		if (-1 == xioctl(fd, VIDIOC_STREAMON, &type))
			errno_exit("VIDIOC_STREAMON");
		break;
	}
}

static void uninit_device(void)
{
	unsigned int i;
	switch (io) {
	case IO_METHOD_READ:
		free(buffers[0].start);
		break;
	case IO_METHOD_MMAP:
		for (i = 0; i < n_buffers; ++i)
			if (-1 == munmap(buffers[i].start, buffers[i].length))
				errno_exit("munmap");
		break;
	case IO_METHOD_USERPTR:
		for (i = 0; i < n_buffers; ++i)
			free(buffers[i].start);
		break;
	}
	free(buffers);
}

static void init_read(unsigned int buffer_size)
{
	buffers = calloc(1, sizeof(*buffers));
	if (!buffers) {
		fprintf(stderr, "Out of memory\n");
		exit(EXIT_FAILURE);
	}
	buffers[0].length = buffer_size;
	buffers[0].start = malloc(buffer_size);
	if (!buffers[0].start) {
		fprintf(stderr, "Out of memory\n");
		exit(EXIT_FAILURE);
	}
}

static void init_mmap(void)
{
	struct v4l2_requestbuffers req;
	CLEAR(req);
	req.count = 4;
	req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	req.memory = V4L2_MEMORY_MMAP;
	if (-1 == xioctl(fd, VIDIOC_REQBUFS, &req)) {
		if (EINVAL == errno) {
			fprintf(stderr, "%s does not support "
				"memory mapping\n", dev_name);
			exit(EXIT_FAILURE);
		} else {
			errno_exit("VIDIOC_REQBUFS");
		}
	}
	if (req.count < 2) {
		fprintf(stderr, "Insufficient buffer memory on %s\n", dev_name);
		exit(EXIT_FAILURE);
	}
	buffers = calloc(req.count, sizeof(*buffers));
	if (!buffers) {
		fprintf(stderr, "Out of memory\n");
		exit(EXIT_FAILURE);
	}
	for (n_buffers = 0; n_buffers < req.count; ++n_buffers) {
		struct v4l2_buffer buf;
		CLEAR(buf);
		buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf.memory = V4L2_MEMORY_MMAP;
		buf.index = n_buffers;
		if (-1 == xioctl(fd, VIDIOC_QUERYBUF, &buf))
			errno_exit("VIDIOC_QUERYBUF");
		buffers[n_buffers].length = buf.length;
		buffers[n_buffers].start = mmap(NULL /* start anywhere */ ,
						buf.length,
						PROT_READ | PROT_WRITE
						/* required */ ,
						MAP_SHARED /* recommended */ ,
						fd, buf.m.offset);
		if (MAP_FAILED == buffers[n_buffers].start)
			errno_exit("mmap");
	}
}

static void init_userp(unsigned int buffer_size)
{
	struct v4l2_requestbuffers req;
	unsigned int page_size;
	page_size = getpagesize();
	buffer_size = (buffer_size + page_size - 1) & ~(page_size - 1);
	CLEAR(req);
	req.count = 4;
	req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	req.memory = V4L2_MEMORY_USERPTR;
	if (-1 == xioctl(fd, VIDIOC_REQBUFS, &req)) {
		if (EINVAL == errno) {
			fprintf(stderr, "%s does not support "
				"user pointer i/o\n", dev_name);
			exit(EXIT_FAILURE);
		} else {
			errno_exit("VIDIOC_REQBUFS");
		}
	}
	buffers = calloc(4, sizeof(*buffers));
	if (!buffers) {
		fprintf(stderr, "Out of memory\n");
		exit(EXIT_FAILURE);
	}
	for (n_buffers = 0; n_buffers < 4; ++n_buffers) {
		buffers[n_buffers].length = buffer_size;
		buffers[n_buffers].start = memalign( /* boundary */ page_size,
						    buffer_size);
		if (!buffers[n_buffers].start) {
			fprintf(stderr, "Out of memory\n");
			exit(EXIT_FAILURE);
		}
	}
}

static void init_device(void)
{
	struct v4l2_capability cap;
	struct v4l2_cropcap cropcap;
	struct v4l2_crop crop;
	struct v4l2_format fmt;
	unsigned int min;
	if (-1 == xioctl(fd, VIDIOC_QUERYCAP, &cap)) {
		if (EINVAL == errno) {
			fprintf(stderr, "%s is no V4L2 device\n", dev_name);
			exit(EXIT_FAILURE);
		} else {
			errno_exit("VIDIOC_QUERYCAP");
		}
	}
	if (!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)) {
		fprintf(stderr, "%s is no video capture device\n", dev_name);
		exit(EXIT_FAILURE);
	}
	switch (io) {
	case IO_METHOD_READ:
		if (!(cap.capabilities & V4L2_CAP_READWRITE)) {
			fprintf(stderr, "%s does not support read i/o\n",
				dev_name);
			exit(EXIT_FAILURE);
		}
		break;
	case IO_METHOD_MMAP:
	case IO_METHOD_USERPTR:
		if (!(cap.capabilities & V4L2_CAP_STREAMING)) {
			fprintf(stderr, "%s does not support streaming i/o\n",
				dev_name);
			exit(EXIT_FAILURE);
		}
		break;
	}
/* Select video input, video standard and tune here. */
	CLEAR(cropcap);
	cropcap.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	if (0 == xioctl(fd, VIDIOC_CROPCAP, &cropcap)) {
		crop.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		crop.c = cropcap.defrect;	/* reset to default */
		if (-1 == xioctl(fd, VIDIOC_S_CROP, &crop)) {
			switch (errno) {
			case EINVAL:
				printf("set crop fails\n");
/* Cropping not supported. */
				break;
			default:
/* Errors ignored. */
				break;
			}
		}
	} else {
		printf("crop not support\n");
/* Errors ignored. */
	}
	CLEAR(fmt);
	fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	fmt.fmt.pix.width = 640;
	fmt.fmt.pix.height = 480;
	fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_RGB565;
	/*fmt.fmt.pix.field = V4L2_FIELD_INTERLACED;*/
	if (-1 == xioctl(fd, VIDIOC_S_FMT, &fmt))
		errno_exit("VIDIOC_S_FMT");
	CLEAR(gFormat);
	memcpy(&gFormat, &fmt, sizeof(gFormat));
	if(ioctl(fd, VIDIOC_G_FMT, &gFormat) < 0)
		errno_exit("VIDIOC_G_FMT");
/* Note VIDIOC_S_FMT may change width and height. */
/* Buggy driver paranoia. */
	min = fmt.fmt.pix.width * 2;
	if (fmt.fmt.pix.bytesperline < min)
		fmt.fmt.pix.bytesperline = min;
	min = fmt.fmt.pix.bytesperline * fmt.fmt.pix.height;
	if (fmt.fmt.pix.sizeimage < min)
		fmt.fmt.pix.sizeimage = min;
	switch (io) {
	case IO_METHOD_READ:
		init_read(fmt.fmt.pix.sizeimage);
		break;
	case IO_METHOD_MMAP:
		init_mmap();
		break;
	case IO_METHOD_USERPTR:
		init_userp(fmt.fmt.pix.sizeimage);
		break;
	}
}

static void close_device(void)
{
	if (-1 == close(fd))
		errno_exit("close");
	fd = -1;
}

static void open_device(void)
{
	struct stat st;
	if (-1 == stat(dev_name, &st)) {
		fprintf(stderr, "Cannot identify '%s' %d, %s\n",
			dev_name, errno, strerror(errno));
		exit(EXIT_FAILURE);
	}
	if (!S_ISCHR(st.st_mode)) {
		fprintf(stderr, "%s is no device\n", dev_name);
		exit(EXIT_FAILURE);
	}
	fd = open(dev_name, O_RDWR /* required */  | O_NONBLOCK, 0);
	if (-1 == fd) {
		fprintf(stderr, "Cannot open '%s' %d, %s\n",
			dev_name, errno, strerror(errno));
		exit(EXIT_FAILURE);
	}
	if ( open_out_device())
	{
		fprintf(stderr, "Cannot open '%s' %d, %s\n",
			fb_name, errno, strerror(errno));
		exit(EXIT_FAILURE);
	}
}

static void usage(FILE * fp, int argc, char **argv)
{
	fprintf(fp,
		"Usage: %s [options]\n\n"
		"Options:\n"
		"-d | --device name Video device name [/dev/video]\n"
		"-h | --help Print this message\n"
		"-m | --mmap Use memory mapped buffers\n"
		"-r | --read Use read() calls\n"
		"-u | --userp Use application allocated buffers\n" "", argv[0]);
}

static const char short_options[] = "b:c:d:hmru";
static const struct option long_options[] = {
	{"fb", required_argument, NULL, 'b'},
	{"count", required_argument, NULL, 'c'},
	{"device", required_argument, NULL, 'd'},
	{"help", no_argument, NULL, 'h'},
	{"mmap", no_argument, NULL, 'm'},
	{"read", no_argument, NULL, 'r'},
	{"userp", no_argument, NULL, 'u'},
	{0, 0, 0, 0}
};

int main(int argc, char **argv)
{
	dev_name = "/dev/video";
	fb_name = "/dev/fb0";
	for (;;) {
		int index;
		int c;
		c = getopt_long(argc, argv,
				short_options, long_options, &index);
		if (-1 == c)
			break;
		switch (c) {
		case 0:	/* getopt_long() flag */
			break;
		case 'b':
			fb_name = optarg;
			break;
		case 'c':
			gCount = atoi(optarg);
			break;
		case 'd':
			dev_name = optarg;
			break;
		case 'h':
			usage(stdout, argc, argv);
			exit(EXIT_SUCCESS);
		case 'm':
			io = IO_METHOD_MMAP;
			break;
		case 'r':
			io = IO_METHOD_READ;
			break;
		case 'u':
			io = IO_METHOD_USERPTR;
			break;
		default:
			usage(stderr, argc, argv);
			exit(EXIT_FAILURE);
		}
	}
	open_device();
	init_device();
	start_capturing();
	mainloop();
	stop_capturing();
	uninit_device();
	close_device();
	exit(EXIT_SUCCESS);
	return 0;
}
