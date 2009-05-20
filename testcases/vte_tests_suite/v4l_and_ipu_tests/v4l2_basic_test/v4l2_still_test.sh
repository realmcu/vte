#!/bin/sh

LTPROOT=`cd \`dirname $0\` && echo $PWD`
cd $LTPROOT
#setup the fb on
echo 0 > /sys/class/graphics/fb0/blank

TSTCMD="/unit_tests/mxc_v4l2_still.out"
PassCount=0

for FORMAT in YUV420 UYVY YUV422P;do
${TSTCMD} -w 640 -h 480 -f $FORMAT
if [ $? -eq 0 ];then
echo "mxc_v4l2_still -w 640 -h 480 -f $FORMAT TPASS"
let PassCount=PassCount+1
else
echo "mxc_v4l2_still -w 640 -h 480 -f $FORMAT TFAIL"
fi
done

for FORMAT in YUV420 UYVY YUV422P;do
${TSTCMD} -w 640 -h 480 -f $FORMAT -c
if [ $? -eq 0 ];then
echo "mxc_v4l2_still -w 640 -h 480 -f $FORMAT -c TPASS"
let PassCount=PassCount+1
else
echo "mxc_v4l2_still -w 640 -h 480 -f $FORMAT -c TFAIL"
fi
done

for FORMAT in YUV420 UYVY YUV422P;do
${TSTCMD} -w 320 -h 240 -f $FORMAT
if [ $? -eq 0 ];then
echo "mxc_v4l2_still -w 320 -h 240 -f $FORMAT TPASS"
let PassCount=PassCount+1
else
echo "mxc_v4l2_still -w 320 -h 240 -f $FORMAT TFAIL"
fi
done

for FORMAT in YUV420 UYVY YUV422P;do
${TSTCMD} -w 320 -h 240 -f $FORMAT -c
if [ $? -eq 0 ];then
echo "mxc_v4l2_still -w 320 -h 240 -f $FORMAT -c TPASS"
let PassCount=PassCount+1
else
echo "mxc_v4l2_still -w 320 -h 240 -f $FORMAT -c TFAIL"
fi
done

echo "This Test Has 12 testcases"
echo "Pass Num is $PassCount"
if [ $PassCount -eq 12 ];then
echo "========================"
echo "Final Result     PASS"
echo "========================"
exit 0
else
echo "========================"
echo "Final Result     FAIL"
echo "========================"
echo "Please check which cases fail in your LOG"
exit 1
fi
