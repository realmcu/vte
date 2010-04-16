#!/bin/bash -x
# $1 install / remove
# $2 path to debian package folder

helpme()
{
 echo "$1 [install/remove] [path to debian package]"
}



install_debian_package()
{
 list=$(ls $1/*.deb)
 for i in $list
 do
 dpkg -i $i
 done
 return 0
}

remove_debian_package()
{
 list=$(ls $1/*.deb)
 for i in $list
 do
 dpkg -r $(basename $i | cut -f 1 -d _)
 done
 return 0
}

if [ $# -ne 2 ];then
  helpme
  exit 1
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


