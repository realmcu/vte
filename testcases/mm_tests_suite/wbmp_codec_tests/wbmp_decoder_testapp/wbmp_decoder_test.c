/**/
/**
    @file   wbmp_decoder_test.c

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
D.Simakov/smkd001c           12/07/2004     TLSbo40263  Initial version
D.Simakov/smkd001c           08/12/2004     TLSbo41675  Ported to read sources
D.Simakov/smkd001c           01/03/2005     TLSbo47117  Update


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

/* Harness Specific Include Files. */
#include "test.h"

/* Verification Test Environment Include Files */
#include "wbmp_decoder_test.h"

#include "stuff/fconf.h"
#include "stuff/fb_draw_api.h"

/*
                                        LOCAL MACROS
*/
#define MAX_CODEC_THREADS 4

#define SAFE_DELETE(p) {if(p){free(p);pNULL;}}

#define NONE_FILE "n/a"

#define INPUT_BUFFER_SIZE 2048 /* size of the input buffer for each decoding thread */

#define DBGM(m){printf("--%s--\n",m);fflush(stdout);}

/*
                          LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
*/

typedef struct
{
    unsigned long thread_idx;

    const char * inp_fname;
    const char * out_fname;
    const char * ref_fname;

    char output_buffer[ 4*INPUT_BUFFER_SIZE ];
    char input_buffer [ INPUT_BUFFER_SIZE ];

    int endurance_count;

    WBMP_error_type last_wbmp_error;

    pthread_t tid;

    WBMP_Decoder_Object * dec_obj;
    WBMP_Decoder_Params dec_param;

    FILE * inp_fstream;
    FILE * out_fstream;
    FILE * ref_fstream;

    int decoded_rows;

    int thread_finished;
    int ltp_retval;

} wbmp_decoder_thread_t;

/*
                                       LOCAL CONSTANTS
*/


/*
                                       LOCAL VARIABLES
*/
static wbmp_decoder_thread_t codec_thread[ MAX_CODEC_THREADS ];
static int test_iter  DEFAULT_ITERATIONS; /* default iteration is hard coded */
static char progress[]  "-\\|/";          /* console progress bar */
static flist_t * file_list  NULL;         /* list of input/output/reference files */
static int num_threads  0;          /* the number of threads for re-entrance & pre-emption */
static int set_priors  FALSE;         /* TRUE means that run_codec shall call nice(XXX) */
static int focus_thread  -1;          /* focus thread is a thread that prints himself statistics
            and statistics of another threads */

/*
                                       GLOBAL CONSTANTS
*/
extern int delay_value;

/*
                                       GLOBAL VARIABLES
*/

/*
                                   LOCAL FUNCTION PROTOTYPES
*/
int codec_engine( void * ptr );

int run_decoding_loop( wbmp_decoder_thread_t * dec_thread );
int alloc_dec_memory( wbmp_decoder_thread_t * dec_thread );
int free_dec_memory( wbmp_decoder_thread_t * dec_thread );
int open_decoder_files( wbmp_decoder_thread_t * dec_thread );
int close_decoder_files( wbmp_decoder_thread_t * dec_thread );
void on_data_output( wbmp_decoder_thread_t * dec_thread );

WBMP_error_type WBMP_get_new_data( WBMP_UINT8 ** buf_ptr, WBMP_UINT32 * buf_len, WBMP_Decoder_Object * dec_obj );

/* helper functions */
int  run_codec( void * ptr );

/* test cases */
int nominal_functionality_test();
int endurance_test            ();
int re_entrance_test          ();
int pre_emption_test          ();
int robustness_test           ();
int load_test                 ();

void reset_decoder_instance( unsigned long thread_idx );
int do_files_exist( const char * fname1, const char * fname2 );
int bitmatch( const char * out_fname, const char * ref_fname );
void detect_enter();

/*
                                       LOCAL FUNCTIONS
*/


/**/
/**/
int VT_wbmp_decoder_setup()
{
    int rv  TFAIL;

    rv  TPASS;
    return rv;
}


/**/
/**/
int VT_wbmp_decoder_cleanup()
{
    int rv  TFAIL;

    if( file_list )
        delete_list( file_list );

    rv  TPASS;
    return rv;
}


/**/
/**/
int VT_wbmp_decoder_test( int testcase, int iter, const char * cfg_file )
{
    int rv  TFAIL;

    test_iter  iter;

    if( file_list )
        delete_list( file_list );

    if( !read_cfg( cfg_file, &file_list ) )
    {
        tst_resm( TWARN, "VT_wbmp_decoder_test : can't parse %s", cfg_file );
        return rv;
    }

    switch( testcase )
    {
        case NOMINAL_FUNCTIONALITY:
            tst_resm( TINFO, "Nominal functionality test" );
            rv  nominal_functionality_test();
            tst_resm( TINFO, "End nominal functionality test" );
            break;

        case ENDURANCE:
            tst_resm( TINFO, "Endurance test" );
            rv  endurance_test();
            tst_resm( TINFO, "End endurance test" );
            break;

        case RE_ENTRANCE:
            tst_resm( TINFO, "Re-entrance test" );
            rv  re_entrance_test();
            tst_resm( TINFO, "End re-entrance test" );
            break;

        case PRE_EMPTION:
            tst_resm( TINFO, "Pre-emption test" );
            rv  pre_emption_test();
            tst_resm( TINFO, "End pre-emption test" );
            break;

        case ROBUSTNESS:
            tst_resm( TINFO, "Robustness test" );
            rv  robustness_test();
            tst_resm( TINFO, "End robustness test" );
            break;

        case LOAD:
            tst_resm( TINFO, "Load test" );
            rv  load_test();
            tst_resm( TINFO, "End load test" );
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
    assert( ptr );

    const framebuffer_t * fb  get_framebuffer();
    assert( fb );
    argb_color_t white  {1.0f, 1.0f, 1.0f, 1.0f};

    if( num_threads  0 )
        fb->clear_screen( &white );

    int rv  TFAIL;
    wbmp_decoder_thread_t * dec_thread  (wbmp_decoder_thread_t*)ptr;
    assert( dec_thread->thread_idx < MAX_CODEC_THREADS );

    WBMP_Decoder_Object * dec_obj;

    /* allocate memory for the decoder object */
    dec_obj  (WBMP_Decoder_Object*)malloc( sizeof(WBMP_Decoder_Object) );
    if( !dec_obj )
    {
        tst_resm( TWARN, "codec_engine : can't allocate memory for the decoder (%d bytes)", sizeof(WBMP_Decoder_Object) );
        return rv;
    }
    dec_thread->dec_obj  dec_obj;

    /* configure the decoder */
    dec_obj->get_new_data             WBMP_get_new_data;
    dec_obj->dec_param.out_format     WBMP_E_OUTPUTFORMAT_RGB888;
    dec_obj->dec_param.scale_mode     dec_thread->dec_param.scale_mode;
    dec_obj->dec_param.output_width   dec_thread->dec_param.output_width;
    dec_obj->dec_param.output_height  dec_thread->dec_param.output_height;

    if( dec_obj->dec_param.output_width  0 )
        dec_obj->dec_param.output_width  fb->width;
    if( dec_obj->dec_param.output_height  0 )
        dec_obj->dec_param.output_height  fb->height;

    /* open needed file streams */
    if( TPASS ! open_decoder_files( dec_thread  ) )
        return rv;

    /* allocate memory that requires by the decoder */
    if( TPASS ! alloc_dec_memory( dec_thread ) )
        return rv;

    fseek( dec_thread->inp_fstream, 0, SEEK_SET );

    /* init */
    dec_thread->last_wbmp_error  WBMP_decoder_init( dec_obj );
    if( dec_thread->last_wbmp_error ! WBMP_ERR_NO_ERROR )
    {
        tst_resm( TWARN, "codec_engine : WBMP_decoder_init fails %d", dec_thread->last_wbmp_error );
        return rv;
    }

    /* set focus */
    if( focus_thread  -1 )
        focus_thread  dec_thread->thread_idx;

    /* run decoding loop */
    if( TPASS ! run_decoding_loop( dec_thread ) )
        return rv;

    /* cleanup... */
    close_decoder_files( dec_thread );
    free_dec_memory( dec_thread );
    SAFE_DELETE( dec_thread->dec_obj );

    rv  TPASS;
    return rv;
}


/**/
/**/
int run_decoding_loop( wbmp_decoder_thread_t * dec_thread )
{
    assert( dec_thread );
    assert( dec_thread->dec_obj );

    int row;

    WBMP_Decoder_Object    * dec_obj  dec_thread->dec_obj;
    WBMP_Decoder_Info_Init * dec_info_init  &dec_obj->dec_info_init;

    for( row  0; row < dec_info_init->output_height; ++row )
    {
        dec_thread->last_wbmp_error  WBMP_decode_row_pp( dec_obj, dec_thread->output_buffer );
        if( dec_thread->last_wbmp_error ! WBMP_ERR_NO_ERROR && dec_thread->last_wbmp_error ! WBMP_ERR_EOF )
        {
            tst_resm( TWARN, "run_decoding_loop : WBMP_decode_row_pp fails" );
            return TFAIL;
        }
        on_data_output( dec_thread );
        dec_thread->decoded_rows++;

        if( dec_thread->thread_idx  focus_thread )
        {
            int k;
            wbmp_decoder_thread_t * e;
            for( k  0; k < num_threads; k++ )
            {
                e  &codec_thread[k];
                printf( "-t[%lu] rows[%d] ", e->thread_idx, e->decoded_rows );
            }
            if( num_threads  0 )
            {
                printf( "-t[%lu] rows[%d] ", dec_thread->thread_idx, dec_thread->decoded_rows );
            }
            printf( "%c\r", progress[(row)%(sizeof(progress)-1)] );
        }
        fflush( stdout );
    }

    detect_enter();

    return TPASS;
}


/**/
/**/
int alloc_dec_memory( wbmp_decoder_thread_t * dec_thread )
{
    assert( dec_thread );
    assert( dec_thread->dec_obj );

    int rv  TFAIL;

    WBMP_Decoder_Object     * dec_obj  dec_thread->dec_obj;
    WBMP_Mem_Alloc_Info     * mem_info  &dec_obj->mem_info;
    WBMP_Mem_Alloc_Info_Sub * mem_info_sub;
    int i;

    dec_thread->last_wbmp_error  WBMP_query_dec_mem( dec_obj );
    if( dec_thread->last_wbmp_error ! WBMP_ERR_NO_ERROR )
    {
        tst_resm( TWARN, "alloc_dec_memory : WBMP_query_dec_mem fails" );
        return rv;
    }

    for( i  0; i < mem_info->num_reqs; ++i )
    {
        mem_info_sub  &mem_info->mem_info_sub[i];
        mem_info_sub->ptr  malloc( mem_info_sub->size );
        if( !mem_info_sub->ptr )
        {
            tst_resm( TWARN, "alloc_dec_memory : can't allocate %ld bytes", mem_info_sub->size );
            return rv;
        }
    }

    rv  TPASS;
    return rv;
}


/**/
/**/
int free_dec_memory( wbmp_decoder_thread_t * dec_thread )
{
    assert( dec_thread );
    assert( dec_thread->dec_obj );

    int rv  TFAIL;

    WBMP_Decoder_Object     * dec_obj  dec_thread->dec_obj;
    WBMP_Mem_Alloc_Info     * mem_info  &dec_obj->mem_info;
    WBMP_Mem_Alloc_Info_Sub * mem_info_sub;
    int i;

    for( i  0; i < mem_info->num_reqs; ++i )
    {
        mem_info_sub  &mem_info->mem_info_sub[i];
        SAFE_DELETE( mem_info_sub->ptr );
    }

    rv  TPASS;
    return rv;
}


/**/
/**/
int open_decoder_files( wbmp_decoder_thread_t * dec_thread )
{
    assert( dec_thread );

    int rv  TFAIL;

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
        if( !(dec_thread->out_fstream  fopen( dec_thread->out_fname, "wb+" )) )
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
int close_decoder_files( wbmp_decoder_thread_t * dec_thread )
{
    assert( dec_thread );

    int rv  TFAIL;

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
int run_codec( void * ptr )
{
    int i, rv  TFAIL, nice_inc;

    if( !ptr ) return rv;

    wbmp_decoder_thread_t * dec_thread  (wbmp_decoder_thread_t*)ptr;

    /* for preemption case */
    if( set_priors )
    {
        nice_inc  (int)((float)dec_thread->thread_idx / MAX_CODEC_THREADS) * 15;

        if( nice(nice_inc) < 0 )
        {
            tst_resm( TWARN, "run_codec : nice(%d) failed", nice_inc );
        }
    }

    dec_thread->ltp_retval  codec_engine( ptr );

    dec_thread->thread_finished  TRUE;
    if( dec_thread->thread_idx  focus_thread )
    {
        for( i  0; i < num_threads; ++i )
        {
            if( !codec_thread[i].thread_finished )
            {
                focus_thread  codec_thread[i].thread_idx;
                break;
            }
        }
    }

    return dec_thread->ltp_retval;
}


/**/
/**/
int nominal_functionality_test()
{
    flist_t * node;
    int i;
    int rv  TPASS;
    wbmp_decoder_thread_t * dec_thread  &codec_thread[0];

    for( node  file_list, i  0; node; node  node->next, ++i )
    {
        reset_decoder_instance( 0 );

#ifdef DEBUG_TEST
        printf( "\n\t----- decoding info ------\n"
            "\tcount      :   %d\n"
            "\tscale_mode :   %d\n"
            "\tout_width  :   %d\n"
            "\tout_height :   %d\n"
            "\tinp_file   :   %s\n"
            "\tout_file   :   %s\n"
            "\tref_file   :   %s\n",
            i,
            node->scale_mode,
            node->out_width,
            node->out_height,
            node->inp_fname,
            node->out_fname,
            node->ref_fname );
        printf( "\n" );
#endif
        tst_resm( TINFO, "input file: %s", node->inp_fname );

        dec_thread->inp_fname   strcmp( node->inp_fname, NONE_FILE ) ! 0 ? node->inp_fname : NULL;
        dec_thread->out_fname   strcmp( node->out_fname, NONE_FILE ) ! 0 ? node->out_fname : NULL;
        dec_thread->ref_fname   strcmp( node->ref_fname, NONE_FILE ) ! 0 ? node->ref_fname : NULL;

        dec_thread->dec_param.scale_mode  node->scale_mode;
        dec_thread->dec_param.output_width   node->out_width;
        dec_thread->dec_param.output_height  node->out_height;

        /* call decoder engine */
        rv + codec_engine( dec_thread );

        /* bitmatch */
        if( do_files_exist( dec_thread->out_fname, dec_thread->ref_fname ) )
        {
            if( !bitmatch( dec_thread->out_fname, dec_thread->ref_fname ) )
            {
                tst_resm( TWARN, "bitmatch failed (%s vs %s)", dec_thread->out_fname, dec_thread->ref_fname );
                rv  TFAIL;
            }
            else
            {
                tst_resm( TINFO, "bitmatch passed (%s vs %s)", dec_thread->out_fname, dec_thread->ref_fname );
            }
        }
    }

    return rv;
}


/**/
/**/
int endurance_test()
{
    int i;
    int rv  TPASS;

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
int re_entrance_test()
{
    flist_t * node;
    int i;
    int rv  TPASS;
    wbmp_decoder_thread_t * dec_thread;

    /* compute the actual number of threads */
    for( node  file_list;
         node && num_threads < MAX_CODEC_THREADS;
         node  node->next, ++num_threads );

    const framebuffer_t * fb  get_framebuffer();
    assert( fb );

    for( node  file_list, i  0; node && i < MAX_CODEC_THREADS; node  node->next, ++i )
    {
        reset_decoder_instance( i );
        dec_thread  &codec_thread[i];

        dec_thread->inp_fname   strcmp( node->inp_fname, NONE_FILE ) ! 0 ? node->inp_fname : NULL;
        dec_thread->out_fname   strcmp( node->out_fname, NONE_FILE ) ! 0 ? node->out_fname : NULL;
        dec_thread->ref_fname   strcmp( node->ref_fname, NONE_FILE ) ! 0 ? node->ref_fname : NULL;

        int y_offset  0;
        float dy  (float)fb->height / num_threads;
        y_offset  (int)(dy * dec_thread->thread_idx);

        dec_thread->dec_param.scale_mode  node->scale_mode;
        dec_thread->dec_param.output_width   node->out_width;
        dec_thread->dec_param.output_height  (int)dy;

        if( pthread_create( &dec_thread->tid, NULL, (void *)&run_codec, dec_thread ) )
        {
            tst_resm( TWARN, "re_entrance_test : error creating thread %d", i );
            return TFAIL;
        }
    }

    for( i  0; i < num_threads; ++i )
    {
        dec_thread  &codec_thread[i];
        pthread_join( dec_thread->tid, NULL );
    }

    printf( "\n" );

    /**/
    for( i  0; i < num_threads; ++i )
    {
        dec_thread  &codec_thread[i];
        rv + dec_thread->ltp_retval;
        /* bitmatch */
        if( do_files_exist( dec_thread->out_fname, dec_thread->ref_fname ) )
        {
            if( !bitmatch( dec_thread->out_fname, dec_thread->ref_fname ) )
            {
                tst_resm( TWARN, "bitmatch failed (%s vs %s)", dec_thread->out_fname, dec_thread->ref_fname );
                rv  TFAIL;
            }
            else
            {
                tst_resm( TINFO, "bitmatch passed (%s vs %s)", dec_thread->out_fname, dec_thread->ref_fname );
            }
        }
    }

    return rv;
}


/**/
/**/
int pre_emption_test()
{
    set_priors  TRUE;
    return re_entrance_test();
}


/**/
/**/
int robustness_test()
{
    return nominal_functionality_test() ? TPASS : TFAIL;
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
         tst_resm( TWARN, "load_test : fork failed" );
        return rv;
     case 0:
            /* child process */
         hogcpu();
    default:
            /* parent */
         sleep(1);
         rv  nominal_functionality_test();
        /* kill child process once decode/encode loop has ended */
         if( kill( pid, SIGKILL ) ! 0 )
         {
       tst_resm( TWARN, "load_test : Kill(SIGKILL) error" );
         return rv;
         }
    }
    return rv;
}


/**/
/**/
void reset_decoder_instance( unsigned long thread_idx )
{
    wbmp_decoder_thread_t * dec_thread;

    if( thread_idx < MAX_CODEC_THREADS )
    {
        dec_thread  &codec_thread[thread_idx];

        dec_thread->thread_idx        thread_idx;
        dec_thread->last_wbmp_error    WBMP_ERR_NO_ERROR;
        dec_thread->inp_fname        NULL;
        dec_thread->out_fname        NULL;
        dec_thread->ref_fname        NULL;
        dec_thread->decoded_rows       0;
        dec_thread->thread_finished    FALSE;
    }
}


/**/
/**/
int do_files_exist( const char * fname1, const char * fname2 )
{
    FILE * fstream1  fopen( fname1, "r" );
    if( fstream1 )
    {
        fclose( fstream1 );
        FILE * fstream2  fopen( fname2, "r" );
        if( fstream2 )
        {
            fclose( fstream2 );
            return TRUE;
        }
    }
    return FALSE;
}


/**/
/**/
int bitmatch( const char * fname1, const char * fname2 )
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
WBMP_error_type WBMP_get_new_data( WBMP_UINT8 ** buf_ptr, WBMP_UINT32 * buf_len, WBMP_Decoder_Object * dec_obj )
{
    assert( dec_obj );

    int bytes_read  0, i;
    wbmp_decoder_thread_t * dec_thread  NULL;

    /* find appropriate thread instance */
    for( i  0; i < MAX_CODEC_THREADS; ++i )
    {
        if( dec_obj  codec_thread[i].dec_obj )
        {
            dec_thread  &codec_thread[i];
            break;
        }
    }
    assert( dec_thread );
    assert( dec_thread->inp_fstream );

    bytes_read  fread( dec_thread->input_buffer, 1, INPUT_BUFFER_SIZE, dec_thread->inp_fstream );

    *buf_ptr  dec_thread->input_buffer;
    *buf_len  bytes_read;

    if( bytes_read < 0 )
    {
        *buf_ptr  0;
        *buf_len  0;
        return 1;
    }

    return 0;
}


/**/
/**/
void on_data_output( wbmp_decoder_thread_t * dec_thread )
{
    assert( dec_thread );

    const framebuffer_t * fb  get_framebuffer();
    assert( fb );

    int y_offset  0;
    if( num_threads )
    {
        float dy  (float)fb->height / num_threads;
        y_offset  (int)(dy * dec_thread->thread_idx);
    }

    pixel_format_e pfmt  PF_RGB_888;
    fb->draw_scanline( dec_thread->output_buffer, 0, dec_thread->decoded_rows + y_offset,
                       dec_thread->dec_obj->dec_info_init.output_width, pfmt );

    if( dec_thread->out_fstream )
    {
        if( dec_thread->decoded_rows  0 )
        {
            /* ppm header */
            fprintf( dec_thread->out_fstream, "%s\n", "P6" );
            fprintf( dec_thread->out_fstream, "%d %d\n",
                dec_thread->dec_obj->dec_info_init.output_width,
                dec_thread->dec_obj->dec_info_init.output_height );
            fprintf( dec_thread->out_fstream, "%d\n", 255 );
            fflush( dec_thread->out_fstream );
            /*fwrite( "P6", 1, sizeof("P6"), dec_thread->out_fstream );
            fwrite( &dec_thread->dec_obj->dec_info_init.output_width,
            1,
            sizeof(dec_thread->dec_obj->dec_info_init.output_width),
            dec_thread->out_fstream );
            fwrite( &dec_thread->dec_obj->dec_info_init.output_height,
            1,
            sizeof(dec_thread->dec_obj->dec_info_init.output_height),
            dec_thread->out_fstream );
            long val255;
            fwrite( &val,
            1,
            sizeof(val),
            dec_thread->out_fstream );*/
        }
        fwrite( dec_thread->output_buffer, 1, dec_thread->dec_obj->dec_info_init.output_width * 3, dec_thread->out_fstream );
    }
}


/**/
/**/
void detect_enter()
{
    if(!delay_value) return;

 int fd_console  0;  /* 0 is the video input */
 fd_set fdset;
 struct timeval timeout;
 char c;

 FD_ZERO(&fdset);
 FD_SET(fd_console, &fdset);
 timeout.tv_sec  delay_value; /* set timeout !0 > blocking select */
 timeout.tv_usec  0;
 if (select(fd_console+1, &fdset, 0, 0, &timeout) > 0)
 {
  do
  {
   read(fd_console, &c, 1);
  } while (c ! 10); // i.e. line-feed
 }
}

int DebugLogText(short int msgid,char *fmt,...)
{
  return 0;
}

#ifdef __cplusplus
}
#endif

