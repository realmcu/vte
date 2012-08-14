#!/bin/sh

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
    #TODO Total test case
    export TST_TOTAL=1

    export TCID="setup"
    export TST_COUNT=0
    RC=0

    trap "cleanup" 0
    modprobe ar6000 || return 1
    sleep 5
    iwconfig wlan0 mode managed || return 1
    sleep 2
    iwlist wlan0 scanning | grep FSLLBGAP_001
    iwconfig wlan0 key bbd9837522
    iwconfig wlan0 essid FSLLBGAP_001
    udhcpc -i wlan0 || dhclient wlan0 || return 1
    sleep 5
    export LOCALIP=$(ifconfig wlan0 | grep 'inet addr:'| grep -v '127.0.0.1' | cut -d: -f2 | awk '{ print $1}')

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
RC=0

#TODO add cleanup code here

return $RC
}

# Function:     test_case_01
# Description   - Test if <CPU freq> ok
#  
test_case_01()
{
#TODO give TCID 
TCID="test_clock_stress"
#TODO give TST_COUNT
TST_COUNT=1
RC=0

#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "

#TODO add function test script here
tcp_stream_2nd_script 10.192.225.222 CPU || RC=$(expr $RC + 1)
udp_stream_2nd_script 10.192.225.222 CPU || RC=$(expr $RC + 1)


RC=$?
return $RC
}

usage()
{
    echo "$0 [case ID]"
    echo "1: WiFi stress test with netperf"
    exit 1
}

# main function

RC=0

#TODO check parameter
if [ $# -lt 1 ]
then
usage
exit 1 
fi

setup || exit $RC

case "$1" in
1)
  test_case_01 || exit $RC 
  ;;
*)
  usage
  ;;
esac

echo "Test PASS"

