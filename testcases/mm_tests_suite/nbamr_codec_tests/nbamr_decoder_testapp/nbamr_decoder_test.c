/*/
  Copyright (C) 2004, Freescale Semiconductor, Inc. All Rights Reserved
  THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
  BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
  Freescale Semiconductor, Inc.
*/

/**
@file nbamr_decoder_test.c

@brief VTE C source of the NB AMR decoder test application.

Description of the file

@par Portability:
        Indicate if this module is portable to other compilers or platforms.
        If not, indicate specific reasons why is it not portable.
*/

/*======================== REVISION HISTORY ==================================

Author (core ID)           Date         CR Number    Description of Changes
-----------------------    ----------   ----------   -------------------------
I. Semenchukov/smng001c    24/01/2005   TLSbo46857   Initial version
I. Semenchukov/smng001c    07/02/2005   TLSbo47179   Changed include directives (lib headers)
I. Semenchukov/smng001c    28/02/2005   TLSbo47115   Changed printf() entries with tst_...()
D. Simakov/smkd001c        01/11/2005   TLSbo57009   Updated
D. Simakov/smkd001c        29/11/2005   TLSbo59534   Robustness test was fixed 
=============================================================================*/

#ifdef __cplusplus
extern "C"{
#endif

/*======================== INCLUDE FILES ====================================*/
/* Standard Include Files */

#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>    /* pthread_...() functions */
#include <math.h>       /* sqrt()                  */
#include <nb_amr_dec_api.h>

/* Harness Specific Include Files. */

#include "test.h"

/* Verification Test Environment Include Files */

#include "nbamr_decoder_test.h"

/*======================== LOCAL CONSTANTS ==================================*/
const char def_list_file[]     = "list/nbamrd_test_files";
const char rob_list_file[]     = "list/nbamrd_rob_test_files";
const char progress[]          = "-\\|/";      /* For rounding indicator */

/* This tables ware taken from library source code. It's a simplest way to find
 * size of encoded MMS/IF1/IF2 packets for each mode
 */

const NBAMR_S16 PackedCodedSizeMMS[]=
{
    13, 14, 16, 18, 20, 21, 27, 32,
    6,  0,  0,  0,  0,  0,  0,  1
};

const NBAMR_S16 PackedCodedSizeIF1[]=
{
    15, 16, 18, 20, 22, 23, 29, 34,
    8,  0,  0,  0,  0,  0,  0,  1
};

const NBAMR_S16 PackedCodedSizeIF2[]=
{
    13, 14, 16, 18, 19, 21, 26, 31,
    6,  0,  0,  0,  0,  0,  0,  1
};

/*======================== LOCAL MACROS =====================================*/
#define MAX_DEC_THREADS 4
#define RELOCATE_CYCLE  10
#define EMPTY_FILE      "n/a"
#define TBD             NULL /* It isn't defined in the dec. API header file */

/*======================== LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS) =======*/
typedef struct
{
    char *fname;
    FILE *fptr;
} filehandle;

/* Structure stores decoder instance/thread parameters, buffers etc.         */

typedef struct
{
    int                    id;
    filehandle             finput,
                           foutput,
                           fref;

    NBAMR_U8               bs_format;
    int                    inbuf_size;
    NBAMR_S16              *out_buf;
    NBAMR_S16              *in_buf;
    sAMRDDecoderConfigType *amrd_config;

    /* Some service variables */

    pthread_t              tid;     /* Th. ID. Used only if thread is created */
    int                    th_finish;
    int                    framecount;

    int                    th_err;  /* Error value returned by pthread_join() */
    int                    ltp_err; /* LTP error value returned by thread     */
    eAMRDReturnType        amrd_err; /* Decoder functions return values       */

} nbamr_dec_inst;

/*======================== LOCAL VARIABLES ==================================*/
static nbamr_dec_inst dec_inst[MAX_DEC_THREADS];

static flist_t *files_list   = NULL;
int            relocate_test = FALSE; /* A switch in the nbamr_decoder_engine()  */
int            thread_test   = 0;     /* 1 - multiple threads will be ran        */
int            th_count      = 0;     /* Used when some info must be printed     */
int            th_printing   = -1;    /* Helps th. to determine it is print. th. */
pthread_mutex_t io_mutex = PTHREAD_MUTEX_INITIALIZER;
int            gTestCase = -1;
/*======================== GLOBAL CONSTANTS =================================*/


/*======================== GLOBAL VARIABLES =================================*/


/*======================== LOCAL FUNCTION PROTOTYPES ========================*/
/* Please see comments to these functions at their implementation */

int  nbamr_decoder_engine(nbamr_dec_inst *inst);
int  alloc_dec_buffers(nbamr_dec_inst *instance);
int  realloc_dec_memory(nbamr_dec_inst *instance);
void dec_cleanup(nbamr_dec_inst *instance);
int  open_fstreams(nbamr_dec_inst *instance);
int  read_frame(NBAMR_S16 *inbuf, FILE *fptr, int bs_format);
int  write_frame(NBAMR_S16 *out_buf, FILE *fptr);
void print_status(void);
int  perform_bitmatch_raw(filehandle *out, filehandle *ref);
int  set_dec_instance(nbamr_dec_inst *inst, int index, flist_t *list_node);
void *run_decoder_thread(void *instance);

flist_t *mk_entry(const char *inp_fname, const char *format, const char *out_fname, const char *ref_fname);
void    delete_list(flist_t *list);
int     read_cfg(const char *filename, flist_t **pplist);

/* test functions */

int nominal_functionality_test();
int reentrance_test();
int relocatability_test();
int robustness_test();

/*======================== LOCAL FUNCTIONS ==================================*/

/*===== VT_nbamr_decoder_setup =====*/
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
int VT_nbamr_decoder_setup()
{
    return TPASS;
}

/*===== VT_nbamr_decoder_cleanup =====*/
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
int VT_nbamr_decoder_cleanup()
{
    return TPASS;
}

/*===== VT_nbamr_decoder_test =====*/
/**
@brief  Reads list of files (input, output and reference). Executes test specified by 'testcase'
        variable.

@param  Input:  testcase - testcase id of the test according to the test plan
                listfile - pointer to the name of list file
        Output: None

@return On success - return TPASS
        On failure - return the error code
*/
int VT_nbamr_decoder_test(int testcase, char *listfile)
{
    int rv = TFAIL;
    
    gTestCase = testcase;

    /*
     * Clear file list, open appropriate listfile (specified by input variable or default) and
     * and read its contents into the list. One test case needs its own file list.
     */

    if (files_list)
        delete_list(files_list);
    if (!listfile)
    {
        if (testcase == ROBUSTNESS)
            listfile = (char *)rob_list_file;
        else
            listfile = (char *)def_list_file;
    }

    if (!read_cfg(listfile, &files_list))
        return rv;

    tst_resm(TINFO, "List of files will be taken from %s", listfile);
    switch (testcase)
    {
	case NOMINAL_FUNCTIONALITY:
	    tst_resm(TINFO, "Nominal functionality test");
	    rv = nominal_functionality_test();
    	    break;

	case REENTRANCE:
	    tst_resm(TINFO, "Reentrance test");
	    rv = reentrance_test();
	    break;

	case RELOCATABILITY:
	    tst_resm(TINFO, "Relocatability test");
	    rv = relocatability_test();
	    break;

	case ROBUSTNESS:
	    tst_resm(TINFO, "Robustness test");
	    rv = robustness_test();
	    break;

	default:
	    tst_resm(TWARN,  "Wrong test case!!");
	    break;
    }

    return rv;
}

/*===== nbamr_decoder_engine =====*/
/**
@brief  Engine of the decoder. Performs decoding of bitstream.
	Also this function saves decoder result, if needed.
	This method is compatible with threads.

@param  Input:  inst - pointer to the structure holding buffers, decoder config structure etc.
        Output: None

@return On success - return TPASS
        On failure - return the error code
*/
int nbamr_decoder_engine(nbamr_dec_inst *inst)
{
    sAMRDDecoderConfigType *conf;        /* Pointer to simplify references */
    int ret = TPASS;

    if (!inst)                  /* Error: NULL pointer */
    {
        tst_resm(TWARN, "ERROR in nbamr_decoder_engine(): invalid parameter");
        return TFAIL;
    }

    /* Allocate memory for decoder parameter structure, populate known fields */

    inst->amrd_config = (sAMRDDecoderConfigType *)malloc(sizeof(sAMRDDecoderConfigType));
    if (!inst->amrd_config)
    {
        tst_resm(TWARN, "ERROR in nbamr_decoder_engine(): malloc() for decoder config returns %s",
                strerror(errno));
        ret = TFAIL;
    }

    if (ret == TPASS)
    {
        conf = inst->amrd_config;
        conf->pvAMRDDecodeInfoPtr = NULL;
        conf->pu8APPDInitializedDataStart = NBAMR_BEGIN_DATA;
        conf->u8RXFrameType = 0;
        conf->u8BitStreamFormat = inst->bs_format;
        conf->u8NumFrameToDecode = 1;
        if (conf->u8BitStreamFormat == NBAMR_ETSI)
            inst->inbuf_size = conf->u8NumFrameToDecode * SERIAL_FRAMESIZE;
        else
            inst->inbuf_size = conf->u8NumFrameToDecode *
                                (NBAMR_MAX_PACKED_SIZE / 2 + NBAMR_MAX_PACKED_SIZE % 2);

        /* Get decoder memory requirements. Info will be placed in the aacd_mem_info field */

        inst->amrd_err = eAMRDQueryMem(conf);
        if (inst->amrd_err != E_NBAMRD_OK)
        {
            tst_resm(TWARN, "ERROR in nbamr_decoder_engine(): eAMRDQueryMem() returns error %d",
                    inst->amrd_err);
            ret = TFAIL;
        }
    }
    
    if (ret == TPASS)
    {
        if (alloc_dec_buffers(inst) != TRUE)
            ret = TFAIL;
    }

    if (ret == TPASS)
    {
        if (open_fstreams(inst) != TRUE) 
            ret = TFAIL;
    }

    if (ret == TPASS)
    {
        inst->amrd_err = eAMRDDecodeInit(conf);
        if (inst->amrd_err != E_NBAMRD_OK)
        {
            tst_resm(TWARN, "ERROR in nbamr_decoder_engine(): eAMRDDecodeInit() returns error %d",
                    inst->amrd_err);
            ret = TFAIL;
        }
    }
    
    /* If an error was occured in one of the initialization steps, cleanup instance and exit */

    if (ret != TPASS)
    {
        dec_cleanup(inst);
        return ret;
    }

    if (th_printing == -1)
        th_printing = inst->id;

#ifdef DEBUG_TEST
    if (thread_test == 0)
        printf("Input file:  %s\n", inst->finput.fname);
#endif
        
    /* 
     * Main decoding cycle continues while end of input file was reached.
     * When a regular frame was decoded, output buffer contents are written into the
     * output file and next frame's turn begins
     */

    while( (ret == TPASS) && (read_frame(inst->in_buf, inst->finput.fptr, inst->bs_format) != FALSE) )
    {
        int i;

        for (i = 0; i < L_FRAME; i++) /* Seems that this cleanup is needed for proper work */
            inst->out_buf[i] = 0;            
        inst->amrd_err = eAMRDDecodeFrame(conf, inst->in_buf, inst->out_buf);
        inst->framecount++;

        if (inst->amrd_err == E_NBAMRD_OK)
        {
            if (th_printing == inst->id)
                print_status();
            
            /* If output file name is set in config list file, write to it */

            if (inst->foutput.fptr)
                if (write_frame(inst->out_buf, inst->foutput.fptr) == FALSE)
                    ret = TFAIL;

            /* In relocatability test realloc decoder memory every 'RELOCATE_CYCLE'-th frame*/

            if ( (!(inst->framecount % RELOCATE_CYCLE)) && relocate_test )
            {
                if (realloc_dec_memory(inst) != TRUE )
                    ret = TFAIL;
                else
                {
                    inst->amrd_err = eAMRDDecodeInit(conf);
                    if (inst->amrd_err != E_NBAMRD_OK)
                    {
                        tst_resm(TWARN, "ERROR in nbamr_decoder_engine(): eAMRDDecodeInit() "
                                "returns error %d", inst->amrd_err);
                        ret = TFAIL;
                    }
                }
            }
        }
        else
        {
            ret = TFAIL;            
            tst_resm(TWARN, "Invalid frame [%d]: %d", inst->framecount, inst->amrd_err);
            break;
        }            
    }
    if (th_count == 1) printf("\n");
    dec_cleanup(inst);
    return ret;
}

/*===== print_status =====*/
/**
@brief  Prints number of frames decoded for all running threads.

@param  Input:  None
        Output: None

@return None
*/
void print_status(void)
{
    int i;
    nbamr_dec_inst *inst;

    for (i = 0; i < th_count; i++)
    {
        inst = &dec_inst[i];
        printf("th[%d]-", inst->id + 1);
        if (inst->th_finish) printf("ended ");
        else printf("frames");
        printf("[%4d] ", inst->framecount);
    }
    if (inst)
        printf("%c\r", progress[inst->framecount % (sizeof(progress) - 1)]);
    fflush(stdout);
    return;
}

/*===== open_fstreams =====*/
/**
@brief  Opens streams associated with input and output files.

@param  Input:  instance - pointer to the structure holding buffers, decoder config structure etc.
        Output: None

@return On success - return TRUE
        On failure - return FALSE
*/
int open_fstreams(nbamr_dec_inst *instance)
{
    NBAMR_S8 s8MagicNumber[MAX_STR_LEN];

    if (!instance)  /* Error: NULL pointer */
    {
        tst_resm(TWARN, "ERROR in open_fstreams(): invalid parameter");
        return FALSE;
    }

    if ((instance->finput.fptr = fopen(instance->finput.fname, "r")) == NULL)
    {
        tst_resm(TWARN, "ERROR in open_fstreams(): fopen() for input file %s returns %s",
                instance->finput.fname, strerror(errno));
        return FALSE;
    }
    if (instance->amrd_config->u8BitStreamFormat == NBAMR_MMSIO)
    {
        fread(s8MagicNumber, sizeof(NBAMR_S8), strlen(NBAMR_MAGIC_NUMBER), instance->finput.fptr);
        if (strncmp((const char *)s8MagicNumber, NBAMR_MAGIC_NUMBER, strlen(NBAMR_MAGIC_NUMBER)))
        {
            tst_resm(TWARN, "ERROR in open_fstreams(): invalid magic number %s", s8MagicNumber);
            return FALSE;
        }
    }

    if (instance->foutput.fname != NULL)
    {
        if ((instance->foutput.fptr = fopen(instance->foutput.fname, "w")) == NULL)
        {
            tst_resm(TWARN, "ERROR in open_fstreams(): fopen() for output file %s returns %s",
                    instance->foutput.fname, strerror(errno));
            return FALSE;
        }
    }

    return TRUE;
}

/*===== alloc_dec_buffers =====*/
/**
@brief  Allocates memory for:
            all chunks requested by decoder (as returned by nbamrd_query_dec_mem());
            application input buffer.

@param  Input:  instance - pointer to the structure holding buffers, decoder config structure etc.
        Output: None

@return On success - return TRUE
        On failure - return FALSE
*/
int alloc_dec_buffers(nbamr_dec_inst *instance)
{
    int chunk_cnt;
    int i;
    sAMRDDecoderConfigType   *conf;
    sAMRDMemAllocInfoSubType *mem;
    int ret = TRUE;

    if (!instance)
    {
        tst_resm(TWARN, "ERROR in alloc_dec_buffers(): invalid parameter");
        return FALSE;
    }
    conf = instance->amrd_config;
    
    /* Allocate memory for all required chunks and buffers */

    chunk_cnt = conf->sAMRDMemInfo.s32AMRDNumMemReqs;
    for (i = 0; i < chunk_cnt; i++)
    {
        mem = &(conf->sAMRDMemInfo.asMemInfoSub[i]);
        mem->pvAPPDBasePtr = malloc(mem->s32AMRDSize);
        if (!mem->pvAPPDBasePtr)
        {
            tst_resm(TWARN, "ERROR in alloc_dec_buffers(): malloc() for chunk %d returns %s",
                    i, strerror(errno));
            ret = FALSE;
        }
    }

    if (ret)
    {
        instance->in_buf = (NBAMR_S16*)malloc(instance->inbuf_size * sizeof(NBAMR_S16));
        if (!instance->in_buf)
        {
            tst_resm(TWARN, "ERROR in alloc_dec_buffers(): malloc() for input buffer returns %s",
                    strerror(errno));
            ret = FALSE;
        }
    }
    if (ret)
    {
        instance->out_buf = (NBAMR_S16*)malloc(L_FRAME * sizeof(NBAMR_S16));
        if (!instance->out_buf)
        {
            tst_resm(TWARN, "ERROR in alloc_dec_buffers(): malloc() for output buffer returns %s",
                    strerror(errno));
            ret = FALSE;
        }
    }

    return ret;
}

/*===== dec_cleanup =====*/
/**
@brief  Releases file streams allocated by open_fstreams().
        Frees memory allocated by alloc_dec_buffers().

@param  Input:  instance - pointer to the structure holding buffers, decoder config structure etc.
        Output: None

@return None
*/
void dec_cleanup(nbamr_dec_inst *instance)
{
    int i;
    int nr;
    sAMRDDecoderConfigType   *conf;
    sAMRDMemAllocInfoSubType *mem;
    
    if (!instance)
    {
        tst_resm(TWARN, "ERROR in dec_cleanup(): invalid parameter");
        return;
    }

    if (instance->finput.fptr)
        fclose(instance->finput.fptr);
    if (instance->foutput.fptr)
        fclose(instance->foutput.fptr);

    conf = instance->amrd_config;
    if(instance->in_buf)
        free(instance->in_buf);
    if(instance->out_buf)
        free(instance->out_buf);
        
    nr = conf->sAMRDMemInfo.s32AMRDNumMemReqs;
    for (i = 0; i < nr; i++)
    {
        mem = &(conf->sAMRDMemInfo.asMemInfoSub[i]);
        if (mem->pvAPPDBasePtr)
            free(mem->pvAPPDBasePtr);
    }

    if (conf)
        free(conf);

    return;
}

/*===== realloc_dec_memory =====*/
/**
@brief  Frees decoder memory and allocates it again, but in other place.

@param  Input:  instance - pointer to the structure holding buffers, decoder config structure etc.
        Output: None

@return On success - return TRUE
        On failure - return FALSE
*/
int realloc_dec_memory(nbamr_dec_inst *instance)
{
    sAMRDDecoderConfigType   *conf;
    sAMRDMemAllocInfoSubType *mem;
    int  i;
    int  nr;
    void *barrier_ptr;
    int  ret = TRUE;
    
    if (!instance)
    {
        tst_resm(TWARN, "ERROR in realloc_dec_memory(): invalid parameter");
        return FALSE;
    }
    conf = instance->amrd_config;

    /* Deallocate all memory chunk and then again allocate them */

    nr = conf->sAMRDMemInfo.s32AMRDNumMemReqs;
    for (i = 0; i < nr; i++)
    {
        mem = &(conf->sAMRDMemInfo.asMemInfoSub[i]);
        if (mem->pvAPPDBasePtr)
            free(mem->pvAPPDBasePtr);
    }

    /* Allocate some memory to be sure that decoder memory will be allocated in other place */

    barrier_ptr = malloc(SERIAL_FRAMESIZE * instance->framecount);
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
            mem = &(conf->sAMRDMemInfo.asMemInfoSub[i]);
            mem->pvAPPDBasePtr = malloc(mem->s32AMRDSize);
            if (!mem->pvAPPDBasePtr)
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

/*===== read_frame =====*/
/**
@brief  Reads next available frame to the input buffer.

@param  Input:  inbuf    - decoder input buffer
                fptr     - input file stream pointer
        Output: None

@return On success - return TRUE
        On failure - return FALSE
*/
int read_frame(NBAMR_S16 *inbuf, FILE *fptr, int bs_format)
{
    int       ret        = FALSE;
    NBAMR_U8  *inbuf_ptr = (NBAMR_U8*)inbuf;
    NBAMR_S16 framesize  = SERIAL_FRAMESIZE;
    size_t    itemsize   = sizeof(NBAMR_S16);

    if ( (!inbuf) || (!fptr) )
    {
        tst_resm(TWARN, "ERROR in read_frame(): invalid argument");
        return ret;
    }

    if (bs_format != NBAMR_ETSI)
    {
        itemsize = sizeof(NBAMR_U8);
        if (fread(inbuf_ptr, sizeof(NBAMR_U8), 1, fptr) != 1)
            return ret;
        switch (bs_format)
        {
            case NBAMR_MMSIO:
                framesize = PackedCodedSizeMMS[(NBAMR_MAX_NUM_MODES - 1) & (*inbuf_ptr >> 3)] - 1;
                break;

            case NBAMR_IF1IO:
                framesize = PackedCodedSizeIF1[(NBAMR_MAX_NUM_MODES - 1) & (*inbuf_ptr >> 4)] - 1;
                break;

            case NBAMR_IF2IO:
                framesize = PackedCodedSizeIF2[(NBAMR_MAX_NUM_MODES - 1) & *inbuf_ptr] - 1;
                break;

            default:
                tst_resm(TWARN, "Invalid bitstream format");
                return ret;
        }
        ++inbuf_ptr;
        if ( fread(inbuf_ptr, itemsize, framesize, fptr) == framesize) 
            ret = TRUE;
    }
    else if ( fread(inbuf, itemsize, framesize, fptr) == framesize) 
        ret = TRUE;
        
    fflush( stdout );

    return ret;
}

/*===== write_frame =====*/
/**
@brief  Writes frame into the output stream.

@param  Input:  outbuf   - decoder output buffer
                fptr     - output file stream pointer
        Output: None

@return On success - return TRUE
        On failure - return FALSE
*/
int write_frame(NBAMR_S16 *out_buf, FILE *fptr)
{
    int ret = FALSE;

    if (!fptr)
    {
        tst_resm(TWARN, "ERROR in write_frame(): invalid argument");
        return ret;
    }

    fwrite(out_buf, sizeof(NBAMR_S16), L_FRAME, fptr);
    
    if (ferror(fptr))
    {
        tst_resm(TWARN, "ERROR in write_frame(): fwrite() returns error %s", strerror(errno));
    }
    else
        ret = TRUE;

    return ret;
}

/*===== run_decoder_thread =====*/
/**
@brief  This is a thread function. It changes process priority in case of preemption test and
        runs decoder engine.

@param  instance - void pointer to thread arguments.

@return NULL
*/
void *run_decoder_thread(void *instance)
{
    int i;
    nbamr_dec_inst *inst = (nbamr_dec_inst *)instance;

    inst->ltp_err = nbamr_decoder_engine(inst);
    inst->th_finish = TRUE;

    /*
     * If it was a printing thread, find another working thread and aasign its id to
     * th_printing variable
     */

    if (th_printing == inst->id)
    {
        th_printing = -1;
        for (i = 0; i < th_count; i++)
        {
            if (dec_inst[i].th_finish == FALSE)
            {
                th_printing = dec_inst[i].id;
                break;
            }
        }
    }

    /* perform bitmatch */

    if ( (inst->fref.fname && inst->foutput.fname) && (inst->ltp_err == TPASS) )
    {
        if (!perform_bitmatch_raw(&inst->foutput, &inst->fref))
        {
            inst->ltp_err = TFAIL;
            pthread_mutex_lock( &io_mutex );
            tst_resm( TFAIL, "Bitmatch failed (%s vs %s)", inst->foutput.fname, inst->fref.fname );
            pthread_mutex_unlock( &io_mutex );
        }            
        else
        {
            pthread_mutex_lock( &io_mutex );
            tst_resm( TINFO, "Bitmatch passed (%s vs %s)", inst->foutput.fname, inst->fref.fname );
            pthread_mutex_unlock( &io_mutex );
        }
    }        

    return NULL;
}

/*===== set_dec_instance =====*/
/**
@brief  Sets instance ID and file names.

@param  inst      - pointer to the structure containing decoder info
        list_node - pointer to the current flist_t entry (that stores file names)

@return On success - return TRUE
        On failure - return FALSE
*/
int set_dec_instance(nbamr_dec_inst *inst, int index, flist_t *list_node)
{
    if (!inst || !list_node)
    {
        tst_resm(TWARN, "ERROR in set_dec_instance(): one of parameters isn't valid");
        return FALSE;
    }

    inst->framecount = 0;
    inst->id = index;
    inst->bs_format = list_node->bitstr_format;
    inst->finput.fname = list_node->inp_fname; /* fname isn't "n/a": already checked */
    if (strcmp(list_node->out_fname, EMPTY_FILE))
        inst->foutput.fname = list_node->out_fname;
    else
        inst->foutput.fname = NULL;

    inst->foutput.fptr = NULL;

    if (strcmp(list_node->ref_fname, EMPTY_FILE))
        inst->fref.fname = list_node->ref_fname;
    else
        inst->fref.fname = NULL;

    inst->fref.fptr = NULL;

    return TRUE;
}

/*===== nominal_functionality_test =====*/
/**
@brief  Testing decoder nominal functionality.

@param  None.

@return On success - return TPASS
        On failure - return the error code
*/
int nominal_functionality_test()
{
    int i;
    flist_t *node;
    int ret = TPASS;
    nbamr_dec_inst *inst = &dec_inst[0];

    /* Check functionality for all entry read from list */

    ++th_count;
    for (node = files_list, i = 0; node && (ret == TPASS); node = node->next, i++)
    {
        if (set_dec_instance(&dec_inst[0], 0, node) == FALSE) /* Set file names and instance ID */
        {
            ret = TFAIL;
            break;
        }
        if (nbamr_decoder_engine(inst) != TPASS)
            ret = TFAIL;
        else if ( inst->fref.fname && inst->foutput.fname )
        {
            if (!perform_bitmatch_raw(&inst->foutput, &inst->fref))
            {
                inst->ltp_err = TFAIL;
                pthread_mutex_lock( &io_mutex );
                tst_resm( TFAIL, "Bitmatch failed (%s vs %s)", inst->foutput.fname, inst->fref.fname );
                pthread_mutex_unlock( &io_mutex );
            }            
            else
            {
                pthread_mutex_lock( &io_mutex );
                tst_resm( TINFO, "Bitmatch passed (%s vs %s)", inst->foutput.fname, inst->fref.fname );
                pthread_mutex_unlock( &io_mutex );
            }
        }        

    }
    return ret;
}

/*===== reentrance_test =====*/
/**
@brief  Reentrance means there should not be any static data or any global 
	variables used in the code. Test this ability.

@param  None.

@return On success - return TPASS
        On failure - return the error code
*/
int reentrance_test()
{
    int     i;
    flist_t *node;
    int     ret = TPASS;

    thread_test = 1;
    for (node = files_list, i = 0; node && (i < MAX_DEC_THREADS) && (ret == TPASS); node = node->next, i++)
    {
        if (set_dec_instance(&dec_inst[i], i, node) == FALSE) /* Set file names and instance ID */
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

/*===== relocatability_test  =====*/
/**
@brief  Test of decoder code relocatability.

@param  None.

@return On success - return TPASS
        On failure - return the error code
*/
int relocatability_test()
{
    int i, j;
    flist_t *node;
    int ret = TPASS;
    nbamr_dec_inst *inst = &dec_inst[0];

    /* Check functionality for all entry read from list */

    
    ++th_count;
    for (node = files_list, i = 0; node && (ret == TPASS); node = node->next, i++)
    {
        for (j = 0; j < 10; ++j )
        {
            if (set_dec_instance(&dec_inst[0], 0, node) == FALSE) /* Set file names and instance ID */
            {
                ret = TFAIL;
                break;
            }
            if (nbamr_decoder_engine(inst) != TPASS)
                ret = TFAIL;
            else if ( inst->fref.fname && inst->foutput.fname )
            {
                if (!perform_bitmatch_raw(&inst->foutput, &inst->fref))
                {
                    inst->ltp_err = TFAIL;
                    pthread_mutex_lock( &io_mutex );
                    tst_resm( TFAIL, "Bitmatch failed (%s vs %s)", inst->foutput.fname, inst->fref.fname );
                    pthread_mutex_unlock( &io_mutex );
                }            
                else
                {
                    pthread_mutex_lock( &io_mutex );
                    tst_resm( TINFO, "Bitmatch passed (%s vs %s)", inst->foutput.fname, inst->fref.fname );
                    pthread_mutex_unlock( &io_mutex );
                }
            }        
            tst_resm( TINFO, "Data memory was relocated" );
        }        
    }        
    return ret;
}

/*===== robustness_test  =====*/
/**
@brief  Test of ability adequately react to a bad input bitstream.

@param  None.

@return On success - return TPASS
        On failure - return the error code
*/
int robustness_test()
{
    int i;
    flist_t *node;
    int ret = TPASS;   

    /* Check functionality for all entry read from list */

    ++th_count;
    for (node = files_list, i = 0; node && (ret == TPASS); node = node->next, i++)
    {
        if (set_dec_instance(&dec_inst[0], 0, node) == FALSE) /* Set file names and instance ID */
        {
            ret = TFAIL;
            break;
        }                    
        
        int rv = nbamr_decoder_engine(&dec_inst[0]);
        
        if( rv == TPASS || dec_inst[0].amrd_err == E_NBAMRD_OK )
        {
            tst_resm( TFAIL, "Robustness to %s failed", dec_inst[0].finput.fname );
            ret = TFAIL;
        }
        else
        {            
            tst_resm( TINFO, "Robustness to %s passed", dec_inst[0].finput.fname );
        }        
    }        
    return ret;
}

/*===== mk_entry =====*/
/**
@brief  Makes flist_t entry from three strings, representing file names. There may be multiple
        reference files, because each channel is stored in separate file.

@param  Input:  inp_fname - pointer to the input file name
                format    - identifies input format
                out_fname - pointer to the output file name
                ref_fname - pointer to the reference file names, delimited by '+' sign
        Output: None

@return On success - return pointer to the created flist_t entry
        On failure - return NULL
*/
flist_t *mk_entry(const char *inp_fname, const char *format, const char *out_fname, const char *ref_fname)
{
    flist_t *list = malloc(sizeof(flist_t));
    
    if (list)
    {
	if ( (strlen(inp_fname) < MAX_STR_LEN) &&
	     (strlen(out_fname) < MAX_STR_LEN) &&
	     (strlen(ref_fname) < MAX_STR_LEN) &&
             (strlen(format)< MAX_STR_LEN) )
	{
	    strcpy(list->inp_fname, inp_fname);
	    strcpy(list->out_fname, out_fname);
	    strcpy(list->ref_fname, ref_fname);
            list->bitstr_format = atoi(format);
	}
        else
            tst_resm(TWARN, "ERROR in mk_entry(): one of file names too long");
    }
    else
        tst_resm(TWARN, "ERROR in mk_entry(): malloc() returns %s", strerror(errno));

    return list;
}

/*===== delete_list =====*/
/**
@brief  Deletes linked list without recursion.

@param  Input:  list - pointer to the first flist_t entry (list head)
        Output: None

@return None
*/
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

/*===== read_cfg =====*/
/**
@brief  Reads list of entries (input, output & reference file names) from file and stores it
        in the linked list flist_t.

@param  Input:  filename  - pointer to the config (list) file name
        Output: pplist    - double pointer to the head of the list that will be created

@return On success - return TRUE
        On failure - return FALSE
*/
int read_cfg(const char *filename, flist_t **pplist)
{
    FILE    *in;
    char    line[4][MAX_STR_LEN];
    flist_t *node;
    flist_t *flist = NULL;
    int     i      = 0;
    int     ret    = TRUE;

    in = fopen(filename, "r");
    if (in == NULL)
    {
        tst_resm(TWARN, "ERROR in read_cfg(): cannot open config file %s: %s", filename, strerror(errno));
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

        if (i == 3)
        {
            if (strcmp(line[0], EMPTY_FILE) != 0) /* No input file - nothing to be do  */
            {
                if (!flist)                       /* First entry will be created       */
                {
                    flist = mk_entry(line[0], line[1], line[2], line[3]);
                    node = flist;
                    if (!flist)
                        ret = FALSE;
                }
                else                              /* Next entries in linked list       */
                {
                    node->next = mk_entry(line[0], line[1], line[2], line[3]);
                    node = node->next;
                    if (!node)
                        ret = FALSE;
                }
            }
            else
                tst_resm(TWARN, "ERROR in read_cfg(): input file name is %s", EMPTY_FILE);
        }       /* if (i == 3) */
        i++;
        i %= 4;
    }

    *pplist = flist;
    return ret;
}

/*===== perform_bitmatch_raw =====*/
/**
@brief 

@param  None
  
@return 
*/
int perform_bitmatch_raw(filehandle *out, filehandle *ref)
{
    unsigned int outval,
                 refval;
    int out_eof = 0,
        ref_eof = 0;
    int ret = TRUE;

    /* Check parameters and prepare environment */

    if (out->fname && ref->fname)
    {
        if ( (out->fptr = fopen(out->fname, "r")) == NULL )
        {
            tst_resm(TWARN, "ERROR for %s: fopen() returns %s", out->fname, strerror(errno));
            ret = FALSE;
        }
        else if ( (ref->fptr = fopen(ref->fname, "r")) == NULL )
        {
            tst_resm(TWARN, "ERROR for %s: fopen() returns %s", ref->fname, strerror(errno));
            ret = FALSE;
        }
    }
    else
        ret = FALSE;

    if (ret)
    {

        /* Begin bitmatch */

        while (ret && !out_eof)
        {
            fread(&outval, 1, sizeof(outval), out->fptr);
            fread(&refval, 1, sizeof(refval), ref->fptr);
            if (outval != refval)
            {
                tst_resm(TWARN, "%s and %s, offset %ld: values mismatch (%X - %X)",
                         out->fname, ref->fname, ftell(out->fptr), outval, refval);
                ret = FALSE;
            }

            out_eof = feof(out->fptr) ? 1 : 0;
            ref_eof = feof(ref->fptr) ? 1 : 0;
            if (out_eof != ref_eof)
            {
                tst_resm(TWARN, "%s and %s: files length mismatch", out->fname, ref->fname);
                ret = FALSE;
            }
        }

        if (!ret)
            tst_resm(TWARN, "Bitmatch for output file %s is failed", out->fname);
    }

    if (out->fptr)
        fclose(out->fptr);
    if (ref->fptr)
        fclose(ref->fptr);

    return ret;
}

#ifdef __cplusplus
}
#endif
