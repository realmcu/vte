#Copyright (C) 2005-2010 Freescale Semiconductor, Inc. All Rights Reserved.
#
#####The code contained herein is licensed under the GNU General Public
#License. You may obtain a copy of the GNU General Public License
#Version 2 or later at the following locations:
#http://www.opensource.org/licenses/gpl-license.html
#http://www.gnu.org/copyleft/gpl.html

SUBDIRS-y += usb_pm
SUBDIRS-y += pwr_consumption
SUBDIRS-y += pwr_battery
SUBDIRS-y += pwr_clocks
scripts = powerstate_test.sh power_test_storage.sh power_test_alsa.sh \
		  power_test_vpu.sh power_test_wifi.sh power_test_v4l.sh \
		  power_test_gpu.sh power_test_enet.sh

all:
	@set -e; for i in $(SUBDIRS-y); do $(MAKE) -C $$i ; done

install:
	@set -e; for i in $(SUBDIRS-y); do $(MAKE) -C $$i install ; done
	-ln -f $(scripts) ../../bin/

clean:
	@set -e; for i in $(SUBDIRS-y) ; do $(MAKE) -C $$i clean ; done

