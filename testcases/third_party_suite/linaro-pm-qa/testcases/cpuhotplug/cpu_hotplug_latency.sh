#!/bin/sh
#
# PM-QA validation test suite for the power management on Linux
#
# Copyright (C) 2011, Linaro Limited.
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
#
# Contributors:
#     Torez Smith <torez.smith@linaro.org> (IBM Corporation)
#       - initial API and implementation
#

PWD=pwd

echo 0 > /sys/kernel/debug/tracing/tracing_on

echo 1 > /sys/kernel/debug/tracing/events/cpu_hotplug/enable

# echo "**** TEST LOADED CPU*****"

LOOP=10
MAX_PROCESS=$LOOP

while ( test $LOOP -ge 0 ); do

	PROCESS=$LOOP

	while ( test $PROCESS -lt $MAX_PROCESS ); do
		# echo "**** Start cyclictest *****"
		cyclictest -D 180 -t 10 -q  > /dev/null &
		PROCESS=$(( $PROCESS + 1 )) 
	done

	ILOOP=20
	echo 1 > /sys/kernel/debug/tracing/tracing_on

	while ( test $ILOOP -ge 1 ); do 

		# echo "**** TEST " $ILOOP " *****"
		# echo ""

		sleep 1

#		echo -n "Disabling CPU1 ... "
		echo 0 > /sys/devices/system/cpu/cpu1/online
		# echo "Disable"

		sleep 1

#		echo -n "Enabling CPU1 ... "
		echo 1 > /sys/devices/system/cpu/cpu1/online
		# echo "Enable"

		sleep 1

		ILOOP=$(( $ILOOP - 1 )) 

	done

	echo 0 > /sys/kernel/debug/tracing/tracing_on

	LIST=`ps | grep cyclictest | awk '{ print $1 }'`

	for I in $LIST; do
		# echo "**** Kill cyclictest *****"
		kill $I
	done

	LOOP=$(( $LOOP - 1 )) 

done

cat /sys/kernel/debug/tracing/trace > $PWD/test_load.txt

echo "PASS"
