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
#include <string.h>
#include <stdlib.h>
#include <linux/fs.h>
#include <fcntl.h>

#include "usb-serial.h"

int main()
{
    int fd, ret;
    int size=1024;
    char readbuffer[1024];
    char *writebuffer={"abcdefghijklmk\0"};
    int nread=0;

    memset(readbuffer, 0, size);
    printf("write buffer size = %d\n", strlen(writebuffer));

    fd=open("/dev/ttygs", O_RDWR);
    if (fd < 0)
    {
        perror("Open /dev/ttygs");
        exit(1);
    }
	
#if 1    
    ret = write(fd, writebuffer, strlen(writebuffer));
    if (ret < 0)
    {
        perror("write ttygs");
        exit(1);
    }
    printf("write size = %d\n", ret);   
#endif
/*
	   ret = read(fd, readbuffer, size);

		printf("read size=%d\n",size);
		
	    if (ret < 0)
	        perror("read ttygs");
	    else    
	        printf("%s: receive from usb-serial:%s", __FILE__, readbuffer);
	 */
    while(1)
    	{
		while((nread=read(fd,readbuffer,size))>0)
		{
			//printf("\nLen:%d\n",nread);
			readbuffer[nread+1]='\0';
			printf("%s: receive from usb-serial:%s", __FILE__, readbuffer);	
		}

	}

    close(fd);
}
