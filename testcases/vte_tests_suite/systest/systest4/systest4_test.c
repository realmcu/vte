/*======================

        Copyright (C) 2007, Freescale Semiconductor, Inc. All Rights Reserved
        THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
        BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
        Freescale Semiconductor, Inc.

====================
Revision History:
                            Modification     Tracking
Author/Core ID                  Date          Number     Description of Changes
-------------------------   ------------    -----------  -------------------------------------------
A.Ozerov/b00320              29/05/2007      ENGR37685   Initial version
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

/* Verification Test Environment Include Files */
#include "systest4_test.h"

/*======================
                                        LOCAL MACROS
======================*/
#define MAX_THREADS 5

/* MMC/SD environment */
#define MMC_SD_BLOCK_SIZE        0x200
#define MMC_SD_BLOCK_COUNT       0x100
#define MMC_SD_OFFSET            0x100

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
/* Common stuff */
static tThreadProc        gThreadProc[MAX_THREADS] = {0};
static sThreadContext     gThreadContext[MAX_THREADS];
static int                gNumThreads = 0;
static pthread_mutex_t    gMutex = PTHREAD_MUTEX_INITIALIZER;

/* MMC/SD stuff */
static int                gMMCDev0 = -1;
static int                gMMCDev1 = -1;

unsigned char             *Dev0_patten_buf = NULL,
                          *Dev1_patten_buf = NULL,
                          *Dev0_read_buf = NULL,
                          *Dev1_read_buf = NULL;

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

/*======================
                                         FUNCTIONS
======================*/
int read_mmc_device(int fd_device, unsigned long start_offset, unsigned long size_to_read, unsigned long bs,
                    unsigned char *read_buf)
{
        unsigned long bytes2read;

        if (lseek(fd_device, start_offset * bs, SEEK_SET) != (start_offset * bs))
        {
                tst_resm(TFAIL,
                         "read_mmc_device() Failed repositions the offset of the file descriptor");
                return TFAIL;
        }
        bytes2read = read(fd_device, read_buf, size_to_read * bs);
        if (bytes2read != (size_to_read * bs))
        {
                tst_resm(TFAIL, "read_mmc_device() Failed read from device");
                return TFAIL;
        }
        return TPASS;
}

/*====================*/
int write_mmc_device(int fd_device, unsigned long start_offset, unsigned long size_to_write, unsigned long bs,
                     unsigned char *write_buf)
{
        unsigned long bytes2write;

        if (lseek(fd_device, start_offset * bs, SEEK_SET) != (start_offset * bs))
        {
                tst_resm(TFAIL,
                         "write_mmc_device() Failed repositions the offset of the file descriptor");
                return TFAIL;
        }
        bytes2write = write(fd_device, write_buf, size_to_write * bs);
        if (bytes2write != (size_to_write * bs))
        {
                tst_resm(TFAIL, "write_mmc_device() Failed write to device");
                return TFAIL;
        }
        return TPASS;
}

/*====================*/
void* Thread1( void * pContext )
{
        unsigned long i;
        sThreadContext * pCtx = (sThreadContext*)pContext;
        assert(pCtx);

        pCtx->mLtpRetval = TPASS;

        for(i = 0; i < (MMC_SD_BLOCK_COUNT * MMC_SD_BLOCK_SIZE); i++)
                Dev0_patten_buf[i] = (unsigned char) (random() * 0xFF);

        if(write_mmc_device(gMMCDev0, MMC_SD_OFFSET, MMC_SD_BLOCK_COUNT, MMC_SD_BLOCK_SIZE, Dev0_patten_buf) == TFAIL)
        {
                pCtx->mLtpRetval = TFAIL;
                pCtx->mErrCount++;

                pthread_mutex_lock(&gMutex);
                tst_resm(TFAIL, "%s() : write_mmc_device() failed", __FUNCTION__);
                pthread_mutex_unlock(&gMutex);
        }

        if(read_mmc_device(gMMCDev0, MMC_SD_OFFSET, MMC_SD_BLOCK_COUNT, MMC_SD_BLOCK_SIZE, Dev0_read_buf) == TFAIL)
        {
                pCtx->mLtpRetval = TFAIL;
                pCtx->mErrCount++;

                pthread_mutex_lock(&gMutex);
                tst_resm(TFAIL, "%s() : read_mmc_device() failed", __FUNCTION__);
                pthread_mutex_unlock(&gMutex);
        }

        pthread_mutex_lock(&gMutex);
        tst_resm( TINFO, "Working with MMC/SD 0 is stopped. Errors count: %d", pCtx->mErrCount );
        pthread_mutex_unlock(&gMutex);

        return 0;
}

/*====================*/
void* Thread2( void * pContext )
{
        unsigned long i;
        sThreadContext * pCtx = (sThreadContext*)pContext;
        assert(pCtx);

        pCtx->mLtpRetval = TPASS;

        for(i = 0; i < (MMC_SD_BLOCK_COUNT * MMC_SD_BLOCK_SIZE); i++)
                Dev1_patten_buf[i] = (unsigned char) (random() * 0xFF);

        if(write_mmc_device(gMMCDev1, MMC_SD_OFFSET, MMC_SD_BLOCK_COUNT, MMC_SD_BLOCK_SIZE, Dev1_patten_buf) == TFAIL)
        {
                pCtx->mLtpRetval = TFAIL;
                pCtx->mErrCount++;

                pthread_mutex_lock(&gMutex);
                tst_resm(TFAIL, "%s() : write_mmc_device() failed", __FUNCTION__);
                pthread_mutex_unlock(&gMutex);
        }

        if(read_mmc_device(gMMCDev1, MMC_SD_OFFSET, MMC_SD_BLOCK_COUNT, MMC_SD_BLOCK_SIZE, Dev1_read_buf) == TFAIL)
        {
                pCtx->mLtpRetval = TFAIL;
                pCtx->mErrCount++;

                pthread_mutex_lock(&gMutex);
                tst_resm(TFAIL, "%s() : read_mmc_device() failed", __FUNCTION__);
                pthread_mutex_unlock(&gMutex);
        }

        pthread_mutex_lock(&gMutex);
        tst_resm( TINFO, "Working with MMC/SD 1 is stopped. Errors count: %d", pCtx->mErrCount );
        pthread_mutex_unlock(&gMutex);

        return 0;
}

/*====================*/
int RunTest( void )
{
        int i;
        int rv = TPASS;
        sThreadContext * pContext;

        for(i = 0; i < gNumThreads; ++i)
        {
                if(gTestappConfig.mThreadToExecute != -1 && gTestappConfig.mThreadToExecute != i)
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
                if(gTestappConfig.mThreadToExecute != -1 && gTestappConfig.mThreadToExecute != i)
                        continue;
                pContext = gThreadContext + i;
                pthread_join( pContext->mThreadID, NULL );
        }
        for( i = 0; i < gNumThreads; ++i )
        {
                if(gTestappConfig.mThreadToExecute != -1 && gTestappConfig.mThreadToExecute != i)
                        continue;
                pContext = gThreadContext + i;
                rv += pContext->mLtpRetval;
        }

        if( TPASS == rv )
                rv = VerificateResults();

        return rv;
}

/*====================*/
int VerificateResults( void )
{
        /* verificate Dev0 */
        if(strncmp((char *) Dev0_patten_buf, (char *) Dev0_read_buf, MMC_SD_BLOCK_SIZE * MMC_SD_BLOCK_COUNT) != 0)
        {
                tst_resm(TFAIL, "VerificateResults() failed");
                return TFAIL;
        }

        /* verificate Dev1 */
        if(strncmp((char *) Dev1_patten_buf, (char *) Dev1_read_buf, MMC_SD_BLOCK_SIZE * MMC_SD_BLOCK_COUNT) != 0)
        {
                tst_resm(TFAIL, "VerificateResults() failed");
                return TFAIL;
        }

        return TPASS;
}

/*====================*/
int VT_systest2_setup( void )
{
        gNumThreads = 2;
        gThreadProc[0] = Thread1;
        gThreadProc[1] = Thread2;

        /* setup for Dev0 */
        gMMCDev0 = open(gTestappConfig.mmc1DevName, O_RDWR);
        if (gMMCDev0 == -1)
        {
                tst_resm(TFAIL, "VT_systest2_setup() Failed open device: %s", gTestappConfig.mmc1DevName);
                return TFAIL;
        }
        srand((unsigned int) time((time_t *) NULL));

        Dev0_patten_buf = (unsigned char *) malloc(MMC_SD_BLOCK_SIZE * MMC_SD_BLOCK_COUNT);
        if(Dev0_patten_buf == 0)
        {
                tst_resm(TFAIL, "VT_systest2_setup() Failed allocate memory");
                return TFAIL;
        }

        Dev0_read_buf = (unsigned char *) malloc(MMC_SD_BLOCK_SIZE * MMC_SD_BLOCK_COUNT);
        if(Dev0_read_buf == 0)
        {
                tst_resm(TFAIL, "VT_systest2_setup() Failed allocate memory");
                return TFAIL;
        }

        /* setup for Dev1 */
        gMMCDev1 = open(gTestappConfig.mmc2DevName, O_RDWR);
        if (gMMCDev1 == -1)
        {
                tst_resm(TFAIL, "VT_systest2_setup() Failed open device: %s", gTestappConfig.mmc2DevName);
                return TFAIL;
        }
        srand((unsigned int) time((time_t *) NULL));

        Dev1_patten_buf = (unsigned char *) malloc(MMC_SD_BLOCK_SIZE * MMC_SD_BLOCK_COUNT);
        if(Dev1_patten_buf == 0)
        {
                tst_resm(TFAIL, "VT_systest2_setup() Failed allocate memory");
                return TFAIL;
        }

        Dev1_read_buf = (unsigned char *) malloc(MMC_SD_BLOCK_SIZE * MMC_SD_BLOCK_COUNT);
        if(Dev1_read_buf == 0)
        {
                tst_resm(TFAIL, "VT_systest2_setup() Failed allocate memory");
                return TFAIL;
        }

        return TPASS;
}

/*====================*/
int VT_systest2_cleanup( void )
{
        /* cleanup for Dev0 */
        if(Dev0_patten_buf != NULL)
                free(Dev0_patten_buf);
        if(Dev0_read_buf != NULL)
                free(Dev0_read_buf);

        if(gMMCDev0 > 0)
        {
                if(close(gMMCDev0) == -1)
                {
                        tst_resm(TFAIL, "VT_systest2_cleanup() Failed close device %s", gTestappConfig.mmc1DevName);
                        return TFAIL;
                }
        }

        /* cleanup for Dev1 */
        if(Dev1_patten_buf != NULL)
                free(Dev1_patten_buf);
        if(Dev1_read_buf != NULL)
                free(Dev1_read_buf);

        if(gMMCDev1 > 0)
        {
                if(close(gMMCDev1) == -1)
                {
                        tst_resm(TFAIL, "VT_systest2_cleanup() Failed close device %s", gTestappConfig.mmc2DevName);
                        return TFAIL;
                }
        }

        return TPASS;
}

/*====================*/
int VT_systest2_test( void )
{
        return RunTest();
}

/*====================*/
