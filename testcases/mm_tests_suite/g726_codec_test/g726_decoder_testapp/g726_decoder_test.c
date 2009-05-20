/**/
/**
    @file   g726_decoder_test.c

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
D.Simakov / smkd001c         04/11/2004     TLSbo43522  Initial version
D.Simakov / smkd001c         25/02/2005     TLSbo47116  Update
D.Simakov / smkd001c         04/04/2005     TLSbo47116  The endurence and load testcases are added.

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
#include <assert.h>

/* Harness Specific Include Files. */
#include "test.h"

/* Verification Test Environment Include Files */
#include "g726_decoder_test.h"

#include "stuff/fconf.h"

/*
                                        LOCAL MACROS
*/
#define MAX_CODEC_THREADS 4

#define SAFE_DELETE(p) {if(p){free(p);pNULL;}}

#define alloc_fast(A)       malloc(A)
#define alloc_slow(A)       malloc(A)

#define NONE_FILE "n/a"

#define BEGIN_G726D_DATA 0

/*
                          LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
*/

typedef struct
{
    /**********************************************
     * Configurable parameters (public interface) *
     **********************************************/

    unsigned long instance_id;


    /* The file contains initialization (homing) sequence to
       drive the Decoder to a known initial state
       Default is no init file and the Codec is in reset state. */
    const char * hom_fname;
    const char * inp_fname;
    const char * out_fname;
    const char * ref_fname;

    int is_homing;

    unsigned short output_buffer[ G726_L_BUFFER ];
    unsigned short input_buffer[ G726_L_BUFFER ];
    short * inp_buf;
    short * out_buf;

    unsigned int bitrate;
    unsigned int law;

    int mem_reloc_freq;

    /*******************************************
     * Internal parameters (private interface) *
     *******************************************/

    eG726DReturnType last_g726D_error;

    pthread_t tid;

    sG726DDecoderConfigType * dec_config;

    int dec_samples;

    int dec_errors_count;
    int thread_finished;
    int ltp_retval;

    FILE * hom_fstream;
    FILE * inp_fstream;
    FILE * out_fstream;
    FILE * ref_fstream;

} g726_decoder_thread_t;

/*
                                       LOCAL CONSTANTS
*/


/*
                                       LOCAL VARIABLES
*/
static g726_decoder_thread_t codec_thread[ MAX_CODEC_THREADS ];
static int                   test_iter  DEFAULT_ITERATIONS; /* default iteration is hard coded */
static char                  progress[]  "-\\|/";           /* console progress bar */
static flist_t *             file_list  NULL;               /* list of input/output/reference files */
static int                   num_threads  1;             /* the number of threads for re-entrance & pre-emption */
static int                   focus_thread  -1;            /* focus thread is a thread that prints himself statistics
                                     and statistics of another threads */
static int                   enable_mem_reloc  FALSE;

/*
                                       GLOBAL CONSTANTS
*/


/*
                                       GLOBAL VARIABLES
*/

/*
                                   LOCAL FUNCTION PROTOTYPES
*/
int codec_engine( void * ptr );
/* for call in engine... */
int alloc_dec_memory( g726_decoder_thread_t * dec_thread );
int run_decoder_loop( g726_decoder_thread_t * dec_thread );
int fetch_data( g726_decoder_thread_t * dec_thread );
int homing_decoder( g726_decoder_thread_t * dec_thread );
int open_decoder_files( g726_decoder_thread_t * dec_thread );
int close_decoder_files( g726_decoder_thread_t * dec_thread );

/* helper functions */
int  run_codec( void * ptr );

/* test cases */
int nominal_functionality_test();
int re_entrance_test          ();
int re_locatability_test      ();
int endurance_test            ();
int load_test                 ();

/**/
void reset_decoder_instance( unsigned long instance_id );
int  bitmach( const char * out_fname, const char * ref_fname );
void outp_data_processing( g726_decoder_thread_t * decoder_thread );

void swap_bytes( short * words, int count );
/*
                                       LOCAL FUNCTIONS
*/


/**/
/**/
int VT_g726_decoder_setup()
{
    int rv  TFAIL;

    /** insert your code here */

    rv  TPASS;
    return rv;
}


/**/
/**/
int VT_g726_decoder_cleanup()
{
    int rv  TFAIL;
    int i;

    /* close all file descriptors */
    for( i  0; i < MAX_CODEC_THREADS; ++i )
    {
        g726_decoder_thread_t * dec_thread  &codec_thread[i];
        close_decoder_files( dec_thread );
        SAFE_DELETE( dec_thread->dec_config );
    }

    if( file_list )
        delete_list( file_list );

    rv  TPASS;
    return rv;
}


/**/
/**/
int VT_g726_decoder_test( int testcase, int iter, const char * cfg_file )
{
    int rv  TFAIL;

    test_iter  iter;

    if( file_list )
     delete_list( file_list );

    if( !read_cfg( cfg_file, &file_list ) )
    {
     tst_resm( TWARN, "VT_g726_decoder_test : can't parse %s", cfg_file );
      return rv;
    }

    switch( testcase )
    {
        case NOMINAL_FUNCTIONALITY:
            tst_resm( TINFO, "Nominal functionality test" );
            rv  nominal_functionality_test();
            tst_resm( TINFO, "End of nominal functionality test" );
            break;

     case RE_ENTRANCE:
         tst_resm( TINFO, "Re-entrance test" );
         rv  re_entrance_test();
         tst_resm( TINFO, "End of re-entrance test" );
         break;

     case RE_LOCATABILITY:
         tst_resm( TINFO, "Relocatability test" );
         rv  re_locatability_test();
         tst_resm( TINFO, "End of relocatability test" );
         break;

        case ENDURANCE:
            tst_resm( TINFO, "Endurance test" );
            rv  endurance_test();
            tst_resm( TINFO, "End of endurance test" );
            break;

        case LOAD:
            tst_resm( TINFO, "Load test" );
            rv  load_test();
            tst_resm( TINFO, "End of load test" );
            break;

     default:
         tst_resm( TWARN, "Wrong test case" );
         break;
    }

    return rv;
}


/**/
/**/
int codec_engine( void * ptr )
{
    int rv  TFAIL;

    assert( ptr );

    g726_decoder_thread_t * dec_thread  (g726_decoder_thread_t*)ptr;
    sG726DDecoderConfigType  * dec_config;

    /* allocate fast memory the decoder config */
    dec_config  (sG726DDecoderConfigType*)alloc_fast( sizeof(sG726DDecoderConfigType) );
    if( !dec_config )
    {
        tst_resm( TWARN, "codec_engine : can't allocate memory for sG726DDecoderConfigType object" );
        return rv;
    }
    dec_thread->dec_config  dec_config;

    dec_config->pvG726DDecodeInfoPtr  NULL;
    /* fill up the rellocated data position */
    dec_config->pu8APPDInitializedDataStart  BEGIN_G726D_DATA;

    /* initialize config structure memory to NULL */
    int i;
    for( i  0; i < G726_MAX_NUM_MEM_REQS; i++ )
    {
        dec_config->sG726DMemInfo.asMemInfoSub[i].pvAPPDBasePtr  NULL;
    }

    /* allocate memory  */
    if( TPASS ! alloc_dec_memory( dec_thread  ) )
    {
        return rv;
    }

    /* configure all configurable params */
    dec_config->s32APPDBitRate  dec_thread->bitrate;
    dec_config->s32APPDPcmFormat  dec_thread->law;

    /* initialize the g726 decoder */
    dec_thread->last_g726D_error  eG726DDecodeInit( dec_config );
    if( dec_thread->last_g726D_error ! G726D_OK )
    {
        tst_resm( TWARN, "codec_engine : the eG726DDecoderInit fails" );
        return rv; /* The memory will be deleted in the cleanup routine. */
    }

    /* open decoder files */
    if( TPASS ! open_decoder_files( dec_thread ) )
    {
        return rv;
    }

    if( focus_thread  -1 )
        focus_thread  dec_thread->instance_id;

    /* homing speech */
    if( TPASS ! homing_decoder( dec_thread ) )
    {
        tst_resm( TWARN, "codec_engine : Homing failed" );
        return rv;
    }

    memset( dec_thread->inp_buf, 0, G726_L_BUFFER*sizeof(short) );

    /* run main encode loop */
    rv  run_decoder_loop( dec_thread );

    /* cleanup */
    close_decoder_files( dec_thread );
    SAFE_DELETE( dec_thread->dec_config );

    rv  TPASS;
    return rv;
}


/**/
/**/
int alloc_dec_memory( g726_decoder_thread_t * dec_thread )
{
    assert( dec_thread );
    assert( dec_thread->dec_config );

    int rv  TFAIL;
    sG726DDecoderConfigType * dec_config;
    sG726DMemAllocInfoSubType * mem;
    int nr, i;

    /* short name for the decoder thread */
    dec_config  dec_thread->dec_config;
    if( !dec_config ) return rv;

    /* query memmory that's necessary for decoding */
    dec_thread->last_g726D_error  eG726DQueryMem( dec_config );
    if( dec_thread->last_g726D_error ! G726D_OK )
    {
        tst_resm( TWARN, "alloc_dec_memory : eG726DQueryMem fails" );
        return rv;
    }

    /* try to allocate requirement memory */
    nr  dec_config->sG726DMemInfo.s32G726DNumMemReqs;
    for( i  0; i < nr; ++i )
    {
        mem  &dec_config->sG726DMemInfo.asMemInfoSub[i];
        if( mem->s32G726DMemTypeFs  G726_FAST_MEMORY )
            mem->pvAPPDBasePtr  alloc_fast( mem->s32G726DSize );
        else
            mem->pvAPPDBasePtr  alloc_slow( mem->s32G726DSize );

        if( !mem->pvAPPDBasePtr )
        {
            tst_resm( TWARN, "alloc_dec_memory : can't allocate %d b", mem->s32G726DSize );
            return rv; /* The memory will be deleted in the cleanup routine. */
        }
    }

    rv  TPASS;
    return rv;
}


/**/
/**/
void free_dec_memory( g726_decoder_thread_t * dec_thread )
{
    assert( dec_thread );
    assert( dec_thread->dec_config );

    sG726DDecoderConfigType * dec_config;
    sG726DMemAllocInfoSubType * mem;
    int nr, i;

    /* short name for the decoder thread */
    dec_config  dec_thread->dec_config;

    nr  dec_config->sG726DMemInfo.s32G726DNumMemReqs;
    for( i  0; i < nr; ++i )
    {
        mem  &dec_config->sG726DMemInfo.asMemInfoSub[i];
        SAFE_DELETE( mem->pvAPPDBasePtr );
    }
}


/**/
/**/
int homing_decoder( g726_decoder_thread_t * dec_thread )
{
    assert( dec_thread );
    assert( dec_thread->dec_config );

    int num_samp, i  0;
    int rv  TFAIL;

    if( !dec_thread->hom_fname || dec_thread->is_homing )
        return TPASS;

    /* run loop */
    while( TRUE )
    {
        /* fetch next data */
        num_samp  fetch_data( dec_thread );

        if( num_samp > 0 )
        {
            /* number of input samples to be encoded */
            dec_thread->dec_config->s32APPDSampleNum  num_samp;
            dec_thread->dec_config->s32APPDBitRate  dec_thread->bitrate;
            dec_thread->dec_config->s32APPDPcmFormat  dec_thread->law;

            for( i  0; i < num_samp; ++i )
                dec_thread->out_buf[i]  0;

            /* homing speech */
            dec_thread->last_g726D_error  eG726DDecode( dec_thread->dec_config,
                                                         dec_thread->inp_buf,
                                                         dec_thread->out_buf );
            if( dec_thread->last_g726D_error ! G726D_OK )
            {
                /* errors occured during decoding */
                return rv;
            }
        }
        else if( num_samp  0 )
        {
            /* ok. end of stream */
            rv  TPASS;
            break;
        }
        else if( num_samp < 0 )
        {
            /* fk. errors */
            break;
        }
    }

    dec_thread->is_homing  TRUE;

    return rv;
}


/**/
/**/
int open_decoder_files( g726_decoder_thread_t * dec_thread )
{
    assert( dec_thread );

    int rv  TFAIL;

    /* homing file */
    if( dec_thread->hom_fname )
    {
        if( !(dec_thread->hom_fstream  fopen( dec_thread->hom_fname, "rb" )) )
        {
            tst_resm( TWARN, "open_decoder_files : can't open %s", dec_thread->hom_fname );
            return rv;
        }
    }

    /* input file */
    if( dec_thread->inp_fname )
    {
        if( !(dec_thread->inp_fstream  fopen( dec_thread->inp_fname, "rb" )) )
        {
            tst_resm( TWARN, "open_decoder_files : can't open %s", dec_thread->inp_fname );
            return rv;
        }
    }

    /* output file */
    if( dec_thread->out_fname )
    {
        if( !(dec_thread->out_fstream  fopen( dec_thread->out_fname, "wb" )) )
        {
            tst_resm( TWARN, "open_decoder_files : can't create %s", dec_thread->out_fname );
            return rv;
        }
    }

    rv  TPASS;
    return rv;
}


/**/
/**/
int close_decoder_files( g726_decoder_thread_t * dec_thread )
{
    assert( dec_thread );

    int rv  TFAIL;

    /* homing file */
    if( dec_thread->hom_fstream )
    {
        fclose( dec_thread->hom_fstream );
        dec_thread->hom_fstream  NULL;
    }

    /* input file */
    if( dec_thread->inp_fstream )
    {
        fclose( dec_thread->inp_fstream );
        dec_thread->inp_fstream  NULL;
    }

    /* output file */
    if( dec_thread->out_fstream )
    {
        fclose( dec_thread->out_fstream );
        dec_thread->out_fstream  NULL;
    }

    rv  TPASS;
    return rv;
}


/**/
/**/
int re_locate_memory( g726_decoder_thread_t * dec_thread )
{
    assert( dec_thread );
    assert( dec_thread->dec_config );

    /* alias */
    int rv  TFAIL;

    free_dec_memory( dec_thread );
    if( TPASS ! alloc_dec_memory( dec_thread ) )
        return rv;
    dec_thread->dec_config->pu8APPDInitializedDataStart  BEGIN_G726D_DATA;
    dec_thread->last_g726D_error  eG726DDecodeInit( dec_thread->dec_config );
    if( dec_thread->last_g726D_error ! G726D_OK )
    {
        tst_resm( TWARN, "re_locate_memory : the g726_decoder_init fails" );
        return rv;
    }

    rv  TPASS;
    return rv;
}


/**/
/**/
int run_decoder_loop( g726_decoder_thread_t * dec_thread )
{
    assert( dec_thread );
    assert( dec_thread->dec_config );

    int num_samp, n  0;
    int rv  TFAIL;

    /* run loop */
    while( TRUE )
    {
        if( dec_thread->mem_reloc_freq )
        {
            if( n % dec_thread->mem_reloc_freq  0 )
                if( TPASS ! re_locate_memory( dec_thread ) )
                    break;
        }

        /* fetch next data */
        num_samp  fetch_data( dec_thread );

        if( num_samp > 0 )
        {
            /* number of input samples to be encoded */
            dec_thread->dec_config->s32APPDSampleNum  num_samp;
            dec_thread->dec_config->s32APPDBitRate  dec_thread->bitrate;
            dec_thread->dec_config->s32APPDPcmFormat  dec_thread->law;
            /* call encode routine */
            dec_thread->last_g726D_error  eG726DDecode( dec_thread->dec_config,
                                                         dec_thread->inp_buf,
                                                         dec_thread->out_buf );
            if( dec_thread->last_g726D_error ! G726D_OK )
            {
                /* errors occured during decoding */
                dec_thread->dec_errors_count++;
                continue;
            }

            dec_thread->dec_samples + num_samp;

            /* ok. processing output data and continue. */
            /* num_samp should lookup from dec_config->s32APPDSampleNum */
            outp_data_processing( dec_thread );
        }
        else if( num_samp  0 )
        {
            /* ok. end of stream */
            rv  TPASS;
            break;
        }
        else if( num_samp < 0 )
        {
            /* fk. errors */
            break;
        }

        if( dec_thread->instance_id  focus_thread )
        {
            int k;
            g726_decoder_thread_t * e;
            for( k  0; k < num_threads; k++ )
            {
                e  &codec_thread[k];
                printf( "-t[%lu] samp[%d] ", e->instance_id, e->dec_samples );
            }
            printf( "%c\r", progress[(n)%(sizeof(progress)-1)] );
        }
        fflush( stdout );

        n++;
    }

    return rv;
}


/**/
/**/
int fetch_data( g726_decoder_thread_t * dec_thread )
{
    int num_samp;
    FILE * in  dec_thread->inp_fstream;
    if( !dec_thread->is_homing && dec_thread->hom_fstream )
        in  dec_thread->hom_fstream;

    if( feof( in ) )
    {
        num_samp  0;
    }
    else
    {
        num_samp  fread( dec_thread->inp_buf, sizeof(short), G726_L_BUFFER, in );
        swap_bytes( dec_thread->inp_buf, num_samp );
    }
    return num_samp;
}

/**/
/**/
int run_codec( void * ptr )
{
    assert( ptr );

    int i;

    g726_decoder_thread_t * dec_thread  (g726_decoder_thread_t*)ptr;
    dec_thread->ltp_retval  codec_engine( ptr );
    dec_thread->thread_finished  TRUE;
    if( dec_thread->instance_id  focus_thread )
    {
        for( i  0; i < num_threads; ++i )
        {
            if( !codec_thread[i].thread_finished )
            {
                focus_thread  codec_thread[i].instance_id;
                break;
            }
        }
    }

    return dec_thread->ltp_retval;
}


/**/
/**/
const char * bitrate2str( int bitrate )
{
    switch( bitrate )
    {
        case BIT_RATE_16KBPS: return "16 kbps";
        case BIT_RATE_24KBPS: return "24 kbps";
        case BIT_RATE_32KBPS: return "32 kbps";
        case BIT_RATE_40KBPS: return "40 kbps";
        default             : return "what did you mean?";
    }
}


/**/
/**/
const char * pcmfmt2str( int pcm_format )
{
    switch( pcm_format )
    {
    case G726_PCM_LINEAR: return "linear";
    case G726_PCM_ALAW  : return "a-law";
    case G726_PCM_ULAW  : return "u-law";
    default        : return "what did you mean?";
    }
}


/**/
/**/
int nominal_functionality_test()
{
    flist_t * node;
    int i;
    int rv  TPASS;
    g726_decoder_thread_t * dec_thread  &codec_thread[0];

    for( node  file_list, i  0; node; node  node->next, ++i )
    {
        reset_decoder_instance( 0 );

        tst_resm( TINFO, "input file: %s", node->inp_fname );

        dec_thread->hom_fname   strcmp( node->hom_fname, NONE_FILE ) ! 0 ? node->hom_fname : NULL;
        dec_thread->inp_fname   strcmp( node->inp_fname, NONE_FILE ) ! 0 ? node->inp_fname : NULL;
        dec_thread->out_fname   strcmp( node->out_fname, NONE_FILE ) ! 0 ? node->out_fname : NULL;
        dec_thread->ref_fname   strcmp( node->ref_fname, NONE_FILE ) ! 0 ? node->ref_fname : NULL;
        dec_thread->bitrate     node->bitrate;
        dec_thread->law         node->pcm_format;

        if( enable_mem_reloc )
            dec_thread->mem_reloc_freq  rand()%10+10;

        /* call decoder engine */
        rv + run_codec( dec_thread );

        /* bitmach */
        if( dec_thread->ref_fname && dec_thread->out_fname )
        {
            int bitmach_pass  bitmach( dec_thread->out_fname, dec_thread->ref_fname );
            if( !bitmach_pass )
            {
                tst_resm( TWARN, "bitmatch failed" );
                rv  TFAIL;
            }
            else
            {
                tst_resm( TINFO, "bitmatch passed" );
            }
        }
    }

    return rv;
}


/**/
/**/
int re_entrance_test()
{
    flist_t * node;
    int i;
    int rv  TPASS;
    g726_decoder_thread_t * dec_thread;

    for( node  file_list, i  0; node && i < MAX_CODEC_THREADS; node  node->next, ++i )
    {
        reset_decoder_instance( i );
        dec_thread  &codec_thread[i];

        dec_thread->hom_fname   strcmp( node->hom_fname, NONE_FILE ) ! 0 ? node->hom_fname : NULL;
        dec_thread->inp_fname   strcmp( node->inp_fname, NONE_FILE ) ! 0 ? node->inp_fname : NULL;
        dec_thread->out_fname   strcmp( node->out_fname, NONE_FILE ) ! 0 ? node->out_fname : NULL;
        dec_thread->ref_fname   strcmp( node->ref_fname, NONE_FILE ) ! 0 ? node->ref_fname : NULL;
        dec_thread->bitrate     node->bitrate;
        dec_thread->law         node->pcm_format;

        if( pthread_create( &dec_thread->tid, NULL, (void*)&run_codec, dec_thread ) )
        {
            tst_resm( TWARN, "re_entrance_test : error creating thread %d", i );
            return TFAIL;
        }
    }

    num_threads  i;
    for( i  0; i < num_threads; ++i )
    {
        dec_thread  &codec_thread[i];
        pthread_join( dec_thread->tid, NULL );
    }

    for( i  0; i < num_threads; ++i )
    {
        dec_thread  &codec_thread[i];
        /* bitmach */
        if( dec_thread->ref_fname && dec_thread->out_fname )
        {
            int bitmach_pass  bitmach( dec_thread->out_fname, dec_thread->ref_fname );
            if( !bitmach_pass )
            {
                tst_resm( TWARN, "bitmach failed" );
                rv  TFAIL;
            }
            else
            {
                tst_resm( TINFO, "bitmach passed" );
            }
        }
        rv + dec_thread->ltp_retval;
    }

    return rv;
}


/**/
/**/
int re_locatability_test()
{
    flist_t * node;
    int i, j;
    int rv  TPASS;
    g726_decoder_thread_t * dec_thread  &codec_thread[0];

    for( node  file_list, i  0; node; node  node->next, ++i )
    {
        tst_resm( TINFO, "input file: %s", node->inp_fname );
        for( j  0; j < test_iter; ++j )
        {
            reset_decoder_instance( 0 );

            dec_thread->hom_fname   strcmp( node->hom_fname, NONE_FILE ) ! 0 ? node->hom_fname : NULL;
            dec_thread->inp_fname   strcmp( node->inp_fname, NONE_FILE ) ! 0 ? node->inp_fname : NULL;
            dec_thread->out_fname   strcmp( node->out_fname, NONE_FILE ) ! 0 ? node->out_fname : NULL;
            dec_thread->ref_fname   strcmp( node->ref_fname, NONE_FILE ) ! 0 ? node->ref_fname : NULL;
            dec_thread->bitrate     node->bitrate;
            dec_thread->law         node->pcm_format;

            if( enable_mem_reloc )
                dec_thread->mem_reloc_freq  rand()%10+10;

            /* call decoder engine */
            rv + run_codec( dec_thread );

            /* bitmach */
            if( dec_thread->ref_fname && dec_thread->out_fname )
            {
                int bitmach_pass  bitmach( dec_thread->out_fname, dec_thread->ref_fname );
                if( !bitmach_pass )
                {
                    tst_resm( TWARN, "bitmatch failed" );
                    rv  TFAIL;
                }
                else
                {
                    tst_resm( TINFO, "bitmatch passed" );
                }
            }
            tst_resm( TINFO, "Data memory was relocated" );
        }
    }

    return rv;
}



/**/
/**/
int endurance_test()
{
    int i, rv  TPASS;
    for( i  0; i < test_iter; ++i )
    {
        tst_resm( TINFO, "The %d iteration is started", i+1 );
        rv + nominal_functionality_test();
        tst_resm( TINFO, "The %d iteration is completed", i+1 );
    }
    return rv;
}


/**/
/**/
int robustness_test()
{
    return TPASS;
}


/**/
/**/
void hogcpu()
{
    while( 1 )
    {
     sqrt( rand() );
    }
}


/**/
/**/
int load_test()
{
    int rv  TFAIL;
    pid_t pid;

    switch( pid  fork() )
    {
     case -1:
         tst_resm( TWARN, "load_envirounment_test : fork failed" );
        return rv;
     case 0:
            /* child process */
         hogcpu();
    default:
            /* parent */
         sleep(10);
         rv  nominal_functionality_test();
        /* kill child process once decode/encode loop has ended */
         if( kill( pid, SIGKILL ) ! 0 )
         {
       tst_resm( TWARN, "load_envirounment_test : Kill(SIGKILL) error" );
         return rv;
         }
    }
    return rv;
}


/**/
/**/
void reset_decoder_instance( unsigned long instance_id )
{
    g726_decoder_thread_t * dec_thread;

    if( instance_id < MAX_CODEC_THREADS )
    {
        dec_thread  &codec_thread[instance_id];

        dec_thread->instance_id       instance_id;
        dec_thread->last_g726D_error  G726D_OK;
        dec_thread->hom_fname         NULL;
        dec_thread->is_homing         FALSE;
        dec_thread->inp_fname       NULL;
        dec_thread->out_fname       NULL;
        dec_thread->ref_fname       NULL;
        dec_thread->thread_finished   FALSE;
        dec_thread->inp_buf           (short*)dec_thread->input_buffer;
        dec_thread->out_buf           (short*)dec_thread->output_buffer;
        dec_thread->dec_errors_count  0;
        dec_thread->mem_reloc_freq    0;
        dec_thread->dec_samples       0;
    }
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


/**/
/**/
int bitmach( const char * out_fname, const char * ref_fname )
{
    return compare_files(out_fname, ref_fname);
}

/**/
/**/
void outp_data_processing( g726_decoder_thread_t * dec_thread )
{
    assert( dec_thread );
    assert( dec_thread->dec_config );
    int num_samp  dec_thread->dec_config->s32APPDSampleNum, i;
    unsigned short word;

    if( dec_thread->out_fstream )
    {
        for( i  0; i < num_samp; ++i )
        {
            word  dec_thread->out_buf[i];
            fwrite( &word, 1, sizeof(short), dec_thread->out_fstream );
        }
    }
}


/**/
/**/
void swap_bytes( short * words, int count )
{
#ifdef G726_BIG_ENDIAN
    char * byte;
    while( count-- )
    {
         byte  (char*)(words+count);
     byte[0] ^ byte[1] ^ byte[0] ^ byte[1];
    }
#endif
}

#ifdef __cplusplus
}
#endif

