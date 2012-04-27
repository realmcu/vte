#!/bin/sh
if [ $# -ne 1 ]; then
echo "need number of process"
echo "$0 1/2/3/4/"
fi

while [ true ];
do
if [ $1 -eq 4  ]; then
dry2 &
fi
if [ $1 -gt 2  ]; then
dry2 &
fi
if [ $1 -gt 1  ]; then
dry2 &
fi
dry2
done

