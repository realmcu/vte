#!/bin/sh
##############################################################################
#Copyright (C) 2012 Freescale Semiconductor, Inc.
#All Rights Reserved.
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
# Author                   Date     Description of Changes
#-------------------   ------------ -----------------------
# Spring Zhang          15/03/2012  Initial ver., integrate old separate script
# Spring Zhang          15/03/2012  Add ethernet clock check


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
    #TODO Total test case
    export TST_TOTAL=10

    export TCID="setup"
    export TST_COUNT=0
    RC=0

    trap "cleanup" 0

    #TODO add setup scripts
    #find the dbugfs
    mountpt=$(mount | grep debugfs)
    if [ -z "$mountpt" ]; then
        mount -t debugfs nodev /sys/kernel/debug || return $?
        mount_pt=/sys/kernel/debug
    else
        mount_pt=$(echo $mountpt | awk '{print $3}')
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

    #TODO add cleanup code here
    return $RC
}

# Function:     test_case_01
# Description   - Aduio clock test
#
test_case_01()
{
    #TODO give TCID
    TCID="audio clock test"
    #TODO give TST_COUNT
    TST_COUNT=1
    RC=1

    #print test info
    tst_resm TINFO "test $TST_COUNT: $TCID "

    #TODO add function test scripte here
    #disable the framebuffer

    #now check the clocks
    esai_ct=$(cat ${mount_pt}/clock/osc_clk/pll3_usb_otg_main_clk/pll3_pfd_508M/esai_clk/usecount | grep -v 0)
    ssi_ct=$(cat ${mount_pt}/clock/osc_clk/pll3_usb_otg_main_clk/pll3_pfd_508M/ssi*_clk/usecount | grep -v 0)
    spdif_ct=$(cat ${mount_pt}/clock/osc_clk/pll3_usb_otg_main_clk/pll3_pfd_454M/spdif0_clk_0/usecount | grep -v 0)
    #cat /sys/kernel/debug/clock/osc_clk/pll3_usb_otg_main_clk/pll3_pfd_540M/usecount

    asrc_list=$(find ${mount_pt}  -name asrc*)
    asrc=0
    for i in $asrc_list
    do
        temp=$(cat ${i}/usecount)
        asrc=$(expr $temp + $asrc)
    done

    if [ -n "$esai_ct" ] || [ -n "$ssi_ct" ] || [ -n "$spdif_ct" ] || [ $asrc -gt 0  ]; then
        RC=1
    else
        RC=0
    fi

    return $RC
}

# Function:     test_case_02
# Description   - Test CAN clk
#
test_case_02()
{
    #TODO give TCID
    TCID="CAN clock test"
    #TODO give TST_COUNT
    TST_COUNT=2
    RC=2

    #print test info
    tst_resm TINFO "test $TST_COUNT: $TCID "

    #TODO add function test script here
    can_list=$(find ${mount_pt}  -name "can*")

    can=0
    for i in $can_list
    do
        temp=$(cat ${i}/usecount)
        can=$(expr $temp + $can)
    done


    if [ $can -gt 0 ]; then
        RC=2
    else
        RC=0
    fi

    return $RC
}

# Function:     test_case_03
# Description   - Test display clock
#
test_case_03()
{
    #TODO give TCID
    TCID="Display clock test"
    #TODO give TST_COUNT
    TST_COUNT=3
    RC=3

    #print test info
    tst_resm TINFO "test $TST_COUNT: $TCID "

    #TODO add function test scripte here
    #disable the framebuffer
    echo 1 > /sys/class/graphics/fb0/blank
    echo 1 > /sys/class/graphics/fb2/blank

    sleep 1

    #now check the clocks
    ipuct=$(cat /sys/kernel/debug/clock/osc_clk/pll5_video_main_clk/ipu*/usecount| grep -v 0)
    ldbct=$(cat /sys/kernel/debug/clock/osc_clk/pll3_usb_otg_main_clk/pll3_pfd_540M/*/usecount | grep -v 0)
    #cat /sys/kernel/debug/clock/osc_clk/pll3_usb_otg_main_clk/pll3_pfd_540M/usecount
    cd /sys/kernel/debug/
    hdmi_list=$(find . -name hdmi*)
    hdmi=0
    for i in $hdmi_list
    do
        temp=$(cat ${i}/usecount)
        hdmi=$(expr $temp + $hdmi)
    done

    echo 0 > /sys/class/graphics/fb0/blank
    echo 0 > /sys/class/graphics/fb2/blank

    if [ -n "$ipuct" ] || [ -n "$ldbct" ] || [ $hdmi -gt 0 ]; then
        RC=3
    else
        RC=0
    fi

    return $RC
}

# Function:     test_case_04
# Description   - Test GPU clock
#
test_case_04()
{
    #TODO give TCID
    TCID="gpu clock test"
    #TODO give TST_COUNT
    TST_COUNT=4
    RC=4

    #print test info
    tst_resm TINFO "test $TST_COUNT: $TCID "

    #TODO add function test scripte here
    #disable the framebuffer

    gpu=0
    #now check the clocks
    cd /sys/kernel/debug/
    gpu_list=$(find . -name "gpu*")
    for i in $gpu_list
    do
        temp=$(cat ${i}/usecount)
        gpu=$(expr $temp + $gpu)
    done

    if [ $gpu -gt 0 ]; then
        RC=4
    else
        RC=0
    fi

    return $RC
}

# Function:     test_case_05
# Description   - Test i2c clk ok
#
test_case_05()
{
    #TODO give TCID
    TCID="i2c/touch screen clock test"
    #TODO give TST_COUNT
    TST_COUNT=5
    RC=5

    #print test info
    tst_resm TINFO "test $TST_COUNT: $TCID "

    #TODO add function test scripte here
    #disable the framebuffer

    i2c_list=$(find ${mount_pt}  -name "i2c*")
    i2c=0
    for i in $i2c_list
    do
        temp=$(cat ${i}/usecount)
        i2c=$(expr $temp + $i2c)
    done


    if [ $i2c -gt 0 ]; then
        RC=5
    else
        RC=0
    fi

    return $RC
}


# Function:     test_case_06
# Description   - SD clock test
#
test_case_06()
{
    #TODO give TCID
    TCID="sd clock test"
    #TODO give TST_COUNT
    TST_COUNT=1
    RC=6

    #print test info
    tst_resm TINFO "test $TST_COUNT: $TCID "

    #TODO add function test scripte here
    #disable the framebuffer

    #now check the clocks
    sdhc_ct=$(cat /sys/kernel/debug/clock/osc_clk/pll2_528_bus_main_clk/pll2_pfd_400M/usdhc*/usecount | grep -v 0)
    #cat /sys/kernel/debug/clock/osc_clk/pll3_usb_otg_main_clk/pll3_pfd_540M/usecount

    if [ -n "$sdhc_ct" ]; then
        RC=6
    else
        RC=0
    fi

    return $RC
}

# Function:     test_case_07
# Description   - Test uart clk ok
#
test_case_07()
{
    #TODO give TCID
    TCID="uart clock test"
    #TODO give TST_COUNT
    TST_COUNT=7
    RC=7

    #print test info
    tst_resm TINFO "test $TST_COUNT: $TCID "

    #TODO add function test scripte here
    #disable the framebuffer

    uart_list=$(find ${mount_pt}  -name "uart*")
    uart=0
    for i in $uart_list
    do
        if [ -e "${i}/usecount"  ]; then
            temp=$(cat ${i}/usecount)
            uart=$(expr $temp + $uart)
        fi
    done

    if [ $uart -gt 1 ]; then
        RC=7
    else
        RC=0
    fi

    return $RC
}

# Function:     test_case_08
# Description   - Test usb clk ok
#
test_case_08()
{
    #TODO give TCID
    TCID="usb clock test"
    #TODO give TST_COUNT
    TST_COUNT=8
    RC=8

    #print test info
    tst_resm TINFO "test $TST_COUNT: $TCID "

    #TODO add function test scripte here
    #disable the framebuffer

    usb_list=$(find ${mount_pt}  -name "usb*")
    usb=0
    for i in $usb_list
    do
        if [ -e "${i}/usecount" ]; then
            temp=$(cat ${i}/usecount)
            usb=$(expr $temp + $usb)
        fi
    done


    if [ $usb -gt 0 ]; then
        RC=8
    else
        RC=0
    fi

    return $RC
}

# Function:     test_case_09
# Description   - Test vpu clock
#
test_case_09()
{
    #TODO give TCID
    TCID="vpu clock test"
    #TODO give TST_COUNT
    TST_COUNT=9
    RC=9

    #print test info
    tst_resm TINFO "test $TST_COUNT: $TCID "

    #TODO add function test scripte here
    #disable the framebuffer

    vpu=0
    #now check the clocks
    cd /sys/kernel/debug/
    vpu_list=$(find . -name "vpu*")
    for i in $vpu_list
    do
        temp=$(cat ${i}/usecount)
        vpu=$(expr $temp + $vpu)
    done

    if [ $vpu -gt 0 ]; then
        RC=9
    else
        RC=0
    fi

    return $RC
}

# Function:     test_case_10
# Description   - Test ethernet clock gate
#
test_case_10()
{
    #TODO give TCID
    TCID="Ethernet clock test"
    #TODO give TST_COUNT
    TST_COUNT=10
    RC=10

    #print test info
    tst_resm TINFO "test $TST_COUNT: $TCID "

    read -p "The Ethernet is going to turn off, are you sure to continue?[y/n]" answer

    if [ "$answer" != "y" ]; then
        RC=1
        echo "The case doesn't run due to a negtive answer"
        return $RC
    fi

    ifconfig eth0 down || return $?

    #TODO add function test scripte here
    #disable the framebuffer

    enet=0
    #now check the clocks
    cd /sys/kernel/debug/
    enet_list=$(find . -name "enet*")
    for i in $enet_list
    do
        temp=$(cat ${i}/usecount)
        enet=$(expr $temp + $enet)
    done

    if [ $enet -gt 0 ]; then
        RC=10
    else
        RC=0
    fi

    echo "Ethernet turn on..."
    ifconfig eth0 up

    return $RC
}

usage()
{
    cat <<-EOF

    Clock gate test for different modules
    $0 [case ID]
    1: Audio
    2: CAN
    3: Display
    4: GPU
    5: I2C
    6: SD
    7: UART
    8: USB
    9: VPU
    10: Ethernet
EOF

    exit 1
}

#check the result
check_result()
{
    if [ $RC -ne 0 ]; then
        echo "TINFO Test FAIL"
        exit $RC
    fi
}

# main function

RC=0

#TODO check parameter
if [ $# -ne 1 ]
then
    usage
fi

setup || exit $RC

case "$1" in
1|"audio")
    test_case_01 || check_result
    ;;
2|"can")
    test_case_02 || check_result
    ;;
3|"display")
    test_case_03 || check_result
    ;;
4|"gpu")
    test_case_04 || check_result
    ;;
5|"i2c")
    test_case_05 || check_result
    ;;
6|"sd")
    test_case_06 || check_result
    ;;
7|"uart")
    test_case_07 || check_result
    ;;
8|"usb")
    test_case_08 || check_result
    ;;
9|"vpu")
    test_case_09 || check_result
    ;;
10|"ethernet")
    test_case_10 || check_result
    ;;
*)
    usage
    ;;
esac

tst_resm TINFO "Test PASS"
