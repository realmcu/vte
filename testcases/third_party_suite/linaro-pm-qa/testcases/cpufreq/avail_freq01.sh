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

# Description : We are checking the frequency scaling file system is
#  available. This test checks we have the framework to run the other
#  tests in order to test the ability to monitor and/or alter cpu
#  frequency on the board, let's assure the basic files are there.

CPU_PATH="/sys/devices/system/cpu"

check_freq() {
    if [ ! -f $CPU_PATH/$1/cpufreq/scaling_available_frequencies ] ; then
	echo "NA no added frequencies"
	return 1;
    fi

    if [ ! -f $CPU_PATH/$1/cpufreq/scaling_cur_freq ] ; then
	echo "missing current frequency file"
	return 1;
    fi

    if [ ! -f $CPU_PATH/$1/cpufreq/scaling_setspeed ] ; then
	echo "missing file to set frequency speed"
	return 1;
    fi

    echo "PASS `cat $CPU_PATH/$1/cpufreq/scaling_available_frequencies `"
    return  0;
}

for i in $(ls $CPU_PATH | grep "cpu[0-9].*"); do
    check_freq $i
done

