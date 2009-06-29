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
/*
 * Copyright 2009 Freescale Semiconductor, Inc. All Rights Reserved.
 *
 */
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h>

int conv_mx25_yuv(unsigned char * data_in,int iw, int ih, 
                  unsigned char * data_out)
{
   unsigned char * data_offset = NULL;
   int i=0, j = 0, k = 0,  m = 0;

#if 1  /* 0x1b */
   for (i = 0; i < ih ; i += 2) {
       data_offset = data_in + i * iw * 3 /2;
       for (j=0; j < iw * 2; j += 4) { 
          data_out[k] = data_offset[j+1];
          data_out[m + iw * ih] = data_offset[j + 0];
          data_out[k + 1] = data_offset[j + 3];
          data_out[m + iw * ih * 5 /4] = data_offset[j + 2];
          k += 2;
          m++;
	}
	j = 0;
     memcpy(data_out + k, data_in + i * iw * 3 / 2 + iw * 2, iw);
     k += iw;
   }	
#endif

#if 0
		for (i=0;i<480;i+=2) {
			data_offset = data_in + i * 640 * 3 /2 + 640;
			memcpy(data_out+k, data_in+ i * 640 * 3 / 2, 640);
			k +=640;
			for (j=0; j< 640 * 2; j+= 4) {
				data_out[k]=data_offset[j+1]; 
				data_out[m + 640 * 480] = data_offset[j+0];
				data_out[k +1] = data_offset[j+3];
				data_out[m + 640 * 480 * 5/4] = data_offset[j+2];
				k += 2;
				m ++;
			}
		}
#endif
//		fwrite(data_out, sizeof(unsigned char) * WIDTH * HEIGHT * 3 / 2, 1, out_file);

	return 0;
}
