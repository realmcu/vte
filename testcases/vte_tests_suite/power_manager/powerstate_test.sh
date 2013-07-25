#!/bin/sh
#refer http://www.kernel.org/doc/Documentation/power/basic-pm-debugging.txt

#do not edit it
if [ -n "$1" ];then
	loops=$1
else
	loops=10
fi

setup()
{
    trap "clean" 0
    # to avoid FS syncing problem
    rtc_testapp_6 -T 20 -m mem
    rtc_testapp_6 -T 20 -m mem
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
    echo mem > /sys/power/state
    echo standby > /sys/power/state
done

#test the freezing of processes, suspending of devices, platform
#global control methods(*) and the disabling of nonboot CPUs
echo "===Power state processors test==="
i=0
echo processors > /sys/power/pm_test
while [ $i -lt $loops ]
do
    i=$(expr $i + 1)
    echo mem > /sys/power/state
    echo standby > /sys/power/state
done

#test the freezing of processes, suspending of devices and platform
#global control methods(*)
echo "===Power state platform test==="
i=0
echo platform > /sys/power/pm_test
while [ $i -lt $loops ]
do
    i=$(expr $i + 1)
    echo mem > /sys/power/state
    echo standby > /sys/power/state
done

#test the freezing of processes and suspending of devices
echo "===Power state devices test==="
i=0
echo devices > /sys/power/pm_test
while [ $i -lt $loops ]
do
    i=$(expr $i + 1)
    echo mem > /sys/power/state
    echo standby > /sys/power/state
done

#test the freezing of processes
echo "===Power state freezer test==="
i=0
echo freezer > /sys/power/pm_test
while [ $i -lt $loops ]
do
    i=$(expr $i + 1)
    echo mem > /sys/power/state
    echo standby > /sys/power/state
done

echo "Test Pass"
