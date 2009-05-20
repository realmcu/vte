/*======================

        Copyright (C) 2007, Freescale Semiconductor, Inc. All Rights Reserved
        THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
        BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
        Freescale Semiconductor, Inc.

====================
Revision History:
                            Modification     Tracking
Author                          Date          Number    Description of Changes
-------------------------   ------------    ----------  -------------------------------------------
D.Kardakov                 13/06/2007        ENGR37681  Initial version
====================
Portability: ARM GCC

======================*/

/*======================
                                        INCLUDE FILES
======================*/

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


/* Verification Test Environment Include Files */
#include "systest5_test.h"

/*======================
                                        LOCAL MACROS
======================*/

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

// IPC stuff. (AP+BP communications)
#define IPC_CHANNEL3              "/dev/mxc_ipc/3"
#define IPC_BUF_SZ                8192
#define IPC_TRANSFER_SZ           8192
#define IPC_CRITICAL_ERRORS_COUNT 50

/*======================
                          LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
======================*/

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

/*======================
                                       LOCAL VARIABLES
======================*/

// Common stuff.
static tThreadProc        gThreadProc[MAX_THREADS] = {0};
static sThreadContext     gThreadContext[MAX_THREADS];
static int                gNumThreads = 0;
static pthread_mutex_t    gMutex = PTHREAD_MUTEX_INITIALIZER;
static int                gStopAllThreads = FALSE;


/*======================
                                       GLOBAL VARIABLES
======================*/

extern sTestappConfig gTestappConfig;


/*======================
                                    FUNCTION PROTOTYPES
======================*/

void* Thread1( void * pContext ); // AP: captures images and save the captured images onto FFS
void* Thread2( void * pContext ); // AP: Sahara crypt/decrypt smth

int RunTest           ( void );
int VerificateResults ( void );
int CheckFile         ( const char * fname, int sz );
int SetupFFS          ( const char * dev, const char * mntdir );
int CloseFFS          ( const char * mntdir );
int BitMatching       ( char * buf1, char * buf2, int count );

/*======================
                                         FUNCTIONS
======================*/


/*====================*/
/*====================*/
void* Thread1( void * pContext )
{
        sThreadContext * pCtx = (sThreadContext*)pContext;
        assert(pCtx);
        int ret = 0;

        tst_resm(TINFO, "vpu_teatapp is started.....");
        ret = system("./vpu_testapp -C loopbackconfig.txt");
        if (ret < 0)
        {
                pCtx->mErrCount++;
                tst_resm(TFAIL, "%s (): vpu_teatapp failed", __FUNCTION__);
        }

        tst_resm(TINFO, "vpu_teatapp is compliated. Errors count: %d", pCtx->mErrCount);

        return 0;
}


/*====================*/
/*====================*/

/*====================*/
/*====================*/
void* Thread2( void * pContext )
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


/*====================*/
/*====================*/
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


/*====================*/
/*====================*/
int VerificateResults( void )
{

        if (gTestappConfig.mVerbose)
                tst_resm(TINFO, "Verification passed");

        return TPASS;
}


/*====================*/
/*====================*/
int VT_systest_setup( void )
{
        gNumThreads = 2;
        gThreadProc[0] = Thread1;
        gThreadProc[1] = Thread2;
        return TPASS;
}

/*====================*/
/*====================*/
int VT_systest_cleanup( void )
{
        //int i;
        int rv = TPASS;
        return rv;
}


/*====================*/
/*====================*/
int VT_systest_test( void )
{
        return RunTest();
}


/*====================*/
/*====================*/
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


/*====================*/
/*====================*/
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


/*====================*/
/*====================*/
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


/*====================*/
/*====================*/
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



