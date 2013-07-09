#!/bin/bash

saved_path=$PWD

if ! mount|grep -sq '/sys/kernel/debug'; then
	mount -t debugfs none /sys/kernel/debug
fi

printf "%-24s %-20s %3s %9s\n" "clock" "parent" "use" "flags" "rate"

for foo in $(find /sys/kernel/debug/clk -type d); do
    if [ "$foo" = '/sys/kernel/debug/clk' ]; then
        continue
    fi
                
    # if directory not exists, and no usecount, rate and flags, will skip
    # for 3.5.7, it's a temp version
    # TODO: replace it as a formal one
    if [ ! -e "$foo" ]; then
        continue
    fi
    cd $foo
                    
    if [ ! -e clk_enable_count ] || [ ! -e clk_rate ] || [ ! -e clk_flags ]; then
        continue
    fi
    ec="$(cat clk_enable_count)"
    rate="$(cat clk_rate)"
    flag="$(cat clk_flags)"
        
    clk="$(basename $foo)"
    cd ..
    parent="$(basename $PWD)"
                                        
    if [ "$parent" = 'clock' ]; then
	parent="   ---"
    fi

    printf "%-24s %-24s %2d %2d %10d\n" "$clk" "$parent" "$ec" "$flag" "$rate"
    cd $saved_path
done
                                                                
