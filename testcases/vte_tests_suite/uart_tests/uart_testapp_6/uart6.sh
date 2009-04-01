#!/bin/sh  

LTPROOT=`cd \`dirname $0\` && echo $PWD`

cd $LTPROOT

TESTAPP6=./uart_testapp_6
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
     echo "uart_testapp_6   TPASS       0"
  else
   rc=1
   echo " testcase      RESULT   Exit value"
   echo " ---------------------------------"
   echo "uart_testapp_6   TFAIL       1"
  fi
  echo"" 
 }


 if [ $# -eq 2 ];
  then
   $TESTAPP6 -s $1 -d $2 -T D
   anal_res
   $TESTAPP6 -s $1 -d $2 -T I
   anal_res
   $TESTAPP6 -s $1 -d $2 -B D
   anal_res
   $TESTAPP6 -s $1 -d $2 -B I
   anal_res
   $TESTAPP6 -s $2 -d $1 -T D
   anal_res
   $TESTAPP6 -s $2 -d $1 -T I
   anal_res
   $TESTAPP6 -s $2 -d $1 -B D
   anal_res
   $TESTAPP6 -s $2 -d $1 -B I
   anal_res
 fi


  echo""
  echo "final script      RESULT   "
  echo " ----------------------------"
 if [ $rc -eq 0 ];
  then
    echo "uart6.sh          TPASS    "
  
  else
    echo "uart6.sh          TFAIL    "
 fi

  echo""

 exit $rc
