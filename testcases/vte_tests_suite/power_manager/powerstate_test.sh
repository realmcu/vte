#!/bin/sh
#refer http://www.kernel.org/doc/Documentation/power/basic-pm-debugging.txt

#do not edit it
if [ -n "$1" ];then
	loops=$1
else
	loops=10
fi

RC=0

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

lines=$(dmesg | grep "PM: early resume of devices complete after" | cut -d " " -f 7)
for li in $lines
do
  if [ $li -lt 1000 ]; then
    RC=1
  fi
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

lines=$(dmesg | grep "PM: early resume of devices complete after" | cut -d " " -f 7)
for li in $lines
do
  if [ $li -lt 1000 ]; then
    RC=2
  fi
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

lines=$(dmesg | grep "PM: early resume of devices complete after" | cut -d " " -f 7)
for li in $lines
do
  if [ $li -lt 1000 ]; then
    RC=3
  fi
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

lines=$(dmesg | grep "PM: early resume of devices complete after" | cut -d " " -f 7)
for li in $lines
do
  if [ $li -lt 1000 ]; then
    RC=4
  fi
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
lines=$(dmesg | grep "PM: early resume of devices complete after" | cut -d " " -f 7)
for li in $lines
do
  if [ $li -lt 1000 ]; then
    RC=5
  fi
done
echo $RC
if [ $RC -eq 0 ]; then
echo "Test Pass"
else
echo "Test Failure"
fi
exit $RC
