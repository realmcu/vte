#Copyright 2005-2009 Freescale Semiconductor, Inc. All Rights Reserved.
#
#The code contained herein is licensed under the GNU General Public
#License. You may obtain a copy of the GNU General Public License
#Version 2 or later at the following locations:
#
#http://www.opensource.org/licenses/gpl-license.html
#http://www.gnu.org/copyleft/gpl.html
#!/bin/sh
script="OWIRE TEST"
platform=$(cat /proc/cpuinfo | grep "Hardware" | cut -d' ' -f3-4)
DATE=`date`
MESSAGE="Hello Now the time is : $(date)"
if [ "$platform" == IMX27ADS ]; then
    insmod_test /lib/modules/2.6.18.1/kernel/drivers/w1/wire.ko
    insmod_test /lib/modules/2.6.18.1/kernel/drivers/w1/masters/mxc_w1.ko
    insmod_test /lib/modules/2.6.18.1/kernel/drivers/w1/slaves/w1_ds2433.ko
fi

echo ""
echo "Checking for One wire master"
CHECK_MASTER="/sys/devices/w1_bus_master1"
if [ ! -e $CHECK_MASTER ]; then
    echo "$script: FAIL One wire Master Not Found $CHECK_MASTER"
	echo ""
	exit 
else
    echo "$script: PASS One Wire Master Found $CHECK_MASTER"
	echo ""
fi

echo "Checking for Slave device"
# Each DS2433 EEPROM has a unique id number which starts with 'family code' of
# '23-'
CHECK_SLAVE="/sys/devices/w1_bus_master1/$(cat /sys/devices/w1_bus_master1/w1_master_slaves|grep '[0-9]*')"

if [ ! -e $CHECK_SLAVE ]; then
    echo "$script: FAIL : One wire Slave Not Found"
	echo ""
	exit 
else
    echo "$script: PASS : One Wire Slave Found $CHECK_SLAVE"
	echo ""

	cd $CHECK_SLAVE
    echo "Wrting \"$MESSAGE\" to eeprom"
	echo ""
    echo "$MESSAGE" > eeprom
    echo "Reading EEPROM"
	echo ""
    read_back="$(cat eeprom | grep "$MESSAGE")"
    echo "Value got from eeprom ..." 
	echo "$read_back"
	echo "" 
    if [ "$read_back" == "$MESSAGE" ]; then
        echo "$script: PASS EEPROM Read Correctly"
		echo "" 
    else
        echo "String read from eeprom was:"
        echo "$read_back"
        echo "$script: FAIL To Read EEPROM"
		exit 
    fi
fi
