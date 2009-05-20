/*
 * Copyright (C) 2004, Freescale Semiconductor, Inc. All Rights Reserved THIS SOURCE CODE IS
 * CONFIDENTIAL AND PROPRIETARY AND MAY NOT BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
 * Freescale Semiconductor, Inc. */

/**
@file beatnik_midi_test.c

@brief VTE C header template

@par Portability:
        Indicate if this module is portable to other compilers or platforms.
        If not, indicate specific reasons why is it not portable.
*/

/* REVISION HISTORY 

Author (core ID)      Date         CR Number    Description of Changes
-------------------   ----------   ----------   ------------------------------
D.Simakov / smkd001c  11/08/2005   TLSbo53248   Initial version
D.Simakov / smkd001c  09/09/2005   TLSbo53248   Multithreaded tests was fixed
D.Simakov / smkd001c  09/12/2005   TLSbo57009   Improved
D.Simakov / smkd001c  13/12/2005   TLSbo57009   LinuxOS PAL
S. V-Guilhou/svan01c  20/12/2005   TLSbo57360   Fix unexpected test exit
D.Simakov             02/05/2006   TLSbo66145   Hardware engages were removed
D.Simakov             21/07/2006   TLSbo67569   support full bitmatch
*/

/*
                                        INCLUDE FILES
*/
#include <pthread.h>
#include <math.h>
#include <ctype.h>

/* Verification Test Environment Include Files */
#include "beatnik_midi_test.h"
/* Beatnik MIDI Engine API. */
#include <mobileBAE.h>
#include <PAL_PlatformOptions.h>

/*
                                        LOCAL MACROS
*/
#define MAX_THREADS 2
#define SAFE_DELETE(p) {if(p){free(p);pNULL;}}
#define NA "n/a"
#define M(m){printf("<<<--- %s --->>>\n",m);fflush(stdout);}
#define DEFAULT_BANK_PATH "Mobile_0100_base_classic.dls"
#define Sleep(time) usleep((time) * 1000)

/**********************************************************************
* Macro name:  CALL_BEATNIK_MIDI()
* Description: Macro checks for any error in function execution
               based on the return value. In case of error, the function exits.
**********************************************************************/
#define CALL_BEATNIK_MIDI(BeatnikMidiRoutine, name)   \
{\
    pHandler->mLastMBResult  BeatnikMidiRoutine; \
    if( (pHandler->mLastMBResult ! mbNo_Err) )\
    {\
        TST_RESM( TWARN, "%s : %s fails #%d [File: %s, line: %d]", __FUNCTION__, name, pHandler->mLastMBResult, __FILE__, __LINE__ );\
        return TFAIL;\
        }\
}

#define TST_RESM_ERROR(msg) TST_RESM( TWARN, "%s : %s fails #%d [File: %s, line: %d]", __FUNCTION__, msg, pHandler->mLastMBResult, __FILE__, __LINE__ )

#define IS_CURRENT_TESTCASE_MULTITHREADED (RE_ENTRANCE  gTestappConfig.mTestCase || PRE_EMPTION  gTestappConfig.mTestCase)

#define TST_RESM(s,format,...) \
{\
    if( IS_CURRENT_TESTCASE_MULTITHREADED ) \
        pthread_mutex_lock( &gMutex );\
    tst_resm((s), (format), ##__VA_ARGS__);\
    if( IS_CURRENT_TESTCASE_MULTITHREADED ) \
        pthread_mutex_unlock( &gMutex );\
}

#define LOCK() \
{\
    if( IS_CURRENT_TESTCASE_MULTITHREADED ) \
    {\
        pthread_mutex_lock( &gMutex );\
    }\
}

#define UNLOCK()\
{\
    if( IS_CURRENT_TESTCASE_MULTITHREADED )\
    {\
        pthread_mutex_unlock( &gMutex ); \
    }\
}

#define result pHandler->mLastMBResult

/*
                          LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
*/
typedef struct
{
        char    mInputFileName[MAX_STR_LEN];
        char    mOutputFileName[MAX_STR_LEN];
        char    mReferenceFileName[MAX_STR_LEN];

        mbBool  mDefaultSystem;
        mbBool  mChorus;
        mbBool  mReverb;
        mbInt32 mChannels;
        mbInt32 mBitDepth;
        mbInt32 mSampleRate;
        mbInt16 mMidiVoices;
        mbBool  mStreamed;
        mbMilliUnit mBufTime;
        const char *mSndBank;
        char    mSoundBank[MAX_STR_LEN];
        mbBool  mEnableAutoDisconnect;
        mbMilliUnit mHardwareVolume;
        mbUInt32 mPlayCount;
        mbBool  mIsLooped;
        mbInt32 mLoopStart;
        mbInt32 mLoopEnd;
        mbInt32 mLoopCount;
        mbMilliUnit mTranspose;
        mbBool  mCreateAndLoadPlayer;
        mbInt32 mVolume;

} sHandlerParams;

typedef struct
{
        unsigned long mIndex;
        sHandlerParams *mpParams;
        FILE   *mpOutStream;
        unsigned long mFramesCount;
        mbSystemID mSystemID;
        mbResult mLastMBResult;
        pthread_t mThreadID;
        int     mIsThreadFinished;
        int     mLtpRetval;
        int     mIdleReq;
} sBeatnikMidiHandler;

typedef struct
{
        const char *mExtension;
        mbFileType mFileType;
        mbBool  mDigital;
} sFileRecognizerEntry;

typedef struct
{
        char    riff_tag[4];
        unsigned riff_len;
        char    wave_tag[4];
        char    fmt_tag[4];
        unsigned fmt_len;
        unsigned short wFormatTag;
        unsigned short wChannels;
        unsigned dwSamplesPerSec;
        unsigned dwAvgBytesPerSec;
        unsigned short wBlockAlign;
        unsigned short wBitsPerSample;
        char    data_tag[4];
        unsigned data_len;
} header_t;

/*
                                       LOCAL CONSTANTS
*/
static const sFileRecognizerEntry gFileRecognizerTable[]  {
        {".aif", mbFileType_AIFF, 1},
        {".aifc", mbFileType_AIFF, 1},
        {".aiff", mbFileType_AIFF, 1},
        {".au", mbFileType_AU, 1},
        {".snd", mbFileType_AU, 1},
        {".bbi", mbFileType_BBI, 0},
        {".bsb", mbFileType_BSB, 0},
        {".dls", mbFileType_DLS_GS, 0},
        {".jts", mbFileType_JTS, 0},
        {".imy", mbFileType_iMelody, 0},
        {".mid", mbFileType_SMF, 0},
        {".midi", mbFileType_SMF, 0},
        {".mmf", mbFileType_SMAF, 0},
        {".mp", mbFileType_MPEG, 1},
        {".mp2", mbFileType_MPEG, 1},
        {".mp3", mbFileType_MPEG, 1},
        {".mpa", mbFileType_MPEG, 1},
        {".mpga", mbFileType_MPEG, 1},
        {".mxmf", mbFileType_XMF, 0},
        {".rmf", mbFileType_RMF, 0},
        {".rng", mbFileType_SMS, 0},
        {".rtx", mbFileType_RTX, 0},
        {".sms", mbFileType_SMS, 0},
        {".wav", mbFileType_WAVE, 1},
        {".xmf", mbFileType_XMF, 0},
        {NULL, mbFileType_Undefined, 1}
};

/*
                                       LOCAL VARIABLES
*/
static sBeatnikMidiHandler gBMidiHandlers[MAX_THREADS];
static int gNumThreads  1;
static sLinkedList *gpParamsList  NULL;
static pthread_mutex_t gMutex;
static mbSystemID gSystemID  0;
static pthread_once_t gIsSystemCreated  PTHREAD_ONCE_INIT;
static sBeatnikMidiHandler *gpCurHandler  0;

/*
                                       GLOBAL CONSTANTS
*/

/*
                                       GLOBAL VARIABLES
*/

/*
                                   LOCAL FUNCTION PROTOTYPES
*/
void    MBAE_MixerOutputBufferCB(void *pContext, void *pSamples,
                                 mbAudioFormat sampleFormat, mbUInt32 lengthInFrames);

int     RunMidiEngine(void *ptr);
int     TestEngine(sBeatnikMidiHandler * pHandler);
void    ResetHandler(sBeatnikMidiHandler * pHandler);
void    CleanupHandler(sBeatnikMidiHandler * pHandler);
int     DoBitmatch(sBeatnikMidiHandler * pHandler);
int     DoFilesExist(const char *fname1, const char *fname2);
void    HogCpu(void);
void    MakeEntry(char entry[WORDS_IN_ENTRY][MAX_STR_LEN], int nEntry);

/* test cases */
int     NominalFunctionalityTest(void);
int     RobustnessTest(void);
int     ReLocatabilityTest(void);
int     ReEntranceTest(void);
int     PreEmptionTest(void);
int     EnduranceTest(void);
int     LoadTest(void);

/* helper */
void    CreateSystem(void);
void    ResetHandlerParams(sHandlerParams * pParams);
int     StrICmp(const char *s1, const char *s2);
void    WriteWAVHeader(mbSystemID sysID, FILE * outputFP);
void    WriteWAVTrailer(FILE * outputFP);

/*
                                       LOCAL FUNCTIONS
*/

/**/
/**/
int VT_beatnik_midi_setup(void)
{
        pthread_mutex_init(&gMutex, NULL);

        int     i;

        /* Reset all handlers. */
        for (i  0; i < MAX_THREADS; ++i)
                ResetHandler(gBMidiHandlers + i);

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
int VT_beatnik_midi_cleanup(void)
{
        pthread_mutex_destroy(&gMutex);

        if (gpParamsList)
                LList_Delete(gpParamsList);

        int     i;

        for (i  0; i < MAX_THREADS; ++i)
        {
                CleanupHandler(gBMidiHandlers + i);
                ResetHandler(gBMidiHandlers + i);
        }

        return TPASS;
}


/**/
/**/
int VT_beatnik_midi_test(void)
{
        int     rv  TFAIL;

        switch (gTestappConfig.mTestCase)
        {
        case NOMINAL_FUNCTIONALITY:
                rv  NominalFunctionalityTest();
                break;

        case ROBUSTNESS:
                rv  RobustnessTest();
                break;

        case RELOCATABILITY:
                rv  ReLocatabilityTest();
                break;

        case RE_ENTRANCE:
                rv  ReEntranceTest();
                break;

        case PRE_EMPTION:
                rv  PreEmptionTest();
                break;

        case ENDURANCE:
                rv  EnduranceTest();
                break;

        case LOAD:
                rv  LoadTest();
                break;

        default:
                TST_RESM(TINFO, "Wrong test case");
                break;
        }

        return rv;
}


/**/
/**/
void MBAE_MixerOutputBufferCB(void *pContext,
                              void *pSamples, mbAudioFormat sampleFormat, mbUInt32 lengthInFrames)
{
        sBeatnikMidiHandler *pHandler  (sBeatnikMidiHandler *) pContext;

        assert(pHandler);
        assert(pHandler->mpOutStream);
        size_t  sz;

        sz  sampleFormat.mBitDepth * sampleFormat.mChannels * lengthInFrames / 8;
        fwrite(pSamples, 1, sz, pHandler->mpOutStream);
        fflush(pHandler->mpOutStream);
}


/**/
/**/
int RunMidiEngine(void *ptr)
{
        assert(ptr);

        sBeatnikMidiHandler *pHandler  (sBeatnikMidiHandler *) ptr;

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

        /* Do bit-matching. */
        const char *fileName1  pHandler->mpParams->mOutputFileName;
        const char *fileName2  pHandler->mpParams->mReferenceFileName;

        if (DoFilesExist(fileName1, fileName2))
        {
                if (!DoBitmatch(pHandler))
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

        /* Return LTP result. */
        return pHandler->mLtpRetval;
}


/**/
/**/
int TestEngine(sBeatnikMidiHandler * pHandler)
{
        assert(pHandler);
        assert(pHandler->mpParams);

        sHandlerParams *pParams  pHandler->mpParams;

    /***********************/
        /* Create the streams. */
    /***********************/
        pHandler->mpOutStream  fopen(pParams->mOutputFileName, "w+b");
        if (!pHandler->mpOutStream)
        {
                tst_brkm(TBROK, (void (*)()) VT_beatnik_midi_cleanup,
                         "%s : Can't create output file %s",
                         __FUNCTION__, pParams->mOutputFileName);
        }
        WriteWAVHeader(pHandler->mSystemID, pHandler->mpOutStream);

    /**********************/
        /* Initialize system. */
    /**********************/
#ifdef PAL_ENABLE_DIGITAL_AUDIO
#define SOUND_VOICE_COUNT 1
#else
#define SOUND_VOICE_COUNT 0
#endif

        pthread_once(&gIsSystemCreated, CreateSystem);
        pHandler->mSystemID  gSystemID;

        mbBool  needServiceIdle  pHandler->mIdleReq;

    /*************************************/
        /* Set audio format on audio output. */
    /*************************************/
        mbAudioFormat audioFmt;

        audioFmt.mBitDepth  pParams->mBitDepth;
        audioFmt.mChannels  pParams->mChannels;
        audioFmt.mSampleRate  pParams->mSampleRate;
        audioFmt.mSampleType  mbSampleType_Signed;
        CALL_BEATNIK_MIDI(mbSetOutputHardwareAudioFormat(pHandler->mSystemID, &audioFmt),
                          "mbSetOutputHardwareAudioFormat");


    /************************************************************/
        /* By setting the bank file, we set which renderer we want. */
    /************************************************************/
        mbFileType sndBankType  mbFileType_Undefined;

        if (strstr(pParams->mSndBank, ".dls") || strstr(pParams->mSndBank, ".DLS")
            || strstr(pParams->mSndBank, ".dlp"))
        {
                sndBankType  mbFileType_DLS_GM2;
        }
        else if (strstr(pParams->mSndBank, ".rmf") || strstr(pParams->mSndBank, ".hsb"))
        {
                sndBankType  mbFileType_RMF;
        }
        else
        {
                TST_RESM_ERROR("");
                return TFAIL;
        }

    /********************************/
        /* Connect to the audio output. */
    /********************************/
        if (pParams->mEnableAutoDisconnect)
        {
                CALL_BEATNIK_MIDI(mbEnableAutoDisconnect(pHandler->mSystemID, mbTrue),
                                  "mbEnableAutoDisconnect");
        }

    /***************/
        /* Set volume. */
    /***************/
        if (pParams->mHardwareVolume ! -1)
        {
                CALL_BEATNIK_MIDI(mbSetOutputHardwareVolume
                                  (pHandler->mSystemID, pParams->mHardwareVolume),
                                  "mbSetOutputHardwareVolume");
        }

        CALL_BEATNIK_MIDI(mbSetMixerOutputBufferCallback
                          (pHandler->mSystemID, MBAE_MixerOutputBufferCB, pHandler),
                          "mbSetMixerOutputBufferCallback");


    /*****************************************************/
        /* Determine the type of file we are trying to play. */
    /*****************************************************/
        mbBool  isSound;
        mbFileType fileType  mbFileType_Undefined;
        int     len;
        const sFileRecognizerEntry *pEntry;

        len  strlen(pParams->mInputFileName);
        for (pEntry  gFileRecognizerTable; pEntry->mExtension; ++pEntry)
        {
                int     extensionLength  strlen(pEntry->mExtension);

                if (len > extensionLength &&
                    !StrICmp(&pParams->mInputFileName[len - extensionLength], pEntry->mExtension))
                {
                        fileType  pEntry->mFileType;
                        isSound  pEntry->mDigital;
                        break;
                }
        }

    /****************************************************************************/
        /* Create the appropriate type of object, initialize it, and load the file. */
    /****************************************************************************/

        mbObjectID playable  0;
        mbObjectID bSound  0;
        mbObjectID song  0;
        int     pCount  pParams->mPlayCount;
        int     lrepeats  1;

        if (isSound)
        {
                bSound  mbCreateAudioFilePlayer(pHandler->mSystemID, pParams->mStreamed, &result);
                CALL_BEATNIK_MIDI(result, "mbCreateAudioFilePlayer");

                playable  bSound;

                CALL_BEATNIK_MIDI(mbLoadFromFile(bSound,
                                                 (const char *) pParams->mInputFileName,
                                                 fileType), "mbLoadFromFile");

                CALL_BEATNIK_MIDI(mbPreroll(bSound), "mbPreroll");

                if (pParams->mStreamed)
                {
                        CALL_BEATNIK_MIDI(mbSetBufferTime(bSound, pParams->mBufTime),
                                          "mbSetBufferTime");
                }

                // Set some state.
                if (pParams->mIsLooped)
                {
                        mbSampleInfo sInfo;

                        CALL_BEATNIK_MIDI(mbEnableLooping(bSound, mbTrue), "mbEnableLooping");
                        CALL_BEATNIK_MIDI(mbGetSampleInfo(bSound, &sInfo), "mbGetSampleInfo");
                        if (-1  pParams->mLoopStart)
                                pParams->mLoopStart 
                                    (sInfo.mStartLoop < sInfo.mEndLoop) ? sInfo.mStartLoop : 0;
                        if (-1  pParams->mLoopEnd)
                                pParams->mLoopEnd 
                                    (sInfo.mStartLoop <
                                     sInfo.mEndLoop) ? sInfo.mEndLoop : sInfo.mWaveFrames - 1;
                        else if (pParams->mLoopEnd < 0)
                                pParams->mLoopEnd  sInfo.mWaveFrames + pParams->mLoopEnd;
                        if (pParams->mLoopStart < pParams->mLoopEnd)
                        {
                                CALL_BEATNIK_MIDI(mbSetLoopPoints
                                                  (bSound, pParams->mLoopStart, pParams->mLoopEnd),
                                                  "mbSetLoopPoints");
                        }
                        else
                        {
                                CALL_BEATNIK_MIDI(mbSetLoopPoints(bSound, 0, sInfo.mWaveFrames - 1),
                                                  "mbSetLoopPoints");
                        }
                        CALL_BEATNIK_MIDI(mbSetLoopCount(bSound, pParams->mLoopCount),
                                          "mbSetLoopCount");
                }
                CALL_BEATNIK_MIDI(mbSetVolume(bSound, pParams->mVolume), "mbSetVolume");
                CALL_BEATNIK_MIDI(mbSetTransposeFactor(bSound, pParams->mTranspose),
                                  "mbSetTransposeFactor");
        }
        else    /* if(sound) */
        {
                if (pParams->mSndBank && !pParams->mDefaultSystem)
                {
                        CALL_BEATNIK_MIDI(mbAddBankFromFile(pHandler->mSystemID,
                                                            pParams->mSndBank, sndBankType, 0),
                                          "mbAddBankFromFile");
                        pParams->mSndBank  NULL;
                }


                if (pParams->mCreateAndLoadPlayer)
                {
                        song  mbCreateAndLoadPlayerFromFile(pHandler->mSystemID,
                                                             pParams->mInputFileName,
                                                             mbFalse, &result);
                        CALL_BEATNIK_MIDI(result, "mbCreateAndLoadPlayerFromFile");
                }
                else
                {
                        song  mbCreateMidiFilePlayer(pHandler->mSystemID, &result);
                        CALL_BEATNIK_MIDI(result, "mbCreateMidiFilePlayer");
                        CALL_BEATNIK_MIDI(mbLoadFromFile(song,
                                                         (const char *) pParams->mInputFileName,
                                                         fileType), "mbLoadFromFile");
                }

                playable  song;

                // We preroll the song to allow the possibility of overriding settings.
                CALL_BEATNIK_MIDI(mbPreroll(song), "mbPreroll");

                // Set some state.
                CALL_BEATNIK_MIDI(mbSetVolume(song, pParams->mVolume), "mbSetVolume");
                if (pParams->mMidiVoices > 0)
                {
                        mbObjectID synth  mbGetMidiSynth(song, 0, &result);

                        // force different voice count
                        if (synth)
                        {
                                CALL_BEATNIK_MIDI(mbSetSynthVoiceLimit(synth, pParams->mMidiVoices),
                                                  "mbSetSynthVoiceLimit");
                                CALL_BEATNIK_MIDI(mbDestroy(synth), "mbDestroy");
                        }
                }
                if (pParams->mIsLooped)
                {
                        CALL_BEATNIK_MIDI(mbEnableLooping(song, mbTrue), "mbEnableLooping");
                        CALL_BEATNIK_MIDI(mbSetLoopCount(song, pParams->mLoopCount),
                                          "mbSetLoopCount");
                }

        }

        if (pParams->mMidiVoices > 0)
        {
                // force different voice count
                CALL_BEATNIK_MIDI(mbSetRendererVoiceLimit(pHandler->mSystemID,
                                                          pParams->mMidiVoices, SOUND_VOICE_COUNT),
                                  "mbSetRendererVoiceLimit");
        }

        mbEnableChorus(pHandler->mSystemID, (mbBool) (isSound ? mbFalse : pParams->mChorus));
        mbEnableReverb(pHandler->mSystemID, (mbBool) (isSound ? mbFalse : pParams->mReverb));

        fflush(stdout);

        while (lrepeats--)
        {
                // Start the object playing.
                CALL_BEATNIK_MIDI(mbStart(playable), "mbStart");
                // CALL_BEATNIK_MIDI(mbEngageOutputHardware(pHandler->mSystemID),
                // "mbEngageOutputHardware");
                while (pCount-- > 0 &&
                       (mbIsPlaying(playable, &result) ||
                        mbIsRendererActive(pHandler->mSystemID, &result)) && result  mbNo_Err)
                {
                        if (needServiceIdle)
                                mbSystemServiceIdle(pHandler->mSystemID);
                        Sleep(10);
                }
        }

        // Now clean up.
        if (playable)
        {
                CALL_BEATNIK_MIDI(mbDestroy(playable), "mbDestroy");
        }

        WriteWAVTrailer(pHandler->mpOutStream);

        /* Cleanup the handler */
        CleanupHandler(pHandler);

        /* Return succees */
        return TPASS;
}


/**/
/**/
void ResetHandler(sBeatnikMidiHandler * pHandler)
{
        assert(pHandler);

        memset(pHandler, 0, sizeof(sBeatnikMidiHandler));
        pHandler->mIndex  0;
        pHandler->mIsThreadFinished  FALSE;
}


/**/
/**/
void CleanupHandler(sBeatnikMidiHandler * pHandler)
{
        assert(pHandler);

        if (pHandler->mSystemID)
        {
                mbSetMixerOutputBufferCallback(pHandler->mSystemID, NULL, NULL);
                // mbDisengageOutputHardware(pHandler->mSystemID);
                mbDestroySystem(pHandler->mSystemID);
                pHandler->mSystemID  0;
        }

        /* to perform after because the output file pointer was being accessed by
         * mbSetMixerOutputBufferCallback */

        if (pHandler->mpOutStream)
        {
                fclose(pHandler->mpOutStream);
                pHandler->mpOutStream  NULL;
        }
}


/**/
/**/
int RemoveZeroes(sBeatnikMidiHandler * pHandler)
{
        assert(pHandler);
        assert(pHandler->mpParams);

        char    tmpFileName[MAX_STR_LEN];

        sprintf(tmpFileName, "~%s", pHandler->mpParams->mOutputFileName);
        FILE   *pTmpFile  fopen(tmpFileName, "wb");

        if (!pTmpFile)
                return FALSE;

        FILE   *pOutFile  fopen(pHandler->mpParams->mOutputFileName, "rb");

        if (!pOutFile)
        {
                fclose(pTmpFile);
                return FALSE;
        }

        char    val;

        fseek(pOutFile, 0, SEEK_END);
        long    startPos  0,
            endPos  ftell(pOutFile);

        fseek(pOutFile, 0, SEEK_SET);
        for (;;)
        {
                fread(&val, 1, 1, pOutFile);
                if (val)
                        break;
                ++startPos;
        }
        fseek(pOutFile, -1, SEEK_END);
        for (;;)
        {
                fread(&val, 1, 1, pOutFile);
                fseek(pOutFile, -2, SEEK_CUR);
                if (val)
                        break;
                --endPos;
        }

        fseek(pOutFile, startPos, SEEK_SET);
        long    i;

        for (i  0; i < (endPos - startPos); ++i)
        {
                fread(&val, 1, 1, pOutFile);
                fwrite(&val, 1, 1, pTmpFile);
        }

        fclose(pTmpFile);
        fclose(pOutFile);

        pTmpFile  fopen(tmpFileName, "wb");
        pOutFile  fopen(pHandler->mpParams->mOutputFileName, "wb");
        while (!feof(pTmpFile))
        {
                fread(&val, 1, 1, pTmpFile);
                fwrite(&val, 1, 1, pOutFile);
        }

        fclose(pTmpFile);
        fclose(pOutFile);
        return TRUE;
}


/**/
/**/
int DoBitmatch(sBeatnikMidiHandler * pHandler)
{
        assert(pHandler);
        assert(pHandler->mpParams);

        RemoveZeroes(pHandler);

        const char *fname1  pHandler->mpParams->mOutputFileName;
        const char *fname2  pHandler->mpParams->mReferenceFileName;

        int     out,
                ref;
        struct stat fstat_out,
                fstat_ref;
        char   *fptr_out,
               *fptr_ref;
        size_t  filesize;
        int     i;
        int     n  0;

        if ((out  open(fname1, O_RDONLY)) < 0)
        {
                tst_resm(TWARN, "problem to open %s", fname1);
                return FALSE;
        }
        if ((ref  open(fname2, O_RDONLY)) < 0)
        {
                tst_resm(TWARN, "problem to open %s", fname2);
                close(out);
                return FALSE;
        }

        fstat(out, &fstat_out);
        fstat(ref, &fstat_ref);
        if (fstat_out.st_size ! fstat_ref.st_size)
        {
                tst_resm(TWARN, "different size: %d  vs %d", fstat_out.st_size, fstat_ref.st_size);
                close(out);
                close(ref);
                return FALSE;
        }

        filesize  fstat_out.st_size;
        fptr_out  (char *) mmap(0, filesize, PROT_READ, MAP_SHARED, out, 0);
        if (fptr_out  MAP_FAILED)
        {
                tst_resm(TWARN, "mmap failure for out file");
                close(out);
                close(ref);
                return FALSE;
        }

        fptr_ref  (char *) mmap(0, filesize, PROT_READ, MAP_SHARED, ref, 0);
        if (fptr_ref  MAP_FAILED)
        {
                tst_resm(TWARN, "mmap failure for ref file");
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
                        ++n;
                }
        }

        munmap(fptr_ref, filesize);
        munmap(fptr_out, filesize);
        return (float) 100 *((float) n / filesize) < 5.0f;
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
                else if (StrICmp(fname2, NA))
                {
                        if (gTestappConfig.mVerbose)
                        {
                                TST_RESM(TWARN, "%s not found", fname2);
                        }
                }
        }
        else if (!StrICmp(fname1, NA))
        {
                if (gTestappConfig.mVerbose)
                {
                        TST_RESM(TWARN, "%s not found", fname1);
                }
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

        assert(pParams);

        ResetHandlerParams(pParams);

        int     n  0;

        strncpy(pParams->mInputFileName, entry[n++], MAX_STR_LEN);
        strncpy(pParams->mOutputFileName, entry[n++], MAX_STR_LEN);
        strncpy(pParams->mReferenceFileName, entry[n++], MAX_STR_LEN);
        pParams->mMidiVoices  atoi(entry[n++]);
        pParams->mSampleRate  atoi(entry[n++]);
        strncpy(pParams->mSoundBank, entry[n++], MAX_STR_LEN);
        pParams->mSndBank  pParams->mSoundBank;


        /* Adjust parameters here... */
        if (!DoFilesExist(pParams->mInputFileName, pParams->mInputFileName))
        {
                tst_brkm(TBROK, (void (*)()) VT_beatnik_midi_cleanup,
                         "The input file %s does not exist. Check the config[file: %s, line: %d]",
                         pParams->mInputFileName, gTestappConfig.mConfigFilename, nEntry);
        }

        LList_PushBack(gpParamsList, pParams);
}


/**/
/**/
int NominalFunctionalityTest(void)
{
        sLinkedList *pNode;
        int     i;
        int     rv  TPASS;
        sBeatnikMidiHandler *pHandler  gBMidiHandlers;

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
                rv + RunMidiEngine(pHandler);
                CleanupHandler(pHandler);
        }

        return rv;
}


/**/
/**/
int RobustnessTest(void)
{
        sLinkedList *pNode;
        int     i;
        int     rv  TPASS;
        sBeatnikMidiHandler *pHandler  gBMidiHandlers;

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
                int     res  RunMidiEngine(pHandler);

                if (TPASS  res)
                {
                        if (gTestappConfig.mVerbose)
                        {
                                TST_RESM(TWARN, "Robustness to %s failed",
                                         pHandler->mpParams->mInputFileName);
                        }
                        rv  TFAIL;
                }
                else
                {
                        if (gTestappConfig.mVerbose)
                        {
                                TST_RESM(TPASS, "Robustness to %s passed",
                                         pHandler->mpParams->mInputFileName);
                        }
                }
                CleanupHandler(pHandler);
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
        sBeatnikMidiHandler *pHandler  gBMidiHandlers;

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

                        /* Run the MidiEngine. */
                        rv + RunMidiEngine(pHandler);
                        CleanupHandler(pHandler);

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
                {
                        ResetHandler(&gBMidiHandlers[i]);
                        gBMidiHandlers[i].mIndex  i;
                        pHead  pHead->mpNext;
                }
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
        sBeatnikMidiHandler *pHandler;

        /* Run all bitstreams in separate threads. */
        for (pNode  pHead, i  0; pNode && i < gNumThreads; pNode  pNode->mpNext, ++i)
        {
                pHandler  gBMidiHandlers + i;
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

                if (pthread_create(&pHandler->mThreadID, NULL, (void *) &RunMidiEngine, pHandler))
                {
                        TST_RESM(TWARN, "%s : error creating thread %d", __FUNCTION__, i);
                        return TFAIL;
                }
        }

        /* Wait for the each thread. */
        for (i  0; i < gNumThreads; ++i)
        {
                pHandler  gBMidiHandlers + i;
                pthread_join(pHandler->mThreadID, NULL);
        }
        for (i  0; i < gNumThreads; ++i)
        {
                pHandler  gBMidiHandlers + i;
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
void ResetHandlerParams(sHandlerParams * pParams)
{
        assert(pParams);

        pParams->mDefaultSystem  mbTrue;
        pParams->mChorus  mbFalse;
        pParams->mReverb  mbFalse;
#ifdef PAL_ENABLE_STEREO_OUTPUT
        pParams->mChannels  2;
#else
        pParams->mChannels  1;
#endif
        pParams->mBitDepth  16;
        pParams->mSampleRate  44100;
        pParams->mMidiVoices  16;
        pParams->mStreamed  mbFalse;
        pParams->mBufTime  500;
        pParams->mSndBank  DEFAULT_BANK_PATH;
        pParams->mEnableAutoDisconnect  mbFalse;
        pParams->mHardwareVolume  -1;
        pParams->mPlayCount  99999999;
        pParams->mIsLooped  mbFalse;
        pParams->mLoopStart  -1;
        pParams->mLoopEnd  -1;
        pParams->mLoopCount  0;
        pParams->mTranspose  1000;
        pParams->mCreateAndLoadPlayer  mbFalse;
        pParams->mVolume  1000;
}


/**/
/**/
int StrICmp(const char *s1, const char *s2)
{
        for (;;)
        {
                int     c1  *s1;
                int     c2  *s2;

                if (isupper(c1))
                        c1  tolower(c1);
                if (isupper(c2))
                        c2  tolower(c2);
                if (c1 ! c2)
                        return c1 - c2;
                if (c1  '\0')
                        break;
                ++s1;
                ++s2;
        }
        return 0;
}

/**/
/**/
void WriteWAVHeader(mbSystemID sysID, FILE * outputFP)
{
        header_t h;
        mbAudioFormat format;

        mbGetOutputHardwareAudioFormat(sysID, &format);
        h.riff_tag[0]  'R';
        h.riff_tag[1]  'I';
        h.riff_tag[2]  'F';
        h.riff_tag[3]  'F';
        h.riff_len  0;
        h.wave_tag[0]  'W';
        h.wave_tag[1]  'A';
        h.wave_tag[2]  'V';
        h.wave_tag[3]  'E';
        h.fmt_tag[0]  'f';
        h.fmt_tag[1]  'm';
        h.fmt_tag[2]  't';
        h.fmt_tag[3]  ' ';
        h.fmt_len  16;
        h.wFormatTag  1;
        h.wChannels  format.mChannels;
        h.dwSamplesPerSec  format.mSampleRate;
        h.dwAvgBytesPerSec  format.mSampleRate * format.mBitDepth * format.mChannels / 8;
        h.wBlockAlign  format.mBitDepth * format.mChannels / 8;
        h.wBitsPerSample  format.mBitDepth;
        h.data_tag[0]  'd';
        h.data_tag[1]  'a';
        h.data_tag[2]  't';
        h.data_tag[3]  'a';
        h.data_len  0;
        fwrite(&h, sizeof(h), 1, outputFP);
}

/**/
/**/
void WriteWAVTrailer(FILE * outputFP)
{
        header_t h;
        size_t  size;

        size  ftell(outputFP);
        rewind(outputFP);
        fread(&h, sizeof(h), 1, outputFP);
        rewind(outputFP);
        h.riff_len  size - 8;
        h.data_len  size - sizeof(h);
        fwrite(&h, sizeof(h), 1, outputFP);
}

void CreateSystem(void)
{
        sBeatnikMidiHandler *pHandler  gpCurHandler;

        assert(pHandler);
        assert(pHandler->mpParams);

        if (pHandler->mpParams->mDefaultSystem)
                gSystemID  mbCreateDefaultSystem(NULL, &result);
        else
                gSystemID  mbCreateSystem(pHandler->mpParams->mMidiVoices,
                                           SOUND_VOICE_COUNT, mbFalse, &result);
        if (result ! mbNo_Err && result ! mbServiceIdle_Required)
        {
                TST_RESM_ERROR(pHandler->mpParams->
                               mDefaultSystem ? "mbCreateDefaultSystem" : "mbCreateSystem");
        }

        pHandler->mIdleReq  (result  mbServiceIdle_Required);
}
