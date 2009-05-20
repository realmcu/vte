/*
 * Copyright (C) 2004, Freescale Semiconductor, Inc. All Rights Reserved THIS SOURCE CODE IS
 * CONFIDENTIAL AND PROPRIETARY AND MAY NOT BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
 * Freescale Semiconductor, Inc. */

/**
@file h264_encoder_test.c

@brief VTE C header template

@par Portability:
        Indicate if this module is portable to other compilers or platforms.
        If not, indicate specific reasons why is it not portable.
*/

/* REVISION HISTORY 

Author (core ID)      Date         CR Number    Description of Changes
-------------------   ----------   ----------   ------------------------------
I.Inkina/nknl001      1/07/2005    TLSbo52105   Initial version
D.Simakov/smkd001c    22/08/2005   TLSbo53252   Re-written
D.Simakov             19/04/2006   TLSbo64918   void * pAppContext was aded into callbacks
*/


/*
                                        INCLUDE FILES
*/
/* Verification Test Environment Include Files */
#include "h264_encoder_test.h"
/* AVC Encoder API. */
#include <AVC_VideoEncoder.h>
#include "stuff/kev.h"
#include "stuff/io.h"
#include "stuff/ui.h"



/*
                                        LOCAL MACROS
*/
#define MAX_THREADS 4
#define SAFE_DELETE(p) {if(p){free(p);pNULL;}}
#define NA "n/a"

#ifdef DEBUG_TEST
#define DPRINTF(fmt,...) printf(fmt, ##__VA_ARGS__)
#else
#define DPRINTF(fmt,...)
#endif

/**********************************************************************
* Macro name:  CALL_H264_ENCODER()
* Description: Macro checks for any error in function execution
               based on the return value. In case of error, the function exits.
**********************************************************************/
#define CALL_H264_ENCODER(H264EncRoutine, name)   \
    pHandler->mLastH264EncError  H264EncRoutine; \
    if( (pHandler->mLastH264EncError > E_H264E_ENCODE_COMPLETED ) )\
    {\
        TST_RESM( TWARN, "%s : %s fails #%d", __FUNCTION__, name, pHandler->mLastH264EncError);\
                return TFAIL;\
        }


#define TST_RESM(s,format,...) \
{\
    if( LOAD ! gTestappConfig.mTestCase )\
        pthread_mutex_lock( &gMutex );\
    tst_resm((s), (format), ##__VA_ARGS__);\
    if( LOAD ! gTestappConfig.mTestCase )\
        pthread_mutex_unlock( &gMutex );\
}

#define alloc_fast(s) malloc(s)
#define alloc_slow(s) malloc(s)

#define AVC_OUTPUT_SUFFIX       ".avc"
#define KEV_RECON_OUTPUT_SUFFIX "_recon.kev"

/*
                          LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
*/
/*---------------------------------------------------*/
typedef struct
{
        char    mInputFileName[MAX_STR_LEN];
        char    mOutputFileName[MAX_STR_LEN];
        char    mReferenceFileName[MAX_STR_LEN];
        char    mControlFileName[MAX_STR_LEN];
        char    mReconFileName[MAX_STR_LEN];
        char    mReconRefFileName[MAX_STR_LEN];

        int     mEncoderFramerate;
        int     mTargetBitRate;
        int     mWriteRecon;
} sHandlerParams;

/*---------------------------------------------------*/
typedef struct
{
        unsigned long mIndex;

        const sHandlerParams *mpParams;

        unsigned long mFramesCount;
        eAVCERetType mLastH264EncError;

        AVC_VideoEncodeStruct mEncConfig;
        IOParams mIoPars;

        pthread_t mThreadID;
        int     mIsThreadFinished;
        int     mLtpRetval;
} sH264EncoderHandler;

/*
                                       LOCAL CONSTANTS
*/


/*
                                       LOCAL VARIABLES
*/
static sH264EncoderHandler gH264EncHandlers[MAX_THREADS];
static int gNumThreads  1;
static int gThreadWithFocus  -1;
static sLinkedList *gpParamsList  NULL;
static pthread_mutex_t gMutex;
static char gProgress[]  "-\\|/";

/*
                                       GLOBAL CONSTANTS
*/

/*
                                       GLOBAL VARIABLES
*/

/*
                                   LOCAL FUNCTION PROTOTYPES
*/
void   *H264E_AllocateMemory(AVC_EncMemBlock * pMemBlk, void *pAppContext);
void    H264E_FreeMemory(void *ptr, void *pAppContext);

int     RunEncoder(void *ptr);
int     TestEngine(sH264EncoderHandler * pHandler);
void    PrintProgress(sH264EncoderHandler * pHandler);
void    ResetHandler(sH264EncoderHandler * pHandler);
void    CleanupHandler(sH264EncoderHandler * pHandler);
int     DoFilesExist(const char *fname1, const char *fname2);
void    HogCpu(void);
void    MakeEntry(char entry[WORDS_IN_ENTRY][MAX_STR_LEN], int nEntry);

/* test cases */
int     NominalFunctionalityTest(void);
int     ReLocatabilityTest(void);
int     ReEntranceTest(void);
int     PreEmptionTest(void);
int     EnduranceTest(void);
int     LoadTest(void);

/* helper */
int     BinCompare(const char *fname1, const char *fname2);
int     KevCompare(const char *kev1, const char *kev2);

/*
                                       LOCAL FUNCTIONS
*/

/**/
/**/
int VT_h264_encoder_setup(void)
{
        pthread_mutex_init(&gMutex, NULL);

        int     i;

        /* Reset all handlers. */
        for (i  0; i < MAX_THREADS; ++i)
                ResetHandler(gH264EncHandlers + i);

        gpParamsList  (sLinkedList *) malloc(sizeof(sLinkedList));
        gpParamsList->mpNext  NULL;
        gpParamsList->mpContent  NULL;

        if (!ParseConfig(gTestappConfig.mConfigFilename))
        {
                TST_RESM(TWARN, "%s : can't parse the %s",
                         __FUNCTION__, gTestappConfig.mConfigFilename);
                return TFAIL;
        }

        /* Compute the actual number of threads. */
        if (RE_ENTRANCE  gTestappConfig.mTestCase || PRE_EMPTION  gTestappConfig.mTestCase)
        {
                sLinkedList *pNode  gpParamsList;

                for (gNumThreads  0; pNode && gNumThreads < MAX_THREADS; ++gNumThreads)
                        pNode  pNode->mpNext;
        }

        return TPASS;
}


/**/
/**/
int VT_h264_encoder_cleanup(void)
{
        pthread_mutex_destroy(&gMutex);

        if (gpParamsList)
                LList_Delete(gpParamsList);

        int     i;

        for (i  0; i < MAX_THREADS; ++i)
        {
                CleanupHandler(gH264EncHandlers + i);
                ResetHandler(gH264EncHandlers + i);
        }

        return TPASS;
}


/**/
/**/
int VT_h264_encoder_test(void)
{
        int     rv  TFAIL;

        switch (gTestappConfig.mTestCase)
        {
        case NOMINAL_FUNCTIONALITY:
                TST_RESM(TINFO, "Nominal functionality test");
                rv  NominalFunctionalityTest();
                TST_RESM(TINFO, "End of nominal functionality test");
                break;

        case RELOCATABILITY:
                TST_RESM(TINFO, "Relocatability test");
                rv  ReLocatabilityTest();
                TST_RESM(TINFO, "End relocatability test");
                break;

        case RE_ENTRANCE:
                TST_RESM(TINFO, "Re-entrance test");
                rv  ReEntranceTest();
                TST_RESM(TINFO, "End of re-entrance test");
                break;

        case PRE_EMPTION:
                TST_RESM(TINFO, "Pre-emption test");
                rv  PreEmptionTest();
                TST_RESM(TINFO, "End of pre-emption test");
                break;

        case ENDURANCE:
                TST_RESM(TINFO, "Endurance test");
                rv  EnduranceTest();
                TST_RESM(TINFO, "End of endurance test");
                break;

        case LOAD:
                TST_RESM(TINFO, "Load test");
                rv  LoadTest();
                TST_RESM(TINFO, "End of load test");
                break;

        default:
                TST_RESM(TINFO, "Wrong test case");
                break;
        }

        return rv;
}


/**/
/**/
void   *H264E_AllocateMemory(AVC_EncMemBlock * pMemBlk, void *pAppContext)
{
        void   *ptr;

        // This is currently defined to be malloc. Application specific
        // allocation can be substituted here
        if (pMemBlk->priority < 4 && E_H264E_FAST_MEMORY  pMemBlk->type)
                ptr  alloc_fast(pMemBlk->size);
        else
                ptr  alloc_slow(pMemBlk->size);

        if (!ptr)
        {
                TST_RESM(TWARN, "%s : Can't allocate %d bytes memory", __FUNCTION__, pMemBlk->size);
        }

        DPRINTF("Memory Allocation : %ld bytes of memory @ %p\n", pMemBlk->size, ptr);

        return ptr;
}


/**/
/**/
void H264E_FreeMemory(void *ptr, void *pAppContext)
{
        // This is currently defined to be "free". Application specific
        // de-allocation can be substituted here
        free(ptr);

        DPRINTF("Memory De-allocation : @ %p\n", ptr);
}


/**/
/**/
int RunEncoder(void *ptr)
{
        assert(ptr);

        sH264EncoderHandler *pHandler  (sH264EncoderHandler *) ptr;

        /* Set priority for the PRE_EMPTION test case. */
        if (PRE_EMPTION  gTestappConfig.mTestCase)
        {
                int     nice_inc  (int) ((20.0f / gNumThreads) * pHandler->mIndex);

                if (nice(nice_inc) < 0)
                {
                        TST_RESM(TWARN, "%s : nice(%d) has failed", __FUNCTION__, nice_inc);
                }
        }

        /* Run TestEngine. */
        pHandler->mLtpRetval  TestEngine(pHandler);

        /* Perform bitmatching. */
        const char *fileName1  pHandler->mpParams->mOutputFileName;
        const char *fileName2  pHandler->mpParams->mReferenceFileName;

        if (DoFilesExist(fileName1, fileName2))
        {
                if (!BinCompare(fileName1, fileName2))
                {
                        if (gTestappConfig.mVerbose)
                                TST_RESM(TFAIL, "Thread[%lu] Bitmatch failed (%s vs %s)",
                                         pHandler->mIndex, fileName1, fileName2);
                        pHandler->mLtpRetval  TFAIL;
                }
                else
                {
                        if (gTestappConfig.mVerbose)
                                TST_RESM(TINFO, "Thread[%lu] Bitmatch passed (%s vs %s)",
                                         pHandler->mIndex, fileName1, fileName2);
                }
        }

        /* Kev bitmatching. */
        fileName1  pHandler->mpParams->mReconFileName;
        fileName2  pHandler->mpParams->mReconRefFileName;
        if (pHandler->mpParams->mWriteRecon)
        {
                if (DoFilesExist(fileName1, fileName2))
                {
                        if (!KevCompare(fileName1, fileName2))
                        {
                                if (gTestappConfig.mVerbose)
                                        TST_RESM(TFAIL, "Thread[%lu] Bitmatch failed (%s vs %s)",
                                                 pHandler->mIndex, fileName1, fileName2);
                                pHandler->mLtpRetval  TFAIL;
                        }
                        else
                        {
                                if (gTestappConfig.mVerbose)
                                        TST_RESM(TINFO, "Thread[%lu] Bitmatch passed (%s vs %s)",
                                                 pHandler->mIndex, fileName1, fileName2);
                        }
                }
        }

        /* Give the focus to an incomplete thread. */
        pHandler->mIsThreadFinished  TRUE;
        if (pHandler->mIndex  gThreadWithFocus)
        {
                int     i;

                /* Search a first incomplete thread and assign them focus. */
                for (i  0; i < gNumThreads; ++i)
                {
                        if (!gH264EncHandlers[i].mIsThreadFinished)
                        {
                                gThreadWithFocus  gH264EncHandlers[i].mIndex;
                                break;
                        }
                }
        }

        /* Return LTP result. */
        return pHandler->mLtpRetval;
}


/**/
/**/
int TestEngine(sH264EncoderHandler * pHandler)
{
        assert(pHandler);
        assert(pHandler->mpParams);

        const sHandlerParams *pParams  pHandler->mpParams;
        AVC_VideoEncodeStruct *pEncConfig  &pHandler->mEncConfig;
        IOParams *pIoParams  &pHandler->mIoPars;

        /* Assume the callbacks. */
        pEncConfig->pAVCEAppMemAlloc  H264E_AllocateMemory;
        pEncConfig->pAVCEAppFree  H264E_FreeMemory;
        CALL_H264_ENCODER(AVC_SetDefaultEncodingParameters(pEncConfig),
                          "AVC_SetDefaultEncodingParameters");

        char    encoderFramerate[MAX_STR_LEN];
        char    targetBitrate[MAX_STR_LEN];
        char    writeRecon[MAX_STR_LEN];

        sprintf(encoderFramerate, "%d", pParams->mEncoderFramerate);
        sprintf(targetBitrate, "%d", pParams->mTargetBitRate);
        sprintf(writeRecon, "%d", pParams->mWriteRecon);
        char   *argv[]  {
                "blablabla",
                "-f",
                (char *) pParams->mControlFileName,
                "-p",
                "SourceFilename",
                "",
                (char *) pParams->mInputFileName,
                "-p",
                "BitFilename",
                "",
                (char *) pParams->mOutputFileName,
                "-p",
                "ReconFilename",
                "",
                (char *) pParams->mReconFileName,
                "-p",
                "EncodedFrameRate",
                "",
                encoderFramerate,
                "-p",
                "TargetBitRate",
                "",
                targetBitrate,
                "-p",
                "WriteRecon",
                "",
                writeRecon
        };
        int     argc  27;

        UI_GetUserInput(pIoParams, &pEncConfig->encPars, argc, argv);

        /* Init the IO params. */
        IO_Init(pIoParams);
        pEncConfig->encPars.frameWidth  pIoParams->kevInput.width;
        pEncConfig->encPars.frameHeight  pIoParams->kevInput.height;

        pEncConfig->version  AVCE_CURRENT_VERSION;

        /* Init the encoder. */
        CALL_H264_ENCODER(AVC_InitVideoEncoder(pEncConfig), "AVC_InitVideoEncoder");

        /* Will assign focus, if it is not assigned. */
        if (gThreadWithFocus  -1)
                gThreadWithFocus  pHandler->mIndex;

        /* Main encoding loop */
        while (TRUE)
        {
                if (pEncConfig->status & 1)
                {
                        // Assuming that width and height are multiples of 16 pixels
                        IO_ReadVideoFrame(pIoParams, pEncConfig->nextSourceFrameNum,
                                          &pEncConfig->ycbcrCurrFrame);
                }

                // Write Annex-B specific trace information
                CALL_H264_ENCODER(AVC_EncodeNALUnit(pEncConfig), "AVC_EncodeNALUnit");

                IO_WriteNALUnit(pIoParams, (char *) pEncConfig->pNALUData,
                                pEncConfig->numBytesNALU);

                if (pEncConfig->status & 1)
                {
                        IO_WriteOutputVideo(pIoParams, &pEncConfig->ycbcrReconFrame);

                        if (pEncConfig->nextSourceFrameNum >
                            (pIoParams->endFrameNum - pIoParams->startFrameNum))
                                break;
                        if (pEncConfig->nextSourceFrameNum > pIoParams->kevInput.totalFrames)
                                break;
                }

                /* Increase the frame counter. */
                ++pHandler->mFramesCount;
                PrintProgress(pHandler);
        }

        /* Free the encoder. */
        CALL_H264_ENCODER(AVC_FreeVideoEncoder(pEncConfig), "AVC_FreeVideoEncoder");

        /* Close all IO's we have opened. */
        IO_Close(pIoParams);

        /* Cleanup the handler */
        CleanupHandler(pHandler);

        /* Return succees */
        return TPASS;
}


/**/
/**/
void PrintProgress(sH264EncoderHandler * pHandler)
{
        assert(pHandler);
        static int n  0;

        if (pHandler->mIndex  gThreadWithFocus)
        {
                int     i;
                sH264EncoderHandler *pOtherHandler;

                for (i  0; i < gNumThreads; ++i)
                {
                        pOtherHandler  &gH264EncHandlers[i];
                        printf("Thread[%lu][%lu] ", pOtherHandler->mIndex,
                               pOtherHandler->mFramesCount);
                }
                printf("%c\r", gProgress[(n) % (sizeof(gProgress) - 1)]);
        }
        fflush(stdout);
        ++n;
}



/**/
/**/
void ResetHandler(sH264EncoderHandler * pHandler)
{
        assert(pHandler);

        memset(pHandler, 0, sizeof(sH264EncoderHandler));
        pHandler->mIndex  0;
        pHandler->mIsThreadFinished  FALSE;
}


/**/
/**/
void CleanupHandler(sH264EncoderHandler * pHandler)
{
        assert(pHandler);
        /* Close all IO's we have opened. */
        if (pHandler->mLtpRetval ! TPASS)
                IO_Close(&pHandler->mIoPars);
}


/**/
/**/
int DoFilesExist(const char *fname1, const char *fname2)
{
        FILE   *fstream1  fopen(fname1, "r");

        if (fstream1)
        {
                fclose(fstream1);
                FILE   *fstream2  fopen(fname2, "r");

                if (fstream2)
                {
                        fclose(fstream2);
                        return TRUE;
                }
                else if (strncmp(fname2, NA, MAX_STR_LEN))
                {
                        TST_RESM(TWARN, "File %s not found", fname2);
                }
        }
        else if (strncmp(fname1, NA, MAX_STR_LEN))
        {
                TST_RESM(TWARN, "File %s not found", fname1);
        }
        return FALSE;
}


/**/
/**/
void HogCpu(void)
{
        while (1)
        {
                sqrt(rand());
        }
}


/**/
/**/
void MakeEntry(char entry[WORDS_IN_ENTRY][MAX_STR_LEN], int nEntry)
{
        sHandlerParams *pParams  (sHandlerParams *) malloc(sizeof(sHandlerParams));

        int     n  0;

        strncpy(pParams->mInputFileName, entry[n++], MAX_STR_LEN);
        strncpy(pParams->mOutputFileName, entry[n++], MAX_STR_LEN);
        strncpy(pParams->mReferenceFileName, entry[n++], MAX_STR_LEN);
        strncpy(pParams->mControlFileName, entry[n++], MAX_STR_LEN);

        pParams->mEncoderFramerate  atoi(entry[n++]);
        pParams->mTargetBitRate  atoi(entry[n++]);
        pParams->mWriteRecon  atoi(entry[n++]);

        /* Adjust parameters here... */
        strncpy(pParams->mReconFileName, pParams->mOutputFileName, MAX_STR_LEN);
        strncat(pParams->mOutputFileName, AVC_OUTPUT_SUFFIX, MAX_STR_LEN);
        strncat(pParams->mReconFileName, KEV_RECON_OUTPUT_SUFFIX, MAX_STR_LEN);
        strncpy(pParams->mReconRefFileName, pParams->mReferenceFileName, MAX_STR_LEN);
        if (strncmp(pParams->mReferenceFileName, NA, MAX_STR_LEN))
        {
                strncat(pParams->mReferenceFileName, ".avc", MAX_STR_LEN);
                strncat(pParams->mReconRefFileName, ".kev", MAX_STR_LEN);
        }

        if (pParams->mWriteRecon)
        {
                umask(0);
                mkdir(pParams->mReconFileName, 0777);
                char    tmp[MAX_STR_LEN];

                strncpy(tmp, pParams->mReconFileName, MAX_STR_LEN);
                strncat(tmp, "/y", MAX_STR_LEN);
                umask(0);
                mkdir(tmp, 0777);

                strncpy(tmp, pParams->mReconFileName, MAX_STR_LEN);
                strncat(tmp, "/cr", MAX_STR_LEN);
                umask(0);
                mkdir(tmp, 0777);

                strncpy(tmp, pParams->mReconFileName, MAX_STR_LEN);
                strncat(tmp, "/cb", MAX_STR_LEN);
                umask(0);
                mkdir(tmp, 0777);

                strncpy(tmp, pParams->mReconFileName, MAX_STR_LEN);
                strncat(tmp, "/ts", MAX_STR_LEN);
                umask(0);
                mkdir(tmp, 0777);
        }

        DPRINTF("--> %s\n", __FUNCTION__);
        DPRINTF("pParams->mInputFileName      %s\n", pParams->mInputFileName);
        DPRINTF("pParams->mOutputFileName     %s\n", pParams->mOutputFileName);
        DPRINTF("pParams->mReferenceFileName  %s\n", pParams->mReferenceFileName);
        DPRINTF("pParams->mReconFileName      %s\n", pParams->mReconFileName);
        DPRINTF("pParams->mControlFileName    %s\n", pParams->mControlFileName);
        DPRINTF("pParams->mEncoderFramerate   %d\n", pParams->mEncoderFramerate);
        DPRINTF("pParams->mTargetBitRate      %d\n", pParams->mTargetBitRate);
        DPRINTF("pParams->mWriteRecon         %d\n", pParams->mWriteRecon);

        LList_PushBack(gpParamsList, pParams);
}


/**/
/**/
int NominalFunctionalityTest(void)
{
        sLinkedList *pNode;
        int     i;
        int     rv  TPASS;
        sH264EncoderHandler *pHandler  gH264EncHandlers;

        /* For the each entry */
        for (pNode  gpParamsList, i  0; pNode; pNode  pNode->mpNext, ++i)
        {
                /* Reset the handler. */
                ResetHandler(pHandler);

                /* Get content. */
                pHandler->mpParams  (sHandlerParams *) pNode->mpContent;

                if (gTestappConfig.mVerbose)
                {
                        TST_RESM(TINFO, "Thread[%lu] Input: %s, Output: %s, Reference: %s",
                                 pHandler->mIndex, pHandler->mpParams->mInputFileName,
                                 pHandler->mpParams->mOutputFileName,
                                 pHandler->mpParams->mReferenceFileName);
                }

                /* Run the Encoder. */
                rv + RunEncoder(pHandler);
        }

        return rv;
}



/**/
/**/
int ReLocatabilityTest(void)
{
        sLinkedList *pNode;
        int     i,
                j;
        int     rv  TPASS;
        sH264EncoderHandler *pHandler  gH264EncHandlers;

        /* For the each entry */
        for (pNode  gpParamsList, i  0; pNode; pNode  pNode->mpNext, ++i)
        {
                for (j  0; j < gTestappConfig.mNumIter; ++j)
                {
                        /* Reset the handler. */
                        ResetHandler(pHandler);

                        /* Get content. */
                        pHandler->mpParams  (sHandlerParams *) pNode->mpContent;

                        if (gTestappConfig.mVerbose && j  0)
                        {
                                TST_RESM(TINFO, "Thread[%lu] Input: %s, Output: %s, Reference: %s",
                                         pHandler->mIndex, pHandler->mpParams->mInputFileName,
                                         pHandler->mpParams->mOutputFileName,
                                         pHandler->mpParams->mReferenceFileName);
                        }

                        /* Run the Encoder. */
                        rv + RunEncoder(pHandler);

                        if (gTestappConfig.mVerbose)
                        {
                                TST_RESM(TINFO, "Data memory was relocated");
                        }
                }
        }
        return rv;
}


/**/
/**/
int ReEntranceTest(void)
{
        int     ReEntranceTestCore(sLinkedList * pHead);
        sLinkedList *pHead  gpParamsList;
        int     rv  TPASS;
        int     i;

        while (pHead)
        {
                rv + ReEntranceTestCore(pHead);
                for (i  0; i < gNumThreads && pHead; ++i)
                        pHead  pHead->mpNext;
        }

        return rv;
}


/**/
/**/
int ReEntranceTestCore(sLinkedList * pHead)
{
        assert(pHead);

        sLinkedList *pNode;
        int     i;
        int     rv  TPASS;
        sH264EncoderHandler *pHandler;

        /* Run all bitstreams in separate threads. */
        for (pNode  pHead, i  0; pNode && i < gNumThreads; pNode  pNode->mpNext, ++i)
        {
                pHandler  gH264EncHandlers + i;
                ResetHandler(pHandler);
                pHandler->mIndex  i;

                /* Get content. */
                pHandler->mpParams  (sHandlerParams *) pNode->mpContent;

                if (gTestappConfig.mVerbose)
                {
                        TST_RESM(TINFO, "Thread[%lu] Input: %s, Output: %s, Reference: %s",
                                 pHandler->mIndex, pHandler->mpParams->mInputFileName,
                                 pHandler->mpParams->mOutputFileName,
                                 pHandler->mpParams->mReferenceFileName);
                }

                if (pthread_create(&pHandler->mThreadID, NULL, (void *) &RunEncoder, pHandler))
                {
                        TST_RESM(TWARN, "%s : error creating thread %d", __FUNCTION__, i);
                        return TFAIL;
                }
        }

        /* Wait for the each thread. */
        for (i  0; i < gNumThreads; ++i)
        {
                pHandler  gH264EncHandlers + i;
                pthread_join(pHandler->mThreadID, NULL);
        }
        for (i  0; i < gNumThreads; ++i)
        {
                pHandler  gH264EncHandlers + i;
                rv + pHandler->mLtpRetval;
        }

        return rv;
}


/**/
/**/
int PreEmptionTest(void)
{
        return ReEntranceTest();
}


/**/
/**/
int EnduranceTest(void)
{
        int     i;
        int     rv  TPASS;

        for (i  0; i < gTestappConfig.mNumIter; ++i)
        {
                if (gTestappConfig.mVerbose)
                        TST_RESM(TINFO, "The %d iteration has been started", i + 1);
                rv + NominalFunctionalityTest();
                if (gTestappConfig.mVerbose)
                        TST_RESM(TINFO, "The %d iteration has been completed", i + 1);
        }

        return rv;
}


/**/
/**/
int LoadTest(void)
{
        int     rv  TFAIL;
        pid_t   pid;

        switch (pid  fork())
        {
        case -1:
                TST_RESM(TWARN, "%s : fork failed", __FUNCTION__);
                return rv;
        case 0:
                /* child process */
                HogCpu();
        default:
                /* parent */
                sleep(2);
                rv  NominalFunctionalityTest();
                /* kill child process once decode/encode loop has ended */
                if (kill(pid, SIGKILL) ! 0)
                {
                        TST_RESM(TWARN, "%s : Kill(SIGKILL) error", __FUNCTION__);
                        return rv;
                }
        }
        return rv;
}


/**/
/**/
int BinCompare(const char *fname1, const char *fname2)
{
        int     out,
                ref;
        struct stat fstat_out,
                fstat_ref;
        char   *fptr_out,
               *fptr_ref;
        size_t  filesize;
        int     i;

        if ((out  open(fname1, O_RDONLY)) < 0)
        {
                return FALSE;
        }
        if ((ref  open(fname2, O_RDONLY)) < 0)
        {
                close(out);
                return FALSE;
        }
        fstat(out, &fstat_out);
        fstat(ref, &fstat_ref);
        if (fstat_out.st_size ! fstat_ref.st_size)
        {
                close(out);
                close(ref);
                return FALSE;
        }
        filesize  fstat_out.st_size;
        fptr_out  (char *) mmap(0, filesize, PROT_READ, MAP_SHARED, out, 0);
        if (fptr_out  MAP_FAILED)
        {
                close(out);
                close(ref);
                return FALSE;
        }
        fptr_ref  (char *) mmap(0, filesize, PROT_READ, MAP_SHARED, ref, 0);
        if (fptr_ref  MAP_FAILED)
        {
                close(out);
                close(ref);
                return FALSE;
        }
        close(out);
        close(ref);
        for (i  0; i < filesize; ++i)
        {
                if (*(fptr_ref + i) ! *(fptr_out + i))
                {
                        munmap(fptr_ref, fstat_ref.st_size);
                        munmap(fptr_out, fstat_out.st_size);
                        return FALSE;
                }
        }
        munmap(fptr_ref, filesize);
        munmap(fptr_out, filesize);
        return TRUE;
}

/**/
/**/
int KevCompare(const char *kev1, const char *kev2)
{
        char    fname1[MAX_STR_LEN];
        char    fname2[MAX_STR_LEN];

        strncpy(fname1, kev1, MAX_STR_LEN);
        strncat(fname1, "/y/data", MAX_STR_LEN);

        strncpy(fname2, kev2, MAX_STR_LEN);
        strncat(fname2, "/y/data", MAX_STR_LEN);

        if (DoFilesExist(fname1, fname2))
        {
                if (!BinCompare(fname1, fname2))
                        return FALSE;
        }
        else
                return FALSE;

        strncpy(fname1, kev1, MAX_STR_LEN);
        strncat(fname1, "/cr/data", MAX_STR_LEN);

        strncpy(fname2, kev2, MAX_STR_LEN);
        strncat(fname2, "/cr/data", MAX_STR_LEN);

        if (DoFilesExist(fname1, fname2))
        {
                if (!BinCompare(fname1, fname2))
                        return FALSE;
        }
        else
                return FALSE;

        strncpy(fname1, kev1, MAX_STR_LEN);
        strncat(fname1, "/cb/data", MAX_STR_LEN);

        strncpy(fname2, kev2, MAX_STR_LEN);
        strncat(fname2, "/cb/data", MAX_STR_LEN);

        if (DoFilesExist(fname1, fname2))
        {
                if (!BinCompare(fname1, fname2))
                        return FALSE;
        }
        else
                return FALSE;

        return TRUE;
}
