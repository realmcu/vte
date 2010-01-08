#!/bin/bash
#Copyright 2008-2010 Freescale Semiconductor, Inc. All Rights Reserved.
#
#The code contained herein is licensed under the GNU General Public
#License. You may obtain a copy of the GNU General Public License
#Version 2 or later at the following locations:
#
#http://www.opensource.org/licenses/gpl-license.html
#http://www.gnu.org/copyleft/gpl.html
#
#
# File :        dvfs_test.sh
#
# Description: enable/disable dvfs 
#    
#=====================================================================================
#Revision History:
#                            Modification     Tracking
# Author                          Date          Number    Description of Changes
#-------------------------   ------------    ----------  -----------------------
# Blake                      20081015
# Spring Zhang               20100108            n/a      Reduce code


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
    export TCID="TGE_LV_DVFS_TEST"
    # Set up is initialized as test 0
    export TST_COUNT=0
    # Initialize cleanup function to execute on program exit.
    # This function will be called before the test program exits.
    trap "cleanup" 0

    return $RC
}

cleanup()
{
    #resume to old status
    echo "Resume to old dvfs status"
    echo $cur_status > ${DVFS_DIR[$PLATFORM]}/enable
}

dvfs_dir_set()
{
    #use bash array
    DVFS_DIR[31]=/sys/devices/system/dvfs/dvfs0
    DVFS_DIR[35]=/sys/devices/system/dvfs/dvfs0
    DVFS_DIR[37]=/sys/devices/platform/mxc_dvfs_core.0
	# modified dvfs dir, developer restructure DVFS CORE to common driver. 
    # added by blake , 2009-02-17
	DVFS_DIR[51]=/sys/devices/platform/mxc_dvfs_core.0
    #i.MX51 BBG
	DVFS_DIR[41]=/sys/devices/platform/mxc_dvfs_core.0

    #dvfs status query
    status[31]=status
    status[35]=status
    status[37]=enable
    status[51]=enable
    status[41]=enable
}

dvfs_test()
{
    RC=0

    platfm.sh || PLATFORM=$?
    if [ $PLATFORM -eq 67 ]
    then
        RC=$PLATFORM
        return $RC
    fi

    #store current dvfs status. cur_status=1 - enabled, =0 - disabled
    cur_status=`cat ${DVFS_DIR[$PLATFORM]}/${status[$PLATFORM]} | grep "enabled" | wc -l`

    # For imx31/35/37/51 
    echo 1 > ${DVFS_DIR[$PLATFORM]}/enable
    res=`cat ${DVFS_DIR[$PLATFORM]}/${status[$PLATFORM]} | grep "enabled" | wc -l`
    if [ $res -eq 1 ];then
        tst_resm TPASS "dvfs is enabled"
    else
        tst_resm TFAIL "fail to enable dvfs"
        RC=1
        return $RC
    fi
    sleep 3 
    
    echo 0 > ${DVFS_DIR[$PLATFORM]}/enable
    res=`cat ${DVFS_DIR[$PLATFORM]}/${status[$PLATFORM]} | grep "disabled" | wc -l`
    if [ $res -eq 1 ];then
        tst_resm TPASS "dvfs is disabled"
    else
        tst_resm TFAIL "fail to disable dvfs"
        RC=1
    fi
	
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
dvfs_dir_set || exit $RC
dvfs_test || exit $RC

