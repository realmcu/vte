/***
**Copyright (C) 2012 2013 Freescale Semiconductor, Inc. All Rights Reserved.
**
**The code contained herein is licensed under the GNU General Public
**License. You may obtain a copy of the GNU General Public License
**Version 2 or later at the following locations:
**
**http://www.opensource.org/licenses/gpl-license.html
**http://www.gnu.org/copyleft/gpl.html
**/
/**
@file   v4l_g_modes.c
@brief  get supported camera modes.
Description of the file
@par Portability: ARM GCC
*/
/*======================== REVISION HISTORY ==================================

Author (core ID)      Date         CR Number    Description of Changes
-------------------   ----------   ----------   ------------------------------
Andy Tian             10/18/2012    NA           Issue version
Andy Tian             01/17/2012    NA           Do cleanup when open
                                                 device failed
=============================================================================*/

#ifdef __cplusplus
extern "C" {
#endif
/*======================== INCLUDE FILES ====================================*/
/* Standard Include Files */
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
/* Harness Specific Include Files. */
#include "test.h"
#include "usctest.h"
/* Verification Test Environment Include Files */
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <linux/videodev2.h>
#include <malloc.h>

#define MAX_MODE	20
/*======================== LOCAL CONSTANTS ==================================*/

/*======================== LOCAL MACROS =====================================*/

/*======================== LOCAL VARIABLES ==================================*/

/*======================== GLOBAL CONSTANTS =================================*/

/*======================== GLOBAL VARIABLES =================================*/
/* Extern Global Variables */
       
/* Global Variables */
	char *TCID = "v4l_modes";			/* test program identifier.          */
	int TST_TOTAL = 1;					/* total number of tests in this file.   */
    int fd_v4l = 0; 					
	int *modes_res=NULL;				/* store the all possible modes with resolution infor */
	int *sup_mode=NULL;					/* store the supported modes number */
	int *unsup_mode=NULL;				/* store the un-supported modes number */
	int ptk_mode=0; 					/* store kernel message print level */
	FILE *null_fp;

/*======================== LOCAL FUNCTION PROTOTYPES ========================*/

	void setup(void);
	void cleanup(void);
	void help(void);
	int main(int argc, char **argv);

/*======================== LOCAL FUNCTIONS ==================================*/


/*===== setup =====*/
/**
@brief  Performs all one time setup for this test. This function is
        typically used to capture signals, create temporary dirs
        and temporary files that may be used in the course of this test.
@param  Input :      None.
        Output:      None.
@return Nothing.
*/

/* insert v4l2 and camera modules */
void setup(void) {

	/* Open /dev/null for re-direct stdout and stderr */
	null_fp=fopen("/dev/null","w");

	/* Get the original kernel print level */
	FILE *ptk_fp=fopen("/proc/sys/kernel/printk","r");
	fscanf(ptk_fp, "%d", &ptk_mode);
	fclose(ptk_fp);

	/* Set the kernel print level as 2 to obmit driver debug message */
	system("echo 2 > /proc/sys/kernel/printk");
}
/*===== help =====*//**
@brief  Inform of the available options and the associated parameters
@param  None.
@return Nothing.
*/ 
void help(void) {
		printf
		    ("Usage : -D <V4L_Device> Select V4L device (default /dev/video0).\n \
			 For i.MX6X, video0 is for ov5642 and video1 is for ov5640_mipi.\n");
		printf("Usage : -R Output the resolution in form of mode:WidthxHeight\n");
		printf("Usage : -r <fps> Assign capture rate (default is 30). \n");
		printf
		    ("Usage : -C Do modes check.\n If some modes can not supported, \
			 case failed and print out the unsupported modes\n"); 
		printf
		    ("For example v4l_modes -D /dev/video0 -f 15.\n \
			 This command returns ov5642 capture supported modes for 15 fps.\n"); 
}


/*===== main =====*/
/**
@brief  Entry point to this test-case. It parses all the command line
        inputs, calls the global setup and executes the test. It logs
                                the test status and results appropriately using the LTP API's
                                On successful completion or premature failure, cleanup() func
                                is called and test exits with an appropriate return code.
@param  Input :      argc - number of command line parameters.
        Output:      **argv - pointer to the array of the command line parameters.
@return On failure - Exits by calling cleanup().
        On success - exits with 0 exit value.
*/
int main(int argc, char **argv) 
{
	struct v4l2_streamparm parm;
	struct v4l2_frmsizeenum fsize;
	int s_num=0, uns_num=0;  /* count the supported and un-supported mode number */

	/* parse options. */
	char *msg;
	int Dflag = 0, Rflag = 0, rflag = 0, Cflag = 0;
	char *Dopt, *ropt;
	char *g_v4l_device;
	char *g_v4l_rate;
	int max_mode;
	int mode;
	int f_mode=0;
	int rc=0;

	option_t options[] = {
		{"D:", &Dflag, &Dopt},	/* Select device                        */
		{"r:", &rflag, &ropt},	/* set capture rate                     */
		{"R", &Rflag, NULL},	/* Add resolution in output 		    */
		{"C", &Cflag, NULL},	/* Do modes supported check             */
		{NULL, NULL, NULL}		/* NULL required to end array           */
	};

	if ((msg = parse_opts(argc, argv, options, help)) != NULL) {
		tst_brkm(TBROK, cleanup, "OPTION PARSING ERROR - %s",
			 msg);
	}
	g_v4l_device = Dflag ? Dopt : "/dev/video0";
	g_v4l_rate = rflag ? ropt : "30";

	setup();

	if ((fd_v4l = open(g_v4l_device, O_RDWR, 0)) < 0)
	{
		printf("Unable to open %s\n", g_v4l_device);
		tst_brkm(TBROK, cleanup, "%s",
			 "Please load the right camera module");
	}

	/* get the all possible mode numbers */
	for ( max_mode=0; max_mode < MAX_MODE; max_mode++){
		fsize.index = max_mode;
		if (ioctl(fd_v4l, VIDIOC_ENUM_FRAMESIZES, &fsize) < 0)
		{
			break;
		}
	}
	/* get the mode resolution */
	if ( (modes_res=(void *)malloc(max_mode*3*sizeof(int))) == NULL ){
		tst_brkm(TBROK, cleanup, "%s",
			 "Allocated memory failed");
	}
	memset(modes_res, 0, max_mode*3*sizeof(int));
	for ( mode=0; mode < max_mode; mode++){
		fsize.index = mode;
		ioctl(fd_v4l, VIDIOC_ENUM_FRAMESIZES, &fsize);
		*(modes_res+mode*3)=mode;
		*(modes_res+mode*3+1)=fsize.discrete.width;
		*(modes_res+mode*3+2)=fsize.discrete.height;
	}

	/* Get the supported and unsupported modes */
	parm.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	parm.parm.capture.timeperframe.numerator = 1;
	parm.parm.capture.timeperframe.denominator = atoi(g_v4l_rate);
	if ( (sup_mode = (int *)malloc(max_mode*sizeof(int))) == NULL ){
		tst_brkm(TBROK, cleanup, "%s",
			 "Allocated memory failed");
	}
	memset(sup_mode, -1, max_mode*sizeof(int));
	if ( (unsup_mode = (int *)malloc(max_mode*sizeof(int))) == NULL ){
		tst_brkm(TBROK, cleanup, "%s",
			 "Allocated memory failed");
	}
	memset(unsup_mode, -1, max_mode*sizeof(int));

	for ( mode=0; mode < max_mode; mode++){
		parm.parm.capture.capturemode = mode;
		if (ioctl(fd_v4l, VIDIOC_S_PARM, &parm) < 0)
		{
	        *(unsup_mode+uns_num)=mode;
			uns_num++;
		}
		else
		{
	        *(sup_mode+s_num)=mode;
			s_num++;
		}
	}

	if ( Cflag ){
		if ( uns_num != 0 )
		{
			printf("Case Failed! Below modes unsupported: \n");
			for ( mode=0; mode<uns_num; mode++)
			{
				f_mode=*(unsup_mode+mode);
				printf("%d:%dx%d ",
						*(modes_res+f_mode*3),
						*(modes_res+f_mode*3+1),
						*(modes_res+f_mode*3+2)
					  );
			}
			rc=-1;
		}
		else
		{
			printf("Case Pass! All modes supported! \n");
			for ( mode=0; mode < max_mode; mode++){
				printf("%d:%dx%d ",
						*(modes_res+mode*3),
						*(modes_res+mode*3+1),
						*(modes_res+mode*3+2)
					  );
			}
		}
	}
	else
	{
		if (Rflag)
		{
			for ( mode=0; mode < s_num; mode++){
				printf("%d:%dx%d ",
						*(modes_res+(*(sup_mode+mode))*3),
						*(modes_res+(*(sup_mode+mode))*3+1),
						*(modes_res+(*(sup_mode+mode))*3+2)
					  );
			}
		}
		else
		{
			for ( mode=0; mode < s_num; mode++)
				printf("%d ", *(sup_mode+mode));
		}
	}
	printf("\n");
	close(fd_v4l);
	cleanup();
	return rc;
}

/*======================== GLOBAL FUNCTIONS =================================*/
/*===== cleanup =====*/
/**
@brief  Performs all one time clean up for this test on successful
                                completion,  premature exit or  failure. Closes all temporary
                                files, removes all temporary directories exits the test with
                                appropriate return code by calling tst_exit() function.cleanup
@param  Input :      None.
        Output:      None.
@return Nothing
*/
void cleanup(void) {
	char cmd[50];
	if(modes_res){
		free(modes_res);
		modes_res=NULL;
	}
	if(sup_mode){
		free(sup_mode);
		sup_mode=NULL;
	}
	if(unsup_mode){
		free(unsup_mode);
		unsup_mode=NULL;
	}
	if(ptk_mode)
	{
		/* recover the kernel message print level */
		sprintf(cmd, "echo %d > /proc/sys/kernel/printk",ptk_mode);
		system(cmd);
	}
	if(null_fp)
		fclose(null_fp);
}

#ifdef __cplusplus
}
#endif
