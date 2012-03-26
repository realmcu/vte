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

all:
	@(cd utils; $(MAKE))
	@(cd testcases; $(MAKE) all)

check:
	@(cd utils; $(MAKE) check)
	@(cd cpufreq; $(MAKE) check)
	@(cd cpuhotplug; $(MAKE) check)
	@(cd cpuidle; $(MAKE) check)
	@(cd sched_mc; $(MAKE) check)

uncheck:
	@(cd cpufreq; $(MAKE) uncheck)
	@(cd cpuhotplug; $(MAKE) uncheck)
	@(cd cpuidle; $(MAKE) uncheck)
	@(cd sched_mc; $(MAKE) uncheck)

recheck: uncheck check

install:
	    cp -rf ./ ../../bin/linaro-pm-qa

clean:
	@(cd utils; $(MAKE) clean)
	@(cd testcases; $(MAKE) clean)

