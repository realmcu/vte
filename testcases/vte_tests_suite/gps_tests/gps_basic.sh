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
#                      Modification     Tracking
# Author                   Date          Number    Description of Changes
#-------------------   ------------    ----------  ---------------------
# Spring Zhang          24/11/2008       n/a        Initial ver. 
# Spring                28/11/2008       n/a      Modify COPYRIGHT header
# Spring                16/01/2008       n/a      MX35 port change to /dev/ttymxc2
#############################################################################
# Portability:  ARM sh 
#
# File Name:     gps_basic.sh
# Total Tests:   1
# Test Strategy: Test basic GPS functions 
# 
# Input:	    Test type
#
# Return:       0: PASS, non-0: FAIL
#
# Command:      "./gps_basic.sh [Test Type]" 

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
    export TCID="TGE_LV_GPS"       # Test case identifier
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
    RC=0
    tst_resm TINFO "Clean GPS module..."
    #killall glgps_freescaleLinux
    rc.gps stop
    return $RC
}

# Function:     gps_smoke()   
#
# Description:  Test if GPS module function is OK 
#
# Exit:         zero on success
#               non-zero on failure.
#
gps_smoke()
{
    TCID="GPS_SMOKE"
    TST_COUNT=1
    RC=0    # Return value from setup, and test functions.

    # return 31,35,37,51 or 67(error)
    # for mx35 have changed to /dev/ttymxc2, so comment platfm identify
    #platfm.sh || platform=$?

    if [ $platform -eq 35 ]
    then
        sed 's/\/dev\/ttymxc2/\/dev\/ttymxc1/g' /usr/local/bin/glconfig.xml | \
         sed 's/cLogEnabled=\"false\"/cLogEnabled=\"true\"/g' > tmp
        mv tmp /usr/local/bin/glconfig.xml
    fi

    tst_resm TINFO "Test #1: Probe GPS module"

    rc.gps start || RC=$?
    if [ $RC -ne 0 ]
    then
        tst_resm TFAIL "Probe GPS module fail"
        return $RC
    fi

    sleep 3

    if [ ! -e /var/run/gpspipe ]
    then 
        tst_resm TFAIL "gps pipe not exists(/var/run/gpspipe)"
        RC=67
        return $RC
    fi

    tst_resm TINFO "Grab GPGGA msg in GPS log"
    cat /var/run/gpspipe > gpsinfo.txt &
    bgpid=$!
    sleep 5
    kill $bgpid
    
    grep -i GPGGA gpsinfo.txt || RC=$?
    if [ $RC -eq 0 ]
    then
        tst_resm TPASS "Test #1: GPS module smoke test success."
    else
        tst_resm TFAIL "Test #1: GPS module smoke test fail."
        RC=67
    fi

    return $RC
}

# Function:     gps_full()   
#
# Description:  Test if GPS can get latitude&longitude position
#
# Exit:         zero on success
#               non-zero on failure.
#
gps_full()
{
    TCID="GPS_FULL"
    TST_COUNT=2
    RC=0    # Return value from setup, and test functions.

    # return 31,35,37,51 or 67(error)
    # for mx35 have changed to /dev/ttymxc2, so comment platfm identify
    #platfm.sh || platform=$?

    if [ $platform -eq 35 ]
    then
        sed 's/\/dev\/ttymxc2/\/dev\/ttymxc1/g' /usr/local/bin/glconfig.xml | \
         sed 's/cLogEnabled=\"false\"/cLogEnabled=\"true\"/g' > tmp
        mv tmp /usr/local/bin/glconfig.xml
    fi

    tst_resm TINFO "Test #2: Probe GPS module"

    rc.gps start || RC=$?
    if [ $RC -ne 0 ]
    then
        tst_resm TFAIL "Test #2: Probe GPS module fail"
        return $RC
    fi
    
    sleep 3

    if [ ! -e /var/run/gpspipe ]
    then 
        tst_resm TFAIL "Test #2: gps pipe not exists(/var/run/gpspipe)"
        RC=67
        return $RC
    fi

    tst_resm TINFO "Test #2: Please pay attention to GPGGA msg in GPS log"
    tst_resm TINFO "Press Ctrl+C to quit the program if you find:"
    tst_resm TINFO "GL_NMEA[$GPGGA,055205.00,3116.807422,N,12031.820805,E,1,05,1.0,011.0,M,,M,,*79" 
    sleep 15
    cat /var/run/gpspipe &
    bgpid=$!

    sleep 600
    kill $bgpid

    tst_resm TINFO "Test #2"
    tst_resm TINFO "GL_NMEA[$GPGGA,055205.00,3116.807422,N,12031.820805,E,1,05,1.0,011.0,M,,M,,*79" 
    tst_resm TINFO "Do you see the message like the former one, esp. 3116.807422,N,12031.820805,E ?[y/n]"
    read answer
    if [ $answer = "y" ]
    then
        tst_resm TPASS "Test #2: GPS can get latitude and longitude."
    else
        tst_resm TFAIL "Test #2: GPS can't get latitude and longitude."
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

    Use this command to test GPS functions.
    usage: ./${0##*/} [Test Type] 
    Test Type: 1 -- GPS module smoke test, 2 -- Full GPS test
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
    gps_smoke "$@" || exit $RC
    ;;
2)
    gps_full "$@" || exit $RC
    ;;
*)
    usage
    exit 1
    ;;
esac



