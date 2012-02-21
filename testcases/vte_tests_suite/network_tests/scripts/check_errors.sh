#!/bin/bash


errors=$(ethtool -S eth0 | grep errors | wc -l)
overruns=$(ifconfig eth0 | grep overruns:0 | wc -l)

if [ $errors -eq 9 ] && [ $overruns -eq 2 ]; then
echo PASS
exit 0
fi

echo FAIL
echo error: $errors
echo overruns: $overruns

exit 255
