#!/bin/bash

setup()
{
    # Total number of test cases in this file. 
    export TST_TOTAL=1
    
    # The TCID and TST_COUNT variables are required by the LTP 
    # command line harness APIs, these variables are not local to this program.

    # Test case identifier
    export TCID="wifi_POWER_test"
    # Set up is initialized as test 0
    export TST_COUNT=0
    # Initialize cleanup function to execute on program exit.
    # This function will be called before the test program exits.
    trap "cleanup" 0
   
    modprobe ath6kl && iwconfig eth1 mode managed && sleep 10 && iwlist eth1 scanning | grep FSLLBGAP_001 && iwconfig eth1 key bbd9837522 && iwconfig eth1 essid FSLLBGAP_001
	if [ $? -ne 0 ];then
       RC=1
	else
      udhcpc -i eth1
	  sleep 3
	  localip=$(ifconfig eth1 | grep addr: | cut -d : -f 2 | cut -d " " -f 1)
	  export LOCALIP=${localip}
	fi


	return $RC
}

cleanup()
{
    echo "CLEANUP "
}

usage()
{
   echo "1: for device suspend resume case for no boot cores "	
   echo "2: for device suspend resume case for all cores "	
}

# Function:     test_case_01
# Description   - Test if device suspend and resume without bootcore
#  
test_case_01()
{
#TODO give TCID 
TCID="wifi_PM_NOBOOTCORE"
#TODO give TST_COUNT
TST_COUNT=1
RC=1

#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "

#TODO add function test scripte here
udp_stream_2nd_script 10.192.225.222 CPU &

echo "core test"
i=0
loops=10
echo core > /sys/power/pm_test
while [ $i -lt $loops ]
do
  i=$(expr $i + 1)
  echo mem > /sys/power/state
 echo standby > /sys/power/state
done

echo none > /sys/power/pm_test

wait

RC=0

return $RC

}

# Function:     test_case_02
# Description   - Test if device suspend and resume without bootcore
#  
test_case_02()
{
#TODO give TCID 
TCID="wifi_PM_BOOTCORE"
#TODO give TST_COUNT
TST_COUNT=1
RC=1

#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "
tloops=100
count=0
#TODO add function test scripte here
while [ $count -lt $tloops ]
do

  udp_stream_2nd_script 10.192.225.222 CPU &

  i=0
  loops=100
  while [ $i -lt $loops ]
  do
    i=$(expr $i + 1)
    echo mem > /sys/power/state
    echo standby > /sys/power/state
  done

  wait
 
  count=$(expr $count + 1)
done

RC=0

return $RC

}


setup || exit 1

case "$1" in
1)
  test_case_01 || exit 2 
  ;;
2)
  test_case_02 || exit 3
  ;;
*)
  usage
  ;;
esac

tst_resm TINFO "Test Finish"
