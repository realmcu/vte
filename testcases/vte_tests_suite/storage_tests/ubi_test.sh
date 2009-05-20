#!/bin/sh
################################################################################
#
#    @file   ubi_test.sh
#
#    @brief  this shell script is used to test the ubifs and ubi module.
#
################################################################################
#
# Copyright 2004-2008 Freescale Semiconductor, Inc. All Rights Reserved.
#
# The code contained herein is licensed under the GNU Lesser General
#
# Public License.  You may obtain a copy of the GNU Lesser General
#
# Public License Version 2.1 or later at the following locations:
#
# http://www.opensource.org/licenses/lgpl-license.html
# http://www.gnu.org/copyleft/lgpl.html
#
################################################################################
#Revision History:
#                            Modification     ClearQuest
#Author                          Date          Number    Description of Changes
#-------------------------   ------------    ----------  -----------------------
#Victor Cui/b19309            29/12/2007      N/A          Initial version
#
################################################################################



# Function:     setup
#
# Description:  - Check if required commands exits
#               - Export global variables
#               - Check if required config files exits
#               - Create temporary files and directories
#
# Return        - zero on success
#               - non zero on failure. return value from commands ($RC)
setup()
{
 #TODO Total test case
 RC=0
 #trap "cleanup" 0

 #TODO add setup scripts
        df | grep "/rootfs/wb"
        if [ $? != 0 ]; then
  mount -t nfs -o nolock,rsize=1024,wsize=1024 10.192.225.222:/rootfs/wb /mnt/nfs
 fi
 PATH=$PATH:/mnt/nfs/util/ubi_test

 insmod ubi.ko
 insmod crc16.ko
 insmod lzo_compress.ko
 insmod lzo_decompress.ko
 insmod crypto_algapi.ko
 insmod deflate.ko
 insmod lzo.ko
 insmod ubifs.ko

 mtd_nand_number=`cat /proc/mtd | wc -l`
 let mtd_nand_number=$mtd_nand_number-2

 cat /proc/mounts | grep /dev/mtdblock$mtd_nand_number
 if [ $? -eq 0 ]; then
       umount /dev/mtdblock$mtd_nand_number
 fi

 return $RC
}
# Function:     cleanup
#
# Description   - remove temporary files and directories.
#
# Return        - zero on success
#               - non zero on failure. return value from commands ($RC)
cleanup()
{
 RC=0
 #TODO add cleanup code here
 rmmod ubifs.ko
 rmmod lzo.ko
 rmmod deflate.ko
 rmmod crypto_algapi.ko
 rmmod lzo_decompress.ko
 rmmod lzo_compress.ko
 rmmod crc16.ko
 rmmod ubi.ko

 return $RC
}


################################################################################
# Function:     ***_test
# Description
#
################################################################################
ubi_test()
{
 RC=0
 #TODO add function test scripte here
 flash_eraseall /dev/mtd$mtd_nand_number > /dev/null
 ubiattach /dev/ubi_ctrl -m $mtd_nand_number
 ubimkvol /dev/ubi0 -N test -m
 mkdir /mnt/ubifs
 mount -t ubifs ubi0:test /mnt/ubifs
 dd if=/dev/urandom of=/mnt/ubifs/tempfile bs=1024k count=1
 if [ $? -eq 0 ]; then
       echo "ubi test: Pass"
 else
  RC=1
       echo "ubi test: Fail"
 fi
 umount /mnt/ubifs
 ubidetach /dev/ubi_ctrl -m $mtd_nand_number
 flash_eraseall /dev/mtd$mtd_nand_number > /dev/null

 #tst_resm TINFO "Test PASS"
}

################################################################################
# Function:     usage()
# Description
# give out how to and using example
################################################################################
usage()
{
 echo "ubi test function!"
}

# main function

RC=0
#TODO check parameter

setup || exit $RC
ubi_test || exit $RC
cleanup || exit $RC

