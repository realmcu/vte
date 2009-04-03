/*================================================================================================*/
/**
    @file   nor_mtd_test_2.c

    @brief Test error cases in do write/read/erase operations on
              a flash memory device using MTD subsystem API
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
V.Becker/rc023c          04/05/2004     TLSbo39142   Initial version 
V.Becker/rc023c          18/06/2004     TLSbo39142   Code reviewed 
V.Becker/rc023c          20/10/2004     TLSbo43924   Update after NOR Flash map change
E.Gromazina              07/07/2005     TLSbo50888   Minor fixes
C.Gagneraud/cgag1c       06/01/2006     TLSbo53742   Complete rewrite
D.Simakov/b00296         06/07/2006     TLSbo67309   Backup/Restore were implemented to
                                                     use the /dev/mtd/1 and /dev/mtd/2 devices
                                                         
====================================================================================================
Portability:  ARM GCC  gnu compiler
==================================================================================================*/



/*==================================================================================================
                                        INCLUDE FILES
==================================================================================================*/
/* Standard Include Files */
#include <errno.h>

/* Harness Specific Include Files. */
#include "test.h"

/* Verification Test Environment Include Files */
#include "nor_mtd_test_2.h"


/*==================================================================================================
                                        LOCAL MACROS
==================================================================================================*/

#if !defined(TRUE)
    #define TRUE 1
#endif
#if !defined(FALSE)
    #define FALSE 0
#endif
            

/*==================================================================================================
                          LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
==================================================================================================*/

typedef struct
{
        void * data;
        size_t sz;
} backup_data_t;
                

/*==================================================================================================
                                       LOCAL CONSTANTS
==================================================================================================*/

/*==================================================================================================
                                       LOCAL VARIABLES
==================================================================================================*/
static int           file_desc = 0;
static backup_data_t backup    = {0,0};

/*==================================================================================================
                                       GLOBAL CONSTANTS
==================================================================================================*/


/*==================================================================================================
                                       GLOBAL VARIABLES
==================================================================================================*/
extern char device_name[];

/*==================================================================================================
                                   LOCAL FUNCTION PROTOTYPES
==================================================================================================*/
int     Erase_Flash(struct erase_info_user *, struct mtd_info_user *, unsigned int);
int     Backup(size_t sz);
int     Restore(void);


/*==================================================================================================
                                       LOCAL FUNCTIONS
==================================================================================================*/


/*================================================================================================*/
/*===== VT_nor_mtd_test2_setup =====*/
/**

@param  None
  
@return On success - return TPASS
        On failure - return the error code
*/
/*================================================================================================*/
int VT_nor_mtd_test2_setup(void)
{
        int     rv = TFAIL;

        /* Open NOR MTD driver */
        tst_resm(TINFO, " Open NOR MTD driver : %s\n", device_name);
        file_desc = open(device_name, O_RDWR);
        sleep(1);
        if (file_desc == -1)
        {
                tst_resm(TFAIL, "ERROR : Open NOR MTD driver fails : %d", errno);
                perror("Error: cannot open device \n");
        }
        else
        {
                rv = TPASS;
        }

        return rv;
}

/*================================================================================================*/
/*===== VT_nor_mtd_test2_cleanup =====*/
/**
@brief  assumes the post-condition of the test case execution

@param  None
  
@return On success - return TPASS
        On failure - return the error code
*/
/*================================================================================================*/
int VT_nor_mtd_test2_cleanup(void)
{
        int     rv = TFAIL;
        int     ret = 0;

        lseek(file_desc, 0, SEEK_SET);
        struct mtd_info_user mxc_mtd;
        struct erase_info_user mxc_erase_mtd;
        ioctl(file_desc, MEMGETINFO, &mxc_mtd);
        Erase_Flash(&mxc_erase_mtd, &mxc_mtd, backup.sz);
        Restore();                                                                                        

        /* Close NOR MTD driver */
        ret = close(file_desc);
        if (ret == -1)
        {
                tst_resm(TFAIL, "ERROR : Close NOR MTD driver fails : %d\n", errno);
                perror("Error: cannot close  device \n");
        }
        else
        {
                rv = TPASS;
        }

        return rv;
}

/*================================================================================================*/
/*===== VT_nor_mtd_test2 =====*/
/**
@brief  nor_mtd_test 2 tests error cases on NOR Flash memory device driver

@param  None
  
@return On success - return TPASS
        On failure - return the error code
*/
/*================================================================================================*/
int VT_nor_mtd_test2(unsigned long size)
{
        tst_resm(TINFO, "NOR MTD driver test 2");
        tst_resm(TINFO, "Test error cases with Flash basic operations");

        int     rv = TFAIL;
        int     retval = -EFAULT;
        int     failure = 0;
        unsigned char w_pattern;
        ssize_t w_size = 0;
        unsigned char r_pattern;
        ssize_t r_size = 0;
        off_t   offset = 0;
        off_t   want_offset = 0;

        /* MTD specific structures */
        struct mtd_info_user mxc_mtd;
        struct erase_info_user mxc_erase_mtd;

        /* Get information on NOR Flash Returns the mtd_info structure */
        tst_resm(TINFO, "Get information on NOR Flash structure");
        retval = ioctl(file_desc, MEMGETINFO, &mxc_mtd);
        if (retval < 0)
        {
                tst_resm(TFAIL, "MEMGETINFO failed: %s", strerror(errno));
                return rv;
        }
        
        if( !Backup(mxc_mtd.size) )
                return rv;
        lseek(file_desc, 0, SEEK_SET);
                        
                                  


        /** Write to NOR Flash memory without erasure:
        * - erase one block of flash
        * - fill it with 0x0F
        * - fill it with 0xF0 => should failed with -EIO
        */
        tst_resm(TINFO, "Write to NOR Flash memory without erasure");

        /* - erase one block of flash */
        mxc_erase_mtd.start = size;
        mxc_erase_mtd.length = mxc_mtd.erasesize;
        tst_resm(TINFO, " - erasing one block of flash (start=0x%lx, length=0x%lx)",
                 (unsigned long) mxc_erase_mtd.start, (unsigned long) mxc_erase_mtd.length);
        retval = ioctl(file_desc, MEMERASE, &mxc_erase_mtd);
        if (retval < 0)
        {
                tst_resm(TFAIL, "    => MEMERASE failed: %d (%s)", errno, strerror(errno));
                return rv;
        }

        /* - write 0x0F at 0 */
        if (lseek(file_desc, size, SEEK_SET) < 0)
        {
                tst_resm(TFAIL, "    => Cannot seek to 0x%lx: %s", size, strerror(errno));
                return rv;
        }
        w_pattern = 0x0F;
        w_size = 0;
        tst_resm(TINFO, " - writing 0x0F at offset 0x%lx", size);
        w_size = write(file_desc, &w_pattern, 1);
        if (w_size != 1)
        {
                tst_resm(TFAIL, "    => write() should not have failed: %d (%s)",
                         errno, strerror(errno));
                return rv;
        }

        /* - write 0xF0 at 0 => should failed with -EIO */
        lseek(file_desc, size, SEEK_SET);       /* TODO: check lseek */
        w_pattern = 0xF0;
        w_size = 0;
        tst_resm(TINFO, " - writing 0xF0 at offset 0x%lx", size);
        w_size = write(file_desc, &w_pattern, 1);
        if (w_size != 1)
        {
                if (errno == EIO)
                        tst_resm(TPASS, "    => write() failed, got EIO: OK", strerror(errno));
                else
                {
                        tst_resm(TFAIL, "    => write() failed, but errno is not EIO: %d (%s)",
                                 errno, strerror(errno));
                        failure = 1;
                }
        }
        else
        {
                tst_resm(TFAIL, "    => write succeed, should have failed!");
                failure = 1;
        }


        /** Write beyond total Flash size 
        * - erase last block of flash
        * - set cursor to the end
        * - write data in last place
        * - write data again => should failed with ENOSPC
        */
        tst_resm(TINFO, "Write at wrong address (pos=0x%x)", mxc_mtd.size);

        /* - erase last block of flash */
        mxc_erase_mtd.start = mxc_mtd.size - mxc_mtd.erasesize;
        mxc_erase_mtd.length = mxc_mtd.erasesize;
        tst_resm(TINFO, " - erasing last block of flash (start=0x%lx, length=0x%lx)",
                 (unsigned long) mxc_erase_mtd.start, (unsigned long) mxc_erase_mtd.length);
        retval = ioctl(file_desc, MEMERASE, &mxc_erase_mtd);
        if (retval < 0)
        {
                tst_resm(TFAIL, "    => MEMERASE failed: %d (%s)", errno, strerror(errno));
                return rv;
        }

        /* - set cursor to the end */
        want_offset = mxc_mtd.size - 1;
        tst_resm(TINFO, " - setting cursor at the end of flash (0x%lx)", want_offset);
        offset = lseek(file_desc, want_offset, SEEK_SET);
        if (offset < 0 || offset != want_offset)
        {
                tst_resm(TFAIL, "    => Cannot seek to 0x%lx: %s", size, strerror(errno));
                return rv;
        }

        /* - write data in last place */
        w_pattern = 0xA5;
        tst_resm(TINFO, " - Writing one byte of data (0x%x)", w_pattern);
        w_size = write(file_desc, &w_pattern, 1);
        if (w_size != 1)
        {
                tst_resm(TFAIL, "    => 1st write() should not have failed!");
                failure = 1;
        }
        else
        {
                tst_resm(TINFO, "    => 1st write() succeed");
        }

        /* second write should failed */
        w_pattern = 0x37;
        tst_resm(TINFO, " - Writing again one byte of data (0x%x)", w_pattern);
        w_size = write(file_desc, &w_pattern, 1);
        if (w_size < 0)
        {
                if (errno == ENOSPC)
                {
                        tst_resm(TPASS, "    => 2nd write() failed, got ENOSPC: OK");
                }
                else
                {
                        tst_resm(TFAIL,
                                 "    => 2nd write() failed, but errno is not ENOSPC: %d (%s)",
                                 errno, strerror(errno));
                        failure = 1;
                }
        }
        else
        {
                tst_resm(TFAIL, "    => 2nd write() succeed, should have failed!");
                failure = 1;
        }

        /** Read NOR flash at wrong address 
        * - set cursor to the end
        * - read data from last place
        * - read data again => should failed with ENOSPC
        */
        tst_resm(TINFO, "Read NOR flash at wrong address (pos=0x%x)", mxc_mtd.size);

        /* - set cursor to the end */
        want_offset = mxc_mtd.size - 1;
        tst_resm(TINFO, " - setting cursor at the end of flash (0x%lx)", want_offset);
        offset = lseek(file_desc, want_offset, SEEK_SET);
        if (offset < 0 || offset != want_offset)
        {
                tst_resm(TFAIL, "    => Cannot seek to 0x%lx: %s", size, strerror(errno));
                return rv;
        }

        /* - read data from last place */
        tst_resm(TINFO, " - reading one byte of data");
        r_size = read(file_desc, &r_pattern, 1);
        if (r_size != 1)
        {
                tst_resm(TFAIL, "    => 1st read() should not have failed!");
                failure = 1;
        }
        else
        {
                tst_resm(TINFO, "    => 1st read() succeed: 0x%x", r_pattern);
        }

        /* second read should failed */
        tst_resm(TINFO, " - reading again one byte of data");
        r_size = read(file_desc, &r_pattern, 1);
        if (r_size < 0)
        {
                if (errno == ENOSPC)
                {
                        tst_resm(TPASS, "    => 2nd read() failed, got ENOSPC: OK");
                }
                else
                {
                        tst_resm(TFAIL,
                                 "    => 2nd read() failed, but errno is not ENOSPC: %d (%s)",
                                 errno, strerror(errno));
                        failure = 1;
                }
        }
        else if (r_size == 0)
        {
                tst_resm(TINFO, "    => 2nd read() succeed by returning 0" ); /* OK */
        }
        else
        {
                tst_resm(TFAIL, "    => 2nd read() succeed: 0x%x, should have failed!", &r_pattern);
                failure = 1;
        }

        if (failure)
                rv = TFAIL;
        else
                rv = TPASS;

        return rv;

}


/*================================================================================================*/
/*================================================================================================*/
int Erase_Flash(struct erase_info_user *mxc_erase, struct mtd_info_user *mtd_info,
                unsigned int Flash_size)
{
        int     retval = 0;
        int     rv = TFAIL;

        /* Flash_size is the total size to erase and
         * ERASE_BLOCK_SIZE is the memory chunk size to
         * unlock/erase at once step
         */
        mxc_erase->length = ERASE_BLOCK_SIZE;

        /* Erase Flash by blocks of size erasesize (min. erasesize) */
        for (mxc_erase->start = 0; mxc_erase->start < Flash_size;
             mxc_erase->start += mtd_info->erasesize)
        {
                retval = ioctl(file_desc, MEMERASE, mxc_erase);
                if (retval < 0)
                {
                        tst_resm(TFAIL, "ERROR : ioctl MEMERASE fails  : %d", errno);
                        perror("ioctl");
                        return rv;
                }
        }

        rv = TPASS;
        return rv;
}


/*================================================================================================*/
/*================================================================================================*/
int Backup(size_t sz)
{        
        if( backup.data || file_desc <= 0 )
                return FALSE;
                
        backup.sz = sz;
        backup.data = malloc( backup.sz );
        if( !backup.data )
        {
                tst_brkm( TBROK, (void(*)())VT_nor_mtd_test2_cleanup,
                         "%s : Can't allocate %lu bytes memory",
                          __FUNCTION__, (unsigned long)backup.sz );
        }                
        
        /* Backup loop */
        int total_bytes_read = 0, bytes_read;
        char * buf = (char*)backup.data;
        while(total_bytes_read < backup.sz)
        {
                bytes_read = read(file_desc, buf, 16);
                if (bytes_read < 0)
                {
                        tst_resm(TFAIL, "%s : read error, pos = %d", total_bytes_read);
                        perror("read");
                        free( backup.data );
                        backup.data = 0;
                        return FALSE;
                }
                total_bytes_read += bytes_read;
                buf += bytes_read;
        }
        
        tst_resm( TINFO, "Flash data was backuped (%lu bytes)", (unsigned long)backup.sz );
        
        /* flashdump */
        FILE * out = fopen( "flashdump", "wb" );
        if( out )
        {
                fwrite( backup.data, 1, backup.sz, out );
                fclose( out );
        }
        
        return TRUE;
}


/*================================================================================================*/
/*================================================================================================*/
int Restore(void)
{
        if( !backup.data || file_desc <= 0 )
                return FALSE;
        
        /* Restore loop */
        int total_bytes_written = 0, bytes_written;
        char * buf = (char*)backup.data;        
        while(total_bytes_written < backup.sz)
        {
                bytes_written = write(file_desc, buf, 8);
                if(bytes_written < 0)
                {
                        tst_resm(TFAIL, "%s : write error, pos = %d", total_bytes_written);
                        perror("write");
                        return FALSE;
                }
                total_bytes_written += bytes_written;
                buf += bytes_written;
        }
                        
        free( backup.data );
        backup.data = 0;                
        
        tst_resm( TINFO, "Flash data was restored (%lu bytes)", (unsigned long)backup.sz );
        backup.sz = 0;
        
        return TRUE;
}
