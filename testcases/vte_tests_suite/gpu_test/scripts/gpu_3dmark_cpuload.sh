#!/bin/sh
##############################################################################
#Copyright (C) 2010 Freescale Semiconductor, Inc. All Rights Reserved.
#
#The code contained herein is licensed under the GNU General Public
#License. You may obtain a copy of the GNU General Public License
#Version 2 or later at the following locations:
#
#http://www.opensource.org/licenses/gpl-license.html
#http://www.gnu.org/copyleft/gpl.html
##############################################################################
#
# Revision History:
#                      Modification     Tracking
# Author                   Date          Number    Description of Changes
#-------------------   ------------    ----------  ---------------------
# Spring Zhang          07/22/2010       n/a        Initial ver. 
#############################################################################
# File Name:    
# Total Tests:        1

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
    export TCID="TGE_LV_3DMARK"       # Test case identifier
    export TST_COUNT=0   # Set up is initialized as test 0
    BIN_DIR=`dirname $0`
    export PATH=$PATH:$BIN_DIR

    if [ -z $LTPTMP ]
    then
        LTPTMP=/tmp
    fi

    if [ $# -lt 1 ]; then
        usage
        RC=67
        return $RC
    fi

    trap "cleanup" 0

    gpu-install install || {
       modprobe gpu || {
            tst_resm TBROK "Test #1: Can't probe gpu module"
            RC=66
            return $RC
       } 
    }

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

    return $RC
}

# Function:     gpu_cpuload     
#
# Description:  Collect CPU usage
#
# Exit:         zero on success
#               non-zero on failure.
#
gpu_cpuload()
{
    RC=0    # Return value from setup, and test functions.

    tst_resm TINFO "Test #1: start to record GPU 3DMark loading."

    #it will get "  PID USER      PR  NI  VIRT  RES  SHR S %CPU %MEM    TIME+  COMMAND"
    line=`top -d1 -n1 |grep CPU`
    col_count=0
    for word in $line; do
        [ "$word" != "%CPU" ] || break
        col_count=`expr $col_count + 1`
    done

    top -d1 |grep fm_oes_player > pid_top.log &
    bpid_top=$!

    OLDPWD=`pwd`
    cd /mnt/nfs/util/3dmarksmobile_EGL/bin/bin/linux_iMX51
    if [ "$1" = "FB" ]; then
        cd /mnt/nfs/util/3dmarks_FB/bin/bin/windows
    fi
    ./fm_oes_player 
    bpid_gpu=$!

    kill -9 $bpid_top
    cd $OLDPWD

    rm -f gpu_3dmark_perf_cpuloading.csv
    #calculate cpu loading
    if grep -i ubuntu /etc/issue ; then
        loading_array=`cat pid_top.log | awk '{print $'$col_count'}'`
    else
        loading_array=`cat pid_top.log | awk '{print $'$col_count'}' | sed 's/\%//'`
    fi

    min=100
    max=0
    sum=0
    array_size=0
    for la in $loading_array 
    do
        la=`echo ${la%%.*}`
        if [ $la -eq 0 ]
        then
            continue
        fi

        echo "la=$la"
        array_size=`expr $array_size + 1`
        sum=`expr $sum + $la`
        if [ $la -lt $min  ]; then
            let min=la
        fi
        if [ $la -gt $max ]; then
            let max=la
        fi
    done

    echo "array_size=$array_size"
    echo "min=$min" 
    echo "max=$max"
    echo "sum=$sum"
    
    echo "delete the mimum one: $min"
    array_size=`expr $array_size - 1`
    sum=`expr $sum - $min`
    if [ $array_size -gt 0 ]
    then
        loading_avg=`expr $sum / $array_size`
    else
        echo "sampled cpu loading is not valid"
        loading_avg=100
    fi
    echo "average loading=$loading_avg"

    echo "min max loading_avg" > gpu_3dmark_perf_cpuloading.csv
    echo "$min   $max     $loading_avg" >> gpu_3dmark_perf_cpuloading.csv

    tst_resm TINFO "Test #1: CPU loading: min=$min max=$max average=$loading_avg"

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

    Use this command to record GPU 3DMark cpu loading
    usage: ./${0##*/} [FB/EGL]
            FB: Gnome mobile rootfs based on frame buffer
            EGL: Ubuntu rootfs based on X-EGL
    e.g.: ./${0##*/} EGL

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

gpu_cpuload "$@" || exit $RC

