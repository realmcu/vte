#!/bin/bash

set -e
set -u

prog="vte-lcov.sh"
version=1.0

infodir=$PWD
datadir=$PWD

declare -i verbose=1

usage()
{
    echo "
${prog} ${version} read lcov capture directory(ies) or tar'ed archive(s) capture directory, 
generate data file(s) (.info) for it/them. 

Usage: 

    ${0##*/} [-v|-vv] [-q] [-d <DATADIR>] [-o <INFODIR>] -S <SRCTREE> [-B <OBJTREE>] <INPUT>|<INPUT.GCOV.TAR>...
    
    Where:
      -v           : Be more verbose, print action on stdout.
      -vv          : Be more verbose, print commands on stdout.
      -q           : Be more quiet, print nothing on stdout.
      --debug      : set -x
      -o <INFODIR> : Directory where to put .info file(s) (default is \$PWD)
      -d <DATADIR> : Directory where to untar <INPUT.GCOV.TAR> (default is \$PWD)
      -S <SRCTREE> : Linux source tree
      -B <OBJTREE> : Linux build tree (if != <SRCTREE>)
      <INPUT>...   : list of capture directory or capture TAR file. 

"
    exit
}

action()
{
    if [ $verbose -gt 0 ];
	then
        echo "[$prog] $*" 2>&1 | tee -a $log
    fi
}

command()
{
    if [ $verbose -gt 1 ];
	then
        echo "  $*" | tee -a $log
    fi
    if [ $verbose -gt 2 ];
	then
        ($*) 2>&1 | tee -a $log
    else
        ($*) >> $log 2>&1
    fi
}

here=$PWD
log=`mktemp /tmp/$prog-log.XXXXXX`

input=""
while [ $# -ne 0 ];
  do
  case $1 in
      -S)
          srctree=$2
          [ ! -e $srctree ] && {
              echo "SRCTREE doesn't exist ($srctree)"
              exit 1;
          }
          shift; shift;
          ;;
      -B)
          objtree=$2
          [ ! -e $objtree ] && {
              echo "OBJTREE doesn't exist ($objtree)"
              exit 1;
          }
          shift; shift;
          ;;
      -o)
          infodir=$2
          shift; shift
          ;;
      -d)
          datadir=$2
          shift; shift
          ;;
      -q)
          verbose=$(($verbose-1))
          shift;
          ;;
      -v)
          verbose=$(($verbose+1))
          shift
          ;;
      -vv)
          verbose=$(($verbose+2))
          shift
          ;;
      --debug)
          set -x
          shift
          ;;
      *)
          ls -- $1 2>&1 > /dev/null 
          if [ ! $? ]
              then
              echo "\"$1\": no such file or directory"
              usage
          fi
          input="$input $1"
          shift
          ;;
  esac
done

if [ x"$input" == "x" ];
    then
    usage
fi

if [ x"$srctree" == "x" ];
    then
    usage
fi

if [ x"$objtree" == "x" ];
    then
    objtree=$srctree
fi

action "Logging commands to \"$log\"."

rm -f $log
touch $log

#
# If <INPUT.TAR.GZ>, untar it in datadir/<INPUT>
#
# busybox's tar (ustar) has a path length limitation, so remove all links
# they will be recreated later
oldinput=$input
input=""
for i in $oldinput;
  do
  dir=`dirname $i`
  file=`basename $i`
  base=${file%%.*}
  ext=${file#*.}
  if [ x$ext == "xgcov.tar" -o x$ext == "xGCOV.TAR" ];
      then
      action "Untaring $i..."
      [ -e $datadir/$base ] && command "rm -Rf $datadir/$base"
      command "mkdir $datadir/$base"
      command "/bin/tar --no-same-owner -xvf $i -C $datadir/$base"
      command "chmod -R ug+rw $datadir/$base"
      action "Fixing symlinks problem with busybox's tar"
      # remove potentially gcno broken link
      for f in `cd $datadir/$base && find . -type l -a -name '*.gcno'`;
      do
        rm $datadir/$base/$f
      done
      # remove potentially source broken link
      for f in `cd $datadir/$base && find . -type l -a -name '*.[ch]'`;
      do
        rm $datadir/$base/$f
      done
      # recreate gcno and source links
      for f in `cd $datadir/$base && find . -name '*.gcda'`;
      do
        ln -s $objtree/${f//.gcda/.gcno} $datadir/$base/${f//.gcda/.gcno}
        src=${f//.gcda/.c}
        if [ -e $objtree/$src ]; 
            then 
            ln -s $objtree/$src $datadir/$base/$src
        elif [ -e $srctree/$src ];
            then
            ln -s $srctree/$src $datadir/$base/$src;
        fi
      done
      input="$input $datadir/$base"
  else
      input="$input `cd $dir && echo $PWD`/$base"
  fi
done

for i in $input;
  do
  file=`basename $i`
  base=${file%%.*}
  title=${base//-/_}
  action "Generating $title.info..."
  [ -e $infodir/$title.info ] && command "rm -f $infodir/$title.info"
  command lcov -c -t $title -d $i --no-checksum -o $infodir/$title.info
done

exit
