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

#include <termios.h>
#include <string.h>
#include "uart_testapp_10.h"

#define LOOPBACK        0x8000
#define SUPPORT_BR      31

const char cBaud_rate[SUPPORT_BR][10] = 
          {"0","50","75","110","134","150","200","300","600","1200","1800",
           "2400","4800","9600","19200","38400","57600","115200","230400",
	   "460800","500000","576000","921600","1000000","1152000","1500000",
	   "2000000","2500000","3000000","3500000","4000000"};
const int LUT_B[SUPPORT_BR] = {
            B0,B50,B75,B110,B134,B150,B200,B300,
	    B600,B1200,B1800,B2400,B4800,B9600,
	    B19200,B38400,B57600,B115200,B230400,
	    B460800,B500000,B576000,B921600,
	    B1000000,B1152000,B1500000,B2000000,
            B2500000,B3000000,B3500000,B4000000
            };

int main(int argc, char **argv)
{
        int uart_file1;
        unsigned int line_val;
        char buf[5];
        struct termios mxc, old;
        int retval;
	int iBaud = 0;
	int speed = -1;
        
        printf("Test: MXC UART!\n");
        printf("Usage: mxc_uart_test <UART device name, opens UART2 if no dev name is specified>\n");

        if (argc == 1) {
                /* No Args, open UART 2 */
                if ((uart_file1 = open("/dev/ttymxc1", O_RDWR)) == -1) {
                        printf("Error opening UART 2\n");
                        exit(1);
                } else {
                        printf("Test: UART 2 opened\n");
                }
        } else {
                /* Open the specified UART device */
                if ((uart_file1 = open(*++argv, O_RDWR)) == -1) {
                        printf("Error opening %s\n", *argv);
                        exit(1);
                } else {
                        printf("%s opened\n", *argv);
                }
		/*option for baud rate*/
		if( argc == 3 )
		{
		  int i = 0;
		  iBaud = 1;
		  do{
		    if(strcmp(cBaud_rate[i],argv[1]) == 0)
		    {
		      printf("match at %d\n",i);
		      speed = LUT_B[i];
		      break;
		    }
		  }while(++i < SUPPORT_BR);

                  switch (speed)
		  {
		    case B0:
		    case B50:
		    case B75:
		    case B110:
		    case B134:
		    case B150:
		    case B200:
		    case B300:
		    case B600:
		    case B1200:
		    case B1800:
		    case B2400:
		    case B4800:
		    case B9600:
		    case B19200:
		    case B38400:
		    case B57600:
		    case B115200:
		    case B230400:
		    case B460800:
		    case B500000:
		    case B576000:
		    case B921600:
		    case B1000000:
		    case B1152000:
		    case B1500000:
		    case B2000000:
		    case B2500000:
		    case B3000000:
		    case B3500000:
		    case B4000000:
		     printf("set baud rate to %d \n", speed);
		     break;
		    default:
		    printf("suooprted baud rate is: \n");
		    printf("0,50,75,,110,134,150,200,300,600,1200\n");
		    printf("1800,2400,4800,9600,57600,115200 \n");
		    printf("230400,460800,500000,576000,921600 \n");
		    printf("1000000,1152000, 1500000,2000000 \n");
		    printf("3000000,3500000,4000000\n");
		    return 1;
		   }
		}/* end if */ 
        }
                
        tcgetattr(uart_file1, &old);
        mxc = old;
        mxc.c_lflag &= ~(ICANON | ECHO | ISIG);
	if(iBaud)
	{
	/*  mxc.c_cflag &= ~mxc.c_cflag;*/
	  printf("old baud %x\n", mxc.c_cflag);
	  mxc.c_cflag |= speed;
	}
        retval = tcsetattr(uart_file1, TCSANOW, &mxc);
        printf("Attributes set %d\n", retval); 
        
        line_val = LOOPBACK;
        retval |= ioctl(uart_file1, TIOCMSET, &line_val);
	if (retval < 0)
	  printf("Test: IOCTL Fail on TIOCMSET\n");
        printf("Test: IOCTL Set\n");

        printf("Data Written= Test\n");
        write(uart_file1, "Test\0", 5);
       
        read(uart_file1, buf, 5);
        printf("Data Read back= %s\n", buf);
	if( 0 != strcmp(buf,"Test"))
	{
	    retval = 1;
	    printf("Test FAIL: data loop bakc fail, should be Test but return %s \n",buf);
	}
        sleep(2);
        retval |= ioctl(uart_file1, TIOCMBIC, &line_val);

        retval |= tcsetattr(uart_file1, TCSAFLUSH, &old);

        close(uart_file1);
        return retval;
}
