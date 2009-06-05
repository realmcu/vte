/*****************************************************************************/
/*! \file    fb_draw_api.h
 *
 *  \author  Dmitry O. Simakov */
/*****************************************************************************/
#ifndef __FB_DRAW_API_H__
#define __FB_DRAW_API_H__

/*****************************************************************************/
/*****************************************************************************/
typedef enum
{
    PF_RGB_555 = 0,
    PF_RGB_565,
    PF_RGB_888,
    PF_ARGB_8888,
    PF_XRGB_8888,
} pixel_format_e;

/*****************************************************************************/
/*****************************************************************************/
typedef struct 
{
    float a, r, g, b;    
} argb_color_t;

/*****************************************************************************/
/*****************************************************************************/
typedef void (*draw_scanline_t)(unsigned char*, int, int, int, pixel_format_e); 
typedef void (*draw_rect_t)    (int, int, int, int, const argb_color_t*);
typedef void (*clear_screen_t) (const argb_color_t*);
typedef void (*set_pixel_t)    (const argb_color_t*);
typedef void (*get_pixel_t)    (argb_color_t*);

/*****************************************************************************/
/*****************************************************************************/
typedef struct 
{
    /* methods */
    draw_scanline_t draw_scanline;
    draw_rect_t     draw_rect;
    clear_screen_t  clear_screen;
    set_pixel_t     set_pixel;
    get_pixel_t     get_pixel;
    
    /* fields */
    int width;
    int height;
    pixel_format_e pixel_format;    
    
    /* Unsafe... */
    unsigned char * _fb_direct_ptr;
    int             _fb_pitch;
} framebuffer_t;

/*****************************************************************************/
/*****************************************************************************/
const framebuffer_t * get_framebuffer();

#endif /* __FB_DRAW_API_H__ */
