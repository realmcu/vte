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
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <signal.h>
#include <unistd.h>

#define IIM_DEVICE "/dev/mxs_viim"
#ifndef NULL
#define NULL 0
#endif
typedef struct{
char name[32];	
unsigned long offset;
} t_regs;

#ifdef MX5
#define MAX_REGS 3
/*mx5x mmap GPT and OCOTP*/
t_regs mreg[] = {{"GPTOCR1",0x0010},
						{"boundry",0x1fff},
						{"exceed",0x21a0}
	};
#endif

#ifdef MX233
#define MAX_REGS 6
t_regs mreg[] = {{"clock",0xC0},
						{"hclock",0x20},
						{"chip",0x1300},
						{"fuse",0x11A0},
						{"boundry",0x1fff},
						{"exceed",0x21a0}
	};
#endif

static int RC = 0;
static char * piim = NULL;

static void exit_sighandler(int x)
{
  int ret = 0;
  if ( x == SIGSEGV && RC == 1)
  {
   printf("excepted SEGSEGV result\n");
   RC = 0;
   printf("TST_INFO: iim test PASS\n");
  ret = munmap(piim, 8 * 1024);
  if(ret == -1)
    perror("iim OCR unmap");
   _exit(0);
  }else{
   printf("un-excepted SEGSEGV result\n");
   RC = 0;
   printf("TST_INFO: iim test FAIL\n");
   _exit(-1);
  }
}

int main()
{
 int fd;
 int i=0;
 int ret = 0;

 signal(SIGSEGV,exit_sighandler);

 fd = open(IIM_DEVICE,O_RDONLY);
 if(fd < 0){
   perror("open");
   return 1;
 }
  
  piim = mmap(NULL, 8 * 1024, PROT_READ, MAP_SHARED ,fd, 0);
  if(piim == (void *)-1 )
   perror("iim OCR");

  close(fd);

  RC = 1;
	for(i =0; i < MAX_REGS; i++)
	{
  printf("%s 0x%x\n",mreg[i].name ,*(int*)(piim+mreg[i].offset));
	}
	/*
  printf("clock 0x%x\n",*(int*)(piim+0xC0));
  printf("hclock 0x%x\n",*(int*)(piim+0x20));
  printf("chip:%s\n",piim+0x1300);
  printf("fuse:0x%x\n",*(int *)(piim+0x11A0));
  printf("test address equal 8k:0x%x\n",*(char *)(piim+0x1fff));
  printf("test address exceed 8k:0x%x\n",*(int *)(piim+0x21a0));
  */
  ret = munmap(piim, 8 * 1024);
  if(ret == -1)
    perror("iim OCR unmap");
   printf("TST_INFO: iim test PASS\n");
  return  0;
}
