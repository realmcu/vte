#Copyright 2005-2009 Freescale Semiconductor, Inc. All Rights Reserved.
#
#The code contained herein is licensed under the GNU General Public
#License. You may obtain a copy of the GNU General Public License
#Version 2 or later at the following locations:
#
#http://www.opensource.org/licenses/gpl-license.html
#http://www.gnu.org/copyleft/gpl.html
#!/bin/sh
##############################################################################
#
# Revision History:
#                          Modification     Tracking
# Author                       Date          Number    Description of Changes
#-----------------------   ------------    ----------  ---------------------
# Spring Zhang               28/05/2008       n/a        Initial ver. 
# Spring                     28/11/2008       n/a      Modify COPYRIGHT header
#############################################################################
# Portability:  ARM sh bash 
#
# File Name:    jffs2_test1.sh
# Total Tests:        1
# Test Strategy: basic file operation such as copy, remove.
# 
# Input:	- $1 - device type
#		    - $2 - device name
#		    - $3 - mount point(dir)
#
# Return:       - 
#
# Use command "./basic_file_ops.sh [device type] [device name] [mount point]" 
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
    export TCID="TGE_LV_FS_0501"       # Test case identifier
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

    tmp_type=`echo $device_type | awk '{ print $1 }'`
    if [ "$tmp_type" = "nfs" ]
    then
        umount $mount_dir 2>/dev/null && sleep 1
        # add for WuKong
        WK_SERVER=10.200.1.1
        ping -c 1 $WK_SERVER
        if [ $? -eq 0 ] 
        then
            device=${WK_SERVER}:/rootfs/temp2
        fi
    else
        umount $device 2>/dev/null && sleep 1
    fi

    if [ -e $mount_dir ]
    then
        rm -rf $mount_dir
    fi
    mkdir $mount_dir

    mount -t $device_type $device $mount_dir || RC=$?
    sleep 2
    if [ $RC -ne 0 ]
    then
        tst_resm TFAIL "Test #1: mount failed, please examine the args, \
            if device name($2) or device type($1) is wrong?"
        return $RC
    fi

#    if [ -z $LTPTMP ] && [ -z $TMPBASE ]
#    then
#        LTPTMP=/tmp
#    else
#        LTPTMP=$TMPBASE
#    fi
#    if [ -z $LTPBIN ] && [ -z $LTPROOT ]
#    then
#        LTPBIN=./
#    else
#        LTPBIN=$LTPROOT/testcases/bin
#    fi

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
    umount $mount_dir && sleep 1
    echo "clean up environment end"
}

# Function:     file_ops
#
# Description:  - file operations: copy, remove, make dir, remove dir, etc.
#
# Exit:         - zero on success
#               - non-zero on failure.
#
file_ops()
{
    RC=0    # Return value from setup, and test functions.
    tst_resm TINFO "Test #1: copy files..."

    dd if=/dev/zero of=$LTPTMP/test.file count=1024 bs=1024
    cp $LTPTMP/test.file $mount_dir || RC=$?
    if [ $RC -ne 0 ]
    then
        tst_resm TFAIL "Test #1: copy failed!"
        return $RC
    fi

    tst_resm TINFO "Test #1: move file..."
    mv $mount_dir/test.file $mount_dir/changed || RC=$?
    if [ $RC -ne 0 ]
    then
        tst_resm TFAIL "Test #1: move file failed!"
        return $RC
    fi

    tst_resm TINFO "Test #1: remove file..."
    rm $mount_dir/changed || RC=$?
    if [ $RC -ne 0 ]
    then
        tst_resm TFAIL "Test #1: remove file failed!"
        return $RC
    fi

    tst_resm TINFO "Test #1: make dir..."
    mkdir $mount_dir/dir1 || RC=$?
    if [ $RC -ne 0 ]
    then
        tst_resm TFAIL "Test #1: make dir failed!"
        return $RC
    fi

    tst_resm TINFO "Test #1: remove dir..."
    rmdir $mount_dir/dir1 || RC=$?
    if [ $RC -ne 0 ]
    then
        tst_resm TFAIL "Test #1: remove dir failed!"
        return $RC
    fi

    tst_resm TPASS "Test #1: jffs2 file opration success."

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

    Use this command to test jffs2/3, yaffs, nfs file system basic functions.
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

file_ops || exit $RC

