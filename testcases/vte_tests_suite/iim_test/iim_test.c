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
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>

#define IIM_DEVICE "/dev/mxs_viim"
#ifndef NULL
#define NULL 0
#endif
int main()
{
 int fd;
 int ret = 0;
 char * piim = NULL;
 fd = open(IIM_DEVICE,O_RDONLY);
 if(fd < 0){
   perror("open");
   return 1;
 }
  
  piim = mmap(NULL, 4 * 1024, PROT_READ, MAP_SHARED ,fd, 0);
  if(piim == (void *)-1 )
   perror("iim OCR");

  close(fd);

  ret = munmap(piim, 4 * 1024);
  if(ret == -1)
    perror("iim OCR unmap");
  
   printf("TST_INFO: iim test PASS\n");
  return  0;
}
