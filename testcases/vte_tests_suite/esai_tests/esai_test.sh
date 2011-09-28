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
TCID="ESAI_7.1_test"
#TODO give TST_COUNT
TST_COUNT=1
RC=0

#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "

echo "Ctrl+C to quit"
speaker-test -t sine -c8

read -p "does the sound normal?y/n" ret

if [ $ret != 'y' && $ret != 'Y' ]; then
RC=1
fi
 
return $RC
}

test_case_02()
{
#TODO give TCID
TCID="ESAI_7.1_auto_test"
#TODO give TST_COUNT
TST_COUNT=1
RC=0

#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "

speaker-test -t sine -c8 -s1 || RC=1
speaker-test -t sine -c8 -s2 || RC=$(echo $RC 2) 
speaker-test -t sine -c8 -s3 || RC=$(echo $RC 3)
speaker-test -t sine -c8 -s4 || RC=$(echo $RC 4)
speaker-test -t sine -c8 -s5 || RC=$(echo $RC 5)
speaker-test -t sine -c8 -s6 || RC=$(echo $RC 6)
speaker-test -t sine -c8 -s7 || RC=$(echo $RC 7)
speaker-test -t sine -c8 -s8 || RC=$(echo $RC 8)

if [ '$RC' != '0' ]; then
RC=1
fi
 
return $RC
}

test_case_03()
{
#TODO give TCID
TCID="ESAI_5.1_auto_test"
#TODO give TST_COUNT
TST_COUNT=1
RC=0

#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "

speaker-test -t sine -c6 -s1 || RC=1
speaker-test -t sine -c6 -s2 || RC=$(echo $RC 2) 
speaker-test -t sine -c6 -s3 || RC=$(echo $RC 3)
speaker-test -t sine -c6 -s4 || RC=$(echo $RC 4)
speaker-test -t sine -c6 -s5 || RC=$(echo $RC 5)
speaker-test -t sine -c6 -s6 || RC=$(echo $RC 6)

if [ '$RC' != '0' ]; then
RC=1
fi
 
return $RC
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

