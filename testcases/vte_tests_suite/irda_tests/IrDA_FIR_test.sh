#Copyright 2005-2009 Freescale Semiconductor, Inc. All Rights Reserved.
#
#The code contained herein is licensed under the GNU General Public
#License. You may obtain a copy of the GNU General Public License
#Version 2 or later at the following locations:
#
#http://www.opensource.org/licenses/gpl-license.html
#http://www.gnu.org/copyleft/gpl.html   
#=====================================================================================
#Revision History:
#                       Modification       Tracking
# Author                Date                Number                Description of Changes
#------------------     ------------       ----------        ------------------------------
#V. Becker/rc023c        31/08/2004        TLSbo40417        Initial version 
#V. Becker/rc023c        27/09/2004        TLSbo40417        Inspection TLS941 
#V. Becker/rc023c        25/10/2004        TLSbo44073        Minor changes 
#V.HALABUDA/HLBV001      12/04/2005        TLSbo40417        control, mandate and limit IrDA stack
#A.Ozerov/b00320         20/10/2006        TLSbo76160        irattach was removed
#Rakesh S Joshi/R65956   01/03/2007        TLSbo87888        ifconfig irda0 up is added  

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
#               - non zero on failure. return value from commands ($RV)
setup()
{
        RV_1=0
        RV_2=0
        RV=0

        # Total number of test cases in this file. 
        export TST_TOTAL=1

        # The TCID and TST_COUNT variables are required by the LTP 
        # command line harness APIs, these variables are not local to this program.

        tst_resm TINFO "Performing setup"
        # Load IrDA module in the kernel
        tst_resm TINFO "Loading irda modules..."
        modprobe irda
        RV_1=$?
        modprobe mxc_ir 2>/dev/null
        RV_2=$?

        # Test case identifier
        export TCID="IrDA_FIR_test"
        # Set up is initialized as test 0
        export TST_COUNT=0

        ! [ $RV_1 -ne 0 -o  $RV_2 -ne 0 -o $RV_3 -ne 0 -o $RV_4 -ne 0 ]
        RV=$?

        if [ $RV -eq 0 ]
        then
            tst_resm TINFO "setup PASSED"
        else
            tst_resm FAIL "setup FAILED"
        fi

        return $RV
}

cleanup()
{
        RV_3=0
        RV_4=0
        RV_5=0
        RV=0

        echo
        tst_resm TINFO "Performing cleanup"
        tst_resm TINFO "Unloading irda modules..."
        rmmod mxc_ir
        RV_3=$?
        rmmod irda
        RV_4=$?
        rmmod crc_ccitt
        RV_5=$?

        ! [ $RV_1 -ne 0 -o  $RV_2 -ne 0 -o $RV_3 -ne 0 -o $RV_4 -ne 0 -o $RV_5 -ne 0 ]
        RV=$? 

        if [ $RV -eq 0 ]
        then
            tst_resm TINFO "cleanup PASSED"
        else
            tst_resm FAIL "cleanup FAILED"
        fi

        return $RV
}

IrDA_FIR_test()
{
        TCID="IrDA_FIR_test"
        TST_COUNT=1
        CONFIG=0
        i=0
        DEVICE_ADDRESS=``

        # Configure network interface for IrDA

        # force the IrDA stack to negociate a lower speed
        echo
        tst_resm TINFO "setting max_baud_rate to 115200"
        echo 115200 > /proc/sys/net/irda/max_baud_rate
        CONFIG=`cat /proc/sys/net/irda/max_baud_rate`
        if [ $CONFIG -eq 115200 ]
        then
            tst_resm TINFO "PASSED"
        else
            tst_resm TFAIL "FAILED"
            return 5
        fi

        # force the IrDA stack to transmit smaller packets
        echo
        tst_resm TINFO "setting max_tx_data_size to 2000"
        echo 2000 > /proc/sys/net/irda/max_tx_data_size
        CONFIG=`cat /proc/sys/net/irda/max_tx_data_size`
        if [ $CONFIG -eq 2000 ]
        then
            tst_resm TINFO "PASSED"
        else
            tst_resm TFAIL "FAILED"
            return 5
        fi

        # mandate that Linux uses a large turnaround time
        echo
        tst_resm TINFO "setting min_tx_turn_time to 1000"
        echo 1000 > /proc/sys/net/irda/min_tx_turn_time
        CONFIG=`cat /proc/sys/net/irda/min_tx_turn_time`
        if [ $CONFIG -eq 1000 ]
        then
            tst_resm TINFO "PASSED"
        else
            tst_resm TFAIL "FAILED"
            return 5
        fi

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
            #Display a list of discovered IrDA devices
            #DEVICE=`eval $cmd`
            sleep 2
        done

        if [ "x$cmd" = "x" ] || [ $i -gt $max ]; then
            tst_resm TFAIL "No IrDA device found!"
            return 5
        fi

        tst_resm TINFO "Remote IrDA device address is: "$DEVICE_ADDRESS

        DEVICE_ADDRESS=`cat /proc/net/irda/discovery | sed -n '/daddr/p' | sed 's/^.*daddr: //' `
        tst_resm TINFO "Remote IrDA device address is: "$DEVICE_ADDRESS

        echo
}

# Function:     main
# 
# Description:  - Execute all tests, exit with test status.
#               
# Exit:         - zero on success
#               - non-zero on failure.
#
# Retuon value from setup, and test functions.
setup

IrDA_FIR_test

cleanup
