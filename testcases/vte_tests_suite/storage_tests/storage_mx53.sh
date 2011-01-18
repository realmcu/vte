#!/bin/bash -x
# Copyright (C) 2011 Freescale Semiconductor, Inc. All Rights Reserved.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License along
# with this program; if not, write to the Free Software Foundation, Inc.,
# 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
#
#    @file   storage_mx53.sh
#
#    @brief  shell script template for "storage".
#
#Revision History:
#                            Modification     Tracking
#Author                          Date          Number    Description of Changes
#Hake                         2011/01/17        NA        init multi-device test
#-------------------------   ------------    ----------  -------------------------------------------
# 

p_node()
{
 RC=1
 tmpfile=$(mktemp -p /tmp)
 if [ $? -ne 0 ]; then
  return $RC
 fi
 cat >$tmpfile <<EOF

n
p
1

+10M
n
p
2


d
1



w
EOF

fdisk $1 < tmpfile
sleep 5

}


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
export TST_TOTAL=1

export TCID="setup"
export TST_COUNT=0
RC=1
trap "cleanup" 0

clear
tmp_dir=$(mktemp -d -p /mnt)
if [ -nz $? ];then
	RC=1
	echo "can not mktemp folder"
	return $RC
fi
#start detect devices
for i in $device_list
do
  #1. find devices node
  d_list=$(cat /proc/partitions | grep -i $i | awk '{print $4}'| grep -vi "${i}[0-9]\+")
  #2. deteminate partition
	for j in $d_list
	do
		p_list=$(cat /proc/partitions | grep -i "${j}.*[0-9]\+" \
		| awk '{print $4}'| grep -i "${i}[0-9]\+")
		for k in $p_list
		do
			#check whether mount
	 		mount | grep $k
	 		if [ $? -ne 0 ]; then
     		echo "not mount then try mount if fail will format to vfat"
				echo "then try mount again, if still fail then quit"
				mount /dev/$k $tmp_dir || mkfs.vfat /dev/$k || break
				sleep 2
				umount $tmp_dir
				sleep 2
				target_list=$target_list" "$k
			fi
      echo "$k mounted now check free space"
			echo "..."
      free_size=$(df /dev/$k | tail -1 | awk '{print $4}')
#if free size is enough then add to target list
			if [ $free_size -gt 100*1024*1024 ];then
        target_list=$target_list" "$k
      else
				 echo "$k free space is not enough for test"
				 echo "skip it!"
			fi
		done
    if [ $(echo $p_list | wc -w) -eq 0 ]; then
#no partition then partition it to 1 partition 
       p_node /dev/$j || break
			 target_list=${j}1
		fi
	done
done

RC=0

#TODO add setup scripts
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

run_single_test_list()
{
   RC=0
   need_umount=1
	 for i in $target_list
	 do
    #test if already mout
    mount | grep $i
		if [ $? -eq 0 ]; then
     #is mounted
		 mount_point=$(mount | grep $i |cut -d" " -f 3)
		 need_umount=0
    else
     #not mount
     mount_point=$(mktemp -d -p /tmp)
		 mount /dev/$i $mount_point || RC=$(echo $RC m$i)
     need_umount=1
		fi
	 	 bonnie\+\+ -d $mount_point -u 0:0 -s 96 -r 100 || RC=$(echo $RC b$i)  
	   dt of=$mount_point/test_file bs=4k limit=96m passes=10 || RC=$(echo $RC d$i)
		 if [ $need_umount -eq 1  ];then
      umount $mount_point || RC=$(echo $RC u$i)
			rm -rf $mount_point
		 fi
	 done
	 return $RC
}

run_multi_test_list()
{
   RC=0
   need_umount=1
	 mkdir -p /tmp/storage
	 for i in $target_list
	 do
    #test if already mout
    mount | grep $i
		if [ $? -eq 0 ]; then
     #is mounted
		 mount_point=$(mount | grep $i |cut -d" " -f 3)
		 need_umount=0
    else
     #not mount
     need_umount=1
     mount_point=$(mktemp -d -p /tmp/storage)
		 mount /dev/$i $mount_point || RC=$(echo $RC $i)
		fi
	 	 sh -c "bonnie\+\+ -d $mount_point -u 0:0 -s 96 -r 100 || RC=$(echo $RC $i)" &  
	   sh -c "dt of=$mount_point/test_file bs=4k limit=96m passes=10 || RC=$(echo $RC $i)" &
	 done
	 echo "wait till all process finished"
	 wait
	 rm -rf /tmp/storage
	 return $RC
}


# Function:     test_case_01
# Description   - Test if single ok
#  
test_case_01()
{
#TODO give TCID 
TCID="test_storage_single"
#TODO give TST_COUNT
TST_COUNT=1
RC=0

#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "

#TODO add function test scripte here


#test list
run_single_test_list

return $RC

}

# Function:     test_case_02
# Description   - Test if multi ok
#  
test_case_02()
{
#TODO give TCID 
TCID="test_storage_multi"
#TODO give TST_COUNT
TST_COUNT=1
RC=0

#print test info
tst_resm TINFO "test $TST_COUNT: $TCID "

#TODO add function test scripte here


#test list
run_multi_test_list

return $RC

}


usage()
{
echo "$0 [case ID]"
echo "1: "
echo "2: "
}

# main function

RC=0

device_list="sd.* mmcblk.*"

target_list=""

#TODO check parameter
if [ $# -ne 1 ]
then
usage
exit 1 
fi

setup || exit $RC

case "$1" in
1)
  test_case_01 || exit $RC 
  ;;
1)
  test_case_02 || exit $RC 
  ;;
*)
  usage
  ;;
esac

tst_resm TINFO "Test PASS"
