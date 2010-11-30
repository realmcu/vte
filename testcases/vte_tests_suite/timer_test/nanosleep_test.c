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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/
/*
 * NAME
 * nanosleep.c	
 *
 * DESCRIPTION
 *	Check if nanosleep accuracy.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <test.h>

#define MAX_GAP 1000

char *TCID = "nanosleep test";	/* Test program identifier.    */
int TST_TOTAL = 1;		/* Total number of test cases. */

int main (int argc, char **argv)
{
  unsigned int nTimeTest = 0;        /* usec */
  struct timeval tvBegin;
  struct timeval tvNow;
  int ret = 0;
  unsigned int nDelay = 0;        /* usec */
  int i = 0;
  struct timespec req;
  unsigned int delay[20] = { 500000, 100000, 50000, 10000, 1000, 900, 500, 100, 10, 1, 0 };
  int nReduce = 0;
  for (i = 0; i < 20; i++)
    {
      if (delay[i] <= 0)
        break;
      nDelay = delay[i];
      gettimeofday (&tvBegin, NULL);
      req.tv_sec = nDelay / 1000000;
      req.tv_nsec = (nDelay % 1000000) * 1000;
      ret = nanosleep (&req, NULL);
      if (-1 == ret)
      {
      	tst_resm(TFAIL,"loop nanosleep() failed: %s\n",strerror(errno));
        return 1;
			}
      else
        {
          gettimeofday (&tvNow, NULL);
          nTimeTest =
            (tvNow.tv_sec - tvBegin.tv_sec) * 1000000 + tvNow.tv_usec -
            tvBegin.tv_usec;
          nReduce = nTimeTest - nDelay;
          if(nReduce > MAX_GAP){
          tst_resm(TFAIL,"\t nanosleep    %8u   %8u   %8d\n", nDelay,
                   nTimeTest, nReduce);
          return 1;
          }
        }
    }
		tst_resm(TPASS, "nanosleep test PASS!");
   return 0;
}
