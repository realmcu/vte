/***
**Copyright 2004-2009 Freescale Semiconductor, Inc. All Rights Reserved.
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
        @file   nand_mtd_test.c

        @brief  NAND MTD test
====================================================================================================
Revision History:
                            Modification     Tracking
Author/core ID                  Date          Number    Description of Changes
-------------------------   ------------    ----------  -------------------------------------------
S.ZAVJALOV/ZVJS001C          27/07/2004     TLSbo40261  Initial version
V.BECKER/rc023c              21/10/2004     TLSbo43924  Correct test VT_nand_mtd_test_wrner
                                                        because it should return PASS
E.Gromazina                  07/07/2005     TLSbo50888  minor fixes
A.Ozerov/b00320              19/04/2006     TLSbo61865  Change erase_flash_block and VT_nand_mtd_test_wrner function.
A.Ozerov/b00320              11/12/2006     TLSbo84161  Minor changes.
Rakesh S Joshi/r65956        25/07/2007     ENGR42511   Fix segmentation error when
Ziye Yang/b21182       	     13/08/2008     N/A         Minor changes.
														erase memory on MXC30031-ADS.

====================================================================================================
Portability:  ARM GCC
==================================================================================================*/

/*==================================================================================================
                                        INCLUDE FILES
==================================================================================================*/
#include "nand_mtd_test.h"
#include <errno.h>
#include <linux/version.h>
/*==================================================================================================
                                        GLOBAL VARIABLES
==================================================================================================*/
extern char *TCID;
extern char device_name[];
extern int h_num,
           v_num;
extern long addr_offset;
extern long length_tmem;
extern int fullPageFlag;
extern long writepage;
struct mtd_info_user mxc_info_mtd;

long numblocks;
long offset;

int     file_desc = 0;
char   *rw_buf = 0;
loff_t offs;


/*==================================================================================================
                                    FUNCTION PROTOTYPES
==================================================================================================*/
int     erase_flash_block(long offset, long length);
int     read_flash_block(long offset, long length, char *mem);
int     write_flash_block(long offset, long length, char *mem);

/*================================================================================================*/
int VT_nand_mtd_test_info(void)
{
        int     number_of_regions = 0;
        struct  region_info_user mxc_region_mtd;
        char    flash_type[64];
        int     i;

        switch (mxc_info_mtd.type)
        {
        case MTD_ABSENT:
                strcpy(flash_type, "MTD_ABSENT");
                break;
        case MTD_RAM:
                strcpy(flash_type, "MTD_RAM");
                break;
        case MTD_ROM:
                strcpy(flash_type, "MTD_ROMMTD_ROM");
                break;
        case MTD_NORFLASH:
                strcpy(flash_type, "MTD_NORFLASH");
                break;
        case MTD_NANDFLASH:
                strcpy(flash_type, "MTD_NANDFLASH");
                break;
        default:
                strcpy(flash_type, "MTD_UNKNOWN");
                break;
        }
		#if ( LINUX_VERSION_CODE  <= KERNEL_VERSION(3,0,15)) 
        tst_resm(TINFO, "\nFlash info :\n \
                - type : %s\n \
                - flags : %d\n \
                - size : 0x%lx\n \
                - erase size : 0x%lx\n\
                - write size : 0x%lx\n\
                - size of OOB block : 0x%lx\n \
                - ECC type : %d\n\
                - ECC size : 0x%lx", flash_type, mxc_info_mtd.flags, mxc_info_mtd.size, mxc_info_mtd.erasesize, mxc_info_mtd.writesize, mxc_info_mtd.oobsize, mxc_info_mtd.ecctype, mxc_info_mtd.eccsize);
		#endif
        /* Get number of erase regions on flash device */
        if (v_num)
        {
                tst_resm(TINFO, "Get number of regions in flash memory");
        }
        if ((ioctl(file_desc, MEMGETREGIONCOUNT, &number_of_regions)) != 0)
        {
                tst_resm(TFAIL, "VT_nand_mtd_test() Failed ioctl MEMGETREGIONCOUNT");
                return TFAIL;
        }
        tst_resm(TINFO, "\nNumber of regions in Flash memory : %d", number_of_regions);

        /* Get memory region information */
        if (v_num)
        {
                tst_resm(TINFO, "Get memory region information");
        }
        // Victor Cui modify: for the bug of kernel upgrade from 26 to 28
	//                    if  number_of_regions=0, virtual address and Segmentation fault
	//for (i = 0; i <= number_of_regions; i++)
        for (i = 0; i < number_of_regions; i++)
        {
                mxc_region_mtd.regionindex = i;
                if ((ioctl(file_desc, MEMGETREGIONINFO, &mxc_region_mtd) != 0) && (errno != EINVAL))
                {
                        tst_resm(TFAIL, "VT_nand_mtd_test() Failed ioctl MEMGETREGIONINFO");
                        return TFAIL;
                }
                tst_resm(TINFO, "\nInformation on memory region index %d :\n \
                - number of blocks : 0x%lx\n \
                - erase size : 0x%lx\n \
                - offset : 0x%lx", i, mxc_region_mtd.numblocks, mxc_region_mtd.erasesize, mxc_region_mtd.offset);
        }

        return TPASS;
}


int VT_nand_mtd_test_regionInfo(void)
{
        int     number_of_regions = 0;
        struct region_info_user mxc_region_info;
        char    flash_type[64];
        int     i;


	
        switch (mxc_info_mtd.type)
        {
        case MTD_ABSENT:
                strcpy(flash_type, "MTD_ABSENT");
                break;
        case MTD_RAM:
                strcpy(flash_type, "MTD_RAM");
                break;
        case MTD_ROM:
                strcpy(flash_type, "MTD_ROMMTD_ROM");
                break;
        case MTD_NORFLASH:
                strcpy(flash_type, "MTD_NORFLASH");
                break;
        case MTD_NANDFLASH:
                strcpy(flash_type, "MTD_NANDFLASH");
                break;
        default:
                strcpy(flash_type, "MTD_UNKNOWN");
                break;
        }
		#if ( LINUX_VERSION_CODE <= KERNEL_VERSION(3,0,15)) 
		tst_resm(TINFO, "\nFlash info :\n \
                - type : %s\n \
                - flags : %d\n \
                - size : 0x%lx\n \
                - erase size : 0x%lx\n\
                - write size : 0x%lx\n\
                - size of OOB block : 0x%lx\n \
                - ECC type : %d\n\
                - ECC size : 0x%lx", flash_type, mxc_info_mtd.flags, mxc_info_mtd.size, mxc_info_mtd.erasesize, mxc_info_mtd.writesize, mxc_info_mtd.oobsize, mxc_info_mtd.ecctype, mxc_info_mtd.eccsize);
		#endif
        /* Get number of erase regions on flash device */
        if (v_num)
        {
                tst_resm(TINFO, "Get number of regions in flash memory");
        }
        if ((ioctl(file_desc, MEMGETREGIONCOUNT, &number_of_regions)) != 0)
        {
                tst_resm(TFAIL, "VT_nand_mtd_test() Failed ioctl MEMGETREGIONCOUNT");
                return TFAIL;
        }
        tst_resm(TINFO, "\nNumber of regions in Flash memory : %d", number_of_regions);

        /* Get memory region information */
        if (v_num)
        {
                tst_resm(TINFO, "Get memory region information");
        }
	//Victor Cui modify: for the bug of kernel upgrade from 26 to 28
	//                    if  number_of_regions=0, virtual address and Segmentation fault
	//for (i = 0; i <= number_of_regions; i++)
        for (i = 0; i < number_of_regions; i++)
        {
                mxc_region_info.regionindex = i;
                if ((ioctl(file_desc, MEMGETREGIONINFO, &mxc_region_info) != 0) && (errno != EINVAL))
                {
                        tst_resm(TFAIL, "VT_nand_mtd_test() Failed ioctl MEMGETREGIONINFO");
                        return TFAIL;
                }
                tst_resm(TINFO, "\nInformation on memory region index %d :\n \
                - number of blocks : 0x%lx\n \
                - erase size : 0x%lx\n \
                - offset : 0x%lx", i, mxc_region_info.numblocks, mxc_region_info.erasesize, mxc_region_info.offset);
        }
	numblocks=mxc_region_info.numblocks;
	offset=mxc_region_info.offset;
	
	return TPASS;
	 
}


/*================================================================================================*/
int VT_nand_mtd_test_wrner(void)
{

        /* Write */
        if ((pwrite(file_desc, rw_buf, length_tmem, addr_offset)) < 0)
        {
                if (v_num)
                {
                        perror("Unable write into flash memory");
                }
                return TFAIL;
        }
        //return TFAIL;
        return TPASS;
}

/*================================================================================================*/
int VT_nand_mtd_test_rdrw(void)
{
#ifdef CONFIG_ARCH_MXC92323
		if (addr_offset % mxc_info_mtd.erasesize)
		{
			 tst_resm(TFAIL,"\n INVALID -A option , it should be multiple of %d ",mxc_info_mtd.erasesize);
			return TFAIL;
		}
#endif
        /* Read */
        if (v_num)
        {
                tst_resm(TINFO, "Performing read memory region 0x%lx - 0x%lx", addr_offset, addr_offset + length_tmem);
        }
        if ((read_flash_block(addr_offset, length_tmem, rw_buf)) == TFAIL)
        {
                tst_resm(TFAIL, "VT_nand_mtd_test() Failed read device");
                return TFAIL;
        }

        /* Erase */
        if (v_num)
        {
                tst_resm(TINFO, "Performing erase memory region 0x%lx - 0x%lx", addr_offset, addr_offset + length_tmem);
        }
        if (erase_flash_block(addr_offset, length_tmem) == TFAIL)
        {
                tst_resm(TFAIL, "VT_nand_mtd_test() Failed erase device, error: %s", strerror(errno));
                return TFAIL;
        }

        /* Write */
        if (v_num)
        {
                tst_resm(TINFO, "Performing write memory region 0x%lx", addr_offset);
        }
        if ((write_flash_block(addr_offset, length_tmem, rw_buf)) == TFAIL)
        {
                tst_resm(TFAIL, "VT_nand_mtd_test() Failed write into device, error: %s", strerror(errno));
                return TFAIL;
        }

        return TPASS;
}

/*check nand bad block and unlock, lock ioctl*/  

int VT_nand_mtd_test_badblk(void)
{
	int i,goodBlockFlag=0;
	struct erase_info_user  erase;
	int rc= TPASS;
	char *rwbuff;
	
	tst_resm(TINFO,"numblocks 0x%lx, offset 0x%lx\n",numblocks,offset);


	offs=offset;
	
	/*get good block*/
	for (i=0;i<=numblocks;i++)
	{
		erase.start=i*mxc_info_mtd.erasesize;
		erase.length=mxc_info_mtd.erasesize;
		offs=offs+i*mxc_info_mtd.erasesize;
		if((ioctl(file_desc, MEMGETBADBLOCK, &offs)) != 0)
		{
			tst_resm(TINFO,"VT_nand_mtd_test_badblk failed ioctl MEMGETBADBLOCK");
			continue;
		}
		else
		{
			tst_resm(TINFO,"get good block, offs=%d !",offs);
			goodBlockFlag=1;
			break;
		}
	}

	if(i>numblocks)
	{
		tst_resm(TINFO,"There is no good block in region");
		return TPASS;
	}

	if (!(rwbuff = (char *) malloc(sizeof(char) * (mxc_info_mtd.erasesize))))
        {
                tst_resm(TFAIL, "VT_nand_mtd_test_badblk() Failed allocate memory");
                return TFAIL;
        }

	int readFlag=0,writeFlag=0,eraseFlag=0;
	
	/*Lock this good block, try to read/write/erase. read/write/erase failure, then unlock this block. try to read/write/erase again*/
	if(goodBlockFlag)
	{
		/*lock this good block, actually no lock support in the kernel*/
		if(ioctl(file_desc,MEMLOCK,&erase)!=0)
		{
			tst_resm(TINFO,"MTD lock failure,since there is no nand lock support in kerne!");
			rc = TPASS;
		}else
		{

			/*If lock success,then read/write/erase*/
			if (pread(file_desc, rwbuff, erase.length, erase.start) < 0)
        		{
        			tst_resm(TINFO,"read failure after lock this block!");
        		}
			else
			{
				tst_resm(TINFO,"Read success after lock this block!");
				readFlag = 1;
			}

			if ((ioctl(file_desc, MEMERASE, &erase)) != 0)
              		{
                        	tst_resm(TINFO, "Unable to erase after lock this block");
              		}
			else
			{
				tst_resm(TINFO,"Erase success after lock this block");
				eraseFlag=1;
			}

			if ((pwrite(file_desc,rwbuff, erase.length, erase.start)) < 0)
			{
				tst_resm(TINFO,"Write failure after lock this block");
			}
			else
			{
				tst_resm(TINFO,"Write success after lock this block!");
				writeFlag=1;
			}

			if(readFlag ||writeFlag || eraseFlag)
			{
				tst_resm(TFAIL,"lock block falure because the locked block is available to rea					d/write/erase!");
				rc = TFAIL;
			}
		}
	}	

	if(rwbuff!=0)
		free(rwbuff);
	
	return rc;
}

int VT_nand_mtd_test_thrdrwe(void)
{

	int i,numOfRead;
	numOfRead=10;

	for(i=0;i<numOfRead;i++)
	{
		/* Read */
	        if (v_num)
	        {
	                tst_resm(TINFO, "Performing read memory region 0x%lx - 0x%lx", addr_offset, addr_offset + length_tmem);
	        }
	        if ((read_flash_block(addr_offset, length_tmem, rw_buf)) == TFAIL)
	        {
	                tst_resm(TFAIL, "VT_nand_mtd_test() Failed read device");
	                return TFAIL;
	        }

	        /* Erase */
	        if (v_num)
	        {
	                tst_resm(TINFO, "Performing erase memory region 0x%lx - 0x%lx", addr_offset, addr_offset + length_tmem);
	        }
	        if (erase_flash_block(addr_offset, length_tmem) == TFAIL)
	        {
	                tst_resm(TFAIL, "VT_nand_mtd_test() Failed erase device, error: %s", strerror(errno));
	                return TFAIL;
	        }

	        /* Write */
	        if (v_num)
	        {
	                tst_resm(TINFO, "Performing write memory region 0x%lx", addr_offset);
	        }
	        if ((write_flash_block(addr_offset, length_tmem, rw_buf)) == TFAIL)
	        {
	                tst_resm(TFAIL, "VT_nand_mtd_test() Failed write into device, error: %s", strerror(errno));
	                return TFAIL;
	        }
	}

		return TPASS;	
}


int VT_nand_mtd_test_thrdrwonepage(void)
{
	int i,numOfRead;
	#ifdef PROJECT_MX37
	i=0;
	#endif
	numOfRead=10;

	for(i=0;i<numOfRead;i++)
	{
		/* Read */
	        if (v_num)
	        {
	                tst_resm(TINFO, "Performing read memory region 0x%lx - 0x%lx", addr_offset, addr_offset + length_tmem);
	        }
	        if ((read_flash_block(addr_offset, length_tmem, rw_buf)) == TFAIL)
	        {
	                tst_resm(TFAIL, "VT_nand_mtd_test() Failed read device");
	                return TFAIL;
	        }

	        /* Write */
	        if (v_num)
	        {
	                tst_resm(TINFO, "Performing write memory region 0x%lx", addr_offset);
	        }
	        if ((write_flash_block(addr_offset, length_tmem, rw_buf)) == TFAIL)
	        {
	                tst_resm(TFAIL, "VT_nand_mtd_test() Failed write into device, error: %s", strerror(errno));
	                return TFAIL;
	        }
	}

	return TPASS;	

}


//test nand performance 

int VT_nand_mtd_test_perform(void)
{
	//unsigned long bcount;
	unsigned long lcount;
	unsigned char *buf = NULL;
	struct timeval tv1,tv2;
	long   readinterval=0,writeinterval=0,eraseinterval=0,sumEraseInterval=0,sumWriteInterval=0;
	double writespeed=0,readspeed=0,erasespeed=0;
	double kbyte=1000000/1024.0;
	// Victor modify 17/11/2008
	//int readloop=1000,writeloop=1000,eraseloop=1000;
	int readloop=100,writeloop=100,eraseloop=100;
	#ifdef PROJECT_MAYLEY	
	long writepage;
	#else
	long writepage=mxc_info_mtd.writesize;
	#endif
	struct  erase_info_user mxc_erase_mtd;
	
	#ifdef PROJECT_MX37
	if(fullPageFlag)
	{
		//full page performance
		writepage=mxc_info_mtd.writesize;
	}
	else
	{
		//half page performance
		writepage=mxc_info_mtd.writesize/2;
	}
	#endif
	if(writepage<=0)
	{
		tst_resm(TFAIL,"get write page size failed");
		return TFAIL;
	}
	//allocate buffer for read or write
	if (!(buf = (char *) malloc(sizeof(char) * (writepage))))
        {
                tst_resm(TFAIL, "nand performance test() Failed allocate memory");
                return TFAIL;
        }

	tst_resm(TINFO,"nand performance test");

	//test everage read speed
	//start count on begin of read
	#ifndef PROJECT_MX37
	gettimeofday(&tv1, NULL);

	for(lcount=0;lcount<writeloop;lcount++)
	{
		//if(pwrite(file_desc, rw_buf, length_tmem, addr_offset) < 0)
		// Victor modify 17/11/2008
		offset=addr_offset+lcount*writepage;

		//if(pwrite(file_desc, buf, writepage, addr_offset) < 0)
		if(pwrite(file_desc, buf, writepage, offset) < 0)
		{
			tst_resm(TFAIL,"Write failure!");
			return TFAIL;
		}
	}
	
	//get time on end of write
	gettimeofday(&tv2, NULL);

	//get interval of write
	writeinterval=(tv2.tv_sec-tv1.tv_sec)*1000000+(tv2.tv_usec-tv1.tv_usec);

	if(writeinterval!=0)
	{

		//calculate write speed 
		writespeed=(double)(kbyte*((double)(writepage*writeloop))/(double)((writeinterval)));
	}

	tst_resm(TINFO,"nand page everage write speed:%lf KBps",writespeed);
	#endif
	//end of test write speed

	//test everage read speed
	//start count on begin of read
	gettimeofday(&tv1, NULL);

	for(lcount=0;lcount<readloop;lcount++)
	{
		//offset=offset+lcount*writepage;
		//if(pread(file_desc, buf, writepage, addr_offset) < 0)
		// Victor modify 17/11/2008
		offset=addr_offset+lcount*writepage;
		
		//if(pread(file_desc, buf, writepage, addr_offset) < 0)
		if(pread(file_desc, buf, writepage, offset) < 0)
		{
			tst_resm(TFAIL,"Read failure!");
			return TFAIL;
		}
	}
	
	//get time on end of write
	gettimeofday(&tv2, NULL);

	//get interval of write
	readinterval=(tv2.tv_sec-tv1.tv_sec)*1000000+(tv2.tv_usec-tv1.tv_usec);

	if(readinterval!=0)
	{

		//calculate write speed 
		readspeed=(double)(kbyte*((double)(writepage*readloop))/(double)((readinterval)));
	}
	#ifndef PROJECT_MX37
	tst_resm(TINFO,"nand page everage read speed:%lf KBps",readspeed);
	#endif
	//end of test read speed


	 

	//test write speed 
	//get time on start of write
	#ifdef PROJECT_MARELY
	//gettimeofday(&tv1, NULL);
	#else
		gettimeofday(&tv1, NULL);
	#endif
	#ifdef PROJECT_MX37
	for(lcount=0;lcount<writeloop;lcount++)
	#else
		for(lcount=0;lcount<eraseloop;lcount++)
	#endif

	{
		// Victor modify 17/11/2008
		//mxc_erase_mtd.start = addr_offset;
		mxc_erase_mtd.start = addr_offset+lcount*mxc_info_mtd.erasesize;
		mxc_erase_mtd.length = mxc_info_mtd.erasesize;
	#ifdef PROJECT_MX37
		gettimeofday(&tv1, NULL);
	#endif
		if (ioctl(file_desc, MEMERASE, &mxc_erase_mtd) != 0)
		{
			// Victor modify 17/11/2008
			//tst_resm(TFAIL, "%s: MTD Erase failure: %s", file_desc, strerror(errno));
			tst_resm(TFAIL, "%d: MTD Erase failure: %s", file_desc, strerror(errno));
					
			return TFAIL;
		}
	#ifdef PROJECT_MX37
		gettimeofday(&tv2, NULL);
		//get erase time
		eraseinterval=(tv2.tv_sec-tv1.tv_sec)*1000000+(tv2.tv_usec-tv1.tv_usec);
		if(eraseinterval>0)
		{
			sumEraseInterval=sumEraseInterval+eraseinterval;
	#endif
		}
	#ifdef PROJECT_MX37
		gettimeofday(&tv1, NULL);
		//if(pwrite(file_desc, rw_buf, length_tmem, addr_offset) < 0)
		if(pwrite(file_desc, buf, writepage, addr_offset) < 0)
		{
			tst_resm(TFAIL,"Write failure!");
			return TFAIL;
		}
	#endif
		gettimeofday(&tv2, NULL);
		//get write time
	#ifdef PROJECT_MX37
		writeinterval=(tv2.tv_sec-tv1.tv_sec)*1000000+(tv2.tv_usec-tv1.tv_usec);
		if(writeinterval>0)
		{
			sumWriteInterval=sumWriteInterval+writeinterval;
		}
	}



	if(sumWriteInterval!=0)
	{

		//calculate write speed 
		writespeed=(double)(kbyte*((double)(writepage*writeloop))/(double)((sumWriteInterval)));
	}




	//end of test write speed
	


	if(sumEraseInterval!=0)
	{

		//calculate write speed 
		erasespeed=(double)(kbyte*((double)(mxc_info_mtd.erasesize*eraseloop))/(double)((sumEraseInterval)));
	}
	#else
		//get interval of write
		eraseinterval=(tv2.tv_sec-tv1.tv_sec)*1000000+(tv2.tv_usec-tv1.tv_usec);
	
		if(eraseinterval!=0)
		{
	
			//calculate write speed 
			erasespeed=(double)(kbyte*((double)(writepage*eraseloop))/(double)((eraseinterval)));
		}
		tst_resm(TINFO,"nand page everage erase speed:%lf KBps",erasespeed);
	
	#endif
	//end of test erase speed 
	#ifdef PROJECT_MX37
	if(fullPageFlag)
	{
		tst_resm(TINFO,"nand page everage read speed:%lf KBps",readspeed);
		tst_resm(TINFO,"nand page everage write speed:%lf KBps",writespeed);
		tst_resm(TINFO,"nand page everage erase speed:%lf KBps",erasespeed);
	}
	else
	{
		tst_resm(TINFO,"nand halfpage everage read speed:%lf KBps",readspeed);
		tst_resm(TINFO,"nand halfpage everage write speed:%lf KBps",writespeed);
		tst_resm(TINFO,"nand halfpage everage erase speed:%lf KBps",erasespeed);	
	}
	#endif	

	 if (buf != 0)
                free(buf);

	return TPASS;

}



/*================================================================================================*/
int VT_nand_mtd_test_setup(void)
{
		tst_resm(TINFO, " Open NAND MTD driver : %s\n", device_name);
        file_desc = open(device_name, O_RDWR);
        sleep(1);
        if (file_desc < 0)
        {
                tst_resm(TFAIL, "VT_nand_mtd_test_setup() Failed open device");
                return TFAIL;
        }
        if (v_num)
        {
                tst_resm(TINFO, "Allocate memory for read buffer");
        }
        if (!(rw_buf = (char *) malloc(sizeof(char) * (length_tmem))))
        {
                tst_resm(TFAIL, "VT_nand_mtd_test() Failed allocate memory");
                return TFAIL;
        }

        /* Get MTD flash information */
        if (v_num)
        {
                tst_resm(TINFO, "Get MTD flash information");
        }
        if ((ioctl(file_desc, MEMGETINFO, &mxc_info_mtd)) != 0)
        {
                tst_resm(TFAIL, "VT_nand_mtd_test() Failed ioctl MEMGETINFO");
                return TFAIL;
        }

        return TPASS;
}

/*================================================================================================*/
int VT_nand_mtd_test_cleanup(void)
{
        if (file_desc > 0)
                close(file_desc);
        if (rw_buf != 0)
                free(rw_buf);

        return TPASS;
}

/*================================================================================================*/
int read_flash_block(long offset, long length, char *mem)
{
        /* Read */
        if (pread(file_desc, mem, length, offset) < 0)
        {
                return TFAIL;
        }
        return TPASS;
}

/*================================================================================================*/
int write_flash_block(long offset, long length, char *mem)
{
        char   *temp_buf;

        /* Write */
        if ((pwrite(file_desc, mem, length, offset)) < 0)
        {
                return TFAIL;
        }
        /* Verify */
        if (v_num)
        {
                tst_resm(TINFO, "Verify written region");
        }
        if (!(temp_buf = (char *) malloc(length * sizeof(char))))
        {
                tst_resm(TFAIL, "WRITE: CAN'T ALLOCATE MEMORY");
                return TFAIL;
        }
        if ((read_flash_block(offset, length, temp_buf)) == TFAIL)
        {
                tst_resm(TFAIL, "WRITE: READ ERROR");
                free(temp_buf);
                return TFAIL;
        }
        if ((memcmp(temp_buf, mem, length) != 0))
        {
                tst_resm(TFAIL, "WRITE: memcmp != 0");
                free(temp_buf);
                return TFAIL;
        }
        free(temp_buf);
        return TPASS;
}

/*================================================================================================*/
int erase_flash_block(long offset, long length)
{
        struct  erase_info_user mxc_erase_mtd;
        char   *temp_buf;
        int     i, j;


        if(offset + length > mxc_info_mtd.size)
        {
                tst_resm(TWARN,"ERASE: Region Is Out Of Range");
        }

        if (!(temp_buf = (char *) malloc(mxc_info_mtd.erasesize * sizeof(char))))
        {
                tst_resm(TFAIL, "ERASE: CAN'T ALLOCATE MEMORY");
                return TFAIL;
        }

        /* Erase */
        for (i = offset; i < (offset + length); i += mxc_info_mtd.erasesize)
        {
                loff_t o_ffset = i;
                int ret = ioctl(file_desc, MEMGETBADBLOCK, &o_ffset);
                if (ret > 0)
                {
                        tst_resm(TWARN, "Skipping bad block at 0x%08x", i);
                        continue;
                }
                else if (ret == -EOPNOTSUPP)
                {
                        tst_resm(TWARN, "%s: Bad block check not available", file_desc);
                }
                else if (ret < 0)
                {
                        tst_resm(TFAIL, "%s: MTD get bad block failed: %s", file_desc, strerror(errno));
                        exit(1);
                }
                else
                {
                        mxc_erase_mtd.start = i;
                        mxc_erase_mtd.length = mxc_info_mtd.erasesize;
                        if (ioctl(file_desc, MEMERASE, &mxc_erase_mtd) != 0)
                        {
                                tst_resm(TFAIL, "%s: MTD Erase failure: %s", file_desc, strerror(errno));
                                free(temp_buf);
                                return TFAIL;
                        }

                /* Verify */
                if (v_num && i == offset)
                {
                        tst_resm(TINFO, "Verify erazed region");
                }

                if ((read_flash_block(i, mxc_info_mtd.erasesize, temp_buf)) == TFAIL)
                {
                        tst_resm(TFAIL, "ERASE: READ ERROR");
                        free(temp_buf);
                        return TFAIL;
                }
                for (j = 0; j < mxc_info_mtd.erasesize; j++)
                {
                        if ((unsigned char)(temp_buf[j]) != 0xFF)
                        {
                                tst_resm(TFAIL, "ERASE: temp_buf[%d] = 0x%x != 0xFF(255)", j, temp_buf[j]);
                                free(temp_buf);
                                return TFAIL;
                        }
                }
                }
        }

        free(temp_buf);
        return TPASS;
}
