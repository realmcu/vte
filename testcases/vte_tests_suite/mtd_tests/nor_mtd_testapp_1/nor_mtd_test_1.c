/*================================================================================================*/
/**
        @file   nor_mtd_test_1.c

        @brief Do write/read/erase operations on a flash memory device
               using MTD subsystem API
               Flash type : 2 Intel StrataFlash 28F256L18 of 16 MB : Total size is 32MB
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
V.Becker/rc023c              04/05/2004     TLSbo39142   Initial version
V.Becker/rc023c              18/06/2004     TLSbo39142   Code reviewed
S.ZAVJALOV/zvjs001c          22/09/2004     TLSbo42376   Print MTD Flash info without any key
D.Kazachkov/e1403c           27/09/2004     TLSbo41355   Return Fail if no regions found
I.Inkina/                    29/09/2004
V.Becker/rc023c              20/10/2004     TLSbo43924   Update after NOR Flash map change
E.Gromazina/NONE             07/07/2005     TLSbo50888   minor fixes
A.Urusov/NONE                13/02/2006     TLSbo61868   Warnings fixup, code formatting
D.Simakov/b00296             06/07/2006     TLSbo67309   Backup/Restore were implemented to
                                                         use the /dev/mtd/1 and /dev/mtd/2 devices
A.Ozerov/b00320              11/12/2006     TLSbo84161   Minor changes.
Rakesh S Joshi/r65956		 24/07/2007		ENGR00042510 Fixed NOR MTD write error and segmentation
														 fault on MXC30031-ADS

====================================================================================================
Portability:  ARM GCC
==================================================================================================*/

/*==================================================================================================
                                        INCLUDE FILES
==================================================================================================*/
/* Standard Include Files */
#include <errno.h>

/* Harness Specific Include Files. */
#include <test.h>

/* Verification Test Environment Include Files */
#include "nor_mtd_test_1.h"


/*==================================================================================================
                                        DEFINES AND MACROS
==================================================================================================*/

#if !defined(TRUE)
    #define TRUE 1
#endif
#if !defined(FALSE)
        #define FALSE 0
#endif


/*==================================================================================================
                                        STRUCTS AND TYPEDEFS
==================================================================================================*/

typedef struct
{
        void * data;
        size_t sz;
} backup_data_t;

/*==================================================================================================
                                        LOCAL VARIABLES
==================================================================================================*/
static int     file_desc = 0;
static backup_data_t backup = {0,0};

/*==================================================================================================
                                        GLOBAL VARIABLES
==================================================================================================*/
extern char device_name[];
extern int flag_get_flash_information;

/*==================================================================================================
                                    LOCAL FUNCTION PROTOTYPES
==================================================================================================*/
int     Erase_Flash(struct erase_info_user *, struct mtd_info_user *, unsigned int);
int     Write_And_Check(unsigned long);
int     Backup(size_t sz);
int     Restore(void);

/*==================================================================================================
                                        LOCAL FUNCTIONS
==================================================================================================*/

/*================================================================================================*/
/*===== VT_nor_mtd_test1_setup =====*/
/**

@param  None

@return On success - return TPASS
        On failure - return the error code
*/
/*================================================================================================*/
int VT_nor_mtd_test1_setup(void)
{
        int     rv = TFAIL;

        /* Open NOR MTD driver */
        tst_resm(TINFO, " Open NOR MTD driver : %s\n", device_name);
        file_desc = open(device_name, O_RDWR);
        sleep(1);
        if (file_desc == -1)
        {
                tst_resm(TFAIL, "ERROR : Open NOR MTD driver fails : %d\n", errno);
                perror("Error: cannot open device \n");
        }
        else
        {
                rv = TPASS;
        }

        return rv;
}

/*================================================================================================*/
/*===== VT_nor_mtd_test1_cleanup =====*/
/**
@brief  assumes the post-condition of the test case execution

@param  None

@return On success - return TPASS
        On failure - return the error code
*/
/*================================================================================================*/
int VT_nor_mtd_test1_cleanup(void)
{
        int     rv = TFAIL;
        int     ret = 0;

        if( !flag_get_flash_information )
        {
                lseek(file_desc, 0, SEEK_SET);
                struct mtd_info_user mxc_mtd;
                struct erase_info_user mxc_erase_mtd;
                ioctl(file_desc, MEMGETINFO, &mxc_mtd);
				Erase_Flash(&mxc_erase_mtd, &mxc_mtd, backup.sz);
                Restore();
        }

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
/*===== VT_nor_mtd_test1 =====*/
/**
@brief  nor_mtd_test1 gets NOR MTD driver information and then
            does basic write/read/erase operations on it

@param  int size_to_write : size of pattern to program

@return On success - return TPASS
        On failure - return the error code
*/
/*================================================================================================*/
int VT_nor_mtd_test1(unsigned long size)
{
        tst_resm(TINFO, "NOR MTD driver test 1");
        tst_resm(TINFO, "Write/Read/Erase flash memory device");

        int     rv = TFAIL;
        int     retval = -EFAULT;
        int     number_of_regions = 0;
        int     i = 0;
        unsigned long size_to_erase = 0;
        off_t   s_position = 0;
        char    flash_type[64];
        unsigned long total_bytes_read = 0;
        ssize_t bytes_read = 0;

        /* Erase NOR Flash memory. Memory is filled with binary value "1" also known as the blank
        * pattern */
        char    read_blank_pattern[16];

        /* MTD specific structures */
        struct mtd_info_user mxc_mtd;
        struct erase_info_user mxc_erase_mtd;
        struct region_info_user mxc_region_mtd;

        /* Get information on NOR Flash. Returns the mtd_info structure */
        tst_resm(TINFO, "Get information on NOR Flash structure");
        retval = ioctl(file_desc, MEMGETINFO, &mxc_mtd);
        if (retval < 0)
        {
                tst_resm(TFAIL, "ERROR : ioctl MEMGETINFO fails  : %d", errno);
                perror("ioctl");
                return rv;
        }

        if( !flag_get_flash_information )
                if( !Backup(size) )
                        return rv;
        lseek(file_desc, 0, SEEK_SET);


        if (flag_get_flash_information)
        {
                switch (mxc_mtd.type)
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
                tst_resm(TINFO, "\nNOR Flash info :\n "
                         "- type : %s\n "
                         "- flags : %d\n "
                         "- size : 0x%lx\n "
                         "- erase size : 0x%lx\n "
                         "- write size : 0x%lx\n "
                         "- size of OOB block : 0x%lx\n "
                         "- Error check type : %d\n "
                         "- Error check size : 0x%lx",
                         flash_type,
                         mxc_mtd.flags,
                         (long) mxc_mtd.size,
                         (long) mxc_mtd.erasesize,
                         (long) mxc_mtd.writesize,
                         (long) mxc_mtd.oobsize, mxc_mtd.ecctype, (long) mxc_mtd.eccsize);

                /* Get number of erase regions on NOR flash device */
                tst_resm(TINFO, "Get number of regions in Flash memory");
                retval = ioctl(file_desc, MEMGETREGIONCOUNT, &number_of_regions);
                if (retval < 0)
                {
                        tst_resm(TFAIL, "ERROR : ioctl MEMGETREGIONCOUNT fails  : %d", errno);
                        perror("ioctl");
                        return rv;
                }

                tst_resm(TINFO, "Number of regions in NOR Flash memory : %d", number_of_regions);

                /* Get memory region information */
                if( number_of_regions > 0 )
                {
                        tst_resm(TINFO, "Get memory region information");
                        i = 0;
                        do
                        {
                                mxc_region_mtd.regionindex = i;
                                retval = ioctl(file_desc, MEMGETREGIONINFO, &mxc_region_mtd);
                                if (retval < 0)
                                {
                                        if (errno == EINVAL)
                                        {
                                                tst_resm(TFAIL, "ERROR: Number of regions is null (0)");
                                        }
                                        else
                                        {
                                                tst_resm(TFAIL,
                                                         "ERROR : ioctl MEMGETREGIONINFO fails  : %d",
                                                         errno);
                                        }
                                        perror("ioctl");
                                        return rv;
                                }
                                i++;
                        } while (i <= number_of_regions);
                }

                tst_resm(TINFO, "Information on memory region index %d :\n "
                         "- number of blocks : %d\n "
                         "- erase size : %d\n "
                         "- offset : 0x%lx",
                         mxc_region_mtd.regionindex,
                         mxc_region_mtd.numblocks,
                         mxc_region_mtd.erasesize, (long) mxc_region_mtd.offset);

        }
        else
        {

                /* Erase NOR Flash */
#ifdef CONFIG_ARCH_MXC92323
				if (size >= mxc_mtd.erasesize )
				{
					tst_resm(TFAIL, "Size should be lesser than 0x%lx\n ", (long) mxc_mtd.erasesize);
					return TFAIL;
				}
#endif

                size_to_erase = size;
                rv = Erase_Flash(&mxc_erase_mtd, &mxc_mtd, size_to_erase);
                if (rv == TFAIL)
                {
                        return rv;
                }

                /* Verify that memory is blanked when erased */
                tst_resm(TINFO, "Check that memory is blanked when erased (0xFF)");

                /* Set start position */
                s_position = lseek(file_desc, 0, SEEK_SET);
                tst_resm(TINFO, "Start position : 0x%lx\n", s_position);
                tst_resm(TINFO, "In asynchronous Page-mode read four data words are "
                         "read simultaneously == 16 bytes");
                rv = TFAIL;
#ifndef CONFIG_ARCH_MXC92323
                while (total_bytes_read < size)
                {
                        bytes_read = read(file_desc, read_blank_pattern, 16 * sizeof(char));
                        if (bytes_read < 0)
                        {
                                tst_resm(TFAIL, "ERROR : Read fails  : %d", errno);
                                perror("read");
                                return rv;
                        }

                        for (i = 0; i < 16; i++)
                        {
                                if (read_blank_pattern[i] != 0xFF)
                                {
                                        tst_resm(TFAIL, "ERROR : Read pattern is not equal "
                                                 "to blank pattern : %d", errno);
                                        return rv;
                                }
                        }
                        total_bytes_read += bytes_read;
                }
#endif
                /* Write and read to NOR Flash */
                rv = Write_And_Check(size);

                if (rv == TFAIL)
                {
                        return rv;
                }

                /* Get last memory region information */
                if( number_of_regions )
                {
                        mxc_region_mtd.regionindex = number_of_regions;
                        retval = ioctl(file_desc, MEMGETREGIONINFO, &mxc_region_mtd);
                        if (retval < 0)
                        {
                                if (errno == EINVAL)
                                {
                                        tst_resm(TFAIL, "ERROR: Number of regions is null (0)");
                                }
                                else
                                {
                                        tst_resm(TFAIL, "ERROR : ioctl MEMGETREGIONINFO fails  : %d",
                                         errno);
                                        perror("ioctl");
                                        return TFAIL;
                                }
                        }
                }

                tst_resm(TINFO, "Information on memory region index %d :\n "
                         "- number of blocks : %d\n "
                         "- erase size : %d\n "
                         "- offset : 0x%lx",
                         mxc_region_mtd.regionindex,
                         mxc_region_mtd.numblocks,
                         mxc_region_mtd.erasesize, (long) mxc_region_mtd.offset);
        }
        rv = TPASS;
        return rv;
}

/* Write NOR Flash with pattern and size. Then read to check if write operation was successfull */
int Write_And_Check(unsigned long size)
{
        int     retval = 0;
        int     rv = TFAIL;
        int     pattern_length = 0;
        ssize_t bytes_written = 0;
        ssize_t bytes_read = 0;
        unsigned long total_bytes_read = 0;
        off_t   s_position = 0;
        char    pattern[8];

#ifdef CONFIG_ARCH_MXC92323
        char    readpat[9];

		readpat[8]='\0';
        /* Set start position */
        s_position = lseek(file_desc, 0, SEEK_SET);
        tst_resm(TINFO, "Start position : 0x%lx\n", s_position);
        tst_resm(TINFO, "Write and Read Flash and compare to see if pattern is same as wrote pattern");
        pattern_length = 8;

        while (total_bytes_read < size)
        {
                strcpy(pattern, "A5B6C7D8");
                bytes_written = write(file_desc, pattern, 8 * sizeof(char));
                if (bytes_written < 0)
                {
                        tst_resm(TFAIL, "ERROR : Write fails  : %d,bytes_written =%d , total_bytes_read=%d\n ", errno,bytes_written,total_bytes_read);
                        perror("write");
                		return TFAIL;
                }
        		if ( (lseek(file_desc, -8 , SEEK_CUR)) < 0 )
				{
					tst_resm(TFAIL, "lseek Failed");
                	return TFAIL;
				}
                strcpy(readpat, "r5r6r7r8");
                bytes_read = read(file_desc, readpat, 8 * sizeof(char));
                if (bytes_read < 0)
                {
                        tst_resm(TFAIL, "ERROR : Read fails  : %d", errno);
                        perror("read");
                		return TFAIL;
                }
                total_bytes_read += SIZE_WRITE_BASE;
                retval = strncmp(pattern, readpat, pattern_length);
                if (retval != 0)
                {
                        tst_resm(TFAIL, "ERROR : Read pattern is not equal to write "
                                 "pattern : %d", errno);
                        perror("strncmp");
                		return TFAIL;
                }
				//tst_resm(TINFO,"comparision done");
				lseek(file_desc, SIZE_WRITE_BASE , SEEK_CUR);
        }

        rv = TPASS;
        return rv;
#else
        unsigned long total_bytes_written = 0;
        char    read_pattern[8];

        /* Make pattern to write on a word boundary */
        /* while (pattern_length < size*sizeof(char)) */
        /* { */
        /* strcat(pattern, "A5"); */
        /* pattern_length = strlen(pattern); */
        /* } */

        /* Write to NOR Flash memory frmxc_mtd.erasesizeom start address with the pattern */
        tst_resm(TINFO, "Fill NOR Flash with pattern. This pattern is word-aligned");

        /* Set start position */
        s_position = lseek(file_desc, 0, SEEK_SET);
        tst_resm(TINFO, "Start position : 0x%lx\n", s_position);

        while (total_bytes_written < size)
        {
                strcpy(pattern, "A5A5A5A5");
                bytes_written = write(file_desc, pattern, 8 * sizeof(char));
                if (bytes_written < 0)
                {
                        tst_resm(TFAIL, "ERROR : Did not write correctly on a %d-bit boundary "
                                 ": %d", size * sizeof(char), errno);
                        perror("write");
                        return rv;
                }
                total_bytes_written += bytes_written;
        }

        /* Read NOR flash to check that it has been successfully written */
        tst_resm(TINFO, "Read NOR flash to check that it has been successfully written");

        /* Set start position */
        s_position = lseek(file_desc, 0, SEEK_SET);
        tst_resm(TINFO, "Start position : 0x%lx\n", s_position);
        tst_resm(TINFO, "Read Flash and compare to see if pattern is same as wrote pattern");
        pattern_length = 9;

        while (total_bytes_read < size)
        {
                bytes_read = read(file_desc, read_pattern, 8 * sizeof(char));
                if (bytes_read < 0)
                {
                        tst_resm(TFAIL, "ERROR : Read fails  : %d", errno);
                        perror("read");
                        return rv;
                }
                total_bytes_read += bytes_read;

                retval = strncmp(pattern, read_pattern, pattern_length - 1);
                if (retval != 0)
                {
                        tst_resm(TFAIL, "ERROR : Read pattern is not equal to write "
                                 "pattern : %d", errno);
                        perror("strncmp");
                        return rv;
                }
        }

        rv = TPASS;
        return rv;
#endif
}

/* Erase NOR Flash memory. Memory is filled with binary value "1" */
int Erase_Flash(struct erase_info_user *mxc_erase, struct mtd_info_user *mtd_info,
                unsigned int Flash_size)
{
        int     retval = 0;
        int     rv = TFAIL;
        off_t   s_position = 0;

        /* Flash_size is the total size to erase and
         * ERASE_BLOCK_SIZE is the memory chunk size to
         * unlock/erase at once step
         */
        mxc_erase->length = ERASE_BLOCK_SIZE;
        s_position = lseek(file_desc, 0, SEEK_SET);

        tst_resm(TINFO, "Information on erase mtd structure :\n "
                 "- start address : 0x%lx\n "
                 "- erase size : 0x%lx\n "
                 "of blocks 0x%lx\n",
                 (long) mxc_erase->start, (long) Flash_size, (long) mtd_info->erasesize);
        /* Erase Flash by blocks of size erasesize (min. erasesize) */
        for (mxc_erase->start = 0; mxc_erase->start < Flash_size;
             mxc_erase->start += mtd_info->erasesize)
        {
                tst_resm(TINFO, "Performing Flash Erase of length %lx. Offset is now 0x%lx",
                         (unsigned long) Flash_size, (long) mxc_erase->start);

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
                tst_brkm( TBROK, (void(*)())VT_nor_mtd_test1_cleanup,
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
        else
        {
                printf("Fail to open flashdump\n");
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
