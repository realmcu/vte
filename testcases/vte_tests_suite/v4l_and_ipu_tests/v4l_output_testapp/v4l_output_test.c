/***
**Copyright (C) 2005-2011 Freescale Semiconductor, Inc. All Rights Reserved.
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
    @file   v4l_output_test.c

    @brief  v4l output Test scenario
    
====================================================================================================
Revision History:
                            Modification     Tracking
Author                          Date          Number    Description of Changes
-------------------------   ------------    ----------  -------------------------------------------
Smirnov Artyom/ID            07/05/2005     TLSbo49894   BRIEF description of changes made 
KHOROSHEV.D                  09/29/2005     TLSbo55077   Review version

====================================================================================================
Portability: ARM GCC
==================================================================================================*/

#ifdef __cplusplus
extern "C"{ 
#endif

/*==================================================================================================
                                        INCLUDE FILES
==================================================================================================*/
/* Standard header files */
#include <string.h>
#include <time.h>
#include <sys/time.h>

/* Verification Test Environment Include Files */
#include "v4l_output_test.h"

/*==================================================================================================
                                        LOCAL MACROS
==================================================================================================*/

#define SLEEP_TIME 10

/*==================================================================================================
                            LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
==================================================================================================*/


/*==================================================================================================
                                        LOCAL CONSTANTS
==================================================================================================*/
const char user_output[3][40] = {
        "biggest size",
        "middle size",
        "smallest size"
};

const int height_table[3] = { 320, 200, 20 };
const int width_table[3] = { 240, 100, 10 };

/*==================================================================================================
                                        LOCAL VARIABLES
==================================================================================================*/

static int v4l_output_fd = 0;
static int input_fd = 0;
static int output_fd = 0;

static struct v4l2_control ctrl;
static struct v4l2_format fmt;
static struct v4l2_cropcap cropcap;
static struct v4l2_crop crop;
static struct v4l2_buffer buf;
static struct v4l2_requestbuffers buf_req;

static video_buffer buffers[MAX_BUFF_NUM];

/*==================================================================================================
                                        GLOBAL CONSTANTS
==================================================================================================*/


/*==================================================================================================
                                        GLOBAL VARIABLES
==================================================================================================*/

extern params p;

/*==================================================================================================
                                    LOCAL FUNCTION PROTOTYPES
==================================================================================================*/

int     parse_file(int in_fd, int *width, int *height, char **image_fmt);
int     configure(void);
int     process_image(void);
int     resize(void);
int     ask_user(void);
int     kbhit(int *sleeptime);
int     pixel_format(char *string);
int     write_file(char *format_name, char *start);
void    write_zero_to_file(FILE * pf, int bytes);
int     do_resizing(void);

/*==================================================================================================
                                        LOCAL FUNCTIONS
==================================================================================================*/
/*================================================================================================*/
/*===== pixel_format =====*/
/**
@brief  Detects pixel format.
        
@param  Input:        string - pointer to string which contains format name.

@return On success - Format number.
        On failure - zero value.*/
/*================================================================================================*/

int pixel_format(char *string)
{
        int     format = 0;

        if (!strcasecmp(string, "BGR24"))
        {
                format = V4L2_PIX_FMT_BGR24;
        }
        else if (!strcasecmp(string, "YUV420"))
        {
                format = V4L2_PIX_FMT_YUV420;
        }
        else if (!strcasecmp(string, "YUV422P"))
        {
                format = V4L2_PIX_FMT_YUV422P;
        }
        else if (!strcasecmp(string, "RGB565"))
        {
                format = V4L2_PIX_FMT_RGB565;
        }
        else if (!strcasecmp(string, "RGB24"))
        {
                format = V4L2_PIX_FMT_RGB24;
        }
        else if (!strcasecmp(string, "RGB32"))
        {
                format = V4L2_PIX_FMT_RGB32;
        }
        else if (!strcasecmp(string, "BGR32"))
        {
                format = V4L2_PIX_FMT_BGR32;
        }else if (!strcasecmp(string,"UYVY"))
	{
	        format = V4L2_PIX_FMT_UYVY;
	}else if (!strcasecmp(string,"YUYV"))
	{
	        format = V4L2_PIX_FMT_YUYV;
	}
        else
        {
                tst_resm(TBROK, "Unsupported file format");
        }
        return format;
}

/*================================================================================================*/
/*===== VT_v4l_output_setup =====*/
/**
@brief  assumes the pre-condition of the test case execution

@param  None
    
@return On success - return TPASS
        On failure - return the error code*/
/*================================================================================================*/
int VT_v4l_output_setup(void)
{
        int     rv = TFAIL;
		struct v4l2_fmtdesc fmtdesc;
		struct v4l2_capability cap;

        v4l_output_fd = open(p.v4l_dev_file, O_RDWR, 0);

        if (v4l_output_fd == -1)
        {
                tst_resm(TBROK, "Can't open V4L_DEVICE: %s", p.v4l_dev_file);
                goto final;
        }
        if (!ioctl(v4l_output_fd, VIDIOC_QUERYCAP, &cap)) 
		{
          printf("driver=%s, card=%s, bus=%s,version=0x%08x,caps=0x%08x\n", 
		  cap.driver, cap.card, cap.bus_info, cap.version, cap.capabilities); 
		}
		fmtdesc.index = 0;
		fmtdesc.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
		while (!ioctl(v4l_output_fd, VIDIOC_ENUM_FMT, &fmtdesc)) {
    printf("fmt %s: fourcc = 0x%08x\n",fmtdesc.description,fmtdesc.pixelformat);
		 fmtdesc.index++;
		}
        input_fd = open(p.input_file, O_RDONLY);

        if (input_fd == -1)
        {
                tst_resm(TBROK, "Can't open input dump file: %s", p.input_file);
                goto final;
        }

        if (p.test_type == 1)
        {
                output_fd = open(p.output_file, O_WRONLY | O_CREAT,S_IRWXU |  S_IRWXG | S_IRWXO);

                if (output_fd == -1)
                {
                        tst_resm(TBROK, "Can't open output dump file: %s", p.output_file);
                        goto final;
                }
        }

        rv = TPASS;
        return rv;
    final:
	    close(v4l_output_fd);
        return rv;
}


/*================================================================================================*/
/*===== VT_v4l_output_cleanup =====*/
/**
@brief  assumes the post-condition of the test case execution

@param  None
    
@return On success - return TPASS
        On failure - return the error code*/
/*================================================================================================*/
int VT_v4l_output_cleanup(void)
{
        int     rv = TPASS;

        #if 0
		memset(&buf_req, 0, sizeof(buf_req));
        /* Freeing buffers */
        buf_req.count = 0;
        buf_req.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
        buf_req.memory = V4L2_MEMORY_MMAP;

        if (ioctl(v4l_output_fd, VIDIOC_REQBUFS, &buf_req) < 0)
        {
                tst_resm(TBROK, "Error: ioctl VIDIOC_REQBUFS failed. Returned code %d", errno);
        }
        #endif
        if (v4l_output_fd >= 0)
                close(v4l_output_fd);

        if ((output_fd >= 0) && (p.test_type == 1))
                close(output_fd);

        return rv;
}


/*================================================================================================*/
/*===== VT_v4l_output_test =====*/
/* 
* @brief This function executes v4l output test scenario.
* 
* @param None
* 
* @return On success - return TPASS On failure - return the error code */
/*================================================================================================*/
int VT_v4l_output_test(void)
{
        int     rv = TFAIL;

        /* parse input file */
        if (parse_file(input_fd, &(p.x_size_src), &(p.y_size_src), &(p.input_fmt)))
                goto final;

/*        tst_resm(TINFO, "file parsed size_src:%d %d, input_fmt %s", p.x_size_src, p.x_size_src,
                 p.input_fmt);*/
        /* get destination frame size */
        if (resize())
                goto final;

        /* configure v4l device */
        if (configure())
                goto final;

        /* process frames and display on LCD */
        if (process_image())
                goto final;
        #if 0
        if (ask_user())
                goto final;
        #endif
        rv = TPASS;

    final:

        return rv;
}


/*================================================================================================*/
/*===== parse_file =====*/
/* 
* @brief
* 
* @param
* 
* @return On success - return TPASS On failure - return the error code */
/*================================================================================================*/
int parse_file(int in_fd, int *width, int *height, char **image_fmt)
{
        dump_file_header hdr;

        lseek(in_fd, 0, SEEK_SET);

        if (read(in_fd, &hdr, sizeof(dump_file_header)) < 0)
                return -1;

        *image_fmt = strdup(hdr.image_fmt);
        *width = atoi(hdr.image_width);
        *height = atoi(hdr.image_height);

        lseek(in_fd, 0, SEEK_SET);

        return 0;
}



/*================================================================================================*/
/*===== configure =====*/
/* 
* @brief
* 
* @param None
* 
* @return On success - return TPASS On failure - return the error code */
/*================================================================================================*/
int configure(void)
{
        int     rv = TFAIL;
        int    status;
     #if 0
        if (ioctl(v4l_output_fd, VIDIOC_S_OUTPUT, &g_output) < 0)
        {
                tst_resm(TBROK, "set output failed. Error code %d", errno);
        }
     #endif

        memset(&cropcap, 0, sizeof(cropcap));

        cropcap.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;

        if ((status = ioctl(v4l_output_fd, VIDIOC_CROPCAP, &cropcap)) < 0)
        {
                tst_resm(TBROK, "Error: VIDIOC_CROPCAP ioctl failed. Returned code %d", status);
                goto final;
        }

      printf("cropcap.bounds.width = %d\ncropcap.bound.height = %d\n"
               "cropcap.defrect.width = %d\ncropcap.defrect.height = %d\n",
               cropcap.bounds.width, cropcap.bounds.height,
               cropcap.defrect.width, cropcap.defrect.height);

        if (cropcap.type != V4L2_BUF_TYPE_VIDEO_OUTPUT)
        {
                tst_resm(TBROK, "Error: cropcap is not V4L2_BUF_TYPE_VIDEO_OUTPUT type");
                goto final;
        }

        /* cropcap.bounds.left.top.width.height */

        /* set cropping parameters */

        crop.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
        crop.c.top = 0;
        crop.c.left = 0;
        crop.c.width = p.x_size_dst;
        crop.c.height = p.y_size_dst;

	if (2 == p.crop_on)
	{
	  /*offset crop*/
          crop.c.top = p.y_offset;
          crop.c.left = p.x_offset;
	}

        /* ignore if cropping is not supported (EINVAL) */
        if (ioctl(v4l_output_fd, VIDIOC_S_CROP, &crop) < 0 && errno != EINVAL)
        {
                tst_resm(TBROK, "Error: VIDIOC_S_CROP ioctl failed. Returned code %d", errno);
                goto final;
        }

        /* ignore if cropping is not supported (EINVAL) */
        if (p.nb_rot)
        {
                ctrl.id = V4L2_CID_ROTATE;
                ctrl.value = p.nb_rot;
                if (ioctl(v4l_output_fd, VIDIOC_S_CTRL, &ctrl) < 0)
                {
                        tst_resm(TBROK, "Error: VIDIOC_S_CROP ioctl failed");
                        goto final;
                }
        }

        memset(&fmt, 0, sizeof(fmt));

        /* compute the output frame size */

        fmt.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;

        fmt.fmt.pix.width = p.x_size_src;
        fmt.fmt.pix.height = p.y_size_src;

        fmt.fmt.pix.pixelformat = pixel_format(p.input_fmt);

        if (ioctl(v4l_output_fd, VIDIOC_S_FMT, &fmt) < 0)
        {
                tst_resm(TBROK,
                         "Error: VIDIOC_S_FMT ioctl failed. Error code: %d, fd: %d, size: %d %d, fmt.fmt.pix.pixelformat: %d",
                         errno, v4l_output_fd, fmt.fmt.pix.width, fmt.fmt.pix.height,
                         fmt.fmt.pix.pixelformat);
                goto final;
        }

        /* if (ioctl(v4l_output_fd, VIDIOC_G_FMT, fmt) < 0) { tst_resm(TBROK, "Error: VIDIOC_G_FMT *
        * * * * ioctl failed"); return TFAIL; } */

        memset(&buf_req, 0, sizeof(buf_req));

        buf_req.count = p.nb_buffers;
        buf_req.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
        buf_req.memory = V4L2_MEMORY_MMAP;

        if (ioctl(v4l_output_fd, VIDIOC_REQBUFS, &buf_req) < 0)
        {
                tst_resm(TBROK, "Error: ioctl VIDIOC_REQBUFS failed. Returned code %d", errno);
                goto final;
        }

        p.nb_buffers = buf_req.count;
        p.frame_size = fmt.fmt.pix.sizeimage;

        /* assume there are no errors */
        rv = TPASS;

    final:

        return rv;
}


/*================================================================================================*/
/*===== process_image =====*/
/* 
* @brief
* 
* @param None
* 
* @return On success - return TPASS On failure - return the error code */
/*================================================================================================*/
int process_image(void)
{
        int     i, type, err = 0, g_frame_period = 33333;
        int     rv = TFAIL;
        struct timeval tv_start;

        unsigned char *b;

        b = (unsigned char *) malloc(p.frame_size + 1);
        lseek(input_fd, RAW_DATA_OFFSET, SEEK_SET);
        err = read(input_fd, b, p.frame_size);

        if (err < p.frame_size)
        {
                lseek(input_fd, RAW_DATA_OFFSET, SEEK_SET);
                err = read(input_fd, (void *) b, p.frame_size);
        }

        tst_resm(TINFO, "Press 'q' for exit.");

        for (i = 0; i < p.nb_buffers; i++)
        {
                memset(&buf, 0, sizeof(buf));

                buf.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
                buf.memory = V4L2_MEMORY_MMAP;
                buf.index = i;

                if ((err = ioctl(v4l_output_fd, VIDIOC_QUERYBUF, &buf)) < 0)
                {
                        tst_resm(TBROK, "Error: query buf failed. Returned code %d", err);
                        goto final;
                }

                buffers[i].length = buf.length;
                buffers[i].offset = (size_t) buf.m.offset;

                buffers[i].start = mmap(NULL, buffers[i].length,
                                        PROT_READ | PROT_WRITE, MAP_SHARED, v4l_output_fd,
                                        buffers[i].offset);

                if (buffers[i].start == MAP_FAILED)
                {
                        tst_resm(TBROK, "Error: can't mmap file");
                        goto final;
                }
        }

        gettimeofday(&tv_start, 0);

        for (i = 0;; i++)
        {
                buf.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
                buf.memory = V4L2_MEMORY_MMAP;
                if (i < p.nb_buffers)
                {
                        buf.index = i;
                        if (ioctl(v4l_output_fd, VIDIOC_QUERYBUF, &buf) < 0)
                        {
                                tst_resm(TBROK, "Error: query buf failed. Returned code %d", errno);
                                break;
                        }

                }
                else
                {
                        buf.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
                        buf.memory = V4L2_MEMORY_MMAP;
                        if (ioctl(v4l_output_fd, VIDIOC_DQBUF, &buf) < 0)
                        {
                                tst_resm(TBROK, "Error: query buf failed(1)");
                                goto final;
                        }
                        if (p.test_type == 1)
                        {
                                write_file(p.output_fmt, (char *)buffers[buf.index].start);
                        }
                }

                memcpy(buffers[buf.index].start, b, p.frame_size);

                buf.timestamp.tv_sec = tv_start.tv_sec;
                buf.timestamp.tv_usec = tv_start.tv_usec + g_frame_period * i;
                if (ioctl(v4l_output_fd, VIDIOC_QBUF, &buf) < 0)
                {
                        tst_resm(TBROK, "Error: VIDIOC_QBUF ioctl failed");
                        goto final;
                }

                if (i == 1)
                {
                        if (p.test_type == 1)
                        {
                                int     conv_fmt = pixel_format(p.output_fmt);

                                if (conv_fmt == 0)
                                        goto final;
                                if (ioctl(v4l_output_fd, VIDIOC_G_FMT, &fmt) < 0)
                                {
                                        tst_resm(TBROK, "Error: VIDIOC_G_FMT ioctl failed");
                                        goto final;
                                }
                                fmt.fmt.pix.pixelformat = conv_fmt;
                                fmt.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
                                if (ioctl(v4l_output_fd, VIDIOC_S_FMT, &fmt) < 0)
                                {
                                        tst_resm(TBROK,
                                                 "Error: VIDIOC_S_FMT ioctl failed.Returned code %d, %s",
                                                 errno, strerror(errno));
                                        goto final;
                                }
                        }



                        type = V4L2_BUF_TYPE_VIDEO_OUTPUT;

                        if (ioctl(v4l_output_fd, VIDIOC_STREAMON, &type) < 0)
                        {
                                tst_resm(TBROK, "Error: can't start stream");
                                goto final;
                        }
                }
		extern int iLOOP;
		if(0 == iLOOP--){
		    break;
		}
		/*
                if (kbhit(&sleeptime))
                {
                        ch = getchar();
                        if (ch == 'q' || ch == 'Q')
                                break;
                }
		*/
        }
        rv = TPASS;
    final:
        type = V4L2_BUF_TYPE_VIDEO_OUTPUT;

        ioctl(v4l_output_fd, VIDIOC_STREAMOFF, &type);
        rv = TPASS;

        for (i = 0; i < p.nb_buffers; i++)
        {
                munmap(buffers[i].start, buffers[i].length);
        }


        return rv;
}

/*================================================================================================*/
/*===== resize =====*/
/* 
* @brief
* 
* @param None
* 
* @return On success - return TPASS On failure - return the error code */
/*================================================================================================*/
int resize(void)
{
        p.x_size_dst = (int) ((float) p.x_ratio / 100.0f * p.x_size_src);
        p.y_size_dst = (int) ((float) p.y_ratio / 100.0f * p.y_size_src);

        if (2 == p.crop_on)
	{
	  /*offset crop enbaled*/
	  p.x_size_dst = p.x_size_crop > p.x_size_dst ? p.x_size_dst : p.x_size_crop ;
	  p.y_size_dst = p.y_size_crop > p.y_size_dst ? p.y_size_dst : p.y_size_crop ;
        }

        if (!p.x_size_dst || !p.y_size_dst)
        {
                tst_resm(TBROK, "Error: bad resizing ratio - destination frame size is zero");
                return -1;
        }

        return 0;
}


/*================================================================================================*/
/*===== ask_user =====*/
/**
@brief  Asks user to answer the question: is the drawn picture right?

@param  Input:  None
        Output: None

@return 1 - if user asks "No,  wrong"
        0 - if user asks "Yes, right"*/
/*================================================================================================*/

int ask_user(void)
{
        int     rv = TFAIL;
        unsigned char answer;
        int     ret = 2;

        do
        {
                printf("Is video displayed right? [y/n] ");
                answer = toupper(fgetc(stdin));

                if (answer == 'Y' || answer == 'y')
                        ret = 0;
                else if (answer == 'N' || answer == 'n')
                        ret = 1;
        }
        while (ret == 2);

        fgetc(stdin);   /* Wipe CR character from stream */

        if (!ret)
                rv = TPASS;

        return rv;
}

/*================================================================================================*/
/*===== kbhit =====*/
/**
@brief  Checks state of stdin input stream. This function wait for changing state of stdin with timeout
        SLEEP_TIME.
@param  Output: pnSleepTime - returns elapsed time in milliseconds.

@return Returns true if stream contains unread symbols, unless return false.*/
/*================================================================================================*/
int kbhit(int *pnSleepTime)
{
        fd_set  rset;
        struct timeval tv;
        int     nSelect;

        FD_ZERO(&rset);
        FD_SET(fileno(stdin), &rset);

        tv.tv_sec = 0;
        tv.tv_usec = *pnSleepTime;

        *pnSleepTime = 0;

        nSelect = select(fileno(stdin) + 1, &rset, NULL, NULL, &tv);

        if (nSelect == -1)
                return 0;

        /* 
        * Calculate the elapsed time
        */
        *pnSleepTime = SLEEP_TIME;
        if (nSelect > 0)
                *pnSleepTime = tv.tv_usec;

        return nSelect > 0;
}

/*===== write_file_header =====*/
/**
@brief  Writting dump file header

@param  None

@return On success - return TPASS
        On failure - return the error code*/
int write_file(char *pix_fmt_name, char *start)
{
        FILE   *pfile;

#ifdef DEBUG_TEST

        printf("\tOutput Dump File : %s\n", p.output_file);

#endif

        if (!(pfile = fopen(p.output_file, "w+b")))
        {
                tst_resm(TWARN,
                         "Error write_file_header() : Unable to open file %s : %s",
                         p.output_file, strerror(errno));
                return TFAIL;
        }
        else
        {
                char    Width[10];
                char    Height[10];

                sprintf(Width, "%d", fmt.fmt.pix.width);
                sprintf(Height, "%d", fmt.fmt.pix.height);

                fseek(pfile, 0, SEEK_SET);

                fwrite(pix_fmt_name, sizeof(unsigned char), strlen(pix_fmt_name), pfile);
                putc('\0', pfile);
                write_zero_to_file(pfile, 0x000F - strlen(pix_fmt_name));

                fwrite(Width, sizeof(unsigned char), strlen(Width), pfile);
                putc('\0', pfile);
                write_zero_to_file(pfile, (0x001F - 0x0010) - strlen(Width));

                fwrite(Height, sizeof(unsigned char), strlen(Height), pfile);
                putc('\0', pfile);
                write_zero_to_file(pfile, (0x002F - 0x0020) - strlen(Height));
                write_zero_to_file(pfile, 0x0100 - 0x0030);

                fwrite(start, p.frame_size, sizeof(char), pfile);


                fclose(pfile);
                pfile = NULL;
        }
        return TPASS;
}

/*===== write_zero_to_file =====*/
/**
@brief  Writting zero to dump file

@param  pf     - Pointer to offset of the file where to write zeros
        bytes  - Number of bytes to write

@return None*/
void write_zero_to_file(FILE * pf, int bytes)
{
        char   *str = malloc(sizeof(unsigned char) * bytes);

        memset(str, 0, bytes);

        fwrite(str, sizeof(unsigned char), bytes, pf);

        free(str);
}

int do_resizing(void)
{
        int     i,
                ret = 0,
            rv = 0;

        for (i = 0; i < 3; i++)
        {
                tst_resm(TINFO, "Performing resizing to the %s\n", user_output[i]);

                fmt.fmt.win.w.left = 0;
                fmt.fmt.win.w.top = 0;
                fmt.fmt.win.w.height = height_table[i];
                fmt.fmt.win.w.width = width_table[i];
#ifdef DEBUG_TEST
                printf(" width = %d, height =%d\n", width_table[i], height_table[i]);
#endif

                if ((rv = ioctl(v4l_output_fd, VIDIOC_S_FMT, &fmt)) < 0)
                {
                        tst_resm(TWARN, "Formatting failed. Returned code %d", rv);
                        return TFAIL;
                }

                ret |= ask_user();
        }

        return ret;
}

#ifdef __cplusplus
} 
#endif
