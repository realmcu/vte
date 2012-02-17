#!/bin/bash -x

loops=0
cpufreq-info

while [ $loops -lt 300 ] 
do
	echo ======================================================
	echo "<<<STRESSINFO $loops cycle start>>>"
	cat /sys/devices/system/cpu/cpu0/cpufreq/stats/time_in_state
	for foo in $(ls /mnt/nfs/test_stream/power_stream/)
	do
		echo "STRESSINFO before test $foo temp"
		cat /sys/devices/platform/ahci.0/temperature
		time -p gplay /mnt/nfs/test_stream/power_stream/${foo}
		echo "STRESSINFO after test $foo temp"
		cat /sys/devices/platform/ahci.0/temperature
	done
	cat /sys/devices/system/cpu/cpu0/cpufreq/stats/time_in_state
	echo "<<<STRESSINFO $loops cycle end>>>"
	loops=$(expr $loops + 1)
	echo =====================================================
done
