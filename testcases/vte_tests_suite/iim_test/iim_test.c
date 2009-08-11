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

  printf("clock 0x%x\n",*(int*)(piim+0xC0));
  printf("hclock 0x%x\n",*(int*)(piim+0x20));
  printf("chip:%s\n",piim+0x1300);
  printf("fuse:0x%x\n",*(int *)(piim+0x11A0));
  printf("test address equal 8k:0x%x\n",*(int *)(piim+0x2000));
  printf("test address exceed 8k:0x%x\n",*(int *)(piim+0x21a0));


  ret = munmap(piim, 4 * 1024);
  if(ret == -1)
    perror("iim OCR unmap");
  
   printf("TST_INFO: iim test PASS\n");
  return  0;
}
