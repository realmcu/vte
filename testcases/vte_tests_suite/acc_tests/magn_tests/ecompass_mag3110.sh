#!/bin/sh
###############################################################################
# Copyright (C) 2011 Freescale Semiconductor, Inc. All Rights Reserved.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License along
# with this program; if not, write to the Free Software Foundation, Inc.,
# 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
##############################################################################
#
# Revision History:
#                      Modification     Tracking
# Author                   Date          Number    Description of Changes
#-------------------   ------------    ----------  ---------------------
# Spring Zhang          May.23,2011      n/a        Initial ver. 
############################################################################

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
    export TCID="TGE_LV_ECOMPASS"       # Test case identifier
    export TST_COUNT=0   # Setup is initialized as test 0
    BIN_DIR=`dirname $0`
    export PATH=$PATH:$BIN_DIR

    if [ -z $LTPTMP ]
    then
        LTPTMP=/tmp
    fi

    trap "cleanup" 0

    dmesg|grep "mag3110 is probed" || {
        tst_resm TFAIL "mag3110 doesn't probe, pls check..."
        RC=65
    }

    find_sys_dir || RC=$?

    return $RC
}

#Find mag3110 operation dir in i2c
find_sys_dir()
{
    RC=0
    BASE_DIR=/sys/class/i2c-dev/i2c-1/device/
    dirs=`ls -F $BASE_DIR |grep "/$"`
    for dir in $dirs; do
        if [ -e $BASE_DIR/${dir}/name ] && 
            [ "`cat $BASE_DIR/${dir}/name`" = "mag3110" ]; then
            break
        fi
    done
    if [ -e $BASE_DIR/${dir}/name -a "`cat $BASE_DIR/${dir}/name`" = "mag3110" ]; then
        SYS_DIR=$BASE_DIR/$dir
    else
        RC=66
    fi

    return $RC
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

    #disable mag3110
    echo 0 > ${SYS_DIR}/enable

    return $RC
}

# Function:     ecompass test
#
# Description:  catch raw data from MAG3110 magnetic sensor
#
# Exit:         zero on success
#               non-zero on failure.
#
ecompass_test()
{
    export TST_COUNT=1
    RC=0    # Return value from setup, and test functions.

    tst_resm TINFO "Magnetic ecompass function."
    tst_resm TINFO "Operation directory in i2c is: ${SYS_DIR}"
    #enable mag3110
    echo 1 > ${SYS_DIR}/enable

    #determine event testapp
    EV_TEST_APP=evtest
    which evtest || EV_TEST_APP=usb_ev_testapp
    tst_resm TINFO "Capture data from mag3110."

    #fix me: event handler could be dynamically determined.
    sh -c "$EV_TEST_APP /dev/input/event5 2>&1 | tee /tmp/mag3110.output" &
    sleep 2
    killall $EV_TEST_APP

    grep -i "mag3110" /tmp/mag3110.output || RC=69
    grep -i "code . (.), value" /tmp/mag3110.output > /dev/null || RC=70
    #disable for the dmesg log could be the one which remained before
    #if dmesg|tail -2 |grep "interrupt not received"; then
    #    RC=71
    #fi

    if [ $RC -eq 0 ]; then
        tst_resm TPASS "E-compass test pass"
    else
        tst_resm TFAIL "E-compass test failed"
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

    Use this command to test MAG3110 sensor ecompass functions.
    usage: ./${0##*/} [test item]
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
	"1")
    ecompass_test || exit $RC
    ;;
	*)
    usage
    exit 1
	;;
esac

