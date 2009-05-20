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
# Spring Zhang               11/06/2008       n/a        Initial ver.
# Spring                     28/11/2008       n/a      Modify COPYRIGHT header
#############################################################################
# Portability:  ARM sh bash
#
# File Name:    cramfs_1.sh
# Total Tests:        1
# Test Strategy: basic file operation such as copy on cramfs.
#
# Input: - $1 - device name
#      - $2 - mount point(dir)
#
# Return:       -
#
# Use command "./cramfs_1.sh [device name] [mount point]"
#               to test cramfs file system


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
    RC=0                # Exit values of system commands used

    export TST_TOTAL=1   # Total number of test cases in this file.
    LTPTMP=${TMP}        # Temporary directory to create files, etc.
    export TCID="TGE_LV_FS_0521"       # Test case identifier
    export TST_COUNT=0   # Set up is initialized as test 0

    if [ -z $LTPTMP ]
    then
        LTPTMP=/tmp
    fi

    if [ $# -lt 2 ]
    then
        usage
        exit 1
    fi

    trap "cleanup" 0

    device=$1
    mount_dir=$2

    if [ -e $mount_dir ]
    then
        rm -rf $mount_dir
    fi
    mkdir $mount_dir

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

# Function:     cramfs_ops
#
# Description:  - file operations: copy, etc.
#
# Exit:         - zero on success
#               - non-zero on failure.
#
cramfs_ops()
{
    RC=0    # Return value from setup, and test functions.
    tst_resm TINFO "Test #1: mount cramfs image"

    mount -t cramfs -o loop $device $mount_dir || RC=$?
    sleep 2
    if [ $RC -ne 0 ]
    then
        tst_resm TFAIL "Test #1: mount failed, please examine the args, \
        if cramfs image($1) is wrong? Or the test failed!"
        return $RC
    fi

    tst_resm TINFO "Test #1: copy file..."
    cp $mount_dir/still_pictures_tests/Makefile /tmp || RC=$?
    if [ $RC -ne 0 ]
    then
        tst_resm TFAIL "Test #1: copy failed!"
        return $RC
    fi

    tst_resm TINFO "Test #1: check the copied file"
    [ `ls -l /tmp/Makefile |awk '{print $5}'` -gt 1000 ] || RC=$?
    if [ $RC -ne 0 ]
    then
        tst_resm TFAIL "Test #1: copied file error"
        return $RC
    fi

    tst_resm TINFO "Test #1: umount cramfs"
    umount $mount_dir || RC=$?
    if [ $RC -ne 0 ]
    then
        tst_resm TFAIL "Test #1: umount failed"
        return $RC
    fi

    tst_resm TPASS "Test #1: cramfs file opration success."

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

    Use this command to test cramfs file system basic functions.
    usage: ./${0##*/} [cramfs image] [mount point]
           cramfs image: the location of cramfs image
    e.g.:  ./${0##*/} sample.cramfs /tmp/cramfs

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

cramfs_ops || exit $RC

