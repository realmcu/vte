/*
 * Copyright (C) 2010 Freescale Semiconductor, Inc. All Rights Reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/
/*
 * NAME
 *	timer_resolution.c
 *
 * DESCRIPTION
 *	Check if gettimeofday has resolution less than 1ms
 *
 */

#include <stdint.h>
#include <stdio.h>
#include <sys/time.h>
#include <signal.h>
#include <stdlib.h>
#include <test.h>
#include <usctest.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>

#define gettimeofday(a,b)  syscall(__NR_gettimeofday,a,b)
#define MAX_LOOP 10

char *TCID = "timer resolutions";	/* Test program identifier.    */
int TST_TOTAL = 1;		/* Total number of test cases. */

int Tflag;
char *tlen = "100";

int Lflag;
char *lres = "1000";

sig_atomic_t done;

option_t opts[] = { {"T:", &Tflag, &tlen}, 
                    {"L:", &Lflag, &lres},
										{} };

void alarm_handle(int sig)
{
	done = 1;
}

void cleanup(void)
{
	TEST_CLEANUP;

	tst_exit();
}

void help()
{
	printf("  -T len  seconds to test gettimeofday resolutions(default %s)\n", tlen);
}

int main(int ac, char **av)
{
	struct timeval tv1, tv2;
	char *msg;

	if ((msg = parse_opts(ac, av, opts, help)) != (char *)NULL) {
		tst_brkm(TBROK, cleanup, "OPTION PARSING ERROR - %s", msg);
	}

	tst_sig(NOFORK, DEF_HANDLER, cleanup);
	TEST_PAUSE;

	tst_resm(TINFO, "checking if gettimeofday resolution, takes %ss",
		 tlen);
	signal(SIGALRM, alarm_handle);
	alarm(atoi(tlen));
   
	if (gettimeofday(&tv1, NULL) != 0)
		tst_brkm(TBROK, cleanup, "first gettimeofday() failed: %s\n",
			 strerror(errno));
	while (!done) {
		if (gettimeofday(&tv2, NULL) != 0)
			tst_brkm(TBROK, cleanup,
				 "loop gettimeofday() failed: %s\n",
				 strerror(errno));

		if (tv2.tv_sec < tv1.tv_sec ||
		    (tv2.tv_sec == tv1.tv_sec && tv2.tv_usec < tv1.tv_usec)) {
			tst_resm(TFAIL,
				 "Time is going backwards: old %jd.%jd vs new %jd.%jd!",
				 (intmax_t)tv1.tv_sec, (intmax_t)tv1.tv_usec, (intmax_t)tv2.tv_sec,
				 (intmax_t)tv2.tv_usec);
			cleanup();
			return 1;
		}
		tv1 = tv2;
		/*now try to get the minimum time interval*/
		{
			int i = MAX_LOOP;
			while(i--);
		}
		if (gettimeofday(&tv2, NULL) != 0)
			tst_brkm(TBROK, cleanup,
				 "loop gettimeofday() failed: %s\n",
				 strerror(errno));
		if (1000000 * (tv2.tv_sec - tv1.tv_sec)
		    + (tv2.tv_usec - tv1.tv_usec) > atoi(lres)) {
			tst_resm(TFAIL,
				 "Time resolution is not achieved %d old %jd.%jd vs new %jd.%jd",atoi(lres),
				 (intmax_t)tv1.tv_sec, (intmax_t)tv1.tv_usec, (intmax_t)tv2.tv_sec,
				 (intmax_t)tv2.tv_usec);
			cleanup();
			return 1;
		}
	}

	tst_resm(TPASS, "gettimeofday resolution is less than %s", lres);

	cleanup();
	return 0;
}
