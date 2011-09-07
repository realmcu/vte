#!/bin/bash
#Copyright 2010-2011 Freescale Semiconductor, Inc. All Rights Reserved.
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
# File :       cpu_lowfreq.sh
#
# Description: Test suspend and resume with low frequency.
#    
#==============================================================================
#Revision History:
#                       Modification     Tracking
# Author                    Date          Number    Description of Changes
#--------------------   ------------    ----------  ----------------------
# Spring Zhang           Jan.11,2010       n/a      Initial version
# Spring Zhang           May.10,2010       n/a      Add support for mx53
# Spring Zhang           Mar.28,2011       n/a      Add mx51 400MHz WP
# Spring Zhang           Mar.28,2011       n/a      Add mx53 WPs convert test
# Spring Zhang           Aug.8,2011        n/a      MX53 WPs delete 160MHz, fix
#   several severe bugs
# Spring Zhang           Aug.9,2011        n/a      Merge MX37 case

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
    export TST_TOTAL=1

    # The TCID and TST_COUNT variables are required by the LTP 
    # command line harness APIs, these variables are not local to this program.

    # Test case identifier
    export TCID="TGE_LV_CPU_LOWFREQ"
    # Set up is initialized as test 0
    export TST_COUNT=0
    # Initialize cleanup function to execute on program exit.
    # This function will be called before the test program exits.
    trap "cleanup" 0

    platfm.sh || platfm=$?
    if [ $platfm -eq 67 ]; then
            RC=$platfm
            return $RC
    fi
    
    CUR_FREQ_GETTER=/sys/devices/system/cpu/cpu0/cpufreq/scaling_cur_freq
    CPU_CTRL=/sys/devices/system/cpu/cpu0/cpufreq/scaling_setspeed
    DVFS_CTRL=/sys/devices/platform/mxc_dvfs_core.0/enable
    old_freq=`cat $CUR_FREQ_GETTER`
    if cat $DVFS_CTRL | grep "enabled"; then
        old_dvfs_status=1
    else
        old_dvfs_status=0
    fi

    echo "`cat $DVFS_CTRL`, and CPU freq: $old_freq"
    return $RC
}

cleanup()
{
    #resume to old frequency
    echo "Resume to old frequency and DVFS mode"
    echo $old_freq > $CPU_CTRL
    echo $old_dvfs_status > $DVFS_CTRL

    return 0
}

# Function:     usage
#
# Description:  - display help info.
#
# Return        - none
usage()
{
cat <<-EOF

    Use this command to test CPU Frequency functions.
    usage: ./${0##*/} 1  -- suspend test at all WPs
           ./${0##*/} 2  -- All working points inter-convert test
    e.g.: ./${0##*/} 2

EOF
}

#parameter: $1-working point
lowfreq_suspend()
{
    RC=1
    wp=$1

    echo 0 > $DVFS_CTRL
    sleep 1
    echo $wp > $CPU_CTRL
    echo =========To test cpu works at $wp=========
    #cpufreq-info
    cur_freq=`cat $CUR_FREQ_GETTER`
    if [ $wp -ne $(cpufreq-info -f) ]; then
       echo =========Current cpu does not work at $wp but $(cpufreq-info -f)=========
       return $RC
    fi

    sleep 3 
    if [ -e /dev/rtc ]; then
       rtc_testapp_6 -m standby -T 15
    else
       rtc_testapp_6 -m standby -T 15 -d rtc0 
    fi
    tst_resm TPASS "Resume from suspend..."

    sleep 5 
    if [ -e /dev/rtc ]; then
        rtc_testapp_6 -m mem -T 15
    else
        rtc_testapp_6 -m mem -T 15 -d rtc0 
    fi
    tst_resm TPASS "Resume from mem..."
}

# Function moved from dvfs_mx37.sh 
# Authur: Blake Liu@20081015
wp_convert_mx37()
{
    RC=0
    export TCID="TGE_LV_DVFS_WP_CONVERT_MX37"

    # For imx37
    for freq in 600000 532000 200000 600000 200000 532000
    do 
        cur_freq=`cat $DVFS_DIR/cpuinfo_cur_freq`
        echo $freq > $DVFS_DIR/scaling_setspeed
        res=`cat $DVFS_DIR/cpuinfo_cur_freq`
        if [ $res -eq $freq ];then
            tst_resm TPASS "cpu frequency changed from $cur_freq --> $res: ok"
        else
            tst_resm TFAIL "cpu frequency changed from $cur_freq --> $res: fail"
            RC=1
        fi
        sleep 3
    done 

    return $RC
}

# function supporting MX53
wp_convert()
{
    export TCID="TGE_LV_DVFS_WP_CONVERT"
    RC=0

    # Set working points arrays
    if [ $platfm -eq 53 ]; then
        WP_list="400000 800000 1000000"
        WP_value[0]=400000
        WP_value[1]=800000
        WP_value[2]=1000000
        rand_factor=3
    elif [ $platfm -eq 41 ]; then
        WP_list="160000 400000 800000"
        WP_value[0]=160000
        WP_value[1]=400000
        WP_value[2]=800000
        rand_factor=3
    elif [ $platfm -eq 51 ]; then
        WP_list="160000 800000"
        WP_value[0]=160000
        WP_value[1]=800000
        rand_factor=2
    elif [ $platfm -eq 37 ]; then
        WP_list="600000 532000 200000"
        WP_value[0]=600000
        WP_value[1]=532000
        WP_value[2]=200000
        rand_factor=3
    else
        tst_resm TFAIL "Platform not supported"
        return $platfm
    fi

    echo 0 > $DVFS_CTRL

    for i in $WP_list; do
        echo $i > $CPU_CTRL
        cur_freq=`cat $CUR_FREQ_GETTER`
        for j in $WP_list; do
            if [ $cur_freq -ne $j ]; then
                echo $j > $CPU_CTRL
                if [ $j -ne $(cpufreq-info -f) ]; then
                    tst_resm TFAIL "Can't convert CPU Working point from $i to $j"
                    RC=1
                    return $RC
                fi
                echo $i > $CPU_CTRL
                if [ $i -ne $(cpufreq-info -f) ]; then
                    tst_resm TFAIL "Can't convert CPU Working point from $j to $i"
                    RC=2
                    return $RC
                fi
            fi
        done
    done

    #Random test for 7 times
    times=32
    count=0
    while [ $count -lt $times ]; do
        rand=`od -vAn -N4 -tu4 < /dev/urandom`
        rand=`expr $rand % $rand_factor`
        value=${WP_value[$rand]}
        tst_resm TINFO "Change working point to $value"
        cpufreq-set -f $value
        value_ret=`cpufreq-info -f`
        if [ $value_ret -ne $value ]; then
            RC=3
            return $RC
        fi
        count=`expr $count + 1`
    done

    tst_resm TPASS "WP convert test"
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

if [ $# -lt 1 ]; then
    usage
    exit 1
fi

# bash specified script, using array, not dash-compatibility
setup  || exit $RC

case "$1" in
    1)
    if [ $platfm -eq 37 ]; then
        WorkPoint_list="600000 532000 200000"
    elif [ $platfm -eq 51 ]; then
        WorkPoint_list="160000 800000"
    elif [ $platfm -eq 41 ]; then
        WorkPoint_list="160000 400000 800000"
    elif [ $platfm -eq 53 ]; then
        #MX53 SMD working points deletes 160MHz
        #WorkPoint_list="160000 400000 800000 1000000"
        WorkPoint_list="400000 800000 1000000"
    fi
    for WorkPoint in $WorkPoint_list; do
        lowfreq_suspend $WorkPoint|| exit $RC
    done
    ;;
    2)
        wp_convert || exit $RC
    ;;
    *)
    usage
    exit 1
    ;;
esac
