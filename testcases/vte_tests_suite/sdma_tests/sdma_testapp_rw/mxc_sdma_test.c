/*
 * Copyright 2004-2006 Freescale Semiconductor, Inc. All rights reserved.
 */

/*
 * The code contained herein is licensed under the GNU General Public
 * License. You may obtain a copy of the GNU General Public License
 * Version 2 or later at the following locations:
 *
 * http://www.opensource.org/licenses/gpl-license.html
 * http://www.gnu.org/copyleft/gpl.html
 */
/*============================================================================
Revision History:
                 Modification     Tracking
Author/core ID       Date          Number    Description of Changes
--------------   ------------    ----------  ----------------------------
Z.Spring          19/11/2008        n/a       Planted from unit test, 
                                                add return value judgement
============================================================================*/
#ifdef __cplusplus
extern "C"{
#endif

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <poll.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>

int test() {
  char buf[48];
  int fd;
  int err=0;

  fd = open("/dev/sdma_test",O_RDWR);
 
  if(fd < 0){
    printf("Failed open\n");
    return 1;
  }
  
  if (err=write(fd, buf, 0)){
      close(fd);
      return err;
  }

  sleep(1);

  if (err=read(fd, buf, 0)){
      close(fd);
      return err;
  }

  close(fd);
  return err;
}

int main(int argc,char **argv) {
  return test();  
}

#ifdef __cplusplus
}
#endif
