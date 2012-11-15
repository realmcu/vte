#!/bin/sh
#refer http://www.kernel.org/doc/Documentation/power/basic-pm-debugging.txt

loops=500

setup()
{
    trap "clean" 0
}

clean()
{
    echo none > /sys/power/pm_test
}

setup

#test the freezing of processes, suspending of devices, platform global
#control methods(*), the disabling of nonboot CPUs and suspending of
#platform/system devices
echo "===Power state core test==="
i=0
echo core > /sys/power/pm_test
while [ $i -lt $loops ]
do
    i=$(expr $i + 1)
    echo "===Power state core test $i times==="
    echo mem > /sys/power/state
    sleep 1
    echo standby > /sys/power/state
    sleep 1
done

#test the freezing of processes, suspending of devices, platform
#global control methods(*) and the disabling of nonboot CPUs
echo "===Power state processors test==="
i=0
echo processors > /sys/power/pm_test
while [ $i -lt $loops ]
do
    i=$(expr $i + 1)
    echo "===Power state processors test $i times==="
    echo mem > /sys/power/state
    sleep 1
    echo standby > /sys/power/state
    sleep 1
done

#test the freezing of processes, suspending of devices and platform
#global control methods(*)
echo "===Power state platform test==="
i=0
echo platform > /sys/power/pm_test
while [ $i -lt $loops ]
do
    i=$(expr $i + 1)
    echo "===Power state platform test $i times==="
    echo mem > /sys/power/state
    sleep 1
    echo standby > /sys/power/state
    sleep 1
done

#test the freezing of processes and suspending of devices
echo "===Power state devices test==="
i=0
echo devices > /sys/power/pm_test
while [ $i -lt $loops ]
do
    i=$(expr $i + 1)
    echo "===Power state devices test $i times==="
    echo mem > /sys/power/state
    sleep 1
    echo standby > /sys/power/state
    sleep 1
done

#test the freezing of processes
echo "===Power state freezer test==="
i=0
echo freezer > /sys/power/pm_test
while [ $i -lt $loops ]
do
    i=$(expr $i + 1)
    echo "===Power state freezer test $i times==="
    echo mem > /sys/power/state
    sleep 1
    echo standby > /sys/power/state
    sleep 1
done

echo "Test Pass"
