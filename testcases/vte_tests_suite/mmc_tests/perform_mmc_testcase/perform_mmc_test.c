/*================================================================================================*/
/**
        @file  perform_mmc_test.c

        @brief MMC driver test scenario
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
S.ZAVJALOV/zvjs001c          22/03/2005     TLSbo46706  Initial version
A.Ozerov/b00320              20/02/2006     TLSbo61899  Testapp was cast to coding standarts
A.Ozerov/b00320              11/12/2006     TLSbo84161  Minor changes.

====================================================================================================
Portability: ARM GCC
==================================================================================================*/

/*==================================================================================================
                                        INCLUDE FILES
==================================================================================================*/
/* Standard Include Files */
#include <errno.h>

/* Harness Specific Include Files. */
#include <test.h>

/* Verification Test Environment Include Files */
#include "perform_mmc_test.h"

/*==================================================================================================
                                        LOCAL VARIABLES
==================================================================================================*/
int     fd_device = 0;
unsigned char *patten_buf = NULL,
    *read_buf = NULL;

/*==================================================================================================
                                        GLOBAL VARIABLES
==================================================================================================*/
extern int vb_mode;
extern unsigned long block_size,
        block_count,
        offset_address;
extern char *device_name;

/*==================================================================================================
                                        LOCAL FUNCTIONS
==================================================================================================*/
/*================================================================================================*/
/*===== VT_rw_mmc_setup =====*/
/** 
@brief  This function assumes the pre-condition of the test case execution

@param  none

@return On success - return TPASS 
        On failure - return TFAIL 
*/
/*================================================================================================*/
int VT_perform_mmc_setup(void)
{
        fd_device = open(device_name, O_RDWR);
        if (fd_device == -1)
        {
                tst_resm(TFAIL, "VT_rw_mmc_setup() Failed open device: %s", device_name);
                return TFAIL;
        }
        srand((unsigned int) time((time_t *) NULL));

        patten_buf = (unsigned char *) malloc(block_size * block_count);
        if (patten_buf == 0)
        {
                tst_resm(TFAIL, "VT_rw_mmc_test() Failed allocate memory");
                return TFAIL;
        }

        read_buf = (unsigned char *) malloc(block_size * block_count);
        if (read_buf == 0)
        {
                tst_resm(TFAIL, "VT_rw_mmc_test() Failed allocate memory");
                return TFAIL;
        }

        return TPASS;
}

/*================================================================================================*/
/*===== VT_rw_mmc_cleanup =====*/
/**
@brief  This function assumes the post-condition of the test case execution

@param  none

@return On success - return TPASS 
        On failure - return TFAIL 
*/
/*================================================================================================*/
int VT_perform_mmc_cleanup(void)
{
        if (patten_buf != NULL)
                free(patten_buf);
        if (read_buf != NULL)
                free(read_buf);

        if (fd_device > 0)
                if (close(fd_device) == -1)
                {
                        tst_resm(TFAIL, "VT_rw_mmc_cleanup() Failed close device");
                        return TFAIL;
                }

        return TPASS;
}

/*================================================================================================*/
/*===== read_mmc_device =====*/
/**
@brief  This function reads a data from the mmc device

@param  start_offset
        Offset start_offset

@param  size_to_read
        size of a reading data

@param  bs
        block size

@param  read_buf
        buffer to store a read data

@return On success - return TPASS
        On failure - return the error code
*/
/*================================================================================================*/
int readperform_mmc_device(unsigned long start_offset, unsigned long size_to_read, unsigned long bs,
                    unsigned char *read_buf)
{
        unsigned long bytes2read;
	 long interval,suminterval;
	 struct timeval tv2,tv1;
	 double readspeed;
	 double kbyte=1000000/1024;
	 int loopcount=5;
	 int i;

	 for(i=0;i<loopcount;i++)
	 {
	        if (lseek(fd_device, start_offset * bs, SEEK_SET) != (start_offset * bs))
	        {
	                tst_resm(TFAIL,
	                         "read_mmc_device() Failed repositions the offset of the file descriptor");
	                return TFAIL;
	        }
		gettimeofday(&tv1, NULL);
	        bytes2read = read(fd_device, read_buf, size_to_read * bs);
		gettimeofday(&tv2,NULL);
		interval=(tv2.tv_sec-tv1.tv_sec)*1000000+(tv2.tv_usec-tv1.tv_usec);
		
	        if (interval <=0 ||bytes2read != (size_to_read * bs))
	        {
	                tst_resm(TFAIL, "read_mmc_device() Failed read from device");
	                return TFAIL;
	        }
		 else
		 	{
			suminterval+=interval;
		 }
		
	 }
	readspeed=(kbyte*((double)bytes2read*loopcount)/suminterval)/1024;
	tst_resm(TINFO,"MMC read speed %lf MByte/sec",readspeed);
	
        return TPASS;
}

/*================================================================================================*/
/*===== write_mmc_device =====*/
/**
@brief  This function writes a data to the mmc device 

@param  start_offset
        Offset start_offset

@param  size_to_write
        size of a writing data

@param  bs
        block size

@param  write_buf
        buffer contains data to write to the device

@return On success - return TPASS
        On failure - return the error code
*/
/*================================================================================================*/
int writeperform_mmc_device(unsigned long start_offset, unsigned long size_to_write, unsigned long bs,
                     unsigned char *write_buf)
{
        unsigned long bytes2write;
	long interval,suminterval;
	 struct timeval tv2,tv1;
	 double writespeed;
	 double kbyte=1000000/1024;
	 int loopcount=5;
	 int i;

	 for(i=0;i<loopcount;i++)
	 {
	        if (lseek(fd_device, start_offset * bs, SEEK_SET) != (start_offset * bs))
	        {
	                tst_resm(TFAIL,
	                         "write_mmc_device() Failed repositions the offset of the file descriptor");
	                return TFAIL;
	        }
		gettimeofday(&tv1, NULL);
	        bytes2write = write(fd_device, write_buf, size_to_write * bs);
		gettimeofday(&tv2,NULL);
		interval=(tv2.tv_sec-tv1.tv_sec)*1000000+(tv2.tv_usec-tv1.tv_usec);
		
	        if (interval<=0 || bytes2write != (size_to_write * bs))
	        {
	                tst_resm(TFAIL, "write_mmc_device() Failed write to device");
	                return TFAIL;
	        }
		else
		{
			suminterval+=interval;	
		}
			
	 }
	
		writespeed=(kbyte*((double)bytes2write*loopcount)/suminterval)/1024;	
		tst_resm(TINFO,"MMC write speed %lf MByte/sec",writespeed);
	
        return TPASS;
}

/*================================================================================================*/
/*===== VT_rw_mmc_test =====*/
/**
@brief  MMC Driver test scenario function

@param  None
    
@return On success - return TPASS
        On failure - return the error code
*/
/*================================================================================================*/
int VT_perform_mmc_test(void)
{
        unsigned long i;

        if (vb_mode)
                tst_resm(TINFO, "Fill pattern buffer");

        for (i = 0; i < (block_count * block_size); i++)
                patten_buf[i] = (unsigned char) (random() * 0xFF);

        if (vb_mode)
                tst_resm(TINFO, "Write to %s", device_name);

        if (writeperform_mmc_device(offset_address, block_count, block_size, patten_buf) == TFAIL)
                return TFAIL;

        if (vb_mode)
                tst_resm(TINFO, "Read from %s", device_name);

        if (readperform_mmc_device(offset_address, block_count, block_size, read_buf) == TFAIL)
                return TFAIL;
/*
        if (vb_mode)
                tst_resm(TINFO, "Compare results");

        if (strncmp((char *) patten_buf, (char *) read_buf, block_size * block_count) != 0)
        {
                tst_resm(TFAIL, "VT_rw_mmc_test() Verify failed");
                return TFAIL;
        }

	 */

        return TPASS;
}
