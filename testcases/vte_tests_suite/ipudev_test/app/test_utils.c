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
/*!
 * @file test_utils.c
 *
 * @brief IPU device lib test utils implementation
 *
 * @ingroup IPU
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#include "mxc_ipudev_test.h"


#define deb_msg 0
#if deb_msg
#define deb_printf printf
#else
#define deb_printf
#endif
/*
 * fourcc
 *  RGB565->RGBP
 *  BGR24 ->BGR3
 *  RGB24 ->RGB3
 *  BGR32 ->BGR4
 *  BGRA32->BGRA
 *  RGB32 ->RGB4
 *  RGBA32->RGBA
 *  ABGR32->ABGR
 *  YUYV  ->YUYV
 *  UYVY  ->UYVY
 *  YUV444->Y444
 *  NV12  ->NV12
 *  YUV420P->I420
 *  YUV422P->422P
 *  YVU422P->YV16
 * */
/*
 * m: mode, TASK_ENC = 0x1, TASK_VF = 0x2, TASK_PP = 0x4, 
 *          NORMAL_MODE = 0x10, STREAM_MODE = 0x20
 *    should combine or TASK and MODE
 * f: frame count
 * E: output1 enable
 * i: w,h,f input width,height,format
 * c: x,y,w,h crop x,y,width,height 
 * o: w,h,fi,r output0 width,height,format,rotation
 * s: tofb,fbid,posx,posy, enable o0 to fb,fb id,position x/y
 * n: output0 file name
 * O: w,h,f,r output1 width,height,format,rotation
 * S: tofb,fbid,posx,posy, enable o1 to fb, fb id,position x/y
 * N: output1 file name
 * */
char * options = "m:f:E:i:c:o:s:n:O:S:N:";

int parse_cmd_input(int argc, char ** argv, ipu_test_handle_t *test_handle)
{
 char opt;
 int status = 0;
 char fourcc[5];
  
  printf("pass cmdline ", argc, argv[0]);
 
 while((opt = getopt(argc, argv, options)) > 0)
 {
  deb_printf("new option : %c \n", opt);
  switch(opt)
  {
   case 'm':/*mode*/
     if(NULL == optarg)
       break;
      test_handle->mode = strtol(optarg, NULL, 16);
      deb_printf("mod set %d \n",test_handle->mode);
      break;
   case 'f': /*frame count*/
     if(NULL == optarg)
       break;
      test_handle->fcount = strtol(optarg, NULL, 10);
      deb_printf("frame count set %d \n",test_handle->fcount);
      break;
   case 'E':/*enable output1*/
     if(NULL == optarg)
       break;
      test_handle->output1_enabled = strtol(optarg, NULL, 10);
      deb_printf("enable output set %d \n",test_handle->output1_enabled);
      break;
   case 'i': /*input param*/
     if(NULL == optarg)
       break;
      memset(fourcc,0,sizeof(fourcc));
      sscanf(optarg,"%d,%d,%s", &(test_handle->input.width),
       &(test_handle->input.height), fourcc);
       test_handle->input.fmt = v4l2_fourcc(fourcc[0], fourcc[1], 
       fourcc[2], fourcc[3]);
      deb_printf("input set w=%d,h=%d,%s=%d \n",test_handle->input.width, test_handle->input.height,
      fourcc,test_handle->input.fmt);
      break;
   case 'c':/*corp setting*/
     if(NULL == optarg)
       break;
      sscanf(optarg,"%d,%d,%d,%d",&(test_handle->input.input_crop_win.pos.x),
        &(test_handle->input.input_crop_win.pos.y),
	&(test_handle->input.input_crop_win.win_w),
	&(test_handle->input.input_crop_win.win_h)
      );
      deb_printf("crop setting: x=%d,y=%d,w=%d,h=%d \n", test_handle->input.input_crop_win.pos.x,
      test_handle->input.input_crop_win.pos.y, test_handle->input.input_crop_win.win_w,
      test_handle->input.input_crop_win.win_h);
      break;
   case 'o':/*output0 setting*/
     if(NULL == optarg)
       break;
     sscanf(optarg,"%d,%d,%s,%d", 
       &(test_handle->output0.width),
       &( test_handle->output0.height),fourcc,
       &(test_handle->output0.rot));
     test_handle->output0.fmt = v4l2_fourcc(fourcc[0], 
       fourcc[1],fourcc[2], fourcc[3]);
      deb_printf("output0 setting: w=%d,h=%d,%s=%d,r=%d \n",test_handle->output0.width,
      test_handle->output0.height,fourcc,test_handle->output0.fmt,test_handle->output0.rot);
      break;
   case 's':/*output0 to fb setting*/
     if(NULL == optarg)
       break;
     sscanf(optarg,"%d,%d,%d,%d", 
       &(test_handle->output0.show_to_fb),
       &(test_handle->output0.fb_disp.fb_num),
       &(test_handle->output0.fb_disp.pos.x),
       &(test_handle->output0.fb_disp.pos.y)
       );
       deb_printf("output0 fb setting: enable=%d,fb=/dev/fb%d,x=%d,y=%d \n",
         test_handle->output0.show_to_fb,test_handle->output0.fb_disp.fb_num,
	 test_handle->output0.fb_disp.pos.x,test_handle->output0.fb_disp.pos.y
       );
      break;
   case 'n':/*output0 file name*/
     if(NULL == optarg)
       break;
     sscanf(optarg,"%s",test_handle->outfile0);
     deb_printf("output0 file name %s \n", test_handle->outfile0);
      break;
   case 'O':/*output1 setting*/
     if(NULL == optarg)
       break;
     sscanf(optarg,"%d,%d,%s,%d", 
       &(test_handle->output1.width),
       &( test_handle->output1.height),fourcc,
       &(test_handle->output1.rot));
     test_handle->output1.fmt = v4l2_fourcc(fourcc[0], 
       fourcc[1],fourcc[2], fourcc[3]);
      deb_printf("output1 setting: w=%d,h=%d,%s=%d,r=%d \n",test_handle->output1.width,
      test_handle->output1.height,fourcc,test_handle->output1.fmt,test_handle->output1.rot);
      break;
   case 'S':/*output1 to fb setting*/
     if(NULL == optarg)
       break;
     sscanf(optarg,"%d,%d,%d,%d", 
       &(test_handle->output1.show_to_fb),
       &(test_handle->output1.fb_disp.fb_num),
       &(test_handle->output1.fb_disp.pos.x),
       &(test_handle->output1.fb_disp.pos.y)
       );
       deb_printf("output1 fb setting: enable=%d,fb=/dev/fb%d,x=%d,y=%d \n",
         test_handle->output1.show_to_fb,test_handle->output1.fb_disp.fb_num,
	 test_handle->output1.fb_disp.pos.x,test_handle->output1.fb_disp.pos.y
       );
      break;
   case 'N':/*output1 to filename */
     if(NULL == optarg)
       break;
      sscanf(optarg,"%s ",test_handle->outfile1);
      deb_printf("output1 file name %s \n",test_handle->outfile1);
      break;
   default:
      return 0;
  }
 }
 return 0;
}


