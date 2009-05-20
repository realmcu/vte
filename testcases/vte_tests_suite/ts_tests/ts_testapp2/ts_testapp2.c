#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <linux/input.h>
#include<sys/select.h>
#include<sys/time.h>
#include<unistd.h>

#define TS_DEV "/dev/input/event1"
int waitkeyin(void)
{
        fd_set rset;
        int nSelect;

        FD_ZERO(&rset);
        FD_SET(fileno(stdin), &rset);

        nSelect = select(fileno(stdin) + 1, &rset, NULL, NULL, NULL);

        if (nSelect == -1)
                return 0;
        return nSelect > 0;
}
/*
main return value 0-success -1-fail
Child process maintains one while(1) loop to get touch value
synchronously while touching screen with a stylus continuously;
Parent process maintains the other loop,if you want to
quit test,you keyin 'Q'or'q',then parent process will send signal'9'
to kill child process, and then return.
---Gamma.Gao,b14842,04/09,2008
*/
int main(void)
{
 struct input_event event;
 int fd, ret;
 int x, y, pressure;
       unsigned char  ch;
       pid_t     pid;

 if ((pid = fork()) < 0) {
        printf("fork child process error\n");
  return -1;
    } else if (pid == 0) {      /* child */
    printf("This is child process:%d\n",getpid());
    printf("fork return value in child process is %d\n",pid);
    fd = open(TS_DEV, O_RDONLY);
 if (fd <= 0) {

  printf("Can not open dev file:%s\n", TS_DEV);
  return -1;
 }
 while (1) {
  ret = read(fd, &event, sizeof(struct input_event));
  if (ret > 0) {

   if (event.type == EV_SYN) {
    printf("Get sync: x(%d), y(%d), pressure(%d)\n", x, y, pressure);
    continue;
   }
   switch (event.code) {

   case ABS_X:
    x = event.value;
   // printf("read X -> %d\n", x);
    break;
   case ABS_Y:
    y = event.value;
   // printf("read Y -> %d\n", y);
    break;
   case ABS_PRESSURE:
    pressure = event.value;
   // printf("read pressure -> %d\n", pressure);
    break;
   }

  }

 }

    } else {    /* parent */
    printf("This is parent process:%d\n",getpid());
    printf("Fork return value in parent process is %d\n",pid);
    printf("If you want to exit touch test,please key in 'Q' or 'q'\n ");
      if (waitkeyin()){
                               ch = getchar();
                               if (ch == 'Q' || ch == 'q'){
        kill(pid,9); /*kill child process now*/
                    close(fd); /*close device file*/
                                }

                        }
    }
return 0;
}
