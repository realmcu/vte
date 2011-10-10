#!/bin/sh -x
# need add kernel command line cooling_device=cpuhotplug
THERMO_PATH="/sys/devices/virtual/thermal/thermal_zone0"

RC=0

loops=10

#store the old temp setting
normal_temp=$(cat ${THERMO_PATH}/temp)
trip0=$(cat ${THERMO_PATH}/trip_point_0_temp)
trip1=$(cat ${THERMO_PATH}/trip_point_1_temp)
trip2=$(cat ${THERMO_PATH}/trip_point_2_temp)
i=0

check=$(cat /proc/cmdline | grep =cpuhotplug | wc -l)
if [ $check -eq 0 ];then
  echo "test setting not correct"
  echo "please add cooling_device=cpuhotplug to kernel command line"
  exit 0
fi

while [ $i -lt $loops ]
	do

i=$(expr $i + 1 )


#change the hot value
# we expect the one cpu is out when hot critical is meet
test_temp=$(expr $normal_temp - 1)

old_cpu=$(cat /proc/cpuinfo | grep processor | wc -l)

echo $test_temp > ${THERMO_PATH}/trip_point_2_temp 
echo $test_temp > ${THERMO_PATH}/trip_point_1_temp 

sleep 5

cur_cpu=$(cat /proc/cpuinfo | grep processor | wc -l)

if [ $cur_cpu -lt $old_cpu ];then
   echo "PASS 1"
else
   echo "Fail 1"
   RC=$(expr $RC + 1)
fi

#move the trip temp back
# we expect the cpufreq up
echo $trip2 > ${THERMO_PATH}/trip_point_2_temp
echo $trip1 > ${THERMO_PATH}/trip_point_1_temp

sleep 5

cur_cpu2=$(cat /proc/cpuinfo | grep processor | wc -l)


if [ $cur_cpu2 -gt $cur_cpu ];then
   echo "PASS 2"
else
   echo "Fail 2"
   RC=$(expr $RC + 1)
fi

done

exit $RC

