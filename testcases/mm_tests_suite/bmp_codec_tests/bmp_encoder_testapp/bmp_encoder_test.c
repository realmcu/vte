/*
  Copyright (C) 2004, Freescale Semiconductor, Inc. All Rights Reserved
  THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
  BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
  Freescale Semiconductor, Inc.
*/

/**
@file bmp_encoder_test.c

@brief VTE C header template

@par Portability:
        Indicate if this module is portable to other compilers or platforms.
        If not, indicate specific reasons why is it not portable.
*/

/* REVISION HISTORY 

Author (core ID)      Date         CR Number    Description of Changes
-------------------   ----------   ----------   ------------------------------
D.Simakov / smkd001c  26/07/2004   TLSbo40263   Initial version
D.Simakov / smkd001c  19/04/2005   TLSbo47117   Some new testcases were added
D.Simakov / smkd001c  01/06/2005   TLSbo48591   Updates for the release 2.2
D.Simakov / smkd001c  27/07/2005   TLSbo52108   Color reduction test was added
*/

#ifdef __cplusplus
extern "C"{
#endif

/*
                                        INCLUDE FILES
*/
/* Verification Test Environment Include Files */
#include "bmp_encoder_test.h"

/*
                                        LOCAL MACROS
*/
#define MAX_CODEC_THREADS 4
#define SAFE_DELETE(p) {if(p){free(p);pNULL;}}
#define NONE_FILE "n/a"
#define alloc_fast(A)       malloc(A)
#define alloc_slow(A)       malloc(A)
// debug marker
#define DBGMI(m,n)  {printf("<<<-- %s : %d -->>>\n",m,n);fflush(stdout);}
#define DBGMS(m,s)  {printf("<<<-- %s : %s -->>>\n",m,s);fflush(stdout);}
#define DBGM(m)     {printf("<<<-- %s -->>>\n",m);fflush(stdout);}
#define DBGMI2(m,n) {printf("<<<-- %s : %d -->>>\n",m,n);fflush(stdout);}

#define INPUT_BUFFER_SIZE 10000 /*Applicable only for Streaming Mode*/
#define OUTPUT_BUFFER_SIZE 60000/*Applicable only for Streaming Mode*/

/*
                          LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
*/

typedef struct
{
    unsigned long thread_idx;

    const char * inp_fname;
    const char * out_fname;
    const char * ref_fname;

    FILE * inp_fstream;
    FILE * out_fstream;
    FILE * ref_fstream;

    unsigned char * inp_buffer[3]; /* R,G,B */
    unsigned char * out_buffer;
    unsigned long   inp_buffer_sz;
    unsigned long   out_buffer_sz;

    int endflag;
    int rows_rewind;
    int row_pixels;

    BMP_enc_object enc_obj;
    BMPE_error_type bmp_last_error;

    pthread_t tid;
    int thread_finished;
    int ltp_retval;
} bmp_encoder_thread_t;

/*
                                       LOCAL CONSTANTS
*/


/*
                                       LOCAL VARIABLES
*/
static bmp_encoder_thread_t  codec_thread[ MAX_CODEC_THREADS ];
static char                  progress[]  "-\\|/";
static int                   num_threads  1;
static int                   focus_thread  -1;
static flist_t             * file_list  NULL;
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

void adjust_enc_params ( bmp_encoder_thread_t * enc_thread );
int  alloc_enc_memory  ( bmp_encoder_thread_t * enc_thread );
void free_enc_memory   ( bmp_encoder_thread_t * enc_thread );
int  alloc_io_buffers  ( bmp_encoder_thread_t * enc_thread );
void free_io_buffers   ( bmp_encoder_thread_t * enc_thread );
int  open_files        ( bmp_encoder_thread_t * enc_thread );
void close_files       ( bmp_encoder_thread_t * enc_thread );
int  run_encoder_loop  ( bmp_encoder_thread_t * enc_thread );

int run_codec( void * ptr );

/* test cases */
int nominal_functionality_test();
int re_entrance_test          ();
int pre_emption_test          ();
int endurance_test            ();
int load_test                 ();

void print_encoding_status( bmp_encoder_thread_t * enc_thread );
void reset_encoder_instance( unsigned long thread_idx );
void on_data_output( bmp_encoder_thread_t * encoder_thread );
int  bitmatch( bmp_encoder_thread_t * enc_thread );
int  get_index_by_encoder( BMP_enc_object * enc_obj );

/* Encoder's I/O callbacks */
BMPE_error_type app_get_new_data_BMPE( BMPE_UINT8 ** buf_ptr_B,
                                       BMPE_UINT8 ** buf_ptr_G,
                                       BMPE_UINT8 ** buf_ptr_R,
                                       BMPE_UINT32 * buf_len,
                                       BMP_enc_object * enc_obj );

BMPE_error_type app_get_new_data_BMPE_gray( BMPE_UINT8 ** buf_ptr_G,
                                            BMPE_UINT32 * buf_len,
                                            BMP_enc_object * enc_obj );

BMPE_error_type app_push_output_BMPE( BMPE_UINT8 ** out_buf_ptr,
                                      BMPE_UINT32 * out_buf_len,
                                      BMP_enc_object * enc_obj );

/*
                                       LOCAL FUNCTIONS
*/

/**/
/**/
int VT_bmp_encoder_setup()
{
    return TPASS;
}


/**/
/**/
int VT_bmp_encoder_cleanup()
{
    if( file_list )
        delete_list( file_list );

    /* full cleanup will be here */

    return TPASS;
}


/**/
/**/
int VT_bmp_encoder_test()
{
    int rv  TFAIL;

    assert( file_list  NULL );

    if( !read_cfg( testapp_cfg.cfg_fname, &file_list ) )
    {
        tst_resm( TWARN, "VT_bmp_encoder_test : can't parse the %s", testapp_cfg.cfg_fname );
        return TFAIL;
    }

    switch( testapp_cfg.test_case )
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

  case PRE_EMPTION:
   tst_resm( TINFO, "Pre-emption test" );
   rv  pre_emption_test();
   tst_resm( TINFO, "End of pre-emption test" );
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

        case COLOR_REDUCTION:
            tst_resm( TINFO, "Color reduction test" );
            rv  nominal_functionality_test();
            tst_resm( TINFO, "End of color reduction test" );
            break;

  default:
   tst_resm( TINFO, "Wrong test case" );
   break;
 }

    return rv;
}


/**/
/**/
int codec_engine( void * ptr )
{
    assert( ptr );

    bmp_encoder_thread_t * enc_thread  (bmp_encoder_thread_t*)ptr;
    BMP_enc_object * enc_obj  &enc_thread->enc_obj;

    /* Open files */
    if( TPASS ! open_files( enc_thread ) )
        return TFAIL;

    /* Fill up the parameters structure */

    enc_obj->app_get_new_data_BMPE         app_get_new_data_BMPE;
 enc_obj->app_get_new_data_BMPE_gray    app_get_new_data_BMPE_gray;
 enc_obj->app_push_output_BMPE          app_push_output_BMPE;

    enc_obj->parameters.dither             TRUE;
    enc_obj->parameters.BMP_Encoding_mode  0;
    enc_obj->parameters.treedepth          0;

    adjust_enc_params( enc_thread );

    /* Allocate required memory */
    if( TPASS ! alloc_enc_memory( enc_thread ) )
        return TFAIL;

    /* Allocate memory for the IO buffers. */
    if( TPASS ! alloc_io_buffers( enc_thread ) )
        return TFAIL;

    /* Initialization */
    enc_thread->bmp_last_error  BMP_enc_init( enc_obj );
    if( enc_thread->bmp_last_error ! BMP_ENC_NO_ERROR )
    {
        tst_resm( TWARN, "codec_engine : BMP_enc_init fails #%d", enc_thread->bmp_last_error );
        return TFAIL;
    }

    /* Will assign focus, if it is not assigned. */
    if( focus_thread  -1 )
        focus_thread  enc_thread->thread_idx;

    /* Decode the bitstream and produce the output. */
    if( TPASS ! run_encoder_loop( enc_thread ) )
        return TFAIL;

    /* Cleanup. */
    free_io_buffers( enc_thread );
    free_enc_memory( enc_thread );
    close_files( enc_thread );

    return TPASS;
}


/**/
/**/
void adjust_enc_params( bmp_encoder_thread_t * enc_thread )
{
    assert( enc_thread );

    BMP_enc_object * enc_obj  &enc_thread->enc_obj;

    /* Streaming mode is not supported for less than or equal to 256 colors.
       Check for this condition is done here. */
    if( enc_obj->parameters.colors_in_output < 256 && enc_obj->parameters.BMP_Encoding_mode  1 )
    {
        /* !!! Where can it be found in the doc? */
        enc_obj->parameters.colors_in_output  MAX_COLORS + 1;
    }

    /* Grayscale encoding is not supported for less than or equal to 256 colors.
       Check for this condition is done here. */
    if( enc_obj->parameters.colors_in_output < 256 && enc_obj->parameters.pformat  E_Gray )
    {
        /* !!! Where can it be found in the doc? */
        enc_obj->parameters.colors_in_output  MAX_COLORS + 1;
    }

    if( enc_obj->parameters.colors_in_output > MAX_COLORS )
        enc_obj->data.bitsperpixel  E_BIT_COUNT_24;

    if( enc_obj->parameters.colors_in_output > 17 && enc_obj->parameters.colors_in_output < MAX_COLORS )
        enc_obj->data.bitsperpixel  E_BIT_COUNT_8;

    if( enc_obj->parameters.colors_in_output > 3 && enc_obj->parameters.colors_in_output < 16 )
        enc_obj->data.bitsperpixel  E_BIT_COUNT_4;

    if( enc_obj->parameters.colors_in_output > 0 && enc_obj->parameters.colors_in_output < 2 )
 {
  enc_obj->parameters.colors_in_output  2;
        enc_obj->data.bitsperpixel  E_BIT_COUNT_1;
 }
}


/**/
/**/
int alloc_enc_memory( bmp_encoder_thread_t * enc_thread )
{
    assert( enc_thread );
    BMP_enc_object * enc_obj  &enc_thread->enc_obj;
    BMP_enc_mem_alloc_info * mem_info  &enc_obj->mem_info;

    enc_thread->bmp_last_error  BMP_enc_query_mem_req( enc_obj );
    if( enc_thread->bmp_last_error ! BMP_ENC_NO_ERROR )
    {
     tst_resm( TWARN, "alloc_enc_memory : BMP_enc_query_mem_req fails #%d", enc_thread->bmp_last_error );
    return TFAIL;
    }

    int i;
    for( i  0; i < mem_info->num_reqs; ++i )
    {
        BMP_enc_mem_alloc_info_sub * mem_info_sub  &mem_info->mem_info_sub[i];
        if( mem_info_sub->type  E_FAST_MEMORY )
            mem_info_sub->memptr  alloc_fast( mem_info_sub->size );
        else
            mem_info_sub->memptr  alloc_fast( mem_info_sub->size );

        if( !mem_info_sub->memptr )
        {
            tst_resm( TWARN, "alloc_enc_memory : can't allocate %d bytes", mem_info_sub->size );
            return TFAIL;
        }
    }

    return TPASS;
}


/**/
/**/
void free_enc_memory( bmp_encoder_thread_t * enc_thread )
{
    assert( enc_thread );

    BMP_enc_mem_alloc_info * mem_info  &enc_thread->enc_obj.mem_info;

    int i;
    for( i  0; i < mem_info->num_reqs; ++i )
    {
        BMP_enc_mem_alloc_info_sub * mem_info_sub  &mem_info->mem_info_sub[i];
        SAFE_DELETE( mem_info_sub->memptr );
    }
}


/**/
/**/
int alloc_io_buffers( bmp_encoder_thread_t * enc_thread )
{
    assert( enc_thread );

    BMP_enc_object * enc_obj  &enc_thread->enc_obj;
    BMP_enc_parameters * enc_params  &enc_obj->parameters;

    /* Buffer-to-Buffer mode and format is RGB */
    if( enc_params->BMP_Encoding_mode  0 && enc_params->pformat  E_RGB888 )
    {
        int size  enc_params->width * enc_params->height;

        enc_thread->inp_buffer_sz  size;
        enc_thread->inp_buffer[0]  (unsigned char*)alloc_fast( size );
        enc_thread->inp_buffer[1]  (unsigned char*)alloc_fast( size );
        enc_thread->inp_buffer[2]  (unsigned char*)alloc_fast( size );

        if( !enc_thread->inp_buffer[0] || !enc_thread->inp_buffer[1] || !enc_thread->inp_buffer[2] )
        {
            tst_resm( TWARN, "alloc_io_buffers : can't allocate memory for input buffers" );
            return TFAIL;
        }
    }
    /* Buffer-to-Buffer mode and image is of type grayscale */
    else if( enc_params->BMP_Encoding_mode  0 && enc_params->pformat  E_Gray )
    {
        /* enc_thread->input_buffer[0] is the grayscale input buffer */
        int size  enc_params->width * enc_params->height;
        enc_thread->inp_buffer_sz  size;
        enc_thread->inp_buffer[0]  (unsigned char*)alloc_fast( size );
        if( !enc_thread->inp_buffer[0] )
        {
            tst_resm( TWARN, "alloc_io_buffers : can't allocate memory for input buffers" );
            return TFAIL;
        }
    }

    /* !!! It is NOT client code! */
    //enc_obj->bmp_file  (BMP_Internal_structure*)enc_obj->mem_info.mem_info_sub[0].memptr;
    //enc_obj->bmp_file->bitmap_file_header  (BITMAPFILEHEADER*)enc_obj->mem_info.mem_info_sub[1].memptr;
    //enc_obj->bmp_file->bitmap_infoheader  (BITMAPINFOHEADER*)enc_obj->mem_info.mem_info_sub[2].memptr;

    /* Query for output buffer size */
    enc_thread->bmp_last_error  BMP_enc_query_size_outbuffer( enc_obj, &enc_thread->out_buffer_sz );
    if( enc_thread->bmp_last_error ! BMP_ENC_NO_ERROR )
    {
        tst_resm( TWARN, "alloc_io_buffers : BMP_enc_query_size_outbuffer fails #%d", enc_thread->bmp_last_error );
        return TFAIL;
    }

    if( enc_params->BMP_Encoding_mode  0 )
    {
        enc_thread->out_buffer  (unsigned char*)alloc_fast( enc_thread->out_buffer_sz );
        if( !enc_thread->out_buffer )
        {
            tst_resm( TWARN, "alloc_io_buffers : can't allocate %d bytes", enc_thread->out_buffer_sz );
            return TFAIL;
        }
    }

    return TPASS;
}


/**/
/**/
void free_io_buffers( bmp_encoder_thread_t * enc_thread )
{
    assert( enc_thread );

    if( enc_thread->inp_buffer[0] && !enc_thread->inp_buffer[1] && !enc_thread->inp_buffer[2] )
    {
        SAFE_DELETE( enc_thread->inp_buffer[0] );
    }
    else if( enc_thread->inp_buffer[0] && enc_thread->inp_buffer[1] && enc_thread->inp_buffer[2] )
    {
        SAFE_DELETE( enc_thread->inp_buffer[0] );
        SAFE_DELETE( enc_thread->inp_buffer[1] );
        SAFE_DELETE( enc_thread->inp_buffer[2] );
    }

    if( enc_thread->out_buffer )
    {
        SAFE_DELETE( enc_thread->out_buffer );
    }
}


/**/
/**/
int open_files( bmp_encoder_thread_t * enc_thread )
{
    assert( enc_thread );

    if( enc_thread->inp_fname )
    {
        enc_thread->inp_fstream  fopen( enc_thread->inp_fname, "rb" );
        if( !enc_thread->inp_fstream )
        {
            tst_resm( TWARN, "open_files : can't open file %s", enc_thread->inp_fname );
            return TFAIL;
        }
    }
    if( enc_thread->out_fname )
    {
        enc_thread->out_fstream  fopen( enc_thread->out_fname, "wb" );
        if( !enc_thread->out_fstream )
        {
            tst_resm( TWARN, "open_files : can't create file %s", enc_thread->out_fname );
            return TFAIL;
        }
    }
    return TPASS;
}


/**/
/**/
void close_files( bmp_encoder_thread_t * enc_thread )
{
    assert( enc_thread );

    if( enc_thread->inp_fstream )
    {
        fclose( enc_thread->inp_fstream );
    }
    if( enc_thread->out_fstream )
    {
        fclose( enc_thread->out_fstream );
    }
}


/**/
/**/
int streaming_encode( bmp_encoder_thread_t * enc_thread )
{
    assert( enc_thread );
    assert( enc_thread->inp_fstream );

    BMP_enc_object * enc_obj  &enc_thread->enc_obj;
    BMP_enc_parameters * enc_params  &enc_obj->parameters;
    FILE * inp_fstream  enc_thread->inp_fstream;
    int r_index  0, g_index  0, b_index  0, gray_index  0;

    if( enc_params->pformat  E_RGB888 )
    {
        fseek( inp_fstream, -((enc_params->width-1)*3+1), SEEK_END );
        while( r_index ! INPUT_BUFFER_SIZE )
        {
            if( ((enc_thread->row_pixels%enc_params->width!0) || enc_thread->row_pixels  0) && !feof(inp_fstream) )
            {
                fread( &enc_thread->inp_buffer[0][r_index++], sizeof(char), 1, inp_fstream );
                fread( &enc_thread->inp_buffer[1][g_index++], sizeof(char), 1, inp_fstream );
                fread( &enc_thread->inp_buffer[2][b_index++], sizeof(char), 1, inp_fstream );
                enc_thread->row_pixels++;
            }
            else
            {
                enc_thread->row_pixels  0;
                fseek( inp_fstream, enc_params->width * (enc_params->height-enc_thread->rows_rewind)*3, SEEK_SET );
                enc_thread->rows_rewind++;
            }
        }
    }
    else if( enc_params->pformat  E_Gray )
    {
        fseek( inp_fstream, -enc_params->width + 1, SEEK_END );
        while( gray_index ! INPUT_BUFFER_SIZE )
        {
            if( (enc_thread->row_pixels%enc_params->width ! 0 || enc_thread->row_pixels  0) && !feof(inp_fstream) )
            {
                fread( &enc_thread->inp_buffer[0][gray_index++], sizeof(char), 1, inp_fstream );
                enc_thread->row_pixels++;
            }
            else
            {
                enc_thread->row_pixels0;
                fseek( inp_fstream, enc_params->width * (enc_params->height-enc_thread->rows_rewind)*3, SEEK_SET );
                enc_thread->rows_rewind++;
            }
        }
    }

    while( !enc_thread->endflag )
    {
        /* For RGB data */
        if( enc_params->pformat  E_RGB888 )
        {
            enc_thread->bmp_last_error  BMP_enc_encodeframe( enc_obj,
                                                              enc_thread->inp_buffer[2],
                                                              enc_thread->inp_buffer[1],
                                                              enc_thread->inp_buffer[0],
                                                              enc_thread->out_buffer );
        }
        /* For grayscale data */
        else if( enc_params->pformat  E_Gray )
        {
            /* input_buff_G is the grayscale input buffer.
               Other 2 are set to NULL */
            enc_thread->bmp_last_error  BMP_enc_encodeframe( enc_obj,
                                                              NULL,
                                                              enc_thread->inp_buffer[0],
                                                              NULL,
                                                              enc_thread->out_buffer );

        }
        if( enc_thread->bmp_last_error ! BMP_ENC_NO_ERROR && enc_thread->bmp_last_error ! BMP_ENC_COMPLETE )
        {
            tst_resm( TWARN, "streaming_encode : BMP_enc_encodeframe fails #%d", enc_thread->bmp_last_error );
            return TFAIL;
        }
    }

    return TPASS;
}


/**/
/**/
int buffer2buffer_encode( bmp_encoder_thread_t * enc_thread )
{
    assert( enc_thread );
    assert( enc_thread->inp_fstream );

    BMP_enc_object * enc_obj  &enc_thread->enc_obj;
    BMP_enc_parameters * enc_params  &enc_obj->parameters;
    FILE * inp_fstream  enc_thread->inp_fstream;
    int r_index  0, g_index  0, b_index  0, gray_index  0;


    #ifndef INFLAG
    #define INFLAG

    /* Read until all pixels buffers are read */
    if( enc_params->pformat  E_RGB888 )
    {
        while( (r_index ! enc_params->width * enc_params->height) && !feof(inp_fstream) )
        {
            fread( &enc_thread->inp_buffer[0][r_index++], sizeof(char), 1, inp_fstream );
            fread( &enc_thread->inp_buffer[1][g_index++], sizeof(char), 1, inp_fstream );
            fread( &enc_thread->inp_buffer[2][b_index++], sizeof(char), 1, inp_fstream );
        }
  if( r_index ! enc_params->width * enc_params->height )
  {
            tst_resm( TWARN, "buffer2buffer_encode : insufficient data" );
            return TFAIL;
  }
    }
    else if (enc_obj->parameters.pformat  E_Gray )
    {
        while( (gray_index ! enc_params->width * enc_params->height) && !feof(inp_fstream) )
        {
            fread( &enc_thread->inp_buffer[0][gray_index++], sizeof(char), 1, inp_fstream );
        }
     if( gray_index ! enc_params->width * enc_params->height )
     {
            tst_resm( TWARN, "buffer2buffer_encode : insufficient data" );
            return TFAIL;
  }
    }

    #undef INFLAG
    #endif

    /* Call the frame encode routine */

    /* For RGB data */
    if( enc_params->pformat  E_RGB888 )
    {
        enc_thread->bmp_last_error  BMP_enc_encodeframe( enc_obj,
                                                          enc_thread->inp_buffer[2],
                                                          enc_thread->inp_buffer[1],
                                                          enc_thread->inp_buffer[0],
                                                          enc_thread->out_buffer );
    }
    /* For Grayscale data */
    else if( enc_params->pformat  E_Gray )
    {
        enc_thread->bmp_last_error  BMP_enc_encodeframe( enc_obj,
                                                          NULL,
                                                          enc_thread->inp_buffer[0],
                                                          NULL,
                                                          enc_thread->out_buffer );
    }
    if( BMP_ENC_NO_ERROR ! enc_thread->bmp_last_error )
    {
        tst_resm( TWARN, "buffer2buffer_encode : BMP_enc_encodeframe fails #%d", enc_thread->bmp_last_error );
        return TFAIL;
    }

    /*!*/
    if( enc_thread->out_fstream )
        fwrite( enc_thread->out_buffer, sizeof(char), enc_thread->out_buffer_sz, enc_thread->out_fstream );

    return TPASS;
}


/**/
/**/
int run_encoder_loop( bmp_encoder_thread_t * enc_thread )
{
    assert( enc_thread );

    if( enc_thread->enc_obj.parameters.BMP_Encoding_mode  0 )
        return buffer2buffer_encode( enc_thread );
    else
        return streaming_encode( enc_thread );
}


/**/
/**/
void print_encoding_status( bmp_encoder_thread_t * enc_thread )
{
    assert( enc_thread );
    static int n  0;

    if( enc_thread->thread_idx  focus_thread )
    {
        int i;
        bmp_encoder_thread_t * other_enc_thread;
        for( i  0; i < num_threads; ++i )
        {
   other_enc_thread  &codec_thread[i];
   //sAVCDYCbCrStruct * frame  &other_enc_thread->enc_obj.sFrameData;
   //printf( "Thread[%lu]:Frame[%ld] ", other_enc_thread->thread_idx, frame->s32FrameNumber );
            printf( "Thread[%lu]:%s", other_enc_thread->thread_idx, other_enc_thread->endflag ? "Complited" : "Executing"  );
  }
  printf( "%c\r", progress[(n)%(sizeof(progress)-1)] );
    }
    fflush( stdout );
    ++n;
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
int run_codec( void * ptr )
{
    assert( ptr );

    int i;

    bmp_encoder_thread_t * enc_thread  (bmp_encoder_thread_t*)ptr;

    /* for preemption test case */
    if( testapp_cfg.test_case  PRE_EMPTION )
    {
  int nice_inc  ((float)20/num_threads)*enc_thread->thread_idx;
  if( nice(nice_inc) < 0 )
  {
   tst_resm( TWARN, "run_codec : nice(%d) failed", nice_inc );
  }
    }

    /* run codec engine */
    enc_thread->ltp_retval  codec_engine( ptr );

    if( enc_thread->ref_fname && enc_thread->out_fname )
    {
        /* perform bitmach here .... */
        if( do_files_exist(enc_thread->out_fname, enc_thread->ref_fname) )
        {
            if( !bitmatch( enc_thread ) )
            {
                tst_resm( TWARN, "bitmatch failed (%s vs %s)", enc_thread->out_fname, enc_thread->ref_fname );
                enc_thread->ltp_retval  TFAIL;
            }
            else
            {
                tst_resm( TINFO, "bitmatch passed (%s vs %s)", enc_thread->out_fname, enc_thread->ref_fname );
            }
        }
    }

    enc_thread->thread_finished  TRUE;
    if( enc_thread->thread_idx  focus_thread )
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
    return enc_thread->ltp_retval;
}


/**/
/**/
int nominal_functionality_test()
{
    flist_t * node;
    int i;
    int rv  TPASS;
    bmp_encoder_thread_t * enc_thread  &codec_thread[0];

    for( node  file_list, i  0; node; node  node->next, ++i )
    {
        reset_encoder_instance( 0 );

        tst_resm( TINFO, "input bitstream: %s", node->inp_fname );

        enc_thread->inp_fname   strcmp( node->inp_fname, "n/a" ) ! 0 ? node->inp_fname : NULL;
        enc_thread->out_fname   strcmp( node->out_fname, "n/a" ) ! 0 ? node->out_fname : NULL;
        enc_thread->ref_fname   strcmp( node->ref_fname, "n/a" ) ! 0 ? node->ref_fname : NULL;

        enc_thread->enc_obj.parameters.width             node->width;
        enc_thread->enc_obj.parameters.height            node->height;
        enc_thread->enc_obj.parameters.cmpr_type         node->cmpr;
        enc_thread->enc_obj.parameters.pformat           node->pfmt;
        enc_thread->enc_obj.parameters.colors_in_output  node->colors_in_output;

        /* Run the encoder. */
        rv + run_codec( enc_thread );
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
    bmp_encoder_thread_t * enc_thread;

    /* Compute total threads number. */
    for( node  file_list, i  0; node && i < MAX_CODEC_THREADS; node  node->next, ++i );
    num_threads  i;

    /* Run all bitstreams in separate threads. */
    for( node  file_list, i  0; node && i < MAX_CODEC_THREADS; node  node->next, ++i )
    {
        reset_encoder_instance( i );
        enc_thread  &codec_thread[i];

        tst_resm( TINFO, "thread #%d, input bitstream: %s", i, node->inp_fname );

        enc_thread->inp_fname   strcmp( node->inp_fname, "n/a" ) ! 0 ? node->inp_fname : NULL;
        enc_thread->out_fname   strcmp( node->out_fname, "n/a" ) ! 0 ? node->out_fname : NULL;
        enc_thread->ref_fname   strcmp( node->ref_fname, "n/a" ) ! 0 ? node->ref_fname : NULL;

        enc_thread->enc_obj.parameters.width             node->width;
        enc_thread->enc_obj.parameters.height            node->height;
        enc_thread->enc_obj.parameters.cmpr_type         node->cmpr;
        enc_thread->enc_obj.parameters.pformat           node->pfmt;
        enc_thread->enc_obj.parameters.colors_in_output  node->colors_in_output;

        if( pthread_create( &enc_thread->tid, NULL, (void*)&run_codec, enc_thread ) )
        {
            tst_resm( TWARN, "re_entrance_test : error creating thread %d", i );
            return TFAIL;
        }
    }

    for( i  0; i < num_threads; ++i )
    {
        enc_thread  &codec_thread[i];
        pthread_join( enc_thread->tid, NULL );
    }

    /* Compute the summary result. */
    for( i  0; i < num_threads; ++i )
    {
        enc_thread  &codec_thread[i];
        rv + enc_thread->ltp_retval;
    }
    return rv;
}


/**/
/**/
int pre_emption_test()
{
    return re_entrance_test();
}


/**/
/**/
int endurance_test()
{
    int i;
    int rv  TPASS;

    for( i  0; i < testapp_cfg.num_iter; ++i )
    {
        tst_resm( TINFO, "The %d iteration is started", i+1 );
        rv + nominal_functionality_test();
        tst_resm( TINFO, "The %d iteration is completed", i+1 );
    }
    return rv;
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
         sleep(2);
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
void reset_encoder_instance( unsigned long thread_idx )
{
    assert( thread_idx < MAX_CODEC_THREADS );

    bmp_encoder_thread_t * enc_thread  &codec_thread[thread_idx];

    BMP_enc_object * enc_obj  &enc_thread->enc_obj;
    memset( enc_obj, 0, sizeof(BMP_enc_object) );

    enc_thread->thread_idx        thread_idx;
    enc_thread->inp_fname       NULL;
    enc_thread->out_fname       NULL;
    enc_thread->ref_fname       NULL;
    enc_thread->inp_fstream       NULL;
    enc_thread->out_fstream       NULL;
    enc_thread->ref_fstream       NULL;
    enc_thread->inp_buffer[0]     NULL;
    enc_thread->inp_buffer[1]     NULL;
    enc_thread->inp_buffer[2]     NULL;
    enc_thread->out_buffer        NULL;
    enc_thread->inp_buffer_sz     0;
    enc_thread->out_buffer_sz     0;
    enc_thread->endflag           FALSE;
    enc_thread->rows_rewind       0;
    enc_thread->row_pixels        0;
    enc_thread->thread_finished   FALSE;
    enc_thread->bmp_last_error    BMP_ENC_NO_ERROR;
    enc_thread->ltp_retval        TPASS;
}


/**/
/**/
void on_data_output( bmp_encoder_thread_t * enc_thread )
{
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
int bitmatch( bmp_encoder_thread_t * enc_thread )
{
    assert( enc_thread );
    return compare_files( enc_thread->out_fname, enc_thread->ref_fname );
}


/**/
/**/
int get_index_by_encoder( BMP_enc_object * enc_obj )
{
    assert( enc_obj );
    int i;
    for( i  0; i < MAX_CODEC_THREADS; ++i )
    {
        if( &codec_thread[i].enc_obj  enc_obj )
            return i;
    }
    return -1;
}


/**/
/**/
BMPE_error_type app_get_new_data_BMPE( BMPE_UINT8 ** buf_ptr_B, BMPE_UINT8 ** buf_ptr_G ,BMPE_UINT8 ** buf_ptr_R,
                                       BMPE_UINT32 * buf_len, BMP_enc_object * obj_ptr )
{
    int bytes_read  0;
    int b_index  0, g_index  0, r_index  0;

    int thread_idx  get_index_by_encoder( obj_ptr );
    assert( thread_idx > 0 && thread_idx < MAX_CODEC_THREADS );

    bmp_encoder_thread_t * enc_thread  &codec_thread[ thread_idx ];
    BMP_enc_parameters * enc_params  &obj_ptr->parameters;
    FILE * inp_fstream  enc_thread->inp_fstream;
    assert( inp_fstream );

    /* This condition becomes true when inputbuffers are to
       be allocated for the first time.*/
    if( *buf_ptr_BNULL || *buf_ptr_GNULL || *buf_ptr_RNULL )
    {
        enc_thread->inp_buffer[0]  (unsigned char*)alloc_fast( INPUT_BUFFER_SIZE );
        enc_thread->inp_buffer[1]  (unsigned char*)alloc_fast( INPUT_BUFFER_SIZE );
        enc_thread->inp_buffer[2]  (unsigned char*)alloc_fast( INPUT_BUFFER_SIZE );

        if( !enc_thread->inp_buffer[0] || !enc_thread->inp_buffer[1] || !enc_thread->inp_buffer[2] )
        {
            tst_resm( TWARN, "app_get_new_data_BMPE : can't allocate memory for input buffers" );
            return TFAIL;
        }

        *buf_len  INPUT_BUFFER_SIZE;
        *buf_ptr_B  enc_thread->inp_buffer[0];
        *buf_ptr_G  enc_thread->inp_buffer[1];
        *buf_ptr_R  enc_thread->inp_buffer[2];

        return BMP_ENC_NO_ERROR;
    }

    while( r_index ! INPUT_BUFFER_SIZE )
    {
        /* Read data from the file till end of the row is reached */
        if( (enc_thread->row_pixels % enc_params->width ! 0 || enc_thread->row_pixels  0) && !feof(inp_fstream) )
        {
            fread( &enc_thread->inp_buffer[0][r_index++], sizeof(char), 1, inp_fstream );
            fread( &enc_thread->inp_buffer[1][g_index++], sizeof(char), 1, inp_fstream );
            fread( &enc_thread->inp_buffer[2][b_index++], sizeof(char), 1, inp_fstream );
            bytes_read++;
            enc_thread->row_pixels++;
        }
        /* If end of the row is reached reposition file pointer
           to continue reading data. */
        else
        {
            /* Reset the row pixels value */
            enc_thread->row_pixels  0;

            /* When all rows have been read */
            if( enc_params->height < enc_thread->rows_rewind )
            {
                /* If some data from the topmost row is read push to the
                   input buffer */
                if( bytes_read )
                {
                    enc_thread->row_pixels  enc_params->width;
                    break;
                }
                /* All the data from the file is read */
                else
                {
                    enc_thread->endflag  TRUE;
                    return BMP_ENC_ERR_IO_ERROR;
                }
            }
            /* Reposition the file pointer to start of the row */
            fseek( inp_fstream, enc_params->width*(enc_params->height-enc_thread->rows_rewind)*3, SEEK_SET );
            enc_thread->rows_rewind++;
        }
    }

    if( bytes_read )
    {
        *buf_len    bytes_read;
        *buf_ptr_B  enc_thread->inp_buffer[0];
        *buf_ptr_G  enc_thread->inp_buffer[1];
        *buf_ptr_R  enc_thread->inp_buffer[2];

        return BMP_ENC_NO_ERROR;
    }

    return BMP_ENC_NO_ERROR;
}


/**/
/**/
BMPE_error_type app_push_output_BMPE( BMPE_UINT8 ** out_buf_ptr, BMPE_UINT32 * out_buf_len, BMP_enc_object * obj_ptr )
{
    BMPE_UINT32 size_outputbuffer0,bytes_written0;
    size_outputbuffer  *out_buf_len;

    int thread_idx  get_index_by_encoder( obj_ptr );
    assert( thread_idx > 0 && thread_idx < MAX_CODEC_THREADS );

    bmp_encoder_thread_t * enc_thread  &codec_thread[ thread_idx ];
    FILE * out_fstream  enc_thread->out_fstream;
    assert( out_fstream );

    /* This condition becomes true when first call to this function is made.
       Output buffer is allocated memory. */
    if( *out_buf_ptr  NULL )
    {
        *out_buf_len  OUTPUT_BUFFER_SIZE;
        size_outputbuffer  *out_buf_len;
        enc_thread->out_buffer  (unsigned char*)alloc_fast(size_outputbuffer);
        enc_thread->out_buffer_sz  size_outputbuffer;

        if( !enc_thread->out_buffer )
        {
            tst_resm( TWARN, "app_push_output_BMPE : can't allocate memory for the output buffer" );
            return BMP_ENC_ERR_NO_MEMORY;
        }

        enc_thread->out_buffer_sz  size_outputbuffer;
        *out_buf_ptr  enc_thread->out_buffer;

        return BMP_ENC_NO_ERROR;
    }

    /* Write the buffer data to the file */
    bytes_written  fwrite( *out_buf_ptr, sizeof(char), size_outputbuffer, out_fstream );

    return BMP_ENC_NO_ERROR;
}


/**/
/**/
BMPE_error_type app_get_new_data_BMPE_gray(BMPE_UINT8 **buf_ptr_G,BMPE_UINT32 *buf_len, BMP_enc_object *obj_ptr)
{
    int bytes_read  0;
    int gray_index  0;

    int thread_idx  get_index_by_encoder( obj_ptr );
    assert( thread_idx > 0 && thread_idx < MAX_CODEC_THREADS );

    bmp_encoder_thread_t * enc_thread  &codec_thread[ thread_idx ];
    BMP_enc_parameters * enc_params  &obj_ptr->parameters;
    FILE * inp_fstream  enc_thread->inp_fstream;
    assert( inp_fstream );

    /* This condition becomes true when inputbuffers are to
       be allocated for the first time.*/
    if( *buf_ptr_G  NULL )
    {
        enc_thread->inp_buffer[0]  (unsigned char*)alloc_fast( INPUT_BUFFER_SIZE );

        if( !enc_thread->inp_buffer[0] )
        {
            tst_resm( TWARN, "app_get_new_data_BMPE_gray : can't allocate memory for input buffers" );
            return TFAIL;
        }

        *buf_len  INPUT_BUFFER_SIZE;
        *buf_ptr_G  enc_thread->inp_buffer[0];

        return BMP_ENC_NO_ERROR;
    }

    while( gray_index ! INPUT_BUFFER_SIZE )
    {
        /* Read data from the file till end of the row is reached */
        if( (enc_thread->row_pixels % enc_params->width ! 0 || enc_thread->row_pixels  0) && !feof(inp_fstream) )
        {
            fread( &enc_thread->inp_buffer[0][gray_index++], sizeof(char), 1, inp_fstream );
            bytes_read++;
            enc_thread->row_pixels++;
        }
        /* If end of the row is reached reposition file pointer
           to continue reading data. */
        else
        {
            /* Reset the row pixels value */
            enc_thread->row_pixels  0;

            /* When all rows have been read */
            if( enc_params->height < enc_thread->rows_rewind )
            {
                /* If some data from the topmost row is read push to the
                   input buffer */
                if( bytes_read )
                {
                    enc_thread->row_pixels  enc_params->width;
                    break;
                }
                /* All the data from the file is read */
                else
                {
                    enc_thread->endflag  TRUE;
                    return BMP_ENC_ERR_IO_ERROR;
                }
            }
            /* Reposition the file pointer to start of the row */
            fseek( inp_fstream, enc_params->width*(enc_params->height-enc_thread->rows_rewind), SEEK_SET );
            enc_thread->rows_rewind++;
        }
    }

    if( bytes_read )
    {
        *buf_len    bytes_read;
        *buf_ptr_G  enc_thread->inp_buffer[0];

        return BMP_ENC_NO_ERROR;
    }

    return BMP_ENC_NO_ERROR;
}

int DebugLogText(short int msgid,char *fmt,...)
{
  return 1;
}

int DebugLogData(short int msgid,void *ptr,int size)
{
    return 1;
}

#ifdef __cplusplus
}
#endif

