#!/bin/bash
#Copyright (C) 2005-2009 Freescale Semiconductor, Inc. All Rights Reserved.
#
#The code contained herein is licensed under the GNU General Public
#License. You may obtain a copy of the GNU General Public License
#Version 2 or later at the following locations:
#
#http://www.opensource.org/licenses/gpl-license.html
#http://www.gnu.org/copyleft/gpl.html

LTPROOT=`cd \`dirname $0\` && echo $PWD`
cd $LTPROOT
STATUS=0
DISPLAY=3
PassCount=0
echo "Turn off fb blanking"
echo -e "\033[9;0]" > /dev/fb0
#setup the fb on
echo 0 > /sys/class/graphics/fb0/blank


cleanup()
{
  auto_prepare.sh -R V4L
}

# V4L2 Output Tests

TSTCMD="/unit_tests/mxc_v4l2_output.out"

echo "SDC output size test cases"
for SIZE in 32 64 80 96 112 128 144 160 176 192 208 224 240; do
${TSTCMD} -iw 128 -ih 128 -ow $SIZE -oh $SIZE -d $DISPLAY -r 0
if [ $? -eq 0 ];then
echo "mxc_v4l2_output -iw 128 -ih 128 -ow $SIZE -oh $SIZE -d $DISPLAY -r 0 TPASS"
let PassCount=PassCount+1
else
echo "mxc_v4l2_output -iw 128 -ih 128 -ow $SIZE -oh $SIZE -d $DISPLAY -r 0 TFAIL"
fi
done

echo "SDC input size test cases"
for SIZE in 32 40 48 64 80 96 112 128 144 160 176 192 208 224 240; do
${TSTCMD} -iw $SIZE -ih $SIZE -ow 120 -oh 120 -d $DISPLAY -r 0
if [ $? -eq 0 ];then
echo "mxc_v4l2_output -iw $SIZE -ih $SIZE -ow 120 -oh 120 -d $DISPLAY -r 0 TPASS"
let PassCount=PassCount+1
else
echo "mxc_v4l2_output -iw $SIZE -ih $SIZE -ow 120 -oh 120 -d $DISPLAY -r 0 TFAIL"
fi
done

echo "SDC output rotation test cases"
for ROT in 0 1 2 3 4 5 6 7; do
${TSTCMD} -iw 352 -ih 288 -ow 240 -oh 320 -d $DISPLAY -r $ROT
if [ $? -eq 0 ];then
echo "mxc_v4l2_output -iw 352 -ih 288 -ow 240 -oh 320 -d $DISPLAY -r $ROT TPASS"
let PassCount=PassCount+1
else
echo "mxc_v4l2_output -iw 352 -ih 288 -ow 240 -oh 320 -d $DISPLAY -r $ROT TFAIL"
fi
done

echo "SDC max input size test case"
${TSTCMD} -iw 480 -ih 640 -ow 240 -oh 320 -d 4 -fr 60
if [ $? -eq 0 ];then
echo "mxc_v4l2_output -iw 480 -ih 640 -ow 240 -oh 320 -d 4 -fr 60 TPASS"
let PassCount=PassCount+1
else 
echo "mxc_v4l2_output -iw 480 -ih 640 -ow 240 -oh 320 -d 4 -fr 60 TFAIL"
fi
${TSTCMD} -iw 720 -ih 512 -ow 240 -oh 184 -d $DISPLAY -fr 60
if [ $? -eq 0 ];then
echo "mxc_v4l2_output -iw 720 -ih 512 -ow 240 -oh 184 -d $DISPLAY -fr 60 TPASS"
let PassCount=PassCount+1
else
echo "mxc_v4l2_output -iw 720 -ih 512 -ow 240 -oh 184 -d $DISPLAY -fr 60 TFAIL"
fi

#V4L2 Capture Tests
TSTCMD="/unit_tests/mxc_v4l2_overlay.out"
auto_prepare.sh -I V4L
trap "cleanup" 0

echo "V4L2 Capture Tests"
${TSTCMD} -iw 640 -ih 480 -ow 240 -oh 320 -r 4 -fr 30 -fg -t 10
if [ $? -eq 0 ];then
echo "mxc_v4l2_overlay -iw 640 -ih 480 -ow 240 -oh 320 -r 4 -fr 30 -fg -t 10 TPASS"
let PassCount=PassCount+1
else
echo "mxc_v4l2_overlay -iw 640 -ih 480 -ow 240 -oh 320 -r 4 -fr 30 -fg -t 10 TFAIL"
fi
${TSTCMD} -iw 640 -ih 480 -ow 240 -oh 320 -r 4 -fr 30 -t 10
if [ $? -eq 0 ];then
echo "mxc_v4l2_overlay -iw 640 -ih 480 -ow 240 -oh 320 -r 4 -fr 30 -t 10 TPASS"
let PassCount=PassCount+1
else
echo "mxc_v4l2_overlay -iw 640 -ih 480 -ow 240 -oh 320 -r 4 -fr 30 -t 10 TFAIL"
fi

for ROT in 0 1 2 3 4 5 6 7; do
${TSTCMD} -iw 640 -ih 480 -ow 240 -oh 184 -r $ROT -fr 30 -fg -t 5
if [ $? -eq 0 ];then
echo "mxc_v4l2_overlay -iw 640 -ih 480 -ow 240 -oh 184 -r $ROT -fr 30 -fg -t 5 TPASS"
let PassCount=PassCount+1
else
echo "mxc_v4l2_overlay -iw 640 -ih 480 -ow 240 -oh 184 -r $ROT -fr 30 -fg -t 5 TFAIL"
fi
done

for POS in 0 4 8 16 32 64 128; do
${TSTCMD} -iw 640 -ih 480 -ot $POS -ol $POS -ow 80 -oh 60 -fr 30 -fg -t 5
if [ $? -eq 0 ];then
echo "mxc_v4l2_overlay -iw 640 -ih 480 -ot $POS -ol $POS -ow 80 -oh 60 -fr 30 -fg -t 5 TPASS"
let PassCount=PassCount+1
else
echo "mxc_v4l2_overlay -iw 640 -ih 480 -ot $POS -ol $POS -ow 80 -oh 60 -fr 30 -fg -t 5 TFAIL"
fi
done
echo "This Test Has 55 testcases"
echo "Pass Num is $PassCount"
if [ $PassCount -eq 55 ];then
echo "========================"
echo "Final Result     PASS"
echo "========================"
else 
echo "========================"
echo "Final Result     FAIL"
echo "========================"
echo "Please check which cases fail in your LOG"
fi
exit 0
