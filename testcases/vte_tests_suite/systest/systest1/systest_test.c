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
D.Simakov                    05/04/2007      ENGR37674  Initial version
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
#include <fcntl.h>
#include <assert.h>
#include <pthread.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/mount.h>


// Sahara
#include <fsl_shw.h> 
#include <sahara_module.h>

// V4L2
#include <asm/types.h>          /* for videodev2.h */
//#include <linux/compiler.h>     /* for videodev2.h */
#include <linux/videodev2.h>


/* Verification Test Environment Include Files */
#include "systest_test.h"

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

// V4L stuff
#define CAPTURE_PIXEL_FMT  V4L2_PIX_FMT_YUV420
#define CAPTURE_IMG_WIDTH  200
#define CAPTURE_IMG_HEIGHT 320
#define FRAMES_TO_CAPTURE  20

// SAHARA stuff
#define SAHARA_DEV "/dev/sahara_test"
#define SAHARA_POOL_SIZE 10

// IPC stuff. (AP+BP communications)
#define IPC_CHANNEL3              "/dev/mxc_ipc/3"
#define IPC_BUF_SZ                8192
#define IPC_TRANSFER_SZ           8192
#define IPC_CRITICAL_ERRORS_COUNT 50

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


// V4L2 stuff.
static sV4LBuffer *       gpV4LBuf = 0;
static unsigned int       gV4LBufNum = 0;
static struct v4l2_format gV4LFormat;  
static int                gV4LDev = -1;

// SAHARA stuff.
static int                gSaharaDev = -1;  


/*==================================================================================================
                                       GLOBAL VARIABLES
==================================================================================================*/

extern sTestappConfig gTestappConfig;


/*==================================================================================================
                                    FUNCTION PROTOTYPES
==================================================================================================*/

void* Thread1( void * pContext ); // AP: captures images and save the captured images onto FFS 
void* Thread2( void * pContext ); // AP: Sahara crypt/decrypt smth
void* Thread3( void * pContext ); // BP: running a memory-not-friendly code. ?howto?

int RunTest           ( void );
int VerificateResults ( void );
int CheckFile         ( const char * fname, int sz );
int SetupFFS          ( const char * dev, const char * mntdir );
int CloseFFS          ( const char * mntdir );
int BitMatching       ( char * buf1, char * buf2, int count );

/*==================================================================================================
                                         FUNCTIONS
==================================================================================================*/


/*================================================================================================*/
/*================================================================================================*/
void* Thread1( void * pContext )
{
        sThreadContext * pCtx = (sThreadContext*)pContext;
        assert(pContext);

        int i;
        struct v4l2_buffer buffer;  
        enum v4l2_buf_type typeBuffer = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        for (i = 0; i < gV4LBufNum; ++i)
        {
                memset(&buffer, 0, sizeof(buffer));      
                buffer.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;     
                buffer.memory = V4L2_MEMORY_MMAP;      
                buffer.index = i;
                if (ioctl(gV4LDev, VIDIOC_QUERYBUF, &buffer) < 0)
                {
                        tst_resm(TWARN, "%s(): ioctl(VIDIOC_QUERYBUF) failed", __FUNCTION__);
                        pCtx->mLtpRetval = TFAIL;
                        pCtx->mErrCount++;                        
                }
                if (ioctl(gV4LDev, VIDIOC_QBUF, &buffer) < 0)
                { 
                        tst_resm(TFAIL,"%s(): ioctl(VIDIOC_QBUF) failed", __FUNCTION__);
                        pCtx->mLtpRetval = TFAIL;
                        pCtx->mErrCount++;
                }          
        }                
        if (ioctl(gV4LDev, VIDIOC_STREAMON, &typeBuffer) < 0)
        {
                tst_resm(TFAIL, "%s(): ioctl(VIDIOC_STREAMON) failed. Reason: %s", __FUNCTION__, strerror(errno));
                pCtx->mLtpRetval = TFAIL;
                pCtx->mErrCount++;
        }       
        
        tst_resm(TINFO, "V4L capturing is started");
                                        
        // ...
        int framesNo = 0;
        char fname[MAX_STR_LEN];
        while (framesNo <= FRAMES_TO_CAPTURE && !pCtx->mErrCount)
        {
                buffer.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;      
                buffer.memory = V4L2_MEMORY_MMAP;                         
                if (ioctl(gV4LDev, VIDIOC_DQBUF, &buffer) < 0)
                {
                        tst_resm(TFAIL, "%s(): ioctl(VIDIOC_DQBUF) failed", __FUNCTION__);                        
                        pCtx->mLtpRetval = TFAIL;
                        pCtx->mErrCount++;
                        break;
                }
                
                if (buffer.index >= gV4LBufNum)      
                {        
                        tst_resm(TFAIL, "%s(): ioctl(VIDIOC_DQBUF) - Invalid buffer index", __FUNCTION__);        
                        pCtx->mLtpRetval = TFAIL;
                        pCtx->mErrCount++;                                                      
                }     
                else
                {                                
                        sprintf(fname, "%s/f%d.out", gTestappConfig.mMountPoint, framesNo);
                        FILE * out = fopen(fname, "wb");
                        if (out)
                        {
                                if (fwrite(gpV4LBuf[buffer.index].mpStart, gpV4LBuf[buffer.index].mSz, 1, out) != 1)
                                {
                                        tst_resm(TFAIL, "%s(): fwrite(%s) failed. Reason: %s", __FUNCTION__, fname, strerror(errno));
                                        pCtx->mLtpRetval = TFAIL;
                                        pCtx->mErrCount++;
                                }
                                fclose(out);
                        }
                        else
                        {
                                tst_resm(TFAIL, "%s(): Can't open %s. Reason: %s", __FUNCTION__, fname, strerror(errno));
                                pCtx->mLtpRetval = TFAIL;
                                pCtx->mErrCount++;
                        }
                }
                
                if (ioctl(gV4LDev, VIDIOC_QBUF, &buffer) < 0)
                {       
                        tst_resm(TFAIL, "%s(): ioctl(VIDIOC_QBUF) failed", __FUNCTION__);                        
                        pCtx->mLtpRetval = TFAIL;
                        pCtx->mErrCount++;
                        break;
                }  
                
                ++framesNo;
        } 
                
        typeBuffer = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        if (ioctl(gV4LDev, VIDIOC_STREAMOFF, &typeBuffer) < 0)
        {            
                tst_resm(TFAIL, "%s(): ioctl(VIDIOC_STREAMOFF) failed", __FUNCTION__);
                pCtx->mLtpRetval = TFAIL;
                pCtx->mErrCount++;                                
        }
        
        tst_resm( TINFO, "V4L capturing is stopped. Errors count: %d", pCtx->mErrCount );
        
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
                
        static fsl_shw_uco_t ctx;
        ctx.pool_size     = SAHARA_POOL_SIZE;
        ctx.flags         = FSL_UCO_BLOCKING_MODE;
        ctx.sahara_openfd = -1;
        ctx.mem_util      = NULL;
        ctx.callback      = NULL;
        
        pCtx->mLtpRetval = TPASS;
        
        
        if (gTestappConfig.mVerbose)
                tst_resm( TINFO, "SAHARA is started" );

        int saharaOp = 0;                
        for(;;)
        {
                if (ioctl(gSaharaDev, SAHARA_TEST_REGISTER_USER, &ctx) != 0)
                {
                        pCtx->mLtpRetval = TFAIL;
                        pCtx->mErrCount++;
                        tst_resm(TFAIL, "%s() : SAHARA_TEST_REGISTER_USER failed", __FUNCTION__);
                }
                else
                {
                        if (ioctl(gSaharaDev, saharaOp, &ctx) != 0)
                        {
                                pCtx->mLtpRetval = TFAIL;
                                pCtx->mErrCount++;
                                tst_resm(TFAIL, "%s() : SAHARA operation failed", __FUNCTION__);                                        
                        }
        
                        if (ioctl(gSaharaDev, SAHARA_TEST_DEREGISTER_USER, &ctx) != 0)
                        {
                                pCtx->mLtpRetval = TFAIL;
                                pCtx->mErrCount++;
                                tst_resm(TFAIL, "%s() : SAHARA_TEST_DEREGISTER_USER failed", __FUNCTION__);
                        }                                                                                                
                }
                saharaOp++;
                saharaOp %= 10;
                pthread_mutex_lock(&gMutex);
                if (gStopAllThreads) 
                        break; 
                pthread_mutex_unlock(&gMutex);
        }
        if (gTestappConfig.mVerbose)        
                tst_resm( TINFO, "SAHARA is stopped. Errors count: %d", pCtx->mErrCount );
        return 0;
}


/*================================================================================================*/
/*================================================================================================*/
void* Thread3( void * pContext )
{
#ifndef IPC_EXCLUDE
        sThreadContext * pCtx = (sThreadContext*)pContext;
        assert(pCtx);

        if (gTestappConfig.mVerbose)
                tst_resm( TINFO, "AP + BP communication over IPC started" );
                
        int     count, count1;
        int     fd = -1;
        int     i;
        char   *tmp_buf;
        
        char wbuf[IPC_BUF_SZ];
        char rbuf[IPC_BUF_SZ];

        if ((fd = open(IPC_CHANNEL3, O_RDWR)) == -1)
        {
                tst_resm(TFAIL, "%s(): Cant open %s. Reason: %s", __FUNCTION__, IPC_CHANNEL3, strerror(errno));
                pCtx->mLtpRetval = TFAIL;
                pCtx->mErrCount++;
        }
        else
        {
                for (;;)
                {
                        for (i=0; i<IPC_BUF_SZ; ++i)
                                wbuf[i] = (char)(i % 256);
                        
                        if ((count = write(fd, wbuf, IPC_TRANSFER_SZ)) < 0)
                        {
                                tst_resm(TFAIL, "%s(): write(%s) failed. Reason: %s", __FUNCTION__, IPC_CHANNEL3, strerror(errno));
                                pCtx->mLtpRetval = TFAIL;
                                pCtx->mErrCount++;                                
                        }
                        else
                        {
                                tmp_buf = rbuf;
                                while (count > 0)
                                {
                                        if ((count1 = read(fd, tmp_buf, count)) < 0)
                                        {
                                                tst_resm(TFAIL, "%s(): read(%s) failed. Reason: %s", __FUNCTION__, IPC_CHANNEL3, strerror(errno));
                                                pCtx->mLtpRetval = TFAIL;
                                                pCtx->mErrCount++;
                                                break;
                                        }
                                        count -= count1;
                                        tmp_buf += count1;
                                }
                                if (!BitMatching(wbuf, rbuf, count))
                                {
                                        tst_resm(TFAIL, "%s(): BitMatching failed.", __FUNCTION__);
                                        pCtx->mLtpRetval = TFAIL;
                                        pCtx->mErrCount++;
                                }
                        }                                
                        
                        // Critical erros count
                        if (pCtx->mErrCount > IPC_CRITICAL_ERRORS_COUNT)
                                break;
                        
                        pthread_mutex_lock(&gMutex);
                        if (gStopAllThreads) 
                                break; 
                        pthread_mutex_unlock(&gMutex);
                } // for(;;)
                close(fd);                
        }                

        if (gTestappConfig.mVerbose)
                tst_resm(TINFO, "AP + BP communication over IPC stoped. Errors Count: %d", pCtx->mErrCount);

#endif
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
        int i, sz = -1;      
        char fname[MAX_STR_LEN];
        for (i = 0; i < FRAMES_TO_CAPTURE; ++i)
        {
                sprintf(fname, "%s/f%d.out", gTestappConfig.mMountPoint, i);
                
                if (i == 0)
                {
                        FILE * f = fopen(fname, "r");
                        if(f) 
                        {
                                fseek(f, 0, SEEK_END);
                                sz = ftell(f);
                                fclose(f);
                        }
                }
                if (!CheckFile(fname, sz) )
                {
                        if (gTestappConfig.mVerbose)
                                tst_resm(TFAIL, "Verification failed");
                        return TFAIL;
                }                        
        }

        if (gTestappConfig.mVerbose)
                tst_resm(TINFO, "Verification passed");
        
        return TPASS;
}


/*================================================================================================*/
/*================================================================================================*/
int VT_systest_setup( void )
{
        gNumThreads = 3;
        gThreadProc[0] = Thread1;
        gThreadProc[1] = Thread2;
        gThreadProc[2] = Thread3;

        /************************************************
         * Init V4L.
         ************************************************/
        
        struct stat st;                        
        if (stat(gTestappConfig.mV4LDevName, &st) < 0)
        {
                tst_resm(TFAIL, "%s(): stat(%s) failed. Reason: %s.", __FUNCTION__, 
                         gTestappConfig.mV4LDevName, strerror(errno));
                return TFAIL;
        }
        if (!S_ISCHR(st.st_mode))
        {
                tst_resm(TFAIL, "%s(): %s is not a char device", __FUNCTION__, 
                         gTestappConfig.mV4LDevName );
                return TFAIL;
        }
        if ((gV4LDev = open(gTestappConfig.mV4LDevName, O_RDWR|O_NONBLOCK, 0)) < 0)
        {
                tst_resm(TFAIL, "%s(): Unable to open %s", __FUNCTION__, gTestappConfig.mV4LDevName);
                return TFAIL;
        }
        
        // Check the capabilities.
        struct v4l2_capability cap;                
        if (ioctl(gV4LDev, VIDIOC_QUERYCAP, &cap) < 0)
        {
                tst_resm(TFAIL, "%s(): ioctl(VIDIOC_QUERYCAP) failed", __FUNCTION__);
                return TFAIL;
        }
        if (!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE))
        {
                tst_resm(TFAIL, "%s(): %s does not support capturing", __FUNCTION__, gTestappConfig.mV4LDevName);
                return TFAIL;
        }
        if (!(cap.capabilities & V4L2_CAP_STREAMING))
        {      
                tst_resm(TWARN, "%s(): %s does not support streaming I/O", __FUNCTION__, gTestappConfig.mV4LDevName);      
                return TFAIL;    
        }
        
        // Init capture.
        memset(&gV4LFormat, 0, sizeof(gV4LFormat));
        gV4LFormat.type                = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        gV4LFormat.fmt.pix.width       = CAPTURE_IMG_WIDTH;
        gV4LFormat.fmt.pix.height      = CAPTURE_IMG_HEIGHT;
        gV4LFormat.fmt.pix.pixelformat = CAPTURE_PIXEL_FMT;
        if (ioctl(gV4LDev, VIDIOC_S_FMT, &gV4LFormat) < 0)
        {
                tst_resm(TFAIL, "%s(): ioctl(VIDIOC_S_FMT) failed", __FUNCTION__);
                return TFAIL;
        }
        
        // Init mmap.
        struct v4l2_requestbuffers reqBuffers;
        struct v4l2_buffer         buffer;
        memset(&reqBuffers, 0, sizeof(reqBuffers));
        reqBuffers.count = 3;  
        reqBuffers.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        reqBuffers.memory = V4L2_MEMORY_MMAP;
        if (ioctl(gV4LDev, VIDIOC_REQBUFS, &reqBuffers) < 0)
        {
                tst_resm(TFAIL, "%s(): ioctl(VIDIOC_REQBUFS) failed", __FUNCTION__);
                return TFAIL;
        }
        if (reqBuffers.count < 2)
        {
                tst_resm(TFAIL, "%s(): Insufficient buffer memory on %s", __FUNCTION__, gTestappConfig.mV4LDevName);
                return TFAIL;
        }
        if (!(gpV4LBuf = calloc(reqBuffers.count, sizeof(sV4LBuffer))))
        {
                tst_resm(TBROK, "%s(): Can't allocate memory for V4L buffers", __FUNCTION__);
                return TFAIL;
        }
        for (gV4LBufNum = 0; gV4LBufNum < reqBuffers.count; ++gV4LBufNum)
        {
                memset(&buffer, 0, sizeof(buffer));
                buffer.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
                buffer.memory = V4L2_MEMORY_MMAP;
                buffer.index = gV4LBufNum;    
                
                if (ioctl(gV4LDev, VIDIOC_QUERYBUF, &buffer) < 0)    
                {      
                        tst_resm(TFAIL, "%s(): ioctl(VIDIOC_QUERYBUF) failed", __FUNCTION__);      
                        return TFAIL;    
                } 
                gpV4LBuf[gV4LBufNum].mSz     = buffer.length;
                gpV4LBuf[gV4LBufNum].mpStart = mmap(NULL, buffer.length,
                                                    PROT_READ | PROT_WRITE, MAP_SHARED,
                                                    gV4LDev, buffer.m.offset);
                if (MAP_FAILED == gpV4LBuf[gV4LBufNum].mpStart)
                {
                        tst_resm(TFAIL, "%s(): mmap on %s failed", __FUNCTION__, gTestappConfig.mV4LDevName);
                        return TFAIL;
                }
        }                                                                                  
        if (gTestappConfig.mVerbose)
        {
                tst_resm(TINFO, "V4L capture init ... ok");
        }        
      

        /************************************************
         * Init SAHARA. 
         ************************************************/
         
        if( (gSaharaDev = open(SAHARA_DEV, O_RDWR)) < 0 )
        {
                tst_resm(TFAIL, "%s() : Can't open %s", __FUNCTION__, SAHARA_DEV);
                return TFAIL;        
        }
        if (gTestappConfig.mVerbose)
        {
                tst_resm(TINFO, "SAHARA init ... ok");
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
        

                
        return TPASS;
}

/*================================================================================================*/
/*================================================================================================*/
int VT_systest_cleanup( void )
{       
        int i;
        int rv = TPASS;

        /************************************************
         * Cleanup V4L. 
         ************************************************/
        CLOSE_DEV(gV4LDev);
         
        for (i = 0; i < gV4LBufNum; ++i)    
        {      
                if( munmap(gpV4LBuf[i].mpStart, gpV4LBuf[i].mSz) < 0)
                {
                        tst_resm(TFAIL, "%s(): munmap on %s failed", __FUNCTION__, gTestappConfig.mV4LDevName);
                        rv = TFAIL;
                }                        
        }
        if (gTestappConfig.mVerbose)
        {
                tst_resm(TINFO, "V4L capture cleanup ... ok");
        }      


        /************************************************
         * Cleanup FFS. 
         ************************************************/
        CloseFFS(gTestappConfig.mMountPoint);
        if (gTestappConfig.mVerbose)
        {
                tst_resm(TINFO, "FFS cleanup ... ok");
        }        


        /************************************************
         * Cleanup SAHARA. 
         ************************************************/
        CLOSE_DEV(gSaharaDev);
        if (gTestappConfig.mVerbose)
        {
                tst_resm(TINFO, "SAHARA cleanup ... ok");
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



