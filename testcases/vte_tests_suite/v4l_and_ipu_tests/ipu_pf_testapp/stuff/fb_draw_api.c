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
        @file   fb_draw_api.c

        @brief  Test scenario
====================================================================================================
Revision History:
                            Modification     Tracking
Author/core ID                  Date          Number    Description of Changes
-------------------------   ------------    ----------  -------------------------------------------
D.Simakov/smkd001c           21/09/2005     TLSbo55077   Initial version

====================================================================================================
Portability: ARM GCC

==================================================================================================*/

/*==================================================================================================
                                        INCLUDE FILES
==================================================================================================*/

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <string.h>
#include <unistd.h>
#include <linux/fb.h>

#include "fb_draw_api.h"

/*==================================================================================================
                                       DEFINES AND MACROS
==================================================================================================*/
#if !defined(TRUE) && !defined(FALSE)
    #define TRUE 1
    #define FALSE 0
#endif

/*==================================================================================================
                                 STRUCTURES AND OTHER TYPEDEFS
==================================================================================================*/
typedef struct
{
        int file_desc;                        /*!< frame buffer file descriptor FGA for display purpose */
        struct fb_var_screeninfo screen_info; /*!< screen parameters */
        unsigned long size;                   /*!< size of the videobuffer in bytes */
        unsigned char * ptr;                  /*!< pointer to frame buffer FGA for display purpose */
} framebuffer_core_t;

/*==================================================================================================
                                     FUNCTION PROTOTYPES
==================================================================================================*/
void _draw_scanline_24( unsigned char * scanline, int x, int y, int width, pixel_format_e pfmt );
void _draw_scanline_16( unsigned char * scanline, int x, int y, int width, pixel_format_e pfmt );
void _draw_rect( int x1, int y1, int x2, int y2, const argb_color_t * color );
void _clear_screen( const argb_color_t * color );
void _set_pixel( const argb_color_t * color );
void _get_pixel( argb_color_t * color );
int  _fb_init( const char * fb_device );
void _fb_done( void );  


/*****************************************************************************/
/*****************************************************************************/
static framebuffer_t * g_framebuffer = NULL;
static framebuffer_core_t g_fb_core;

/*****************************************************************************/
/*! Get framebuffer instance.
 *
 *  \return
 *  Returns the framebuffer_t instance or NULL if an errors are occured. */	
/*****************************************************************************/
const framebuffer_t * get_framebuffer( void )
{
        if( !g_framebuffer )
                if( !_fb_init( "/dev/fb0" ) )
                        return NULL;
    return g_framebuffer;
}

/*****************************************************************************/
/*! Init routine. 
 * 
 *  \param fb_device \n[in] Famebuffer device name.	
 *
 *  \return
 *  all ok ? TRUE : FALSE */
/*****************************************************************************/
int _fb_init( const char * fb_device )
{
        assert( fb_device );    
        
        /* reinit */
        if( g_framebuffer )
        {
                _fb_done();
                assert( g_framebuffer );
                return _fb_init( fb_device );
        }
        
        g_framebuffer = malloc( sizeof(framebuffer_t) );    
        atexit( _fb_done );
        
        /* open framebuffer */ 
        g_fb_core.file_desc = open( fb_device, O_RDWR );
        
        /* get screen infos  (size, resolution)*/
        if( ioctl( g_fb_core.file_desc, FBIOGET_VSCREENINFO, &g_fb_core.screen_info ) )
        {
                fprintf( stderr, "_fb_init : error reading variable information\n" );
                return FALSE;
        }
        
        g_fb_core.size = g_fb_core.screen_info.xres * g_fb_core.screen_info.yres * g_fb_core.screen_info.bits_per_pixel / 8;
        g_framebuffer->width = g_fb_core.screen_info.xres;
        g_framebuffer->height = g_fb_core.screen_info.yres;
        switch( g_fb_core.screen_info.bits_per_pixel )
        {
        case 24:
                g_framebuffer->pixel_format = PF_RGB_888;   
                g_framebuffer->draw_scanline = _draw_scanline_24;
                g_framebuffer->_fb_pitch = g_framebuffer->width * 3;
                break;
        case 16: 
                g_framebuffer->pixel_format = PF_RGB_565;   
                g_framebuffer->draw_scanline = _draw_scanline_16;
                g_framebuffer->_fb_pitch = g_framebuffer->width * 2;
                break;
        default:
                fprintf( stderr, "_fb_init : %d bits per pixel display mode is not supported\n", g_fb_core.screen_info.bits_per_pixel );
                return FALSE;
        }
        g_framebuffer->draw_rect = _draw_rect;
        g_framebuffer->clear_screen = _clear_screen;
        g_framebuffer->set_pixel = _set_pixel;
        g_framebuffer->get_pixel = _get_pixel;
        
        /* map the device to memory */
        g_fb_core.ptr = (unsigned char*)mmap( 0, g_fb_core.size, PROT_READ|PROT_WRITE, MAP_SHARED, g_fb_core.file_desc, 0 );
        if( (int)g_fb_core.ptr == -1 )
        {
                fprintf( stderr, "_fb_init : error: failed to map framebuffer device to memory.\n" );
                return FALSE;
        }
        g_framebuffer->_fb_direct_ptr = g_fb_core.ptr;
        
        memset( g_fb_core.ptr, 0xFF, g_fb_core.size );
        return TRUE;
}

/*****************************************************************************/
/*! Close and destroy all. */
/*****************************************************************************/
void _fb_done( void )
{
        if( g_framebuffer )
        {
                free( g_framebuffer );
                g_framebuffer = NULL;
        }    
        munmap( g_fb_core.ptr, g_fb_core.size );
        close( g_fb_core.file_desc );
}


/*****************************************************************************/
/*****************************************************************************/
void _draw_scanline_24( unsigned char * scanline, int x, int y, int width, pixel_format_e pfmt )
{
        assert( scanline );
        assert( width > 0 );
        
        struct fb_var_screeninfo * sinfo = &g_fb_core.screen_info;
        
        if(x < 0) x = 0; if(x > sinfo->xres) x = sinfo->xres;
        if(y < 0) y = 0; if(y > sinfo->yres) y = sinfo->yres;
        if(x + width > sinfo->xres) { width -= (sinfo->xres - x + width); width = -width; }
        
        int offset = (y * sinfo->xres + x)*3;
        unsigned char * ptr = g_fb_core.ptr + offset;
        
        unsigned char  * p888  = scanline;
        unsigned short * p16   = (unsigned short*)scanline;
        unsigned char  * p8888 = scanline;
        int i;
        
        switch( pfmt )
        {
        case PF_ARGB_8888:
                for( i = 0; i < width; ++i )
                {
                        p8888++; // skip alpha
                        unsigned char b = *(p8888++);
                        unsigned char g = *(p8888++);
                        unsigned char r = *(p8888++);
                        *(ptr++) = b;
                        *(ptr++) = g;
                        *(ptr++) = r;
                }
                break;
        case PF_RGB_888:
                for( i = 0; i < width; ++i )
                {    
                        unsigned char b = *(p888++);
                        unsigned char g = *(p888++);
                        unsigned char r = *(p888++);
                        *(ptr++) = b;
                        *(ptr++) = g;
                        *(ptr++) = r;
                }
                break;
        case PF_RGB_565: case PF_RGB_555:
                for( i = 0; i < width; ++i )
                {
                        unsigned short pixel = *(p16++);
                        unsigned char r = (unsigned char)( pixel>>11 );
                        unsigned char g = (unsigned char)( (pixel>>6) & 0x1f  );
                        unsigned char b = (unsigned char)( pixel & 0x1f );
                        *(ptr++) = r*8;
                        *(ptr++) = pfmt==PF_RGB_555 ? g*8 : g*8;
                        *(ptr++) = b*8;
                }
                break;
        default:
                assert( !"unsupported pixel format" );
        }
}

void _draw_scanline_16( unsigned char * scanline, int x, int y, int width, pixel_format_e pfmt )
{
        assert( scanline );
        assert( width > 0 );
        
        struct fb_var_screeninfo * sinfo = &g_fb_core.screen_info;
        
        if(x < 0) x = 0; if(x > sinfo->xres) x = sinfo->xres;
        if(y < 0) y = 0; if(y > sinfo->yres) y = sinfo->yres;
        if(x + width > sinfo->xres) { width -= (sinfo->xres - x + width); width = -width; }
        
        int offset = (y * sinfo->xres + x)*2;
        unsigned char * ptr = g_fb_core.ptr + offset;
        
        unsigned char  * p888  = scanline;
        unsigned short * p16   = (unsigned short*)scanline;    
        unsigned short * dest16 = (unsigned short*)ptr;
        int i;
        
        switch( pfmt )
        {
        case PF_RGB_888:
                for( i = 0; i < width; ++i )
                {    
                        unsigned char r = *p888++;
                        unsigned char g = *p888++;
                        unsigned char b = *p888++;
                        *ptr++ = (unsigned char)((((g>>2)<<5) & 0xE0) | ((b>>3) & 0x1F));
                        *ptr++ = (unsigned char)((((g>>2)>>3) & 0x07) | ( r & 0xF8) );
                }
                break;
        case PF_RGB_565: case PF_RGB_555:	
                for( i = 0; i < width; ++i )
                {
                        *dest16++ = *p16++;
                }
                break;
        default:
                assert( !"unsupported pixel format" );
        }
}

void _draw_rect( int x1, int y1, int x2, int y2, const argb_color_t * color )
{
        assert( color );
        
        struct fb_var_screeninfo * sinfo = &g_fb_core.screen_info;
        if(x1 < 0) x1 = 0; if(x1 > sinfo->xres) x1 = sinfo->xres;  
        if(x2 < 0) x2 = 0; if(x2 > sinfo->xres) x2 = sinfo->xres;  
        if(y1 < 0) y1 = 0; if(y1 > sinfo->yres) y1 = sinfo->yres;  
        if(y2 < 0) y2 = 0; if(y2 > sinfo->yres) y2 = sinfo->yres; 
        if((x1 >= x2) || (y1 >= y2)) return;
        
        int ix, iy;
        unsigned char r24 = (unsigned char)(color->r*255);
        unsigned char g24 = (unsigned char)(color->g*255);
        unsigned char b24 = (unsigned char)(color->b*255);
        
        unsigned short r16 = (unsigned char)(color->r*31) << (6+5);
        unsigned short g16 = (unsigned char)(color->r*63) << (5);
        unsigned short b16 = (unsigned char)(color->r*31);
        
        switch( sinfo->bits_per_pixel )
        {
        case 24:
                for( iy = y1; iy <= y2; ++iy )
                {   
                        int offset = (iy*sinfo->xres+x1)*3;
                        unsigned char * p24 = g_fb_core.ptr + offset;
                        for( ix = 0; ix <(x2-x1); ++ix )
                        {
                                *p24++ = r24;
                                *p24++ = g24;
                                *p24++ = b24;	
                        }
                }
                break;
        case 16:
                for( iy = y1; iy <= y2; ++iy )
                {
                        int offset = (iy*sinfo->xres+x1)*2;
                        unsigned short * p16 = (unsigned short*)(g_fb_core.ptr + offset);
                        for( ix = 0; ix < (x2-x1); ++ix )
                        {
                                *p16  = r16;
                                *p16 |= g16;
                                *p16 |= b16;
                        }
                }
                break;
        default:
                assert( !"unsupported display mode" );
        }
}

void _clear_screen( const argb_color_t * color )
{    
        memset( g_fb_core.ptr, 0xff, g_fb_core.size );
}

void _set_pixel( const argb_color_t * color )
{
        assert( !"not implemented yet" );
}

void _get_pixel( argb_color_t * color )
{
        assert( !"not implemented yet" );
}
