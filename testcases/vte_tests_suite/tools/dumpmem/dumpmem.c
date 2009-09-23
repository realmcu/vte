/**
 * Copyright 2005-2009 Freescale Semiconductor, Inc. All Rights Reserved.
 *
 * The code contained herein is licensed under the GNU General Public
 * License. You may obtain a copy of the GNU General Public License
 * Version 2 or later at the following locations:
 *
 * http://www.opensource.org/licenses/gpl-license.html
 * http://www.gnu.org/copyleft/gpl.html
 * **/

#include <stdio.h>
#include <fcntl.h>

#include <unistd.h>
#include <sys/mman.h>

void help()
{
 printf("usage: \n");
 printf("dumpmem <start position> <mem length>\n");
 printf("start position default is 0xfff00000\n");
 printf("length default is 0x100000 = 1Mi\n");
 printf("\n");
}

int main(int argc, char *argv[])
{
  int i;
  volatile unsigned char *cp;
  int fd;
  volatile void *v;
  off_t nvram = 0xfff00000;
  /* avoid linux mmap bug */
  size_t length = 0x100000 /*- 0x1000*/;
  help();
  if (argc > 1)
    nvram = (strtol(argv[1], 0, 0)) << 16;
  if (argc > 2)
    length = (strtol(argv[2], 0, 0)) ;
    if((fd = open("/dev/mem",O_RDWR)) != -1)
    {
      v = mmap(0, length, PROT_READ | PROT_WRITE, MAP_SHARED,fd,nvram);
      fprintf(stderr, "mmap returns %p\n", v);

      if ( (int)v == -1)
      {
        perror("mmap");
        _exit(1);
      }
    } else {
      perror("open /dev/mem");
      _exit(1);
    }
  write(1, (void *)v, length);
  return 0;
}

