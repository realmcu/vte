#!/bin/sh
##############################################################################
#
#  Copyright 2004-2008 Freescale Semiconductor, Inc. All Rights Reserved.
#
##############################################################################
#
#  The code contained herein is licensed under the GNU Lesser General Public
#  License.  You may obtain a copy of the GNU Lesser General Public License
#  Version 2.1 or later at the following locations:
#
#  http://www.opensource.org/licenses/lgpl-license.html
#  http://www.gnu.org/copyleft/lgpl.html
#
##############################################################################
#
# Revision History:
#                          Modification     Tracking
# Author                       Date          Number    Description of Changes
#-----------------------   ------------    ----------  ---------------------
# Spring Zhang               03/07/2008       n/a        Initial ver. 
# Spring                     30/10/2008       n/a        Add NAND erase
# Spring                     28/11/2008       n/a      Modify COPYRIGHT header
#############################################################################
# Portability:  ARM sh bash 
#
# File Name:    jffs2_2.sh
# Total Tests:      1
# Test Strategy: file system stress test
# 
# Input:	- $1 - device type
#		    - $2 - device name
#		    - $3 - mount point(dir)
#
# Return:       - 
#
# Use command "./jffs2_test2.sh [device type] [device name] [mount point]" 
#               to test jffs2, yaffs, jffs3 file system


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
    # Initialize return code to zero.
    RC=0                 # Exit values of system commands used

    export TST_TOTAL=1   # Total number of test cases in this file.
    LTPTMP=${TMP}        # Temporary directory to create files, etc.
    export TCID="TGE_LV_FS_0502"       # Test case identifier
    export TST_COUNT=0   # Set up is initialized as test 0
    BIN_DIR=`dirname $0`
    export PATH=$PATH:$BIN_DIR

    if [ -z $LTPTMP ]
    then
        LTPTMP=/tmp
    fi

    if [ $# -lt 3 ]
    then
        usage
        exit 1
    fi

    trap "cleanup" 0

    device_type=$1
    device=$2
    mount_dir=$3

    if [ $device_type = "nfs" ]
    then
        umount $mount_dir 2>/dev/null && sleep 1
    else
        umount $device 2>/dev/null && sleep 1
    fi

    if [ $device_type == "jffs2" ]
    then
        mtd_dev=`echo $device|sed 's/mtdblock/mtd/'`
        flash_eraseall $mtd_dev > /dev/null
    fi

    if [ -e $mount_dir ]
    then
        rm -rf $mount_dir
    fi
    mkdir $mount_dir

    #mount -t jffs2 /dev/mtdblock2 /tmp/nand   
    mount -t $device_type $device $mount_dir || RC=$?
    sleep 2
    if [ $RC -ne 0 ]
    then
        tst_resm TFAIL "Test #1: mount failed, please examine the args, \
            if device name($1) or device type($2) is wrong?"
        return $RC
    fi

}

# Function:     cleanup
#
# Description   - remove temporary files and directories.
#
# Return        - zero on success
#               - non zero on failure. return value from commands ($RC)
cleanup() 
{
    echo "clean up environment..."
    umount $device && sleep 1
    umount $mount_dir && sleep 1
    echo "clean up environment end"
}

# Function:     grow_files
#
# Description:  - grow a list of files, use "LTP growfiles"
#
# Exit:         - zero on success
#               - non-zero on failure.
#
grow_files()
{
    # store old dir, cd to mount_dir, which is the real to-be-test file system
    OLDDIR=`pwd`
    cd $mount_dir

    RC=0    # Return value from setup, and test functions.
    tst_resm TINFO "Test #1: file system stress test: grow files..."

    growfiles_list.sh || RC=$?

    if [ $RC -ne 0 ]
    then
        tst_resm TFAIL "Test #1: the last failure test cases is #$RC"
        return $RC
    fi

    tst_resm TPASS "Test #1: file system stress test success."

    cd $OLDDIR

    return $RC
}

# Function:     usage
#
# Description:  - display help info.
#
# Return        - none
usage()
{
    cat <<-EOF 

    Use this command to test jffs2/3, yaffs, nfs file system stress test.
    usage: ./${0##*/} [device type] [device name] [mount point]
           device type: jffs2, yaffs, jffs3
           device name: /dev/...
    e.g.: ./${0##*/} jffs2 /dev/mtdblock6 /mnt/nand
          ./${0##*/} yaffs /dev/mtdblock6 /tmp/yaffs
    for nfs: device type must be quoted(""), e.g.:
      ./${0##*/} "nfs -o nolock,rsize=1024,wsize=1024" 10.192.220.45:/rootfs/Release/LPDK_1_3_MX31 /tmp/nand

EOF
}


# Function:     main
#
# Description:  - Execute all tests, exit with test status.
#
# Exit:         - zero on success
#               - non-zero on failure.
#
RC=0    # Return value from setup, and test functions.

#"" will pass the whole args to function setup()
setup "$@" || exit $RC

grow_files || exit $RC

