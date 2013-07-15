#!/bin/sh
if [ $# -le 1 ]; then
    echo "need number of process"
    echo "$0 1/2/3/4/"
    hint_msg="Warning: For no number of process given, CPU stress run on all cpu \
cores, or please use Ctrl+C to interrupt and input a number."
    cpu_num=`cat /proc/cpuinfo |grep processor |wc -l`
else
    cpu_num=$1
fi


while [ true ]; do
    echo
    echo $hint_msg
    sleep 10

    if [ $cpu_num -eq 4  ]; then
    dry2 &
    fi
    if [ $cpu_num -gt 3  ]; then
    dry2 &
    fi
    if [ $cpu_num -gt 2  ]; then
    dry2 &
    fi
    dry2
done

