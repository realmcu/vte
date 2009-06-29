/***
**Copyright 2007-2009 Freescale Semiconductor, Inc. All Rights Reserved.
**
**The code contained herein is licensed under the GNU General Public
**License. You may obtain a copy of the GNU General Public License
**Version 2 or later at the following locations:
**
**http://www.opensource.org/licenses/gpl-license.html
**http://www.gnu.org/copyleft/gpl.html
**/
/*=================================================================================================
Revision History:
                            Modification     Tracking
Author                          Date          Number    Description of Changes
-------------------------   ------------    ----------  -------------------------------------------
D.Khoroshev                  06/05/2007     ENGR37697   Initial version
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
#include <linux/types.h>

// SCC
#include <fsl_shw.h>
#include <sahara_module.h>
#include <scc_test_module.h>

// V4L2
#include <asm/types.h>          /* for videodev2.h */
#include <linux/compiler.h>     /* for videodev2.h */
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
#define CAPTURE_IMG_WIDTH  240
#define CAPTURE_IMG_HEIGHT 320
#define FRAMES_TO_CAPTURE  20

// IPC stuff. (AP+BP communications)
#define IPC_CHANNEL3              "/dev/mxc_ipc/3"
#define IPC_BUF_SZ                8192
#define IPC_TRANSFER_SZ           8192
#define IPC_CRITICAL_ERRORS_COUNT 50        

// SCC stuff.
#define DEFAULT_PLAINTEXT_LENGTH 24
/** Normal number of bytes to encrypt */
#define SCC_DEV "/dev/scc_test_module"
/** Normal number of bytes of padding to add to ciphertext storage */
#define DEFAULT_PADDING_ALLOWANCE 10


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
static sV4LBuffer *       gpV4LBuf = 0, * gpV4LBuf1 = 0;
static unsigned int       gV4LBufNum = 0, gV4LBufNum1 = 0;
static struct v4l2_format gV4LFormat, gV4LFormat1;  
static int                gV4LDev = -1;
static int                gV4LDevO = -1;


// SCC stuff.
static uint8_t plaintext[4096] =
        { 0xca, 0xbb, 0xad, 0xba,
          0xde, 0xad, 0xbe, 0xef,
          0xdb, 0xea, 0x11, 0xbe,
          'A', 'B', 'C', 'D',
          0x73, 0x1c, 0xab, 0xad,
          0xab, 0xad, 0xac, 0x24
};

static int byte_offset = 0;
static int plaintext_length = DEFAULT_PLAINTEXT_LENGTH;
static int encrypt_padding_allowance = DEFAULT_PADDING_ALLOWANCE;
static int decrypt_padding_allowance = 0;
static scc_crypto_mode_t crypto_mode = SCC_ECB_MODE;
static scc_verify_t check_mode = SCC_VERIFY_MODE_NONE;
static int gSCCDev = -1;

/*==================================================================================================
                                       GLOBAL VARIABLES
==================================================================================================*/

extern sTestappConfig gTestappConfig;


/*==================================================================================================
                                    FUNCTION PROTOTYPES
==================================================================================================*/

void* Thread1( void * pContext ); // AP: captures images and save the captured images onto FFS 
void* Thread2( void * pContext ); // AP: SCC crypt/decrypt smth
//void* Thread3( void * pContext ); // BP: running a memory-not-friendly code. ?howto?
void* Thread3( void * pContext ); // AP: reads files from SD/MMC and show them on LCD

int RunTest           ( void );
int VerificateResults ( void );
int CheckFile         ( const char * fname, int sz );
int SetupFFS          ( const char * dev, const char * mntdir );
int CloseFFS          ( const char * mntdir );
int BitMatching       ( char * buf1, char * buf2, int count );
int write_scc_register(uint32_t reg, uint32_t value);
int read_scc_register(uint32_t reg, uint32_t * value);
int run_zeroize_tests(void);
void print_ram_data(uint32_t * ram, uint32_t address, int count);
void print_scc_return_code(scc_return_t code);
int run_cipher_tests(void);

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
/*        struct v4l2_buffer buffer1;  */
        enum v4l2_buf_type typeBuffer = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        for (i = 0; i < gV4LBufNum; ++i)
        {
                memset(&buffer, 0, sizeof buffer);      
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
//                printf("----frame No %d\n", framesNo);
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

                                
/*                                memset(&buffer1, 0, sizeof buffer1);
                                buffer1.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
                                buffer1.memory = V4L2_MEMORY_MMAP;
                                if ( framesNo < gV4LBufNum1)
                                {
                                        buffer1.index = framesNo;
                                        if (ioctl(gV4LDevO, VIDIOC_QUERYBUF, &buffer1) < 0)
                                        {
                                                tst_resm(TWARN, "%s(): output ioctl(VIDIOC_QUERYBUF) failed: %s", __FUNCTION__, strerror(errno));
                                                pCtx->mLtpRetval = TFAIL;
                                                pCtx->mErrCount++;
                                        }
                                }
                                else
                                {
                                        if (ioctl(gV4LDevO, VIDIOC_DQBUF, &buffer1) < 0 && errno != EAGAIN)
                                        {
                                                tst_resm(TWARN, "%s(): output ioctl(VIDIOC_DQBUF) failed: %s", __FUNCTION__, strerror(errno));
                                                pCtx->mLtpRetval = TFAIL;
                                                pCtx->mErrCount++;
                                        }

                                }

                                memcpy(gpV4LBuf[buffer.index].mpStart, gpV4LBuf1[buffer1.index].mpStart, gpV4LBuf[buffer.index].mSz);
                                if (ioctl(gV4LDevO, VIDIOC_QBUF, &buffer1) < 0)
                                {
                                        tst_resm(TFAIL, "%s(): output ioctl(VIDIOC_QBUF) failed: %s", __FUNCTION__, strerror(errno));
                                        pCtx->mLtpRetval = TFAIL;
                                        pCtx->mErrCount++;
                                        break;
                                }
                                if (framesNo == gV4LBufNum1)
                                {
                                        tst_resm(TINFO, "V4L starting output...");
                                        typeBuffer = V4L2_BUF_TYPE_VIDEO_OUTPUT;
                                        if (ioctl(gV4LDevO, VIDIOC_STREAMON, &typeBuffer) < 0)
                                        {
                                                tst_resm(TFAIL, "%s(): output ioctl(VIDIOC_STREAMON) failed. Reason: %s", __FUNCTION__, strerror(errno));
                                                pCtx->mLtpRetval = TFAIL;
                                                pCtx->mErrCount++;
                                        }
                                        tst_resm(TINFO, "V4L output is started");
                                }*/
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
        else
        if (gTestappConfig.mVerbose)
                tst_resm(TINFO, "Stream capture stopped ...ok");

/*        typeBuffer = V4L2_BUF_TYPE_VIDEO_OUTPUT;
        if (ioctl(gV4LDevO, VIDIOC_STREAMOFF, &typeBuffer) < 0)
        {
                tst_resm(TFAIL, "%s(): output ioctl(VIDIOC_STREAMOFF) failed", __FUNCTION__);
                pCtx->mLtpRetval = TFAIL;
                pCtx->mErrCount++;
        }
        else
        if (gTestappConfig.mVerbose)
                tst_resm(TINFO, "Stream output stopped ...ok");
*/
        
        tst_resm( TINFO, "V4L capturing is stopped. Errors count: %d", pCtx->mErrCount );
        
        pthread_mutex_lock(&gMutex);
        gStopAllThreads = TRUE;
        pthread_mutex_unlock(&gMutex);
        
        return 0;                    
}


/*================================================================================================*/
/*================================================================================================*/


/*================================================================================================*/
/*================================================================================================*/

void *Thread2( void *pContext )
{
        sThreadContext * pCtx = (sThreadContext*)pContext;
        assert(pCtx);

        tst_resm(TINFO, "Starting SCC test");
        for (;;)
        {        int res = 0;
                res = run_cipher_tests();
                if (res)
                {
                                pCtx->mLtpRetval = TFAIL;
                                pCtx->mErrCount++;
                                tst_resm(TFAIL, "%s() : SCC operation failed", __FUNCTION__);
                }
                sleep(3);
                pthread_mutex_lock(&gMutex);
                if (gStopAllThreads)
                        break;
                pthread_mutex_unlock(&gMutex);                        
        }
        tst_resm(TINFO, "SCC test stopped");
        return 0;
}


/*================================================================================================*/
/*===== write_scc_resgister =====*/
/**
@brief  Performs write data into SCC register

@param  Input :      reg   - the register to be written
                     value - the value to store

@return On failure - TFAIL
        On success - TPASS
*/
/*================================================================================================*/
int write_scc_register(uint32_t reg, uint32_t value)
{
        scc_reg_access  register_access;
        int             VT_rv = TPASS;
        int             status;

        register_access.reg_offset = reg;
        register_access.reg_data = value;

        status = ioctl(gSCCDev, SCC_TEST_WRITE_REG, &register_access);
        if ( status != SCC_RET_OK)
        {
                VT_rv = TFAIL;
        }

        return VT_rv;
}

/*================================================================================================*/
/*===== read_scc_resgister =====*/
/**
@brief  Performs read data from SCC register

@param  Input :      reg -   the register to be read
                     value - the location for return value
        Output:      register_access.reg_data - the return value

@return On failure - TFAIL
        On success - TPASS
*/
/*================================================================================================*/
int read_scc_register(uint32_t reg, uint32_t * value)
{
        scc_reg_access  register_access;
        int             VT_rv = TPASS;
        int             status;

        register_access.reg_offset = reg;
        status = ioctl(gSCCDev, SCC_TEST_READ_REG, &register_access);

        if (status == SCC_RET_OK)
        {
                *value = register_access.reg_data;
        }
        else
        {
                VT_rv = TFAIL;
        }

        return VT_rv;
}

/*================================================================================================*/
/*===== run_zeroize_tests =====*/
/**
@brief  Performs Zeroize tests

@param

@return On failure - TFAIL
        On success - TPASS
*/
/*================================================================================================*/
int run_zeroize_tests(void)
{
        int             VT_rv = TPASS;
        uint32_t        value;

        /* set up some known values */
        if (write_scc_register(SCM_RED_MEMORY, 42) == TPASS)
        {
                if (gTestappConfig.mVerbose)
                        tst_resm(TPASS, "Writing value (42) into SCM_RED_MEMORY (0x%08X) passed",
                         SCM_RED_MEMORY);
        }
        else
        {
                VT_rv = TFAIL;
                tst_resm(TFAIL, "Writing value (42) into SCM_RED_MEMORY (0x%08X) failed",
                         SCM_RED_MEMORY);
        }
        if (write_scc_register(SCM_RED_MEMORY + 1020, 42) == TPASS)
        {
                if (gTestappConfig.mVerbose)
                        tst_resm(TPASS, "Writing value (42) into SCM_RED_MEMORY+0x00001020 (0x%08X) passed",
                         SCM_RED_MEMORY + 0x00001020);
        }
        else
        {
                VT_rv = TFAIL;
                tst_resm(TFAIL, "Writing value (42) into SCM_RED_MEMORY+0x00001020 (0x%08X) failed",
                         SCM_RED_MEMORY + 0x00001020);
        }
        if (write_scc_register(SCM_BLACK_MEMORY, 42) == TPASS)
        {
                if (gTestappConfig.mVerbose)
                        tst_resm(TPASS, "Writing value (42) into SCM_BLACK_MEMORY (0x%08X) passed",
                         SCM_BLACK_MEMORY);
        }
        else
        {
                VT_rv = TFAIL;
                tst_resm(TFAIL, "Writing value (42) into SCM_BLACK_MEMORY (0x%08X) failed",
                         SCM_BLACK_MEMORY);
        }
        if (write_scc_register(SCM_BLACK_MEMORY + 1020, 42) == TPASS)
        {
                if (gTestappConfig.mVerbose)
                        tst_resm(TPASS,
                         "Writing value (42) into SCM_BLACK_MEMORY+0x00001020 (0x%08X) passed",
                         SCM_BLACK_MEMORY + 0x00001020);
        }
        else
        {
                VT_rv = TFAIL;
                tst_resm(TFAIL,
                         "Writing value (42) into SCM_BLACK_MEMORY+0x00001020 (0x%08X) failed",
                         SCM_BLACK_MEMORY + 0x00001020);
        }

        if (ioctl(gSCCDev, SCC_TEST_ZEROIZE, &value) != SCC_RET_OK)
        {
                VT_rv = TFAIL;
                tst_resm(TFAIL, "Zeroize test failed. Function return code: %0X", value);
        }
        else
        {
                /* no errors, verify values are gone */
                if (read_scc_register(SCM_RED_MEMORY, &value) == TPASS)
                {
                        if (gTestappConfig.mVerbose)
                                tst_resm(TPASS, "Reading value (42) from SCM_RED_MEMORY (0x%08X) passed",
                                 SCM_RED_MEMORY);
                        if (value == 42)
                        {
                                VT_rv = TFAIL;
                                tst_resm(TFAIL, "Zeroize failed at 0x%08X", SCM_RED_MEMORY);
                        }
                        else if (gTestappConfig.mVerbose)
                        {
                                tst_resm(TPASS, "Zeroize passed at 0x%08X", SCM_RED_MEMORY);
                        }
                }
                else
                {
                        VT_rv = TFAIL;
                        tst_resm(TFAIL, "Reading value (42) from SCM_RED_MEMORY (0x%08X) failed",
                                 SCM_RED_MEMORY);
                }
                if (read_scc_register(SCM_RED_MEMORY + 1020, &value) == TPASS)
                {        
                        if (gTestappConfig.mVerbose)
                                tst_resm(TPASS,
                                 "Reading value (42) from SCM_RED_MEMORY+0x00001020 (0x%08X) passed",
                                 SCM_RED_MEMORY + 0x00001020);
                        if (value == 42)
                        {
                                VT_rv = TFAIL;
                                tst_resm(TFAIL, "Zeroize failed at 0x%08X",
                                         SCM_RED_MEMORY + 0x00001020);
                        }
                        else if (gTestappConfig.mVerbose)
                        {
                                tst_resm(TPASS, "Zeroize passed at 0x%08X",
                                         SCM_RED_MEMORY + 0x00001020);
                        }
                }
                else
                {
                        VT_rv = TFAIL;
                        tst_resm(TFAIL,
                                 "Reading value (42) from SCM_RED_MEMORY+0x00001020 (0x%08X) failed",
                                 SCM_RED_MEMORY + 0x00001020);
                }
                if (read_scc_register(SCM_BLACK_MEMORY, &value) == TPASS)
                {
                        if (gTestappConfig.mVerbose)
                                tst_resm(TPASS, "Reading value (42) from SCM_BLACK_MEMORY (0x%08X) passed",
                                         SCM_BLACK_MEMORY);
                        if (value == 42)
                        {
                                VT_rv = TFAIL;
                                tst_resm(TFAIL, "Zeroize failed at 0x%08X", SCM_BLACK_MEMORY);
                        }
                        else if (gTestappConfig.mVerbose)
                        {
                                tst_resm(TPASS, "Zeroize passed at 0x%08X", SCM_BLACK_MEMORY);
                        }
                }
                else
                {
                        VT_rv = TFAIL;
                        tst_resm(TFAIL, "Reading value (42) from SCM_BLACK_MEMORY (0x%08X) failed",
                                 SCM_BLACK_MEMORY);
                }
                if (read_scc_register(SCM_BLACK_MEMORY + 1020, &value) == TPASS)
                {
                        if (gTestappConfig.mVerbose)
                                tst_resm(TPASS,
                                         "Reading value (42) from SCM_BLACK_MEMORY+0x00001020 (0x%08X) passed",
                                 SCM_BLACK_MEMORY + 0x00001020);
                        if (value == 42)
                        {
                                VT_rv = TFAIL;
                                tst_resm(TFAIL, "Zeroize failed at 0x%08X",
                                         SCM_BLACK_MEMORY + 0x00001020);
                        }
                        else if (gTestappConfig.mVerbose)
                        {
                                tst_resm(TPASS, "Zeroize passed at 0x%08X",
                                         SCM_BLACK_MEMORY + 0x00001020);
                        }
                }
                else
                {
                        VT_rv = TFAIL;
                        tst_resm(TFAIL,
                                 "Reading value (42) from SCM_BLACK_MEMORY+0x00001020 (0x%08X) failed",
                                 SCM_BLACK_MEMORY + 0x00001020);
                }
        }
        return VT_rv;
}

/*================================================================================================*/
/*===== print_ram_data =====*/
/**
@brief  Performs print eight words per line, starting at ram, as though they
        started at address, until count words have been printed

@param  Input :      ram    -  start
                     address - byte address
                     count   - word counter

@return
*/
/*================================================================================================*/
void print_ram_data(uint32_t * ram, uint32_t address, int count)
{
        int     i;

        for (i = 0; i < count; i++)
        {
                if (i % 8 == 0)
                {
                        printf("Byte address: %04X ", address + i * 4); /* print byte *address */
                }

                printf("Word: %08X ", *ram++);  /* print a word */

                if (i % 8 == 7)
                {
                        printf("\n");   /* end of line - do newline */
                }
                else if (i % 8 == 3)
                {
                        printf(" ");    /* add space in the middle */
                }
        }

        if (count % 8 != 0)     /* if didn't have mod8 words... */
        {
                printf("\n");
        }
}

/*================================================================================================*/
/*===== print_scc_return_code =====*/
/**
@brief  Print an interpretation (the symbol name) of @c code

@param  Input :      code - the @c code

@return
*/
/*================================================================================================*/
void print_scc_return_code(scc_return_t code)
{
        char   *msg = NULL;

        switch (code)
        {
        case SCC_RET_OK:
                msg = "SCC_RET_OK";
                break;
        case SCC_RET_FAIL:
                msg = "SCC_RET_FAIL";
                break;
        case SCC_RET_VERIFICATION_FAILED:
                msg = "SCC_RET_VERIFICATION_FAILED";
                break;
        case SCC_RET_TOO_MANY_FUNCTIONS:
                msg = "SCC_RET_TOO_MANY_FUNCTIONS";
                break;
        case SCC_RET_BUSY:
                msg = "SCC_RET_BUSY";
                break;
        case SCC_RET_INSUFFICIENT_SPACE:
                msg = "SCC_RET_INSUFFICIENT_SPACE";
                break;
        default:
                break;
        }

        if (msg)
        {
                printf("SCC return code: %s\n", msg);
        }
        else
        {
                printf("SCC return code: %d\n", code);
        }
}

int run_cipher_tests(void)
{
        scc_encrypt_decrypt     cipher_control;
//        uint32_t                value; /* value of various registers */
        int                     VT_rv = TPASS;
        uint8_t                 *ciphertext = NULL;
        uint8_t                 *new_plaintext = NULL;

        ciphertext = malloc(sizeof(plaintext)) + byte_offset;
        new_plaintext = malloc(sizeof(plaintext) + byte_offset);

        cipher_control.data_in = plaintext;
        cipher_control.data_in_length = plaintext_length;
        cipher_control.data_out = ciphertext;
        /* sufficient for block and padding */
        cipher_control.data_out_length = plaintext_length + encrypt_padding_allowance;

        cipher_control.init_vector[0] = 0;
        cipher_control.init_vector[1] = 0;

        cipher_control.crypto_mode = crypto_mode;
        cipher_control.check_mode = check_mode;
        cipher_control.wait_mode = SCC_ENCRYPT_POLL;

        /* clear these to make sure they are set by driver */
        if (write_scc_register(SCM_INIT_VECTOR_0, 0) == TPASS)
        {
                if (gTestappConfig.mVerbose)
                        tst_resm(TPASS, "Writing 0 into SCM_INIT_VECTOR_0 (0x%08X) passed",
                         SCM_INIT_VECTOR_0);
        }
        else
        {
                VT_rv = TFAIL;
                tst_resm(TFAIL, "Writing 0 into SCM_INIT_VECTOR_0 (0x%08X) failed",
                         SCM_INIT_VECTOR_0);
        }
        if (write_scc_register(SCM_INIT_VECTOR_1, 0) == TPASS)
        {
                if (gTestappConfig.mVerbose)
                       tst_resm(TPASS, "Writing 0 into SCM_INIT_VECTOR_1 (0x%08X) passed",
                         SCM_INIT_VECTOR_1);
        }
        else
        {
                VT_rv = TFAIL;
                tst_resm(TFAIL, "Writing 0 into SCM_INIT_VECTOR_1 (0x%08X) failed",
                         SCM_INIT_VECTOR_1);
        }
        /* Start Encryption */
        if (gTestappConfig.mVerbose)
                tst_resm(TINFO, "CIPHER: Start Encrypting....");
        if (ioctl(gSCCDev, SCC_TEST_ENCRYPT, &cipher_control) != SCC_RET_OK)
        {
                VT_rv = TFAIL;
                tst_resm(TFAIL, "Encryption failed");
                print_scc_return_code(cipher_control.function_return_code);
        }
        else
        {
#ifdef SCC_DEBUG
                if (debug)      /* used 'D' option */
                {
                        dump_cipher_control_block(&cipher_control);
                        print_ram_data((void *) ciphertext, SCM_BLACK_MEMORY, 32);

                        if (read_scc_register(SCM_STATUS, &value) == TPASS)
                        {
                                tst_resm(TPASS, "Reading SCM_STATUS (0x%08X) value: 0x%08X",
                                         SCM_STATUS, value);
                        }
                        else
                        {
                                VT_rv = TFAIL;
                                tst_resm(TFAIL, "Reading SCM_STATUS (0x%08X) value: 0x%08X",
                                         SCM_STATUS, value);
                        }
                        print_scm_status_register(value);

                        if (read_scc_register(SMN_STATUS, &value) == TPASS)
                        {
                                tst_resm(TPASS, "Reading SMN_STATUS (0x%08X) value: 0x%08X",
                                         SMN_STATUS, value);
                        }
                        else
                        {
                                VT_rv = TFAIL;
                                tst_resm(TFAIL, "Reading SMN_STATUS (0x%08X) value: 0x%08X",
                                         SMN_STATUS, value);
                        }
                        print_smn_status_register(value);

                        read_scc_register(SCM_ERROR_STATUS, &value);
                        if (read_scc_register(SCM_ERROR_STATUS, &value) == TPASS)
                        {
                                tst_resm(TPASS, "Reading SCM_ERROR_STATUS (0x%08X) value: 0x%08X",
                                         SCM_ERROR_STATUS, value);
                        }
                        else
                        {
                                VT_rv = TFAIL;
                                tst_resm(TFAIL, "Reading SCM_ERROR_STATUS (0x%08X) value: 0x%08X",
                                         SCM_ERROR_STATUS, value);
                        }
                        print_scc_error_status_register(value);
                }
#endif // SCC_DEBUG
                if (VT_rv == TPASS)
                {
                        if (gTestappConfig.mVerbose)
                               tst_resm(TPASS, "***End of Encryption");
                }
                else
                {
                        if (gTestappConfig.mVerbose)
                               tst_resm(TFAIL, "***Encryption not ended");
                }

                if (!gTestappConfig.mEncryptOnly)      /* not used 'e' option */
                {
                        /* wipe out any remnants of encryption */
                        run_zeroize_tests();

                        /* DECRYPTION TEST */

                        cipher_control.data_in = ciphertext;    /* feed ciphertext back in */
                        cipher_control.data_in_length = cipher_control.data_out_length;
                        cipher_control.data_out = new_plaintext;
                        cipher_control.data_out_length =
                            plaintext_length + decrypt_padding_allowance;

                        if (gTestappConfig.mInjectCRCError)
                        {
                                ciphertext[rand() % cipher_control.data_in_length] ^= 1;
                        }

                        if (crypto_mode == SCC_CBC_MODE)
                        {
                                cipher_control.init_vector[0] = 0;
                                cipher_control.init_vector[1] = 0;
                        }

                        cipher_control.crypto_mode = crypto_mode;
                        cipher_control.wait_mode = SCC_ENCRYPT_POLL;

                        /* clear these again to make sure they are set by driver */
                        if (write_scc_register(SCM_INIT_VECTOR_0, 0) == TPASS)
                        {
                                if (gTestappConfig.mVerbose)
                                        tst_resm(TPASS, "Writing 0 into SCM_INIT_VECTOR_0 (0x%08X) passed",
                                         SCM_INIT_VECTOR_0);
                        }
                        else
                        {
                                VT_rv = TFAIL;
                                tst_resm(TFAIL, "Writing 0 into SCM_INIT_VECTOR_0 (0x%08X) failed",
                                         SCM_INIT_VECTOR_0);
                        }
                        if (write_scc_register(SCM_INIT_VECTOR_1, 0) == TPASS)
                        {
                                if (gTestappConfig.mVerbose)
                                        tst_resm(TPASS, "Writing 0 into SCM_INIT_VECTOR_1 (0x%08X) passed",
                                         SCM_INIT_VECTOR_1);
                        }
                        else
                        {
                                VT_rv = TFAIL;
                                tst_resm(TFAIL, "Writing 0 into SCM_INIT_VECTOR_1 (0x%08X) failed",
                                         SCM_INIT_VECTOR_1);
                        }
                        if (write_scc_register(SCM_INTERRUPT_CTRL,
                                               SCM_INTERRUPT_CTRL_CLEAR_INTERRUPT
                                               | SCM_INTERRUPT_CTRL_MASK_INTERRUPTS) == TPASS)
                        {
                                if (gTestappConfig.mVerbose)
                                        tst_resm(TPASS,
                                         "Writing 0x%08X into SCM_INTERRUPT_CTRL (0x%08X) passed",
                                         SCM_INTERRUPT_CTRL_CLEAR_INTERRUPT |
                                         SCM_INTERRUPT_CTRL_MASK_INTERRUPTS, SCM_INTERRUPT_CTRL);
                        }
                        else
                        {
                                VT_rv = TFAIL;
                                tst_resm(TFAIL,
                                         "Writing 0x%08X into SCM_INTERRUPT_CTRL (0x%08X) failed",
                                         SCM_INTERRUPT_CTRL_CLEAR_INTERRUPT |
                                         SCM_INTERRUPT_CTRL_MASK_INTERRUPTS, SCM_INTERRUPT_CTRL);
                        }

                        /* Start Decryption */                
                        
                        if (gTestappConfig.mVerbose)
                                tst_resm(TINFO, "CIPHER: Start Decrypting....");
                        if (ioctl(gSCCDev, SCC_TEST_DECRYPT, &cipher_control) != SCC_RET_OK)
                        {
                                tst_resm(TFAIL, "Decryption failed");
                                print_scc_return_code(cipher_control.function_return_code);
                        }
                        else
                        {
                                int     bytes_to_check = plaintext_length;

#ifdef SCC_DEBUG
                                if (debug)
                                {

                                        dump_cipher_control_block(&cipher_control);
                                        print_ram_data((void *) new_plaintext, SCM_RED_MEMORY, 10);

                                        if (read_scc_register(SCM_STATUS, &value) == TPASS)
                                        {
                                                tst_resm(TPASS,
                                                         "Reading SCM_STATUS (0x%08X) value: 0x%08X",
                                                         SCM_STATUS, value);
                                        }
                                        else
                                        {
                                                VT_rv = TFAIL;
                                                tst_resm(TFAIL,
                                                         "Reading SCM_STATUS (0x%08X) value: 0x%08X",
                                                         SCM_STATUS, value);
                                        }
                                        print_scm_status_register(value);

                                        if (read_scc_register(SMN_STATUS, &value) == TPASS)
                                        {
                                                tst_resm(TPASS,
                                                         "Reading SMN_STATUS (0x%08X) value: 0x%08X",
                                                         SMN_STATUS, value);
                                        }
                                        else
                                        {
                                                VT_rv = TFAIL;
                                                tst_resm(TFAIL,
                                                         "Reading SMN_STATUS (0x%08X) value: 0x%08X",
                                                         SMN_STATUS, value);
                                        }
                                        print_smn_status_register(value);

                                        read_scc_register(SCM_ERROR_STATUS, &value);
                                        if (read_scc_register(SCM_ERROR_STATUS, &value) == TPASS)
                                        {
                                                tst_resm(TPASS,
                                                         "Reading SCM_ERROR_STATUS (0x%08X) value: 0x%08X",
                                                         SCM_ERROR_STATUS, value);
                                        }
                                        else
                                        {
                                                VT_rv = TFAIL;
                                                tst_resm(TFAIL,
                                                         "Reading SCM_ERROR_STATUS (0x%08X) value: 0x%08X",
                                                         SCM_ERROR_STATUS, value);
                                        }
                                        print_scc_error_status_register(value);
                                }
#endif // SCC_DEBUG
                                if (cipher_control.data_out_length != plaintext_length)
                                {
                                        if (gTestappConfig.mVerbose)
                                                tst_resm(TINFO,
                                                 "Error:  input plaintext length (%d) and output "
                                                 "plaintext length (%ld) do not match.\n",
                                                 plaintext_length, cipher_control.data_out_length);
                                }
                                while (bytes_to_check--)
                                {
                                        if (plaintext[bytes_to_check] !=
                                            new_plaintext[bytes_to_check])
                                        {
                                                if (gTestappConfig.mVerbose)
                                                        tst_resm(TPASS,
                                                         "output plaintext (0x%02x) do not match at "
                                                         "offset %d.\n", plaintext[bytes_to_check],
                                                         new_plaintext[bytes_to_check],
                                                         bytes_to_check);
                                        }
                                }
                                if (gTestappConfig.mVerbose)
                                        tst_resm(TPASS, "End of Cipher Test");
                        }       /* else decrypt failed */
                }       /* encrypt_only */
        }       /* else encrypt failed */

        return VT_rv;
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
        int i;
        gNumThreads = 2;
        gThreadProc[0] = Thread1;
        gThreadProc[1] = Thread2;

        /************************************************
         * Init V4L.
         ************************************************/
        
        if ((gV4LDev = open(gTestappConfig.mV4LDevName, O_RDWR|O_NONBLOCK, 0)) < 0)
        {
                tst_resm(TFAIL, "%s(): Unable to open %s", __FUNCTION__, gTestappConfig.mV4LDevName);
                return TFAIL;
        }
        
        if ((gV4LDevO = open(gTestappConfig.mV4LDevOName, O_RDWR|O_NONBLOCK, 0)) < 0)
        {
                tst_resm(TFAIL, "%s(): Unable to open %s", __FUNCTION__, gTestappConfig.mV4LDevOName);
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
        
        memset(&cap, 0, sizeof cap);
        if (ioctl(gV4LDevO, VIDIOC_QUERYCAP, &cap) < 0)
        {
                tst_resm(TFAIL, "%s(): ioctl(VIDIOC_QUERYCAP) failed", __FUNCTION__);
                return TFAIL;
        }
        if (!(cap.capabilities & V4L2_CAP_VIDEO_OUTPUT))
        {
                tst_resm(TFAIL, "%s(): %s does not support output", __FUNCTION__, gTestappConfig.mV4LDevOName);
                return TFAIL;
        }

        int g_output = 3;

        if (ioctl(gV4LDevO, VIDIOC_S_OUTPUT, &g_output) < 0)
        {
                tst_resm(TFAIL, "%s(): %s can not set output 3: %s", __FUNCTION__, gTestappConfig.mV4LDevOName, strerror(errno));
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

        memcpy(&gV4LFormat1, &gV4LFormat, sizeof gV4LFormat);
        gV4LFormat1.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
        if (ioctl(gV4LDevO, VIDIOC_S_FMT, &gV4LFormat1) < 0)
        {
                tst_resm(TFAIL, "%s(): ioctl(VIDIOC_S_FMT) failed", __FUNCTION__);
                return TFAIL;
        }
        
        // Init mmap.
        struct v4l2_requestbuffers reqBuffers;
        struct v4l2_buffer         buffer;
        memset(&reqBuffers, 0, sizeof reqBuffers);
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
        if (!(gpV4LBuf = calloc(reqBuffers.count, sizeof (sV4LBuffer))))
        {
                tst_resm(TBROK, "%s(): Can't allocate memory for V4L buffers", __FUNCTION__);
                return TFAIL;
        }
        for (gV4LBufNum = 0; gV4LBufNum < reqBuffers.count; ++gV4LBufNum)
        {
                memset(&buffer, 0, sizeof buffer);
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
        
        // video output  mmap
        memset(&reqBuffers, 0, sizeof reqBuffers);
        reqBuffers.count = 3;  
        reqBuffers.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
        reqBuffers.memory = V4L2_MEMORY_MMAP;
        if (ioctl(gV4LDevO, VIDIOC_REQBUFS, &reqBuffers) < 0)
        {
                tst_resm(TFAIL, "%s(): output ioctl(VIDIOC_REQBUFS) failed", __FUNCTION__);
                return TFAIL;
        }
        if (reqBuffers.count < 2)
        {
                tst_resm(TFAIL, "%s(): Insufficient buffer memory on %s", __FUNCTION__, gTestappConfig.mV4LDevOName);
                return TFAIL;
        }
        if (!(gpV4LBuf1 = calloc(reqBuffers.count, sizeof (sV4LBuffer))))
        {
                tst_resm(TBROK, "%s(): Can't allocate memory for V4L buffers", __FUNCTION__);
                return TFAIL;
        }
        for (gV4LBufNum1 = 0; gV4LBufNum1 < reqBuffers.count; ++gV4LBufNum1)
        {
                memset(&buffer, 0, sizeof buffer);
                buffer.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
                buffer.memory = V4L2_MEMORY_MMAP;
                buffer.index = gV4LBufNum1;    
                
                if (ioctl(gV4LDevO, VIDIOC_QUERYBUF, &buffer) < 0)    
                {      
                        tst_resm(TFAIL, "%s(): output ioctl(VIDIOC_QUERYBUF) failed", __FUNCTION__);      
                        return TFAIL;    
                } 
                gpV4LBuf1[gV4LBufNum1].mSz     = buffer.length;
                gpV4LBuf1[gV4LBufNum1].mpStart = mmap(NULL, buffer.length,
                                                    PROT_READ | PROT_WRITE, MAP_SHARED,
                                                    gV4LDev, buffer.m.offset);
                if (MAP_FAILED == gpV4LBuf1[gV4LBufNum1].mpStart)
                {
                        tst_resm(TFAIL, "%s(): mmap on %s failed", __FUNCTION__, gTestappConfig.mV4LDevOName);
                        return TFAIL;
                }
        }                                                                                  
        if (gTestappConfig.mVerbose)
        {
                tst_resm(TINFO, "V4L output init ... ok");
        }         
      
        int temp_sz = gpV4LBuf[0].mSz;
        for (i = 1; i < gV4LBufNum; ++i)
        {
                if (gpV4LBuf[i].mSz != temp_sz)
                {
                        tst_resm(TFAIL, "%s(): gpV4LBuf[%d] size (%d) differs from others (%d)", __FUNCTION__, i, gpV4LBuf[i].mSz, temp_sz);
                        return TFAIL;
                }
        }

        for (i = 0; i < gV4LBufNum1; ++i)
        {
                if (gpV4LBuf1[i].mSz != temp_sz)
                {
                        tst_resm(TFAIL, "%s(): gpV4LBuf1[%d] size (%d) differs from others (%d)", __FUNCTION__, i, gpV4LBuf1[i].mSz, temp_sz);
                        return TFAIL;
                }
        }

        /************************************************
         * Init SCC.
         ************************************************/
        
        if( (gSCCDev = open(SCC_DEV, O_RDWR)) < 0 )
        {
                tst_resm(TFAIL, "%s() : Can't open %s", __FUNCTION__, SCC_DEV);
                return TFAIL;
        }
        if (gTestappConfig.mVerbose)
        {
                tst_resm(TINFO, "SCC init ... ok");
        }
        
        for (i = 24; i < sizeof(plaintext); i++)
        {
                plaintext[i] = i % 256;
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
        CLOSE_DEV(gV4LDev);
        
        for (i = 0; i < gV4LBufNum1; ++i)
        {
                if( munmap(gpV4LBuf1[i].mpStart, gpV4LBuf1[i].mSz) < 0)
                {
                        tst_resm(TFAIL, "%s(): munmap on %s failed", __FUNCTION__, gTestappConfig.mV4LDevOName);
                        rv = TFAIL;
                }
        }
        if (gTestappConfig.mVerbose)
        {
                tst_resm(TINFO, "V4L output cleanup ... ok");
        }
        CLOSE_DEV(gV4LDevO);
        
        /************************************************
         * Cleanup FFS. 
         ************************************************/
        CloseFFS(gTestappConfig.mMountPoint);
        if (gTestappConfig.mVerbose)
        {
                tst_resm(TINFO, "FFS cleanup ... ok");
        }        

        /************************************************
         * Cleanup SCC.
         ************************************************/
        CLOSE_DEV(gSCCDev);
        if (gTestappConfig.mVerbose)
        {
                tst_resm(TINFO, "SCC cleanup ... ok");
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



