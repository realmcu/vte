#!/bin/sh

set -e

usage()
{

    cat <<EOF
mkbackup.sh [--debug] [--dry-run] <additional_dirs...>
  Make a backup of VTE developers env + additional_dirs if supplied on command line.
  I assume to be launch in the top VTE env dir.
EOF

}

if [ "$1" = "--help" ];
then  
    usage 
    exit 1
fi

if [ "$1" = "--debug" ];
then  
    shift
    set -x
fi

dryrun=no
if [ "$1" = "--dry-run" ];
then
    dryrun="yes"
    shift
fi

BKUP=VTE-BACKUP-`date +"%F"`
BKUPDIR=/tmp/$BKUP
BKUPISO=/tmp/$BKUP.iso
BKUPLOG=$BKUPDIR/backup.log
BKUPLST=$BKUPDIR/backup.list

[ -e $BKUPDIR ] && { echo "$BKUPDIR already exist!"; exit 1; }

mkdir $BKUPDIR;
touch $BKUPLOG
touch $BKUPLST

info()
{
    echo "### [$0] $*" 2>&1 | tee -a $BKUPLOG
}

action()
{
    echo "$*" 2>&1 | tee -a $BKUPLOG
    if [ "$dryrun" = "no" ];
        then
        $* 2>&1 | tee -a $BKUPLOG
    fi
}

# full path of file(s) or dir(s) to backup
backup()
{
    for f in $*;
    do
      ls -dils "$f" >> $BKUPLST
    done
    action "cp -af --parents $* $BKUPDIR"
}


info "Processing additional dirs supplied by user..."
if [ "x$*" != "x" ];
    then
    for f in `ls -d $*`;
    do
      backup $f
    done
else
    echo "Nothing to be done"
fi

info "Processing VTE root..."

# à la racine, on garde tout sauf LIV, REF et les ESTR
info " Copying all except LIV, REF, ESTR_* in $BKUPDIR"
for f in `ls | grep -v 'LIV\|REF\|ESTR'`;
  do
  backup $f
done

# Traitement des ESTR
for estr in `ls -d ESTR_*`;
  do
  info " Processing $estr..."
  # on garde tout sauf BOOT, BUILD, COVERAGE, FTP, ROOTFS, SRC, TMP
  info "  Copying all except BOOT, BUILD, COVERAGE, FTP, ROOTFS, SRC and TMP to $estr"
  mkdir -p $BKUPDIR/$estr
  for f in `ls $estr | grep -v 'BOOT\|BUILD\|COVERAGE\|FTP\|ROOTFS\|SRC\|TMP'`;
  do
    backup "$estr/$f"
  done
  # on va garder tout les fichiers résultat (log, out, cov)
  info " Copying results files"
  for f in `find $estr/FTP $estr/COVERAGE -name 'results*' -print`;
  do
    backup $f
  done
done

info "Backup size:"
du -sh $BKUPDIR

info "Generating iso fs..."
mkisofs -lLRT -o $BKUPISO $BKUPDIR 

info "Backup finished."
du -sh $BKUPISO

info ""
info "To test the iso image, mount it by running:"
info "mkdir /tmp/$BKUPDIR-TEST"
info "sudo mount -o loop -t iso9660 $BKUPISO /tmp/$BKUPDIR-TEST"
info "ls -als /tmp/$BKUPDIR-TEST"
info ""
info "To burn it run \"sudo cdrecord -v dev=/dev/cdwriter speed=12 -eject $BKUPISO\""
info "Be sure needed modules are loaded (sg ide-scsi)"
info ""
info "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
info "!!!! No automatic backup for LIV and REF, be sure to not have"
info "!!!! forget important files."
info "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
info ""

