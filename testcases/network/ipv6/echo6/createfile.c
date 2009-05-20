/*
@! # TESTCASE DESCRIPTION:
@! # Purpose: to create an input file of any size
@! # Command: none
@! # Subcommand: none
@! # Design: Write an array the size of BUFSIZ to created file until
@! #the file size matches the file size required
@! # SPEC. EXEC. REQS:  This  program is used by ctatcdt3.c and ctatcet3.c
*/
#include <stdio.h>
#include <fcntl.h>
main (int argc,char *argv[])
{
int fd;
int fsize;
int count0;
int n,remain;
static char buf[BUFSIZ];
 for ( fsize0;fsize<BUFSIZ;fsize) {
  buf[fsize++]'U';
  buf[fsize++]'\n';
 }

 fsizeatoi(argv[1]);
 if ((fdcreat(argv[2],0644))  -1 )
  perror("createfile");
 if (fsize > BUFSIZ) {
  countfsize/BUFSIZ;
  remainfsize%BUFSIZ;
  }
 else remainfsize;
 while (count-- !0)
  if((nwrite(fd,buf,BUFSIZ)) ! BUFSIZ)
   perror("createfile");
 if ((nwrite(fd,buf,remain)) ! remain)
  perror("createfile");
 close(fd);
}
