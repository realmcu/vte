#!/bin/ash

gcov_result_dir="$LTPROOT/results.cov"
gcov_dir="/proc/gcov"
# Tar all, included vmlinux
gcov_subdirs="."

here=`pwd`

# Copy any revelant informations
echo "Cleaning $gcov_result_dir/$1..."
rm -Rf $gcov_result_dir/$1.gcov.tar

echo "Archiving captured data ..."
cd /proc/gcov && tar -cf $gcov_result_dir/$1.gcov.tar `find $gcov_subdirs -type f`
cd $here

#unload module => ?
# on peut faire un reset complet avec echo "0" > /proc/vmlinux
echo "Unloading gcov module(s)..."
rmmod gcov_proc
