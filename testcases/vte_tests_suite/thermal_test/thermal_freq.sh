#!/bin/sh -x
THERMO_PATH="/sys/devices/virtual/thermal/thermal_zone0"

RC=0

loops=10

#store the old temp setting
normal_temp=$(cat ${THERMO_PATH}/temp)
trip0=$(cat ${THERMO_PATH}/trip_point_0_temp)
trip1=$(cat ${THERMO_PATH}/trip_point_1_temp)
trip2=$(cat ${THERMO_PATH}/trip_point_2_temp)
i=0
while [ $i -lt $loops ]
	do

i=$(expr $i + 1 )


#change the hot value
# we expect the cpufreq is lower when hot critical is meet
test_temp=$(expr $normal_temp - 1)

old_cpufreq=$(cpufreq-info -f)

echo $test_temp > ${THERMO_PATH}/trip_point_2_temp 
echo $test_temp > ${THERMO_PATH}/trip_point_1_temp 

sleep 5

cur_cpufreq=$(cpufreq-info -f)

if [ $cur_cpufreq -lt $old_cpufreq ];then
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

cur_cpufreq2=$(cpufreq-info -f)


if [ $cur_cpufreq2 -gt $cur_cpufreq ];then
   echo "PASS 2"
else
   echo "Fail 2"
   RC=$(expr $RC + 1)
fi

done

exit $RC

