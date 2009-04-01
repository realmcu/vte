/*================================================================================================*/
/**
        @file   nand_mtd_test.h

        @brief  NAND MTD test header file
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
S.ZAVJALOV/ZVJS001C          27/07/2004     TLSbo40261  Initial version
L.Delaspre/rc149c            08/11/2004     TLSbo44472  Resolve compilation issue
C.GAGNERAUD/cgag1c           09/11/2004     TLSbo44474  Warnings fixup.
A.Ozerov/b00320              19/04/2006     TLSbo61865  Cast to coding conversions

==================================================================================================*/
#ifndef NAND_MTD_TEST_H
#define NAND_MTD_TEST_H

#ifdef __cplusplus
extern "C"{
#endif

/*==================================================================================================
                                        INCLUDE FILES
==================================================================================================*/
#define _XOPEN_SOURCE        500        /* for pwrite() and pread() */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/compiler.h>
#include <mtd/mtd-user.h>
#include <sys/time.h>


#include <test.h>
#include <usctest.h>

/*==================================================================================================
                                        DEFINES AND MACROS
==================================================================================================*/
#define TEST_CASE_INFO         "INFO"
#define TEST_CASE_WRNER        "WRNER"
#define TEST_CASE_RDRW         "RDRW"
//add by dongfang
#define TEST_CASE_GETBADBLOCK   "BADBLK"     //check badblock and unlock, lock ioctl
#define TEST_CASE_PERFORMACE      "PERFORM"   //get read/write speed
#define TEST_CASE_THRDRWE              "THRDRWE"   // read/write/erase at threshold station on 10 times
#define TEST_CASE_THRDRWONEPAGE          "THRDRWONEPAGE"  //read/write one page at threshold station on 10 times

#define NAND_MTD_DEVICE        "/dev/mtd/4"

#define ADDR_OFFSET            0x0
#define LENGTH_TEST_MEM        0x40000

/*==================================================================================================
                                    FUNCTION PROTOTYPES
==================================================================================================*/
void help(void);

int VT_nand_mtd_test_setup(void);
int VT_nand_mtd_test_cleanup(void);

int VT_nand_mtd_test_rdrw(void);
int VT_nand_mtd_test_info(void);
int VT_nand_mtd_test_erlck(void);
int VT_nand_mtd_test_wrner(void);
int VT_nand_mtd_test_wrlck(void);

int VT_nand_mtd_test_badblk(void);
int VT_nand_mtd_test_thrdrwe(void);
int VT_nand_mtd_test_regionInfo(void);
int VT_nand_mtd_test_perform(void);
int VT_nand_mtd_test_thrdrwonepage(void);

#ifdef __cplusplus
}
#endif

#endif        /* NAND_MTD_TEST_H */
