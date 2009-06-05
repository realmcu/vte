/*================================================================================================*/
/**
        @file   ipc_module.c

        @brief  IPC dirver API
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
-------------------------   ------------    ----------  --------------------------------------------
A.Ozerov/b00320              26/04/2006     TLSbo61791  Initial version
V.Khalabuda/b00306           07/04/2006     TLSbo63489  Update version for linux-2.6.10-rel-L26_1_19

====================================================================================================
Portability: ARM GCC

==================================================================================================*/

/*==================================================================================================
                                        INCLUDE FILES
==================================================================================================*/
//#include <linux/config.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/string.h>
#include <linux/fs.h>
#include <linux/major.h>
//---#include <linux/devfs_fs_kernel.h>

#include "ipc_module.h"

/*==================================================================================================
                                        LOCAL VARIABLES
==================================================================================================*/
static int major_num = 0;

static  wait_queue_head_t write_queue,
        read_queue,
        notify_queue;
static bool     read_done,
                write_done;
static struct   class *ipc_class;

/*==================================================================================================
                                    FUNCTION PROTOTYPES
==================================================================================================*/
static ssize_t mxc_ipc_read(struct file *file, char *buf, size_t count, loff_t * ppos)
{
        return 0;
}

static ssize_t mxc_ipc_write(struct file *filp, const char *buf, size_t count, loff_t * ppos)
{
        return 0;
}

/*==================================================================================================
                                        LOCAL FUNCTIONS
==================================================================================================*/
static void read_callback(HW_CTRL_IPC_READ_STATUS_T * status)
{
        DPRINTK("IPC: %d bytes read on channel nb %d\n", status->nb_bytes,
                status->channel->channel_nb);
        read_done = true;
        wake_up_interruptible(&read_queue);
}

/*================================================================================================*/
static void write_callback(HW_CTRL_IPC_WRITE_STATUS_T * status)
{
        DPRINTK("IPC: %d bytes wrote on channel nb %d\n", status->nb_bytes,
                status->channel->channel_nb);
        write_done = true;
        wake_up_interruptible(&write_queue);
}

static void read_callback_new(HW_CTRL_IPC_READ_STATUS_T * status)
{
        DPRINTK("IPC: New Read callback invoked successfully\n");
        read_callback(status);
}

static void write_callback_new(HW_CTRL_IPC_WRITE_STATUS_T * status)
{
        DPRINTK("IPC: New Write callback invoked successfully\n");
        write_callback(status);
}


/*================================================================================================*/
static void notify_callback(HW_CTRL_IPC_NOTIFY_STATUS_T * status)
{
        DPRINTK("IPC: Notify callback called from channel nb %d\n", status->channel->channel_nb);
        wake_up_interruptible(&notify_queue);
}

/*================================================================================================*/
int check_data_integrity(char *buf1, char *buf2, int count)
{
        int     result = 0;
        int     i;

        for (i = 0; i < count; i++)
        {
                if (buf1[i] != buf2[i])
                {
                        printk("Corrupted data at %d wbuf = %d rbuf = %d\n", i, buf1[i], buf2[i]);
                        result = -1;
                }
        }
        return result;
}

/*================================================================================================*/
/*===== open_close_test =====*/
/**
@brief  This function implements the action open/close a IPC device.

@param  inode       pointer on the node
        file        pointer on the file

@return This function returns 0.
*/
/*================================================================================================*/
int open_close_test(unsigned long arg)
{
        HW_CTRL_IPC_OPEN_T      config;
        HW_CTRL_IPC_CHANNEL_T  *channel;
        int     result;
        int     i;

        channel = kmalloc(sizeof(HW_CTRL_IPC_CHANNEL_T), GFP_KERNEL);
        channel->channel_nb = ((struct ioctl_args *)arg)->channel;

        DPRINTK("IPC: opening and closing a SHORT MESSAGE IPC channel from "
                "kernel multiple times\n");

        config.index = channel->channel_nb;
        config.type = HW_CTRL_IPC_SHORT_MSG;
        config.read_callback = read_callback;
        config.write_callback = write_callback;
        config.notify_callback = notify_callback;
        kfree(channel);

        for (i = 0; i < 100; i++)
        {
                channel = hw_ctrl_ipc_open(&config);
                if (channel == 0)
                {
                        DPRINTK("IPC: Unable to open virtual channel %d\n", channel->channel_nb);
                        return -1;
                }

                result = hw_ctrl_ipc_close(channel);
                if (result != HW_CTRL_IPC_STATUS_OK)
                {
                        DPRINTK("IPC: Unable to close virtual channel %d\n", channel->channel_nb);
                        return -1;
                }
        }

        DPRINTK("IPC: open_close test OK \n");

        return 0;
}

/*================================================================================================*/
/*===== short_message_loopback =====*/
/**
@brief  This function implements the action of loopback short message on a IPC device.

@param  inode       pointer on the node
        file        pointer on the file

@return This function returns 0.
*/
/*================================================================================================*/
int short_message_loopback(unsigned long arg)
{
        HW_CTRL_IPC_OPEN_T      config;
        unsigned short  channel;
        int     result = 0,
                i;
        int     iterations;
        char    wbuf[4];
        char    rbuf[4];
        int     message;
        HW_CTRL_IPC_CHANNEL_T *vchannel = NULL;

        channel = ((struct ioctl_args *)arg)->channel;
        message = ((struct ioctl_args *)arg)->message;
        iterations = ((struct ioctl_args *)arg)->iterations;

        DPRINTK("IPC: about to send %d messages on channel %d\n", iterations, channel);

        config.index = channel;
        config.type = HW_CTRL_IPC_SHORT_MSG;
        config.read_callback = read_callback;
        config.write_callback = write_callback;
        config.notify_callback = notify_callback;

        vchannel = hw_ctrl_ipc_open(&config);
        if (vchannel == 0)
        {
                DPRINTK("IPC: Unable to open virtual channel %d\n", vchannel->channel_nb);
                return -1;
        }

        memset(wbuf, 0, 4);
        memset(rbuf, 0, 4);

        for (i = 0; i < 4; i++)
        {
                wbuf[i] = (char)i;
        }

        i = 0;
        while (i < iterations)
        {
                write_done = false;
                read_done = false;
                result = hw_ctrl_ipc_write(vchannel, wbuf, 4);
                if (result == HW_CTRL_IPC_STATUS_ERROR)
                {
                        DPRINTK("IPC: Error on hw_ctrl_ipc_write function\n");
                        break;
                }

                wait_event_interruptible(write_queue, write_done);

                result = hw_ctrl_ipc_read(vchannel, rbuf, 4);
                if (result != HW_CTRL_IPC_STATUS_OK)
                {
                        DPRINTK("IPC: Error on hw_ctrl_ipc_read function\n");
                        break;
                }

                wait_event_interruptible(read_queue, read_done);

                DPRINTK("IPC: Received message # %d from channel %d\n", i, channel);

                if (check_data_integrity(wbuf, rbuf, 4) == -1)
                {
                        break;
                }

                memset(rbuf, 0, 4);

                i++;
        }

        if (hw_ctrl_ipc_close(vchannel) != HW_CTRL_IPC_STATUS_OK)
        {
                DPRINTK("IPC: Error on hw_ctrl_ipc_close function\n");
                result = -1;
        }

        if (result == 0)
        {
                DPRINTK("TEST for Short Message channels OK\n");
        }
        else
        {
                DPRINTK("TEST for Short Message channels FAILED\n");
        }

        return result;
}

/*================================================================================================*/
/*===== packet_data_loopback =====*/
/**
@brief  This function implements the action of loopback packet data method on a IPC device.

@param  inode       pointer on the node
        file        pointer on the file

@return This function returns 0.
*/
/*================================================================================================*/
int packet_data_loopback(unsigned long arg)
{
        HW_CTRL_IPC_OPEN_T      config;
        int     status = 0,
                i;
        int     iterations;
        HW_CTRL_IPC_CHANNEL_T  *vchannel = NULL;
        char   *wbuf;
        char   *rbuf;
	dma_addr_t wpaddr;
	dma_addr_t rpaddr;
        int     bytes;

        bytes = ((struct ioctl_args *) arg)->bytes;
        iterations = ((struct ioctl_args *) arg)->iterations;

        wbuf = dma_alloc_coherent(NULL, bytes, &wpaddr, GFP_DMA);
	rbuf = dma_alloc_coherent(NULL, bytes, &rpaddr, GFP_DMA);

        DPRINTK("IPC: about to send %d bytes on channel 2\n", bytes);

        config.index = 0;
        config.type = HW_CTRL_IPC_PACKET_DATA;
        config.read_callback = read_callback;
        config.write_callback = write_callback;
        config.notify_callback = notify_callback;

        vchannel = hw_ctrl_ipc_open(&config);
        if (vchannel == 0)
        {
                DPRINTK("IPC: Unable to open virtual channel %d\n", vchannel->channel_nb);
                return -1;
        }

        memset(wbuf, 0, bytes);
        memset(rbuf, 0, bytes);

        for (i = 0; i < bytes; i++)
        {
                wbuf[i] = (char) i;
        }

        i = 0;
        while (i < iterations)
        {
                write_done = false;
                read_done = false;
                status = hw_ctrl_ipc_write(vchannel, (unsigned char *)wpaddr, bytes);
                if (status == HW_CTRL_IPC_STATUS_ERROR)
                {
                        DPRINTK("IPC: Error on hw_ctrl_ipc_write function\n");
                        status = -1;
                        break;
                }

                wait_event_interruptible(write_queue, write_done);

                status = hw_ctrl_ipc_read(vchannel, (unsigned char *)rpaddr, bytes);
                if (status != HW_CTRL_IPC_STATUS_OK)
                {
                        DPRINTK("IPC: Error on hw_ctrl_ipc_read function\n");
                        status = -1;
                        break;
                }

                wait_event_interruptible(read_queue, read_done);

                DPRINTK("IPC: Received message # %d from channel 2\n", i);

                if (check_data_integrity(wbuf, rbuf, bytes) == -1)
                {
                        DPRINTK("IPC: TEST FAILED on channel %d iteration %d\n",
                                vchannel->channel_nb, i);
                        status = -1;
                        break;
                }

                memset(rbuf, 0, bytes);
                i++;
		/* Change callbacks for last iteration */
		if (i == (iterations - 1)) {
			hw_ctrl_ipc_ioctl(vchannel,
					  HW_CTRL_IPC_SET_READ_CALLBACK,
					  (void *)read_callback_new);
			hw_ctrl_ipc_ioctl(vchannel,
					  HW_CTRL_IPC_SET_WRITE_CALLBACK,
					  (void *)write_callback_new);
			hw_ctrl_ipc_ioctl(vchannel,
					  HW_CTRL_IPC_SET_MAX_CTRL_STRUCT_NB,
					  (void *)32);
		}
        }

        if (hw_ctrl_ipc_close(vchannel) != HW_CTRL_IPC_STATUS_OK)
        {
                DPRINTK("IPC: Error on hw_ctrl_ipc_close function\n");
                status = -1;
        }

        dma_free_coherent(NULL, bytes, wbuf, wpaddr);
	 dma_free_coherent(NULL, bytes, rbuf, rpaddr);
        if (status == 0)
        {
                DPRINTK("TEST for Packet Data channel OK\n");
        }
        else
        {
                DPRINTK("TEST for Packet Data channel FAILED\n");
        }

        return status;
}

/*================================================================================================*/
/*===== packet_data_write_ex_cont_loopback =====*/
/**
@brief  This function implements the action of loopback packet data write exec link on a IPC device.

@param  inode       pointer on the node
        file        pointer on the file

@return This function returns 0.
*/
/*================================================================================================*/
int packet_data_write_ex_cont_loopback(unsigned long arg)
{
// Unit test code

	HW_CTRL_IPC_OPEN_T config;
	int status = 0, i;
	int iterations;
	HW_CTRL_IPC_CHANNEL_T *vchannel = NULL;
	HW_CTRL_IPC_WRITE_PARAMS_T write_buf;
	char *wbuf;
	char *rbuf;
	dma_addr_t wpaddr;
	dma_addr_t rpaddr;
	int bytes;

	bytes = ((struct ioctl_args *)arg)->bytes;
	iterations = ((struct ioctl_args *)arg)->iterations;

	wbuf = dma_alloc_coherent(NULL, bytes, &wpaddr, GFP_DMA);
	rbuf = dma_alloc_coherent(NULL, bytes, &rpaddr, GFP_DMA);

	DPRINTK("IPC: about to send %d bytes on channel 2\n", bytes);

	config.index = 0;
	config.type = HW_CTRL_IPC_PACKET_DATA;
	config.read_callback = read_callback;
	config.write_callback = write_callback;
	config.notify_callback = notify_callback;

	vchannel = hw_ctrl_ipc_open(&config);
	if (vchannel == 0) {
		DPRINTK("IPC: Unable to open virtual channel %d\n",
			vchannel->channel_nb);
		return -1;
	}

	memset(wbuf, 0, bytes);
	memset(rbuf, 0, bytes);

	for (i = 0; i < bytes; i++) {
		wbuf[i] = (char)i;
	}

	write_buf.ipc_memory_read_mode = HW_CTRL_IPC_MODE_CONTIGUOUS;
	write_buf.read.cont_ptr = (HW_CTRL_IPC_CONTIGUOUS_T *)
	    kmalloc(sizeof(HW_CTRL_IPC_CONTIGUOUS_T), GFP_KERNEL);
	write_buf.read.cont_ptr->data_ptr = (unsigned char *)wpaddr;
	write_buf.read.cont_ptr->length = bytes;

	i = 0;
	while (i < iterations) {
		write_done = false;
		read_done = false;

		status = hw_ctrl_ipc_write_ex(vchannel, &write_buf);
		if (status == HW_CTRL_IPC_STATUS_ERROR) {
			DPRINTK
			    ("IPC:Error in hw_ctrl_ipc_write_ex function %d\n",
			     status);
			status = -1;
			break;
		}

		wait_event_interruptible(write_queue, write_done);

		status =
		    hw_ctrl_ipc_read(vchannel, (unsigned char *)rpaddr, bytes);
		if (status != HW_CTRL_IPC_STATUS_OK) {
			DPRINTK("IPC:Error on hw_ctrl_ipc_read function\n");
			status = -1;
			break;
		}

		wait_event_interruptible(read_queue, read_done);

		DPRINTK("IPC: Received message # %d from channel 2\n", i);

		if (check_data_integrity(wbuf, rbuf, bytes) == -1) {
			DPRINTK("IPC: TEST FAILED on channel %d iteration %d\n",
				vchannel->channel_nb, i);
			status = -1;
			break;
		}

		memset(rbuf, 0, bytes);
		i++;
	}

	if (hw_ctrl_ipc_close(vchannel) != HW_CTRL_IPC_STATUS_OK) {
		DPRINTK("IPC: Error on hw_ctrl_ipc_close function\n");
		status = -1;
	}
	write_buf.read.cont_ptr->data_ptr = NULL;
	kfree(write_buf.read.cont_ptr);
	dma_free_coherent(NULL, bytes, wbuf, wpaddr);
	dma_free_coherent(NULL, bytes, rbuf, rpaddr);
	if (status == 0) {
		DPRINTK
		    ("TEST for Contiguous write_ex with Packet Data chnl OK\n");
	} else {
		DPRINTK
		    ("TEST for Contiguous write_ex with Packet Data chnl FAILED\n");

	}

	return status;
// END unit test code

#if 0
//TELMA CODE
        HW_CTRL_IPC_OPEN_T config;
        int     status = 0,
                i;
        int     iterations;
        HW_CTRL_IPC_CHANNEL_T *vchannel = NULL;
        HW_CTRL_IPC_WRITE_PARAMS_T write_buf;
        char   *wbuf;
        char   *rbuf;
        int     bytes;

        bytes = ((struct ioctl_args *) arg)->bytes;
        iterations = ((struct ioctl_args *) arg)->iterations;

        wbuf = (char *) kmalloc(bytes, GFP_KERNEL);
        rbuf = (char *) kmalloc(bytes, GFP_KERNEL);

        DPRINTK("IPC: about to send %d bytes on channel 2\n", bytes);

        config.index = 0;
        config.type = HW_CTRL_IPC_PACKET_DATA;
        config.read_callback = read_callback;
        config.write_callback = write_callback;
        config.notify_callback = notify_callback;

        vchannel = hw_ctrl_ipc_open(&config);
        if (vchannel == 0)
        {
                DPRINTK("IPC: Unable to open virtual channel %d\n", vchannel->channel_nb);
                return -1;
        }

        memset(wbuf, 0, bytes);
        memset(rbuf, 0, bytes);

        for (i = 0; i < bytes; i++)
        {
                wbuf[i] = (char) i;
        }

        write_buf.ipc_memory_read_mode = HW_CTRL_IPC_MODE_CONTIGUOUS;
        write_buf.read.cont_ptr = (HW_CTRL_IPC_CONTIGUOUS_T *)
            kmalloc(sizeof(HW_CTRL_IPC_CONTIGUOUS_T), GFP_KERNEL);
        write_buf.read.cont_ptr->data_ptr = wbuf;
        write_buf.read.cont_ptr->length = bytes;

        i = 0;
        while (i < iterations)
        {
                write_done = false;
                read_done = false;

                status = hw_ctrl_ipc_write_ex(vchannel, &write_buf);
                if (status == HW_CTRL_IPC_STATUS_ERROR)
                {
                        DPRINTK("IPC:Error in hw_ctrl_ipc_write_ex function %d\n", status);
                        status = -1;
                        break;
                }

                wait_event_interruptible(write_queue, write_done);

                status = hw_ctrl_ipc_read(vchannel, rbuf, bytes);
                if (status != HW_CTRL_IPC_STATUS_OK)
                {
                        DPRINTK("IPC:Error on hw_ctrl_ipc_read function\n");
                        status = -1;
                        break;
                }

                wait_event_interruptible(read_queue, read_done);

                DPRINTK("IPC: Received message # %d from channel 2\n", i);

                if (check_data_integrity(wbuf, rbuf, bytes) == -1)
                {
                        DPRINTK("IPC: TEST FAILED on channel %d iteration %d\n",
                                vchannel->channel_nb, i);
                        status = -1;
                        break;
                }

                memset(rbuf, 0, bytes);
                i++;
        }

        if (hw_ctrl_ipc_close(vchannel) != HW_CTRL_IPC_STATUS_OK)
        {
                DPRINTK("IPC: Error on hw_ctrl_ipc_close function\n");
                status = -1;
        }
        write_buf.read.cont_ptr->data_ptr = NULL;
        kfree(write_buf.read.cont_ptr);
        kfree(wbuf);
        kfree(rbuf);
        if (status == 0)
        {
                DPRINTK("TEST for Contiguous write_ex with Packet Data chnl OK\n");
        }
        else
        {
                DPRINTK("TEST for Contiguous write_ex with Packet Data chnl FAILED\n");

        }

        return status;
//END TELMA
#endif

}

/*================================================================================================*/
/*===== packet_data_write_ex_link_loopback =====*/
/**
@brief  This function implements the action of loopback packet data write exec link on a IPC device.

@param  inode       pointer on the node
        file        pointer on the file

@return This function returns 0.
*/
/*================================================================================================*/
int packet_data_write_ex_link_loopback(unsigned long arg)
{
        HW_CTRL_IPC_OPEN_T config;
        int     status = 0,
                i;
        int     iterations;
        HW_CTRL_IPC_CHANNEL_T *vchannel = NULL;
        HW_CTRL_IPC_WRITE_PARAMS_T write_buf;
        char   *wbuf;
        char   *rbuf;
	dma_addr_t wpaddr;
	dma_addr_t rpaddr;
        int     bytes;

        bytes = ((struct ioctl_args *) arg)->bytes;
        iterations = ((struct ioctl_args *) arg)->iterations;

	wbuf = dma_alloc_coherent(NULL, bytes, &wpaddr, GFP_DMA);
	rbuf = dma_alloc_coherent(NULL, bytes, &rpaddr, GFP_DMA);

        DPRINTK("IPC: about to send %d bytes on channel 2\n", bytes);

        config.index = 0;
        config.type = HW_CTRL_IPC_PACKET_DATA;
        config.read_callback = read_callback;
        config.write_callback = write_callback;
        config.notify_callback = notify_callback;

        vchannel = hw_ctrl_ipc_open(&config);
        if (vchannel == 0)
        {
                DPRINTK("IPC: Unable to open virtual channel %d\n", vchannel->channel_nb);
                return -1;
        }

        memset(wbuf, 0, bytes);
        memset(rbuf, 0, bytes);

        for (i = 0; i < bytes; i++)
        {
                wbuf[i] = (char) i;
        }

        write_buf.ipc_memory_read_mode = HW_CTRL_IPC_MODE_LINKED_LIST;
        write_buf.read.list_ptr = (HW_CTRL_IPC_LINKED_LIST_T *)
            kmalloc(sizeof(HW_CTRL_IPC_LINKED_LIST_T), GFP_KERNEL);
	write_buf.read.list_ptr->data_ptr = (unsigned char *)wpaddr;
        write_buf.read.list_ptr->length = bytes;
        write_buf.read.list_ptr->next = NULL;

        i = 0;
        while (i < iterations)
        {
                write_done = false;
                read_done = false;

                status = hw_ctrl_ipc_write_ex(vchannel, &write_buf);
                if (status == HW_CTRL_IPC_STATUS_ERROR)
                {
                        DPRINTK("IPC:Error in hw_ctrl_ipc_write_ex function\n");
                        status = -1;
                        break;
                }

                wait_event_interruptible(write_queue, write_done);

		status =
		    hw_ctrl_ipc_read(vchannel, (unsigned char *)rpaddr, bytes);
		if (status != HW_CTRL_IPC_STATUS_OK) {
                        DPRINTK("IPC:Error on hw_ctrl_ipc_read function\n");
                        status = -1;
                        break;
                }

                wait_event_interruptible(read_queue, read_done);

                DPRINTK("IPC: Received message # %d from channel 2\n", i);

                if (check_data_integrity(wbuf, rbuf, bytes) == -1)
                {
                        DPRINTK("IPC: TEST FAILED on channel %d iteration %d\n",
                                vchannel->channel_nb, i);
                        status = -1;
                        break;
                }

                memset(rbuf, 0, bytes);
                i++;
        }

        if (hw_ctrl_ipc_close(vchannel) != HW_CTRL_IPC_STATUS_OK)
        {
                DPRINTK("IPC: Error on hw_ctrl_ipc_close function\n");
                status = -1;
        }
        write_buf.read.list_ptr->data_ptr = NULL;
        kfree(write_buf.read.list_ptr);
	dma_free_coherent(NULL, bytes, wbuf, wpaddr);
	dma_free_coherent(NULL, bytes, rbuf, rpaddr);
	if (status == 0) {
                DPRINTK("TEST for Linked write_ex with Packet Data chnl OK\n");
        }
        else
        {
                DPRINTK("TEST for Linked write_ex with Packet Data chnl FAILED\n");
        }

        return status;
}

/*================================================================================================*/
/*===== logging_loopback =====*/
/**
@brief  This function implements the action of loopback logging method on a IPC device.

@param  inode       pointer on the node
        file        pointer on the file

@return This function returns 0.
*/
/*================================================================================================*/
int logging_loopback(unsigned long arg)
{
        return 0;
}

/*================================================================================================*/
/*===== mxc_ipc_open =====*/
/**
@brief  This function implements the open method on a IPC device.

@param  inode       pointer on the node
        file        pointer on the file

@return This function returns 0.
*/
/*================================================================================================*/
static int mxc_ipc_open(struct inode *inode, struct file *filp)
{
        return 0;
}

/*================================================================================================*/
/*===== mxc_ipc_close =====*/
/**
@brief  This function implements the release method on a IPC device.

@param  inode       pointer on the node
        file        pointer on the file

@return This function returns 0.
*/
/*================================================================================================*/
static int mxc_ipc_close(struct inode *inode, struct file *filp)
{
        unsigned int minor;

        minor = MINOR(inode->i_rdev);
        return 0;
}

/*================================================================================================*/
/*===== mxc_ipc_ioctl =====*/
/**
@brief  This function implements IOCTL controls on a IPC device driver.

@param  inode       pointer on the node
        file        pointer on the file
        cmd         the command
        arg         the parameter

@return This function returns 0 if successful.
*/
/*================================================================================================*/
static int mxc_ipc_ioctl(struct inode *inode,
                         struct file *file, unsigned int action, unsigned long arg)
{
        int     result = 0;

        //DPRINTK("IPC: ioctl to execute: %d\n", action);

        switch (action)
        {
        case OPEN_CLOSE_TEST:
                DPRINTK("mxc_ipc_ioctl: action is = OPEN_CLOSE_TEST\n");
                result = open_close_test(arg);
                break;
        case PACKET_DATA_LOOPBACK:
                DPRINTK("mxc_ipc_ioctl: action is = PACKET_DATA_LOOPBACK\n");
                result = packet_data_loopback(arg);
                break;
        case SHORT_MSG_LOOPBACK:
                DPRINTK("mxc_ipc_ioctl: action is = SHORT_MSG_LOOPBACK\n");
                result = short_message_loopback(arg);
                break;
        case PACKET_DATA_CONT_LOOPBACK:
                DPRINTK("mxc_ipc_ioctl: action is = PACKET_DATA_CONT_LOOPBACK\n");
                result = packet_data_write_ex_cont_loopback(arg);
                break;
        case PACKET_DATA_LINK_LOOPBACK:
                DPRINTK("mxc_ipc_ioctl: action is = PACKET_DATA_LINK_LOOPBACK\n");
                result = packet_data_write_ex_link_loopback(arg);
                break;
        case LOGGING_LOOPBACK:
                DPRINTK("mxc_ipc_ioctl: action is = LOGGING_LOOPBACK\n");
                result = logging_loopback(arg);
                break;
        default:
                DPRINTK("IPC: Unknown ioctl: %d\n", action);
                return -1;
        }

        return result;
}

/*==================================================================================================
                                GLOBAL VARIABLE DECLARATIONS
==================================================================================================*/
/*! This structure defines file operations for a IPC device. */
static struct file_operations ipctest_fops =
{
        owner:          THIS_MODULE,
        open:           mxc_ipc_open,
        release:        mxc_ipc_close,
        read:           mxc_ipc_read,
        write:          mxc_ipc_write,
        ioctl:          mxc_ipc_ioctl
};



/*================================================================================================*/
/*===== mxc_ipc_init =====*/
/**
@brief  This function implements the init function of the IPC device.
        This function is called when the module is loaded.

@param  None

@return This function returns 0.
*/
/*================================================================================================*/
int __init mxc_ipc_init(void)
{
        major_num = register_chrdev(0, MXC_IPC_DEV, &ipctest_fops);
        if (major_num < 0)
        {
                DPRINTK("IPC driver module is not registered\n");
                return -1;
        }

        ipc_class = class_create(THIS_MODULE, MXC_IPC_DEV);
        if (IS_ERR(ipc_class))
        {
                DPRINTK("class simple created failed\n");
                goto err_out;
        }

        if (IS_ERR(class_device_create(ipc_class, NULL, MKDEV(major_num, 0), NULL, MXC_IPC_DEV)))
        {
 	       printk(KERN_ALERT "class simple add failed\n");
               goto err_out;
        }

        //---devfs_mk_cdev(MKDEV(major_num, 0), S_IFCHR | S_IRUSR | S_IWUSR, MXC_IPC_DEV);
	      init_waitqueue_head(&write_queue);
	      init_waitqueue_head(&read_queue);
	      init_waitqueue_head(&notify_queue);

        DPRINTK("IPC driver module loaded\n");

        return 0;

err_out:
        DPRINTK("IPC : error creating test module class.\n");
        class_device_destroy(ipc_class, MKDEV(major_num, 0));
        class_destroy(ipc_class);
//        //---devfs_remove("ipctest");
        unregister_chrdev(major_num, MXC_IPC_DEV);
        return -1;

}

/*================================================================================================*/
/*===== mxc_ipc_clean =====*/
/**
@brief  This function implements the exit function of the IPC device.
        This function is called when the module is unloaded.

@param  None

@return Nothing
*/
/*================================================================================================*/
static void mxc_ipc_clean(void)
{
        unregister_chrdev(major_num, MXC_IPC_DEV);
        //---devfs_remove(MXC_IPC_DEV);

        class_device_destroy(ipc_class, MKDEV(major_num, 0));
        class_destroy(ipc_class);

        DPRINTK("IPC driver module unloaded\n");

}

/*================================================================================================*/
module_init(mxc_ipc_init);
module_exit(mxc_ipc_clean);

MODULE_AUTHOR("Freescale Semiconductor");
MODULE_DESCRIPTION("IPC module");
MODULE_LICENSE("GPL");
