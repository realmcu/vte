#!/bin/ash

# (unload gcov_module)
# load gcov module(s)
echo "Loading gcov module(s)..."
rmmod gcov-proc 2>&1 > /dev/null || :
insmod /lib/modules/`uname -r`/kernel/drivers/gcov/gcov-proc.ko
# reset global counter
echo "Reseting global counter..."
echo "0" > /proc/gcov/vmlinux
