#!/bin/sh
mac=$(cat /sys/class/net/eth0/address | sed 's/:/_/g')
echo "------------------------------------------------------------"
while true
do
 sleep 30
 if [ -e /sys/devices/virtual/thermal/thermal_zone0/temp ]; then
	cat /sys/devices/virtual/thermal/thermal_zone0/temp >> /root/${mac}
 fi
done

