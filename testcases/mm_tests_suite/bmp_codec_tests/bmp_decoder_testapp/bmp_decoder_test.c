/*================================================================================================*/
/**
    @file   bmp_decoder_test.c

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
D.Simakov / smkd001c         12/07/2004     TLSbo40263  Initial version 
D.Simakov / smkd001c         02/03/2005     TLSbo47117  Update
D.Simakov / smkd001c         27/05/2005     TLSbo47117  Bug fix
D.Simakov / smkd001c         02/06/2005     TLSbo50884  Updated according to the 2.2 bmp decoder release.
D.Simakov / smkd001c         02/06/2005     TLSbo50899  Robustness test case was improved 
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
    
/* Harness Specific Include Files. */
#include "test.h"

/* Verification Test Environment Include Files */
#include "bmp_decoder_test.h"
#include "stuff/fb_draw_api.h"
#include "stuff/fconf.h"
#include <sys/mman.h>
               

/*==================================================================================================
                                        LOCAL MACROS
==================================================================================================*/
#define TRUE 1
#define FALSE 0
#define READ_BINARY "rb"

//#define DBGM(m,v) {printf("-- %s -- %d --\n",m,v);fflush(stdout);}

#define SAFE_DELETE(s) {if(s){free(s); s=NULL;}}

// for ppm_bitmach
#define FILES_ARE_EQUAL     0
#define FILES_ARE_NOT_EQUAL 1
#define NO_OUT_FILE         2
#define NO_REF_FILE         3
#define OUT_IS_NOT_PPM      4
#define REF_IS_NOT_PPM      5
#define FATAL_ERROR         6

/*==================================================================================================
                          LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
==================================================================================================*/


/*==================================================================================================
                                       LOCAL CONSTANTS
==================================================================================================*/

/*==================================================================================================
                                       LOCAL VARIABLES
==================================================================================================*/

/*==================================================================================================
                                       GLOBAL CONSTANTS
==================================================================================================*/

/*==================================================================================================
                                       GLOBAL VARIABLES
==================================================================================================*/
static int threadSynchro   = FALSE;	           /* boolean used by the loop thread to inform the thread */
static int clearFullScreen = FALSE; 	       /* working on timer that the 1st thread has ended */
static int test_iter	   = ITERATIONS;  	   /* default iteration is hard coded */
static int lastError       = BMP_ERR_NO_ERROR;
static int test_case_id    = -1;
static int delay_value     = 5;
static flist_t * file_list = NULL;             /* list of input/output/reference files */

char * word_file = NULL;
char * word_file_with_bmp_hdr = NULL;

static int rv_mt[2] = {TPASS,TPASS};

/*==================================================================================================
                                   LOCAL FUNCTION PROTOTYPES
==================================================================================================*/
int decode_picture(void *ptr);
int decode_picture_on_timer(void *ptr);
BMP_error_type BMP_get_new_data (BMP_UINT8 **new_buf_ptr, BMP_UINT32 *new_buf_len, BMP_Decoder_Object *);
void timer_handler (int signum);
int decode_picture_on_timer(void *ptr);
int decode_picture_in_loop(void *ptr);
void detect_enter();
int hogcpu (void);
void write_data(unsigned char * scanline, int stride, int thread_idx );
void write_ppm_header( FILE * out, int width, int height );
int ppm_bitmach( const char * out_fname, const char * ref_fname, int bpp );

/*==================================================================================================
                                       LOCAL FUNCTIONS
==================================================================================================*/


/*================================================================================================*/
/*================================================================================================*/
int hogcpu (void)
{
	while (1)
    {
		sqrt (rand ());
	}
	return 0;
}

/*================================================================================================*/
/*================================================================================================*/
int decode_picture(void *ptr)
{
	lastError = BMP_ERR_NO_ERROR;

    int rv = TFAIL;
	
	if( ptr == NULL )
	{
	    tst_resm( TWARN, "decode_picture : invalid args" );
	    return rv;
	}    
	
	bmp_decoder_thread *thread_info = (bmp_decoder_thread *) ptr;

    /* Retrieve all the parameters given through the thread structure */
	char *bmp_filename = thread_info->input_file_name;
	int instance_id = thread_info->instance_id;
	unsigned int originYDualDisplayOffset = thread_info->offsetYDualScreen;	/* Y offset in case of dual screeen */

	unsigned int originXOffset = 0;		/* X offset */
	unsigned int originYOffset = 0;		/* Y offset */

	BMP_Decoder_Object dec_obj;
	BMP_Mem_Alloc_Info *mem_info = &dec_obj.mem_info;
	BMP_INT32 ret;
	BMP_UINT8 *output_buf = NULL;
	
	BMP_Decoder_Params    *dec_param;
	BMP_Decoder_Info_Init *dec_info_init;
	
	dec_param = &dec_obj.dec_param;   
	dec_info_init = &dec_obj.dec_info_init; 
		
	p_dec_obj[instance_id] = &dec_obj;

    /* Open the input file. */
	if ((input_file[instance_id] = fopen(bmp_filename, READ_BINARY)) == NULL) 
	{
		tst_resm( TWARN, "can't open %s", bmp_filename);
      	return rv;
	}
    /* Open the output file. */
    if( thread_info->output_file_name )
    {
        if( !(output_file[instance_id] = fopen( thread_info->output_file_name, "wb" ) ) )
        {
            tst_resm( TWARN, "can't create %s", thread_info->output_file_name );
            return rv;    
        }
    }

    const framebuffer_t * fb = get_framebuffer();
    assert(fb);
    argb_color_t white = {1.0f, 1.0f, 1.0f, 1.0f};
 
    /* ugly... */   
    if( test_case_id == PRE_EMPTION || test_case_id == RE_ENTRANCE || test_case_id == LOAD_TEST )
    {        
        if( clearFullScreen == FALSE )
        {	
            fb->draw_rect( 0, originYDualDisplayOffset+fb->height/4, 
                           fb->width, originYDualDisplayOffset+fb->height/4 + fb->height/2, 
                           &white );
        }
        else
        {
            fb->clear_screen(&white);
        }
    }
    
    dec_obj.BMP_get_new_data = BMP_get_new_data;
    dec_obj.BMP_seek_file = BMP_seek_file;

    /* This test is done for displaying the picture on the display */ 
    /* So, it needs to force some parameter as RGB 565 format */
    /* and output size that must contains into the frame buffer */
    dec_param->out_format = thread_info->dec_param.out_format;
	dec_param->scale_mode = thread_info->dec_param.scale_mode;
     
	dec_param->output_width = thread_info->width;
    dec_param->output_height = thread_info->height;
    if( dec_param->output_width == 0 )
        dec_param->output_width = fb->width;
    if( dec_param->output_height == 0 )
        dec_param->output_height = fb->height;        
    if( test_case_id == PRE_EMPTION || test_case_id == RE_ENTRANCE )
    {
        dec_param->output_height /= 2;
        dec_param->scale_mode = E_INT_SCALE_PRESERVE_AR;
    }

    fseek(input_file[instance_id] , 0, SEEK_SET);
    /* Query for memory required */
	ret = BMP_query_dec_mem (&dec_obj);
	if (ret != BMP_ERR_NO_ERROR)
	{
		tst_resm( TWARN, "Error on calling BMP_query_dec_mem %lu", ret );
		lastError = ret;
	    	return rv;
	}
	
    /* Allocate required memory */
	{
	    int i;
	    for (i = 0; i < mem_info->num_reqs; i++)
	    {
	       mem_info->mem_info_sub[i].ptr =
	             (void *) malloc (mem_info->mem_info_sub[i].size);
		     
	       if( mem_info->mem_info_sub[i].ptr == 0 ) 
	       {
	    	    tst_resm( TWARN, "Error on calling malloc" );	        
	    	    return rv;
	       }	     
	    }
	}
	
    /* Seek to the begining of the file again */
	fseek(input_file[instance_id] , 0, SEEK_SET);

	int endurance = 0;
	while( endurance < thread_info->endurance ) 
    {
        if( thread_info->endurance > 1 )
	    {	
            fb->clear_screen(&white);
            tst_resm( TINFO, "Endurance iteration: %d", endurance );
	    }
	
	    ++endurance;
	
	    fseek(input_file[instance_id] , 0, SEEK_SET);

        /* Initialize the decoder */
	    ret = BMP_decoder_init (&dec_obj);
	    if (ret != BMP_ERR_NO_ERROR)
	    {
            tst_resm( TWARN, "Error on calling BMP_decoder_init %d", ret );
            return rv;
	    }

        /* Allocate output buffer */
	
	    BMP_UINT32 output_buf_size = dec_info_init->output_width * dec_info_init->output_height;
	    BMP_UINT32 color_depth;
        if( dec_param->out_format == E_OUTPUTFORMAT_RGB888 )
        {
            color_depth = 3;
        } 
        else if( dec_param->out_format == E_OUTPUTFORMAT_RGB565 )
        {
            color_depth = 2;
        }
	
	    output_buf_size *= color_depth;
	
        output_buf = (BMP_UINT8*)malloc( output_buf_size );
        if( output_buf == NULL )
        {
            tst_resm( TWARN, "Error on calling malloc" );
            return rv;
        }
	
	    /********************
	     * Print statistics *
	     *******************/	
	 
        const char * compr_type;
        switch( dec_info_init->cmpr_type )
        {
            case E_RGB  : compr_type = "RGB";     break;
            case E_RLE4 : compr_type = "RLE4";    break;
            case E_RLE8 : compr_type = "RLE8";    break;
            default     : compr_type = "Unknown"; break;
        }
		                    
	    if( fb->height >= dec_info_init->output_height )
		    originYOffset = (fb->height - dec_info_init->output_height) / 2;
	
	    if( fb->width >= dec_info_init->output_width )    
		    originXOffset = (fb->width - dec_info_init->output_width) / 2;        
        
        /* Decoder loop to generate the RGB outputs */
        int scanline = 0;
        
        pixel_format_e pfmt = PF_RGB_888;
        if( dec_param->out_format == E_OUTPUTFORMAT_RGB565 )
        {
            pfmt = PF_RGB_565;
        }
        else if( dec_param->out_format == E_OUTPUTFORMAT_RGB555 )
        {
            pfmt = PF_RGB_555;
        }        

        if( output_file[instance_id] && endurance == 1 )
        {
            write_ppm_header( output_file[instance_id], dec_info_init->output_width, dec_info_init->output_height );    
        }
        
        int mult_factor;
    	if ((dec_obj.dec_param.out_format == E_OUTPUTFORMAT_RGB555)
	    		      || (dec_obj.dec_param.out_format == E_OUTPUTFORMAT_RGB565))
    	{
	    	mult_factor=2;
    	}
	    else if (dec_obj.dec_param.out_format == E_OUTPUTFORMAT_RGB666)
    	{
	    	mult_factor=4;
    	}
	    else
    	{
	    	mult_factor=3;
    	}

        BMP_UINT8 * out_buf_ptr = output_buf + output_buf_size - (dec_obj.dec_info_init.output_width * mult_factor);
        //out_buf = (out_image + mem_out_pix - (int)(BmpDecObject.dec_info_init.output_width * mult_factor));
        for( scanline = dec_info_init->output_height-1; scanline >= 0; --scanline )
        //for( scanline = 0; scanline < dec_info_init->output_height; ++scanline )
        {
            ret = BMP_decode_row_pp( &dec_obj, out_buf_ptr );	      
            if( ret != BMP_ERR_NO_ERROR )
            {
                tst_resm( TWARN, "Error on calling BMP_decode_row_pp %d", ret );	    	        
                lastError = ret;
                return rv;
            }
            
            fb->draw_scanline( out_buf_ptr,
                               originXOffset,  
                               scanline + originYOffset + originYDualDisplayOffset,
                               dec_info_init->output_width,
                               pfmt );                        
            /*if( endurance == 1 )
            {
                write_data( out_buf_ptr, dec_info_init->output_width * color_depth, instance_id );                               
            }*/
            out_buf_ptr -= (dec_obj.dec_info_init.output_width*mult_factor);            
        } 
        if( endurance == 1 )
            write_data(output_buf, output_buf_size, instance_id);
        
        /* Finish decompression and release memory. */

        /* Free the output buffer */
        free (output_buf);
    }
    
    /* Free the allocated memory */
    int i;
    for (i = 0; i < mem_info->num_reqs; i++)
    {
        free (mem_info->mem_info_sub[i].ptr);
    }
    
    /* Close files, if we opened them */
    if (input_file[instance_id]  != stdin)
        fclose(input_file[instance_id] );
    if (output_file[instance_id])
        fclose(output_file[instance_id]); 

    /* All done. */
    rv = TPASS;
    return rv;
}

/*================================================================================================*/
/*================================================================================================*/
BMP_error_type BMP_seek_file( BMP_Decoder_Object * dec_obj, 
                              BMP_INT32 num_bytes, 
                              BMP_Seek_File_Position start_or_current )
{
    int instance_id = -1;
    int i;
    for( i = 0; i < DECODER_THREAD; ++i )
    {
        if( p_dec_obj[i] == dec_obj )
        {
            instance_id = i;
            break;
        }
    }
    assert( instance_id != -1 );
    
    if( start_or_current == BMP_SEEK_FILE_CURR_POSITION )
    {
        if( fseek( input_file[instance_id], num_bytes, SEEK_CUR ) != 0 )
        {
            return -1;
        }	
    }
    else if( start_or_current == BMP_SEEK_FILE_START )
    {
        if( fseek( input_file[instance_id], num_bytes, SEEK_SET ) != 0 )
        {
            return -1;
        }
    }
    
    return 0;
}

/*================================================================================================*/
/*================================================================================================*/
BMP_error_type BMP_get_new_data( BMP_UINT8 ** new_buf_ptr, 
                                 BMP_UINT32 * new_buf_len, 
                                 BMP_Decoder_Object * dec_obj )
{
    int instance_id = -1;
    int i;
    for( i = 0; i < DECODER_THREAD; ++i )
    {
        if( p_dec_obj[i] == dec_obj )
        {
            instance_id = i;
            break;
        }
    }
    assert(instance_id != -1);
    
    int bytes_read;
   
    bytes_read = fread(BMP_input_buffer[instance_id], sizeof(char), INPUT_BUF_SIZE, input_file[instance_id]);
    if(bytes_read)
    {
		*new_buf_len = bytes_read;
		*new_buf_ptr = BMP_input_buffer[instance_id];
        return 0;
    }
    else
    {
        return BMP_ERR_EOF;
    }    
}

/*================================================================================================*/
/*================================================================================================*/
void timer_handler(int signum)
{
	static int pending = FALSE;
	static int retval= 0;

	if(!pending)
	{
		pending = TRUE;
		retval += decode_picture(&thread_decoder[1]);
		pending = FALSE;
        rv_mt[1] += retval;
	}
	else 
	{
		tst_resm( TWARN, "Picture decode skipped, timer too fast!!!" );
	}
	fflush (stdout);

    /* if the loop thread ends, terminate the thread working with timer */
    if(threadSynchro)
        pthread_exit(&retval);
}


/*================================================================================================*/
/*================================================================================================*/
int decode_picture_in_loop(void *ptr)
{
	int i, retval = 0, rv = TFAIL;

    bmp_decoder_thread * thread = (bmp_decoder_thread*)ptr;

	for(i = 0; i < test_iter; i++) 
	{
		retval += decode_picture(ptr);
        detect_enter();
	}

    /* Set that boolean that is a global variable of the main process */
    /* to inform the second thread that the 1st one has ended. */
    /* It allows the 2nd thread to terminate. */
	threadSynchro = TRUE;

	if(!retval)
		rv = TPASS;
    
    rv_mt[thread->instance_id] = rv;       

	return 0;
}

/*================================================================================================*/
/*================================================================================================*/
int decode_picture_on_timer(void *ptr)
{
	struct sigaction sa;
	struct itimerval timer;

    /* Install the timer handler... */
	memset (&sa, 0, sizeof (sa));
	sa.sa_handler= &timer_handler;
	sigaction (SIGALRM, &sa, NULL);
	
    /* Configure the timer to expire every 100 msec...  */
    timer.it_value.tv_sec= 0; 			/* First timeout */
    timer.it_value.tv_usec= 100000  	/* 100000 */;
    timer.it_interval.tv_sec= 0;  		/* Interval */
    timer.it_interval.tv_usec= 100000  	/* 100000 */;
	
    /* Start timer...  */
	setitimer (ITIMER_REAL, &timer, NULL);

	while (1)
		sleep (10);
    return 0;
}

/*================================================================================================*/
/*================================================================================================*/
int VT_bmp_decoder_setup(void)
{
    int rv = TFAIL;
  
    int i;
    for( i = 0; i < DECODER_THREAD; ++i )
    {
        p_dec_obj[i] = NULL; 
        input_file[i] = NULL;
        output_file[i] = NULL;
        ref_fstream[i] = NULL;
    }           
    rv = TPASS;
    return rv;
}


/*================================================================================================*/
/*================================================================================================*/
int VT_bmp_decoder_cleanup(void)
{
	int rv = TFAIL;
    
    if( file_list )
        delete_list( file_list );
        
    if(word_file) free(word_file);
    if(word_file_with_bmp_hdr) free(word_file_with_bmp_hdr);        

    rv = TPASS;
  	return rv;
}

/*================================================================================================*/
/*================================================================================================*/
void detect_enter()
{   
    if(!delay_value) return;
    
	int fd_console = 0;		/* 0 is the video input */
	fd_set fdset;
	struct timeval timeout;
	char c;

	FD_ZERO(&fdset);
	FD_SET(fd_console, &fdset);
	timeout.tv_sec = delay_value;	/* set timeout !=0 => blocking select */
	timeout.tv_usec = 0;
	if (select(fd_console+1, &fdset, 0, 0, &timeout) > 0)
	{
		do 
		{
			read(fd_console, &c, 1);
		} while (c != 10);	// i.e. line-feed 
	}
}

/*================================================================================================*/
/*================================================================================================*/
int do_files_exist( const char * fname1, const char * fname2 )
{
    FILE * fstream1 = fopen( fname1, "r" );
    if( fstream1 )
    {
        fclose( fstream1 );
        FILE * fstream2 = fopen( fname2, "r" );
        if( fstream2 )
        {
            fclose( fstream2 );
            return TRUE;
        }        
    }
    return FALSE;    
}

int exhaustive_decode()
{
    flist_t * node;
    int i;
    int rv = TPASS;    
    
    for( node = file_list, i = 0; node; node = node->next, ++i )
    {
        tst_resm( TINFO, "input file: %s", node->inp_fname );      
		
        #define NONE_FILE "n/a"    
    
        thread_decoder[0].instance_id = 0;
        thread_decoder[0].offsetYDualScreen = 0;
        thread_decoder[0].endurance = 1;    
        
        thread_decoder[0].input_file_name      = strcmp( node->inp_fname, NONE_FILE ) != 0 ? node->inp_fname : NULL;
        thread_decoder[0].output_file_name     = strcmp( node->out_fname, NONE_FILE ) != 0 ? node->out_fname : NULL;
        thread_decoder[0].ref_fname            = strcmp( node->ref_fname, NONE_FILE ) != 0 ? node->ref_fname : NULL;
	    thread_decoder[0].dec_param.scale_mode = node->scale_mode;				     
        thread_decoder[0].dec_param.out_format = node->out_fmt;
        thread_decoder[0].width = node->width;
        thread_decoder[0].height = node->height;
        
        const framebuffer_t * fb = get_framebuffer();
        argb_color_t white = {1.0f, 1.0f, 1.0f, 1.0f};
        fb->clear_screen(&white);
        int r =  decode_picture(&thread_decoder[0]);						     
	    tst_resm(TINFO, "decoding %s", (r == TFAIL) ? "failed" : "succeeded");		     
        rv += r;
        if( do_files_exist(thread_decoder[0].output_file_name, thread_decoder[0].ref_fname) )
        {
            int bpp = 3;
            if( node->out_fmt == E_OUTPUTFORMAT_RGB565 )
                bpp = 2;
            int bm = ppm_bitmach(thread_decoder[0].output_file_name, thread_decoder[0].ref_fname, bpp );
            switch(bm)
            {
                case FILES_ARE_EQUAL:
                    tst_resm( TINFO, "bitmach passed" );
                    break;
                
                case FILES_ARE_NOT_EQUAL:
                    rv = TFAIL;
                    tst_resm( TWARN, "bitmach failed" );
                    break;
    
                case NO_OUT_FILE:
                    tst_resm( TWARN, "bitmach : can't open %s", thread_decoder[0].output_file_name );
                    break;

                case NO_REF_FILE:
                    tst_resm( TWARN, "bitmach : can't open %s", thread_decoder[0].ref_fname );
                    break;
                    
                case OUT_IS_NOT_PPM:
                    tst_resm( TWARN, "bitmach : %s is not ppm", thread_decoder[0].output_file_name );
                    break;
                
                case REF_IS_NOT_PPM:
                    tst_resm( TWARN, "bitmach : %s is not ppm", thread_decoder[0].ref_fname );
                    break;
                    
                case FATAL_ERROR: default:
                    tst_resm( TWARN, "bitmach : fatal error" );                    
                    break;
            }
        }
        detect_enter(); 	
    }								         
    
    return rv;
}

int reentrance_test()
{
    flist_t * node;
    int i;
    int rv = TPASS;    
    
    for( node = file_list, i = 0; node && i < 2; node = node->next, ++i )
    {
        tst_resm( TINFO, "input file: %s", node->inp_fname );      
		
        const framebuffer_t * fb = get_framebuffer();
        
        #define NONE_FILE "n/a"    
    
        thread_decoder[i].instance_id = i;
        if( i == 0 ) thread_decoder[i].offsetYDualScreen = -fb->height/4;
        else if( i == 1 ) thread_decoder[i].offsetYDualScreen = +fb->height/4;
        thread_decoder[i].endurance = 1;    
        
        thread_decoder[i].input_file_name      = strcmp( node->inp_fname, NONE_FILE ) != 0 ? node->inp_fname : NULL;
        thread_decoder[i].output_file_name     = strcmp( node->out_fname, NONE_FILE ) != 0 ? node->out_fname : NULL;
        thread_decoder[i].ref_fname            = strcmp( node->ref_fname, NONE_FILE ) != 0 ? node->ref_fname : NULL;
	    thread_decoder[i].dec_param.scale_mode = node->scale_mode;				     
        thread_decoder[i].dec_param.out_format = node->out_fmt;
        thread_decoder[i].width = node->width;
        thread_decoder[i].height = node->height;

        
        if( pthread_create( &tid[i], NULL, (void *) &decode_picture_in_loop, (void *) &thread_decoder[i]) )
		{
            tst_resm( TWARN, "Error creating thread %d", i );
			return rv;
		}        
    }
    
    for( i = 0; i < 2; ++i )
    {
        pthread_join( tid[i], NULL );
    }    
    
    for( i = 0; i < 2; ++i )
    {
        if( do_files_exist( thread_decoder[i].output_file_name, thread_decoder[i].ref_fname ) )
        {            
            const char * out = thread_decoder[i].output_file_name;
            const char * ref = thread_decoder[i].ref_fname;
            int bpp = 3;
            if( thread_decoder[i].dec_param.out_format == E_OUTPUTFORMAT_RGB565 )
                bpp = 2;
            int bm = ppm_bitmach(thread_decoder[i].output_file_name, thread_decoder[i].ref_fname, bpp );
            printf("%d\n", bm);
            switch(bm)
            {
                case FILES_ARE_EQUAL:
                    tst_resm( TINFO, "%s %s : bitmach passed", out, ref );
                    break;
                
                case FILES_ARE_NOT_EQUAL:
                    rv = TFAIL;
                    tst_resm( TWARN, "%s %s : bitmach failed", out, ref );
                    break;
    
                case NO_OUT_FILE:
                    tst_resm( TWARN, "bitmach : can't open %s", thread_decoder[i].output_file_name );
                    break;

                case NO_REF_FILE:
                    tst_resm( TWARN, "bitmach : can't open %s", thread_decoder[i].ref_fname );
                    break;
                    
                case OUT_IS_NOT_PPM:
                    tst_resm( TWARN, "bitmach : %s is not ppm", thread_decoder[i].output_file_name );
                    break;
                
                case REF_IS_NOT_PPM:
                    tst_resm( TWARN, "bitmach : %s is not ppm", thread_decoder[i].ref_fname );
                    break;
                    
                case FATAL_ERROR: default:
                    tst_resm( TWARN, "%s %s : bitmach : fatal error", out, ref );                    
                    break;
            }
        }
        rv += rv_mt[i];
    }								         
    
    return rv;
}

int preemptive_test()
{
    flist_t * node;
    int i;
    int rv = TPASS;    
    
    for( node = file_list, i = 0; node && i < 2; node = node->next, ++i )
    {
        tst_resm( TINFO, "input file: %s", node->inp_fname );      
		
        const framebuffer_t * fb = get_framebuffer();
        
        #define NONE_FILE "n/a"    
    
        thread_decoder[i].instance_id = i;
        if( i == 0 ) thread_decoder[i].offsetYDualScreen = -fb->height/4;
        else if( i == 1 ) thread_decoder[i].offsetYDualScreen = +fb->height/4;
        thread_decoder[i].endurance = 1;    
        
        thread_decoder[i].input_file_name      = strcmp( node->inp_fname, NONE_FILE ) != 0 ? node->inp_fname : NULL;
        thread_decoder[i].output_file_name     = strcmp( node->out_fname, NONE_FILE ) != 0 ? node->out_fname : NULL;
        thread_decoder[i].ref_fname            = strcmp( node->ref_fname, NONE_FILE ) != 0 ? node->ref_fname : NULL;
	    thread_decoder[i].dec_param.scale_mode = node->scale_mode;				     
        thread_decoder[i].dec_param.out_format = node->out_fmt;
        thread_decoder[i].width = node->width;
        thread_decoder[i].height = node->height;

        
        if( i == 0 )
        {
            if( pthread_create( &tid[i], NULL, (void *) &decode_picture_in_loop, (void *) &thread_decoder[i]) )
    		{
                tst_resm( TWARN, "Error creating thread %d", i );
		    	return rv;
    		}        
        }
        else if( i == 1 )
        {
            if( pthread_create( &tid[i], NULL, (void *) &decode_picture_on_timer, (void *) &thread_decoder[i]) )
    		{
                tst_resm( TWARN, "Error creating thread %d", i );
		    	return rv;
    		}                    
        }
    }
    
    for( i = 0; i < 2; ++i )
    {
        pthread_join( tid[i], NULL );
    }    
    
    for( i = 0; i < 2; ++i )
    {
        if( do_files_exist(thread_decoder[i].output_file_name, thread_decoder[i].ref_fname ) )
        {
            const char * out = thread_decoder[i].output_file_name;
            const char * ref = thread_decoder[i].ref_fname;
            int bpp = 3;
            if( thread_decoder[i].dec_param.out_format == E_OUTPUTFORMAT_RGB565 )
                bpp = 2;
            int bm = ppm_bitmach(thread_decoder[i].output_file_name, thread_decoder[i].ref_fname, bpp );
            switch(bm)
            {
                case FILES_ARE_EQUAL:
                    tst_resm( TINFO, "%s %s : bitmach passed", out, ref );
                    break;
                
                case FILES_ARE_NOT_EQUAL:
                    rv = TFAIL;
                    tst_resm( TWARN, "%s %s : bitmach failed", out, ref );
                    break;
    
                case NO_OUT_FILE:
                    tst_resm( TWARN, "bitmach : can't open %s", thread_decoder[i].output_file_name );
                    break;

                case NO_REF_FILE:
                    tst_resm( TWARN, "bitmach : can't open %s", thread_decoder[i].ref_fname );
                    break;
                    
                case OUT_IS_NOT_PPM:
                    tst_resm( TWARN, "bitmach : %s is not ppm", thread_decoder[0].output_file_name );
                    break;
                
                case REF_IS_NOT_PPM:
                    tst_resm( TWARN, "bitmach : %s is not ppm", thread_decoder[0].ref_fname );
                    break;
                    
                case FATAL_ERROR: default:
                    tst_resm( TWARN, "%s %s : bitmach : fatal error", out, ref );                    
                    break;
            }
        }
        rv += rv_mt[i];
    }								         
    
    return rv;
}
/*================================================================================================*/
/*================================================================================================*/
int VT_bmp_decoder_test(int test_case, int iter, const char * cfg_file)
{
    int rv = TFAIL;
    test_iter = iter;
    pid_t pid;
    int retval = 0;
    int robustnessFailed = FALSE;
    test_case_id = test_case;
    
    if( file_list )
	    delete_list( file_list );

    if( ROBUSTNESS_1 != test_case && ROBUSTNESS_2 != test_case )
    {
        if( !read_cfg( cfg_file, &file_list ) )		
        {
        	tst_resm( TWARN, "VT_bmp_decoder_test : can't parse %s", cfg_file );
	        return rv;
        }
    }
    
    const framebuffer_t * fb = get_framebuffer();
    assert(fb);
        
    /* Test launcher according to the test ID entered by user (default 0) */
	switch(test_case)
    {

		case NOMINAL_FUNCTIONALITY:
            tst_resm( TINFO, "EXHAUSTIVE DECODE test" );
            rv = exhaustive_decode();
            tst_resm( TINFO, "End of EXHAUSTIVE DECODE test");
			break;
			
		case ENDURANCE:
			tst_resm(TINFO, "ENDURANCE test");            
            {
                rv = TPASS;
                int i;
                for( i = 0; i < test_iter; ++i )
                {
                    rv += exhaustive_decode();
                }
            }
            
			tst_resm( TINFO, "End of ENDURANCE test");
			break;
		
		case ROBUSTNESS_1:		
        case ROBUSTNESS_2:
		
			tst_resm( TINFO, "ROBUSTNESS TEST" );
		
            if( ROBUSTNESS_1 == test_case )
            {
	    		/************************************************************
    			 * Decoding a word file. 			                        *
			     * Expected result is lastError == BMP_ERR_UNSUPPORTED_TYPE *
		    	 ************************************************************/
                 assert( word_file );
			 
	    		tst_resm( TINFO, "Decoding a word file... Expected error is BMP_ERR_UNSUPPORTED_TYPE" );
			 
    			thread_decoder[0].input_file_name = word_file;
			    thread_decoder[0].dec_param.out_format = E_OUTPUTFORMAT_RGB888;			     	
		    	thread_decoder[0].dec_param.scale_mode = E_NO_SCALE;				     
	    		thread_decoder[0].offsetYDualScreen = 0;
    			thread_decoder[0].instance_id = 0;
			    thread_decoder[0].endurance = 1;
						
		    	decode_picture( &thread_decoder[0] );
	    		if( lastError != BMP_ERR_UNSUPPORTED_TYPE )
    		    {
		    	    tst_resm( TWARN, "The BMP_ERR_UNSUPPORTED_TYPE return value has been expected" );
	    		}
    			if( lastError == BMP_ERR_NO_ERROR )
			    {
		    	    robustnessFailed = TRUE;
	        		tst_resm( TWARN, "ROBUSTNESS to bad flux FAILED" );
    			}
			    else
		    	{
	    		    tst_resm( TINFO, "ROBUSTNESS to bad flux PASSED\n" );			    
    			}
            }
            else if( ROBUSTNESS_2 == test_case )    
            {
    			/**********************************************************
	    		 * Decoding a word file with bitmap header. 	          *
		    	 * Expected result is lastError != BMP_ERR_NO_ERRROR	  *
			     **********************************************************/
			    assert( word_file_with_bmp_hdr );
                
    			tst_resm( TINFO, "Decoding a word file with bmp header... Expected result is an error" );
                
                /* Check if thre is a bitmap header in the input file. */
                FILE * test = fopen( word_file_with_bmp_hdr, "rb" );
                assert( test );
                char magic[2];
                unsigned long sz;
                fread( magic, 1, 2, test );
                fread( &sz,   1, 4, test );  
                fclose( test );                
                if( magic[0] != 'B' || magic[1] != 'M' || sz != 14 )
                {
                    tst_resm( TWARN, "The input file %s has no a bmp header", word_file_with_bmp_hdr );                                        
                    robustnessFailed = TRUE;
                }
                else
                {	    		 
    		    	thread_decoder[0].input_file_name = word_file_with_bmp_hdr;
	    		    thread_decoder[0].dec_param.out_format = E_OUTPUTFORMAT_RGB888;			     	
    	    		thread_decoder[0].dec_param.scale_mode = E_NO_SCALE;				     
	    	    	thread_decoder[0].offsetYDualScreen = 0;
		    	    thread_decoder[0].instance_id = 0;
    			    thread_decoder[0].endurance = 1;
			
        			decode_picture( &thread_decoder[0] );
	        		if( lastError == BMP_ERR_NO_ERROR )
		        	{
			            robustnessFailed = TRUE;
    	    	        tst_resm( TWARN, "ROBUSTNESS to corrupted file FAILED" );
            		}
	    	    	else
		    	    {
			            tst_resm( TINFO, "ROBUSTNESS to corrupted file PASSED" );			    
        			}
                }
            }
			 
			if( robustnessFailed == FALSE )
			    rv = TPASS;
			else     
			    rv = TFAIL;
			
			tst_resm( TINFO, "End of ROBUSTNESS TEST" );
			break;
			
		case RE_ENTRANCE:
			tst_resm( TINFO, "REENTRANCE test using 2 threads");
            rv = reentrance_test();            
            tst_resm( TINFO, "END of REENTRANCE test" );            
			break;

		case PRE_EMPTION:
			tst_resm( TINFO, "PREEMPTION test ");		
            rv = preemptive_test();
			tst_resm( TINFO, "END of PREEMPTION test" );
			break;

		case LOAD_TEST:
			tst_resm( TINFO, "LOAD test " );
			switch (pid = fork())
			{
				case -1:
			       	tst_resm( TINFO, "Fork failed" );
			       	return rv;
				case 0:       
					hogcpu();                    
			    default:
                    retval = exhaustive_decode();
				    /* kill child process once decode loop has ended*/
					if( kill(pid, SIGKILL) != 0 )
                    {
						tst_resm( TWARN, "Kill(SIGKILL) error" );
						return rv;
					}

					if( !retval )
						rv = TPASS;
			}	
			tst_resm( TINFO, "END LOAD test" );
			break;
            
		default:
			break;
	}

    return rv;
}

void write_data(unsigned char * scanline, int stride, int thread_idx )
{
    assert( thread_idx >= 0 && thread_idx < DECODER_THREAD );
    if( output_file[thread_idx] )
    {
        fwrite( scanline, 1, stride, output_file[thread_idx] );        
    }
}

void write_ppm_header( FILE * out, int width, int height )
{
    assert( out && width > 0 && height > 0 );    
    fprintf( out, "P6\n%d %d\n255\n", width, height );
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

int ppm_bitmach( const char * out_fname, const char * ref_fname, int bpp )
{

    if( !compare_files(out_fname, ref_fname) )
        return FILES_ARE_NOT_EQUAL;
    else
        return FILES_ARE_EQUAL;            
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
