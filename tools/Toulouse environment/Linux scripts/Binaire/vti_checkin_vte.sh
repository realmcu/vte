#!/bin/sh

set -u
set +e

mfiles=$(cleartool lsco -s -avobs -brtype "$1")
info "Checking in files..." 
#echo $mfiles
cleartool ci -nc ${mfiles}
