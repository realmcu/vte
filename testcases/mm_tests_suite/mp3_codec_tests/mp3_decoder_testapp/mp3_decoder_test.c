/**/
/**
    @file   mp3_decoder_test.c

    @brief  Test scenario C source template.
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
D.Simakov / smkd001c        12/10/2004      TLSbo43519   Intial version
D.Simakov / smkd001c        11/12/2004      TLSbo47113   Update
D.Simakov / smkd001c        21/07/2005      TLSbo52628   Relocatability test case was added
D.Simakov / smkd001c        19/10/2005      TLSbo57009   Update
D.Simakov                   18/04/2006      TLSbo66228   app_initialized_data_start was removed

Portability: Indicate if this module is portable to other compilers or platforms.
             If not, indicate specific reasons why is it not portable.

*/

/*
                                        INCLUDE FILES
*/
/* Standard Include Files */
#include <errno.h>

/* Harness Specific Include Files. */
#include "test.h"

/* Verification Test Environment Include Files */
#include "mp3_decoder_test.h"

#include "stuff/fileio.h"
#include "stuff/fconf.h"

/*
                                        LOCAL MACROS
*/
#define MAX_CODEC_THREADS 4

#define SAFE_DELETE(p) {if(p){free(p);pNULL;}}

#define NONE_FILE "n/a"

/*
                          LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
*/

typedef struct
{
    /**********************************************
     * Configurable parameters (public interface) *
     **********************************************/

    /** As well as MP3D_decoder_Config::instance_id. */
        unsigned long instance_id;

    /** Input file name. This pointer should be valid. */
        const char *inp_fname;
    /** Output file name. If this field is not NULL codec_engine will create
        output file and write PCM into this file. */
        const char *out_fname;
    /** Reference file name. If this fileld is not NULL codec_engine will perform
        bitmach test */
        const char *ref_fname;

    /**
        Output buffer. This buffer contains a decoded PCM data. Output buffer will
        be filled by codec_engine.
    */
        short   output_buffer[2 * 4 * MP3D_FRAME_SIZE];

    /**
        Input buffer. This buffer contains a mp3 data. Input buffer will be filled in
        the app_swap_buffers function.
    */
        short   input_buffer[MP3D_INPUT_BUFFER_SIZE];

    /**
        Reference buffer. This buffer contains a reference data for the bitmach test.
        Memory will be allocated in the codec_engine if the ref_filename will be valid.
    */
        unsigned char *ref_buffer;

    /** Count for endurance test. */
        int     endurance_count;

    /*******************************************
     * Internal parameters (private interface) *
     *******************************************/

    /** This field contains a result of the last mp3d call. */
        MP3D_RET_TYPE last_mp3d_error;

        unsigned long num_out_iter;
        int     decoded_frames;
        int     num_frames;     /* -1 dont use this mach */

    /** Thread id. */
        pthread_t tid;

    /** Decoder parameters and memory information structures. */
        MP3D_Decode_Config *mp3d_config;

    /**
        Input file stream. This stream will be opened in the codec_engine.
        After that this pointer will be passed into app_swap_buffers function
        through this structure.
    */
        file_buf_t *in_filebuf;
        FILE   *out_stream;

        int     eof_flag;
        int     thread_finished;
        int     ltp_retval;

} mp3_decoder_thread_t;

/*
                                       LOCAL CONSTANTS
*/


/*
                                       LOCAL VARIABLES
*/
static mp3_decoder_thread_t codec_thread[MAX_CODEC_THREADS];
static int test_iter  DEFAULT_ITERATIONS;      /* default iteration is hard coded */
static char progress[]  "-\\|/";       /* console progress bar */
static flist_t *file_list  NULL;       /* list of input/output/reference files */
static int num_threads  0;     /* the number of threads for re-entrance & pre-emption */
static int set_priors  FALSE;  /* TRUE means that run_codec shall call nice(XXX) */
static int focus_thread  -1;   /* focus thread is a thread that prints himself statistics and
                                 * statistics of another threads */

/*
                                       GLOBAL CONSTANTS
*/


/*
                                       GLOBAL VARIABLES
*/

/*
                                   LOCAL FUNCTION PROTOTYPES
*/
int     codec_engine(void *ptr);

/* helper functions */
int     run_codec_in_loop(void *ptr);
int     run_codec_on_timer(void *ptr);
void    timer_handler(int signum);
void    hogcpu(void);

/* test functions */
int     nominal_functionality_test(void);
int     endurance_test(void);
int     re_entrance_test(void);
int     pre_emption_test(void);
int     load_envirounment_test(void);
int     robustness_test(void);
int     relocatability_test(void);

void    reset_decoder_instance(unsigned long instance_id);
int     perform_bitmach_hex(const char *out_fname, const char *ref_fname, int bits);
int     perform_bitmach_raw(const char *out_fname, const char *ref_fname, int bits);
int     perform_bitmach_dif32(const char *out_fname, const char *ref_fname, int bits);

/* mp3 decoder functions */
MP3D_INT8 app_swap_buffers(MP3D_INT16 ** new_buf_ptr, MP3D_INT32 * new_buf_len,
                           MP3D_Decode_Config * dec_config);

const char *str_mp3d_error(MP3D_RET_TYPE ret);
void    processing_pcm_frame(mp3_decoder_thread_t * decoder_thread,
                             MP3D_Decode_Params * mp3d_dec_params);

/*
                                       LOCAL FUNCTIONS
*/


/**/
/* VT_mp3_decoder_setup */
/**
@brief  assumes the pre-condition of the test case execution

@param  None

@return On success - return TPASS
        On failure - return the error code
*/
/**/
int VT_mp3_decoder_setup(void)
{
        int     rv  TFAIL;

        rv  TPASS;
        return rv;
}


/**/
/* VT_mp3d_ecoder_cleanup */
/**
@brief  assumes the post-condition of the test case execution

@param  None

@return On success - return TPASS
        On failure - return the error code
*/
/**/
int VT_mp3_decoder_cleanup(void)
{
        int     rv  TFAIL;

        int     i;

        /* close all file descriptors */
        for (i  0; i < MAX_CODEC_THREADS; ++i)
        {
                /* if( codec_thread[i].in_filebuf ) { f_close( codec_thread[i].in_filebuf );
                 * codec_thread[i].in_filebuf  NULL; } */
                if (codec_thread[i].out_stream)
                {
                        fclose(codec_thread[i].out_stream);
                        codec_thread[i].out_stream  NULL;
                }
        }

        if (file_list)
                delete_list(file_list);

        rv  TPASS;
        return rv;
}


/**/
/* VT_mp3_decoder_test */
/**
@brief  Template test scenario function

@param  testcase - Testcase id of the test according to the test plan \n
        iter     - Iteration of the loop in case of an endurance/stress test

@return On success - return TPASS
        On failure - return the error code
*/
/**/
int VT_mp3_decoder_test(int testcase, int iter, const char *cfg_file)
{
        int     rv  TFAIL;

        test_iter  iter;

        if (file_list)
                delete_list(file_list);

        if (!read_cfg(cfg_file, &file_list))
        {
                tst_resm(TWARN, "VT_mp3_decoder_test : can't parse %s", cfg_file);
                return rv;
        }

        switch (testcase)
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
                rv  pre_emption_test();
                tst_resm(TINFO, "End pre-emption test");
                break;

        case LOAD_ENVIROUNMENT:
                tst_resm(TINFO, "Load envirounment test");
                rv  load_envirounment_test();
                tst_resm(TINFO, "End load envirounment test");
                break;

        case ROBUSTNESS:
                tst_resm(TINFO, "Robustness test");
                rv  robustness_test();
                tst_resm(TINFO, "End robustness test");
                break;

        case RELOCATABILITY:
                tst_resm(TINFO, "Relocatability test");
                rv  relocatability_test();
                tst_resm(TINFO, "End relocatability test");
                break;

        default:
                tst_resm(TWARN, "Wrong test case");
                break;
        }

        return rv;
}


/**/
/* codec_engine */
/**
@brief  Engine of the codec. The encoding/decoding of a bit-stream should be presented here.
        Also this function has processed enc/dec result, i.e. displayed it for a video data case
        or sounded it for a sound data case.
        This method should be compatible with a threads.

@param  ptr - TBD

@return On success - return TPASS
        On failure - return the error code
*/
/**/
int codec_engine(void *ptr)
{
        int     rv  TFAIL;

        int     i;      /* loop iterator */

        /* Aliases. The purpose of this is fast access only. */
        MP3D_Decode_Config *mp3d_cfg;
        MP3D_Mem_Alloc_Info *mp3d_mem_info;

        MP3D_Decode_Params mp3d_params;

        /* for memory allocation */
        MP3D_Mem_Alloc_Info_Sub *mem_sub;
        int     num_reqs;

        int     prog_cnt  0;

        /* ptr should be valid */
        if (!ptr)
        {
                tst_resm(TWARN, "codec_engine : invalid argument");
                return rv;
        }

        mp3_decoder_thread_t * decoder_thread  (mp3_decoder_thread_t *) ptr;
        decoder_thread->last_mp3d_error  MP3D_OK;

        /* checking for instance_id */
        if (decoder_thread->instance_id > MAX_CODEC_THREADS)
        {
                tst_resm(TWARN, "codec_engine : invalid instance id (%lu)",
                         decoder_thread->instance_id);
                return rv;
        }

        /* Step 1 */

        /* Allocate memory for the MP3D_Decode_Config. */
        decoder_thread->mp3d_config  (MP3D_Decode_Config *) malloc(sizeof(MP3D_Decode_Config));
        if (!decoder_thread->mp3d_config)
        {
                tst_resm(TWARN,
                         "codec_engine : can't allocate memory for the MP3D_Decode_Config object");
                return rv;
        }
        mp3d_cfg  decoder_thread->mp3d_config;

        /* Assign instance id. */
        // mp3d_cfg->app_instance_id  decoder_thread->instance_id;

        /* Fill up the Relocated data position */
        mp3d_cfg->app_out_format  MP3D_24_BIT_OUTPUT;

        /* Swap buffer function pointer initializations */
        mp3d_cfg->app_swap_buf  app_swap_buffers;

        /* Step 2 */

        /* Query memory */
        decoder_thread->last_mp3d_error  mp3d_query_dec_mem(mp3d_cfg);
        if (decoder_thread->last_mp3d_error ! MP3D_OK)
        {
                tst_resm(TWARN, "codec_engine : mp3d_query_dec_mem fails (%s)",
                         str_mp3d_error(decoder_thread->last_mp3d_error));
                return rv;
        }
        mp3d_mem_info  &mp3d_cfg->mp3d_mem_info;

        /* Allocate memory */
        /* mem_type is ignored yet */
        num_reqs  mp3d_mem_info->mp3d_num_reqs;
        for (i  0; i < num_reqs; ++i)
        {
                mem_sub  &mp3d_mem_info->mem_info_sub[i];
                mem_sub->app_base_ptr  malloc(mem_sub->mp3d_size);
                if (!mem_sub->app_base_ptr)
                {
                        /* Dont worry about memory losing. All memory will be deallocated in the
                         * VT_mp3_deocder_cleanup. */
                        tst_resm(TWARN, "codec_engine : can't allocate required memory");
                        return rv;
                }
        }

        /* Step 3 */
        /* Step 4 */

        /* Initialization routine */
        decoder_thread->last_mp3d_error 
            mp3d_decode_init(mp3d_cfg, decoder_thread->input_buffer,
                             sizeof(decoder_thread->input_buffer));
        if (decoder_thread->last_mp3d_error ! MP3D_OK)
        {
                tst_resm(TWARN, "codec_engine : mp3d_decode_init fails (%s)",
                         str_mp3d_error(decoder_thread->last_mp3d_error));
                return rv;
        }

        /* ///////////////////////////////////////////////////////// */
        /* file/io */
        decoder_thread->in_filebuf  f_open(decoder_thread->inp_fname);
        if (!decoder_thread->in_filebuf || !f_is_open(decoder_thread->in_filebuf))
        {
                tst_resm(TWARN, "codec_engine : can't open file %s", decoder_thread->inp_fname);
                return TFAIL;
        }

#ifndef SHOULD_WORK_AS_DESIGNED
    /****** SOME MAGIC CODE*********/
        fflush(stdout);
        int     kk;
        unsigned short *src;
        unsigned short *dest  (unsigned short *) decoder_thread->in_filebuf->data;
        file_buf_t *src_fbuf  f_copy(decoder_thread->in_filebuf);

        if (src_fbuf)
        {
                src  (unsigned short *) src_fbuf->data;
                for (kk  0; kk < src_fbuf->size / sizeof(short); kk++)
                {
                        dest[kk]  (src[kk] & 0xff00) >> 8;
                        dest[kk] | (src[kk] & 0x00ff) << 8;
                }
                f_close(src_fbuf);
        }
    /************************/
#endif                          /* SHOULD_WORK_AS_DESIGNED */

        if (decoder_thread->out_fname)
        {
                decoder_thread->out_stream  fopen(decoder_thread->out_fname, "wb");
                if (!decoder_thread->out_stream)
                {
                        tst_resm(TWARN, "codec_engine : can't open create %s",
                                 decoder_thread->out_fname);
                        return rv;
                }
        }
        else
                decoder_thread->out_stream  NULL;

        /* Step 7 */

        // decoder_thread->thread_ready  TRUE;
        if (focus_thread  -1)
                focus_thread  decoder_thread->instance_id;

        /* Main decoding loop (frame by frame) */
        while (TRUE)
        {
                decoder_thread->last_mp3d_error  mp3d_decode_frame(mp3d_cfg, &mp3d_params,
                                                                    (MP3D_INT32 *) decoder_thread->
                                                                    output_buffer);
#ifdef SHOULD_WORK_AS_DESIGNED
                if (decoder_thread->last_mp3d_error  MP3D_END_OF_STREAM)
                        break;  /* Reached the end of bitstream */
#else
                if (decoder_thread->eof_flag)
                        break;
#endif                          /* SHOULD_WORK_AS_DESIGNED */

                if (decoder_thread->last_mp3d_error ! MP3D_OK)
                {
                        return TFAIL;
                        // continue;
                }
                else
                        decoder_thread->decoded_frames++;

                /* if( decoder_thread->decoded_frames  1 && num_threads  0 ) tst_resm( TINFO,
                 * "sampling_freq  %d num_channels  %d mp3d_frame_size  %d",
                 * mp3d_params.mp3d_sampling_freq, mp3d_params.mp3d_num_channels,
                 * mp3d_params.mp3d_frame_size ); */

                if (num_threads && decoder_thread->instance_id  focus_thread)
                {
                        int     k;
                        mp3_decoder_thread_t *dec_thread;

                        for (k  0; k < num_threads; k++)
                        {
                                dec_thread  &codec_thread[k];
                                printf("-t[%lu] frames[%d] ", dec_thread->instance_id,
                                       dec_thread->decoded_frames);
                        }
                        printf("%c\r", progress[(prog_cnt++) % (sizeof(progress) - 1)]);
                }
                else if (num_threads  0)
                {
                        printf("\r\tFrames decoded: [%4d] %c",
                               decoder_thread->decoded_frames,
                               progress[(prog_cnt++) % (sizeof(progress) - 1)]);
                }
                fflush(stdout);

                processing_pcm_frame(decoder_thread, &mp3d_params);
        }

        if (num_threads  0)
                printf("\n");

        f_close(decoder_thread->in_filebuf);
        if (decoder_thread->out_stream)
        {
                fclose(decoder_thread->out_stream);
                decoder_thread->out_stream  NULL;
        }

        rv  TPASS;
        return rv;
}

/**/
/* run_codec */
/**
@brief  This method called by a special codec thread decode/encode in loop the same bitstreams.

@param  ptr - TBD

@return On success - return TPASS
        On failure - return the error code
*/
/**/
int run_codec(void *ptr)
{
        int     i,
                rv  TFAIL,
            nice_inc;

        if (!ptr)
                return rv;

        mp3_decoder_thread_t *dec_thread  (mp3_decoder_thread_t *) ptr;

        /* for preemption case */
        if (set_priors)
        {
                nice_inc  rand() % 40;

                if (nice(nice_inc) < 0)
                {
                        tst_resm(TWARN, "run_codec : nice(%d) failed", nice_inc);
                }
                else
                {
                        // printf( "[%d]nice(%d)\n", dec_thread->instance_id, nice_inc );
                }
        }

        dec_thread->ltp_retval  codec_engine(ptr);

        dec_thread->thread_finished  TRUE;
        if (dec_thread->instance_id  focus_thread)
        {
                for (i  0; i < num_threads; ++i)
                {
                        if (!codec_thread[i].thread_finished)
                        {
                                focus_thread  codec_thread[i].instance_id;
                                break;
                        }
                }
        }

        return dec_thread->ltp_retval;
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
@brief  Testing of a nominal functionality of a encoder/decoder.

@param  None.

@return On success - return TPASS
        On failure - return the error code
*/
/**/
int nominal_functionality_test(void)
{
        flist_t *node;
        int     i;
        int     rv  TPASS;
        mp3_decoder_thread_t *dec_thread  &0[codec_thread];

        int     api_pass;
        int     bitmach_pass;
        int     num_frames_mach_pass;

        for (node  file_list, i  0; node; node  node->next, ++i)
        {
                reset_decoder_instance(0);

#ifdef DEBUG_TEST
                printf("\n\t----- decoding info -------\n"
                       "\tcount:\t    %d\n"
                       "\tinp_file:   %s\n"
                       "\tout_file:   %s\n"
                       "\tref_file:   %s\n", i, node->inp_fname, node->out_fname, node->ref_fname);
                if (node->num_frames > 0)
                        printf("\tnum_frames: %d\n", node->num_frames);
#endif
                tst_resm(TINFO, "input file: %s", node->inp_fname);

                // printf( "\n" );

                dec_thread->inp_fname 
                    strcmp(node->inp_fname, NONE_FILE) ! 0 ? node->inp_fname : NULL;
                dec_thread->out_fname 
                    strcmp(node->out_fname, NONE_FILE) ! 0 ? node->out_fname : NULL;
                dec_thread->ref_fname 
                    strcmp(node->ref_fname, NONE_FILE) ! 0 ? node->ref_fname : NULL;
                dec_thread->num_frames  node->num_frames;

                api_pass  bitmach_pass  num_frames_mach_pass  TRUE;

                if (dec_thread->ref_fname)
                {
                        FILE   *tst  fopen(dec_thread->ref_fname, "r");

                        if (tst)
                                fclose(tst);
                        else
                                dec_thread->ref_fname  NULL;
                }


                /* call decoder engine */
                api_pass  codec_engine(&codec_thread[0])  TPASS ? TRUE : FALSE;

                /* perform bitmach here... */
                if (api_pass && dec_thread->ref_fname && dec_thread->out_fname)
                {
                        bitmach_pass 
                            perform_bitmach_dif32(dec_thread->ref_fname, dec_thread->out_fname, 15);
                }

                /* perform num_frames match here... */
                // if( node->num_frames > 0 || dec_thread->decoded_frames  0 )
                // num_frames_mach_pass  ( node->num_frames  dec_thread->decoded_frames );

                /* Print some information ...  */

                if (!num_frames_mach_pass)
                        tst_resm(TWARN, "Too small frames is decoded!!!");      /* ifdef PRINT */

                if (!bitmach_pass && dec_thread->ref_fname && api_pass)
                        tst_resm(TFAIL, "bitmach failed");
                else if (dec_thread->ref_fname && api_pass)
                        tst_resm(TINFO, "bitmach passed");

                rv + (api_pass && bitmach_pass && num_frames_mach_pass) ? TPASS : TFAIL;
        }

        return rv;
}

int relocatability_test(void)
{
        flist_t *node;
        int     i,
                j;
        int     rv  TPASS;
        mp3_decoder_thread_t *dec_thread  &0[codec_thread];

        int     api_pass;
        int     bitmach_pass;
        int     num_frames_mach_pass;


        for (node  file_list, i  0; node; node  node->next, ++i)
        {
                for (j  0; j < test_iter; ++j)
                {
                        reset_decoder_instance(0);

                        tst_resm(TINFO, "input file: %s", node->inp_fname);

                        dec_thread->inp_fname 
                            strcmp(node->inp_fname, NONE_FILE) ! 0 ? node->inp_fname : NULL;
                        dec_thread->out_fname 
                            strcmp(node->out_fname, NONE_FILE) ! 0 ? node->out_fname : NULL;
                        dec_thread->ref_fname 
                            strcmp(node->ref_fname, NONE_FILE) ! 0 ? node->ref_fname : NULL;
                        dec_thread->num_frames  node->num_frames;

                        if (dec_thread->ref_fname)
                        {
                                FILE   *tst  fopen(dec_thread->ref_fname, "r");

                                if (tst)
                                        fclose(tst);
                                else
                                        dec_thread->ref_fname  NULL;
                        }

                        api_pass  bitmach_pass  num_frames_mach_pass  TRUE;

                        /* call decoder engine */
                        api_pass  codec_engine(&codec_thread[0])  TPASS ? TRUE : FALSE;

                        /* perform bitmach here... */
                        if (dec_thread->ref_fname && dec_thread->out_fname && api_pass)
                        {
                                bitmach_pass 
                                    perform_bitmach_dif32(dec_thread->ref_fname,
                                                          dec_thread->out_fname, 15);
                        }

                        /* perform num_frames match here... */
                        // if( node->num_frames > 0 || dec_thread->decoded_frames  0 )
                        // num_frames_mach_pass  ( node->num_frames  dec_thread->decoded_frames );
                        //

                        /* Print some information ...  */

                        if (!num_frames_mach_pass)
                                tst_resm(TWARN, "Too small frames is decoded!!!");      /* ifdef
                                                                                         * PRINT */

                        if (!bitmach_pass && dec_thread->ref_fname && api_pass)
                                tst_resm(TFAIL, "bitmach failed");
                        else if (dec_thread->ref_fname && api_pass)
                                tst_resm(TINFO, "bitmach passed");

                        rv + (api_pass && bitmach_pass && num_frames_mach_pass) ? TPASS : TFAIL;

                        tst_resm(TINFO, "Data memory was relocated");
                }
        }

        return rv;
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
        int     i;
        int     rv  TPASS;

        for (i  0; i < test_iter; ++i)
        {
                tst_resm(TINFO, "Endurance iter: %4d", i);
                rv + nominal_functionality_test();
        }

        return rv;
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
        flist_t *node;
        int     i;
        int     rv  TPASS;
        mp3_decoder_thread_t *dec_thread  &0[codec_thread];

        fflush(stdout);

        for (node  file_list, i  0; node && i < MAX_CODEC_THREADS; node  node->next, ++i)
        {
                reset_decoder_instance(i);
                dec_thread  &i[codec_thread];

                dec_thread->inp_fname 
                    strcmp(node->inp_fname, NONE_FILE) ! 0 ? node->inp_fname : NULL;
                dec_thread->out_fname 
                    strcmp(node->out_fname, NONE_FILE) ! 0 ? node->out_fname : NULL;
                dec_thread->ref_fname 
                    strcmp(node->ref_fname, NONE_FILE) ! 0 ? node->ref_fname : NULL;
                if (dec_thread->ref_fname)
                {
                        FILE   *tst  fopen(dec_thread->ref_fname, "r");

                        if (tst)
                                fclose(tst);
                        else
                                dec_thread->ref_fname  NULL;
                }

                if (pthread_create(&dec_thread->tid, NULL, (void *) &run_codec, dec_thread))
                {
                        tst_resm(TWARN, "re_entrance_test : error creating thread %d", i);
                        return TFAIL;
                }
        }

        num_threads  i;
        for (i  0; i < num_threads; ++i)
        {
                dec_thread  &i[codec_thread];
                pthread_join(dec_thread->tid, NULL);
        }

        printf("\n");

        /* perform bitmach here... */

        for (i  0; i < num_threads; ++i)
        {
                dec_thread  &i[codec_thread];
                rv + dec_thread->ltp_retval;
        }

        return rv;
}


/**/
/* pre_emption_test */
/**
@brief  The codec should function correctly in a pre-emptive environment. Test this ability.

@param  None.

@return On success - return TPASS
        On failure - return the error code
*/
/**/
int pre_emption_test(void)
{
        set_priors  TRUE;
        return re_entrance_test();
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

        switch (pid  fork())
        {
        case -1:
                tst_resm(TWARN, "load_envirounment_test : fork failed");
                return rv;
        case 0:        /* child -- finish straight away */
                hogcpu();
        default:       /* parent */
                sleep(10);
                retval  nominal_functionality_test();

                /* kill child process once decode/encode loop has ended */
                if (kill(pid, SIGKILL) ! 0)
                {
                        tst_resm(TWARN, "load_envirounment_test : Kill(SIGKILL) error");
                        return rv;
                }
                if (retval  TPASS)
                        rv  TPASS;
        }

        return rv;
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
int robustness_test(void)
{
        flist_t *node;
        int     i;
        int     rv  TPASS;
        mp3_decoder_thread_t *dec_thread  &0[codec_thread];

        int     api_pass;
        int     bitmach_pass;
        int     num_frames_mach_pass;

        for (node  file_list, i  0; node; node  node->next, ++i)
        {
                reset_decoder_instance(0);

                tst_resm(TINFO, "input file: %s", node->inp_fname);


                dec_thread->inp_fname 
                    strcmp(node->inp_fname, NONE_FILE) ! 0 ? node->inp_fname : NULL;
                dec_thread->out_fname 
                    strcmp(node->out_fname, NONE_FILE) ! 0 ? node->out_fname : NULL;
                dec_thread->ref_fname 
                    strcmp(node->ref_fname, NONE_FILE) ! 0 ? node->ref_fname : NULL;
                dec_thread->num_frames  node->num_frames;

                api_pass  bitmach_pass  num_frames_mach_pass  TRUE;


                /* call decoder engine */
                api_pass  codec_engine(&codec_thread[0])  TPASS ? TRUE : FALSE;
                if (!api_pass)
                        tst_resm(TINFO, "Robustness to %s passed", dec_thread->inp_fname);
                else
                        tst_resm(TFAIL, "Robustness to %s failed", dec_thread->inp_fname);

                /* Print some information ...  */

                rv + (!api_pass && bitmach_pass && num_frames_mach_pass) ? TPASS : TFAIL;
        }

        return rv;
}

/**/
/* reset_decoder_instance */
/**
@brief Reset decoder instance

@param  None

@return On success - return TPASS
        On failure - return the error code
*/
/**/
void reset_decoder_instance(unsigned long instance_id)
{
        mp3_decoder_thread_t *dec_thread;

        if (instance_id < MAX_CODEC_THREADS)
        {
                /* dec_thread  codec_thread + instance_id; */
                /* Let's make a not readable code :) */
                dec_thread  &instance_id[codec_thread];

                dec_thread->instance_id  instance_id;
                dec_thread->eof_flag  FALSE;
                dec_thread->last_mp3d_error  MP3D_OK;
                dec_thread->inp_fname  NULL;
                dec_thread->out_fname  NULL;
                dec_thread->ref_fname  NULL;
                dec_thread->num_out_iter  rand() % test_iter;
                dec_thread->decoded_frames  0;
                dec_thread->num_frames  -1;
                dec_thread->thread_finished  FALSE;
        }
}

/**/
/* perform_bitmach_hex */
/**
@brief

@param  None

@return
*/
/**/
int perform_bitmach_hex(const char *out_fname, const char *ref_fname, int bits)
{
        FILE   *pf1,
               *pf2;
        int     rd1,
                rd2;
        unsigned int dw1,
                dw2;
        int     mask  1 << bits - 1;

        if (out_fname && ref_fname)
        {
                pf1  fopen(out_fname, "rt");
                pf2  fopen(ref_fname, "rt");

                if (pf1 && pf2)
                {
                        while (TRUE)
                        {
                                if (feof(pf1) ! feof(pf2))
                                {
                                        break;  /* bitmach fails */
                                }

                                rd1  fscanf(pf1, "%lx", &dw1);
                                rd2  fscanf(pf2, "%lx", &dw2);

                                if ((dw1 & mask) ! (dw2 & mask))
                                {
                                        break;  /* bitmach fails */
                                }

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

/**/
/* perform_bitmach_raw */
/**
@brief

@param  None

@return
*/
/**/
int perform_bitmach_raw(const char *out_fname, const char *ref_fname, int bits)
{
        FILE   *pf1,
               *pf2;
        int     rd1,
                rd2;
        unsigned long dw1,
                dw2;
        int     mask  1 << bits - 1;


        if (out_fname && ref_fname)
        {
                pf1  fopen(out_fname, "rb");
                pf2  fopen(ref_fname, "rb");

                if (pf1 && pf2)
                {
                        while (TRUE)
                        {
                                if (feof(pf1) ! feof(pf2))
                                {
                                        break;  /* bitmach fails */
                                }

                                rd1  fread(&dw1, 1, sizeof(dw1), pf1);
                                rd2  fread(&dw2, 1, sizeof(dw2), pf2);

                                if ((dw1 & mask) ! (dw2 & mask))
                                {
                                        break;  /* bitmach fails */
                                }

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

/**/
/**/
int perform_bitmach_dif32(const char *fname1, const char *fname2, int bits)
{
        /* This code integrates dif32 compare tool. */
        bits  8;
        char    str_bits[3];

        sprintf(str_bits, "%d", bits);
        char   *argv[4]  {
                "skip me",
                (char *) fname1,
                (char *) fname2,
                str_bits
        };

        /* Defined in the stuff/dif32.c */
        extern int bits_acc;
        extern int main_cmp(int argc, char *argv1[]);

        main_cmp(4, argv);

        return bits_acc > bits;
}

/**/
/* app_swap_buffers */
/**
@brief

@param

@return
*/
/**/
// char app_swap_buffers( WORD16 ** new_buf_ptr, WORD32 * new_buf_len, UWORD32 instance_id )
MP3D_INT8 app_swap_buffers(MP3D_INT16 ** new_buf_ptr, MP3D_INT32 * new_buf_len,
                           MP3D_Decode_Config * dec_obj)
{
        mp3_decoder_thread_t *dec_thread;
        MP3D_Decode_Config *dec_config;
        file_buf_t *fbuf;

        *new_buf_len  0;
        *new_buf_ptr  NULL;

        int     instance_id  -1,
            i;

        for (i  0; i < MAX_CODEC_THREADS; ++i)
        {
                if (dec_obj  codec_thread[i].mp3d_config)
                {
                        instance_id  i;
                        break;
                }
        }
        assert(instance_id ! -1);

        if (instance_id < MAX_CODEC_THREADS)
        {
                dec_thread  &codec_thread[instance_id];
                dec_config  dec_thread->mp3d_config;
                fbuf  dec_thread->in_filebuf;

                if (!f_eof(fbuf))
                {
                        /* Contrary to the documentation the *new_buf_len should be set in 'shorts' */
                        *new_buf_len 
                            f_read(dec_thread->input_buffer, sizeof(short) * MP3D_INPUT_BUFFER_SIZE,
                                   fbuf);
                        *new_buf_ptr  dec_thread->input_buffer;
                        // #ifndef SHOULD_WORK_AS_DESIGNED
                        *new_buf_len / sizeof(short);
                        // #endif
                        return 0;
                }
                else
                {
                        dec_thread->eof_flag  TRUE;    /* decoder does not generate
                                                         * MP3D_END_OF_STREAM error. */
                }
        }

        return -1;
}

/**/
/* str_mp3d_error */
/**
@brief

@param  None

@return
*/
/**/
const char *str_mp3d_error(MP3D_RET_TYPE ret)
{
        switch (ret)
        {
        case MP3D_OK:
                return "no error";
        case MP3D_ERROR_LAYER:
                return "invalid layer";
        case MP3D_ERROR_SAMP_FREQ:
                return "invalid sampling frequency";
        case MP3D_ERROR_BIT_RATE:
                return "invalid bit rate";
        case MP3D_ERROR_BLOCK_TYPE:
                return "invalid block type";
        case MP3D_ERROR_CRC:
                return "CRC error";
        case MP3D_ERROR_STREAM:
                return "stream error";
        case MP3D_ERROR_DATA:
                return "not enough main data";
        case MP3D_ERROR_WRAP:
                return "bit error detected in hwrapbuf";
        case MP3D_ERROR_FREE_BIT_RATE:
                return "error computing free bit rate";
        case MP3D_END_OF_STREAM:
                return "end of bit stream";
        case MP3D_ERROR_INIT:
                return "initialization error";
        default:
                return "what do you mean?";
        }
}

/**/
/* processing_pcm_frame */
/**
@brief

@param  None

@return
*/
/**/
void processing_pcm_frame(mp3_decoder_thread_t * decoder_thread, MP3D_Decode_Params * dec_params)
{
        unsigned long *ptr;
        int     count;

        if (decoder_thread->out_fname)
        {
                for (count  dec_params->mp3d_frame_size, ptr 
                     (unsigned long *) decoder_thread->output_buffer; count; count--, ptr++)
                        fprintf(decoder_thread->out_stream, "%08lx\n", *ptr);
        }
}
