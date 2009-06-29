/***
**Copyright 2006-2009 Freescale Semiconductor, Inc. All Rights Reserved.
**
**The code contained herein is licensed under the GNU General Public
**License. You may obtain a copy of the GNU General Public License
**Version 2 or later at the following locations:
**
**http://www.opensource.org/licenses/gpl-license.html
**http://www.gnu.org/copyleft/gpl.html
**/
/**
@file   v4l_capture_main.c

@brief  EMMA capture test main unit.

Description of the file

@par Portability: ARM GCC

*/

/*======================== REVISION HISTORY ==================================

Author (core ID)      Date         CR Number    Description of Changes
-------------------   ----------   ----------   ------------------------------
Kardakov Dmitriy/ID            09/11/06       TLSbo76802   Initial version 
=============================================================================*/

/*============================================================================
Total Tests: 1

Test Name:   EMMA capture test

Test Assertion
& Strategy:    

=============================================================================*/


#ifdef __cplusplus
extern "C"{
#endif

/*======================== INCLUDE FILES ====================================*/
/* Standard Include Files */
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <pthread.h>
    
/* Harness Specific Include Files. */
#include "test.h"
#include "usctest.h"

/* Verification Test Environment Include Files */
#include "v4l_capture_test.h"

/*======================== LOCAL CONSTANTS ==================================*/


/*======================== LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS) =======*/


/*======================== LOCAL MACROS =====================================*/

/*======================== LOCAL VARIABLES ==================================*/

/*======================== GLOBAL CONSTANTS =================================*/


/*======================== GLOBAL VARIABLES =================================*/
/* Extern Global Variables */
extern int  Tst_count;               /* counter for tst_xxx routines.         */
extern char *TESTDIR;                /* temporary dir created by tst_tmpdir */

int gExitCleanup = 1;

/* Global Variables */
char *TCID     = "v4l_capture_testapp"; /* test program identifier.          */
int  TST_TOTAL = 1;                  /* total number of tests in this file.   */

sV4LTestConfig gV4LTestConfig;

/*======================== LOCAL FUNCTION PROTOTYPES ========================*/

void setup(void);
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

void setup(void)
{
        int VT_rv = VT_v4l_capture_setup();  
 
        if(VT_rv != TPASS)
        {                
                tst_brkm(TBROK , cleanup, "VT_setup() Failed : error code = %d", VT_rv);
        }
}
/*===== help =====*/

/**
@brief  Inform of the available options and the associated parameters

@param  None.

@return Nothing.
*/

void help(void)
{
        printf("Usage : -D <V4L_Device>.To put V4L device (default /dev/v4l/video0)\n");
        printf("Usage : -H <height>. To put capture image height\n");
        printf("Usage : -W <width>. Capture image width\n");
        printf("Usage : -S To do interactive resizing\n");
        printf("Usage : -B <left,top,width,height> To do cropping of the video.\n");
        printf("                                   The cropping restangle size is (width,heyght) pixels.\n");
        printf("                                   The offset of this restangle is (left,top)\n");
        printf("Usage : -E To imply \"v4l_capture_testcase -V\" error cases with unsupported format\n");
        printf("Usage : -T <n>. To make the capture n times\n");
        printf("Usage : -C <case_number>.To put number corresponded to case of capture\n");
        printf("\t-C <1>:\tTo get video overlay device test\n");
        printf("\t-C <2>:\tTo get video capture device test and to display video on frame buffer device\n");
        printf("\t-C <3>:\tTo get video capture device test and to write snapshort to dump file\n");
        printf("\t-C <4>:\tTo get video capture device test and to write video data to dump file\n");
        printf("\t-C other value:\t to get TBROK result\n"); 
        printf("Usage : -J <frame_rate, duration>  To put the video record parameters. \n");
        printf("                                   The frame rate of video record is equal <frame_rate> fr/sec.\n");
        printf("                                   The duration of video record is equal <duration> sec.\n");
        printf("Usage : -o <output file>.  To put output file(full path)\n");
        printf("Usage : -O <pixel_format>. To put pixel format\n");
        printf("Usage : -u <output device>. To put output device\n");  
        printf("Usage : -Y <overlay_type>. 1=V4L2_FBUF_FLAG_PRIMARY, 2=V4L2_FBUF_FLAG_OVERLAY\n");  
        
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
        int VT_rv = TFAIL;
        int ret=0; 
        
        const int heightTable[3]     =  {  240, 120, 10 };
        const int widthTable[3]      =  {  320, 200, 20 };


        const char userOutput[3][40] = {
                                            "biggest size",
                                            "middle size",
                                            "smallest size"
                                        };        
        
        /* parse options. */  
        char *msg;

        int Dflag = 0, 
            Hflag = 0, 
            Wflag = 0, 
            Sflag = 0,
            Bflag = 0,
            Tflag = 0,
            oflag = 0,
            Oflag = 0,
            Cflag = 0,
            uflag = 0, 
            Eflag = 0, 
            Yflag = 0,
            Jflag = 0,
            vflag = 0;

        char *Dopt, 
             *Hopt, 
             *Wopt, 
             *Topt,
             *oopt,
             *Oopt,
             *Copt,
             *uopt,
             *Yopt,
             *Bopt, 
             *Jopt;              
      
        option_t options[] =
        {
                { "D:", &Dflag,         &Dopt },        /* Video capturing device               */
                { "H:", &Hflag,         &Hopt },        /* Capturing height                     */
                { "W:", &Wflag,         &Wopt },        /* Capturing width                      */
                { "S" , &Sflag,         NULL  },        /* Resize test                          */
                { "B:", &Bflag,         &Bopt },        /* Cropping test                        */
                { "J:", &Jflag,         &Jopt },        /* Video option                        */
                { "T:", &Tflag,         &Topt },        /* Capture times                        */
                { "o:", &oflag,         &oopt },        /* Path to output file                  */
                { "O:", &Oflag,         &Oopt },        /* Pixel format                         */
                { "C:", &Cflag,         &Copt },        /* Case number                          */
                { "u:", &uflag,         &uopt },        /* Output device (default /dev/fb0)     */
                { "Y:", &Yflag,         &Yopt },        /* Overlay Type (for Overlay only)      */
                { "E",  &Eflag,         NULL  },        /* Error cases                          */
                { "v",  &vflag,         NULL  },        /* Verbose mode                         */
                { NULL, NULL,           NULL  }         /* NULL required to end array           */
        };

        if((msg=parse_opts(argc, argv, options, help)) != NULL)
        {
                tst_brkm(TBROK, cleanup, "OPTION PARSING ERROR - %s", msg);
        }       
        
        /* Init struct sV4LTestConfig*/
        
        gV4LTestConfig.mV4LDevice =  Dflag ? Dopt : "/dev/v4l/video0";
        gV4LTestConfig.mOutputDevice =  uflag ? uopt : "/dev/fb0";
        gV4LTestConfig.mOutputFile = NULL;
        gV4LTestConfig.mWidth = Wflag ? atoi(Wopt) : 240;
        gV4LTestConfig.mHeight = Hflag ? atoi(Hopt) : 190;
        if((gV4LTestConfig.mCount = Tflag ? atoi(Topt) : 10) <=0)
                tst_resm(TWARN, "Invalid argument for -T  %d", gV4LTestConfig.mCount);;
        gV4LTestConfig.mCaseNum  = Cflag ? atoi(Copt) : PRP_VF;
        gV4LTestConfig.mOutputFormat = Oflag;
        gV4LTestConfig.mCrop = Bflag;
        gV4LTestConfig.mVerbose = vflag;
        gV4LTestConfig.mPixFormat = Oflag ? Oopt : NULL;
        gV4LTestConfig.mCropRect.left = 0;
        gV4LTestConfig.mCropRect.top = 0;
        gV4LTestConfig.mCropRect.width = 640;
        gV4LTestConfig.mCropRect.height = 480;
        gV4LTestConfig.mOverlayType = Yflag ? atoi(Yopt) : V4L2_FBUF_FLAG_OVERLAY;
       
        if((gV4LTestConfig.mCaseNum > 4)||(gV4LTestConfig.mCaseNum < 1))
        {
                tst_resm(TBROK, "Invalid option for -C flag : %d", gV4LTestConfig.mCaseNum);
                return TFAIL;
        }

        if(Bflag)
        {
                sscanf(Bopt,
                       "%d,%d,%d,%d",
                       &gV4LTestConfig.mCropRect.left,
                       &gV4LTestConfig.mCropRect.top,
                       &gV4LTestConfig.mCropRect.width,
                       &gV4LTestConfig.mCropRect.height);
                       
                if(gV4LTestConfig.mCropRect.left < 0)
                {
                        tst_resm(TBROK, "Invalid left corner for -B (Cropping): %d", gV4LTestConfig.mCropRect.left);
                        return TFAIL;
                }
                        
                if(gV4LTestConfig.mCropRect.top < 0)
                {
                        tst_resm(TBROK, "Invalid top corner for -B (Cropping): %d", gV4LTestConfig.mCropRect.top);
                        return TFAIL;
                }
                        
                if(gV4LTestConfig.mCropRect.width < 0)
                {
                        tst_resm(TBROK, "Invalid width for -B (Cropping): %d", gV4LTestConfig.mCropRect.width);
                        return TFAIL;        
                }
                        
                if(gV4LTestConfig.mCropRect.height < 0)
                {
                        tst_resm(TBROK, "Invalid height for -B (Cropping): %d", gV4LTestConfig.mCropRect.height);
                        return TFAIL;
                } 
                        
                if(gV4LTestConfig.mCropRect.left + gV4LTestConfig.mCropRect.width > 640)
                {
                        tst_resm(TBROK, "Invalid right bound for -B (Cropping): left = %d and width = %d.It must be > 640", 
                                 gV4LTestConfig.mCropRect.left, gV4LTestConfig.mCropRect.width);
                        return TFAIL; 
                }
                                 
                if(gV4LTestConfig.mCropRect.top + gV4LTestConfig.mCropRect.height > 480)
                {
                        tst_resm(TBROK, "Invalid bottom bound for -B (Cropping): top = %d and height = %d.It must be > 480", 
                                 gV4LTestConfig.mCropRect.top, gV4LTestConfig.mCropRect.height);   
                        return TFAIL;             
                }                
        }
        
        if(Jflag)
        {
                sscanf(Jopt,
                        "%d,%d",
                        &gV4LTestConfig.mFrameRate,
                        &gV4LTestConfig.mDuration);
                if (gV4LTestConfig.mFrameRate > 30 || gV4LTestConfig.mFrameRate < 1)
                        gV4LTestConfig.mFrameRate = 20;
                        
                if (gV4LTestConfig.mDuration > 16 || gV4LTestConfig.mDuration < 1)
                        gV4LTestConfig.mDuration = 4;
        }
        else 
        {
                gV4LTestConfig.mFrameRate = 20;
                gV4LTestConfig.mDuration = 4;
        }
        
        
        if(oflag)
        {
                gV4LTestConfig.mOutputFile = malloc(strlen(oopt)*sizeof(char));
                strcpy(gV4LTestConfig.mOutputFile,oopt);
        }
        else
        {
                gV4LTestConfig.mOutputFile = malloc(9*sizeof(char));
                strcpy(gV4LTestConfig.mOutputFile,"./output");
        }
        
        if(vflag)
        {
                tst_resm(TINFO, "V4L2 device = %s", gV4LTestConfig.mV4LDevice);
                tst_resm(TINFO, "Output device = %s", gV4LTestConfig.mOutputDevice);
                tst_resm(TINFO, "Height = %d", gV4LTestConfig.mHeight);
                tst_resm(TINFO, "Width = %d", gV4LTestConfig.mWidth);
                tst_resm(TINFO, "Case number = %d", gV4LTestConfig.mCaseNum);
                tst_resm(TINFO, "Path to output file : %s", gV4LTestConfig.mOutputFile);
               
                if(Bflag)
                        tst_resm(TINFO, 
                                 "Crop to image: left = %d, top = %d, width = %d, height = %d", 
                                 gV4LTestConfig.mCropRect.left,
                                 gV4LTestConfig.mCropRect.top,
                                 gV4LTestConfig.mCropRect.width,
                                 gV4LTestConfig.mCropRect.height);
              
        }
        
        /* Resizing test */
        
        if((Sflag)&&(gV4LTestConfig.mCaseNum == 1))
        {
                
                /* Print test Assertion using tst_resm() function with argument TINFO. */

                tst_resm(TINFO, "Testing if %s test case is OK", TCID);

                int i;
        
                for(i = 0; i < 3; i++)
                {
                        tst_resm(TINFO,"Performing resizing to the %s\n", userOutput[i]);
                        gV4LTestConfig.mHeight = heightTable[i];
                        gV4LTestConfig.mWidth  = widthTable[i];
                                                                                                        
                        /* Perform global test setup, call setup() function. */

                        setup();

                        /* Test Case Body. */

                        ret |= VT_v4l_capture_test();
                        
                        gExitCleanup = 0;
                        
                        cleanup();
                }
                if(ret == TPASS)
                        tst_resm(TPASS, "%s test case worked as expected", TCID);
                else
                        tst_resm(TFAIL, "%s test case did NOT work as expected", TCID);
                                
                return VT_rv;
        }
        else
                if((Sflag)&&(gV4LTestConfig.mCaseNum != 1))
                {
                        tst_resm(TBROK, "Resizing not supported while capturing");
                        return TFAIL;                             
                }
        
            
        /* Perform global test setup, call setup() function. */
        setup();
                
        /* Print test Assertion using tst_resm() function with argument TINFO. */
        tst_resm(TINFO, "Testing if %s test case is OK", TCID);
                        
        /* Test Case Body. */
        VT_rv = VT_v4l_capture_test();
        
        if(Eflag)   /* threat error as PASS */
        {
                if(VT_rv == TFAIL)                        
                        VT_rv = TPASS;
                else
                        VT_rv = TFAIL;
        }   
                             
        if(VT_rv == TPASS)
                tst_resm(TPASS, "%s test case worked as expected", TCID);
        else
                tst_resm(TFAIL, "%s test case did NOT work as expected", TCID);
                                                                                
        cleanup();        

        return VT_rv;
        
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

void cleanup(void)
{
        int VT_rv;
        
        if(gV4LTestConfig.mOutputFile)
                free(gV4LTestConfig.mOutputFile);
        
        gV4LTestConfig.mOutputFile = NULL;
          
        VT_rv = VT_v4l_capture_cleanup();
        
        if(gExitCleanup)
        {
                if (VT_rv != TPASS)
                {
                        tst_resm(TWARN, "VT_cleanup() Failed : error code = %d", VT_rv);
                }
        
                tst_exit(VT_rv);
        }
}       

#ifdef __cplusplus
}
#endif
