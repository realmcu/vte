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

#include <string.h>
#include "uart_testapp_11.h"
#include <signal.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>

#define LOOPBACK 0x8000
#define SUPPORT_BR      31
#define PATTERN 0xF0

const char cBaud_rate[SUPPORT_BR][10] =
    { "0", "50", "75", "110", "134", "150", "200", "300", "600", "1200", "1800",
	"2400", "4800", "9600", "19200", "38400", "57600", "115200", "230400",
	"460800", "500000", "576000", "921600", "1000000", "1152000", "1500000",
	"2000000", "2500000", "3000000", "3500000", "4000000"
};
const int LUT_B[SUPPORT_BR] = {
	B0, B50, B75, B110, B134, B150, B200, B300,
	B600, B1200, B1800, B2400, B4800, B9600,
	B19200, B38400, B57600, B115200, B230400,
	B460800, B500000, B576000, B921600,
	B1000000, B1152000, B1500000, B2000000,
	B2500000, B3000000, B3500000, B4000000
};

int SEND_SIZE = 10 * 1024;
int ctrl_c_rev = 0;
pid_t pid;

void ctrl_c_handler(int signum, siginfo_t * info, void *myact)
{
	ctrl_c_rev = 1;
	if (pid != 0)
		kill(pid, SIGKILL);
}

int main(int argc, char **argv)
{
	int uart_file1;
	unsigned int line_val;
	struct termios mxc, old;
	int retval;
	int iBaud = 0;
	int speed = -1;
	struct sigaction act;

	printf("Test: MXC UART!\n");
	printf
	    ("Usage: mxc_uart_test <UART device name, opens UART2 if no dev name is specified>\n");

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
		if ((uart_file1 = open(argv[1], O_RDWR)) == -1) {
			printf("Error opening %s\n", *argv);
			exit(1);
		} else {
			printf("%s opened\n", *argv);
		}
		/*option for baud rate */
		if (argc >= 3) {
			int i = 0;
			iBaud = 1;
			do {
				if (strcmp(cBaud_rate[i], argv[2]) == 0) {
					printf("match at %d\n", i);
					speed = LUT_B[i];
					break;
				}
			} while (++i < SUPPORT_BR);

			if (argc == 4)
				SEND_SIZE = atoi(argv[3]);

			switch (speed) {
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
				printf("set baud rate to %o \n", speed);
				break;
			default:
				printf("suooprted baud rate is: \n");
				printf
				    ("0,50,75,,110,134,150,200,300,600,1200\n");
				printf("1800,2400,4800,9600,57600,115200 \n");
				printf("230400,460800,500000,576000,921600 \n");
				printf("1000000,1152000, 1500000,2000000 \n");
				printf("3000000,3500000,4000000\n");
				return 1;
			}
		}		/* end if */
	}
	tcgetattr(uart_file1, &old);
	mxc = old;
	mxc.c_lflag &= ~(ICANON | ECHO | ISIG | IXON | IXOFF | CRTSCTS);
	if (iBaud) {
		/*  mxc.c_cflag &= ~mxc.c_cflag; */
		printf("old baud %o\n", mxc.c_cflag & CBAUD);
		mxc.c_cflag &= ~CBAUD;
		mxc.c_cflag |= speed;
	}
	retval = tcsetattr(uart_file1, TCSANOW, &mxc);
	printf("Attributes set %d\n", retval);

	ioctl(uart_file1, TIOCMGET, &line_val);
	line_val |= LOOPBACK;
	retval |= ioctl(uart_file1, TIOCMSET, &line_val);

	if (retval < 0)
		printf("Test: IOCTL Fail on TIOCMSET\n");

	/*fork process */
	/*
	 * parent do read 
	 * child do write 100M data and calculate the average speed
	 */
	pid = fork();
	if (pid < 0)
		perror("fork");
	else if (pid == 0) {
		int volumn = SEND_SIZE;
		int step = 1024;
		char *tx = (char *)malloc(step * sizeof(char));
		struct timeval begin, end;
		int sec = 0, usec = 0;
		int rt = 0;

		memset(tx, PATTERN, step);
		if (tx == NULL) {
			perror("malloc");
			exit(-1);
		}
		printf("send %d\n", volumn);
#if 1
		while (volumn) {
			gettimeofday(&begin, NULL);
			rt = write(uart_file1, tx, step);
			if (rt < 0) {
				perror("write");
				kill(0, SIGKILL);
				break;
			}
			gettimeofday(&end, NULL);
			sec += end.tv_sec - begin.tv_sec;
			usec += end.tv_usec - begin.tv_usec;
			while (usec > 1000000) {
				sec++;
				usec -= 1000000;
			}
			volumn -= step;
			//printf("remain %d\n", volumn);
			usleep(100);
		}
#endif
		if (sec > 0)
			printf("\n\raverage speed %d Bits at %o baud\n",
			       SEND_SIZE * 8 / (sec + 1), speed);
		else if (sec == 0) {
			printf("\n\raverage speed %ld Bits at %o baud\n",
			       (long)(SEND_SIZE * 1.0 / usec) * 8 * 1000000,
			       speed);
			printf("total time %d\n", usec);
		}
		exit(0);
	} else {
		static int iores = 0;
		int iocount;
		int cont = 1;
		unsigned char *rx = NULL;
		int cbuff = 1024;
		int retry = 0;

		sigemptyset(&act.sa_mask);
		act.sa_flags = SA_SIGINFO;
		act.sa_sigaction = ctrl_c_handler;

		rx = (unsigned char *)calloc(cbuff, sizeof(unsigned char));
		if (NULL == rx) {
			perror("malloc");
			return -1;
		}
		while (cont) {
			retry++;
			if (iores >= SEND_SIZE || ctrl_c_rev) {
				cont = 0;
				break;
			}
			ioctl(uart_file1, FIONREAD, &iocount);
			if (!iocount) {
				char cmd[64];
				int rt = 0;
#if 1
				rt = waitpid(0, NULL, WNOHANG);
				if (rt == 0)
					continue;
				else
					cont = 0;
#else
				sprintf(cmd,
					"ps -a | grep %d | awk '{print $4}' | grep 'Z' > /dev/null",
					pid);
				rt = system(cmd);
				if (rt != 0)
					continue;
				else
					cont = 0;
#endif
			} else {
				retry = 0;
				if (iocount > cbuff) {
					rx = realloc(rx,
						     iocount *
						     sizeof(unsigned char));
					cbuff = iocount;
				}
				iores += read(uart_file1, rx, iocount);
				iocount = 0;
				/*printf("\rreceive %d bytes", iores); */
				if ((unsigned char)rx[0] != PATTERN) {
					printf("receive error %x at %d!!!\n",
					       (unsigned char)rx[0], iores);
					cont = 1;
					retval = 1;
				}
				iocount = 0;
			}
		}
		free(rx);
		wait(NULL);
	}
	printf("\r\ntest finished\n");
	/*restore */
	ioctl(uart_file1, TIOCMGET, &line_val);
	line_val |= ~LOOPBACK;
	retval |= ioctl(uart_file1, TIOCMSET, &line_val);
	close(uart_file1);
	return retval;
}
