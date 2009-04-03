/*================================================================================================*/
/**
    @file   opl_mirror_test.c

    @brief  OPL image processing library mirroring functions tests source file 
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
Smirnov, Kazachkov          9/9/2004     TLSbo41679   BRIEF description of changes made 
Francois GAFFIE		15/9/2004        TLSbo42679   Added 16bpp display support


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
#include <stdlib.h>
#include <time.h>
    
/* Harness Specific Include Files. */
#include "test.h"

/* Verification Test Environment Include Files */
#include "opl_mirror_test.h"

/*==================================================================================================
                                        LOCAL MACROS
==================================================================================================*/


#define WIDTH           240	/* pic width in pixels    */
#define HEIGHT          160	/* pic height in pixels   */

extern int x_size;
extern int y_size;
extern int verbose_mode;


#define RGB_MAX_PIC_NUM 2	/* all pic stuff is hard coded */

#ifdef GRS_INCLUDED
 #define GRS_MAX_PIC_NUM  2
#endif 


#define DTKMaskRed	0xf800	/* R, G, B color masks     */
#define DTKMaskGreen	0xf7e0
#define DTKMaskBlue	0x001f

#define DTKShiftRed	11	/* shift for RGB565    */
#define DTKShiftGreen	5
#define DTKShiftBlue	0

#define DTKSizeRed	5	/* color range in bits */
#define DTKSizeGreen	6
#define DTKSizeBlue	5


	
typedef OPLError (*PFunc)( const U8	*pSrcBuf, const OPLRect	srcRect, const U32 srcLinestride, U8 *pDstBuf );

PFunc   r_func = oplIPMirrorHoriz_U16;
PFunc   R_func = oplIPMirrorVert_U16;

/*==================================================================================================
                          LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
==================================================================================================*/

typedef enum pic_type_tag
{
    GRS,
    RGB565,
    RGB888,  /* not used */
    RGBA8888 /* not used */
    
} pic_type;
		    
		/* structure that groups all variables needed for working with framebuffer */
typedef struct
{
    int    file_desc;				/* frame buffer file descriptor FGA for display purpose */
    struct fb_var_screeninfo screen_info;	/* screen parameters */	
    
    U32 size;				/* size of the videobuffer in bytes */
    U8* ptr;					/* pointer to frame buffer FGA for display purpose */    
      
} framebuffer_t;


U8* src = 0;		/* pointer to original pic    */
U8* dst = 0;		/* pointer to transformed pic */

/*==================================================================================================
                                       LOCAL CONSTANTS
==================================================================================================*/


/*==================================================================================================
                                       LOCAL VARIABLES
==================================================================================================*/

static framebuffer_t framebuffer;	/* framebuffer struct		*/ 


static int test_rv;

/*==================================================================================================
                                   GLOBAL CONSTANTS
==================================================================================================*/


/*==================================================================================================
                                       GLOBAL VARIABLES
==================================================================================================*/

extern int color_depth;
int bytes_per_pixel = 1;
pic_type type;

/*==================================================================================================
                                   LOCAL FUNCTION PROTOTYPES
==================================================================================================*/


/* init the FB and clear screen */
int configure_screen(); 

/* draw different types of pics */
int draw_pic(U8* src, pic_type type, OPLRect rect);

/* func for thread execution 	*/
void* thread_test_func(void* ptr);

int test_mirror_horiz(const int, const int);
int test_mirror_vert (const int, const int);
int test_mirror(const int srcBuffW, const int srcBuffH, int op);

/* load CPU */
int hogcpu(void);

/* read file contents to memory */
int load_file(char* path_file, U8* pic_data);

/* write pic data to BMP file 	*/
int  GenerateBitmap(U8*, pic_type, int, int, char*);

void print_err_type(OPLError err);
/*==================================================================================================
                                       LOCAL FUNCTIONS
==================================================================================================*/


void print_err_type(OPLError err)
	
{
    switch(err)
	{
	    case OPLErrorNullPtr:    
            fprintf(stderr, "\tNull pointer\n");	
            break;
		case OPLErrorBadArg:     
            fprintf(stderr, "\tBad arguments\n");	break;
		case OPLErrorDivByZero:  
            fprintf(stderr, "\tDiv by zero\n");	
            break;
		case OPLErrorOverFlow:   
            fprintf(stderr, "\tOverflow\n"); 	
            break;
		case OPLErrorUnderFlow:  
            fprintf(stderr, "\tUnderflow\n"); 	
            break;
		case OPLErrorMisaligned: 
            fprintf(stderr, "\tMisaligned\n");     
            break;
		case OPLErrorSuccess:    
            fprintf(stderr, "\tNo Error\n");     
            break;
        default: 
            fprintf(stderr, "\tUnknown Error\n");     
	} 
}

/*================================================================================================*/
/*===== VT_opl_rotate_setup =====*/
/**
@brief  assumes the pre-condition of the test case execution

@param  int color_depth  -  8, 16, 32
  
@return On success - return TPASS
        On failure - return the error code
*/
/*================================================================================================*/
int VT_opl_mirror_setup()
{
    int rv = TFAIL;

    bytes_per_pixel = color_depth/8;

    switch (bytes_per_pixel)
    {

    case 1:
        type = GRS;
        r_func = oplIPMirrorHoriz_U8; 
        R_func = oplIPMirrorVert_U8;
        printf("\n set 8 ");
        break;
    case 2:
        type = RGB565;
        r_func = oplIPMirrorHoriz_U16; 
        R_func = oplIPMirrorVert_U16;
        break;
/*  not implemented in the library
    case 3:
        type = RGB888;
        r_func = oplIPRotate_CW_U24; 
        R_func = oplIPRotate_CCW_U24;
        break;

    case 4:
        type = RGBA8888;
        r_func = oplIPRotate_CW_U32; 
        R_func = oplIPRotate_CCW_U32;
        break;
*/
    default:
        fprintf(stderr, "\n\tError: unsupported resolution\n");
        return rv;
    }
	srand( (unsigned)time( NULL ) );
	src = (U8*)malloc(WIDTH*HEIGHT*bytes_per_pixel);
	dst = (U8*)malloc(WIDTH*HEIGHT*bytes_per_pixel);    
    
    if(!src || !dst)
    {
	    fprintf(stderr, "\n\tError: memory allocation for screen buffer failed\n");
	} 
    else
    {
         if (load_file("p1_240x160.rgb", src) == -1) 
	    {
	        fprintf(stderr, "\n\tError: unable to load file\n");
	    }
	    else
        {
            rv = TPASS;
        }
    }
    
    return rv;
}


/*================================================================================================*/
/*===== VT_opl_rotate_cleanup =====*/
/**
@brief  assumes the post-condition of the test case execution

@param  None
  
@return On success - return TPASS
        On failure - return the error code
*/
/*================================================================================================*/
int VT_opl_mirror_cleanup(void)
{
    int rv = TFAIL;
    
    /** insert your code here */
    if(src)
        free(src);
    if(dst)
        free(dst);

    
    memset(framebuffer.ptr, 0x0, framebuffer.size);   /* blacken the screen */
    
    rv = TPASS;
    
    return rv;
}


/*================================================================================================*/
/*===== VT_opl_rotate_test =====*/
/**
@brief  Template test scenario X function

@param  None
  
@return On success - return TPASS
        On failure - return the error code
*/
/*================================================================================================*/
int VT_opl_mirror_test(int Id, int Case, int Iter, int Write_flag, int r_flag, int R_flag)
{
    test_rv = TFAIL;	/* by default test fails	*/
    int i, j;
    int grs_rv = 0;        
    pid_t    pid;	/* for forked functions       */
    int err_flag   = 0;
    int err_count  = 0;	
    pthread_t* threads  = NULL;
    U8* ref_src    = NULL;
    OPLRect  rect    = {0, 0, WIDTH, HEIGHT}; /* if it's other way it's redetermined later */
    OPLError err;
    
    int linestride = WIDTH*bytes_per_pixel; /* scanline size ib bytes */

    if (configure_screen() != 0)
    {
	    fprintf(stderr, "\n\tError: configure screen failed\n");
	    return test_rv;	
    }

    switch (Id)
    {
    
     case ENDURANCE:	/* verify that mirroring algorithms work properly in general */

	printf("Original picture...\n\n");
	    
	draw_pic(src, type, rect);
	
	if (Write_flag)
	    GenerateBitmap(src, type, HEIGHT, WIDTH, "orig_pic_m.bmp");
	        
	if(r_flag)
        {
            /* mirroring horizontal */

	        if ((err=r_func(src, rect, linestride, dst)) != OPLErrorSuccess)
	        {
    	    	    fprintf(stderr, "\n\tError: mirroring horizontal failed\n");
   		    print_err_type(err);
	        }							
       
		printf("Mirroring horizontal...\n\n");
	        draw_pic(dst, type, rect);
	
		if (Write_flag)	
		    GenerateBitmap(dst, type, HEIGHT, WIDTH, "mirr_horiz.bmp");
        }

        if(R_flag)
        {
            /* mirroring vertical */

            if ((err = R_func(src, rect, linestride, dst)) != OPLErrorSuccess)
	        {
		        fprintf(stderr, "\n\tError: mirroring vertical failed\n");
		        print_err_type(err);
	        }
	        
	        printf("Mirroring vertical...\n\n");
        	draw_pic(dst, type, rect);
	    
		if (Write_flag)
		    GenerateBitmap(dst, type, HEIGHT, WIDTH, "mirr_vert.bmp");						 
        }

        printf("Done!\n\nIf you have seen the pic properly mirrored press \'y\'  otherwise press any key...\n\n");
	    
	    while (1)
	    {
		    if (toupper(getc(stdin)) == 'Y')
		    {
		        ((grs_rv == 0) && (test_rv = TPASS));
		        break;
		    }
		    else
		    {
		        printf("Color pictures have not been properly  displayed\n");
		        break;
		    }
	    }    

	    
	    printf("Endurance test finished\n\n");
	    break;
	    
    case STRESS:	/* verify that lib robust in loaded environment */
    
	    if ((pid = fork()) == -1)
	    {
	        fprintf(stderr, "\n\tError: fork() failed\n");
	        return test_rv;
	    }
	    else if (pid == 0)
	    {
	        hogcpu(); /* child procces that load CPU */ 
	    }
	    else
	    {
	        test_rv = TPASS;
        	
		for(i=0;i<Iter;i++)
	            thread_test_func(0);
	    }
	    
	    if (kill(pid, SIGKILL) != 0) 
	    {
	        fprintf(stderr, "\n\tError: Kill(SIGKILL) error\n");
	        return test_rv;
	    } 
    
	    printf("Stress test has finished\n\n");
	    break;
	    
	    
    case REENTRANCE:	/* verify that lib is reentrant */
    
	    switch (Case)
	    {
	    
	    case PTHREADED:
            
	    
            threads = (pthread_t*)malloc(sizeof(pthread_t)*Iter); 
            if(!threads)
            {
		        fprintf(stderr, "\n\tError: unable to allocate thread buffer\n");
		        return test_rv;            
            }
            
            for(i=0;i<Iter;i++)
            {
    	        if (pthread_create(&threads[i], NULL, (void* )&thread_test_func, NULL))
	            {
		            fprintf(stderr, "\n\tError: unable to create thread %d\n",i);
		            return test_rv;
    	        }
                else
                    printf("\nThread %d created\n",i);
            }
	        
	        test_rv = TPASS;
            for(i=0;i<Iter;i++)
            {
    	      pthread_join(threads[i],NULL);
              printf("\nThread %d finished\n",i);
            }
	        break;
	    
	    case PROCESSED:
	    
	        if ((pid = fork()) == -1)
	        {
		        fprintf(stderr, "\n\tError: fork failed\n");
		        return test_rv;
	        }
		    
		    test_rv = TPASS;
						       
		    if (pid == 0)
		    {
		        while(1) /*child */
		        {
			        thread_test_func(0);
		        }
		    }
		    else
		    {
                for(i=0;i<Iter;i++)
		            thread_test_func(0);
		        
		        if (kill(pid, SIGKILL) != 0)
		        {
			        fprintf(stderr, "\n\tError: Kill(SIGKILL) error\n");
		            return (test_rv=TFAIL);
		        }
		    }
	        
	        break;	     
	    
	    default:
	    
	        fprintf(stderr, "\n\tError: unsuported REENTRANCE test type\n");
	    }
    
	    printf("Reentrance test has finished\n\n");
	    break;  
        
    case PREEMPTIVE:	/* verify that two preemptive lib tasks are possible */
    
	    if ((pid = fork()) == -1)
	    {
	        fprintf(stderr, "\n\tError: fork failed\n");
	        return test_rv;
	    }
	    else
	    {
	        test_rv = TPASS;
															       
	        if (pid == 0)
	        {
		            nice(-10);
    		            
		            while(1)
		            {
            	       ; //thread_test_func(0);
		            }
            }
            else
            {
                sleep(1);
                for(i=0;i<Iter;i++)
                    thread_test_func(0);

		        if (kill(pid, SIGKILL) != 0)
    	        {
		            fprintf(stderr, "\n\tError: Kill(SIGKILL) error\n");
		            return (test_rv=TFAIL);
		        }
            }
	    }
	    
																																									    
	    printf("Preemptive test finished\n");	
	    break;

	    
    case RESTITUTION:	/* verify that twice mirrored picture is the same picture*/

	    printf("Pic original...\n\n");
	    
	    draw_pic(src, type, rect);

	    ref_src = (U8*)malloc(WIDTH*HEIGHT*bytes_per_pixel);
	    
	    if (ref_src == NULL) 
	    {
	        fprintf(stderr, "\n\tError: out of memory\n");
	        return test_rv;    
	    }			    
		    
	    memcpy(ref_src, src, WIDTH*HEIGHT*bytes_per_pixel);        /* create reference copy for bit comparing */		
	    
        test_rv = TPASS;
        
        {
            PFunc func=r_func;
            while(r_flag | R_flag )
            {
                if(r_flag)
                {
                    func = r_func;
                    r_flag = 0;
                    printf("\n Mirroring horizontal\n");
                }
                else if(R_flag)
                {
                    func = R_func;
                    R_flag = 0;
                    printf("\n Mirroring vertical\n");
                }

                linestride = WIDTH*bytes_per_pixel;
    
	            if ((err = func(src, rect, linestride, dst)) != OPLErrorSuccess)
	            {
	                fprintf(stderr, "\n\tError: first mirror failed\n");
	                print_err_type(err);
	                err_count++;
	            }

 	            printf("First step...\n\n");
                draw_pic(dst, type, rect);  

	            if ((err = func(dst, rect, linestride, src)) != OPLErrorSuccess)
	            {
	                fprintf(stderr, "\n\tError: second mirror failed\n");
	                print_err_type(err);
	                err_count++;
	            }

	            printf("Second step...\n\n");
                draw_pic(src, type, rect);
			            
	           						                
	            if (memcmp(src,ref_src,WIDTH*HEIGHT*bytes_per_pixel))
	            {
				            
	                printf("Bitmatch failed\n\n");						
                    test_rv = TFAIL;
	            }
                else
                {
                    printf("Bitmatch passed\n\n");
                }
            }
        }
	    free(ref_src);
	    printf("Restitution test finished\n");	    
	    break;

    
     default:
	        fprintf(stderr, "\n\tError: unsupported test type...\n");
    }

    return test_rv;
}


int configure_screen()
{
    int rv = -1;
    
    /* Open the framebuffer */ 	
    framebuffer.file_desc = open("/dev/fb0", O_RDWR); 

    /* Get screen infos  (size, resolution)	*/
    if( ioctl( framebuffer.file_desc, FBIOGET_VSCREENINFO, &framebuffer.screen_info ) )
    {
	fprintf(stderr, "\n\tError: configure_screen - reading variable information\n" );
	return rv;
    } 
    
    framebuffer.size = framebuffer.screen_info.xres * 
		       framebuffer.screen_info.yres * framebuffer.screen_info.bits_per_pixel / 8;
         
    /* Map the device to memory */
    framebuffer.ptr = (U8 *)mmap(0, framebuffer.size, 
                                    PROT_READ | PROT_WRITE, MAP_SHARED, framebuffer.file_desc, 0);
				    
    if ((int)framebuffer.ptr == -1)
    {
	fprintf(stderr, "\n\tError: configure_screen -  failed to map framebuffer device to memory.\n" );
	
	close(framebuffer.file_desc);
	return rv;
    }
    else
    {				     
	    printf("\nFrame buffer mapping succeed\n");
	    printf("\t\t screensize = %d\n", framebuffer.size);
	    printf("\t\t Width = %d\n", framebuffer.screen_info.xres);
	    printf("\t\t Height = %d\n", framebuffer.screen_info.yres);
	    printf("\t\t Color depth = %d bpp\n\n", framebuffer.screen_info.bits_per_pixel);
    }
    
    memset(framebuffer.ptr, 0x0, framebuffer.size);	/* blacken the screen */
	
    rv = 0;
		
    return rv; 
}



int hogcpu (void)
{
    while (1)
	    sqrt (rand ());

    return 0;
}


void detect_enter()
{
	int fd_console = 0;		/* 0 is the video input */
	fd_set fdset;
	struct timeval timeout;
	char c;

	printf("Press enter to run next test...timeout 15s\n");
	FD_ZERO(&fdset);
	FD_SET(fd_console, &fdset);
	timeout.tv_sec = 15;	/* set timeout !=0 => blocking select */
	timeout.tv_usec = 0;
	if (select(fd_console+1, &fdset, 0, 0, &timeout) > 0)
	{
		do 
		{
			read(fd_console, &c, 1);
		} while (c != 10);	// i.e. line-feed 
	}
}


int draw_pic(U8* src, pic_type type, OPLRect rect)
{
    int i, j;

    U8 r,g,b;     
    U8* ptr;
    U8* tmp_src;
    U8* tmp_ptr;
    
    U32 h_shift;        

    U16 pix;
    U8  red, green, blue;
    
    int file_size;

    if (type == GRS)	/* convert GREY | RGB565 to RGB888 */
    {
 	if(framebuffer.screen_info.bits_per_pixel == 24)
 	{
		file_size = WIDTH*HEIGHT;
	    
		tmp_src = (U8*)malloc(file_size*3);	/* grey - 1 byte per pix,   */
	    
		tmp_ptr = tmp_src;
		
		for(i = 0; i < file_size; i++)
		{
		    *(tmp_ptr++) = *src;
		    *(tmp_ptr++) = *src;
		    *(tmp_ptr++) = *(src++);
		}  
 	}
	else if(framebuffer.screen_info.bits_per_pixel == 16)
	{
		file_size = WIDTH*HEIGHT;
	    
		tmp_src = (U8*)malloc(file_size*2);	/* grey - 1 byte per pix,   */
	    
		tmp_ptr = tmp_src;
		
		for(i = 0; i < file_size; i++)
		{
		    	r = *src;    	    
			g = *src;
			b = *(src++);

			*(tmp_ptr++)  = (U8)((((g>>2)<<5) & 0xE0) | ((b>>3) & 0x1F));
			*(tmp_ptr++)  = (U8)((((g>>2)>>3) & 0x07) | ( r & 0xF8) );
		} 
	}
	else
		tst_resm(TFAIL, "Cannot be displayed\n");
		
    }
    else if (type == RGB565)			/* rgb565 - 2 bytes per pix */
    {
    	if(framebuffer.screen_info.bits_per_pixel == 24)
 	{
		file_size = WIDTH*HEIGHT*2;
		
		tmp_src = (U8*)malloc(file_size/2*3);	/* rgb888 - 3 bytes per pix */
		
		_TRACE
		
		tmp_ptr = tmp_src;
		
		for (i = 0; i < file_size/2; i++)
		{
		    pix = ((U16*)src)[i];	/* convert every pix - move ahead on 2 bytes */

		    red     = (pix & DTKMaskRed) >> DTKShiftRed;
		    green   = (pix & DTKMaskGreen) >> DTKShiftGreen;
		    blue    = (pix & DTKMaskBlue) >> DTKShiftBlue;
		    
		    red   <<= (8 - DTKSizeRed);   
		    green <<= (8 - DTKSizeGreen);
		    blue  <<= (8 - DTKSizeBlue);
		    
		    *(tmp_ptr++) = red;
		    *(tmp_ptr++) = green;
		    *(tmp_ptr++) = blue;	    
		}
    	}
	else if(framebuffer.screen_info.bits_per_pixel == 16)
	{
		file_size = WIDTH*HEIGHT*2; /* rgb565 - 2 bytes per pix */
	    
		tmp_src = (U8*)malloc(file_size);
	    
		tmp_ptr = tmp_src;

		memcpy(tmp_ptr, src, file_size);
	}

	else
		tst_resm(TFAIL, "Cannot be displayed\n");
    }
    else
    {
	fprintf(stderr,"\n\tError: unsupported pic type\n");
	return -1;
    }
    
    
    _TRACE
    
/* draw pic onto 24 or 16 bit color LCD display - it's assumed to be so */		    

    memset(framebuffer.ptr, 0xFF, framebuffer.size);    /* blacken the screen  */

    _TRACE
    
    if ((framebuffer.screen_info.xres < rect.mWidth) || (framebuffer.screen_info.yres < rect.mHeight))
    {
	fprintf(stderr, "\n\tError: pic size is greater than display size\n");
	return -1;
    }


	if(framebuffer.screen_info.bits_per_pixel == 24)
 	{
		h_shift = 3*(framebuffer.screen_info.xres - rect.mWidth);	/* initial shift to center pic */

		ptr = framebuffer.ptr + 3*(framebuffer.screen_info.xres*(framebuffer.screen_info.yres - rect.mHeight)/2 + 
					    (framebuffer.screen_info.xres - rect.mWidth)/2);
				
		_TRACE

		tmp_ptr = tmp_src;

		for(i = 0; i < rect.mHeight; i++)
		{
			for(j = 0; j < rect.mWidth; j++) 
			{    
				*ptr++ = *tmp_ptr++;	/* draw a scanline onto LCD display */
				*ptr++ = *tmp_ptr++;
				*ptr++ = *tmp_ptr++;
			}

			ptr += h_shift;		/* omit black part of LCD screen    */
		}
	}
	else if(framebuffer.screen_info.bits_per_pixel == 16)
	{
		h_shift = 2*(framebuffer.screen_info.xres - rect.mWidth);	/* initial shift to center pic */

		ptr = framebuffer.ptr + 2*(framebuffer.screen_info.xres*(framebuffer.screen_info.yres - rect.mHeight)/2 + 
					    (framebuffer.screen_info.xres - rect.mWidth)/2);

		tmp_ptr = tmp_src;

		for(i = 0; i < rect.mHeight; i++)
		{
			for(j = 0; j < rect.mWidth; j++) 
			{    
			    *(ptr++) = *(tmp_ptr++);	/* draw a scanline onto LCD display */
			    *(ptr++) = *(tmp_ptr++);
			}
			
			ptr += h_shift;		/* omit black part of LCD screen    */
		}
	}
	
    _TRACE
    
    detect_enter();
    
  //  memset(framebuffer.ptr, 0x0, framebuffer.size);   /* blacken the screen */
    
    free(tmp_src);
    

    return 0;
} 
 
void* thread_test_func(void* ptr)
{
    int i;
    
    int rv1, rv2;

    
    rv1 = test_mirror_horiz(x_size,y_size);	
    rv2 = test_mirror_vert (x_size, y_size);	

    if(rv1 == TFAIL || rv2 == TFAIL)
	    test_rv = TFAIL;		
    
    return ptr; /* not used  */
}

int test_mirror_horiz(const int srcBuffW, const int srcBuffH)
{
    return test_mirror(srcBuffW, srcBuffH, 'm');
}

int test_mirror_vert(const int srcBuffW, const int srcBuffH)
{
    return test_mirror(srcBuffW, srcBuffH,'M');
}


int test_mirror(const int srcBuffW, const int srcBuffH, int op)
{
    int         rv              = 0;
    
    const int   dstBuffW        = srcBuffW;
    const int   dstBuffH        = srcBuffH;
	    
    const int   srcBuffBytes    = srcBuffW*bytes_per_pixel*srcBuffH;
    const int   srcBuffStride   = srcBuffW*bytes_per_pixel;
		    
    const int   dstBuffBytes    = dstBuffW*bytes_per_pixel*dstBuffH;
    const int   dstBuffStride   = dstBuffW*bytes_per_pixel;
			    
    int         srcX;
    int         srcY;
    int         dstX;
    int         dstY;
    int         temp;
		    
    U8          *src;
    U8          *dst;

    PFunc func;

    U8         *p;

    int i,j;

    func = (op=='m'?r_func:R_func);
  
    OPLRect     srcRect;
    srcRect.mX          = 0;
    srcRect.mY          = 0;
    srcRect.mWidth      = srcBuffW;
    srcRect.mHeight     = srcBuffH;
    
    OPLRect     dstRect;
    dstRect.mX          = 0;
    dstRect.mY          = 0;
    dstRect.mWidth      = srcBuffW;
    dstRect.mHeight     = srcBuffH;



   src = (U8*)malloc(srcBuffBytes);
   dst = (U8*)malloc(dstBuffBytes);

    if(!src || !dst)
    {
	    fprintf(stderr, "\n\tError: memory allocation for screen buffer failed\n");
	} 

    memset(src, 0, srcBuffBytes);
    memset(dst, 0, dstBuffBytes);
		
    srcX = rand()%srcBuffW;
    srcY = rand()%srcBuffH;
	    
    dstX = (srcX - srcBuffW/2);
    dstY = (srcY - srcBuffH/2);

    if(op=='M')
    {
        dstX = -dstX-1;
    }
    else  /* 'm' */
    {
        dstY = -dstY-1;
    }


    dstX+=srcBuffW/2;
    dstY+=srcBuffH/2;

    p = src;
  
    /*
    for(i=0;i<srcBuffW*srcBuffH*bytes_per_pixel;i++)
      p[i] = rand();
    */

    
   for(i=0;i<bytes_per_pixel;i++) 
    p[srcX*bytes_per_pixel+srcY*srcBuffW*bytes_per_pixel+i] = 0xFF;

   if(verbose_mode)
   {
       printf("\n pixel %X set at %d %d", p[srcX+srcY*srcBuffW], srcX,srcY);
       printf("\nsrc:\n");  

       for(i=0;i<srcBuffBytes;i++)
           if(src[i])
               printf("\n value: %X  index: %d x=%d y=%d",src[i],i,
                        (i%srcBuffStride)/bytes_per_pixel, i/srcBuffStride);
   }

    if (func(src, srcRect, srcBuffStride, dst) != OPLErrorSuccess)
    {
    	fprintf(stderr, "\n\tError: rotate failed\n");
	    rv =  -1;
    }

    p = dst;

    if(verbose_mode)
    {
        printf("\ndst:\n");

         for(i=0;i<dstBuffBytes;i++)
           if(dst[i])
               printf("\n value: %X  index: %d x=%d y=%d",dst[i],i,
                        (i%dstBuffStride)/bytes_per_pixel, i/dstBuffStride);

    }


    printf(" %d %d : %d %d -> %d %d  ",srcBuffW, srcBuffH, srcX,srcY,dstX,dstY);        
    
    rv = TPASS;
    for(i=0;i<bytes_per_pixel;i++)    
    {
        if (   p[dstX*bytes_per_pixel+dstY*dstBuffW*bytes_per_pixel+i] != 0xFF)
	        rv =  TFAIL;
    }

    if( rv == TPASS) 
    {
	    printf("mirroring works properly\n\n");
    }
    else
    {
	    printf("mirroring doesn't work properly\n\n");
    }



   free(src);
   free(dst);
							   
   return rv;
							       							    
}
    
void U16_to_U8( U8 * rgb565, U8 * u8, U32 width, U32 height )
{
    U32 i;
    U16 * prgb = (U16*)rgb565;    
    U8 r, g, b;
    
    if( !rgb565 || !u8 )
    {
	fprintf( stderr, "U16_to_U8 : invalid arguments\n" );
	return;
    }

    printf( "converting U16 to U8, please wait..." );
    fflush( stdout );


    for( i = 0; i < width*height; ++i )
    {
        r = ((*prgb) >>DTKShiftRed)<<3;
        g = ((*prgb & DTKMaskGreen) >>DTKShiftGreen)<<2;
        b = ((*prgb & DTKMaskBlue))<<3;
         ++prgb;

	    *u8++ = ((r+g+b) / 3);
    }

    printf( " ok\n" );
}


void U16_to_U32( U8 * rgb565, U8 * u32, U32 width, U32 height )
{
    U32 i;
    U16 * prgb = (U16*)rgb565;    
    U8 r, g, b;
    
    if( !rgb565 || !u32 )
    {
	fprintf( stderr, "U16_to_U32 : invalid arguments\n" );
	return;
    }

    printf( "converting U16 to U2, please wait..." );
    fflush( stdout );


    for( i = 0; i < width*height; ++i )
    {
        r = (*prgb) >>DTKShiftRed;
        g = (*prgb & DTKMaskGreen) >>DTKShiftGreen;
        b = (*prgb & DTKMaskBlue);
        ++prgb;
	    *u32++ = r;
	    *u32++ = g;
	    *u32++ = b;
	    *u32++ = 255;
    }

    printf( " ok\n" );
}

  
void U16_to_U24( U8 * rgb565, U8 * u24, U32 width, U32 height )
{
    U32 i;
    U16 * prgb = (U16*)rgb565;    
    U8 r, g, b;
    
    if( !rgb565 || !u24 )
    {
	fprintf( stderr, "U16_to_U24 : invalid arguments\n" );
	return;
    }

    printf( "converting U16 to U24, please wait..." );
    fflush( stdout );


    for( i = 0; i < width*height; ++i )
    {
        r = (*prgb) >>DTKShiftRed;
        g = (*prgb & DTKMaskGreen) >>DTKShiftGreen;
        b = (*prgb & DTKMaskBlue);
        ++prgb;
	    *u24++ = r;
	    *u24++ = g;
	    *u24++ = b;
    }

    printf( " ok\n" );
}  


int load_file(char* path_file, U8* pic_data)
{
    _TRACE    

    int file_size = 0; 

    U8* buffer = malloc(WIDTH*HEIGHT*2);    
    if(!buffer)
    {
	    fprintf(stderr, "\n\tError: memory allocation for file buffer failed\n");
	} 


    FILE* fp = fopen(path_file, "rb");
    
    
    if ((fp == NULL) || (pic_data == NULL))
    {
	fprintf(stderr, "\n\tError: can't open pic file or pic buffer memory error...\n");
	free(buffer);
    return -1;
    }
    

   

    file_size = fread(buffer, 1, WIDTH*HEIGHT*2, fp);    
    fclose(fp);
    
    if (file_size == 0)	
    {
        fprintf(stderr, "\n\tError: reading error ...\n");
        free(buffer);
        return -1;
    }

    switch(bytes_per_pixel)
    {
        case 1:
                U16_to_U8( buffer, pic_data, WIDTH, HEIGHT );
                break;
        case 2:
                memcpy(pic_data, buffer, WIDTH*HEIGHT*2);;
                break;
/*

        case 4:
                U16_to_U32( buffer, pic_data, WIDTH, HEIGHT );
                break;
*/
        default:
            	fprintf(stderr, "\n\tError: unknown resolution...\n");
	            file_size = -1;
    }

    free(buffer);
    
    return file_size;	       
}



						/* type arg not ised so far - default RGB565 */

int GenerateBitmap(unsigned char *pBackBuffer, pic_type type, int height, int width, char *pBMPPath)
{
    typedef short	 WORD;
    typedef unsigned int DWORD;
    typedef int          LONG;
    
    FILE*	 hf; 
    int		 pitch, buflen, i, j;
    unsigned int r,g,b;
    U16          col;
		
    typedef struct
    { 
	WORD 	DUMMY;
	WORD    bfType; 
	DWORD   bfSize; 
	WORD    bfReserved1; 
	WORD    bfReserved2; 
	DWORD   bfOffBits; 
    
    } BITMAPFILEHEADER;
    
    BITMAPFILEHEADER	bfh;
    
								
    typedef struct tagBITMAPINFOHEADER
    {
	DWORD  biSize;
	LONG   biWidth; 
	LONG   biHeight;
	WORD   biPlanes;
	WORD   biBitCount;
	DWORD  biCompression;
	DWORD  biSizeImage;
	LONG   biXPelsPerMeter; 
	LONG   biYPelsPerMeter;
	DWORD  biClrUsed;
	DWORD  biClrImportant; 
    
    } BITMAPINFOHEADER; 
												    			    
    BITMAPINFOHEADER	bmiHeader;

	
    pitch  = ((3 * width + 3) & ~3);
	    
    buflen = height * pitch;
		
    hf = fopen( pBMPPath, "wb" ); 
    
    if ( hf == NULL )
    {
	return 1;
    }
    
    /* fill in the BITMAPINFOHEADER structure */
    
    bfh.bfType 		= 0x4D42; 
    bfh.bfSize 		= sizeof(BITMAPFILEHEADER) + buflen 
                                                   - sizeof(bfh.DUMMY);
    bfh.bfReserved1     = 0; 
    bfh.bfReserved2     = 0; 
    bfh.bfOffBits 	= sizeof(BITMAPFILEHEADER) - sizeof(bfh.DUMMY) 
                                                   + sizeof(BITMAPINFOHEADER);
						   
    fwrite( &bfh.bfType, 1, sizeof(BITMAPFILEHEADER) - sizeof(bfh.DUMMY), hf );
    
    /* fill in the BITMAPINFOHEADER structure */
    
    bmiHeader.biSize	    	= sizeof(BITMAPINFOHEADER);
    bmiHeader.biWidth		= width;
    bmiHeader.biHeight		= height;
    bmiHeader.biPlanes		= 1;
    bmiHeader.biBitCount	= 24;
    bmiHeader.biCompression	= 0;
    bmiHeader.biSizeImage	= buflen;
    bmiHeader.biXPelsPerMeter	= 0;
    bmiHeader.biYPelsPerMeter	= 0;
    bmiHeader.biClrUsed		= 0;
    bmiHeader.biClrImportant	= 0;
					     
    fwrite( &bmiHeader, 1, bmiHeader.biSize, hf );
    
    
    /* write the DIB */
		
    for (i = height - 1; i >= 0; i--)
    {
	for (j = 0; j < width; j++)
	{
	    col	= ( (U16 *)pBackBuffer )[(i * width) + j];
	    
	    r	= (col & DTKMaskRed)	>> DTKShiftRed;
	    g	= (col & DTKMaskGreen)	>> DTKShiftGreen;
	    b	= (col & DTKMaskBlue)	>> DTKShiftBlue;
						
	    r	= r << (8 - DTKSizeRed);
	    g	= g << (8 - DTKSizeGreen);
	    b	= b << (8 - DTKSizeBlue);
						
	    fwrite(&b, 1, 1, hf);
	    fwrite(&g, 1, 1, hf);
	    fwrite(&r, 1, 1, hf);
	}
	
	j = pitch - 3 * width;
	
	if (j > 0)
	{
	    fwrite(pBackBuffer, 1, j, hf);
	}
    }
    
    fclose (hf);
    
    return 0;
}
								



#ifdef __cplusplus
}
#endif