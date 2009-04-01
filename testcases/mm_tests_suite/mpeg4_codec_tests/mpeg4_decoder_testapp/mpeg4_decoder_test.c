/*/
  Copyright (C) 2004, Freescale Semiconductor, Inc. All Rights Reserved
  THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
  BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
  Freescale Semiconductor, Inc.
*/

/**
@file   mpeg4_decoder_test.c

@brief VTE C source template

Description of the file

@par Portability:
        Indicate if this module is portable to other compilers or platforms.
        If not, indicate specific reasons why is it not portable.
*/

/*======================== REVISION HISTORY ==================================

Author (core ID)      Date         CR Number    Description of Changes
-------------------   ----------   ----------   ------------------------------
Filinova Natalya      15/02/2005   TLSbo47115    Initial version
Filinova Natalya      28/02/2005   TLSbo47115    Initial version
D.Simakov/smkd001c    01/04/2005   TLSbo49126    Updates according to the new API.
                                                 The draw routines is also improved. 
                                                 Now it clamps to 0..255 range.
D.Simakov/smkd001c    21/07/2005   TLSbo52629    Relocatability test case was added                                                  
D.Simakov/smkd001c    20/10/2005   TLSbo57009    Update
A.Pshenichnikov       28/11/2005   TLSbo59468    bugs in draw routines were fixed	
A.Pshenichnikov       29/11/2005   TLSbo59468    twinkling was fixed 
D.Simakov/smkd001c    16/12/2005   TLSbo59667    Advanced decode w skip fixed 
=============================================================================*/

#ifdef __cplusplus
extern "C"{
#endif

/*======================== INCLUDE FILES ====================================*/
/* Standard Include Files */
#include <errno.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <assert.h>               
#include <unistd.h>       
#include <pthread.h>      
#include <sys/time.h>     
#include <math.h>         
#include <linux/fb.h>     
#include <string.h>

    
/* Harness Specific Include Files. */
#include "test.h"

/* Verification Test Environment Include Files */
#include "mpeg4_decoder_test.h"
#include "mpeg4_dec_api.h"
#include "kev.h"

#define SKIP_FRAMES eMPEG4DSkipFrames
#define SKIP_2NEXT_IFRAME eMPEG4DSkip2NextIFrame

/*======================== LOCAL CONSTANTS ==================================*/


/*======================== LOCAL MACROS =====================================*/
#define MAX_DECODER_THREADS 2

#define DBGM(s,n) {printf("%d:---%s---\n",n,s);fflush(stdout);}

/*======================== LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS) =======*/

typedef struct
{
    pthread_t          tid;   /**< Thread id */
    UINT               instance_id;                /* id of thread */  
    FILE*              input_file;                 /* pointer to input bitstream  */
    char               own_out_file_name[256];     /* output file name without suffix */  
    KevFile            sKevFileObj;                /* structure for holding YCbCr data */
    UCHAR              *frame;                     /* buffer for one frame */
    UCHAR              *pY;                        /* buffer for Y component */
    UCHAR              *pCb;                       /* buffer for Cb component */
    UCHAR              *pCr;                       /* buffer for Cr component */
    int                FWidth, FHeight;            /* height and width of frame */ 
    int                iBeetwinFramesTime;         /* delay between showed frames */ 
    int                read_bytes;                 /**/

} Mpeg4_dec_thread_t; /* structure for thread of decoder */


/*======================== LOCAL VARIABLES ==================================*/

Mpeg4_dec_thread_t dec_thread[ MAX_DECODER_THREADS ];  /* array of decode threads */
static int thread_synchro      = FALSE;	               /* boolean used by the loop thread to inform the thread */
static int test_iter           = DEFAULT_ITERATIONS;   /* default iteration is hard coded */
static int test_testcase       = NOMINAL_FUNCTIONALITY;/* default test Nominal Functionality */
static int test_output         = WITHOUT_OUTPUT;       /* default output data have not output form*/
static int test_skip_mode      = SKIP_TO_INTRA;        /* default the skipping mode is to skip to INTRA Frame*/
static int test_start_frame    = DEFAULT_START_FRAME;  /* default start skipping from frame number */
static int test_number_to_skip = DEFAULT_NUM_TO_SKIP;  /* default number of frames to skip */
static int test_disp_time      = DEFAULT_DISPLAY_TIME; /* default do not displays time of decoding*/
static int test_delay          = DEFAULT_DELAY;        /*default delay between displaying frames*/
static int test_comm           = DEFAULT_COMM;         /*default comments are not displayed with decode process*/
static int display_info        = 1;

sMpeg4DecObject   pMpeg4DecObj[MAX_DECODER_THREADS]; /*main data structures for two treads of decoder*/

/*======================== GLOBAL CONSTANTS =================================*/

static char* input_dir = DEFAULT_PATH;
static char* input_file_name = DEFAULT_INPUT_BITSTREAM; /* name of input bitstream */
static char* output_file_name = "_kev";                 /* suffix for output folder name for each thread */
                                       
/*======================== GLOBAL VARIABLES =================================*/

static int fd_fb;			/* frame buffer file descriptor FGA for display purpose */
static UCHAR *fb;	        	/* pointer to frame buffer FGA for display purpose */
struct fb_var_screeninfo gScreenInfo;	/* global variable that contains the screen parameters */
static int gVideoBufferSize = 0;        /* size of frame buffer*/
static int BitPerPix = 24;              /* bit per pixel */
static int framebuffer = FRAMEBUFFER;   /* frame buffer is supported*/
extern int skip_at_flag;

/*======================== LOCAL FUNCTION PROTOTYPES ========================*/

/* main decod function*/
int decoder_engine( void * ptr );

/* helper functions */
int  run_decoder_in_loop ( void * ptr );                    
int  run_decoder_on_timer( void * ptr );
void timer_handler( int signum );
void hogcpu();                                      /* loading CPU */

/*display functions */
int configure_screen();     /* configure frame buffer */                   
int draw_frame24(void* ptr,sMpeg4DecoderParams *pDecParams); /* write frame from YCbCr into RGB  with 24 bit per pixel */
int draw_frame16(void* ptr,sMpeg4DecoderParams *pDecParams); /* write frame from YCbCr into RGB  with 16 bit per pixel */

int (*draw_frame)(void* ,sMpeg4DecoderParams *) = draw_frame24;
void show_frame(void* ptr); /* show frame on LCD with writting into frame buffer */  

/* test functions */
int nominal_functionality_test();    /* decode */
int advanced_functionality_test();   /* decode with skipping */
int endurance_test            ();    /* decode process with some iterations */
int re_entrance_test          ();    /* some threads of decode process together */
int pre_emption_test          ();    /* one thread of decode is normal and one is on timer */
int load_envirounment_test    ();    /* one thread is decode and one loads CPU */
int robustness_test           ();    /* input bitstream is error bitstream */
int relocatability_test       ();

/* memory functions */
int allocate_memory(sMpeg4DecMemAllocInfo *psMemInfo);        /* allocate memory for decoder */
void free_memory_app (sMpeg4DecObject* psMpeg4DecObj);        /* free memory allocated application */
void free_bitbuffer(UCHAR **ppu8BitBuffer,int* pBitBuffLen);  /* free buffer for read bitstream */
void free_memory_dec (sMpeg4DecMemAllocInfo *psMemAllocInfo); /* free memory allocated for decoder */
void free_decoder_thread (void* ptr);                         /* close files opened decode thread */

/* decoder functions */
eMpeg4DecRetType eMPEG4DecoderLoop (sMpeg4DecObject *psMpeg4DecObject,
                                    void*  ptr);                  /* decode process */
int cbkMPEG4DBufRead (int s32EncStrBufLen, UCHAR *pu8EncStrBuf, 
                      int offset, void *pvAppContext);                        /* read bitstream */  

           
/*======================== GLOBAL FUNCTIONS =================================*/


/*===== VT_mpeg4_decoder_setup =====*/
/**
@brief  assumes the pre-condition of the test case execution

@param  None
  
@return On success - return TPASS
        On failure - return the error code
*/

int VT_mpeg4_decoder_setup()
{
    int rv = TFAIL;
    
    if( configure_screen() == TFAIL) framebuffer = 0; /* if frame buffer is not supported then framebuffer flag = 0 */
    
    rv = TPASS;
    
    return rv;
}



/*===== VT_mpeg4_decoder_cleanup =====*/
/**
@brief  assumes the post-condition of the test case execution

@param  None
  
@return On success - return TPASS
        On failure - return the error code
*/

int VT_mpeg4_decoder_cleanup()
{
    int rv = TFAIL;
    if(((test_output == VIDEO_OUTPUT)||(test_output == FILES_AND_VIDEO))&&framebuffer)
    {
      memset(fb,0,gVideoBufferSize);            /* to clear frame buffer */
    }
    if(framebuffer) /* if frame buffer is supported then memory for frame buffer had been to free*/
    {
      if(munmap(fb,gVideoBufferSize)<0)
	    {
	       tst_resm(TWARN,"Error: failed to unmap framebuffer device to memory.");
	       return rv;
	    }
    }
    rv = TPASS;
    return rv;
}



/*===== VT_mpeg4_decoder_test =====*/
/**
@brief  Test scenario function

@param  testcase    - Testcase id of the test according to the test plan \n
	      iter        - Iteration of the loop in case of an endurance/stress test
        output      - Output form of output data
        skip_mode   - Mode of skipping frame: to skip to an INTRA Frame or specified number of frames.
        start_frame - Skips beginning from start_frame
        number      - Number of frames to skip
        time        - Displays time of decoding
	input_fname - Name of input bitstream
	delay       - Delay between showed frames
	comm        - Comments with decode process 
  
@return On success - return TPASS
        On failure - return the error code
*/

int VT_mpeg4_decoder_test( int testcase, int iter, int output, 
                           int skip_mode, int start_frame, int number, 
			   int time, char* input_fname,int delay,int comm,char* dir)
{
    int i;
    int rv = TFAIL;
    
    test_testcase   = testcase;
    test_iter       = iter;
    test_output     = output;
    test_disp_time  = time;
   // input_file_name = input_fname;
    test_delay      = delay;
    test_comm       = comm;  
    input_dir = NULL;
    if(dir[strlen(dir)-1]=='/')
    {
      input_dir = malloc(sizeof(UCHAR)*strlen(dir));			
      memset(input_dir,0,sizeof(UCHAR)*strlen(dir));
      strcpy(input_dir,dir);
    }
    else 
    {
      input_dir = malloc(sizeof(UCHAR)*(strlen(dir)+1));
      memset(input_dir,0,sizeof(UCHAR)*(strlen(dir)+1));
      sprintf(input_dir,"%s%c",dir,'/');
    }
    for (i = 0; i < MAX_DECODER_THREADS; i++) /* init decode threads */
    {
       dec_thread[i].instance_id = i;
       memset(dec_thread[i].own_out_file_name,0,sizeof(dec_thread[i].own_out_file_name));
       sprintf(dec_thread[i].own_out_file_name,"mpeg4d_output/%s%s%d",input_fname,output_file_name,i);
    }   
    input_file_name = malloc(sizeof(UCHAR)*(strlen(input_dir)+strlen(input_fname)));
    memset(input_file_name,0,sizeof(UCHAR)*(strlen(input_dir)+strlen(input_fname)));
    sprintf(input_file_name,"%s%s",input_dir,input_fname);

    switch( testcase )
    {
	  case NOMINAL_FUNCTIONALITY:
	    tst_resm(TINFO,"Nominal functionality test" );
    	    tst_resm(TINFO,"Input Bitstream Name : %s",input_fname);
	    rv = nominal_functionality_test();
	    tst_resm(TINFO,"End nominal functionality test" );		
    	    break;
	    
	  case ADVANCED_FUNCTIONALITY:
    	    tst_resm(TINFO,"Advanced functionality test" );
            tst_resm(TINFO,"Input Bitstream Name : %s",input_fname);
            test_skip_mode = skip_mode;
            test_start_frame = start_frame;
            test_number_to_skip = number;
	    rv = advanced_functionality_test();
	    tst_resm(TINFO,"End advanced functionality test" );
    	    break;  
	    
	  case ENDURANCE:
	    tst_resm(TINFO,"Endurance test" );
            tst_resm(TINFO,"Input Bitstream Name : %s",input_fname);      
	    rv = endurance_test();
	    tst_resm(TINFO,"End endurance test" );		
	    break;
	    
	  case RE_ENTRANCE:
	    tst_resm(TINFO,"Re-entrance test" );
            tst_resm(TINFO,"Input Bitstream Name : %s",input_fname);     
	    rv = re_entrance_test();
	    tst_resm(TINFO,"End re-entrance test" );		
	    break;
	    
	  case PRE_EMPTION:
	    tst_resm(TINFO,"Pre-emption test" );
            tst_resm(TINFO,"Input Bitstream Name : %s",input_fname);      
	    rv = pre_emption_test();
	    tst_resm(TINFO,"End pre-emption test" );		
	    break;
	    
	  case LOAD_ENVIROUNMENT:
	    tst_resm(TINFO,"Load envirounment test" );
           tst_resm(TINFO,"Input Bitstream Name : %s",input_fname);    
	    rv = load_envirounment_test();
	    tst_resm(TINFO,"End load envirounment test" );		
	    break;                
	    
	  case ROBUSTNESS:
	    tst_resm(TINFO,"Robustness test" );
            tst_resm(TINFO,"Input Bitstream Name : %s","error.bits");
	    rv = robustness_test();
	    tst_resm(TINFO,"End robustness test" );		
	    break;                   
        
      case RELOCATABILITY:
        tst_resm( TINFO, "Relocatability test" );
        tst_resm( TINFO, "Input Bitstream Name : %s", input_fname );    
        rv = relocatability_test();
        tst_resm( TINFO, "End relocatability test" );
        break;            
	
	  default:
	    tst_resm(TWARN,"Wrong test case" );
	    break;    
    }    
    free(input_file_name);
    free(input_dir); 
    return rv;
}


/*===== cbkMPEG4DBufRead =====*/
/**
@brief  Bitstream reading function

@param  s32EncStrBufLen - Length of bitstream buffer 
        pu8EncStrBuf    - Pointer to bitstream buffer
        offset          - Offset of the starting byte of this portion of bitstream 
                          from the start of the bitstream. 
  	    pvAppContext    - Pointer to void for getting input file  
	
@return On success - Number of read bytes
        On failure - ERROR
*/
int cbkMPEG4DBufRead (int s32EncStrBufLen, UCHAR *pu8EncStrBuf,
                      int offset, void *pvAppContext)
{
    UINT  u32BytesRead;

    Mpeg4_dec_thread_t * pDecThread = (Mpeg4_dec_thread_t*)pvAppContext;
    assert( pDecThread );
    
    if( pDecThread->read_bytes != offset )
    {
        fseek( pDecThread->input_file, offset, SEEK_SET );
        pDecThread->read_bytes = offset;        
    }


    u32BytesRead = fread (pu8EncStrBuf, sizeof(unsigned char), s32EncStrBufLen,
                          pDecThread->input_file);
    pDecThread->read_bytes += u32BytesRead;                          
    if (u32BytesRead == 0)
    {
        if (feof (pDecThread->input_file) == 0)
        {
            return -1;
        }
    }
    return u32BytesRead;
}

/*======================== LOCAL FUNCTIONS ==================================*/


/*===== allocate_memory =====*/
/**
@brief  Allocating memory for decoder function 

@param  psMemInfo - Pointer to decoder memory structure 

@return On success - SUCCESS
        On failure - ERROR
*/

int allocate_memory(sMpeg4DecMemAllocInfo *psMemInfo)
{
    int i;

    for(i=0;i<psMemInfo->s32NumReqs;i++)
    {
      if(MPEG4D_IS_FAST_MEMORY(psMemInfo->asMemBlks[i].s32Type))
      {
        psMemInfo->asMemBlks[i].pvBuffer =  malloc(psMemInfo->asMemBlks[i].s32Size);
      }
      else
      {
        psMemInfo->asMemBlks[i].pvBuffer = malloc(psMemInfo->asMemBlks[i].s32Size);
      }
      if (psMemInfo->asMemBlks[i].pvBuffer == NULL)
      {
          tst_resm(TWARN,"Decoder engine ERROR: unable to allocate memory for Mpeg4 Decoder");
          return ERROR;
      }
    }
    return SUCCESS;
    
}


/*===== free_bitbuffer =====*/
/**
@brief  Freeing bitstream buffer function

@param  ppu8BitBuffer - Pointer to pointer to Bitstream buffer
	pBitBuffLen   - Pointer to Bitstream buffer length 

@return no
        
*/

void free_bitbuffer(UCHAR **ppu8BitBuffer,int* pBitBuffLen)
{
   if (*ppu8BitBuffer != NULL)
   {
     free(*ppu8BitBuffer);
     *ppu8BitBuffer = NULL;
     *pBitBuffLen = 0;
   }  
}


/*===== free_memory_app =====*/
/**
@brief  Freeing memory allocated by application function

@param  psMpeg4DecObject - Pointer to main decode structure 

@return no

*/

void free_memory_app (sMpeg4DecObject* psMpeg4DecObject)
{
    sMpeg4DecMemAllocInfo   *psMemAllocInfo = &(psMpeg4DecObject->sMemInfo);

    /*!
    *   Freeing Memory Allocated by the Application for Decoder
    */
    free_memory_dec(psMemAllocInfo);
    psMemAllocInfo = NULL;

      
    if (psMpeg4DecObject->sDecParam.sOutputBuffer.pu8YBuf != NULL)
    {
        free (psMpeg4DecObject->sDecParam.sOutputBuffer.pu8YBuf);
        psMpeg4DecObject->sDecParam.sOutputBuffer.pu8YBuf = NULL;
    }

    if (psMpeg4DecObject->sDecParam.sOutputBuffer.pu8CbBuf != NULL)
    {
        free (psMpeg4DecObject->sDecParam.sOutputBuffer.pu8CbBuf);
        psMpeg4DecObject->sDecParam.sOutputBuffer.pu8CbBuf = NULL;
    }

    if (psMpeg4DecObject->sDecParam.sOutputBuffer.pu8CrBuf != NULL)
    {
        free (psMpeg4DecObject->sDecParam.sOutputBuffer.pu8CrBuf);
        psMpeg4DecObject->sDecParam.sOutputBuffer.pu8CrBuf = NULL;
    }
    
    if (psMpeg4DecObject->sDecParam.p8MbQuants != NULL )
    {
        free (psMpeg4DecObject->sDecParam.p8MbQuants);
        psMpeg4DecObject->sDecParam.p8MbQuants = NULL;
    }
  
}


/*===== free_memory_dec =====*/
/**
@brief  Freeing memory allocated for decoder function

@param  psMemAllocInfo - Pointer to decode memory structure

@return no

*/

void free_memory_dec (sMpeg4DecMemAllocInfo *psMemAllocInfo)
{
    int s32MemBlkCnt = 0;

    for (s32MemBlkCnt = 0; s32MemBlkCnt < psMemAllocInfo->s32NumReqs;
         s32MemBlkCnt++)
    {
        if (psMemAllocInfo->asMemBlks[s32MemBlkCnt].pvBuffer != NULL)
        {
            free (psMemAllocInfo->asMemBlks[s32MemBlkCnt].pvBuffer);
            psMemAllocInfo->asMemBlks[s32MemBlkCnt].pvBuffer = NULL;
        }
    }

}


/*===== free_decoder_thread =====*/
/**
@brief  Closing files opened by decode thread function

@param  psMemAllocInfo - Pointer to decode memory structure

@return no

*/

void free_decoder_thread (void* ptr)
{
     Mpeg4_dec_thread_t* pDecThread = (Mpeg4_dec_thread_t*)ptr;

     fclose(pDecThread->input_file);     /* close file of input bitstream */ 
}


/*===== decoder_engine =====*/
/**
@brief  Engine of the decoder. The decoding of a bit-stream should be presented here.
	Also this function has processed dec result, i.e. displayed it for a video data case
	or sounded it for a sound data case.
	This method should be compatible with a threads.

@param  ptr - Pointer to void

@return On success - return TPASS
        On failure - return the error code
*/
int decoder_engine( void * ptr )
{
    int ret = TFAIL;
 
    eMpeg4DecRetType error;

    int s32BytesRead = 0;
    UCHAR *pu8BitBuffer = NULL;
    int s32BitBufferLen = 0;
                           
    if( ptr == NULL )
    {
	tst_resm(TWARN,"Decoder engine ERROR: invalid args" );
	return ret;
    }
   
    Mpeg4_dec_thread_t* pDecThread = (Mpeg4_dec_thread_t*)ptr;

    pDecThread->frame = NULL;
    
    /* Open input bitstream */
    if ((pDecThread->input_file = fopen(input_file_name, "rb")) == NULL)
    {
        tst_resm(TWARN,"Decoder engine ERROR: cannot open input file: %s",
                input_file_name);
        return ret;
    }
    
    /* Allocate memory to frame */
    if((pDecThread->frame = (UCHAR*)malloc(gVideoBufferSize))==NULL)
    {
        tst_resm(TWARN,"Decoder engine ERROR: unable to allocate memory for decoder thread buffer");
        fclose(pDecThread->input_file);
        return ret;     
    }

    /* Init decode structures */

    sMpeg4DecObject sMpeg4DecObj;
    sMpeg4DecObj.pvAppContext = (void*)pDecThread;//(void *)(pDecThread->input_file);      
    sMpeg4DecMemAllocInfo *psMemInfo = &(sMpeg4DecObj.sMemInfo);
    sMpeg4DecoderParams *pDecParams = &(sMpeg4DecObj.sDecParam);
    
    sMpeg4DecObj.sVisParam.s32NumberOfVos=1;
    sMpeg4DecObj.sVisParam.as32NumberOfVols[0]=1;
    
    Mpeg4_register_func(&sMpeg4DecObj,cbkMPEG4DBufRead);

    /* Reading input bitstream */

    s32BitBufferLen = BIT_BUFFER_SIZE;
    pu8BitBuffer = (UCHAR *) malloc (sizeof(unsigned char) *
                                             s32BitBufferLen);
    if (pu8BitBuffer == NULL)
    {
        tst_resm(TWARN,"Decoder engine ERROR: unable to allocate memory for BitBuffer\n");
        free_decoder_thread(pDecThread);
        return ret;
    }    
    s32BytesRead = fread (pu8BitBuffer, sizeof(unsigned char), s32BitBufferLen,
                          pDecThread->input_file);
    if (s32BytesRead < s32BitBufferLen)            
    {
       /*! Let Decoder handle the Error */
       tst_resm(TWARN,"Decoder engine ERROR: insufficient bytes in encoded Bitstream file \"%s\""
              "Bytes Read=%d", input_file_name, s32BytesRead);                      
       free_decoder_thread(pDecThread);
       return ret;
    }

    fseek(pDecThread->input_file, 0, SEEK_SET);
    pDecThread->read_bytes = 0;
    
    /* Query size of memory need for decoder */
    
    if(test_testcase == ROBUSTNESS)printf("Start Query Memeory:eMPEG4DQuerymem()...\n");
    
    error = eMPEG4DQuerymem(&sMpeg4DecObj,pu8BitBuffer,s32BitBufferLen);
    
    if(test_testcase == ROBUSTNESS)printf("...Succesfull Query Memeory:eMPEG4DQuerymem()\n");
    
    free_bitbuffer(&pu8BitBuffer,&s32BitBufferLen);

    if (error != E_MPEG4D_SUCCESS) /* if query has not completed successful */
    {
        tst_resm(TWARN,"Decoder engine ERROR: Function eMPEG4DQuerymem() resulted in failure.MPEG4D Error Type : %d", error);
        free_decoder_thread(pDecThread);
        return ret;
    } 
   /*!
    *   Allocating Memory for Output Buffer
    */        
                              
    sMpeg4DecObj.sDecParam.sOutputBuffer.pu8YBuf = (UCHAR *)
                     malloc (sMpeg4DecObj.sDecParam.sOutputBuffer.s32YBufLen *
                     sizeof(unsigned char));
    //tst_resm( TINFO, "memory for Y = %d\n", sMpeg4DecObj.sDecParam.sOutputBuffer.s32YBufLen ); 
    if (sMpeg4DecObj.sDecParam.sOutputBuffer.pu8YBuf == NULL)
    {
        tst_resm(TWARN,"Decoder engine ERROR: unable to allocate memory for Output Buffer");
        free_decoder_thread(pDecThread);
        free_memory_app (&sMpeg4DecObj);
        return ret;
    }

    sMpeg4DecObj.sDecParam.sOutputBuffer.pu8CbBuf = (UCHAR *)
		    malloc(sMpeg4DecObj.sDecParam.sOutputBuffer.s32CbBufLen*sizeof(unsigned char));
    //tst_resm( TINFO, "memory for U = %d\n", sMpeg4DecObj.sDecParam.sOutputBuffer.s32CbBufLen); 
    
    if (sMpeg4DecObj.sDecParam.sOutputBuffer.pu8CbBuf == NULL)
    {
        tst_resm(TWARN,"Decoder engine ERROR: unable to allocate memory for Output Buffer");

        free_decoder_thread(pDecThread);
        free_memory_app (&sMpeg4DecObj);
        
        return ret;
    }

    sMpeg4DecObj.sDecParam.sOutputBuffer.pu8CrBuf = (UCHAR *)
                     malloc (sMpeg4DecObj.sDecParam.sOutputBuffer.s32CrBufLen*sizeof(unsigned char));
//    tst_resm( TINFO, "memory for U = %d\n", sMpeg4DecObj.sDecParam.sOutputBuffer.s32CrBufLen); 
		     
    if (sMpeg4DecObj.sDecParam.sOutputBuffer.pu8CrBuf == NULL)
    {
        tst_resm(TWARN,"Decoder engine ERROR: unable to allocate memory for Output Buffer");

        free_decoder_thread(pDecThread);
        free_memory_app (&sMpeg4DecObj);
        
	return ret;
    }
    /* Allocate memory to hold the quant values, make sure that we round it
     * up in the higher side, as non-multiple of 16 will be extended to 
     * next 16 bits value 
     */
    sMpeg4DecObj.sDecParam.p8MbQuants = (signed char*)
        malloc ((((sMpeg4DecObj.sDecParam.u16FrameWidth  + 15) >> 4 ) << 4)  * 
                (((sMpeg4DecObj.sDecParam.u16FrameHeight + 15) >> 4 ) << 4) *
                sizeof (signed char));
    
    if (sMpeg4DecObj.sDecParam.p8MbQuants == NULL)
    {
        tst_resm(TWARN,"Unable to allocate memory for quant values");

        free_decoder_thread(pDecThread);
        free_memory_app (&sMpeg4DecObj);        
    	return ret;
    }                
                    

    /*!
    *   Allocating Memory for MPEG4 Decoder
    */

    if(allocate_memory(psMemInfo)==ERROR)
    {
      free_decoder_thread(pDecThread);
      free_memory_app(&sMpeg4DecObj);
      return ret;
    }      
    
   /*!
    *   Calling MPEG4 Decoder Init Function
    */
    error = eMPEG4DInit(&sMpeg4DecObj);
    if ((error != E_MPEG4D_SUCCESS) &&
        (error != E_MPEG4D_ENDOF_BITSTREAM))
    {
        tst_resm(TWARN,"Decoder engine ERROR: Decoder initialization failed with return value %d", error);

        free_decoder_thread(pDecThread);
        /*!  Freeing Memory allocated by the Application */
        free_memory_app(&sMpeg4DecObj);
        return ret;
    }
    sMpeg4DecObj.sDecParam.u16DecodingScheme = skip_at_flag;
    /*!
    *   Opening Output file given by the user 
    *   for output as files or outpuat as files and displaying frames together 
    */
   if((test_output == FILES_OUTPUT)||(test_output == FILES_AND_VIDEO))
   {
      KevOpen (pDecThread->own_out_file_name, KEV_WRITE,
               &(pDecThread->sKevFileObj));
      SetDimensions (&(pDecThread->sKevFileObj),
                     pDecParams->u16FrameWidth,
                     pDecParams->u16FrameHeight);
    }  
    //printf("\nFrame Width %d, Frame Height %d\n\n",pDecParams->u16FrameWidth,pDecParams->u16FrameHeight);

    /*!
    *   Calling MPEG4 Decoder Function
    */    
    error = eMPEG4DecoderLoop (&sMpeg4DecObj,pDecThread);
                                
    if((test_output == FILES_OUTPUT)||(test_output == FILES_AND_VIDEO))
      KevClose (&(pDecThread->sKevFileObj));

    if (error == E_MPEG4D_ENDOF_BITSTREAM) /* if return value of MPEG4DecoderLoop() was end of bitstream */
    {
#ifdef DEBUG_TEST
        if(display_info)printf ("\nCompleted decoding the bitstream %s\n",input_file_name);
#endif
    }
    else if (error != E_MPEG4D_SUCCESS) /*if return value of MPEG4DecoderLoop() was error */
    {
        tst_resm(TWARN,"Function eMPEG4DecoderLoop() failed with %d", error);

        free_decoder_thread(pDecThread);
        /*!  Freeing Memory allocated by the Application */
        free_memory_app(&sMpeg4DecObj);
        return ret;
    }

    /*!
    *   Releasing any resours used by the decoder
    */

    error = eMPEG4DFree (&sMpeg4DecObj);
    
    if ((error != E_MPEG4D_SUCCESS) &&
        (error != E_MPEG4D_ENDOF_BITSTREAM))
    {
        tst_resm(TWARN,"Decoder engine ERROR: Function eMPEG4DFree() failed with %d", error);

        free_decoder_thread(pDecThread);
        /*!  Freeing Memory allocated by the Application */
        free_memory_app(&sMpeg4DecObj);
        return ret;
    }
    free_decoder_thread(pDecThread);
    free_memory_app(&sMpeg4DecObj);
                              
    ret = TPASS;
                                                                                                                                            
    return ret;
}



/*===== run_decoder_in_loop =====*/
/**
@brief  This method called by a special decoder thread decode in loop the same bitstreams.

@param  ptr - Pointer to void
  
@return On success - return TPASS
        On failure - return the error code
*/

int run_decoder_in_loop( void * ptr )
{
    int i;
    int retval = 0;
    static int rv = TFAIL;
    
    Mpeg4_dec_thread_t* pDecThread = (Mpeg4_dec_thread_t*)ptr;

    for( i = 0; i < test_iter; ++i ) 
    {
              if((test_testcase==RE_ENTRANCE)||(test_testcase==PRE_EMPTION))
	        tst_resm(TINFO,"Thread no. %d : Itearation no. %d",pDecThread->instance_id,i+1);
        else tst_resm(TINFO,"Itearation no. %d",i+1);
	retval += decoder_engine( ptr );
    }

    /* Set that boolean that is a global variable of the main process */
    /* to inform the second thread that the 1st one has ended. */
    /* It allows the 2nd thread to terminate. */
    if((test_testcase==RE_ENTRANCE)||(test_testcase==PRE_EMPTION))
      tst_resm(TINFO,"Thread no. %d : Test iterations number = %d",pDecThread->instance_id,test_iter);
    else tst_resm(TINFO,"Test iterations number = %d",test_iter); 
    
    thread_synchro = TRUE;
    
    if(!retval) rv = TPASS;
    
    return rv;
}
                         

/*===== run_decoder_on_timer =====*/
/**
@brief  This method is for a thread working on a timer for preemption test.

@param  None
  
@return On success - return TPASS
        On failure - return the error code
*/

int run_decoder_on_timer( void * ptr )
{
    struct sigaction sa;
    struct itimerval timer;

    /* Install the timer handler... */
    memset( &sa, 0, sizeof (sa) );
    sa.sa_handler = &timer_handler;
    sigaction( SIGALRM, &sa, NULL );
	
    /* Configure the timer to expire every 100 msec...  */
    timer.it_value.tv_sec     = 0;	/* First timeout */
    timer.it_value.tv_usec    = 100000; /* 100000 */
    timer.it_interval.tv_sec  = 0;  	/* Interval */
    timer.it_interval.tv_usec = 100000;	/* 100000 */
	
    /* Start timer...  */
    setitimer( ITIMER_REAL, &timer, NULL );

    while( 1 ) sleep( 10 );

    return TPASS;
}

/*===== timer_handler=====*/
/**
@brief This is a timer handler.

@param signum - signal number 
  
@return None
*/

void timer_handler( int signum )
{
    static int pending = FALSE;
    static int retval = 0;

    if( !pending )
    {
        pending = TRUE;
        tst_resm(TINFO,"Thread no. 1 on Timer");
        retval += decoder_engine( &dec_thread[1] );
        pending = FALSE;
    }
    else 
    {
#ifdef DEBUG_TEST      
    	printf( "Skipped, timer too fast!!! \n" );
#endif
    }
    fflush( stdout );

    /* if the loop thread ends, terminate the thread working with timer */
    if( thread_synchro )
	pthread_exit( &retval );

}


/*===== hogcpu=====*/
/**
@brief  Hog the CPU for stress test in a load environment.

@param  None
  
@return None
*/

void hogcpu()
{
    while( 1 )
    {
      sqrt( rand () );
    }
}



/*===== nominal_functionality_test =====*/
/**
@brief  Testing of a nominal functionality of a encoder/decoder.

@param  None.
  
@return On success - return TPASS
        On failure - return the error code
*/

int nominal_functionality_test()
{
    return decoder_engine(&dec_thread[0]);
}


/*===== advanced_functionality_test =====*/
/**
@brief  Testing of a advanced functionality of a decoder.

@param  None.

@return On success - return TPASS
        On failure - return the error code
*/

int advanced_functionality_test()
{
    return decoder_engine(&dec_thread[0]);
}


/*===== endurance_test =====*/
/**
@brief  Test of ability to work long time without crashes.

@param  None.
  
@return On success - return TPASS
        On failure - return the error code
*/

int endurance_test()
{
   int rt = TFAIL;
   display_info = 0;
   if(run_decoder_in_loop(&dec_thread[0])==0)
   {
     rt = TPASS;
     return rt;
   }
   return rt;
}


/*===== re_entrance_test =====*/
/**
@brief  Re-entrance means there should not be any static data or any global 
	variables used in the code. Test this ability.

@param  None.
  
@return On success - return TPASS
        On failure - return the error code
*/

int re_entrance_test()
{
    display_info = 0;
    if( pthread_create( &dec_thread[0].tid, NULL, (void *)&run_decoder_in_loop, (void *)&dec_thread[0] ) )
    {
        tst_resm(TWARN,"re_entrance_test : error creating thread 0 " );
        return TFAIL;
    }
    if( pthread_create( &dec_thread[1].tid, NULL, (void *)&run_decoder_in_loop, (void *)&dec_thread[1] ) )
    {
        tst_resm(TWARN,"re_entrance_test : error creating thread 1 ");
        return TFAIL;
    }
    
    /* Wait till threads are complete before main continues. Unless we  */
    /* wait we run the risk of executing an exit which will terminate   */
    /* the process and all threads before the threads have completed.   */
#ifdef DEBUG_TEST
    printf( "wait for 1st thread to end\n" );
#endif
    pthread_join( dec_thread[0].tid, NULL );
#ifdef DEBUG_TEST
    printf( "wait for 2nd thread to end\n"  );
#endif
    pthread_join( dec_thread[1].tid, NULL ); 
    
    return TPASS;
}



/*===== pre_emption_test =====*/
/**
@brief  The decoder should function correctly in a pre-emptive environment. Test this ability.

@param  None.
  
@return On success - return TPASS
        On failure - return the error code
*/

int pre_emption_test()
{
    display_info = 0;
    if( pthread_create( &dec_thread[0].tid, NULL, (void *)&run_decoder_in_loop, (void *)&dec_thread[0] ) )
    {
	tst_resm(TWARN,"pre_emption_test : error creating thread 0" );
	return TFAIL;
    }
    if( pthread_create( &dec_thread[1].tid, NULL, (void *)&run_decoder_on_timer, (void *)&dec_thread[1] ) )
    {
	tst_resm(TWARN,"pre_emption_test : error creating thread 1" );
	return TFAIL;
    }
			
    /* Wait till threads are 
    complete before main continues. Unless we  */
    /* wait we run the risk of executing an exit which will terminate   */
    /* the process and all threads before the threads have completed.   */
    /* The first thread is expected to die the first one. */
#ifdef DEBUG_TEST
    printf( "wait for 1st thread to end\n " );
#endif
    pthread_join( dec_thread[0].tid, NULL );

    /* The end of the first thread is triggering the end of the second */
#ifdef DEBUG_TEST
    printf( "wait for 2nd thread to end\n " );
#endif
    pthread_join( dec_thread[1].tid, NULL ); 

    return TPASS;
}


/*===== load_envirounment_test =====*/
/**
@brief  Test of ability to work in a loaded (even oberloaded) envirounment. [optional]

@param  None.
  
@return On success - return TPASS
        On failure - return the error code
*/

int load_envirounment_test()
{
    int rv = TFAIL;
    int retval;
    pid_t pid;
    display_info = 0;
    switch( pid = fork() )
    {
	   case -1:
	    tst_resm(TWARN,"load_envirounment_test : fork failed" );
	    return rv;
	   case 0:                         /* child -- finish straight away */
	    tst_resm(TINFO,"Child process that load the CPU" );
	    hogcpu();
	   default:                        /* parent */
	    tst_resm(TINFO,"Parent process that run decoder in loop");
	    retval = run_decoder_in_loop( &dec_thread[0] );

	    /* kill child process once decode loop has ended */
	    if( kill( pid, SIGKILL ) != 0 )
	    {
	        tst_resm(TWARN,"load_envirounment_test : Kill(SIGKILL) error" );
	        return rv;
	    }
	    if(!retval) rv = TPASS;
    }	

    return rv;
}

 
/*===== robustness_test  =====*/
/**
@brief  Test of ability adequately react to a bad input bitstream.

@param  None.
  
@return On success - return TPASS
        On failure - return the error code
*/

int robustness_test()
{
    int rv = TPASS;
    int retval = 0;
    char* old_in_file = malloc(sizeof(UCHAR)*strlen(input_file_name));
    strcpy(old_in_file,input_file_name);
     
    memset(input_file_name,0,sizeof(UCHAR)*strlen(input_file_name));
    sprintf(input_file_name,"%s%s",input_dir,"error.bits");
  
    retval = decoder_engine(&dec_thread[0]);
 
    tst_resm(TINFO,"Decoder error input file: %d", retval);
    
    memset(input_file_name,0,sizeof(UCHAR)*strlen(input_file_name));
    strcpy(input_file_name,old_in_file);
    free(old_in_file);
 
    return rv;
}

int relocatability_test()
{
    int rv = TPASS;
    int i;
    for( i = 0; i < test_iter; ++i )
    {
        rv += decoder_engine(&dec_thread[0]);
        tst_resm( TINFO, "Data memory was relocated" );
    }
    return rv;
}

/*===== eMPEG4DecoderLoop =====*/
/**
@brief  Decoding in Loop

@param  psMpeg4DecObject - Pointer to main decode structure

@return On success - E_MPEG4D_SUCCESS
        On failure - error code
*/

eMpeg4DecRetType eMPEG4DecoderLoop (sMpeg4DecObject* psMpeg4DecObject,
                                    void*  ptr)
{

     Int       s32FrameNum = 0;           /*!< Decoded Frame Num */
     Int       s32SrcFrameNum = 0;        /*!< FrameNum w.r.t src frame rate */
     Int       s32MilliSecsPerFrame = 0;
     Int       s32Fps30FrameNum = 0;      /*!< FrameNum w.r.t 30 fps */
     UINT      u32NumSkippedFrames = 0;   /*!< Number of skipped frames */
     UINT      u32SkipCount = 0;          /* Summary of skipped frames */
     UINT      u32DecodeCount = 0;        /* Summary of decoded frames */
     double    DecProcTimeInSec = 0;      /* Decoding processor time */
     double    DecTimeInSec = 0;          /* Decoding time */

     eMpeg4DecRetType   eDecRetVal = E_MPEG4D_FAILURE;
     
     Mpeg4_dec_thread_t* pDecThread = (Mpeg4_dec_thread_t*)ptr;
#ifdef DEBUG_TEST     
     if(test_testcase == ADVANCED_FUNCTIONALITY)/* if mode with skipping frames */
     {

       printf("*****Conditions of skipping*****\n\n");

       if(test_skip_mode == SKIP_TO_INTRA)
       {
         printf("The skipping mode: to INTRA Frame.\n");
         printf("To skip frames from frame no. %d.\n", test_start_frame);
       }
       else
       {
         printf("The skipping mode: specified number of frames.\n");
         printf("To skip frames from frame no. %d.\n", test_start_frame);
         printf("Number of frames to skip: %d.\n",test_number_to_skip);
       }
       printf("\n*******************************\n");
     }
#endif     
     //if((test_testcase == RE_ENTRANCE)||(test_testcase == PRE_EMPTION))
     //     printf("\nStart decoding thread number %d...\n\n",pDecThread->instance_id);
     //else printf("\nStart decoding...\n\n");
     if(display_info)printf("\nStart decoding...\n\n");
     
     while (1) /* Decode Loop */
     {
          if(test_testcase == ADVANCED_FUNCTIONALITY)
          {                             
             if(s32FrameNum+1==test_start_frame) /* begining to skip */
             {
#ifdef DEBUG_TEST
              printf ("Skipping frames after decoding frame no. %d ... ", s32FrameNum);
#endif                           
              if(test_skip_mode == SKIP_TO_INTRA) /* if mode is to skip to INTRA */
              {
                 eDecRetVal = SKIP_2NEXT_IFRAME (psMpeg4DecObject,
                                                          &u32NumSkippedFrames);

                 u32SkipCount+=u32NumSkippedFrames;
                 s32FrameNum += u32NumSkippedFrames;   
                 if((eDecRetVal != E_MPEG4D_SUCCESS) && (eDecRetVal != E_MPEG4D_ENDOF_BITSTREAM) && (eDecRetVal != E_MPEG4D_SKIPPED_END_OF_SEQUENCE_CODE))
                 {
                    tst_resm(TWARN,"ERROR: Function eMPEG4DSkip2NextIFrame() resulted in failure: eDecRetVal = %d", eDecRetVal);
                    break;
                 }
                 else
                 if(eDecRetVal == E_MPEG4D_ENDOF_BITSTREAM)
                 {
#ifdef DEBUG_TEST
                   printf ("End of Bitstream!\n\n");
#endif
                   break;
                 }
#ifdef DEBUG_TEST
                 printf ("End of the skipping\n\n");
#endif
              }
              else /* if mode is to skip specified number of frames */
              {
                 eDecRetVal = SKIP_FRAMES (psMpeg4DecObject,
                                                 test_number_to_skip,
                                                 &u32NumSkippedFrames);
                 u32SkipCount+=u32NumSkippedFrames;
                 s32FrameNum += u32NumSkippedFrames;   
                 if((eDecRetVal != E_MPEG4D_SUCCESS) && (eDecRetVal != E_MPEG4D_ENDOF_BITSTREAM) && (eDecRetVal != E_MPEG4D_SKIPPED_END_OF_SEQUENCE_CODE))
                 {
                    tst_resm(TWARN,"ERROR: Function eMPEG4DSkipFrames resulted in failure: eDecRetVal = %d\n\n", eDecRetVal);
                    break;
                 }
                 else
                 if(eDecRetVal == E_MPEG4D_ENDOF_BITSTREAM)
                 {
#ifdef DEBUG_TEST
                   printf ("End of Bitstream!\n\n");
#endif
                   break;
                 }
#ifdef DEBUG_TEST
                 printf ("End of the skipping\n\n");
#endif    
              }
           
    	     } /* end skipping */

          }
	  
          /* if with comments mode */
          if(test_comm)
          {
	     printf ("Decoding frame number %d ...\r", s32FrameNum+1);
             fflush(stdout);
	  }
	  struct timeval tv;			 /* structure for getting current time*/
	  long start, finish;                    /* begining and ending time of decoding call*/
	  double duration_proc, duration;        /* processor time and time of decoding each frame*/
	  clock_t start_proc, finish_proc;       /* begining and ending processor time of decoding call*/
	  
	  if(test_disp_time)  /* if there was setted option real-time exucation test*/
          {
            gettimeofday(&tv,NULL);
            start = tv.tv_sec * 1000000 + tv.tv_usec;   /* time as microseconds */
            start_proc = clock();                       /* time as seconds */
          }
    
          /* Decodes the Mpeg4 bitstream and writes bitstream to output buffer */
          eDecRetVal = eMPEG4DDecode (psMpeg4DecObject);
          
          if(test_disp_time)  /* if there was setted option real-time exucation test */
          {
            finish_proc = clock();
            gettimeofday(&tv,NULL);
            finish = tv.tv_sec * 1000000 + tv.tv_usec;

            duration_proc = (double)(finish_proc-start_proc)/CLOCKS_PER_SEC;
            duration = (double)(finish - start)/1000000;

            DecTimeInSec += duration;
            DecProcTimeInSec += duration_proc;
          }
    
        
	  s32SrcFrameNum = psMpeg4DecObject->sDecParam.sTime.s32Seconds *
                          psMpeg4DecObject->sDecParam.u16TicksPerSec;


          s32MilliSecsPerFrame = 1000 / psMpeg4DecObject->sDecParam.u16TicksPerSec;
         
         
          /* Adjust for rounding problem */
          s32SrcFrameNum += (psMpeg4DecObject->sDecParam.sTime.s32MilliSeconds *
                            psMpeg4DecObject->sDecParam.u16TicksPerSec +
                            s32MilliSecsPerFrame / 2)/1000;
                            
 
          s32Fps30FrameNum = ((30 * s32SrcFrameNum)/
                             psMpeg4DecObject->sDecParam.u16TicksPerSec);

                    
          if (eDecRetVal == E_MPEG4D_SUCCESS) /* if decode function returned success */
          {
               ++u32DecodeCount;
               s32FrameNum++;
               if((test_output == FILES_OUTPUT)||(test_output == FILES_AND_VIDEO))
               {
                 if(test_comm)printf ("Writting to files ... \n");
                 WriteKevFrame (&(pDecThread->sKevFileObj), s32Fps30FrameNum,
                            psMpeg4DecObject->sDecParam.sOutputBuffer.pu8YBuf,
                            psMpeg4DecObject->sDecParam.u16FrameWidth,
                            psMpeg4DecObject->sDecParam.sOutputBuffer.pu8CbBuf,
                            psMpeg4DecObject->sDecParam.sOutputBuffer.pu8CrBuf,
                            psMpeg4DecObject->sDecParam.u16FrameWidth >> 1);
            
               }
               
               if(((test_output == VIDEO_OUTPUT)||(test_output == FILES_AND_VIDEO))&&framebuffer)
               {
                  if(test_comm)printf ("Writting frame for Video Buffer ... \n");
         
                  pDecThread->iBeetwinFramesTime = ((psMpeg4DecObject->sDecParam.sTime.s32Seconds)*1000000.0+(psMpeg4DecObject->sDecParam.sTime.s32MilliSeconds)*1000.0)/u32DecodeCount;
		  
	          if(!draw_frame(pDecThread,&(psMpeg4DecObject->sDecParam)))
                  {
                    if(test_delay)usleep((int)pDecThread->iBeetwinFramesTime);
		  }
                  else tst_resm(TWARN,"ERROR : Frame number %d has been not written into Frame Buffer!\n",s32FrameNum);
	       }
		 
	     //  if(test_comm)
		   // printf ("... OK!\n"); 
               
	       if(test_disp_time)/* if there was setted option real-time exucation test */
	       {
                   tst_resm(TINFO,"Processor time of decoding of the frame no. %d: %.6fc",s32FrameNum,duration_proc);
	    	   tst_resm(TINFO,"Time of decoding of the frame no. %d: %.6fc",s32FrameNum,duration);
	       }  
          }
          else /* if decode function returned error */
          {
               if(eDecRetVal != E_MPEG4D_ENDOF_BITSTREAM)
                tst_resm(TWARN,"Decoder Loop: eDecRetVal = %d\n", eDecRetVal);
               break;
          }
                                  
     } /* End of Decode while(1) Loop */

     if(display_info)
     {
       tst_resm(TINFO,"Total Frame Count = %d", s32FrameNum);
       tst_resm(TINFO,"No. of Frames Decoded = %d", u32DecodeCount);
     }
     if(test_testcase==ADVANCED_FUNCTIONALITY)
       tst_resm(TINFO,"No. of Frames Skipped = %d", u32SkipCount);
     if(test_disp_time)/* if there was setted option real-time exucation test */
     {
       tst_resm(TINFO,"Proccesor time of the decoding (Sec) of bitstream %s = %f",
                                                   input_file_name, 
                                                   DecProcTimeInSec);
       tst_resm(TINFO,"Time of the decoding (Sec) of bitstream %s = %f", 
                                                   input_file_name, 
                  				   DecTimeInSec);
     }
     return eDecRetVal;
}

/*===== configure_screen =====*/
/**
@brief  Configure frame buffer

@param  None

@return On success - TPASS
        On failure - TFAIL
*/

int configure_screen()
{
    int rv = TFAIL;
  
    /* Open frame buffer */
  
    if ((fd_fb = open( "/dev/fb0", O_RDWR )) < 0)
    {
	tst_resm(TWARN,"ERROR: Unable to open frame buffer\n");
        return rv;
    }
    
    /* Get info about frame buffer configure */

    if (ioctl(fd_fb, FBIOGET_VSCREENINFO, &gScreenInfo))
    {
	tst_resm(TWARN,"ERROR: Unable to read FB information\n");
        return rv;
    } 
    else
    {
#ifdef DEBUG_TEST 
	printf("\nFrame buffer mapping succeed\n");
        printf("\t Width of framebuffer: %d\n", gScreenInfo.xres);
        printf("\t Height of framebuffer: %d\n", gScreenInfo.yres);
        printf("\tBit per pixel = %d\n",gScreenInfo.bits_per_pixel);
#endif
    }
    gVideoBufferSize = gScreenInfo.xres * gScreenInfo.yres * gScreenInfo.bits_per_pixel / 8;
#ifdef DEBUG_TEST
    printf("Video buffer size: %d\n",gVideoBufferSize);
#endif
    if( gScreenInfo.bits_per_pixel == 24 )
    {
        BitPerPix = 24;
        draw_frame = draw_frame24;
    }
    else if( gScreenInfo.bits_per_pixel == 16 )
        {
           BitPerPix = 16;    
           draw_frame = draw_frame16;
        }
        else
        {
           tst_resm(TWARN,"ERORR: %d bpp display mode is not supported yet", gScreenInfo.bits_per_pixel / 8 );
        }  

    if ((int)((fb = (UCHAR *)mmap(0, gVideoBufferSize, PROT_READ | PROT_WRITE, MAP_SHARED, fd_fb, 0)) )<0)
    {
	tst_resm(TWARN,"ERROR: failed to map framebuffer device to memory.");
        return rv;
    }

    if (close(fd_fb) == -1)
    {
        tst_resm(TWARN,"ERROR: cannot close /dev/fb0 device ");
        return rv;
    }
    else
    {
	rv = TPASS;
    }    
  
    memset(fb,0,gVideoBufferSize);
    rv = TPASS;

    return rv;	
}


/*===== draw_frame24 =====*/
/**
@brief  Converting from YCbCr to RGB with frame buffer of depth 24 bit per pixel

@param  ptr - Pointer to void
        pDecParams - Pointer to decode parametrs structure

@return On success - SUCCESS
        On failure - ERROR
*/

int draw_frame24(void* ptr, sMpeg4DecoderParams *pDecParams)
{
  int xp,yp;

  /*int step = 0;*/
                      
  UCHAR r;
  UCHAR g;
  UCHAR b;
  UCHAR * ptr_y = pDecParams->sOutputBuffer.pu8YBuf;
  UCHAR * ptr_Cr_start = pDecParams->sOutputBuffer.pu8CrBuf;
  UCHAR * ptr_Cb_start = pDecParams->sOutputBuffer.pu8CbBuf;

  Mpeg4_dec_thread_t* pDecThread = (Mpeg4_dec_thread_t*)ptr;
  
  /* Allocate memory for frame */

  pDecThread->frame = malloc( (pDecParams->u16FrameHeight) * (pDecParams->u16FrameWidth) * (BitPerPix / 8 ));

  /* Get width and height of frame frome decoder */    

  pDecThread->FWidth = pDecParams->u16FrameWidth;
  pDecThread->FHeight = pDecParams->u16FrameHeight;
  
  /* Converting from YCbCr to RGB */

  if(pDecThread->frame)
  {
     for(yp = 0; yp < pDecParams->u16FrameHeight; yp++)
     {
      for(xp = 0; xp < pDecParams->u16FrameWidth; xp++)
      {
    	 int offset = pDecParams->u16FrameWidth * yp + xp; 
   	 int offset_bpp = offset * BitPerPix >> 3;
         int offset_CrCb = /*step + */(xp >> 1) + ( (pDecParams->u16FrameWidth >> 1) * ( yp>>1 ) );
              
  	 if( offset_bpp < gVideoBufferSize )
  	 {
  	   UCHAR * ptr_frame = pDecThread->frame + offset_bpp;
           UCHAR * ptr_Cr = ptr_Cr_start + offset_CrCb;
           UCHAR * ptr_Cb = ptr_Cb_start + offset_CrCb;

           double rr = (*ptr_y) + 1.371*(( double )(*ptr_Cr) - 128);
           double gg = (*ptr_y) - 0.698*(( double )(*ptr_Cr) - 128) - 0.336*((double)(*ptr_Cb) - 128);
           double bb = (*ptr_y) + 1.732*(( double) (*ptr_Cb) - 128);
           if(rr<0) rr = 0; if(rr>255) rr=255;
           if(gg<0) gg = 0; if(gg>255) gg=255;
           if(bb<0) bb = 0; if(bb>255) bb=255;
           r=rr;g=gg;b=bb;
/*
           r = (*ptr_y) + 1.402*((*ptr_Cr) - 128);
           g = (*ptr_y) - 0.34414*((*ptr_Cb) - 128) - 0.71414*((*ptr_Cr) - 128);
           b = (*ptr_y) + 1.772*((*ptr_Cb) - 128);        
*/
           ptr_y++;
           *ptr_frame++ = b;
	   *ptr_frame++ = g;
	   *ptr_frame++ = r;
	}
       }
      /*step+=2;*/
     }
  
   }
   else              
   {
     tst_resm(TWARN,"ERROR : Not allocated  memory for frame");
     return ERROR;
   }
   
   /* Show frame on LCD*/
   
   show_frame(pDecThread);
   
   /* Free memory for frame */
   
   free(pDecThread->frame);
   pDecThread->frame = NULL;

   return SUCCESS;
}


/*===== draw_frame16 =====*/
/**
@brief  Converting from YCbCr to RGB with frame buffer of depth 16 bit per pixel

@param  ptr - Pointer to void
        pDecParams - Pointer to decode parametrs structure

@return On success - SUCCESS
        On failure - ERROR

*/

int draw_frame16(void* ptr, sMpeg4DecoderParams *pDecParams)
{
  int xp,yp;

  /*int step = 0;*/

  UCHAR r;
  UCHAR g;
  UCHAR b;
  UCHAR * ptr_y = pDecParams->sOutputBuffer.pu8YBuf;
  UCHAR * ptr_Cr_start = pDecParams->sOutputBuffer.pu8CrBuf;
  UCHAR * ptr_Cb_start = pDecParams->sOutputBuffer.pu8CbBuf;

  Mpeg4_dec_thread_t* pDecThread = (Mpeg4_dec_thread_t*)ptr;
  pDecThread->frame = malloc((pDecParams->u16FrameHeight)*(pDecParams->u16FrameWidth)*(BitPerPix/8));

  pDecThread->FWidth = pDecParams->u16FrameWidth;
  pDecThread->FHeight = pDecParams->u16FrameHeight;

  if(pDecThread->frame)
  {
     for(yp = 0; yp < pDecParams->u16FrameHeight; yp++)
     {
      for(xp = 0; xp < pDecParams->u16FrameWidth; xp++)
      {
       int offset = pDecParams->u16FrameWidth * yp + xp;
       int offset_bpp = offset * BitPerPix/8;
       int offset_CrCb = /*step+*/ xp / 2 + ( pDecParams->u16FrameWidth / 2 * yp / 2);

       if( offset_bpp < gVideoBufferSize )
       {
        U16 * ptr_frame = (U16*)(pDecThread->frame + offset_bpp);
        UCHAR * ptr_Cr = ptr_Cr_start + offset_CrCb;
        UCHAR * ptr_Cb = ptr_Cb_start + offset_CrCb;
        
        double rr = (double)(*ptr_y) + 1.371*((double)(*ptr_Cr) - 128);
        double gg = (double)(*ptr_y) - 0.698*((double)(*ptr_Cr) - 128) - 0.336*((double)(*ptr_Cb) - 128);
        double bb = (double)(*ptr_y) + 1.732*((double)(*ptr_Cb) - 128);
        if(rr<0) rr = 0; if(rr>255) rr=255;
        if(gg<0) gg = 0; if(gg>255) gg=255;
        if(bb<0) bb = 0; if(bb>255) bb=255;
        r=rr;g=gg;b=bb;

        ptr_y++;

        *ptr_frame   = (r/8 << 11);
	*ptr_frame   |= (g/8 << 6 );
	*ptr_frame++ |= (b/8);
       }
      }
      /*step+=2;*/

     }
   }
   else
   {
     tst_resm(TWARN,"ERROR : Not allocated  memory for frame\n");
     return ERROR;
   }

   show_frame(pDecThread);

   free(pDecThread->frame);
   pDecThread->frame = NULL;

   return SUCCESS;
}


/*===== show_frame =====*/
/**
@brief  Writting frame into frame buffer for showing on LCD

@param  ptr - Pointer to void

@return None

*/

void show_frame(void* ptr)
{
  int iLine;

  Mpeg4_dec_thread_t* pDecThread = (Mpeg4_dec_thread_t*)ptr;

  int start_fb = (pDecThread->instance_id)*(gVideoBufferSize/MAX_DECODER_THREADS);
  //int frame_size = pDecThread->FWidth * pDecThread->FHeight * ( BitPerPix / 8 );
  int offset_line = pDecThread->FWidth * ( BitPerPix / 8 );
  int offset_fb = gScreenInfo.xres * ( BitPerPix / 8 );
  /*memset(fb + start_fb,0,gVideoBufferSize);*/
  UCHAR* pFrame = pDecThread->frame;
  
  /* Copy frame to frame buffer line by line */
  
  for( iLine = 0; iLine<pDecThread->FHeight; iLine++ )
  {
      UCHAR* pLine = pFrame + offset_line * iLine;
      UCHAR* pFb = fb + start_fb + offset_fb * iLine;
      memcpy( pFb, pLine, offset_line );
  }

}
                                             
#ifdef __cplusplus
}
#endif
