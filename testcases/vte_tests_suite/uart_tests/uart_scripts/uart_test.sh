#!/bin/sh
#Copyright (C) 2008-2010,2012 Freescale Semiconductor, Inc. All Rights Reserved.
#
#The code contained herein is licensed under the GNU General Public
#License. You may obtain a copy of the GNU General Public License
#Version 2 or later at the following locations:
#
#http://www.opensource.org/licenses/gpl-license.html
#http://www.gnu.org/copyleft/gpl.html
################################################################################
#
#    @file   uart_test.sh
#
#    @brief  shell script template for testcase design "TODO" is where to modify
#    block.
#
################################################################################
#Revision History:
#
#Author                                     Date        Description of Changes
#------------------------------------   ------------    ------------------------
#<Hake Huang>/-----                      2008-11-27      Initial version
#Hake Huang                              2010-06-10      Initialize cmd sequence for case 0017
#Ahmann Philipp/<pahmann@de.adit-jv.com> 2012-02-27      Add script for case0017
#Spring Zhang                            2012-03-05      Modify into VTE
#
################################################################################

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
    export TST_TOTAL=2

    export TCID="setup"
    export TST_COUNT=0
    RC=0

    trap "cleanup" 0

    #TODO add setup scripts

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
# Description   - Test if stty baud rate ok
#
test_case_01()
{
    #TODO give TCID
    TCID="test_baud_rate"
    #TODO give TST_COUNT
    TST_COUNT=1
    RC=1

    #print test info
    tst_resm TINFO "test $TST_COUNT: $TCID "

    #TODO add function test scripte here
    BRATE="9600 19200 38400 57600 115200"

    for i in $BRATE
    do
        stty -F $DEVICE $i || exit $RC
    done

    RC=0

    return $RC
}

# Test description:
#
# this test will check the number of interrupts on an interface and the number
# of interrupts after some kind of data transfer
# this test was originally written for a UART interface of imx6Q with 4 CPUs
#
test_case_02()
{
    TCID="UART_INTERRUPT"
    TST_COUNT=2
    RC=1

    module=uart
    cores=0
    ir_tmp=0
    int_start=0
    int_end=0

    #get the total number of cores (0 is one core)
    cores=`cat /proc/cpuinfo | grep processor | awk '{print $3}'|wc -l`
    echo "Total cores: $cores"

    # check how many interupts are counted for the i/f to be checked
    ir_tmp=`cat /proc/interrupts | grep $module |
        awk '{ for ( j = 2; j <= '$cores+1'; j++)  print \$j }'`
        echo "interrupts array: <" $ir_tmp ">"

    if [ -z "$ir_tmp" ] ; then
        echo "$module was not found in /proc/interrupts"
        return $RC
    fi

    for i in $ir_tmp
    do
        int_start=`expr $int_start + $i`
    done
    echo "total interrupts before data transfer: $int_start\n"


    # send some data over the interface
    cat /etc/passwd > $DEVICE
    sleep 1

    # check again how many interupts are counted for the i/f to be checked
    ir_tmp=`cat /proc/interrupts | grep $module |
        awk '{ for ( j = 2; j <= '$cores+1'; j++)  print \$j }'`
        echo "interrupts array: <" $ir_tmp ">"
    for i in $ir_tmp
    do
        int_end=`expr $int_end + $i`
    done;
    echo "total interrupts after data transfer: $int_end \n"

    if [ $int_start -eq 0 -o  $int_end -eq 0 ] ; then
        echo "TEST FAILED, no interrupts found"
        return $RC
    fi

    if [ $int_start -lt $int_end ] ; then
        RC=0
        echo "TEST PASS"
    else
        echo "TEST FAILED"
        return $RC
    fi

    return $RC
}

usage()
{
    echo "usage $0 <serial device> <case ID>"
    exit 1
}

# main function

RC=0

#TODO check parameter
if [ $# -ne 2 ]
then
    usage $0
fi

DEVICE=$1

if [ ! -e $DEVICE ]
then
    RC=1
    echo device not exist!
    exit $RC
fi


setup || exit $RC

case "$2" in
1)
  test_case_01 || exit $RC
  ;;
2|"interrupt")
  test_case_02 || exit $RC
  ;;
*)
  usage $0
  ;;
esac

tst_resm TINFO "Test PASS"
