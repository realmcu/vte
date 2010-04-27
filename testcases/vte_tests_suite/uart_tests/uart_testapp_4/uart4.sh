#Copyright (C) 2005-2009 Freescale Semiconductor, Inc. All Rights Reserved.
#
#The code contained herein is licensed under the GNU General Public
#License. You may obtain a copy of the GNU General Public License
#Version 2 or later at the following locations:
#
#http://www.opensource.org/licenses/gpl-license.html
#http://www.gnu.org/copyleft/gpl.html
#!/bin/sh  

LTPROOT=`cd \`dirname $0\` && echo $PWD`

cd $LTPROOT

 TESTAPP4=./uart_testapp_4
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
     echo "uart_testapp_4   TPASS       0"
  else
   rc=1
   echo " testcase      RESULT   Exit value"
   echo " ---------------------------------"
   echo "uart_testapp_4   TFAIL       1"
  fi
  echo""                             
 }
                             



 if [ $# -eq 2 ];
  then
   $TESTAPP4 -s $1 -d $2 -T 1
    anal_res                          
   $TESTAPP4 -s $1 -d $2 -T 2
    anal_res
   $TESTAPP4 -s $2 -d $1 -T 1
   anal_res
   $TESTAPP4 -s $2 -d $1 -T 2
   anal_res
 fi

  echo""
  echo "final script      RESULT   "
  echo " ----------------------------"
 if [ $rc -eq 0 ];
  then
    echo "uart4.sh          TPASS    "
  
  else
    echo "uart4.sh          TFAIL    "
 fi

  echo""

 exit $rc
