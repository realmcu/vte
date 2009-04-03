/*================================================================================================*/
/**
    @file   aaclc_decoder_test.c

    @brief  C source file of the AAC LC decoder test application.
*/
/*==================================================================================================

  Copyright (C) 2004, Freescale Semiconductor, Inc. All Rights Reserved
  THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
  BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
  Freescale Semiconductor, Inc.

====================================================================================================
Revision History:
                            Modification     Tracking
Author                          Date          Number    Description of Changes
-------------------------   ------------    ----------  -------------------------------------------
Igor Semenchukov/smng001c    26/10/2004     TLSbo43521   Initial version
Igor Semenchukov/smng001c    04/02/2005     TLSbo47179   Changed include directives (lib headers)
Igor Semenchukov/smng001c    28/02/2005     TLSbo47115   Changed printf() entries to tst_...()
Igor Semenchukov/smng001c    29/03/2005     TLSbo48795   Updated due to changes in the codec's code
D.Simakov/smkd001c           22/10/2005     TLSbo57009   dif32 is used for bit-matching
D.Simakov/smkd001c           30/11/2005     TLSbo59613   Bus error was fixed
D.Simakov/smkd001c           15/12/2005     TLSbo59668   Test resync feature of ADTS streams
====================================================================================================
Portability: Indicate if this module is portable to other compilers or platforms.
             If not, indicate specific reasons why is it not portable.

==================================================================================================*/

#ifdef __cplusplus
extern "C"{
#endif

/*==================================================================================================
                                        INCLUDE FILES
==================================================================================================*/

/* Standard Include Files */

#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <math.h>
#include <assert.h>

/* Harness Specific Include Files. */

#include "test.h"

/* Verification Test Environment Include Files */

#include "aaclc_decoder_test.h"

/*==================================================================================================
                                        LOCAL MACROS
==================================================================================================*/
#define MAX_DEC_THREADS 4
#define RELOCATE_CYCLE  10
#define EMPTY_FILE      "n/a"
#define MAX_NICE_PRIOR  19
#define ADIF_ID         0x41444946  /* "ADIF"        */
#define ADTS_ID         0xFFF00000  /* ADTS syncword */
#define IS_ADIF         1
#define IS_ADTS         2

/*==================================================================================================
                          LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
==================================================================================================*/

typedef struct
{
    char *fname;
    FILE *fptr;
} filehandle;

/* Structure stores decoder instance/thread parameters, buffers etc. */

typedef struct
{
    int                 instance_id;
    filehandle          finput,
                        foutput,
                        fref[Chans];

    AACD_INT8*          in_buf;
    AACD_OutputFmtType  out_buf[Chans][AAC_FRAME_SIZE];
    AACD_Decoder_Config *aac_config;

    /* Some service variables */

    pthread_t           tid;                        /* Thread ID. Used only if thread is created */
    int                 th_nice;                    /* Nice value for preemption test            */
    int                 th_finish;                  /* */
    int                 framecount;

    int                 th_err;                     /* Error value returned by pthread_join()    */
    int                 ltp_err;                    /* LTP error value returned by thread        */
    AACD_RET_TYPE       aacd_err;                   /* Decoder functions return values           */

    /* These vars must be deleted because decoder itself must be responsible for their creation  */

    void *aptable[58];                           /* My app must not responsible for this var     */
    char global_buffer[AACD_GLOBAL_STRUCT_SIZE]; /* Is this buffer must be created by my app???? */
} aaclc_dec_inst;

/*==================================================================================================
                                       LOCAL CONSTANTS
==================================================================================================*/
const char def_list_file[]     = "list/aacd_def_test_files";
const char adts_list_file[]    = "list/aacd_adts_test_files";
const char rob_list_file[]     = "list/aacd_rob_test_files";
const char progress[]          = "-\\|/";      /* For rounding indicator */

/*==================================================================================================
                                       LOCAL VARIABLES
==================================================================================================*/
static aaclc_dec_inst dec_inst[MAX_DEC_THREADS];
static int            test_iter;
static flist_t        *files_list      = NULL;
int                   preempt_test     = FALSE; /* Acts as a switch in the run_decoder_thread()   */
int                   relocate_test    = FALSE; /* Acts as a switch in the aaclc_decoder_engine() */
int                   thread_test      = 0;     /* 1 - multiple threads will be ran               */
int                   th_count         = 0;     /* Used when some info must be printed            */
int                   th_printing      = -1;    /* Helps thread to determine it is print. thread  */

/*==================================================================================================
                                       GLOBAL CONSTANTS
==================================================================================================*/


/*==================================================================================================
                                       GLOBAL VARIABLES
==================================================================================================*/


/*==================================================================================================
                                   LOCAL FUNCTION PROTOTYPES
==================================================================================================*/

/* Please see comments to these functions at their implementation */

int   aaclc_decoder_engine(aaclc_dec_inst* inst);
int   alloc_dec_buffers(aaclc_dec_inst* instance);
int   realloc_dec_memory(aaclc_dec_inst* instance);
void  dec_cleanup(aaclc_dec_inst* instance);
int   open_fstreams(aaclc_dec_inst* instance);
int   write_frame(AACD_OutputFmtType outbuf[][AAC_FRAME_SIZE], FILE* fptr, MC_Info* p);
void  print_status(void);
char  app_swap_buffers_aac_dec(AACD_UCHAR** new_buf_ptr, AACD_UINT32* new_buf_size, AACD_Decoder_Config* conf);
int   perform_bitmatch_hex(filehandle* out, filehandle ref[], int refcnt);
int   set_dec_instance(int index, flist_t* list_node);
void* run_decoder_thread(void* instance);
void  hogcpu(void);
void  tables_init(void);

flist_t* mk_entry(const char* inp_fname, const char* out_fname, const char* ref_fname);
void     delete_list(flist_t* list);
int      read_cfg(const char* filename, flist_t** pplist);

int        parse_aac_header(FILE* fptr, AACD_Block_Params* params);
int        get_adif_header(FILE* fptr, AACD_Block_Params* params);
int        get_adts_header(FILE* fptr, AACD_Block_Params* params);
int        get_pce_section(FILE* fptr, AACD_ProgConfig* pce, unsigned int* bitoffp);
int        get_ele_section(FILE* fptr, AACD_EleList* list, int en_cpe, unsigned int* bitoffp);
AACD_INT32 read_hdr_val(FILE* fptr, unsigned int* beg, unsigned int cnt, int* error);

/* test functions */

int nominal_functionality_test();
int preemption_test();
int reentrance_test();
int relocatability_test();
int load_environment_test();
int adts_streams_test();
int robustness_test();
int endurance_test();

/*==================================================================================================
                                       LOCAL FUNCTIONS
==================================================================================================*/


/*================================================================================================*/
/*===== VT_aaclc_decoder_setup =====*/
/**
@brief  assumes the pre-condition of the test case execution

@param  None

@return On success - return TPASS
        On failure - return the error code
*/
/*================================================================================================*/
int VT_aaclc_decoder_setup()
{
    return TPASS;
}

/*================================================================================================*/
/*===== VT_aaclc_decoder_cleanup =====*/
/**
@brief  assumes the post-condition of the test case execution

@param  None

@return On success - return TPASS
        On failure - return the error code
*/
/*================================================================================================*/
int VT_aaclc_decoder_cleanup()
{
    return TPASS;
}

/*================================================================================================*/
/*===== VT_aaclc_decoder_test =====*/
/**
@brief  Reads list of files (input, output and reference). Executes test specified by 'testcase'
        variable.

@param  Input:  testcase - Testcase id of the test according to the test plan
        	iter     - Iteration of the loop in case of an endurance/stress test
        Output: None
        

@return On success - return TPASS
        On failure - return the error code
*/
/*================================================================================================*/
int VT_aaclc_decoder_test(int testcase, int iter, char *listfile)
{
    int rv = TFAIL;

    test_iter = iter;   /* Number of iterations - valid only for endurance test */

    /*
     * Clear file list, open appropriate listfile (specified by input variable or default) and
     * and read its contents into list. Two test cases need their own files.
     */

    if (files_list)
        delete_list(files_list);
    if (!listfile)
    {
        if (testcase == ADTS_STREAMS)
            listfile = (char *)adts_list_file;
        /*else if (testcase == ROBUSTNESS)
            listfile = (char *)rob_list_file;*/
        else
            listfile = (char *)def_list_file;
    }

    if (!read_cfg(listfile, &files_list))
        return rv;

    tables_init();      /* This line does some work that must be done by library */
    
    tst_resm(TINFO, "List of files will be taken from %s", listfile);
    switch (testcase)
    {
	case NOMINAL_FUNCTIONALITY:
	    tst_resm(TINFO, "Nominal functionality test");
	    rv = nominal_functionality_test();
    	    break;

	case PREEMPTION:
	    tst_resm(TINFO, "Preemption test");
	    rv = preemption_test();
	    break;

	case REENTRANCE:
	    tst_resm(TINFO, "Reentrance test");
	    rv = reentrance_test();
	    break;
	case RELOCATABILITY:
	    tst_resm(TINFO, "Relocatability test");
	    rv = relocatability_test();
	    break;

	case LOAD_ENVIRONMENT:
	    tst_resm(TINFO, "Load environment test");
	    rv = load_environment_test();
	    break;

	case ENDURANCE:
	    tst_resm(TINFO, "Endurance test");
	    rv = endurance_test();
	    break;

    case ADTS_STREAMS:
        tst_resm(TINFO, "ADTS streams test");
        rv = adts_streams_test();
        break;                                        

	default:
	    tst_resm(TWARN, "Wrong test case!!");
	    break;
    }

    return rv;
}

/*================================================================================================*/
/*===== aaclc_decoder_engine =====*/
/**
@brief  Engine of the decoder. Performs decoding of bitstream.
	Also this function saves decoder result, if needed.
	This method is compatible with threads.

@param  Input:  inst - pointer to the structure holding buffers, decoder config structure etc.
        Output: None

@return On success - return TPASS
        On failure - return the error code
*/
/*================================================================================================*/
int aaclc_decoder_engine(aaclc_dec_inst *inst)
{
    AACD_Decoder_Config* conf;         /* Pointer to simplify references                     */
    AACD_Decoder_info    dec_info;     /* Stores decoder parameters                          */
    AACD_Decoder_info    dec_info_prt; /* For printing                                       */
    AACD_global_struct*  ptr;
    AACD_UINT32          inbuf_len;
    AACD_UCHAR*          inbuf_ptr;
    int  header      = 0;
    long avg_bitrate = 0;             /* Calculate avg bitrate and print it at summary info */
    int  ret         = TPASS;

    assert(inst);

    /* Allocate memory for decoder parameter structure, populate known fields */

    inst->aac_config = (AACD_Decoder_Config *)malloc(sizeof(AACD_Decoder_Config));
    if (!inst->aac_config)
    {
        tst_resm(TWARN, "ERROR in aaclc_decoder_engine(): malloc() for decoder config returns %s",
                 strerror(errno));
        ret = TFAIL;
    }

    if (ret == TPASS)
    {
        conf = inst->aac_config;
        conf->aacd_decode_info_struct_ptr = (void *)inst->global_buffer; /* Don't know why ... */
        conf->aacd_initialized_data_start = (void *)inst->aptable;
        conf->num_pcm_bits = AACD_16_BIT_OUTPUT;
        conf->app_swap_buf = app_swap_buffers_aac_dec;

        /* Get decoder memory requirements. Info will be placed in the aacd_mem_info field */

        inst->aacd_err = aacd_query_dec_mem(conf);
        if (inst->aacd_err != AACD_ERROR_NO_ERROR)
        {
            tst_resm(TWARN, "ERROR in aaclc_decoder_engine(): aacd_query_dec_mem() returns error %d", inst->aacd_err);
            ret = TFAIL;
        }
    }

    if (ret == TPASS)
    {
        if (alloc_dec_buffers(inst) != TRUE)
        {
            ret = TFAIL;
        }            
    }

    /*
     * Open input and output streams. Input stream must be opened before aacd_decoder_init()
     * will be called, because it calls app_swap_buffers_aac_dec(), which reads from the
     * input stream.
     */

    if (ret == TPASS)
    {
        if (open_fstreams(inst) != TRUE) 
        {
            ret = TFAIL;
        }            
    }

    if (ret == TPASS)
    {
        header = parse_aac_header(inst->finput.fptr, conf->params);
        if (!header)
        {
            ret = TFAIL;
        }            
    }

    if (ret == TPASS)
    {
        inst->aacd_err = aacd_decoder_init(conf);
        if (inst->aacd_err != AACD_ERROR_NO_ERROR)
        {
            tst_resm(TWARN, "ERROR in aaclc_decoder_engine(): aacd_decoder_init() returns error %d", inst->aacd_err);
            ret = TFAIL;
        }
        else
            ptr = (AACD_global_struct*)conf->aacd_decode_info_struct_ptr;
    }

    /* If an error was occured in one of the initialization steps, cleanup instance and exit */

    if (ret != TPASS)
    {
        dec_cleanup(inst);
        return ret;
    }

    if (th_printing == -1)      /* If there is no printing thread, we become it  */
        th_printing = inst->instance_id;

#ifdef DEBUG_TEST
    if (thread_test == 0)       /* Print more info if only one thread is running */
        printf("\nInput file: %s\n", inst->finput.fname);
#endif
        
    /* 
     * Main decoding cycle continues while end of input file was reached.
     * When a regular frame was decoded, output buffer contents are written into the
     * output file and next frame's turn begins
     */

    if (app_swap_buffers_aac_dec(&inbuf_ptr, &inbuf_len, conf))
    {
        ret = TFAIL;
    }        
    while ( (ret == TPASS) && (inst->aacd_err != AACD_ERROR_EOF) && (inbuf_len > 0) )
    {
        inst->aacd_err = aacd_decode_frame(conf, &dec_info, inst->out_buf, inst->in_buf, inbuf_len);
        inst->framecount++;

        if (fseek(inst->finput.fptr, dec_info.BitsInBlock / LEN_BYTE - inbuf_len, SEEK_CUR))
        {
            ret = TFAIL;
        }            

        /* Find new ADTS frame */

        if (header == IS_ADTS && inst->aacd_err != AACD_ERROR_EOF)
        {
            header = parse_aac_header(inst->finput.fptr, conf->params);
            if (!header)
            {
                ret = TFAIL;
            }                
        }

        if (app_swap_buffers_aac_dec(&inbuf_ptr, &inbuf_len, conf))
        {
            ret = TFAIL;
        }            

        if (inst->aacd_err == AACD_ERROR_NO_ERROR)
        {
            if (th_printing == inst->instance_id)
                print_status();
            
            /*
             * Each frame may use different bitrate, so we calculate average and print it in
             * summary results output
             */

            if (th_count == 1)
                avg_bitrate += (long)dec_info.aacd_bit_rate;

            /* If output file name is set in config list file, write to this file */
            
            if (inst->foutput.fptr && inst->framecount > 2)
            {
                if (write_frame(inst->out_buf, inst->foutput.fptr, ptr->AACD_mip) == FALSE)
                {
                    ret = TFAIL;
                }
            }

            /* Last frame doesn't contain any info, so we copy it in the temporary variable  */

            if ( (th_count == 1) && (inst->framecount == 3) )
                dec_info_prt = dec_info;

            /* In relocatability test realloc decoder memory every 'RELOCATE_CYCLE'-th frame */

            if ( (!(inst->framecount % RELOCATE_CYCLE)) && relocate_test )
            {
                if (realloc_dec_memory(inst) != TRUE )
                {
                    ret = TFAIL;
                }
            }                
        }
        else if (inst->aacd_err != AACD_ERROR_EOF)
        {
            ret = TFAIL;
            tst_resm(TWARN, "ERROR %d in aacd_decode_frame()", inst->aacd_err);
        }
    }

    if (th_count == 1)  /* Print more info if only one thread is running */
    {
        printf("\n");
#ifdef DEBUG_TEST
        avg_bitrate /= (long)(inst->framecount);
        printf("\nStream parameters:\n\tSampling frequency: %d Hz\n\tNumber of channels: %d"
               "\n\tAverage bitrate   : %d b/s\n\tFrame size\t  : %d bytes\n",
               dec_info_prt.aacd_sampling_frequency, dec_info_prt.aacd_num_channels,
               (int)avg_bitrate, dec_info_prt.aacd_len);
#endif          /* DEBUG_TEST */
    }

    dec_cleanup(inst);
    
    return ret;
}

/*================================================================================================*/
/*===== print_status =====*/
/**
@brief  Prints number of frames decoded for all running threads.

@param  Input:  None
        Output: None

@return None
*/
/*================================================================================================*/
void print_status(void)
{
    int i;
    aaclc_dec_inst *inst;

    for (i = 0; i < th_count; i++)
    {
        inst = &dec_inst[i];
        printf("th[%d]-", inst->instance_id + 1);
        if (inst->th_finish) printf("ended ");
        else printf("frames");
        printf("[%3d] ", inst->framecount);
    }
    if (inst)
        printf("%c\r", progress[inst->framecount % (sizeof(progress) - 1)]);
    fflush(stdout);
    return;
}

/*================================================================================================*/
/*===== open_fstreams =====*/
/**
@brief  Opens streams associated with input and output files.

@param  Input:  instance - pointer to the structure holding buffers, decoder config structure etc.
        Output: None

@return On success - return TRUE
        On failure - return FALSE
*/
/*================================================================================================*/
int open_fstreams(aaclc_dec_inst *instance)
{
    if (!instance)  /* Error: NULL pointer */
    {
        tst_resm(TWARN, "ERROR in open_fstreams(): invalid parameter");
        return FALSE;
    }

    if ((instance->finput.fptr = fopen(instance->finput.fname, "r")) == NULL)
    {
        tst_resm(TWARN, "ERROR in open_fstreams(): fopen() for %s returns %s",
                 instance->finput.fname, strerror(errno));
        return FALSE;
    }
    if (instance->foutput.fname != NULL)
    {
        if ((instance->foutput.fptr = fopen(instance->foutput.fname, "w")) == NULL)
        {
            tst_resm(TWARN, "ERROR in open_fstreams(): fopen() for %s returns %s",
                     instance->foutput.fname, strerror(errno));
            return FALSE;
        }
    }

    return TRUE;
}

/*================================================================================================*/
/*===== alloc_dec_buffers =====*/
/**
@brief  Allocates memory for:
            all chunks requested by decoder (as returned by aacd_query_dec_mem());
            application input buffer.

@param  Input:  instance - pointer to the structure holding buffers, decoder config structure etc.
        Output: None

@return On success - return TRUE
        On failure - return FALSE
*/
/*================================================================================================*/
int alloc_dec_buffers(aaclc_dec_inst *instance)
{
    AACD_INT32               chunk_cnt;
    AACD_Decoder_Config*     conf;
    AACD_Mem_Alloc_Info_Sub* mem;
    int i;
    int ret = TRUE;

    if (!instance)
    {
        tst_resm(TWARN, "ERROR in alloc_dec_buffers(): invalid parameter");
        return FALSE;
    }
    conf = instance->aac_config;

    /* Allocate memory for all required chunks and buffers */

    chunk_cnt = conf->aacd_mem_info.aacd_num_reqs;
    for (i = 0; i < chunk_cnt; i++)
    {
        mem = &(conf->aacd_mem_info.mem_info_sub[i]);
        mem->app_base_ptr = malloc(mem->aacd_size);
        if (!mem->app_base_ptr)
        {
            tst_resm(TWARN, "ERROR in alloc_dec_buffers(): malloc() for chunk %d returns %s",
                     i, strerror(errno));
            ret = FALSE;
        }
    }

    if (ret)
    {
        conf->params = (AACD_Block_Params*)malloc(sizeof(AACD_Block_Params));            
        if (!conf->params)
        {
            tst_resm(TWARN, "ERROR in alloc_dec_buffers(): malloc() for params returns %s", strerror(errno));
            ret = FALSE;
        }
        memset( conf->params, 0, sizeof(AACD_Block_Params) );
    }

    if (ret)
    {
        instance->in_buf = (AACD_INT8*)malloc(AACD_INPUT_BUFFER_SIZE);
        if (!instance->in_buf)
        {
            tst_resm(TWARN, "ERROR in alloc_dec_buffers(): malloc() for input buffer returns %s",
                     strerror(errno));
            ret = FALSE;
        }
    }

    return ret;
}

/*================================================================================================*/
/*===== dec_cleanup =====*/
/**
@brief  Releases file streams allocated by open_fstreams().
        Frees memory allocated by alloc_dec_buffers().

@param  Input:  instance - pointer to the structure holding buffers, decoder config structure etc.
        Output: None

@return None
*/
/*================================================================================================*/
void dec_cleanup(aaclc_dec_inst *instance)
{
    int i;
    int nr;
    AACD_Decoder_Config *conf;
    AACD_Mem_Alloc_Info_Sub *mem;
    
    if (!instance)
    {
        tst_resm(TWARN, "ERROR in dec_cleanup(): invalid parameter");
        return;
    }

    if (instance->finput.fptr)
        fclose(instance->finput.fptr);
    if (instance->foutput.fptr)
        fclose(instance->foutput.fptr);

    conf = instance->aac_config;
    if(instance->in_buf)
    {
        free(instance->in_buf);
    }        
        
    nr = conf->aacd_mem_info.aacd_num_reqs;
    for (i = 0; i < nr; i++)
    {
        mem = &(conf->aacd_mem_info.mem_info_sub[i]);
        if (mem->app_base_ptr)
        {
            free(mem->app_base_ptr);
            mem->app_base_ptr = NULL;
        }
    }

    if (conf->params->pce)
    {
        free(conf->params->pce);
        conf->params->pce = NULL;
    }

    if (conf->params)
    {
        free(conf->params);
        conf->params = NULL;
    }

    if (conf)
    {
        free(conf);
        conf = NULL;
    }

    return;
}

/*================================================================================================*/
/*===== realloc_dec_memory =====*/
/**
@brief  Frees decoder memory and allocates it again, but in other place.

@param  Input:  instance - pointer to the structure holding buffers, decoder config structure etc.
        Output: None

@return On success - return TRUE
        On failure - return FALSE
*/
/*================================================================================================*/
int realloc_dec_memory(aaclc_dec_inst *instance)
{
    AACD_Decoder_Config*     conf;
    AACD_Mem_Alloc_Info_Sub* mem;
    AACD_INT32               nr;
    int  i;
    void *barrier_ptr;
    int  ret = TRUE;
    
    if (!instance)
    {
        tst_resm(TWARN, "ERROR in realloc_dec_memory(): invalid parameter");
        return FALSE;
    }
    conf = instance->aac_config;

    /* Deallocate all memory chunk and then again allocate them */

    nr = conf->aacd_mem_info.aacd_num_reqs;
    for (i = 0; i < nr; i++)
    {
        mem = &(conf->aacd_mem_info.mem_info_sub[i]);
        if (mem->app_base_ptr)
            free(mem->app_base_ptr);
    }

    /* Allocate some memory to be sure that decoder memory will be allocated in other place */

    barrier_ptr = malloc(AACD_INPUT_BUFFER_SIZE * instance->framecount);
    if (!barrier_ptr)
    {
        tst_resm(TWARN, "ERROR in realloc_dec_memory(): malloc() for barrier_ptr returns %s",
                 strerror(errno));
        ret = FALSE;
    }
    else
    {
        for (i = 0; i < nr; i++)
        {
            mem = &(conf->aacd_mem_info.mem_info_sub[i]);
            mem->app_base_ptr = malloc(mem->aacd_size);
            if (!mem->app_base_ptr)
            {
                tst_resm(TWARN, "ERROR in realloc_dec_memory: malloc() for chunk %d returns %s",
                         i, strerror(errno));
                ret = FALSE;
            }
        }
        free(barrier_ptr);
    }

    return ret;
}


/*================================================================================================*/
/*===== write_frame =====*/
/**
@brief  Builds interlaced multichannel frame from non-interlaced frame and writes it into the
        output stream.

@param  Input:  outbuf   - decoder output buffer
                fptr     - output file stream pointer
                p        - pointer to structure containing channels availability info
        Output: None

@return On success - return TRUE
        On failure - return FALSE
*/
/*================================================================================================*/
int write_frame(AACD_OutputFmtType outbuf[][AAC_FRAME_SIZE], FILE *fptr, MC_Info* p)
{
    char   str_buf[MAX_STR_LEN];
    int    i, j;
    int    ret = TRUE;

    assert(fptr);
    for (i = 0; (i < Chans) && (ret = TRUE); ++i)
    {
        if (p->ch_info[i].present)
        {
            for (j = 0; j < AAC_FRAME_SIZE; j++)
            {
                sprintf(str_buf, "%08lx\r\n", outbuf[i][j]);
                fwrite(str_buf, 1, strlen(str_buf), fptr);
                if (ferror(fptr))
                {
                    tst_resm(TWARN, "ERROR in write_frame(): fwrite() returns error %s", strerror(errno));
                    ret = FALSE;
                }
            }
        }
    }

    return ret;
}

/*================================================================================================*/
/*===== app_swap_buffers_aac_dec =====*/
/**
@brief  Called by decoder when it runs out of current bit stream input buffer. Gets a new buffer
        for decoding.

@param  Input:  instance_id - decoder instance ID
        Output: new_buf_ptr - pointer to the new buffer given by the application
                new_buf_len - pointer to length of the new buffer in bytes


@return On success - return zero
        On failure - return the negative error code
*/
/*================================================================================================*/
char app_swap_buffers_aac_dec(AACD_UCHAR** new_buf_ptr, AACD_UINT32* new_buf_size, AACD_Decoder_Config* conf)
{
    aaclc_dec_inst *instance;
    int i;
    int ret = -1;
    
    *new_buf_size = 0;   /* Must be 0 and NULL       */
    *new_buf_ptr = NULL; /* if an error will occured */

    /*
     * If instance_id, input file pointer are valid and we aren't at end of file, read next block
     * into the input buffer.
     */

    for (i = 0; i < MAX_DEC_THREADS; i++)
    {
        if (conf == dec_inst[i].aac_config)
        {
            instance = &dec_inst[i];
            if (!feof(instance->finput.fptr))
            {
                *new_buf_size = fread((void *)instance->in_buf, 1, AACD_INPUT_BUFFER_SIZE, instance->finput.fptr);
                if(!ferror(instance->finput.fptr))
                {
                    *new_buf_ptr = instance->in_buf;
                    ret = 0;
                }
                else
                {
                    *new_buf_size = 0;
                }
            }
            break;
        }
    }
    return ret;
}

/*================================================================================================*/
/*===== run_decoder_thread =====*/
/**
@brief  This is a thread function. It changes process priority in case of preemption test and
        runs decoder engine.

@param  instance - void pointer to thread arguments.

@return NULL
*/
/*================================================================================================*/
void *run_decoder_thread(void *instance)
{
    int i;
    aaclc_dec_inst *inst = (aaclc_dec_inst *)instance;
    int nice_inc = MAX_NICE_PRIOR / (MAX_DEC_THREADS - 1);

    if (preempt_test)
        inst->th_nice = nice(nice_inc * inst->instance_id);

    inst->ltp_err = aaclc_decoder_engine(inst);
    inst->th_finish = TRUE;

    /*
     * If it was a printing thread, find another working thread and aasign its instance_id to
     * th_printing variable
     */

    if (th_printing == inst->instance_id)
    {
        th_printing = -1;
        for (i = 0; i < th_count; i++)
        {
            if (dec_inst[i].th_finish == FALSE)
            {
                th_printing = dec_inst[i].instance_id;
                break;
            }
        }
    }

    return NULL;
}

/*================================================================================================*/
/*===== hogcpu=====*/
/**
@brief  Hogs the CPU for stress test in a load environment.

@param  None

@return None
*/
/*================================================================================================*/
void hogcpu()
{
    while (1) sqrt(rand());
}

/*================================================================================================*/
/*===== set_dec_instance =====*/
/**
@brief  Sets instance ID and file names.

@param  index     - index of instance that will be used
        list_node - pointer to the current flist_t entry (that stores file names)

@return On success - return TRUE
        On failure - return FALSE
*/
/*================================================================================================*/
int set_dec_instance(int index, flist_t *list_node)
{
    int i;

    if ( (index >= MAX_DEC_THREADS) || (!list_node) )
    {
        tst_resm(TWARN, "ERROR in set_dec_instance(): one of parameters isn't valid");
        return FALSE;
    }

    dec_inst[index].framecount = 0;
    dec_inst[index].th_nice = 0;
    dec_inst[index].th_finish = FALSE;
    dec_inst[index].instance_id = index;
    dec_inst[index].finput.fname = list_node->inp_fname; /* fname isn't "n/a": already checked */
    if (strcmp(list_node->out_fname, EMPTY_FILE))
        dec_inst[index].foutput.fname = list_node->out_fname;
    else
        dec_inst[index].foutput.fname = NULL;

    dec_inst[index].foutput.fptr = NULL;

    for (i = 0; i < list_node->ch; i++)
    {
        if (list_node->ref_fname[i] && strcmp(list_node->ref_fname[i], EMPTY_FILE))
            dec_inst[index].fref[i].fname = list_node->ref_fname[i];
        else
            dec_inst[index].fref[i].fname = NULL;

        dec_inst[index].fref[i].fptr = NULL;
    }

    return TRUE;
}

/*================================================================================================*/
/*===== nominal_functionality_test =====*/
/**
@brief  Testing decoder nominal functionality.

@param  None.

@return On success - return TPASS
        On failure - return the error code
*/
/*================================================================================================*/
int nominal_functionality_test()
{
    int i;
    flist_t *node;
    int ret = TPASS;
    aaclc_dec_inst *inst = &dec_inst[0];

    /* Check functionality for all entry read from list */

    ++th_count;
    for (node = files_list, i = 0; node; node = node->next, i++)
    {
        if (set_dec_instance(0, node) == FALSE) /* Set file names and instance ID */
        {
            ret = TFAIL;
            break;
        }
        int DoFilesExist(const char*, const char*);
        if (aaclc_decoder_engine(inst) != TPASS)
        {
            ret = TFAIL;
        }            
        else if (DoFilesExist((const char*)inst->fref[0].fname,(const char*)inst->foutput.fname))  /* perform bitmatch */
        {
            if (!perform_bitmatch_hex(&inst->foutput, inst->fref, node->ch))
            {   
                ret = TFAIL;
            }                
        }
    }
    return ret;
}

/*================================================================================================*/
/*===== preemption_test =====*/
/**
@brief  The decoder should function correctly in a preemptive environment. Test this ability.

@param  None.

@return On success - return TPASS
        On failure - return the error code
*/
/*================================================================================================*/
int preemption_test()
{
    preempt_test = TRUE;
    return reentrance_test();
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
int reentrance_test()
{
    int     i;
    flist_t *node;
    int     ret = TPASS;

    thread_test = 1;
    for (node = files_list, i = 0; node && (i < MAX_DEC_THREADS) && (ret == TPASS); node = node->next, i++)
    {
        if (set_dec_instance(i, node) == FALSE) /* Set file names and instance ID */
        {
            ret = TFAIL;
            break;
        }
        ++th_count;
        if (pthread_create(&dec_inst[i].tid, NULL, (void *)&run_decoder_thread, (void *)&dec_inst[i]))
        {
            tst_resm(TWARN, "ERROR: cannot create thread %d: %s", i + 1, strerror(errno));
            ret = TFAIL;
            break;
        }
    }

    if (ret == TPASS)
    {

        /*
         * Wait till threads are complete before main continues. Unless we
         * wait we run the risk of executing an exit which will terminate
         * the process and all threads before the threads have completed.
         */

        for (i = 0; i < th_count; i++)
            dec_inst[i].th_err = pthread_join(dec_inst[i].tid, NULL);

        for (i = 0; i < th_count; i++)
        {
            if (dec_inst[i].th_err)
            {
                tst_resm(TWARN, "Thread %2d was finished with error %s", i + 1, strerror(errno));
                ret = TFAIL;
            }
            else if (dec_inst[i].ltp_err != TPASS)
            {
                tst_resm(TWARN, "Thread %2d was finished with UNsuccessful result", i + 1);
                ret = dec_inst[i].ltp_err;
            }
        }
    }

    printf("\n");
    return ret;
}

/*================================================================================================*/
/*===== relocatability_test =====*/
/**
@brief  Test of decoder code relocatability.

@param  None.

@return On success - return TPASS
        On failure - return the error code
*/
/*================================================================================================*/
int relocatability_test()
{
    int i, j;
    flist_t *node;
    int ret = TPASS;
    aaclc_dec_inst *inst = &dec_inst[0];

    /* Check functionality for all entry read from list */

    ++th_count;
    for (node = files_list, i = 0; node; node = node->next, i++)
    {
        for( j = 0; j < test_iter; ++j )
        {    
        if (set_dec_instance(0, node) == FALSE) /* Set file names and instance ID */
        {
            ret = TFAIL;
            break;
        }
        if (aaclc_decoder_engine(inst) != TPASS)
            ret = TFAIL;
        else if (inst->fref[0].fname && inst->foutput.fname)  /* perform bitmatch */
        {
            if (!perform_bitmatch_hex(&inst->foutput, inst->fref, node->ch))
                ret = TFAIL;
        }
        tst_resm( TINFO, "Data memory was relocated. File No. %d, Iter No. %d\n", i, j );
        }
    }
    return ret;
}


/*================================================================================================*/
/*===== load_environment_test =====*/
/**
@brief  Test of ability to work in a loaded (even oberloaded) environment. [optional]

@param  None.

@return On success - return TPASS
        On failure - return the error code
*/
/*================================================================================================*/
int load_environment_test()
{
    int   ret = TFAIL;
    pid_t pid;

    /* TODO : insert codec_thread[0/1] configuration here */

    switch (pid = fork())
    {
	case -1:
	    tst_resm(TWARN, "load_environment_test: fork failed: %s\n", strerror(errno));
	    return ret;

	case 0:                         /* child -- finish straight away */
	    hogcpu();
            return 0;

	default:                        /* parent */
	    ret = nominal_functionality_test();

	    /* kill child process when decoding ends */
	    if (kill(pid, SIGKILL) != 0)
	    {
		tst_resm(TWARN, "load_environment_test: Kill(SIGKILL) error");
		ret = TFAIL;
	    }
    }

    return ret;
}

/*================================================================================================*/
/*===== adts_streams_test =====*/
/**
@brief  Test of ADTS streams resync feature.

@param  None.

@return On success - return TPASS
        On failure - return the error code
*/
/*================================================================================================*/
int adts_streams_test()
{
    int i;
    flist_t *node;
    int ret = TPASS;
    aaclc_dec_inst *inst = &dec_inst[0];

    /* Check functionality for all entry read from list */

    ++th_count;
    for (node = files_list, i = 0; node; node = node->next, i++)
    {
        if (set_dec_instance(0, node) == FALSE) /* Set file names and instance ID */
        {
            ret = TFAIL;
            break;
        }
        int DoFilesExist(const char*, const char*);
        if (aaclc_decoder_engine(inst) != TPASS)
        {
            ret = TFAIL;
        }            
        else if (DoFilesExist((const char*)inst->fref[0].fname,(const char*)inst->foutput.fname))  /* perform bitmatch */
        {
            if (!perform_bitmatch_hex(&inst->foutput, inst->fref, node->ch))
            {   
                ret = TFAIL;
            }                
        }
    }
    return ret;
}

/*================================================================================================*/
/*===== robustness_test =====*/
/**
@brief  Test of ability adequately react to a bad input bitstream.

@param  None.

@return On success - return TPASS
        On failure - return the error code
*/
/*================================================================================================*/
int robustness_test()
{
    int i;
    flist_t *node;
    int ret = TPASS;

    /* Check functionality for all entry read from list */

    ++th_count;
    for (node = files_list, i = 0; node && (ret == TPASS); node = node->next, i++)
    {
        if (set_dec_instance(0, node) == FALSE) /* Set file names and instance ID */
        {
            ret = TFAIL;
            break;
        }
        if ( (aaclc_decoder_engine(&dec_inst[0]) != TPASS) && (dec_inst[0].aacd_err != AACD_ERROR_INVALID_HEADER) )
            ret = TFAIL;
    }
    return ret;
}

/*================================================================================================*/
/*===== endurance_test =====*/
/**
@brief  Test of ability to work long time without crashes.

@param  None.

@return On success - return TPASS
        On failure - return the error code
*/
/*================================================================================================*/
int endurance_test()
{
    int i;
    int ret = TPASS;
    
    for (i = 0; (i < test_iter) && (ret == TPASS); i++)
    {
        tst_resm(TINFO, "Endurance iteration no. %d has been started", i + 1);
        if (nominal_functionality_test() != TPASS)
            ret = TFAIL;
    }
    
    return ret;
}

/*================================================================================================*/
/*===== mk_entry =====*/
/**
@brief  Makes flist_t entry from three strings, representing file names. There may be multiple
        reference files, because each channel is stored in separate file.

@param  Input:  inp_fname - pointer to the input file name
                out_fname - pointer to the output file name
                ref_fname - pointer to the reference file names, delimited by '+' sign
        Output: None

@return On success - return pointer to the created flist_t entry
        On failure - return NULL
*/
/*================================================================================================*/
flist_t *mk_entry(const char *inp_fname, const char *out_fname, const char *ref_fname)
{
    char    buf_ref[MAX_STR_LEN];
    char    *cur_ref;
    char    *next_ref;

    flist_t *list = calloc(1, sizeof(flist_t));
    
    if (list)
    {
	if ( (strlen(inp_fname) < MAX_STR_LEN) &&
	     (strlen(out_fname) < MAX_STR_LEN) &&
	     (strlen(ref_fname) < MAX_STR_LEN) )
	{
	    strcpy(list->inp_fname, inp_fname);
	    strcpy(list->out_fname, out_fname);
            strcpy(buf_ref, ref_fname);         /* because may be splitted, if necessary */
            cur_ref = buf_ref;
            list->ch = 0;

            /* If multiple file names presented, split string and extract them            */

            while ( (next_ref = strchr(cur_ref, CHNL_DELIM)) != NULL )
            {
                *next_ref = '\0';                               /* split from next        */
                strcpy(list->ref_fname[list->ch++], cur_ref);   /* copy current           */
                cur_ref = next_ref + 1;                         /* go to the next         */
                if (list->ch == Chans)                          /* too many ref. channels */
                    break;
            }
            if (list->ch < Chans)                               /* copy last string       */
                strcpy(list->ref_fname[list->ch++], cur_ref);
	    list->next = NULL;
	}
        else
            tst_resm(TWARN, "ERROR in mk_entry(): one of file names is too long");
    }
    else
        tst_resm(TWARN, "ERROR in mk_entry(): malloc() returns %s", strerror(errno));


    return list;
}

/*================================================================================================*/
/*===== delete_list =====*/
/**
@brief  Deletes linked list without recursion.

@param  Input:  list - pointer to the first flist_t entry (list head)
        Output: None

@return None
*/
/*================================================================================================*/
void delete_list(flist_t *list)
{
    flist_t *node = list;
    flist_t *next;

    while (node)
    {
        next = node->next;
        free(node);
        node = next;
    }
}

/*================================================================================================*/
/*===== read_cfg =====*/
/**
@brief  Reads list of entries (input, output & reference file names) from file and stores it
        in the linked list flist_t.

@param  Input:  filename  - pointer to the config (list) file name
        Output: pplist    - double pointer to the head of the list that will be created

@return On success - return TRUE
        On failure - return FALSE
*/
/*================================================================================================*/
int read_cfg(const char *filename, flist_t **pplist)
{
    FILE    *in;
    char    line[3][MAX_STR_LEN];
    flist_t *node;
    flist_t *flist = NULL;
    int     i      = 0;
    int     ret    = TRUE;

    in = fopen(filename, "r");
    if (in == NULL)
    {
        tst_resm(TWARN, "ERROR in read_cfg(): cannot open config file: %s",
                 strerror(errno));
        return FALSE;
    }

    /*
     * When ret becomes FALSE, it means that malloc() error was occured in mk_entry()
     * and it returned NULL
     */
     
    while (!feof(in) && (ret != FALSE) )
    {
        if (fscanf(in, "%s", line[i]) <= 0)
            continue;

        if (i == 2)
        {
            if (strcmp(line[0], EMPTY_FILE) != 0) /* No input file - nothing to be do  */
            {
                if (!flist)                       /* First entry will be created       */
                {
                    flist = mk_entry(line[0], line[1], line[2]);
                    node = flist;
                    if (!flist)
                        ret = FALSE;
                }
                else                              /* Next entries in linked list       */
                {
                    node->next = mk_entry(line[0], line[1], line[2]);
                    node = node->next;
                    if (!node)
                        ret = FALSE;
                }
            }
            else
                tst_resm(TWARN, "ERROR in read_cfg(): input file name is %s",
                         EMPTY_FILE);
        }       /* if (i == 2) */
        i++;
        i %= 3;
    }
    fclose(in);
    *pplist = flist;
    return ret;
}


/*================================================================================================*/
/*================================================================================================*/
int DoFilesExist( const char * fname1, const char * fname2 )
{
    FILE * fstream1 = fopen( fname1, "r" );
    if( fstream1 )
    {
        fclose( fstream1 );
        FILE * fstream2 = fopen( fname2, "r" );
        if( fstream2 )
        {
            fclose( fstream2 );
            return TRUE;
        }        
    }
    return FALSE;    
}


/*================================================================================================*/
/*================================================================================================*/
int DoBitmatch( char * fname1, char * fname2 )
{
    /* This code integrates dif32 compare tool. */

    char * argv[4] = 
    {
        "skip me",
        fname1,
        fname2,
        "15"    
    };
    
    /* Defined in the dif32.c */
    extern int bits_acc;
    extern int main_cmp( int argc, char *argv1[] );
    main_cmp( 4, argv );
    
    return bits_acc >= 15;
}

/*================================================================================================*/
/*===== perform_bitmatch_hex =====*/
/**
@brief 

@param  None
  
@return 
*/
/*================================================================================================*/
int perform_bitmatch_hex(filehandle *out, filehandle ref[], int refcnt)
{
    /* Check parameters and prepare environment */
    if (!refcnt)
        return FALSE;

    int i;
    for (i = 0; i < refcnt; i++)
    {
        if( !DoBitmatch( (char*)out->fname, (char*)ref[i].fname ) )
        {
            tst_resm( TFAIL, "Bitmatch failed (%s vs %s)", (char*)out->fname, (char*)ref[i].fname );
            return FALSE;                       
        }
        else
        {
            tst_resm( TINFO, "Bitmatch passed (%s vs %s)", (char*)out->fname, (char*)ref[i].fname );
        }                    
    }

    return TRUE;
}

/*================================================================================================*/
/*===== tables_init =====*/
/**
@brief  Initializes decoder tables. I believe this function will be removed from the release
        because this work must be done within the decoder code. Function body was copied from
        the test made by decoder developers.

@param  Input:  None
        Output: None

@return On success - return zero
        On failure - return the negative error code
*/
/*================================================================================================*/
void tables_init(void)
{
    int i;
    
    for (i = 0; i < MAX_DEC_THREADS; i++)
    {
        dec_inst[i].aptable[0] = (Lfract *)AACD_pow_4by3_table_lf;
        dec_inst[i].aptable[1] = (int *)AACD_pred_max_bands_tbl;
        dec_inst[i].aptable[2] = (unsigned short *)AACD_neg_mask;
        dec_inst[i].aptable[3] = (short *)AACD_sgn_mask;
        dec_inst[i].aptable[4] = (unsigned int *)AACD_hufftab_off;
        dec_inst[i].aptable[5] = (int (*)[])AACD_tns_max_bands_tbl;
        dec_inst[i].aptable[6] = (unsigned char **)AACD_huffstart;
        dec_inst[i].aptable[7] = (Lfract *(*)[])AACD_windowPtr;
        dec_inst[i].aptable[8] = (unsigned char *)AACD_HuffTable1;
        dec_inst[i].aptable[9] = (unsigned char *)AACD_HuffTable2;
        dec_inst[i].aptable[10] = (unsigned char *)AACD_HuffTable3;
        dec_inst[i].aptable[11] = (unsigned char *)AACD_HuffTable4;
        dec_inst[i].aptable[12] = (unsigned char *)AACD_HuffTable5;
        dec_inst[i].aptable[13] = (unsigned char *)AACD_HuffTable6;
        dec_inst[i].aptable[14] = (unsigned char *)AACD_HuffTable7;
        dec_inst[i].aptable[15] = (unsigned char *)AACD_HuffTable8;
        dec_inst[i].aptable[16] = (unsigned char *)AACD_HuffTable9;
        dec_inst[i].aptable[17] = (unsigned char *)AACD_HuffTable10;
        dec_inst[i].aptable[18] = (unsigned char *)AACD_HuffTableEsc;
        dec_inst[i].aptable[19] = (unsigned char *)AACD_HuffTableScl;
        dec_inst[i].aptable[20] = (SR_Info *)AACD_samp_rate_info;
        dec_inst[i].aptable[21] = (short *)AACD_sfb_96_1024;
        dec_inst[i].aptable[22] = (short *)AACD_sfb_96_128;
        dec_inst[i].aptable[23] = (short *)AACD_sfb_64_1024;
        dec_inst[i].aptable[24] = (short *)AACD_sfb_64_128;
        dec_inst[i].aptable[25] = (short *)AACD_sfb_48_1024;
        dec_inst[i].aptable[26] = (short *)AACD_sfb_48_128;
        dec_inst[i].aptable[27] = (short *)AACD_sfb_32_1024;
        dec_inst[i].aptable[28] = (short *)AACD_sfb_24_1024;
        dec_inst[i].aptable[29] = (short *)AACD_sfb_24_128;
        dec_inst[i].aptable[30] = (short *)AACD_sfb_16_1024;
        dec_inst[i].aptable[31] = (short *)AACD_sfb_16_128;
        dec_inst[i].aptable[32] = (short *)AACD_sfb_8_1024;
        dec_inst[i].aptable[33] = (short *)AACD_sfb_8_128;
        dec_inst[i].aptable[34] = (unsigned char *)AACD_n_lzeros_8bit;
        dec_inst[i].aptable[35] = (short *)AACD_bitrev_indices_256;
        dec_inst[i].aptable[36] = (short *)AACD_bitrev_indices_2048;
        dec_inst[i].aptable[37] = (short **)AACD_bitrev_indices;
        dec_inst[i].aptable[38] = (short *)AACD_num_bitrev_sets;
        dec_inst[i].aptable[39] = (Lfract *)AACD_fft_radix4_twidfac;
        dec_inst[i].aptable[40] = (Lfract *)AACD_prepost_twidfac;
        dec_inst[i].aptable[41] = (Lfract *)AACD_scale_fac_table_lf;
        dec_inst[i].aptable[42] = (Lfract *)AACD_k_tab_lf;
        dec_inst[i].aptable[43] = (Lfract *)AACD_window_fhg_long;
        dec_inst[i].aptable[44] = (Lfract *)AACD_window_fhg_short;
        dec_inst[i].aptable[45] = (Lfract *)AACD_window_dol_long;
        dec_inst[i].aptable[46] = (Lfract *)AACD_window_dol_short;
        dec_inst[i].aptable[47] = (Lfract(*)[])log2_coef;
        dec_inst[i].aptable[48] = (Lfract (*)[])pow2_coef;
        dec_inst[i].aptable[49] = (Lfract *)pow2_neg_coef;
        dec_inst[i].aptable[50] = (Lfract *)Coeff_CR_512;
        dec_inst[i].aptable[51] = (Lfract *)Coeff_CI_512;
        dec_inst[i].aptable[52] = (Lfract *)Coeff_CR_64;
        dec_inst[i].aptable[53] = (Lfract *)Coeff_CI_64;
        dec_inst[i].aptable[54] = (Lfract *)Coeff_PostTwid_CR_long;
        dec_inst[i].aptable[55] = (Lfract *)Coeff_PostTwid_CI_long;
        dec_inst[i].aptable[56] = (Lfract *)Coeff_PostTwid_CR_short;
        dec_inst[i].aptable[57] = (Lfract *)Coeff_PostTwid_CI_short;
    }
}

int parse_aac_header(FILE* fptr, AACD_Block_Params* params)
{
    AACD_INT32 val;
    int offset = 0;

    assert(fptr);
    assert(params);
    if ( !(read_hdr_val(fptr, &offset, sizeof(AACD_INT32) * LEN_BYTE, &val)) )
    {
        return 0;
    }        
    if (val == ADIF_ID)
    {
        if (get_adif_header(fptr, params))
            return IS_ADIF;
    }
    else
    {
    	if (get_adts_header(fptr, params))
            return IS_ADTS;
    }

    return 0;
}

int get_adif_header(FILE* fptr, AACD_Block_Params* params)
{
    unsigned int bitoff = 0;
    int          success;
    int          i;
    AACD_INT32   val;

    assert(params);
    bitoff += LEN_ADIF_ID * LEN_BYTE;
    if ( !(success = read_hdr_val(fptr, &bitoff, LEN_COPYRT_PRES, &val)) )
        return success;

    if (val)                             /* Copyright string is presented - skip it */
        bitoff += LEN_COPYRT_ID * LEN_BYTE;
    bitoff += LEN_ORIG + LEN_HOME;

    if ( !(success = read_hdr_val(fptr, &bitoff, LEN_BS_TYPE, &params->BitstreamType)) )
        return success;

    if ( !(success = read_hdr_val(fptr, &bitoff, LEN_BIT_RATE, &params->BitRate)) )
        return success;

    if ( !(success = read_hdr_val(fptr, &bitoff, LEN_NUM_PCE, &params->num_pce)) )
        return success;

    params->num_pce++;
    if ( !(params->pce = (AACD_ProgConfig*)malloc(params->num_pce * sizeof(AACD_ProgConfig))) )
    {
        tst_resm(TWARN, "ERROR in get_adif_header(): malloc for pce returns %s", strerror(errno));
        return FALSE;
    }

    for (i = 0; i < params->num_pce; ++i)
    {
        if (!params->BitstreamType)
        {
            success = read_hdr_val(fptr, &bitoff, LEN_ADIF_BF, &params->pce[i].buffer_fullness);
            if (!success)
                break;
        }
        else
            params->pce[i].buffer_fullness = 0;
    
        if ( !(success = get_pce_section(fptr, &params->pce[i], &bitoff)) )
            break;
    }

    if (bitoff % LEN_BYTE)
        bitoff += LEN_BYTE - bitoff % LEN_BYTE;
    fseek(fptr, bitoff / LEN_BYTE, SEEK_CUR);
    return success;
}

int get_adts_header(FILE* fptr, AACD_Block_Params* params)
{
    unsigned int bitoff = 0;
    int          success;
    AACD_INT32   val;
    AACD_INT32   id;
    AACD_INT32   adts_buf_fl;
#ifdef OLD_FORMAT_ADTS_HEADER
    int emphasis;
#endif

    assert(params);
    while (!FALSE)
    {
        if (bitoff / LEN_BYTE > ADTS_FRAME_MAX_SIZE)
            return FALSE;
        if ( !(success = read_hdr_val(fptr, &bitoff, LEN_SYNCWORD - LEN_BYTE, &val)) )
            return success;
        while (val != (1 << LEN_SYNCWORD) - 1)
        {
            AACD_INT32 n_val;
            if ( !(success = read_hdr_val(fptr, &bitoff, LEN_BYTE, &n_val)) )
                return success;
            val = (val << LEN_BYTE) | n_val;
            val &= (1 << LEN_SYNCWORD) - 1;
            if (bitoff / LEN_BYTE > ADTS_FRAME_MAX_SIZE)
                return FALSE;
        }

        if ( !(success = read_hdr_val(fptr, &bitoff, LEN_ID, &id)) )
            return success;

        if ( !(success = read_hdr_val(fptr, &bitoff, LEN_LAYER, &val)) )
            return success;
        if (val)
	    continue;

        if ( !(success = read_hdr_val(fptr, &bitoff, LEN_PROTECT_ABS, &params->ProtectionAbsent)) )
            return success;

        if ( !(success = read_hdr_val(fptr, &bitoff, LEN_PROFILE, &val)) )
            return success;
        if (val != 1)
	    continue;

        if ( !(success = read_hdr_val(fptr, &bitoff, LEN_SAMP_IDX, &params->SamplingFreqIndex)) )
            return success;
        if (params->SamplingFreqIndex > 0xC)
	    continue;

        bitoff += LEN_PRIVTE_BIT;
        if ( !(success = read_hdr_val(fptr, &bitoff, LEN_CHANNEL_CONF, &params->ChannelConfig)) )
            return success;
        bitoff += (LEN_ORIG + LEN_HOME);

#ifdef OLD_FORMAT_ADTS_HEADER
    	params->Flush_LEN_EMPHASIS_Bits = 0;
        if (!id)
        {
            if ( !(success = read_hdr_val(fptr, &bitoff, LEN_EMPHASIS, &emphasis)) )
                return success;
            params->Flush_LEN_EMPHASIS_Bits = 1;
        }
#endif

        bitoff += LEN_COPYRT_ID_ADTS + LEN_COPYRT_START + LEN_FRAME_LEN;
        if ( !(success = read_hdr_val(fptr, &bitoff, LEN_ADTS_BUF_FULLNESS, &adts_buf_fl)) )
            return success;
        params->BitstreamType = (adts_buf_fl == 0x7ff) ? 1 : 0;
        params->BufferFullness = (adts_buf_fl) << 5;

        bitoff += LEN_NUM_OF_RDB;
        if (!params->ProtectionAbsent)
        {
            if ( !(success = read_hdr_val(fptr, &bitoff, LEN_CRC, &params->CrcCheck)) )
                return success;
        }
        break;
    }

    params->num_pce = 0;
    params->BitRate = 0;

#ifndef OLD_FORMAT_ADTS_HEADER
    if (bitoff % LEN_BYTE)
        bitoff += LEN_BYTE - bitoff % LEN_BYTE;
#else
    if (id)
    {
        if (bitoff % LEN_BYTE)
            bitoff += LEN_BYTE - bitoff % LEN_BYTE;
    }
    else
    	bitoff -= LEN_EMPHASIS;
#endif
    fseek(fptr, bitoff / LEN_BYTE, SEEK_CUR);

    return success;
}

int get_pce_section(FILE* fptr, AACD_ProgConfig* pce, unsigned int* bitoffp)
{
    int success;
    int num_comment;

    if ( !(success = read_hdr_val(fptr, bitoffp, LEN_TAG, &pce->tag)) )
        return success;

    success = read_hdr_val(fptr, bitoffp, LEN_PROFILE, &pce->profile);
    if (!success || pce->profile != 1)
        return FALSE;

    if ( !(success = read_hdr_val(fptr, bitoffp, LEN_SAMP_IDX, &pce->sampling_rate_idx)) )
        return success;

    success = read_hdr_val(fptr, bitoffp, LEN_NUM_ELE, &pce->front.num_ele);
    if (!success || pce->front.num_ele > FChans)
        return FALSE;

    success = read_hdr_val(fptr, bitoffp, LEN_NUM_ELE, &pce->side.num_ele);
    if (!success || pce->side.num_ele > SChans)
        return FALSE;

    success = read_hdr_val(fptr, bitoffp, LEN_NUM_ELE, &pce->back.num_ele);
    if (!success || pce->back.num_ele > BChans)
        return FALSE;

    success = read_hdr_val(fptr, bitoffp, LEN_NUM_LFE, &pce->lfe.num_ele);
    if (!success || pce->lfe.num_ele > LChans)
        return FALSE;

    if ( !(success = read_hdr_val(fptr, bitoffp, LEN_NUM_DAT, &pce->data.num_ele)) )
        return success;

    success = read_hdr_val(fptr, bitoffp, LEN_NUM_CCE, &pce->coupling.num_ele);
    if (!success || pce->coupling.num_ele > CChans)
        return FALSE;

    if ( !(success = read_hdr_val(fptr, bitoffp, LEN_MIX_PRES, &pce->mono_mix.present)) )
        return success;
    if (pce->mono_mix.present == 1)
    {
        if ( !(success = read_hdr_val(fptr, bitoffp, LEN_TAG, &pce->mono_mix.ele_tag)) )
            return success;
    }

    if ( !(success = read_hdr_val(fptr, bitoffp, LEN_MIX_PRES, &pce->stereo_mix.present)) )
        return success;
    if (pce->stereo_mix.present == 1)
    {
        if ( !(success = read_hdr_val(fptr, bitoffp, LEN_TAG, &pce->stereo_mix.ele_tag)) )
            return success;
    }

    if ( !(success = read_hdr_val(fptr, bitoffp, LEN_MIX_PRES, &pce->matrix_mix.present)) )
        return success;
    if (pce->matrix_mix.present == 1)
    {
        if ( !(success = read_hdr_val(fptr, bitoffp, LEN_MMIX_IDX, &pce->matrix_mix.ele_tag)) )
            return success;
        if ( !(success = read_hdr_val(fptr, bitoffp, LEN_PSUR_ENAB, &pce->matrix_mix.ele_tag)) )
            return success;
    }

    if ( !(success = get_ele_section(fptr, &pce->front, 1, bitoffp)) )
        return success;
    if ( !(success = get_ele_section(fptr, &pce->side, 1, bitoffp)) )
        return success;
    if ( !(success = get_ele_section(fptr, &pce->back, 1, bitoffp)) )
        return success;
    if ( !(success = get_ele_section(fptr, &pce->lfe, 0, bitoffp)) )
        return success;
    if ( !(success = get_ele_section(fptr, &pce->data, 0, bitoffp)) )
        return success;
    if ( !(success = get_ele_section(fptr, &pce->coupling, 1, bitoffp)) )
        return success;

    if (*bitoffp % LEN_BYTE)
        *bitoffp += LEN_BYTE - *bitoffp % LEN_BYTE;
    if ( !(success = read_hdr_val(fptr, bitoffp, LEN_COMMENT_BYTES, &num_comment)) )
        return success;
    *bitoffp += num_comment * LEN_BYTE;

    return success;
}

int get_ele_section(FILE* fptr, AACD_EleList* list, int en_cpe, unsigned int* bitoffp)
{
    int success;
    int i;

    for (i = 0; i < list->num_ele; ++i)
    {
        if (en_cpe)
        {
            success = read_hdr_val(fptr, bitoffp, LEN_ELE_IS_CPE, &list->ele_is_cpe[i]);
            if (!success)
                break;
        }
        else
            list->ele_is_cpe[i] = 0;

        success = read_hdr_val(fptr, bitoffp, LEN_TAG, &list->ele_tag[i]);
        if (!success)
            break;
    }

    return success;
}

int read_hdr_val(FILE* fptr, unsigned int* beg, unsigned int cnt, AACD_INT32* val)
{
    unsigned char buffer[sizeof(AACD_INT32)];
    int bytes_to_skip;
    int bytes_to_rd;
    int begin = *beg;
    int end;
    int i;

    assert(fptr);
    assert(val);
    assert(cnt / LEN_BYTE <= sizeof(AACD_INT32));          /* Return value must fit the buffer */
    errno = 0;
    *val = 0;

    /* If begin points too far, seek to the corresponding byte and position begin accordingly  */

    bytes_to_skip = begin / LEN_BYTE;
    if (fseek(fptr, bytes_to_skip, SEEK_CUR) < 0)
    {
        tst_resm(TWARN, "ERROR in read_hdr_val(): cannot skip %d bytes of file: %s", bytes_to_skip,
                 strerror(errno));
        return FALSE;
    }
    begin %= LEN_BYTE;

    /* Compute number of bytes to be read and read them */

    end = begin + cnt;
    bytes_to_rd = (end - 1) / LEN_BYTE + 1;
    if (fread(buffer, 1, bytes_to_rd, fptr) < 0)
    {
        tst_resm(TWARN, "ERROR in parse_aac_header(): fread returns %s", strerror(errno));
        return FALSE;
    }

    buffer[0] &= 0xFF >> begin;          /* Cut bits that don't belong to needed positions       */
    for (i = 0; i < bytes_to_rd; ++i)  /* Convert string representation of our value to number */
        *val |= buffer[i] << (LEN_BYTE * (bytes_to_rd - i - 1));

    if (end % LEN_BYTE)                /* Shift to requested position                          */
        *val >>= LEN_BYTE - end % LEN_BYTE;

    if (fseek(fptr, -(bytes_to_rd + bytes_to_skip), SEEK_CUR) < 0) /* Rewind to the beginning of header */
    {
        tst_resm(TWARN, "ERROR in read_hdr_val(): cannot rewind to the beginning of hdr: %s", strerror(errno));
        return FALSE;
    }
    *beg += cnt;

    return TRUE;
}

#ifdef __cplusplus
}
#endif
