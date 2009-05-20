/*
  Copyright (C) 2004, Freescale Semiconductor, Inc. All Rights Reserved
  THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
  BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
  Freescale Semiconductor, Inc.
*/

/**
@file fb_draw_api.h

@par Portability:
        ARM GCC
*/

/*======================== REVISION HISTORY ==================================

Author (core ID)      Date         CR Number    Description of Changes
-------------------   ----------   ----------   ------------------------------
D.Simakov / smkd001c  31/01/2006   TLSbo61035   Initial version
=============================================================================*/

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
} ePixelFormat;


/*****************************************************************************/
/*****************************************************************************/
typedef enum
{
#ifdef  BGR_COLOR /* BGR */
        CC_BLUE,
        CC_GREEN,
        CC_RED,
#else             /* RGB */
        CC_RED,
        CC_GREEN,
        CC_BLUE,
#endif
        CC_ALPHA
} eColorComponent;


/*****************************************************************************/
/*****************************************************************************/
typedef union
{
        struct
        {
#ifdef  BGR_COLOR /* BGR */
                unsigned char mBlue;
                unsigned char mGreen;
                unsigned char mRed;
#else             /* RGB */
                unsigned char mRed;
                unsigned char mGreen;
                unsigned char mBlue;
#endif
        unsigned char mAlpha;
        } mColor;
        unsigned long mAll;
        unsigned char mArray[4];
} sColor;

/*****************************************************************************/
/*****************************************************************************/
typedef void (*pDrawScanlineFnc)(unsigned char*, int, int, int, ePixelFormat);
typedef void (*pDrawRectFnc)    (int, int, int, int, const sColor*);
typedef void (*pClearScreenFnc) (const sColor*);
typedef void (*pSetPixelFnc)    (const sColor*, int, int);
typedef void (*pGetPixelFnc)    (sColor*, int, int);

/*****************************************************************************/
/*****************************************************************************/
typedef struct
{
    /* methods */
    pDrawScanlineFnc DrawScanline;
    pDrawRectFnc     DrawRect;
    pClearScreenFnc  ClearScreen;
    pSetPixelFnc     SetPixel;
    pGetPixelFnc     GetPixel;

    /* fields */
    int              mWidth;
    int              mHeight;
    ePixelFormat     mPixelFormat;
} sFramebuffer;

/*****************************************************************************/
/*****************************************************************************/
const sFramebuffer * GetFramebuffer( void );

#endif /* __FB_DRAW_API_H__ */
