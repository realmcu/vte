/* / Copyright (C) 2004, Freescale Semiconductor, Inc. All Rights Reserved THIS SOURCE CODE IS
 * CONFIDENTIAL AND PROPRIETARY AND MAY NOT BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
 * Freescale Semiconductor, Inc. */

/**
@file png_decoder_test.c

@brief VTE C source PNG decoder test

PNG decoder test source

@par Portability:
        SCM-A11, Argon+
        arm-linux-gcc 3.4
*/

/*======================== REVISION HISTORY ==================================

Author (core ID)      Date         CR Number    Description of Changes
-------------------   ----------   ----------   ------------------------------
D.Simakov/smkd001c    28/09/2004   TLSbo41678 	Initial version	
D.Simakov/smkd001c    06/04/2005   TLSbo47116   The reentrance, preemptive and load
                                                testcases were added
D.Simakov             18/04/2006   TLSbo66230   Updted with the new decoder's version                                                 

=============================================================================*/

#ifdef __cplusplus
extern  "C"
{
#endif

/*======================== INCLUDE FILES ====================================*/

        /* Standard Include Files */
#include <errno.h>

        /* Harness Specific Include Files. */
#include "test.h"

        /* Verification Test Environment Include Files */
#include "png_decoder_test.h"

#include "stuff/fconf.h"
#include "stuff/fb_draw_api.h"

/*======================== LOCAL CONSTANTS ==================================*/

/*======================== LOCAL MACROS =====================================*/

#define MAX_CODEC_THREADS 4

#define SAFE_DELETE(p) {if(p){free(p);p=NULL;}}

#define NONE_FILE "n/a"

#define INPUT_BUFFER_SIZE 4096  /* size of the input buffer for each decoding thread */

#define DECL_PNG_APP_READ_DATA(thread_idx)							 \
PNGD_RET_TYPE PNG_app_read_data##thread_idx( void * input_ptr, PNG_UINT8 * input_data,		 \
                                          PNG_UINT32 length_requested, PNG_UINT32 length_returned, void * p) \
{												 \
    assert( thread_idx < MAX_CODEC_THREADS ); /* compile-time _assert should be there instead*/	 \
    png_decoder_thread_t * dec_thread = &codec_thread[thread_idx];				 \
    assert( dec_thread->inp_fstream );								 \
                                                                                                 \
    if( !input_ptr ) input_ptr = dec_thread->inp_fstream;					 \
                                                                                                 \
    length_returned = fread( (void*)input_data, 1, length_requested, (FILE*)input_ptr );	 \
    return PNGD_OK;										 \
}

/*======================== LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS) =======*/

        typedef struct
        {
                unsigned long thread_idx;

                const char *inp_fname;
                const char *out_fname;
                const char *ref_fname;

                char    output_buffer[4 * INPUT_BUFFER_SIZE];
                char    input_buffer[INPUT_BUFFER_SIZE];

                PNG_Decoder_Object *dec_obj;
                PNG_Decoder_Params dec_param;

                int     endurance_count;

                pthread_t tid;

                FILE   *inp_fstream;
                FILE   *out_fstream;
                FILE   *ref_fstream;

                int     decoded_rows;

                PNGD_RET_TYPE last_png_error;

                int     thread_finished;
                int     ltp_retval;

        } png_decoder_thread_t;

        typedef PNGD_RET_TYPE(*PNG_app_read_data_t) (void *, PNG_UINT8 *, PNG_UINT32, PNG_UINT32,
                                                     void *);

/*======================== LOCAL VARIABLES ==================================*/

        static png_decoder_thread_t codec_thread[MAX_CODEC_THREADS];
        static PNG_app_read_data_t png_app_read_data[MAX_CODEC_THREADS];
        static char progress[] = "-\\|/";       /* console progress bar */
        static flist_t *file_list = NULL;       /* list of input/output/reference files */
        static int num_threads = 0;     /* the number of threads for re-entrance & pre-emption */
        static int set_priors = FALSE;  /* TRUE means that run_codec shall call nice(XXX) */
        static int focus_thread = -1;   /* focus thread is a thread that prints himself statistics
                                         * and statistics of another threads */
        extern testapp_configuration_t testapp_cfg;

/*======================== GLOBAL CONSTANTS =================================*/

/*======================== GLOBAL VARIABLES =================================*/

/*======================== LOCAL FUNCTION PROTOTYPES ========================*/

        int     codec_engine(void *ptr);

        int     run_decoding_loop(png_decoder_thread_t * dec_thread);
        int     open_decoder_files(png_decoder_thread_t * dec_thread);
        int     close_decoder_files(png_decoder_thread_t * dec_thread);
        void    on_data_output(png_decoder_thread_t * dec_thread);

        // PNGD_RET_TYPE PNG_get_new_data( PNG_UINT8 ** buf_ptr, PNG_UINT32 * buf_len,
        // PNG_Decoder_Object * dec_obj );

        /* helper functions */
        int     run_codec(void *ptr);

        /* test cases */
        int     nominal_functionality_test();
        int     endurance_test();
        int     robustness_test();
        int     re_entrance_test();
        int     pre_emptive_test();
        int     load_test();

        void    reset_decoder_instance(unsigned long thread_idx);
        int     bitmach(const char *out_fname, const char *ref_fname);

/*======================== LOCAL FUNCTIONS ==================================*/

             
             
            DECL_PNG_APP_READ_DATA(0)
            DECL_PNG_APP_READ_DATA(1) DECL_PNG_APP_READ_DATA(2) DECL_PNG_APP_READ_DATA(3)
/*======================== GLOBAL FUNCTIONS =================================*/
/*================================================================================================*/
/*================================================================================================*/
        int     VT_png_decoder_setup()
        {
                int     rv = TFAIL;

                        png_app_read_data[0] = PNG_app_read_data0;
                        png_app_read_data[1] = PNG_app_read_data1;
                        png_app_read_data[2] = PNG_app_read_data2;
                        png_app_read_data[3] = PNG_app_read_data3;

                        rv = TPASS;
                        return rv;
        }


/*================================================================================================*/
/*================================================================================================*/
        int     VT_png_decoder_cleanup()
        {
                int     rv = TFAIL;

                if (file_list)
                        delete_list(file_list);

                rv = TPASS;
                return rv;
        }


/*================================================================================================*/
/*================================================================================================*/
        int     VT_png_decoder_test()
        {
                int     rv = TFAIL;

                if (file_list)
                        delete_list(file_list);

                if (!read_cfg(testapp_cfg.cfg_fname, &file_list))
                {
                        tst_resm(TWARN, "VT_png_decoder_test : can't parse %s",
                                 testapp_cfg.cfg_fname);
                        return rv;
                }

                switch (testapp_cfg.testcase)
                {
                case NOMINAL_FUNCTIONALITY:
                        tst_resm(TINFO, "Nominal functionality test");
                        rv = nominal_functionality_test();
                        tst_resm(TINFO, "End of nominal functionality test");
                        break;

                case ENDURANCE:
                        tst_resm(TINFO, "Endurance test");
                        rv = endurance_test();
                        tst_resm(TINFO, "End of endurance test");
                        break;

                case ROBUSTNESS:
                        tst_resm(TINFO, "Robustness test");
                        rv = robustness_test();
                        tst_resm(TINFO, "End of robustness test");
                        break;

                case REENTRANCE:
                        tst_resm(TINFO, "Reentrance test");
                        rv = re_entrance_test();
                        tst_resm(TINFO, "End of reentrance test");
                        break;

                case PREEMPTION:
                        tst_resm(TINFO, "Preemptive test");
                        rv = pre_emptive_test();
                        tst_resm(TINFO, "End of preemptive test");
                        break;

                case LOAD:
                        tst_resm(TINFO, "Load test");
                        rv = load_test();
                        tst_resm(TINFO, "End of load test");
                        break;

                default:
                        tst_resm(TWARN, "Wrong test case");
                        break;
                }

                return rv;
        }


/*================================================================================================*/
/*================================================================================================*/
        int     codec_engine(void *ptr)
        {
                assert(ptr);

                int     rv = TFAIL;
                png_decoder_thread_t *dec_thread = (png_decoder_thread_t *) ptr;

                assert(dec_thread->thread_idx < MAX_CODEC_THREADS);

                PNG_Decoder_Object *dec_obj;

                /* allocate memory for the decoder object */
                dec_obj = (PNG_Decoder_Object *) malloc(sizeof(PNG_Decoder_Object));
                if (!dec_obj)
                {
                        tst_resm(TWARN,
                                 "codec_engine : can't allocate memory for the decoder (%d bytes)",
                                 sizeof(PNG_Decoder_Object));
                        return rv;
                }
                dec_thread->dec_obj = dec_obj;

                /* configure the decoder */
                dec_obj->PNG_app_read_data = png_app_read_data[dec_thread->thread_idx];
                dec_obj->PNG_app_malloc = PNG_app_malloc;
                dec_obj->PNG_app_free = PNG_app_free;
                dec_obj->dec_param.outformat = E_PNG_OUTPUTFORMAT_ARGB;
                dec_obj->dec_param.scale_mode = dec_thread->dec_param.scale_mode;
                dec_obj->dec_param.output_width = dec_thread->dec_param.output_width;
                dec_obj->dec_param.output_height = dec_thread->dec_param.output_height;

                /* open needed file streams */
                if (TPASS != open_decoder_files(dec_thread))
                        return rv;

                fseek(dec_thread->inp_fstream, 0, SEEK_SET);

                /* init */
                dec_thread->last_png_error = PNG_dec_init(dec_obj);
                if (dec_thread->last_png_error != PNGD_OK)
                {
                        tst_resm(TWARN, "codec_engine : PNG_decoder_init fails #%d",
                                 dec_thread->last_png_error);
                        return rv;
                }

                /* why should i do it in the client code??? */
                dec_obj->pixels =
                    (PNG_INT32 *) malloc(4 * dec_obj->dec_info_init.output_width * sizeof(int));
                if (!dec_obj->pixels)
                {
                        tst_resm(TWARN,
                                 "codec_engine : can't allocate memory for the input pixels");
                        return rv;
                }

                /* set focus */
                if (focus_thread == -1)
                        focus_thread = dec_thread->thread_idx;

                /* run decoding loop */
                if (TPASS != run_decoding_loop(dec_thread))
                        return rv;

                /* cleanup... */
                PNG_cleanup(dec_thread->dec_obj);
                close_decoder_files(dec_thread);
                SAFE_DELETE(dec_thread->dec_obj);

                rv = TPASS;
                return rv;
        }


/*================================================================================================*/
/*================================================================================================*/
        void    detect_enter()
        {
                if (!testapp_cfg.delay)
                        return;

                int     fd_console = 0; /* 0 is the video input */
                fd_set  fdset;
                struct timeval timeout;
                char    c;

                FD_ZERO(&fdset);
                FD_SET(fd_console, &fdset);
                timeout.tv_sec = testapp_cfg.delay;     /* set timeout !=0 => blocking select */
                timeout.tv_usec = 0;
                if (select(fd_console + 1, &fdset, 0, 0, &timeout) > 0)
                {
                        do
                        {
                                read(fd_console, &c, 1);
                        }
                        while (c != 10);        // i.e. line-feed 
                }
        }


/*================================================================================================*/
/*================================================================================================*/
        int     run_decoding_loop(png_decoder_thread_t * dec_thread)
        {
                assert(dec_thread);
                assert(dec_thread->dec_obj);

                int     rv = TFAIL;
                int     pass;
                int     row_index;

                PNG_Decoder_Object *dec_obj = dec_thread->dec_obj;      /* alias */

                const framebuffer_t *fb = get_framebuffer();

                assert(fb);
                argb_color_t white = { 1.0f, 1.0f, 1.0f, 1.0f };

                /* Decode row by row */
                /* For interlaced images passes=7, else number of passes=1 */
                for (pass = 0; pass < dec_obj->dec_info_init.number_passes; ++pass)
                {
                        // if( pass == 6 ) printf( "\n" );
                        if (pass == 6 || dec_obj->dec_info_init.number_passes == 1)
                        {
                                if (num_threads == 0)
                                {
                                        fb->clear_screen(&white);
                                        dec_thread->decoded_rows = 0;
                                }
                        }
                        for (row_index = 0; row_index < dec_obj->dec_info_init.output_height;
                             ++row_index)
                        {
                                dec_thread->last_png_error =
                                    PNG_decode_row(dec_obj, dec_thread->output_buffer);
                                if (dec_thread->last_png_error != PNGD_OK)
                                {
                                        tst_resm(TWARN, "run_decoding_loop : PNG_decode_row fails");
                                        return rv;
                                }
                                if (pass == 6 || dec_obj->dec_info_init.number_passes == 1)
                                {
                                        on_data_output(dec_thread);
                                        dec_thread->decoded_rows++;

                                        if (dec_thread->thread_idx == focus_thread)
                                        {
                                                int     k;
                                                png_decoder_thread_t *another_dec_thread;

                                                for (k = 0; k < num_threads; k++)
                                                {
                                                        another_dec_thread = &codec_thread[k];
                                                        printf("-t[%lu] rows[%d] ",
                                                               another_dec_thread->thread_idx,
                                                               another_dec_thread->decoded_rows);
                                                }
                                                if (num_threads == 0)
                                                {
                                                        printf("-t[%lu] rows[%d] ",
                                                               dec_thread->thread_idx,
                                                               dec_thread->decoded_rows);
                                                }
                                                printf("%c\r",
                                                       progress[(row_index) %
                                                                (sizeof(progress) - 1)]);
                                        }
                                        fflush(stdout);
                                }
                                else
                                {
                                        // printf( "interlace passes, please wait pass[%d] \r", pass
                                        // );
                                        // fflush( stdout );
                                }
                        }
                        dec_obj->rows_decoded = 0;
                        dec_obj->dec_info_init.pass++;
                }

                // printf( "\n" );

                detect_enter();

                rv = TPASS;
                return rv;
        }


/*================================================================================================*/
/*================================================================================================*/
        int     open_decoder_files(png_decoder_thread_t * dec_thread)
        {
                assert(dec_thread);

                int     rv = TFAIL;

                /* input file */
                if (dec_thread->inp_fname)
                {
                        if (!(dec_thread->inp_fstream = fopen(dec_thread->inp_fname, "rb")))
                        {
                                tst_resm(TWARN, "open_decoder_files : can't open %s",
                                         dec_thread->inp_fname);
                                return rv;
                        }
                }

                /* output file */
                if (dec_thread->out_fname)
                {
                        if (!(dec_thread->out_fstream = fopen(dec_thread->out_fname, "wb+")))
                        {
                                tst_resm(TWARN, "open_decoder_files : can't create %s",
                                         dec_thread->out_fname);
                                return rv;
                        }
                }

                rv = TPASS;
                return rv;
        }


/*================================================================================================*/
/*================================================================================================*/
        int     close_decoder_files(png_decoder_thread_t * dec_thread)
        {
                assert(dec_thread);

                int     rv = TFAIL;

                /* input file */
                if (dec_thread->inp_fstream)
                {
                        fclose(dec_thread->inp_fstream);
                        dec_thread->inp_fstream = NULL;
                }

                /* output file */
                if (dec_thread->out_fstream)
                {
                        fclose(dec_thread->out_fstream);
                        dec_thread->out_fstream = NULL;
                }

                rv = TPASS;
                return rv;
        }


/*================================================================================================*/
/*================================================================================================*/
        int     run_codec(void *ptr)
        {
                int     i,
                        rv = TFAIL;

                if (!ptr)
                        return rv;

                png_decoder_thread_t *dec_thread = (png_decoder_thread_t *) ptr;

                if (set_priors)
                {
                        int     nice_inc = ((float) 20 / num_threads) * dec_thread->thread_idx; // rand()%40;

                        if (nice(nice_inc) < 0)
                        {
                                tst_resm(TWARN, "run_codec : nice(%d) failed", nice_inc);
                        }
                }

                dec_thread->ltp_retval = codec_engine(ptr);
                dec_thread->thread_finished = TRUE;
                if (dec_thread->thread_idx == focus_thread)
                {
                        for (i = 0; i < num_threads; ++i)
                        {
                                if (!codec_thread[i].thread_finished)
                                {
                                        focus_thread = codec_thread[i].thread_idx;
                                        break;
                                }
                        }
                }
                return dec_thread->ltp_retval;
        }


/*================================================================================================*/
/*================================================================================================*/
        int     nominal_functionality_test()
        {
                flist_t *node;
                int     i;
                int     rv = TPASS;
                png_decoder_thread_t *dec_thread = &codec_thread[0];

                const framebuffer_t *fb = get_framebuffer();

                assert(fb);

                for (node = file_list, i = 0; node; node = node->next, ++i)
                {
                        reset_decoder_instance(0);

#ifdef DEBUG_TEST
                        printf("\n\t----- decoding info ------\n"
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
                               node->out_height, node->inp_fname, node->out_fname, node->ref_fname);
#endif
                        tst_resm(TINFO, "input_file: %s", node->inp_fname);

                        dec_thread->inp_fname =
                            strcmp(node->inp_fname, NONE_FILE) != 0 ? node->inp_fname : NULL;
                        dec_thread->out_fname =
                            strcmp(node->out_fname, NONE_FILE) != 0 ? node->out_fname : NULL;
                        dec_thread->ref_fname =
                            strcmp(node->ref_fname, NONE_FILE) != 0 ? node->ref_fname : NULL;
                        dec_thread->dec_param.scale_mode = node->scale_mode;
                        dec_thread->dec_param.output_width =
                            node->out_width ? node->out_width : fb->width;
                        dec_thread->dec_param.output_height =
                            node->out_height ? node->out_height : fb->height;

                        /* call decoder engine */
                        rv += run_codec(dec_thread);

                        /* bitmatch */
                        if (dec_thread->ref_fname && dec_thread->out_fname)
                        {
                                FILE   *ref_fstream = fopen(dec_thread->ref_fname, "rb");

                                if (ref_fstream)
                                {
                                        fclose(ref_fstream);
                                        int     bm =
                                            bitmach(dec_thread->out_fname, dec_thread->ref_fname);
                                        if (!bm)
                                        {
                                                tst_resm(TWARN, "bitmatch failed");
                                                rv = TFAIL;
                                        }
                                        else
                                        {
                                                tst_resm(TINFO, "bitmatch passed");
                                        }
                                }
                                else
                                {
                                        tst_resm(TWARN, "the reference file \'%s\' does not exist",
                                                 dec_thread->ref_fname);
                                }
                        }
                }

                return rv;
        }


/*================================================================================================*/
/*================================================================================================*/
        int     endurance_test()
        {
                int     i;
                int     rv = TPASS;

                for (i = 0; i < testapp_cfg.iter; ++i)
                {
                        tst_resm(TINFO, "The %d iteration is started", i + 1);
                        rv += nominal_functionality_test();
                        tst_resm(TINFO, "The %d iteration is completed", i + 1);
                }
                return rv;
        }


/*================================================================================================*/
/*================================================================================================*/
        int     robustness_test()
        {
                flist_t *node;
                int     i;
                int     rv = TPASS;
                png_decoder_thread_t *dec_thread = &codec_thread[0];

                const framebuffer_t *fb = get_framebuffer();

                assert(fb);

                for (node = file_list, i = 0; node; node = node->next, ++i)
                {
                        reset_decoder_instance(0);

                        tst_resm(TINFO, "input file: %s", node->inp_fname);

                        dec_thread->inp_fname =
                            strcmp(node->inp_fname, NONE_FILE) != 0 ? node->inp_fname : NULL;
                        dec_thread->out_fname = NULL;
                        dec_thread->ref_fname = NULL;
                        dec_thread->dec_param.scale_mode = node->scale_mode;
                        dec_thread->dec_param.output_width =
                            node->out_width ? node->out_width : fb->width;
                        dec_thread->dec_param.output_height =
                            node->out_height ? node->out_height : fb->height;

                        /* call decoder engine */
                        rv += run_codec(dec_thread);
                }

                return rv != TPASS ? TPASS : rv;
        }


/*================================================================================================*/
/*================================================================================================*/
        int     re_entrance_test()
        {
                flist_t *node;
                int     i;
                int     rv = TPASS;
                png_decoder_thread_t *dec_thread;

                const framebuffer_t *fb = get_framebuffer();

                assert(fb);

                for (node = file_list, i = 0; node && i < MAX_CODEC_THREADS;
                     node = node->next, ++i);
                num_threads = i;

                for (node = file_list, i = 0; node && i < MAX_CODEC_THREADS; node = node->next, ++i)
                {
                        reset_decoder_instance(i);
                        dec_thread = &codec_thread[i];

                        dec_thread->inp_fname =
                            strcmp(node->inp_fname, NONE_FILE) != 0 ? node->inp_fname : NULL;
                        dec_thread->out_fname =
                            strcmp(node->out_fname, NONE_FILE) != 0 ? node->out_fname : NULL;
                        dec_thread->ref_fname =
                            strcmp(node->ref_fname, NONE_FILE) != 0 ? node->ref_fname : NULL;
                        dec_thread->dec_param.scale_mode = node->scale_mode;
                        dec_thread->dec_param.output_width =
                            node->out_width ? node->out_width : fb->width;
                        dec_thread->dec_param.output_height =
                            node->out_height ? node->out_height : fb->height;

                        if (pthread_create(&dec_thread->tid, NULL, (void *) &run_codec, dec_thread))
                        {
                                tst_resm(TWARN, "re_entrance_test : error creating thread %d", i);
                                return TFAIL;
                        }
                }

                // num_threads = i;
                for (i = 0; i < num_threads; ++i)
                {
                        dec_thread = &codec_thread[i];
                        pthread_join(dec_thread->tid, NULL);
                }

                for (i = 0; i < num_threads; ++i)
                {
                        dec_thread = &codec_thread[i];
                        /* bitmatch */
                        if (dec_thread->ref_fname && dec_thread->out_fname)
                        {
                                FILE   *ref_fstream = fopen(dec_thread->ref_fname, "rb");

                                if (ref_fstream)
                                {
                                        fclose(ref_fstream);
                                        int     bm =
                                            bitmach(dec_thread->out_fname, dec_thread->ref_fname);
                                        if (!bm)
                                        {
                                                tst_resm(TWARN, "bitmatch failed");
                                                rv = TFAIL;
                                        }
                                        else
                                        {
                                                tst_resm(TINFO, "bitmatch passed");
                                        }
                                }
                                else
                                {
                                        tst_resm(TWARN, "the reference file \'%s\' does not exist",
                                                 dec_thread->ref_fname);
                                }
                        }
                        rv += dec_thread->ltp_retval;
                }

                return rv;
        }

        int     pre_emptive_test()
        {
                set_priors = TRUE;
                return re_entrance_test();
        }

/*================================================================================================*/
/*================================================================================================*/
        void    hogcpu()
        {
                while (1)
                {
                        sqrt(rand());
                }
        }


/*================================================================================================*/
/*================================================================================================*/
        int     load_test()
        {
                int     rv = TFAIL;
                pid_t   pid;

                switch (pid = fork())
                {
                case -1:
                        tst_resm(TWARN, "load_envirounment_test : fork failed");
                        return rv;
                case 0:
                        /* child process */
                        hogcpu();
                default:
                        /* parent */
                        sleep(10);
                        rv = nominal_functionality_test();
                        /* kill child process once decode/encode loop has ended */
                        if (kill(pid, SIGKILL) != 0)
                        {
                                tst_resm(TWARN, "load_envirounment_test : Kill(SIGKILL) error");
                                return rv;
                        }
                }
                return rv;
        }


/*================================================================================================*/
/*================================================================================================*/
        void    reset_decoder_instance(unsigned long thread_idx)
        {
                png_decoder_thread_t *dec_thread;

                if (thread_idx < MAX_CODEC_THREADS)
                {
                        dec_thread = &codec_thread[thread_idx];

                        dec_thread->thread_idx = thread_idx;
                        dec_thread->last_png_error = PNGD_OK;
                        dec_thread->inp_fname = NULL;
                        dec_thread->out_fname = NULL;
                        dec_thread->ref_fname = NULL;
                        dec_thread->decoded_rows = 0;
                        dec_thread->thread_finished = FALSE;
                }
        }


/*================================================================================================*/
/*================================================================================================*/
        int     bitmach(const char *out_fname, const char *ref_fname)
        {
                FILE   *pf1,
                       *pf2;
                int     rd1,
                        rd2;
                unsigned long dw1,
                        dw2;

                if (out_fname && ref_fname)
                {
                        pf1 = fopen(out_fname, "rb");
                        pf2 = fopen(ref_fname, "rb");

                        if (pf1 && pf2)
                        {
                                while (TRUE)
                                {
                                        if (feof(pf1) != feof(pf2))
                                        {
                                                break;  /* bitmach fails */
                                        }

                                        rd1 = fread(&dw1, 1, sizeof(dw1), pf1);
                                        rd2 = fread(&dw2, 1, sizeof(dw2), pf2);

                                        if (dw1 != dw2)
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


/*================================================================================================*/
/*================================================================================================*/
        void    PNG_app_free(void *ptr, void *pAppContext)
        {
                SAFE_DELETE(ptr);
        }


/*================================================================================================*/
/*================================================================================================*/
        void   *PNG_app_malloc(void *ptr, PNG_UINT32 size, void *pAppContext)
        {
                return (PNG_UINT8 *) calloc(size, sizeof(PNG_UINT8));
        }


/*================================================================================================*/
/*================================================================================================*/
        void    on_data_output(png_decoder_thread_t * dec_thread)
        {
                assert(dec_thread);
                assert(dec_thread->dec_obj);

                const framebuffer_t *fb = get_framebuffer();

                assert(fb);

                int     width = dec_thread->dec_obj->dec_info_init.output_width;
                int     height = dec_thread->dec_obj->dec_info_init.output_height;

                int     y_offset = 0;

                if (num_threads)
                {
                        int     h = (float) fb->height / num_threads;
                        // float dy = (float)fb->height / num_threads;
                        // y_offset = (int)(dy * dec_thread->thread_idx); 
                        y_offset = h / 2 - height / 2;
                        y_offset += dec_thread->thread_idx * h / 2;
                }
                else if (height <= fb->height)
                {
                        y_offset = fb->height / 2 - height / 2;
                }

                // printf( "<<<<<<<<<<<<<<<<<<%d %d %d >>>>>>>>>>>>>>>>>>>>>>>>>\n", num_threads,
                // dec_thread->thread_idx, y_offset);

                int     x1 = 0,
                    x2 = fb->width;

                if (dec_thread->dec_obj->dec_info_init.output_width <= x2)
                {
                        x1 = fb->width - width / 2 - fb->width / 2;
                        x2 = fb->width + width / 2 - fb->width / 2;
                }

                if (x1 < 0)
                        x1 = 0;
                if (x2 > fb->width)
                        x2 = fb->width;

                // framebuffer.draw_scanline( dec_thread->output_buffer, 
                // dec_thread->decoded_rows + y_offset, 
                // x1, x2, 32 );
                fb->draw_scanline(dec_thread->output_buffer,
                                  x1, dec_thread->decoded_rows + y_offset, width, PF_ARGB_8888);

                if (dec_thread->out_fstream)
                {
                        if (dec_thread->decoded_rows == 0)
                        {
                                /* ppm header */
                                fprintf(dec_thread->out_fstream, "%s\n", "P6");
                                fprintf(dec_thread->out_fstream, "%d %d\n",
                                        dec_thread->dec_obj->dec_info_init.output_width,
                                        dec_thread->dec_obj->dec_info_init.output_height);
                                fprintf(dec_thread->out_fstream, "%d\n", 255);
                                fflush(dec_thread->out_fstream);
                        }

                        // ARGB
                        // fwrite( dec_thread->output_buffer, 1,
                        // dec_thread->dec_obj->dec_info_init.output_width * 3,
                        // dec_thread->out_fstream );
                        unsigned char *pix8888 = dec_thread->output_buffer;
                        int     i;

                        for (i = 0; i < width; ++i)
                        {
                                pix8888++;      /* skip alpha */
                                fwrite(pix8888, 1, 3, dec_thread->out_fstream); // write rgb
                                pix8888 += 3;
                        }
                }
        }

        int     DebugLogText(short int msgid, char *fmt, ...)
        {
                return 0;
        }
