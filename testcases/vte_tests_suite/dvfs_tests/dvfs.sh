#Copyright 2005-2009 Freescale Semiconductor, Inc. All Rights Reserved.
#
#The code contained herein is licensed under the GNU General Public
#License. You may obtain a copy of the GNU General Public License
#Version 2 or later at the following locations:
#
#http://www.opensource.org/licenses/gpl-license.html
#http://www.gnu.org/copyleft/gpl.html
#!/bin/sh
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
#-------------------------   ------------    ----------  -------------------------------------------
# Blake                      20081015

#Set path variable to add vte binaries
#export TESTCASES_HOME= `pwd`
#export PATH=${PATH}:${TESTCASES_HOME}

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
    # Total number of test cases in this file. 
    export TST_TOTAL=1

    # The TCID and TST_COUNT variables are required by the LTP 
    # command line harness APIs, these variables are not local to this program.

    # Test case identifier
    export TCID="dvfs_test"
    # Set up is initialized as test 0
    export TST_COUNT=0
    # Initialize cleanup function to execute on program exit.
    # This function will be called before the test program exits.
    trap "cleanup" 0
    RC=0
    return $RC
}

cleanup()
{
    echo "CLEANUP ";
}

env_test()
{
    RC=0
    local result=0
    PLATFORM=0

    # Get cpu info and which platform
    result=`cat /proc/cpuinfo | grep "Revision" | grep " 31.*" | wc -l`
    if [ $result -eq 1 ];then
    PLATFORM=31
    DVFS_DIR=/sys/devices/system/dvfs/dvfs0
    fi

    result=`cat /proc/cpuinfo | grep "Revision" | grep " 35.*" | wc -l`
    if [ $result -eq 1 ];then
    PLATFORM=35
    DVFS_DIR=/sys/devices/system/dvfs/dvfs0
    fi

	#surpport imx37 platform , added by blake , 2009-02-17
	result=`cat /proc/cpuinfo | grep "Revision" | grep " 37.*" | wc -l`
    if [ $result -eq 1 ];then
    PLATFORM=37
    DVFS_DIR=/sys/devices/platform/mxc_dvfs_core.0
    fi
    
	result=`cat /proc/cpuinfo | grep "Revision" | grep " 51.*" | wc -l`
    if [ $result -eq 1 ];then
    PLATFORM=51
    #DVFS_DIR=/sys/devices/system/dvfs/dvfs0
	#modified dvfs dir, developer restructure DVFS CORE to common driver. added by blake , 2009-02-17
	DVFS_DIR=/sys/devices/platform/mxc_dvfs_core.0
    fi

    if [ $PLATFORM -eq 0 ]
    then
        tst_resm TFAIL "Only support imx31/35/37/51 platform! "
        RC=1
        return $RC
    fi

    return $RC
}

dvfs_test()
{
    RC=0

    # For imx31/35/37/51 
    if [ $PLATFORM -eq 31 ];then
        echo 1 > $DVFS_DIR/enable
        res=`cat $DVFS_DIR/status | grep "enabled" | wc -l`
        if [ $res -eq 1 ];then
            tst_resm TPASS "dvfs is enabled"
        else
            tst_resm TFAIL "fail to enable dvfs"
            RC=1
			return $RC
        fi
        sleep 3 
        
        echo 0 > $DVFS_DIR/enable
        res=`cat $DVFS_DIR/status | grep "disabled" | wc -l`
        if [ $res -eq 1 ];then
            tst_resm TPASS "dvfs is disabled"
        else
            tst_resm TFAIL "fail to disable dvfs"
            RC=1
        fi
    fi
	
	#For imx35
	if [ $PLATFORM -eq 35 ];then
        echo 1 > $DVFS_DIR/enable
        res=`cat $DVFS_DIR/status | grep "enabled" | wc -l`
        if [ $res -eq 1 ];then
            tst_resm TPASS "dvfs is enabled"
        else
            tst_resm TFAIL "fail to enable dvfs"
            RC=1
			return $RC
        fi
        sleep 3 
        
        echo 0 > $DVFS_DIR/enable
        res=`cat $DVFS_DIR/status | grep "disabled" | wc -l`
        if [ $res -eq 1 ];then
            tst_resm TPASS "dvfs is disabled"
        else
            tst_resm TFAIL "fail to disable dvfs"
            RC=1
        fi
    fi
	# For imx37
	if [ $PLATFORM -eq 37 ];then
        echo 1 > $DVFS_DIR/enable
        res=`cat $DVFS_DIR/enable | grep "enabled" | wc -l`
        if [ $res -eq 1 ];then
            tst_resm TPASS "dvfs is enabled"
        else
            tst_resm TFAIL "fail to enable dvfs"
            RC=1
			return $RC
        fi
        sleep 3 
        
        echo 0 > $DVFS_DIR/enable
        res=`cat $DVFS_DIR/enable | grep "disabled" | wc -l`
        if [ $res -eq 1 ];then
            tst_resm TPASS "dvfs is disabled"
        else
            tst_resm TFAIL "fail to disable dvfs"
            RC=1
        fi
    fi
	# For imx51
	if [ $PLATFORM -eq 51 ];then
        echo 1 > $DVFS_DIR/enable
        res=`cat $DVFS_DIR/enable | grep "enabled" | wc -l`
        if [ $res -eq 1 ];then
            tst_resm TPASS "dvfs is enabled"
        else
            tst_resm TFAIL "fail to enable dvfs"
            RC=1
			return $RC
        fi
        sleep 3 
        
        echo 0 > $DVFS_DIR/enable
        res=`cat $DVFS_DIR/enable | grep "disabled" | wc -l`
        if [ $res -eq 1 ];then
            tst_resm TPASS "dvfs is disabled"
        else
            tst_resm TFAIL "fail to disable dvfs"
            RC=1
        fi
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

setup  || exit $RC
env_test || exit $RC
dvfs_test || exit $RC

