/*================================================================================================*/
/**
    @file   wbamr_encoder_test.c

    @brief  C source file of the WB AMR encoder test application.
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
Igor Semenchukov/smng001c    01/12/2004     TLSbo43523   Initial version
Igor Semenchukov/smng001c    14/12/2004     TLSbo43523   Little changes concerning mode error
Igor Semenchukov/smng001c    07/02/2005     TLSbo47179   Changed include directives (lib headers)
Igor Semenchukov/smng001c    28/02/2005     TLSbo47117   Changed printf() entries with tst_...()
D.Simakov/smkd001c           23/05/2005     TLSbo47117   Changes concerning compilation with the new 
                                                         encoder version.
S. V-Guilhou/svan01c         25/05/2005     TLSbo50534   P4 Codec Campaign / Add traces
D.Simakov/smkd001c           09/06/2005     TLSbo50905   Bug fix.                                                         
D.Simakov/smkd001c           03/11/2005     TLSbo57009   Update
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
#include <wbamr_enc_interface.h>

/* Harness Specific Include Files. */

#include "test.h"

/* Verification Test Environment Include Files */

#include "wbamr_encoder_test.h"

/*==================================================================================================
                                        LOCAL MACROS
==================================================================================================*/
#define MAX_ENC_THREADS 4
#define RELOCATE_CYCLE  10
#define EMPTY_FILE      "n/a"
#define DEF_OUT_FORMAT  0
#define TBD         NULL  /* It isn't defined in the enc. API header file :) */

/*==================================================================================================
                          LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
==================================================================================================*/

typedef struct
{
    char *fname;
    FILE *fptr;
} filehandle;

/* Structure stores encoder instance/thread parameters, buffers etc. */

typedef struct
{
    int                   id;
    filehandle            finput,
                          foutput,
                          fref;
    WBAMR_S16             bitrate;
    WBAMR_S32             dtx_enabled;
    WBAMR_U16             output_size;

    WBAMR_S16             *in_buf;
    WBAMR_S16             *out_buf;
    WBAMRE_Encoder_Config *amre_config;

    /* Some service variables */

    pthread_t             tid;        /* Thread ID. Used only if thread is created */
    int                   th_finish;
    int                   framecount;

    int                   th_err;     /* Error value returned by pthread_join()    */
    int                   ltp_err;    /* LTP error value returned by thread        */
    WBAMRE_RET_TYPE       amre_err;   /* Encoder functions return values           */

} wbamr_enc_inst;

/*==================================================================================================
                                       LOCAL CONSTANTS
==================================================================================================*/
const char def_list_file[]     = "wbamre_def_test_files";

const char *wbamre_err_msg[14] =
{
    "WBAMRE_OK",
    "WBAMRE_WARNING",
    "WBAMRE_INVALID_MODE",
    "WBAMRE_INIT_ERROR",
    "WBAMRE_MEMALLOC_ERROR",
};

const char progress[] = "-\\|/";      /* For rounding indicator */
/*==================================================================================================
                                       LOCAL VARIABLES
==================================================================================================*/
static wbamr_enc_inst enc_inst[MAX_ENC_THREADS];
static flist_t        *files_list      = NULL;
int                   relocate_test    = FALSE; /* Acts as a switch in the aaclc_encoder_engine() */
int                   th_count         = 0;     /* Used when some info must be printed            */
int                   th_printing      = -1;    /* Helps thread to determine it is print. thread  */
pthread_mutex_t io_mutex = PTHREAD_MUTEX_INITIALIZER;

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

int  wbamr_encoder_engine(wbamr_enc_inst *inst);
int  alloc_enc_buffers(wbamr_enc_inst *instance);
int  realloc_enc_memory(wbamr_enc_inst *instance);
void enc_cleanup(wbamr_enc_inst *instance);
int  open_fstreams(wbamr_enc_inst *instance);
int  write_frame(WBAMR_S16 *out_buf, FILE *fptr, WBAMR_U16 *outbuf_size);
void print_status(void);
int  perform_bitmatch_raw(filehandle *out, filehandle *ref);
int  set_enc_instance(int index, flist_t *list_node);
void *run_encoder_thread(void *instance);

/* test functions */

int nominal_functionality_test();
int reentrance_test();
int relocatability_test();

/*==================================================================================================
                                       LOCAL FUNCTIONS
==================================================================================================*/


/*================================================================================================*/
/*===== VT_wbamr_encoder_setup =====*/
/**
@brief  assumes the pre-condition of the test case execution

@param  None

@return On success - return TPASS
        On failure - return the error code
*/
/*================================================================================================*/
int VT_wbamr_encoder_setup()
{
    return TPASS;
}

/*================================================================================================*/
/*===== VT_wbamr_encoder_cleanup =====*/
/**
@brief  assumes the post-condition of the test case execution

@param  None

@return On success - return TPASS
        On failure - return the error code
*/
/*================================================================================================*/
int VT_wbamr_encoder_cleanup()
{
    return TPASS;
}

/*================================================================================================*/
/*===== VT_wbamr_encoder_test =====*/
/**
@brief  Reads list of files (input, output and reference). Executes test specified by 'testcase'
        variable.

@param  Input:  testcase - Testcase id of the test according to the test plan
                listfile - pointer to the name of list file
        Output: None
        

@return On success - return TPASS
        On failure - return the error code
*/
/*================================================================================================*/
int VT_wbamr_encoder_test(int testcase, char *listfile)
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
	    tst_resm(TFAIL, "Wrong test case!!");
	    break;
    }

    return rv;
}

/*================================================================================================*/
/*===== wbamr_encoder_engine =====*/
/**
@brief  Engine of the encoder. The encoding of a bitstream should be presented here.
	Also this function processes encoder result, i.e. displays it for a video data case
	or plays it for a sound data case.
	This method should be compatible with a threads.

@param  Input:  inst - pointer to the structure holding buffers, encoder config structure etc.
        Output: None

@return On success - return TPASS
        On failure - return the error code
*/
/*================================================================================================*/
int wbamr_encoder_engine(wbamr_enc_inst *inst)
{
    WBAMRE_Encoder_Config *conf; /* Pointer to simplify references */
    int  ret         = TPASS;
    int mode_val;                /* Added because of a bug in library (wbappe_modeStr) changes */

    if (!inst)                   /* Error: NULL pointer */
    {
        tst_resm(TFAIL, "ERROR in wbamr_encoder_engine(): invalid parameter");
        return TFAIL;
    }

    /* Allocate memory for encoder parameter structure, populate known fields */

    
    inst->amre_config = (WBAMRE_Encoder_Config *)malloc(sizeof(WBAMRE_Encoder_Config));
    
    if (!inst->amre_config)
    {
        tst_resm(TFAIL, "ERROR in wbamr_encoder_engine(): malloc() for encoder config returns "
                "%s", strerror(errno));
        ret = TFAIL;
    }

    if (ret == TPASS)
    {
        conf = inst->amre_config;
        conf->wbappe_initialized_data_start = BEGIN_WBAMRE_DATA;
        conf->wbamre_output_format = DEF_OUT_FORMAT;
        mode_val = inst->bitrate; /* Added because of a bug in library (wbappe_modeStr) changes */
        conf->wbappe_dtx_flag = inst->dtx_enabled;
        conf->wbappe_mode = &(inst->bitrate);
        conf->wbamre_output_size = &(inst->output_size);

        /* Get encoder memory requirements. Info will be placed in the aacd_mem_info field */

        inst->amre_err = wbamre_query_enc_mem(conf);
        if (inst->amre_err != WBAMRE_OK)
        {
            tst_resm(TFAIL, "ERROR in wbamr_encoder_engine(): wbamre_query_enc_mem() returns "
                    "error '%s'", wbamre_err_msg[inst->amre_err]);
            ret = TFAIL;
        }
    }

    if (ret == TPASS)
    {
        if (alloc_enc_buffers(inst) != TRUE)
            ret = TFAIL;
    }

    /*
     * Open input and output streams. Input stream must be opened before aacd_encoder_init()
     * will be called, because it calls app_swap_buffers_aac_enc(), which reads from
     * input stream.
     */

    if (ret == TPASS)
    {
        if (open_fstreams(inst) != TRUE) 
            ret = TFAIL;
    }

    if (ret == TPASS)
    {
        inst->amre_err = wbamre_encode_init(conf);
        if (inst->amre_err != WBAMRE_OK)
        {
            tst_resm(TFAIL, "ERROR in wbamr_encoder_engine(): wbamre_encoder_init() returns "
                    "error '%s'", wbamre_err_msg[inst->amre_err]);
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
    if (th_count == 1)
        printf("\nInput file:  %s\n", inst->finput.fname);
#endif
    
    /* 
     * Main encoding cycle continues while end of input file was reached.
     * When a regular frame was encoded, output buffer contents are written into the
     * output file and next frame's turn begins
     */

    

    while ( (ret == TPASS) && (fread(inst->in_buf, sizeof(WBAMR_S16), WBAMR_L_FRAME,
             inst->finput.fptr) == WBAMR_L_FRAME) )
    {
        *conf->wbappe_mode = mode_val; /* Added because of a bug in library (wbappe_modeStr) changes */
        inst->amre_err = wbamre_encode_frame(conf, inst->in_buf, inst->out_buf);
        inst->framecount++;

        if (inst->amre_err == WBAMRE_OK)
        {
            if (th_printing == inst->id)
                print_status();
            
            /* If output file name is set in config list file, write to it */

            if (inst->foutput.fptr)
            {
                if (write_frame(inst->out_buf, inst->foutput.fptr, conf->wbamre_output_size)
                    == FALSE)
                {
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
                    inst->amre_err = wbamre_encode_init(conf);
                    if (inst->amre_err != WBAMRE_OK)
                    {
                        tst_resm(TFAIL, "ERROR in wbamr_encoder_engine(): wbamre_encode_init() "
                                "returns error '%s'", wbamre_err_msg[inst->amre_err]);
                        ret = TFAIL;
                    }
                }
            }
        }
        else
            tst_resm(TWARN, "Invalid frame [%d]: %s", inst->framecount, wbamre_err_msg[inst->amre_err]);
    }

    enc_cleanup(inst);
    
    return ret;
}

/*================================================================================================*/
/*===== print_status =====*/
/**
@brief  Prints number of frames encoded for all running threads.

@param  Input:  None
        Output: None

@return None
*/
/*================================================================================================*/
void print_status(void)
{
    int i;
    wbamr_enc_inst *inst;

    for (i = 0; i < th_count; i++)
    {
        inst = &enc_inst[i];
        printf("th[%d]-", inst->id + 1);
        if (inst->th_finish) printf("ended ");
        else printf("frames");
        printf("[%3d] ", inst->framecount);
    }
    printf("%c\r", progress[inst->framecount % (sizeof(progress) - 1)]);
    fflush(stdout);
    return;
}

/*================================================================================================*/
/*===== open_fstreams =====*/
/**
@brief  Opens streams associated with input and output files.

@param  Input:  instance - pointer to the structure holding buffers, encoder config structure etc.
        Output: None

@return On success - return TRUE
        On failure - return FALSE
*/
/*================================================================================================*/
int open_fstreams(wbamr_enc_inst *instance)
{
    if (!instance)  /* Error: NULL pointer */
    {
        tst_resm(TFAIL, "ERROR in open_fstreams(): invalid parameter");
        return FALSE;
    }

    if ((instance->finput.fptr = fopen(instance->finput.fname, "r")) == NULL)
    {
        tst_resm(TFAIL, "ERROR in open_fstreams(): fopen() for input file %s returns %s",
                instance->finput.fname, strerror(errno));
        return FALSE;
    }
    if (instance->foutput.fname != NULL)
    {
        if ((instance->foutput.fptr = fopen(instance->foutput.fname, "w")) == NULL)
        {
            tst_resm(TFAIL, "ERROR in open_fstreams(): fopen() for output file %s returns %s",
                    instance->foutput.fname, strerror(errno));
            return FALSE;
        }
    }

    return TRUE;
}

/*================================================================================================*/
/*===== alloc_enc_buffers =====*/
/**
@brief  Allocates memory for:
            all chunks requested by encoder (as returned by aacd_query_enc_mem());
            application input buffer.

@param  Input:  instance - pointer to the structure holding buffers, encoder config structure etc.
        Output: None

@return On success - return TRUE
        On failure - return FALSE
*/
/*================================================================================================*/
int alloc_enc_buffers(wbamr_enc_inst *instance)
{
    int chunk_cnt;
    int i;
    WBAMRE_Encoder_Config *conf;
    WBAMRE_Mem_Alloc_Info_Sub *mem;
    int ret = TRUE;

    if (!instance)
    {
        tst_resm(TFAIL, "ERROR in alloc_enc_buffers(): invalid parameter");
        return FALSE;
    }
    conf = instance->amre_config;
    
    /* Allocate memory for all required chunks and buffers */

    chunk_cnt = conf->wbamre_mem_info.wbamre_num_reqs;
    for (i = 0; i < chunk_cnt; i++)
    {
        mem = &(conf->wbamre_mem_info.mem_info_sub[i]);
        mem->wbappe_base_ptr = malloc(mem->wbamre_size);
        if (!mem->wbappe_base_ptr)
        {
            tst_resm(TFAIL, "ERROR in alloc_enc_buffers(): malloc() for chunk %d returns %s",
                    i, strerror(errno));
            ret = FALSE;
        }
    }

    if (ret)
    {
        instance->in_buf = (WBAMR_S16 *)malloc(WBAMR_L_FRAME * sizeof(WBAMR_S16));
        if (!instance->in_buf)
        {
            tst_resm(TFAIL, "ERROR in alloc_enc_buffers(): malloc() for input buffer returns %s",
                    strerror(errno));
            ret = FALSE;
        }
    }
    if (ret)
    {
        instance->out_buf = (WBAMR_S16 *)malloc(WBAMR_SERIAL_FRAMESIZE * sizeof(WBAMR_S16));
        if (!instance->out_buf)
        {
            tst_resm(TFAIL, "ERROR in alloc_enc_buffers(): malloc() for output buffer returns %s",
                    strerror(errno));
            ret = FALSE;
        }
    }

    return ret;
}

/*================================================================================================*/
/*===== enc_cleanup =====*/
/**
@brief  Releases file streams allocated by open_fstreams().
        Frees memory allocated by alloc_enc_buffers().

@param  Input:  instance - pointer to the structure holding buffers, encoder config structure etc.
        Output: None

@return Nothing
*/
/*================================================================================================*/
void enc_cleanup(wbamr_enc_inst *instance)
{
    int i;
    int nr;
    WBAMRE_Encoder_Config *conf;
    WBAMRE_Mem_Alloc_Info_Sub *mem;
    
    if (!instance)
    {
        tst_resm(TFAIL, "ERROR in enc_cleanup(): invalid parameter");
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
        
    nr = conf->wbamre_mem_info.wbamre_num_reqs;
    for (i = 0; i < nr; i++)
    {
        mem = &(conf->wbamre_mem_info.mem_info_sub[i]);
        if (mem->wbappe_base_ptr)
            free(mem->wbappe_base_ptr);
    }

    if (conf)
        free(conf);

    return;
}

/*================================================================================================*/
/*===== realloc_enc_memory =====*/
/**
@brief  Frees encoder memory and allocates it again, but in other place.

@param  Input:  instance - pointer to the structure holding buffers, encoder config structure etc.
        Output: None

@return On success - return TRUE
        On failure - return FALSE
*/
/*================================================================================================*/
int realloc_enc_memory(wbamr_enc_inst *instance)
{
    WBAMRE_Encoder_Config     *conf;
    WBAMRE_Mem_Alloc_Info_Sub *mem;
    int  i;
    int  nr;
    void *barrier_ptr;
    int  ret = TRUE;
    
    if (!instance)
    {
        tst_resm(TFAIL, "ERROR in realloc_enc_memory(): invalid parameter");
        return FALSE;
    }
    conf = instance->amre_config;

    /* Deallocate all memory chunk and then again allocate them */

    nr = conf->wbamre_mem_info.wbamre_num_reqs;
    for (i = 0; i < nr; i++)
    {
        mem = &(conf->wbamre_mem_info.mem_info_sub[i]);
        if (mem->wbappe_base_ptr)
            free(mem->wbappe_base_ptr);
    }

    /* Allocate some memory to be sure that encoder memory will be allocated in other place */

    barrier_ptr = malloc(WBAMR_SERIAL_FRAMESIZE * instance->framecount);
    if (!barrier_ptr)
    {
        tst_resm(TFAIL, "ERROR in realloc_enc_memory(): malloc() for barrier_ptr returns %s",
                strerror(errno));
        ret = FALSE;
    }
    else
    {
        for (i = 0; i < nr; i++)
        {
            mem = &(conf->wbamre_mem_info.mem_info_sub[i]);
            mem->wbappe_base_ptr = malloc(mem->wbamre_size);
            if (!mem->wbappe_base_ptr)
            {
                tst_resm(TFAIL, "ERROR in realloc_enc_memory: malloc() for chunk %d returns %s",
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

@param  Input:  outbuf      - encoder output buffer
                fptr        - output file stream pointer
                outbuf_size - pointer to the variable holding size of the encoded frame
        Output: None

@return On success - return TRUE
        On failure - return FALSE
*/
/*================================================================================================*/
int write_frame(WBAMR_S16 *out_buf, FILE *fptr, WBAMR_U16 *outbuf_size)
{
    int ret = FALSE;

    if (!fptr)
    {
        tst_resm(TFAIL, "ERROR in write_frame(): invalid argument");
        return ret;
    }

    fwrite(out_buf, sizeof(WBAMR_S16), *outbuf_size, fptr);
    
    if (ferror(fptr))
    {
        tst_resm(TFAIL, "ERROR in write_frame(): fwrite() returns error %s", strerror(errno));
    }
    else
        ret = TRUE;

    return ret;
}

/*================================================================================================*/
/*===== run_encoder_thread =====*/
/**
@brief  This is a thread function. It changes process priority in case of preemption test and
        runs encoder engine.

@param  instance - void pointer to thread arguments.

@return NULL
*/
/*================================================================================================*/
void *run_encoder_thread(void *instance)
{
    int i;
    wbamr_enc_inst *inst = (wbamr_enc_inst *)instance;

    inst->ltp_err = wbamr_encoder_engine(inst);
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

/*================================================================================================*/
/*===== set_enc_instance =====*/
/**
@brief  Sets instance ID and file names.

@param  index     - index of instance that will be used
        list_node - pointer to the current flist_t entry (that stores file names)

@return On success - return TRUE
        On failure - return FALSE
*/
/*================================================================================================*/
int set_enc_instance(int index, flist_t *list_node)
{
    if ( (index >= MAX_ENC_THREADS) || (!list_node) )
    {
        tst_resm(TFAIL, "ERROR in set_enc_instance(): one of parameters isn't valid");
        return FALSE;
    }

    enc_inst[index].framecount = 0;
    enc_inst[index].th_finish = FALSE;
    enc_inst[index].id = index;
    enc_inst[index].bitrate = list_node->br_mode;
    enc_inst[index].dtx_enabled = list_node->dtx;
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

/*================================================================================================*/
/*===== nominal_functionality_test =====*/
/**
@brief  Testing encoder nominal functionality.

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
    wbamr_enc_inst *inst = &enc_inst[0];

    if (!files_list)
    {
        tst_resm(TFAIL, "ERROR in nominal functionality test: list of files is empty");
        return TFAIL;
    }

    /* Check functionality for all entry read from list */

    th_count = 1;
    for (node = files_list, i = 0; node; node = node->next, i++)
    {
        if (set_enc_instance(0, node) == FALSE) /* Set file names and instance ID */
        {
            ret = TFAIL;
            break;
        }
        if (wbamr_encoder_engine(inst) != TPASS)
            ret = TFAIL;

        /* perform bitmatch */
        if (inst->fref.fname && inst->foutput.fname)
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

    if (!files_list)
    {
        tst_resm(TFAIL, "ERROR in reentrance test: list of files is empty");
        return TFAIL;
    }

    for (node = files_list, i = 0; node && (i < MAX_ENC_THREADS); node = node->next, i++)
    {
        if (set_enc_instance(i, node) == FALSE) /* Set file names and instance ID */
        {
            ret = TFAIL;
            break;
        }
        if (pthread_create(&enc_inst[i].tid, NULL, (void *)&run_encoder_thread,
            (void *)&enc_inst[i]))
        {
            tst_resm(TFAIL, "ERROR: cannot create thread %d: %s", i + 1, strerror(errno));
            ret = TFAIL;
            break;
        }
    }

    if (ret == TPASS)
    {
        th_count = i;

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
                tst_resm(TFAIL, "Thread %2d was finished with error %s", i + 1, strerror(errno));
                ret = TFAIL;
            }
            else if (enc_inst[i].ltp_err != TPASS)
            {
                tst_resm(TFAIL, "Thread %2d was finished with UNsuccessful result", i + 1);
                ret = enc_inst[i].ltp_err;
            }
        }
    }

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
int relocatability_test()
{
    int i, j;
    flist_t *node;
    int ret = TPASS;
    wbamr_enc_inst *inst = &enc_inst[0];

    if (!files_list)
    {
        tst_resm(TFAIL, "ERROR in nominal functionality test: list of files is empty");
        return TFAIL;
    }

    /* Check functionality for all entry read from list */

    th_count = 1;
    for (node = files_list, i = 0; node; node = node->next, i++)
    {
        for( j = 0; j < 10; ++j )
        {
            if (set_enc_instance(0, node) == FALSE) /* Set file names and instance ID */
            {
                ret = TFAIL;
                break;
            }
            if (wbamr_encoder_engine(inst) != TPASS)
                ret = TFAIL;

            /* perform bitmatch */
            if (inst->fref.fname && inst->foutput.fname)
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
flist_t *mk_entry(const char *inp_fname, const char *mode, const char *dtx_flag,
                  const char *out_fname, const char *ref_fname)
{
    flist_t *list = malloc(sizeof(flist_t));
    
    if (list)
    {
	if ( (strlen(inp_fname) < MAX_STR_LEN) &&
	     (strlen(out_fname) < MAX_STR_LEN) &&
	     (strlen(ref_fname) < MAX_STR_LEN) )
	{
	    strcpy(list->inp_fname, inp_fname);
	    strcpy(list->out_fname, out_fname);
	    strcpy(list->ref_fname, ref_fname);
	    list->br_mode = strcmp(mode, "n/a") != 0 ? atoi(mode) : 0;
            list->dtx = atoi(dtx_flag);
	}
        else
            tst_resm(TFAIL, "ERROR in mk_entry(): one of file names too long");
    }
    else
        tst_resm(TFAIL, "ERROR in mk_entry(): malloc() returns %s", strerror(errno));

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
    char    line[5][MAX_STR_LEN];
    flist_t *node;
    flist_t *flist = NULL;
    int     i      = 0;
    int     ret    = TRUE;

    in = fopen(filename, "r");
    if (in == NULL)
    {
        tst_resm(TFAIL, "ERROR in read_cfg(): cannot open config file: %s",
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

        if (i == 4)
        {
            if (strcmp(line[0], EMPTY_FILE) != 0) /* No input file - nothing to be do  */
            {
                if (!flist)                       /* First entry will be created       */
                {
                    flist = mk_entry(line[0], line[1], line[2], line[3], line[4]);
                    node = flist;
                    if (!flist)
                        ret = FALSE;
                }
                else                              /* Next entries in linked list       */
                {
                    node->next = mk_entry(line[0], line[1], line[2], line[3], line[4]);
                    node = node->next;
                    if (!node)
                        ret = FALSE;
                }
            }
            else
                tst_resm(TFAIL, "ERROR in read_cfg(): input file name is %s",
                        EMPTY_FILE);
        }  /* if (i == 4) */
        i++;
        i %= 5;
    }

    *pplist = flist;
    return ret;
}

/*================================================================================================*/
/*===== perform_bitmatch_raw =====*/
/**
@brief 

@param  None
  
@return 
*/
/*================================================================================================*/
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
            tst_resm(TFAIL, "ERROR in perform_bitmatch_raw(): fopen() returns %s", strerror(errno));
            ret = FALSE;
        }
        else if ( (ref->fptr = fopen(ref->fname, "r")) == NULL )
        {
            tst_resm(TFAIL, "ERROR in perform_bitmatch_raw(): fopen() returns %s", strerror(errno));
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
                ret = FALSE;

            out_eof = feof(out->fptr) ? 1 : 0;
            ref_eof = feof(ref->fptr) ? 1 : 0;
            if (out_eof != ref_eof)
                ret = FALSE;
        }
    }

    if (out->fptr)
        fclose(out->fptr);
    if (ref->fptr)
        fclose(ref->fptr);

    if (!ret)
        tst_resm(TFAIL, "Bitmatch for output file %s is failed", out->fname);

    return ret;
}

#ifdef __cplusplus
}
#endif
