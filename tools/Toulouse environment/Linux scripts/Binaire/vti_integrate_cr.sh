#!/bin/sh

# TODO: add the possibility to run dos2unix if check failed?
# TODO: add another diff: ignore all blank and new lines: -EBbw
# TODO: handle criteria acceptance (generate a file?)
# TODO: add uncheckin action
# TODO: add possibility to labelize/unlabelize the integrated files:
# ct find /vob/vb_sw_linux_ap_vta/ -version 'version(/main/int_tlsbo66009_zfr11_cgag1c_sps-linux-ap-vte-2.1/LATEST)' -print -nxn
# ct find <...> -exec 'cleartool mklabel -rep CO_VTE_02.01I $CLEARCASE_PN'
# TODO: store merge log in VTE_USER_CFG_DIR

set -u
set +e
#set -x

me=$(basename $(which $0))




function usage
{
    cat <<EOF
usage: ${me} [-<action>] <CR_NUMBER_WITHOUT_TLSBO>
Where action can be:
- a: all of the folowing (except unmerge), this is the default behaviour
- l: List modified files 
- v: Check for Convention (coding rule)
- d: Diff with /main/${VTE_LABEL_INT}
- m: Merge files
- u: Unmerge (uncheckout) files (don't keep).
- c: Check in files
- b: Put VTE integration label on files
- e: End CR, mark it as done

EOF
    vti_ls_crs.sh
    exit 1
}

# If 2 arguments, first is the action flag
if [ $# == 2 ]; 
    then
    case $1 in
        -l) action="list";;
        -v) action="conv";;
        -d) action="diff";;
        -m) action="merge";;
        -u) action="unmerge";;
        -c) action="checkin";;
        -b) action="label";;
        -e) action="end";;
        *)
            usage;
    esac
    shift
else
    action="all"
fi

# The remainig argument is CR number
if [ $# != 1 ]; 
    then 
    usage; 
fi

CR="tlsbo$1"

# Get the label(s) associated with this CR
echo "sed -e 's/${CR}//g'"
labels=$(cd ${VTE_USER_CR_DIR} && ls ${CR}* 2>/dev/null | sed -e 's/^\(.*\)\.\(.*\)\.\(.*\)\.\(.*\)/\2\.\3\.\4/g')
#'s/\(.*\)\.\(.*\)$/\1/g')
echo ${labels}
if [ "x$labels" == "x" ];
then
    echo -e "\n*** $me: CR not found!\n"
    vti_ls_crs.sh
    exit 1;
fi

# If more than one label found, let user choose one
if [ `echo $labels | wc -w` != 1 ];
then
    menu "$labels" "Please choose a delivery label"
    LABEL=$choice
else
    LABEL=$labels
echo LABEL = $LABEL
fi

# TODO: 
#  - put the following into a function, thus allowing to not launch CC, when ending only a CR.
#  - compare found files vs file list from the release file

info "Finding modified files for CR ${CR}/${LABEL}..."
# get files and version (as a list)
mfiles_v=$(cleartool find /vob/vb_sw_linux_ap_vta -version "version($LABEL)" -exec 'echo $CLEARCASE_XPN')
mfiles=$(echo ${mfiles_v} | sed -e 's/@@[^[:space:]]*//g')
mvers=$(echo ${mfiles_v} | sed -e 's/[^[:space:]]*@@\([^[:space:]]*\)/\1/g')

# same but as array
tmfiles_v=(${mfiles_v})
tmfiles=(${mfiles})
tmvers=(${mvers})

declare -i nb_mfiles;
nb_mfiles=`echo  ${mfiles} | wc -w`
indexes=`seq 0 $(($nb_mfiles-1))`

function list
{
    for i in $indexes;
      do
      echo "[$i] @${tmvers[$i]}"
      echo "${tmfiles[$i]}"
    done
}

function conv
{
    files=""
    for f in $mfiles_v;
      do
      [ ! -d $f ] && files="$files $f";
    done
    ${VTE_CHECK_CONVENTION} $files 
}

# BUG, we diff label ${VTE_LABEL_INT} vs new vers, doesn't work if
#  file is not labelizzed ${VTE_LABEL_INT} do it with /LATEST?
# PROCESS: All files have to be labelized with VTE_LABEL_IN when
#  starting a new integration sequence (see vti_init_integration.sh)
function diff
{
    for i in $indexes;
      do
      #vers1=${tmfiles[$i]}@@/main/${VTE_LABEL_INT}
      #vers1=${tmfiles[$i]}@@/main/LATEST
      vers1=${tmfiles[$i]}
      vers2=${tmfiles[$i]}@@${tmvers[$i]}
      next="false"
      while [ $next != "true" ];
      do
        menu "quit text graph next" "Diff ${tmfiles[$i]}?"
        case $choice in
            text)
                info "Diffing $vers1 against ${tmvers[$i]}..."
                cleartool diff \
                    -options -blank_ignore -diff_format \
                    ${vers1} \
                    ${vers2} | less
                ;;
            graph)
                info "Diffing $vers1 against ${tmvers[$i]}..."
                cleartool diff -graphical \
                    -options -blank_ignore \
                    ${vers1} \
                    ${vers2}
                ;;
            next)
                next=true
                ;;
            quit|*)
                exit 1; # exit function
        esac
      done
    done;
}

function merge
{
    info "Merging CR ${CR}/${LABEL} with VTE ${VTE_VERS}"
    cleartool findm \
        -avobs -fver ${LABEL} \
        -c "${me}: Integration of CR ${CR}/${LABEL} delivery with VTE ${VTE_VERS}" \
        -mer -gm
}

function unmerge
{
    info "Unmerging CR ${CR}/${LABEL} with VTE ${VTE_VERS}"
    cleartool unco -rm ${mfiles}
}

function checkin
{
    info "Checking in files..." 
    cleartool ci -nc ${mfiles}
    # If user have change other files?
    # If file is identical it will not be checked out by the merge.
    # => compare ct lsco with ${mfiles} and emit a warning if !=
    #    + propose to diff files?
}

function label
{
    info "Apply label ${VTE_LABEL_INT} to files..." 
    #cleartool find /vob/vb_sw_linux_ap_vta -version 'version(.../${VTE_BRANCH}/LATEST) && \! version(${VTE_LABEL_INT})' -exec 'cleartool mklabel -rep ${VTE_LABEL_INT} $CLEARCASE_PN'
    for f in $mfiles;
      do
      cleartool mklabel -rep ${VTE_LABEL_INT} $f;
    done
}

function end
{
    info "Moving ${CR}.${LABEL} to ${CR}.${LABEL}~DONE"
    mv ${VTE_USER_CR_DIR}/${CR}.${LABEL} ${VTE_USER_CR_DIR}/${CR}.${LABEL}~DONE
}


#
# Here we go.
#

# If "All" action then ask for each step.
# If an action was specified do the corresponding action.
# Unmerge is special action: do unmerge and exit

function do_it
{
    do="$1"
    question="$2"
    choice="no"

    log=${VTE_USER_CR_DIR}/${LABEL}.${CR}~$do.$$

    [ $action == "all" ] && \
        menu_yes_no "$question"
    [ "$choice" == "yes" -o $action == "$do" ] && {
        $do 2>&1 | tee $log
        info "\nDone: log can be found in $log\n"
    }
}

[ $action == "unmerge" ] && { \
    unmerge;
    exit $?;
}

do_it list "Find $nb_mfiles modified files, see list?"

do_it conv "Run checkConventions.pl ?"

do_it diff "Would you like to view modifications as diff?"

do_it merge "Would you like to merge this CR?"

do_it checkin "Would you like to check in this CR?"

do_it label "Would you like to apply label ${VTE_LABEL_INT} to this CR?"

do_it end "Would you like to mark this CR as DONE?"

exit 0

