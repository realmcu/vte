#Copyright 2005-2009 Freescale Semiconductor, Inc. All Rights Reserved.
#
#The code contained herein is licensed under the GNU General Public
#License. You may obtain a copy of the GNU General Public License
#Version 2 or later at the following locations:
#
#http://www.opensource.org/licenses/gpl-license.html
#http://www.gnu.org/copyleft/gpl.html
#!/bin/ash
# File :        IrDA_FIR_receive.sh
#
# Description:  This is a test to fully validate an irattach process and an irdaping with FIR (Fast InfraRed)
#
#=====================================================================================
#Revision History:
#                       Modification	Tracking
# Author		        Date		Number		Description of Changes
#------------------	    ------------	----------	------------------------------
#V. Becker/rc023c	    31/08/2004	TLSbo40417	Initial version 
#V. Becker/rc023c	    27/09/2004	TLSbo40417	Inspection TLS941 
#V. Becker/rc023c       25/10/2004	TLSbo44073	Minor changes 
#V.HALABUDA/HLBV001     12/04/2005	TLSbo40417	control, mandate and limit IrDA stack
#Rakesh S Joshi/R65956  19/02/2007	TLSbo87888	Added irxfer

#Set path variable to add vte binaries
#export TESTCASES_HOME=/tmp/vte/testcases/bin
export PATH=${PATH}:${PWD}
tst_resm()
{
    echo $1 $2
}

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
    RV_1=0
    RV_2=0
    RV=0
    cp -f libopenobex.a /usr/lib/
    # Total number of test cases in this file. 
    export TST_TOTAL=1

    #The TCID and TST_COUNT variables are required by the LTP 
    #command line harness APIs, these variables are not local to this program.

	#Load IrDA module in the kernel
    modprobe irda
    RV_1=$?
    modprobe mxc_ir 2>/dev/null
    RV_2=$?

    cp -f libopenobex.a /usr/lib/
    # Test case identifier
    export TCID="IrDA_FIR_receive"
    # Set up is initialized as test 0
    export TST_COUNT=0

    # Initialize cleanup function to execute on program exit. 
    # This function will be called before the test program exits.
    ! [ $RV_1 -ne 0 -o  $RV_2 -ne 0 ]
    RV=$?

    if [ $RV -eq 0 ]
    then
        tst_resm TINFO "setup PASSED"
    else
        tst_resm FAIL "setup FAILED"
    fi

    RC=0
    return $RC
}

cleanup()
{
    RV_1=0
    RV_2=0
    RV_3=0
    RV=0

    echo
    tst_resm TINFO "Performing cleanup"
    tst_resm TINFO "Unloading irda modules..."
    rmmod mxc_ir
    RV_1=$?
    rmmod irda
    RV_2=$?
    rmmod crc_ccitt
    RV_3=$?

    ! [ $RV_1 -ne 0 -o  $RV_2 -ne 0 -o $RV_3 -ne 0 ]
    RV=$?

    if [ $RV -eq 0 ]
    then
        tst_resm TINFO "cleanup PASSED"
    else
        tst_resm FAIL "cleanup FAILED"
    fi

    return $RV

}

IrDA_FIR_BaudRate()
{
        for baudrate in 9200 19200 38400 57600 115200 4000000
        do
        # force the IrDA stack to negociate a different speed
            echo
            tst_resm TINFO "setting max_baud_rate to $baudrate"
            echo $baudrate > /proc/sys/net/irda/max_baud_rate
            CONFIG=`cat /proc/sys/net/irda/max_baud_rate`
            echo "$CONFIG"
            if [ $CONFIG -eq $baudrate ]
            then
                tst_resm TINFO "PASSED"
            else
                tst_resm TFAIL "FAILED"
                return 5
            fi
            IrDA_FIR_receive
            sleep 3
        done

        echo "Clean Up"
        read clean
        if [ $clean = 'y' ]
        then
            cleanup
        fi
        echo
        tst_resm TINFO "Detach successfull!"
}

IrDA_FIR_receive()
{
    TCID="IrDA_FIR_receive"
    TST_COUNT=1
    CONFIG=0
    i=0
    DEVICE_ADDRESS=``

 # limit Linux to send one frame per IrLAP window
    echo
    tst_resm TINFO "setting max_tx_window to 1"
    echo 1 > /proc/sys/net/irda/max_tx_window
    CONFIG=`cat /proc/sys/net/irda/max_tx_window`
    if [ $CONFIG -eq 1 ]
    then
        tst_resm TINFO "PASSED"
        echo
    else
        tst_resm TFAIL "FAILED"
        return 5
    fi

    ifconfig irda0 up
    echo 1 > /proc/sys/net/irda/discovery
    #Sleep for 5 second so that the device gets the address
    sleep 5
    #Display a list of discovered IrDA devices
    cmd=`grep daddr /proc/net/irda/discovery`
    i=1
    max=20
    while [ "x$cmd" = "x"  ] && [ $i -le $max ]; do
        tst_resm TINFO "Discovering [Try $i/$max] ..."
        i=`expr $i + 1`
        cmd=`grep daddr /proc/net/irda/discovery`
        sleep 2
    done

    if [ "x$cmd" = "x" ] || [ $i -gt $max ]; then
        tst_resm TFAIL "No IrDA device found!"
        return 5
    fi

    DEVICE_ADDRESS=`cat /proc/net/irda/discovery | sed -n '/daddr/p' | sed 's/^.*daddr: //' `
    tst_resm TINFO "Remote IrDA device address is: "$DEVICE_ADDRESS

    echo "Device is ready to receive files"
    irxfer
}

# Function:     main
# 
# Description:  - Execute all tests, exit with test status.
#               
# Exit:         - zero on success
#               - non-zero on failure.
#
# Return value from setup, and test functions.

setup

IrDA_FIR_BaudRate
