/*
 * Copyright 2004-2011 Freescale Semiconductor, Inc. All rights reserved.
 */
 
/*
 * The code contained herein is licensed under the GNU General Public
 * License. You may obtain a copy of the GNU General Public License
 * Version 2 or later at the following locations:
 *
 * http://www.opensource.org/licenses/gpl-license.html
 * http://www.gnu.org/copyleft/gpl.html
 */

#include <stdio.h>
#include <stdint.h>
#include <linux/time.h>

uint32_t spread4(uint32_t v) __attribute__((noinline));

uint32_t spread4(uint32_t v)
{
   __asm__("and %0, %0, #255\n\t"
       "orr %0, %0, %0, lsl #8\n\t"
       "orr %0, %0, %0, lsl #16\n\t"
       :
			 : "r" (v)
       );
   return v;
}

int main()
{
   struct timespec start, end;
   clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);
   
   for (int i = 0; i < 10000; i++)
     {
	for (int j = 0; j < 10000; j++)
	  {
	     spread4(1);
	     spread4(22);
	     spread4(133);
	     spread4(221);
	  }
     }

   clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end);
   double elapsed = end.tv_sec - start.tv_sec + (end.tv_nsec - start.tv_nsec)*1e-9;
   printf("%.6f \n", elapsed);
   return 0;
}
