#!/bin/sh
echo "If these cases can work,TPASS"
sleep 1
echo "Set TSLIB_TSDEVICE value"
#export TSLIB_TSDEVICE=/dev/input/ts0
export TSLIB_TSDEVICE=/dev/input/event1
if [ $1 -eq 0 ]
then
echo "Basic Test by ts_test"
echo "ctrl+C to quit"
sleep 1
ts_test
elif [ $1 -eq 1 ]
then
echo "Calibrating Test by ts_calibrate"
sleep 1
ts_calibrate
elif [ $1 -eq 2 ]
then
echo "Print touchscreen events by ts_print"
echo "touch with a pen,ctrl+C to quit"
sleep 1
ts_print
elif [ $1 -eq 3 ]
then
echo "Read raw pressure, x, y, and timestamp from a touchscreen device by ts_print_raw"
echo "touch with a pen,ctrl+C to quit"
sleep 1
ts_print_raw
elif [ $1 -eq 4 ]
then
echo "Harvest hundreds of raw touchscreen coordinates by ts_harvest"
echo "Touch cross points with a pen,ctrl+C to quit"
echo "Test will create ts_harvest.out to record data,you can check this LOG after test"
sleep 1
ts_harvest
else
echo "TestNum parameter error!"
echo "\$1=0~4"
echo "0-ts_test;1-ts_calibrate;2-ts_print;3-ts_print_raw;4-ts_harvest"
fi
exit 0
