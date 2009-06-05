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

usage: ${me} [-<action>]  <CR> <LABEL> <DIRECTORY> <FEATURE>
Where action can be:
- a: all of the folowing, this is the default behaviour
- v: Check for Convention (coding rule)
- c: Convert all files to Unix
- m: Import files
- b: Put JET integration label on files
- e: End CR, mark it as done

Where DIRECTORY is:
  the full path to  the testsuite directory
e.g. xxxxx/telephony/voicecall_testsuite
Be careful to have the exact same testsuite name than in ClearCase (if it already exists)

Where FEATURE can be:
- multimedia
- connectivity
- telephony

EOF
    exit 1
}

# If 6 arguments, first is the action flag
if [ $# == 6 ]; 
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
if [ $# != 4 ]; 
    then 
    usage; 
fi

CR="tlsbo$1"
LABEL=$2
DELIV_DIR=$3
FEATURE=$4

CC_DIR="/vobs/vb_sw_test_jet_suites/testcases/$FEATURE/."


echo "CR : $CR, LABEL : $LABEL, DIR : $DELIV_DIR"
echo "ClearCase dir : $CC_DIR"


# ===============================================================
# Check for convention (jet_check_convention.pl)
# ===============================================================

function check
{
    info "Checking the coding rules"
    find $DELIV_DIR -type f \
       -exec /local/linux_baseport/bin/jet_check_convention.pl -nospell {} \;
}

# ===============================================================
# Convert all files to Unix (dos2unix)
# ===============================================================

function convert
{
    info "Converting all files from Dos to Unix"
    echo "Execute manually the following command:"
    echo "     find $DELIV_DIR -type f -exec dos2unix {} \\;"
    #action "find $DELIV_DIR -type f -exec dos2unix {} \;"
    echo "or"
    echo "    find $DELIV_DIR -type f | xargs dos2unix"
    #action "find $DELIV_DIR -type f | xargs dos2unix"
}

# ===============================================================
# Set the config spec to import the delivery into the dvt branch
# ===============================================================
# For the moment, choose your view to work with

function configSpec
{
    info "Double-check your ConfigSpec before going on with the import"

    action "cleartool catcs"

    if [ ! -d $CC_DIR ]
    then
        echo "The directory $CC_DIR does not exist"
        echo
        echo "You probably forgot to set your ClearCase view"
        exit 1
    fi
}

# ===============================================================
# Import the delivered files (clearfsimport)
# ===============================================================

function import
{
    info "Importing Wipro delivery"

#    cleartool ls $CC_DIR
    clearfsimport \
        -comment "integration of Wipro delivery: $CR" \
        -nsetevent -rmname -recurse $DELIV_DIR $CC_DIR
}


# ===============================================================
# Create a label
# ===============================================================

function create_label
{
    info "Create the ${LABEL} label..." 
    cleartool mklbtype $LABEL
}

# ===============================================================
# Label the files
# ===============================================================

function apply_label
{
    info "Apply label ${LABEL} to files..." 
    cleartool mklabel -rec -rep $LABEL $CC_DIR
#    cleartool rmlabel $LABEL $CC_DIR/lost+found
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

    log="/tmp/".${LABEL}.${CR}~$do.$$

    [ $action == "all" ] && \
        menu_yes_no "$question"
    [ "$choice" == "yes" -o $action == "$do" ] && {
        $do 2>&1 | tee $log
        info "\nDone: log can be found in $log\n"
    }
}

echo "Reminder: you need to be in a Clearcase view"
echo "********************************************"

do_it check "Run checkConventions.pl ?"

do_it convert "Would you like to convert all files to Unix? -- NEW ONE, NOT TESTED YET"

do_it configSpec "Would you like to check your config spec?"

do_it import "Would you like to import this delivery?"

do_it create_label "Would you like to create the ${LABEL} label?"

do_it apply_label "Would you like to apply the ${LABEL} label?"

exit 0


