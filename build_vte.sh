#!/bin/sh

usage()
{
  echo "how to run"
  echo "build_vte.sh <platform config file>"
  echo "current available platcorm config file"
  echo "mx6x_evk_config"
}

if [ $# -lt 1 ];then
 usage
 exit 1
fi


if [ -z $EDITOR ]; then
	EDITOR=nano
	export $EDITOR
fi
$EDITOR $1 
make distclean
./armconfig
. ./$1
make vte
