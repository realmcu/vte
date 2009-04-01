/* 
 * Copyright (C) 2004, Freescale Semiconductor, Inc. All Rights Reserved THIS SOURCE CODE IS
 * CONFIDENTIAL AND PROPRIETARY AND MAY NOT BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
 * Freescale Semiconductor, Inc. */

/**
@file g723_1_encoder_test.c

@brief VTE C source template

Description of the file

@par Portability:
        Indicate if this module is portable to other compilers or platforms.
        If not, indicate specific reasons why is it not portable.
*/

/*======================== REVISION HISTORY ==================================

Author (core ID)      Date         CR Number    Description of Changes
-------------------   ----------   ----------   ------------------------------
I.Inkina/nknl001      27/12/2004   TLSbo47117   BRIEF desc. of changes
D.Simakov/smkd001c    15/04/2005   TLSbo47117   Some new testcases were added
S. V-Guilhou/svan01c  26/05/2005   TLSbo50534   P4 Codecs Campaign (add traces)
D.Simakov/smkd001c    24/10/2005   TLSbo57009   Re-locatability test was fixed
D.Simakov             17/04/2006   TLSbo66146   L_FRAME -> G723_L_FRAME
=============================================================================*/

/*==================================================================================================
                                        INCLUDE FILES
==================================================================================================*/
/* Standard Include Files */
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include "g723_enc_api.h"
#include "g723_common_api.h"

/* Harness Specific Include Files. */
#include "test.h"

/* Verification Test Environment Include Files */
#include "g723_1_encoder_test.h"

/*==================================================================================================
                                        LOCAL MACROS
==================================================================================================*/

/*==================================================================================================
                          LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
==================================================================================================*/

/*==================================================================================================
                                       LOCAL VARIABLES
==================================================================================================*/
static g732_encoder_thread_t g732_encoder_thread[ENCODER_THREAD];

static int thread_synchro = FALSE;      /* boolean used by the loop thread to inform the thread */
static int test_iter = DEFAULT_ITERATIONS;      /* default iteration is hard coded */
int     test_case = -1;
int     Flag = 0;
int     Flag_write;
FILE   *fp_mes;

// char file_mes[12]="message.txt";

struct s_list
{
        char   *file_name_in;
        char   *file_name_out;
        char   *file_compare;

        G723_U8 rate;
        G723_U8 filter;
        G723_U8 vad;

        struct s_list *next;
};


/*==================================================================================================
                                       GLOBAL CONSTANTS
==================================================================================================*/
/*==================================================================================================
                                       GLOBAL VARIABLES
==================================================================================================*/
/*==================================================================================================
                                   LOCAL FUNCTION PROTOTYPES
==================================================================================================*/
/* helper functions */

int     hogcpu(void);
void   *run_encoder_in_loop(void *ptr);
struct s_list *enter_encode(char *file_list, struct s_list *root);
int     init_param(int, struct s_list *);
int     compare_files(void *ptr);
struct s_list *addtree(struct s_list *p, char *w, char *w_1, char *w_2, char *w_3, char *w_4,
                       char *w_5);
int     LineWrite(void *ptr, G723_S8 * ps8Line, FILE * Fp, G723_S8 * buf_pr);

G723_Void vReadlbc(G723_S16 * ps16Dpnt, G723_S32 s32Len, FILE * Fp);
G723_S32 s32G723EProcessCmdLineOptions(void *ptr,
                                       FILE ** ppInpFile,
                                       FILE ** ppOutFile,
                                       G723_S32 * ps32UseHighPass, G723_S32 * ps32UseVAD,
                                       G723_S32 * ps32Quiet);

int     eG723EEncodeExit(FILE * InpFile, FILE * OutFile,
                         sG723EEncoderConfigType * psEncConfig,
                         G723_S16 * ps16InBuf, G723_S16 * ps16OutBuf);

/*================================================================================================
                                       LOCAL FUNCTIONS
==================================================================================================*/
int     nominal_functionality_test(struct s_list *root);
int     relocatability_test(struct s_list *root);
int     reentrance_test(struct s_list *root);
int     preemption_test(struct s_list *root);
int     endurance_test(struct s_list *root);
int     load_test(struct s_list *root);
int     run_encoder(void *);
int     realloc_enc_memory(g732_encoder_thread_t *);

/*================================================================================================*/
/*===== VT_g723_1_encoder_setup =====*/
/**
@brief  assumes the pre-condition of the test case execution

@param  None
  
@return On success - return TPASS
        On failure - return the error code
*/
/*================================================================================================*/
int VT_g723_1_encoder_setup(void)
{
        int     rv = TFAIL;

    /** insert your code here */
        rv = TPASS;

        return rv;
}


/*================================================================================================*/
/*===== VT_g723_1_encoder_cleanup =====*/
/**
@brief  assumes the post-condition of the test case execution

@param  None
  
@return On success - return TPASS
        On failure - return the error code
*/
/*================================================================================================*/
int VT_g723_1_encoder_cleanup(void)
{
        int     rv = TFAIL;

        rv = TPASS;
    /** insert your code here */

        return rv;
}

/*****************************************************=============================*/

/*================================================================================================*/
/*===== VT_g723_1_encoder_test =====*/
/**
@brief   a scenario of the test functions

@param  testcase - Testcase id of the test according to the test plan \n
        iter     - Iteration of the loop in case of an endurance/stress test 
  
@return On success - return TPASS
        On failure - return the error code
*/
/*================================================================================================*/
int VT_g723_1_encoder_test(int testcase, int iter, char *listfile, int number)
{
        int     rv = TFAIL;
        int     retval = 0;
        char   *file_list;
        struct s_list *root;

        test_iter = iter;
        Flag_write = number;
        root = NULL;
        if (!Flag_write)
                fp_mes = fopen("message.txt", "w");

        if (!listfile)
                file_list = "g723e_cfg";
        else
                file_list = listfile;

        root = enter_encode(file_list, root);

        /** insert your code here */
        switch (testcase)
        {
        case NOMINAL_FUNCTIONALITY:

                tst_resm(TINFO, "Nominal functionality test");
                retval += nominal_functionality_test(root);
                if (!retval)
                        rv = TPASS;
                tst_resm(TINFO, "End nominal functionality test\n");
                break;

        case RELOCATABILITY:
                tst_resm(TINFO, "re-locatability test");
                retval += relocatability_test(root);
                if (!retval)
                        rv = TPASS;
                tst_resm(TINFO, "End re-locatability test");
                break;

        case RE_ENTRANCE:
                tst_resm(TINFO, " Re-entrance test");
                retval += reentrance_test(root);
                if (!retval)
                        rv = TPASS;
                tst_resm(TINFO, " End re-entrance test");
                break;

        case PRE_EMPTION:
                tst_resm(TINFO, "Preemptive test");
                rv = preemption_test(root);
                tst_resm(TINFO, "End of preemptive test");
                break;

        case ENDURANCE:
                tst_resm(TINFO, "Endurance test");
                rv = endurance_test(root);
                tst_resm(TINFO, "End of endurance test");
                break;

        case LOAD:
                tst_resm(TINFO, "Load test");
                rv = load_test(root);
                tst_resm(TINFO, "End of load test");
                break;

        default:
                tst_resm(TINFO, "Wrong test case");
                break;
        }

        if (!Flag_write)
                fclose(fp_mes);

        return rv;
}

/*================================================================================================*/
/*===== nominal_functionality_test =====*/
/**
@brief  Testing encoder nominal functionality.

@param  None.

@return On success - return TPASS
        On failure - return the error code
*/
/*================================================================================================*/
int nominal_functionality_test(struct s_list *root)
{
        int     ret = TPASS;
        int     rv = TPASS;
        struct s_list *root_n;
        g732_encoder_thread_t *g732_encoder = &g732_encoder_thread[0];

        for (root_n = root; root_n != NULL; root_n = root_n->next)
        {

                init_param(0, root_n);
                ret = run_encoder(g732_encoder);
                if (ret != TPASS)
                {
                        tst_resm(TFAIL, "ERROR in the function run_encoder");
                        if (!Flag_write)
                        {
                                fprintf(fp_mes, "ERROR in the function run_encoder");
                                fprintf(fp_mes,
                                        " Encoder[%d] file_name_input=%s\nfile_name_output=%s file_name_compare =%s\n",
                                        g732_encoder->Index, g732_encoder->file_name_input,
                                        g732_encoder->file_name_output,
                                        g732_encoder->file_name_compare);
                        }
                        rv = TFAIL;
                }
        }

        ret = rv;

        return ret;
}


/*================================================================================================*/
/*===== relocatability_test  =====*/
/**
@brief  Test of encoder code relocatability.

@param  None.

@return On success - return TPASS
        On failure - return the error code
*/
/*================================================================================================*/
int relocatability_test(struct s_list *root)
{
        int     ret = TPASS;
        int     rv = TPASS,
            i;
        struct s_list *root_n;
        g732_encoder_thread_t *g732_encoder = &g732_encoder_thread[0];

        for (root_n = root; root_n != NULL; root_n = root_n->next)
        {
                for (i = 0; i < test_iter; ++i)
                {
                        init_param(0, root_n);
                        ret = run_encoder(g732_encoder);
                        if (ret != TPASS)
                        {
                                tst_resm(TFAIL, "ERROR in the function run_encoder");
                                if (!Flag_write)
                                {
                                        fprintf(fp_mes, "ERROR in the function run_encoder");
                                        fprintf(fp_mes,
                                                " Encoder[%d] file_name_input=%s\nfile_name_output=%s file_name_compare =%s\n",
                                                g732_encoder->Index, g732_encoder->file_name_input,
                                                g732_encoder->file_name_output,
                                                g732_encoder->file_name_compare);
                                }
                                rv = TFAIL;
                        }
                        tst_resm(TINFO, "Data memory was relocated");
                }
        }

        ret = rv;

        return ret;
}

/*================================================================================================*/
/*===== reentrance_test =====*/
/**
@brief  Reentrance means there should not be any static data or any global
        variables used in the code. Test this ability.

@param  None.

@return On success - return TPASS
        On failure - return the error code
*/
/*================================================================================================*/
int reentrance_test(struct s_list *root)
{
        int     j,
                i;
        int     ret = TPASS;
        int     rv = TPASS;
        struct s_list *root_n;

        for (i = 0, root_n = root; (root_n != NULL) && (i < ENCODER_THREAD);
             i++, root_n = root_n->next)
        {
                init_param(i, root_n);  /* Set file names and instance ID */

                if (pthread_create(&g732_encoder_thread[i].tid, NULL, (void *) &run_encoder_in_loop,
                                   (void *) &g732_encoder_thread[i]))
                {
                        tst_resm(TFAIL, "ERROR: cannot create thread %d ", i);
                        // fprintf(stderr, "ERROR: cannot create thread %d: %s\n", i,
                        // strerror(errno));
                        if (!Flag_write)
                        {
                                fprintf(fp_mes, "ERROR: cannot create thread %d \n ", i);
                                fprintf(fp_mes,
                                        " Encoder[%d]  file_name_input= %s\n file_name_output=%s\n file_name_compare =%s",
                                        g732_encoder_thread->Index,
                                        g732_encoder_thread->file_name_input,
                                        g732_encoder_thread->file_name_output,
                                        g732_encoder_thread->file_name_compare);
                        }

                        rv = TFAIL;
                        break;
                }
        }

        j = i;

        for (i = 0; i < j; i++)
        {
                tst_resm(TINFO, "wait for %dst thread to end", i);
                pthread_join(g732_encoder_thread[i].tid, NULL);

        }
        ret = rv;
        return ret;
}

int preemption_test(struct s_list *root)
{
        return reentrance_test(root);
}

int endurance_test(struct s_list *root)
{
        int     i;
        int     rv = TPASS;

        for (i = 0; i < test_iter; ++i)
        {
                tst_resm(TINFO, "The %d iteration is started", i + 1);
                rv += nominal_functionality_test(root);
                tst_resm(TINFO, "The %d iteration is completed", i + 1);
        }
        return rv;
}


/*================================================================================================*/
/*================================================================================================*/
int load_test(struct s_list *root)
{
        int     rv = TFAIL;
        pid_t   pid;

        switch (pid = fork())
        {
        case -1:
                tst_resm(TWARN, "load_envirounment_test : fork failed");
                return rv;
        case 0:
                /* child process */
                hogcpu();
        default:
                /* parent */
                sleep(2);
                rv = nominal_functionality_test(root);
                /* kill child process once decode/encode loop has ended */
                if (kill(pid, SIGKILL) != 0)
                {
                        tst_resm(TWARN, "load_envirounment_test : Kill(SIGKILL) error");
                        return rv;
                }
        }
        return rv;
}


/*===============================*/
/*================================================================================================*/
/*===== compare_files=====*/
/**
@brief  Function compares the output file with the reference file.

@param  the thread nomer .

@return On success - return TPASS
        On failure - return the error code
*/
/*================================================================================================*/
int compare_files(void *ptr)
{
        int     rv = FALSE;
        FILE   *fp;
        FILE   *fp_com;
        long    size_c = 0,
            size = 0;
        struct stat s;
        int     bytes_read = 0,
            bytes_read_com = 0;
        unsigned char *buf,
               *buf_com;
        int     i;
        char   *file_name;

        g732_encoder_thread_t *g732_encoder = (g732_encoder_thread_t *) ptr;

        file_name = g732_encoder->file_name_compare;

        if (!Flag_write)
                fprintf(fp_mes, "Encoder[%d] the compare file name =%s\n ", g732_encoder->Index,
                        file_name);



        stat(file_name, &s);
        size_c = s.st_size;
        buf_com = (unsigned char *) calloc(size_c, 1);

        if ((fp_com = fopen(file_name, "rb")) != NULL)
        {
                bytes_read_com = fread(buf_com, sizeof(unsigned char), size_c, fp_com);
        }
        else
        {
                tst_resm(TFAIL, "error - could not open the compare file %s", file_name);
                free(buf_com);
                return rv;
        }

        fclose(fp_com);

        if (Flag_write == 1)
        {
                // tst_resm( TINFO, " buf_byte =%d
                // Index=%d",g732_encoder->buf_byte,g732_encoder->Index);
                for (i = 0; i < bytes_read_com; i++)
                {
                        if (g732_encoder->buf_pr[i] != buf_com[i])
                        {
                                tst_resm(TFAIL,
                                         "error - size byte of the  %s file does not equality the outbuffer",
                                         file_name);

                                free(g732_encoder->buf_pr);
                                free(buf_com);
                                return rv;
                        }
                }
                free(g732_encoder->buf_pr);
                g732_encoder->buf_pr = NULL;

        }       // end if ,do't write
        else
        {

                stat(g732_encoder->file_name_output, &s);
                size = s.st_size;

                if (size_c != size)
                {
                        tst_resm(TFAIL,
                                 "error - size byte of the  %s file does not equality the %s file",
                                 file_name, g732_encoder->file_name_output);
                        if (!Flag_write)
                        {
                                fprintf(fp_mes, "error - size byte of the  %s file does not equality the outbuffer\n\
                                Encoder[%d]  file_name_input= %s\n file_name_output=%s\n",
                                        file_name, g732_encoder->Index, g732_encoder->file_name_input, g732_encoder->file_name_output);
                        }
                        free(buf_com);
                        return rv;

                }

                buf = (unsigned char *) calloc(size, 1);

                if ((fp = fopen(g732_encoder->file_name_output, "rb")) != NULL)
                {
                        bytes_read = fread(buf, sizeof(unsigned char), size, fp);
                }
                else
                {
                        tst_resm(TFAIL, "error - could not open the out file %s",
                                 g732_encoder->file_name_output);
                        free(buf);
                        free(buf_com);
                        return rv;
                }

                fclose(fp);

                for (i = 0; i < bytes_read; i++)
                        if (buf[i] != buf_com[i])
                        {
                                tst_resm(TFAIL,
                                         "error - size byte of the  %s file does not equality the %s file",
                                         file_name, g732_encoder->file_name_output);
                                if (!Flag_write)
                                {
                                        fprintf(fp_mes, "error - size byte of the  %s file does not equality the %s file\n\
                              Encoder[%d]  file_name_input= %s\n ",
                                                file_name, g732_encoder->file_name_output, g732_encoder->Index, g732_encoder->file_name_input);
                                }
                                free(buf);
                                free(buf_com);
                                return rv;
                        }
                free(buf);
        }       // end else

        free(buf_com);

        tst_resm(TINFO, "bitmatch passed (%s vs %s)", g732_encoder->file_name_output, file_name);

        rv = TRUE;
        return rv;
}

/*================================================================================================*/
/*===== enter_encode=====*/
/**
@brief  function input reads a  data  of the list file

@param  the file list name and the test nomer .

@return On success - return TPASS
        On failure - return the error code
*/
/*================================================================================================*/

struct s_list *enter_encode(char *file_list, struct s_list *root)
{
        int     i = 0;
        FILE   *fp;
        unsigned char lis[7][80];

        if ((fp = fopen(file_list, "r")) == NULL)
        {
                tst_resm(TFAIL, "ERROR in read_list(): cannot open config file: %s",
                         strerror(errno));
                exit(1);
        }

        while (fscanf
               (fp, "%s %s %s %s %s %s", lis[i], lis[i + 1], lis[i + 2], lis[i + 3], lis[i + 4],
                lis[i + 5]) != EOF)
                root =
                    addtree(root, lis[i], lis[i + 1], lis[i + 2], lis[i + 3], lis[i + 4],
                            lis[i + 5]);

        fclose(fp);

        return root;
}


/*================================================================================================*/
/*===== enter_encode=====*/
/**
@brief  function input reads a  data  of the list file
             and write the structure
@param  the file list name and the test nomer .

@return On success - return TPASS
        On failure - return the error code
*/
/*================================================================================================*/

struct s_list *addtree(struct s_list *p, char *w, char *w_1, char *w_2, char *w_3, char *w_4,
                       char *w_5)
{

        if (p == NULL)
        {
                p = (struct s_list *) malloc(sizeof(struct s_list));
                p->file_name_in = strdup(w);
                p->file_name_out = strdup(w_1);
                p->file_compare = strdup(w_2);
                p->rate = (atoi(w_3) == 53) ? E_G723E_BITRATE_53 : E_G723E_BITRATE_63;
                p->filter =
                    (strcmp(w_4, "F0") == 0) ? E_G723E_HPFILTER_ENABLE : E_G723E_HPFILTER_DISABLE;
                p->vad = (strcmp(w_5, "V0") == 0) ? E_G723_VAD_ENABLE : E_G723_VAD_DISABLE;
                p->next = NULL;
        }
        else
        {
                p->next = addtree(p->next, w, w_1, w_2, w_3, w_4, w_5);
        }

        return p;
}

/*============================================*/
/* The program initialization of the encoder parametrs */

/*================================================================================================*/
/*===== enter_encode=====*/
/**
@brief  function input reads a  data  of the list file

@param  the file list name and the test nomer .

@return On success - return TPASS
        On failure - return the error code
*/
/*================================================================================================*/

int init_param(int nom_thread, struct s_list *root)
{
        int     rv = FALSE;

        struct s_list *p_list;

        p_list = NULL;
        p_list = root;

        g732_encoder_thread_t *g732_encoder = &g732_encoder_thread[nom_thread];

        if ((nom_thread >= ENCODER_THREAD) || (!p_list))
        {
                tst_resm(TFAIL, "ERROR in init_param(): one of parameters isn't valid");
                return rv;
        }

        /* set the encoder, currently the parametrs of the thread */

        g732_encoder->Index = nom_thread;
        g732_encoder->numberframe = 0;
        g732_encoder->psEncConfig.u8APPEHighPassFilter = p_list->filter;
        g732_encoder->psEncConfig.u8APPEVADFlag = p_list->vad;
        g732_encoder->psEncConfig.u8APPEBitRate = p_list->rate;
        g732_encoder->file_name_input = p_list->file_name_in;
        g732_encoder->file_name_output = p_list->file_name_out;
        g732_encoder->file_name_compare = p_list->file_compare;

        rv = TRUE;
        return rv;

}

/*================================================================================================*/
/*===== run_codec_in_loop =====*/
/**
@brief  This method called by a special codec thread decode/encode in loop the same bitstreams.

param  Input:  ptr - pointer to the structure holding buffers, encoder config structure etc.
        Output: None
  
@return On success - return TPASS
        On failure - return the error code
*/
/*================================================================================================*/
void   *run_encoder_in_loop(void *ptr)
{
        int     i,
                retval = 0;
        g732_encoder_thread_t *g_encoder = (g732_encoder_thread_t *) ptr;

        if (test_case == PRE_EMPTION)
        {
                nice((int) (20 * (float) g_encoder->Index / ENCODER_THREAD));
        }

        for (i = 0; i < test_iter; ++i)
        {
                retval += run_encoder(g_encoder);
        }

        /* Set that boolean that is a global variable of the main process */
        /* to inform the second thread that the 1st one has ended. */
        /* It allows the 2nd thread to terminate. */
        thread_synchro = TRUE;

        return NULL;
}

/*================================================================================================*/
/*===== hogcpu=====*/
/**
@brief  Hog the CPU for stress test in a load environment.

@param  None
  
@return None
*/
/*================================================================================================*/
int hogcpu(void)
{
        while (1)
        {
                sqrt(rand());
        }
}

/*================================================================================================*/
/*===== vReadlbc=====*/
/**
@brief  function reads a  data  of the input file

@param  the input buffer, the poiner on the fail.

@return On success - return TPASS
        On failure - return the error code
*/
/*================================================================================================*/
G723_Void vReadlbc(G723_S16 * ps16Dpnt, G723_S32 s32Len, FILE * Fp)
{
        G723_S32 s32i;

        for (s32i = 0; s32i < s32Len; s32i++)
                ps16Dpnt[s32i] = (G723_S16) 0;

        fread((G723_S8 *) ps16Dpnt, sizeof(G723_S16), s32Len, Fp);
        /* 
         * #ifdef G723_BIG_ENDIAN vSwapBytes(ps16Dpnt, s32Len); #endif */
        return;
}


/*================================================================================================*/
/*===== enter_encode=====*/
/**
@brief  function  set of the option data  

@param  the file list name and the test nomer .

@return On success - return TPASS
        On failure - return the error code
*/
/*================================================================================================*/

G723_S32 s32G723EProcessCmdLineOptions(void *ptr,
                                       FILE ** ppInpFile,
                                       FILE ** ppOutFile,
                                       G723_S32 * ps32UseHighPass, G723_S32 * ps32UseVAD,
                                       G723_S32 * ps32Quiet)
{

        g732_encoder_thread_t *g732_encoder = (g732_encoder_thread_t *) ptr;


        G723_S32 Flen;
        G723_S8 *ps8InpFileName = NULL;
        G723_S8 *ps8OutFileName = NULL;

        /* Process encoding argument */
        *ppInpFile = NULL;      /* Input file */
        *ppOutFile = NULL;      /* Output file */
        *ps32Quiet = G723_FALSE;        /* Default: Verbose Mode */
        *ps32UseHighPass = E_G723E_HPFILTER_ENABLE;     /* Default: Enable */
        *ps32UseVAD = E_G723_VAD_ENABLE;

        ps8InpFileName = g732_encoder->file_name_input;
        if (Flag_write == 0)
                ps8OutFileName = g732_encoder->file_name_output;

        *ppInpFile = fopen(ps8InpFileName, "rb");
        if (*ppInpFile == NULL)
        {
                tst_resm(TFAIL, "Invalid input file name: %s", ps8InpFileName);
                exit(1);
        }

        if ((*ps32Quiet == G723_FALSE) && (Flag_write == 0))
                fprintf(fp_mes, "Encoder[%d] the input file name =%s ", g732_encoder->Index,
                        ps8InpFileName);

        if (Flag_write == 0)
        {
                *ppOutFile = fopen(ps8OutFileName, "wb");
                if (*ppOutFile == NULL)
                {
                        tst_resm(TFAIL, "Can't open output file: %s", ps8OutFileName);
                        exit(1);
                }
        }


        if ((*ps32Quiet == G723_FALSE) && (!Flag_write))
                fprintf(fp_mes, "Encoder[%d] the output file name =%s ", g732_encoder->Index,
                        ps8OutFileName);

        /* Options report */
        if (*ps32Quiet == G723_FALSE)
        {
                if (!Flag_write)
                        fprintf(fp_mes, "Encoder[%d] Options:\n", g732_encoder->Index);

                if ((g732_encoder->psEncConfig.u8APPEBitRate == E_G723E_BITRATE_63)
                    && (Flag_write == 0))
                        fprintf(fp_mes, "Encoder[%d] Rate 6.3 kb/s\n", g732_encoder->Index);
                else if (!Flag_write)
                        fprintf(fp_mes, "Encoder[%d] Rate 5.3 kb/s\n", g732_encoder->Index);
                if ((g732_encoder->psEncConfig.u8APPEHighPassFilter == E_G723E_HPFILTER_ENABLE)
                    && (Flag_write == 0))
                        fprintf(fp_mes, "Encoder[%d] Highpassfilter enabled\n",
                                g732_encoder->Index);
                else if (!Flag_write)
                        fprintf(fp_mes, "Encoder[%d] Highpassfilter disabled\n",
                                g732_encoder->Index);

                if ((g732_encoder->psEncConfig.u8APPEVADFlag == E_G723_VAD_DISABLE)
                    && (!Flag_write))
                        fprintf(fp_mes, "Encoder[%d] VAD/CNG disabled\n", g732_encoder->Index);
                else if (!Flag_write)
                        fprintf(fp_mes, "Encoder[%d] VAD/CNG enabled\n", g732_encoder->Index);
        }

        /* 
         * Compute the file length */
        fseek(*ppInpFile, 0L, SEEK_END);
        Flen = ftell(*ppInpFile);
        rewind(*ppInpFile);

        Flen /= sizeof(G723_S16) * G723_L_FRAME;

        return Flen;
}

/*================================================================================================*/
/*===== run_encoder=====*/
/**
@brief  Run of the encoder. The  function processes encoder result displays. 

@param  Input:  ptr - pointer to the structure holding buffers, encoder config structure etc.
        Output: None

@return On success - return TPASS
        On failure - return the error code
*/
/*================================================================================================*/

int run_encoder(void *ptr)
{

        /* Pointer to new speech data */
        G723_S16 *ps16InBuf;

        /* Output bitstream buffer */
        G723_S16 *ps16OutBuf;
        G723_S32 s32i;
        G723_S32 s32UseHighPass,
                s32UseVx,
                s32Quiet;       // , s32Rate;
        G723_S32 FlLen;

        // G723_S8 Rate_Rd;
        G723_S32 s32NumMemReqs;
        sG723EMemAllocInfoSubType *psMem;
        FILE   *fp_input;
        FILE   *fp_output;
        int     ret = TPASS;
        int     TotalMem = 0;

        ps16InBuf = NULL;
        ps16OutBuf = NULL;
        /* Allocate memory for encoder configuration structure */
        g732_encoder_thread_t *g732_encoder = (g732_encoder_thread_t *) ptr;

        g732_encoder->buf_pr = (unsigned char *) malloc(30000);


        FlLen = s32G723EProcessCmdLineOptions(g732_encoder, &fp_input, &fp_output,
                                              &s32UseHighPass, &s32UseVx, &s32Quiet);
        g732_encoder->numberframe = 0;
        /* Allocate memory for encoder to use */
        g732_encoder->psEncConfig.pvG723EEncodeInfoPtr = NULL;

        g732_encoder->buf_byte = 0;
        /* Not Use */
        g732_encoder->psEncConfig.pu8APPEInitializedDataStart = NULL;
        /* Query for memory */
        if (eG723EQueryMem(&g732_encoder->psEncConfig) != E_G723E_OK)
        {       /* Deallocate memory allocated for encoder config */
                eG723EEncodeExit(fp_input, fp_output, &g732_encoder->psEncConfig, ps16InBuf,
                                 ps16OutBuf);
                return TFAIL;
        }
        /* Number of memory chunk requests by the encoder */
        s32NumMemReqs = g732_encoder->psEncConfig.sG723EMemInfo.s32G723ENumMemReqs;
        /* Allocate memory requested by the encoder */

        for (s32i = 0; s32i < s32NumMemReqs; s32i++)
        {

                psMem = &(g732_encoder->psEncConfig.sG723EMemInfo.asMemInfoSub[s32i]);
                if (psMem->u8G723EMemTypeFs == G723_FAST_MEMORY)
                {       /* Check for priority and memory description can be added here */
                        psMem->pvAPPEBasePtr = alloc_fast(psMem->s32G723ESize);
                }
                else
                        psMem->pvAPPEBasePtr = alloc_slow(psMem->s32G723ESize);

                TotalMem += psMem->s32G723ESize;
        }
        tst_resm(TINFO, "[Encoder %d] Total dynamic memory allocated for library %d",
                 g732_encoder->Index, TotalMem);

        /* Initialize the G723 encoder */

        if (eG723EEncodeInit(&g732_encoder->psEncConfig) != E_G723E_OK)
        {
                eG723EEncodeExit(fp_input, fp_output, &g732_encoder->psEncConfig, ps16InBuf,
                                 ps16OutBuf);
                return TFAIL;
        }

        /* Allocate memory for input buffer */
        if ((ps16InBuf = alloc_fast(G723_L_FRAME * sizeof(G723_S16))) == NULL)
        {
                eG723EEncodeExit(fp_input, fp_output, &g732_encoder->psEncConfig, ps16InBuf,
                                 ps16OutBuf);
                return TFAIL;
        }

        if ((ps16OutBuf = alloc_fast((CODED_FRAMESIZE / 2) * sizeof(G723_S16))) == NULL)
        {
                eG723EEncodeExit(fp_input, fp_output, &g732_encoder->psEncConfig, ps16InBuf,
                                 ps16OutBuf);
                return TFAIL;
        }


        while (g732_encoder->numberframe < FlLen)
        {
                vReadlbc(ps16InBuf, G723_L_FRAME, fp_input);
                /* if testing re-locability Flag=1 */
                if ((!(g732_encoder->numberframe % RELOCATE_CYCLE)) && Flag)
                {

                        if ((ret = realloc_enc_memory(g732_encoder)) == TPASS)
                        {
                                if (eG723EEncodeInit(&g732_encoder->psEncConfig) != E_G723E_OK)
                                {
                                        tst_resm(TFAIL,
                                                 "ERROR in g723 run_encoder(): g723_encode_init() "
                                                 "returns error  E_G723E_INIT_ERROR");
                                        if (!Flag_write)
                                        {
                                                fprintf(fp_mes,
                                                        " ERROR in g723 run_encoder(): g723_encode_init() "
                                                        "returns error  E_G723E_INIT_ERROR\n,"
                                                        "Encoder[%d]  file_name_input= %s\n file_name_output=%s",
                                                        g732_encoder->Index,
                                                        g732_encoder->file_name_input,
                                                        g732_encoder->file_name_output);
                                        }
                                        eG723EEncodeExit(fp_input, fp_output,
                                                         &g732_encoder->psEncConfig, ps16InBuf,
                                                         ps16OutBuf);
                                        return TFAIL;

                                }
                        }

                }


                if (eG723EEncodeFrame(&g732_encoder->psEncConfig, ps16InBuf, ps16OutBuf) !=
                    E_G723E_OK)
                {
                        eG723EEncodeExit(fp_input, fp_output, &g732_encoder->psEncConfig, ps16InBuf,
                                         ps16OutBuf);
                        return TFAIL;
                }

                LineWrite(g732_encoder, (G723_S8 *) ps16OutBuf, fp_output,
                          (G723_S8 *) g732_encoder->buf_pr);
                g732_encoder->numberframe++;

                fprintf(stdout, "Encoder[%d]  Done : number of the frame  %3d   %3d \r",
                        g732_encoder->Index, g732_encoder->numberframe,
                        g732_encoder->numberframe * 100 / FlLen);
                fflush(stdout);

        }       // end while


        /*************************************************************
         * Closedown speech coder
         ************************************************************/
        eG723EEncodeExit(fp_input, fp_output, &g732_encoder->psEncConfig, ps16InBuf, ps16OutBuf);

        if (compare_files(g732_encoder) != TRUE)
                ret = TFAIL;

        return ret;

}

/*================================================================================================*/
/*===== enter_encode=====*/
/**
@brief  function write output  data  to the file or buffer

@param  the buffer output .

@return On success - return TPASS
        On failure - return the error code
*/
/*================================================================================================*/

int LineWrite(void *ptr, G723_S8 * ps8Line, FILE * Fp, G723_S8 * buf_pr)
{
        G723_S16 s16Info;
        G723_S32 s32Size;
        int     i;

        g732_encoder_thread_t *g732_encoder = (g732_encoder_thread_t *) ptr;

        s16Info = ps8Line[0] & (G723_S16) 0x0003;
        /* Check frame type and rate informations */
        switch (s16Info)
        {
        case 0x0002:   /* SID frame */
                s32Size = 4;
                break;

        case 0x0003:   /* untransmitted silence frame */
                s32Size = 1;
                break;

        case 0x0001:   /* active frame, low rate */
                s32Size = 20;
                break;

        default:       /* active frame, high rate */
                s32Size = 24;
        }

        if (Flag_write == 0)
                fwrite(ps8Line, s32Size, 1, Fp);
        else
        {
                for (i = 0; i < s32Size; i++)
                        buf_pr[g732_encoder->buf_byte + i] = ps8Line[i];
                g732_encoder->buf_byte += i;
        }

        return 0;
}



/*================================================================================================*/
/*===== realloc_enc_memory =====*/
/**
@brief  This method moves the data memory to a different adress spase.

@param  None

@return On success - return TPASS
        On failure - return the error code
*/
/*================================================================================================*/

int realloc_enc_memory(g732_encoder_thread_t * g732_encoder)
{

        G723_S32 s32NumMemReqs;
        sG723EMemAllocInfoSubType *psMem;
        int     i;
        int     TotalMemory = 0;
        void   *barrier_ptr;
        int     ret = TPASS;

        if (!g732_encoder)
        {
                tst_resm(TFAIL, "ERROR in realloc_enc_memory(): invalid parameter");
                return TFAIL;
        }

        s32NumMemReqs = g732_encoder->psEncConfig.sG723EMemInfo.s32G723ENumMemReqs;
        /* Allocate memory requested by the encoder */
        for (i = 0; i < s32NumMemReqs; i++)
        {
                psMem = &(g732_encoder->psEncConfig.sG723EMemInfo.asMemInfoSub[i]);
                if (psMem->pvAPPEBasePtr)
                        free(psMem->pvAPPEBasePtr);
        }



        /* Allocate some memory to be sure that encoder memory will be allocated in other place */
        barrier_ptr = malloc(CODED_FRAMESIZE * g732_encoder->numberframe);
        if (!barrier_ptr)
        {
                tst_resm(TFAIL, "ERROR in realloc_enc_memory(): malloc() for barrier_ptr returns");
                ret = TFAIL;
        }
        else
        {
                for (i = 0; i < s32NumMemReqs; i++)
                {
                        psMem = &(g732_encoder->psEncConfig.sG723EMemInfo.asMemInfoSub[i]);
                        if (psMem->u8G723EMemTypeFs == G723_FAST_MEMORY)
                                psMem->pvAPPEBasePtr = alloc_fast(psMem->s32G723ESize);
                        else
                                psMem->pvAPPEBasePtr = alloc_slow(psMem->s32G723ESize);

                        if (!psMem->pvAPPEBasePtr)
                        {
                                tst_resm(TFAIL,
                                         "ERROR in realloc_enc_memory: malloc() for chunk %d returns %s",
                                         i, strerror(errno));
                                if (!Flag_write)
                                {
                                        fprintf(fp_mes, " ERROR in realloc_enc_memory Encoder[%d]"
                                                "  file_name_input= %s file_name_output=%s \n",
                                                g732_encoder->Index, g732_encoder->file_name_input,
                                                g732_encoder->file_name_output);
                                }
                                ret = TFAIL;
                        }
                        TotalMemory += psMem->s32G723ESize;
                }


                free(barrier_ptr);
        }

        if (!Flag_write)
                fprintf(fp_mes, "[Encoder %d] new allocate  total  memory  %d", g732_encoder->Index,
                        TotalMemory);

        return ret;
}

/*================================================================================================*/
/*===== eG723EEncodeExit=====*/
/**
@brief  function exit all opened files and free memmory

@param   None

@return 
*/
/*================================================================================================*/
int eG723EEncodeExit(FILE * InpFile, FILE * OutFile,
                     sG723EEncoderConfigType * psEncConfig,
                     G723_S16 * ps16InBuf, G723_S16 * ps16OutBuf)
{

        G723_S16 s16Counter;

        if (ps16InBuf != NULL)
        {
                /* free input buffer */
                mem_free(ps16InBuf);
                ps16InBuf = NULL;
        }

        if (ps16OutBuf != NULL)
        {
                /* free output buffer */
                mem_free(ps16OutBuf);
                ps16OutBuf = NULL;
        }

        /* Free all the memory allocated for encoder config param */
        for (s16Counter = 0; s16Counter < psEncConfig->sG723EMemInfo.s32G723ENumMemReqs;
             s16Counter++)
        {
                if ((psEncConfig->sG723EMemInfo.asMemInfoSub[s16Counter].pvAPPEBasePtr) != NULL)
                {
                        mem_free((psEncConfig->sG723EMemInfo.asMemInfoSub[s16Counter].
                                  pvAPPEBasePtr));
                        (psEncConfig->sG723EMemInfo.asMemInfoSub[s16Counter].pvAPPEBasePtr) = NULL;
                }
        }

        if (InpFile)
                fclose(InpFile);
        if (OutFile)
                fclose(OutFile);


        return 0;
}
