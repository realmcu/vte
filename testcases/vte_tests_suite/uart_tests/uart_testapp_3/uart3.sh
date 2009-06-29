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

TESTAPP3=./uart_testapp_3

defaultorder=2

if [ "$1" = "--order" ];
    then
    order="$2"
    shift; shift;
else
    order=$defaultorder
fi

if [ $# -ne 4 ];
then
    echo "usage: $0 <SOURSE> <DESTINATION> <BAUD> <FLOW>"
    exit 1
fi

TMP_RC=0
rc=0

anal_res()
{
  TMP_RC=$?
  echo""
  if [ $TMP_RC -eq 0 ];
   then
    echo " order   testcase         RESULT   Exit value"
    echo " ------------------------------------"
    echo " $order     uart_testapp_3 $*  TPASS        0   "
  else
    rc=1
    echo " order   testcase         RESULT   Exit value"
    echo " ------------------------------------"
    echo " $order     uart_testapp_3 $*  TFAIL        1   "
  fi
  echo""
}

runit()
{
    $TESTAPP3 $*
    anal_res $*
}


case $order in
    "1")
	runit -s $1 -d $2 -B $3 -T $4 -R O -C 7 -S 1
	runit -s $1 -d $2 -B $3 -T $4 -R E -C 7 -S 1
	runit -s $1 -d $2 -B $3 -T $4 -R O -C 8 -S 1
	runit -s $1 -d $2 -B $3 -T $4 -R E -C 8 -S 1
	runit -s $1 -d $2 -B $3 -T $4 -R O -C 7 -S 2
	runit -s $1 -d $2 -B $3 -T $4 -R E -C 7 -S 2
	runit -s $1 -d $2 -B $3 -T $4 -R O -C 8 -S 2
	runit -s $1 -d $2 -B $3 -T $4 -R E -C 8 -S 2
	runit -s $2 -d $1 -B $3 -T $4 -R O -C 7 -S 1
	runit -s $2 -d $1 -B $3 -T $4 -R E -C 7 -S 1
	runit -s $2 -d $1 -B $3 -T $4 -R O -C 8 -S 1
	runit -s $2 -d $1 -B $3 -T $4 -R E -C 8 -S 1
	runit -s $2 -d $1 -B $3 -T $4 -R O -C 7 -S 2
	runit -s $2 -d $1 -B $3 -T $4 -R E -C 7 -S 2
	runit -s $2 -d $1 -B $3 -T $4 -R O -C 8 -S 2
       runit -s $2 -d $1 -B $3 -T $4 -R E -C 8 -S 2
	;;
    "2")
	for S in 1 2;
	  do 
	  for R in O E;
	    do
	    for C in 7 8;
	      do
	      runit -s $1 -d $2 -B $3 -T $4 -R $R -C $C -S $S
	      runit -s $2 -d $1 -B $3 -T $4 -R $R -C $C -S $S
	    done;
	  done;
	done;
	;;
    "3")
	for C in 8 7;
	  do 
	  for R in E 0;
	    do
	    for S in 2 1;
	      do
	      runit -s $2 -d $1 -B $3 -T $4 -R $R -C $C -S $S
	      runit -s $1 -d $2 -B $3 -T $4 -R $R -C $C -S $S
	    done;
	  done;
	done;
	;;
    "4")
	for S in 1 2;
	  do 
	  for R in O E;
	    do
	    for C in 7 8;
	      do
	      runit -s $1 -d $2 -B $3 -T $4 -R $R -C $C -S $S
	    done;
	  done;
	done;
	for S in 1 2;
	  do 
	  for R in O E;
	    do
	    for C in 7 8;
	      do
	      runit -s $2 -d $1 -B $3 -T $4 -R $R -C $C -S $S
	    done;
	  done;
	done;
	;;
    *)
	echo "Unsupported order: \"$order\""
	exit 1
	;;
esac

  echo""
  echo""
  echo " final script       RESULT   "
  echo " ----------------------------"
if [ $rc -eq 0 ];
  then
    echo " uart3.sh          TPASS    "
else
    echo " uart3.sh          TFAIL    "
fi
  echo""

exit $rc
