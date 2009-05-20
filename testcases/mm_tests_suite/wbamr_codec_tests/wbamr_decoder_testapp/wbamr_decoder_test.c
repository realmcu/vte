/**/
/**
    @file   wbamr_decoder_test.c

    @brief  C source file of the WB AMR decoder test application.
*/
/*

  Copyright (C) 2004, Freescale Semiconductor, Inc. All Rights Reserved
  THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
  BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
  Freescale Semiconductor, Inc.


Revision History:
                            Modification     Tracking
Author                          Date          Number    Description of Changes
-------------------------   ------------    ----------  -------------------------------------------
Igor Semenchukov/smng001c    30/11/2004     TLSbo43523   Initial version
Igor Semenchukov/smng001c    07/02/2005     TLSbo47179   Changed include directives (lib headers)
Igor Semenchukov/smng001c    28/02/2005     TLSbo47117   Changed printf() entries with tst_...()
D.Simakov/smkd001c           24/05/2005     TLSbo47117   Changes concerning compilation with the new
                                                         decoder version.
D.Simakov / smkd001c         08/06/2005     TLSbo50994   Reentrance test case was improved.
D.Simakov / smkd001c         16/06/2005     TLSbo51687   Robustness test case was improved, configs
                                                         was prefixed with the wbamrd_ prefix.
D.Simakov / smkd001c         03/11/2005     TLSbo57009   Updated
D.Simakov/smkd001c           16/12/2005     TLSbo60691   Unitialized bitstreamformat variable was fixed

Portability: Indicate if this module is portable to other compilers or platforms.
             If not, indicate specific reasons why is it not portable.

*/

#ifdef __cplusplus
extern "C"{
#endif

/*
                                        INCLUDE FILES
*/

/* Standard Include Files */

#include <errno.h>
#include <wbamr_dec_interface.h>

/* Harness Specific Include Files. */

#include "test.h"

/* Verification Test Environment Include Files */

#include "wbamr_decoder_test.h"
#include "bits_test.h"
#include "nb_bits.h"

/*
                                        LOCAL MACROS
*/
#define MAX_DEC_THREADS 4
#define RELOCATE_CYCLE  10
#define EMPTY_FILE      "n/a"
#define TBD         NULL  /* It isn't defined in the enc. API header file :) */

/*
                          LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
*/

typedef struct
{
    char *fname;
    FILE *fptr;
} filehandle;

/* Structure stores decoder instance/thread parameters, buffers etc. */

typedef struct
{
    int                   id;
    filehandle            finput,
                          foutput,
                          fref;

    WBAMR_S16             *in_buf;
    WBAMR_S16             *out_buf;
    WBAMRD_Decoder_Config *amrd_config;

    /* Some service variables */

    pthread_t             tid;          /* Thread ID. Used only if thread is created */
    int                   th_finish;
    int                   framecount;

    int                   th_err;       /* Error value returned by pthread_join()    */
    int                   ltp_err;      /* LTP error value returned by thread        */
    WBAMRD_RET_TYPE       amrd_err;     /* Decoder functions return values           */
    int                   bs_error;     /* Bitstream error                           */

} wbamr_dec_inst;

/*
                                       LOCAL CONSTANTS
*/
const char def_list_file[]      "list/wbamrd_def_test_files";
const char rob_list_file[]      "list/wbamrd_robustness_test_files";

const char *wbamrd_err_msg[14] 
{
    "WBAMRD_OK",
    "WBAMRD_WARNING",
    "WBAMRD_INVALID_MODE",
    "WBAMRD_INIT_ERROR",
    "WBAMRD_MEMALLOC_ERROR",
};

const char progress[]  "-\\|/";      /* For rounding indicator */
/*
                                       LOCAL VARIABLES
*/
static wbamr_dec_inst dec_inst[MAX_DEC_THREADS];
static flist_t        *files_list       NULL;
int                   relocate_test     FALSE; /* Acts as a switch in the aaclc_decoder_engine() */
int                   th_count          0;     /* Used when some info must be printed            */
int                   th_printing       -1;    /* Helps thread to determine it is print. thread  */
pthread_mutex_t io_mutex  PTHREAD_MUTEX_INITIALIZER;

/*
                                       GLOBAL CONSTANTS
*/


/*
                                       GLOBAL VARIABLES
*/


/*
                                   LOCAL FUNCTION PROTOTYPES
*/

/* Please see comments to these functions at their implementation */

int  wbamr_decoder_engine(wbamr_dec_inst *inst);
int  alloc_dec_buffers(wbamr_dec_inst *instance);
int  realloc_dec_memory(wbamr_dec_inst *instance);
void dec_cleanup(wbamr_dec_inst *instance);
int  open_fstreams(wbamr_dec_inst *instance);
int  write_frame(WBAMR_S16 *out_buf, FILE *fptr);
void print_status(void);
int  perform_bitmatch_raw(filehandle *out, filehandle *ref);
int  set_dec_instance(int index, flist_t *list_node);
int  run_decoder_thread(void *instance);

/* test functions */

int nominal_functionality_test();
int reentrance_test();
int relocatability_test();
int robustness_test();
int endurance_test();

/*
                                       LOCAL FUNCTIONS
*/


/**/
/* VT_wbamr_decoder_setup */
/**
@brief  assumes the pre-condition of the test case execution

@param  None

@return On success - return TPASS
        On failure - return the error code
*/
/**/
int VT_wbamr_decoder_setup()
{
    return TPASS;
}

/**/
/* VT_wbamr_decoder_cleanup */
/**
@brief  assumes the post-condition of the test case execution

@param  None

@return On success - return TPASS
        On failure - return the error code
*/
/**/
int VT_wbamr_decoder_cleanup()
{
    return TPASS;
}

/**/
/* VT_wbamr_decoder_test */
/**
@brief  Reads list of files (input, output and reference). Executes test specified by 'testcase'
        variable.

@param  Input:  testcase - testcase id of the test according to the test plan
                listfile - pointer to the name of list file
        Output: None


@return On success - return TPASS
        On failure - return the error code
*/
/**/
int VT_wbamr_decoder_test(int testcase, char *listfile)
{
    int rv  TFAIL;
    const char * flist  0;

    /*
     * Clear file list, open appropriate listfile (specified by input variable or default) and
     * and read its contents into the list. One test case needs its own file list.
     */

    if (files_list)
        delete_list(files_list);
    if (!listfile)
    {
        if (testcase  ROBUSTNESS)
            flist  (const char*)rob_list_file;
        else
            flist  (const char*)def_list_file;
    }
    else
        flist  listfile;

    if (!read_cfg(flist, &files_list))
        return rv;

    tst_resm(TINFO, "List of files will be taken from %s", flist);

    switch (testcase)
    {
 case NOMINAL_FUNCTIONALITY:
     tst_resm(TINFO, "Nominal functionality test");
     rv  nominal_functionality_test();
        break;

 case REENTRANCE:
     tst_resm(TINFO, "Reentrance test");
     rv  reentrance_test();
     break;

 case RELOCATABILITY:
     tst_resm(TINFO, "Relocatability test");
     rv  relocatability_test();
     break;

 case ROBUSTNESS:
     tst_resm(TINFO, "Robustness test");
     rv  robustness_test();
     break;

 default:
     tst_resm(TFAIL, "Wrong test case!!");
     break;
    }

    return rv;
}


WBAMR_S16 Read_serial( wbamr_dec_inst * inst, WBAMR_U8 bitstreamformat )
{
    FILE * fp  inst->finput.fptr;
    WBAMR_S16 *prms  inst->in_buf;
    WBAMR_S16 n, n1, type_of_frame_type, coding_mode, datalen, i;
    WBAMR_U8 toc, q, temp, *packet_ptr, packet[64];
    WBAMR_S16 frame_type,mode0,*prms_ptr;


    prms_ptr  prms;
    prms+2;

    if(bitstreamformat  0)    /* default file format */
    {
        n  (WBAMR_S16) fread(&type_of_frame_type, sizeof(WBAMR_S16), 1, fp);
        n  (WBAMR_S16) (n + fread(&frame_type, sizeof(WBAMR_S16), 1, fp));
        n  (WBAMR_S16) (n + fread(&mode, sizeof(WBAMR_S16), 1, fp));

        if(n!3)return 0;

        coding_mode  mode;
        if(mode < 0 || mode > NUM_OF_MODES-1)
        {

            inst->bs_error  TRUE;
            return 0;
        }

        if (type_of_frame_type  TX_FRAME_TYPE)
        {
            switch (frame_type)
            {
                case TX_SPEECH:
                    frame_type  RX_SPEECH_GOOD;
                    break;
                case TX_SID_FIRST:
                    frame_type  RX_SID_FIRST;
                    break;
                case TX_SID_UPDATE:
                    frame_type  RX_SID_UPDATE;
                    break;
                case TX_NO_DATA:
                    frame_type  RX_NO_DATA;
                    break;
            }
        } else if (type_of_frame_type ! RX_FRAME_TYPE)
        {

            inst->bs_error  1;
            return 0;
        }

        if ((frame_type  RX_SID_FIRST) | (frame_type  RX_SID_UPDATE) | (frame_type  RX_NO_DATA) | (frame_type  RX_SID_BAD))
        {
            coding_mode  MRDTX;
        }
        n  (WBAMR_S16) fread(prms, sizeof(WBAMR_S16), nb_of_bits[coding_mode], fp);

        if (n ! nb_of_bits[coding_mode])
            n  0;

        prms_ptr[0]  frame_type;
        prms_ptr[1]  mode;
        return (n);
    }
    else
    {
        if (bitstreamformat  1)  /* ITU file format */
        {
            n  (WBAMR_S16) fread(&type_of_frame_type, sizeof(WBAMR_S16), 1, fp);
            n  (WBAMR_S16)(n+fread(&datalen, sizeof(WBAMR_S16), 1, fp));
            if(n!2)return 0;

            if (datalen ! 0)
            {
                coding_mode  -1;
                for(iNUM_OF_MODES-1; i>0; i--)
                {
                    if(datalen  nb_of_bits[i])
                    {
                        coding_mode  i;
                    }
                }
                if(coding_mode  -1)
                {

                    inst->bs_error  TRUE;
                    return 0;
                }
            }

            n1  fread(prms, sizeof(WBAMR_S16), datalen, fp);
            prms_ptr[0]  type_of_frame_type;
            prms_ptr[1]  datalen;
            return(n);

        }
        else if(bitstreamformat2)   /* MIME/storage file format */
        {
            /* read ToC byte, return immediately if no more data available */
            if (fread(&toc, sizeof(WBAMR_U8), 1, fp)  0)
            {
                return 0;
            }

            /* extract q and mode from ToC */
            q   (toc >> 2) & 0x01;
            mode  (toc >> 3) & 0x0F;

            /* read speech bits, return with empty frame if mismatch between mode info and available data */
            if ((WBAMR_S16)fread(packet, sizeof(WBAMR_U8), packed_size[mode], fp) ! packed_size[mode])
            {
                return 0;
            }

            packet_ptr  (WBAMR_U8 *) prms_ptr;


            *packet_ptr++  q;
            *packet_ptr++  (WBAMR_U8) mode;

            for (i  0; i < packed_size[mode]; i++)
            {
                *packet_ptr++  packet[i];
            }

            /* return 1 to indicate succesfully parsed frame */
            return 1;
        }

        /* if IF1 format or IF2 format.. */
        else
        {
            /* read ToC byte, return immediately if no more data available */
            if (fread(&temp, sizeof(WBAMR_U8), 1, fp)  0)
            {
                return 0;
            }

            /* extract mode from ToC */
            mode  (temp >> 4) & 0x0F;  /* frame_type */

            if (bitstreamformat  3)
            {
                /* read speech bits, return with empty frame if mismatch between mode info and available data */
                if ((WBAMR_S16)fread(packet, sizeof(WBAMR_U8), if2_packed_size[mode]-1, fp) ! if2_packed_size[mode]-1)
                {
                    return 0;
                }
            }
            else
            {
                if ((WBAMR_S16)fread(packet, sizeof(WBAMR_U8), if1_packed_size[mode]-1, fp) ! if1_packed_size[mode]-1)
                {
                    return 0;
                }
            }

            packet_ptr  (WBAMR_U8 *) prms_ptr;

            *packet_ptr++  temp;
            if (bitstreamformat  3)
            {
                *packet_ptr++  (WBAMR_U8) mode;

                for (i  0; i < if2_packed_size[mode]-1; i++)
                {
                    *packet_ptr++  packet[i];
                }
            }
            else
            {
                for (i  0; i < if1_packed_size[mode]-1; i++)
                {
                    *packet_ptr++  packet[i];
                }
            }

            /* return 1 to indicate succesfully parsed frame */
            return 1;

        }
    }
}


/**/
/* wbamr_decoder_engine */
/**
@brief  Engine of the decoder. Performs decoding of bitstream.
 Also this function saves decoder result, if needed.
 This method is compatible with threads.

@param  Input:  inst - pointer to the structure holding buffers, decoder config structure etc.
        Output: None

@return On success - return TPASS
        On failure - return the error code
*/
/**/
int wbamr_decoder_engine(wbamr_dec_inst *inst)
{
    WBAMRD_Decoder_Config *conf;        /* Pointer to simplify references */
    int ret  TPASS;

    if (!inst)                  /* Error: NULL pointer */
    {
        tst_resm(TFAIL, "ERROR in wbamr_decoder_engine(): invalid parameter");
        return TFAIL;
    }

    /* Allocate memory for decoder parameter structure, populate known fields */

    inst->amrd_config  (WBAMRD_Decoder_Config *)malloc(sizeof(WBAMRD_Decoder_Config));
    if (!inst->amrd_config)
    {
        tst_resm(TFAIL, "ERROR in wbamr_decoder_engine(): malloc() for decoder config returns %s",
                strerror(errno));
        ret  TFAIL;
    }

    //printf( "thread[%d].input  %s\n", inst->id, inst->finput.fname );
    int i;
    if (ret  TPASS)
    {
        conf  inst->amrd_config;
        conf->wbamrd_decode_info_struct_ptr  NULL;
        for( i  0; i < WBAMR_MAX_NUM_MEM_REQS; ++i )
        {
            conf->wbamrd_mem_info.mem_info_sub[i].wbappd_base_ptr  NULL;
        }
        conf->wbappd_initialized_data_start  BEGIN_WBAMRD_DATA;
        conf->bitstreamformat  0;

        /* Get decoder memory requirements. Info will be placed in the aacd_mem_info field */

        inst->amrd_err  wbamrd_query_dec_mem(conf);
        if (inst->amrd_err ! WBAMRD_OK)
        {
            tst_resm(TFAIL, "ERROR in wbamr_decoder_engine(): wbamrd_query_dec_mem() returns error '%s'",
                    wbamrd_err_msg[inst->amrd_err]);
            ret  TFAIL;
        }
    }

    if (ret  TPASS)
    {
        if (alloc_dec_buffers(inst) ! TRUE)
            ret  TFAIL;
    }

    /*
     * Open input and output streams. Input stream must be opened before wbamrd_decoder_init()
     * will be called, because it calls app_swap_buffers_aac_dec(), which reads from
     * input stream.
     */

    if (ret  TPASS)
    {
        if (open_fstreams(inst) ! TRUE)
            ret  TFAIL;
    }

    if (ret  TPASS)
    {
        inst->amrd_err  wbamrd_decode_init(conf);
        if (inst->amrd_err ! WBAMRD_OK)
        {
            tst_resm(TFAIL, "ERROR in wbamr_decoder_engine(): wbamrd_decode_init() returns error '%s'",
                    wbamrd_err_msg[inst->amrd_err]);
            ret  TFAIL;
        }
    }

    /* If an error was occured in one of the initialization steps, cleanup instance and exit */

    if (ret ! TPASS)
    {
        dec_cleanup(inst);
        return ret;
    }

    if (th_printing  -1)
        th_printing  inst->id;

#ifdef DEBUG_TEST
    if (th_count  1)
        printf("\nInput file:  %s\n", inst->finput.fname);
#endif

    /*
     * Main decoding cycle continues while end of input file was reached.
     * When a regular frame was decoded, output buffer contents are written into the
     * output file and next frame's turn begins
     */
    conf->bitstreamformat  0;
    while ( (ret  TPASS) && (Read_serial(inst, conf->bitstreamformat) ! 0) )
    {
        conf->bitstreamformat  0;
        inst->amrd_err  wbamrd_decode_frame(conf, inst->in_buf, inst->out_buf);
        inst->framecount++;

        if (inst->amrd_err  WBAMRD_OK)
        {
            #if 0
            if (th_printing  inst->id)
                print_status();
            #endif

            /* If output file name is set in config list file, write to it */

            if (inst->foutput.fptr)
                if (write_frame(inst->out_buf, inst->foutput.fptr)  FALSE)
                    ret  TFAIL;

            /* In relocatability test realloc decoder memory every 'RELOCATE_CYCLE'-th frame*/
            /*
            if ( (!(inst->framecount % RELOCATE_CYCLE)) && relocate_test )
            {
                if (realloc_dec_memory(inst) ! TRUE )
                    ret  TFAIL;
                else
                {
                    inst->amrd_err  wbamrd_decode_init(conf);
                    if (inst->amrd_err ! WBAMRD_OK)
                    {
                        tst_resm(TFAIL, "ERROR in wbamr_decoder_engine(): wbamrd_decode_init() "
                                "returns error '%s'", wbamrd_err_msg[inst->amrd_err]);
                        ret  TFAIL;
                    }
                }
            }*/
        }
        else
            tst_resm(TWARN, "Invalid frame [%d]: %s", inst->framecount, wbamrd_err_msg[inst->amrd_err]);
    }

    dec_cleanup(inst);
    return ret;
}

/**/
/* print_status */
/**
@brief  Prints number of frames decoded for all running threads.

@param  Input:  None
        Output: None

@return None
*/
/**/
void print_status(void)
{
    int i;
    wbamr_dec_inst *inst;

    for (i  0; i < th_count; i++)
    {
        inst  &dec_inst[i];
        printf("th[%d]-", inst->id + 1);
        if (inst->th_finish) printf("ended ");
        else printf("frames");
        printf("[%3d] ", inst->framecount);
    }
    printf("%c\r", progress[inst->framecount % (sizeof(progress) - 1)]);
    fflush(stdout);
    return;
}

/**/
/* open_fstreams */
/**
@brief  Opens streams associated with input and output files.

@param  Input:  instance - pointer to the structure holding buffers, decoder config structure etc.
        Output: None

@return On success - return TRUE
        On failure - return FALSE
*/
/**/
int open_fstreams(wbamr_dec_inst *instance)
{
    if (!instance)  /* Error: NULL pointer */
    {
        tst_resm(TFAIL, "ERROR in open_fstreams(): invalid parameter");
        return FALSE;
    }

    if ((instance->finput.fptr  fopen(instance->finput.fname, "r"))  NULL)
    {
        tst_resm(TFAIL, "ERROR in open_fstreams(): fopen() for input file %s returns %s",
                instance->finput.fname, strerror(errno));
        return FALSE;
    }
    if (instance->foutput.fname ! NULL)
    {
        if ((instance->foutput.fptr  fopen(instance->foutput.fname, "w"))  NULL)
        {
            tst_resm(TFAIL, "ERROR in open_fstreams(): fopen() for output file %s returns %s",
                    instance->foutput.fname, strerror(errno));
            return FALSE;
        }
    }

    return TRUE;
}

/**/
/* alloc_dec_buffers */
/**
@brief  Allocates memory for:
            all chunks requested by decoder (as returned by wbamrd_query_dec_mem());
            application input buffer.

@param  Input:  instance - pointer to the structure holding buffers, decoder config structure etc.
        Output: None

@return On success - return TRUE
        On failure - return FALSE
*/
/**/
int alloc_dec_buffers(wbamr_dec_inst *instance)
{
    int chunk_cnt;
    int i;
    WBAMRD_Decoder_Config *conf;
    WBAMRD_Mem_Alloc_Info_Sub *mem;
    int ret  TRUE;

    if (!instance)
    {
        tst_resm(TFAIL, "ERROR in alloc_dec_buffers(): invalid parameter");
        return FALSE;
    }
    conf  instance->amrd_config;

    /* Allocate memory for all required chunks and buffers */

    chunk_cnt  conf->wbamrd_mem_info.wbamrd_num_reqs;
    for (i  0; i < chunk_cnt; i++)
    {
        mem  &(conf->wbamrd_mem_info.mem_info_sub[i]);
        mem->wbappd_base_ptr  malloc(mem->wbamrd_size);
        if (!mem->wbappd_base_ptr)
        {
            tst_resm(TFAIL, "ERROR in alloc_dec_buffers(): malloc() for chunk %d returns %s",
                    i, strerror(errno));
            ret  FALSE;
        }
    }

    if (ret)
    {
        instance->in_buf  (WBAMR_S16 *)malloc(WBAMR_SERIAL_FRAMESIZE * sizeof(WBAMR_S16));
        if (!instance->in_buf)
        {
            tst_resm(TFAIL, "ERROR in alloc_dec_buffers(): malloc() for input buffer returns %s",
                    strerror(errno));
            ret  FALSE;
        }
    }
    if (ret)
    {
        instance->out_buf  (WBAMR_S16 *)malloc(WBAMR_L_FRAME * sizeof(WBAMR_S16));
        if (!instance->out_buf)
        {
            tst_resm(TFAIL, "ERROR in alloc_dec_buffers(): malloc() for output buffer returns %s",
                    strerror(errno));
            ret  FALSE;
        }
    }

    return ret;
}

/**/
/* dec_cleanup */
/**
@brief  Releases file streams allocated by open_fstreams().
        Frees memory allocated by alloc_dec_buffers().

@param  Input:  instance - pointer to the structure holding buffers, decoder config structure etc.
        Output: None

@return None
*/
/**/
void dec_cleanup(wbamr_dec_inst *instance)
{
    int i;
    int nr;
    WBAMRD_Decoder_Config *conf;
    WBAMRD_Mem_Alloc_Info_Sub *mem;

    if (!instance)
    {
        tst_resm(TFAIL, "ERROR in dec_cleanup(): invalid parameter");
        return;
    }

    if (instance->finput.fptr)
        fclose(instance->finput.fptr);
    if (instance->foutput.fptr)
        fclose(instance->foutput.fptr);

    conf  instance->amrd_config;
    if(instance->in_buf)
        free(instance->in_buf);
    if(instance->out_buf)
        free(instance->out_buf);

    nr  conf->wbamrd_mem_info.wbamrd_num_reqs;
    for (i  0; i < nr; i++)
    {
        mem  &(conf->wbamrd_mem_info.mem_info_sub[i]);
        if (mem->wbappd_base_ptr)
            free(mem->wbappd_base_ptr);
    }

    if (conf)
        free(conf);

    return;
}

/**/
/* realloc_dec_memory */
/**
@brief  Frees decoder memory and allocates it again, but in other place.

@param  Input:  instance - pointer to the structure holding buffers, decoder config structure etc.
        Output: None

@return On success - return TRUE
        On failure - return FALSE
*/
/**/
int realloc_dec_memory(wbamr_dec_inst *instance)
{
    WBAMRD_Decoder_Config     *conf;
    WBAMRD_Mem_Alloc_Info_Sub *mem;
    int  i;
    int  nr;
    void *barrier_ptr;
    int  ret  TRUE;

    if (!instance)
    {
        tst_resm(TFAIL, "ERROR in realloc_dec_memory(): invalid parameter");
        return FALSE;
    }
    conf  instance->amrd_config;

    /* Deallocate all memory chunk and then again allocate them */

    nr  conf->wbamrd_mem_info.wbamrd_num_reqs;
    for (i  0; i < nr; i++)
    {
        mem  &(conf->wbamrd_mem_info.mem_info_sub[i]);
        if (mem->wbappd_base_ptr)
            free(mem->wbappd_base_ptr);
    }

    /* Allocate some memory to be sure that decoder memory will be allocated in other place */

    barrier_ptr  malloc(WBAMR_SERIAL_FRAMESIZE * instance->framecount);
    if (!barrier_ptr)
    {
        tst_resm(TFAIL, "ERROR in realloc_dec_memory(): malloc() for barrier_ptr returns %s",
                strerror(errno));
        ret  FALSE;
    }
    else
    {
        for (i  0; i < nr; i++)
        {
            mem  &(conf->wbamrd_mem_info.mem_info_sub[i]);
            mem->wbappd_base_ptr  malloc(mem->wbamrd_size);
            if (!mem->wbappd_base_ptr)
            {
                tst_resm(TFAIL, "ERROR in realloc_dec_memory: malloc() for chunk %d returns %s",
                        i, strerror(errno));
                ret  FALSE;
            }
        }
        free(barrier_ptr);
    }

    return ret;
}

/**/
/* write_frame */
/**
@brief  Builds interlaced multichannel frame from non-interlaced frame and writes it into the
        output stream.

@param  Input:  outbuf   - decoder output buffer
                fptr     - output file stream pointer
        Output: None

@return On success - return TRUE
        On failure - return FALSE
*/
/**/
int write_frame(WBAMR_S16 *out_buf, FILE *fptr)
{
    int ret  FALSE;

    if (!fptr)
    {
        tst_resm(TFAIL, "ERROR in write_frame(): invalid argument");
        return ret;
    }

    fwrite(out_buf, sizeof(WBAMR_S16), WBAMR_L_FRAME, fptr);

    if (ferror(fptr))
    {
        tst_resm(TFAIL, "ERROR in write_frame(): fwrite() returns error %s", strerror(errno));
    }
    else
        ret  TRUE;

    return ret;
}

/**/
/* run_decoder_thread */
/**
@brief  This is a thread function. It changes process priority in case of preemption test and
        runs decoder engine.

@param  instance - void pointer to thread arguments.

@return NULL
*/
/**/
int run_decoder_thread(void *instance)
{
    int i;
    wbamr_dec_inst *inst  (wbamr_dec_inst *)instance;

    inst->ltp_err  wbamr_decoder_engine(inst);
    inst->th_finish  TRUE;

    /*
     * If it was a printing thread, find another working thread and aasign its id to
     * th_printing variable
     */

    if (th_printing  inst->id)
    {
        th_printing  -1;
        for (i  0; i < th_count; i++)
        {
            if (dec_inst[i].th_finish  FALSE)
            {
                th_printing  dec_inst[i].id;
                break;
            }
        }
    }

    /* perform bitmatch */


    /*pthread_exit(NULL);*/

    return  inst->ltp_err;
}

/**/
/* set_dec_instance */
/**
@brief  Sets instance ID and file names.

@param  index     - index of instance that will be used
        list_node - pointer to the current flist_t entry (that stores file names)

@return On success - return TRUE
        On failure - return FALSE
*/
/**/
int set_dec_instance(int index, flist_t *list_node)
{
    if ( (index > MAX_DEC_THREADS) || (!list_node) )
    {
        tst_resm(TFAIL, "ERROR in set_dec_instance(): one of parameters isn't valid");
        return FALSE;
    }

    dec_inst[index].framecount  0;
    dec_inst[index].th_finish  FALSE;
    dec_inst[index].id  index;
    dec_inst[index].finput.fname  list_node->inp_fname; /* fname isn't "n/a": already checked */
    if (strcmp(list_node->out_fname, EMPTY_FILE))
        dec_inst[index].foutput.fname  list_node->out_fname;
    else
        dec_inst[index].foutput.fname  NULL;

    dec_inst[index].foutput.fptr  NULL;

    if (strcmp(list_node->ref_fname, EMPTY_FILE))
        dec_inst[index].fref.fname  list_node->ref_fname;
    else
        dec_inst[index].fref.fname  NULL;

    dec_inst[index].fref.fptr  NULL;

    return TRUE;
}

/**/
/* nominal_functionality_test */
/**
@brief  Testing decoder nominal functionality.

@param  None.

@return On success - return TPASS
        On failure - return the error code
*/
/**/
int nominal_functionality_test()
{
    int i;
    flist_t *node;
    int ret  TPASS;
    wbamr_dec_inst *inst  &dec_inst[0];

    if (!files_list)
    {
        tst_resm(TFAIL, "ERROR in nominal functionality test: list of files is empty");
        return TFAIL;
    }

    /* Check functionality for all entry read from list */

    th_count  1;
    for (node  files_list, i  0; node; node  node->next, i++)
    {
        if (set_dec_instance(0, node)  FALSE) /* Set file names and instance ID */
        {
            ret  TFAIL;
            break;
        }
        if (wbamr_decoder_engine(inst) ! TPASS)
            ret  TFAIL;

        /* perform bitmatch */

        if (inst->fref.fname && inst->foutput.fname)
        {
            if (!perform_bitmatch_raw(&inst->foutput, &inst->fref))
            {
                ret  inst->ltp_err  TFAIL;
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

/**/
/* reentrance_test */
/**
@brief  Reentrance means there should not be any static data or any global
 variables used in the code. Test this ability.

@param  None.

@return On success - return TPASS
        On failure - return the error code
*/
/**/
int reentrance_test_core(flist_t * head)
{
    int     i;
    flist_t *node;
    int     ret  TPASS;

    if (!head)
    {
        tst_resm(TFAIL, "ERROR in reentrance test: list of files is empty");
        return TFAIL;
    }


    for (node  head, i  0; node && (i < MAX_DEC_THREADS); node  node->next, i++)
    {
        if (set_dec_instance(i, node)  FALSE) /* Set file names and instance ID */
        {
            ret  TFAIL;
            break;
        }

        if (pthread_create(&dec_inst[i].tid, NULL, (void *)&run_decoder_thread,
            (void *)&dec_inst[i]))
        {
            tst_resm(TFAIL, "ERROR: cannot create thread %d: %s", i + 1, strerror(errno));
            ret  TFAIL;
            break;
        }
    }

    if (ret  TPASS)
    {
        th_count  i;

        /*
         * Wait till threads are complete before main continues. Unless we
         * wait we run the risk of executing an exit which will terminate
         * the process and all threads before the threads have completed.
         */

        for (i  0; i < MAX_DEC_THREADS; i++)
            dec_inst[i].th_err  pthread_join(dec_inst[i].tid, NULL);

        for (i  0; i < th_count; i++)
        {
            if (dec_inst[i].th_err)
            {
                tst_resm(TFAIL, "Thread %2d was finished with error %s", i + 1, strerror(errno));
                ret  TFAIL;
            }
            else if (dec_inst[i].ltp_err ! TPASS)
            {
                tst_resm(TFAIL, "Thread %2d was finished with UNsuccessful result", i + 1);
                ret  dec_inst[i].ltp_err;
            }
        }
    }

    return ret;
}

int reentrance_test()
{
    flist_t * head  files_list;
    int rv  TPASS;
    int i;

    while (head)
    {
        for( i  0; i < MAX_DEC_THREADS; ++i )
        {
            memset(&dec_inst[i], 0, sizeof(dec_inst[i]) );
        }
        rv + reentrance_test_core( head );
        for( i  0; i < th_count && head; ++i )
            head  head->next;
    }


    head  files_list;
    while( head )
    {
        if( !compare_files(head->out_fname, head->ref_fname) )
        {
            rv  TFAIL;
            tst_resm( TWARN, "Bitmatch has failed (%s vs %s)", head->out_fname, head->ref_fname );
        }
        else
            tst_resm( TINFO, "Bitmatch has passed (%s vs%s)", head->out_fname, head->ref_fname );
        head  head->next;
    }
    return rv;
}


/**/
/* relocatability_test  */
/**
@brief  Test of decoder code relocatability.

@param  None.

@return On success - return TPASS
        On failure - return the error code
*/
/**/
int relocatability_test()
{
    int i, j;
    flist_t *node;
    int ret  TPASS;
    wbamr_dec_inst *inst  &dec_inst[0];

    if (!files_list)
    {
        tst_resm(TFAIL, "ERROR in nominal functionality test: list of files is empty");
        return TFAIL;
    }

    /* Check functionality for all entry read from list */

    th_count  1;
    for (node  files_list, i  0; node; node  node->next, i++)
    {
        for( j  0; j < 10; ++j )
        {
            if (set_dec_instance(0, node)  FALSE) /* Set file names and instance ID */
            {
                ret  TFAIL;
                break;
            }
            if (wbamr_decoder_engine(inst) ! TPASS)
                ret  TFAIL;

            /* perform bitmatch */

            if (inst->fref.fname && inst->foutput.fname)
            {
                if (!perform_bitmatch_raw(&inst->foutput, &inst->fref))
                {
                    inst->ltp_err  TFAIL;
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

/**/
/* robustness_test  */
/**
@brief  Test of ability adequately react to a bad input bitstream.

@param  None.

@return On success - return TPASS
        On failure - return the error code
*/
/**/
int robustness_test()
{
    int i;
    flist_t *node;
    int ret  TPASS;

    if (!files_list)
    {
        tst_resm(TFAIL, "ERROR in robustness test: list of files is empty");
        return TFAIL;
    }

    /* Check functionality for all entry read from list */

    th_count  1;
    for (node  files_list, i  0; node; node  node->next, i++)
    {
        if (set_dec_instance(0, node)  FALSE) /* Set file names and instance ID */
        {
            ret  TFAIL;
            break;
        }
        if ( ((wbamr_decoder_engine(&dec_inst[0]) ! TPASS) &&
             (dec_inst[0].amrd_err ! WBAMRD_OK)) || dec_inst[0].bs_error )
        {
            tst_resm( TINFO, "Robustness to %s passed", node->inp_fname );
            ret  TPASS;
    }
        else
            tst_resm( TINFO, "Robustness to %s failed", node->inp_fname );
    }
    return ret;
}

/**/
/* mk_entry */
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
/**/
flist_t *mk_entry(const char *inp_fname, const char *out_fname, const char *ref_fname)
{
    flist_t *list  malloc(sizeof(flist_t));

    if (list)
    {
 if ( (strlen(inp_fname) < MAX_STR_LEN) &&
      (strlen(out_fname) < MAX_STR_LEN) &&
      (strlen(ref_fname) < MAX_STR_LEN) )
 {
     strcpy(list->inp_fname, inp_fname);
     strcpy(list->out_fname, out_fname);
     strcpy(list->ref_fname, ref_fname);
 }
        else
            tst_resm(TFAIL, "ERROR in mk_entry(): one of file names too long");
    }
    else
        tst_resm(TFAIL, "ERROR in mk_entry(): malloc() returns %s", strerror(errno));

    return list;
}

/**/
/* delete_list */
/**
@brief  Deletes linked list without recursion.

@param  Input:  list - pointer to the first flist_t entry (list head)
        Output: None

@return None
*/
/**/
void delete_list(flist_t *list)
{
    flist_t *node  list;
    flist_t *next;

    while (node)
    {
        next  node->next;
        free(node);
        node  next;
    }
}

/**/
/* read_cfg */
/**
@brief  Reads list of entries (input, output & reference file names) from file and stores it
        in the linked list flist_t.

@param  Input:  filename  - pointer to the config (list) file name
        Output: pplist    - double pointer to the head of the list that will be created

@return On success - return TRUE
        On failure - return FALSE
*/
/**/
int read_cfg(const char *filename, flist_t **pplist)
{
    FILE    *in;
    char    line[3][MAX_STR_LEN];
    flist_t *node;
    flist_t *flist  NULL;
    int     i       0;
    int     ret     TRUE;

    in  fopen(filename, "r");
    if (in  NULL)
    {
        tst_resm(TFAIL, "ERROR in read_cfg(): cannot open config file: %s",
                strerror(errno));
        return FALSE;
    }

    /*
     * When ret becomes FALSE, it means that malloc() error was occured in mk_entry()
     * and it returned NULL
     */

    while (!feof(in) && (ret ! FALSE) )
    {
        if (fscanf(in, "%s", line[i]) < 0)
            continue;

        if (i  2)
        {
            if (strcmp(line[0], EMPTY_FILE) ! 0) /* No input file - nothing to be do  */
            {
                if (!flist)                       /* First entry will be created       */
                {
                    flist  mk_entry(line[0], line[1], line[2]);
                    node  flist;
                    if (!flist)
                        ret  FALSE;
                }
                else                              /* Next entries in linked list       */
                {
                    node->next  mk_entry(line[0], line[1], line[2]);
                    node  node->next;
                    if (!node)
                        ret  FALSE;
                }
            }
            else
                tst_resm(TFAIL, "ERROR in read_cfg(): input file name is %s",
                        EMPTY_FILE);
        }       /* if (i  2) */
        i++;
        i % 3;
    }

    *pplist  flist;
    return ret;
}

/**/
/**/
int compare_files( const char * fname1, const char * fname2 )
{
    int out, ref;
    struct stat fstat_out, fstat_ref;
    char *fptr_out, *fptr_ref;
    size_t filesize;
    int i;

    if( (out  open(fname1, O_RDONLY)) < 0 )
    {
        return FALSE;
    }
    if ((ref  open(fname2, O_RDONLY)) < 0)
    {
    close(out);
        return FALSE;
    }
    fstat( out, &fstat_out );
    fstat( ref, &fstat_ref );
    if( fstat_out.st_size ! fstat_ref.st_size )
    {
     close(out);
    close(ref);
     return FALSE;
    }
    filesize  fstat_out.st_size;
    fptr_out  (char*)mmap( 0, filesize, PROT_READ, MAP_SHARED, out, 0 );
    if( fptr_out  MAP_FAILED )
    {
    close( out );
     close( ref );
        return FALSE;
    }
    fptr_ref  (char*) mmap(0, filesize, PROT_READ, MAP_SHARED, ref, 0);
    if( fptr_ref  MAP_FAILED )
    {
     close( out );
    close( ref );
     return FALSE;
    }
    close( out );
    close( ref );
    for( i  0; i < filesize; ++i )
    {
    if( *(fptr_ref + i) ! *(fptr_out + i) )
     {
         munmap( fptr_ref, fstat_ref.st_size );
         munmap( fptr_out, fstat_out.st_size );
        return FALSE;
    }
    }
    munmap( fptr_ref, filesize );
    munmap( fptr_out, filesize );
    return TRUE;
}

int perform_bitmatch_raw(filehandle *out, filehandle *ref)
{
    return compare_files( out->fname, ref->fname );
}

/**/
/* perform_bitmatch_raw */
/**
@brief

@param  None

@return
*/
/**/
int perform_bitmatch_raw_old(filehandle *out, filehandle *ref)
{
    unsigned int outval,
                 refval;
    int out_eof  0,
        ref_eof  0;
    int ret  TRUE;

    /* Check parameters and prepare environment */

    if (out->fname && ref->fname)
    {
        if ( (out->fptr  fopen(out->fname, "r"))  NULL )
        {
            tst_resm(TFAIL, "ERROR in perform_bitmatch_raw(): fopen() returns %s", strerror(errno));
            ret  FALSE;
        }
        else if ( (ref->fptr  fopen(ref->fname, "r"))  NULL )
        {
            tst_resm(TFAIL, "ERROR in perform_bitmatch_raw(): fopen() returns %s", strerror(errno));
            ret  FALSE;
        }
    }
    else
        ret  FALSE;

    if (ret)
    {

        /* Begin bitmatch */

        while (ret && !out_eof)
        {
            fread(&outval, 1, sizeof(outval), out->fptr);
            fread(&refval, 1, sizeof(refval), ref->fptr);
            if (outval ! refval)
                ret  FALSE;

            out_eof  feof(out->fptr) ? 1 : 0;
            ref_eof  feof(ref->fptr) ? 1 : 0;
            if (out_eof ! ref_eof)
                ret  FALSE;
        }
    }

    if (out->fptr)
        fclose(out->fptr);
    if (ref->fptr)
        fclose(ref->fptr);

    if (!ret)
        tst_resm(TFAIL, "Bitmatch for %s vs %s is failed", out->fname, ref->fname);

    return ret;
}

#ifdef __cplusplus
}
#endif
