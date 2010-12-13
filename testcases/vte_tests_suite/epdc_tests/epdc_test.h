/*
 * Copyright (C) 2010 Freescale Semiconductor, Inc. All Rights Reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef EPDC_TEST_H
#define EPDC_TEST_H

#ifdef __cplusplus
extern "C"{
#endif

/*INCLUDE FILES */
#include <linux/fb.h>   /* framebuffer related information */
#include <linux/mxcfb.h>
/* CONSTANTS */

/* DEFINES AND MACROS */
#define PATH_LEN  128

#define USE_PIC

#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif

typedef unsigned char BOOL;
/* ENUMS */

/* STRUCTURES AND OTHER TYPEDEFS */
/* This structure contains information taken from fb_var_screeninfo struct */
struct px_field
{
    int offset;                 /* Bitfield offset */
    int length;                 /* Bitfield length */
};

/* This structure is used when particular pixel is being written into fb memory */
struct pixel
{
    int bpp;                    /* Color depth in bytes            */
    int xres;                   /* X resolution in pixels          */
    int yres;                   /* Y resolution in pixels          */
    unsigned char   r_color;    /* Red color value to be written   */
    struct px_field r_field;
    unsigned char   g_color;    /* Green color value to be written */
    struct px_field g_field;
    unsigned char   b_color;    /* Blue color value to be written  */
    struct px_field b_field;
    unsigned char   trans;    /* Transparency value   */
    struct px_field t_field;
   int line_length;            /* length of a line in px  */
};

typedef struct epdc_opts {
   int Tid;
   char dev[128];
   struct mxcfb_waveform_modes waveform;
   int temp;
   int grayscale;
   int au;/*auto update or regional update*/
   int su;/*use alt update*/
	 int rot;/*rotation mode*/
   struct mxcfb_update_data update;
   int wt;
	 int delay;
	 int scheme;
 } epdc_opts;

/* GLOBAL VARIABLE DECLARATIONS */


/* FUNCTION PROTOTYPES */
int epdc_fb_setup();
int epdc_fb_cleanup();
int epdc_fb_test(void);

void cleanup();


#ifdef __cplusplus
}
#endif

#endif  /* EPDC_TEST_H */
