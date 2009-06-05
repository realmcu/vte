#!/bin/bash


#cd /pjt/linux_baseport/VTE/runtest

#usage: ./set_cmdfiles.sh ./runtest L26_1_9-SCMA11EVB

cd $1
dos2unix vt_*

for f in *.txt;
do 
  sed -ne 's/^TGE-LV-[[:alpha:]]*-[[:digit:]]* \([[:alnum:]_-]*\).*$/#files: \1/gp' -ne 's/^\(#files:.*\)$/\1/gp' $f |sort| uniq > $f.tmp;
  cat $f >> $f.tmp;
  mv $f.tmp $f;
  mv $f ${f/.txt/};
done

for f in *_?; do mv $f `echo $f | sed -e 's/^\(.*_\)\(.\)$/\10\2/'`; done

chmod a+rw-x vt_*

mv runtest ${2}_runtest

