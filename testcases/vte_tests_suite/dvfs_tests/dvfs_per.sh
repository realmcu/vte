#!/bin/sh
##############################################################################
#Copyright 2010-2012 Freescale Semiconductor, Inc. All Rights Reserved.
#
#This program is free software; you can redistribute it and/or modify
#it under the terms of the GNU General Public License as published by
#the Free Software Foundation; either version 2 of the License, or
#(at your option) any later version.
#
#This program is distributed in the hope that it will be useful,
#but WITHOUT ANY WARRANTY; without even the implied warranty of
#MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#GNU General Public License for more details.
#
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
# Author                    Date        Description of Changes
#--------------------   ------------    ----------------------
# Spring Zhang           Jan.12,2010    Initial version
# Spring Zhang           May.10,2010    Add support for mx53
# Spring Zhang           Apr.26,2012    Add busfreq switch for mx6

# Function:     usage
#
# Description:  - display help info.
#
# Return        - none
usage()
{
    cat <<-EOF

    Use this command to test DVFS PER or Busfreq functions.
    usage: ./${0##*/} 1  -- DVFS PER basic test
           ./${0##*/} 2  -- DVFS PER stress test
           ./${0##*/} 3  -- Busfreq mode switch test
           ./${0##*/} 4  -- Suspend and resume stress test in busfreq mode
           e.g.: ./${0##*/} 2

EOF
    exit 67
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
    export TST_TOTAL=4

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

    #enable dvfs-core on mx51&53
    DVFS_CORE_DIR=/sys/bus/platform/drivers/mxc_dvfs_core/mxc_dvfs_core.0
    dvfs_core_ctl=$DVFS_CORE_DIR/enable
    if [ $platfm -eq 51 ] || [ $platfm -eq 41 ] || [ $platfm -eq 53 ]; then
        echo 1 > $dvfs_core_ctl
        res=`cat $dvfs_core_ctl | grep "enabled" | wc -l`

        if [ $res -ne 1 ]; then
            tst_resm TFAIL "Fail to enable dvfs-core"
            RC=1
            return $RC
        fi
    fi

    if ! mount|grep -sq '/sys/kernel/debug'; then
        mount -t debugfs none /sys/kernel/debug
    fi

    modprobe ar6000
    if ifconfig -a |grep wlan0; then
        sleep 2
        iwconfig wlan0 mode managed || return 1
        sleep 2
        iwlist wlan0 scanning | grep FSLLBGAP_001
        iwconfig wlan0 key bbd9837522
        iwconfig wlan0 essid FSLLBGAP_001
        ifconfig wlan0 up
        udhcpc -i wlan0 || dhclient wlan0 || return 1
        route add -host 10.192.225.222 dev wlan0
        sleep 5
        export LOCALIP=$(ifconfig wlan0 | grep 'inet addr:'| grep -v '127.0.0.1' | cut -d: -f2 | awk '{ print $1}')
    fi

    #enable dvfs-per on MX51 only
    DVFS_PER_DIR=/sys/devices/platform/mxc_dvfsper.0
    dvfs_per_ctl=${DVFS_PER_DIR}/enable
    if [ $platfm -eq 51 ] || [ $platfm -eq 41 ]; then
        echo 1 > $dvfs_per_ctl
        res=`cat $dvfs_per_ctl | grep "enabled" | wc -l`
        if [ $res -eq 1 ]; then
            tst_resm TPASS "DVFS-PER is enabled"
        else
            tst_resm TFAIL "fail to enable DVFS-PER"
            RC=1
            return $RC
        fi
    else
        #enable bus frequency scaling on other platforms, from MX53
        # for MX6x, can't enable FEC, HDMI, SATA
        #BUS_FREQ_DIR=/sys/devices/platform/busfreq.0
        BUS_FREQ_DIR=`find /sys/devices -name "*busfreq*"|sed -n '1p'`
        bus_freq_ctl=${BUS_FREQ_DIR}/enable
        echo 1 > $bus_freq_ctl
        res=`cat $bus_freq_ctl | grep "enabled" | wc -l`
        if [ $res -eq 1 ]; then
            tst_resm TPASS "Bus Freq scaling is enabled"
        else
            tst_resm TFAIL "fail to enable BUS-FREQ"
            RC=1
            return $RC
        fi

        # if for mx6
        if [ $platfm -ne 53 ]; then
            # turn off screen
            for i in 0 1 2 3 4; do
                echo 1 > /sys/class/graphics/fb${i}/blank
            done
            # turn off ethernet
            ifconfig eth0 down
            ifconfig eth1 down 2>/dev/null

            ahb_path=`find /sys/kernel/debug/clock -name "ahb_clk"`
            # Clock determination for entered low busfreq mode
            sleep 10
            loop=3
            k=0
            if_success=0
            while [ $k -lt $loop ]; do
                ahb_rate=`cat $ahb_path/rate`
                if [ $ahb_rate -eq 24000000 ]; then
                    if_success=1
                    break
                fi
                k=`expr $k + 1`
                sleep 10
            done
            if [ $if_success -eq 1 ]; then
                echo "TPASS: System enters low busfreq mode"
            else
                echo "TFAIL: System can't enter low busfreq mode, AHB rate doesn't meet 24MHz"
                RC=1
                return $RC
            fi
        fi
    fi

    return $RC
}

cleanup()
{
    if [ $platfm -eq 51 ] || [ $platfm -eq 41 ]; then
        echo 0 > $dvfs_per_ctl
    fi
    if [ $platfm -eq 53 ]; then
        echo 0 > $bus_freq_ctl
    fi

    if [ $platfm -eq 51 ] || [ $platfm -eq 41 ] || [ $platfm -eq 53 ]; then
        echo 0 > $dvfs_core_ctl
    fi
}

dvfs_per_basic()
{
    RC=1

    #LCD test
    lcd_testapp -T 1 -B /dev/fb0 -D 16 -X 10 || return $RC

    #storage test
    storage_all.sh 2

    #V4L2-0071
    v4l_output_testapp -C 2 -R 5 -X 75 -Y 50 -S 5 -F $LTPROOT/testcases/bin/red_BGR24 || return $RC
    vpu_dec_test.sh 1 || return $RC
    
    #suspend test
    rtc_testapp_6 -m standby -T 50 || return $RC

    #ALSA capture test
    adc_test1.sh -f S16_LE -d 5 -c 1 -r 44100 || return $RC

    sleep 5
    rtc_testapp_6 -m mem -T 50 || return $RC

    echo "Pass DVFS-per/busfreq basic test"
    RC=0
    return $RC
}

# Busfreq stress test
dvfs_per_stress()
{
    RC=1

    echo "Start SD/MMC bonnie++ stress test"
    i=0
    if ! mount |grep mmcblk0p1; then
        umount /dev/mmcblk0p1
        mkdir -p /mnt/mmcblk0p1
        mount /dev/mmcblk0p1 /mnt/mmcblk0p1 || {
            RC=$?
            echo "TFAIL: /dev/mmcblk0p1 can't mount"
            return $RC
        }
    fi

    while [ $i -lt 10 ]; do
        i=`expr $i + 1`

        RC=2
        bonnie++ -d /mnt/mmcblk0p1 -s 32 -r 16 -u 0:0 -m FSL || return $RC
        tst_resm TINFO "bonnie++ run times: $i"

        RC=3
        adc_test1.sh -f S16_LE -d 5 -c 1 -r 44100 || return $RC
        tst_resm TINFO "Audio catpure test run times: $i"

        RC=4

        if ifconfig -a|grep wlan0; then
            iperf -B $LOCALIP -c 10.192.225.222 -n 10M || return $RC
            tst_resm TINFO "WiFi test run times: $i"
        fi
    done

    RC=0
    echo "Pass DVFS-per/busfreq stress test"
    return $RC
}

# Busfreq stress test on suspend/resume
suspend_stress()
{
    RC=4
	cp `which rtc_testapp_6` /tmp/ || {
        echo "TFAIL: no rtc_testapp_6 found"
        exit $RC
    }
    i=0
    while [ $i -lt 200 ]; do
        i=`expr $i + 1`
        # Screen will unblank when resume, now blank them again to satisfy the busfreq condition
        for j in 0 1 2 3 4; do
            echo 1 > /sys/class/graphics/fb${j}/blank
        done
        sleep 2
        /tmp/rtc_testapp_6 -m standby -T 50 || return $RC
        tst_resm TINFO "RTC wakeup standby mode test times: $i"

        RC=5
        for j in 0 1 2 3 4; do
            echo 1 > /sys/class/graphics/fb${j}/blank
        done
        sleep 2
        /tmp/rtc_testapp_6 -m mem -T 50 || return $RC
        tst_resm TINFO "RTC wakeup mem mode test times: $i"
    done

	rm -rf  /tmp/rtc_testapp_6 
    RC=0
    echo "Pass DVFS-per/busfreq suspend and resume stress test"
    return $RC
}

# Test enter and exit of busfreq mode when the condition is permitted or not
# Only called by mx6x
busfreq_switch()
{
    RC=0

    i=0
    while [ $i -lt 200 ]; do
        i=`expr $i + 1`
        echo "exit busfreq times $i"
        echo 0 > /sys/class/graphics/fb0/blank
        #TODO add the determination that system exit low busfreq
        sleep 5

        echo "enter busfreq times $i"
        echo 1 > /sys/class/graphics/fb0/blank
        #TODO add the determination that system enter low busfreq
        sleep 5
    done

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
    0)
    # only enable busfreq and quit
    exit 0
    ;;
    1)
    dvfs_per_basic || exit $RC
    ;;
    2)
    dvfs_per_stress || exit $RC
    ;;
    3)
    if [ $platfm -ne 53 ] && [ $platfm -ne 41 ]; then
        busfreq_switch || exit $RC
    else
        echo "Can't run on non-supported platforms"
        usage
    fi
    ;;
    4)
    suspend_stress || exit $RC
    ;;
    *)
    usage
    ;;
esac

