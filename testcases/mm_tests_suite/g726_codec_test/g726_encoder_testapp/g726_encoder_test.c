/*================================================================================================*/
/**
    @file   g726_encoder_test.c

    @brief  Test scenario C source template.
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
D.Simakov / smkd001c         04/11/2004     TLSbo43522  Initial version 
D.Simakov / smkd001c         25/02/2005     TLSbo47116  Update 
D.Simakov / smkd001c         04/04/2005     TLSbo47116  The endurence and load testcases are added.
D.Simakov / smkd001c         18/10/2005     TLSbo57009  Update
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
#include <assert.h>
    
/* Harness Specific Include Files. */
#include "test.h"

/* Verification Test Environment Include Files */
#include "g726_encoder_test.h"

#include "stuff/fconf.h"

/*==================================================================================================
                                        LOCAL MACROS
==================================================================================================*/
#define MAX_CODEC_THREADS 4

#define SAFE_DELETE(p) {if(p){free(p);p=NULL;}}

#define alloc_fast(A)       malloc(A)
#define alloc_slow(A)       malloc(A)

#define NONE_FILE "n/a"

#define BEGIN_G726E_DATA 0

/*==================================================================================================
                          LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
==================================================================================================*/

typedef struct
{
    /**********************************************
     * Configurable parameters (public interface) *
     **********************************************/
     
    unsigned long instance_id;
    
    
    /* The file contains initialization (homing) sequence to
       drive the Encoder to a known initial state
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
     
    eG726EReturnType last_g726D_error;
    
    pthread_t tid;  
    
    sG726EEncoderConfigType * enc_config;
    
    int dec_samples;
    
    int dec_errors_count;
    int thread_finished;
    int ltp_retval;
    
    FILE * hom_fstream;
    FILE * inp_fstream;
    FILE * out_fstream;
    FILE * ref_fstream;        
        
} g726_encoder_thread_t;

/*==================================================================================================
                                       LOCAL CONSTANTS
==================================================================================================*/


/*==================================================================================================
                                       LOCAL VARIABLES
==================================================================================================*/
static g726_encoder_thread_t codec_thread[ MAX_CODEC_THREADS ];
static int                   test_iter = DEFAULT_ITERATIONS; /* default iteration is hard coded */
static char                  progress[] = "-\\|/";           /* console progress bar */
static flist_t *             file_list = NULL;               /* list of input/output/reference files */
static int                   num_threads = 1;    	         /* the number of threads for re-entrance & pre-emption */
static int                   focus_thread = -1;   	         /* focus thread is a thread that prints himself statistics 
						    		                            and statistics of another threads */
static int                   enable_mem_reloc = FALSE;								

/*==================================================================================================
                                       GLOBAL CONSTANTS
==================================================================================================*/


/*==================================================================================================
                                       GLOBAL VARIABLES
==================================================================================================*/

/*==================================================================================================
                                   LOCAL FUNCTION PROTOTYPES
==================================================================================================*/
int codec_engine( void * ptr );
/* for call in engine... */
int alloc_dec_memory( g726_encoder_thread_t * enc_thread );
int run_encoder_loop( g726_encoder_thread_t * enc_thread );
int fetch_data( g726_encoder_thread_t * enc_thread );
int homing_encoder( g726_encoder_thread_t * enc_thread );
int open_encoder_files( g726_encoder_thread_t * enc_thread );
int close_encoder_files( g726_encoder_thread_t * enc_thread );

/* helper functions */
int  run_codec( void * ptr );

/* test cases */
int nominal_functionality_test();
int re_entrance_test          ();
int re_locatability_test      ();
int endurance_test            ();
int load_test                 ();

/**/
void reset_encoder_instance( unsigned long instance_id );
int  bitmach( const char * out_fname, const char * ref_fname );
void outp_data_processing( g726_encoder_thread_t * encoder_thread );

void swap_bytes( short * words, int count );
/*==================================================================================================
                                       LOCAL FUNCTIONS
==================================================================================================*/


/*================================================================================================*/
/*================================================================================================*/
int VT_g726_encoder_setup()
{
    int rv = TFAIL;
    
    /** insert your code here */    
    
    rv = TPASS;
    return rv;
}


/*================================================================================================*/
/*================================================================================================*/
int VT_g726_encoder_cleanup()
{
    int rv = TFAIL;    
    int i;
    
    /* close all file descriptors */
    for( i = 0; i < MAX_CODEC_THREADS; ++i )
    {
        g726_encoder_thread_t * enc_thread = &codec_thread[i];
        close_encoder_files( enc_thread );
        SAFE_DELETE( enc_thread->enc_config );	
    }    
    
    if( file_list )
        delete_list( file_list );
    
    rv = TPASS;
    return rv;
}


/*================================================================================================*/
/*================================================================================================*/
int VT_g726_encoder_test( int testcase, int iter, const char * cfg_file )
{
    int rv = TFAIL;
    
    test_iter = iter;
    
    if( file_list )
	    delete_list( file_list );
	
    if( !read_cfg( cfg_file, &file_list ) )		
    {
	    tst_resm( TWARN, "VT_g726_encoder_test : can't parse %s", cfg_file );
  	    return rv;
    }
    
    switch( testcase )
    {
        case NOMINAL_FUNCTIONALITY:	    
            tst_resm( TINFO, "Nominal functionality test" );
            rv = nominal_functionality_test();
            tst_resm( TINFO, "End of nominal functionality test" );		
            break;
	    
	    case RE_ENTRANCE:
	        tst_resm( TINFO, "Re-entrance test" );
	        rv = re_entrance_test();
	        tst_resm( TINFO, "End of re-entrance test" );		
	        break;
	        
	    case RE_LOCATABILITY:
	        tst_resm( TINFO, "Relocatability test" );
	        rv = re_locatability_test();
	        tst_resm( TINFO, "End of relocatability test" );		
	        break;                    
	    
        case ENDURANCE:
            tst_resm( TINFO, "Endurance test" );
            rv = endurance_test();
            tst_resm( TINFO, "End of endurance test" );
            break;
        
        case LOAD:
            tst_resm( TINFO, "Load test" );
            rv = load_test();
            tst_resm( TINFO, "End of load test" );
            break;
                        
	    default:
	        tst_resm( TWARN, "Wrong test case" );
	        break;    
    }    
    
    return rv;
}


/*================================================================================================*/
/*================================================================================================*/
int codec_engine( void * ptr )
{
    int rv = TFAIL;	
    
    assert( ptr );
    
    g726_encoder_thread_t * enc_thread = (g726_encoder_thread_t*)ptr;
    sG726EEncoderConfigType  * enc_config; 
    
    /* allocate fast memory the encoder config */
    enc_config = (sG726EEncoderConfigType*)alloc_fast( sizeof(sG726EEncoderConfigType) );     
    if( !enc_config )
    {
        tst_resm( TWARN, "codec_engine : can't allocate memory for sG726EEncoderConfigType object" );
        return rv;
    }    
    enc_thread->enc_config = enc_config;
    
    enc_config->pvG726EEncodeInfoPtr = NULL;
    /* fill up the rellocated data position */
    enc_config->pu8APPEInitializedDataStart = BEGIN_G726E_DATA;
    
    /* initialize config structure memory to NULL */
    int i;
    for( i = 0; i < G726_MAX_NUM_MEM_REQS; i++ )
    {
        enc_config->sG726EMemInfo.asMemInfoSub[i].pvAPPEBasePtr = NULL;
    }
    
    /* allocate memory  */
    if( TPASS != alloc_dec_memory( enc_thread  ) )
    {
        return rv;
    }
    
    /* configure all configurable params */
    enc_config->s32APPEBitRate = enc_thread->bitrate;
    enc_config->s32APPEPcmFormat = enc_thread->law;    
    
    /* initialize the g726 encoder */
    enc_thread->last_g726D_error = eG726EEncodeInit( enc_config );
    if( enc_thread->last_g726D_error != G726E_OK )
    {
        tst_resm( TWARN, "codec_engine : the eG726EEncoderInit fails" );
        return rv; /* The memory will be deleted in the cleanup routine. */
    }

    /* open encoder files */
    if( TPASS != open_encoder_files( enc_thread ) )
    {
        return rv;
    }
    
    if( focus_thread == -1 )
        focus_thread = enc_thread->instance_id;
	
    /* homing speech */
    if( TPASS != homing_encoder( enc_thread ) )
    {
        tst_resm( TWARN, "codec_engine : Homing failed" );
        return rv;  
    }
    
    memset( enc_thread->inp_buf, 0, G726_L_BUFFER*sizeof(short) );
	
    /* run main encode loop */    
    rv = run_encoder_loop( enc_thread );
    
    /* cleanup */    
    close_encoder_files( enc_thread );    
    SAFE_DELETE( enc_thread->enc_config );    
    
    rv = TPASS;
    return rv;
}


/*================================================================================================*/
/*================================================================================================*/
int alloc_dec_memory( g726_encoder_thread_t * enc_thread )
{
    assert( enc_thread );
    assert( enc_thread->enc_config );

    int rv = TFAIL;
    sG726EEncoderConfigType * enc_config;
    sG726EMemAllocInfoSubType * mem;
    int nr, i;
    
    /* short name for the encoder thread */
    enc_config = enc_thread->enc_config;
    if( !enc_config ) return rv;
    
    /* query memmory that's necessary for decoding */
    enc_thread->last_g726D_error = eG726EQueryMem( enc_config );
    if( enc_thread->last_g726D_error != G726E_OK )
    {
        tst_resm( TWARN, "alloc_dec_memory : eG726EQueryMem fails" );
        return rv;
    }
        
    /* try to allocate requirement memory */
    nr = enc_config->sG726EMemInfo.s32G726ENumMemReqs;
    for( i = 0; i < nr; ++i )
    {
        mem = &enc_config->sG726EMemInfo.asMemInfoSub[i];
        if( mem->s32G726EMemTypeFs == G726_FAST_MEMORY )
            mem->pvAPPEBasePtr = alloc_fast( mem->s32G726ESize );
        else 
            mem->pvAPPEBasePtr = alloc_slow( mem->s32G726ESize );
        
        if( !mem->pvAPPEBasePtr )
        {
            tst_resm( TWARN, "alloc_dec_memory : can't allocate %d b", mem->s32G726ESize );
            return rv; /* The memory will be deleted in the cleanup routine. */
        }    
    }    
    
    rv = TPASS;
    return rv;
}


/*================================================================================================*/
/*================================================================================================*/
void free_dec_memory( g726_encoder_thread_t * enc_thread )
{
    assert( enc_thread );
    assert( enc_thread->enc_config );
    
    sG726EEncoderConfigType * enc_config;
    sG726EMemAllocInfoSubType * mem;
    int nr, i;
    
    /* short name for the encoder thread */
    enc_config = enc_thread->enc_config;
    
    nr = enc_config->sG726EMemInfo.s32G726ENumMemReqs;
    for( i = 0; i < nr; ++i )
    {
        mem = &enc_config->sG726EMemInfo.asMemInfoSub[i];
        SAFE_DELETE( mem->pvAPPEBasePtr );
    }        
}


/*================================================================================================*/
/*================================================================================================*/
int homing_encoder( g726_encoder_thread_t * enc_thread )
{
    assert( enc_thread );
    assert( enc_thread->enc_config );
    
    int num_samp, i = 0;
    int rv = TFAIL;
    
    if( !enc_thread->hom_fname || enc_thread->is_homing )
        return TPASS;
    
    /* run loop */
    while( TRUE )
    {
        /* fetch next data */
        num_samp = fetch_data( enc_thread );
        
        if( num_samp > 0 )
        {
            /* number of input samples to be encoded */
            enc_thread->enc_config->s32APPESampleNum = num_samp;
            enc_thread->enc_config->s32APPEBitRate = enc_thread->bitrate;
            enc_thread->enc_config->s32APPEPcmFormat = enc_thread->law;    
            
            for( i = 0; i < num_samp; ++i )
                enc_thread->out_buf[i] = 0;	    
            
            /* homing speech */
            enc_thread->last_g726D_error = eG726EEncode( enc_thread->enc_config, 
                                                         enc_thread->inp_buf, 
                                                         enc_thread->out_buf );
            if( enc_thread->last_g726D_error != G726E_OK )
            {
                /* errors occured during decoding */
                return rv;
            }
        }	
        else if( num_samp == 0 )
        {
            /* ok. end of stream */
            rv = TPASS;
            break;
        }
        else if( num_samp < 0 )
        {
            /* fk. errors */
            break; 	    	    	    
        }	   
    }    
    
    enc_thread->is_homing = TRUE;
    
    return rv;    
}


/*================================================================================================*/
/*================================================================================================*/
int open_encoder_files( g726_encoder_thread_t * enc_thread )
{
    assert( enc_thread );
    
    int rv = TFAIL;
    
    /* homing file */
    if( enc_thread->hom_fname )
    {
        if( !(enc_thread->hom_fstream = fopen( enc_thread->hom_fname, "rb" )) )
        {
            tst_resm( TWARN, "open_encoder_files : can't open %s", enc_thread->hom_fname );
            return rv;
        }
    }
    
    /* input file */
    if( enc_thread->inp_fname )
    {
        if( !(enc_thread->inp_fstream = fopen( enc_thread->inp_fname, "rb" )) )
        {
            tst_resm( TWARN, "open_encoder_files : can't open %s", enc_thread->inp_fname );
            return rv;
        }		
    }
    
    /* output file */
    if( enc_thread->out_fname )
    {
        if( !(enc_thread->out_fstream = fopen( enc_thread->out_fname, "wb" )) )
        {
            tst_resm( TWARN, "open_encoder_files : can't create %s", enc_thread->out_fname );
            return rv;	    
        }	
    }
    
    rv = TPASS;
    return rv;
}


/*================================================================================================*/
/*================================================================================================*/
int close_encoder_files( g726_encoder_thread_t * enc_thread )
{
    assert( enc_thread );
    
    int rv = TFAIL;
    
    /* homing file */
    if( enc_thread->hom_fstream )
    {
        fclose( enc_thread->hom_fstream );
        enc_thread->hom_fstream = NULL;
    }
    
    /* input file */
    if( enc_thread->inp_fstream )
    {
        fclose( enc_thread->inp_fstream );
        enc_thread->inp_fstream = NULL;
    }
    
    /* output file */
    if( enc_thread->out_fstream )
    {
        fclose( enc_thread->out_fstream );
        enc_thread->out_fstream = NULL;
    }
       
    rv = TPASS;
    return rv;
}


/*================================================================================================*/
/*================================================================================================*/
int re_locate_memory( g726_encoder_thread_t * enc_thread )
{
    assert( enc_thread );
    assert( enc_thread->enc_config );
    
    /* alias */
    int rv = TFAIL;
    
    free_dec_memory( enc_thread );    
    if( TPASS != alloc_dec_memory( enc_thread ) )
        return rv;
    enc_thread->enc_config->pu8APPEInitializedDataStart = BEGIN_G726E_DATA;
    enc_thread->last_g726D_error = eG726EEncodeInit( enc_thread->enc_config );
    if( enc_thread->last_g726D_error != G726E_OK )
    {
        tst_resm( TWARN, "re_locate_memory : the g726_encoder_init fails" );
        return rv;
    }    
    
    rv = TPASS;
    return rv;
}


/*================================================================================================*/
/*================================================================================================*/
int run_encoder_loop( g726_encoder_thread_t * enc_thread )
{
    assert( enc_thread );
    assert( enc_thread->enc_config );
    
    int num_samp, n = 0;
    int rv = TFAIL;
    
    /* run loop */
    while( TRUE )
    {
        if( enc_thread->mem_reloc_freq )
        {
            if( n % enc_thread->mem_reloc_freq == 0 )
                if( TPASS != re_locate_memory( enc_thread ) )
                    break;
        }
        
        /* fetch next data */
        num_samp = fetch_data( enc_thread );
        
        if( num_samp > 0 )
        {
            /* number of input samples to be encoded */
            enc_thread->enc_config->s32APPESampleNum = num_samp;
            enc_thread->enc_config->s32APPEBitRate = enc_thread->bitrate;
            enc_thread->enc_config->s32APPEPcmFormat = enc_thread->law;  
            /* call encode routine */
            enc_thread->last_g726D_error = eG726EEncode( enc_thread->enc_config, 
                                                         enc_thread->inp_buf, 
                                                         enc_thread->out_buf );
            if( enc_thread->last_g726D_error != G726E_OK )
            {
                /* errors occured during decoding */
                enc_thread->dec_errors_count++;
                continue;
            }
            
            enc_thread->dec_samples += num_samp;
            
            /* ok. processing output data and continue. */
            /* num_samp should lookup from enc_config->s32APPESampleNum */
            outp_data_processing( enc_thread );	    
        }	
        else if( num_samp == 0 )
        {
            /* ok. end of stream */
            rv = TPASS;
            break;
        }
        else if( num_samp < 0 )
        {
            /* fk. errors */
            break; 	    	    	    
        }	   
        
        if( enc_thread->instance_id == focus_thread )
        {
            int k;
            g726_encoder_thread_t * e;
            for( k = 0; k < num_threads; k++ )	    
            {
                e = &codec_thread[k];
                printf( "-t[%lu] samp[%d] ", e->instance_id, e->dec_samples );
            }
            printf( "%c\r", progress[(n)%(sizeof(progress)-1)] );		
        }		
        fflush( stdout );
        
        n++; 	
    }    
    
    return rv;
}


/*================================================================================================*/
/*================================================================================================*/
int fetch_data( g726_encoder_thread_t * enc_thread )
{
    int num_samp;
    FILE * in = enc_thread->inp_fstream;
    if( !enc_thread->is_homing && enc_thread->hom_fstream )
        in = enc_thread->hom_fstream;
    
    if( feof( in ) )
    {
        num_samp = 0;
    }	
    else
    {
        num_samp = fread( enc_thread->inp_buf, sizeof(short), G726_L_BUFFER, in );	
        swap_bytes( enc_thread->inp_buf, num_samp );
    }        
    return num_samp;
}

/*================================================================================================*/
/*================================================================================================*/
int run_codec( void * ptr )
{
    assert( ptr );

    int i;        
    
    g726_encoder_thread_t * enc_thread = (g726_encoder_thread_t*)ptr;        
    enc_thread->ltp_retval = codec_engine( ptr );        
    enc_thread->thread_finished = TRUE;    
    if( enc_thread->instance_id == focus_thread )
    {
        for( i = 0; i < num_threads; ++i )
        {
            if( !codec_thread[i].thread_finished )
            {
                focus_thread = codec_thread[i].instance_id;
                break;
            }
        }
    }
    
    return enc_thread->ltp_retval;
}


/*================================================================================================*/
/*================================================================================================*/
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


/*================================================================================================*/
/*================================================================================================*/
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


/*================================================================================================*/
/*================================================================================================*/
int nominal_functionality_test()
{
    flist_t * node;
    int i;
    int rv = TPASS;    
    g726_encoder_thread_t * enc_thread = &codec_thread[0];     
        
    for( node = file_list, i = 0; node; node = node->next, ++i )
    {
        reset_encoder_instance( 0 );
        
        tst_resm( TINFO, "input file: %s", node->inp_fname );
        
        enc_thread->hom_fname  = strcmp( node->hom_fname, NONE_FILE ) != 0 ? node->hom_fname : NULL;
        enc_thread->inp_fname  = strcmp( node->inp_fname, NONE_FILE ) != 0 ? node->inp_fname : NULL;
        enc_thread->out_fname  = strcmp( node->out_fname, NONE_FILE ) != 0 ? node->out_fname : NULL;		
        enc_thread->ref_fname  = strcmp( node->ref_fname, NONE_FILE ) != 0 ? node->ref_fname : NULL;        
        enc_thread->bitrate    = node->bitrate;
        enc_thread->law        = node->pcm_format;        
                
        if( enable_mem_reloc )
            enc_thread->mem_reloc_freq = rand()%10+10;
        
        /* call encoder engine */
        rv += run_codec( enc_thread );
        
        /* bitmach */
        if( enc_thread->ref_fname && enc_thread->out_fname )
        {
            int bitmach_pass = bitmach( enc_thread->out_fname, enc_thread->ref_fname );
            if( !bitmach_pass )
            {
                tst_resm( TWARN, "bitmatch failed" );    
                rv = TFAIL;
            }
            else
            {
                tst_resm( TINFO, "bitmatch passed" );
            }
        }       	                        
    }
    
    return rv;
}


/*================================================================================================*/
/*================================================================================================*/
int re_entrance_test()
{
    flist_t * node;
    int i;
    int rv = TPASS;    
    g726_encoder_thread_t * enc_thread;
    
    for( node = file_list, i = 0; node && i < MAX_CODEC_THREADS; node = node->next, ++i )
    {
        reset_encoder_instance( i );
        enc_thread = &codec_thread[i];     
        
        enc_thread->hom_fname  = strcmp( node->hom_fname, NONE_FILE ) != 0 ? node->hom_fname : NULL;
        enc_thread->inp_fname  = strcmp( node->inp_fname, NONE_FILE ) != 0 ? node->inp_fname : NULL;
        enc_thread->out_fname  = strcmp( node->out_fname, NONE_FILE ) != 0 ? node->out_fname : NULL;		
        enc_thread->ref_fname  = strcmp( node->ref_fname, NONE_FILE ) != 0 ? node->ref_fname : NULL;        
        enc_thread->bitrate    = node->bitrate;
        enc_thread->law        = node->pcm_format;	
        
        if( pthread_create( &enc_thread->tid, NULL, (void*)&run_codec, enc_thread ) )
        {
            tst_resm( TWARN, "re_entrance_test : error creating thread %d", i );
            return TFAIL;	    
        }	
    }    
    
    num_threads = i;
    for( i = 0; i < num_threads; ++i )
    {
        enc_thread = &codec_thread[i];     
        pthread_join( enc_thread->tid, NULL );
    }
    
    for( i = 0; i < num_threads; ++i )
    {
        enc_thread = &codec_thread[i];     
        /* bitmach */
        if( enc_thread->ref_fname && enc_thread->out_fname )
        {
            int bitmach_pass = bitmach( enc_thread->out_fname, enc_thread->ref_fname );            
            if( !bitmach_pass )
            {
                tst_resm( TWARN, "bitmach failed" );    
                rv = TFAIL;
            }
            else
            {
                tst_resm( TINFO, "bitmach passed" );
            }
        }                
        rv += enc_thread->ltp_retval;
    }
    
    return rv;
}

/*================================================================================================*/
/*================================================================================================*/
int re_locatability_test()
{
    flist_t * node;
    int i, j;
    int rv = TPASS;    
    g726_encoder_thread_t * enc_thread = &codec_thread[0];     
        
    for( node = file_list, i = 0; node; node = node->next, ++i )
    {
        tst_resm( TINFO, "input file: %s", node->inp_fname );
        for( j = 0; j < test_iter; ++j )
        {
            reset_encoder_instance( 0 );
                       
            enc_thread->hom_fname  = strcmp( node->hom_fname, NONE_FILE ) != 0 ? node->hom_fname : NULL;
            enc_thread->inp_fname  = strcmp( node->inp_fname, NONE_FILE ) != 0 ? node->inp_fname : NULL;
            enc_thread->out_fname  = strcmp( node->out_fname, NONE_FILE ) != 0 ? node->out_fname : NULL;		
            enc_thread->ref_fname  = strcmp( node->ref_fname, NONE_FILE ) != 0 ? node->ref_fname : NULL;        
            enc_thread->bitrate    = node->bitrate;
            enc_thread->law        = node->pcm_format;        
                
            if( enable_mem_reloc )
                enc_thread->mem_reloc_freq = rand()%10+10;
        
            /* call encoder engine */
            rv += run_codec( enc_thread );
        
            /* bitmach */
            if( enc_thread->ref_fname && enc_thread->out_fname )
            {
                int bitmach_pass = bitmach( enc_thread->out_fname, enc_thread->ref_fname );
                if( !bitmach_pass )
                {
                    tst_resm( TWARN, "bitmatch failed" );    
                    rv = TFAIL;
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


/*================================================================================================*/
/*================================================================================================*/
int endurance_test()
{
    int i, rv = TPASS;
    for( i = 0; i < test_iter; ++i )
    {
        tst_resm( TINFO, "The %d iteration is started", i+1 );
        rv += nominal_functionality_test();
        tst_resm( TINFO, "The %d iteration is completed", i+1 );        
    }   
    return rv;
}


/*================================================================================================*/
/*================================================================================================*/
int robustness_test()
{
    return TPASS;
}


/*================================================================================================*/
/*================================================================================================*/
void hogcpu()
{
    while( 1 )
    {
	    sqrt( rand() );
    }
}


/*================================================================================================*/
/*================================================================================================*/
int load_test()
{
    int rv = TFAIL;
    pid_t pid;
    
    switch( pid = fork() )
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
	        rv = nominal_functionality_test();
    	    /* kill child process once decode/encode loop has ended */
	        if( kill( pid, SIGKILL ) != 0 )
	        {
        		tst_resm( TWARN, "load_envirounment_test : Kill(SIGKILL) error" );
	    	    return rv;
	        }
    }	
    return rv;
}


/*================================================================================================*/
/*================================================================================================*/
void reset_encoder_instance( unsigned long instance_id )
{
    g726_encoder_thread_t * enc_thread;

    if( instance_id < MAX_CODEC_THREADS )
    {
        enc_thread = &codec_thread[instance_id];
        
        enc_thread->instance_id      = instance_id;
        enc_thread->last_g726D_error = G726E_OK;
        enc_thread->hom_fname        = NULL;
        enc_thread->is_homing        = FALSE;
        enc_thread->inp_fname 	     = NULL;
        enc_thread->out_fname	     = NULL;
        enc_thread->ref_fname 	     = NULL;	
        enc_thread->thread_finished  = FALSE;
        enc_thread->inp_buf          = (short*)enc_thread->input_buffer;
        enc_thread->out_buf          = (short*)enc_thread->output_buffer;
        enc_thread->dec_errors_count = 0;
        enc_thread->mem_reloc_freq   = 0;
        enc_thread->dec_samples      = 0; 
    }
}


/*================================================================================================*/
/*================================================================================================*/
int compare_files( const char * fname1, const char * fname2 )
{
    int out, ref;
    struct stat fstat_out, fstat_ref;
    char *fptr_out, *fptr_ref;
    size_t filesize;
    int i;

    if( (out = open(fname1, O_RDONLY)) < 0 )
    {
        return FALSE;
    }
    if ((ref = open(fname2, O_RDONLY)) < 0)
    {
    	close(out);
        return FALSE;
    }
    fstat( out, &fstat_out );
    fstat( ref, &fstat_ref );
    if( fstat_out.st_size != fstat_ref.st_size )
    {
	    close(out);
    	close(ref);
	    return FALSE;
    }
    filesize = fstat_out.st_size;
    fptr_out = (char*)mmap( 0, filesize, PROT_READ, MAP_SHARED, out, 0 );
    if( fptr_out == MAP_FAILED )
    {
    	close( out );
	    close( ref );
        return FALSE;
    }
    fptr_ref = (char*) mmap(0, filesize, PROT_READ, MAP_SHARED, ref, 0);
    if( fptr_ref == MAP_FAILED )
    {
	    close( out );
    	close( ref );
	    return FALSE;
    }
    close( out );
    close( ref );
    for( i = 0; i < filesize; ++i )
    {
    	if( *(fptr_ref + i) != *(fptr_out + i) )
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


/*================================================================================================*/
/*================================================================================================*/
int bitmach( const char * out_fname, const char * ref_fname )
{
    return compare_files( out_fname, ref_fname );
}


/*================================================================================================*/
/*================================================================================================*/
void outp_data_processing( g726_encoder_thread_t * enc_thread )
{
    assert( enc_thread );
    assert( enc_thread->enc_config );
    int num_samp = enc_thread->enc_config->s32APPESampleNum, i;
    unsigned short word;
    
    if( enc_thread->out_fstream )
    {
        for( i = 0; i < num_samp; ++i )
        {
            word = enc_thread->out_buf[i];
            fwrite( &word, 1, sizeof(short), enc_thread->out_fstream );
        }		
    }
}


/*================================================================================================*/
/*================================================================================================*/
void swap_bytes( short * words, int count )
{
#ifdef G726_BIG_ENDIAN
    char * byte;
    while( count-- )
    {
         byte = (char*)(words+count);
    	 byte[0] ^= byte[1] ^= byte[0] ^= byte[1];
    }
#endif    
}

#ifdef __cplusplus
}
#endif

