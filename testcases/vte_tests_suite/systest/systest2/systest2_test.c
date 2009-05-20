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
D.Simakov                    10/04/2007      ENGR37676   Initial version
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
#include <string.h>

/* Verification Test Environment Include Files */
#include "systest2_test.h"

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

#define FS_FILE_NAME             "rand_file"
#define FS_FILE_SZ               2096
#define FS_MV_ITERATIONS         100

// IPC stuff. (AP+BP communications)
#define IPC_CHANNEL_SDMA         "/dev/mxc_ipc/3"
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

static unsigned char      gFileData[FS_FILE_SZ];

/*======================
                                       GLOBAL VARIABLES
======================*/

extern sTestappConfig gTestappConfig;


/*======================
                                    FUNCTION PROTOTYPES
======================*/

void* Thread1( void * pContext ); // AP + BP communication over SDMA
void* Thread2( void * pContext ); // FS transfer to/from SD/MMC card

int RunTest           ( void );
int VerificateResults ( void );
int CheckFile         ( const char * fname, int sz );
int SetupFFS          ( const char * dev, const char * mntdir );
int CloseFFS          ( const char * mntdir );
int BitMatching       ( char * buf1, char * buf2, int count );
int MoveFile          (const char * from, const char * to);
/*======================
                                         FUNCTIONS
======================*/


/*====================*/
/*====================*/
void* Thread1( void * pContext )
{
#ifndef IPC_EXCLUDE
        sThreadContext * pCtx = (sThreadContext*)pContext;
        assert(pCtx);

        if (gTestappConfig.mVerbose)
                tst_resm( TINFO, "AP + BP communication over SDMA started" );

        int     count, count1;
        int     fd = -1;
        int     i;
        char   *tmp_buf;

        char wbuf[IPC_BUF_SZ];
        char rbuf[IPC_BUF_SZ];

        if ((fd = open(IPC_CHANNEL_SDMA, O_RDWR)) == -1)
        {
                tst_resm(TFAIL, "%s(): Cant open %s. Reason: %s", __FUNCTION__, IPC_CHANNEL_SDMA, strerror(errno));
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
                                tst_resm(TFAIL, "%s(): write(%s) failed. Reason: %s", __FUNCTION__, IPC_CHANNEL_SDMA, strerror(errno));
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
                                                tst_resm(TFAIL, "%s(): read(%s) failed. Reason: %s", __FUNCTION__, IPC_CHANNEL_SDMA, strerror(errno));
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
                tst_resm(TINFO, "AP + BP communication over SDMA stoped. Errors Count: %d", pCtx->mErrCount);

#endif
        return 0;
}


/*====================*/
/*====================*/
void* Thread2( void * pContext )
{
        sThreadContext * pCtx = (sThreadContext*)pContext;
        assert(pCtx);

        if (gTestappConfig.mVerbose)
                tst_resm(TINFO, "Transfer to/from SD(MMC) card started");


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

        pthread_mutex_lock(&gMutex);
        gStopAllThreads = TRUE;
        pthread_mutex_unlock(&gMutex);

        if (gTestappConfig.mVerbose)
                tst_resm(TINFO, "Transfer to/from SD(MMC) card stopped. Errors Count: %d", pCtx->mErrCount);

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
        int i;
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


/*====================*/
/*====================*/
int VT_systest_cleanup( void )
{
        int rv = TPASS;

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




