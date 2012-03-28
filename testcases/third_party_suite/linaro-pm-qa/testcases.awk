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
 # For a series of power management related test, cycle through and 
 # execute. Each test to be executed is in file testcases  where the 
 # following columns make sense....
 # column 1:  name of the test case
 # column 2:  sub directory housing the test
 # column 3:  name of file for the test case
 # column 4:  from column 4 onwards, any arguments to pass on to the test
###
{
	printf $1 ":    "
	cmd = "cd  ./testcases/"$2   " ;  sudo ./"$3   " " substr($0, length($1 $2 $3) +4)
	system(cmd)
	printf "\n"
}

