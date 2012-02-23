#!/bin/sh 

###################################################################################################
#Change History:
#                            Modification     Tracking
#Author                          Date          Number    Description of Changes
#-------------------------   ------------    ----------  -------------------------------------------
# 
#Andy Tian/-----              11/16/2011     N/A         1, Set the hot temp and active temp with a gap of 10  
#                                                        to make the cpu hotplug event easy to see;
#                                                        2, re-struct the code according to the template
#
###################################################################################################



# Function:     setup
#
# Description:  - Check if required commands exits
#               - Export global variables
#               - Check if required config files exits
#               - Create temporary files and directories
#
# Return        - zero on success
#               - non zero on failure. return value from commands ($RC)
setup()
{
RC=1
#TODO Total test case
export TST_TOTAL=1

export TCID="setup"
export TST_COUNT=0

#trap exit and ctrl+c message
trap "cleanup" 0 2


#TODO add setup scripts

THERMO_PATH="/sys/devices/virtual/thermal/thermal_zone0"

loops=10

#store the old temp setting
normal_temp=$(cat ${THERMO_PATH}/temp)
trip0=$(cat ${THERMO_PATH}/trip_point_0_temp)
trip1=$(cat ${THERMO_PATH}/trip_point_1_temp)
trip2=$(cat ${THERMO_PATH}/trip_point_2_temp)

check=$(cat /proc/cmdline | grep =cpuhotplug | wc -l)
if [ $check -eq 0 ];then
  echo "test setting not correct"
  echo "please add cooling_device=cpuhotplug to kernel command line"
  exit 0
fi

RC=0
return $RC
}


# Function:     cleanup
#
# Description   - remove temporary files and directories.
#
# Return        - zero on success
#               - non zero on failure. return value from commands ($RC)
cleanup()
{

#TODO add cleanup code here
# Recover the original thermal setting

echo $trip2 > ${THERMO_PATH}/trip_point_2_temp
echo $trip1 > ${THERMO_PATH}/trip_point_1_temp
if [ $RC -ne 0 ]
then
    tst_resm TINFO "Test FAIL"      
fi

exit $RC
}

usage()
{
echo "Usage: $0"
}

test_case_01()
{
RC=1
#TODO give TCID 
TCID="thermal_hotplug_test"
#TODO give TST_COUNT
TST_COUNT=1

#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "

#TODO add function test scripte here

i=0

while [ $i -lt $loops ]
        do

i=$(expr $i + 1 )


#change the hot value
# we expect the one cpu is out when hot critical is meet
test_temp_hot=$(expr $normal_temp - 5)
test_temp_active=$(expr $normal_temp - 15)

old_cpu=$(cat /proc/cpuinfo | grep processor | wc -l)

echo $test_temp_active > ${THERMO_PATH}/trip_point_2_temp
echo $test_temp_hot > ${THERMO_PATH}/trip_point_1_temp

sleep 5

cur_cpu=$(cat /proc/cpuinfo | grep processor | wc -l)

if [ $cur_cpu -lt $old_cpu ];then
   echo "PASS 1"
else
   echo "Fail 1"
   RC=$i
   exit $i
fi

#move the trip temp back
# we expect the core back
echo $trip2 > ${THERMO_PATH}/trip_point_2_temp
echo $trip1 > ${THERMO_PATH}/trip_point_1_temp

sleep 5

cur_cpu2=$(cat /proc/cpuinfo | grep processor | wc -l)


if [ $cur_cpu2 -gt $cur_cpu ];then
   echo "PASS 2"
else
   echo "Fail 2"
   RC=$i
   exit $i
fi

done

RC=0

return $RC
}

# main function

if [ $# -ne 0 ]
then
usage
exit 1
fi

setup || exit $RC

test_case_01 || exit $RC

tst_resm TINFO "Test PASS"
