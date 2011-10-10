#!/bin/sh
#refer http://www.kernel.org/doc/Documentation/power/basic-pm-debugging.txt

loops=10

setup()
{
trap "clean" 0
}

clean()
{
	echo none > /sys/power/pm_test
}

#test the freezing of processes, suspending of devices, platform global
#control methods(*), the disabling of nonboot CPUs and suspending of
#platform/system devices
echo "core test"
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
echo "processors test"
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
echo "platform test"
i=0
echo platform > /sys/power/pm_test
while [ $i -lt $loops ]
	do
   i=$(expr $i + 1)
   echo mem > /sys/power/state
	 echo standby > /sys/power/state
	done

#test the freezing of processes and suspending of devices
echo "devices test"
i=0
echo devices > /sys/power/pm_test
while [ $i -lt $loops ]
	do
   i=$(expr $i + 1)
   echo mem > /sys/power/state
	 echo standby > /sys/power/state
	done

#test the freezing of processes
echo "freezer test"
i=0
echo freezer > /sys/power/pm_test
while [ $i -lt $loops ]
	do
   i=$(expr $i + 1)
   echo mem > /sys/power/state
	 echo standby > /sys/power/state
	done

echo "test Pass"
