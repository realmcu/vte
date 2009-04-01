#!/bin/sh
##############################################################################
#
#  Copyright 2004-2008 Freescale Semiconductor, Inc. All Rights Reserved.
#
##############################################################################
#
#  The code contained herein is licensed under the GNU Lesser General Public
#  License.  You may obtain a copy of the GNU Lesser General Public License
#  Version 2.1 or later at the following locations:
#
#  http://www.opensource.org/licenses/lgpl-license.html
#  http://www.gnu.org/copyleft/lgpl.html
#
##############################################################################
#
# Revision History:
#                       Modification     Tracking
# Author                    Date          Number    Description of Changes
#--------------------   ------------    ----------  ---------------------
# Spring Zhang           14/10/2008       n/a       Initial ver.
# Spring                 28/11/2008       n/a       Add COPYRIGHT header
#############################################################################
# Func: Used to support resuming running feature.

usage()
{
    cat <<-EOF
    Use the command to add # before the lines which have been tested.
    So they will not test again.
    usage: ./${0##*/} [cmd list file] [previous result file]
    e.g.: ./${0##*/} mx51_3stack_full_a mx51_auto_test.txt
EOF
}

# main{}

if [ $# -lt 2 ]
then
    usage
    exit 1
fi

cmdlist_file=$1
origin_result=$2

sed -n '/PASS/p' $origin_result >  tested_cases
sed -n '/FAIL/p' $origin_result >> tested_cases

cp -f tested_cases tmp.txt

awk '{print $1}' tmp.txt > tested_cases
rm -f tmp.txt

#comment(by #) cmdlist file by result
for i in `cat tested_cases`
do
  sed "s/^\($i\)/#\1/" $cmdlist_file > tmp_cmdlist
  cp tmp_cmdlist $cmdlist_file 
done 

rm -f tmp_cmdlist

exit
