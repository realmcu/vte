/*================================================================================================*/
/**
    @file   opl_chain_test.c

    @brief  Test scenario C source template.
*/
/*==================================================================================================

  Copyright (C) 2004, Freescale Semiconductor, Inc. All Rights Reserved
  THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
  BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
  Freescale Semiconductor, Inc.
     
====================================================================================================
Revision History:
                            Modification     Tracking
Author                          Date          Number    Description of Changes
-------------------------   ------------    ----------  -------------------------------------------
D.SIMAKOV/smkd001c           11/07/2004     TLSbo41679   Initial version 
Francois GAFFIE		         15/09/2004     TLSbo42679   Added 16bpp display support
Ludovic DELASPRE / rc149c    17/09/2004     TLSbo42022   VTE 1.5 integration

====================================================================================================
Portability: Indicate if this module is portable to other compilers or platforms. 
             If not, indicate specific reasons why is it not portable.

==================================================================================================*/

#ifdef __cplusplus
extern "C"{
#endif

/*==================================================================================================
                                        INCLUDE FILES
==================================================================================================*/
/* Standard Include Files */
#include <errno.h>
    
/* Harness Specific Include Files. */
#include "test.h"

/* Verification Test Environment Include Files */
#include "opl_chain_test.h"

/* OPL */
#include "oplIP.h"

/*==================================================================================================
                                        LOCAL MACROS
==================================================================================================*/
#define INPUT_IMAGE_WIDTH       128
#define INPUT_IMAGE_HEIGHT      128

/*==================================================================================================
                          LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
==================================================================================================*/							 
typedef OPLError (*oplip_op_t)( const U8*, const OPLRect, const U32, U8* );

/* structure that groups all variables needed for working with framebuffer */
typedef struct
{
    int file_desc;                                                    /* frame buffer file descriptor FGA for display purpose */
    struct fb_var_screeninfo screen_info;                             /* screen parameters */	
    U32 size;                                                      /* size of the videobuffer in bytes */
    U8 * ptr;                                                      /* pointer to frame buffer FGA for display purpose */    
    void (*draw_scanline)( U8 *, U32, U32, U32, U32 ); /* draw routine */      
} framebuffer_t;

/*==================================================================================================
                                       LOCAL CONSTANTS
==================================================================================================*/


/*==================================================================================================
                                       LOCAL VARIABLES
==================================================================================================*/
static oplip_op_t opl_op_array[MAX_OPL_OPERATIONS] = {0};
static int opl_op_count = 0;

static oplip_op_t oplIPRotate_CW   = 0;
static oplip_op_t oplIPRotate_CCW  = 0;
static oplip_op_t oplIPMirrorVert  = 0;
static oplip_op_t oplIPMirrorHoriz = 0;

static U8  input_image[ INPUT_IMAGE_WIDTH * INPUT_IMAGE_HEIGHT * 4 ];
static U8  image_buffer[ INPUT_IMAGE_WIDTH * INPUT_IMAGE_HEIGHT * 4 ];
static U8  output_image[ INPUT_IMAGE_WIDTH * INPUT_IMAGE_HEIGHT * 4 ];
static U32 image_line_stride = 0;

static char * input_image_filename = "pic_128x128.rgb";

static framebuffer_t framebuffer;

/*==================================================================================================
                                       GLOBAL CONSTANTS
==================================================================================================*/

/*==================================================================================================
                                       GLOBAL VARIABLES
==================================================================================================*/


/*==================================================================================================
                                   LOCAL FUNCTION PROTOTYPES
==================================================================================================*/


/*==================================================================================================
                                       LOCAL FUNCTIONS
==================================================================================================*/
void parse_sequense( char * sequense );
void RGB_to_U8( U8 * rgb888, U8 * u8, U32 width, U32 height );
void RGB_to_U16( U8 * rgb888, U8 * rgb565, U32 width, U32 height );
void RGB_to_U32( U8 * rgb888, U8 * u32, U32 width, U32 height );
void draw_pixels( U32 x, U32 y, U8 * pixels, U32 width, U32 height, U32 bpp );


/*================================================================================================*/
/*===== VT_opl_chain_setup =====*/
/**
@brief  assumes the pre-condition of the test case execution

@param  None
  
@return On success - return TPASS
        On failure - return the error code
*/
/*================================================================================================*/
int VT_opl_chain_setup()
{
    int rv = TFAIL;
    
    /* Open the framebuffer */ 	
    framebuffer.file_desc = open( "/dev/fb0", O_RDWR );

    /* Get screen infos  (size, resolution)	*/
    if( ioctl( framebuffer.file_desc, FBIOGET_VSCREENINFO, &framebuffer.screen_info ) )
    {
    	fprintf( stderr, "VT_opl_chain_setup : error reading variable information\n" );
    	return rv;
    }
    
    framebuffer.size = framebuffer.screen_info.xres * framebuffer.screen_info.yres * framebuffer.screen_info.bits_per_pixel / 8;

    /* Map the device to memory */
    framebuffer.ptr = (U8 *)mmap( 0, framebuffer.size, PROT_READ | PROT_WRITE, MAP_SHARED, framebuffer.file_desc, 0 );
    if( (int)framebuffer.ptr == -1 )
    {
	fprintf( stderr, "VT_opl_chain_setup : error: failed to map framebuffer device to memory.\n" );
	close( framebuffer.file_desc );
	return rv;
    }
    memset( framebuffer.ptr, 0xFFFF, framebuffer.size );
    
    FILE * in = fopen( input_image_filename, "rb" );
    if( 0 == in )
    {
	fprintf( stderr, "VT_opl_chain_setup : can't open %s\n", input_image_filename );
	return rv;	
    }
    
    fread( input_image, INPUT_IMAGE_HEIGHT, INPUT_IMAGE_WIDTH * 3, in );
    
    fclose( in );

    rv = TPASS;    
    return rv;
}


/*================================================================================================*/
/*===== VT_opl_chain_cleanup =====*/
/**
@brief  assumes the post-condition of the test case execution

@param  None
  
@return On success - return TPASS
        On failure - return the error code
*/
/*================================================================================================*/
int VT_opl_chain_cleanup(void)
{
    int rv = TFAIL;
    
    rv = TPASS;    
    return rv;
}

/*================================================================================================*/
/*===== VT_opl_chain_test =====*/
/**
@brief  OPL chain test

@param  color_depth - color depth of an input image
	sequense - a opl operation sequense
  
@return On success - return TPASS
        On failure - return the error code
*/
/*================================================================================================*/
int VT_opl_chain_test( int color_depth, char * sequense )
{
    int rv = TFAIL;

    /* select operations based on color depth */
    switch( color_depth )
    {
	case 8:
	    oplIPRotate_CW    = oplIPRotate_CW_U8;
	    oplIPRotate_CCW   = oplIPRotate_CCW_U8;
	    oplIPMirrorVert   = oplIPMirrorVert_U8;
	    oplIPMirrorHoriz  = oplIPMirrorHoriz_U8; 
	    /* convert RGB888 to grayscale */
	    RGB_to_U8( input_image, input_image, INPUT_IMAGE_WIDTH, INPUT_IMAGE_HEIGHT );
	    image_line_stride = INPUT_IMAGE_WIDTH * 1;
	    break;
	    
	case 16:
	    oplIPRotate_CW    = oplIPRotate_CW_U16;
	    oplIPRotate_CCW   = oplIPRotate_CCW_U16;
	    oplIPMirrorVert   = oplIPMirrorVert_U16;
	    oplIPMirrorHoriz  = oplIPMirrorHoriz_U16; 
	    /* convert RGB888 to RGB565 */
	    RGB_to_U16( input_image, input_image, INPUT_IMAGE_WIDTH, INPUT_IMAGE_HEIGHT );
	    image_line_stride = INPUT_IMAGE_WIDTH * 2;
	    break;
	    
#if 0
	case 32:
	    oplIPRotate_CW    = oplIPRotate_CW_U32;
	    oplIPRotate_CCW   = oplIPRotate_CCW_U32;
	    oplIPMirrorVert   = oplIPMirrorVert_U32;
	    oplIPMirrorHoriz  = oplIPMirrorHoriz_U32; 
	    /* convert RGB888 to RGBA (alpha channel always 1) */
	    RGB_to_U32( input_image, input_image, INPUT_IMAGE_WIDTH, INPUT_IMAGE_HEIGHT );
	    image_line_stride = INPUT_IMAGE_WIDTH * 4;	    	    
	    break;            
#endif	    
	    
	default:
	    fprintf( stderr, "Unsupported color depth %d\n", color_depth );
	    return rv;
	    break;
    }
    
    printf( "OPL operations sequense : %s\n", sequense );
    
    /* call the sequense parser */
    parse_sequense( sequense );
    
    U32 i;
    OPLRect rc;   
    OPLError err;
    rc.mX = 0; rc.mY = 0; rc.mWidth = INPUT_IMAGE_WIDTH; rc.mHeight = INPUT_IMAGE_HEIGHT;
    /* execute secuense */
    for( i = 0; i < opl_op_count; ++i )
    {
	if( i == 0 )
	    err = opl_op_array[0]( input_image, rc, image_line_stride, image_buffer );
	else if( i%2 == 0 )
	    err += opl_op_array[i]( output_image, rc, image_line_stride, image_buffer );
	else
	    err += opl_op_array[i]( image_buffer, rc, image_line_stride, output_image );
    }
    
    /* check OPL error */
    if( err != OPLErrorSuccess )
    {
	tst_resm( TFAIL, "An error has been happen during OPL call" );
	return rv;
    }
    
    /* draw images onto framebuffer */
    U32 bpp = (image_line_stride/INPUT_IMAGE_WIDTH) * 8;
    U32 xoffset = (framebuffer.screen_info.xres - INPUT_IMAGE_WIDTH)/2;
    U32 y1offset = (framebuffer.screen_info.yres/2 - INPUT_IMAGE_HEIGHT)/2;
    U32 y2offset = (framebuffer.screen_info.yres/2 - INPUT_IMAGE_HEIGHT)/2+framebuffer.screen_info.yres/2;    
    draw_pixels( xoffset, y1offset, input_image, INPUT_IMAGE_WIDTH, INPUT_IMAGE_HEIGHT, bpp );
    draw_pixels( xoffset, y2offset, output_image, INPUT_IMAGE_WIDTH, INPUT_IMAGE_HEIGHT, bpp );

    /* perform bitmach */
    U32 bitmach = 0;
    for( i = 0; i < INPUT_IMAGE_HEIGHT * image_line_stride; ++i )
    {
	bitmach += (( input_image[i] - output_image[i] ) * ( input_image[i] - output_image[i] ));		
    }
    
    if( bitmach != 0 )
    {
	tst_resm( TFAIL, "Bitmach failed" );
	return rv;	
    }
    else
	tst_resm( TPASS, "Bitmach passed" );

    rv = TPASS;
    return rv;
}

/*================================================================================================*/
/*===== parse_sequense =====*/
/**
@brief  Parsing of operations sequense

@param  sequense - a opl operation sequense, like this "mMrR" where:
		   r - CW rotating, R - CCW rotating, m - horizontal mirrorring, 
		   M - vertical mirrorring    
  
@return None
*/
/*================================================================================================*/
void parse_sequense( char * sequense )
{
    if( 0 == sequense )
	return; 
    
    do
    {
	switch( *sequense )
	{
	    case 'r':
		opl_op_array[opl_op_count++] = oplIPRotate_CW;
		break;
	    case 'R':
		opl_op_array[opl_op_count++] = oplIPRotate_CCW;
		break;
	    case 'm':
		opl_op_array[opl_op_count++] = oplIPMirrorVert;
		break;
	    case 'M': 
		opl_op_array[opl_op_count++] = oplIPMirrorHoriz;
		break;
	    default:
		if( *sequense != 0 )
		    fprintf( stderr, "parse error: unknown option '%c'\n", *sequense );		
		break;
	}
    }
    while( *sequense++ );
}

/*================================================================================================*/
/*===== RGB_to_U32  =====*/
/**
@brief  Converting from RGB888 to RGB8888 pixel format

@param  rgb888 - input image (RGB888)
	rgb565 - output image (RGB8888)
	width  - input/output image width
	height - input/output image height
  
@return None
*/
/*================================================================================================*/
void RGB_to_U32( U8 * rgb888, U8 * u32, U32 width, U32 height )
{
    if( !rgb888 || !u32 )
    {
	fprintf( stderr, "RGB_to_U32 : invalid arguments\n" );
	return;
    }

    printf( "converting RGB to U8, please wait..." );    
    fflush( stdout );
    
    U32 i;
    for( i = 0; i < width*height; ++i )
    {
	*u32++ = *rgb888++;
	*u32++ = *rgb888++;
	*u32++ = *rgb888++;
	*u32++ = 255;
    }
    
    printf( " ok\n" );
    
}

/*================================================================================================*/
/*===== RGB_to_U16  =====*/
/**
@brief  Converting from RGB888 to RGB565 pixel format

@param  rgb888 - input image (RGB888)
	rgb565 - output image (RGB565)
	width  - input/output image width
	height - input/output image height
  
@return None
*/
/*================================================================================================*/
void RGB_to_U16( U8 * rgb888, U8 * rgb565, U32 width, U32 height )
{
    if( !rgb888 || !rgb565 )
    {
	fprintf( stderr, "RGB_to_U16 : invalid arguments\n" );
	return;
    }
    
    printf( "converting RGB to U16(565), please wait..." );
    fflush( stdout );

    U8 * prgb888 = rgb888;
    U16 * prgb565 = (U16*)rgb565;
    
    U8 r, g, b;
    U32 i;
    for( i = 0; i < height*width; ++i )
    {
        b = (U8)((float)*prgb888++ / 8);    	    
	g = (U8)((float)*prgb888++ / 8);
	r = (U8)((float)*prgb888++ / 8);
	    
	*prgb565    = (r << 11);
	*prgb565   |= (g << 6 );
	*prgb565++ |= (b);
    }    
    
    printf( " ok\n" );
}

/*================================================================================================*/
/*===== RGB_to_U8  =====*/
/**
@brief  Converting from RGB888 to grayscale pixel format

@param  rgb888 - input image (RGB888)
	u8     - output image (grayscale)
	width  - input/output image width
	height - input/output image height
  
@return None
*/
/*================================================================================================*/
void RGB_to_U8( U8 * rgb888, U8 * u8, U32 width, U32 height )
{
	U8 r,g,b;
    if( !rgb888 || !u8 )
    {
	fprintf( stderr, "RGB_to_U8 : invalid arguments\n" );
	return;
    }

    printf( "converting RGB to U8, please wait..." );    
    fflush( stdout );
    
    U32 i;
    for( i = 0; i < width*height; ++i )
    {
    	r = *rgb888++;
	g = *rgb888++;
	b = *rgb888++;
	*u8++ = (r + g + b) / 3;
	
//	*u8++ = (*rgb888++ + *rgb888++ + *rgb888++) / 3;
    }
    
    printf( " ok\n" );
}
void detect_enter()
{
	int fd_console = 0;		/* 0 is the video input */
	fd_set fdset;
	struct timeval timeout;
	char c;

	printf("Press enter to run next test...timeout 15s\n");
	FD_ZERO(&fdset);
	FD_SET(fd_console, &fdset);
	timeout.tv_sec = 15;	/* set timeout !=0 => blocking select */
	timeout.tv_usec = 0;
	if (select(fd_console+1, &fdset, 0, 0, &timeout) > 0)
	{
		do 
		{
			read(fd_console, &c, 1);
		} while (c != 10);	// i.e. line-feed 
	}
}

/*================================================================================================*/
/*===== draw_pixels  =====*/
/**
@brief  Drawing pixels on the framebuffer

@param  x,y    - left-top corner
	pixels - image
	width  - image width
	height - image height
	bpp    - image color depth
  
@return None
*/
/*================================================================================================*/
void draw_pixels( U32 x, U32 y, U8 * pixels, U32 width, U32 height, U32 bpp )
{
    U8 * ptr;
    U16 * pix565 = (U16*)pixels;
    U32 i, j;
    
    U16 pix;
    U8 r ,g ,b;
    for( j = 0; j < height; ++j )
    {
	for( i = 0; i < width; ++i )
	{
		if(framebuffer.screen_info.bits_per_pixel == 24)
		{
			ptr = framebuffer.ptr + ( (y+j) * framebuffer.screen_info.xres + (x+i) ) * 3;
			switch( bpp )
			{
				case 32:
					*ptr++ = *pixels++;
					*ptr++ = *pixels++;
					*ptr++ = *pixels++;
					*pixels++;

				case 24:
					*ptr++ = *pixels++;
					*ptr++ = *pixels++;
					*ptr++ = *pixels++;
					break;

				case 16:
					pix = *pix565++;
					r = (U8)( pix>>11 );
					g = (U8)( (pix>>6) & 0x1f  );
					b = (U8)( pix & 0x1f );						 
					*ptr++ = b*8;
					*ptr++ = g*8;
					*ptr++ = r*8; 
					break;

				case 8:
					*ptr++ = *pixels;
					*ptr++ = *pixels;
					*ptr++ = *pixels++;
					break;   

				default:
					break;
	    		}
		}
		else if(framebuffer.screen_info.bits_per_pixel == 16)
		{
			ptr = framebuffer.ptr + ( (y+j) * framebuffer.screen_info.xres + (x+i) ) * 2;
			switch( bpp )
			{
				case 32:
					r = *pixels++;    	    
					g = *pixels++;
					b = *pixels++;
					*pixels++;
					
					*ptr++ = (U8)((((g>>2)<<5) & 0xE0) | ((b>>3) & 0x1F));
					*ptr++ = (U8)((((g>>2)>>3) & 0x07) | ( r & 0xF8) );

				case 24:
					r = *pixels++;    	    
					g = *pixels++;
					b = *pixels++;

					*ptr++ = (U8)((((g>>2)<<5) & 0xE0) | ((b>>3) & 0x1F));
					*ptr++ = (U8)((((g>>2)>>3) & 0x07) | ( r & 0xF8) );
					
					break;

				case 16:
					*ptr++ = *pixels++;
					*ptr++ = *pixels++;
					break;

				case 8:
					r = *pixels;    	    
					g = *pixels;
					b = *pixels++;

					*ptr++ = (U8)((((g>>2)<<5) & 0xE0) | ((b>>3) & 0x1F));
					*ptr++ = (U8)((((g>>2)>>3) & 0x07) | ( r & 0xF8) );
				
					break;   

				default:
					break;
			}
		}
		else 
			tst_resm(TFAIL, "WRONG bit depth \n Cannot display to the screen \n");
	}
    }
}



#ifdef __cplusplus
}
#endif
