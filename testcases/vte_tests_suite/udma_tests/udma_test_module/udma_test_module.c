/*================================================================================================*/
/**
        @file   udma_test_module.c

        @brief  Unified DMA test module C-file
*/
/*==================================================================================================

        Copyright (C) 2006, Freescale Semiconductor, Inc. All Rights Reserved
        THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
        BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
        Freescale Semiconductor, Inc.

====================================================================================================
Revision History:
                            Modification     Tracking
Author/core ID                  Date          Number    Description of Changes
-------------------------   ------------    ----------  -------------------------------------------
A.Ozerov/b00320              02/10/2006     TLSbo78550  Initial version.
A.Ozerov/b00320              01/11/2006     TLSbo81158  UDMA module was fixed for working with all platforms.
A.Ozerov/b00320              11/12/2006     TLSbo84161  Minor changes.
A.Ozerov/b00320              05/02/2007     TLSbo87473  One of testcases was removed.

====================================================================================================
Portability:  ARM GCC
==================================================================================================*/
#include "udma_test_module.h"

int     udma_open    (struct inode * inode, struct file * filp);
int     udma_release (struct inode * inode, struct file * filp);
int     udma_ioctl   (struct inode * inode, struct file *filp, unsigned int cmd, unsigned long arg);

static int                   udma_major;
static struct class          *udma_tm_class;
static char                  *udma_src;
static char                  *udma_dest;

#ifdef CONFIG_OTHER_PLATFORM
static dma_addr_t            udma_handle_src = 0;
static dma_addr_t            udma_handle_dest = 0;
#endif

static int                   udma_sand = 100;

static volatile int          udma_done = 0;
static int                   udma_status = 0;

#ifdef CONFIG_OTHER_PLATFORM
static int                   udma_channel = 0;
static int                   udma_sglist_set = 0;
//static int                   udma_num_buf = 0;
struct scatterlist           *sg;
#endif

#ifdef CONFIG_IMX27
wait_queue_head_t            q;
static int                   udma_opened = 0;
#endif

struct file_operations udma_fops =
{
        open:           udma_open,
        release:        udma_release,
        ioctl:          udma_ioctl
};

/*================================================================================================*/
/*================================================================================================*/
static void sand(int rand)
{
        udma_sand = rand;
        return;
}

/*================================================================================================*/
/*================================================================================================*/
static char rand(void)
{
        udma_sand = udma_sand*12357/1103;
        return udma_sand;
}

/*================================================================================================*/
/*================================================================================================*/
void udma_fill_mem(char *p, int size, int rd)
{
        int i = 0;

#ifdef CONFIG_IMX27
        UDMA_TRACE("fill 1d memory %8x\n", (unsigned int)p);
#else
        UDMA_TRACE("fill memory %8x\n", (unsigned int)p);
#endif
        sand(rd);
        for(i = 0; i < size; i++)
                *p++ = rand();
}

/*================================================================================================*/
/*================================================================================================*/
static void udma_callback(void * args, int error, unsigned int count)
{
        if(error != 0)
        {
                UDMA_TRACE("========== udma transfer is failure. Error code: %d ==========\n", error);
        }
        else
        {
                UDMA_TRACE("========== udma transfer was performed successfully ==========\n");
        }
        udma_status = error;
        udma_done = 1;
}

/*================================================================================================*/
/*================================================================================================*/
int udma_verify_mem(int size)
{
        int i = 0, j = 0;
        UDMA_TRACE("cheking the source and the destination buffers for equality...\n");
        for(i = 0; i < size; i++)
        {
                if(udma_dest[i] != udma_src[i])
                {
                        j++;
                }
                UDMA_TRACE("src[%d] = %d, dest[%d] = %d\n", i, udma_src[i], i, udma_dest[i]);
        }

        if(j > 0)
        {
                UDMA_TRACE("source and destination buffers aren't equal!!!\n");
                return 1;
        }
        else
        {
                UDMA_TRACE("source and destination buffers are equal\n");
                return 0;
        }
}

/*================================================================================================*/
/*================================================================================================*/
#ifdef CONFIG_IMX27
void udma_fill_2d_mem(char *p, int x, int y, int w, int rd, int dir)
{
        int i = 0, j = 0;
        int dec_offset = 0;

        if(dir)
                dec_offset = w-x;

        UDMA_TRACE("fill 2D memory %8x\n", (unsigned int)p);
        sand(rd);
        for(; j < y; j++)
        {
                for(; i < x; i++)
                {
                        *(p+j*w+i+dec_offset) = rand();
                }
        }
}

/*================================================================================================*/
/*================================================================================================*/
int udma_verify_1d_mem(char *p, int size, int rd)
{
        int i = 0, j = 0;
        char x;

        sand(rd);
        for(; i < size; i++)
        {
                x = rand();
                if(i == 10)
                        UDMA_TRACE("verify 1d memory %8x, x = %d, p[10] = %d\n", (unsigned int)p, (unsigned int)x, p[10]);
                if(x != p[i])
                {
                        j++;
                        if(j < 30)
                        {
                                UDMA_TRACE("i = %d, exp %d: %d\n", i, x, p[i]);

                        }
                        else
                                return 1;
                }
        }

        if(j > 0)
                return 1;
        else
                return 0;
}

/*================================================================================================*/
/*================================================================================================*/
int udma_verify_2d_mem(char *p, int x, int y, int w, int rd, int dir)
{
        int i = 0, j = 0;
        int error_counter = 0;
        char temp = 0;
        int dec_offset = 0;

        if(dir)
                dec_offset = w-x;

        sand(rd);
        for(; j < y; j++)
        {
                for(; i < x; i++)
                {
                        temp = rand();
                        if(i == 1 && j == 2)
                        UDMA_TRACE("verify 2d memory %8x, x=%d, p[%d*%d+%d+%d] = %d\n",(unsigned int)p, temp, j, w, i, dec_offset, p[j*w+i+dec_offset]);
                }
                if(temp != p[j*w+i+dec_offset])
                {
                        error_counter++;
                        if(error_counter < 30)
                        {
                                printk("i = %d; j = %d ; exp %d: %d\n", i, j, temp, p[j*w+i+dec_offset]);
                        }
                        else
                                return 1;
                }
        }

        if(error_counter > 0)
                return 1;
        else
                return 0;
}

/*================================================================================================*/
/*================================================================================================*/
mxc_dma_requestbuf_t *udma_build_bufferlist(unsigned int num_buf, char *base)
{
        int i = 0;
        mxc_dma_requestbuf_t *buf;

        buf = (mxc_dma_requestbuf_t*)kmalloc(num_buf*sizeof(mxc_dma_requestbuf_t), GFP_KERNEL|GFP_DMA);

        if(buf == NULL)
                return buf;

        for(; i < num_buf; i++)
        {
                buf[i].src_addr = virt_to_phys(base+(UDMA_INTERVAL_MEM*i));
                buf[i].dst_addr = virt_to_phys(base+(UDMA_INTERVAL_MEM*i+UDMA_TRANS_SIZE));
                buf[i].num_of_bytes = UDMA_TRANS_SIZE;
                UDMA_TRACE("%d: src = %x, dest = %x, length = %x\n", i, buf[i].src_addr, buf[i].dst_addr, buf[i].num_of_bytes);
        }
        return buf;
}

/*================================================================================================*/
/*================================================================================================*/
mxc_dma_requestbuf_t *udma_build_speclist(unsigned int num_buf, char * base)
{
        int i = 0;
        mxc_dma_requestbuf_t *buf;
        int width, gap;

        buf = (mxc_dma_requestbuf_t*)kmalloc(num_buf*sizeof(mxc_dma_requestbuf_t), GFP_KERNEL|GFP_DMA);

        if(buf == NULL)
                return buf;

        width = (1024/num_buf)*PAGE_SIZE;
        gap = width/2;

        for(; i < num_buf; i++)
        {
                buf[i].src_addr = virt_to_phys(base+(width*i));
                buf[i].dst_addr = virt_to_phys(base+(width*i+gap));
                buf[i].num_of_bytes = gap;
                UDMA_TRACE("%d: src = %x, dest = %x, length = %x\n", i, buf[i].src_addr, buf[i].dst_addr, buf[i].num_of_bytes);
        }
        return buf;
}

/*================================================================================================*/
/*================================================================================================*/
int udma_data_transfer(int channel_number)
{
        dmach_t channel;
        mxc_dma_requestbuf_t buf;
        int count = 0;
        char *memory_base, *src, *dest;
        channel = mxc_dma_request(channel_number, "mxc_udma");
        if(channel < 0)
        {
                UDMA_TRACE("unable to allocate udma channel\n");
                return -ENODEV;
        }

        memory_base = kmalloc(16384*sizeof(char), GFP_KERNEL|GFP_DMA);
        if(memory_base == NULL)
        {
                UDMA_TRACE("unable to allocate memory for base buffer\n");
                mxc_dma_free(channel);
                return -ENOMEM;
        }

        mxc_dma_callback_set(channel, udma_callback, NULL);
        src = memory_base;
        dest = src+UDMA_INTERVAL_MEM;

        switch(channel_number)
        {
        case MXC_DMA_TEST_RAM2D2RAM:
                udma_fill_2d_mem(src, 0x100, 0x10, 0x100, UDMA_SAND, 0);
                break;
        case MXC_DMA_TEST_RAM2RAM2D:
                udma_fill_mem(src, 0x80*0x10, UDMA_SAND);
                break;
        case MXC_DMA_TEST_RAM2D2RAM2D:
                udma_fill_2d_mem(src, 0x40, 0x10,0x80, UDMA_SAND, 0);
                break;
        case MXC_DMA_TEST_RAM2RAM:
                udma_fill_mem(src, UDMA_INTERVAL_MEM, UDMA_SAND);
                break;
        }

        memset(dest, 0, UDMA_INTERVAL_MEM);
        buf.src_addr = virt_to_phys(src);
        buf.dst_addr = virt_to_phys(dest);
        buf.num_of_bytes = UDMA_INTERVAL_MEM;
        mxc_dma_config(channel, &buf, 1, MXC_DMA_MODE_READ);
        mxc_dma_enable(channel);
        for(; count < 100; count++)
        {
                if(udma_done)
                {
                        UDMA_TRACE("udma transfer complete: error = %x\n", udma_status);
                        break;
                }
                set_current_state(TASK_UNINTERRUPTIBLE);
                schedule_timeout(100*(HZ/1000));
                set_current_state(TASK_RUNNING);
        }
        if(count >= 100)
        {
                UDMA_TRACE("udma transfer timeout\n");
                mxc_dma_disable(channel);
        }
        else
        {
                switch(channel_number)
                {
                case MXC_DMA_TEST_RAM2D2RAM:
                        if(!udma_verify_1d_mem(dest, 0x100*0x10, UDMA_SAND))
                        {
                                UDMA_TRACE("PASSED\n");
                        }
                        else
                        {
                                UDMA_TRACE("VERIFY FAILURED\n");
                        }
                        break;
                case MXC_DMA_TEST_RAM2RAM2D:
                        if(!udma_verify_2d_mem(dest, 0x80, 0x10, 0x100, UDMA_SAND, 0))
                        {
                                UDMA_TRACE("PASSED\n");
                        }
                        else
                        {
                                UDMA_TRACE("VERIFY FAILURED\n");
                        }
                        break;
                case MXC_DMA_TEST_RAM2D2RAM2D:
                        if(!udma_verify_2d_mem(dest, 0x40, 0x10, 0x80, UDMA_SAND, 0))
                        {
                                UDMA_TRACE("PASSED\n");
                        }
                        else
                        {
                                UDMA_TRACE("VERIFY FAILURED\n");
                        }
                        break;
                case MXC_DMA_TEST_RAM2RAM:
                        if(!udma_verify_1d_mem(dest, UDMA_INTERVAL_MEM, UDMA_SAND))
                        {
                                UDMA_TRACE("PASSED\n");
                        }
                        else
                        {
                                UDMA_TRACE("VERIFY FAILURED\n");
                        }
                        break;
                }
        }

        kfree(memory_base);
        mxc_dma_free(channel);

        return 0;
}

/*================================================================================================*/
/*================================================================================================*/
int udma_hw_chain_buffer_transfer(unsigned int num_buf)
{
        int i = 0, count = 0;
        dmach_t channel;
        mxc_dma_requestbuf_t *buf;
        char *memory_base, *src, *dest;
        int width, gap;

        if(num_buf > 4)
        {
                UDMA_TRACE("too large transtime\n");
                return -EINVAL;
        }

        channel = mxc_dma_request(MXC_DMA_TEST_HW_CHAINING, "mxc_udma");
        if(channel < 0)
        {
                UDMA_TRACE("unable to allocate udma channel\n");
                return -ENODEV;
        }

        memory_base = kmalloc(40960*sizeof(char), GFP_KERNEL|GFP_DMA);
        if(memory_base == NULL)
        {
                UDMA_TRACE("unable to allocate memory for base buffer\n");
                mxc_dma_free(channel);
                return -ENOMEM;
        }

        buf = udma_build_speclist(num_buf, memory_base);
        if(buf == NULL)
        {
                UDMA_TRACE("unable to allocate memory for special list of buffers\n");
                return -ENOMEM;
        }

        mxc_dma_callback_set(channel, udma_callback, NULL);

        width = (1024/num_buf)*PAGE_SIZE;
        gap = width/2;

        for(; i < num_buf; i++)
        {
                src = (char*)(memory_base+width*i);
                dest = (char*)(memory_base+(width*i+gap));

                udma_fill_mem(src, gap, UDMA_SAND+i);
                memset(dest, 0, gap);
        }

        mxc_dma_config(channel, buf, num_buf, MXC_DMA_MODE_READ);
        mxc_dma_enable(channel);

        for(; count < 1000; count++)
        {
                if(udma_done)
                {
                        UDMA_TRACE("udma transfer complete: error =%x\n", udma_status);
                        break;
                }
                set_current_state(TASK_UNINTERRUPTIBLE);
                schedule_timeout(500*(HZ/1000));
                set_current_state(TASK_RUNNING);
        }
        if(count >= 1000)
        {
                UDMA_TRACE("udma transfer timeout\n");
                mxc_dma_disable(channel);
        }

        for(i = 0; i < num_buf; i++)
        {
                UDMA_TRACE("get buffer %d verify\n", i);
                dest = (char*)(memory_base+(width*i+gap));

                if(udma_verify_1d_mem(dest, gap, UDMA_SAND+i)) break;
        }
        if(i >= num_buf)
                UDMA_TRACE("PASSED\n");
        else
                UDMA_TRACE("VERIFY FAILURED\n");

        kfree(buf);
        kfree(memory_base);
        mxc_dma_free(channel);

        return 0;
}

/*================================================================================================*/
/*================================================================================================*/
int udma_sw_chain_buffer_transfer(unsigned int num_buf)
{
        int i = 0, count = 0;
        dmach_t channel;
        mxc_dma_requestbuf_t *buf;
        char *memory_base, *src, *dest;

        if(num_buf > 4)
        {
                UDMA_TRACE("too large transtime\n");
                return -EINVAL;
        }

        channel = mxc_dma_request(MXC_DMA_TEST_SW_CHAINING, "mxc_udma");
        if(channel < 0)
        {
                UDMA_TRACE("unable to allocate udma channel\n");
                return -ENODEV;
        }

        memory_base = kmalloc(16384*sizeof(char), GFP_KERNEL|GFP_DMA);
        if(memory_base == NULL)
        {
                UDMA_TRACE("unable to allocate memory for base buffer\n");
                mxc_dma_free(channel);
                return -ENOMEM;
        }

        buf = udma_build_bufferlist(num_buf, memory_base);
        if(buf == NULL)
        {
                UDMA_TRACE("unable to allocate memory for list of buffers\n");
                return -ENOMEM;
        }

        mxc_dma_callback_set(channel, udma_callback, NULL);

        for(; i < num_buf; i++)
        {
                src = (char*)(memory_base+UDMA_INTERVAL_MEM*i);
                dest = (char*)(memory_base+(UDMA_INTERVAL_MEM*i+UDMA_TRANS_SIZE));

                udma_fill_mem(src, UDMA_TRANS_SIZE, UDMA_SAND+i);
                memset(dest, 0, UDMA_TRANS_SIZE);
        }

        mxc_dma_config(channel, buf, num_buf, MXC_DMA_MODE_READ);
        mxc_dma_enable(channel);

        for(; count < 400; count++)
        {
                if(udma_done)
                {
                        UDMA_TRACE("udma transfer complete: error = %x\n", udma_status);
                        break;
                }
                set_current_state(TASK_UNINTERRUPTIBLE);
                schedule_timeout(100*(HZ/1000));
                set_current_state(TASK_RUNNING);
        }
        if(count >= 400)
        {
                UDMA_TRACE("udma transfer timeout\n");
                mxc_dma_disable(channel);
        }
        else
        {
                for(i = 0; i < num_buf; i++)
                {
                        UDMA_TRACE("get buffer %d verify\n", i);
                        dest = (char*)(memory_base+UDMA_INTERVAL_MEM*i+UDMA_TRANS_SIZE);
                        if(udma_verify_1d_mem(dest, UDMA_TRANS_SIZE, UDMA_SAND+i)) break;
                }
                if(i >= num_buf)
                        UDMA_TRACE("PASSED\n");
                else
                        UDMA_TRACE("VERIFY FAILURED\n");
        }

        kfree(buf);
        kfree(memory_base);
        mxc_dma_free(channel);

        return 0;
}
#endif

/*================================================================================================*/
/*================================================================================================*/
#ifdef CONFIG_OTHER_PLATFORM
int udma_set_config(void)
{
        int ret = 0;
        mxc_dma_requestbuf_t *dma_req = kmalloc(sizeof(mxc_dma_requestbuf_t), GFP_KERNEL|GFP_DMA);
        if(dma_req == NULL)
        {
                return -ENOMEM;
        }

        memset(dma_req, 0, sizeof(mxc_dma_requestbuf_t));

        dma_req->src_addr = udma_handle_src;
        dma_req->dst_addr = udma_handle_dest;
        dma_req->num_of_bytes = UDMA_BUF_SIZE;

        ret = mxc_dma_config(udma_channel, dma_req, 1, MXC_DMA_MODE_WRITE);

        return ret;
}
#endif

/*================================================================================================*/
/*================================================================================================*/
static int __init udma_init_module(void)
{
        struct class_device *temp_class;
        int error;

        UDMA_TRACE("register virtual udma driver\n");

        /* register a character device */
        error = register_chrdev(0, UDMA_NAME, &udma_fops);
        if(error < 0)
        {
                UDMA_TRACE("udma driver can't get major number\n");
                return error;
        }
        udma_major = error;
        UDMA_TRACE("udma major number = %d\n",udma_major);

        udma_tm_class = class_create(THIS_MODULE, UDMA_NAME);
        if(IS_ERR(udma_tm_class))
        {
                UDMA_TRACE(KERN_ERR "error creating udma test module class\n");
                unregister_chrdev(udma_major, UDMA_NAME);
                class_device_destroy(udma_tm_class, MKDEV(udma_major, 0));
                return PTR_ERR(udma_tm_class);
        }

        temp_class = class_device_create(udma_tm_class, NULL,  MKDEV(udma_major, 0), NULL, UDMA_NAME);
        if(IS_ERR(temp_class))
        {
                printk(KERN_ERR "error creating udma test module class device\n");
                unregister_chrdev(udma_major, UDMA_NAME);
                class_device_destroy(udma_tm_class, MKDEV(udma_major, 0));
                class_destroy(udma_tm_class);
                return -1;
        }

        UDMA_TRACE("creating devfs entry for udma\n");

#ifdef CONFIG_IMX27
        init_waitqueue_head(&q);
#endif

        return 0;
}

/*================================================================================================*/
/*================================================================================================*/
#ifdef CONFIG_IMX27
int udma_open(struct inode * inode, struct file * filp)
{
        udma_done = 0;
        if(xchg(&udma_opened, 1))return -EBUSY;
        return 0;
}

/*================================================================================================*/
/*================================================================================================*/
int udma_release(struct inode * inode, struct file * filp)
{
        if(xchg(&udma_opened, 0))return -EBUSY;;
        return 0;
}
#endif

/*================================================================================================*/
/*================================================================================================*/
#ifdef CONFIG_OTHER_PLATFORM
int udma_open(struct inode *inode, struct file *filp)
{
        printk("\n");
        udma_done = 0;
        udma_sglist_set = 0;

        udma_channel = mxc_dma_request(MXC_DMA_MEMORY, "mxc_udma");
        if(udma_channel < 0)
        {
                UDMA_TRACE("unable to allocate udma channel\n");
                return -ENODEV;
        }
        UDMA_TRACE("channel %d was opened\n", udma_channel);

        udma_src = dma_alloc_coherent(NULL, PAGE_ALIGN(UDMA_BUF_SIZE*8*sizeof(char)), &udma_handle_src, GFP_KERNEL|GFP_DMA);
        if(udma_src == NULL)
        {
                UDMA_TRACE("unable to allocate memory for source buffer\n");
                return -ENOMEM;
        }
        udma_dest = dma_alloc_coherent(NULL, PAGE_ALIGN(UDMA_BUF_SIZE*8*sizeof(char)), &udma_handle_dest, GFP_KERNEL|GFP_DMA);
        if(udma_dest == NULL)
        {
                UDMA_TRACE("unable to allocate memory for destination buffer\n");
                return -ENOMEM;
        }

        return 0;
}

/*================================================================================================*/
/*================================================================================================*/
int udma_release(struct inode * inode, struct file * filp)
{
        int ret = 0;

        UDMA_TRACE("begin udma free...\n");

        if(udma_sglist_set) kfree(sg);

        dma_free_coherent(NULL, PAGE_ALIGN(UDMA_BUF_SIZE*8*sizeof(char)), udma_src, udma_handle_src);
        dma_free_coherent(NULL, PAGE_ALIGN(UDMA_BUF_SIZE*8*sizeof(char)), udma_dest, udma_handle_dest);

        ret = mxc_dma_free(udma_channel);
        if(ret < 0)
        {
                UDMA_TRACE("unable to free udma channel. Error code: %d\n", ret);
                return ret;
        }
        UDMA_TRACE("udma channel was freed\n");
        UDMA_TRACE("end udma free\n\n");

        return ret;
}

/*================================================================================================*/
/*================================================================================================*/
int udma_data_transfer(void)
{
        int ret = 0, i = 0;
        DECLARE_MUTEX(udma_mux);

        down_interruptible(&udma_mux);

        udma_fill_mem(udma_src, UDMA_BUF_SIZE, UDMA_SAND);
        memset(udma_dest, 0, UDMA_BUF_SIZE);

        ret = udma_set_config();
        if(ret != 0)
        {
                UDMA_TRACE("udma_set_config failed. Error code: %d\n", ret);
                return ret;
        }
        UDMA_TRACE("udma_set_config passed\n");

        ret = mxc_dma_callback_set(udma_channel, udma_callback, NULL);
        if(ret < 0)
        {
                UDMA_TRACE("error in mxc_dma_callback_set. Error code: %d\n", ret);
                return ret;
        }
        UDMA_TRACE("mxc_dma_callback_set passed\n");

        UDMA_TRACE("************ begin data transfer(single buffer) ************\n");
        ret = mxc_dma_enable(udma_channel);
        if(ret < 0)
        {
                UDMA_TRACE("error in mxc_dma_enable. Error code: %d\n", ret);
                return ret;
        }

        for(i = 0; i < 1000; i++)
        {
                if(udma_done)
                {
                        UDMA_TRACE("udma transfer complete. Error code: %d\n", udma_status);
                        break;
                }

                set_current_state(TASK_UNINTERRUPTIBLE);
                schedule_timeout(100*(HZ/1000));
                set_current_state(TASK_RUNNING);
        }
        if(i >= 1000)
        {
                UDMA_TRACE("udma transfer timeout\n");
                mxc_dma_disable(udma_channel);
        }

        UDMA_TRACE("************ end data transfer ************\n");
        if(udma_verify_mem(UDMA_BUF_SIZE))
                return -1;

        up(&udma_mux);

        return ret;
}

/*================================================================================================*/
/*================================================================================================*/
/*int udma_build_sglist(int num_buf)
{
        int n_buf = 0;

        n_buf = dma_map_sg(NULL, sg, num_buf, MXC_DMA_MODE_READ);
        if(sg == NULL) return -ENOMEM;

        return n_buf;
}
*/
#endif

/*================================================================================================*/
/*================================================================================================*/
/*int udma_chain_buffer_transfer(void)
{
        int i = 0, j = 0, ret = 0;
        DECLARE_MUTEX(udma_mux);

        if(udma_num_buf > 8)
        {
                UDMA_TRACE("too large transfer time\n");
                return -EINVAL;
        }

        down_interruptible(&udma_mux);

        ret = udma_build_sglist(udma_num_buf, &udma_handle_src);
        if(ret < 0)
        {
                UDMA_TRACE("udma_build_sglist failed. Error code: %d\n", ret);
                return ret;
        }
        UDMA_TRACE("udma_build_sglist passed\n");

        udma_fill_mem(udma_src, UDMA_BUF_SIZE*udma_num_buf, UDMA_SAND);
        memset(udma_dest, 0, UDMA_BUF_SIZE*udma_num_buf);

        mxc_dma_sg_config(udma_channel, sg, udma_num_buf, UDMA_BUF_SIZE, MXC_DMA_MODE_WRITE);

        ret = mxc_dma_callback_set(udma_channel, udma_callback, NULL);
        if(ret < 0)
        {
                UDMA_TRACE("error in mxc_dma_callback_set. Error code: %d\n", ret);
                return ret;
        }
        UDMA_TRACE("mxc_dma_callback_set passed\n");

        UDMA_TRACE("************ begin data transfer(using chain of buffers) ************\n");
        ret = mxc_dma_enable(udma_channel);
        if(ret < 0)
        {
                UDMA_TRACE("error in mxc_dma_enable. Error code: %d\n", ret);
                return ret;
        }

        for(i = 0; i < 1000; i++)
        {
                if(udma_done)
                {
                        UDMA_TRACE("udma transfer complete. Error code: %d\n", udma_status);
                        break;
                }

                set_current_state(TASK_UNINTERRUPTIBLE);
                schedule_timeout(100*(HZ/1000));
                set_current_state(TASK_RUNNING);
        }
        if(i >= 1000)
        {
                UDMA_TRACE("udma transfer timeout\n");
                mxc_dma_disable(udma_channel);
        }

        UDMA_TRACE("************ end data transfer ************\n");

        for(i = 0; i < udma_num_buf; i++)
        {
                UDMA_TRACE("verify buffer number %d\n", i+1);
                udma_dest = (char*)(udma_dest+UDMA_BUF_SIZE*i);
                udma_src = (char*)(udma_src+UDMA_BUF_SIZE*i);
                if(udma_verify_mem(UDMA_BUF_SIZE)) j++;
        }
        if(j > 0)
                return -1;

        up(&udma_mux);

        return ret;
}
*/
/*================================================================================================*/
/*================================================================================================*/
int udma_ioctl(struct inode * inode, struct file *filp, unsigned int cmd, unsigned long arg)
{
        int status = 0;

/*#ifdef CONFIG_OTHER_PLATFORM
int num_buf_ret = 0;
#endif
*/
        switch(cmd)
        {
#ifdef CONFIG_OTHER_PLATFORM
        case UDMA_IOC_SET_CONFIG:
/*                udma_num_buf = arg;
                if(udma_num_buf > 8)
                {
                        UDMA_TRACE("too large transfer time\n");
                        return -EINVAL;
                }
*/
                status = udma_set_config();
                if(status < 0)
                {
                        UDMA_TRACE("udma_set_config failed. Error code: %d\n", status);
                        return status;
                }
                UDMA_TRACE("udma_set_config passed\n");
/*                num_buf_ret = udma_build_sglist(udma_num_buf);
                if(status < 0)
                {
                        UDMA_TRACE("udma_build_sglist failed. Error code: %d\n", status);
                        return status;
                }
                UDMA_TRACE("udma_build_sglist passed\n");
                udma_sglist_set = 1;
                status = mxc_dma_sg_config(udma_channel, sg, num_buf_ret, UDMA_BUF_SIZE, MXC_DMA_MODE_READ);
                if(status < 0)
                {
                        UDMA_TRACE("mxc_dma_sg_config failed. Error code: %d\n", status);
                        return status;
                }
                UDMA_TRACE("mxc_dma_sg_config passed\n");
                dma_unmap_sg(NULL, sg, udma_num_buf, MXC_DMA_MODE_READ);
*/
                break;
        case UDMA_IOC_SET_CALLBACK:
                status = mxc_dma_callback_set(udma_channel, udma_callback, NULL);
                if(status < 0)
                {
                        UDMA_TRACE("mxc_dma_callback_set failed. Error code: %d\n", status);
                        return status;
                }
                UDMA_TRACE("mxc_dma_callback_set passed\n");
                break;
/*        case UDMA_IOC_TEST_CHAINBUFFER:
                udma_num_buf = arg;
                status = udma_chain_buffer_transfer();
                if(status < 0)
                {
                        UDMA_TRACE("udma_chain_buffer_transfer failed. Error code: %d\n", status);
                        return status;
                }
                UDMA_TRACE("udma_chain_buffer_transfer passed\n");
                break;
*/
        case UDMA_IOC_DATA_TRANSFER:
                status = udma_data_transfer();
                if(status < 0)
                {
                        UDMA_TRACE("udma_data_transfer failed. Error code: %d\n", status);
                        return status;
                }
                UDMA_TRACE("udma_data_transfer passed\n");
                break;
#endif
#ifdef CONFIG_IMX27
        case UDMA_IOC_RAM2RAM:
                status = udma_data_transfer(MXC_DMA_TEST_RAM2RAM);
                if(status < 0)
                {
                        UDMA_TRACE("(ram to ram)udma_data_transfer failed. Error code: %d\n", status);
                        return status;
                }
                UDMA_TRACE("(ram to ram)udma_data_transfer passed\n");
                break;
        case UDMA_IOC_RAM2D2RAM2D:
                status = udma_data_transfer(MXC_DMA_TEST_RAM2D2RAM2D);
                if(status < 0)
                {
                        UDMA_TRACE("(ram_2d to ram_2d)udma_data_transfer failed. Error code: %d\n", status);
                        return status;
                }
                UDMA_TRACE("(ram_2d to ram_2d)udma_data_transfer passed\n");
                break;
        case UDMA_IOC_RAM2RAM2D:
                status = udma_data_transfer(MXC_DMA_TEST_RAM2RAM2D);
                if(status < 0)
                {
                        UDMA_TRACE("(ram to ram_2d)udma_data_transfer failed. Error code: %d\n", status);
                        return status;
                }
                UDMA_TRACE("(ram to ram_2d)udma_data_transfer passed\n");
                break;
        case UDMA_IOC_RAM2D2RAM:
                status = udma_data_transfer(MXC_DMA_TEST_RAM2D2RAM);
                if(status < 0)
                {
                        UDMA_TRACE("(ram_2d to ram)udma_data_transfer failed. Error code: %d\n", status);
                        return status;
                }
                UDMA_TRACE("(ram_2d to ram)udma_data_transfer passed\n");
                break;
        case UDMA_IOC_HW_CHAINBUFFER:
                status = udma_hw_chain_buffer_transfer(arg);
                if(status < 0)
                {
                        UDMA_TRACE("udma_hw_chain_buffer_transfer failed. Error code: %d\n", status);
                        return status;
                }
                UDMA_TRACE("udma_hw_chain_buffer_transfer passed\n");
                break;
        case UDMA_IOC_SW_CHAINBUFFER:
                status = udma_sw_chain_buffer_transfer(arg);
                if(status < 0)
                {
                        UDMA_TRACE("udma_sw_chain_buffer_transfer failed. Error code: %d\n", status);
                        return status;
                }
                UDMA_TRACE("udma_sw_chain_buffer_transfer passed\n");
                break;
#endif
        default:
                UDMA_TRACE("no such ioctl command\n");
                break;
        }

        return status;
}

/*================================================================================================*/
/*================================================================================================*/
static void __exit udma_cleanup_module(void)
{
        unregister_chrdev(udma_major, UDMA_NAME);
        class_device_destroy(udma_tm_class, MKDEV(udma_major, 0));
        class_destroy(udma_tm_class);
        UDMA_TRACE("UDMA test: removing virtual device\n");
}

module_init(udma_init_module);
module_exit(udma_cleanup_module);

MODULE_DESCRIPTION("UDMA test device driver");
MODULE_AUTHOR("Freescale Semiconductor, Inc.");
MODULE_LICENSE("GPL");
