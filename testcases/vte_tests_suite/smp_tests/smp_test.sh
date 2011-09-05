#!/bin/bash

setup()
{
#TODO Total test case
export TST_TOTAL=4
 
export TCID="setup"
export TST_COUNT=0
RC=0
 
trap "cleanup" 0
 
#TODO add setup scripts
return $RC
}

cleanup()
{
RC=0
 
#TODO add cleanup code here
return $RC
}

test_case_01()
{
#TODO give TCID
TCID="SMP_test"
#TODO give TST_COUNT
TST_COUNT=1
RC=0

#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "

times=500

while [ $times -gt 0 ]
do
echo 0 > /sys/devices/system/cpu/cpu1/online
echo 0 > /sys/devices/system/cpu/cpu2/online
echo 0 > /sys/devices/system/cpu/cpu3/online

sleep 1

echo 1 > /sys/devices/system/cpu/cpu1/online
echo 1 > /sys/devices/system/cpu/cpu2/online
echo 1 > /sys/devices/system/cpu/cpu3/online

sleep 1
times=$(expr $times - 1)
done

return $RC
}

test_case_02()
{
#TODO give TCID
TCID=""
#TODO give TST_COUNT
TST_COUNT=1
RC=0

#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "
times=5000

while [ $times -gt 0 ]
do
echo 0 > /sys/devices/system/cpu/cpu1/online
echo 0 > /sys/devices/system/cpu/cpu2/online
echo 0 > /sys/devices/system/cpu/cpu3/online

echo 1 > /sys/devices/system/cpu/cpu1/online
echo 1 > /sys/devices/system/cpu/cpu2/online
echo 1 > /sys/devices/system/cpu/cpu3/online

times=$(expr $times - 1)
done


return $RC
}

useage()
{
echo "1: cpu hotplug 500 times "	
}

# main function
 
RC=0
 
#TODO check parameter
if [ $# -ne 1 ]
then
usage
exit 1
fi
 
setup || exit $RC
 
case "$1" in
1)
  test_case_01 || exit $RC
  ;;
2)
  test_case_02 || exit $RC
  ;;
3)
  test_case_03 || exit $RC
  ;;
4)
  test_case_04 || exit $RC
  ;;
5)
  test_case_05 || exit $RC
  ;;
*)
  usage
  ;;
esac
 
tst_resm TINFO "Test PASS"

