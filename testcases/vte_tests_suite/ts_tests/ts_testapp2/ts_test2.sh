#!/bin/sh  

 LTPROOT=`cd \`dirname $0\` && echo $PWD`

 cd $LTPROOT
 TESTAPP5=./ts_testapp2
 TMP_RC=0
 rc=0

 anal_res()
 {
  TMP_RC=$?
  echo""
  if [ $TMP_RC -eq 0 ];
    then
     echo " testcase      RESULT   Exit value"
     echo " ---------------------------------"
     echo " ts_testapp2   TPASS       0"
    else
    rc=1
    echo " testcase      RESULT   Exit value"
    echo " ---------------------------------"
    echo "ts_testapp2    TFAIL       -1"
  fi
  echo ""
 }
  echo "Please touch screen with a pen continuously!\n"
  $TESTAPP5
  anal_res
  exit $rc


