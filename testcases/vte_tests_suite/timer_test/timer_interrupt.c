/*
 * Copyright (C) 2012 Freescale Semiconductor, Inc. All Rights Reserved.
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
 *	timer_resolution.c
 *
 * DESCRIPTION
 *	Check if gettimeofday has resolution less than 1ms
 *
 */

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>


#define USECREQ 2
#define LOOPS   1000

void event_handler (int signum)
{
 static unsigned long cnt = 0;
 static struct timeval tsFirst;
 if (cnt == 0) {
   gettimeofday (&tsFirst, 0);
 } 
 cnt ++;
 if (cnt >= LOOPS) {
   struct timeval tsNow;
   struct timeval diff;
   setitimer (ITIMER_REAL, NULL, NULL);
   gettimeofday (&tsNow, 0);
   timersub(&tsNow, &tsFirst, &diff);
   unsigned long long udiff = (diff.tv_sec * 1000000) + diff.tv_usec;
   double delta = (double)(udiff/cnt)/1000000;
   int hz = (unsigned)(1.0/delta);
   printf ("kernel timer interrupt frequency is approx. %d Hz", hz);
   if (hz >= (int) (1.0/((double)(USECREQ)/1000000))) {
     printf (" or higher");
   }       
   printf ("\n");
   exit (0);
 }
}

int main (int argc, char **argv)
{
 struct sigaction sa;
 struct itimerval timer;

 memset (&sa, 0, sizeof (sa));
 sa.sa_handler = &event_handler;
 sigaction (SIGALRM, &sa, NULL);
 timer.it_value.tv_sec = 0;
 timer.it_value.tv_usec = USECREQ;
 timer.it_interval.tv_sec = 0;
 timer.it_interval.tv_usec = USECREQ;
 setitimer (ITIMER_REAL, &timer, NULL);
 while (1);
}

