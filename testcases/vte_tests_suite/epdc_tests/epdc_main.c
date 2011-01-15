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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */
/**
    @file   epdc_main.c

    @brief  main file for epdc frame buffer test.
*/

#ifdef __cplusplus
extern "C" {
#endif

/*==================================================================================================
                                        INCLUDE FILES
==================================================================================================*/
/* Standard Include Files */
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

/* Harness Specific Include Files. */
#include "test.h"
#include "usctest.h"

/* Verification Test Environment Include Files */
#include "epdc_test.h"

/* LOCAL MACROS */

/* LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS) */

/* LOCAL CONSTANTS */

/* LOCAL VARIABLES */

	int T_flag;		/* test case */
	char *T_opt;		/* test case */
	int d_flag;		/* fisrt fb to open */
	char *d_opt;		/* fb name  */
	int F_flag;		/* waveform setting */
	char *F_opt;
	int g_flag;		/* pixel format */
	char *g_opt;
	int H_flag;		/* tempture setting */
	char *H_opt;
	int u_flag;		/* auto update */
	char *u_opt;		/* auto update */
	int s_flag;		/* send update setting */
	char *s_opt;
	int S_flag;		/* send update setting */
	char *S_opt;
	int R_flag;		/* Rotation */
	char *R_opt;
	int y_flag;		/* power down delay */
	char *y_opt;
	int l_flag;
	char *l_opt;

	option_t opts[] = {
		{"T:", &T_flag, &T_opt},
		{"d:", &d_flag, &d_opt},
		{"F:", &F_flag, &F_opt},
		{"g:", &g_flag, &g_opt},
		{"H:", &H_flag, &H_opt},
		{"u:", &u_flag, &u_opt},
		{"s:", &s_flag, &s_opt},
		{"S:", &S_flag, &S_opt},
		{"R:", &R_flag, &R_opt},
		{"y:", &y_flag, &y_opt},
		{"l:", &l_flag, &l_opt},
		{NULL, NULL, NULL}
	};

/* GLOBAL VARIABLES */
	epdc_opts m_opt;

/* Extern Global Variables */
	extern int Tst_count;	/* counter for tst_xxx routines. */
	extern char *TESTDIR;	/* temporary dir created by tst_tmpdir(...) */
/* Global Variables */
	char *TCID = "epdcfb";	/* test program identifier */
	int TST_TOTAL = 6;	/* total number of tests in this file */

/* GLOBAL FUNCTION PROTOTYPES */
	void cleanup();

/* LOCAL FUNCTION PROTOTYPES */
	static void help(void);
	static void setup(void);

/* help */
	void help(void) {
		printf("Usage:\n");
		printf("[-T <int> : special test\n");
		printf("\t0(setting framebuffer):\n");
		printf("\t1(pan test):\n");
		printf("\t2(draw test):\n");
		printf("\t3(wait update test):\n");
		printf("\t4(alt buffer overlay test):\n");
		printf("\t5(collision region update test)]:\n");
		printf("\t6(max update region count test)]\n");
		printf
		    ("\t7(1000 frames sequence region no collision frame rate test)]\n");
		printf("\t8(power delay test)]\n");
		printf("\t9(draw graysacle 256)]\n");
		printf("[-F <string> : wave form 0,1,3,2,2,2]\n");
		printf("[-g <int> : grayscale 1(normal):2(inverion)]\n");
		printf("[-H <int> : tempture ]\n");
		printf("[-d /dev/fb0: fb device]\n");
		printf("[-u <int> : auto update mode 0(partial)/1(full)]\n");
		printf
		    ("[-s <string>: send update with format only in partial update]\n");
		printf
		    ("[-S <int>: SCHEME: 0/1/2 SNAPSHOT/QUEUE/QUEUE_AND_MERGE]\n");
		printf("[-R <int>: Rotation 0/1/2/3]\n");
		printf("[-y <int>: power down delay after last update]\n");
		printf("[-l <int>: set update flag 0,1,2 ]\n");
		return;
	}
/* cleanup */
/**
@brief  Performs all one time clean up for this test on successful
        completion,  premature exit or  failure. Closes all temporary
        files, removes all temporary directories exits the test with
        appropriate return code by calling tst_exit(...) function.cleanup

@param  Input :      None.
        Output:      None.

@return Nothing
*/
	void cleanup(void) {
		int VT_rv = TFAIL;
		/* VTE : Actions needed to get a stable target environment */
		VT_rv = epdc_fb_cleanup();
		if (VT_rv != TPASS) {
			tst_resm(TWARN,
				 "epdc_fb_cleanup() Failed : error code = %d",
				 VT_rv);
		}
		/* Exit with appropriate return code. */
		tst_exit();
	}

/* LOCAL FUNCTIONS */

/*===== setup =====*/
/**
@brief  Performs all one time setup for this test. This function is
        typically used to capture signals, create temporary dirs
        and temporary files that may be used in the course of this test.

@param  Input :      None.
        Output:      None.

@return On failure - Exits by calling cleanup().
        On success - returns 0.
*/
	void setup(void) {
		int VT_rv = TFAIL;
		/* VTE : Actions needed to prepare the test running */
		VT_rv = epdc_fb_setup();
		if (VT_rv != TPASS) {
			tst_brkm(TBROK, cleanup,
				 "VT_fb_setup() Failed : error code = %d",
				 VT_rv);
		}

		return;
	}

/*===== main =====*/
/**
@brief  Entry point to this test-case. It parses all the command line
        inputs, calls the global setup and executes the test. It logs
        the test status and results appropriately using the LTP API's
        On successful completion or premature failure, cleanup() func
        is called and test exits with an appropriate return code.

@param  Input :  argc - number of command line parameters.
                 **argv - pointer to the array of the command line parameters.
        Output:  None

@return On failure - Exits by calling cleanup().
        On success - exits with 0 exit value.
*/
	int main(int argc, char **argv) {
		int VT_rv = TFAIL;
		char *message;	/* From parse_opts() */

		if ((message = parse_opts(argc, argv, opts, &help)) != NULL) {
			printf("An error occured while parsing options: %s\n",
			       message);
			return VT_rv;
		}
		memset(&m_opt, 0, sizeof(m_opt));
		m_opt.Tid = T_flag ? atoi(T_opt) : 0;
		if (d_flag)
			strcpy(m_opt.dev, d_opt);
		else
			strcpy(m_opt.dev, "/dev/fb");
		if (l_flag)
			m_opt.update.flags = atoi(l_opt);
		if (F_flag)
			sscanf(F_opt, "%d,%d,%d,%d,%d,%d",
			       &(m_opt.waveform.mode_init),
			       &(m_opt.waveform.mode_du),
			       &(m_opt.waveform.mode_gc4),
			       &(m_opt.waveform.mode_gc8),
			       &(m_opt.waveform.mode_gc16),
			       &(m_opt.waveform.mode_gc32)
			    );
		else {
			m_opt.waveform.mode_init = 0;
			m_opt.waveform.mode_du = 1;
			m_opt.waveform.mode_gc4 = 3;
			m_opt.waveform.mode_gc8 = 2;
			m_opt.waveform.mode_gc16 = 2;
			m_opt.waveform.mode_gc32 = 2;
		}
		if (g_flag) {
			m_opt.grayscale = atoi(g_opt);
		} else {
			m_opt.grayscale = GRAYSCALE_8BIT;
		}
		if (H_flag) {
			m_opt.temp = atoi(H_opt);
		} else {
			m_opt.temp = -1;
		}
		if (u_flag) {
			m_opt.au = atoi(u_opt);
		} else {
			m_opt.au = -1;
		}
		if (R_flag) {
			m_opt.rot = atoi(R_opt);
		}
		if (s_flag) {
			/*the phy addr will get from get_mem
			   can not defined by commandline */
			sscanf(s_opt,
			       "%d:%d:%d:%d,%d,%d,%d,%d,%d,%d,%d:%d:%d:%d",
			       &(m_opt.update.update_region.top),
			       &(m_opt.update.update_region.left),
			       &(m_opt.update.update_region.width),
			       &(m_opt.update.update_region.height),
			       &(m_opt.update.waveform_mode),
			       &(m_opt.update.update_marker),
			       &(m_opt.update.temp), &(m_opt.update.flags),
			       &(m_opt.update.alt_buffer_data.width),
			       &(m_opt.update.alt_buffer_data.height),
			       &(m_opt.update.alt_buffer_data.alt_update_region.
				 top),
			       &(m_opt.update.alt_buffer_data.alt_update_region.
				 left),
			       &(m_opt.update.alt_buffer_data.alt_update_region.
				 width),
			       &(m_opt.update.alt_buffer_data.alt_update_region.
				 height)
			    );
			m_opt.su = 1;
		}
		if (S_flag) {
			m_opt.scheme = atoi(S_opt);
		} else {
			m_opt.scheme = UPDATE_SCHEME_SNAPSHOT;
		}
		if (y_flag) {
			m_opt.delay = atoi(y_opt);
		} else {
			m_opt.delay = FB_POWERDOWN_DISABLE;
		}
/*
		printf("current settings\n");
		printf("TestID = %d \ndevice is %s \n\r \
		waveform setting is %d,%d,%d,%d,%d,%d \n\r \
		tempture is %d\n\rgrayscale is %d \n\r \
		auto update mode is %d \n\rupdate setting is %d \n\r \
		rotation is %d \n\r scheme is %d\n\r update flag is %d \n\r", 
		m_opt.Tid, m_opt.dev, m_opt.waveform.mode_init, 
		m_opt.waveform.mode_du, m_opt.waveform.mode_gc4, 
		m_opt.waveform.mode_gc8, m_opt.waveform.mode_gc16, 
		m_opt.waveform.mode_gc32, m_opt.temp, 
		m_opt.grayscale, m_opt.au, m_opt.su, 
		m_opt.rot, m_opt.scheme, m_opt.update.flags);
		if (m_opt.su) {
			printf("\r\talt update region %d,%d,%d,%d \n\r\t
					alt waveform_mode %d \n\r\talt update_mode %d \n\r\t
					alt tempture %d \n\r\talt buffer width = %d \n\r\t
					alt buffer Height = %d \n\r\t
					update region within %d,%d,%d,%d \n", 
					m_opt.update.update_region.top, m_opt.update.update_region.left, 
					m_opt.update.update_region.width, m_opt.update.update_region.height, 
					m_opt.update.waveform_mode, m_opt.update.update_marker, 
					m_opt.update.temp, m_opt.update.alt_buffer_data.width, 
					m_opt.update.alt_buffer_data.height, 
					m_opt.update.alt_buffer_data.alt_update_region.top, 
					m_opt.update.alt_buffer_data.alt_update_region.left, 
					m_opt.update.alt_buffer_data.alt_update_region.width, 
					m_opt.update.alt_buffer_data.alt_update_region.height);
		}
*/
		/* perform global test setup, call setup() function. */
		setup();
		/* Print test Assertion using tst_resm() function with argument TINFO. */
		tst_resm(TINFO, "test case %d", m_opt.Tid);
		VT_rv = epdc_fb_test();
		/* VTE : print results and exit test scenario */
		if (VT_rv == TPASS) {
			tst_resm(TPASS, "%s worked as expected", TCID);
		} else {
			tst_resm(TFAIL, "%s did NOT work as expected", TCID);
		}
		cleanup();

		return VT_rv;
	}
#ifdef __cplusplus
}
#endif
