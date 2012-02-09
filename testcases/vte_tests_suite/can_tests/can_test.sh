#!/bin/bash
#Copyright (C) 2008,2010-2011 Freescale Semiconductor, Inc. All Rights Reserved.
#
#The code contained herein is licensed under the GNU General Public
#License. You may obtain a copy of the GNU General Public License
#Version 2 or later at the following locations:
#
#http://www.opensource.org/licenses/gpl-license.html
#http://www.gnu.org/copyleft/gpl.html
###################################################################################################
#
#    @file   can_test.sh
#
#    @brief  shell script to test can function block.
#
###################################################################################################
#Revision History:
#                            Modification     Tracking
#Author                          Date          Number    Description of Changes
#-------------------------   ------------    ----------  -------------------------------------------
#Hake.Huang/-----             08/06/2008     N/A          Initial version
#Spring.Zhang/---             11/08/2010     N/A          Fix MX53 built-in issue
#Spring.Zhang/---             11/22/2011     N/A          Add support for MX6Q
# 
###################################################################################################
#
# Notes:
# 1. The loopback and own message feature not support. and therefore not test.
# 2. BCM package is utalized in socketCAN not in driver therefore not test.
# 3. filter function is utalized in socketCAN not in driver therefore not test.
#

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
    export TST_TOTAL=1

    export TCID="setup"
    export TST_COUNT=0
    RC=0

    if [ -e $LTPROOT ]
    then
    export LTPSET=0
    else
    export LTPSET=1
    fi

    trap "cleanup" 0

    platfm.sh || platfm=$?
    if [ $platfm -ne 53 ] && [ $platfm -ne 61 ] && [ $platfm -ne 63 ]; then
            modprobe flexcan
    fi

    ip link set $CANID up type can bitrate 125000

    ifconfig $CANID up

    if [ $? -ne 0 ]
    then
    tst_resm TINFO "can init failed!"
    RC=2
    return $RC
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
    ifconfig $CANID down
    sleep 1
    if [ $platfm -ne 53 ] && [ $platfm -ne 61 ] && [ $platfm -ne 63 ]; then
            modprobe flexcan -r
        fi
    fi

    return $RC
}

#
# Function:     test_can_01
# Description   - Test check the CAN basic send and receive
# auto-manual test
test_can_01()
{
    RC=0
    TCID="test_can_01"
    TST_COUNT=1

    echo please check the CAN cable 

    read -p "Is the can cable ok? y/n" Rnt

    echo $Rnt

    if [ "$Rnt" = "n" ]
    then
    RC=$TST_COUNT
    return $RC
    fi

    read -p "IS the receiver deamon runs? y/n" Rnt

    if [ $Rnt = "n" ]
    then
    RC=$TST_COUNT
    return $RC
    fi

    echo Now start test!

    cansend $CANID 123#11
    sleep 1
    cansend $CANID 123#1122
    sleep 1
    cansend $CANID 123#112233
    sleep 1
    cansend $CANID 123#11223344
    sleep 1
    cansend $CANID 123#1122334455
    sleep 1
    cansend $CANID 123#112233445566
    sleep 1
    cansend $CANID 123#11223344556677
    sleep 1
    cansend $CANID 123#1122334455667788
    sleep 1
    cansend $CANID 1F334455#11
    sleep 1
    cansend $CANID 1F334455#1122
    sleep 1
    cansend $CANID 1F334455#112233
    sleep 1
    cansend $CANID 1F334455#11223344
    sleep 1
    cansend $CANID 1F334455#1122334455
    sleep 1
    cansend $CANID 1F334455#112233445566
    sleep 1
    cansend $CANID 1F334455#11223344556677
    sleep 1
    cansend $CANID 1F334455#1122334455667788
    sleep 1

    echo please check the data in receiver

    read -p "is the data match? y/n" Rnt

    if [ "$Rnt" = "y" ]
    then
    return $RC
    fi

    RC=$TST_COUNT
    return $RC
}

#
# the blk_time is the time interval
#
subtest_02()
{
  while true;
  do
  cansend $CANID 123#1122334455667788
  sleep 1
  done

}

#
# Function:     test_can_02
# Description   - Test check the can echo back data stress test
# catalog: auto-manual test
test_can_02()
{
    RC=0
    TCID="test_can_02"
    TST_COUNT=2

    subtest_02 &

    read -p "q to quit the test" Rnt

    while [ "$Rnt" != "q" ]
    do
    read -p "q to quit the test" Rnt
    done

    kill -9 $!

    sleep 1

    read -p "the data still recived OK? y/n" Rnt

    if [ "$Rnt" = "y" ]
    then
    RC=0
    return $RC
    fi

    RC=$TST_COUNT
    return $RC
}

#
# Function: test_can-03
# Description: -test the loop back test used to test the can socket
# catalog: auto-manual test
# 
test_can_03()
{
    RC=0
    TCID="test_can_03"
    TST_COUNT=3

    #Note: CAN_RAW_LOOPBACK / CAN_RAW_RECV_OWN_MSGS not support so far
    #
    #tst-raw -i $CANID -l 1 -r 1 &

    tst-raw -i $CANID &

    sleep 1

    cansend $CANID 123#1122334455667788

    read -p "whether the loop back data right? y/n" Rnt

    kill -9 $!

    if [ "$Rnt" = "y" ]
    then
    return $RC
    fi

    RC=$TST_COUNT
    return $RC
}

#
# Function: test_can_04
# Description: -test the sys file system test (driver test)
# catalog: auto test
#
test_can_04()
{
    RC=0
    TCID="test_can_04"
    TST_COUNT=4

    CANBUS=$(echo $CANID | sed 's/can/./')

    echo "this test should run when CAN bus is not busy!"
    #not test this registers reserver for future test
    REGISTERS_RW="br_presdiv br_rjw br_propseg br_pseg1 br_pseg2 xmit_maxmb maxmb"

    REGISTERS_RW_BIT="abort bcc boff_rec fifo listen local_priority \
                    loopback smp srx_dis ext_msg std_msg tsyn wak_src wakeup"

    #not test for read only register                 
    REGISTER_RO="bitrate state"

    ifconfig $CANID down

    bk=$(cat /sys/devices/platform/FlexCAN$CANBUS/br_clksrc)
    echo "bus" > /sys/devices/platform/FlexCAN$CANBUS/br_clksrc
    Rnt=$(cat /sys/devices/platform/FlexCAN$CANBUS/br_clksrc)
    if [ "$Rnt" != "bus" ]
    then
    RC=$TST_COUNT
    return $RC
    fi
    echo "osc" > /sys/devices/platform/FlexCAN$CANBUS/br_clksrc
    Rnt=$(cat /sys/devices/platform/FlexCAN$CANBUS/br_clksrc)
    if [ "$Rnt" != "osc" ]
    then
    RC=$TST_COUNT
    return $RC
    fi
    echo $bk > /sys/devices/platform/FlexCAN$CANBUS/br_clksrc


    for i in $REGISTERS_RW_BIT
    do
    bk=$(cat /sys/devices/platform/FlexCAN$CANBUS/$i)
    echo 1 >  /sys/devices/platform/FlexCAN$CANBUS/$i
    Rnt=$(cat /sys/devices/platform/FlexCAN$CANBUS/$i) 

    if [ $Rnt -ne 1 ]
    then
    RC=$TST_COUNT
    return $RC
    fi

    echo 0 >  /sys/devices/platform/FlexCAN$CANBUS/$i
    Rnt=$(cat /sys/devices/platform/FlexCAN$CANBUS/$i) 
    if [ $Rnt -ne 0 ]
    then
    RC=$TST_COUNT
    return $RC
    fi
    done

    return $RC
}

#
# Function: test_can_05
# Description: -power saving test case
# catalog: manual test
#
test_can_05()
{
    RC=0
    TCID="test_can_05"
    TST_COUNT=5

    echo "****************************"
    echo "****************************"
    echo -e "press power key to recover"
    echo "****************************"
    echo "****************************"

    echo standby > /sys/power/state

    read -p "Can you see me?y/n" Rnt

    if [ "$Rnt" = "n" ]
    then
    RC=$TST_COUNT
    return $RC
    fi

    read -p "Is the can cable ok? y/n" Rnt

    if [ "$Rnt" = "n" ]
    then
    RC=$TST_COUNT
    return $RC
    fi

    read -p "Is the receiver deamon runs? y/n" Rnt

    if [ "$Rnt" = "n" ]
    then
    RC=$TST_COUNT
    return $RC
    fi

    echo Now start test!

    cansend $CANID 123#1122334455667788

    echo please check the data in receiver

    read -p "is the data match? y/n" Rnt

    if [ "$Rnt" = "y" ]
    then
    return $RC
    fi

    RC=$TST_COUNT
    return $RC
}

#
# Function: test_can_06
# Description: basic module available test
# catalog: auto test
#
test_can_06()
{
    RC=0
    TCID="test_can_module"
    TST_COUNT=6

    return $RC
    }

    #
    # Function: test_can_07
    # Description: basic module available test
    # catalog: auto test
    #
    test_can_07()
    {
    RC=0
    TCID="test_can_bitrate"
    TST_COUNT=7
    #ref 
    #http://www.softing.com/home/en/industrial-automation/products/can-bus/more-can-bus/high-speed/cia-ds-102-baudrates.php
    STAND_BIT_RATES="1000000 800000 500000 250000 125000 50000 20000 10000"

    read -p "please connect the boards two can buses: then press any key"

    for i in $STAND_BIT_RATES
    do
    ifconfig can0 down
    ifconfig can1 down
    sleep 2
    echo $i > /sys/devices/platform/FlexCAN.0/bitrate
    cat /sys/devices/platform/FlexCAN.0/bitrate | grep $i || RC=1

    echo $i > /sys/devices/platform/FlexCAN.1/bitrate
    cat /sys/devices/platform/FlexCAN.1/bitrate | grep $i || RC=1

    ifconfig can0 up
    ifconfig can1 up

    canecho can1 -v &
    bgpid=$!

    cansend can0 123#1122334455667788 || RC=2

    if [ $RC -ne 0 ]; then
    RC="$RC $i"
    fi

    kill -9 $bgpid >/dev/null 2>&1

    done
    return $RC
}

#
# Function: test_can_08
# Description: can filter test
# catalog: maunal test
#
test_can_08()
{
    RC=0
    TCID="test_can_filter"
    TST_COUNT=8
    #ref

    if [ ! -e /etc/modprobe.d/vcan ]; then
        mkdir /etc/modprobe.d
        cat <<-EOF > /etc/modprobe.d/vcan
        # protocol family PF_CAN
        alias net-pf-29 can
        # protocols in PF_CAN
        alias can-proto-1 can-raw
        alias can-proto-2 can-bcm
        alias can-proto-3 can-tp16
        alias can-proto-4 can-tp20
        alias can-proto-5 can-mcnet
        alias can-proto-6 can-isotp
EOF
    fi

    ${LTPROOT}/testcases/bin/ip link add dev vcan0 type vcan
    ifconfig vcan0 up
    tst-filter-server > output_ltp-can.txt &
    sleep 1
    tst-filter-master | tee output_ltp-can-verify.txt
    diff output_ltp-can.txt output_ltp-can-verify.txt || RC=1
    ${LTPROOT}/testcases/bin/ip link del dev vcan0 type vcan
    return $RC
}



usage()
{
    echo "can_test.sh [TEST ID]"
    echo "1: data transfer test"
    echo "2: data transfer and receiving test"
    echo "3: loop back test"
    echo "4: CAN register setting test"
    echo "5: power management test"
    echo "6: module available test"
    echo "7: bit rate test"
    echo "8: vcan filter test"
}

# main function
export TST_TOTAL=8

RC=0

if [ $# -ne 2 ]
then
    usage
    exit 1 
fi

CANID=$1

setup || exit $RC

case "$2" in
1)
    test_can_01 || exit $RC 
    ;;
2)
    test_can_02 || exit $RC
    ;;
3)
    test_can_03 || exit $RC
    ;;
4)
    test_can_04 || exit $RC
    ;;
5)
    test_can_05 || exit $RC
    ;;
6)
    test_can_06 || exit $RC
    ;;
7)
    test_can_07 || exit $RC
    ;;
8)
    test_can_08 || exit $RC
    ;;
*)
    usage
    ;;
esac

tst_resm TPASS "test PASS"

