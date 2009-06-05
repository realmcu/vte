/*/
  Copyright (C) 2004, Freescale Semiconductor, Inc. All Rights Reserved
  THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
  BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
  Freescale Semiconductor, Inc.
*/

/**
@file nbamr_encoder_test.c

@brief VTE C source of the NB AMR encoder test application.

Description of the file

@par Portability:
        Indicate if this module is portable to other compilers or platforms.
        If not, indicate specific reasons why is it not portable.
*/

/*======================== REVISION HISTORY ==================================

Author (core ID)           Date         CR Number    Description of Changes
-----------------------    ----------   ----------   -------------------------
I. Semenchukov/smng001c    26/01/2005   TLSbo46857   Initial version
I. Semenchukov/smng001c    07/02/2005   TLSbo47179   Changed include directives (lib headers)
I. Semenchukov/smng001c    28/02/2005   TLSbo47115   Changed printf() entries with tst_...()
I. Semenchukov/smng001c    23/03/2005   TLSbo48795   Made changes to reflect with new version
D. Simakov/smkd001c        03/11/2005   TLSbo57009   Update
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
#include <nb_amr_enc_api.h>

/* Harness Specific Include Files. */

#include "test.h"

/* Verification Test Environment Include Files */

#include "nbamr_encoder_test.h"

/*======================== LOCAL CONSTANTS ==================================*/

const char def_list_file[] = "list/nbamre_test_files";
const char progress[]      = "-\\|/";    /* For rounding indicator */

/*======================== LOCAL MACROS =====================================*/
#define MAX_ENC_THREADS 4
#define RELOCATE_CYCLE  10
#define EMPTY_FILE      "n/a"
#define TBD              NULL /* It isn't defined in the enc. API hdr file */

/*======================== LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS) =======*/
typedef struct
{
    char *fname;
    FILE *fptr;
} filehandle;

/* Structure stores encoder instance/thread parameters, buffers etc. */

typedef struct
{
    int                    id;
    filehandle             finput,
                           foutput,
                           fref;

    NBAMR_S8               *bitrate;
    NBAMR_U8               bs_format;
    NBAMR_S16              dtx_enabled;
    NBAMR_U32              packed_size;
    NBAMR_U8               output_size;

    int                    outbuf_size;
    NBAMR_S16              *out_buf;
    NBAMR_S16              *in_buf;
    sAMREEncoderConfigType *amre_config;

    /* Some service variables */

    pthread_t              tid;     /* Th. ID. Used only if thread is created */
    int                    th_finish;
    int                    framecount;

    int                    th_err;  /* Error value returned by pthread_join() */
    int                    ltp_err; /* LTP error value returned by thread     */
    eAMREReturnType        amre_err; /* Encoder functions return values       */

} nbamr_enc_inst;

/*======================== LOCAL VARIABLES ==================================*/
static nbamr_enc_inst enc_inst[MAX_ENC_THREADS];

static flist_t *files_list   = NULL;
int            relocate_test = FALSE; /* A switch in the nbamr_encoder_engine()  */
int            thread_test   = 0;     /* 1 - multiple threads will be ran        */
int            th_count      = 0;     /* Used when some info must be printed     */
int            th_printing   = -1;    /* Helps th. to determine it is print. th. */
pthread_mutex_t io_mutex = PTHREAD_MUTEX_INITIALIZER;

/*======================== GLOBAL CONSTANTS =================================*/


/*======================== GLOBAL VARIABLES =================================*/


/*======================== LOCAL FUNCTION PROTOTYPES ========================*/
/* Please see comments to these functions at their implementation */

int  nbamr_encoder_engine(nbamr_enc_inst *inst);
int  alloc_enc_buffers(nbamr_enc_inst *instance);
int  realloc_enc_memory(nbamr_enc_inst *instance);
void enc_cleanup(nbamr_enc_inst *instance);
int  open_fstreams(nbamr_enc_inst *instance);
int  write_frame(NBAMR_S16 *out_buf, int framesize, int itemsize, FILE *fptr);
void print_status(void);
int  perform_bitmatch_raw(filehandle *out, filehandle *ref);
int  set_enc_instance(int index, flist_t *list_node);
void *run_encoder_thread(void *instance);

flist_t *mk_entry(const char *inp_fname, const char *mode, const char *dtx_flag,
                  const char *format, const char *out_fname, const char *ref_fname);
void    delete_list(flist_t *list);
int     read_cfg(const char *filename, flist_t **pplist);

/* test functions */

int nominal_functionality_test();
int reentrance_test();
int relocatability_test();

/*======================== LOCAL FUNCTIONS ==================================*/

/*===== VT_nbamr_encoder_setup =====*/
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
int VT_nbamr_encoder_setup()
{
    return TPASS;
}

/*===== VT_nbamr_encoder_cleanup =====*/
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
int VT_nbamr_encoder_cleanup()
{
    return TPASS;
}

/*===== VT_nbamr_encoder_test =====*/
/**
@brief  Reads list of files (input, output and reference). Executes test specified by 'testcase'
        variable.

@param  Input:  testcase - Testcase id of the test according to the test plan
                listfile - pointer to the name of list file
        Output: None
        

@return On success - return TPASS
        On failure - return the error code
*/
int VT_nbamr_encoder_test(int testcase, char *listfile)
{
    int rv = TFAIL;

    /*
     * Clear file list, open appropriate listfile (specified by input variable or default) and
     * and read its contents into the list.
     */

    if (files_list)
        delete_list(files_list);
    if (!listfile)
        listfile = (char *)def_list_file;

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

	default:
	    tst_resm(TWARN, "Wrong test case!!");
	    break;
    }

    return rv;
}

/*===== nbamr_encoder_engine =====*/
/**
@brief  Engine of the encoder. The encoding of a bitstream should be presented here.
	Also this function processes encoder result, i.e. displays it for a video data case
	or plays it for a sound data case.
	This method should be compatible with threads.

@param  Input:  inst - pointer to the structure holding buffers, encoder config structure etc.
        Output: None

@return On success - return TPASS
        On failure - return the error code
*/
int nbamr_encoder_engine(nbamr_enc_inst *inst)
{
    sAMREEncoderConfigType *conf; /* Pointer to simplify references */
    NBAMR_S8 s8AMREUsedModeStr;
    NBAMR_S8* ps8AMREUsedModeStr = &s8AMREUsedModeStr;
    int  ret = TPASS;

    if (!inst)                   /* Error: NULL pointer */
    {
        tst_resm(TWARN, "ERROR in nbamr_encoder_engine(): invalid parameter");
        return TFAIL;
    }

    /* Allocate memory for encoder parameter structure, populate known fields */

    inst->amre_config = (sAMREEncoderConfigType *)malloc(sizeof(sAMREEncoderConfigType));
    if (!inst->amre_config)
    {
        tst_resm(TWARN, "ERROR in nbamr_encoder_engine(): malloc() for enc. config returns %s",
                 strerror(errno));
        ret = TFAIL;
    }

    /* Initialize encoder config structure */

    if (ret == TPASS)
    {
        conf = inst->amre_config;
        conf->pvAMREEncodeInfoPtr = NULL;
        conf->pu8APPEInitializedDataStart = NBAMR_BEGIN_DATA;
        conf->s16APPEDtxFlag = inst->dtx_enabled;
        conf->u8BitStreamFormat = inst->bs_format;
        conf->u8NumFrameToEncode = 1;
        conf->pu32AMREPackedSize = &(inst->packed_size);
        conf->pps8APPEModeStr = &(inst->bitrate);
        conf->pps8AMREUsedModeStr = &ps8AMREUsedModeStr;
        if (conf->u8BitStreamFormat == NBAMR_ETSI)
            inst->outbuf_size = conf->u8NumFrameToEncode * SERIAL_FRAMESIZE;
        else
            inst->outbuf_size = conf->u8NumFrameToEncode *
                                (NBAMR_MAX_PACKED_SIZE / 2 + NBAMR_MAX_PACKED_SIZE % 2);

        /* Get encoder memory requirements. Info will be placed in the sAMREMemInfo field */

        inst->amre_err = eAMREQueryMem(conf);
        if (inst->amre_err != E_NBAMRE_OK)
        {
            tst_resm(TWARN, "ERROR in nbamr_encoder_engine(): eAMREQueryMem() returns error %d",
                     inst->amre_err);
            ret = TFAIL;
        }
    }

    if (ret == TPASS)
    {
        if (alloc_enc_buffers(inst) != TRUE)
            ret = TFAIL;
    }

    if (ret == TPASS)
    {
        if (open_fstreams(inst) != TRUE) 
            ret = TFAIL;
    }

    if (ret == TPASS)
    {
        inst->amre_err = eAMREEncodeInit(conf);
        if (inst->amre_err != E_NBAMRE_OK)
        {
            tst_resm(TWARN, "ERROR in nbamr_encoder_engine(): eAMREEncodeInit() returns error %d",
                     inst->amre_err);
            ret = TFAIL;
        }
    }

    /* If an error was occured in one of the initialization steps, cleanup instance and exit */

    if (ret != TPASS)
    {
        enc_cleanup(inst);
        return ret;
    }

    if (th_printing == -1)
        th_printing = inst->id;

#ifdef DEBUG_TEST
    if (thread_test == 0)
        printf("Input file:  %s\n", inst->finput.fname);
#endif

    /* 
     * Main encoding cycle continues while end of input file was reached.
     * When a regular frame was encoded, output buffer contents are written into the
     * output file and next frame's turn begins
     */

    while ( (ret == TPASS) &&
            (fread(inst->in_buf, sizeof(NBAMR_S16), L_FRAME, inst->finput.fptr) == L_FRAME) )
    {
        int i;

        for (i = 0; i < inst->outbuf_size; i++) /* Seems that this cleanup is needed for proper work */
            inst->out_buf[i] = 0;
        inst->amre_err = eAMREEncodeFrame(conf, inst->in_buf, inst->out_buf);
        inst->framecount++;

        if (inst->amre_err == E_NBAMRE_OK)
        {
            if (th_printing == inst->id)
                print_status();

            /* If output file name is set in config list file, write to it */

            if (inst->foutput.fptr)
            {
                if (conf->u8BitStreamFormat == NBAMR_ETSI)
                {
                    if (write_frame(inst->out_buf, inst->outbuf_size, sizeof(NBAMR_S16), inst->foutput.fptr) == FALSE)
                        ret = TFAIL;
                }
                else
                {
                
                    /* IF1 frame format returns number of bits based on the mode used. This includes
                     * the header part of 8 * 3 = 24 bits. Following calculation is done to get the
                     * number of packed bytes to be written into the file. 1 is added for the remainder bits.
                     */

                    if (conf->u8BitStreamFormat == NBAMR_IF1IO)
                        *conf->pu32AMREPackedSize = (*conf->pu32AMREPackedSize >> 3) + 1;

                    if (write_frame(inst->out_buf, *conf->pu32AMREPackedSize, sizeof(NBAMR_U8), inst->foutput.fptr) == FALSE)
                        ret = TFAIL;
                }
            }

            /* In relocatability test realloc encoder memory every 'RELOCATE_CYCLE'-th frame*/

            if ( (!(inst->framecount % RELOCATE_CYCLE)) && relocate_test )
            {
                if (realloc_enc_memory(inst) != TRUE )
                    ret = TFAIL;
                else
                {
                    inst->amre_err = eAMREEncodeInit(conf);
                    if (inst->amre_err != E_NBAMRE_OK)
                    {
                        tst_resm(TWARN, "ERROR in nbamr_encoder_engine(): eAMREEncodeInit() "
                                "returns error %d", inst->amre_err);
                        ret = TFAIL;
                    }
                }
            }
        }
        else
        {
            ret = TFAIL;
            if (inst->amre_err == E_NBAMRE_INVALID_MODE)
                tst_resm(TWARN, "ERROR: invalid amr mode specified: '%s'", *conf->pps8APPEModeStr);
            else
                tst_resm(TWARN, "ERROR %d in eAMREEncodeFrame()", inst->amre_err);
        }
        
    }
    if (th_count == 1) printf("\n");
    enc_cleanup(inst);
    return ret;
}

/*===== print_status =====*/
/**
@brief  Prints number of frames encoded for all running threads.

@param  Input:  None
        Output: None

@return None
*/
void print_status(void)
{
    int i;
    nbamr_enc_inst *inst;

    for (i = 0; i < th_count; i++)
    {
        inst = &enc_inst[i];
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

@param  Input:  instance - pointer to the structure holding buffers, encoder config structure etc.
        Output: None

@return On success - return TRUE
        On failure - return FALSE
*/
int open_fstreams(nbamr_enc_inst *instance)
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

    /* write AMR magic number to indicate single channel AMR file format */

    if (instance->amre_config->u8BitStreamFormat == NBAMR_MMSIO)
    {
        fwrite(NBAMR_MAGIC_NUMBER, sizeof(NBAMR_U8), strlen(NBAMR_MAGIC_NUMBER), instance->foutput.fptr);
        fflush(instance->foutput.fptr);
        if (ferror(instance->foutput.fptr))
            tst_resm(TWARN, "ERROR in open_fstreams() for %s: %s", instance->foutput.fname, strerror(errno));
    }

    return TRUE;
}

/*===== alloc_enc_buffers =====*/
/**
@brief  Allocates memory for:
        all chunks requested by encoder (as returned by eAMREQueryMem());
        application input buffer.

@param  Input:  instance - pointer to the structure holding buffers, encoder config structure etc.
        Output: None

@return On success - return TRUE
        On failure - return FALSE
*/
int alloc_enc_buffers(nbamr_enc_inst *instance)
{
    int chunk_cnt;
    int i;
    sAMREEncoderConfigType *conf;
    sAMREMemAllocInfoSubType *mem;
    int ret = TRUE;

    if (!instance)
    {
        tst_resm(TWARN, "ERROR in alloc_enc_buffers(): invalid parameter");
        return FALSE;
    }
    conf = instance->amre_config;
    
    /* Allocate memory for all required chunks and buffers */

    chunk_cnt = conf->sAMREMemInfo.s32AMRENumMemReqs;
    for (i = 0; i < chunk_cnt; i++)
    {
        mem = &(conf->sAMREMemInfo.asMemInfoSub[i]);
        mem->pvAPPEBasePtr = malloc(mem->s32AMRESize);
        if (!mem->pvAPPEBasePtr)
        {
            tst_resm(TWARN, "ERROR in alloc_enc_buffers(): malloc() for chunk %d returns %s",
                    i, strerror(errno));
            ret = FALSE;
        }
    }

    if (ret)
    {
        instance->in_buf = (NBAMR_S16 *)malloc(conf->u8NumFrameToEncode * L_FRAME * sizeof(NBAMR_S16));
        if (!instance->in_buf)
        {
            tst_resm(TWARN, "ERROR in alloc_enc_buffers(): malloc() for input buffer returns %s",
                    strerror(errno));
            ret = FALSE;
        }
    }
    if (ret)
    {
        instance->out_buf = (NBAMR_S16 *)malloc(instance->outbuf_size * sizeof(NBAMR_S16));
        if (!instance->out_buf)
        {
            tst_resm(TWARN, "ERROR in alloc_enc_buffers(): malloc() for output buffer returns %s",
                    strerror(errno));
            ret = FALSE;
        }
    }

    return ret;
}

/*===== enc_cleanup =====*/
/**
@brief  Releases file streams allocated by open_fstreams().
        Frees memory allocated by alloc_enc_buffers().

@param  Input:  instance - pointer to the structure holding buffers, encoder config structure etc.
        Output: None

@return Nothing
*/
void enc_cleanup(nbamr_enc_inst *instance)
{
    int i;
    int nr;
    sAMREEncoderConfigType *conf;
    sAMREMemAllocInfoSubType *mem;
    
    if (!instance)
    {
        tst_resm(TWARN, "ERROR in enc_cleanup(): invalid parameter");
        return;
    }

    if (instance->finput.fptr)
        fclose(instance->finput.fptr);
    if (instance->foutput.fptr)
        fclose(instance->foutput.fptr);

    conf = instance->amre_config;
    if(instance->in_buf)
        free(instance->in_buf);
    if(instance->out_buf)
        free(instance->out_buf);
        
    nr = conf->sAMREMemInfo.s32AMRENumMemReqs;
    for (i = 0; i < nr; i++)
    {
        mem = &(conf->sAMREMemInfo.asMemInfoSub[i]);
        if (mem->pvAPPEBasePtr)
            free(mem->pvAPPEBasePtr);
    }

    if (conf)
        free(conf);

    return;
}

/*===== realloc_enc_memory =====*/
/**
@brief  Frees encoder memory and allocates it again, but in other place.

@param  Input:  instance - pointer to the structure holding buffers, encoder config structure etc.
        Output: None

@return On success - return TRUE
        On failure - return FALSE
*/
int realloc_enc_memory(nbamr_enc_inst *instance)
{
    sAMREEncoderConfigType *conf;
    sAMREMemAllocInfoSubType *mem;
    int  i;
    int  nr;
    void *barrier_ptr;
    int  ret = TRUE;
    
    if (!instance)
    {
        tst_resm(TWARN, "ERROR in realloc_enc_memory(): invalid parameter");
        return FALSE;
    }
    conf = instance->amre_config;

    /* Deallocate all memory chunk and then again allocate them */

    nr = conf->sAMREMemInfo.s32AMRENumMemReqs;
    for (i = 0; i < nr; i++)
    {
        mem = &(conf->sAMREMemInfo.asMemInfoSub[i]);
        if (mem->pvAPPEBasePtr)
            free(mem->pvAPPEBasePtr);
    }

    /* Allocate some memory to be sure that encoder memory will be allocated in other place */

    barrier_ptr = malloc(SERIAL_FRAMESIZE * instance->framecount);
    if (!barrier_ptr)
    {
        tst_resm(TWARN, "ERROR in realloc_enc_memory(): malloc() for barrier_ptr returns %s",
                strerror(errno));
        ret = FALSE;
    }
    else
    {
        for (i = 0; i < nr; i++)
        {
            mem = &(conf->sAMREMemInfo.asMemInfoSub[i]);
            mem->pvAPPEBasePtr = malloc(mem->s32AMRESize);
            if (!mem->pvAPPEBasePtr)
            {
                tst_resm(TWARN, "ERROR in realloc_enc_memory: malloc() for chunk %d returns %s",
                        i, strerror(errno));
                ret = FALSE;
            }
        }
        free(barrier_ptr);
    }

    return ret;
}

/*===== write_frame =====*/
/**
@brief  Builds interlaced multichannel frame from non-interlaced frame and writes it into the
        output stream.

@param  Input:  out_buf     - encoder output buffer
                framesize   - size of frame to be written
                itemsize    - size of one item in output buffer
                fptr        - output file stream pointer
        Output: None

@return On success - return TRUE
        On failure - return FALSE
*/
int write_frame(NBAMR_S16 *out_buf, int framesize, int itemsize, FILE *fptr)
{
    int ret = FALSE;
    
    if (!fptr)
    {
        tst_resm(TWARN, "ERROR in write_frame(): invalid argument");
        return ret;
    }

    fwrite(out_buf, itemsize, framesize, fptr);
    
    if (ferror(fptr))
        tst_resm(TWARN, "ERROR in write_frame(): fwrite() returns error %s", strerror(errno));
    else
        ret = TRUE;

    return ret;
}

/*===== run_encoder_thread =====*/
/**
@brief  This is a thread function. It changes process priority in case of preemption test and
        runs encoder engine.

@param  instance - void pointer to thread arguments.

@return NULL
*/
void *run_encoder_thread(void *instance)
{
    int i;
    nbamr_enc_inst *inst = (nbamr_enc_inst *)instance;

    inst->ltp_err = nbamr_encoder_engine(inst);
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
            if (enc_inst[i].th_finish == FALSE)
            {
                th_printing = enc_inst[i].id;
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

/*===== set_enc_instance =====*/
/**
@brief  Sets instance ID and file names.

@param  index     - index of instance that will be used
        list_node - pointer to the current flist_t entry (that stores file names)

@return On success - return TRUE
        On failure - return FALSE
*/
int set_enc_instance(int index, flist_t *list_node)
{
    if ( (index >= MAX_ENC_THREADS) || (!list_node) )
    {
        tst_resm(TWARN, "ERROR in set_enc_instance(): one of parameters isn't valid");
        return FALSE;
    }

    enc_inst[index].framecount = 0;
    enc_inst[index].th_finish = FALSE;
    enc_inst[index].id = index;
    enc_inst[index].bitrate = list_node->br_mode;
    enc_inst[index].dtx_enabled = list_node->dtx;
    enc_inst[index].bs_format = list_node->bitstr_format;
    enc_inst[index].packed_size = 0;
    enc_inst[index].finput.fname = list_node->inp_fname; /* fname isn't "n/a": already checked */
    if (strcmp(list_node->out_fname, EMPTY_FILE))
        enc_inst[index].foutput.fname = list_node->out_fname;
    else
        enc_inst[index].foutput.fname = NULL;

    enc_inst[index].foutput.fptr = NULL;

    if (strcmp(list_node->ref_fname, EMPTY_FILE))
        enc_inst[index].fref.fname = list_node->ref_fname;
    else
        enc_inst[index].fref.fname = NULL;

    enc_inst[index].fref.fptr = NULL;

    return TRUE;
}

/*===== nominal_functionality_test =====*/
/**
@brief  Testing encoder nominal functionality.

@param  None.

@return On success - return TPASS
        On failure - return the error code
*/
int nominal_functionality_test()
{
    int i;
    flist_t *node;
    int ret = TPASS;
    nbamr_enc_inst *inst = &enc_inst[0];

    /* Check functionality for all entry read from list */

    ++th_count;
    for (node = files_list, i = 0; node && (ret == TPASS); node = node->next, i++)
    {
        if (set_enc_instance(0, node) == FALSE) /* Set file names and instance ID */
        {
            ret = TFAIL;
            break;
        }
        if (nbamr_encoder_engine(inst) != TPASS)
            ret = TFAIL;
        else if (inst->fref.fname && inst->foutput.fname)  /* perform bitmatch */
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
    for (node = files_list, i = 0; node && (i < MAX_ENC_THREADS) && (ret == TPASS); node = node->next, i++)
    {
        if (set_enc_instance(i, node) == FALSE) /* Set file names and instance ID */
        {
            ret = TFAIL;
            break;
        }
        ++th_count;
        if (pthread_create(&enc_inst[i].tid, NULL, (void *)&run_encoder_thread, (void *)&enc_inst[i]))
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
            enc_inst[i].th_err = pthread_join(enc_inst[i].tid, NULL);

        for (i = 0; i < th_count; i++)
        {
            if (enc_inst[i].th_err)
            {
                tst_resm(TWARN, "Thread %2d was finished with error %s", i + 1, strerror(errno));
                ret = TFAIL;
            }
            else if (enc_inst[i].ltp_err != TPASS)
            {
                tst_resm(TWARN, "Thread %2d was finished with UNsuccessful result", i + 1);
                ret = enc_inst[i].ltp_err;
            }
        }
    }

    printf("\n");
    return ret;
}

/*===== relocatability_test  =====*/
/**
@brief  Test of encoder code relocatability.

@param  None.

@return On success - return TPASS
        On failure - return the error code
*/
int relocatability_test()
{
    int i, j;
    flist_t *node;
    int ret = TPASS;
    nbamr_enc_inst *inst = &enc_inst[0];

    /* Check functionality for all entry read from list */

    ++th_count;
    for (node = files_list, i = 0; node && (ret == TPASS); node = node->next, i++)
    {
        for( j = 0; j < 10; ++j )
        {
            if (set_enc_instance(0, node) == FALSE) /* Set file names and instance ID */
            {
                ret = TFAIL;
                break;
            }
            if (nbamr_encoder_engine(inst) != TPASS)
                ret = TFAIL;
            else if (inst->fref.fname && inst->foutput.fname)  /* perform bitmatch */
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

/*===== mk_entry =====*/
/**
@brief  Makes flist_t entry from three strings, representing file names. There may be multiple
        reference files, because each channel is stored in separate file.

@param  Input:  inp_fname - pointer to the input file name
                mode      - string representing bitrate mode
                dtx_flag  - value that equals: 0 if dtx isn't used, 1 or 2 if dtx with vad1 or vad2 is used
                format    - identifies output format
                out_fname - pointer to the output file name
                ref_fname - pointer to the reference file names, delimited by '+' sign
        Output: None

@return On success - return pointer to the created flist_t entry
        On failure - return NULL
*/
flist_t *mk_entry(const char *inp_fname, const char *mode, const char *dtx_flag,
                  const char *format, const char *out_fname, const char *ref_fname)
{
    flist_t *list = malloc(sizeof(flist_t));
    
    if (list)
    {
	if ( (strlen(inp_fname) < MAX_STR_LEN) &&
	     (strlen(out_fname) < MAX_STR_LEN) &&
	     (strlen(ref_fname) < MAX_STR_LEN) &&
             (strlen(mode) < MAX_STR_LEN) &&
             (strlen(dtx_flag) < MAX_STR_LEN) &&
             (strlen(format)< MAX_STR_LEN) )
	{
	    strcpy(list->inp_fname, inp_fname);
	    strcpy(list->out_fname, out_fname);
	    strcpy(list->ref_fname, ref_fname);
	    strcpy(list->br_mode, mode);
            list->dtx = atoi(dtx_flag);
            list->bitstr_format = atoi(format);
	}
        else
            tst_resm(TWARN, "ERROR in mk_entry(): one of parameteres in config is too long");
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
    char    line[6][MAX_STR_LEN];
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

        if (i == 5)
        {
            if (strcmp(line[0], EMPTY_FILE) != 0) /* No input file - nothing to be do  */
            {
                if (!flist)                       /* First entry will be created       */
                {
                    flist = mk_entry(line[0], line[1], line[2], line[3], line[4], line[5]);
                    node = flist;
                    if (!flist)
                        ret = FALSE;
                }
                else                              /* Next entries in linked list       */
                {
                    node->next = mk_entry(line[0], line[1], line[2], line[3], line[4], line[5]);
                    node = node->next;
                    if (!node)
                        ret = FALSE;
                }
            }
            else
                tst_resm(TWARN, "ERROR in read_cfg(): config file name is %s", EMPTY_FILE);
        }  /* if (i == 5) */
        i++;
        i %= 6;
    }

    *pplist = flist;
    return ret;
}

/*===== perform_bitmatch_raw =====*/
/**
@brief  Compares two binary files. If files mismatch, returns an error.

@param  None
  
@return On success - return TRUE
        On failure - return FALSE
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
