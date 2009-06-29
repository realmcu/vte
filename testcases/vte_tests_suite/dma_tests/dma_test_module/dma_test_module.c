/***
**Copyright 2005-2009 Freescale Semiconductor, Inc. All Rights Reserved.
**
**The code contained herein is licensed under the GNU General Public
**License. You may obtain a copy of the GNU General Public License
**Version 2 or later at the following locations:
**
**http://www.opensource.org/licenses/gpl-license.html
**http://www.gnu.org/copyleft/gpl.html
**/
/*================================================================================================*/
/**
        @file   dma_test_module.c

        @brief  DMA test module C-file
====================================================================================================
Revision History:
                            Modification     Tracking
Author/core ID                  Date          Number    Description of Changes
-------------------------   ------------    ----------  -------------------------------------------
D.Simakov / B00236           08/30/2006     TLSbo76801  Initial version
====================================================================================================
Portability:  ARM GCC
==================================================================================================*/

#include <linux/module.h>
#include <linux/slab.h>
#include <linux/sched.h>
#include <linux/mman.h>
#include <linux/init.h>
#include <linux/spinlock.h>
#include <linux/interrupt.h>	
#include <linux/device.h>      /* Added on 05/01/06 by Bunloeur Sean */
#include <linux/devfs_fs_kernel.h>

#include <asm/dma.h>
#include <asm/mach/dma.h>
#include <asm/delay.h>
#include <asm/scatterlist.h>

#include <linux/device.h>
#include <asm/arch/dma.h>

#include <asm/arch/hardware.h>
#include "dma_test_module.h"

#ifdef DEBUG_TEST
#define DMA_TRACE(msg,...) do {printk( "--- DMA ---> " printk(msg,##__VA_ARGS__) )} while(0)
#else
#define DMA_TRACE(msg,...) do {} while(0)
#endif

#define DMA_IRQ_BASE 32
#define DMA_TRANS_SIZE  0x80
#define DMA_INTERVAL_MEM 0x2000
#define DMA_SAND 13
#define DMA_NAME "mxc_dma"

dmach_t get_channel (pid_t pid, struct file *filp);
int     dma_open    (struct inode * inode, struct file * filp);
ssize_t dma_read    (struct file * filp, char __user  * buf, size_t count, loff_t * offset);
int     dma_release (struct inode * inode, struct file * filp);
ssize_t dma_write   (struct file * filp, const char __user  * buf, size_t count, loff_t * offset);
int     dma_ioctl   (struct inode * inode, struct file *filp, unsigned int cmd, unsigned long arg);

static dma_ownership_t       dma_ownership[MXC_DMA_CHANNELS]={{0,0}};
static int                   gMajor; 
static struct class_simple * dma_tm_class;
wait_queue_head_t            q;
static char                * g_pDmaSource_Ori;
static char                * g_pDmaDest_Ori;
static char                * g_pDmaSource;
static char                * g_pDmaDest;
static dma_request_t         g_DmaReq_t;
static dma_channel_params    g_DmaPara_t;
static int                   g_sand = 100;
struct scatterlist         * sg;
extern dma_t               * dma_chan;
static int                   g_channel;
static int                   g_DmaIntCount = 0;
static int                   g_DmaTranferTime = 0;
static volatile int          g_DmaWait = 0;

struct file_operations dma_fops = 
{
        open:           dma_open,
        release:        dma_release,
        read:           dma_read,
        write:          dma_write,
        ioctl:          dma_ioctl,
};


/*================================================================================================*/
/*================================================================================================*/
int pid_is_valid(pid_t pid)
{
        return 1;
}


/*================================================================================================*/
/*================================================================================================*/
void set_channel_ownership(dmach_t channel, pid_t pid, struct file *filp)
{
        if ( channel > MXC_DMA_CHANNELS)
        {
                DMA_TRACE("dma: channel number beyond capability.\n");
                return;
        }
        
        if ( pid_is_valid(pid) )
        {
                dma_ownership[channel].pid = pid;
                dma_ownership[channel].filp= filp;
        }
        return;
}


/*================================================================================================*/
/*================================================================================================*/
static void sand(int rand)
{
        g_sand=rand;
        return;
}


/*================================================================================================*/
/*================================================================================================*/
static char rand(void)
{
        g_sand=g_sand*12357/1103;
        return g_sand;
}


/*================================================================================================*/
/*================================================================================================*/
void FillMem(char * p, int size, int rd)
{
        int i=0;
        DMA_TRACE("Fill 1d mem %8x\n",(unsigned int)p);
        
        sand(rd);
        for(i=0;i<size;i++)
                *p++=rand();        
        return;
}


/*================================================================================================*/
/*================================================================================================*/
int Verify(char *p, int size,int rd)
{
        int i=0,j=0;
        char x;        
        sand(rd);
        for(i=0;i<size;i++)
        {
                x=rand();
                if(i==10)
                        DMA_TRACE("Verify 1d mem %8x,x=%d,p[10]=%d\n",(unsigned int)p,(unsigned int)x,p[10]);
                if(x!=p[i])
                {
                        j++;
                        if(j<30)
                                DMA_TRACE("i = %d, Exp %d:%d\n",i,x,p[i]);                        
                        else
                                return 1;		
                }	
        }
        
        if(j>0)
                return 1;
        else 
                return 0;	
}


/*================================================================================================*/
/*================================================================================================*/
void FillMem_2D(char * p, int x,int y,int w, int rd,int dir)
{
        int i,j;
        int DecOffset=0;
        if(dir)
                DecOffset=w-x;
        DMA_TRACE("Fill 2d mem %8x\n",(unsigned int)p);
        sand(rd);
        for(j=0;j<y;j++) 
        {
                for(i=0;i<x;i++)
                {
                        *(p+j*w+i+DecOffset)=rand();
                }                
        }
        return;
}


/*================================================================================================*/
/*================================================================================================*/
int Verify_2D(char * p, int x,int y,int w, int rd,int dir)
{
        int i=0, j=0;
        int iCountError=0;
        char temp;
        int DecOffset=0;
        if(dir)
                DecOffset=w-x;        
        sand(rd);
        for(j=0;j<y;j++)
        {
                for(i=0;i<x;i++)
                {                        
                        temp=rand();
                        if(i==1&&j==2)
                                DMA_TRACE("Verify 2d mem %8x,x=%d,p[%d*%d+%d+%d]=%d\n",(unsigned int)p,temp,j,w,i,DecOffset,p[j*w+i+DecOffset]);
                        
                        if(temp!=p[j*w+i+DecOffset])
                        {
                                iCountError++;
                                if(iCountError<30)
                                {
                                        DMA_TRACE("i = %d; j =%d ; Exp %d:%d\n",i,j,temp,p[j*w+i+DecOffset]);				
                                }
                                else
                                        return 1;		
                        }	
                }
        }
        if(iCountError>0)
                return 1;
        else 
                return 0;	
}


/*================================================================================================*/
/*================================================================================================*/
int __init dma_init_module(void)
{
        struct class_device *temp_class;
        int error;
        int i;
        
        DMA_TRACE("register virtual dma driver.\n");
       
        /* register a character device */
        error = register_chrdev(0, DMA_NAME, &dma_fops);
        if (error < 0) 
        {
                DMA_TRACE("dma driver can't get major number\n");
                return error;
        }
        gMajor = error;
        DMA_TRACE("dma major number = %d\n",gMajor);
        
        dma_tm_class = class_simple_create(THIS_MODULE, DMA_NAME);
        if (IS_ERR(dma_tm_class)) 
        {
                DMA_TRACE(KERN_ERR "Error creating dma test module class.\n");
                unregister_chrdev(gMajor, DMA_NAME);
                class_simple_device_remove(MKDEV(gMajor, 0));
                return PTR_ERR(dma_tm_class);
        }
        
        temp_class = class_simple_device_add(dma_tm_class, MKDEV(gMajor, 0), NULL, DMA_NAME);
        if (IS_ERR(temp_class)) 
        {
                printk(KERN_ERR "Error creating dma test module class device.\n");
                class_simple_device_remove(MKDEV(gMajor, 0));
                class_simple_destroy(dma_tm_class);
                unregister_chrdev(gMajor, DMA_NAME);
                return -1;
        }
        
        DMA_TRACE("creating devfs entry for dma \n");
        devfs_mk_cdev(MKDEV(gMajor, 0), S_IFCHR | S_IRUGO | S_IWUSR, DMA_NAME);

        for(i=0;i<MXC_DMA_CHANNELS;i++)
        {
                dma_ownership[i].pid=0;
                dma_ownership[i].filp=0;
        }
        
        init_waitqueue_head(&q);
        return 0;
}


/*================================================================================================*/
/*================================================================================================*/
irqreturn_t Dma_Finish(int irq, void *dev_id, struct pt_regs *dma_channel)
{
        int channel;
        dma_t dma;
        channel = irq - 32;	
        dma = dma_chan[channel];
        g_DmaIntCount ++;
        if (!dma.using_sg)
        {
                if (dma.state == DMA_DONE)	    
                {
                        g_DmaWait = 0;
                }
        }	  
        // few delays and no DMA_TRACE are allowed for chainbuffer operation
        // 5 udelay is the maximum as has been tested, ???
        // udelay(5);  
        return IRQ_HANDLED;
}


/*================================================================================================*/
/*================================================================================================*/
int Dma_SetConfig(struct file *filp)
{
        dmach_t channel;
        unsigned int SourceDecOffset,DestDecOffset;
        dma_request_t dma ;
        dma_channel_params dma_param;  
        channel = get_channel((pid_t)current,filp);
        if(channel == -1)
        {
                DMA_TRACE("no dma channel is allocated to the process 0x%x.\n", (unsigned int)current);
                return -EINVAL;
        }
        /**************Begin of Set dma para******************************/
        DMA_TRACE("Dma_SetConfig.\n");
        
        memset(&dma,0,sizeof(dma_request_t));
        
        
        dma.count = g_DmaReq_t.count;
        
        dma.sourceAddr = (__u8*)virt_to_phys(dma_ownership[channel].memory_source);
        dma.destAddr = (__u8*)virt_to_phys(dma_ownership[channel].memory_dest);
        
        memset(&dma_param,0,sizeof(dma_channel_params));
        
        dma_param.sourcePort = g_DmaPara_t.sourcePort;
        dma_param.destPort = g_DmaPara_t.destPort;
        dma_param.sourceType = g_DmaPara_t.sourceType;
        dma_param.destType = g_DmaPara_t.destType;
        dma_param.burstLength = g_DmaPara_t.burstLength;
        dma_param.repeat = g_DmaPara_t.repeat;
        dma_param.dir = g_DmaPara_t.dir;
        
        dma_param.X = g_DmaPara_t.X;
        dma_param.Y = g_DmaPara_t.Y;
        dma_param.W = g_DmaPara_t.W;
        
        
        if(dma_param.dir)
        {
                if(dma_param.sourceType==DMA_TYPE_2D)
                        SourceDecOffset = dma_param.W*dma_param.Y;
                else 
                        SourceDecOffset = dma.count;
                if(dma_param.destType==DMA_TYPE_2D)
                        DestDecOffset= dma_param.W*dma_param.Y;
                else
                        DestDecOffset= dma.count;
                dma.sourceAddr =  (__u8*)virt_to_phys(dma_ownership[channel].memory_source+SourceDecOffset);
                dma.destAddr = (__u8*)virt_to_phys(dma_ownership[channel].memory_dest+DestDecOffset);
        }
        DMA_TRACE("Source Mode %8x \n",dma_param.sourceType);
        DMA_TRACE("Dest Mode %8x  \n",dma_param.destType);
        
        DMA_TRACE("Source Port %8x\n",dma_param.sourcePort);
        DMA_TRACE("Dest Port %8x \n",dma_param.destPort);
        
        DMA_TRACE("Count %8x \n",dma.count);
        
        DMA_TRACE("Source addr %8x \n",(unsigned int)dma.sourceAddr);
        DMA_TRACE("Dest addr %8x \n",(unsigned int)dma.destAddr);
        
        DMA_TRACE("Burst length %8x \n",dma_param.burstLength);
        DMA_TRACE("Repeat %8x \n",dma_param.repeat);
        DMA_TRACE("Mem direction %8x \n",dma_param.dir);
        DMA_TRACE("x %8x \n",dma_param.X);
        DMA_TRACE("w %8x \n",dma_param.W);
        DMA_TRACE("y %8x \n",dma_param.Y);
        
        
        mxc_dma_setup_channel(channel,&dma_param);
        mxc_dma_set_config(channel,&dma,0);
        DMA_TRACE("mxc_dma_set_config/channel param \n");
        
        /**************End of Set dma para******************************/
        
        return 0;
}


/*================================================================================================*/
/*================================================================================================*/
void dma_build_sglist(unsigned int iTransTime)
{
        int i;
        sg =(struct scatterlist *)kmalloc(iTransTime*sizeof(struct scatterlist ), GFP_KERNEL); 
        
        for(i=0;i<iTransTime;i++)	
        {
                sg[i].__address = (char*)(g_pDmaDest_Ori+DMA_INTERVAL_MEM*i);
                sg[i].dma_address = (dma_addr_t)virt_to_phys((char*)(g_pDmaDest_Ori+DMA_INTERVAL_MEM*i));
                sg[i].length = DMA_TRANS_SIZE;                
        }	                
}

/*================================================================================================*/
/*This function test linear to linear increase Chainbuffer dma transfer*/
/*================================================================================================*/
int Dma_TestChainBuffer(struct file *filp,unsigned int iTransTime)
{
        int i;
        dmach_t channel;
        dma_channel_params dma ;
        dma_request_t dma_req ;
        if(iTransTime>8)
        {
                DMA_TRACE("Too large transtime.\n");
                return -EINVAL;
        }
        channel = get_channel((pid_t)current,filp);
        if(channel == -1)
        {
                DMA_TRACE("no dma channel is allocated to the process 0x%x.\n", (unsigned int)current);
                return -EINVAL;
        }
        dma_build_sglist(iTransTime);
        
        /*initialize the source and dest memory*/
        for(i=0;i<iTransTime;i++)	
        {
                g_pDmaSource = (char*)(g_pDmaSource_Ori+DMA_INTERVAL_MEM*i);
                
                /*Only test linear memthod, so fill source with this function*/
                FillMem(g_pDmaSource,DMA_TRANS_SIZE,DMA_SAND+i);
                g_pDmaDest = (char*)(g_pDmaDest_Ori+DMA_INTERVAL_MEM*i);
                memset(g_pDmaDest,0,DMA_TRANS_SIZE);
        }
        
        /**************Begin of Set dma para******************************/
        memset(&dma,0,sizeof(dma_channel_params));
        memset(&dma_req,0,sizeof(dma_request_t));
        
        g_pDmaSource = (char*)(g_pDmaSource_Ori);
        g_pDmaDest = (char*)(g_pDmaDest_Ori);
        
        dma_req.sourceAddr = (__u8*)virt_to_phys(g_pDmaSource);
        dma_req.destAddr = (__u8*)virt_to_phys(g_pDmaDest);
        dma_req.count = DMA_TRANS_SIZE;	   
        dma.sourcePort = DMA_MEM_SIZE_32;
        dma.destPort = DMA_MEM_SIZE_32;
        dma.sourceType = DMA_TYPE_LINEAR;
        dma.destType = DMA_TYPE_LINEAR;
        dma.burstLength = 4;
        dma.repeat = 1;
        dma.AcRpt = 1;
        dma.dir = 0;
        dma.X = 0;
        dma.Y = 0;
        dma.W = 0;
        /*Set to support 2d memory*/
        
        
        DMA_TRACE("Source Mode %8x \n",dma.sourceType);
        DMA_TRACE("Dest Mode %8x  \n",dma.destType);
        
        DMA_TRACE("Source Port %8x\n",dma.sourcePort);
        DMA_TRACE("Dest Port %8x \n",dma.destPort);
        
        DMA_TRACE("Count %8x \n",dma_req.count);
        
        DMA_TRACE("Source addr %8x \n",(unsigned int)dma_req.sourceAddr);
        DMA_TRACE("Dest addr %8x \n",(unsigned int)dma_req.destAddr);
        
        DMA_TRACE("Burst length %8x \n",dma.burstLength);
        DMA_TRACE("Repeat %8x \n",dma.repeat);
        DMA_TRACE("Mem direction %8x \n",dma.dir);
        DMA_TRACE("x %8x \n",dma.X);
        DMA_TRACE("w %8x \n",dma.W);
        DMA_TRACE("y %8x \n",dma.Y);
        
        mxc_dma_setup_channel(channel, &dma);
        mxc_dma_set_config(channel,&dma_req, 0);
        
        /*set acrpt*/
        set_dma_sg(channel,sg, iTransTime);
        set_dma_mode(channel,DMA_MODE_READ );	   
        DMA_TRACE("Set chainbuffer 0 config\n");
        DMA_TRACE("Begin read.\n");
        g_DmaIntCount = 0;
        g_DmaWait = 1;
        enable_dma(channel);
        if (g_DmaWait)
        {
                do {;}while (g_DmaWait);
        }	
        DMA_TRACE("count is %d@\n", g_DmaIntCount);  
        DMA_TRACE("mode is %d@\n", dma_chan[g_channel].state);   
        for(i=0;i<iTransTime;i++)	
        {
                DMA_TRACE("Get chainbuffer %d verify\n",i);
                g_pDmaDest = (char*)(g_pDmaDest_Ori+DMA_INTERVAL_MEM*i);
                Verify(g_pDmaDest,DMA_TRANS_SIZE,DMA_SAND);
        }
        return 0;
}


/*================================================================================================*/
/*================================================================================================*/
int dma_open(struct inode * inode, struct file * filp)
{
        
        int error;
        int channel=-1;
        int i;
        DMA_TRACE("---- open a dma channel----,fd=%8x\n",(unsigned int)filp);
        
        //request dma channel.
        for (i = 0;i<MXC_DMA_CHANNELS;i++)
        {
                error = -1;
                error = request_dma(i, "dma_test");
                if (error)
                {
                        continue;
                }
                else
                {
                        channel = i;	  
                        g_channel = i;
                        break;
                }
                
        }
        if (i== MXC_DMA_CHANNELS)
        {
                DMA_TRACE("no channel is available.\n");
                return -1;
        }
        /****************init dma mem para.***************/
        DMA_TRACE("begin request mem.\n");
        g_pDmaSource_Ori=(char*)__get_free_pages(GFP_KERNEL | GFP_DMA, 4);
        DMA_TRACE("End request mem.\n");
        if(g_pDmaSource_Ori==NULL)
        {
                DMA_TRACE("No Memory\n");
                return error;
        }
        g_pDmaDest_Ori=g_pDmaSource_Ori+DMA_INTERVAL_MEM;
        memset(&g_DmaPara_t,0,sizeof(dma_request_t));
        /****************init dma mem para.***************/
        DMA_TRACE("channel %d is opened.\n",channel);
        set_channel_ownership(channel,(pid_t)current,filp);
        dma_ownership[channel].memory_oripage = g_pDmaSource_Ori;
        
        error = request_irq(DMA_IRQ_BASE+channel, Dma_Finish, SA_INTERRUPT|SA_SHIRQ,"dma_test", Dma_Finish);
        
        if (error)
        {
                DMA_TRACE(KERN_ERR
                        ": unable to request IRQ  for DMA channel\n");
                return -1;
        }
        
        return 0;
}


/*================================================================================================*/
/*================================================================================================*/
int dma_release(struct inode * inode, struct file * filp)
{
        dmach_t channel;
        
        channel = get_channel((pid_t)current,filp);
        if(channel == -1)
        {
                DMA_TRACE("no dma channel is allocated to the process 0x%x.\n", (unsigned int)current);
                return -1;
        }
        free_irq(DMA_IRQ_BASE+channel,Dma_Finish);
        __free_pages((struct page *)(dma_ownership[channel].memory_oripage),4);
        set_channel_ownership(channel,0,0);
        DMA_TRACE("begin dma free\n");
        free_dma(channel);
        DMA_TRACE("End dma free\n");
        return 0;
}


/*================================================================================================*/
/*================================================================================================*/
ssize_t dma_read (struct file *filp, char __user * buf, size_t count, loff_t * offset)
{
        dmach_t channel;        
        
        channel = get_channel((pid_t)current,filp);
        if(channel == -1)
        {
                DMA_TRACE("no dma channel is allocated to the process 0x%x.\n", (unsigned int)current);
                return -EINVAL;
        }
        
        /**************Start transfer          ******************************/
        DMA_TRACE("Begin read.\n");
        
        g_DmaIntCount = 0;
        g_DmaWait = 1;      
        
        enable_dma(channel);
        if (g_DmaWait)
        {
                do {;}while (g_DmaWait);
        }	
        mxc_dump_dma_register(channel);
        //interruptible_sleep_on_timeout(&q, 100);
        DMA_TRACE("count is %d@\n", g_DmaIntCount);   
        DMA_TRACE("mode is %d@\n", dma_chan[g_channel].state);   
        DMA_TRACE("transferred is %x\n",get_dma_residue(channel));
        DMA_TRACE("End read.\n");
        
        return 0;
}


/*================================================================================================*/
/*================================================================================================*/
ssize_t dma_write(struct file * filp, const char __user * buf, size_t count, loff_t * offset)
{
        dmach_t channel;
        
        channel = get_channel((pid_t)current,filp);
        if(channel == -1)
        {
                DMA_TRACE("no dma channel is allocated to the process 0x%x.\n", (unsigned int)current);
                return -EINVAL;
        }
        
        /**************Start transfer**************************/
        DMA_TRACE("Begin write.\n");
        g_DmaIntCount = 0;
        g_DmaWait = 1;
        enable_dma(channel);
        if (g_DmaWait)
        {
                do {;}while (g_DmaWait);
        }	
        mxc_dump_dma_register(channel);
        DMA_TRACE("count is %d@\n", g_DmaIntCount);   
        DMA_TRACE("mode is %d@\n", dma_chan[g_channel].state);   
        DMA_TRACE("transferred is %x\n",get_dma_residue(channel));
        DMA_TRACE("End write.\n");
        
        return 0;
}


/*================================================================================================*/
/*================================================================================================*/
int dma_ioctl(struct inode * inode, struct file *filp, unsigned int cmd, unsigned long arg)
{
        dmach_t channel;
        dma_regs_t *regs;
        int status = 0;
        dma_channel_params* p_dma;
        dma_request_t* p_dma_req;
        channel = get_channel((pid_t)current,filp);
        if(channel == -1)
        {
                DMA_TRACE("no dma channel is allocated to the process 0x%x.\n", (unsigned int)current);
                return -EINVAL;
        }
        p_dma = &g_DmaPara_t;
        p_dma_req = &g_DmaReq_t;
        switch(cmd)
        {
        case DMA_IOC_SET_SOURCE_ADDR:
                g_pDmaSource = (char*)(g_pDmaSource_Ori+DMA_INTERVAL_MEM*arg);
                dma_ownership[channel].memory_source = g_pDmaSource;
                break;
        case DMA_IOC_SET_DEST_ADDR:
                g_pDmaDest = (char*)(g_pDmaDest_Ori+DMA_INTERVAL_MEM*arg);
                memset(g_pDmaDest,0,DMA_TRANS_SIZE);
                dma_ownership[channel].memory_dest = g_pDmaDest;
                break;
        case DMA_IOC_SET_SOURCE_SIZE:
                p_dma->sourcePort=arg;
                break;
                
        case DMA_IOC_SET_DEST_SIZE:
                p_dma->destPort=arg;
                break;
                
        case DMA_IOC_SET_COUNT:
                p_dma_req->count=arg;
                break;
                
        case DMA_IOC_SET_BURST_LENGTH:
                p_dma->burstLength=arg;
                break;
        case DMA_IOC_SET_REPEAT:
                p_dma->repeat=arg;
                break;
        case DMA_IOC_TEST_CHAINBUFFER:
                g_DmaIntCount = 0;
                g_DmaTranferTime = arg;
                Dma_TestChainBuffer(filp,arg);
                break;
        case DMA_IOC_SET_ACRPT:
                regs = (dma_regs_t*)(IO_ADDRESS(DMA_CH_BASE(channel)));
                regs->Ctl|=DMA_CTL_ACRPT;
                break;
        case DMA_IOC_SET_SOURCE_MODE:
                p_dma->sourceType=arg;
                switch(arg)
                {
                case DMA_TYPE_LINEAR:
                        FillMem(dma_ownership[channel].memory_source,p_dma_req->count,DMA_SAND);
                        break;
                case DMA_TYPE_2D:
                        FillMem_2D(dma_ownership[channel].memory_source,p_dma->X,p_dma->Y,p_dma->W,DMA_SAND,p_dma->dir);
                        break;
                case DMA_TYPE_FIFO:
                        break;
                case DMA_TYPE_EBE:
                        break;
                }
                break;
                
                case DMA_IOC_SET_DEST_MODE:
                        p_dma->destType=arg;
                        break;
                        
                case DMA_IOC_SET_MEMORY_DIRECTION:
                        p_dma->dir=arg;
                        DMA_TRACE("mem direction\n");		 
                        break;
                case DMA_IOC_SET_2D_REG_X_SIZE:
                        p_dma->X=arg;
                        break;
                case DMA_IOC_SET_2D_REG_Y_SIZE:
                        p_dma->Y=arg;
                        break;
                case DMA_IOC_SET_2D_REG_W_SIZE:
                        p_dma->W=arg;
                        break;
                case DMA_IOC_QUERY_REPEAT:  
                        regs = (dma_regs_t*)(IO_ADDRESS(DMA_CH_BASE(channel)));
                        if(regs->Ctl&DMA_CTL_RPT)
                                DMA_TRACE("Auto Clear Fail!\n");
                        else
                                DMA_TRACE("Auto Clear Success!\n");
                        break;
                case DMA_IOC_SET_CONFIG:  
                        DMA_TRACE("Dma_SetConfig\n");	
                        Dma_SetConfig(filp);
                        break;
                        
                case DMA_IOC_SET_VERIFY:
                        { 
                                /*Verify DMA result*/
                                switch(p_dma->destType)
                                {
                                case DMA_TYPE_LINEAR:
                                        status = Verify(dma_ownership[channel].memory_dest,p_dma_req->count,DMA_SAND);
                                        break;
                                case DMA_TYPE_2D:
                                        status = Verify_2D(dma_ownership[channel].memory_dest,p_dma->X,p_dma->Y,p_dma->W,DMA_SAND,p_dma->dir);
                                        break;
                                case DMA_TYPE_FIFO:
                                        break;
                                case DMA_TYPE_EBE:
                                        break;
                                }
                                if(status)
                                        DMA_TRACE("Verify Fail\n");
                                else
                                        DMA_TRACE("Verify Success\n");
                                break;
                        }
                        
                default:
                        DMA_TRACE("No such IO ctl\n");
                        break;
    }
    return 0;
}


/*================================================================================================*/
/*================================================================================================*/
void __init dma_cleanup_module(void)
{
        //before exit, free all of the dma resources including irpt, memory and so on.
        class_simple_device_remove(MKDEV(gMajor, 0));
        class_simple_destroy(dma_tm_class);
        unregister_chrdev(gMajor, "dma");
        
}


/*================================================================================================*/
/*================================================================================================*/
dma_ownership_t* get_channel_ownership(dmach_t channel)
{
        if ( channel > MXC_DMA_CHANNELS)
        {
                DMA_TRACE("dma: channel number beyond capability.\n");
                return 0;
        }
        
        return &dma_ownership[channel];		
}


/*================================================================================================*/
/*================================================================================================*/
dmach_t get_channel(pid_t pid, struct file *filp)
{
        int channel;
        
        for(channel=0;channel<MXC_DMA_CHANNELS;channel++)
        {
                if(dma_ownership[channel].filp==filp)
                {
                        if(dma_ownership[channel].pid!=pid)
                        {
                                DMA_TRACE("dma: dma_ownership has error.\n");
                                return -1;
                        }
                        else
                        {
                                return channel;
                        }
                        
                }
        }
        return -1;
}

module_init(dma_init_module);
module_exit(dma_cleanup_module);

MODULE_DESCRIPTION("DMA test device driver");
MODULE_AUTHOR("Freescale Semiconductor, Inc.");
MODULE_LICENSE("GPL");


