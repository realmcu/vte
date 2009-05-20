/*================================================================================================*/
/**
        @file   ycbcr.h
        @brief
*/
/*==================================================================================================

        Copyright (C) 2004, Freescale Semiconductor, Inc. All Rights Reserved
        THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
        BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
        Freescale Semiconductor, Inc.

====================================================================================================
Revision History:
                            Modification     Tracking
Author/core ID                  Date          Number    Description of Changes
-------------------------   ------------    ----------  -------------------------------------------
Kardakov Dmitriy/ID          18/12/2006     TLSbo80545   Initial version

==================================================================================================*/

/*==================================================================================================
                                       DEFINES AND MACROS
==================================================================================================*/
#ifndef YCBCR_H_
#define YCBCR_H_

/************ Color formats  ****************/
#define Y_CB_CR                     0   /* !< Bypass: No color conversion */
#define RGB_888                     1   /* !< 24 bit RGB */
#define RGB_565                     2   /* !< 16 bit RGB */
#define RGB_444                     3   /* !< 12 bit RGB */

/************* RGB orientation  ***************/
#define RGB_ORIENT_NORMAL           0
#define RGB_ORIENT_UPSIDEDOWN       1

int     YCbCrToRGB (unsigned char *Y, int xsize, unsigned char *Cb,
                    unsigned char *Cr, int cxsize, unsigned char *rgbPixelPtr,
                    int width, int height, int rgbOrientation, int colorFormat);

int     YCbYCrToRGB (unsigned char * YCbYCr, int stride,  unsigned char *rgbPixelPtr,
                     int width, int height, int rgbOrientation, int colorFormat);

#endif        /* #ifndef YCBCR_H_ */
