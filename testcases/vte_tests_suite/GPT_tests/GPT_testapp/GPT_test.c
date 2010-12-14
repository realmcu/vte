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
#include <stdio.h> 
#include<unistd.h>
#include<signal.h>
#include <sys/time.h>

void handler() 
{
	printf("5 seconds pass\n");
}

void Timer(int sec, long usec)
{
    struct timeval tvSelect;
    
    tvSelect.tv_sec = sec;
    tvSelect.tv_usec = usec;
    select(FD_SETSIZE, NULL, NULL, NULL, &tvSelect);
}
int main()
{
	int i;
    int rv=0;
	printf("--- begin ---\n");
	Timer(5, 1000*500);
	printf("--- bye ---\n");
	
	printf("begin to count time!\n");
	signal(SIGALRM,handler);
	rv=alarm(15);
	if(rv!=0)
	{
		printf("test case doesn't work well!");
	}
	else
	{
	for(i=1;i<20;i++){
		printf("sleep %d ...\n",i);
		rv=sleep(1);
		if(rv!=0)
			{
			printf("test case doesn't work well!");
			break;
		}
	}
	}
	if(rv==0)
	    printf("test case works well!\n");
	return rv;
}



