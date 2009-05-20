#!/bin/sh
# File :        IrDA_SIR_test.sh
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
#                         Modification Tracking
# Author     Date  Number  Description of Changes
#------------------------ ---------- ---------- ------------------------------
#V. Becker/rc023c   29/08/2004 TLSbo40417 Initial version
#V. Becker/rc023c   27/09/2004 TLSbo40417 Inspection TLS941
#V. Becker/rc023c   25/10/2004 TLSbo44073 Minor changes
#V.HALABUDA/HLBV001   12/04/2005 TLSbo40417 control, mandate and limit IrDA stack
#C. Gagneraud 13/09/2005  TLSbo52990  various minor fixes + clean up output.
#Rakesh. S. Joshi/r65956    13/07/2007         Fixes for Cleanup

# Function:     setup
#
# Description:  - Check if required commands exits
#               - Export global variables
#               - Check if required config files exits
#               - Create temporary files and directories
#
# Return        - zero on success
#               - non zero on failure. return value from commands ($RC)

tst_resm()
{
    echo $1 $2
}

if [ $# -eq 0 ] || [ $# -gt 1 ]
then
 echo ""
 echo " Usage is :"
 echo "  $0 /dev/ttymxc/X"
 echo " where X is MXC UART number used for IrDA SIR transmission"
 echo ""
 exit
fi

setup()
{
    #Load IrDA module in the kernel
    tst_resm TINFO "Loading modules..."
    /sbin/modprobe irtty-sir 2> /dev/null
}

cleanup()
{
    tst_resm TINFO "Removing all IrDA modules..."
    #Kill all current irattach processes
 pid=`ps -A| grep irattach| sed 's/^ \+//;s/ .*$//'`
 kill $pid 2> /dev/null
    sleep 1
 /sbin/rmmod irtty_sir 2>/dev/null
 /sbin/rmmod sir_dev 2>/dev/null
 /sbin/rmmod irda 2>/dev/null
 /sbin/rmmod crc_ccitt 2>/dev/null
 exit
}

IrDA_SIR_test()
{
    RC=0
    i=0
    DEVICE_ADDRESS=""

 uartdev=$1
 tst_resm TINFO "Attaching to device $uartdev ..."
    irattach $uartdev -s || RC=$?
    if [ $RC -ne 0 ];
    then
 tst_resm TFAIL "irattach failed"
 cleanup
    fi

    #Get remote IrDA device address
    cmd="grep daddr /proc/net/irda/discovery | sed -e 's/^.*nickname: \([^,]*\),.*daddr: \(.*\)$/\1 (\2)/'"
    DEVICE=""
    i=1
    max=20
    while [ "x$DEVICE" = "x"  ] && [ $i -le $max ]; do
  tst_resm TINFO "Discovering [Try $i/$max] ..."
  i=`expr $i + 1`
  #Display a list of discovered IrDA devices
  DEVICE=`eval $cmd`
  sleep 1
    done

    if [ "x$DEVICE" = "x" ] || [ $i -gt $max ]; then
  tst_resm TFAIL "No IrDA device found !"
   tst_resm TFAIL "Test FAIL !"
  cleanup
    fi
    tst_resm TINFO "Found a remote IrDA device: \"$DEVICE\""
    tst_resm TINFO "Is this correct?"
    echo -n "Y/N (N)? "
    read resp
    case $resp in
 y|Y|yes|YES)
     ;;
 *)
     tst_resm TFAIL "Invalid response"
  cleanup
     ;;
    esac

    tst_resm TINFO "Now move away the 2 IrDA devices to cut the IrDA link"
    tst_resm TINFO "Press <enter> to continue."
    read tmp

    oldev=$DEVICE
    i=1
    max=10
    while [ "x$DEVICE" != "x" ] && [ $i -le $max ]; do
  tst_resm TINFO "Waiting $oldev to be removed [Try $i/$max] ..."
  i=`expr $i + 1`
  #Display a list of discovered IrDA devices
  DEVICE=`eval $cmd`
  sleep 1
    done

    if [ "x$DEVICE" != "x" ] || [ $i -gt $max ]; then
  tst_resm TFAIL "$oldev still present!"
   tst_resm TFAIL "Test FAIL !"
  cleanup
    fi

    tst_resm TINFO "Detach successfull !"

    #Set test to PASS
    tst_resm TPASS "Test PASS !"
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

setup  || exit $RC

IrDA_SIR_test $1 || exit $RC

cleanup
