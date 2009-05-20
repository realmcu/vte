/*
  Copyright (C) 2004, Freescale Semiconductor, Inc. All Rights Reserved
  THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
  BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
  Freescale Semiconductor, Inc.
*/

/**
@file gif_decoder_test.c

@brief VTE C source template

Description of the file

@par Portability:
        Indicate if this module is portable to other compilers or platforms.
        If not, indicate specific reasons why is it not portable.
*/

/* REVISION HISTORY 

Author (core ID)      Date         CR Number    Description of Changes
-------------------   ----------   ----------   ------------------------------
 E.Gromazina           25/02/2005  TLSbo47116   Initial version.
 D.Simakov / smkd001c  06/05/2005  TLSbo47116   The endurance and load test
                                                cases were added. The framebuffer's
                                                support was added.
 A.Pshenichnikov       07/12/2005  TLSbo59709   bugs with the preemtive and
      reentrance test cases were fixed
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
#include <stdio.h>

/* Harness Specific Include Files. */
#include "test.h"

/* Verification Test Environment Include Files */
#include "gif_decoder_test.h"

/* Subs Include Files */
#include "sub/pars_conf.h"
#include "sub/fb_draw_api.h"
#include "sub/mem_stat.h"
/*
                                        LOCAL MACROS
*/

#define MAX_CHUNK_BUF 4096

#ifdef DEBUG_TEST
extern long  max_size;
#define SAFE_DELETE(p) { if ( p ) { MemStat_Free(p); p  NULL;}}
#define PRINT_MEM { printf ( "maximum size of used memory : %lu bytes %f MB\n", max_size, ( float ) max_size / 1048576 );}
#else
#define SAFE_DELETE(p) {if(p){free(p);pNULL;}}
#endif

//#define DEBUG_TEST

/*
                          LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
*/

typedef struct
{
    /**********************************************
     * Configurable parameters (public interface) *
     **********************************************/
    GIF_Decoder_Object * gif_dec_obj;
    flist_t *node;

    char * inp_buf;
    unsigned char *screen_buffer;

    FILE * inp_fstream;
    FILE * out_fstream;

    pthread_t tid;
}gif_decoder_thread_t;

/*
                                       LOCAL VARIABLES
*/
static     gif_decoder_thread_t codec_thread[ MAX_CODEC_THREADS ];
static int test_iter  DEFAULT_ITERATIONS; /* default iteration is hard coded */
static int test_case  NOMINAL_FUNCTIONALITY;
static int thread_synchro  FALSE;    /* boolean used by the loop thread to inform the thread */

static flist_t * file_list  NULL;               /* list of input/output/reference files */

#ifdef DEBUG_TEST
  FILE * error_log;   /*  log file for errors  */
#endif

static int cout_timer_sign0;

/*
                                       GLOBAL CONSTANTS
*/


/*
                                       GLOBAL VARIABLES
*/

/*
                                   LOCAL FUNCTION PROTOTYPES
*/
/* helper functions */
int close_decoder_files( gif_decoder_thread_t * enc_thread );

gif_decoder_thread_t *get_ThreadID(GIF_Decoder_Object * gif_dec_obj);
int  init_param (gif_decoder_thread_t * enc_thread);
int run_decoder (gif_decoder_thread_t * enc_thread);
void clean_allocated_memory (gif_decoder_thread_t * enc_thread);
int bitmach( const char * out_fname, const char * ref_fname );
void *run_decoder_in_loop( void * ptr );
void  reset_decoder_instance(gif_decoder_thread_t * enc_thread );
void timer_handler (int signum);
int run_decoder_on_timer( void * ptr );

/* test functions */
int nominal_functionality_test();
int reentrance_test  (flist_t * node);
int preemption_test(flist_t * node);
int endurance_test(flist_t * node);
int load_test(flist_t * node);

/*
                                       LOCAL FUNCTIONS
*/


/**/
/* VT_gif_decoder_setup */
/**
@brief  assumes the pre-condition of the test case execution

@param  None

@return On success - return TPASS
        On failure - return the error code
*/
/**/
int VT_gif_decoder_setup()
{
    int rv  TFAIL;

    /** insert your code here */

    rv  TPASS;
    return rv;
}


/**/
/* VT_gif_decoder_cleanup */
/**
@brief  assumes the post-condition of the test case execution

@param  None

@return On success - return TPASS
        On failure - return the error code
*/
/**/
int VT_gif_decoder_cleanup()
{
    int rv  TFAIL;

    int i;

    /* close all file descriptors */
    for( i  0; i < MAX_CODEC_THREADS; ++i )
    {
      if (&codec_thread[i])
          close_decoder_files( &codec_thread[i] );
    }

    if( file_list )
        delete_list( file_list );

    rv  TPASS;
    return rv;
}


/**/
/* VT_gif_decoder_test */
/*
@brief  Template test scenario function

@param  testcase - Testcase id of the test according to the test plan \n
 iter     - Iteration of the loop in case of an endurance/stress test

@return On success - return TPASS
        On failure - return the error code
*/
/**/

int VT_gif_decoder_test( int testcase, int iter, char * cfg_file )
{
    int rv  TFAIL;
    char c_f[80];

    test_iter  iter;
    test_case  testcase;

    if(!cfg_file)
      strcpy(c_f, "opts_cfg");
    else
      strcpy(c_f, cfg_file);

    if( file_list )
        delete_list( file_list );

    if( !read_cfg( c_f, &file_list ) )
    {
  tst_resm( TWARN, "VT_gif_decoder_test : can't parse %s", c_f );
        return rv;
    }

#ifdef DEBUG_TEST
     error_log  fopen( "./output/f_log", "wb" );
     if(error_logNULL)
           printf ("\nError openning log file \n");
#endif

     switch( testcase )
    {
        case NOMINAL_FUNCTIONALITY:
     tst_resm( TINFO, "Nominal functionality test");
            rv  nominal_functionality_test();
     tst_resm( TINFO, "End nominal functionality test");
            break;

        case RE_ENTRANCE:
     tst_resm( TINFO,"Re-entrance test" );
            rv  reentrance_test(file_list);
     tst_resm( TINFO,"End re-entrance test" );
            break;

  case PRE_EMPTION:
     tst_resm( TINFO,"Pre-emption test" );
            rv  preemption_test(file_list);
     tst_resm( TINFO,"End pre-emption test" );
            break;

        case ENDURANCE:
            tst_resm( TINFO, "Endurance test" );
            rv  endurance_test(file_list);
            tst_resm( TINFO, "End of the endurance test" );
            break;

        case LOAD:
            tst_resm( TINFO, "Load test" );
            rv  load_test(file_list);
            tst_resm( TINFO, "End of the load test" );
            break;

        default:
     tst_resm(TWARN,"Wrong test case" );

            break;
    }

#ifdef DEBUG_TEST
     if (error_log)
            fclose(error_log);
#endif

    return rv;
}


int close_decoder_files( gif_decoder_thread_t * enc_thread )
{
    assert( enc_thread );

    int rv ;
    /* input file */
    if( enc_thread->inp_fstream )
    {
        fclose( enc_thread->inp_fstream );
        enc_thread->inp_fstream  NULL;

    }

    rv  TPASS;
    return rv;
}


gif_decoder_thread_t * get_ThreadID(GIF_Decoder_Object * gif_dec_obj)
{
    int i;

    for (i  0;i < MAX_CODEC_THREADS; i++)
    if (codec_thread[i].gif_dec_obj  gif_dec_obj)
        return &codec_thread[i];

     return NULL;
}

/*******************************************************************************
 *
 *   FUNCTION NAME - GIF_get_new_data
 *
 *   ARGUMENTS     - new_buf_ptr : address of new buffer pointer
 *                 - new_buf_len : GIF_UINT32 type pointer to numbytes read.
 *                 - gif_dec_obj : GIF decoder object
 *
 *   RETURN VALUE  - GIFD_RET_TYPE : Error value
 *
 *******************************************************************************
 *
 *   DESCRIPTION
 *   Read bytes from the file of the input buffer size in buffer.
 ******************************************************************************/

GIFD_RET_TYPE GIF_get_new_data (GIF_UINT8 **new_buf_ptr,GIF_UINT32 *new_buf_len,GIF_Decoder_Object *gif_dec_obj)
{
    int bytes_read;
    gif_decoder_thread_t * enc_thread  get_ThreadID(gif_dec_obj);

    if (enc_thread  NULL)
        return GIF_ERR_EOF;

    bytes_read  fread(enc_thread->inp_buf,sizeof(char),MAX_CHUNK_BUF, enc_thread->inp_fstream);

    if(bytes_read)
    {
        *new_buf_len  bytes_read;
        *new_buf_ptr  enc_thread->inp_buf;
        return GIF_ERR_NO_ERROR;
    }

    else
    {
        return GIF_ERR_EOF;
    }
}

/**/
/* init_param */
/**

@param  gif_decoder_thread_t *

@return On success - return TRUE
        On failure - return the FALSE
*/
/**/

int  init_param(gif_decoder_thread_t * enc_thread)
{
        char imf[MAX_STR_LEN];
        int rv  FALSE;

        enc_thread->gif_dec_obj  (GIF_Decoder_Object *)alloc_fast/*malloc*/(sizeof(GIF_Decoder_Object));

        if (enc_thread->gif_dec_obj  NULL)
            return FALSE;

        memset( enc_thread->gif_dec_obj, 0, sizeof(GIF_Decoder_Object) );
        enc_thread->gif_dec_obj->dec_param.out_format  enc_thread->node->rgb_format;
        enc_thread->gif_dec_obj->dec_param.scale_mode  enc_thread->node->skale_mod;
        enc_thread->gif_dec_obj->dec_param.output_height  enc_thread->node->height;
        enc_thread->gif_dec_obj->dec_param.output_width  enc_thread->node->width;
        enc_thread->gif_dec_obj->GIF_get_new_data  GIF_get_new_data;
        enc_thread->inp_buf  (unsigned char *)alloc_fast/*malloc*/(MAX_CHUNK_BUF);

        if (enc_thread->inp_buf NULL)
            return FALSE;

        memset(imf,0,MAX_STR_LEN);
        sprintf(imf,"%s%s%s",enc_thread->node->inp_dir, enc_thread->node->im_file, ".gif");

         enc_thread->inp_fstream  fopen(imf,"rb");

        if(enc_thread->inp_fstreamNULL)
        {
     tst_resm( TWARN,"Error Opening input GIF file %s",imf);
            return rv;
        }

        return TRUE;

}

/**/
/**/
void detect_enter()
{
    extern int delay_value;
    if( !delay_value ) return;

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

/**/
/**/
void draw_frame( gif_decoder_thread_t * dec_thread, int left, int top )
{
    assert( dec_thread );
    assert( dec_thread->gif_dec_obj );
    assert( dec_thread->screen_buffer );

    const framebuffer_t * fb  get_framebuffer();
    assert( fb );

    GIF_Decoder_Info_Init * dec_info_init  &dec_thread->gif_dec_obj->dec_info_init;
    int width  dec_info_init->out_image_width;
    int height  dec_info_init->out_image_height;
    pixel_format_e pfmt;

    int scanline_pitch  0;
    unsigned char * scanline  dec_thread->screen_buffer;

    switch( dec_thread->gif_dec_obj->dec_param.out_format )
 {
         case E_OUTPUTFORMAT_RGB555:
  case E_OUTPUTFORMAT_RGB565:
            scanline_pitch  2 * width;
            pfmt  PF_RGB_565;
           break;

  case E_OUTPUTFORMAT_RGB888:
            scanline_pitch  3 * width;
            pfmt  PF_RGB_888;
   break;
  default:
  fprintf ( stderr, "warning : unsupported pixel format\n");
  return;
    }

    if( 0  scanline_pitch )
        return;

    int i;
    for( i  0; i < height; i++ )
    {
        fb->draw_scanline( scanline, left, top + i, width, pfmt );
        scanline + scanline_pitch;
    }
}

/**/
/* run_decoder */
/**
@brief  This method called by a special codec thread decode in loop the same bitstreams.

@param  ptr - TBD

@return On success - return TPASS
        On failure - return the error code
*/
/**/
int run_decoder (gif_decoder_thread_t *enc_thread)
{
        GIFD_RET_TYPE res;
        int rv  TFAIL;
        int index, memreq;
        int glob_mem_index0;
        int mem_out_pix;
        char imf[MAX_STR_LEN];
        int mult_factor3, scale_flag  FALSE;
        int counter  0;

        if (! (rv  init_param (enc_thread)))
        {
     tst_resm( TWARN,"Init param for decoder error ");
            return rv;
        }
        resGIF_query_dec_mem (enc_thread->gif_dec_obj);

        if(res!GIF_ERR_NO_ERROR)
        {
     tst_resm( TWARN,"Memory Query for decoder error: input file - %s",enc_thread->node->im_file);
            return rv;
        }

        for( memreq0; memreq<enc_thread->gif_dec_obj->mem_info.num_reqs; memreq++ )
         {
            if (enc_thread->gif_dec_obj->mem_info.mem_info_sub[memreq].type  E_FAST_MEMORY)
            {      /* Check for priority and memory description can be added here */
                    enc_thread->gif_dec_obj->mem_info.mem_info_sub[memreq].ptr
                        alloc_fast(enc_thread->gif_dec_obj->mem_info.mem_info_sub[memreq].size);
            }
            else
                    enc_thread->gif_dec_obj->mem_info.mem_info_sub[memreq].ptr
                                   alloc_slow(enc_thread->gif_dec_obj->mem_info.mem_info_sub[memreq].size);

              if(enc_thread->gif_dec_obj->mem_info.mem_info_sub[memreq].ptrNULL)
              {
      tst_resm( TWARN,"Malloc error after query");
                    return rv;
              }
        }

        glob_mem_indexenc_thread->gif_dec_obj->mem_info.num_reqs;

      /*Seek to the start of the file befor calling init*/

        if(fseek(enc_thread->inp_fstream, 0, SEEK_SET) ! 0)
        {
                return rv;
        }

        /*Initialization of decoder for global data*/
        res  GIF_decoder_init(enc_thread->gif_dec_obj);

        if(res! GIF_ERR_NO_ERROR)
        {
  tst_resm( TWARN,"error GIF_decoder_init ");
                return rv;
        }

        /*Decode frame by frame.Break from the loop
          when Terminator block is reached */

        while(1)
        {

        /*Query for memory requirements of a frame*/
     resGIF_query_dec_mem_frame(enc_thread->gif_dec_obj);

             if (res  GIF_ERR_TERMINATOR_REACHED)
  {
   tst_resm( TINFO,"End of GIF file reached. Input file - %s",enc_thread->node->im_file);
   break;
  }
  else if(res !GIF_ERR_NO_ERROR)
  {
   tst_resm( TWARN,"Error in querying memory requirments for a frame");
   return rv;

  }
     for(memreqglob_mem_index;memreq<enc_thread->gif_dec_obj->mem_info.num_reqs;memreq++)
            {
             if (enc_thread->gif_dec_obj->mem_info.mem_info_sub[memreq].type  E_FAST_MEMORY)
                   enc_thread->gif_dec_obj->mem_info.mem_info_sub[memreq].ptr
                                alloc_fast(enc_thread->gif_dec_obj->mem_info.mem_info_sub[memreq].size);
   else
    enc_thread->gif_dec_obj->mem_info.mem_info_sub[memreq].ptr
                                alloc_slow(enc_thread->gif_dec_obj->mem_info.mem_info_sub[memreq].size);

                   if (enc_thread->gif_dec_obj->mem_info.mem_info_sub[memreq].ptrNULL)
                   {
    tst_resm( TWARN,"Malloc error after query");
                         return rv;
                   }
              }

  /*Initializiation for a frame*/
  resGIF_decoder_init_frame(enc_thread->gif_dec_obj);

  if(res ! GIF_ERR_NO_ERROR)
  {
   tst_resm( TWARN,"Error in initialization of frame data");
   return rv;
  }

  switch ( enc_thread->gif_dec_obj->dec_param.out_format)
  {
   case E_OUTPUTFORMAT_RGB555:
         mult_factor2; strcpy(enc_thread->node->rgb_str,"RGB555");
           break;

              case E_OUTPUTFORMAT_RGB565:
      mult_factor2; strcpy(enc_thread->node->rgb_str,"RGB565");
               break;

     case E_OUTPUTFORMAT_RGB666:
       mult_factor4; strcpy(enc_thread->node->rgb_str,"RGB666");
    break;

   case E_OUTPUTFORMAT_RGB888:
       mult_factor3; strcpy(enc_thread->node->rgb_str,"RGB888");
    break;

     default:
    mult_factor3; strcpy(enc_thread->node->rgb_str,"RGB888");

  }

  if (enc_thread->gif_dec_obj->dec_param.scale_mode  E_INT_SCALE_PRESERVE_AR)
                        scale_flag  TRUE;
  else
                        scale_flag  FALSE;

              enc_thread->node->skale_mod  scale_flag;

       mem_out_pix  enc_thread->gif_dec_obj->dec_info_init.out_image_width
                       * enc_thread->gif_dec_obj->dec_info_init.out_image_height
                       * mult_factor;

  enc_thread->screen_buffer  alloc_fast /*malloc*/(mem_out_pix);

  if(enc_thread->screen_buffer   NULL)
  {
   tst_resm( TWARN," out_image memory allocation error ");
   return rv;
  }

  /*Decode a frame*/
  res  GIF_decode(enc_thread->gif_dec_obj,enc_thread->screen_buffer);

  if(res !GIF_ERR_DECODING_COMPLETE)
  {
   tst_resm( TWARN,"Error in decoding frame data");
   return rv;
  }

        /*************************************/
        /*              DRAWING              */
        /*************************************/

        if( /*0 &&*/ test_case ! RE_ENTRANCE && test_case ! PRE_EMPTION )
        {
            const framebuffer_t * fb  get_framebuffer();
            assert( fb );

            /* clear screen */
            argb_color_t white  {1.0f, 1.0f, 1.0f, 1.0f};
            fb->clear_screen( &white );

            /* draw frame */
            draw_frame( enc_thread, 0, 0 );

            detect_enter();
        }

  tst_resm( TINFO,"Decoding frame done successfully ");

  /*Write decoded output of the frame to a file.
    The output filename(corresponding to each frame)
    is prefixed with the frame number*/

  memset(imf,0,MAX_STR_LEN);


              if( ( test_case  RE_ENTRANCE ) || ( test_case  PRE_EMPTION ) )
       {
  if (scale_flag)
                        sprintf(imf,"%s%d_%s_%d_%s_%s%s",enc_thread->node->out_dir, counter, enc_thread->node->im_file, enc_thread - codec_thread, "scale", enc_thread->node->rgb_str, ".ppm");
                else
                        sprintf(imf,"%s%d_%s_%d_%s%s",enc_thread->node->out_dir, counter, enc_thread->node->im_file, enc_thread - codec_thread, enc_thread->node->rgb_str, ".ppm");
       }
       else
       {
  if (scale_flag)
                        sprintf(imf,"%s%d_%s_%s_%s%s",enc_thread->node->out_dir, counter, enc_thread->node->im_file, "scale", enc_thread->node->rgb_str, ".ppm");
                else
                        sprintf(imf,"%s%d_%s_%s%s",enc_thread->node->out_dir, counter, enc_thread->node->im_file, enc_thread->node->rgb_str, ".ppm");
       }
              enc_thread->out_fstream  fopen(imf,"wb");
#ifdef DEBUG_TEST
       printf ( "output file  %s thread_id   %d\n", imf, pthread_self() );
#endif
       if(enc_thread->out_fstreamNULL)
         {
          printf("Error Opening out GIF file");
         return rv;
        }

                fprintf(enc_thread->out_fstream, "%s\n", "P6");

  fprintf(enc_thread->out_fstream, "%d %d\n", enc_thread->gif_dec_obj->dec_info_init.out_image_width,
   enc_thread->gif_dec_obj->dec_info_init.out_image_height);

  fprintf(enc_thread->out_fstream, "%d\n", 255);
  fflush(enc_thread->out_fstream);

  fwrite(enc_thread->screen_buffer, sizeof(char), (unsigned int)(mem_out_pix), (enc_thread->out_fstream));

  fclose(enc_thread->out_fstream);
#ifdef DEBUG_TEST
 MemStat_GetStat ( );
#endif

  SAFE_DELETE(enc_thread->screen_buffer);

  /*Free up the local memory info pointers*/
  for(index  glob_mem_index; index < enc_thread->gif_dec_obj->mem_info.num_reqs; index++)
  {
                  SAFE_DELETE(enc_thread->gif_dec_obj->mem_info.mem_info_sub[index].ptr);
  }
  counter++;

 }//end while(1)

 enc_thread->node->count_frames  counter;
 if (enc_thread->inp_fstream)
            fclose(enc_thread->inp_fstream);
 enc_thread->inp_fstream  NULL;
 return TPASS;
}

/**/
/* bitmach */
/**
@brief

@param  out_fname, ref_fname

@return On success - return TRUE
        On failure - return the FALSE
*/
/**/

int bitmach( const char * out_fname, const char * ref_fname )
{
    FILE * pf1, *pf2;
    int rd1, rd2;
    unsigned char b1, b2;

    if( out_fname && ref_fname )
    {

        pf1  fopen( out_fname, "rb" );
        pf2  fopen( ref_fname, "rb" );

        if( pf1 && pf2 )
        {
     while( TRUE )
     {

  if( feof(pf1) ! feof(pf2) )
      break; /* bitmach fails */

  rd1  fread( &b1, 1, sizeof(b1), pf1 );
  rd2  fread( &b2, 1, sizeof(b2), pf2 );

  if( b1 ! b2 )
      break; /* bitmach fails */

  if( feof(pf1) && feof(pf2) )
  {
      fclose( pf1 );
      fclose( pf2 );
      return TRUE;
  }
     }
 }

 if( pf1 ) fclose( pf1 );
 if( pf2 ) fclose( pf2 );
    }

    return FALSE;
}


/**/
/* nominal_functionality_test */
/**
@brief  Testing of a nominal functionality of a dencoder.

@param  None.

@return On success - return TPASS
        On failure - return the error code
*/
/**/
int nominal_functionality_test()
{
    flist_t * node;
    int i;
    char out_file[MAX_STR_LEN], ref_file[MAX_STR_LEN];
    int rv  TPASS;

#ifdef DEBUG_TEST
   if (error_log)
         fprintf(error_log,"Nominal functionality \n");
#endif

    memset( &codec_thread[0], 0, sizeof(gif_decoder_thread_t)*MAX_CODEC_THREADS);

    node  file_list;
    while( node )
    {
       codec_thread[0].node  node;
       //if (( rv + run_decoder (&codec_thread[0])) ! TPASS)
    // tst_resm( TWARN,"Error run decoder");
       /* Don't print any intermediate results here!
          It will be printed in the run_decoder, if an error occurs.
        */
       /* does it look cool ? */
       rv + run_decoder( &0[codec_thread] );

       clean_allocated_memory(&codec_thread[0]);

        /* perform compare files */
        for ( i  0; i < node->count_frames; i++)
        {
                if (node->skale_mod)
                        sprintf(out_file,"%s%d_%s_%s_%s%s",node->out_dir, i, node->im_file, "scale", node->rgb_str, ".ppm");
                else
                        sprintf(out_file,"%s%d_%s_%s%s",node->out_dir, i, node->im_file, node->rgb_str, ".ppm");

                if (node->skale_mod)
                        sprintf(ref_file,"%s%d_%s_%s_%s%s",node->ref_dir, i, node->im_file, "scale", node->rgb_str, ".ppm");
                else
                        sprintf(ref_file,"%s%d_%s_%s%s", node->ref_dir, i, node->im_file, node->rgb_str, ".ppm");

                FILE * tst_ref  fopen( ref_file, "r" );
                if( tst_ref )
                {
                    fclose( tst_ref );
                    if(!bitmach( out_file, ref_file))
                    {
               tst_resm( TWARN, "bitmatch failed" );

                        rv  TFAIL;

                    #ifdef DEBUG_TEST
                        if (error_log)
                      fprintf(error_log,"ERROR in bitmach: out_file - %s    ref_file - %s \n",out_file, ref_file);
                    #endif
                    }
                    else
                    {
                        tst_resm( TINFO, "bitmacth passed" );
                    }
          }
                else
                {
                #ifdef DEBUG_TEST
                    if (error_log)
                  fprintf(error_log,"ref_file - %s was not found\n", ref_file);
                #endif
                }
         }
         node  node->next;
    }

#ifdef DEBUG_TEST
    if (error_log)
         fprintf(error_log,"End of nominal functionality \n");
    PRINT_MEM
#endif

    return rv;
}

/**/
/* run_codec_in_loop */
/**
@brief  This method called by a special codec thread decode/encode in loop the same bitstreams.

param  Input:  ptr - pointer to the structure holding buffers, encoder config structure etc.
        Output: None

@return no
*/
/**/
void *run_decoder_in_loop( void * ptr )
{
    int i, retval  0;
    gif_decoder_thread_t * enc_thread  (gif_decoder_thread_t*)ptr;

   for( i  0; i < test_iter; ++i )
    {

            retval  +run_decoder(enc_thread);
            clean_allocated_memory(enc_thread);
    }

    /* Set that boolean that is a global variable of the main process */
    /* to inform the second thread that the 1st one has ended. */
    /* It allows the 2nd thread to terminate. */
    thread_synchro  TRUE;

    return NULL;
}


/**/
/* re_entrance_test */
/**
@brief  Re-entrance means there should not be any static data or any global
 variables used in the code. Test this ability.

@param  flist_t *.

@return On success - return TPASS
        On failure - return the error code
*/
/**/

int reentrance_test(flist_t * node)
{
    int    j, i;
    int    rv  TPASS;
    flist_t * node_n;
    void *res;

    char out_file[MAX_STR_LEN], ref_file[MAX_STR_LEN];

#ifdef DEBUG_TEST
    if (error_log)
         fprintf(error_log,"Reentrance test\n");
#endif

    for ( i0,node_n  node;  (node_n!NULL)   &&  (i< MAX_CODEC_THREADS)  ;  i++, node_n   node_n ->next)
    {
        codec_thread[i].node  node_n;
        printf ( "thread_id  %d : file  %s\n", i, codec_thread[ i ].node->im_file );
 if( pthread_create( &codec_thread[i].tid, NULL,(void *)&run_decoder_in_loop,
                        (void *)&codec_thread[i] ))
         {
     tst_resm( TWARN,"ERROR: cannot create thread %d", i);

            rv  TFAIL;
            break;
         }
    }

    for ( i0,node_n  node;  (node_n!NULL)   &&  (i< MAX_CODEC_THREADS)  ;  i++,node_n   node_n ->next)
    {
     tst_resm( TINFO,"wait for %dst thread to end",i );
            pthread_join( codec_thread[i].tid, &res );
     tst_resm( TINFO,"We have waited for %dst thread to end",i );

           for ( j  0; j < node_n->count_frames; j++)
            {
                if (node_n->skale_mod)
                {
             sprintf(out_file,"%s%d_%s_%d_%s_%s%s",node_n->out_dir, j, node_n->im_file, i, "scale", node_n->rgb_str, ".ppm");
   sprintf(ref_file,"%s%d_%s_%s_%s%s",node_n->ref_dir, j, node_n->im_file, "scale", node_n->rgb_str, ".ppm");
                }
                else
                {
                    sprintf(out_file,"%s%d_%s_%d_%s%s", node_n->out_dir, j, node_n->im_file, i, node_n->rgb_str, ".ppm");
      sprintf(ref_file,"%s%d_%s_%s%s", node_n->ref_dir, j, node_n->im_file, node_n->rgb_str, ".ppm");
                }

                FILE * tst_ref  fopen( ref_file, "r" );
                if(  tst_ref )
                {
                    fclose( tst_ref );
                    if(!bitmach( out_file, ref_file))
                    {
                       tst_resm( TWARN, "bitmatch failed" );
                        rv  TFAIL;
                    #ifdef DEBUG_TEST
                        if (error_log)
                            fprintf(error_log, "ERROR in bitmach in thread - %d : out_file - %s    ref_file - %s \n",
                                    i, out_file, ref_file);
                    #endif
                    }
                    else
                    {
                        tst_resm( TINFO, "bitmach passed" );
                    }
                }
                else
  {
   tst_resm ( TWARN, "can't open reference file %s: %s\n", ref_file, strerror ( errno ) );
  }
       }

    }
#ifdef DEBUG_TEST
    if (error_log)
         fprintf(error_log,"End of reentrance test\n");
PRINT_MEM
#endif

    return rv;
}


/**/
/* timer_handler*/
/**
@brief This is a timer handler.

@param signum - signal number

@return None
*/
/**/
void timer_handler( int signum )
{
    cout_timer_sign++;
}

/**/
/* run_codec_on_timer */
/**
@brief  This method is for a thread working on a timer for preemption test.

@param  None

@return On success - return TPASS
        On failure - return the error code
*/
/**/
int run_decoder_on_timer( void * ptr )
{
    static int pending  FALSE;
    static int retval  0;

    struct sigaction sa;
    struct itimerval timer;
    int cout_timer0;

    gif_decoder_thread_t * enc_thread  (gif_decoder_thread_t*)ptr;

    /* Install the timer handler... */
    memset( &sa, 0, sizeof (sa) );
    sa.sa_handler &timer_handler;

    sigaction( SIGALRM, &sa, NULL );

    /* Configure the timer to expire every 100 msec...  */
    timer.it_value.tv_sec      0; /* First timeout */
    timer.it_value.tv_usec     100000; /* 100000 */
    timer.it_interval.tv_sec   0;  /* Interval */
    timer.it_interval.tv_usec  100000; /* 100000 */

    /* Start timer...  */
    setitimer( ITIMER_REAL, &timer, NULL );

    while(!thread_synchro)
     {
      if(cout_timer!cout_timer_sign)
       {
        if( !pending )
         {
          pending  TRUE;
          retval +run_decoder(enc_thread);
     clean_allocated_memory(enc_thread);
          pending  FALSE;
         }
        else printf("Picture encode skipped, timer too fast!!! \n" );
        fflush( stdout );
        cout_timer_sign0;
       }
     }
    /* if the loop thread ends, terminate the thread working with timer */
    pthread_exit( &retval );

    return TPASS;
}

/**/
/* pre_emption_test */
/**
@brief  The codec should function correctly in a pre-emptive environment. Test this ability.

@param  flist_t *.

@return On success - return TPASS
        On failure - return the error code
*/
/**/
int preemption_test(flist_t * node)
{
    /* TODO : insert codec_thread[0/1] configuration here */

    flist_t * node_n;
#ifdef DEBUG_TEST
    if (error_log)
         fprintf(error_log,"Pre-emption test\n");
#endif
    for ( node_n  node;  (node_n ! NULL)   ; node_n   node_n ->next)
    {
   codec_thread[0].node  node_n;
   codec_thread[1].node  node_n;
   if( pthread_create( &codec_thread[0].tid, NULL, (void *)&run_decoder_in_loop, (void *)&codec_thread[0] ) )
    {
  tst_resm( TWARN,"pre_emption_test : error creating thread 0" );
  return TFAIL;
     }
   if( pthread_create( &codec_thread[1].tid, NULL, (void *)&run_decoder_on_timer, (void *)&codec_thread[1] ) )
    {
  tst_resm( TWARN,"pre_emption_test : error creating thread 1" );
  return TFAIL;
    }
#ifdef DEBUG_TEST
    printf ( "input file: %s\n",      node_n->im_file );
   printf ( "first thread_id %d\n",  codec_thread[0].tid );
   printf ( "second thread_id %d\n", codec_thread[1].tid );
#endif

    /* Wait till threads are complete before main continues. Unless we  */
    /* wait we run the risk of executing an exit which will terminate   */
    /* the process and all threads before the threads have completed.   */
    /* The first thread is expected to die the first one. */

 tst_resm( TINFO,"wait for 1st thread to end");

   pthread_join( codec_thread[0].tid, NULL );

    /* The end of the first thread is triggering the end of the second */
 tst_resm( TINFO,"wait for 2nd thread to end");
   pthread_join( codec_thread[1].tid, NULL );
    thread_synchro  FALSE;
     }
#ifdef DEBUG_TEST
    if (error_log)
         fprintf(error_log,"End of pre-emption test\n");
PRINT_MEM
#endif
    return 0;
}

int endurance_test(flist_t * node)
{
    int i;
    int rv  TPASS;

    for( i  0; i < test_iter; ++i )
    {
        tst_resm( TINFO, "The %d iteration is started", i+1 );
        rv + nominal_functionality_test(node);
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
int load_test(flist_t * node)
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
         rv  nominal_functionality_test(node);
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
/* reset_decoder_instance */
/**
@brief Reset decoder instance

@param  gif_decoder_thread_t *

@return No
*/
/**/
void reset_decoder_instance(gif_decoder_thread_t * enc_thread )
{
        SAFE_DELETE(enc_thread->gif_dec_obj);;
        SAFE_DELETE(enc_thread->inp_buf);
        SAFE_DELETE(enc_thread->screen_buffer);
        if(enc_thread->inp_fstream ! NULL)
     fclose(enc_thread->inp_fstream);
    enc_thread->inp_fstream  NULL;
}


/**/
/* clean_allocated_memory */
/**
@brief Clean memory

@param  gif_decoder_thread_t *

@return No
*/
/**/
void clean_allocated_memory(gif_decoder_thread_t * enc_thread)
{
    int index;

        if(enc_thread->gif_dec_obj)
        {

                for(index  0; index < enc_thread->gif_dec_obj->mem_info.num_reqs; index++)
                {
                    SAFE_DELETE(enc_thread->gif_dec_obj->mem_info.mem_info_sub[index].ptr);
                }
        }
        reset_decoder_instance(enc_thread);
}

#ifdef __cplusplus
}
#endif

