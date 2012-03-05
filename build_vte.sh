#!/bin/sh
if [ -z $EDITOR ]; then
	EDITOR=nano
	export $EDITOR
fi
$EDITOR $1 
make distclean
./armconfig
. ./$1
make vte
