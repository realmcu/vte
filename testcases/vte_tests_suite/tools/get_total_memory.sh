#!/bin/bash

total_k=$(free | grep Mem | awk '{print $2}')

if [ $# == 1  ]; then
	if [ $1 = '-m' ];then
		total=$(echo $total_k/1024+1 | bc )  
	else
		total=$total_k
	fi
else
total=$total_k
fi

echo $total
