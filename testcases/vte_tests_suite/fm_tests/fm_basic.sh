#Copyright 2005-2009 Freescale Semiconductor, Inc. All Rights Reserved.
#
#The code contained herein is licensed under the GNU General Public
#License. You may obtain a copy of the GNU General Public License
#Version 2 or later at the following locations:
#
#http://www.opensource.org/licenses/gpl-license.html
#http://www.gnu.org/copyleft/gpl.html
#!/bin/sh
###############################################################################
#
#    @file   fm_basic.sh
#
#    @brief  Script to test FM.
#
##############################################################################
#
# Revision History:
#                      Modification     Tracking
# Author                   Date          Number    Description of Changes
#-------------------   ------------    ----------  ---------------------
# Spring Zhang          11/12/2008     ENGR102289      Initial ver. 
# Spring Zhang          11/3/2009          n/a      35/51 switch to sgtl5000
#############################################################################
# Portability:  ARM sh 
#
# File Name:     fm_basic.sh
# Total Tests:   2
# Test Strategy: Test basic FM functions 
# 
# Input:	    Test type
#
# Return:       0: PASS, non-0: FAIL
#
# Command:      "./fm_basic.sh [Test Type]" 

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

    export TST_TOTAL=2   # Total number of test cases in this file.
    LTPTMP=${TMP}        # Temporary directory to create files, etc.
    export TCID="TGE_LV_FM"       # Test case identifier
    export TST_COUNT=0   # Set up is initialized as test 0
    BIN_DIR=`dirname $0`
    export PATH=$PATH:$BIN_DIR

    if [ -z $LTPTMP ]
    then
        LTPTMP=/tmp
    fi

    if [ $# -lt 1 ]
    then 
        usage
        exit 1
    fi

    platform=`platfm.sh`
    if [ "$platform" == "" ]
    then
        echo "unrecognized platform"
        exit 1
    fi

    trap "cleanup" 0
}

# Function:     cleanup
#
# Description   - remove temporary files and directories.
#
# Return        - zero on success
#               - non zero on failure. return value from commands ($RC)
cleanup() 
{
    echo "cleanup"
    echo halt > $FM_CTL

    if [ $platform == "IMX35_3STACK" ] || [ $platform == "IMX51_3STACK" ]
    then
        amixer cset name='DAC Mux' 0  > /dev/null
    else
        amixer cset name='Loopback Line-in' 0 > /dev/null
    fi

    sleep 1
    modprobe mxc_si4702 -r
}

# Function:     fm_probe()   
#
# Description:  Test if FM module function is OK 
#
# Exit:         zero on success
#               non-zero on failure.
#
fm_probe()
{
    TCID="FM_PROBE"
    TST_COUNT=1
    RC=0    # Return value from setup, and test functions.

    tst_resm TINFO "Test #1: Probe FM module"

    modprobe mxc_si4702 || RC=$?
    [ $RC -ne 0 ] && {
        tst_resm TFAIL "Probe FM module fail"
        return $RC
    }

    FM_CTL=`find /sys/class/i2c-adapter -name si4702_ctl`
    [ -e $FM_CTL ] || {
        tst_resm TFAIL "FM i2c control not exist"
        $RC=67
        return $RC 
    }

    if [ $platform == "IMX35_3STACK" ] || [ $platform == "IMX51_3STACK" ]
    then
        amixer cset name='DAC Mux' 1 ||RC=$?
    else
        amixer cset name='Loopback Line-in' 1 ||RC=$?
    fi
    [ $RC -ne 0 ] && {
        tst_resm TFAIL "Probe FM Loopback Line-in fail"
        return $RC
    }

    #Test 'reset' cmd
    echo reset > $FM_CTL || RC=$?
    [ $RC -ne 0 ] && {
        tst_resm TFAIL "FM Reset fail"
        return $RC
    }

    #Test 'start' cmd
    echo start > $FM_CTL || RC=$?
    [ $RC -ne 0 ] && {
        tst_resm TFAIL "FM Start fail"
        return $RC
    }

    if [ $RC -eq 0 ]
    then
        tst_resm TPASS "Test #1: FM module probe success."
    else
        tst_resm TFAIL "Test #1: FM module probe fail."
    fi

    return $RC
}

# Function:     fm_search()   
#
# Description:  Test if FM can get station
#
# Exit:         zero on success
#               non-zero on failure.
#
fm_search()
{
    TCID="FM_SEARCH"
    TST_COUNT=2
    RC=0    # Return value from setup, and test functions.

    tst_resm TINFO "Test #2: Probe FM module"

    fm_probe || RC=$?
    if [ $RC -ne 0 ]
    then
        tst_resm TFAIL "Test #2: Probe FM module fail"
        return $RC
    fi

    #Test 'seek' cmd, try 4 times.
    echo seek > $FM_CTL || RC=$?
    [ $RC -ne 0 ] && {
        tst_resm TFAIL "FM seek fail"
        return $RC
    }
    tst_resm TINFO "Wait......10 secs"
    sleep 10
    echo seek > $FM_CTL || RC=$?
    [ $RC -ne 0 ] && {
        tst_resm TFAIL "FM seek fail"
        return $RC
    }
    tst_resm TINFO "Wait......10 secs"
    sleep 10
    echo seeku > $FM_CTL || RC=$?
    [ $RC -ne 0 ] && {
        tst_resm TFAIL "FM seeku fail"
        return $RC
    }
    tst_resm TINFO "Wait......10 secs"
    sleep 10
    echo seekd > $FM_CTL || RC=$?
    [ $RC -ne 0 ] && {
        tst_resm TFAIL "FM seekd fail"
        return $RC
    }

    tst_resm TINFO "Wait......10 secs"
    sleep 10
    
    tst_resm TINFO "Do you hear one FM station voice clear?[y/n]"
    read answer
    if [ $answer = "y" ]
    then
        tst_resm TPASS "Test #2: FM search function test success."
    else
        tst_resm TFAIL "Test #2: FM search function test fail."
        RC=67
    fi

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

    Use this command to test FM functions.
    usage: ./${0##*/} [Test Type] 
    Test Type: 1 -- FM module probe test
               2 -- FM station search test
    e.g.: ./${0##*/} 1

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

case $1 in 
1)
    fm_probe "$@" || exit $RC
    ;;
2)
    fm_search "$@" || exit $RC
    ;;
*)
    usage
    exit 1
    ;;
esac

