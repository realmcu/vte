#!/bin/sh

set -u
set +e
#set -x

me=$(basename $(which $0))

. ../JET_config/functions

# Input arguments
#    Root directory with the delivery to import into ClearCase
#    CR number + core_id for the dvt branch


function usage
{
    cat <<EOF
usage: ${me} [-<action>] <FROM_LABEL> <TO_LABEL>
 
Where action can be:
- a: all of the following, this is the default behaviour
- m: Import files
- b: Put JET integration label on files
- e: End CR, mark it as done

EOF
    exit 1
}

# If 3 arguments, first is the action flag
if [ $# == 3 ]; 
    then
    case $1 in
        -v) action="check";;
        -c) action="convert";;
        -m) action="merge";;
        -b) action="label";;
        *)
            usage;
    esac
    shift
else
    action="all"
fi

# remaining arguments
if [ $# != 2 ]; 
    then 
    usage; 
fi

LABEL_FROM=$1
LABEL_TO=$2

JET_TS_VOB="/vobs/vb_sw_test_jet_suites"

echo "FROM_LABEL : $LABEL_FROM, TO_LABEL : $LABEL_TO, VOB : $JET_TS_VOB"




# ===============================================================
# Set the config spec to import the delivery into the dvt branch
# ===============================================================
# For the moment, choose your view to work with

function configSpec
{
    info "Double-check your ConfigSpec before going on with the import"

    action "cleartool catcs"

    if [ ! -d $JET_TS_VOB ]
    then
        echo "The directory $JET_TS_VOB does not exist"
        echo
        echo "You probably forgot to set your ClearCase view"
        exit 1
    fi
}

# ===============================================================
# List all files needing a merge
# ===============================================================

function listMerge
{
    info "List all files needing a merge"

    echo "cleartool find $JET_TS_VOB -version 'version($LABEL_FROM)' -print -nxn"
    action "cleartool find $JET_TS_VOB -version 'version($LABEL_FROM)' -print -nxn"
}

# ===============================================================
# Merge 
# ===============================================================

function merge
{
    info "Importing ${LABEL_FROM}"

    echo "cleartool findmerge $JET_TS_VOB -fversion $LABEL_FROM -comment "integration $LABEL_FROM" -merge -gmerge"

    cleartool findmerge $JET_TS_VOB -fversion $LABEL_FROM \
       -comment "integration $LABEL_FROM" \
       -merge -gmerge
}

# ===============================================================
# List all files needing a check-in
# ===============================================================

function listCheckedOutFiles
{
    info "List all files needing a check-in"

    action "cleartool lsco -cview -rec -short $JET_TS_VOB"
}

# ===============================================================
# Check in all checked-out files
# ===============================================================


function checkin
{
    info "Checking in files..." 
    cleartool ci -nc `cleartool lsco -cview -rec -short $JET_TS_VOB`
}

# ===============================================================
# Create a label
# ===============================================================

function create_label
{
    info "Create the ${LABEL_TO} label..." 
    cleartool mklbtype $LABEL_TO
}

# ===============================================================
# Label the files
# ===============================================================

function apply_label
{
    info "Apply label ${LABEL_TO} to files..." 
    cleartool mklabel -rec -rep $LABEL_TO $JET_TS_VOB
    cleartool rmlabel $LABEL_TO $JET_TS_VOB/lost+found
}



#
# Here we go.
#

# If "All" action then ask for each step.
# If an action was specified do the corresponding action.

function do_it
{
    do="$1"
    question="$2"
    choice="no"

    log="/tmp/".${LABEL_FROM}~$do.$$

    [ $action == "all" ] && \
        menu_yes_no "$question"
    [ "$choice" == "yes" -o $action == "$do" ] && {
        $do 2>&1 | tee $log
        info "\nDone: log can be found in $log\n"
    }
}

echo "Reminder: you need to be in a Clearcase view"
echo "********************************************"

do_it configSpec "Would you like to check your config spec?"

do_it listMerge "Would you like to list of files needing to be merged?"

do_it merge "Would you like to merge this delivery?"

do_it listCheckedOutFiles "Would you like to list all checked-out files?"

do_it checkin "Would you like to check in all checked-out files?"

do_it create_label "Would you like to create the ${LABEL_TO} label?"

do_it apply_label "Would you like to apply the ${LABEL_TO} label?"

exit 0


