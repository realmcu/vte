/*================================================================================================*/
/**
        @file   ata_driver_testapp.h

        @brief  Header file for ATA Disk driver test.
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

==================================================================================================*/
#ifndef ATA_DRIVER_TESTAPP_H
#define ATA_DRIVER_TESTAPP_H

#ifdef __cplusplus
extern "C"{
#endif

/*==================================================================================================
                                        INCLUDE FILES
==================================================================================================*/
#include <linux/types.h>

/*==================================================================================================
                                        DEFINES AND MACROS
==================================================================================================*/
#undef DO_FLUSHCACHE            /* under construction: force cache flush on -W0 */

#ifndef O_DIRECT
#define O_DIRECT        0x200000        /* direct disk access, not easily obtained from headers */
#endif

#ifndef BLKGETSIZE64
#define BLKGETSIZE64            _IOR(0x12, 114, int)
#endif

#define TIMING_BUF_MB           2
#define TIMING_BUF_BYTES        (TIMING_BUF_MB * 1024 * 1024)
#define BUFCACHE_FACTOR         2

#ifndef VXVM_MAJOR
#define VXVM_MAJOR 199
#endif

#ifndef CCISS_MAJOR
#define CCISS_MAJOR 104
#endif

#define YN(b)   (((b)==0)?"no":"yes")

/*==================================================================================================
                                    STRUCTURES AND TYPEDEFS
==================================================================================================*/
struct xfermode_entry
{
        int val;
        const char *name;
};

static const struct xfermode_entry xfermode_table[] =
{
        { 8,    "pio0" },
        { 9,    "pio1" },
        { 10,   "pio2" },
        { 11,   "pio3" },
        { 12,   "pio4" },
        { 13,   "pio5" },
        { 14,   "pio6" },
        { 15,   "pio7" },
        { 16,   "sdma0" },
        { 17,   "sdma1" },
        { 18,   "sdma2" },
        { 19,   "sdma3" },
        { 20,   "sdma4" },
        { 21,   "sdma5" },
        { 22,   "sdma6" },
        { 23,   "sdma7" },
        { 32,   "mdma0" },
        { 33,   "mdma1" },
        { 34,   "mdma2" },
        { 35,   "mdma3" },
        { 36,   "mdma4" },
        { 37,   "mdma5" },
        { 38,   "mdma6" },
        { 39,   "mdma7" },
        { 64,   "udma0" },
        { 65,   "udma1" },
        { 66,   "udma2" },
        { 67,   "udma3" },
        { 68,   "udma4" },
        { 69,   "udma5" },
        { 70,   "udma6" },
        { 71,   "udma7" },
        { 0, NULL }
};

/*==================================================================================================
                                    GLOBAL VARIABLE DECLARATIONS
==================================================================================================*/

/*==================================================================================================
                                        FUNCTION PROTOTYPES
==================================================================================================*/
void identify(__u16 *id_supplied, const char *devname);
void usage_error(int out);
void flush_buffer_cache(int fd);
int read_big_block(int fd, char *buf);
int write_big_block(int fd, char *buf);
void time_device(int fd);
void no_scsi(void);
void no_xt(void);

int VT_ata_driver_test_setup(void);
int VT_ata_driver_test_cleanup(void);
int VT_ata_driver_test(char *devname, int testcase, int dma_flag, int io32bit, int xfermode, int flush_buffer);
int ata_readperfrom_test(int fd);
int ata_writeperform_test(int fd);
int ata_ReadWritePerformancePIOMode(int fd);
int ata_ReadWritePerformanceMDMAMode(int fd);
int ata_ReadWritePerformanceUDMAMode(int fd);


#ifdef __cplusplus
}
#endif

#endif /* ATA_DRIVER_TESTAPP_H */
