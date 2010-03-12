#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>

typedef unsigned char                   UInt8;  
typedef signed char                     SInt8;  
typedef unsigned short                   UInt16;  
typedef signed short                     SInt16;  
   
#if __LP64__  
typedef unsigned int                    UInt32;  
typedef signed int                      SInt32;  
#else  
typedef unsigned long                   UInt32;  
typedef signed long                     SInt32;  
#endif  
   
typedef     UInt32      DWORD;  
typedef     UInt16      WORD;  
typedef     unsigned char   BYTE; 
typedef     long        LONG;
   
#ifndef bool  
typedef     unsigned short bool;  
#define false 0  
#define true 0  
#endif  
#define OPT 1
#define SBIT 10
#define A1 ((unsigned int)(1.164 * (1<<SBIT)))
#define A2 ((unsigned int)(1.596 * (1<<SBIT)))
#define A3 ((unsigned int)(0.813 * (1<<SBIT)))
#define A4 ((unsigned int)(0.391 * (1<<SBIT)))
#define A5 ((unsigned int)(2.018 * (1<<SBIT)))

#define B1 ((unsigned int)(1.370705 * (1<<SBIT)))
#define B2 ((unsigned int)(0.698001 * (1<<SBIT)))
#define B3 ((unsigned int)(0.337633 * (1<<SBIT)))
#define B4 ((unsigned int)(1.732446 * (1<<SBIT)))


#define OUTFORMAT "RAW_DATA"

inline unsigned char  BOUND255(short a)
{
      return (a > 255) ? 255: (a < 0)? 0: a;  
}


typedef struct tagBITMAPFILEHEADER  
{  
     WORD    bfType;  
     DWORD   bfSize;  
     WORD    bfReserved1;  
     WORD    bfReserved2;  
     DWORD   bfOffBits;  
} __attribute__((packed)) BITMAPFILEHEADER, *PBITMAPFILEHEADER;  
   
 typedef struct tagBITMAPINFOHEADER {  
     DWORD   biSize;  
     LONG    biWidth;  
     LONG    biHeight;  
     WORD    biPlanes;  
     WORD    biBitCount;  
     DWORD   biCompression;  
     DWORD   biSizeImage;  
     LONG    biXPelsPerMeter;  
     LONG    biYPelsPerMeter;  
     DWORD   biClrUsed;  
     DWORD   biClrImportant;  
 } BITMAPINFOHEADER, * PBITMAPINFOHEADER;   
   
 typedef struct tagRGBQUAD {  
     BYTE rgbBlue;  
     BYTE rgbGreen;  
     BYTE rgbRed;  
     BYTE rgbReserved;  
 }RGBQUAD;


typedef enum {
     e1Index = 0,
     e4Index,
     e8Index,
     e16Gray,
     eRGB555,
     eRGB565,
     eARGB1555,
     eRGB24,
     eBGR24,
     eRGB32,
     eBGR32,
     eARGB32,
     eRGBA32,
     eYUV420,
     eYUV422P,
     eBMP24,
     eUYVY422,
     eYUY2,/*YUYV*/
     eUNKNOWN
   }eEPType_Format;

typedef struct SFMT{
 char * fmt;
 int et;
 int bs; /*bits pre pixel*/
}sFmt;


static sFmt aFmt[ ]= { 
       {"1bpp", 0, 1},
       {"4bpp", 1, 4},
       {"8bpp", 2, 8},
       {"16Gray",3, 4},
       {"RGB555",4, 16},
       {"RGB565",5, 16},
       {"ARGB1555",6, 16},
       {"RGB24",7, 24},
       {"BGR24",8, 24},
       {"RGB32",9, 32},
       {"BGR32",10, 32},
       {"ARGB32",11, 32},
       {"RGBA32",12, 32},
       {"YUV420",13, 16},
       {"YUV422P",14, 16},
       {"BMP24",15, 24},
       {"UYVY",16,16},
       {"YUY2",16,16},
       {NULL, 17, 0}
      };

char * options = "i:I:x:y:O:o:b:f:Hi:C";

char test_head[256]; 

char ifilename[256];
char iformat[256];
int xres;
int yres;
long offset = 0l;
char ofilename[256];
char oformat[256];
int oenc=5; /*same with eEPType_Format*/
int head=0;
int cflag = 0;/*conv input for mx25*/

extern int conv_mx25_yuv(unsigned char * data_in,int iw, int ih,unsigned char * data_out);


static void  usage()
{
 printf("command line:  \n   \
       -i [input file name], \n \
       -I [format], \n  \
       -b [offset], \n  \
       -x [xsize], \n   \
       -y [ysize], \n   \
       -o [output file name] \n \
       -O [out format] \n  \
       -f [raw out encoding] \n \
       -H [raw out head on/off] \n \
       -C [conver yuv420 to formal yuv420P as input for mx25] \n ");
}


static int parse_input(int argc, char ** argv)
{
 char opt;
 int status = 0;
 
 strcpy(ofilename,"out.bmp");
 while((opt = getopt(argc, argv, options)) > 0)
 {
  switch(opt)
  {
   case 'i':
      status |= 0x01; 
      strncpy(ifilename,optarg,sizeof(ifilename) - 1);
      break;
   case 'I': 
      status |= 0x02;
      strncpy(iformat,optarg,sizeof(iformat) - 1);
      break;
   case 'x':
      status |=0x04;
      xres = atoi(optarg);
      break;
   case 'y': 
      status |= 0x08;
      yres = atoi(optarg);
      break;
   case 'b':
      offset = atoi(optarg);
      break;
   case 'o':
      strncpy(ofilename,optarg,sizeof(ofilename) - 1);
      break;
   case 'O':
      strncpy(oformat,optarg,sizeof(oformat) - 1);
      break;
   case 'f':
      oenc = atoi(optarg);
      break;
   case 'H':
      
      break;
   case 'C':
      cflag = 1;
      break;
   default:
      printf("wrong parameter \n");
      usage();
  }
 }
 if(status == 0x0F)
   return 0;
 printf("\n not enough parameter\n");
 usage();
 return 1;
}

static int process_img()
{
  int i,j;
  int fdin, fdout;
  long l, fmt = -1;
  size_t isz = 0;
  BITMAPINFOHEADER BitmapInfoHeader;  
  BITMAPFILEHEADER BitmapFileHeader; 
  RGBQUAD rgb;
  unsigned char *pdata, * pout;
  struct stat file_stat;
  void *start_fp, *p_ft;
  unsigned char * temp = NULL;


   umask(0x000);
   if((fdin = open(ifilename, O_RDWR)) < 0)
   {
     printf("open file wrong!");
       return -1;
    }

  if ( fstat( fdin, &file_stat) < 0 )
  {
       printf(" fstat wrong");
       return -1;
  }

  isz = file_stat.st_size;

  if( ( start_fp = mmap(NULL, file_stat.st_size, PROT_READ|PROT_WRITE, MAP_SHARED, fdin, 0 )) == MAP_FAILED)
  {
      printf(" in mmap wrong\n");
      exit(0);
  }

  if ( (fdout = open(ofilename, O_RDWR|O_CREAT,S_IRWXU|S_IRWXG|S_IRWXO)) < 0){
     printf("Cannot create the output file %s !\n", ofilename);  
     return -5;  
  }

   if ( fstat( fdout, &file_stat) < 0 )
   {
      printf(" fstat wrong");
      return -6;
   }


  if(strcmp(OUTFORMAT, oformat)!=0)
  {
    l = ((xres * 24 + 31) & ~31) / 8;
    //Write bitmap header  
    BitmapFileHeader.bfType = 0x4D42;
    BitmapFileHeader.bfReserved1 = 0;
    BitmapFileHeader.bfReserved2 = 0;
    /*only output RGB24 bit format*/
    BitmapFileHeader.bfSize = l * yres + 54;
    BitmapFileHeader.bfOffBits = 54;
  
    BitmapInfoHeader.biSize = 40;
    BitmapInfoHeader.biWidth = xres;
    BitmapInfoHeader.biHeight = yres;
    BitmapInfoHeader.biPlanes = 1;
    BitmapInfoHeader.biBitCount = 24;
    BitmapInfoHeader.biCompression = 0;
    BitmapInfoHeader.biSizeImage = l * yres;
    BitmapInfoHeader.biXPelsPerMeter = 0;
    BitmapInfoHeader.biYPelsPerMeter = 0;
    BitmapInfoHeader.biClrUsed = 0;
    BitmapInfoHeader.biClrImportant = 0;
  
    lseek(fdout,l * yres + 53,SEEK_SET);
    write(fdout,"",1);
    if( ( p_ft = mmap(NULL, l * yres + 54, PROT_READ|PROT_WRITE, MAP_SHARED, fdout, 0 )) == MAP_FAILED)
    {
     perror("mmap"); 
     exit(0);
    }
    memcpy(p_ft,(unsigned char *)&BitmapFileHeader,14);
    memcpy((unsigned char *)p_ft + 14, (unsigned char *)&BitmapInfoHeader, 40);
  }else{
      l = ((xres * 24 + 31) & ~31) / 8;
      if ( strcmp(iformat,"BMP24") ==0)
      {
       int bc=((aFmt[oenc].bs + 7)&(~7)) / 8;  
       lseek(fdout,xres * yres * bc - 1,SEEK_SET);
       write(fdout,"",1);
       printf("the output file size is %d * %d * %d \n",  xres , yres , bc);
       if( ( p_ft = mmap(NULL, xres * yres * bc, PROT_READ|PROT_WRITE, MAP_SHARED, fdout, 0 )) == MAP_FAILED)
       {
         perror("mmap"); 
         exit(0);
       }
      }else{
       printf("the output file size is %d \n", isz - offset);
       lseek(fdout,isz - offset,SEEK_SET);
       write(fdout,"",1);
       if( ( p_ft = mmap(NULL, isz - offset, PROT_READ|PROT_WRITE, MAP_SHARED, fdout, 0 )) == MAP_FAILED)
       {
         perror("mmap"); 
         exit(0);
       }
      }
  }/*end strcmp*/
/*
  fwrite((unsigned char *)&BitmapFileHeader,14,1,fpdst);  
  fwrite((unsigned char *)&BitmapInfoHeader,40,1,fpdst);
*/
   pdata = (unsigned char *)start_fp + offset;
   if(strcmp(OUTFORMAT, oformat) != 0)
   {
     pout = (unsigned char *)p_ft + 14 + 40;
   }else{
    printf("%s \n", iformat);
    pout = (unsigned char *)p_ft;
    if (strcmp(iformat,"BMP24") == 0)
    {      
      printf("BMP to raw data is partial supported\n"); 
    }else{
      memcpy(pout,pdata,isz - offset);
      goto END; 
    }
   }

  for(i = 0; i < eUNKNOWN; i++)
  {
    if(strcmp(iformat, aFmt[i].fmt) == 0)
    {
        fmt = aFmt[i].et;
	printf("format %s found %d \n",iformat,fmt);
	break;
    }
  }
   
  switch(fmt)
  {
   case eRGB565:
   if(isz - offset < yres * xres * 2)
   {
       printf("%d,%d,%d,%d\n", isz,offset, yres, xres);
       printf("file size or offset wrong\n");
       exit(-2);
   }
   for(i = 0 ; i < yres; i++)
    for(j = 0; j < xres; j++)
    {
     long k = (i * xres + j) * 2;
     rgb.rgbRed = pdata[k + 1]&0xf8; 
     rgb.rgbGreen = ((pdata[k]&0xe0)>>3)  + ((pdata[ k + 1] & 0x07)<<5);
     rgb.rgbBlue =  (pdata[k]&0x1f)<<3;
     rgb.rgbReserved = 0;
     k = i * l + j * 3;
     pout[k + 2] = rgb.rgbRed;
     pout[k + 1] = rgb.rgbGreen;
     pout[k] = rgb.rgbBlue;
    }
      break;
   case eBGR24:   
   if(isz - offset < yres * xres * 3)
   {
       printf("%d,%d,%d,%d\n", isz,offset, yres, xres);
       printf("file size or offset wrong\n");
       exit(-2);
   }
   for(i = 0 ; i < yres; i++)
    for(j = 0; j < xres; j++)
    {
     long k = (i * xres + j) * 3;
     rgb.rgbRed = pdata[k]; 
     rgb.rgbGreen = pdata[k + 1];
     rgb.rgbBlue =  pdata[k + 2];
     rgb.rgbReserved = 0;
     k = i * l + j * 3;
     pout[k] = rgb.rgbRed;
     pout[k + 1] = rgb.rgbGreen;
     pout[k + 2] = rgb.rgbBlue;
    }
      break;
   case eRGB24:
   if(isz - offset < yres * xres * 3)
   {
       printf("file size or offset wrong\n");
       exit(-2);
   }
   for(i = 0 ; i < yres; i++)
    for(j = 0; j < xres; j++)
    {
     long k = (i * xres + j) * 3;
     rgb.rgbRed = pdata[k + 2]; 
     rgb.rgbGreen = pdata[k + 1];
     rgb.rgbBlue =  pdata[k];
     rgb.rgbReserved = 0;
     k = i * l  + j * 3;
     pout[k] = rgb.rgbRed;
     pout[k + 1] = rgb.rgbGreen;
     pout[k + 2] = rgb.rgbBlue;
    }
      break;
   case eBGR32:
   if(isz - offset < yres * xres * 4)
   {
       printf("file size or offset wrong\n");
       exit(-2);
   }
   for(i = 0 ; i < yres; i++)
    for(j = 0; j < xres; j++)
    {
     long k = (i * xres + j) * 4;
     /*
     printf("%u,%u,%u \n",pdata[k], pdata[k+1], pdata[k+2]);
     getchar();
     */
     rgb.rgbRed = pdata[k]; 
     rgb.rgbGreen = pdata[k + 1];
     rgb.rgbBlue =  pdata[k + 2];
     rgb.rgbReserved = 0;
     k = i * l  + j * 3;
     pout[k] = rgb.rgbRed;
     pout[k + 1] = rgb.rgbGreen;
     pout[k + 2] = rgb.rgbBlue;
    }
    break;
   case eRGB32:
   if(isz - offset < yres * xres * 4)
   {
       printf("file size or offset wrong\n");
       exit(-2);
   }
   for(i = 0 ; i < yres; i++)
    for(j = 0; j < xres; j++)
    {
     long k = (i * xres + j) * 4;
     rgb.rgbRed = pdata[k + 2]; 
     rgb.rgbGreen = pdata[k + 1];
     rgb.rgbBlue =  pdata[k];
     rgb.rgbReserved = 0;
     k = i * l  + j * 3;
     pout[k] = rgb.rgbRed;
     pout[k + 1] = rgb.rgbGreen;
     pout[k + 2] = rgb.rgbBlue;
    }
   case eYUV422P:
   {
    long bias = xres * yres;
    if(isz - offset < yres * xres * 2)
    {
       printf("file size or offset wrong %d - %d < %d * %d * 2 \n", isz, offset ,yres, xres);
       exit(-2);
    }
    /*http://hi.baidu.com/whandsome/blog/item/d060ffef3f1c6b37acafd580.html*/
    for(i = 0 ; i < yres; i++)
     for(j = 0; j < xres; j = j + 2)
     {
     long y1,u,y2,v,k;
     long ky = i * xres + j; 
     long ku = ((i * xres)>>1) + (j>>1) + bias;/*u plane*/
     long kv = ((i * xres)>>1) + (j>>1) + bias + (bias>>1);/*v plane*/   
     y1 = pdata[ky];
     u  = pdata[ku]; 
     y2 = pdata[ky + 1];
     v  = pdata[kv];
     rgb.rgbRed   = BOUND255((A1 * (y1 - 16) + A2 * (v - 128))>>10); 
     rgb.rgbGreen = BOUND255((A1 * (y1 - 16) - A3 * (v - 128) - A4 * (u - 128))>>10);
     rgb.rgbBlue  = BOUND255((A1 * (y1 - 16) + A5 * (u - 128))>>10);
     rgb.rgbReserved = 0;
     k = i * l  + j * 3;
     pout[k + 2] = rgb.rgbRed;
     pout[k + 1] = rgb.rgbGreen;
     pout[k] = rgb.rgbBlue;
     rgb.rgbRed   =  BOUND255((A1 * (y2 - 16) + A2 * (v - 128))>>10); 
     rgb.rgbGreen =  BOUND255((A1 * (y2 - 16) - A3 * (v - 128) - A4 * (u - 128))>>10);
     rgb.rgbBlue  =  BOUND255((A1 * (y2 - 16) + A5 * (u - 128))>>10);
     pout[k + 5] = rgb.rgbRed;
     pout[k + 4] = rgb.rgbGreen;
     pout[k + 3] = rgb.rgbBlue;
     }
    }
    break;
   case eYUY2:
   if(isz - offset < yres * xres * 2)
   {
       printf("file size or offset wrong\n");
       exit(-2);
   }
   /*serial 2 pixels in 4 byte*/
   for(i = 0; i < yres; i++)
    for(j = 0; j < xres - 2; j = j + 2)
    {
     long u,y1,v,y2;
     long k = (i * xres + j) * 2;
     if (0x01)
     {
     y1  = pdata[k];
     u  = pdata[k + 1];
     y2 = pdata[k + 2];
     v  = pdata[k + 3];
     }else{
     y1 = pdata[k];
     v  = pdata[k + 1];
     y2 = pdata[k + 2];
     u  = pdata[k + 3];
     }
     /*refer to http://www.vckbase.com/document/viewdoc/?id=1780 */
     #if OPT
     rgb.rgbRed   = BOUND255(((y1<<SBIT) +  B1 * (v - 128))>>SBIT);
     rgb.rgbGreen = BOUND255(((y1<<SBIT) - B2 * (v -128) - B3 * (u - 128))>>SBIT);
     rgb.rgbBlue  = BOUND255(((y1<<SBIT) + B4 * (u - 128))>>SBIT);
     #else
     rgb.rgbRed   = BOUND255(y1 +  1.370705 * (v - 128));
     rgb.rgbGreen = BOUND255(y1 - 0.698001 * (v -128) - 0.337633 * (u - 128));
     rgb.rgbBlue  = BOUND255(y1 + 1.732446 * (u - 128));
     #endif
     rgb.rgbReserved = 0;
     k = (yres - i - 1) * l  + j * 3;
     pout[k + 2] = rgb.rgbRed;
     pout[k + 1] = rgb.rgbGreen;
     pout[k] = rgb.rgbBlue;
     #if OPT
     rgb.rgbRed   = BOUND255(((y2<<SBIT) +  B1 * (v - 128))>>SBIT);
     rgb.rgbGreen = BOUND255(((y2<<SBIT) - B2 * (v - 128) - B3 * (u - 128))>>SBIT);
     rgb.rgbBlue  = BOUND255(((y2<<SBIT) + B4 * (u - 128))>>SBIT);
     #else
     rgb.rgbRed   = BOUND255(y2 +  1.370705 * (v - 128));
     rgb.rgbGreen = BOUND255(y2 - 0.698001 * (v -128) - 0.337633 * (u - 128));
     rgb.rgbBlue  = BOUND255(y2 + 1.732446 * (u - 128));
     #endif
     pout[k + 5] = rgb.rgbRed;
     pout[k + 4] = rgb.rgbGreen;
     pout[k + 3] = rgb.rgbBlue;
    }

    break;
   case eUYVY422:
   if(isz - offset < yres * xres * 2)
   {
       printf("file size or offset wrong\n");
       exit(-2);
   }
   /*serial 2 pixels in 4 byte*/
   for(i = 0; i < yres; i++)
    for(j = 0; j < xres - 2; j = j + 2)
    {
     long u,y1,v,y2;
     long k = (i * xres + j) * 2;
     if (0x01)
     {
     u  = pdata[k];
     y1  = pdata[k + 1]; 
     v = pdata[k + 2];
     y2  = pdata[k + 3];
     }else{
     v = pdata[k];
     y1  = pdata[k + 1]; 
     u = pdata[k + 2];
     y2  = pdata[k + 3];
     }
     /*refer to http://www.vckbase.com/document/viewdoc/?id=1780 */
     #if OPT
     rgb.rgbRed   = BOUND255(((y1<<SBIT) +  B1 * (v - 128))>>SBIT); 
     rgb.rgbGreen = BOUND255(((y1<<SBIT) - B2 * (v -128) - B3 * (u - 128))>>SBIT);
     rgb.rgbBlue  = BOUND255(((y1<<SBIT) + B4 * (u - 128))>>SBIT);
     #else
     rgb.rgbRed   = BOUND255(y1 +  1.370705 * (v - 128)); 
     rgb.rgbGreen = BOUND255(y1 - 0.698001 * (v -128) - 0.337633 * (u - 128));
     rgb.rgbBlue  = BOUND255(y1 + 1.732446 * (u - 128));
     #endif
     rgb.rgbReserved = 0;
     k = (yres - i - 1) * l  + j * 3;
     pout[k + 2] = rgb.rgbRed;
     pout[k + 1] = rgb.rgbGreen;
     pout[k] = rgb.rgbBlue;
     #if OPT
     rgb.rgbRed   = BOUND255(((y2<<SBIT) +  B1 * (v - 128))>>SBIT); 
     rgb.rgbGreen = BOUND255(((y2<<SBIT) - B2 * (v - 128) - B3 * (u - 128))>>SBIT);
     rgb.rgbBlue  = BOUND255(((y2<<SBIT) + B4 * (u - 128))>>SBIT);
     #else
     rgb.rgbRed   = BOUND255(y2 +  1.370705 * (v - 128)); 
     rgb.rgbGreen = BOUND255(y2 - 0.698001 * (v -128) - 0.337633 * (u - 128));
     rgb.rgbBlue  = BOUND255(y2 + 1.732446 * (u - 128));
     #endif
     pout[k + 5] = rgb.rgbRed;
     pout[k + 4] = rgb.rgbGreen;
     pout[k + 3] = rgb.rgbBlue;
    }
    break;

   case eYUV420:
   {
    long bias = xres * yres;
   if(isz - offset < yres * xres * 1.5)
   {
       printf("isz = %d, where %d * %d * 1.5, add offset %d \n", isz, xres, yres, offset);
       printf("file size or offset wrong\n");
       exit(-2);
   }
   if(cflag)
   {
     temp = (unsigned char *)malloc(file_stat.st_size);
     conv_mx25_yuv(pdata,xres,yres,temp);
     pdata = temp;
     //printf("temp size is %ld\n", file_stat.st_size);
   }
   /*http://www.fourcc.org/yuv.php#IYUV*/
   /*2x2 pixel for one time*/
    for(i = 0 ; i < yres - 1; i += 2)
     for(j = 0; j < xres - 1; j += 2)
     {
     unsigned char y0,y1,y2,y3;
     long u,v;
     long k = 0;
     long ky = i * xres + j;
     long ku = ((i * xres)>>2) + (j>>1) + bias;/*u plane*/
     long kv = ((i * xres)>>2) + (j>>1) + bias + (bias>>2);/*v plane*/   
     #if 0 
     printf("\r processing %d pixel , line %d, x %d\n",ky,i,j);
     printf("\r bias %d, kv %d,ku %d\n",bias, kv,ku );
     #endif
     y0 = pdata[ky];
     u = pdata[ku];
     v = pdata[kv];
     y1 = pdata[ky + 1];
     ky  = (i + 1) * xres + j;
     y2 = pdata[ky];
     y3 = pdata[ky +1];
     
     rgb.rgbRed   = BOUND255((A1 * (y0 - 16) + A2 * (v - 128))>>SBIT); 
     rgb.rgbGreen = BOUND255((A1 * (y0 - 16) - A3 * (v - 128) - A4 * (u - 128))>>SBIT);
     rgb.rgbBlue  = BOUND255((A1 * (y0 - 16) + A5 * (u - 128))>>SBIT);
     rgb.rgbReserved = 0;
     k = i * l  + j * 3;
     pout[k + 2] = rgb.rgbRed;
     pout[k + 1] = rgb.rgbGreen;
     pout[k]     = rgb.rgbBlue;
     rgb.rgbRed   = BOUND255((A1 * (y1 - 16) + A2 * (v - 128))>>SBIT); 
     rgb.rgbGreen = BOUND255((A1 * (y1 - 16) - A3 * (v - 128) - A4 * (u - 128))>>SBIT);
     rgb.rgbBlue  = BOUND255((A1 * (y1 - 16) + A5 * (u - 128))>>SBIT);
     pout[k + 5] = rgb.rgbRed;
     pout[k + 4] = rgb.rgbGreen;
     pout[k + 3] = rgb.rgbBlue;
    
     k = (i + 1) * l  + j * 3;

     rgb.rgbRed   = BOUND255((A1 * (y2 - 16) + A2 * (v - 128))>>SBIT); 
     rgb.rgbGreen = BOUND255((A1 * (y2 - 16) - A3 * (v - 128) - A4 * (u - 128))>>SBIT);
     rgb.rgbBlue  = BOUND255((A1 * (y2 - 16) + A5 * (u - 128))>>SBIT);
     pout[k + 2] = rgb.rgbRed;
     pout[k + 1] = rgb.rgbGreen;
     pout[k] = rgb.rgbBlue;
    
     rgb.rgbRed   =  BOUND255((A1 * (y3 - 16) + A2 * (v - 128))>>SBIT); 
     rgb.rgbGreen =  BOUND255((A1 * (y3 - 16) - A3 * (v - 128) - A4 * (u - 128))>>SBIT);
     rgb.rgbBlue  =  BOUND255((A1 * (y3 - 16) + A5 * (u - 128))>>SBIT);
     pout[k + 5] = rgb.rgbRed;
     pout[k + 4] = rgb.rgbGreen;
     pout[k + 3] = rgb.rgbBlue;
     }
   }
    break;
   case eBMP24: 
    pdata = (unsigned char *)start_fp + 54;
    for(i = 0 ; i < yres; i++)
     for(j = 0; j < xres; j++)
     {
       long k = i * l  + j * 3;
       if(oenc ==  eRGB565)
       {/*rgb565*/
         unsigned char r,g,b;
         long k2 = (i * xres + j) * 2;
	 r = pdata[k + 2];
	 g = pdata[k + 1];
	 b = pdata[k];
	 pout[k2 + 1] = (r&0xf8) + ((g&0xe0)>>5);
	 pout[k2] = ((g&0x1c)<<3) + ((b&0xf8)>>3);
	 /*
	 printf("r = %u, g = %u, b= %u \n",r,g,b);
	 printf("lb = %u, hb = %u\n",pout[k2],pout[k2 + 1]);
	 getchar();
	 */
       }else if(oenc ==  eYUV422P){
       /*yuv422*/
         unsigned char r,g,b;
	 long ubias = xres * yres;
	 int vbias = ubias + (ubias>>1);
         long k2 = i * xres + j;
	 r = pdata[k + 2];
	 g = pdata[k + 1];
         b = pdata[k];
	 pout[k2] =  (unsigned char)(0.257 * r + 0.504 * g + 0.098 * b + 16);/*y*/
	 if(k2&0x01)
	 {
	  int ko = (k2>>1);
	  pout[ko + ubias] = (unsigned char)(-0.148 * r - 0.291 * g + 0.439 * b + 128);/*u*/
	  pout[ko + vbias] = (unsigned char)(0.439 * r - 0.368 * g - 0.071 * b + 128);/*v*/
	 }
       }else if(oenc == eYUV420){
       /*yuv420*/
       }else if(oenc == eRGB24){ 
       /*rgb24*/
       pout[k + 2] = pdata[k];
       pout[k + 1] = pdata[k + 1];
       pout[k] = pdata[k + 2];
       }else if(oenc == eRGBA32){
       /*rgba32*/
       long k2 = (i * xres + j) * 4;
       pout[k2] = pdata[k];
       pout[k2 + 1] = pdata[k + 1];
       pout[k2 + 2] = pdata[k + 2];
       pout[k2 + 3] = 0; /*alpha=0*/
       }else{
        printf("unsupported format");
	goto END;
       }
     }
    break;
   default:
     printf("unsupported format %d \n", fmt);
     break;
  }
 END:
 if(temp != NULL){
    free(temp);
    temp = NULL;
 }
 munmap(p_ft, l * yres + 54);
}

int main(int argc,char ** argv)
{
  if( parse_input(argc,argv))
    return 1;
  process_img();
  printf("\nwe done!\n");
  return 0;
}


