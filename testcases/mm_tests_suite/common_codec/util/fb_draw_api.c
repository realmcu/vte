/*
  Copyright (C) 2004, Freescale Semiconductor, Inc. All Rights Reserved
  THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
  BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
  Freescale Semiconductor, Inc.
*/

/**
@file fb_draw_api.c

@par Portability:
        ARM GCC
*/

/*======================== REVISION HISTORY ==================================

Author (core ID)      Date         CR Number    Description of Changes
-------------------   ----------   ----------   ------------------------------
D.Simakov / smkd001c  31/01/2006   TLSbo61035   Initial version
=============================================================================*/ 

#include <linux/fb.h>
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

#include "fb_draw_api.h"

#if !defined(TRUE) && !defined(FALSE)
    #define TRUE 1
    #define FALSE 0
#endif    

/*****************************************************************************/
/*****************************************************************************/
typedef struct
{
        int                      mFileDesc; /*!< Frame buffer file descriptor FGA for display purpose. */
        struct fb_var_screeninfo mScrInfo;  /*!< Screen parameters. */	
        unsigned long            mSz;       /*!< Size of the videobuffer in bytes. */
        unsigned char          * mPtr;      /*!< Pointer to the frame buffer FGA for display purpose. */    
} sFramebufferCore;

/*****************************************************************************/
/*****************************************************************************/
void _DrawScanline32 ( unsigned char * pScanline, int x, int y, int width, ePixelFormat pfmt );
void _DrawScanline24 ( unsigned char * pScanline, int x, int y, int width, ePixelFormat pfmt );
void _DrawScanline16 ( unsigned char * pScanline, int x, int y, int width, ePixelFormat pfmt );
void _DrawRect       ( int x1, int y1, int x2, int y2, const sColor * color );
void _ClearScreen    ( const sColor * pColor);
void _SetPixel       ( const sColor * pColor, int x, int y );
void _GetPixel       ( sColor * pColor, int x, int y );
int  _Init           ( const char * pDevName );
void _Done           ( void );  


/*****************************************************************************/
/*****************************************************************************/
static sFramebuffer *   gpFramebuffer = NULL;
static sFramebufferCore gFbCore;


/*****************************************************************************/
/*****************************************************************************/
const sFramebuffer * GetFramebuffer( void )
{
        if( !gpFramebuffer )
                if( !_Init( "/dev/fb0" ) )
                        return NULL;
        return gpFramebuffer;	    
}


/*****************************************************************************/
/*****************************************************************************/
int _Init( const char * pDevName )
{
        assert( pDevName );    
        
        /* reinit */
        if( gpFramebuffer )
        {
                _Done();
                assert( gpFramebuffer );
                return _Init( pDevName );
        }
        
        gpFramebuffer = malloc( sizeof(sFramebuffer) ); 
        assert( gpFramebuffer );
        atexit( _Done );
        
        /* Open framebuffer. */ 
        gFbCore.mFileDesc = open( pDevName, O_RDWR );

        /* Get screen infos  (size, resolution).*/
        if( ioctl( gFbCore.mFileDesc, FBIOGET_VSCREENINFO, &gFbCore.mScrInfo ) )
        {                                
                fprintf( stderr, "%s : Error reading variable information [File: %s, line: %d]\n", __FUNCTION__, __FILE__, __LINE__-3 );
                return FALSE;
        }
    
        gFbCore.mSz = gFbCore.mScrInfo.xres * gFbCore.mScrInfo.yres * gFbCore.mScrInfo.bits_per_pixel / 8;
        gpFramebuffer->mWidth = gFbCore.mScrInfo.xres;
        gpFramebuffer->mHeight = gFbCore.mScrInfo.yres;
        /*printf("Display: %dx%dx%d\n", gFbCore.mScrInfo.xres, gFbCore.mScrInfo.yres, gFbCore.mScrInfo.bits_per_pixel );*/
        switch( gFbCore.mScrInfo.bits_per_pixel )
        {
        case 32:
                gpFramebuffer->mPixelFormat = PF_ARGB_8888;
                gpFramebuffer->DrawScanline = _DrawScanline32;        
        case 24:
                gpFramebuffer->mPixelFormat = PF_RGB_888;   
                gpFramebuffer->DrawScanline = _DrawScanline24;
                break;
        case 16: 
                gpFramebuffer->mPixelFormat = PF_RGB_565;   
                gpFramebuffer->DrawScanline = _DrawScanline16;
                break;
        default:
                fprintf( stderr, "%s : %d bits per pixel display mode is not supported [File: %s, line: %d]\n", 
                        __FUNCTION__, gFbCore.mScrInfo.bits_per_pixel, __FILE__, __LINE__ );                
                return FALSE;
        }
        gpFramebuffer->DrawRect    = _DrawRect;
        gpFramebuffer->ClearScreen = _ClearScreen;
        gpFramebuffer->SetPixel    = _SetPixel;
        gpFramebuffer->GetPixel    = _GetPixel;
    
        /* Map the device to memory. */
        gFbCore.mPtr = (unsigned char*)mmap( 0, 
                                             gFbCore.mSz, 
                                             PROT_READ | PROT_WRITE, 
                                             MAP_SHARED, 
                                             gFbCore.mFileDesc, 
                                             0 );
        if( MAP_FAILED == gFbCore.mPtr )
        {
                fprintf( stderr, "%s : Can't mmap framebuffer [File: %s, line: %d]\n",
                        __FUNCTION__, __FILE__, __LINE__-2);
                return FALSE;
        }
        
        memset( gFbCore.mPtr, 0xFF, gFbCore.mSz );
        return TRUE;
}


/*****************************************************************************/
/*****************************************************************************/
void _Done( void )
{
        if( gpFramebuffer )
        {
                free( gpFramebuffer );
                gpFramebuffer = NULL;
        }    
        munmap( gFbCore.mPtr, gFbCore.mSz );
        close( gFbCore.mFileDesc );
}


/*****************************************************************************/
/*****************************************************************************/
void _DrawScanline32( unsigned char * pScanline,
                      int x, int y, int width, ePixelFormat pfmt )
{
        assert( pScanline && width > 0 );    
        
        struct fb_var_screeninfo * sinfo = &gFbCore.mScrInfo;
        
        if(x < 0) x = 0; if(x > sinfo->xres) x = sinfo->xres;
        if(y < 0) y = 0; if(y > sinfo->yres) y = sinfo->yres;
        if(x + width > sinfo->xres)
        { 
                width -= (sinfo->xres - x + width); 
                width = -width; 
        }
        
        int offset = (y * sinfo->xres + x)*4;
        unsigned char * ptr = gFbCore.mPtr + offset;
        
        unsigned char  * p888  = pScanline;
        unsigned short * p16   = (unsigned short*)pScanline;
        unsigned char  * p8888 = pScanline;
        int i;
        
        switch( pfmt )
        {
        case PF_ARGB_8888:
                for( i = 0; i < width; ++i )
                {    
                        unsigned char b = *(p8888++);
                        unsigned char g = *(p8888++);
                        unsigned char r = *(p8888++);
                        unsigned char a = *(p8888++);
                        *(ptr++) = b;
                        *(ptr++) = g;
                        *(ptr++) = r;
                        *(ptr++) = a; 
                }
                break;
        case PF_RGB_888:
                for( i = 0; i < width; ++i )
                {    
                        unsigned char b = *(p888++);
                        unsigned char g = *(p888++);
                        unsigned char r = *(p888++);
                        *(ptr++) = 255;
                        *(ptr++) = r;
                        *(ptr++) = g;
                        *(ptr++) = b;
                }
                break;
        case PF_RGB_565: case PF_RGB_555:
                for( i = 0; i < width; ++i )
                {
                        unsigned short pixel = *(p16++);
                        unsigned char r = (unsigned char)( pixel>>11 );
                        unsigned char g = (unsigned char)( (pixel>>6) & 0x1f  );
                        unsigned char b = (unsigned char)( pixel & 0x1f );
                        *(ptr++) = b*8;
                        *(ptr++) = pfmt==PF_RGB_555 ? g*8 : g*8;
                        *(ptr++) = r*8;
                        *(ptr++) = 255;
                }
                break;
        default:
                assert( !"unsupported pixel format" );
        }    
}


/*****************************************************************************/
/*****************************************************************************/
void _DrawScanline24( unsigned char * pScanline, 
                      int x, int y, int width, ePixelFormat pfmt )
{
        assert( pScanline && width > 0 );    
        
        struct fb_var_screeninfo * sinfo = &gFbCore.mScrInfo;
        
        if(x < 0) x = 0; if(x > sinfo->xres) x = sinfo->xres;
        if(y < 0) y = 0; if(y > sinfo->yres) y = sinfo->yres;
        if(x + width > sinfo->xres) 
        { 
                width -= (sinfo->xres - x + width); 
                width = -width; 
        }
        
        int offset = (y * sinfo->xres + x)*3;
        unsigned char * ptr = gFbCore.mPtr + offset;
        
        unsigned char  * p888  = pScanline;
        unsigned short * p16   = (unsigned short*)pScanline;
        unsigned char  * p8888 = pScanline;
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
                        *(ptr++) = r;
                        *(ptr++) = g;
                        *(ptr++) = b;
                }
                break;
        case PF_RGB_888:
                for( i = 0; i < width; ++i )
                {    
                        unsigned char b = *(p888++);
                        unsigned char g = *(p888++);
                        unsigned char r = *(p888++);
                        *(ptr++) = r;
                        *(ptr++) = g;
                        *(ptr++) = b;
                }
                break;
        case PF_RGB_565: case PF_RGB_555:	
                for( i = 0; i < width; ++i )
                {
                        unsigned short pixel = *(p16++);
                        unsigned char r = (unsigned char)( pixel>>11 );
                        unsigned char g = (unsigned char)( (pixel>>6) & 0x1f  );
                        unsigned char b = (unsigned char)( pixel & 0x1f );
                        *(ptr++) = b*8;
                        *(ptr++) = pfmt==PF_RGB_555 ? g*8 : g*8;
                        *(ptr++) = r*8;
                }
                break;
        default:
                assert( !"unsupported pixel format" );
        }
}


/*****************************************************************************/
/*****************************************************************************/
void _DrawScanline16( unsigned char * pScanline, 
                      int x, int y, int width, ePixelFormat pfmt )
{
        assert( pScanline && width > 0 );        
        
        struct fb_var_screeninfo * sinfo = &gFbCore.mScrInfo;
        
        if(x < 0) x = 0; if(x > sinfo->xres) x = sinfo->xres;
        if(y < 0) y = 0; if(y > sinfo->yres) y = sinfo->yres;
        if(x + width > sinfo->xres) 
        { 
                width -= (sinfo->xres - x + width); 
                width = -width; 
        }
        
        int offset = (y * sinfo->xres + x)*2;
        unsigned char * ptr = gFbCore.mPtr + offset;
        
        unsigned char  * p888   = pScanline;
        unsigned short * p16    = (unsigned short*)pScanline;    
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


/*****************************************************************************/
/*****************************************************************************/
void _DrawRect( int x1, int y1, int x2, int y2, const sColor * pColor )
{
        assert( pColor );
        
        int x, y;

        for( y = y1; y < y2; ++y )
                for( x = x1; x < x2; ++x )
                        _SetPixel( pColor, x, y );

}


/*****************************************************************************/
/*****************************************************************************/
void _ClearScreen( const sColor * pColor )
{    
        if( pColor->mArray[CC_RED]   == pColor->mArray[CC_GREEN] && 
            pColor->mArray[CC_GREEN] == pColor->mArray[CC_BLUE] )
        {
                memset( gFbCore.mPtr, pColor->mArray[CC_RED], gFbCore.mSz );
        }
        else
        {
                _DrawRect( 0, 0, gpFramebuffer->mWidth, gpFramebuffer->mHeight, pColor );
        }                
}


/*****************************************************************************/
/*****************************************************************************/
void _SetPixel( const sColor * pColor, int x, int y )
{
        assert( pColor );
        unsigned char  r24 = pColor->mArray[CC_BLUE];
        unsigned char  g24 = pColor->mArray[CC_GREEN];
        unsigned char  b24 = pColor->mArray[CC_RED];        
        unsigned short r16 = (unsigned char)((float)pColor->mArray[CC_RED]/8) << (6+5);
        unsigned short g16 = (unsigned char)((float)pColor->mArray[CC_GREEN]/4) << (5);
        unsigned short b16 = (unsigned char)((float)pColor->mArray[CC_BLUE]/8);
        int offset;
            
        struct fb_var_screeninfo * sinfo = &gFbCore.mScrInfo;
        switch( sinfo->bits_per_pixel )
        {
        case 32:
                offset = (y*sinfo->xres+x)*4;
                if( offset < gFbCore.mSz )
                {                
                        unsigned char * p32 = gFbCore.mPtr + offset;
                        *p32++ = 0xff;
                        *p32++ = r24;
                        *p32++ = g24;
                        *p32++ = b24;                        
                }                
                break;
        case 24:
                offset = (y*sinfo->xres+x)*3;
                if( offset < gFbCore.mSz )
                {                
                        unsigned char * p24 = gFbCore.mPtr + offset;
                        *p24++ = r24;
                        *p24++ = g24;
                        *p24++ = b24;                        
                }                
                break;
        case 16:
                offset = (y*sinfo->xres+x)*2;
                if( offset < gFbCore.mSz )
                {                        
                        unsigned short * p16 = (unsigned short*)(gFbCore.mPtr + offset);
                        *p16  = r16;
                        *p16 |= g16;
                        *p16 |= b16;
                }
                break;
        default:
                assert( !"unsupported display mode" );
        }
}


/*****************************************************************************/
/*****************************************************************************/
void _GetPixel( sColor * pColor, int x, int y )
{
        assert( !"not implemented yet" );
}
