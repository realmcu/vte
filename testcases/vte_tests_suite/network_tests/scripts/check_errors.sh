#!/bin/bash


errors=$(ifconfig eth0 | grep errors:0 | wc -l)
overruns=$(ifconfig eth0 | grep overruns:0 | wc -l)

if [ $errors -eq 2 ] && [ $overruns -eq 2 ]; then
echo PASS
exit 0
fi

echo FAIL
echo "`ifconfig eth0 | grep error`"

exit 255
