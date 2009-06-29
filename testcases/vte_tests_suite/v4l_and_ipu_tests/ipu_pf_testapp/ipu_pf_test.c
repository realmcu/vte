/***
**Copyright 2005-2009 Freescale Semiconductor, Inc. All Rights Reserved.
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
        @file   ipu_pf_test.c

        @brief  Test scenario
====================================================================================================
Revision History:
                            Modification     Tracking
Author                          Date          Number    Description of Changes
-------------------------   ------------    ----------  -------------------------------------------
Artyom Smirnov              26/05/2005      TLSbo49894   Initial version
D.Simakov / smkd001c        21/09/2005      TLSbo55077   Re-written
A.Pshenichnikov             23/11/2005      TLSbo58740   some bugs were fixed
E.Gromazina/NONE            10/01/2006      TLSbo61481   Clean the LCD after testing
D.Kardakov                  30/08/2006      TLSbo75997   some bugs were fixed, 
                                                         option the number of image filtering was added 
D.Kardakov                  02/01/2007      TLsbo87909   Register_conf( void )
====================================================================================================
Portability: ARM GCC

==================================================================================================*/

/*==================================================================================================
                                        INCLUDE FILES
==================================================================================================*/

/* Verification Test Environment Include Files */
#include <string.h>
#include <errno.h>
#include "ipu_pf_test.h"
#include "stuff/fb_draw_api.h"
#include "stuff/ycbcr.h"
#include <assert.h>

/*==================================================================================================
                                        LOCAL MACROS
==================================================================================================*/
#define IPU_PF_DEVICE_FILE "/dev/mxc_ipu_pf"

/************************************************************************/
/*  Data offset    | Content                                            */
/*  ---------------+----------------------------------------------      */
/*  0x0000-0x000F  | Image format as C string:                          */
/*                 |     - "RGB565"                                     */
/*                 |     - "BGR24"                                      */
/*                 |     - "RGB24"                                      */
/*                 |     - "BGR32"                                      */
/*                 |     - "RGB32"                                      */
/*                 |     - "YUV422P"                                    */
/*                 |     - "YUV420"                                     */
/*  0x0010-0x001F  | Image X size as C string                           */
/*  0x0020-0x002F  | Image Y size as C string                           */
/*  0x0030-0x00FF  | <RESERVED_AREA>                                    */
/*  0x0100-0xXXXX  | Raw data to/from IPU, V4L(2) or FB buffers         */
/************************************************************************/
#define FIELDS_LENGTH            0x0010
#define FMT_OFFSET               0x0000
#define X_SIZE_OFFSET            0x0010
#define Y_SIZE_OFFSET            0x0020
#define RESERVED_AREA_OFFSET     0x0030
#define RAW_DATA_OFFSET          0x0100

/*********************************************************************************/
/* Macro name:  CALL_IOCTL()                                                     */
/* Description: Macro checks for any error in function execution                 */
/*              based on the return value. In case of error, the function exits. */
/*********************************************************************************/
#define CALL_IOCTL(ioctl_cmd)\
{ \
    if( (ioctl_cmd) < 0 )\
    {\
        tst_resm( TFAIL, "%s : %s fails #%d [File: %s, line: %d]", __FUNCTION__, "ioctl", errno, __FILE__, __LINE__-2);\
        perror("ioctl"); \
        return TFAIL;\
    }\
}

/*****************/
/* Debug output. */
/*****************/
#ifdef DEBUG_TEST
    #define DPRINTF(fmt,...) {printf((fmt), ##__VA_ARGS__); fflush(stdout);}
#else
    #define DPRINTF(fmt,...) do {} while(0)
#endif

/*==================================================================================================
                          LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
==================================================================================================*/
typedef struct
{
        char mFormat[FIELDS_LENGTH];
        char mWidth[FIELDS_LENGTH];
        char mHeight[FIELDS_LENGTH];
        char mReserved[RAW_DATA_OFFSET - RESERVED_AREA_OFFSET];
} sDumpFileHeader;

/*==================================================================================================
                                       LOCAL VARIABLES
==================================================================================================*/
static int               gIpuPfFd          = -1;         /* IPU post-filter device. */
static pf_init_params    gPfInitParams;                  /* Init post filter parameters. */
static pf_reqbufs_params gPfReqbufsParams;               /* Request buffers for processing. */
static pf_start_params   gPfStartParams;                 /* Start frame processing. */
static int               gImageWidth;                    /* Width of Y plane of the input picture in pixels. */
static int               gImageHeight;                   /* Height of Y plane of the input picture in pixels. */
static void            * gpInp             = MAP_FAILED; /* Mapped input buffer. */
static void            * gpOut             = MAP_FAILED; /* Mapped output buffer. */
static unsigned char   * gpRgbFrame        = NULL;       /* Intermediate RGB buffer. */
static size_t            gRgbFrameSz       = 0;          /* Size of the intermediate RGB buffer. */
static FILE            * gInpStream        = NULL;       /* Input file stream. */
static FILE            * gOutStream        = NULL;       /* Output file stream. */
static unsigned char   * pFb_saved         = NULL;       /* Pointer to framebuffer memory */
static unsigned long     sizeFb_saved      = 0;          /* Size of framebuffer memory */
static void            * gpInpStat         = NULL;       /* Buffer for initial image data*/

/*==================================================================================================
                                       GLOBAL VARIABLES
==================================================================================================*/
extern sTestappConfig gTestappConfig;

/*==================================================================================================
                                   LOCAL FUNCTION PROTOTYPES
==================================================================================================*/
int DisplayImage( unsigned char * pYuv, int width, int height, int left, int top );
int Register_conf( void );
/*==================================================================================================
                                       LOCAL FUNCTIONS
==================================================================================================*/

/*================================================================================================*/
/*===== VT_ipu_pf__setup =====*/
/**
@brief  assumes the pre-condition of the test case execution

@param  None

@return On success - return TPASS
        On failure - return TFAIL
*/
/*================================================================================================*/
int VT_ipu_pf_setup( void )
{
        assert( gTestappConfig.mInputFileName && gTestappConfig.mOutputFileName );

        /*******************************/
        /* Open all files and devices. */
        /*******************************/

        /* Open the IPU post-filter device. */
        gIpuPfFd = open( IPU_PF_DEVICE_FILE, O_RDWR );
        if( -1 == gIpuPfFd )
        {
                tst_resm( TBROK, "%s : Can't open IPU_PF char device file %s",
                        __FUNCTION__, IPU_PF_DEVICE_FILE );
                return TFAIL;
        }

        /* Open the input image. */
        gInpStream = fopen( gTestappConfig.mInputFileName, "rb" );
        if( !gInpStream )
        {
                tst_resm( TBROK, "%s : Can't open input image file %s",
                        __FUNCTION__, gTestappConfig.mInputFileName );
                return TFAIL;
        }

        /* Create the output image file. */
        gOutStream = fopen( gTestappConfig.mOutputFileName, "wb" );
        if( !gOutStream )
        {
                tst_resm( TBROK, "%s : Can't create output image file %s",
                __FUNCTION__, gTestappConfig.mOutputFileName );
                return TFAIL;
        }

        /*************************/
        /* Parse the input file. */
        /*************************/
        sDumpFileHeader hdr;
        fread( &hdr, 1, sizeof(hdr), gInpStream );
        if( strncmp( hdr.mFormat, "YUV420", FIELDS_LENGTH) )
        {
                tst_resm( TBROK, "%s : The image %s has unsupported pixel format (%s). "
                        "The testapp supports only YUV420 pixel format.",
                        __FUNCTION__, gTestappConfig.mInputFileName, hdr.mFormat );
                return FALSE;
        }
        gImageWidth  = atoi( hdr.mWidth );
        gImageHeight = atoi( hdr.mHeight );

        /* Dump some debug info. */
        DPRINTF( "Format: %s\n", hdr.mFormat );
        DPRINTF( "Width:  %d\n", gImageWidth );
        DPRINTF( "Height: %d\n", gImageHeight );

        /****************************************************************************/
        /* Allocate memory for the intermediate RGB buffer (to display the images). */
        /****************************************************************************/
        gRgbFrameSz = gImageWidth * gImageHeight * 3;
        gpRgbFrame = (unsigned char*)malloc( gRgbFrameSz );
        if( !gpRgbFrame )
        {
                tst_resm( TBROK, "%s : Can't allocate %lu bytes for the RGB buffer",
                        __FUNCTION__, (unsigned long)gRgbFrameSz );
                return TFAIL;
        }

        return TPASS;
}

/*================================================================================================*/
/*===== VT_ipu_pf_cleanup =====*/
/**
@brief  assumes the post-condition of the test case execution

@param  None

@return TPASS
*/
/*================================================================================================*/
int VT_ipu_pf_cleanup( void )
{
        /* Clean the LCD  */
        if(pFb_saved)
        {
                memset(pFb_saved, 0x00, sizeFb_saved);
                pFb_saved = NULL;
        }

        /**********************************************/
        /* Close all files and device we have opened. */
        /**********************************************/

        /* Close the IPU post-filter device. */
        if( -1 != gIpuPfFd )
        {
                close( gIpuPfFd );
                gIpuPfFd = -1;
        }

        /* Close the input image file. */
        if( gInpStream )
        {
                fclose( gInpStream );
                gInpStream = NULL;
        }

        /* Close the output image file. */
        if( gOutStream )
        {
                fclose( gOutStream );
                gOutStream = NULL;
        }

        /********************************************/
        /* Deallocate all memory we have allocated. */
        /********************************************/

        /* Deallocate RGB buffer. */
        if( gpRgbFrame )
        {
                free( gpRgbFrame );
                gpRgbFrame = NULL;
                gRgbFrameSz = 0;
        }
        /* Unmap. */
        if( MAP_FAILED != gpInp )
        {
		/*flush the output*/
		/*Hake 20090403 for sync driver test. not necessary*/
	        msync(gpInp, gPfStartParams.in.size,MS_SYNC);
                munmap( gpInp, gPfStartParams.in.size );
                gpInp = MAP_FAILED;
        }
        if( MAP_FAILED != gpOut )
        {
                munmap( gpOut, gPfStartParams.out.size );
                gpOut = MAP_FAILED;
        }

        return TPASS;
}

/*================================================================================================*/
/*===== VT_ipu_pf_test =====*/
/**
@brief  Initialisation IPU,  filtering image and display original and filtered image

@param  None

@return On success - return TPASS
        On failure - return the error code and TFAIL
*/
/*================================================================================================*/
int VT_ipu_pf_test(void)
{
        assert( gTestappConfig.mInputFileName && gTestappConfig.mOutputFileName );
        int i;

        /*************************/
        /* Init the post-filter. */
        /*************************/

        /* Fill up the gPfInitParams. */
        memset( &gPfInitParams, 0, sizeof(gPfInitParams) );
        switch( gTestappConfig.mTestCase )
        {
        case 1:
                gPfInitParams.pf_mode = PF_MPEG4_DEBLOCK;
                break;
        case 2:
                gPfInitParams.pf_mode = PF_MPEG4_DERING;
                break;
        case 3:
                gPfInitParams.pf_mode = PF_MPEG4_DEBLOCK_DERING;
                break;
        case 4:
                gPfInitParams.pf_mode = PF_H264_DEBLOCK;
                break;
	case 5:
                gPfInitParams.pf_mode = PF_H264_DEBLOCK;
	       break;
        default:
                gPfInitParams.pf_mode = PF_DISABLE_ALL;
        }

        gPfInitParams.width  = gImageWidth;
        gPfInitParams.height = gImageHeight;
        gPfInitParams.stride = gImageWidth;

        /* Call ioctl to init the post-filter. */
        CALL_IOCTL( ioctl( gIpuPfFd, PF_IOCTL_INIT, (void*)&gPfInitParams ) );

        /* Dump some debug info. */
        DPRINTF( "init_params.qp_paddr = %lu\n", (unsigned long)gPfInitParams.qp_paddr );
        DPRINTF( "init_params.qp_size = %lu\n", (unsigned long)gPfInitParams.qp_size );

        /***************************************/
        /* Request driver to allocate buffers. */
        /***************************************/

        /* Fill up the gPfReqbufsParams. */
        memset( &gPfReqbufsParams, 0, sizeof(gPfReqbufsParams) );
        gPfReqbufsParams.count = 2;

        /* Call ioctl to perform the request. */
        CALL_IOCTL( ioctl( gIpuPfFd, PF_IOCTL_REQBUFS, (void*)&gPfReqbufsParams ) );

        /* Dump some debug info. */
        DPRINTF( "reqbufs_params.req_size = %lu\n", (unsigned long)gPfReqbufsParams.req_size );

        /***************************************************************/
        /* Query input and output buffers and do map it to user space. */
        /***************************************************************/

        /* Fill up the gPfStartParams. */
        memset( &gPfStartParams, 0, sizeof(gPfStartParams) );
        gPfStartParams.in.index  = 0;
        gPfStartParams.out.index = 1;

        /* Call ioctl to query input/output buffer. */
        CALL_IOCTL( ioctl( gIpuPfFd, PF_IOCTL_QUERYBUF, (void*)&gPfStartParams.in  ) );
        CALL_IOCTL( ioctl( gIpuPfFd, PF_IOCTL_QUERYBUF, (void*)&gPfStartParams.out ) );

        /* Dump some debug info. */
        DPRINTF( "start_params.in.size = %lu\n", (unsigned long)gPfStartParams.in.size );
        DPRINTF( "start_params.in.offset = %lu\n", (unsigned long)gPfStartParams.in.offset );
        DPRINTF( "start_params.in.y_offset = %lu\n", (unsigned long)gPfStartParams.in.y_offset );
        DPRINTF( "start_params.in.u_offset = %lu\n", (unsigned long)gPfStartParams.in.u_offset );
        DPRINTF( "start_params.in.v_offset = %lu\n", (unsigned long)gPfStartParams.in.v_offset );

        DPRINTF( "start_params.out.size = %lu\n", (unsigned long)gPfStartParams.out.size );
        DPRINTF( "start_params.out.offset = %lu\n", (unsigned long)gPfStartParams.out.offset );
        DPRINTF( "start_params.out.y_offset = %lu\n", (unsigned long)gPfStartParams.out.y_offset );
        DPRINTF( "start_params.out.u_offset = %lu\n", (unsigned long)gPfStartParams.out.u_offset );
        DPRINTF( "start_params.out.v_offset = %lu\n", (unsigned long)gPfStartParams.out.v_offset );

        gpInpStat = (unsigned char*) malloc( gPfStartParams.in.size );
        if( !gpInpStat )
        {
                tst_resm( TBROK, "%s : Can't allocate %lu bytes for the data buffer",
                __FUNCTION__, (unsigned long)gpInpStat );
                return TFAIL;
        }
        /* Map the input buffer. */
        gpInp = mmap( NULL, gPfStartParams.in.size,
                      PROT_READ | PROT_WRITE,
                      MAP_SHARED,
                      gIpuPfFd,
                      gPfStartParams.in.offset );

        if( MAP_FAILED == gpInp )
        {
                tst_resm( TBROK, "%s : Can't map input buffer", __FUNCTION__ );
                return TFAIL;
        }

        /* Map the output buffer. */
        gpOut = mmap( NULL, gPfStartParams.out.size,
                      PROT_READ | PROT_WRITE,
                      MAP_SHARED,
                      gIpuPfFd,
                      gPfStartParams.out.offset );
        
        if( MAP_FAILED == gpOut )
        {
                tst_resm( TBROK, "%s : Can't map output buffer", __FUNCTION__ );
                return TFAIL;
        }

        /*********************************************************/
        /* Read data to the mmaped input buffer and initial image buffer. */
        /*********************************************************/

        fseek( gInpStream, sizeof(sDumpFileHeader), SEEK_SET );
        fread( gpInp, 1, gPfStartParams.in.size, gInpStream );
        fseek( gInpStream, sizeof(sDumpFileHeader), SEEK_SET );
        fread( gpInpStat, 1, gPfStartParams.in.size, gInpStream );

        /*********************/
        /* Init the framebuffer. */
        /*********************/
        
        const framebuffer_t * pFb = get_framebuffer();
        assert( pFb );
        pFb_saved = pFb->_fb_direct_ptr;
        sizeFb_saved = pFb->_fb_pitch * pFb->height;

        argb_color_t white;
        pFb->clear_screen( &white );
        int left = gImageWidth < pFb->width ? (pFb->width - gImageWidth)/2 : 0;

        /*********************/
        /* Start processing. */
        /*********************/
        if ( gTestappConfig.mTestCase == 5)
        {
         printf("this parameter has to be 0!");
	/* gPfStartParams.h264_pause_row =  gTestappConfig.mH264PauseRow;*/
	}else
	 gPfStartParams.h264_pause_row = 0;
        gPfStartParams.wait           = 1;

        for ( i = 0; i < gTestappConfig.mNumFilterCycle; i++ )
        {
           /*code to test the poll system call*/
	   /*not necessary for function */
	   /* Hake 20090402*/
	   {
              fd_set rfds;
	      struct timeval tv;
	      int retval;
	      FD_ZERO(&rfds);
	      FD_SET(gIpuPfFd, &rfds);
              tv.tv_sec = 1;
	      tv.tv_usec = 0;
	      retval = select(gIpuPfFd+1, &rfds, NULL, NULL, &tv);
	      if (retval == -1)
		   perror("select()");
	       else if (retval)
		   printf("Data is available now.\n");
		 /* FD_ISSET(0, &rfds) will be true. */
		else
		 printf("No data within one seconds.\n");
	   }

                if ( gPfInitParams.pf_mode == PF_MPEG4_DERING )
                        Register_conf();
                CALL_IOCTL( ioctl( gIpuPfFd, PF_IOCTL_START, (void*)&gPfStartParams ) );
		if ( gTestappConfig.mTestCase == 5 ) /* PF_H264_DEBLOCK_RESUME MCU decode does nothing*/
		{
		     int row = 0;
                     /* CALL_IOCTL(ioctl( gIpuPfFd, PF_IOCTL_RESUME , (void *)&row)); */
		}
                CALL_IOCTL( ioctl( gIpuPfFd, PF_IOCTL_WAIT , PF_WAIT_ALL) );
		if ( gPfInitParams.pf_mode == PF_MPEG4_DERING )
                        Register_conf();

                if ( gPfInitParams.pf_mode != PF_H264_DEBLOCK ) /* In case PF_H264_DEBLOCK input buffer is equal output buffer */
                        memcpy(gpInp, gpOut, gPfStartParams.in.size);
        }
        tst_resm( TINFO, "Number cycles : %d", gTestappConfig.mNumFilterCycle );

        /****************************/
        /* Save the filtered image. */
        /****************************/
        sDumpFileHeader hdr;
        memset( &hdr, 0, sizeof(hdr) );
        sprintf( hdr.mFormat, "%s", "YUV420" );
        sprintf( hdr.mWidth, "%d", gImageWidth );
        sprintf( hdr.mHeight, "%d", gImageHeight );

        fwrite( &hdr, 1, sizeof(hdr), gOutStream );
        if ( gPfInitParams.pf_mode != PF_H264_DEBLOCK )
                fwrite( gpOut, 1, gPfStartParams.out.size, gOutStream );
        else
                fwrite( gpInp, 1, gPfStartParams.in.size, gOutStream );
        tst_resm (TINFO, "Post-filter image is saved!");

        /***********************/
        /* Display the images. */
        /***********************/

        DisplayImage( gpInpStat, gImageWidth, gImageHeight, left, 0 ); //initial image
        if ( gPfInitParams.pf_mode != PF_H264_DEBLOCK )
                DisplayImage( gpOut, gImageWidth, gImageHeight, left, pFb->height/2 ); //filtered image
        else
                DisplayImage( gpInp, gImageWidth, gImageHeight, left, pFb->height/2 ); //filtered image


        /************/
        /* Cleanup. */
        /************/
        CALL_IOCTL( ioctl( gIpuPfFd, PF_IOCTL_UNINIT ) );

        if ( gpInpStat != NULL )
        {
                free ( gpInpStat );
                gpInpStat = NULL;
        }

        return TPASS;
}

/*================================================================================================*/
/*===== DisplayImage =====*/
/**
@brief  Draws image

@param  Input:  pYuv - pointer to the input or output buffer,
                width, height - width and height of image,
                left, top - coordinates left top summit of image
        Output: None

@return 0 if test passed
        1 if test failed
*/
/*================================================================================================*/
int DisplayImage( unsigned char * pYuv, int width, int height, int left, int top )
{
        int i;
        size_t uOffset = width * height;
        size_t vOffset = uOffset + (width/2) * (height/2);
        unsigned char * yPtr = (unsigned char*)pYuv;
        unsigned char * uPtr = (unsigned char*)yPtr + uOffset;
        unsigned char * vPtr = (unsigned char*)yPtr + vOffset;

        /* Convert YUV420 -> RGB888 to display the frame. */
        YCbCrToRGB(
                yPtr, width,
                uPtr, vPtr, width/2,
                gpRgbFrame, width, height,
                RGB_ORIENT_NORMAL, RGB_888 );

        /* Draw the frame row by row. */
        const framebuffer_t * pFb = get_framebuffer();
        assert( pFb );
        unsigned char *pRow = gpRgbFrame;
        size_t stride = gRgbFrameSz / height;
        for( i = 0; i < height; ++i )
        {
                pFb->draw_scanline( pRow, left, i+top, width, PF_RGB_888 );
                pRow += stride;
        }

        return TRUE;
}
/*===== Register_conf =====*/
/**
@brief Configure the IPU_CHA_BUF0_RDY register, clear eight high-order bits in IPU_CHA_BUF0_RDY register.

@param None

@return On success - return TPASS
        On failure - return the error
*/
/*================================================================================================*/
int Register_conf( void )
{
        int gmap_fd                          = 0;
        unsigned int read_register_value     = 0;
        void *pIPUReg                        = MAP_FAILED;
        int  *pIPU_cha_buf0_rdy              = NULL;

        if ((gmap_fd = open("/dev/mem", O_RDWR)) < 0)
        {
                tst_resm( TFAIL, "Error open /dev/mem : %s", strerror(errno) );
                return TFAIL;
        }

        pIPUReg = mmap(
                NULL,
                16,
                PROT_READ|PROT_WRITE,
                MAP_SHARED,
                gmap_fd,
                0x53FC0000 //0x53FC0000 - the initial address IPU registers
        );

        if( MAP_FAILED == pIPUReg )
        {
                tst_resm( TFAIL, "%s : Can't mmap /dev/mem %s", strerror(errno) );
                return TFAIL;
        }

        pIPU_cha_buf0_rdy = (int *)pIPUReg + 1; // pIPU_cha_buf0_rdy point to IPU_CHA_BUF0_RDY register
        read_register_value = *pIPU_cha_buf0_rdy; // read register by address 0x53FC0004
        *pIPU_cha_buf0_rdy = read_register_value & 0x00ffffff; // write registr by address 0x53FC0004

        if( MAP_FAILED != pIPUReg )
        {
	   munmap( pIPUReg, 16 );
           pIPUReg = MAP_FAILED;
        }

        if( (gmap_fd > 0) && (close(gmap_fd) < 0) )
        {
                tst_resm(TFAIL, "Unable to close /dev/mam : %s", strerror(errno) );
                return TFAIL;
        }

        return TPASS;
}
