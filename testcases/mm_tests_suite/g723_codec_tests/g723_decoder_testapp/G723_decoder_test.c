/* / Copyright (C) 2004, Freescale Semiconductor, Inc. All Rights Reserved THIS SOURCE CODE IS
 * CONFIDENTIAL AND PROPRIETARY AND MAY NOT BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
 * Freescale Semiconductor, Inc. */

/**
@file G723_decoder_test.c

@brief VTE C source template

Description of the file

@par Portability:
        Indicate if this module is portable to other compilers or platforms.
        If not, indicate specific reasons why is it not portable.
*/

/* REVISION HISTORY 

Author (core ID)      Date         CR Number    Description of Changes
-------------------   ----------   ----------   ------------------------------
Filinova Natalya      15/02/2005   TLSbo47117   BRIEF desc. of changes
Filinova Natalya      28/02/2005   TLSbo47117   BRIEF desc. of changes
Delaspre/rc149c       07/12/2004   TLSbo40142   update copyrights with Freescale
D.Simakov/smkd001c    15/04/2005   TLSbo47117   Pre-emptivity test case was added
D.Simakov/smkd001c    24/10/2005   TLSbo57009   Default output dir was fixed : output -> g723d_output.
                                                Re-locatability test was fixed.
D.Simakov/smkd001c    20/12/2005   TLSbo60690   Pre-emption test case was fixed.
D.Simakov             17/04/2006   TLSbo66146   L_FRAME -> G723_L_FRAME
*/

/* INCLUDE FILES */
/* Standard Include Files */
#include <errno.h>

/* Harness Specific Include Files. */
#include "test.h"

/* Verification Test Environment Include Files */
#include "G723_decoder_test.h"
#include "g723_common_api.h"
#include "g723_dec_api.h"
#include "log_api.h"

/* LOCAL CONSTANTS */


/* LOCAL MACROS */

#define MAX_DECODER_THREADS 2

/* LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS) */

typedef struct
{
        pthread_t tid;                                  /**< Thread id */
        G723_U32 instance_id;                          /**< Thread number*/
        G723_S8 input_name[MAX_STR_LEN];               /**< File name for input data */
        G723_S8 output_name[MAX_STR_LEN];              /**< File name for output data */
        G723_S8 ref_name[MAX_STR_LEN];                 /**< File name for reference data */
        G723_S8 crc_name[MAX_STR_LEN];                 /**< CRC file name */
        FILE   *pInputFile;                             /**< Pointer to input file */
        FILE   *pOutputFile;                            /**< Pointer to output file */
        FILE   *pCRCFile;                                /**< Pointer to CRC file*/
        /* Decoder parameters */
        G723_S32 post_filter;                           /**< Post-filter */
        G723_S16 erased_frame;                          /**< Flag for erased frame */

} G723_dec_thread_t;

/* LOCAL VARIABLES */

G723_dec_thread_t dec_thread[MAX_DECODER_THREADS];
static int thread_synchro  FALSE;      /* boolean used by the loop thread to inform the thread */
static int test_iter  DEFAULT_ITERATIONS;      /* default iteration is hard coded */
static int test_testcase  NOMINAL_FUNCTIONALITY;       /* Id of testcase. Default it is Nominal
                                                         * functionality. */
static int test_duration  0;   /* default no encoding/decoding time count */
static int test_output  0;     /* default no output files */
static char *test_cfg_dir  DEFAULT_CFG_DIR_PATH;       /* default path to dir with configure file */
static int display_info  1;    /* display decode info */

/* GLOBAL CONSTANTS */

static const char *out_dir  "./g723d_output";  /* default folder for output data */

/* GLOBAL VARIABLES */


/* LOCAL FUNCTION PROTOTYPES */


int     decoder_engine(void *ptr);      /* functional body of decoder realization */

/* assistant functions */
eG723DReturnType eG723Decoder(void *ptr);       /* Decoding process */
int     run_decoder_in_loop(void *ptr); /* wrapper for the decoder realization */
int     bitmatch(const char *out_fname, const char *ref_fname); /* bit match for decoded output file
                                                                 * and reference file */
void    hogcpu(void);       /* loading CPU */

/* test functions */
int     nominal_functionality_test(void);   /* function for Nominal decoding */
int     endurance_test(void);       /* some iterations of decoding process */
int     re_entrance_test(void);     /* launch some threads of decoding process together */
int     re_locatability(void);      /* relocate memory for decoder during decoding process */
int     load_envirounment_test(void);       /* launch one thread of decode ane one loads CPU */

/* decoder initialization functions */
int     allocate_dec(G723_S16 ** ppInBuf, G723_S16 ** ppOutBuf);        /* allocate memory for
                                                                         * decoder */
void    free_dec(G723_S16 * pInBuf, G723_S16 * pOutBuf);        /* free memory for decoder */
int     allocate_memory(sG723DMemAllocInfoType * psMemInfo);    /* allocate memory for decoder */
int     relocate_memory(sG723DMemAllocInfoType * psMemInfo);    /* relocate memory for decoder */
void    free_memory(sG723DMemAllocInfoType * psMemInfo);        /* free memory for decoder */
int     init_dec_thread(void *ptr);     /* initalization of decoder thread structure */
void    close_dec_thread(void *ptr);    /* free data are used with decoder thread */
G723_Void vSwapBytes(G723_S16 * ps16Buf, G723_S32 s32Cnt);


/* GLOBAL FUNCTIONS */

/* VT_G723_decoder_setup */
/**
Description of the function
@brief  assumes the pre-condition of the test case execution
@pre None
@post None
@param  Input : None.
        Output: None.
@return On success - return TPASS
        On failure - return the error code
@bug No bug
@deprecated Not applicable
@todo Nothing
*/
int VT_G723_decoder_setup(void)
{
        int     rv  TFAIL;

        rv  TPASS;

        return rv;
}


/* VT_G723_decoder_cleanup */
/**
Description of the function
@brief  assumes the post-condition of the test case execution
@pre None
@post None
@param  Input : None.
        Output: None.
@return On success - return TPASS
        On failure - return the error code
@bug No bug
@deprecated Not applicable
@todo Nothing
*/
int VT_G723_decoder_cleanup(void)
{
        int     rv  TFAIL;

    /** insert your code here */
        rv  TPASS;

        return rv;
}

/* VT_G723_decoder_test */
/**
@brief  Template test scenario X function

@param  testcase - Testcase id of the test according to the test plan \n
        iter     - Iteration of the loop in case of an endurance/stress test
        duration - Possibility of encoding/decoding time count

@return On success - return TPASS
        On failure - return the error code
*/
int VT_G723_decoder_test(int testcase, int iter, int duration, int no_output, char *cfg_dir)
{
        int     rv  TFAIL;
        int     i;

        /*
         * rv  VT_TEMPLATE_setup(); if (rv ! TPASS) { tst_brkm(TBROK , cleanup,
         * "VT_TEMPLATE_setup() Failed : error code  %d", rv); } */
        test_testcase  testcase;
        test_iter  iter;
        test_duration  duration;
        test_cfg_dir  cfg_dir;

        for (i  0; i < MAX_DECODER_THREADS; i++)
        {
                dec_thread[i].instance_id  i;
                if (no_output)
                        test_output  0;
                else
                        test_output  1;
        }
    /** replace this following example code by ours */
        switch (test_testcase)
        {
        case NOMINAL_FUNCTIONALITY:
                tst_resm(TINFO, "Nominal functionality test");
                rv  nominal_functionality_test();
                tst_resm(TINFO, "End nominal functionality test");
                break;

        case ENDURANCE:
                tst_resm(TINFO, "Endurance test");
                rv  endurance_test();
                tst_resm(TINFO, "End endurance test");
                break;

        case RE_ENTRANCE:
                tst_resm(TINFO, "Re-entrance test");
                rv  re_entrance_test();
                tst_resm(TINFO, "End re-entrance test");
                break;

        case PRE_EMPTION:
                tst_resm(TINFO, "Pre-emption test");
                rv  re_entrance_test();
                tst_resm(TINFO, "End pre-emption test");
                break;

        case RE_LOCATABILITY:
                tst_resm(TINFO, "Re-locability test");
                rv  nominal_functionality_test();
                tst_resm(TINFO, "End re-locability test");
                break;

        case LOAD_ENVIROUNMENT:
                tst_resm(TINFO, "Load envirounment test");
                rv  load_envirounment_test();
                tst_resm(TINFO, "End load envirounment test");
                break;

        default:
                tst_resm(TWARN, "Wrong test case");
                break;
        }
    /** replace this previous example code by ours */

        return rv;
}

/* LOCAL FUNCTIONS */

/* allocate_dec */
/**
@brief  assumes allocation memory for decoder thread structure

@param  ppInBuf - Pointer to input buffer used by the decoder
        ppInBuf - Pointer to output buffer used by the decoder

@return On success - return SUCCESS
        On failure - return ERROR
*/
int allocate_dec(G723_S16 ** ppInBuf, G723_S16 ** ppOutBuf)
{
        if (!(*ppInBuf  alloc_fast((CODED_FRAMESIZE / 2) * sizeof(G723_S16))))
        {
                tst_resm(TWARN, "Decoder engine ERROR: unable to allocate memory for Input Buffer");
                return ERROR;
        }
        if (!(*ppOutBuf  alloc_fast(G723_L_FRAME * sizeof(G723_S16))))
        {
                tst_resm(TWARN,
                         "Decoder engine ERROR: unable to allocate memory for Output Buffer\n");
                mem_free(ppInBuf);
                return ERROR;
        }
        return SUCCESS;
}

/* free_dec */
/**
@brief  assumes release of memory for decoder thread structure

@param  ptr - Pointer to void for current decoder thread

@return On success - return SUCCESS
        On failure - return ERROR
*/
void free_dec(G723_S16 * pInBuf, G723_S16 * pOutBuf)
{
        if (pInBuf ! NULL)
                mem_free(pInBuf);
        pInBuf  NULL;
        if (pOutBuf ! NULL)
                mem_free(pOutBuf);
        pOutBuf  NULL;
}

/* allocate_memory */
/**
@brief  assumes allocation memory for decoder

@param  ptr - Pointer to memory information structure for decoder

@return On success - return SUCCESS
        On failure - return ERROR
*/
int allocate_memory(sG723DMemAllocInfoType * psMemInfo)
{
        int     iChunk;

        for (iChunk  0; iChunk < (psMemInfo->s32G723DNumMemReqs); iChunk++)
        {
                if (psMemInfo->asMemInfoSub[iChunk].u8G723DMemTypeFs  G723_FAST_MEMORY)
                {
                        psMemInfo->asMemInfoSub[iChunk].pvAPPDBasePtr 
                            alloc_fast(psMemInfo->asMemInfoSub[iChunk].s32G723DSize);
                }
                else
                {
                        psMemInfo->asMemInfoSub[iChunk].pvAPPDBasePtr 
                            alloc_slow(psMemInfo->asMemInfoSub[iChunk].s32G723DSize);
                }
                if (psMemInfo->asMemInfoSub[iChunk].pvAPPDBasePtr  NULL)
                {
                        free_memory(psMemInfo);
                        tst_resm(TWARN,
                                 "Decoder engine ERROR: unable to allocate memory for G723 Decoder");
                        return ERROR;
                }
        }
        return SUCCESS;
}

/* free_memory */
/**
@brief  assumes release of memory for decoder

@param  ptr - Pointer to memory information structure for decoder

@return On success - return SUCCESS
        On failure - return ERROR
*/
void free_memory(sG723DMemAllocInfoType * psMemInfo)
{
        int     iChunk  0;

        for (iChunk  0; iChunk < psMemInfo->s32G723DNumMemReqs; iChunk++)
        {
                if (psMemInfo->asMemInfoSub[iChunk].pvAPPDBasePtr ! NULL)
                {
                        mem_free(psMemInfo->asMemInfoSub[iChunk].pvAPPDBasePtr);
                        psMemInfo->asMemInfoSub[iChunk].pvAPPDBasePtr  NULL;
                }
        }
}

/* relocate_memory */
/**
@brief  assumes relocation memory for decoder

@param  ptr - Pointer to memory information structure for decoder

@return On success - return SUCCESS
        On failure - return ERROR
*/
int relocate_memory(sG723DMemAllocInfoType * psMemInfo)
{
        G723_Void *pvAPPDNewBasePtr[G723_MAX_NUM_MEM_REQS];
        int     iChunk;

#ifdef DEBUG_TEST
        printf("\nRe-locating memory for G723 Decoder...\n");
#endif

        for (iChunk  0; iChunk < psMemInfo->s32G723DNumMemReqs; iChunk++)
        {
                if (psMemInfo->asMemInfoSub[iChunk].u8G723DMemTypeFs  G723_FAST_MEMORY)
                {
                        pvAPPDNewBasePtr[iChunk] 
                            alloc_fast(psMemInfo->asMemInfoSub[iChunk].s32G723DSize);
                }
                else
                {
                        pvAPPDNewBasePtr[iChunk] 
                            alloc_slow(psMemInfo->asMemInfoSub[iChunk].s32G723DSize);
                }
                if (pvAPPDNewBasePtr[iChunk]  NULL)
                {
                        free_memory(psMemInfo);
                        tst_resm(TWARN,
                                 "Decoder engine ERROR: unable to re-locate memory for G723 Decoder");
                        return ERROR;
                }
        }
        free_memory(psMemInfo);
        for (iChunk  0; iChunk < psMemInfo->s32G723DNumMemReqs; iChunk++)
        {
                psMemInfo->asMemInfoSub[iChunk].pvAPPDBasePtr  pvAPPDNewBasePtr[iChunk];
        }
#ifdef DEBUG_TEST
        printf("...Memory have been re-located for G723 Decoder\n\n");
#endif
        return SUCCESS;
}

/* init_dec_thread */
/**
@brief  assumes initialization of decoder thread structure

@param  ptr - Pointer to void for current decoder thread

@return On success - return SUCCESS
        On failure - return ERROR
*/
int init_dec_thread(void *ptr)
{
        G723_dec_thread_t *pDecThread  (G723_dec_thread_t *) ptr;

        pDecThread->pInputFile  NULL;
        pDecThread->pOutputFile  NULL;
        pDecThread->pCRCFile  NULL;
        pDecThread->post_filter  E_G723D_P_FILTER_ENABLE;
        pDecThread->erased_frame  E_G723D_FR_NOTERASED;

        return SUCCESS;
}

/* close_dec_thread */
/**
@brief  assumes release of decoder thread structure

@param  ptr - Pointer to void for current decoder thread

@return None
*/
void close_dec_thread(void *ptr)
{
        G723_dec_thread_t *pDecThread  (G723_dec_thread_t *) ptr;

        if (pDecThread->pInputFile)
                fclose(pDecThread->pInputFile);
        if (pDecThread->pOutputFile)
                fclose(pDecThread->pOutputFile);
        if (pDecThread->pCRCFile)
                fclose(pDecThread->pCRCFile);
        pDecThread->pInputFile  NULL;
        pDecThread->pOutputFile  NULL;
        pDecThread->pCRCFile  NULL;
}


/**/
/* decoder_engine */
/**
@brief  Engine of the decoder. The decoding of a bit-stream should be presented here.
        Also this function has processed dec result, i.e. sounded it for a sound data case.
        This method should be compatible with a threads.

@param  ptr - Pointer to void for current decoder thread

@return On success - return TPASS
        On failure - return the error code
*/
/**/
int decoder_engine(void *ptr)
{
        int     ret  TFAIL;
        int     rv  TPASS;
        int     j,
                nr  1;

        eG723DReturnType eDecRet;

        if (ptr  NULL)
        {
                tst_resm(TWARN, "Decoder engine ERROR: invalid args");
                return ret;
        }

        G723_dec_thread_t *pDecThread  (G723_dec_thread_t *) ptr;

        FILE   *pFList  NULL;  /* Pointer to config file */

        char   *cfg_dir  malloc(sizeof(G723_U8) * (3 + strlen(test_cfg_dir) + strlen(FLIST_CFG)));

        if (test_cfg_dir[strlen(test_cfg_dir) - 1]  '/')
                sprintf(cfg_dir, "%s%s", test_cfg_dir, FLIST_CFG);
        else
                sprintf(cfg_dir, "%s%c%s", test_cfg_dir, '/', FLIST_CFG);

        if (!(pFList  fopen(cfg_dir, "r")))
        {
                tst_resm(TWARN, "Decoder engine ERROR: unable to open file %s (%s)\n", cfg_dir,
                         strerror(errno));
                free(cfg_dir);
                return ret;
        }
        free(cfg_dir);

        char    FListStr[MAX_STR_LEN];  /* for reading line of config file */
        char    in_file_name[MAX_STR_LEN];      /* name of next input file from config file */
        char    ref_file_name[MAX_STR_LEN];     /* name of reference file from config file */
        char    crc_file_name[MAX_STR_LEN];     /* name of crc file */
        char    in_dir_path[MAX_STR_LEN];       /* path to input directory with bitstreams */

        while (!feof(pFList))   /* reading line by line config file */
        {

                if (RE_LOCATABILITY  test_testcase)
                        nr  test_iter;
                else
                        nr  1;

                for (j  0; j < nr; ++j)
                {

                        init_dec_thread(pDecThread);

                        memset(FListStr, 0, MAX_STR_LEN);
                        memset(in_file_name, 0, MAX_STR_LEN);
                        memset(ref_file_name, 0, MAX_STR_LEN);
                        memset(crc_file_name, 0, MAX_STR_LEN);
                        memset(in_dir_path, 0, MAX_STR_LEN);
                        memset(pDecThread->input_name, 0, MAX_STR_LEN);
                        memset(pDecThread->output_name, 0, MAX_STR_LEN);
                        memset(pDecThread->crc_name, 0, MAX_STR_LEN);

                        char   *res  fgets(FListStr, MAX_STR_LEN, pFList);     /* getting line of
                                                                                 * config file */

                        if (res  NULL)
                        {
                                close_dec_thread(pDecThread);   /* close files under decoder thread
                                                                 * structure */
                                break;
                        }

                        /* set decoder parameters and names of input and reference files from config
                         * line */
                        sscanf(FListStr, "%s%d%s%s%s", in_dir_path, &(pDecThread->post_filter),
                               in_file_name, ref_file_name, crc_file_name);
                        /* set full path of input file */
                        if (in_dir_path[strlen(in_dir_path) - 1] ! '/')
                                strcat(in_dir_path, "/");
                        sprintf(pDecThread->input_name, "%s%s", in_dir_path, in_file_name);

                        // if(test_testcaseRE_ENTRANCE)
                        // tst_resm(TINFO, "Thread\t\t: %d",pDecThread->instance_id);

                        if (display_info)
                                tst_resm(TINFO, "Decode file\t: %s", pDecThread->input_name);

                        /* set value for Erased Frame flag in decoder structure */
                        if (strcmp(crc_file_name, "None"))
                        {
                                sprintf(pDecThread->crc_name, "%s%s", in_dir_path, crc_file_name);
#ifdef DEBUG_TEST
                                if (display_info)
                                        printf("\tCRC file\t: %s\n", pDecThread->crc_name);
#endif
                                if (!(pDecThread->pCRCFile  fopen(pDecThread->crc_name, "rb")))
                                {
                                        tst_resm(TWARN,
                                                 "Decoder engine ERROR: unable to open file %s (%s)",
                                                 pDecThread->crc_name, strerror(errno));
                                        rv  TFAIL;
                                        continue;
                                }
                        }
                        else
                        {
#ifdef DEBUG_TEST
                                if (display_info)
                                        printf("\tCRC file\t: None\n");
#endif
                                pDecThread->pCRCFile  NULL;
                        }
                        /*
                         * Open sample
                         */
                        pDecThread->pInputFile  fopen(pDecThread->input_name, "rb");
                        if (!(pDecThread->pInputFile))
                        {
                                tst_resm(TWARN,
                                         "Decoder engine ERROR: unable to open input file %s (%s)\n",
                                         pDecThread->input_name, strerror(errno));
                                rv  TFAIL;
                                continue;
                        }
                        if (test_output)
                        {
                                if (RE_ENTRANCE  test_testcase)
                                        sprintf(pDecThread->output_name, "%s/%s.re_%d", out_dir,
                                                ref_file_name, pDecThread->instance_id);
                                else if (PRE_EMPTION  test_testcase)
                                        sprintf(pDecThread->output_name, "%s/%s.pre_%d", out_dir,
                                                ref_file_name, pDecThread->instance_id);
                                else
                                        sprintf(pDecThread->output_name, "%s/%s_test_%d", out_dir,
                                                ref_file_name, test_testcase);

                                /*
                                 * Open output file for decoded data
                                 */
                                pDecThread->pOutputFile  fopen(pDecThread->output_name, "wb");

                                if (!(pDecThread->pOutputFile))
                                {
                                        tst_resm(TWARN,
                                                 "Decoder engine ERROR: unable to open output file %s (%s)",
                                                 pDecThread->output_name, strerror(errno));
                                        close_dec_thread(pDecThread);
                                        rv  TFAIL;
                                        continue;
                                }
                                /* set full path for reference file */
                                sprintf(pDecThread->ref_name, "%s%s", in_dir_path, ref_file_name);
#ifdef DEBUG_TEST
                                if (display_info)
                                {
                                        printf("\tReference file\t: %s\n", pDecThread->ref_name);
                                        printf("\tOutput file\t: %s\n", pDecThread->output_name);
                                }
#endif
                        }
                        /* ! * Calling G723 Decoder Function */
                        eDecRet  eG723Decoder(pDecThread);

                        if (eDecRet  E_G723D_OK)
                        {
#ifdef DEBUG_TEST
                                if (display_info)
                                        tst_resm(TINFO, "Completed decoding the bitstream %s",
                                                 pDecThread->input_name);
#endif
                        }
                        else
                        {
                                tst_resm(TFAIL, "Function eG723Decoder() failed with %d", eDecRet);
                                close_dec_thread(pDecThread);
                                return ret;
                        }
                        /* close decoder thread files opened */
                        close_dec_thread(pDecThread);

                        if (test_output)
                        {
                                /* Obtain bit match for output file and reference file */
                                if (!bitmatch(pDecThread->output_name, pDecThread->ref_name))
                                {
                                        tst_resm(TWARN,
                                                 "Output file %s and reference file %s not bit match",
                                                 pDecThread->output_name, pDecThread->ref_name);
                                        rv  TFAIL;
                                        continue;
                                }
                                else
                                {
                                        tst_resm(TINFO,
                                                 "Bit match with reference file \"%s\" is OK",
                                                 ref_file_name);
                                }
                        }
#ifdef DEBUG_TEST
                        if (display_info)
                                printf
                                    ("-----------------------------------------------------------\n");
#endif
                        if (RE_LOCATABILITY  test_testcase)
                                tst_resm(TINFO, "Data memory was relocated");
                }       // end for
        }       /* End reading config file */

        fclose(pFList); /* close config file */
        pFList  NULL;

        if (!rv)
                ret  TPASS;

        return ret;
}


/**/
/* run_decoder_in_loop */
/**
@brief  This method called by a special decoder thread decode in loop the same bitstreams.

@param  ptr - Pointer to void for current decoder thread

@return On success - return TPASS
        On failure - return the error code
*/
/**/
int run_decoder_in_loop(void *ptr)
{
        int     i;
        int     retval  0;
        static int rv  TFAIL;
        int     prior  0;

        G723_dec_thread_t *pDecThread  (G723_dec_thread_t *) ptr;

        if (test_testcase  PRE_EMPTION)
        {
                prior  pDecThread->instance_id * 15;
                nice(prior);
        }

        for (i  0; i < test_iter; ++i)
        {
                if (test_testcase  RE_ENTRANCE || test_testcase  PRE_EMPTION)
                        tst_resm(TINFO, "Thread no. %d : Itearation no. %d",
                                 pDecThread->instance_id, i + 1);
                else
                        tst_resm(TINFO, "Itearation no. %d", i + 1);
                retval + decoder_engine(ptr);
        }

        /* Set that boolean that is a global variable of the main process */
        /* to inform the second thread that the 1st one has ended. */
        /* It allows the 2nd thread to terminate. */
        if (test_testcase  RE_ENTRANCE || test_testcase  PRE_EMPTION)
                tst_resm(TINFO, "Thread no. %d : Test iterations number  %d",
                         pDecThread->instance_id, test_iter);
        else
                tst_resm(TINFO, "Test iterations number  %d", test_iter);

        thread_synchro  TRUE;
        if (!retval)
        {
                rv  TPASS;
        }
        return rv;
}

/**/
/* hogcpu*/
/**
@brief  Hog the CPU for stress test in a load environment.

@param  None

@return None
*/
/**/
void hogcpu(void)
{
        while (1)
        {
                sqrt(rand());
        }
}


/**/
/* nominal_functionality_test */
/**
@brief  Testing of a nominal functionality of a decoder.

@param  None.

@return On success - return TPASS
        On failure - return the error code
*/
/**/
int nominal_functionality_test(void)
{
        return decoder_engine(&dec_thread[0]);  // decoding with first thread
}

/**/
/* endurance_test */
/**
@brief  Test of ability to work long time without crashes.

@param  None.

@return On success - return TPASS
        On failure - return the error code
*/
/**/
int endurance_test(void)
{
        int     rt  TFAIL;

        display_info  0;
        if (run_decoder_in_loop(&dec_thread[0])  0)   /* decoding with first thread in loop */
        {
                rt  TPASS;
                return rt;
        }
        return rt;
}


/**/
 /* re_entrance_test */
/**
@brief  Re-entrance means there should not be any static data or any global
        variables used in the code. Test this ability.

@param  None.

@return On success - return TPASS
        On failure - return the error code
*/
/**/
int re_entrance_test(void)
{
        display_info  0;
        /* Creating two decoder thread with decoding in loop */

        if (pthread_create
            (&dec_thread[0].tid, NULL, (void *) &run_decoder_in_loop, (void *) &dec_thread[0]))
        {
                tst_resm(TWARN, "re_entrance_test : error creating thread 0");
                return TFAIL;
        }

        if (pthread_create
            (&dec_thread[1].tid, NULL, (void *) &run_decoder_in_loop, (void *) &dec_thread[1]))
        {
                tst_resm(TWARN, "re_entrance_test : error creating thread 1");
                return TFAIL;
        }

        /* Wait till threads are complete before main continues. Unless we */
        /* wait we run the risk of executing an exit which will terminate */
        /* the process and all threads before the threads have completed.  */
#ifdef DEBUG_TEST
        printf("wait for 1st thread to end\n");
#endif
        pthread_join(dec_thread[0].tid, NULL);
#ifdef DEBUG_TEST
        printf("wait for 2nd thread to end\n");
#endif
        pthread_join(dec_thread[1].tid, NULL);

        return TPASS;
}

/**/
/* re_locatability_test */
/**
@brief  Re-locatability means a sample application can move the data memory
        to a different address space and shall consequently call
        the G.723.1 decoder init and G.723.1 decode routines.
        Bit match should be obtained.

@param  None.

@return On success - return TPASS
        On failure - return the error code
*/
/**/
int re_locatability(void)
{
        return nominal_functionality_test();
}

/**/
/* load_envirounment_test */
/**
@brief  Test of ability to work in a loaded (even oberloaded) envirounment. [optional]

@param  None.

@return On success - return TPASS
        On failure - return the error code
*/
/**/
int load_envirounment_test(void)
{
        int     rv  TFAIL;
        int     retval;
        pid_t   pid;

        display_info  0;

        switch (pid  fork())
        {
        case -1:
                tst_resm(TWARN, "load_envirounment_test : fork failed");
                return rv;
        case 0:        /* child -- finish straight away */
#ifdef DEBUG_TEST
                tst_resm(TINFO, "Child process that load the CPU");
#endif
                hogcpu();
        default:       /* parent */
#ifdef DEBUG_TEST
                tst_resm(TINFO, "Parent process that run decoder in loop");
#endif
                retval  run_decoder_in_loop(&dec_thread[0]);

                /* kill child process once decode loop has ended */
                if (kill(pid, SIGKILL) ! 0)
                {
                        tst_resm(TWARN, "load_envirounment_test : Kill(SIGKILL) error\n");
                        return rv;
                }
                if (!retval)
                        rv  TPASS;
        }

        return rv;
}

/**/
/* eG723Decoder */
/**
@brief  Decoding process in loop.

@param  ptr - Pointer to void for current decoder thread
        pG723DecObj - Pointer to decoder config parameter structure

@return On success - return E_G723D_OK
        On failure - return the error code
*/
/**/
eG723DReturnType eG723Decoder(void *ptr)
{
        double  DecProcTimeInSec  0;   /* Decoding processor time */
        double  DecTimeInSec  0;       /* Decoding time */

        eG723DReturnType eDecRetVal  E_G723D_ERROR;

        G723_dec_thread_t *pDecThread  (G723_dec_thread_t *) ptr;

        G723_S16 *ps16InBuf  NULL;     /* Pointer to buffer for input data */
        G723_S16 *ps16OutBuf  NULL;    /* Pointer to buffer for output data */

        sG723DDecoderConfigType oG723Dec;       /* Object of decoder config structure */
        sG723DDecoderConfigType *pG723DecObj  &oG723Dec;
        sG723DMemAllocInfoType *psMemInfo  &(pG723DecObj->sG723DMemInfo);

        oG723Dec.pvG723DDecodeInfoPtr  NULL;
        oG723Dec.pu8APPDInitializedDataStart  NULL;

        /*
         * Query memory need for decoder
         */
        eDecRetVal  eG723DQueryMem(pG723DecObj);       /* to get the decoder memory requirements */

        if (eDecRetVal ! E_G723D_OK)
        {
                tst_resm(TFAIL,
                         "Decoder engine ERROR: Function eG723DQuerymem() resulted in failure. G723D Error Type : %d",
                         eDecRetVal);
                return eDecRetVal;
        }

        /* ! * Allocating Memory for G723 Decoder */
        if (allocate_memory(psMemInfo)  ERROR)
                return eDecRetVal;

        /* ! * Calling G723 Decoder Init Function */
        eDecRetVal  eG723DDecodeInit(pG723DecObj);

        if (eDecRetVal ! E_G723D_OK)
        {
                tst_resm(TFAIL,
                         "Decoder engine ERROR: Decoder initialization failed with return value %d",
                         eDecRetVal);
                free_memory(psMemInfo);
                return eDecRetVal;
        }

        int     s32FrameNum  0;        /* number of decoding frame */

        /*
         *  Allocate memory for input and output buffers used by decoder
         */
        allocate_dec(&ps16InBuf, &ps16OutBuf);
        /* ! * Decode bitstream */
#ifdef DEBUG_TEST
        if (display_info)
                printf("\nDecoding ...\n");
#endif
        do      /* Reading of bitstream */
        {
                /* set value for Post Filter flag in decoder structure */
                if (pDecThread->post_filter)
                        pG723DecObj->u8APPDPostFilter  E_G723D_P_FILTER_ENABLE;
                else
                        pG723DecObj->u8APPDPostFilter  E_G723D_P_FILTER_DISABLE;

                /* to clear buffer for input data before each frame reading from input file */
                memset(ps16InBuf, 0, sizeof(G723_S16) * CODED_FRAMESIZE / 2);

                /*
                 * Reading frame frome input file
                 */

                G723_S16 s16FrameType;  /* type of read frame */
                G723_S8 *ps8Line  (G723_S8 *) ps16InBuf;

                if (fread(ps8Line, 1, 1, pDecThread->pInputFile) ! 1)
                        break;  /* End of reading bitstream */

                if (ferror(pDecThread->pInputFile))
                {
                        tst_resm(TWARN,
                                 "Decoder engine ERROR: unable to read input bitstream %s (%s)\n",
                                 pDecThread->input_name, strerror(errno));
                        return eDecRetVal;
                }
                s16FrameType  ps8Line[0] & ((G723_S16) 0x0003);

                G723_S32 s32SizeOfFrame  0;    /* size in bytes of read frame */
                int     bTransmittedFrame  0;  /* flag of transmitable frame */

                switch (s16FrameType)
                {
                case 0:
                        s32SizeOfFrame  23;    /* Active frame, high rate */
                        bTransmittedFrame  1;
                        break;
                case 1:
                        s32SizeOfFrame  19;    /* Active frame, low rate */
                        bTransmittedFrame  1;
                        break;
                case 2:
                        s32SizeOfFrame  3;     /* Sid Frame */
                        bTransmittedFrame  1;
                        break;
                default:
                        break;
                }
                if (bTransmittedFrame)  /* if frame is not untransmitted then reading it */
                {
                        fread(&ps8Line[1], s32SizeOfFrame, 1, pDecThread->pInputFile);

                        if (ferror(pDecThread->pInputFile))
                        {
                                tst_resm(TWARN,
                                         "Decoder engine ERROR: unable to read input bitstream %s (%s)\n",
                                         pDecThread->input_name, strerror(errno));
                                return eDecRetVal;
                        }
                }
                /*
                 *  Read CRC file if it exists
                 */
                if (pDecThread->pCRCFile)
                {
                        fread((G723_S8 *) (&(pDecThread->erased_frame)), sizeof(G723_S16), 1,
                              pDecThread->pCRCFile);
                        if (ferror(pDecThread->pCRCFile))
                                tst_resm(TWARN,
                                         "Decoder engine ERROR: unable to read CRC file %s (%s)\n",
                                         pDecThread->crc_name, strerror(errno));

#ifdef G723_BIG_ENDIAN
                        vSwapBytes((G723_S16 *) & pDecThread->erased_frame, 1);
#endif

                }
                else
                        pDecThread->erased_frame  (G723_S16) 0;

                pG723DecObj->u8APPDFrameErasureFlag  (G723_S8) (pDecThread->erased_frame);
                /* to clear buffer for output data before decoding routain */
                memset(ps16OutBuf, 0, sizeof(G723_S16) * G723_L_FRAME);

                struct timeval tv;      /* structure for getting current time */
                clock_t start_proc,
                        finish_proc;    /* begining and ending processor time of decoding call */
                long    start,
                        finish; /* begining and ending time of decoding call */
                double  duration_proc,
                        duration;       /* processor time and time of decoding each frame */

                if (test_duration)      /* if there was setted option real-time exucation test */
                {
                        gettimeofday(&tv, NULL);
                        start  tv.tv_sec * 1000000 + tv.tv_usec;       /* time as microseconds */
                        start_proc  clock();   /* time as seconds */
                }
                /* Decodes the G.723.1 bitstream and writes bitstream to output buffer */
                eDecRetVal  eG723DDecodeFrame(pG723DecObj, ps16InBuf, ps16OutBuf);

                if (test_duration)      /* if there was setted option real-time exucation test */
                {
                        finish_proc  clock();
                        gettimeofday(&tv, NULL);
                        finish  tv.tv_sec * 1000000 + tv.tv_usec;

                        duration_proc  (double) (finish_proc - start_proc) / CLOCKS_PER_SEC;
                        duration  (double) (finish - start) / 1000000;

                        DecTimeInSec + duration;
                        DecProcTimeInSec + duration_proc;
                }
                if (eDecRetVal  E_G723D_OK)   /* if success ending of decoding frame */
                {
                        s32FrameNum++;

                        if (test_output)
                        {
                                /* Writting output buffer into output file */
#ifdef G723_BIG_ENDIAN
                                vSwapBytes(ps16OutBuf, G723_L_FRAME);
#endif
                                fwrite((G723_S8 *) ps16OutBuf, sizeof(G723_S16), G723_L_FRAME,
                                       pDecThread->pOutputFile);
                        }
                        if (test_duration)      /* if there was setted option real-time exucation
                                                 * test */
                        {
                                tst_resm(TINFO,
                                         "Processor time of decoding of the frame no.%d: %.6fc",
                                         s32FrameNum, duration_proc);
                                tst_resm(TINFO, "Time of decoding of the frame no.%d: %.6fc\n",
                                         s32FrameNum, duration);
                                fflush(stdout);
                        }
                }
                else    /* if not success ending of decoding frame */
                {
                        tst_resm(TFAIL, "Decoder Loop ERROR: Return Value  %d\n", eDecRetVal);
                        break;
                }

        }
        while (G723_TRUE);      /* End reading of input bitstream */

        /*
         *  Free input and output buffers used by decoder
         */
        free_dec(ps16InBuf, ps16OutBuf);

        if (display_info)
                tst_resm(TINFO, "%d frames have been decoded for %s", s32FrameNum,
                         pDecThread->input_name);

        if (test_duration)      /* if there was setted option real-time exucation test */
        {
                tst_resm(TINFO, "Proccesor time of the decoding (Sec) of bitstream %s  %f",
                         pDecThread->input_name, DecProcTimeInSec);
                tst_resm(TINFO, "Time of the decoding (Sec) of bitstream %s  %f",
                         pDecThread->input_name, DecTimeInSec);
        }

        /*
         *  Free memory allocated for decoder
         */
        free_memory(psMemInfo);

        eDecRetVal  E_G723D_OK;

        return eDecRetVal;
}

/**/
/* bitmatch */
/**
@brief  Obtains bit match for decoded bitstrem and reference bitstream file

@param  out_fname - Name of decoded bitstream file
        ref_fname - Name of reference bitstream file

@return On success - return TRUE
        On failure - return FALSE
*/
/**/
int bitmatch(const char *out_fname, const char *ref_fname)
{
        FILE   *pf1,
               *pf2;
        int     rd1,
                rd2;
        unsigned char b1,
                b2;

        if (out_fname && ref_fname)
        {
                if (!(pf1  fopen(out_fname, "rb")))
                {
                        tst_resm(TWARN, "Bitmatch() ERROR : unable to open file %s", out_fname);
                        return FALSE;
                }
                if (!(pf2  fopen(ref_fname, "rb")))
                {
                        fclose(pf1);
                        tst_resm(TWARN, "Bitmatch() ERROR : unable to open file %s", ref_fname);
                        return FALSE;
                }
                if (pf1 && pf2)
                {
                        while (TRUE)
                        {
                                if (feof(pf1) ! feof(pf2))
                                        break;  /* bitmatch fails */

                                rd1  fread(&b1, 1, sizeof(b1), pf1);
                                if (ferror(pf1))
                                {
                                        tst_resm(TWARN,
                                                 "Bitmatch() ERROR: unable to read file %s (%s)",
                                                 out_fname, strerror(errno));
                                        fclose(pf1);
                                        fclose(pf2);
                                        return FALSE;
                                }
                                rd2  fread(&b2, 1, sizeof(b2), pf2);
                                if (ferror(pf2))
                                {
                                        tst_resm(TWARN,
                                                 "Bitmatch() ERROR: unable to read file %s (%s)",
                                                 ref_fname, strerror(errno));
                                        fclose(pf1);
                                        fclose(pf2);
                                        return FALSE;
                                }
                                if (b1 ! b2)
                                        break;  /* bitmatch fails */

                                if (feof(pf1) && feof(pf2))
                                {
                                        fclose(pf1);
                                        fclose(pf2);
                                        return TRUE;
                                }
                        }
                }

                if (pf1)
                        fclose(pf1);
                if (pf2)
                        fclose(pf2);
        }
        return FALSE;
}

G723_Void vSwapBytes(G723_S16 * ps16Buf, G723_S32 s32Cnt)
{
        G723_S8 s8ch;
        G723_S8 *s8p;

        while (--s32Cnt > 0)
        {
                s8p  (G723_S8 *) (ps16Buf + s32Cnt);
                s8ch  s8p[0];
                s8p[0]  s8p[1];
                s8p[1]  s8ch;
        }
}
