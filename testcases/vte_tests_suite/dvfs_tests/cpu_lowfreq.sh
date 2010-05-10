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

    return $RC
}

cleanup()
{
    #resume to old frequency
    echo "Resume to old frequency"
    echo $old_freq > $CPU_CTRL
}

lowfreq_suspend()
{
    RC=0

    platfm.sh || platfm=$?
    if [ $platfm -eq 67 ]; then
        RC=$platfm
        return $RC
    fi

    CPU_CTRL=/sys/devices/system/cpu/cpu0/cpufreq/scaling_setspeed
    old_freq=`cat /sys/devices/system/cpu/cpu0/cpufreq/scaling_cur_freq`
    if [ $platfm -eq 37 ]; then
        echo 200000 > $CPU_CTRL
    elif [ $platfm -eq 51 ] || [ $platfm -eq 41 ] || [ $platfm -eq 53 ]; then
        echo 160000 > $CPU_CTRL
    else
        tst_resm TWARN "platform not support"
        RC=67
        return $RC
    fi

    sleep 3 
    if [ -e /dev/rtc ]; then
       rtc_testapp_6 -m standby -T 10
    else
       rtc_testapp_6 -m standby -T 10 -d rtc0 
    fi
    tst_resm TPASS "Resume from suspend..."

    sleep 5
    if [ -e /dev/rtc ]; then
        rtc_testapp_6 -m mem -T 15
    else
        rtc_testapp_6 -m mem -T 15 -d rtc0
    fi
    tst_resm TPASS "Resume from mem..."
    
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
lowfreq_suspend || exit $RC

