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
 #  Simple test to assure governor support for cpu frequency altering is possible.
 #  Assure the files are available that allow you to switch between governors.
###
if [ -d /sys/devices/system/cpu/cpu0/cpufreq ] ; then
	if [ ! -f /sys/devices/system/cpu/cpu0/cpufreq/scaling_available_governors ] ; then
		echo "NA   no added frequencies"
		exit -1;
	fi
	if [ ! -f /sys/devices/system/cpu/cpu0/cpufreq/scaling_governor ] ; then
		echo "FAIL   missing scaling governor file"
		exit -1;
	fi
	echo "PASS      `cat /sys/devices/system/cpu/cpu0/cpufreq/scaling_available_governors `"
	exit  0;
else
	echo "NA   no added frequencies"
	exit  -1;
fi

