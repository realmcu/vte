#!/bin/bash

declare -a cpufreq_value


cleanup()
{
echo "on line all cpus"
echo 1 > /sys/devices/system/cpu/cpu3/online
echo 1 > /sys/devices/system/cpu/cpu2/online
echo 1 > /sys/devices/system/cpu/cpu1/online
}

setup()
{
    #TODO Total test case
    export TCID="SETUP"
    export TST_COUNT=0
    RC=0

	trap cleanup 0
    i=0
    count=$(cpufreq-info -s | wc -w)
    while [ $i -lt $count ]
    do
        # cut: fields and positions are numbered from 1
        field=$(expr $i + 1)
        freq=$(cpufreq-info -s | cut -d " " -f $field | cut -d ":" -f 1)
        if [ ! -z $freq ]; then
            cpufreq_value[$i]=$freq
        fi
        i=$(expr $i + 1)
    done

    TOTAL_PT=${#cpufreq_value[@]}
    echo -n "CPUFREQ is:"
    for (( i = 0; i < TOTAL_PT; i++ )); do
        echo -n " ${cpufreq_value[$i]}"
    done
    echo

    return $RC
}


do_print()
{
echo "++++++++++++++++++++++++++++++++"
  for i in $timer_in_ms
  do
	  printf "%-15s" $(expr 1000000 / $i )
  done
  printf "\n"
  for i in $hz_results
  do
	  printf "%-15s" $i
   done
  printf "\n"
echo "++++++++++++++++++++++++++++++++"
hz_results=
}

do_test_interrupt()
{
	for i in $timer_in_ms
	do
		result=$(timer_interrupt $i | awk '{print $7}')
		hz_results=$(echo $hz_results $result)
   done
}

do_test_task()
{
	for i in $timer_in_ms
	do
		result=$(timer_interrupt $i 1 | awk '{print $7}')
		hz_results=$(echo $hz_results $result)
	done
}

setup || exit -1

RC=0
count=0
hz_results=
timer_in_ms="2 20 100 300 500"
cpufreq-set -g userspace || RC=1

echo "test different cpu freq"

while [ $count -lt $TOTAL_PT  ]; do
echo "************************************"
	value=${cpufreq_value[${count}]}
	echo " test latency in $value"
	cpufreq-set -f ${value}
#	echo "timer latency"
#	do_test_interrupt
#	do_print
	echo "latency with task sleep"
	do_test_task
	do_print
	count=$(expr $count + 1)
done


run_3core()
{
echo "3 core data"
echo 0 > /sys/devices/system/cpu/cpu3/online
echo "-------------------------------------------"
count=0
while [ $count -lt $TOTAL_PT  ]; do
echo "************************************"
	value=${cpufreq_value[${count}]}
	echo " test latency in $value"
	cpufreq-set -f ${value}
	echo "timer latency"
	do_test_interrupt
	do_print
	echo "add task swith latency"
	do_test_task
	do_print
	count=$(expr $count + 1)
done
}

run_2core()
{
echo "2 core data"
echo 0 > /sys/devices/system/cpu/cpu2/online
echo "-------------------------------------------"
count=0
while [ $count -lt $TOTAL_PT  ]; do
echo "************************************"
	value=${cpufreq_value[${count}]}
	echo " test latency in $value"
	cpufreq-set -f ${value}
	echo "timer latency"
	do_test_interrupt
	do_print
	echo "add task swith latency"
	do_test_task
	do_print
	count=$(expr $count + 1)
done
}

run_1core()
{
echo "1 core data"
echo 0 > /sys/devices/system/cpu/cpu1/online
echo "-------------------------------------------"
count=0
while [ $count -lt $TOTAL_PT  ]; do
echo "************************************"
	value=${cpufreq_value[${count}]}
	echo " test latency in $value"
	cpufreq-set -f ${value}
#	echo "timer latency"
#	do_test_interrupt
#	do_print
	echo "add task swith latency"
	do_test_task
	do_print
	count=$(expr $count + 1)
done
}


echo "on line all cpus"
echo 1 > /sys/devices/system/cpu/cpu3/online
echo 1 > /sys/devices/system/cpu/cpu2/online
echo 1 > /sys/devices/system/cpu/cpu1/online


interactive()
{

echo "-------------------------------------------"
echo "test interactive mode"

cpufreq-set -g interactive || RC=1

echo "************************************"
echo "4 core data"

do_test_interrupt
do_print
do_test_task 
do_print

echo "************************************"
echo "3 core data"
echo 0 > /sys/devices/system/cpu/cpu3/online

do_test_interrupt
do_print
do_test_task
do_print

echo "************************************"
echo "2 core data"
echo 0 > /sys/devices/system/cpu/cpu2/online

do_test_interrupt
do_print
do_test_task
do_print

echo "************************************"
echo "1 core data"
echo 0 > /sys/devices/system/cpu/cpu1/online

do_test_interrupt
do_print
do_test_task
do_print
}
exit $RC
