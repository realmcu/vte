/*
 * Watchdog Timer test application
 *
 * Vladislav Buzov <vbuzov@embeddedalley.com>
 *
 * Copyright (C) 2008 Embedded Alley Solutions, Inc
 *
 * Copyright (C) 2009 Freescale Semiconductor, Inc. All Rights Reserved.
 *
 * This file is licensed under the terms of the GNU General Public License
 * version 2.  This program  is licensed "as is" without any warranty of any
 * kind, whether express or implied.
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <linux/watchdog.h>

/* Defatult watchdog timer timeout, in seconds */
#define WDT_TIMEOUT		5

/* Default number of watchdog timer pings */
#define WDT_PING_NUM		10

/* Default number of seconds to wait for reset */
#define WDT_WAIT_TIMEOUT	5

/* Default watchdog device file */
#define WDT_FILE	"/dev/watchdog"

int wdt_timeout = WDT_TIMEOUT;
int wdt_pings = WDT_PING_NUM;
int wdt_wait_timeout = WDT_WAIT_TIMEOUT;

/* Ping forewer flag */
int wdt_ping_forever;

/* Stop watchdog after last ping */
int wdt_stop;

/* Watchdog device node */
char *wdt_file = WDT_FILE;

void usage (char *argv[])
{
	printf(
		"Use: %s [OPTIONS] [FILE]\n" \
		"Watchdog timer test\n"	\
		"OPTIONS:\n" \
		"\t-t N\tWatchdog timer timeout in seconds, default is %d\n" \
		"\t-p N\tNumber of Watchdog timer pings, default is %d\n" \
		"\t-w N\tReset wait timeout in seconds, default is %d\n" \
		"\t-f\tPing forever flag, overrides -p option\n" \
		"\t-s\tStop watchdog timer after last ping\n" \
		"\t-h\tDypslay this help and exit\n" \
		"FILE:\n" \
		"\tWatchdog device node file, default is /dev/watchdog\n",
		argv[0], WDT_TIMEOUT, WDT_PING_NUM, WDT_WAIT_TIMEOUT
	);
}

void parse_opts (int argc, char *argv[])
{
	int c;
	int p = 0;

	while ((c = getopt(argc, argv, "t:p:w:fsh")) != -1) {
		switch (c) {
		case 't':
			wdt_timeout = strtoul(optarg, NULL, 10);
			if (wdt_timeout <= 0) {
				printf("Invalid timeout value\n");
				usage(argv);
				exit(1);
			}
			break;

		case 'p':
			wdt_pings = strtoul(optarg, NULL, 10);
			if (wdt_timeout <= 0) {
				printf("Invalid number of pings value\n");
				usage(argv);
				exit(1);
			}
			p = 1;
			break;

		case 'w':
			wdt_wait_timeout = strtoul(optarg, NULL, 10);
			if (wdt_wait_timeout <= 0) {
				printf("Wrong wait timeout value\n");
				usage(argv);
				exit(1);
			}
			break;

		case 'f':
			wdt_ping_forever = 1;
			break;

		case 's':
			wdt_stop = 1;
			break;

		case 'h':
			usage(argv);
			exit(0);

		default:
			printf("Wrong option: %c\n", c);
			usage(argv);
			exit(1);
		}
	
	}

	if (optind == argc)
		printf("No watchdog file passed, using default %s\n", wdt_file);
	else
		wdt_file = argv[optind];

	/* -f option owerrides -p*/
	if (wdt_ping_forever) {
		wdt_pings = 1;
		if (p)
			printf("-p option is ignored\n");
	}
}

void wdt_test (void)
{
	int fd;
	int s, l;
	int r;

	printf("Watchdog timer test\n");

	/* Open file */
	fd = open(wdt_file, O_RDWR);
	if (fd < 0) {
		printf("Can't open watchdog file %s", wdt_file);
		exit(1);
	}

	/* Set timeout */
	r = ioctl(fd, WDIOC_SETTIMEOUT, &wdt_timeout);
	if (r < 0) {
		printf("Can't set watchdog timeout\n");
		close(fd);
		exit(1);
	}

	printf("Set watchdog timeout to %d seconds\n", wdt_timeout);

	/* Ping watchdog timer */
	while (wdt_pings) {
	
		/* Ping in a half of watchdog timer period */
		s = wdt_timeout / 2;
		sleep(s);

		printf("Ping watchdog\n");

		r = ioctl(fd, WDIOC_KEEPALIVE);
		if (r < 0) {
			printf("Can't ping watchdog timer\n");
			close(fd);
			exit(1);
		}

		if (!wdt_ping_forever)
			wdt_pings--;
	}

	/* Stop watchdog on closing: send 'V' to watchdog */
	if (wdt_stop) {
		r = write(fd, "V", 1);
		if (r < 0)
			printf("Can't send stop flag 'V' to watchdog\n");
	}

	close(fd);

	for (l = wdt_timeout; l > 0; l--) {
		printf("Wait for %d seconds\n", l);
		sleep(1);
	}

	/*
	 * Let's sleep for few seconds and then let user now
	 * the system is alive
	 */
	sleep(wdt_wait_timeout);

	if (!wdt_stop)
		printf("ERROR: ");

	printf("System is alive\n");
}

int main (int argc, char *argv[])
{
	parse_opts(argc, argv);
	wdt_test();

	return 0;
}
