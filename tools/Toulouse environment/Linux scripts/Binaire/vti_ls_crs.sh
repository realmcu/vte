#!/bin/sh

set -u
set +e
#set -x

me=${0##.*/}

VTE_USER_CRS=$(cd ${VTE_USER_CR_DIR} && ls tlsbo*.* 2>/dev/null | grep -v '~')
VTE_USER_CRS_DONE=$(cd ${VTE_USER_CR_DIR} && ls tlsbo*.*~DONE 2>/dev/null )
    
echo "CR found in ${VTE_USER_CR_DIR}:"
for c in ${VTE_USER_CRS}; do echo "- $c"; done
echo ""
echo "CR DONE found in ${VTE_USER_CR_DIR}:"
for c in ${VTE_USER_CRS_DONE}; do echo "- $c"; done
echo ""

exit 0
