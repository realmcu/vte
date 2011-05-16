#!/bin/bash -x
#Copyright (C) 2005-2010 Freescale Semiconductor, Inc. All Rights Reserved.
#
#The code contained herein is licensed under the GNU General Public
#License. You may obtain a copy of the GNU General Public License
#Version 2 or later at the following locations:
#http://www.opensource.org/licenses/gpl-license.html
#http://www.gnu.org/copyleft/gpl.html
# $1 install / remove
# $2 path to debian package folder

helpme()
{
 echo "$1 [install/remove] [path to debian package]"
 echo "or"
 echo "sudo ROOTFS=$ROOTFS $1 [install/remove] [path to debian package]"
}


install_debian_package()
{
 RC=0
 list=$(ls $1/*.deb)
 for i in $list
 do
	dpkg --force-architecture --root $ROOTFS -i $i
 	ipkg=$(echo $i | cut -d _ -f 1)
 	iver=$(echo $i | cut -d _ -f 2 | cut -d - -f 1)
 	iiver=$(dpkg --list || grep $ipkg || awk '{print $3}')
 	if [ ! $iiver = $iver ]; then
  	RC=$(echo $RC $i)
 	fi
 done
 echo $RC
 if [ "$RC" = 0 ]; then
	return 0
 else
	 return 1
 fi
}

remove_debian_package()
{
 list=$(ls $1/*.deb)
 for i in $list
 do
 dpkg --force-architecture --root $ROOTFS -r $(basename $i | cut -f 1 -d _)
 done
 return 0
}

if [ $# -ne 2 ];then
  helpme
  exit 1
fi

if [ -z $ROOTFS ]; then
ROOTFS=/
fi

if [ $1 = "install" ];then
  echo $2
  install_debian_package $2 || exit 1
  exit 0
fi

if [ $1 = "remove" ]; then
  remove_debian_package $2 || exit 1
  exit 0
fi


