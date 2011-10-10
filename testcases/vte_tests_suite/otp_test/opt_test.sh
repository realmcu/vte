#!/bin/sh -x

RC=0

opt_ctl="/sys/fsl_otp"

ctl_list=$(ls $opt_ctl)

for i in $ctl_list
	do
cat $opt_ctl/$i || RC=$(expr $RC + 1)
  done

if [ $RC -eq 0 ];then
echo "test PASS"
else
echo "test Fail"
fi

exit $RC
