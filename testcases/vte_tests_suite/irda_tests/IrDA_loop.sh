#!/bin/sh

set -e
#set -x

export TCID="IrDA_FIR_send"
export TST_COUNT=0
export TST_TOTAL=1

#
# Get a clen env.
#
tst_resm TINFO "Removing all IrDA modules..."
# Kill all current irattach processes
ifconfig irda0 down 2> /dev/null || :
killall irattach 2> /dev/null || :
sleep 1
# Unload IrDa stack module and dependencies
rmmod irtty_sir 2> /dev/null || :
rmmod sir_dev 2> /dev/null || :
rmmod ircomm_tty 2> /dev/null || :
rmmod ircomm 2> /dev/null || :
rmmod irlan 2> /dev/null || :
rmmod irport 2> /dev/null || :
rmmod mxc_fir 2> /dev/null || :
rmmod irda 2> /dev/null || :
rmmod crc_ccitt 2> /dev/null || :
#lsmod || :

# Create the IrDA devices :
# tst_resm TINFO "Creating devices (if needed) ..."
# [ -c /dev/ircomm0 ] || mknod /dev/ircomm0 c 161 0
# [ -c /dev/ircomm1 ] || mknod /dev/ircomm1 c 161 1
# [ -c /dev/irlpt0 ] || mknod /dev/irlpt0 c 161 16
# [ -c /dev/irlpt1 ] || mknod /dev/irlpt1 c 161 17
# [ -c /dev/irnet ] || mknod /dev/irnet c 10 187
# chmod 666 /dev/ir*

# Load IrDA module in the kernel
tst_resm TINFO "Loading required modules..."
modprobe irtty-sir 2> /dev/null


uartdev=$1
tst_resm TINFO "Attaching to device $uartdev ..."
irattach $uartdev -s

# Wait for remote device to be detected
tst_resm TINFO "Waiting for a remote device..."
cmd="grep daddr /proc/net/irda/discovery | sed -e 's/^.*nickname: \([^,]*\),.*daddr: \(.*\)$/\1 (\2)/'"
DEVICE=""
while [ "x$DEVICE" = "x"  ]; do
    echo -n "."
    DEVICE=`eval $cmd`
    sleep 1
done
echo ""
tst_resm TPASS "Found a remote IrDA device: \"$DEVICE\""

# Bring up the IrDA interface
tst_resm TINFO "Bringing up irda0 interface ..."
#lsmod || :
#modprobe ircomm-tty 2> /dev/null
insmod ircomm.ko
insmod ircomm-tty.ko
#lsmod || :
ifconfig irda0 up

# loopback
tst_resm TINFO "Looping back..."
while :; do
    dd if=/dev/ircomm0 of=/dev/ircomm0 bs=1 count=1024
done

