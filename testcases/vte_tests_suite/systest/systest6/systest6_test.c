/*==================================================================================================

        Copyright (C) 2007, Freescale Semiconductor, Inc. All Rights Reserved
        THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
        BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
        Freescale Semiconductor, Inc.

====================================================================================================
Revision History:
                            Modification     Tracking
Author                          Date          Number    Description of Changes
-------------------------   ------------    ----------  -------------------------------------------
D.Simakov                    06/06/2007      ENGR37682   Initial version
====================================================================================================
Portability: ARM GCC

==================================================================================================*/

/*==================================================================================================
                                        INCLUDE FILES
==================================================================================================*/

/* Standard Include Files */
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <fcntl.h>
#include <assert.h>
#include <pthread.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/mount.h>
#include <string.h>

// V4L2
#include <asm/types.h>
//#include <linux/compiler.h>
#include <linux/videodev2.h>


/* Verification Test Environment Include Files */
#include "systest6_test.h"

/*==================================================================================================
                                        LOCAL MACROS
==================================================================================================*/

#define DM__() do{printf("%s:%d %s()\n", __FILE__, __LINE__, __FUNCTION__); fflush(stdout);}while(0)

/*****************/
/* Debug output. */
/*****************/
#ifdef DEBUG_TEST
    #define DPRINTF(fmt,...) do {printf((fmt), ##__VA_ARGS__); fflush(stdout);}while(0)
#else
    #define DPRINTF(fmt,...) do {} while(0)
#endif

#define CLOSE_DEV(fd) do{if(fd!=-1){close(fd);fd=-1;}}while(0)

#define MAX_THREADS 5

#define FS_FILE_NAME             "rand_file"
#define FS_FILE_SZ               2096
#define FS_MV_ITERATIONS         100

// V4L stuff
#define MAX_V4L_BUFFERS          3

/*==================================================================================================
                          LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
==================================================================================================*/

typedef struct 
{               
        pthread_t mThreadID;  // Thread ID
        size_t    mIndex;     // Thread index
        int       mLtpRetval; // Thread's LTP return status       
        int       mErrCount;
} sThreadContext;

/* V4L Buffer. */
typedef struct
{
        void * mpStart;
        size_t mSz;
        size_t mOffset;
} sV4LBuffer;
                

typedef void* (*tThreadProc)(void*);

/*==================================================================================================
                                       LOCAL VARIABLES
==================================================================================================*/

// Common stuff.
static tThreadProc        gThreadProc[MAX_THREADS] = {0};
static sThreadContext     gThreadContext[MAX_THREADS];
static int                gNumThreads = 0; 
static pthread_mutex_t    gMutex = PTHREAD_MUTEX_INITIALIZER;
static int                gStopAllThreads = FALSE;

// V4L stuff
static int                                    gV4LOutputDevice = -1;
static struct             v4l2_format         gV4Lfmt;
static struct             v4l2_cropcap        gV4Lcropcap;
static struct             v4l2_crop           gV4Lcrop;
static struct             v4l2_buffer         gV4Lbuf;
static struct             v4l2_requestbuffers gV4LbufReq;
static                    sV4LBuffer          gV4LBuffers[MAX_V4L_BUFFERS];

static unsigned char      gFileData[FS_FILE_SZ];

/*==================================================================================================
                                       GLOBAL VARIABLES
==================================================================================================*/

extern sTestappConfig gTestappConfig;


/*==================================================================================================
                                    FUNCTION PROTOTYPES
==================================================================================================*/

void* Thread1( void * pContext ); // Video playback
void* Thread2( void * pContext ); // USB HS transfer 

int RunTest           ( void );
int VerificateResults ( void );
int CheckFile         ( const char * fname, int sz );
int pixel_format      (char *string);
int parse_file        (int in_fd, int *width, int *height, char **image_fmt);
int SetupFFS          ( const char * dev, const char * mntdir );
int CloseFFS          ( const char * mntdir );
int BitMatching       ( char * buf1, char * buf2, int count );
int MoveFile          (const char * from, const char * to);
/*==================================================================================================
                                         FUNCTIONS
==================================================================================================*/


/*================================================================================================*/
/*================================================================================================*/
void* Thread1( void * pContext )
{
        sThreadContext * pCtx = (sThreadContext*)pContext;
        assert(pContext);

        int frame_size, err;
        int g_frame_period = 33333;
        struct v4l2_buffer buffer;
        struct timeval tv_start;
        char *buf;
        enum v4l2_buf_type typeBuffer = V4L2_BUF_TYPE_VIDEO_OUTPUT;        

        frame_size = gV4Lfmt.fmt.pix.sizeimage;

        buf = (char*)malloc(frame_size);
        if (!buf)
        {
                tst_resm(TFAIL, "%s(): malloc(buf) failed.", __FUNCTION__);
                pCtx->mLtpRetval = TFAIL;
                pCtx->mErrCount++;
                return 0;
        }
        memset(buf, 0, frame_size);

        gettimeofday(&tv_start, 0);
        
        lseek(gTestappConfig.mVideoFileDesc, RAW_DATA_OFFSET, SEEK_SET);
        // ...
        int framesNo;
        for (framesNo = 0; !pCtx->mErrCount; ++framesNo)
        {
//                DPRINTF("----frame No %d\n", framesNo);
                memset(&buffer, 0, sizeof buffer);
                buffer.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
                buffer.memory = V4L2_MEMORY_MMAP;

                buffer.timestamp.tv_sec = tv_start.tv_sec;
                buffer.timestamp.tv_usec = tv_start.tv_usec + g_frame_period * framesNo;

                err = read(gTestappConfig.mVideoFileDesc, buf, frame_size);
                if (err < 0)
                {
                        tst_resm(TFAIL, "%s(): read(%s) failed. Reason: %s", __FUNCTION__, gTestappConfig.mVideoFileDesc, strerror(errno));
                        pCtx->mLtpRetval = TFAIL;
                        pCtx->mErrCount++;
                        break;
                }
                if ( err < frame_size )
                {
                        tst_resm(TINFO, "%d incomplete frame", framesNo);
                        break;
                }

                        
                if (framesNo == 1)
                {
                        tst_resm(TINFO, "V4L starting output...");
                        if (ioctl(gV4LOutputDevice, VIDIOC_STREAMON, &typeBuffer) < 0)
                        {
                                tst_resm(TFAIL, "%s(): output ioctl(VIDIOC_STREAMON) failed. Reason: %s", __FUNCTION__, strerror(errno));
                                pCtx->mLtpRetval = TFAIL;
                                pCtx->mErrCount++;
                        }
                        tst_resm(TINFO, "V4L output is started");
                }
                if ( framesNo < gV4LbufReq.count )
                {
                        buffer.index = framesNo;
                        if (ioctl(gV4LOutputDevice, VIDIOC_QUERYBUF, &buffer) < 0)
                        {
                                                tst_resm(TWARN, "%s(): output ioctl(VIDIOC_QUERYBUF) failed: %s", __FUNCTION__, strerror(errno));
                                                pCtx->mLtpRetval = TFAIL;
                                                pCtx->mErrCount++;
                        }
                }
                else
                {
                        if (ioctl(gV4LOutputDevice, VIDIOC_DQBUF, &buffer) < 0)
                        {
                                if (errno != EAGAIN)
                                {
                                        tst_resm(TWARN, "%s(): output ioctl(VIDIOC_DQBUF) failed: %s", __FUNCTION__, strerror(errno));
                                        pCtx->mLtpRetval = TFAIL;
                                        pCtx->mErrCount++;
                                }
                                else
                                {
        				if (gTestappConfig.mVerbose)
        					tst_resm(TWARN, "%s(): output ioctl(VIDIOC_DQBUF) failed: %s", __FUNCTION__, strerror(errno));
                                }
                        }
                }

                memcpy(gV4LBuffers[buffer.index].mpStart, buf, frame_size);
                if (ioctl(gV4LOutputDevice, VIDIOC_QBUF, &buffer) < 0)
                {
                        tst_resm(TFAIL, "%s(): output ioctl(VIDIOC_QBUF) failed: %s", __FUNCTION__, strerror(errno));
                        pCtx->mLtpRetval = TFAIL;
                        pCtx->mErrCount++;
                        break;
                }
        }

        if (ioctl(gV4LOutputDevice, VIDIOC_STREAMOFF, &typeBuffer) < 0)
        {
                tst_resm(TFAIL, "%s(): output ioctl(VIDIOC_STREAMOFF) failed: %s", __FUNCTION__, strerror(errno));
                pCtx->mLtpRetval = TFAIL;
                pCtx->mErrCount++;
        }
        else
        if (gTestappConfig.mVerbose)
                tst_resm(TINFO, "Stream output stopped ...ok");
                
        if (gTestappConfig.mVerbose)
                tst_resm(TINFO, "Video playback stopped. Errors Count: %d", pCtx->mErrCount);

        pthread_mutex_lock(&gMutex);
        gStopAllThreads = TRUE;
        pthread_mutex_unlock(&gMutex);
        
        return 0;                
}


/*================================================================================================*/
/*================================================================================================*/
void* Thread2( void * pContext )
{
        sThreadContext * pCtx = (sThreadContext*)pContext;
        assert(pCtx);

        if (gTestappConfig.mVerbose)
                tst_resm(TINFO, "Transfer to/from flash card (over USB HS interface) started");

        
        int toSd = TRUE;
        char cmd[MAX_STR_LEN];
        char fname[MAX_STR_LEN];
        unsigned char refBuffer[FS_FILE_SZ];
        int i;
        for (i=0; i<FS_MV_ITERATIONS; ++i)
        {        
                if (toSd)
                        snprintf(cmd, MAX_STR_LEN, "mv -f %s %s/", FS_FILE_NAME, 
                                 gTestappConfig.mMountPoint);
                                                    
                else
                        snprintf(cmd, MAX_STR_LEN, "mv -f %s/%s .", gTestappConfig.mMountPoint, 
                                 FS_FILE_NAME);
                
                if (-1 == system(cmd))
                {
                        tst_resm(TFAIL, "%s(): system(%s) failed. Reason: %s", __FUNCTION__, cmd, strerror(errno));
                        pCtx->mErrCount++;
                        pCtx->mLtpRetval = TFAIL;                                                
                }
                else
                {               
                        // Verification                        
                        if (toSd)                                
                                snprintf(fname, MAX_STR_LEN, "%s/%s", gTestappConfig.mMountPoint, FS_FILE_NAME);                                                                        
                        else
                                snprintf(fname, MAX_STR_LEN, "%s", FS_FILE_NAME);
                 
                        if (!CheckFile(fname, FS_FILE_SZ))
                        {
                                tst_resm(TFAIL, "%s(): CheckFile(%s) failed. Reason: file not found or it has a wrong size",
                                         __FUNCTION__, fname);                                
                                pCtx->mErrCount++;
                                pCtx->mLtpRetval = TFAIL;                                                                                                                                 
                        }                        
                        else
                        {
                                FILE * in = fopen(fname, "rb");
                                if (in)
                                {
                                        if (1 != fread(refBuffer, FS_FILE_SZ, 1, in) )
                                        {
                                                tst_resm(TFAIL, "%s(): fread(%s) failed. Reason: %s",
                                                         __FUNCTION__, fname, strerror(errno));                                
                                                pCtx->mErrCount++;
                                                pCtx->mLtpRetval = TFAIL;                                                                                                                                                                                                                         
                                        } 
                                        else
                                        {
                                                if (!BitMatching((char*)gFileData, (char*)refBuffer, FS_FILE_SZ))
                                                {
                                                        tst_resm(TFAIL, "%s(): BitMatching(%s) failed",
                                                                 __FUNCTION__, fname);                                
                                                        pCtx->mErrCount++;
                                                        pCtx->mLtpRetval = TFAIL;                                                                                                                                 
                                                }
                                        }
                                        fclose(in);
                                }
                                else
                                {
                                        tst_resm(TFAIL, "%s(): fopen(%s) failed. Reason: %s",
                                                 __FUNCTION__, fname, strerror(errno));                                
                                        pCtx->mErrCount++;
                                        pCtx->mLtpRetval = TFAIL;                                                                                                                                 
                                }                        
                        }                              
                        toSd = toSd ? FALSE : TRUE;        
                } // for (;;)                
        }
#if 0        
        pthread_mutex_lock(&gMutex);
        gStopAllThreads = TRUE;
        pthread_mutex_unlock(&gMutex);
#endif
        
        if (gTestappConfig.mVerbose)
                tst_resm(TINFO, "Transfer to/from flash card (over USB HS interface) stopped. Errors Count: %d", pCtx->mErrCount);
        
        return 0;
}


/*================================================================================================*/
/*================================================================================================*/
int RunTest( void )
{
        int i;
        int rv = TPASS;    
        sThreadContext * pContext;        

        for (i = 0; i < gNumThreads; ++i)
        {        
                if (gTestappConfig.mThreadToExecute != -1 && gTestappConfig.mThreadToExecute != i)
                        continue;
                pContext = gThreadContext + i;
                memset( pContext, 0, sizeof(sThreadContext) );
                pContext->mIndex = i;
                
        
                if( pthread_create( &pContext->mThreadID, NULL, (void* (*)(void*))(gThreadProc[i]), pContext ) )
                {
                        tst_resm( TWARN, "%s : error creating thread %d", __FUNCTION__, i );
                        return TFAIL;            
                }
        }    
    
        /* Wait for the each thread. */
        for( i = 0; i < gNumThreads; ++i )
        {
                if (gTestappConfig.mThreadToExecute != -1 && gTestappConfig.mThreadToExecute != i)
                        continue;    
                pContext = gThreadContext + i;     
                pthread_join( pContext->mThreadID, NULL );
        }
        for( i = 0; i < gNumThreads; ++i )
        {
                if (gTestappConfig.mThreadToExecute != -1 && gTestappConfig.mThreadToExecute != i)
                        continue;    
                pContext = gThreadContext + i;     
                rv += pContext->mLtpRetval;
        }
    
        if( TPASS == rv )
                rv = VerificateResults();                     

        return rv;
}


/*================================================================================================*/
/*================================================================================================*/
int VerificateResults( void )
{        
        if (gTestappConfig.mVerbose)
                tst_resm(TINFO, "Verification passed");
        
        return TPASS;
}


/*================================================================================================*/
/*================================================================================================*/
int VT_systest_setup( void )
{
        gNumThreads = 2;
        gThreadProc[0] = Thread1;
        gThreadProc[1] = Thread2;

        
        /************************************************
         * Setup V4L. 
         ************************************************/
        
        int output = 3, i;

        char *cpixformat;

        gTestappConfig.mVideoFileDesc = open(gTestappConfig.mVideoFileName, O_RDONLY);
        if (gTestappConfig.mVideoFileDesc == -1)
        {
                tst_resm(TFAIL, "%s() : open(%s) failed. Reason: %s", __FUNCTION__, gTestappConfig.mVideoFileName,
                          strerror(errno));
                return TFAIL;
        }
        
        if ( (gV4LOutputDevice = open(gTestappConfig.mV4LDevName, O_RDWR)) == -1)
        {
                tst_resm(TFAIL, "%s() : open(%s) failed. Reason: %s", __FUNCTION__, gTestappConfig.mV4LDevName,
                          strerror(errno));
                return TFAIL;                          
        }
       
        // Check the capabilities.
        struct v4l2_capability cap;
        if (ioctl(gV4LOutputDevice, VIDIOC_QUERYCAP, &cap) < 0)
        {
                tst_resm(TFAIL, "%s(): ioctl(VIDIOC_QUERYCAP) failed", __FUNCTION__);
                return TFAIL;
        }
        if (!(cap.capabilities & V4L2_CAP_VIDEO_OUTPUT))
        {
                tst_resm(TFAIL, "%s(): %s does not support output", __FUNCTION__, gTestappConfig.mV4LDevName);
                return TFAIL;
        }
        if (!(cap.capabilities & V4L2_CAP_STREAMING))
        {
                tst_resm(TWARN, "%s(): %s does not support streaming I/O", __FUNCTION__, gTestappConfig.mV4LDevName);
                return TFAIL;
        }
 
        if (ioctl(gV4LOutputDevice, VIDIOC_S_OUTPUT, &output) < 0)
        {
                tst_resm(TFAIL, "%s() : ioctl(VIDIOC_S_OUTPUT) failed. Reason: %s", __FUNCTION__, strerror(errno));        
                return TFAIL;
        }
      
        memset(&gV4Lcropcap, 0, sizeof(gV4Lcropcap));
        gV4Lcropcap.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
        if ((ioctl(gV4LOutputDevice, VIDIOC_CROPCAP, &gV4Lcropcap)) < 0)
        {
                tst_resm(TFAIL, "%s() : ioctl(VIDIOC_CROPCAP) failed. Reason: %s", __FUNCTION__, strerror(errno));
                return TFAIL;                                
        }        
        if (gV4Lcropcap.type != V4L2_BUF_TYPE_VIDEO_OUTPUT)
        {
                tst_resm(TFAIL, "%s() : Error: cropcap is not V4L2_BUF_TYPE_VIDEO_OUTPUT type");
                return TFAIL;
        }
       
        if (parse_file(gTestappConfig.mVideoFileDesc, &gTestappConfig.mWidth, &gTestappConfig.mHeight, &cpixformat))
        {
                tst_resm(TFAIL, "%s() : parse_file failed.", __FUNCTION__);
                return TFAIL;
        }
         
        memset(&gV4Lcrop, 0, sizeof gV4Lcrop);
        gV4Lcrop.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
        gV4Lcrop.c.top = 0;
        gV4Lcrop.c.left = 0;
        gV4Lcrop.c.width = gTestappConfig.mWidth;
        gV4Lcrop.c.height = gTestappConfig.mHeight;

        /* ignore if cropping is not supported (EINVAL) */
        if (ioctl(gV4LOutputDevice, VIDIOC_S_CROP, &gV4Lcrop) < 0 && errno != EINVAL)
        {
                tst_resm(TFAIL, "%s() : ioctl(VIDIOC_S_CROP) failed. Reason: %s", __FUNCTION__, strerror(errno));
                return TFAIL;                                
        }
        
        memset(&gV4Lfmt, 0, sizeof gV4Lfmt);
        gV4Lfmt.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;        
        gV4Lfmt.fmt.pix.width = gTestappConfig.mWidth;
        gV4Lfmt.fmt.pix.height = gTestappConfig.mHeight;
        gV4Lfmt.fmt.pix.pixelformat = pixel_format(cpixformat);
        if (ioctl(gV4LOutputDevice, VIDIOC_S_FMT, &gV4Lfmt) < 0)
        {
                tst_resm(TFAIL, "%s() : ioctl(VIDIOC_S_FMT) failed. Reason: %s", __FUNCTION__, strerror(errno));
                return TFAIL;                                        
        }
        
        memset(&gV4LbufReq, 0, sizeof(gV4LbufReq));
        gV4LbufReq.count = 3;
        gV4LbufReq.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
        gV4LbufReq.memory = V4L2_MEMORY_MMAP;
        if (ioctl(gV4LOutputDevice, VIDIOC_REQBUFS, &gV4LbufReq) < 0)
        {
                tst_resm(TFAIL, "%s() : ioctl(VIDIOC_S_FMT) failed. Reason: %s", __FUNCTION__, strerror(errno));
                return TFAIL;                                                                
        }
       
        for (i = 0; i < gV4LbufReq.count; i++)
        {
                memset(&gV4Lbuf, 0, sizeof(gV4Lbuf));
                gV4Lbuf.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
                gV4Lbuf.memory = V4L2_MEMORY_MMAP;
                gV4Lbuf.index = i;
                if ((ioctl(gV4LOutputDevice, VIDIOC_QUERYBUF, &gV4Lbuf)) < 0)
                {
                        tst_resm(TFAIL, "%s() : ioctl(VIDIOC_S_FMT) failed. Reason: %s", __FUNCTION__, strerror(errno));
                        return TFAIL;                                                                                
                }
                gV4LBuffers[i].mSz = gV4Lbuf.length;
                gV4LBuffers[i].mpStart = mmap(NULL, gV4Lbuf.length,
                                        PROT_READ | PROT_WRITE, MAP_SHARED, gV4LOutputDevice, gV4Lbuf.m.offset);
                if (gV4LBuffers[i].mpStart == MAP_FAILED)
                {
                        tst_resm(TFAIL, "%s() : mmap(%s) failed. Reason: %s", __FUNCTION__, 
                                 gTestappConfig.mV4LDevName, strerror(errno));
                        return TFAIL;                                                     
                }
        }
                                        

        /************************************************
         * Init FFS. 
         ************************************************/

        if (!SetupFFS(gTestappConfig.mFFSDevName, gTestappConfig.mMountPoint))
        {
                return TFAIL;
        }
         
        if (gTestappConfig.mVerbose)
        {
                tst_resm(TINFO, "FFS init ... ok");
        }
        
        // Create a file
        unsigned char val;
        FILE * out = fopen(FS_FILE_NAME, "wb");
        if (out)
        {
                for (i=0; i < FS_FILE_SZ; ++i)
                {
                        val = (unsigned char)(rand()%256);
                        gFileData[i] = val;
                        if (EOF == fputc(val, out))                         
                        {
                                tst_resm(TBROK, "%s(): fputc(%s) failed. Reason: %s", __FUNCTION__, FS_FILE_NAME, strerror(errno));
                                fclose(out);
                                return TBROK; 
                        }
                }
        
                fclose(out);
        } 
        else
        {
                tst_resm(TBROK, "%s(): fopen(%s) failed. Reason: %s", __FUNCTION__, FS_FILE_NAME, strerror(errno));
                return TBROK;
        }        
        
        return TPASS;
}


/*================================================================================================*/
/*================================================================================================*/
int VT_systest_cleanup( void )
{       
        int rv = TPASS;
        int i;

        for (i = 0; i < gV4LbufReq.count; ++i)
        {
                if( munmap(gV4LBuffers[i].mpStart, gV4LBuffers[i].mSz) < 0)
                {
                        tst_resm(TFAIL, "%s(): munmap on %s failed", __FUNCTION__, gTestappConfig.mVideoFileName);
                        rv = TFAIL;
                }
        }
        if (gTestappConfig.mVerbose)
        {
                tst_resm(TINFO, "V4L output cleanup ... ok");
        }

        CLOSE_DEV(gV4LOutputDevice);
        
        CLOSE_DEV(gTestappConfig.mVideoFileDesc);

        /************************************************
         * Cleanup FFS. 
         ************************************************/

        CloseFFS(gTestappConfig.mMountPoint);
        if (gTestappConfig.mVerbose)
        {
                tst_resm(TINFO, "FFS cleanup ... ok");
        }                

        return rv;
}


/*================================================================================================*/
/*================================================================================================*/
int VT_systest_test( void )
{                          
        return RunTest();
}


/*================================================================================================*/
/*================================================================================================*/
int CheckFile( const char * fname, int sz )
{
        FILE * fstream = fopen( fname, "r" );
        if( fstream )
        {
                fseek( fstream, 0, SEEK_END );
                size_t fsz = ftell( fstream );
                fclose( fstream );
                if( sz >= 0 && sz != fsz )
                {
                        if( gTestappConfig.mVerbose )
                        {
                                tst_resm( TWARN, "%s(): %s has a wrong size (actual: %ld, correct: %d)", 
                                          __FUNCTION__, fname, fsz, sz );
                        }

                        return FALSE;
                }
                
                return TRUE;
        }

        // File is not found.
        if( gTestappConfig.mVerbose )
        {
                tst_resm( TWARN, "%s not found", fname );
        }

        return FALSE;
} 

int pixel_format(char *string)
{
        int     format = 0;

        if (!strcasecmp(string, "BGR24"))
        {
                format = V4L2_PIX_FMT_BGR24;
        }
        else if (!strcasecmp(string, "YUV420"))
        {
                format = V4L2_PIX_FMT_YUV420;
        }
        else if (!strcasecmp(string, "YUV422P"))
        {
                format = V4L2_PIX_FMT_YUV422P;
        }
        else if (!strcasecmp(string, "RGB565"))
        {
                format = V4L2_PIX_FMT_RGB565;
        }
        else if (!strcasecmp(string, "RGB24"))
        {
                format = V4L2_PIX_FMT_RGB24;
        }
        else if (!strcasecmp(string, "RGB32"))
        {
                format = V4L2_PIX_FMT_RGB32;
        }
        else if (!strcasecmp(string, "BGR32"))
        {
                format = V4L2_PIX_FMT_BGR32;
        }
        else
        {
                tst_resm(TBROK, "Unsupported file format");
        }
        return format;
}

int parse_file(int in_fd, int *width, int *height, char **image_fmt)
{
        dump_file_header hdr;

        lseek(in_fd, 0, SEEK_SET);

        if (read(in_fd, &hdr, sizeof(dump_file_header)) < 0)
                return -1;

        *image_fmt = strdup(hdr.image_fmt);
        *width = atoi(hdr.image_width);
        *height = atoi(hdr.image_height);

        lseek(in_fd, 0, SEEK_SET);

        return 0;
}

/*================================================================================================*/
/*================================================================================================*/
int SetupFFS( const char * dev, const char * mntdir )
{
        struct stat st;
        int res, err;

        assert(dev && mntdir);
                
        memset(&st, 0, sizeof(st));
        if (stat(dev, &st))
        {
                tst_resm(TFAIL, "%s(): stat(%s) failed. Reason: %s", __FUNCTION__, dev, strerror(errno));
                return FALSE;
        }
        else if (!S_ISCHR(st.st_mode) && !S_ISBLK(st.st_mode))
        {
                tst_resm(TFAIL, "%s(): %s is not a char or a block device", __FUNCTION__, dev);
                return FALSE;
        }
        
        memset(&st, 0, sizeof st);
        res =  stat(mntdir, &st);
        if (res < 0 || !S_ISDIR(st.st_mode))
        {
                if (mkdir(mntdir, 0x777) < 0)
                {
                        tst_resm(TFAIL, "%s(): Can't create dir %s. Reason: %s", __FUNCTION__, mntdir, strerror(errno));
                        return FALSE;
                }
        }

        char cmd[MAX_STR_LEN];
        snprintf(cmd, MAX_STR_LEN, "mount %s %s", dev, mntdir);
        if (system(cmd) == -1)
        {
                tst_resm(TFAIL, "%s(): mount(%s) failed. Reason: %s", __FUNCTION__, mntdir, strerror(err));
                return FALSE;
        }
        return TRUE;
}


/*================================================================================================*/
/*================================================================================================*/
int CloseFFS( const char * mntdir )
{
        if (gTestappConfig.mVerbose)
                tst_resm(TINFO, "Unmounting FFS, it may take a few minutes...");

        assert(mntdir);
        if (umount(mntdir) < 0)
        {
                tst_resm(TFAIL, "%s(): umount(%s) failed. Reason: %s", __FUNCTION__, mntdir, strerror(errno));                
                return FALSE;
        }
        
        return TRUE;
}


/*================================================================================================*/
/*================================================================================================*/
int BitMatching(char *buf1, char *buf2, int count)
{
        int     i;

        for (i = 0; i < count; i++)
        {
                if (buf1[i] != buf2[i])
                {
                        return FALSE;
                }
        }
        return TRUE;
}
