#!/bin/bash
# I need bash, surely!

#TODO: Add -f <basename_for_out_and_log_file>
#TODO: Add an md5sum to check that the script was not modified

VERSION=1.0.1

set -u
set +e

me=$(basename $(which $0))

function usage
{
    cat <<EOF
$me <BASENAME_FOR_OUT_AND_LOG_FILE> <CR_NUMBER_WITHOUT_TLSBO> <CLEARCASE_LABEL_WITHOUT_CO_VT> 
EOF
    exit 1
}

echo $#

[ $# -lt 3 -o $# -gt 4 ] && usage;

# Used when deliveries from telma does not use this script 
# and the checkconvention failed.
nocheck="no"
[ "$#" == "4" -a "x$1" == "x--no-check" ] && { shift; nocheck=yes; } 


ENGINEER=$(whoami)
TRACE=$1
CR=tlsbo$2
LABEL=CO_VT-$3
DATE=$(LC_ALL=en_US date --universal)

echo "Finding modified files for CR $CR/$LABEL ..."
MODIFIED_FILES=$(cleartool find /vob/vb_sw_linux_ap_vta -version "version($LABEL)" -print -nxn)
# | sed -e 's,/vob/[^/]*/,,'
#MODIFIED_FILES="tmp.c"

[ "x${MODIFIED_FILES}" == "x" ] && { echo "$me: No modified files found!"; exit 1; }

echo "Checking coding rules ..."
for f in ${MODIFIED_FILES};
  do
  if [ ! -d $f -a "$nocheck" != "yes" ];
      then
      vte_check_convention.pl -nospell $f || \
      { echo "Check failed!"; exit 1; }
  fi
done

echo "Enter a short description for this delivery (only one line allowed)"
read DESCRIPTION
[ "x$DESCRIPTION" == "x" ] && { echo "Description empty!"; exit 1; }

ofile=${LABEL}.${CR}
touch $ofile || { echo "Unable to create output file". exit 1; }
cat > $ofile <<EOF
# -*- sh -*-
# $me v$VERSION
LABEL='$LABEL'
DESCRIPTION='$DESCRIPTION'
ENGINEER='$ENGINEER'
DATE='$DATE'
MODIFIED_FILES='\\
EOF
for f in ${MODIFIED_FILES}; do echo "$f \\"; done >> $ofile
cat >> $ofile <<EOF
'
EOF

exit 0

cat >> $ofile <<EOF
LOG='
EOF
# Handle ' ` " \ and eol, we have 3 problems: the shell we are using, we generate a 
# variable assignement, this variable will be echo'd -e
# =>
# double all \
# prepend \ to ` and "
# add \n\
sed -e "s,\\\\,\\\\\\\\\\\\\\\\,g" $TRACE.log | \
sed -e "s,\",\\\\\\\",g" | \
sed -e "s,',\\\',g" | \
sed -e "s,\([()]\),\\\\\1,g" | \
sed -e "s,^\(.*\)$,\1\\\\n,g" >> $ofile
#sed -e "s,\`,\\\\\\\`,g" | \
cat >> $ofile <<EOF
'
OUT="\\
"
EOF
# while read line; do
#     echo -n "$line" | sed -e "s,\`,\\\\\\\\\\\\\`,g";
#     echo "\\\\n\\";
# done < $TRACE.out >> $ofile
# cat >> $ofile <<EOF
# "
# # EOF
# EOF
echo "File $ofile created."
