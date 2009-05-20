#!/bin/ash
# File :        IrDA_SIR_send.sh
#
# Description:  This is a test to fully validate an irattach process and an irdaping with SIR
#
#======================================================================
#
#                             Freescale SemiconductorConfidential Proprietary
#                   (c) Copyright 2004, Freescale Semiconductor, Inc.  All rights reserved.
#
#Presence of a copyright notice is not an acknowledgement of publication.
#This software file listing contains information of Freescale Semiconductor, Inc. that is of a confidential and
#proprietary nature and any viewing or use of this file is prohibited without specific written
#permission from Freescale Semiconductor, Inc.

#=====================================================================================
#Revision History:
#                       Modification Tracking
# Author  Date  Number  Description of Changes
#------------------ ------------ ---------- ------------------------------
#V. Becker/rc023c 29/08/2004 TLSbo40417 Initial version
#V. Becker/rc023c 27/09/2004 TLSbo40417 Inspection TLS941
#V. Becker/rc023c 25/10/2004 TLSbo44073 Minor changes
#V.HALABUDA/HLBV001 12/04/2005 TLSbo40417 control, mandate and limit IrDA stack

#Set path variable to add vte binaries
#export TESTCASES_HOME=/tmp/vte/testcases/bin
#export PATH=${PATH}:${PWD}
#set -x
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
  # Total number of test cases in this file
   export TST_TOTAL=1

   #The TCID and TST_COUNT variables are required by the LTP
   #command line harness APIs, these variables are not local to this program.

    # Test case identifier
    export TCID="IrDA_SIR_send"
    # Set up is initialized as test 0
    export TST_COUNT=0

    # Initialize cleanup function to execute on program exit.
    # This function will be called before the test program exits.
    trap "cleanup" 0
    #trap "cleanup" SIGQUIT
    # We call cleanup here to be sure to have a clean env.
    cleanup


    RC=0
    return $RC
}

cleanup()
{
    tst_resm TINFO "Cleaning up IrDA..."
    #Kill all current irattach processes
    killall irattach 2> /dev/null
    ifconfig irda0 down 2>/dev/null
    sleep 1
    #Unload IrDa stack module and dependencies
    rmmod irtty_sir 2> /dev/null || :
    rmmod sir_dev 2> /dev/null || :
    rmmod ircomm_tty 2> /dev/null || :
    rmmod ircomm 2> /dev/null || :
    rmmod irlan 2> /dev/null || :
    rmmod irport 2> /dev/null || :
    rmmod mxc_fir 2> /dev/null || :
    rmmod irda 2> /dev/null || :
    rmmod crc_ccitt 2> /dev/null || :
    # wait all children
    tst_resm TINFO "Waiting children(s)..."
}

IrDA_SIR_send()
{
    TCID="IrDA_SIR_send"
    TST_COUNT=1
    RC=0
    i=0
    DEVICE_ADDRESS=""
    UART_NUMBER="$1"

    if [ -z "$1" ];  then
 tst_resm TINFO "Usage : give MXC UART number used for IrDA SIR transmission"
 tst_resm TINFO "Example : ./IrDA_SIR_send.sh 2   for UART 2"
 RC=1
 return $RC
    fi

    #Load IrDA module in the kernel
    tst_resm TINFO "Loading modules..."
    modprobe irtty-sir 2> /dev/null

    # Create the IrDA devices :
    tst_resm TINFO "Creating devices (if needed) ..."
    [ -c /dev/ircomm0 ] || mknod /dev/ircomm0 c 161 0
    [ -c /dev/ircomm1 ] || mknod /dev/ircomm1 c 161 1
    [ -c /dev/irlpt0 ] || mknod /dev/irlpt0 c 161 16
    [ -c /dev/irlpt1 ] || mknod /dev/irlpt1 c 161 17
    [ -c /dev/irnet ] || mknod /dev/irnet c 10 187
    chmod 666 /dev/ir*

    #Attach the IrDA stack to the MXC UART2 and force discovery of IrDA devices
    uartdev=$1
    tst_resm TINFO "Attaching to device $uartdev ..."
    irattach $uartdev -s || RC=$?
    if [ $RC -ne 0 ];
    then
 tst_resm TFAIL "irattach failed"
 return $RC
    fi

    #Get remote IrDA device address
    cmd="grep daddr /proc/net/irda/discovery | sed -e 's/^.*nickname: \([^,]*\),.*daddr: \(.*\)$/\1 (\2)/'"
    DEVICE=""
    i=1
    max=10
    tst_resm TINFO "Discovering..."
    while [ "x$DEVICE" = "x"  ]; do
 echo -n "."
 DEVICE=`eval $cmd`
 sleep 1
    done

    tst_resm TPASS "Discover device \"$DEVICE\"."

    # Bring up the IrDA interface
    tst_resm TINFO "Bringing up irda0 interface ..."
    #lsmod || :
    #modprobe ircomm-tty 2> /dev/null
    insmod ircomm.ko
    insmod ircomm-tty.ko
    #lsmod || :
    ifconfig irda0 up

    tst_resm TINFO "Press <ENTER> to start transmission."
    read tmp

    recv=/tmp/`basename $0`.recv
    send=/tmp/`basename $0`.send

    # We need to launch the reader before
    tst_resm TINFO "Launching reader process..."
    dd if=/dev/ircomm0 bs=1 count=10 > $recv &
    pid=$!

    tst_resm TINFO "Sending data..."
    echo 1234567890 > $send
    cat $send > /dev/ircomm0

    timeout=15
    tst_resm TINFO "Waiting for reader process (timeout ${timeout}s)..."
    # on timeout kill the receiving process
    sleep $timeout && kill $pid 2>/dev/null || : &
    # now wait for it
    wait $pid
    RC=$?
    if [ $RC -eq 127 ] || [ $RC -eq 0 ];
    then
        # 127 is returned by wait when $pid doesn't
        # identify an existing process
        # 0 mean wait successfull and exit with 0
 tst_resm TINFO "Receive successfull"
    else
        # Exit status of the process
 tst_resm TFAIL "Receive data failed: status is $RC"
 return $RC
    fi

    tst_resm TINFO "Comparing data..."
    RC=0
    cmp $send $recv || RC=$?
    if [ $RC -ne 0 ];
    then
 tst_resm TFAIL "Data are not the same"
 return $RC
    fi

    #Set test to PASS
    tst_resm TPASS "Send, receive and compare OK"
    return 0
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

# Now setup env.
setup  || exit $RC

# and launch test
IrDA_SIR_send $1 || exit $RC
