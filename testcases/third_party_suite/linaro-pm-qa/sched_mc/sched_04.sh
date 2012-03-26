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
#     Daniel Lezcano <daniel.lezcano@linaro.org> (IBM Corporation)
#       - initial API and implementation
#

# URL : https://wiki.linaro.org/WorkingGroups/PowerManagement/Doc/QA/Scripts#sched_mc_04

source ../include/functions.sh

check_change() {
    local val=$1
    local path=$2

    echo $val > $path
}

check_invalid_change() {

    local val=$1
    local path=$2

    echo $val > $path
    if [ "$?" != "0" ]; then
	return 0
    fi

    return 1
}

check_sched_mc_change() {

    local path=$CPU_PATH/sched_mc_power_savings
    local oldval=$(cat $path)

    check "setting value to 0" check_change 0 $path
    check "setting value to 1" check_change 1 $path
    check "setting value to 2" check_change 2 $path
    check "setting invalid value to 3" check_invalid_change 3 $path
    check "setting invalid value to -1" check_invalid_change -1 $path

    echo $oldval > $path
}

if [ $(id -u) != 0 ]; then
    log_skip "run as non-root"
    exit 0
fi

# check_sched_mc_files sched_mc_power_savings || exit 1
check_sched_mc_change
