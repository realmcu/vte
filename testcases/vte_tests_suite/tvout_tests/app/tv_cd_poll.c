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
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#define TV_CD_SYS_ATTR	"/sys/devices/platform/tve.0/headphone"

int main()
{
	int fd, num;
	fd_set rfds;
	char buf[20];

	fd = open(TV_CD_SYS_ATTR, O_RDWR, S_IRUSR | S_IWUSR);
	if (fd != - 1)
	{
		while (1)
		{
			FD_ZERO(&rfds);
			FD_SET(fd, &rfds);

			select(fd + 1, &rfds, NULL, NULL, NULL);

			if (FD_ISSET(fd, &rfds))
			{
				lseek(fd, 0, SEEK_SET);
				memset(buf, 0 , 20);
				num = read(fd, buf, 20);

				if (num != 0) {
					printf("\nThe string read len is %d\n", num);
					printf("The string read is %s\n", buf);
					if (strcmp(buf, "tve power off\n") == 0)
						printf("Confirm: tve power off!\n");
					if (strcmp(buf, "cvbs\n") == 0)
						printf("Confirm: cvbs cable connected!\n");
					if (strcmp(buf, "component\n") == 0)
						printf("Confirm: component cable connected!\n");
					if (strcmp(buf, "svideo\n") == 0)
					       printf("Confirm: svideo cable connected!\n");
					if (strcmp(buf, "headset\n") == 0)
						printf("Confirm: headset connected!\n");
					if (strcmp(buf, "none\n") == 0)
						printf("Confirm: none connect!\n");
				}
			}
		}
	}
	else
	{
		printf("sys file open failure\n");
		return -1;
	}

	close(fd);
	return 0;
}

