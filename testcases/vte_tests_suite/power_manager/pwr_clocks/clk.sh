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
# Andy Tian             09/04/2012  Add pcie clock check


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

get_clk_cnt()
{
  target=$1
  ct=0
  list=$(find ${mount_pt}/clock -name "${target}*")
  for i in $list; do
    if [ -e $i/usecount ]; then
        cnt=$(cat ${i}/usecount)
        ct=$(expr $cnt + $ct)
    fi
  done
  return $ct	
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

    #TODO add function test script here
    #disable the framebuffer

    #now check the clocks
    #esai_ct=$(cat ${mount_pt}/clock/osc_clk/pll3_usb_otg_main_clk/pll3_pfd_508M/esai_clk/usecount | grep -v 0)
    get_clk_cnt esai
    esai_ct=$?
    get_clk_cnt ssi
	ssi_ct=$?
	get_clk_cnt spdif
    spdif_ct=$?

    get_clk_cnt asrc
    asrc=$?
    if [ $esai_ct -gt 0 ] || [ $ssi_ct -gt 0 ] || [ $spdif_ct -gt 0 ] || [ $asrc -gt 0  ]; then
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
    get_clk_cnt can
    can=$?

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

    #TODO add function test script here
    #disable the framebuffer
    echo 1 > /sys/class/graphics/fb0/blank
    echo 1 > /sys/class/graphics/fb1/blank
    echo 1 > /sys/class/graphics/fb2/blank
    #for MX6Q
    echo 1 > /sys/class/graphics/fb3/blank
    echo 1 > /sys/class/graphics/fb4/blank

    sleep 1

    #now check the clocks
    get_clk_cnt ipu
	ipuct=$?
    # ldb usecount can't use the routine
    ldbct_list=$(find ${mount_pt}/clock/osc_clk/pll3_usb_otg_main_clk/pll3_pfd_540M/*/usecount | grep -v "axi_clk/usecount" | grep -v "pll3_pfd_540M/usecount")
    ldbct=$(cat $ldbct_list |grep -v 0)

    hdmi_cnt=0
    if ! cat /proc/cmdline |grep -i hdmi; then
        get_clk_cnt hdmi
        hdmi_cnt=$?
    fi

    echo 0 > /sys/class/graphics/fb0/blank
    echo 0 > /sys/class/graphics/fb1/blank
    echo 0 > /sys/class/graphics/fb2/blank
    #for MX6Q
    echo 0 > /sys/class/graphics/fb3/blank
    echo 0 > /sys/class/graphics/fb4/blank

    if [ $ipuct -gt 0 ] || [ -n "$ldbct" ] || [ $hdmi_cnt -gt 0 ]; then
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

    #TODO add function test script here
    #disable the framebuffer
    
	get_clk_cnt gpu
    gpu_cnt=$?
    
    #now check the clocks
    if [ $gpu_cnt -gt 0 ]; then
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

    #TODO add function test script here
    #disable the framebuffer

	get_clk_cnt i2c
    i2c_cnt=$?

    if [ $i2c_cnt -gt 0 ]; then
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

    #TODO add function test script here
    #disable the framebuffer

    #now check the clocks
	get_clk_cnt usdhc
    sdhc_ct=$?
    #cat /sys/kernel/debug/clock/osc_clk/pll3_usb_otg_main_clk/pll3_pfd_540M/usecount

    if [ $sdhc_ct -gt 0 ]; then
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

    #TODO add function test script here
    #disable the framebuffer

    get_clk_cnt uart
    uart_cnt=$?

    if [ $uart_cnt -gt 1 ]; then
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

    #TODO add function test script here
    #disable the framebuffer

    get_clk_cnt usb
    usb_cnt=$?

    if [ $usb_cnt -gt 0 ]; then
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

    #TODO add function test script here
    #disable the framebuffer

    #now check the clocks
    get_clk_cnt vpu
	vpu_cnt=$?

    if [ $vpu_cnt -gt 0 ]; then
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
    tst_resm TINFO "test $TST_COUNT: $TCID"

    ifconfig eth0 down || return $?

    echo "Ethernet turn off..."
    #TODO add function test script here
    #disable the framebuffer

    #now check the clocks
    get_clk_cnt enet
	enet_cnt=$?
    if [ $enet_cnt -gt 0 ]; then
        RC=10
    else
        RC=0
    fi

    echo "Ethernet turn on..."
    ifconfig eth0 up

    return $RC
}

# Function:     test_case_11
# Description   - Test pcie clk
#
test_case_11()
{
    #TODO give TCID
    TCID="PCIe clock test"
    #TODO give TST_COUNT
    TST_COUNT=11
    RC=11

    #print test info
    tst_resm TINFO "test $TST_COUNT: $TCID "

    #TODO add function test script here
    get_clk_cnt pcie
    pcie_cnt=$?

    if [ $pcie_cnt -gt 0 ]; then
        RC=11
    else
        RC=0
    fi

    return $RC
}

test_case_12()
{
    #TODO give TCID
    TCID="mipi clock test"
    #TODO give TST_COUNT
    TST_COUNT=11
    RC=11

    #print test info
    tst_resm TINFO "test $TST_COUNT: $TCID "

    #TODO add function test script here
    get_clk_cnt mipi_pllref_clk
    mipi_cnt=$?

    if [ $mipi_cnt -gt 0 ]; then
        RC=11
    else
        RC=0
    fi

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
	11: PCIe
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
11|"pcie")
    test_case_11 || check_result
    ;;
12|"mipi_pllref_clk")
    test_case_12 || check_result
    ;;
*)
    usage
    ;;
esac

tst_resm TINFO "Test PASS"
