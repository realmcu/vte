#!/bin/bash

target=$1

cpufreq-set -g performance
cpufreq-info -f  
cpufreq-set -f 996000  

if [ -e $1  ]; then
echo "4 core data"
dd if=$1 of=/dev/null bs=1M count=500
sleep 3 

echo "3 core data"
echo 0 > /sys/devices/system/cpu/cpu3/online
dd if=$1 of=/dev/null bs=1M count=500
sleep 3 


echo "2 core data"
echo 0 > /sys/devices/system/cpu/cpu2/online
dd if=$1 of=/dev/null bs=1M count=500
sleep 3 

echo "1 core data"
echo 0 > /sys/devices/system/cpu/cpu1/online
dd if=$1 of=/dev/null bs=1M count=500
sleep 3 

echo "on line all cpus"
echo 1 > /sys/devices/system/cpu/cpu3/online
echo 1 > /sys/devices/system/cpu/cpu2/online
echo 1 > /sys/devices/system/cpu/cpu1/online

fi
