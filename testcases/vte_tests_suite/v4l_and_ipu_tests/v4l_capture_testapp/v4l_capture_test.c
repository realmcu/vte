/***
**Copyright 2004-2009 Freescale Semiconductor, Inc. All Rights Reserved.
**
**The code contained herein is licensed under the GNU General Public
**License. You may obtain a copy of the GNU General Public License
**Version 2 or later at the following locations:
**
**http://www.opensource.org/licenses/gpl-license.html
**http://www.gnu.org/copyleft/gpl.html
**/

/**
@file   v4l_capture_test.c

@brief  V4L capture test scenario C source.

Description of the file

@par Portability: ARM GCC

*/

/*======================== REVISION HISTORY ==================================

Author (core ID)      Date         CR Number    Description of Changes
-------------------   ----------   ----------   ------------------------------
A.Geniatov/gntl002    20/07/2004   TLSbo40898   Initial version
A.Geniatov/gntl002    27/09/2004   TLSbo40898   Change after review
Filinova Natalya      11/04/2005   TLSbo48417   Added functionality for 
                                                such usecase as View Finder
                                                via Overlay v4l2 feature.
                                                Divided capture functionality on
                                                a output display case and write
                                                to dump file case.
                                                Added a ability to write output 
                                                to a dump file.
Delaspre/rc149c       07/12/2004   TLSbo40142   update copyrights with Freescale
Bezrukov.S/SBAZR1C    08/17/2005   TLSbo53919   Remove the Brightness feature 
Filinova N./NFILI1C   15/09/2005   TLSbo54946   Remove all VIDIOC_S/G_CTRL calls using Brightness & other sub-commands 
                                                linked to camera set-up.
                                                Overlay updated.
Filinova N./NFILI1C   27/09/2005   TLSbo54946   Resizing, rotation tests updated.
Filinova N./nfili1c   20/10/2005   TLSbo56683   Reinitializetion all cropping, resize etc to nominal value
Filinova N./nfili1c   10/11/2005   TLSbo58341   Add the Rotaion feature to the Capture Test Case
Filinova N./nfili1c   23/11/2005   TLSbo58746   Add the settings of cropping restangle via four new options: 
                                                left corner, top corner, width, height                                                  
Kazachkov D/e1403c    10/03/2006   TLSbo64430   Cleanup: stop than reset
Kazachkov D/e1403c    15/03/2006   TLSbo63493   Add a 1 second sleep so the hw will have time to reset
Kazachkov D/b0016      5/04/2006   TLSbo64363   Add PRIMARY type overlay
Kazachkov D/b0016     23/05/2006   TLSbo68938   Fix Video Init error
Kardakov  Dmitriy     25/07/2006   TLSbo71971   Add the function which configure CSI registers 
                                                and fixed random strips on display.    
Kardakov  Dmitriy     30/08/2006   TLSbo75997   Bug with capture tests was fixed. 

=============================================================================*/

#ifdef __cplusplus
extern "C"{
#endif

/*======================== INCLUDE FILES ====================================*/
/* Standard Include Files */
#include <errno.h>

/* Harness Specific Include Files. */
#include "test.h"

/* Verification Test Environment Include Files */
#include "v4l_capture_test.h"

#define ipu_fourcc(a,b,c,d)\
        (((__u32)(a)<<0)|((__u32)(b)<<8)|((__u32)(c)<<16)|((__u32)(d)<<24))

#define IPU_PIX_FMT_RGB565  ipu_fourcc('R','G','B','P') /*!< 16  RGB-5-6-5     */
#define IPU_PIX_FMT_BGR24   ipu_fourcc('B','G','R','3') /*!< 24  BGR-8-8-8     */
#define IPU_PIX_FMT_BGR32   ipu_fourcc('B','G','R','4') /*!< 32  BGR-8-8-8-8   */


/*======================== LOCAL CONSTANTS ==================================*/


const char gaPixFormat[7][10] = {
                                    "RGB565",
                                    "BGR24",
                                    "RGB24", 
                                    "BGR32",
                                    "RGB32", 
                                    "YUV422P",
                                    "YUV420"
                                };
                                
                                
const int gaPixFormatID[7] =   {
                                    V4L2_PIX_FMT_RGB565, 
                                    V4L2_PIX_FMT_BGR24, 
                                    V4L2_PIX_FMT_RGB24,  
                                    V4L2_PIX_FMT_BGR32,  
                                    V4L2_PIX_FMT_RGB32,  
                                    V4L2_PIX_FMT_YUV422P, 
                                    V4L2_PIX_FMT_YUV420
                                };
/*======================== LOCAL MACROS =====================================*/
//#define mxc91131evb_CSI_STRIPS_FIX      1
#define PIX_FMT_NUM                     7
#define CLEAR(x)                        memset(&x, 0, sizeof(x));
#define MXCFB_MEM_ADDRESS               0x83F00000
#define MAX_STR_LEN 50

/*======================== LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS) =======*/

typedef struct
{
        void    *mpStart;
        size_t   mLength;
	size_t offset;
} sBuffer;

/*======================== LOCAL VARIABLES ==================================*/
static int gFdFB = 0, gFdV4L = 0;
static unsigned char * gpFB = NULL;
static unsigned char * gpFB_buf = NULL;
static struct fb_var_screeninfo gScreenInfo;
static struct fb_fix_screeninfo fScreenInfo;
static struct fb_var_screeninfo var_info;
static struct fb_var_screeninfo old_var_info;
static unsigned long gVideoBufferSize;

static sBuffer * gpBuffers = NULL;
static unsigned int gBuffNumber = 0;
static struct v4l2_format gFormat;

static struct v4l2_framebuffer fbuffer;
static struct v4l2_framebuffer fbuffer_save;

static int gPixelFormat = V4L2_PIX_FMT_RGB565;
static char gPixFmtName[40] = "RGB565";
static char gOrigPixFmtName[40] = "Unknown";
static FILE* pDumpFile = NULL;
static int gFBPixFormat = V4L2_PIX_FMT_RGB565;//V4L2_PIX_FMT_BGR24;
static char gFBPixFmtName[40] = "RGB565";

static int gUsecase = V4L2_BUF_TYPE_VIDEO_CAPTURE;

static int gSnapshot = 0;
static int gStartStream = 0;

struct v4l2_rect gOrigCropRect;
struct v4l2_rect gOrigFormatRect;

#ifndef MAD_TEST_MODIFY
struct v4l2_cropcap cropcap;
struct v4l2_crop crop;  
#endif

unsigned long gOrigPixFormat = V4L2_PIX_FMT_RGB565;

static int inSrc = -1;
/*======================== GLOBAL CONSTANTS =================================*/


/*======================== GLOBAL VARIABLES =================================*/


/*======================== LOCAL FUNCTION PROTOTYPES ========================*/

int parse_options(void);
int open_device (void);
int close_device (void);
int open_out_device (void);
int close_out_device (void);
int start_capturing (void);
int stop_capturing (void);
int init_device (void);
int init_overlay(void);
int init_capture(void);
int init_mmap(void);
int detect_fb_fmt(void);
int uninit_device (void);
int config_device_for_rotation(int aRotNum);
int reset_device(void);
int process_image(const unsigned char *aStart, int aLength);
int read_frame(void);
int write_file_header(void);
void write_zero_to_file(FILE* apFile, int aBytes);
int ask_user(void);
void display_to_fb(unsigned char * aStart, int aLength);
int do_cropping(void);
int setup_device(void);
int cleanup_device(void);
#ifdef mxc91131evb_CSI_STRIPS_FIX
int register_conf(int div_ratio);
#endif


/*======================== LOCAL FUNCTIONS ==================================*/

/*===== parse_options =====*/
/**
@brief  Parse options

@param  None

@return On success - return TPASS
        On failure - return the error code
*/
int parse_options(void)
{
        /* Parse use case by buffer type */

        gUsecase = V4L2_BUF_TYPE_VIDEO_CAPTURE;

        switch(gV4LTestConfig.mCaseNum)
        {
                case PRP_VF:
                        gUsecase = V4L2_BUF_TYPE_VIDEO_OVERLAY;
                        break;
                case PRP_ENC_ON_D:
                case PRP_ENC_TO_F:
                        gUsecase = V4L2_BUF_TYPE_VIDEO_CAPTURE;
                        break;
                default:
                        tst_resm(TWARN,"Case number is erroneous : %d",gV4LTestConfig.mCaseNum);
                        return TFAIL;
        }

        /* Parse pixel format */

        if(gV4LTestConfig.mOutputFormat)
        {
                int i = 0;
                for(i=0; i < PIX_FMT_NUM; i++)
                {
                        if(!strcasecmp(gV4LTestConfig.mPixFormat,gaPixFormat[i]))
                        {
                                gPixelFormat = gaPixFormatID[i];
                                strcpy(gPixFmtName,gV4LTestConfig.mPixFormat);
                                break;
                        }
                }
    
                /* If no parsed format in the array of formats */
                if(i == PIX_FMT_NUM)  
                {
                        tst_resm(TWARN, "The pixel format %s is not supported by the driver", gV4LTestConfig.mPixFormat);
                        return TFAIL;
                }
        }
        
        return TPASS;
}

/*===== open_device =====*/
/**
@brief  Open V4L2 device

@param  None

@return On success - return TPASS
        On failure - return the error code
*/
int open_device(void)
{
        /* Open Video4Linux Device */

        struct stat st;
        
        if(parse_options() == TFAIL)
        {
                tst_resm(TBROK, "Option parsing error");
                return TFAIL;        
        }
        
        if(stat (gV4LTestConfig.mV4LDevice, &st) < 0)
        {
                tst_resm(TBROK, "Cannot identify '%s': %d, %s", gV4LTestConfig.mV4LDevice, errno, strerror (errno));
                return TFAIL;
        }    

        if(!S_ISCHR (st.st_mode))
        {
                tst_resm(TBROK, "%s is not a char device", gV4LTestConfig.mV4LDevice);
                return TFAIL;
        }

	if ( gV4LTestConfig.mIsBlock == 0 )
	{
        	if((gFdV4L = open(gV4LTestConfig.mV4LDevice, O_RDWR|O_NONBLOCK, 0)) < 0)
        	{
                tst_resm(TBROK, "Unable to open %s", gV4LTestConfig.mV4LDevice);
                return TFAIL;
       		}	
	}else{
	
        	if((gFdV4L = open(gV4LTestConfig.mV4LDevice, O_RDWR, 0)) < 0)
        	{
                tst_resm(TBROK, "Unable to open %s", gV4LTestConfig.mV4LDevice);
                return TFAIL;
		}
	}

        return TPASS;
}

/*===== close_device =====*/
/**
@brief  Close V4L2 device

@param  None

@return On success - return TPASS
        On failure - return the error code
*/
int close_device(void)
{
        int retValue = TPASS;
	
	if( gFdV4L > 0 && inSrc != -1)
	   ioctl(gFdV4L, VIDIOC_S_INPUT, &inSrc);

        if((gFdV4L > 0) && (close(gFdV4L) < 0))
        {
                tst_resm(TBROK, "Unable to close %s", gV4LTestConfig.mV4LDevice);
                retValue = TFAIL;
        }

        if ( gpFB_buf != NULL ) 
                free(gpFB_buf);
 
        return retValue;
}

/*===== open_out_device =====*/
/**
@brief  Open output device

@param  None

@return On success - return TPASS
        On failure - return the error code
*/
int open_out_device(void)
{
        /* Open FrameBuffer Device */

        if((gFdFB = open(gV4LTestConfig.mOutputDevice, O_RDWR)) < 0)
        {
                tst_resm(TBROK, "Unable to open frame buffer");
                return TFAIL;
        }
    
        //Set the BGR24 framebuffer format 
        memset(&var_info, 0, sizeof(var_info) );
        if(ioctl(gFdFB, FBIOGET_VSCREENINFO, &var_info))
        {
                tst_resm(TBROK, "Unable to read FB information");
                return TFAIL;
        }
    
        memcpy(&old_var_info, &var_info, sizeof(var_info));
    
        var_info.red.offset = 11;
        var_info.green.offset = 5;
        var_info.blue.offset = 0;
    
        var_info.red.length = 5;
        var_info.green.length = 6;
        var_info.blue.length = 5;
        var_info.bits_per_pixel = 16;
   #if 0
        if(ioctl(gFdFB, FBIOPUT_VSCREENINFO, &var_info))
        {
                tst_resm(TBROK, "Unable to write FB information");
                return TFAIL;
        }
#endif
    
        if(ioctl(gFdFB, FBIOGET_VSCREENINFO, &gScreenInfo))
        {
                tst_resm(TBROK, "Unable to read FB information");
                return TFAIL;
        }
        else
        {
                if(gV4LTestConfig.mVerbose) 
                {
                        tst_resm(TINFO,"\t Framebuffer Width = %d\n", gScreenInfo.xres);
                        tst_resm(TINFO,"\t Framebuffer Height = %d\n", gScreenInfo.yres);
                }       
        }

        gVideoBufferSize = gScreenInfo.xres * gScreenInfo.yres * gScreenInfo.bits_per_pixel / 8;

        if((gpFB = mmap(0, gVideoBufferSize, PROT_READ | PROT_WRITE, MAP_SHARED, gFdFB, 0)) < 0)
        {
                tst_resm(TBROK, "Error: failed to map framebuffer device to memory.");
                return TFAIL;
        }
        
        return TPASS;
}

/*===== close_out_device =====*/
/**
@brief  Close output device

@param  None

@return On success - return TPASS
        On failure - return the error code
*/
int close_out_device(void)
{
        int retValue = TPASS;
    
        if((gFdFB > 0) && (close(gFdFB) < 0))
        {
                tst_resm(TBROK, "Unable to close %s", gV4LTestConfig.mOutputDevice);
                retValue = TFAIL;
        }
        
        return retValue;  
}



/*===== init_overlay =====*/
/**
@brief  Video overlay device initialization

@param  None

@return On success - return TPASS
        On failure - return the error code
*/
int init_overlay(void)
{
        struct v4l2_streamparm streamParm;
        v4l2_std_id stdID;
        int displayLCD = 0; 

        /* Get Frame Buffer overlay parametrs */
        if(ioctl(gFdV4L, VIDIOC_G_FBUF, &fbuffer) < 0)
        {
                tst_resm(TWARN, "Error VIDIOC_G_FBUF : Overlay is not supported by device %s", gV4LTestConfig.mV4LDevice);
                return TFAIL;
        }
        /* save the original values */
        memcpy(&fbuffer_save, &fbuffer, sizeof(fbuffer));;

        memset(&fbuffer, 0, sizeof(fbuffer) );
        fbuffer.flags = V4L2_FBUF_FLAG_OVERLAY;

        /* set framebuffer info for primary type overlay */         
        if(gV4LTestConfig.mOverlayType == V4L2_FBUF_FLAG_PRIMARY)
        {
                if((gFdFB = open(gV4LTestConfig.mOutputDevice, O_RDWR)) < 0)
                {
                        tst_resm(TBROK, "Unable to open frame buffer");
                        return TFAIL;
                }
  
                if(ioctl(gFdFB, FBIOGET_VSCREENINFO, &gScreenInfo))
                {
                        tst_resm(TBROK, "Unable to read FB var information");
                        return TFAIL;
                }

                if(ioctl(gFdFB, FBIOGET_FSCREENINFO, &fScreenInfo))
                {
                        tst_resm(TBROK, "Unable to read FB fix information");
                        return TFAIL;
                }
                
        
                fbuffer.fmt.width  = gScreenInfo.xres;
                fbuffer.fmt.height = gScreenInfo.yres;
                fbuffer.fmt.bytesperline = gScreenInfo.bits_per_pixel/8*fbuffer.fmt.width;
                
                fbuffer.fmt.pixelformat = IPU_PIX_FMT_RGB565;
                if(gScreenInfo.bits_per_pixel == 24 )
                {
                        fbuffer.fmt.pixelformat = IPU_PIX_FMT_BGR24;
                }
                if(gScreenInfo.bits_per_pixel == 32 )
                {
                        fbuffer.fmt.pixelformat = IPU_PIX_FMT_BGR24;
                }

                fbuffer.base = (void*)fScreenInfo.smem_start;
                fbuffer.fmt.sizeimage = fbuffer.fmt.bytesperline * fbuffer.fmt.height;
                fbuffer.flags = V4L2_FBUF_FLAG_PRIMARY;
        }

         
        /* Get original format */
        CLEAR(gFormat);

        gFormat.type = V4L2_BUF_TYPE_VIDEO_OVERLAY;

        if(ioctl(gFdV4L, VIDIOC_G_FMT, &gFormat) < 0)
        {
                tst_resm(TWARN, "%s formatting failed", gV4LTestConfig.mV4LDevice);
                return TFAIL;
        }
        streamParm.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        streamParm.parm.capture.timeperframe.numerator = 1;
        streamParm.parm.capture.timeperframe.denominator = gV4LTestConfig.mFrameRate;
        streamParm.parm.capture.capturemode =  gV4LTestConfig.mMode;
        
        
        if(ioctl(gFdV4L, VIDIOC_S_PARM, &streamParm) < 0)
        {
                tst_resm(TWARN,"ERROR init_overlay() : set frame rate failed");
                return TFAIL;
        } 
        
        streamParm.parm.capture.timeperframe.numerator = 0;
        streamParm.parm.capture.timeperframe.denominator = 0;
                
        if(ioctl(gFdV4L, VIDIOC_G_PARM, &streamParm) < 0)
        {
                tst_resm(TWARN,"ERROR init_overlay() : get frame rate failed");
                return TFAIL;
        } 


        gOrigFormatRect.left = gFormat.fmt.win.w.left;
        gOrigFormatRect.top = gFormat.fmt.win.w.top;
        gOrigFormatRect.width = gFormat.fmt.win.w.width;
        gOrigFormatRect.height = gFormat.fmt.win.w.height; 
        
        if(gV4LTestConfig.mVerbose)
        {
                tst_resm(TINFO,"Original image size: left = %d, top = %d, width = %d, height = %d",
                        gOrigFormatRect.left, gOrigFormatRect.top, gOrigFormatRect.width, gOrigFormatRect.height);        
        }
         
        if(gV4LTestConfig.mCrop)
        {
                if(do_cropping())
                        return TFAIL;
        }
        
        
       gFormat.type = V4L2_BUF_TYPE_VIDEO_OVERLAY;
        /*gFormat.fmt.pix.pixelformat = IPU_PIX_FMT_RGB565;*/
	if(gV4LTestConfig.mCrop)
	{
	gFormat.fmt.win.w.left = crop.c.left;
        gFormat.fmt.win.w.top = crop.c.top;
        gFormat.fmt.win.w.height = crop.c.width;
        gFormat.fmt.win.w.width = crop.c.height;
	
	tst_resm(TINFO,"Crop to image: left = %d, top = %d, width = %d, height = %d", gFormat.fmt.win.w.left, gFormat.fmt.win.w.top, gFormat.fmt.win.w.width, gFormat.fmt.win.w.height); 
	}else{
	gFormat.fmt.win.w.left = 0;
        gFormat.fmt.win.w.top = 0;
        gFormat.fmt.win.w.height = gV4LTestConfig.mHeight;
        gFormat.fmt.win.w.width = gV4LTestConfig.mWidth;
        
	tst_resm(TINFO,"Default image: left = %d, top = %d, width = %d, height = %d", gFormat.fmt.win.w.left, gFormat.fmt.win.w.top, gFormat.fmt.win.w.width, gFormat.fmt.win.w.height);
	}
        if(gV4LTestConfig.mRotation)
        {
                if(config_device_for_rotation(gV4LTestConfig.mRotationMode))
                        return TFAIL;                             
        }

        if(ioctl(gFdV4L, VIDIOC_S_FMT, &gFormat) < 0)
        {
                tst_resm(TWARN,"ERROR init_overlay() : set format failed with code %d", errno);
                return TFAIL;
        }
                
        if(ioctl(gFdV4L, VIDIOC_G_FMT, &gFormat) < 0)
        {
                tst_resm(TWARN,"ERROR init_overlay() : get format failed");
                return TFAIL;
        }
	/*
        if(ioctl(gFdV4L, VIDIOC_G_STD, &stdID) < 0)
        {
                tst_resm(TWARN,"ERROR init_overlay() : VIDIOC_G_STD failed");
                return TFAIL;
        }
        if(ioctl(gFdV4L, VIDIOC_S_OUTPUT, &displayLCD) < 0)
        {
                tst_resm(TWARN,"ERROR init_overlay() : VIDIOC_S_OUTPUT failed");
                return TFAIL;
        } 
        streamParm.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        streamParm.parm.capture.timeperframe.numerator = 1;
        streamParm.parm.capture.timeperframe.denominator = gV4LTestConfig.mFrameRate;
        streamParm.parm.capture.capturemode =  gV4LTestConfig.mMode;
        
        if(ioctl(gFdV4L, VIDIOC_S_PARM, &streamParm) < 0)
        {
                tst_resm(TWARN,"ERROR init_overlay() : set frame rate failed");
                return TFAIL;
        } 
        
        streamParm.parm.capture.timeperframe.numerator = 0;
        streamParm.parm.capture.timeperframe.denominator = 0;
                
        if(ioctl(gFdV4L, VIDIOC_G_PARM, &streamParm) < 0)
        {
                tst_resm(TWARN,"ERROR init_overlay() : get frame rate failed");
                return TFAIL;
        } 


	*/
        /* Set Frame Buffer Format */
        if(ioctl(gFdV4L, VIDIOC_S_FBUF, &fbuffer) < 0)
        {
                switch(errno)
                {
                        case EACCES:
                                tst_resm(TWARN, "Error VIDIOC_S_FBUF can only be called by a privileged user");
                                break;
                        case EINVAL:
                                tst_resm(TWARN, "The VIDIOC_S_FBUF parameters are unsuitable");
                                break;
                        default:
                                tst_resm(TWARN, "%s",strerror(errno));
                                break;                
                }
                return TFAIL;
        }
        
        tst_resm(TINFO, "VIDIOC_S_FBUF executed - overlay type %s is set", 
                gV4LTestConfig.mOverlayType==V4L2_FBUF_FLAG_PRIMARY?"PRIMARY":"OVERLAY");             
         
 
    
        return TPASS;
}

/*===== init_capture =====*/
/**
@brief  Video capture device initialization

@param  None

@return On success - return TPASS
        On failure - return the error code
*/
int init_capture(void)
{

        /* Open Frame Buffer Device */

        if(gV4LTestConfig.mCaseNum == PRP_ENC_ON_D)
                if(open_out_device() == TFAIL) 
                        return TFAIL;
      
        /* Get original format */
        
        CLEAR(gFormat);

        gFormat.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

        if(ioctl(gFdV4L, VIDIOC_G_FMT, &gFormat) < 0)
        {
                tst_resm(TWARN, "%s formatting failed", gV4LTestConfig.mV4LDevice);
                return TFAIL;
        }
        
        gOrigFormatRect.width = gFormat.fmt.pix.width;
        gOrigFormatRect.height = gFormat.fmt.pix.height;
        gOrigPixFormat = gFormat.fmt.pix.pixelformat;
        
        int i = 0;
        
        for(i=0; i < PIX_FMT_NUM; i++)
        {
                if(gOrigPixFormat == gaPixFormatID[i])
                {
                        memset(gOrigPixFmtName, 0, 40);
                        strcpy(gOrigPixFmtName, gaPixFormat[i]);
                        break;
                }
        }
        
        if(gV4LTestConfig.mVerbose)
        {
                tst_resm(TINFO,"Original image size: width = %d, height = %d",
                        gOrigFormatRect.width, gOrigFormatRect.height);        
                tst_resm(TINFO,"Original pixel format: %s",gOrigPixFmtName);
        }

        /* Set format */

        CLEAR(gFormat);
        
        gFormat.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        gFormat.fmt.pix.width       = gV4LTestConfig.mWidth;
        gFormat.fmt.pix.height      = gV4LTestConfig.mHeight;
        gFormat.fmt.pix.pixelformat = gPixelFormat;

        if(ioctl(gFdV4L, VIDIOC_S_FMT, &gFormat) < 0)
        {
                tst_resm(TWARN, "%s formatting failed", gV4LTestConfig.mV4LDevice);
                return TFAIL;
        }

        /* Verification pixel format of the device */
        
        if(ioctl(gFdV4L, VIDIOC_G_FMT, &gFormat) < 0)
        {
                tst_resm(TWARN, "%s formatting failed", gV4LTestConfig.mV4LDevice);
                return TFAIL;
        }
        
        if(gFormat.fmt.pix.pixelformat != gPixelFormat)
        {
                tst_resm(TWARN, "Pixel format %s is not supported by device %s", gPixFmtName, gV4LTestConfig.mV4LDevice);
                return TFAIL;
        }   

        if ( (gpFB_buf = (unsigned char*) malloc(gFormat.fmt.pix.sizeimage)) == NULL)
        {
                tst_resm(TFAIL, "gpFB_buf malloc error!");
                return TFAIL;
        }
        
        if(gV4LTestConfig.mVerbose) 
        {
                tst_resm(TINFO,"\tCapture : Format image width = %d", gFormat.fmt.pix.width);
                tst_resm(TINFO,"\tCapture : Format image height = %d", gFormat.fmt.pix.height);
        }

	struct v4l2_streamparm parm;
	memset(&parm, 0, sizeof(parm));
	parm.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
   	parm.parm.capture.timeperframe.numerator = 1;
  	parm.parm.capture.timeperframe.denominator = gV4LTestConfig.mFrameRate;
	parm.parm.capture.capturemode = 0;
     
	if (ioctl(gFdV4L, VIDIOC_S_PARM, &parm) < 0)
	{
		tst_resm(TFAIL, "set parm error!");
		close(gFdV4L);
		return TFAIL;
	} 
        
        /* Set rotation */
        
        if(gV4LTestConfig.mRotation)
        {
                if(config_device_for_rotation(gV4LTestConfig.mRotationMode))
                        return TFAIL;                             
        }
        
        return TPASS;
}


/*===== init_mmap =====*/
/**
@brief  Initiate Memory Mapping

@param  None

@return On success - return TPASS
        On failure - return the error code
*/
int init_mmap(void)
{
        struct v4l2_requestbuffers reqBuffers;
        struct v4l2_buffer buffer; 
  
        CLEAR(reqBuffers);  
        
        reqBuffers.count = 3;  /* 4 doesn't work */
        reqBuffers.type = gUsecase;  
        reqBuffers.memory = V4L2_MEMORY_MMAP;

        if(ioctl (gFdV4L, VIDIOC_REQBUFS, &reqBuffers) < 0)
        {
                tst_resm(TWARN, "%s does not support memory mapping : %s", gV4LTestConfig.mV4LDevice, strerror(errno));
                return TFAIL;
        }

        if(reqBuffers.count < 2)
        {
                tst_resm(TWARN, "Insufficient buffer memory on %s", gV4LTestConfig.mV4LDevice);
                return TFAIL;
        }

        if(!(gpBuffers = calloc(reqBuffers.count, sizeof(*gpBuffers))))
        {
                tst_resm(TWARN, "Out of memory");
                return TFAIL;
        }

        for(gBuffNumber = 0; gBuffNumber < reqBuffers.count; gBuffNumber++)
        {    
                                  
                CLEAR(buffer);    
                
                buffer.type = gUsecase;    
                buffer.memory = V4L2_MEMORY_MMAP;    
                buffer.index = gBuffNumber;    
                
                if (ioctl(gFdV4L, VIDIOC_QUERYBUF, &buffer) < 0)    
                {      
                        tst_resm(TWARN, "VIDIOC_QUERYBUF error");      
                        return TFAIL;    
                } 
                       
                gpBuffers[gBuffNumber].mpStart = mmap (NULL, buffer.length,
                                                 PROT_READ | PROT_WRITE, MAP_SHARED,
                                                 gFdV4L, buffer.m.offset);
		  gpBuffers[gBuffNumber].mLength = buffer.length;
		  gpBuffers[gBuffNumber].offset = buffer.m.offset;
                if(gpBuffers[gBuffNumber].mpStart == MAP_FAILED)
                {
                        tst_resm(TWARN, "Buffers mapping failed");      
                        return TFAIL;    
                }  
        }  
        return TPASS;
}

/*===== start_capturing =====*/
/**
@brief  start capturing

@param  None

@return On success - return TPASS
        On failure - return the error code
*/

int start_capturing (void)
{  
        unsigned int i;  
        struct v4l2_buffer buffer;  
        enum v4l2_buf_type typeBuffer = gUsecase;
        unsigned long start = 0;
         
        switch(gUsecase)
        {    
                case V4L2_BUF_TYPE_VIDEO_CAPTURE:   
                       
                        for(i = 0; i < gBuffNumber; i++)
                        {           
                                memset(&buffer, 0, sizeof (buffer));      
                                buffer.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;     
                                buffer.memory = V4L2_MEMORY_MMAP; 
				    buffer.m.offset = gpBuffers[i].offset;
                                buffer.index = i;
                                
                                if (ioctl (gFdV4L, VIDIOC_QBUF, &buffer) < 0)
                                { 
                                        tst_resm(TBROK,"VIDIOC_QBUF failed. ERROR : %s", strerror(errno));
                                        return TFAIL;
                                }     
                        }
                       
                        if(ioctl (gFdV4L, VIDIOC_STREAMON, &typeBuffer) < 0)
                        {
                                tst_resm(TBROK,"VIDIOC_STREAMON failed. ERROR : %s", strerror(errno));
                                return TFAIL;
                        }
                        
                        break;     
                        
                case V4L2_BUF_TYPE_VIDEO_OVERLAY: 
                       {
		        #define MX25_TEMP_SLT 0
		        #if MX25_TEMP_SLT
		        int fd_fb = 0;
			struct fb_fix_screeninfo fix;
			if ((fd_fb = open("/dev/fb0", O_RDWR )) < 0) {
			 printf("Unable to open frame buffer\n");
			 return TFAIL;
			}
			if (ioctl(fd_fb, FBIOGET_FSCREENINFO, &fix) < 0) {
			 printf("Unable to get frame buffer parameter\n");
			 return TFAIL;
			}

                       #endif

                        if(gV4LTestConfig.mVerbose) 
                        {
                                tst_resm(TINFO,"OVERLAY MODE");
                        }
                        #if MX25_TEMP_SLT
                        start = fix.smem_start;
			printf("smem_start = %ul \n", start);
                        #else
                        start = 1;
			#endif
                        if(ioctl (gFdV4L, VIDIOC_OVERLAY, &start) < 0) 
                        {
                                tst_resm(TWARN,"Error start_capturing() for VIDIOC_OVERLAY : %s",strerror(errno));
                                return TFAIL;
                        }

                        
                        tst_resm(TINFO,"Please wait %d sec ... \n",gV4LTestConfig.mCount);
                        
                        sleep(gV4LTestConfig.mCount);
                        }         
                        break;
                        
                default:          
                        tst_resm(TWARN,"The buffer type %d is not supported",gUsecase);
                        return TFAIL;
        }
        
        gStartStream = 1;
        
        return TPASS;
}

/*===== stop_capturing =====*/
/**
@brief  stop capturing

@param  None

@return On success - return TPASS
        On failure - return the error code
*/

int stop_capturing (void)
{  
        enum v4l2_buf_type typeBuffer = gUsecase;
        int stop = 1;

        if(!gStartStream) 
                return TPASS;
  
        switch(gUsecase)
        {
                case V4L2_BUF_TYPE_VIDEO_CAPTURE: 
                         
                        typeBuffer = V4L2_BUF_TYPE_VIDEO_CAPTURE;          
                        
                        if(ioctl (gFdV4L, VIDIOC_STREAMOFF, &typeBuffer) < 0)          
                        {            
                                tst_resm(TWARN,"Error stop_capturing() for VIDIOC_STREAMOFF : %s",strerror(errno));
                                return TFAIL;
                        }
                        
                        break;
                        
                case V4L2_BUF_TYPE_VIDEO_OVERLAY:
                
                        stop = 0;          
                        
                        if(ioctl (gFdV4L, VIDIOC_OVERLAY, &stop) < 0)          
                        {            
                                tst_resm(TWARN,
                                        "Error stop_capturing() for VIDIOC_OVERLAY : %s",
                                        strerror(errno));                       
                                return TFAIL;
                        }          
                        
                        break;     
                
                default:          
                
                        tst_resm(TWARN,"The buffer type %d is not supported",gUsecase);          
                        return TFAIL;          
        }
  
        return TPASS;
}

/*===== init_device =====*/
/**
@brief  init V4L device

@param  None

@return On success - return TPASS
        On failure - return the error code
*/

int init_device (void)
{  
        struct v4l2_capability cap;
        
        /* Init device */
     
        if(ioctl (gFdV4L, VIDIOC_QUERYCAP, &cap) < 0)
        {
                tst_resm(TWARN, "%s is not a v4l2 device", gV4LTestConfig.mV4LDevice);
                return TFAIL;
        }

	#if 1
        if( gV4LTestConfig.inputSrc == eInCSI_IC_MEM)
	{
	  int index = eInCSI_IC_MEM;
	  ioctl (gFdV4L, VIDIOC_G_INPUT, &inSrc);
	  if(ioctl (gFdV4L, VIDIOC_S_INPUT, &index)< 0)
	  {
            tst_resm(TFAIL, "select input CSI_IC_MEM FAIL" );
	    return TFAIL;
	  }
	}else if( gV4LTestConfig.inputSrc == eInCSI_MEM){
	  int index = eInCSI_MEM;
	  ioctl (gFdV4L, VIDIOC_G_INPUT, &inSrc);
	  if(ioctl (gFdV4L, VIDIOC_S_INPUT, &index)< 0)
	  {
            tst_resm(TFAIL, "select input CSI_MEM FAIL" );
	    return TFAIL;
	  }
	}else
	;/*no selection*/

	#endif
        
        if(gUsecase == V4L2_BUF_TYPE_VIDEO_CAPTURE)
        {

                if(!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE))
                {
                        tst_resm(TWARN, "%s does not support capturing", gV4LTestConfig.mV4LDevice);
                        return TFAIL;
                }
        
                if(!(cap.capabilities & V4L2_CAP_STREAMING))
                {      
                        tst_resm(TWARN, "%s does not support streaming I/O", gV4LTestConfig.mV4LDevice);      
                        return TFAIL;    
                }
        
                if(init_capture() == TFAIL) 
                        return TFAIL;
    
                if(init_mmap() == TFAIL)
                        return TFAIL;
    
                if(gV4LTestConfig.mCaseNum == PRP_ENC_ON_D)
                        if(detect_fb_fmt() == TFAIL) 
                                return TFAIL;
        }
        else
        {
                
                if(!(cap.capabilities & V4L2_CAP_VIDEO_OVERLAY))
                {      
                        tst_resm(TWARN, "%s does not support overlaying", gV4LTestConfig.mV4LDevice);      
                        return TFAIL;    
                }    
                
                if(init_overlay() == TFAIL) 
                        return TFAIL;  
        }
#ifdef mxc91131evb_CSI_STRIPS_FIX        
        if ( register_conf(4) != TPASS ) 
                return TFAIL;
#endif
 
        return TPASS;
}

/*===== uninit_device =====*/
/**
@brief  uninit V4L device

@param  None

@return On success - return TPASS
        On failure - return the error code
*/

int uninit_device (void)
{
        int retValue = TPASS;
        unsigned int i;
  
        if(gUsecase == V4L2_BUF_TYPE_VIDEO_CAPTURE)  
        {    
                for(i = 0; i < gBuffNumber; i++)    
                {      
                        if(munmap(gpBuffers[i].mpStart, gpBuffers[i].mLength) < 0)      
                                retValue = TFAIL;    
                }
        }  
        else
        {
                ioctl(gFdV4L, VIDIOC_S_FBUF, &fbuffer_save);
        }
        
        if(gpBuffers != NULL) 
                free(gpBuffers);  
        
        
        if(close_out_device() == TFAIL) 
                return TFAIL;
  
        return retValue;
}

/*===== config_device_for_rotation =====*/
/**
@brief  config V4L device config for rotation

@param  aRotNum - rotation mumber

@return On success - return TPASS
        On failure - return the error code
*/

int config_device_for_rotation(int aRotNum)
{
        struct v4l2_control control;
        
        if(gV4LTestConfig.mRotation)
        {    
                memset(&control, 0, sizeof(control));    
        
                control.id = V4L2_CID_PRIVATE_BASE;
                control.value = aRotNum;
                
                switch(aRotNum)   /* print rotation type */
                {
                        case 0:
                               /* no rotation */
                                break;
                        case 1:
                                tst_resm(TINFO, "Rotation type: vertical flip");
                                break;
                        case 2:
                                tst_resm(TINFO, "Rotation type: horizontal flip");
                                break;
                        case 3:
                                tst_resm(TINFO, "Rotation type: on 180 degrees");
                                break;
                        case 4:
                                tst_resm(TINFO, "Rotation type: on 90 degrees clock-wise");
                                break;
                        case 5:
                                tst_resm(TINFO, "Rotation type: on 90 degrees clock-wise + "
                                                "vertical flip");
                                break;
                        case 6:
                                tst_resm(TINFO, "Rotation type: on 90 degrees clock-wise + "
                                                "horizontal flip");
                                break;
                        case 7:
                                tst_resm(TINFO, "Rotation type: on 90 degrees counter-clock-wise");
                                break;
                        default:
                                tst_resm(TINFO, "Rotation type : unknown");
                } 

                /* Perform the rotation */
                if (ioctl(gFdV4L, VIDIOC_S_CTRL, &control) < 0)
                {
                        tst_resm(TBROK, "Error: Rotation configuration failed");
                        return TFAIL;
                }
                                          
                /* Check the result  */
                if (ioctl(gFdV4L, VIDIOC_G_CTRL, &control) < 0)
                {
                        if(control.value != aRotNum)
                        {    
                            tst_resm(TBROK, "Error: Rotation option %d is not supported",aRotNum);
                            return TFAIL;
                        }
                }
                                                                                                                
        }
        
        return TPASS;
}

/*===== reset_device =====*/
/**
@brief  V4L device reset

@param  None

@return On success - return TPASS
        On failure - return the error code
*/

int reset_device(void)
{
        struct v4l2_crop crop;  
        
      
 
        config_device_for_rotation(0);
        
        if(gV4LTestConfig.mCrop)
        {
                CLEAR(crop);
   
                crop.type = gUsecase;
                
                crop.c.left   = gOrigCropRect.left;
                crop.c.top    = gOrigCropRect.top;
                crop.c.width  = gOrigCropRect.width;
                crop.c.height = gOrigCropRect.height;
    
                if(ioctl (gFdV4L, VIDIOC_S_CROP, &crop) < 0) 
                {
                        tst_resm(TBROK, "Error: VIDIOC_S_CROP ioctl failed");
                        return TFAIL;
                }
        
                if(gV4LTestConfig.mVerbose)
                {
                        tst_resm(TINFO,"Reinit to default cropping settings: left = %d, top = %d, width = %d, height = %d", 
                                crop.c.left, crop.c.top, crop.c.width, crop.c.height);
                }
        }
        
        
        CLEAR(gFormat);
         
        gFormat.type = gUsecase;      
        
        if(gUsecase == V4L2_BUF_TYPE_VIDEO_OVERLAY)   
        {
                gFormat.fmt.win.w.left = gOrigFormatRect.left;
                gFormat.fmt.win.w.top = gOrigFormatRect.top;
                gFormat.fmt.win.w.height = gOrigFormatRect.height;
                gFormat.fmt.win.w.width = gOrigFormatRect.width;
        }
        else
        {
                gFormat.fmt.pix.width       = gOrigFormatRect.width;
                gFormat.fmt.pix.height      = gOrigFormatRect.height;
                gFormat.fmt.pix.pixelformat = gOrigPixFormat; 
        }
               
        if(ioctl(gFdV4L, VIDIOC_S_FMT, &gFormat) < 0)
        {
                tst_resm(TWARN, "%s formatting failed", gV4LTestConfig.mV4LDevice);
                return TFAIL;
        }
        
        if(gV4LTestConfig.mVerbose)
        {
                if(gUsecase == V4L2_BUF_TYPE_VIDEO_OVERLAY)   
                        tst_resm(TINFO,"Reinit to default image size: left = %d, top = %d, width = %d, height = %d",
                                gFormat.fmt.win.w.left, gFormat.fmt.win.w.top, gFormat.fmt.win.w.width, gFormat.fmt.win.w.height);        
                else
                {
                        tst_resm(TINFO,"Reinit to default image size:  width = %d, height = %d", gFormat.fmt.pix.width, gFormat.fmt.pix.height);  
                        if(gFormat.fmt.pix.pixelformat == gOrigPixFormat)
                                tst_resm(TINFO,"Reinit to default pixel format: %s",gOrigPixFmtName); 
                        else  
                                tst_resm(TINFO,"Not reinit to default pixel format: %s",gPixFmtName); 
                }
        }
 
        return TPASS;
}

/*===== process_image =====*/
/**
@brief  Copy image to framebuffer

@param  *aStart - capture buffer ptr

@return On success - return TPASS
        On failure - return the error code
*/

int process_image(const unsigned char *aStart, int aLength)
{
        switch(gV4LTestConfig.mCaseNum)
        {     
                /* Write to file */
                case PRP_ENC_TO_F:  
                                       
                        if(gSnapshot)      
                        {        
                                tst_resm(TINFO,"Writting to dump file...");     
                                
                                char outFileName[MAX_STR_LEN];   
                                
                                sprintf(outFileName,"%s%s%s",gV4LTestConfig.mOutputFile,"_",gPixFmtName);    
                    
                                if (!(pDumpFile = fopen(outFileName,"ab")))        
                                {        
                                        tst_resm(TWARN, 
                                                 "Error process_image() : Unable to open file %s", 
                                                 gV4LTestConfig.mOutputFile);   
                                                        
                                        return TFAIL;
                                }        
                                else    
                                {         
                                
                                        int i = 0;
                                        
                                        if(gV4LTestConfig.mVerbose) 
                                        {
                                                tst_resm(TINFO,"Sizeimage = %d", gFormat.fmt.pix.sizeimage);
                                        }    

                                        unsigned char *pData = (unsigned char *)aStart;          
                                        
                                        while(i < gFormat.fmt.pix.sizeimage)         
                                        {   
                                                fwrite(pData,sizeof(unsigned char),1,pDumpFile);            
                                                pData++;            
                                                i++;      
                                        }         

                                        fclose(pDumpFile);          
                                        pDumpFile = NULL;   
                                }

                                tst_resm(TINFO,"Dump file is written!");
                        }
                break; 
             
                /* Show on display */    
                case PRP_ENC_ON_D:  
              
                        /* Here conversion of buffer pix foramt to FB pix format */

                        if(gFBPixFormat == gPixelFormat)      
                        {
                                if(gV4LTestConfig.mVerbose) 
                                {
                                        tst_resm(TINFO,"process_image() : displaying on framebuffer...");
                                }

                                sleep(1);
                                display_to_fb((void*)aStart,aLength);
                        }
                        else
                        {        
                                tst_resm(TWARN,
                                        "Pixel format %s is not supported by frame buffer",
                                        gPixFmtName);
                                        
                                return TFAIL;
                        }
                break;
                
                /* Overlay mode */
                case PRP_VF:

                        if(gV4LTestConfig.mVerbose) 
                        {
                                tst_resm(TINFO,"process_image() : displaying overlay...");
                        }
                break;

        }
 
        return TPASS;
}


/*===== read_frame =====*/
/**
@brief  Reading frame

@param  None

@return On success - return TPASS
        On failure - return the error code
*/
int read_frame(void)
{  
        struct v4l2_buffer buffer;
        void *aStart;

        switch(gUsecase)
        {
                case V4L2_BUF_TYPE_VIDEO_CAPTURE:

                        if(gV4LTestConfig.mVerbose) 
                        {
                                tst_resm(TINFO,"read_frame() for Encoder");
                        }
	AGAIN:
                        CLEAR(buffer); 
                             
                        buffer.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;      
                        buffer.memory = V4L2_MEMORY_MMAP;      
                        
                        if(ioctl (gFdV4L, VIDIOC_DQBUF, &buffer) < 0)      
                        {       
                                if(gV4LTestConfig.mVerbose) 
                                {                                
                                        tst_resm(TINFO,"ERROR read_frame(): %s",strerror(errno));
                                } 
                                              
                                switch(errno)        
                                {          
                                        case EAGAIN:  
					/*
                                                tst_resm(TWARN,
                                                         "Non-blocking I/O has been selected using O_NONBLOCK and no buffer was in the outgoing queue");
                                          */      
						usleep(200);
                                                goto AGAIN;
						/*
						return TFAIL;      
                                                */
                                        case EIO:                               
                                        default:                
                                                tst_resm(TWARN, "VIDIOC_DQBUF fail");           
                                                return TFAIL;   
                                }      
                        }      
                        
                        if(buffer.index >= gBuffNumber)      
                        {        
                                tst_resm(TWARN, "Invalid buffer index");        
                                return TFAIL;      
                        }      
                        
                        if ( buffer.index == 0 && (gV4LTestConfig.mCaseNum == PRP_ENC_ON_D ||
                            (gV4LTestConfig.mCaseNum == PRP_ENC_TO_F && gSnapshot)) )
                        {
                                memcpy (gpFB_buf, gpBuffers[0].mpStart, gFormat.fmt.pix.sizeimage);
                                aStart = (void *)gpFB_buf;
                        }
                        else 
                                aStart = gpBuffers[buffer.index].mpStart;

                        if(process_image (aStart,gpBuffers[buffer.index].mLength) == TFAIL) 
                                return TFAIL;      
                                
                        if(ioctl (gFdV4L, VIDIOC_QBUF, &buffer) < 0)      
                        {       
                                tst_resm(TWARN, "Buffer error");        
                                return TFAIL;      
                        }      
                break;
                            
                case V4L2_BUF_TYPE_VIDEO_OVERLAY:
                        
                break;
                                    
                default:      
                        tst_resm(TWARN,"The buffer type %d is not supported", gUsecase);      
                        return TFAIL;  
        }  
        
        return TPASS;
}

/*===== write_file_header =====*/
/**
@brief  Writting dump file header

@param  None

@return On success - return TPASS
        On failure - return the error code
*/
int write_file_header(void)
{  
        char outFileName[MAX_STR_LEN]; 
         
        sprintf(outFileName, "%s%s%s", gV4LTestConfig.mOutputFile, "_", gPixFmtName);

        if(gV4LTestConfig.mVerbose) 
                tst_resm(TINFO,"Output Dump File : %s", outFileName);


        if(!(pDumpFile=fopen(outFileName,"w+b")))  
        {    
                tst_resm(TWARN, 
                        "Error write_file_header() : Unable to open file %s : %s", 
                        gV4LTestConfig.mOutputFile, strerror(errno));
                return TFAIL;
        }  
        else  
        {    
                char width[10];    
                char height[10];    
                
                sprintf(width, "%d", gFormat.fmt.pix.width);    
                sprintf(height,"%d", gFormat.fmt.pix.height);   
                
                fseek(pDumpFile,0,SEEK_SET);

                fwrite(gPixFmtName,sizeof(unsigned char),strlen(gPixFmtName),pDumpFile);
                putc('\0',pDumpFile);
                write_zero_to_file(pDumpFile,0x000F - strlen(gPixFmtName));
    
                fwrite(width,sizeof(unsigned char),strlen(width),pDumpFile);
                putc('\0',pDumpFile);   
                write_zero_to_file(pDumpFile,(0x001F - 0x0010) - strlen(width));    
                
                fwrite(height,sizeof(unsigned char),strlen(height),pDumpFile);    
                putc('\0',pDumpFile);    
                write_zero_to_file(pDumpFile,(0x002F - 0x0020) - strlen(height));    
                write_zero_to_file(pDumpFile,0x0100 - 0x0030);
     
                fclose(pDumpFile);
                pDumpFile = NULL;
        }
        
        return TPASS;
}

/*===== write_zero_to_file =====*/
/**
@brief  Writting zero to dump file

@param  apFile     - Pointer to offset of the file where to write zeros
        aBytes  - Number of bytes to write

@return None
*/
void write_zero_to_file(FILE* apFile, int aBytes)
{  
        char* zero = malloc(sizeof(unsigned char)*aBytes);
  
        memset(zero, 0, aBytes);   
        
        fwrite(zero, sizeof(unsigned char), aBytes, apFile);  
        
        free(zero);
}

/*===== detect_fb_fmt =====*/
/**
@brief  Detect frame buffer device pixel format

@param  None

@return On success - return TPASS
        On failure - return the error code
*/
int detect_fb_fmt(void)
{
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
                                strcpy(gFBPixFmtName,"RGB565X");
                                break;
                        }        
                        
                        if ((gScreenInfo.red.offset == 11) && (gScreenInfo.red.length == 5) &&      
                            (gScreenInfo.green.offset == 6) && (gScreenInfo.green.length == 5) &&           
                            (gScreenInfo.blue.offset == 1) && (gScreenInfo.blue.length == 5))   
                        {          
                                gFBPixFormat = V4L2_PIX_FMT_RGB555X;          
                                strcpy(gFBPixFmtName,"RGB555X");          
                                break;        
                        }

                        if ((gScreenInfo.red.offset == 0) && (gScreenInfo.red.length == 5) &&       
                            (gScreenInfo.green.offset == 5) && (gScreenInfo.green.length == 6) &&           
                            (gScreenInfo.blue.offset == 11) && (gScreenInfo.blue.length == 5))  
                        {          
                                gFBPixFormat = V4L2_PIX_FMT_RGB565;          
                                strcpy(gFBPixFmtName,"RGB565");          
                                break;        
                        }        
                        
                        if ((gScreenInfo.red.offset == 0) && (gScreenInfo.red.length == 5) &&       
                            (gScreenInfo.green.offset == 6) && (gScreenInfo.green.length == 5) &&           
                            (gScreenInfo.blue.offset == 11) && (gScreenInfo.blue.length == 5))  
                        {          
                                gFBPixFormat = V4L2_PIX_FMT_RGB555;          
                                strcpy(gFBPixFmtName,"RGB555");          
                                break;        
                        }
                        
                        strcpy(gFBPixFmtName,"Unknown");        
                        tst_resm(TWARN, "Unsupported FB format", gV4LTestConfig.mV4LDevice);
                        
                        if(gV4LTestConfig.mVerbose) 
                        {
                                printf("\t Resolution = %d bpp\n", gScreenInfo.bits_per_pixel); 
                                printf("\t Color\toffset\tlength\n");   
                                printf("\t Red\t%d\t%d\n", gScreenInfo.red.offset, gScreenInfo.red.length);     
                                printf("\t Green\t%d\t%d\n", gScreenInfo.green.offset, gScreenInfo.green.length);
                                printf("\t Blue\t%d\t%d\n", gScreenInfo.blue.offset, gScreenInfo.blue.length);
                        }

                        return TFAIL;

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

                        tst_resm(TWARN, "Unsupported FB format", gV4LTestConfig.mV4LDevice);

                        if(gV4LTestConfig.mVerbose) 
                        {
                                printf("\t Resolution = %d bpp\n", gScreenInfo.bits_per_pixel); 
                                printf("\t Color\toffset\tlength\n");   
                                printf("\t Red\t%d\t%d\n", gScreenInfo.red.offset, gScreenInfo.red.length);     
                                printf("\t Green\t%d\t%d\n", gScreenInfo.green.offset, gScreenInfo.green.length);
                                printf("\t Blue\t%d\t%d\n", gScreenInfo.blue.offset, gScreenInfo.blue.length);
                        }

                        return TFAIL;

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

                        tst_resm(TWARN, "Unsupported FB format", gV4LTestConfig.mV4LDevice);
                        
                        if(gV4LTestConfig.mVerbose) 
                        {
                                printf("\t Resolution = %d bpp\n", gScreenInfo.bits_per_pixel); 
                                printf("\t Color\toffset\tlength\n");   
                                printf("\t Red\t%d\t%d\n", gScreenInfo.red.offset, gScreenInfo.red.length);     
                                printf("\t Green\t%d\t%d\n", gScreenInfo.green.offset, gScreenInfo.green.length);
                                printf("\t Blue\t%d\t%d\n", gScreenInfo.blue.offset, gScreenInfo.blue.length);
                        }

                        return TFAIL;

                default:
                        
                        tst_resm(TWARN, "Unsupported FB resolution", gV4LTestConfig.mV4LDevice);
                        
                        if(gV4LTestConfig.mVerbose) 
                        {
                                printf("\t Resolution = %d bpp\n", gScreenInfo.bits_per_pixel); 
                                printf("\t Color\toffset\tlength\n");   
                                printf("\t Red\t%d\t%d\n", gScreenInfo.red.offset, gScreenInfo.red.length);     
                                printf("\t Green\t%d\t%d\n", gScreenInfo.green.offset, gScreenInfo.green.length);
                                printf("\t Blue\t%d\t%d\n", gScreenInfo.blue.offset, gScreenInfo.blue.length);
                        }

                return TFAIL;

        }
        
        if(gV4LTestConfig.mVerbose) 
                tst_resm(TINFO,"detect_fb_fmt() has detect framebuffer pix format : %s",gFBPixFmtName);


        return TPASS;
}


/*===== display_to_fb =====*/
/**
@brief  Display a image to frame buffer

@param  aStart - Pointer to a output buffer
        aLength - Length of the output buffer

@return None
*/

void display_to_fb (unsigned char * aStart, int aLength)
{  
        int dstWidth = gScreenInfo.xres * gScreenInfo.bits_per_pixel/8;  
        int srcWidth = gFormat.fmt.pix.width * gScreenInfo.bits_per_pixel/8;  
         
        unsigned char *pDst = gpFB;  
        unsigned char *pSrc = (unsigned char *)aStart;

#ifndef MAD_TEST_MODIFY
printf("\t Display to FB dstWidth and srcWidth\t%d\t%d\n", dstWidth, srcWidth);
printf("\t Display to FB aLength\t%d\n", aLength);
printf("\t sizeimage is \t%d\n", gFormat.fmt.pix.sizeimage);
#endif        
        int i = 0, j = 0; 
        
        while(i < gFormat.fmt.pix.sizeimage)
        {       
                *pDst = *pSrc;    
                i++; pSrc++; 
                j++; pDst++;
                        
                if (i%srcWidth == 0)       
                {         
                        while(j%dstWidth != 0)      
                        {               
                                j++;            
                                pDst++;        
                        }    
                }  
        }
}

/*===== ask_user =====*/
/**
@brief  Asks user to answer the question: is the drawn picture right?

@param  Input:  None
        Output: None

@return 1 - if user asks "No,  wrong"
        0 - if user asks "Yes, right"
*/

int ask_user(void)
{  
#ifndef MAD_TEST_MODIFY

	if (gV4LTestConfig.mNeedAsk)
	{
	       tst_resm(TINFO, "Disable user enquiry\n");
		return TPASS;
	}
#endif
        int retValue = TFAIL;  
        unsigned char answer;  
        int retKeyPress = 2;
        
        do  
        {   
                tst_resm(TINFO,"Is video displayed right? [y/n] ");
                fflush(stdout);
                answer = toupper(fgetc(stdin));
                
                if(answer == 'Y') 
                        retKeyPress = 0;    
                else 
                
                if(answer == 'N')            
                        retKeyPress = 1;  

        }
        while(retKeyPress == 2);
        
        fgetc(stdin);       /* Wipe CR character from stream */  
        
        if(!retKeyPress) 
                retValue = TPASS;  
        
        return retValue;
}

/*======================== GLOBAL FUNCTIONS =================================*/

/*===== VT_v4l_capture_setup =====*/
/**
@brief  assumes the pre-condition of the test case execution

@param  None

@return On success - return TPASS
        On failure - return the error code
*/
int VT_v4l_capture_setup(void)
{
        if(open_device() != TPASS)  
        {    
                tst_resm(TWARN, "%s open failed", gV4LTestConfig.mV4LDevice);    
                return  TFAIL;  
        }  

        return TPASS;
}

/*===== VT_cleanup =====*/
/**
@brief  Assumes the post-condition of the test case execution

@param  None

@return On success - return TPASS
        On failure - return the error code
*/
int VT_v4l_capture_cleanup(void)
{
        if(close_device() == TFAIL) 
                return TFAIL;  
                
        if(pDumpFile)  
        {    
                fclose(pDumpFile);    
                pDumpFile = NULL;  
        }
        
        return TPASS;
}

/*===== VT_v4l_capture_test =====*/
/**
@brief  Performs ipu capture test

@param  None

@return On success - return TPASS
        On failure - return the error code
*/
int VT_v4l_capture_test(void)
{
        int retValue = TFAIL;
        unsigned int cnt = gV4LTestConfig.mCount;
        fd_set fds;  
        struct timeval tv;  
        
        if(setup_device() != TPASS) 
        {
                cleanup_device();
                return TFAIL;
        }
        /* V4l test */

        if(gV4LTestConfig.mCaseNum == PRP_ENC_TO_F)  
        {    
        
               if(write_file_header()==TFAIL) 
                        return retValue; 
        }
          
        tst_resm(TINFO,"Start capturing..."); 

        while(cnt-- > 0)  
        {    
                int ret = -1;   
                while(ret < 0)    
                {       
                        FD_ZERO (&fds); 
                        FD_SET (gFdV4L, &fds);  

                        tv.tv_sec = 2;  
                        tv.tv_usec = 0; 
                        
                        ret = select (gFdV4L + 1, &fds, NULL, NULL, &tv);       
                        
                        if((ret < 0) && (errno != EINTR))      
                        {           
                                tst_resm(TWARN, "Select fault");
                                return retValue;
                        }       
                        
                        if(ret == 0)
                        {           
                                tst_resm(TWARN, "Select timeout");          
                                return retValue;  
                        }    
                }    
                
                gSnapshot = 0;    
                
                if((cnt == (int)(gV4LTestConfig.mCount/2 + 1)) && (gV4LTestConfig.mCaseNum==PRP_ENC_TO_F)) 
                        gSnapshot = 1;

                if(read_frame() == TFAIL) 
                        return retValue;
        }  
        
        if((gV4LTestConfig.mCaseNum != PRP_ENC_TO_F) || gV4LTestConfig.mRotation || gV4LTestConfig.mCrop) 
                retValue = ask_user();  
        else 
                retValue = TPASS; 
        
        sleep(1);
                
        if(cleanup_device() != TPASS) return TFAIL; 
                
        return retValue;
}

/*===== do_cropping =====*/
/**
@brief  Performs cropping

@param  None

@return On success - return TPASS
        On failure - return the error code
*/
int do_cropping(void)
{
#ifdef MAD_TEST_MODIFY
        struct v4l2_cropcap cropcap;
        struct v4l2_crop crop;        
#endif           
        tst_resm(TINFO,
                 "Performing cropping of the video to the %dx%d framesize\n", 
                 gV4LTestConfig.mCropRect.width, 
                 gV4LTestConfig.mCropRect.height); 

        memset (&cropcap, 0, sizeof (cropcap));
    
        cropcap.type = gUsecase; /* Defualt is  V4L2_BUF_TYPE_VIDEO_CAPTURE */
    
        if(ioctl (gFdV4L, VIDIOC_CROPCAP, &cropcap) < 0) 
        {
                tst_resm(TBROK, "Error: VIDIOC_CROPCAP ioctl failed");
                return TFAIL;
        }
        
        if(gV4LTestConfig.mVerbose)
        {
                tst_resm(TINFO,"Cropping bounds: left = %d, top = %d, width = %d, height = %d",
                       cropcap.bounds.left, cropcap.bounds.top, cropcap.bounds.width, cropcap.bounds.height);
        }
    
        memset(&crop, 0x0, sizeof(crop));
                
        crop.type = gUsecase; /* Defualt is  V4L2_BUF_TYPE_VIDEO_CAPTURE */
        
        if(ioctl (gFdV4L, VIDIOC_G_CROP, &crop) < 0) 
        {
                tst_resm(TBROK, "Error: VIDIOC_G_CROP ioctl failed");
                return TFAIL;
        }
        
        if(gV4LTestConfig.mVerbose)   
        {     
                tst_resm(TINFO,"Default cropping restangle: left = %d, top = %d, width = %d, height = %d", 
                         crop.c.left, crop.c.top, crop.c.width, crop.c.height); 
        }
        
        gOrigCropRect.left = crop.c.left;
        gOrigCropRect.top = crop.c.top;
        gOrigCropRect.width = crop.c.width;
        gOrigCropRect.height = crop.c.height;

        crop.c.left   = gV4LTestConfig.mCropRect.left;
        crop.c.top    = gV4LTestConfig.mCropRect.top;
        crop.c.width  = gV4LTestConfig.mCropRect.width;
        crop.c.height = gV4LTestConfig.mCropRect.height;
    
        if(ioctl (gFdV4L, VIDIOC_S_CROP, &crop) < 0) 
        {
                tst_resm(TBROK, "Error: VIDIOC_S_CROP ioctl failed");
                return TFAIL;
        }
                
        if(gV4LTestConfig.mVerbose)   
        {     
                tst_resm(TINFO,"Crop to image: left = %d, top = %d, width = %d, height = %d", 
                         crop.c.left, crop.c.top, crop.c.width, crop.c.height); 
        }
        
        return TPASS;
}

/*===== setup_device =====*/
/**
@brief  Setup and start device

@param  None

@return On success - return TPASS
        On failure - return the error 
*/
int setup_device(void)
{
        /* V4l setup */
        
        if(init_device() != TPASS)  
        {    
                tst_resm(TWARN, "%s init failed", gV4LTestConfig.mV4LDevice);    
                return  TFAIL;  
        }
        
        if(start_capturing() != TPASS)  
        {    
                tst_resm(TWARN, "%s capturing start failed", gV4LTestConfig.mV4LDevice);    
                return  TFAIL;  
        }  

        return TPASS;
}

/*===== cleanup_device =====*/
/**
@brief  Stop and cleanup device

@param  None

@return On success - return TPASS
        On failure - return the error
*/
int cleanup_device(void)
{
        /* V4l cleanup */
        if(stop_capturing() != TPASS)  
        {    
                tst_resm(TBROK, "%s capturing stop failed", gV4LTestConfig.mV4LDevice);    
                return TFAIL;  
        }  

        if(reset_device() != TPASS)  
        {    
                tst_resm(TBROK, "%s reset failed", gV4LTestConfig.mV4LDevice);    
                return TFAIL;  
        }
       
        if(uninit_device() != TPASS)  
        {    
                tst_resm(TBROK, "%s uninit failed", gV4LTestConfig.mV4LDevice);
                return TFAIL;  
        }  
        
        return TPASS;
}
#ifdef mxc91131evb_CSI_STRIPS_FIX
/*===== register_conf =====*/
/**
@brief Configure the CSI_SENS_CONF register

@param int div_ratio - division ratio of HSP_CLK into SENSB_MCLK minus 1

@return On success - return TPASS
        On failure - return the error 
*/
int register_conf(int div_ratio)
{
        int gmap_fd                          = 0;
        unsigned int read_register_value     = 0;
        void *pCSIReg                        = MAP_FAILED;
        int  *pCSI_sens_conf_reg             = NULL;
        
        if ( div_ratio < 0 || div_ratio > 255 ) div_ratio = 4;
        
        if ((gmap_fd = open("/dev/mem", O_RDWR)) < 0)
        {
                tst_resm( TFAIL, "Error open /dev/mem : %s", strerror(errno) );
                return TFAIL;
        }
        
        pCSIReg = mmap(
                NULL,
                256,
                PROT_READ|PROT_WRITE,
                MAP_SHARED,
                gmap_fd,
                0x53FC0000 //0x53FC0000 - the initial address CSI registers
        );
        
        if( MAP_FAILED == pCSIReg )
        {
                tst_resm( TFAIL, "%s : Can't mmap /dev/mem %s", strerror(errno) );
                return TFAIL;
        }
        
        pCSI_sens_conf_reg = (int *)pCSIReg + 24;   
        // read register by address 0x53FC0060
        read_register_value = *pCSI_sens_conf_reg;
        // write registr by address 0x53FC0060
        *pCSI_sens_conf_reg = (read_register_value & 0xff00ffff) | (div_ratio << 16) | (1 << 7);
        
        if( MAP_FAILED != pCSIReg )
        {
                munmap( pCSIReg, 256 );
                pCSIReg = MAP_FAILED;
        }
        
        if( (gmap_fd > 0) && (close(gmap_fd) < 0) )
        {
                tst_resm(TFAIL, "Unable to close /dev/mam : %s", strerror(errno) );
                return TFAIL;
        }
        
        return TPASS;                                                 
}
#endif 

#ifdef __cplusplus
}
#endif
