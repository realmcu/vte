#!/bin/ash

# debug
#set -x

VERSION="1.0-pre4"

LTPROOT=`cd \`dirname $0\` && echo $PWD`

# This one is updated automaticaly depending on laramade environement
FTPDIR=L26_1_10-SCMA11EVB-with_qt

cd $LTPROOT

#TODO: 
# change --fetch to --just-fetch
# add --no-fetch
# add --no-clean
usage()
{
    cat <<EOF
Usage: 
${0##*/} [-C|--coverage] [-M|--manual] [-o <BASEDIR>] <CMDFILE(S)>
    -C|--coverage: Use coverage feature.
    -M|--manual: run only manual tests.
    -o <BASEDIR>: Prefix where to put the results and results.cov directories (default is $LTPROOT)
    <CMDFILE(S)>: list of command file(s) (runtest) to run.
${0##*/} --fetch <CMDFILE(S)>
    When using FTP method, just fetch binaries needed by CMDFILE(S)
EOF
    exit
}

info()
{
    # blue
    echo -e "\033[0;37;44m[INFO]\033[0m " $*
}

info_nolf()
{
    # blue
    echo -ne "\033[0;37;44m[INFO]\033[0m " $*
}

error()
{
    # red
    echo -e "\033[0;37;41m[ERROR]\033[0m " $*
}

warning()
{
    # purple
    echo -e "\033[0;37;45[WARNING]\033[0m " $* 
}

debug()
{
    # purple
    echo -e "\033[0;37;45[WARNING]\033[0m " $* 
}

##
## Some preliminary sanity checks
##

if [ ! -d pan -o ! -d runtest -o ! -d testcases/bin -o ! -f runltp -o \
     ! -f IDcheck.sh -o ! -f ver_linux -o ! -f ${0##*/} ];
then
    error "\"$LTPROOT\" (here) doesn't seems to be a good LTPROOT directory!"
    cat <<EOF
One of the folowing file/directory is missing:
pan/
runtest/
testcases/
testcases/bin/
runltp
IDcheck.sh
ver_linux
EOF
    exit 1;
fi


if [ x$VTE_NFS != "xyes" -a x$VTE_TFP != "xyes" -a x$VTE_TTY != "xyes" ];
then
    error "${0##*/} needs to know which transfert method you want to use"
    cat <<EOF
For this you have to set one of the folowing environment variable to "yes":
VTE_NFS: When VTE is used with an NFS mount point.
VTE_TFP: When VTE is used with FTP transfert.
VTE_TTY: When VTE is used with serial console transfert.
Example:
me@here> export VTE_NFS=yes
me@here> ./${0##*/} vt_<WHAT_YOU_WANT>
EOF
    exit 1
fi


#set -e

#
# FTP helpers
#
ftp_get_runtest()
{
    files=`sed -ne 's/^#files:\(.*\)$/\1/gp' $1`
    [ x"$files" = "x" ] && return
    info "Fetching files for `basename $1`..."
    for f in $files;
      do
      echo "         -> $f"
      if [ `dirname $f` != "." ];
          then
          dir=`dirname $f`
          mkdir -p testcases/bin/$dir
      else
          dir=""
      fi
      ftp $VTE_FTPSERVER > /dev/null <<EOF
lcd testcases/bin/$dir
cd $FTPDIR/testcases/$dir
get `basename $f`
EOF
      if [ $? -eq 0 ];
          then 
          chmod a+rwx testcases/bin/$f 
      else
          return $?
      fi
    done
    return 0
}

ftp_runtest_clean()
{
    files=`sed -ne 's/^#files:\(.*\)$/\1/gp' $1`
    info "Cleaning away testcases/bin for `basename $1`..."
    for f in $files;
    do
      rm -f testcases/bin/$f
    done
}

# $1 subdir for server
# $2... files
ftp_send_files()
{
    if [ x$VTE_FTP = "xyes" ];
        then
        info "Sending files in $1..."
        dir=$1; shift
        for f in $*;
          do
          echo "         -> $f"
          ftp $VTE_FTPSERVER > /dev/null <<EOF
cd $FTPDIR/$dir
lcd `dirname $f`
put `basename $f`
EOF
        done
    elif [ x$VTE_NFS != "xyes" ];
        then
        shift
        info "Sending $*..."
        sz $* > /dev/null
    fi
}

oom_dont_kill_them()
{
    
    [ ! -e /proc/sys/kernel/oom ] && return;
    files=`sed -ne 's/^#files:\(.*\)$/\1/gp' $1`
    [ x"$files" = "x" ] && return
    info "Adding `basename $1` to the oom don't kill list..."
    for f in $files;
      do
      echo "         -> $f"
      echo "$f" > /proc/sys/kernel/oom/dontkill
    done
    return 0
}

flush_oom_dont_kill()
{
    [ ! -e /proc/sys/kernel/oom ] && return;
    echo "erase" > /proc/sys/kernel/oom/dontkill
}

# vte default config
tmpdir="/tmp"
result_dir="$PWD/results"
cov_dir="$PWD/results.cov"
just_fetch=no
just_clean=no

input=""
coverage="no"
manual="no"
fetch="yes"
clean="yes"
while [ $# -ne 0 ];
  do
  case $1 in
      -C|--coverage)
          coverage="yes"
          shift
          ;;
      -M|--manual)
          manual="yes"
          shift
          ;;
      --fetch)
          just_fetch="yes";
          shift;
          ;;
      --no-fetch)
          fetch="no";
          shift;
          ;;
      --clean)
          just_clean="yes";
          ;;
      --no-clean)
          clean="no";
          shift;
          ;;
      --keep)
          clean="no";
          fetch="no";
          shift;
          ;;
      -o)
          result_dir=$2/results
          cov_dir=$2/results.cov
          if [ ! -d $2 ];
              then
              mkdir $2
          fi
          shift;
          shift;
          ;;
      -*)
          error "Unknow option $1"
          usage
          ;;
      *)
          ls -- runtest/$1 > /dev/null 2>&1 
          if [ ! $? ]
              then
              error "\"runtest/$1\": no such file or directory"
              usage
          fi
          input="$input $1"
          shift
          ;;
  esac
done


if [ "x$input" = "x" ];
    then
    error "No command file!"
    usage
fi

dmesg -n 1

export PATH=$PATH:$PWD

#
# Here we go!
# process each command file
#
for t in $input;
  do

  log=$result_dir/$t.log
  out=$result_dir/$t.out
  cmdfile=$PWD/runtest/$t

  # Check for commanf file
  if [ ! -f $cmdfile ];
      then
      error "Command file \"$cmdfile\" not found"
      exit 1;
  fi

  # Print a banner
  echo -e "\033[0;33;40m
*******************************************************************************
* Runtest = \033[0;35;40m $t \033[0;33;40m
*******************************************************************************
*     log = \033[0;35;40m $log \033[0;33;40m
*     out = \033[0;35;40m $out \033[0;33;40m
*     cmd = \033[0;35;40m $cmdfile \033[0;33;40m
*******************************************************************************
\033[0m"

  # Best place to break campaign
  info_nolf "Giving you 3s to kill me (CTRL-C):"
  for i in 1 2 3; 
    do
    echo -en ".\a";
    sleep 1;
  done
  echo -e "."

  # some clean up
  rm -Rf $log $out
  rm -Rf /tmp/$t

  # If just clean, then clean and process next command file
  [ "x$VTE_METHOD" = "xFTP" -a "x$just_clean" = "xyes" ] && {
      ftp_runtest_clean $cmdfile
      continue
  }

  # Get the binaries by ftp if needed
  [ "x$VTE_METHOD" = "xFTP" -a "x$fetch" = "xyes" ] && {
      mkdir -p testcases/bin
      ftp_get_runtest $cmdfile
      # just fetch?
      [ "x$just_fetch" = "xyes" ] && continue
  }

  # Adapt command file for manual tests:
  # Change line beginning with "#TGE-LV-" by "TGE-LV-...-manu"
  # Add an stdin 
  # delete lines beginning with "TGE-LV"
  if [ "x$manual" = "xyes" ];
      then
      pid=$$
      info "Converting command file for manual testing..."
      cat $cmdfile | grep -v '^TGE' | \
          sed -e 's/^#\(TGE[^[:blank:]]*\)/\1-manu/g' \
          > $cmdfile.manu
      cmdfile=$cmdfile.manu
  fi

  # Adapt command file for lcov:
  # add calls to lcov-reset.sh and lcov-capture.sh
  if [ "x$coverage" = "xyes" ];
      then
      info "Converting command file for coverage measurement..."
      mkdir -p $cov_dir
      cat $cmdfile | lcovize-runtest.sed > /tmp/$t;
      cmdfile=/tmp/$t 
      [ $? -ne 0 ] && {
          error "Encourt error while converting $t, skipping."
          continue;
      }
  fi

  #
  # On MVista linux, the OOM killer is active, add process to its "don't kill" list
  #
  oom_dont_kill_them $cmdfile;

  #
  # launch runtest
  #
  info "Running $t ..."
  if [ "x$manual" = "xyes" ];
      then
      info ""
      info "Manual tests: $cmdfile"
      info ""
      info "(press enter to continue)"
      read a
      # "Manual" run
      log=$log.tmp
      out=$out.tmp
      rm -f $log $out
      # can't use runltp/pan when user interaction is needed
      # since test app loose stdin, so do-it-yourself!
      # It's a little ugly, i know! ;o)
      stdin=/proc/$$/fd/0
      cat | sh <<EOF
      while read entry;
        do
        id=\`echo \$entry | sed -ne 's/^\([[:alnum:]_-]\+\)[[:blank:]]\+.*$/\1/p'\`
        cmd=\`echo \$entry | sed -ne 's/^[[:alnum:]_-]\+[[:blank:]]\+\(.*\)$/\1/p'\`
        if [ "x\$id" != "x" -a "x\$cmd" != "x" ];
            then
            echo -e "\033[0;37;46m[MANU]\033[0m Running \"\$id\" \"\$cmd\""; sleep 1
            PATH=$PATH:$PWD/testcases/bin \$cmd < $stdin
            rc=\$?
            if [ \$rc != 0 ];
                then
                echo -e "\$id\tFAIL\t\$rc" >> $log
            else
                echo -e "\$id\tPASS\t\$rc" >> $log
            fi
        fi
      done < $cmdfile
EOF

  else
      # "Normal" run
      time ./runltp \
          -d $tmpdir \
          -r `pwd` \
          -p -q \
          -f $cmdfile \
          -l $log \
          -o $out > /dev/null
  fi

  info "Tests results for $t:"
  [ -e $log ] && cat $log

  # 
  cat >>$out <<EOF
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+++++++++++++++++++++++++ $0 v$VERSION
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
++++ cat /proc/cpuinfo
`cat /proc/cpuinfo`
++++ cat /proc/lspinfo/summary
`cat /proc/lspinfo/summary`
++++ uname -a
`uname -a`
++++ cat /etc/fsl-release
`cat /etc/fsl-release`
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
EOF


  # Send coverage results if needed
  [ "x$coverage" = "xyes" ] && {
      for a in `grep '^[^#]' $cmdfile | cut -f1 -d' '`;
        do
        tar=$cov_dir/$a.gcov.tar
        ftp_send_files "results.cov" $tar
        [ $? -eq 0 ] && rm -f $tar
      done
  }

  # erase OOM killer "don't kill" list
  flush_oom_dont_kill ;

  # Send test results and Clean away if FTP is used
  [ "x$VTE_METHOD" = "xFTP" -a "x$clean" = "xyes" ] && {
      ftp_send_files "results" $log $out
      ftp_runtest_clean $cmdfile
  }
done
