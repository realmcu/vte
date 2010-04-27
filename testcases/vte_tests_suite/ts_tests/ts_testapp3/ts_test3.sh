#Copyright (C) 2004-2009 Freescale Semiconductor, Inc. All Rights Reserved.
#
#The code contained herein is licensed under the GNU General Public
#License. You may obtain a copy of the GNU General Public License
#Version 2 or later at the following locations:
#
#http://www.opensource.org/licenses/gpl-license.html
#http://www.gnu.org/copyleft/gpl.html
#!/bin/sh
#
echo "Set TSLIB_TSDEVICE value"
export TSLIB_TSDEVICE=/dev/input/ts0
if [ $1 -eq 0 ]
then
echo "Basic Test by ts_test"
echo "ctrl+C to quit"
echo "Draw: draw a picture to see if the picture is the vision you wanted."
echo "Drag: drag pen on the screen and check crosshair following pen's trace"
echo "If OK, case is PASS, otherwise, FAIL."
sleep 1
ts_test
elif [ $1 -eq 1 ]
then
echo "Calibrating Test by ts_calibrate"
echo "Please calibrate the crosshair on the screen"
echo "to see if the process can go with our error"
echo "if No Error, case is PASS, otherwise, FAIL."
sleep 1
ts_calibrate
elif [ $1 -eq 2 ]
then
echo "Print touchscreen events by ts_print"
echo "touch with pen in a few different positions,ctrl+C to quit"
echo "this program will print x, y and press state for each touch"
echo "try to change you touch point and judge:"
echo "1. the digits should be possitive,"
echo "   and changed as pen moves within valid range"
echo "2. when pen press on the screen the press state is always 1."
echo "3. when leave you pen the press state is 0."
echo "4. when you does touch the screen, no message prompted"
echo "if the four judgements PASS, then case is PASS, otherwise FAIL."
sleep 1
ts_print
elif [ $1 -eq 3 ]
then
echo "Read raw pressure, x, y, and timestamp from a touchscreen device by ts_print_raw"
echo "touch with a pen,ctrl+C to quit"
echo "this program will print raw x, y and press state for each touch"
echo "1. the digits should be possitive, "
echo "and changed as pen movesi within valid range."
echo "2. when pen press on the screen the press state is always 1."
echo "3. when leave you pen the press state is 0."
echo "4. when you do not touch the screen, no message prompted"
echo "if the four judgements PASS, then case is PASS, otherwise FAIL."
sleep 1
ts_print_raw
elif [ $1 -eq 4 ]
then
echo "Harvest hundreds of raw touchscreen coordinates by ts_harvest"
echo "Touch cross points with a pen,ctrl+C to quit"
echo "Test will create ts_harvest.out to record data,you can check this LOG after test"
echo "this program will go through the screen"
echo "1. They are samples reported when you touch the screen."
echo "2. After exist please check the ts_harvest.out which should have raw dates."
echo "3. The raw date is comply to test 3."
echo "4. when you do not touch the screen, no message prompted"
echo "if the four judgements PASS, then case is PASS, otherwise FAIL."
sleep 1
ts_harvest
else
echo "TestNum parameter error!"
echo "\$1=0~4"
echo "0-ts_test;1-ts_calibrate;2-ts_print;3-ts_print_raw;4-ts_harvest"
fi
exit 0
