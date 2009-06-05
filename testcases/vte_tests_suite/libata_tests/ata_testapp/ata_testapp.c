/*================================================================================================*/
/**
        @file   ata_driver_testapp.c

        @brief  Source file for ATA Disk driver test.
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
A.Ozerov/b00320              10/09/2006     TLSbo76800  Initial version.
D.Kazachkov/b00316            6/12/2006     TLSbo80788  Cosmetic fix


====================================================================================================
Portability: ARM GCC
==================================================================================================*/

/*==================================================================================================
                                        INCLUDE FILES
==================================================================================================*/
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <ctype.h>
#include <endian.h>
#include <sys/ioctl.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/sysmacros.h>
#include <sys/time.h>
#include <sys/times.h>
#include <sys/types.h>
#include <sys/mount.h>
#include <linux/types.h>
#include <linux/hdreg.h>
#include <linux/major.h>
#include <asm/byteorder.h>
//#include <linux/ata.h>

#include <test.h>

/* Verification Test Environment Include Files */
#include "ata_testapp.h"

/*==================================================================================================
                                        GLOBAL VARIABLES
===================================================================================================*/
int     fd = 0;

extern const char *minor_str[];
char   *progname;
static int do_writing = 0,
    do_timings = 0;
static int get_IDentity = 0;
static int open_flags = O_RDWR | O_NONBLOCK;

const char *cfg_str[] = 
{       "", " HardSect", " SoftSect", " NotMFM",
        " HdSw>15uSec", " SpinMotCtl", " Fixed", " Removeable",
        " DTR<=5Mbs", " DTR>5Mbs", " DTR>10Mbs", " RotSpdTol>.5%",
        " dStbOff", " TrkOff", " FmtGapReq", " nonMagnetic"
};

const char *SlowMedFast[] = { "slow", "medium", "fast", "eide", "ata" };
const char *BuffType[] = { "unknown", "1Sect", "DualPort", "DualPortCache" };

/*==================================================================================================
                                        LOCAL FUNCTIONS
==================================================================================================*/

/*================================================================================================*/
/*===== VT_ata_driver_test_setup =====*/
/**
@brief  This function assumes the pre-condition of the test case execution

@param  None.

@return On success - return TPASS.
        On failure - return the error code.
*/
/*================================================================================================*/
int VT_ata_driver_test_setup(void)
{
        return TPASS;
}

/*================================================================================================*/
/*===== VT_ata_driver_test_cleanup =====*/
/**
@brief  This function assumes the post-condition of the test case execution

@param  None.

@return None.
*/
/*================================================================================================*/
int VT_ata_driver_test_cleanup(void)
{
        return TPASS;
}

/*================================================================================================*/
static void dmpstr(const char *prefix, unsigned int i, const char *s[], unsigned int maxi)
{
        if (i > maxi)
                tst_resm(TINFO, "%s%u", prefix, i);
        else
                tst_resm(TINFO, "%s%s", prefix, s[i]);
}

/*================================================================================================*/
static void dump_identity(const struct hd_driveid *id)
{
        int     i;
        char    pmodes[64] = { 0, }, 
                dmodes[128] = { 0,}, 
                umodes[128] = { 0,};

        const unsigned short int *id_regs = (const void *) id;
        unsigned long capacity;

        tst_resm(TINFO, "Model=%.40s, FwRev=%.8s, SerialNo=%.20s", id->model, id->fw_rev, id->serial_no);
        printf("Config={");
        for (i = 0; i <= 15; i++)
        {
                if (id->config & (1 << i))
                        printf("%s", cfg_str[i]);
        }
        printf("}\n");
        tst_resm(TINFO, "RawCHS=%u/%u/%u, TrkSize=%u, SectSize=%u, ECCbytes=%u",
               id->cyls, id->heads, id->sectors, id->track_bytes, id->sector_bytes, id->ecc_bytes);
        dmpstr("BuffType=", id->buf_type, BuffType, 3);
        tst_resm(TINFO, "BuffSize=%ukB, MaxMultSect=%u", id->buf_size / 2, id->max_multsect);
        if (id->max_multsect)
        {
                if (!(id->multsect_valid & 1))
                        tst_resm(TINFO, "MultSect=?%u?", id->multsect);
                else if (id->multsect)
                        tst_resm(TINFO, "MultSect=%u", id->multsect);
                else
                        tst_resm(TINFO, "off");
        }
        if (id->tPIO <= 5)
        {
                strcat(pmodes, "pio0 ");
                if (id->tPIO >= 1)
                        strcat(pmodes, "pio1 ");
                if (id->tPIO >= 2)
                        strcat(pmodes, "pio2 ");
        }
        if (!(id->field_valid & 1))
                tst_resm(TINFO, "(maybe):");
        capacity = (id->cur_capacity1 << 16) | id->cur_capacity0;
        tst_resm(TINFO, "CurCHS=%u/%u/%u, CurSects=%lu", id->cur_cyls, id->cur_heads, id->cur_sectors,
               capacity);
        tst_resm(TINFO, ", LBA=%s", YN(id->capability & 2));
        if (id->capability & 2)
                tst_resm(TINFO, ", LBAsects=%u", id->lba_capacity);

        if (id->capability & 1)
        {
                if (id->dma_1word | id->dma_mword)
                {
                        if (id->dma_1word & 0x100)
                                strcat(dmodes, "*");
                        if (id->dma_1word & 1)
                                strcat(dmodes, "sdma0 ");
                        if (id->dma_1word & 0x200)
                                strcat(dmodes, "*");
                        if (id->dma_1word & 2)
                                strcat(dmodes, "sdma1 ");
                        if (id->dma_1word & 0x400)
                                strcat(dmodes, "*");
                        if (id->dma_1word & 4)
                                strcat(dmodes, "sdma2 ");
                        if (id->dma_1word & 0xf800)
                                strcat(dmodes, "*");
                        if (id->dma_1word & 0xf8)
                                strcat(dmodes, "sdma? ");
                        if (id->dma_mword & 0x100)
                                strcat(dmodes, "*");
                        if (id->dma_mword & 1)
                                strcat(dmodes, "mdma0 ");
                        if (id->dma_mword & 0x200)
                                strcat(dmodes, "*");
                        if (id->dma_mword & 2)
                                strcat(dmodes, "mdma1 ");
                        if (id->dma_mword & 0x400)
                                strcat(dmodes, "*");
                        if (id->dma_mword & 4)
                                strcat(dmodes, "mdma2 ");
                        if (id->dma_mword & 0xf800)
                                strcat(dmodes, "*");
                        if (id->dma_mword & 0xf8)
                                strcat(dmodes, "mdma? ");
                }
        }
        if (id->capability & 8)
                tst_resm(TINFO, "IORDY=%s", (id->capability & 4) ? "on/off" : "yes");
        else
                tst_resm(TINFO, "IORDY=no");
        if ((id->capability & 8) || (id->field_valid & 2))
        {
                if (id->field_valid & 2)
                {
                        tst_resm(TINFO, ", tPIO={min:%u,w/IORDY:%u}", id->eide_pio, id->eide_pio_iordy);
                        if (id->eide_pio_modes & 1)
                                strcat(pmodes, "pio3 ");
                        if (id->eide_pio_modes & 2)
                                strcat(pmodes, "pio4 ");
                        if (id->eide_pio_modes & ~3)
                                strcat(pmodes, "pio? ");
                }
                if (id->field_valid & 4)
                {
                        if (id->dma_ultra & 0x100)
                                strcat(umodes, "*");
                        if (id->dma_ultra & 0x001)
                                strcat(umodes, "udma0 ");
                        if (id->dma_ultra & 0x200)
                                strcat(umodes, "*");
                        if (id->dma_ultra & 0x002)
                                strcat(umodes, "udma1 ");
                        if (id->dma_ultra & 0x400)
                                strcat(umodes, "*");
                        if (id->dma_ultra & 0x004)
                                strcat(umodes, "udma2 ");
#ifdef __NEW_HD_DRIVE_ID
                        if (id->hw_config & 0x2000)
                        {
#else                           /* !__NEW_HD_DRIVE_ID */
                        if (id->word93 & 0x2000)
                        {
#endif                          /* __NEW_HD_DRIVE_ID */
                                if (id->dma_ultra & 0x0800)
                                        strcat(umodes, "*");
                                if (id->dma_ultra & 0x0008)
                                        strcat(umodes, "udma3 ");
                                if (id->dma_ultra & 0x1000)
                                        strcat(umodes, "*");
                                if (id->dma_ultra & 0x0010)
                                        strcat(umodes, "udma4 ");
                                if (id->dma_ultra & 0x2000)
                                        strcat(umodes, "*");
                                if (id->dma_ultra & 0x0020)
                                        strcat(umodes, "udma5 ");
                                if (id->dma_ultra & 0x4000)
                                        strcat(umodes, "*");
                                if (id->dma_ultra & 0x0040)
                                        strcat(umodes, "udma6 ");
                                if (id->dma_ultra & 0x8000)
                                        strcat(umodes, "*");
                                if (id->dma_ultra & 0x0080)
                                        strcat(umodes, "udma7 ");
                        }
                }
        }
        if ((id->capability & 1) && (id->field_valid & 2))
                tst_resm(TINFO, ", tDMA={min:%u,rec:%u}", id->eide_dma_min, id->eide_dma_time);
        tst_resm(TINFO, "PIO modes: %s", pmodes);
        if (*dmodes)
                tst_resm(TINFO, "DMA modes: %s", dmodes);
        if (*umodes)
                tst_resm(TINFO, "UDMA modes: %s", umodes);

        printf("AdvancedPM=%s", YN(id_regs[83] & 8));
        if (id_regs[83] & 8)
        {
                if (!(id_regs[86] & 8))
                        printf(": disabled(255)");
                else if ((id_regs[91] & 0xFF00) != 0x4000)
                        printf(": unknown setting");
                else
                        printf(": mode=0x%02X(%u)", id_regs[91] & 0xFF, id_regs[91] & 0xFF);
        }
        if (id_regs[82] & 0x20)
                tst_resm(TINFO, "WriteCache=%s", (id_regs[85] & 0x20) ? "enabled" : "disabled");
#ifdef __NEW_HD_DRIVE_ID
        if (id->minor_rev_num || id->major_rev_num)
        {
                printf("Drive conforms to: ");
                if (id->minor_rev_num <= 31)
                        printf("%s: ", minor_str[id->minor_rev_num]);
                else
                        printf("unknown: ");
                if (id->major_rev_num != 0x0000 &&      /* NOVAL_0 */
                    id->major_rev_num != 0xFFFF)
                {       /* NOVAL_1 */
                        /* through ATA/ATAPI-7 is currently defined-- increase this value as further
                        * specs are standardized(though we can guess safely to 15) */
                        for (i = 0; i <= 7; i++)
                        {
                                if (id->major_rev_num & (1 << i))
                                        printf(" ATA/ATAPI-%u", i);
                        }
                }
        }
#endif                          /* __NEW_HD_DRIVE_ID */
        printf("* signifies the current active mode\n");
}

//read/write perfromnace test

int ata_readperfrom_test(int fd)
{
	int rv=TPASS;
	char *readbuf;
	unsigned long long size;
	struct timeval tv1,tv2;
	long  interval;
	long sumInterval=0;
	double readspeed;
	double MByte=1000000/1024;
	 int     shmid;
	 int i,readtimes=10;

	for(i=0;i<readtimes;i++)
	{
		if ((shmid = shmget(IPC_PRIVATE, TIMING_BUF_BYTES, 0600)) == -1)
	        {
	                perror("could not allocate sharedmem buf");
	                return TFAIL;
	        }
	        if (shmctl(shmid, SHM_LOCK, NULL) == -1)
	        {
	                perror("could not lock sharedmem buf");
	                (void) shmctl(shmid, IPC_RMID, NULL);
	                return TFAIL;
	        }
	        if ((readbuf = shmat(shmid, (char *) 0, 0)) == (char *) -1)
	        {
	                perror("could not attach sharedmem buf");
	                (void) shmctl(shmid, IPC_RMID, NULL);
	                return TFAIL;
	        }
	        if (shmctl(shmid, IPC_RMID, NULL) == -1)
	                perror("shmctl(,IPC_RMID,) failed");

	        /* Clear out the device request queues & give them time to complete */
	        sync();
	        sleep(3);

		gettimeofday(&tv1,NULL);

		size=read(fd,readbuf,TIMING_BUF_BYTES);

		gettimeofday(&tv2, NULL);

		if(size!=TIMING_BUF_BYTES)
		{
			if (size)
	                {
	                        if (size== -1)
	                                tst_resm(TINFO,"read() failed");
	                        else
	                                tst_resm(TINFO, "read(%u) returned %u bytes", TIMING_BUF_BYTES, size);
	                }
	                else
	                {
	                        tst_resm(TINFO,"read() hit EOF - device too small\n");
	                }
			  rv=TFAIL;
	                return rv;	
		}

		interval=(tv2.tv_sec-tv1.tv_sec)*1000000+(tv2.tv_usec-tv1.tv_usec);
		sumInterval=sumInterval+interval;
		

	}

	readspeed=(MByte*((double)size*readtimes)/sumInterval)/1024;
	
	tst_resm(TINFO,"HDD average read speed  %lf MB/sec",readspeed);
	
	 if (-1 == shmdt(readbuf))
                perror("could not detach sharedmem buf");
	return rv;
}

//ata writeperformance test

int ata_writeperform_test(int fd)
{

	int rv=TPASS;
	char *writebuf;
	unsigned long long size;
	struct timeval tv1,tv2;
	long  interval;
	long sumInterval=0;
	double writespeed;
	double MByte=1000000/1024;
	 int     shmid;
	 int i,writetimes=10;

	for(i=0;i<writetimes;i++)
	{
		if ((shmid = shmget(IPC_PRIVATE, TIMING_BUF_BYTES, 0600)) == -1)
	        {
	                perror("could not allocate sharedmem buf");
	                return TFAIL;
	        }
	        if (shmctl(shmid, SHM_LOCK, NULL) == -1)
	        {
	                perror("could not lock sharedmem buf");
	                (void) shmctl(shmid, IPC_RMID, NULL);
	                return TFAIL;
	        }
	        if ((writebuf = shmat(shmid, (char *) 0, 0)) == (char *) -1)
	        {
	                perror("could not attach sharedmem buf");
	                (void) shmctl(shmid, IPC_RMID, NULL);
	                return TFAIL;
	        }
	        if (shmctl(shmid, IPC_RMID, NULL) == -1)
	                perror("shmctl(,IPC_RMID,) failed");

	        /* Clear out the device request queues & give them time to complete */
	        sync();
	        sleep(3);

		gettimeofday(&tv1,NULL);

		size=write(fd,writebuf,TIMING_BUF_BYTES);

		gettimeofday(&tv2, NULL);

		if(size!=TIMING_BUF_BYTES)
		{
			if (size)
	                {
	                        if (size== -1)
	                                tst_resm(TINFO,"write() failed");
	                        else
	                                tst_resm(TINFO, "write(%u) returned %u bytes", TIMING_BUF_BYTES, size);
	                }
	                else
	                {
	                        tst_resm(TINFO,"write() hit EOF - device too small\n");
	                }
			  rv=TFAIL;
	                return rv;	
		}
		
		interval=(tv2.tv_sec-tv1.tv_sec)*1000000+(tv2.tv_usec-tv1.tv_usec);
		sumInterval=sumInterval+interval;
		
	}
	
	
	//writespeed=(MByte*((double)size)/interval)/1024;
	writespeed=(MByte*((double)size*writetimes)/sumInterval)/1024;

	
	tst_resm(TINFO,"HDD average write speed %lf MB/sec",writespeed);
	 if (-1 == shmdt(writebuf))
                perror("could not detach sharedmem buf");
	return rv;
	
}


//test read/write performance at PIO mode 

int ata_ReadWritePerformancePIOMode(int fd)
{
	unsigned char args1[4] = { WIN_SETFEATURES, 0, 3, 0 };
	int xfermode;
	int VT_rv_read[5] ={TPASS,TPASS,TPASS,TPASS,TPASS};
	int VT_rv_write[5] ={TPASS,TPASS,TPASS,TPASS,TPASS};
	int VT_rv=TPASS;
	int i=0,maxPIOMode=5;
	int failFlag=TPASS;
	
	for(i=0;i<maxPIOMode;i++)
	{
		args1[0]=WIN_SETFEATURES;
		args1[1]=0;
		args1[2]=3;
		args1[3]=0;
		tst_resm(TINFO,"read/write performance at PIO mode %d",i);
		xfermode=8+i;;
		args1[1] = xfermode;
	    tst_resm(TINFO, "setting xfermode to %d", xfermode);
	    //interpret_xfermode(xfermode);

	    if (ioctl(fd, HDIO_DRIVE_CMD, &args1) != TPASS)
	    {
	    	tst_resm(TFAIL, "HDIO_DRIVE_CMD(setxfermode) failed. Error string: %s",
	                                 strerror(errno));
	        VT_rv = TFAIL;
			return VT_rv;
	    }

		flush_buffer_cache(fd);
		
		//ata read performance at PIO mode 0
		tst_resm(TINFO,"ATA read performance at PIO mode %d",i);
		VT_rv=ata_readperfrom_test(fd);
		if(VT_rv==TFAIL)
		{
			tst_resm(TFAIL,"read performance test failed at PIO mode %d",i);
			VT_rv_read[i]=TFAIL;
		}

		flush_buffer_cache(fd);
		
		//ata write performance at PIO mode 0
		tst_resm(TINFO,"ATA write performance at PIO mode %d",i);
		VT_rv=ata_writeperform_test(fd);
		if(VT_rv==TFAIL)
		{
			tst_resm(TFAIL,"write performance test failed at PIO mode %d",i);
			VT_rv_write[i]=TFAIL;
		}
		flush_buffer_cache(fd);
	}

	for(i=0;i<maxPIOMode;i++)
	{
		if(VT_rv_read[i]==TFAIL || VT_rv_write[i]==TFAIL)
			{
			failFlag=TFAIL;
			break;
		}
	}

	if(failFlag==TFAIL)
		{
		tst_resm(TFAIL,"read/write performance test at PIO mode failed");
		return failFlag;
	}
	return TPASS;
}

int ata_ReadWritePerformanceMDMAMode(int fd)
{
	unsigned char args1[4] = { WIN_SETFEATURES, 0, 3, 0 };
	int xfermode;
	int VT_rv_read[3] ={TPASS,TPASS,TPASS};
	int VT_rv_write[3] ={TPASS,TPASS,TPASS};
	int VT_rv=TPASS;
	int i=0,maxMDMAMode=3;
	int failFlag=TPASS;
	
	for(i=0;i<maxMDMAMode;i++)
	{
		args1[0]=WIN_SETFEATURES;
		args1[1]=0;
		args1[2]=3;
		args1[3]=0;
		tst_resm(TINFO,"read/write performance at MDMA mode %d",i);
		xfermode=32+i;;
		args1[1] = xfermode;
	    tst_resm(TINFO, "setting xfermode to %d", xfermode);
	    //interpret_xfermode(xfermode);

	    if (ioctl(fd, HDIO_DRIVE_CMD, &args1) != TPASS)
	    {
	    	tst_resm(TFAIL, "HDIO_DRIVE_CMD(setxfermode) failed. Error string: %s",
	                                 strerror(errno));
	        VT_rv = TFAIL;
			return VT_rv;
	    }

		flush_buffer_cache(fd);
		
		//ata read performance at MDMA mode 0
		tst_resm(TINFO,"ATA read performance at MDMA mode %d",i);
		VT_rv=ata_readperfrom_test(fd);
		if(VT_rv==TFAIL)
		{
			tst_resm(TFAIL,"read performance test failed at MDMA mode %d",i);
			VT_rv_read[i]=TFAIL;
		}

		flush_buffer_cache(fd);
		
		//ata write performance at MDMA mode 0
		tst_resm(TINFO,"ATA write performance at MDMA mode %d",i);
		VT_rv=ata_writeperform_test(fd);
		if(VT_rv==TFAIL)
		{
			tst_resm(TFAIL,"write performance test failed at MDMA mode %d",i);
			VT_rv_write[i]=TFAIL;
		}
		flush_buffer_cache(fd);
	}

	for(i=0;i<maxMDMAMode;i++)
	{
		if(VT_rv_read[i]==TFAIL || VT_rv_write[i]==TFAIL)
			{
			failFlag=TFAIL;
			break;
		}
	}

	if(failFlag==TFAIL)
		{
		tst_resm(TFAIL,"read/write performance test at MDMA mode failed");
		return failFlag;
	}
	
	return TPASS;
}

int ata_ReadWritePerformanceUDMAMode(int fd)
{
	unsigned char args1[4] = { WIN_SETFEATURES, 0, 3, 0 };
	int xfermode;
	int VT_rv_read[4] ={TPASS,TPASS,TPASS,TPASS};
	int VT_rv_write[4] ={TPASS,TPASS,TPASS,TPASS};
	int VT_rv=TPASS;
	int i=0,maxUDMAMode=4;
	int failFlag=TPASS;
	
	for(i=0;i<maxUDMAMode;i++)
	{
		args1[0]=WIN_SETFEATURES;
		args1[1]=0;
		args1[2]=3;
		args1[3]=0;
		tst_resm(TINFO,"read/write performance at UDMA mode %d",i);
		xfermode=64+i;;
		args1[1] = xfermode;
	    tst_resm(TINFO, "setting xfermode to %d", xfermode);
	    //interpret_xfermode(xfermode);

	    if (ioctl(fd, HDIO_DRIVE_CMD, &args1) != TPASS)
	    {
	    	tst_resm(TFAIL, "HDIO_DRIVE_CMD(setxfermode) failed. Error string: %s",
	                                 strerror(errno));
	        VT_rv = TFAIL;
			return VT_rv;
	    }

		flush_buffer_cache(fd);
		
		//ata read performance at MDMA mode 0
		tst_resm(TINFO,"ATA read performance at UDMA mode %d",i);
		VT_rv=ata_readperfrom_test(fd);
		if(VT_rv==TFAIL)
		{
			tst_resm(TFAIL,"read performance test failed at UDMA mode %d",i);
			VT_rv_read[i]=TFAIL;
		}

		flush_buffer_cache(fd);
		
		//ata write performance at MDMA mode 0
		tst_resm(TINFO,"ATA write performance at UDMA mode %d",i);
		VT_rv=ata_writeperform_test(fd);
		if(VT_rv==TFAIL)
		{
			tst_resm(TFAIL,"write performance test failed at UDMA mode %d",i);
			VT_rv_write[i]=TFAIL;
		}
		flush_buffer_cache(fd);
	}

	for(i=0;i<maxUDMAMode;i++)
	{
		if(VT_rv_read[i]==TFAIL || VT_rv_write[i]==TFAIL)
			{
			failFlag=TFAIL;
			break;
		}
	}

	if(failFlag==TFAIL)
		{
		tst_resm(TFAIL,"read/write performance test at UDMA mode failed");
		return failFlag;
	}
	
	return TPASS;	
}



/*================================================================================================*/
void flush_buffer_cache(int fd)
{
        fsync(fd);      /* flush buffers */
        if (ioctl(fd, BLKFLSBUF, NULL)) /* do it again, big time */
                perror("BLKFLSBUF failed");
        if (ioctl(fd, HDIO_DRIVE_CMD, NULL) && errno != EINVAL) /* await completion */
                perror("HDIO_DRIVE_CMD(null)(wait for flush complete) failed");
}

/*================================================================================================*/
int read_big_block(int fd, char *buf)
{
        int     i,
                rc;

        if ((rc = read(fd, buf, TIMING_BUF_BYTES)) != TIMING_BUF_BYTES)
        {
                if (rc)
                {
                        if (rc == -1)
                                perror("read() failed");
                        else
                                tst_resm(TINFO, "read(%u) returned %u bytes", TIMING_BUF_BYTES, rc);
                }
                else
                {
                        fputs("read() hit EOF - device too small\n", stderr);
                }
                return 1;
        }
        /* access all sectors of buf to ensure the read fully completed */
        for (i = 0; i < TIMING_BUF_BYTES; i += 512)
                buf[i] &= 1;
        return 0;
}

/*================================================================================================*/
int write_big_block(int fd, char *buf)
{
        int     i,
                rc;

        if ((rc = write(fd, buf, TIMING_BUF_BYTES)) != TIMING_BUF_BYTES)
        {
                if (rc)
                {
                        if (rc == -1)
                                perror("read() failed");
                        else
                                tst_resm(TINFO, "read(%u) returned %u bytes", TIMING_BUF_BYTES, rc);
                }
                else
                {
                        fputs("read() hit EOF - device too small\n", stderr);
                }
                return 1;
        }
        /* access all sectors of buf to ensure the read fully completed */
        for (i = 0; i < TIMING_BUF_BYTES; i += 512)
                buf[i] &= 1;
        return 0;
}

/*================================================================================================*/
static int do_blkgetsize(int fd, unsigned long long *blksize64)
{
        int     rc;
        unsigned int blksize32 = 0;

        if (0 == ioctl(fd, BLKGETSIZE64, blksize64))
        {       // returns bytes
                *blksize64 /= 512;
                return 0;
        }
        rc = ioctl(fd, BLKGETSIZE, &blksize32); // returns sectors
        if (rc)
                perror(" BLKGETSIZE failed");
        *blksize64 = blksize32;
        return rc;
}

/*================================================================================================*/
void time_device(int fd)
{
        char   *buf;
        double  elapsed;
        struct itimerval e1,
                e2;
        int     shmid;
        unsigned int max_iterations = 1024,
            total_MB,
            iterations;


        /* get device size */
        if (do_timings)
        {
                unsigned long long blksize;

                if (do_blkgetsize(fd, &blksize) == 0)
                        max_iterations = blksize / (2 * 1024) / TIMING_BUF_MB;
        }

        if ((shmid = shmget(IPC_PRIVATE, TIMING_BUF_BYTES, 0600)) == -1)
        {
                perror("could not allocate sharedmem buf");
                return;
        }
        if (shmctl(shmid, SHM_LOCK, NULL) == -1)
        {
                perror("could not lock sharedmem buf");
                (void) shmctl(shmid, IPC_RMID, NULL);
                return;
        }
        if ((buf = shmat(shmid, (char *) 0, 0)) == (char *) -1)
        {
                perror("could not attach sharedmem buf");
                (void) shmctl(shmid, IPC_RMID, NULL);
                return;
        }
        if (shmctl(shmid, IPC_RMID, NULL) == -1)
                perror("shmctl(,IPC_RMID,) failed");

        /* Clear out the device request queues & give them time to complete */
        sync();
        sleep(3);

        tst_resm(TINFO, "Timing %s disk %s:  ", (open_flags & O_DIRECT) ? "O_DIRECT" : "buffered",
               do_writing ? "writes" : "reads");
        fflush(stdout);

        /* 
        * getitimer() is used rather than gettimeofday() because
        * it is much more consistent(on my machine, at least).
        */

        struct itimerval e3 = 
        {
                {1000, 0},
                {1000, 0}
        };

        setitimer(ITIMER_REAL, &e3, NULL);

        /* Now do the timings for real */
        iterations = 0;
        getitimer(ITIMER_REAL, &e1);
        do
        {
                ++iterations;
                if (do_writing ? write_big_block(fd, buf) : read_big_block(fd, buf))
                        goto quit;
                getitimer(ITIMER_REAL, &e2);
                elapsed = (e1.it_value.tv_sec - e2.it_value.tv_sec)
                    + ((e1.it_value.tv_usec - e2.it_value.tv_usec) / 1000000.0);
        }
        while (elapsed < 3.0 && iterations < max_iterations);

        total_MB = iterations * TIMING_BUF_MB;
        if ((total_MB / elapsed) > 1.0) /* more than 1MB/s */
                tst_resm(TINFO, "%3u MB in %5.2f seconds = %6.2f MB/sec",
                       total_MB, elapsed, total_MB / elapsed);
        else
                tst_resm(TINFO, "%3u MB in %5.2f seconds = %6.2f kB/sec",
                       total_MB, elapsed, total_MB / elapsed * 1024);
        quit:
        if (-1 == shmdt(buf))
                perror("could not detach sharedmem buf");
}

/*================================================================================================*/
static void on_off(unsigned int value)
{
        printf(value ? "(on)" : "(off)");
}

/*================================================================================================*/
static void interpret_xfermode(unsigned int xfermode)
{
        switch (xfermode)
        {
        case 0:
                tst_resm(TINFO, "(default PIO mode)");
                break;
        case 1:
                tst_resm(TINFO, "(default PIO mode, disable IORDY)");
                break;
        case 8:
        case 9:
        case 10:
        case 11:
        case 12:
        case 13:
        case 14:
        case 15:
                tst_resm(TINFO, "(PIO flow control mode%u)", xfermode - 8);
                break;
        case 16:
        case 17:
        case 18:
        case 19:
        case 20:
        case 21:
        case 22:
        case 23:
                tst_resm(TINFO, "(singleword DMA mode%u)", xfermode - 16);
                break;
        case 32:
        case 33:
        case 34:
        case 35:
        case 36:
        case 37:
        case 38:
        case 39:
                tst_resm(TINFO, "(multiword DMA mode%u)", xfermode - 32);
                break;
        case 64:
        case 65:
        case 66:
        case 67:
        case 68:
        case 69:
        case 70:
        case 71:
                tst_resm(TINFO, "(UltraDMA mode%u)", xfermode - 64);
                break;
        default:
                tst_resm(TINFO, "(unknown, probably not valid)");
                break;
        }
}

/*================================================================================================*/
/*===== VT_ata_driver_test =====*/
/**
@brief  ATA Disk driver test scenario.

@param  switch_fct
        test case number.

@return On success - return TPASS
        On failure - return the error code.
*/
/*================================================================================================*/
int VT_ata_driver_test(char *devname, int testcase, int dma_flag, int io32bit, int xfermode,
                       int flush_buffer)
{
        int     fd,
                VT_rv = TPASS;
        static long parm,
                multcount;
        unsigned long long blksize;
        static const char msg[] = "geometry = %u/%u/%u, sectors = %lld, start = %ld\n";
        static struct hd_geometry g;
        static struct hd_driveid id;
        unsigned char args1[4] = { WIN_SETFEATURES, 0, 3, 0 };
        unsigned char args2[4 + 512] = { WIN_IDENTIFY, 0, 0, 1, };
         
        unsigned i;
        __u16  *id1;

        fd = open(devname, open_flags);
        if (fd < 0)
        {
                perror(devname);
                exit(errno);
        }

        switch (testcase)
        {
        case 0:
                tst_resm(TINFO, "setting 32-bit IO_support flag to %d\n", io32bit);
                if (ioctl(fd, ATA_IOC_SET_IO32, io32bit) != TPASS)
                {
                        tst_resm(TFAIL, " ATA_IOC_SET_IO32 failed. Error string: %s", strerror(errno));
                        VT_rv = TFAIL;
                }
                if (ioctl(fd, ATA_IOC_GET_IO32, &parm) == TPASS)
                {
                        printf("IO_support = %3ld (", parm);
                        switch (parm)
                        {
                        case 0:
                                printf("default - ");
                        case 2:
                                printf("16-bit)\n");
                                break;
                        case 1:
                                printf("32-bit)\n");
                                break;
                        case 3:
                                printf("32-bit w/sync)\n");
                                break;
                        case 8:
                                printf("Request-Queue-Bypass)\n");
                                break;
                        default:
                                printf("\?\?\?)\n");
                        }
                }
                else
                {
                        tst_resm(TFAIL, "ATA_IOC_GET_IO32 failed. Error string: %s", strerror(errno));
                        VT_rv = TFAIL;
                }
                break;
      
        case 1:
                if (ioctl(fd, HDIO_GET_IDENTITY, &id) == TPASS)
                {
                        if (multcount != -1)
                        {
                                id.multsect = multcount;
                                id.multsect_valid |= 1;
                        }
                        else
                                id.multsect_valid &= ~1;
                        dump_identity(&id);
                }
                else if (errno == -ENOMSG)
                        tst_resm(TINFO, "no identification info available\n");
                else
                {
                        tst_resm(TFAIL, "HDIO_GET_IDENTITY failed. Error string: %s",
                                 strerror(errno));
                        VT_rv = TFAIL;
                }

                break;
        case 2:
                if (ioctl(fd, HDIO_DRIVE_CMD, &args2))
                {
                        args2[0] = WIN_PIDENTIFY;
                        if (ioctl(fd, HDIO_DRIVE_CMD, &args2) )
                        {
                                tst_resm(TFAIL, "HDIO_DRIVE_CMD(identify) failed. Error string: %s",
                                         strerror(errno));
                                VT_rv = TFAIL;
                                break;
                        }
                }
                id1 = (__u16 *) & args2[4];
                if (get_IDentity == 2)
                {
                        for (i = 0; i < (256 / 8); ++i)
                        {
                                tst_resm(TINFO, "%04x %04x %04x %04x %04x %04x %04x %04x\n",
                                                id1[0], id1[1], id1[2], id1[3], id1[4],
                                                id1[5], id1[6], id1[7]);
                                                id1 += 8;
                        }
                }
                else
                {
                        for (i = 0; i < 0x100; ++i)
                        {
                                __le16_to_cpus(&id1[i]);
                        }
                        identify((void *) id1, NULL);
                }
                break;
        case 3:
                do_timings = 1;
                time_device(fd);
                break;
       
        case 4:
                args1[1] = xfermode;
                tst_resm(TINFO, "setting xfermode to %d", xfermode);
                interpret_xfermode(xfermode);

                if (ioctl(fd, HDIO_DRIVE_CMD, &args1) != TPASS)
                {
                        tst_resm(TFAIL, "HDIO_DRIVE_CMD(setxfermode) failed. Error string: %s",
                                 strerror(errno));
                        VT_rv = TFAIL;
                }
                break;
		case 5: 
			//PIO mode read/write performance
			VT_rv=ata_ReadWritePerformancePIOMode(fd);
			if(VT_rv==TFAIL)
			{
				tst_resm(TFAIL,"HDD read/write performance test at PIO mode failed");
			}
			break;
			
		case 6: 
			//MDMA mode read/write performance
			VT_rv=ata_ReadWritePerformanceMDMAMode(fd);
			if(VT_rv==TFAIL)
			{
				tst_resm(TFAIL,"HDD read/write performance test at MDMA mode failed");
			
			}
			break;
		case 7: 
			VT_rv=ata_ReadWritePerformanceUDMAMode(fd);
			if(VT_rv==TFAIL)
				{
				tst_resm(TFAIL,"HDD read/write performance test at UDMA mode failed");
			}
			break;
		
        }

        if (flush_buffer)
                flush_buffer_cache(fd);

        close(fd);

        return VT_rv;
}
