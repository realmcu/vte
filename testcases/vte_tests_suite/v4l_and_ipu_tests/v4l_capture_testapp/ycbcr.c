/***
**Copyright 2006-2009 Freescale Semiconductor, Inc. All Rights Reserved.
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
        @file   ycbcr.c  
  
        @brief  Contains functions for conversion YCbCr format to RGB format
====================================================================================================
Revision History:
                            Modification     Tracking
Author/core ID                  Date          Number    Description of Changes
-------------------------   ------------    ----------  -------------------------------------------
Kardakov Dmitriy/ID          18/12/2006     TLSbo80545   Initial version

==================================================================================================*/

/*==================================================================================================
                                         INCLUDE FILES
==================================================================================================*/
#include <stdlib.h>
#include <stdio.h>
#include "ycbcr.h"

/*==================================================================================================
                                       DEFINES AND MACROS
==================================================================================================*/
#define SUPPORT_RGB_888     0
#define SUPPORT_RGB_565     1
#define SUPPORT_RGB_444     0

/*==================================================================================================
        Conversion Algorithm:
        Currently only look-up table based color conversion is supported.
        If equation based computation is required, uncoment RGB_LOOKUP
==================================================================================================*/
#define RGB_LOOKUP

#ifdef RGB_LOOKUP

const short int r_cr[256] = 
{
            -204, -202, -200, -199, -197, -196, -194, -192,
            -191, -189, -188, -186, -184, -183, -181, -180,
            -178, -176, -175, -173, -172, -170, -169, -167,
            -165, -164, -162, -161, -159, -157, -156, -154,
            -153, -151, -149, -148, -146, -145, -143, -142,
            -140, -138, -137, -135, -134, -132, -130, -129,
            -127, -126, -124, -122, -121, -119, -118, -116,
            -114, -113, -111, -110, -108, -107, -105, -103,
            -102, -100, -99, -97, -95, -94, -92, -91,
            -89, -87, -86, -84, -83, -81, -80, -78,
            -76, -75, -73, -72, -70, -68, -67, -65,
            -64, -62, -60, -59, -57, -56, -54, -52,
            -51, -49, -48, -46, -45, -43, -41, -40,
            -38, -37, -35, -33, -32, -30, -29, -27,
            -25, -24, -22, -21, -19, -17, -16, -14,
            -13, -11, -10, -8, -6, -5, -3, -2,
            0, 2, 3, 5, 6, 8, 10, 11,
            13, 14, 16, 17, 19, 21, 22, 24,
            25, 27, 29, 30, 32, 33, 35, 37,
            38, 40, 41, 43, 45, 46, 48, 49,
            51, 52, 54, 56, 57, 59, 60, 62,
            64, 65, 67, 68, 70, 72, 73, 75,
            76, 78, 80, 81, 83, 84, 86, 87,
            89, 91, 92, 94, 95, 97, 99, 100,
            102, 103, 105, 107, 108, 110, 111, 113,
            114, 116, 118, 119, 121, 122, 124, 126,
            127, 129, 130, 132, 134, 135, 137, 138,
            140, 142, 143, 145, 146, 148, 149, 151,
            153, 154, 156, 157, 159, 161, 162, 164,
            165, 167, 169, 170, 172, 173, 175, 176,
            178, 180, 181, 183, 184, 186, 188, 189,
            191, 192, 194, 196, 197, 199, 200, 202
};

const short int g_cr[256] = 
{
            104, 103, 103, 102, 101, 100, 99, 99,
            98, 97, 96, 95, 95, 94, 93, 92,
            91, 90, 90, 89, 88, 87, 86, 86,
            85, 84, 83, 82, 81, 81, 80, 79,
            78, 77, 77, 76, 75, 74, 73, 73,
            72, 71, 70, 69, 68, 68, 67, 66,
            65, 64, 64, 63, 62, 61, 60, 59,
            59, 58, 57, 56, 55, 55, 54, 53,
            52, 51, 51, 50, 49, 48, 47, 46,
            46, 45, 44, 43, 42, 42, 41, 40,
            39, 38, 37, 37, 36, 35, 34, 33,
            33, 32, 31, 30, 29, 29, 28, 27,
            26, 25, 24, 24, 23, 22, 21, 20,
            20, 19, 18, 17, 16, 15, 15, 14,
            13, 12, 11, 11, 10, 9, 8, 7,
            7, 6, 5, 4, 3, 2, 2, 1,
            0, -1, -2, -2, -3, -4, -5, -6,
            -7, -7, -8, -9, -10, -11, -11, -12,
            -13, -14, -15, -15, -16, -17, -18, -19,
            -20, -20, -21, -22, -23, -24, -24, -25,
            -26, -27, -28, -29, -29, -30, -31, -32,
            -33, -33, -34, -35, -36, -37, -37, -38,
            -39, -40, -41, -42, -42, -43, -44, -45,
            -46, -46, -47, -48, -49, -50, -51, -51,
            -52, -53, -54, -55, -55, -56, -57, -58,
            -59, -59, -60, -61, -62, -63, -64, -64,
            -65, -66, -67, -68, -68, -69, -70, -71,
            -72, -73, -73, -74, -75, -76, -77, -77,
            -78, -79, -80, -81, -81, -82, -83, -84,
            -85, -86, -86, -87, -88, -89, -90, -90,
            -91, -92, -93, -94, -95, -95, -96, -97,
            -98, -99, -99, -100, -101, -102, -103, -103
};

const short int g_cb[256] = 
{
            50, 49, 49, 49, 48, 48, 47, 47,
            47, 46, 46, 45, 45, 45, 44, 44,
            43, 43, 43, 42, 42, 42, 41, 41,
            40, 40, 40, 39, 39, 38, 38, 38,
            37, 37, 37, 36, 36, 35, 35, 35,
            34, 34, 33, 33, 33, 32, 32, 31,
            31, 31, 30, 30, 30, 29, 29, 28,
            28, 28, 27, 27, 26, 26, 26, 25,
            25, 24, 24, 24, 23, 23, 23, 22,
            22, 21, 21, 21, 20, 20, 19, 19,
            19, 18, 18, 17, 17, 17, 16, 16,
            16, 15, 15, 14, 14, 14, 13, 13,
            12, 12, 12, 11, 11, 10, 10, 10,
            9, 9, 9, 8, 8, 7, 7, 7,
            6, 6, 5, 5, 5, 4, 4, 3,
            3, 3, 2, 2, 2, 1, 1, 0,
            0, 0, -1, -1, -2, -2, -2, -3,
            -3, -3, -4, -4, -5, -5, -5, -6,
            -6, -7, -7, -7, -8, -8, -9, -9,
            -9, -10, -10, -10, -11, -11, -12, -12,
            -12, -13, -13, -14, -14, -14, -15, -15,
            -16, -16, -16, -17, -17, -17, -18, -18,
            -19, -19, -19, -20, -20, -21, -21, -21,
            -22, -22, -23, -23, -23, -24, -24, -24,
            -25, -25, -26, -26, -26, -27, -27, -28,
            -28, -28, -29, -29, -30, -30, -30, -31,
            -31, -31, -32, -32, -33, -33, -33, -34,
            -34, -35, -35, -35, -36, -36, -37, -37,
            -37, -38, -38, -38, -39, -39, -40, -40,
            -40, -41, -41, -42, -42, -42, -43, -43,
            -43, -44, -44, -45, -45, -45, -46, -46,
            -47, -47, -47, -48, -48, -49, -49, -49
};

const short int b_cb[256] = 
{
            -258, -256, -254, -252, -250, -248, -246, -244,
            -242, -240, -238, -236, -234, -232, -230, -228,
            -226, -224, -222, -220, -218, -216, -214, -212,
            -210, -208, -206, -204, -202, -200, -198, -196,
            -194, -192, -190, -188, -186, -184, -181, -179,
            -177, -175, -173, -171, -169, -167, -165, -163,
            -161, -159, -157, -155, -153, -151, -149, -147,
            -145, -143, -141, -139, -137, -135, -133, -131,
            -129, -127, -125, -123, -121, -119, -117, -115,
            -113, -111, -109, -107, -105, -103, -101, -99,
            -97, -95, -93, -91, -89, -87, -85, -83,
            -81, -79, -77, -75, -73, -71, -69, -67,
            -65, -63, -60, -58, -56, -54, -52, -50,
            -48, -46, -44, -42, -40, -38, -36, -34,
            -32, -30, -28, -26, -24, -22, -20, -18,
            -16, -14, -12, -10, -8, -6, -4, -2,
            0, 2, 4, 6, 8, 10, 12, 14,
            16, 18, 20, 22, 24, 26, 28, 30,
            32, 34, 36, 38, 40, 42, 44, 46,
            48, 50, 52, 54, 56, 58, 60, 63,
            65, 67, 69, 71, 73, 75, 77, 79,
            81, 83, 85, 87, 89, 91, 93, 95,
            97, 99, 101, 103, 105, 107, 109, 111,
            113, 115, 117, 119, 121, 123, 125, 127,
            129, 131, 133, 135, 137, 139, 141, 143,
            145, 147, 149, 151, 153, 155, 157, 159,
            161, 163, 165, 167, 169, 171, 173, 175,
            177, 179, 181, 184, 186, 188, 190, 192,
            194, 196, 198, 200, 202, 204, 206, 208,
            210, 212, 214, 216, 218, 220, 222, 224,
            226, 228, 230, 232, 234, 236, 238, 240,
            242, 244, 246, 248, 250, 252, 254, 256
};

#if (SUPPORT_RGB_444)
/*==================================================================================================
        Use this clipping array when 4-bit output is required.
        These values are calculated using clipitArr4[i] = clipitArr[i] >> 4
==================================================================================================*/
const unsigned char clipitArr4[832] = 
{
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
            2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
            3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
            4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
            5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
            6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
            7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
            8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
            9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9,
            10, 10, 10, 10, 10, 10, 10, 10,
            10, 10, 10, 10, 10, 10, 10, 10,
            11, 11, 11, 11, 11, 11, 11, 11,
            11, 11, 11, 11, 11, 11, 11, 11,
            12, 12, 12, 12, 12, 12, 12, 12,
            12, 12, 12, 12, 12, 12, 12, 12,
            13, 13, 13, 13, 13, 13, 13, 13,
            13, 13, 13, 13, 13, 13, 13, 13,
            14, 14, 14, 14, 14, 14, 14, 14,
            14, 14, 14, 14, 14, 14, 14, 14,
            15, 15, 15, 15, 15, 15, 15, 15,
            15, 15, 15, 15, 15, 15, 15, 15,
            15, 15, 15, 15, 15, 15, 15, 15,
            15, 15, 15, 15, 15, 15, 15, 15,
            15, 15, 15, 15, 15, 15, 15, 15,
            15, 15, 15, 15, 15, 15, 15, 15,
            15, 15, 15, 15, 15, 15, 15, 15,
            15, 15, 15, 15, 15, 15, 15, 15,
            15, 15, 15, 15, 15, 15, 15, 15,
            15, 15, 15, 15, 15, 15, 15, 15,
            15, 15, 15, 15, 15, 15, 15, 15,
            15, 15, 15, 15, 15, 15, 15, 15,
            15, 15, 15, 15, 15, 15, 15, 15,
            15, 15, 15, 15, 15, 15, 15, 15,
            15, 15, 15, 15, 15, 15, 15, 15,
            15, 15, 15, 15, 15, 15, 15, 15,
            15, 15, 15, 15, 15, 15, 15, 15,
            15, 15, 15, 15, 15, 15, 15, 15,
            15, 15, 15, 15, 15, 15, 15, 15,
            15, 15, 15, 15, 15, 15, 15, 15,
            15, 15, 15, 15, 15, 15, 15, 15,
            15, 15, 15, 15, 15, 15, 15, 15,
            15, 15, 15, 15, 15, 15, 15, 15,
            15, 15, 15, 15, 15, 15, 15, 15,
            15, 15, 15, 15, 15, 15, 15, 15,
            15, 15, 15, 15, 15, 15, 15, 15,
            15, 15, 15, 15, 15, 15, 15, 15,
            15, 15, 15, 15, 15, 15, 15, 15,
            15, 15, 15, 15, 15, 15, 15, 15,
            15, 15, 15, 15, 15, 15, 15, 15,
            15, 15, 15, 15, 15, 15, 15, 15,
            15, 15, 15, 15, 15, 15, 15, 15,
            15, 15, 15, 15, 15, 15, 15, 15,
            15, 15, 15, 15, 15, 15, 15, 15,
            15, 15, 15, 15, 15, 15, 15, 15,
            15, 15, 15, 15, 15, 15, 15, 15,
            15, 15, 15, 15, 15, 15, 15, 15,
            15, 15, 15, 15, 15, 15, 15, 15
};

/*==================================================================================================
        YCbCrToRGB16bpp()
        Optimized and re-organized function
==================================================================================================*/
/* Set up the macro to pack into the desired output RGB format */

/* (0000 rrrr gggg bbbb) */
#define AVCD_MakeRGB_444(out,r,g,b) out = (((r) << 8) | ((g) << 4) | (b))

void YCbCrToRGB_444(unsigned char *Y, int xsize,
                    unsigned char *Cb, unsigned char *Cr, int cxsize,
                    unsigned char *rgbPixelPtr, int width, int height,
                    int rgbOrientation)
{
        int     row, col;
        unsigned char *topLuminancePtr, *bottomLuminancePtr, *chromBluePtr,
                *chromRedPtr;
        unsigned short *topOutputPtr, *bottomOutputPtr;
        int     linePitch, next;
        int     r, b, yy;
        const unsigned char *clipR, *clipG, *clipB;
        const unsigned char *clipitPtrR = clipitArr4 + 288;
        const unsigned char *clipitPtrG = clipitArr4 + 288;
        const unsigned char *clipitPtrB = clipitArr4 + 288;
        
        linePitch = width * 2;
        topLuminancePtr = Y;
        chromBluePtr = Cb;
        chromRedPtr = Cr;
        
        if (rgbOrientation == RGB_ORIENT_NORMAL)
        {
                // Normal Top-to-Bottom raster
                next = 1;
                topOutputPtr = (unsigned short *) rgbPixelPtr;
                bottomOutputPtr = topOutputPtr + width;
        }
        else
        {
                // Bottom-to-Top raster : compatible with Windows bit-plane
                next = -1;
                topOutputPtr =
                        (unsigned short *) (rgbPixelPtr + linePitch * (height - 1));
                bottomOutputPtr = topOutputPtr - width;
        }
        
        for (row = height - 1; row > 0; row -= 2)
        {
                bottomLuminancePtr = topLuminancePtr + xsize;
                col = width / 2;
                while (col)
                {
                        r = *chromRedPtr++;
                        b = *chromBluePtr++;
                        clipR = clipitPtrR + r_cr[r];
                        clipB = clipitPtrB + b_cb[b];
                        clipG = clipitPtrG + g_cr[r] + g_cb[b];
                        
                        yy = topLuminancePtr[0];
                        AVCD_MakeRGB_444(topOutputPtr[0], clipR[yy], clipG[yy], clipB[yy]);
                        
                        yy = topLuminancePtr[1];
                        AVCD_MakeRGB_444(topOutputPtr[1], clipR[yy], clipG[yy], clipB[yy]);
                        
                        yy = bottomLuminancePtr[0];
                        AVCD_MakeRGB_444(bottomOutputPtr[0], clipR[yy], clipG[yy],
                                clipB[yy]);
                        
                        yy = bottomLuminancePtr[1];
                        AVCD_MakeRGB_444(bottomOutputPtr[1], clipR[yy], clipG[yy],
                                clipB[yy]);
                        
                        topLuminancePtr += 2;
                        bottomLuminancePtr += 2;
                        topOutputPtr += 2;
                        bottomOutputPtr += 2;
                        col--;
                }
                chromBluePtr += (cxsize - (width / 2));
                chromRedPtr += (cxsize - (width / 2));
                topLuminancePtr = bottomLuminancePtr + (xsize - width);
                topOutputPtr += (next * (2 * width) - width);
                bottomOutputPtr += (next * (2 * width) - width);
        }
}
#endif

#if (SUPPORT_RGB_565)
/*==================================================================================================
        Use this clipping array when 5-bit output is required.
        These values are calculated using clipitArr5[i] = clipitArr[i] >> 3
==================================================================================================*/
const unsigned char clipitArr5[832] = 
{
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1,
            2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3,
            4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 5, 5, 5, 5,
            6, 6, 6, 6, 6, 6, 6, 6, 7, 7, 7, 7, 7, 7, 7, 7,
            8, 8, 8, 8, 8, 8, 8, 8, 9, 9, 9, 9, 9, 9, 9, 9,
            10, 10, 10, 10, 10, 10, 10, 10, 11, 11, 11, 11, 11, 11, 11, 11,
            12, 12, 12, 12, 12, 12, 12, 12,
            13, 13, 13, 13, 13, 13, 13, 13,
            14, 14, 14, 14, 14, 14, 14, 14,
            15, 15, 15, 15, 15, 15, 15, 15,
            16, 16, 16, 16, 16, 16, 16, 16,
            17, 17, 17, 17, 17, 17, 17, 17,
            18, 18, 18, 18, 18, 18, 18, 18,
            19, 19, 19, 19, 19, 19, 19, 19,
            20, 20, 20, 20, 20, 20, 20, 20,
            21, 21, 21, 21, 21, 21, 21, 21,
            22, 22, 22, 22, 22, 22, 22, 22,
            23, 23, 23, 23, 23, 23, 23, 23,
            24, 24, 24, 24, 24, 24, 24, 24,
            25, 25, 25, 25, 25, 25, 25, 25,
            26, 26, 26, 26, 26, 26, 26, 26,
            27, 27, 27, 27, 27, 27, 27, 27,
            28, 28, 28, 28, 28, 28, 28, 28,
            29, 29, 29, 29, 29, 29, 29, 29,
            30, 30, 30, 30, 30, 30, 30, 30,
            31, 31, 31, 31, 31, 31, 31, 31,
            31, 31, 31, 31, 31, 31, 31, 31,
            31, 31, 31, 31, 31, 31, 31, 31,
            31, 31, 31, 31, 31, 31, 31, 31,
            31, 31, 31, 31, 31, 31, 31, 31,
            31, 31, 31, 31, 31, 31, 31, 31,
            31, 31, 31, 31, 31, 31, 31, 31,
            31, 31, 31, 31, 31, 31, 31, 31,
            31, 31, 31, 31, 31, 31, 31, 31,
            31, 31, 31, 31, 31, 31, 31, 31,
            31, 31, 31, 31, 31, 31, 31, 31,
            31, 31, 31, 31, 31, 31, 31, 31,
            31, 31, 31, 31, 31, 31, 31, 31,
            31, 31, 31, 31, 31, 31, 31, 31,
            31, 31, 31, 31, 31, 31, 31, 31,
            31, 31, 31, 31, 31, 31, 31, 31,
            31, 31, 31, 31, 31, 31, 31, 31,
            31, 31, 31, 31, 31, 31, 31, 31,
            31, 31, 31, 31, 31, 31, 31, 31,
            31, 31, 31, 31, 31, 31, 31, 31,
            31, 31, 31, 31, 31, 31, 31, 31,
            31, 31, 31, 31, 31, 31, 31, 31,
            31, 31, 31, 31, 31, 31, 31, 31,
            31, 31, 31, 31, 31, 31, 31, 31,
            31, 31, 31, 31, 31, 31, 31, 31,
            31, 31, 31, 31, 31, 31, 31, 31,
            31, 31, 31, 31, 31, 31, 31, 31,
            31, 31, 31, 31, 31, 31, 31, 31,
            31, 31, 31, 31, 31, 31, 31, 31,
            31, 31, 31, 31, 31, 31, 31, 31,
            31, 31, 31, 31, 31, 31, 31, 31,
            31, 31, 31, 31, 31, 31, 31, 31,
            31, 31, 31, 31, 31, 31, 31, 31,
            31, 31, 31, 31, 31, 31, 31, 31,
            31, 31, 31, 31, 31, 31, 31, 31,
            31, 31, 31, 31, 31, 31, 31, 31,
            31, 31, 31, 31, 31, 31, 31, 31
};

/*==================================================================================================
        Use this clipping array when 6-bit output is required.
        These values are calculated using clipitArr6[i] = clipitArr[i] >> 2
==================================================================================================*/
const unsigned char clipitArr6[832] = 
{
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3,
            4, 4, 4, 4, 5, 5, 5, 5, 6, 6, 6, 6, 7, 7, 7, 7,
            8, 8, 8, 8, 9, 9, 9, 9, 10, 10, 10, 10, 11, 11, 11, 11,
            12, 12, 12, 12, 13, 13, 13, 13, 14, 14, 14, 14, 15, 15, 15, 15,
            16, 16, 16, 16, 17, 17, 17, 17, 18, 18, 18, 18, 19, 19, 19, 19,
            20, 20, 20, 20, 21, 21, 21, 21, 22, 22, 22, 22, 23, 23, 23, 23,
            24, 24, 24, 24, 25, 25, 25, 25,
            26, 26, 26, 26, 27, 27, 27, 27,
            28, 28, 28, 28, 29, 29, 29, 29,
            30, 30, 30, 30, 31, 31, 31, 31,
            32, 32, 32, 32, 33, 33, 33, 33,
            34, 34, 34, 34, 35, 35, 35, 35,
            36, 36, 36, 36, 37, 37, 37, 37,
            38, 38, 38, 38, 39, 39, 39, 39,
            40, 40, 40, 40, 41, 41, 41, 41,
            42, 42, 42, 42, 43, 43, 43, 43,
            44, 44, 44, 44, 45, 45, 45, 45,
            46, 46, 46, 46, 47, 47, 47, 47,
            48, 48, 48, 48, 49, 49, 49, 49,
            50, 50, 50, 50, 51, 51, 51, 51,
            52, 52, 52, 52, 53, 53, 53, 53,
            54, 54, 54, 54, 55, 55, 55, 55,
            56, 56, 56, 56, 57, 57, 57, 57,
            58, 58, 58, 58, 59, 59, 59, 59,
            60, 60, 60, 60, 61, 61, 61, 61,
            62, 62, 62, 62, 63, 63, 63, 63,
            63, 63, 63, 63, 63, 63, 63, 63,
            63, 63, 63, 63, 63, 63, 63, 63,
            63, 63, 63, 63, 63, 63, 63, 63,
            63, 63, 63, 63, 63, 63, 63, 63,
            63, 63, 63, 63, 63, 63, 63, 63,
            63, 63, 63, 63, 63, 63, 63, 63,
            63, 63, 63, 63, 63, 63, 63, 63,
            63, 63, 63, 63, 63, 63, 63, 63,
            63, 63, 63, 63, 63, 63, 63, 63,
            63, 63, 63, 63, 63, 63, 63, 63,
            63, 63, 63, 63, 63, 63, 63, 63,
            63, 63, 63, 63, 63, 63, 63, 63,
            63, 63, 63, 63, 63, 63, 63, 63,
            63, 63, 63, 63, 63, 63, 63, 63,
            63, 63, 63, 63, 63, 63, 63, 63,
            63, 63, 63, 63, 63, 63, 63, 63,
            63, 63, 63, 63, 63, 63, 63, 63,
            63, 63, 63, 63, 63, 63, 63, 63,
            63, 63, 63, 63, 63, 63, 63, 63,
            63, 63, 63, 63, 63, 63, 63, 63,
            63, 63, 63, 63, 63, 63, 63, 63,
            63, 63, 63, 63, 63, 63, 63, 63,
            63, 63, 63, 63, 63, 63, 63, 63,
            63, 63, 63, 63, 63, 63, 63, 63,
            63, 63, 63, 63, 63, 63, 63, 63,
            63, 63, 63, 63, 63, 63, 63, 63,
            63, 63, 63, 63, 63, 63, 63, 63,
            63, 63, 63, 63, 63, 63, 63, 63,
            63, 63, 63, 63, 63, 63, 63, 63,
            63, 63, 63, 63, 63, 63, 63, 63,
            63, 63, 63, 63, 63, 63, 63, 63,
            63, 63, 63, 63, 63, 63, 63, 63,
            63, 63, 63, 63, 63, 63, 63, 63,
            63, 63, 63, 63, 63, 63, 63, 63,
            63, 63, 63, 63, 63, 63, 63, 63,
            63, 63, 63, 63, 63, 63, 63, 63
};

/*==================================================================================================
        YCbCrToRGB16bpp()
        Optimized and re-organized function
==================================================================================================*/
/* Set up the macro to pack into the desired output RGB format */

/* (rrrr rggg gggb bbbb) */
#    define AVCD_MakeRGB_565(out,r,g,b) out = (((r) << 11) | ((g) << 5) | (b))

void YCbCrToRGB_565(unsigned char *Y, int xsize,
                    unsigned char *Cb, unsigned char *Cr, int cxsize,
                    unsigned char *rgbPixelPtr, int width, int height,
                    int rgbOrientation)
{
        int     row, col;
        unsigned char *topLuminancePtr, *bottomLuminancePtr, *chromBluePtr,
                *chromRedPtr;
        unsigned short *topOutputPtr, *bottomOutputPtr;
        int     linePitch, next;
        int     r, b, yy;
        const unsigned char *clipR, *clipG, *clipB;
        const unsigned char *clipitPtrR = clipitArr5 + 288;
        const unsigned char *clipitPtrG = clipitArr6 + 288;
        const unsigned char *clipitPtrB = clipitArr5 + 288;
        
        linePitch = width * 2;
        topLuminancePtr = Y;
        chromBluePtr = Cb;
        chromRedPtr = Cr;
        
        if (rgbOrientation == RGB_ORIENT_NORMAL)
        {
                // Normal Top-to-Bottom raster
                next = 1;
                topOutputPtr = (unsigned short *) rgbPixelPtr;
                bottomOutputPtr = topOutputPtr + width;
        }
        else
        {
                // Bottom-to-Top raster : compatible with Windows bit-plane
                next = -1;
                topOutputPtr =
                        (unsigned short *) (rgbPixelPtr + linePitch * (height - 1));
                bottomOutputPtr = topOutputPtr - width;
        }
        
        for (row = height - 1; row > 0; row -= 2)
        {
                bottomLuminancePtr = topLuminancePtr + xsize;
                col = width / 2;
                while (col)
                {
                        r = *chromRedPtr++;
                        b = *chromBluePtr++;
                        clipR = clipitPtrR + r_cr[r];
                        clipB = clipitPtrB + b_cb[b];
                        clipG = clipitPtrG + g_cr[r] + g_cb[b];
                        
                        yy = topLuminancePtr[0];
                        AVCD_MakeRGB_565(topOutputPtr[0], clipR[yy], clipG[yy], clipB[yy]);
                        
                        yy = topLuminancePtr[1];
                        AVCD_MakeRGB_565(topOutputPtr[1], clipR[yy], clipG[yy], clipB[yy]);
                        
                        yy = bottomLuminancePtr[0];
                        AVCD_MakeRGB_565(bottomOutputPtr[0], clipR[yy], clipG[yy],
                                clipB[yy]);
                        
                        yy = bottomLuminancePtr[1];
                        AVCD_MakeRGB_565(bottomOutputPtr[1], clipR[yy], clipG[yy],
                                clipB[yy]);
                        
                        topLuminancePtr += 2;
                        bottomLuminancePtr += 2;
                        topOutputPtr += 2;
                        bottomOutputPtr += 2;
                        col--;
                }
                chromBluePtr += (cxsize - (width / 2));
                chromRedPtr += (cxsize - (width / 2));
                topLuminancePtr = bottomLuminancePtr + (xsize - width);
                topOutputPtr += (next * (2 * width) - width);
                bottomOutputPtr += (next * (2 * width) - width);
        }
}
#endif

#if (SUPPORT_RGB_888)
static const unsigned char clipitArr[832] = 
{
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
            16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31,
            32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47,
            48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63,
            64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79,
            80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95,
            96, 97, 98, 99, 100, 101, 102, 103,
            104, 105, 106, 107, 108, 109, 110, 111,
            112, 113, 114, 115, 116, 117, 118, 119,
            120, 121, 122, 123, 124, 125, 126, 127,
            128, 129, 130, 131, 132, 133, 134, 135,
            136, 137, 138, 139, 140, 141, 142, 143,
            144, 145, 146, 147, 148, 149, 150, 151,
            152, 153, 154, 155, 156, 157, 158, 159,
            160, 161, 162, 163, 164, 165, 166, 167,
            168, 169, 170, 171, 172, 173, 174, 175,
            176, 177, 178, 179, 180, 181, 182, 183,
            184, 185, 186, 187, 188, 189, 190, 191,
            192, 193, 194, 195, 196, 197, 198, 199,
            200, 201, 202, 203, 204, 205, 206, 207,
            208, 209, 210, 211, 212, 213, 214, 215,
            216, 217, 218, 219, 220, 221, 222, 223,
            224, 225, 226, 227, 228, 229, 230, 231,
            232, 233, 234, 235, 236, 237, 238, 239,
            240, 241, 242, 243, 244, 245, 246, 247,
            248, 249, 250, 251, 252, 253, 254, 255,
            255, 255, 255, 255, 255, 255, 255, 255,
            255, 255, 255, 255, 255, 255, 255, 255,
            255, 255, 255, 255, 255, 255, 255, 255,
            255, 255, 255, 255, 255, 255, 255, 255,
            255, 255, 255, 255, 255, 255, 255, 255,
            255, 255, 255, 255, 255, 255, 255, 255,
            255, 255, 255, 255, 255, 255, 255, 255,
            255, 255, 255, 255, 255, 255, 255, 255,
            255, 255, 255, 255, 255, 255, 255, 255,
            255, 255, 255, 255, 255, 255, 255, 255,
            255, 255, 255, 255, 255, 255, 255, 255,
            255, 255, 255, 255, 255, 255, 255, 255,
            255, 255, 255, 255, 255, 255, 255, 255,
            255, 255, 255, 255, 255, 255, 255, 255,
            255, 255, 255, 255, 255, 255, 255, 255,
            255, 255, 255, 255, 255, 255, 255, 255,
            255, 255, 255, 255, 255, 255, 255, 255,
            255, 255, 255, 255, 255, 255, 255, 255,
            255, 255, 255, 255, 255, 255, 255, 255,
            255, 255, 255, 255, 255, 255, 255, 255,
            255, 255, 255, 255, 255, 255, 255, 255,
            255, 255, 255, 255, 255, 255, 255, 255,
            255, 255, 255, 255, 255, 255, 255, 255,
            255, 255, 255, 255, 255, 255, 255, 255,
            255, 255, 255, 255, 255, 255, 255, 255,
            255, 255, 255, 255, 255, 255, 255, 255,
            255, 255, 255, 255, 255, 255, 255, 255,
            255, 255, 255, 255, 255, 255, 255, 255,
            255, 255, 255, 255, 255, 255, 255, 255,
            255, 255, 255, 255, 255, 255, 255, 255,
            255, 255, 255, 255, 255, 255, 255, 255,
            255, 255, 255, 255, 255, 255, 255, 255,
            255, 255, 255, 255, 255, 255, 255, 255,
            255, 255, 255, 255, 255, 255, 255, 255,
            255, 255, 255, 255, 255, 255, 255, 255,
            255, 255, 255, 255, 255, 255, 255, 255
};

void YCbCrToRGB_888(unsigned char *Y, int xsize,
                    unsigned char *Cb, unsigned char *Cr, int cxsize,
                    unsigned char *rgbPixelPtr, int width, int height,
                    int rgbOrientation)
{
        int     row, col;
        unsigned char *topLuminancePtr, *bottomLuminancePtr, *chromBluePtr,
                *chromRedPtr;
        unsigned char *topOutputPtr, *bottomOutputPtr;
        int     linePitch, next;
        int     r, b, yy;
        const unsigned char *clipR, *clipG, *clipB;
        const unsigned char *clipitPtr = clipitArr + 288;

        linePitch = width * 3;
        topLuminancePtr = Y;
        chromBluePtr = Cb;
        chromRedPtr = Cr;
        
        if (rgbOrientation == RGB_ORIENT_NORMAL)
        {
                // Normal Top-to-Bottom raster
                next = 1;
                topOutputPtr = rgbPixelPtr;
                bottomOutputPtr = topOutputPtr + linePitch;
        }
        else
        {
                // Bottom-to-Top raster : compatible with Windows bit-plane
                next = -1;
                topOutputPtr = rgbPixelPtr + linePitch * (height - 1);
                bottomOutputPtr = topOutputPtr - linePitch;
        }
        
        for (row = height - 1; row > 0; row -= 2)
        {
                bottomLuminancePtr = topLuminancePtr + xsize;
                col = width / 2;
                while (col)
                {
                        r = *chromRedPtr++;
                        b = *chromBluePtr++;
                        clipR = clipitPtr + r_cr[r];
                        clipB = clipitPtr + b_cb[b];
                        clipG = clipitPtr + g_cr[r] + g_cb[b];
                        
                        yy = topLuminancePtr[0];
                        topOutputPtr[0] = clipB[yy];        //blue
                        topOutputPtr[1] = clipG[yy];        // green
                        topOutputPtr[2] = clipR[yy];        // red
                        
                        yy = topLuminancePtr[1];
                        topOutputPtr[3] = clipB[yy];        //blue
                        topOutputPtr[4] = clipG[yy];        // green
                        topOutputPtr[5] = clipR[yy];        // red
                        
                        yy = bottomLuminancePtr[0];
                        bottomOutputPtr[0] = clipB[yy];     // blue
                        bottomOutputPtr[1] = clipG[yy];     //green
                        bottomOutputPtr[2] = clipR[yy];     // red
                        
                        yy = bottomLuminancePtr[1];
                        bottomOutputPtr[3] = clipB[yy];     // blue
                        bottomOutputPtr[4] = clipG[yy];     //green
                        bottomOutputPtr[5] = clipR[yy];     // red
                        
                        topLuminancePtr += 2;
                        bottomLuminancePtr += 2;
                        topOutputPtr += 6;
                        bottomOutputPtr += 6;
                        col--;
                }
                chromBluePtr += (cxsize - (width / 2));
                chromRedPtr += (cxsize - (width / 2));
                topLuminancePtr = bottomLuminancePtr + (xsize - width);
                topOutputPtr += (next * (2 * linePitch) - 3 * width);
                bottomOutputPtr += (next * (2 * linePitch) - 3 * width);
        }
}

#endif
#endif                                  // #ifdef RGB_LOOKUP

int YCbCrToRGB(unsigned char *Y, int xsize,
               unsigned char *Cb, unsigned char *Cr, int cxsize,
               unsigned char *rgbPixelPtr, int width, int height,
               int rgbOrientation, int colorFormat)
{
        switch (colorFormat)
        {
#if (SUPPORT_RGB_888)
        case RGB_888:
                YCbCrToRGB_888(Y, xsize, Cb, Cr, cxsize, rgbPixelPtr,
                        width, height, rgbOrientation);
                return 0;
#endif
#if (SUPPORT_RGB_565)
        case RGB_565:
                YCbCrToRGB_565(Y, xsize, Cb, Cr, cxsize, rgbPixelPtr,
                        width, height, rgbOrientation);
                return 0;
#endif
#if (SUPPORT_RGB_444)
        case RGB_444:
                YCbCrToRGB_444(Y, xsize, Cb, Cr, cxsize, rgbPixelPtr,
                        width, height, rgbOrientation);
                return 0;
#endif
        default:
                return 1;
        }
}

int YCbYCrToRGB (unsigned char * YCbYCr, int stride,  unsigned char *rgbPixelPtr, 
                 int width, int height, int rgbOrientation, int colorFormat)
{
        char *YPtr, *CbPtr, *CrPtr;
        int i, j, err;
        if (width  % 2 != 0) width  -= 1;
        if (height % 2 != 0) height -= 1;
        YPtr  = (unsigned char *) malloc ( 3 * width * height / 2 + 1);
        if (YPtr == NULL) return 2;
        CbPtr = YPtr + width * height;
        CrPtr = CbPtr + width * height / 4;
        for (j = 0; j < height; j++)
            for (i = 0; i < width; i++)
            {
                    *(YPtr + j * width + i) = *(YCbYCr + j * stride + 2*i);
                    if (j % 2 == 0 && i % 2 == 0) 
                    {
                            * (CbPtr + (width / 2)* j / 2 + i / 2) = * (YCbYCr + stride * j + 4 * (i / 2) + 1);
                            * (CrPtr + (width / 2)* j / 2 + i / 2) = * (YCbYCr + stride * j + 4 * (i / 2) + 3);
                    }
            }
            
        err = YCbCrToRGB(YPtr, width, 
                   CbPtr, CrPtr, width / 2, 
                   rgbPixelPtr, width, height, 
                   rgbOrientation, colorFormat);
        
        free (YPtr);
        return err;
}