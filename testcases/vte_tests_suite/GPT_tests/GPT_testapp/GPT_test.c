/***
**Copyright (C) 2005-2009 Freescale Semiconductor, Inc. All Rights Reserved.
**
**The code contained herein is licensed under the GNU General Public
**License. You may obtain a copy of the GNU General Public License
**Version 2 or later at the following locations:
**
**http://www.opensource.org/licenses/gpl-license.html
**http://www.gnu.org/copyleft/gpl.html
**/

/*
Revision History:
                 Modification     Tracking
Author               Date          Number    Description of Changes
-------------   ------------    ----------  -------------------------------
Andy Tian          20120305        N/A       adjust code to test GPT 
                                             power managemant
###########################################################################
 */

#include <stdio.h> 
#include<unistd.h>
#include<signal.h>
#include <sys/time.h>


void handler() 
{
	printf("15 seconds pass\n");
}

void Timer(int sec, long usec)
{
    struct timeval tvSelect;
    
    tvSelect.tv_sec = sec;
    tvSelect.tv_usec = usec;
    select(FD_SETSIZE, NULL, NULL, NULL, &tvSelect);
}
int main(int argc, char **argv)
{
    int rv=0;
	printf("--- begin ---\n");
	Timer(5, 1000*500);
	printf("--- bye ---\n");
	printf("begin to count time!\n");
	signal(SIGALRM,handler);
	rv=alarm(15);
	if(rv!=0)
	{
		printf("Test case doesn't work well!");
	}
	else
	{
		printf("Try to sleep %d Seconds...\n",20);
		rv=sleep(20);
		if(rv==5)
		{
			printf("Test case work well!\n");
		}
		else
		{
			printf("Test case doesn't work well!\n");
		}

	}
	return (rv==5)?0:1;
}



