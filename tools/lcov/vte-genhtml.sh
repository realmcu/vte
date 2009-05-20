prog=vte-genhtml.sh
version=1.0

infodir=$PWD
datadir=$PWD
htmldir=$PWD


declare -i verbose=1

usage()
{
    echo "
${prog} ${version} read lcov capture data file(s) (.info), and generate a unique HTML report for the file(s).

Usage:

    ${0##*/} [-v] [-q] [-o <HTMLDIR>] -t <TITLE> <INFO_FILE>...

    Where:
      -v           : Be more verbose, print command on stdout.
      -q           : Be more quiet, print nothing on stdout.
      -t <TITLE>   : Title/name for the HTML report.
      -o <HTMLDIR> : Directory where to put HTML files (default is $htmldir/<TITLE>)
      <INFO_FILE>..: list of info file(s) to process for the HTML report.
"
    exit
}

action()
{
 if [ $verbose -gt 0 ];
 then
  echo "$*" | tee -a $log
 fi
}

command()
{
 if [ $verbose -gt 1 ];
 then
  echo "  $*" | tee -a $log
 fi
 ($*) >> $log # 2>&1
}

here=$PWD
log=`mktemp /tmp/$prog-log.XXXXXX`

title=""
input=""
outdir=""
while [ $# -ne 0 ];
do
 case $1 in
  -o)
  outdir=$2
  shift; shift
  ;;
  -q)
  verbose=$((verbose-1))
  shift;
  ;;
  -v)
  verbose=$((verbose+1))
  shift
  ;;
  -vv)
  verbose=$((verbose+2))
  shift
  ;;
  --debug)
  set -x
  shift
  ;;
  -t)
  title=$2
  shift; shift
  ;;
  *)
  ls -- $1 > /dev/null 2>&1
  result=$?
  if [ $result -ne 0 ];
  then
   echo "\"$1\": no such file or directory"
   usage
  fi
  input="$input $1"
  shift
  ;;
 esac
done

if [ x"$input" == "x" -o "x$title" == "x" ];
then
 usage
fi

outdir=${outdir:-$htmldir/$title}

echo "[$prog] Logging commands to \"$log\"."

rm -f $log
touch $log


action "Generating HTML report in $outdir..."
command genhtml -s -t $title -o $outdir $input


