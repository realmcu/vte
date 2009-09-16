#Copyright 2005-2009 Freescale Semiconductor, Inc. All Rights Reserved.
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
 TESTAPP=uart_testapp_9
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
     echo "uart_testapp_9   TPASS       0"
    else
    rc=1
    echo " testcase      RESULT   Exit value"
    echo " ---------------------------------"
    echo "uart_testapp_9   TFAIL       1"
  fi
  echo ""
 }

 if [ $# -eq 1 ]
 then
  $TESTAPP $1 
  anal_res
 else exit 1
 fi

  echo""
  echo "final script      RESULT   "
  echo " ----------------------------"
 if [ $rc -eq 0 ];
  then
    echo "uart9.sh          TPASS    "

  else
    echo "uart9.sh          TFAIL    "
 fi    
  
  echo""

 exit $rc


