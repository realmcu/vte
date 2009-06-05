#!/bin/bash
set -u
set -e
set -x

OLD_DIRECTORY=`pwd`
VERBOSE="no"
TAR_VERBOSE=""
TAR_EXT="tgz"

TIMESTAMP=`date +%Y%m%d-%H%M`
WORKDIR="/pjt/linux_baseport"
VTE_RELEASE="NULL"
CT_VIEW="NULL"
LTP_RELEASE="NULL"

usage()
{
    cat <<EOF
Usage: ${0##*/} -R|--release <vte release number> -C|--cview <clearcase view> -L|--ltp <LTP release> [-v|--verbose]
    -R|--release: release number (example: vte-1.9).
    -C|--cview: clearcase view from where the source codes can be copied.
    -L|--ltp: LTP release version (example: ltp-full-20050405) (DO NOT EXIST ANYMORE)
    -v|--verbose: print additional information during the execution.
    
EOF
    exit
}


prepare_packaging()
{
    if [ $VERBOSE == "yes" ];
    then
        echo "prepare_packaging()"
    fi
    echo -n "[$VTE_RELEASE]  Preparing Package ..."
    cd $WORKDIR
    rm -rf $VTE_RELEASE $VTE_RELEASE.$TAR_EXT
    mkdir -p $WORKDIR/$VTE_RELEASE
    echo " DONE"
}

copy_all_vte_source_code()
{
    if [ $VERBOSE == "yes" ];
    then
        echo "copy_all_vte_source_code()"
    fi
    echo -n "[$VTE_RELEASE]  Getting VTE source code from Clearcase ..."
    cleartool startview $CT_VIEW
    cd /view/$CT_VIEW/vob/vb_sw_linux_ap_vte
    cp -Lrf `ls | grep -v lost` $WORKDIR/$VTE_RELEASE
    cleartool endview $CT_VIEW
    echo " DONE"
}

generate_vte_deliverable()
{
    if [ $VERBOSE == "yes" ];
    then
        echo "generate_vte_deliverable()"
    fi
    echo -n "[$VTE_RELEASE]  Generating $VTE_RELEASE.$TAR_EXT ..."
    cd $WORKDIR/$VTE_RELEASE/..
    tar cfz$TAR_VERBOSE $VTE_RELEASE.$TAR_EXT ./$VTE_RELEASE
    echo " DONE"
}

generate_patch_deliverable()
{
    if [ $VERBOSE == "yes" ];
    then
        echo "generate_patch_deliverable()"
    fi
    echo -n "[$VTE_RELEASE]  Generating ${LTP_RELEASE}_to_${VTE_RELEASE}.patch ..."
    cd $WORKDIR/$VTE_RELEASE/..
    diff -Naur ./$LTP_RELEASE ./$VTE_RELEASE > ${LTP_RELEASE}_to_${VTE_RELEASE}.patch || true
    echo " DONE"
}

finalize_packaging()
{
    if [ $VERBOSE == "yes" ];
    then
        echo "finalize_packaging()"
    fi
    echo -n "[$VTE_RELEASE]  Finalizing Package ..."
    rm -rf $WORKDIR/$VTE_RELEASE
    cd $OLD_DIRECTORY
    echo " DONE"
}


if [ $# -eq 0 ];
 then
    usage
else
    while [ $# -ne 0 ];
        do
        case $1 in
            -R|--release)
                VTE_RELEASE=$2
                shift
                shift
                ;;
            -C|--cview)
                CT_VIEW=$2
                shift
                shift
                ;;
#            -L|--ltp)
#                LTP_RELEASE=$2
#                shift
#                shift
#                ;;
            -v|--verbose)
               VERBOSE="yes"
               TAR_VERBOSE="v"
               shift
                ;;
            -*)
                echo "Unknow option $1"
                usage
                ;;
            *)
               echo "Unknow option $1"
               usage
               ;;
      esac
    done
fi


if [ $VTE_RELEASE == "NULL" ] || [ $CT_VIEW == "NULL" ];
then
    usage
fi

prepare_packaging
copy_all_vte_source_code
generate_vte_deliverable
#generate_patch_deliverable
finalize_packaging
echo "[$VTE_RELEASE]  << PACKAGE AVAILABLE >>"


