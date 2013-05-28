#!/bin/sh
mac=$(cat /sys/class/net/eth0/address | sed 's/:/_/g')
#THERMO_PATH="/sys/devices/soc.0/2000000.aips-bus/21bc4e0.thermal"
echo "------------------------------------------------------------"
while true
do
 sleep 30
 echo "<<<MEASUER_START>>>" >> /root/${mac}
 if [ -e /sys/devices/soc.0/2000000.aips-bus/21bc4e0.thermal/temp ]; then
	cat /sys/devices/soc.0/2000000.aips-bus/21bc4e0.thermal/temp >> /root/${mac}
 fi
 top -n 1  | head -n 10 >> /root/${mac}
 date -u >> /root/${mac}
 echo "<<<MEASUER_END>>>" >> /root/${mac}
done

