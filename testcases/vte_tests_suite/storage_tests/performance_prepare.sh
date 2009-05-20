#!/bin/sh
################################################################################
#
#    @file   performance_prepare.sh
#
#    @brief  this shell script is used to prepare environment for performance
#            test.
#
################################################################################
#
# Copyright 2004-2008 Freescale Semiconductor, Inc. All Rights Reserved.
#
# The code contained herein is licensed under the GNU Lesser General
#
# Public License.  You may obtain a copy of the GNU Lesser General
#
# Public License Version 2.1 or later at the following locations:
#
# http://www.opensource.org/licenses/lgpl-license.html
# http://www.gnu.org/copyleft/lgpl.html
#
################################################################################
#Revision History:
#                            Modification     ClearQuest
#Author                          Date          Number    Description of Changes
#-------------------------   ------------    ----------  -----------------------
#Victor Cui/b19309            18/12/2007      N/A          Initial version
#
################################################################################



# Function:     setup
#
# Description:  - Check if required commands exits
#               - Export global variables
#               - Check if required config files exits
#               - Create temporary files and directories
#
# Return        - zero on success
#               - non zero on failure. return value from commands ($RC)
setup()
{
#TODO Total test case
RC=0
trap "cleanup" 0

#TODO add setup scripts
PATH_PERFORMANCE=/home/performance_test
export PATH_PERFORMANCE

PATH=$PATH:$PATH_PERFORMANCE
export PATH

return $RC
}
# Function:     cleanup
#
# Description   - remove temporary files and directories.
#
# Return        - zero on success
#               - non zero on failure. return value from commands ($RC)
cleanup()
{
RC=0
#TODO add cleanup code here

return $RC
}


################################################################################
# Function:     ***_test
# Description
#
################################################################################
performance_prepare_test()
{
RC=0
#TODO add function test scripte here

#tst_resm TINFO "Test PASS"
#echo "Test PASS"
}

################################################################################
# Function:     usage()
# Description
# give out how to and using example
################################################################################
usage()
{
 echo "usage"
}

# main function

RC=0

setup || exit $RC

#TODO check parameter
cat /etc/rc.d/rc.conf | grep "performance"
if [ $? != "0" ]; then
 sed -i "4s/dropbear/dropbear    performance/" /etc/rc.d/rc.conf
fi

if [ ! -f "/etc/rc.d/init.d/performance" ]; then
 cp performance /etc/rc.d/init.d/
 chmod +x /etc/rc.d/init.d/performance
fi

mkdir /$PATH_PERFORMANCE
cp performance_command_line /$PATH_PERFORMANCE/
cp performance_prepare.sh /$PATH_PERFORMANCE/
cp performance_test.sh /$PATH_PERFORMANCE/
cp auto_prepare.sh /$PATH_PERFORMANCE/

dos2unix /$PATH_PERFORMANCE/*
chmod +x /$PATH_PERFORMANCE/*

#only 1 time for prepare
if [ ! -f "/$PATH_PERFORMANCE/performance_command_line_tmp" ]; then
 cat /$PATH_PERFORMANCE/performance_command_line | sed "s/\//\&/g" > /$PATH_PERFORMANCE/performance_command_line_tmp
fi

reboot
