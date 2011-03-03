#!/bin/sh
#Copyright 2010 Freescale Semiconductor, Inc. All Rights Reserved.
#
#This program is free software; you can redistribute it and/or modify
#it under the terms of the GNU General Public License as published by
#the Free Software Foundation; either version 2 of the License, or
#(at your option) any later version.
#This program is distributed in the hope that it will be useful,
#but WITHOUT ANY WARRANTY; without even the implied warranty of
#MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#GNU General Public License for more details.
#You should have received a copy of the GNU General Public License along
#with this program; if not, write to the Free Software Foundation, Inc.,
#51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
##############################################################################
#
#
# File :       dvfs_per.sh
#
# Description: Test DVFS peripheral function and stress test
#    
#==============================================================================
#Revision History:
#                       Modification     Tracking
# Author                    Date          Number    Description of Changes
#--------------------   ------------    ----------  ----------------------
# Spring Zhang           Jan.12,2010       n/a      Initial version
# Spring Zhang           May.10,2010       n/a      Add support for mx53

# Function:     usage
#
# Description:  - display help info.
#
# Return        - none
usage()
{
    cat <<-EOF

    Use this command to test DVFS PER functions.
    usage: ./${0##*/} 1  -- DVFS PER basic test
           ./${0##*/} 2  -- DVFS PER stress test
           e.g.: ./${0##*/} 2

EOF
}

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
    RC=0

    # Total number of test cases in this file. 
    export TST_TOTAL=2

    # The TCID and TST_COUNT variables are required by the LTP 
    # command line harness APIs, these variables are not local to this program.

    # Test case identifier
    export TCID="TGE_LV_DVFS_PER"
    # Set up is initialized as test 0
    export TST_COUNT=0
    # Initialize cleanup function to execute on program exit.
    # This function will be called before the test program exits.
    BIN_DIR=`dirname $0`
    export PATH=$PATH:$BIN_DIR

    LTPTMP=${TMP}        # Temporary directory to create files, etc.
    if [ -z $LTPTMP ]; then
        LTPTMP=/tmp
    fi

    trap "cleanup" 0

    platfm.sh || platfm=$?
    if [ $platfm -eq 67 ]; then
        RC=$platfm
        return $RC
    fi

    #enable dvfs-per on MX51 only
    DVFS_PER_DIR=/sys/devices/platform/mxc_dvfsper.0
    if [ $platfm -eq 51 ] || [ $platfm -eq 41 ]; then
        echo 1 > $DVFS_PER_DIR/enable
        res=`cat $DVFS_PER_DIR/enable | grep "enabled" | wc -l`
        if [ $res -eq 1 ]; then
            tst_resm TPASS "DVFS-PER is enabled"
        else
            tst_resm TFAIL "fail to enable DVFS-PER"
            RC=1
            return $RC
        fi
    fi

    #enable dvfs-core
    echo 1 > /sys/bus/platform/drivers/mxc_dvfs_core/mxc_dvfs_core.0/enable
    res=`cat /sys/bus/platform/drivers/mxc_dvfs_core/mxc_dvfs_core.0/enable | grep "enabled" | wc -l`

    if [ $res -ne 1 ]; then
        tst_resm TFAIL "Fail to enable dvfs-core"
        RC=1
        return $RC
    fi

    return $RC
}

cleanup()
{
    if [ $platfm -eq 51 ] || [ $platfm -eq 41 ]; then
        echo 0 > $DVFS_PER_DIR/enable
    fi
    echo 0 > /sys/bus/platform/drivers/mxc_dvfs_core/mxc_dvfs_core.0/enable
}

dvfs_per_basic()
{
    RC=1

    #LCD test
    lcd_testapp -T 1 -B /dev/fb0 -D 16 -X 10 || return $RC

    #storage test
    #storage_test.sh CD /mnt/mmcblk0p1 /mnt/msc 100 3

    #V4L2 test
    v4l_capture_testapp -C 3 -O BGR32 -o ./output || return $RC
    vpu_dec_test.sh 1
    
    
    #suspend test
    rtc_testapp_6 -m standby -T 10 || return $RC

    #ALSA capture test
    adc_test1.sh -f S16_LE -d 5 -c 1 -r 44100 -A || return $RC

    sleep 5
    rtc_testapp_6 -m mem -T 10 || return $RC

    echo "Pass DVFS basic test"
    RC=0
    return $RC
}

# ToDo: add suspend resume stress test
dvfs_per_stress()
{
    RC=0

    echo "Start USB bonnie++ stress test"
    i=0
    while [ $i -lt 100 ]; do
        bonnie++ -d /mnt/msc -s 32 -r 16 -u 0:0 -m FSL || return $i
        tst_resm TINFO "bonnie++ times: $i"
        i=`expr $i + 1`
    done

    i=0
    while [ $i -lt 50 ]; do
        rtc_testapp_6 -m standby -T 10 || return $i
        rtc_testapp_6 -m mem -T 10 || return $i
        i=`expr $i + 1`
    done

    echo "Pass DVFS stress test"
    return $RC
}

# Function:     main
#
# Description:  - Execute all tests, exit with test status.
#
# Exit:         - zero on success
#               - non-zero on failure.
#
# Return value from setup, and test functions.
RC=0

# bash specified script, using array, not dash-compatibility
setup  || exit $RC

case "$1" in
    1)
    dvfs_per_basic || exit $RC
    ;;
    2)
    dvfs_per_stress || exit $RC
    ;;
    *)
    usage
    exit 67
    ;;
esac

