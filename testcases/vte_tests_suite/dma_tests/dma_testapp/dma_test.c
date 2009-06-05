/*
  Copyright (C) 2004, Freescale Semiconductor, Inc. All Rights Reserved
  THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
  BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
  Freescale Semiconductor, Inc.
*/

/**
@file dma_test.c

@par Portability:
        ARM GCC
*/

/*======================== REVISION HISTORY ==================================

Author (core ID)      Date         CR Number    Description of Changes
-------------------   ----------   ----------   ------------------------------
D.Simakov / b00236    31/08/2006   TLSbo76801   Initial version 
=============================================================================*/


/*==================================================================================================
                                              INCLUDE FILES
 ==================================================================================================*/
                                        
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <malloc.h>
#include <unistd.h>
#include <asm/page.h>
#include <sys/ioctl.h>
#include "dma_test.h"

#ifdef CONFIG_ARCH_MX2ADS
#include <asm/dma.h>
#include <asm/arch/dma.h>
#endif


/*==================================================================================================
                                              DEFINES AND MACROSES
 ==================================================================================================*/


#define DMA_BURST_LENGTH_4   4
#define DMA_BURST_LENGTH_8   8
#define DMA_BURST_LENGTH_16 16
#define DMA_BURST_LENGTH_32 32
#define DMA_BURST_LENGTH_64  0
#define DMA_NAME  "/dev/mxc_dma"

#define CALL_IOCTL(ioctl_) {if((ioctl_) < 0) {tst_resm(TINFO,"%s : ioctl failed [file %s, line %d]", __FUNCTION__, __FILE__, __LINE__); return -1;} }
#define CALL_IOCTL_NR(ioctl_) {if((ioctl_) < 0) {tst_resm(TINFO,"%s : ioctl failed [file %s, line %d]", __FUNCTION__, __FILE__, __LINE__); return -1;} }


/*==================================================================================================
                                              LOCAL VARIABLES
 ==================================================================================================*/

static int dmafd = -1;


/*==================================================================================================
                                              FUNCTIONS
 ==================================================================================================*/

int Test_1D_to_1D    (void);
int Test_2D_to_2D    (void);
int Test_1D_to_2D    (void);
int Test_2D_to_1D    (void);
int Test_By_Input    (void);
int Test_16_Channel  (void);
int Test_ChainBuffer (unsigned int);
int Test_Bus         (void);



/*================================================================================================*/
/*================================================================================================*/
int Test_1D_to_1D(void)
{
        int iCount = 0x1000;
        int iBurstLength =4;
        unsigned long arg;
        int fd = dmafd;
        
        /*make sure that count set before the source mode*/
        arg = 0;
        CALL_IOCTL(ioctl(fd,DMA_IOC_SET_SOURCE_ADDR,arg));
        CALL_IOCTL(ioctl(fd,DMA_IOC_SET_DEST_ADDR,arg));
        
        arg = iCount;
        CALL_IOCTL(ioctl(fd,DMA_IOC_SET_COUNT,arg));
        
        arg = DMA_MEM_SIZE_32;
        CALL_IOCTL(ioctl(fd,DMA_IOC_SET_SOURCE_SIZE,arg));
        
        arg = DMA_MEM_SIZE_32;
        CALL_IOCTL(ioctl(fd,DMA_IOC_SET_DEST_SIZE,arg));
        
        arg = DMA_TYPE_LINEAR;
        CALL_IOCTL(ioctl(fd,DMA_IOC_SET_SOURCE_MODE,arg));
        
        arg = DMA_TYPE_LINEAR;
        CALL_IOCTL(ioctl(fd,DMA_IOC_SET_DEST_MODE,arg));
        
        
        /*when 8 bit transfer , burst length is needed*/
        arg = iBurstLength;
        CALL_IOCTL(ioctl(fd,DMA_IOC_SET_BURST_LENGTH,arg));
        
        arg = ADDR_INC;
        CALL_IOCTL(ioctl(fd,DMA_IOC_SET_MEMORY_DIRECTION,arg));
        tst_resm(TINFO, "begin set config");	
        CALL_IOCTL(ioctl(fd,DMA_IOC_SET_CONFIG,NULL));
        tst_resm(TINFO, "Set 1D to 1D para");
        
        /*Begin DMA*/
        read(fd,NULL,0);
        
        tst_resm(TINFO, "get verify.");
        CALL_IOCTL(ioctl(fd,DMA_IOC_SET_VERIFY,NULL));
        
        return 0;
}


/*================================================================================================*/
/*================================================================================================*/
int Test_2D_to_2D(void)
{
        unsigned long arg;
        int ix = 0x40;
        int iy = 0x10;
        int iw = 0x80;
        int iCount = ix*iy;
        int iBurstLength = DMA_BURST_LENGTH_64;
        int fd = dmafd;
        
        /*make sure that count set before the source mode and after x/y/w*/
        arg = 0;
        CALL_IOCTL(ioctl(fd,DMA_IOC_SET_SOURCE_ADDR,arg));
        CALL_IOCTL(ioctl(fd,DMA_IOC_SET_DEST_ADDR,arg));
        
        arg = ix;
        CALL_IOCTL(ioctl(fd,DMA_IOC_SET_2D_REG_X_SIZE,arg));
        
        arg = iy;
        CALL_IOCTL(ioctl(fd,DMA_IOC_SET_2D_REG_Y_SIZE,arg));
        
        arg = iw;
        CALL_IOCTL(ioctl(fd,DMA_IOC_SET_2D_REG_W_SIZE,arg));
        
        arg = iCount;
        CALL_IOCTL(ioctl(fd,DMA_IOC_SET_COUNT,arg));
        
        arg = DMA_MEM_SIZE_32;
        CALL_IOCTL(ioctl(fd,DMA_IOC_SET_SOURCE_SIZE,arg));
        
        arg = DMA_MEM_SIZE_32;
        CALL_IOCTL(ioctl(fd,DMA_IOC_SET_DEST_SIZE,arg));
        
        arg = DMA_TYPE_2D;
        CALL_IOCTL(ioctl(fd,DMA_IOC_SET_SOURCE_MODE,arg));
        
        arg = DMA_TYPE_2D;
        CALL_IOCTL(ioctl(fd,DMA_IOC_SET_DEST_MODE,arg));
        
        arg = iBurstLength;
        CALL_IOCTL(ioctl(fd,DMA_IOC_SET_BURST_LENGTH,arg));
        
        arg = ADDR_INC;
        CALL_IOCTL(ioctl(fd,DMA_IOC_SET_MEMORY_DIRECTION,arg));
        
        CALL_IOCTL(ioctl(fd,DMA_IOC_SET_CONFIG,NULL));
        tst_resm(TINFO, "Set 2D to 2D param");
        
        /*Begin DMA*/
        read(fd,NULL,0);
        tst_resm(TINFO, "get verify");
        CALL_IOCTL(ioctl(fd,DMA_IOC_SET_VERIFY,NULL));        
        return 0;
}


/*================================================================================================*/
/*================================================================================================*/
int Test_1D_to_2D(void)
{
        unsigned long arg;
        int ix = 0x80;
        int iy = 0x10;
        int iw = 0x100;
        int iCount = ix*iy;
        int iBurstLength = 4;
        int fd = dmafd;
        
        /*make sure that count set before the source mode and after x/y/w*/
        arg = 0;
        CALL_IOCTL(ioctl(fd,DMA_IOC_SET_SOURCE_ADDR,arg));
        CALL_IOCTL(ioctl(fd,DMA_IOC_SET_DEST_ADDR,arg));
        
        arg = ix;
        CALL_IOCTL(ioctl(fd,DMA_IOC_SET_2D_REG_X_SIZE,arg));
        
        arg = iy;
        CALL_IOCTL(ioctl(fd,DMA_IOC_SET_2D_REG_Y_SIZE,arg));
        
        arg = iw;
        CALL_IOCTL(ioctl(fd,DMA_IOC_SET_2D_REG_W_SIZE,arg));
        
        arg = iCount;
        CALL_IOCTL(ioctl(fd,DMA_IOC_SET_COUNT,arg));
        
        arg = DMA_MEM_SIZE_32;
        CALL_IOCTL(ioctl(fd,DMA_IOC_SET_SOURCE_SIZE,arg));
        
        arg = DMA_MEM_SIZE_32;
        CALL_IOCTL(ioctl(fd,DMA_IOC_SET_DEST_SIZE,arg));
        
        arg = DMA_TYPE_LINEAR;
        CALL_IOCTL(ioctl(fd,DMA_IOC_SET_SOURCE_MODE,arg));
        
        arg = DMA_TYPE_2D;
        CALL_IOCTL(ioctl(fd,DMA_IOC_SET_DEST_MODE,arg));
        
        arg = iBurstLength;
        CALL_IOCTL(ioctl(fd,DMA_IOC_SET_BURST_LENGTH,arg));
        
        arg = ADDR_DEC;
        CALL_IOCTL(ioctl(fd,DMA_IOC_SET_MEMORY_DIRECTION,arg));
        
        CALL_IOCTL(ioctl(fd,DMA_IOC_SET_CONFIG,NULL));
        tst_resm(TINFO,"Set 1D to 2D para");
        
        /*Begin DMA*/
        read(fd,NULL,0);
        tst_resm(TINFO,"get verify");
        CALL_IOCTL(ioctl(fd,DMA_IOC_SET_VERIFY,NULL));        
        return 0;
}


/*================================================================================================*/
/*================================================================================================*/
int Test_2D_to_1D(void)
{
        unsigned long arg;
        int ix = 0x100;
        int iy = 0x10;
        int iw = 0x100;
        int iCount = ix*iy;
        int iBurstLength = 4;
        int fd = dmafd;        
        /*make sure that count set before the source mode and after x/y/w*/
        arg = 0;
        CALL_IOCTL(ioctl(fd,DMA_IOC_SET_SOURCE_ADDR,arg));
        CALL_IOCTL(ioctl(fd,DMA_IOC_SET_DEST_ADDR,arg));
        
        arg = ix;
        CALL_IOCTL(ioctl(fd,DMA_IOC_SET_2D_REG_X_SIZE,arg));
        
        arg = iy;
        CALL_IOCTL(ioctl(fd,DMA_IOC_SET_2D_REG_Y_SIZE,arg));
        
        arg = iw;
        CALL_IOCTL(ioctl(fd,DMA_IOC_SET_2D_REG_W_SIZE,arg));
        
        arg = iCount;
        CALL_IOCTL(ioctl(fd,DMA_IOC_SET_COUNT,arg));
        
        arg = DMA_MEM_SIZE_32;
        CALL_IOCTL(ioctl(fd,DMA_IOC_SET_SOURCE_SIZE,arg));
        
        arg = DMA_MEM_SIZE_32;
        CALL_IOCTL(ioctl(fd,DMA_IOC_SET_DEST_SIZE,arg));
        
        arg = DMA_TYPE_2D;
        CALL_IOCTL(ioctl(fd,DMA_IOC_SET_SOURCE_MODE,arg));
        
        arg = DMA_TYPE_LINEAR;
        CALL_IOCTL(ioctl(fd,DMA_IOC_SET_DEST_MODE,arg));
        
        arg = iBurstLength;
        CALL_IOCTL(ioctl(fd,DMA_IOC_SET_BURST_LENGTH,arg));
        
        arg = ADDR_INC;
        CALL_IOCTL(ioctl(fd,DMA_IOC_SET_MEMORY_DIRECTION,arg));
        
        CALL_IOCTL(ioctl(fd,DMA_IOC_SET_CONFIG,NULL));
        tst_resm(TINFO,"Set 2D to 1D para");
        
        /*Begin DMA*/
        read(fd,NULL,0);
        
        tst_resm(TINFO,"get verify");
        CALL_IOCTL(ioctl(fd,DMA_IOC_SET_VERIFY,NULL));        
        return 0;
}


/*================================================================================================*/
/*================================================================================================*/
int Test_By_Input(void)
{
        unsigned long arg;
        unsigned long ulSourceMode,ulDestMode;
        unsigned long ulSourcePort, ulDestPort;
        unsigned long X, Y, W;
        unsigned long ulCount;
        unsigned long ulDirection, ulBurstLength;
        unsigned long ulRepeat;
        
        char str[16];
        unsigned long params[11];
        int i;
        const char * src = gTestappConfig.mCustumParams;
        for(i = 0; i < 11; ++i)
        {
                sscanf(src, "%s", str); 
                params[i] = atoi(src); 
                src += (strlen(src)+1);
        }
        
        
        int fd = dmafd;
        ulSourceMode = params[0];
        ulDestMode = params[1];
        ulSourcePort = params[2];
        ulDestPort = params[3];
        X = params[4];
        Y = params[5];
        W = params[6];
        ulCount = params[7];
        ulDirection = params[8];
        ulBurstLength = params[9];
        ulRepeat = params[10];
        
        /*make sure the rationality of the para*/
        if(X>W)
        {
                tst_resm(TWARN, "W should be no less than X");          
                return -1;
        }
        if(ulSourceMode>3||ulDestMode>3)
        {
                tst_resm(TWARN,"SourceMode and DestMode should be no greater than 3");      
                return -1;
        }
        if(ulSourcePort>2||ulDestPort>2)
        {
                tst_resm(TWARN,"SourcePort and DestPort should be no greater than 2");      
                return -1;
        }
        if(ulBurstLength>63)
        {
                tst_resm(TWARN,"BurstLength should be no greater than 64");      
                return -1;
        }
        if((ulDirection>1)|(ulRepeat>1))
        {
                tst_resm(TWARN,"Direction and Repeat should be no greater than 1");      
                return -1;
        }
        
        if(ulSourceMode==DMA_TYPE_2D||ulDestMode==DMA_TYPE_2D)
                ulCount = X*Y;
        if(ulCount>0x1000)
        {
                tst_resm(TWARN,"Count should be less than one page size");      
                return -1;
        }
        
        /*make sure the rationality of the para*/
        arg = 0;
        CALL_IOCTL(ioctl(fd,DMA_IOC_SET_SOURCE_ADDR,arg));
        CALL_IOCTL(ioctl(fd,DMA_IOC_SET_DEST_ADDR,arg));
        
        arg = X;
        CALL_IOCTL(ioctl(fd,DMA_IOC_SET_2D_REG_X_SIZE,arg));
        
        arg = Y;
        CALL_IOCTL(ioctl(fd,DMA_IOC_SET_2D_REG_Y_SIZE,arg));
        
        arg = W;
        CALL_IOCTL(ioctl(fd,DMA_IOC_SET_2D_REG_W_SIZE,arg));
        
        arg = ulCount;
        CALL_IOCTL(ioctl(fd,DMA_IOC_SET_COUNT,arg));
        
        arg = ulSourcePort;
        CALL_IOCTL(ioctl(fd,DMA_IOC_SET_SOURCE_SIZE,arg));
        
        arg = ulDestPort;
        CALL_IOCTL(ioctl(fd,DMA_IOC_SET_DEST_SIZE,arg));
        
        /*set direction before the sourc emode*/
        arg = ulDirection;
        CALL_IOCTL(ioctl(fd,DMA_IOC_SET_MEMORY_DIRECTION,arg));
        
        arg = ulSourceMode;
        CALL_IOCTL(ioctl(fd,DMA_IOC_SET_SOURCE_MODE,arg));
        
        arg = ulDestMode;
        CALL_IOCTL(ioctl(fd,DMA_IOC_SET_DEST_MODE,arg));
        
        arg = ulBurstLength;
        CALL_IOCTL(ioctl(fd,DMA_IOC_SET_BURST_LENGTH,arg));
        
        CALL_IOCTL(ioctl(fd,DMA_IOC_SET_CONFIG,NULL));
        tst_resm(TINFO, "Set by self para");
        
        /*Begin DMA*/
        read(fd,NULL,0);
        
        tst_resm(TINFO, "get verify");
        CALL_IOCTL(ioctl(fd,DMA_IOC_SET_VERIFY,NULL));        
        return 0;
}


/*================================================================================================*/
/*================================================================================================*/
int Test_ChainBuffer(unsigned int iTransTime)
{
        unsigned long arg;
        int fd = dmafd;
        if(iTransTime>8 || iTransTime < 3)
        {
                tst_resm(TWARN, "Error transtime, it may between 3 to 8");
                return -1;
        }        
        /*this CALL_IOCTL start testing the chainbuffer*/
        arg = iTransTime;
        CALL_IOCTL(ioctl(fd,DMA_IOC_TEST_CHAINBUFFER,arg));	        
        return 0;
}


/*================================================================================================*/
/*================================================================================================*/
int Test_Bus(void)
{
        int iCount = 0x1000;
        int iBurstLength =4;
        unsigned long arg;
        int fd = dmafd;        
        /*make sure that count set before the source mode*/
        int i;
        for(i=0;i<5;i++)
        {
                arg = 0;
                CALL_IOCTL(ioctl(fd,DMA_IOC_SET_SOURCE_ADDR,arg));
                CALL_IOCTL(ioctl(fd,DMA_IOC_SET_DEST_ADDR,arg));
                
                arg = iCount;
                CALL_IOCTL(ioctl(fd,DMA_IOC_SET_COUNT,arg));
                
                arg = DMA_MEM_SIZE_32;
                CALL_IOCTL(ioctl(fd,DMA_IOC_SET_SOURCE_SIZE,arg));
                
                arg = DMA_MEM_SIZE_32;
                CALL_IOCTL(ioctl(fd,DMA_IOC_SET_DEST_SIZE,arg));
                
                arg = DMA_TYPE_LINEAR;
                CALL_IOCTL(ioctl(fd,DMA_IOC_SET_SOURCE_MODE,arg));
                
                arg = DMA_TYPE_LINEAR;
                CALL_IOCTL(ioctl(fd,DMA_IOC_SET_DEST_MODE,arg));
                
                
                /*when 8 bit transfer , burstlength is needed*/
                arg = iBurstLength;
                CALL_IOCTL(ioctl(fd,DMA_IOC_SET_BURST_LENGTH,arg));
                
                arg = ADDR_INC;
                CALL_IOCTL(ioctl(fd,DMA_IOC_SET_MEMORY_DIRECTION,arg));
                
                CALL_IOCTL(ioctl(fd,DMA_IOC_SET_CONFIG,NULL));
                tst_resm(TINFO, "Set 1D to 1D para");
                
                /*Begin DMA*/
                read(fd,NULL,0);
                
                tst_resm(TINFO, "get verify");
                CALL_IOCTL(ioctl(fd,DMA_IOC_SET_VERIFY,NULL));
                sleep(2);
        }        
        return 0;
}


/*================================================================================================*/
/*================================================================================================*/
int Test_16_Channel(void)
{
        int iCount = 0x1000;
        int iBurstLength =4;
        unsigned long arg;
        int fd[16];
        int i;        

#define CALL_IOCTL_CLEANUP(ioctl_) {if((ioctl_) < 0) {tst_resm(TINFO,"%s : ioctl failed [file %s, line %d]", __FUNCTION__, __FILE__, __LINE__); {for(i=0;i<16;i++){close(fd[i]); return -1;}}} }

        for(i=0;i<16;i++)
        {      
                fd[i] = open(DMA_NAME, O_RDWR);
                if(fd[i] == 0)
                {
                        tst_resm(TWARN, "can not open file dma %d",i);
                        return -1;
                }
                
                arg = 0;
                CALL_IOCTL_CLEANUP(ioctl(fd[i],DMA_IOC_SET_SOURCE_ADDR,arg));                        
                CALL_IOCTL_CLEANUP(ioctl(fd[i],DMA_IOC_SET_DEST_ADDR,arg));
                
                arg = iCount;
                CALL_IOCTL_CLEANUP(ioctl(fd[i],DMA_IOC_SET_COUNT,arg));
                
                arg = DMA_MEM_SIZE_32;
                CALL_IOCTL_CLEANUP(ioctl(fd[i],DMA_IOC_SET_SOURCE_SIZE,arg));
                
                arg = DMA_MEM_SIZE_32;
                CALL_IOCTL_CLEANUP(ioctl(fd[i],DMA_IOC_SET_DEST_SIZE,arg));
                
                arg = DMA_TYPE_LINEAR;
                CALL_IOCTL_CLEANUP(ioctl(fd[i],DMA_IOC_SET_SOURCE_MODE,arg));
                
                arg = DMA_TYPE_LINEAR;
                CALL_IOCTL_CLEANUP(ioctl(fd[i],DMA_IOC_SET_DEST_MODE,arg));
                
                
                /*when 8 bit transfer , burstlength is needed*/
                arg = iBurstLength;
                CALL_IOCTL_CLEANUP(ioctl(fd[i],DMA_IOC_SET_BURST_LENGTH,arg));
                
                arg = ADDR_INC;
                CALL_IOCTL_CLEANUP(ioctl(fd[i],DMA_IOC_SET_MEMORY_DIRECTION,arg));
                
                CALL_IOCTL_CLEANUP(ioctl(fd[i],DMA_IOC_SET_CONFIG,NULL));
                tst_resm(TINFO, "Set 1D to 1D para");
                
        }            
        tst_resm(TINFO, "Set 16 Channel para");
        for(i=0;i<16;i++)
        {  
                /*Begin DMA*/
                read(fd[i],NULL,0);
                tst_resm(TINFO,"get Buffer %d verify",i);
                CALL_IOCTL_CLEANUP(ioctl(fd[i],DMA_IOC_SET_VERIFY,NULL));
                close(fd[i]);                
        }
        
        return 0;
}


/*================================================================================================*/
/*================================================================================================*/
int VT_dma_setup(void)
{
        dmafd = open(DMA_NAME, O_RDWR);
        if(dmafd <= 0)
        {
                tst_resm(TWARN, "cant open %s", DMA_NAME);
                return TFAIL;
        }
        return TPASS;
}


/*================================================================================================*/
/*================================================================================================*/
int VT_dma_cleanup(void)
{
        if(dmafd)
        {
                close(dmafd);
                dmafd = -1;
        }
        return TPASS;
}


/*================================================================================================*/
/*================================================================================================*/
int VT_dma_test(void)
{
        int ret = -1;
        switch(gTestappConfig.mTestCase)
        {
        case 0:
                tst_resm(TINFO, "Testing 1D to 1D");
                ret = Test_1D_to_1D();
                break;
        case 1:
                tst_resm(TINFO, "Testing 2D to 2D");
                ret = Test_2D_to_2D();
                break;
        case 2: 
                tst_resm(TINFO, "Testing 1D to 2D");
                ret = Test_1D_to_2D();
                break;
        case 3:
                tst_resm(TINFO, "Testing 2D to 1D");
                ret = Test_2D_to_1D();
                break;
        case 4: 
                tst_resm(TINFO, "Testing user params");
                ret = Test_By_Input();
                break;
        case 5:
                tst_resm(TINFO, "Testing 16 channel");
                ret = Test_16_Channel();
                break;
        case 6:
                tst_resm(TINFO, "Testing bus");
                Test_Bus();
                break;
        default:
                tst_resm(TWARN, "WRONG TEST CASE");
        }
        return ret;
}

