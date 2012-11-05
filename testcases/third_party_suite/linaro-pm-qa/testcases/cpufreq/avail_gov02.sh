#!/bin/bash
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

###
 #  For each available governor, can we change the cpu governor to
 #  that new value?  Sleep a few seconds after changing to allow system
 #  to settle down.
###

for i in `cat /sys/devices/system/cpu/cpu0/cpufreq/scaling_available_governors`
do
	echo $i >  /sys/devices/system/cpu/cpu0/cpufreq/scaling_governor
	if [ $? != 0 ]; then 
		echo "FAIL   can not write governor for $i"
		exit -1
	fi
	sleep 5
	if [ `cat /sys/devices/system/cpu/cpu0/cpufreq/scaling_governor` != $i ]; then
		echo "FAIL   could not change governor to $i"
		exit -1
	fi
done

echo PASS


