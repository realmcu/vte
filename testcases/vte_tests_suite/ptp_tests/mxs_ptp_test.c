/*
 * Copyright 2010 Freescale Semiconductor, Inc. All rights reserved.
 */

/*
 * The code contained herein is licensed under the GNU General Public
 * License. You may obtain a copy of the GNU General Public License
 * Version 2 or later at the following locations:
 *
 * http://www.opensource.org/licenses/gpl-license.html
 * http://www.gnu.org/copyleft/gpl.html
 */

/*
 * @file mxs_ptp_test.c
 *
 * @brief MXS 1588 driver test application
 *
 */

#ifdef __cplusplus
extern "C"{
#endif

/*=======================================================================
                                        INCLUDE FILES
=======================================================================*/
/* Standard Include Files */
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/mman.h>
#include <math.h>
#include <string.h>
#include <malloc.h>

#include "mxs_ptp_test.h"

#define BUF_MAX_SIZE	0x21	/* 32 + 1 to hold 'NULL' */
#define DEV_ptp0	"/dev/ptp0"
#define DEV_ptp1   	"/dev/ptp1"

struct ptp_command {
	u32 commd;
	struct ptp_time ptime;
};

struct rtc_time_t {
	u8	hh;
	u8	mm;
	u8	ss;
	u8	resv;
};

void help_info(const char *appname)
{
	printf("\n"
	       "*******************************************************\n"
	       "*******    Test Application for 1588 driver    ********\n"
	       "*******************************************************\n"
	       "*    This test set and get value to 1588 timer        *\n"
	       "*                                                     *\n"
	       "*                                                     *\n"
	       "*    Options : %s <command> <devid> <value>           *\n"
	       "*                                                     *\n"
	       "*    <command> - 1588 driver ioctl command            *\n"
	       "*    <devid> - FEC device number in [0, 1]            *\n"
	       "*    <value> - setting actual value                   *\n"
	       "*    Syntax: Set FEC1 timer value                     *\n"
	       "*            %s -st 1 <hh:mm:ss>                      *\n"
	       "*            Get FEC0 timer value                     *\n"
	       "*            %s -gt 0                                 *\n"
	       "*******************************************************\n"
	       "\n", appname, appname, appname);
}

int process_time_param(char **argv, struct ptp_command *pcommd,
			struct rtc_time_t *rtime)
{
	char *tmp;
	char stime[2];

	tmp = argv[3];
	strncpy(stime, tmp, 2);
	rtime->hh = atoi(stime);
	tmp = tmp + 3;
	strncpy(stime, tmp, 2);
	rtime->mm = atoi(stime);
	tmp = tmp + 3;
	strncpy(stime, tmp, 2);
	rtime->ss = atoi(stime);

	return 0;
}

int convert_time(struct rtc_time_t *rtime, struct ptp_rtc_time *ptime, int mode)
{
	struct ptp_time *ptptime = &(ptime->rtc_time);

	if (mode ==0) {
		ptptime->sec = rtime->ss + (rtime->mm * 60) + (rtime->hh *3600);
		ptptime->nsec =0;
	} else {
		rtime->hh = (u8)(ptptime->sec / 3600);
		rtime->mm = (u8)((ptptime->sec % 3600) / 60);
		rtime->ss = (u8)((ptptime->sec % 3600) % 60);
	}

	return 0;
}

int process_cmdline(int argc, char **argv, struct ptp_command *pcommd,
			struct rtc_time_t *rtime)
{
	if (strcmp(argv[1], "-st") == 0) {
		pcommd->commd = PTP_COMD_SETCNT;
		process_time_param(argv, pcommd, rtime);
	}
	else if (strcmp(argv[1], "-gt") == 0) {
		pcommd->commd = PTP_COMD_GETCNT;
	}
	else if (strcmp(argv[1], "-help") == 0) {
		help_info(argv[0]);
		return -1;
	}

	return 0;
}

int execute_ptp_test(struct ptp_command *pcommd, struct rtc_time_t *rtime,
			 char **argv)
{
	int fd = 0;
	int dev_id = 0;
	struct ptp_rtc_time p_time;
	int res = 0;

	dev_id = atoi(argv[2]);
	if (dev_id == 0)
		fd = open(DEV_ptp0, O_RDWR, 0);
	else
		fd = open(DEV_ptp1, O_RDWR, 0);

	if (fd < 0) {
		printf("Unable to open %s\n", DEV_ptp0);
		return -1;
	}
	if(pcommd->commd == PTP_COMD_SETCNT) {
		convert_time(rtime, &p_time, 0);
		if (ioctl(fd, PTP_SET_RTC_TIME, &p_time) < 0) {
			printf("PTP_SET_RTC_TIME failed\n");
			res = -1;
			goto exit;
		} else
			printf("Set timer %02d:%02d:%02d successfully\n",
				rtime->hh, rtime->mm, rtime->ss);
	} else if (pcommd->commd == PTP_COMD_GETCNT) {
		if (ioctl(fd, PTP_GET_CURRENT_TIME, &p_time) < 0) {
			printf("PTP_GET_CURRENT_TIME failed\n");
			res = -1;
			goto exit;
		} else {
			convert_time(rtime, &p_time, 1);
			printf("PTP_GET_CURRENT_TIME: %02d:%02d:%02d\n",
				rtime->hh, rtime->mm, rtime->ss);
		}
	}

exit:
	close(fd);

	return res;
}

int main(int argc, char **argv)
{
	struct ptp_command *p_commd;
	struct rtc_time_t rtctime;
	int res;

	if (argc < 3) {
		help_info(argv[0]);
		return 1;
	}

	p_commd =  malloc(sizeof(struct ptp_command));
	memset(p_commd, 0, sizeof(struct ptp_command));
	printf("MXS ENET 1588 driver test\n");
        if (process_cmdline(argc, argv, p_commd, &rtctime) < 0) {
                return -1;
        }

	printf("Execute ptp test: \n");
	res = execute_ptp_test(p_commd, &rtctime, argv);

	free(p_commd);
	return res;
}
