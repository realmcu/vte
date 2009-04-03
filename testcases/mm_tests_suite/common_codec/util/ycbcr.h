/*
  Copyright (C) 2004, Freescale Semiconductor, Inc. All Rights Reserved
  THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
  BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
  Freescale Semiconductor, Inc.
*/

/**
@file ycbcr.h

@par Portability:
        ARM GCC
*/

/*======================== REVISION HISTORY ==================================

Author (core ID)      Date         CR Number    Description of Changes
-------------------   ----------   ----------   ------------------------------
D.Simakov / smkd001c  21/02/2006   TLSbo61035   Initial version
=============================================================================*/ 

#ifndef __YCBCR_H__
#define __YCBCR_H__

#define RGB_888                     1   
#define RGB_565                     2   

int YCrCbToRGB( unsigned char *Y, int xsize,
                unsigned char *Cb, unsigned char *Cr, int cxsize,
                unsigned char *rgbPixelPtr, int width, int height, int colorFormat );

#endif //__YCBCR_H__
