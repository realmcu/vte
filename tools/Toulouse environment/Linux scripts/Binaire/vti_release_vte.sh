#!/bin/bash

set -e

# to do:
#  -  catch exit and ctrl-C and ct setcs $oldcs
#  - Compare list of all files from all CR delivery files vs all files labellized CO_VT...I
#

menu_yes_no "You are going release vte-${VTE_VERS} from vte and vta vob, continue?" || exit 1


#
# Manage config specs
#

oldcs=/tmp/`basename $0`.$$.oldcs
newcs=/tmp/`basename $0`.$$.newcs

cleartool catcs > $oldcs

info "Setting new config spec..."
cat > $newcs <<EOF
#--------------------------------------------
element * CHECKEDOUT
#--------------------------------------------

#VTE Test Applications
element /vob/vb_sw_linux_ap_vta/... /main/LATEST

#Linux Test Projet
element /vob/vb_sw_linux_ap_vte/... /main/LATEST
#--------------------------------------------
EOF

cleartool setcs $newcs

#
# Sanity checks
#  TODO: LATEST int branch has LABEL_INT
#

info "Looking for private files..."
files=`cleartool lsp -other -short`
[ "x$files" != "x" ] && {
    warning "Found private files:"
    for f in $files; do echo " - $f"; done
    menu_yes_no "Do you want to delete them (`echo $files | wc -w` files)?" && \rm -Rf $files
}
info "Looking for checked out files..."
files=`cleartool lsco -avobs -short`
[ "x$files" != "x" ] && {
    error "Found checked out files:"
    for f in $files; do echo " - $f"; done
    menu_yes_no "Found `echo $files | wc -w` files! Are you sure you want to continue???" || {
        ct setcs $oldcs
        exit 1
    }
}

#
# merge vta vob
#

menu_yes_no "Do you want to merge vta / ${VTE_LABEL_INT} on main?" && { 
    info "Merging vta..."
    cleartool findmerge /vob/vb_sw_linux_ap_vta \
        -fversion ${VTE_LABEL_INT} \
        -comment "VTE ${VTE_VERS} release" \
        -merge -gmerge
}

info "Finding checked out (merged) files in vta vob..."
cofiles=`cleartool lsco -cview -rec -short /vob/vb_sw_linux_ap_vta`
menu_yes_no "Do you want to check in all the `echo $cofiles | wc -w` files in vta vob?" && { 
    info "Checking in vta..."
    cleartool ci -nc $cofiles
}


#
# merge vte vob.
#

menu_yes_no "Do you want to merge vte vob / ${VTE_LABEL_INT} on main?" && { 
    info "Merging vte..."    
    cleartool findmerge /vob/vb_sw_linux_ap_vte \
        -fversion ${VTE_LABEL_INT} \
        -comment "VTE ${VTE_VERS} release" \
        -merge -gmerge
}

info "Finding checked out (merged) files in vte vob..."
cofiles=`cleartool lsco -cview -rec -short /vob/vb_sw_linux_ap_vte`
menu_yes_no "Do you want to check in all the `echo $cofiles | wc -w` files in vte vob?" && { 
    info "Checking in vte..."
    cleartool ci -nc $cofiles
}

#
# remove private files (.contrib created by merge operation)
#
info "Looking for private files..."
files=`cleartool lsp -other -short`
[ "x$files" != "x" ] && {
    warning "Found private files:"
    for f in $files; do echo " - $f"; done
    menu_yes_no "Do you want to delete all the `echo $files | wc -w` files? (this is strongly recommended)" && \rm -Rf $files
}

#
# for now, no more checked out files allowed
#
info "Looking for checked out files..."
files=`cleartool lsco -avobs -short`
[ "x$files" != "x" ] && {
    error "Found checked out files:"
    for f in $files; do echo " - $f"; done
    error "Found `echo $files | wc -w` files!!!"
    echo "ABORTING!!!";
    ct setcs $oldcs
    exit 1
}


#
# labelized
#

menu_yes_no "Do you want to put ${VTE_LABEL_REL} on vte and vta vobs?" && { 
    info "Applying label on vta..."
    cleartool mklabel -rec -rep ${VTE_LABEL_REL} \
        /vob/vb_sw_linux_ap_vta
    info "Removing label on vta/lost+found..."
    cleartool rmlabel ${VTE_LABEL_REL} \
        /vob/vb_sw_linux_ap_vta/lost+found
    info "Applying label on vte..."
    cleartool mklabel -rec -rep ${VTE_LABEL_REL} \
        /vob/vb_sw_linux_ap_vte
    info "Removing label on vte/lost+found..."
    cleartool rmlabel ${VTE_LABEL_REL} \
        /vob/vb_sw_linux_ap_vte/lost+found
}

#
# create tar/gz
#
base=/pjt/linux_baseport
dir=vte-${VTE_VERS}
tar=$dir.tar
zip=$tar.bz2
menu_yes_no "Do you want to create $base/$zip?" && {
    for f in $dir $tar $zip; do
        [ -e $base/$f ] && \rm -f $base/$f
    done
    ln -s /vob/vb_sw_linux_ap_vte $base/$dir
    tar -cvh --exclude="lost+found" -C $base -f $base/$tar $dir
    bzip2 -z --best $base/$tar
    \rm -f $base/$dir
    info "$base/$zip is ready:"
    ls -lh $base/$zip
}

rep=$base/LIV/vte/vte-${VTE_VERS}
database=APSW_TestsDataBase_LinuxBasePort.xls
relnote=VTE-${VTE_VERS}_ReleaseNotes.doc

menu_yes_no "Do you want to create delivery repository in $rep?" && {
    [ ! -e $rep ] && mkdir -p $rep
    [ ! -e $base/$zip ] && {
        error "$base/$zip not found, aborting!"
        exit 1
    }
    cp $base/$zip $rep
    for f in $database $relnote; do
        [ ! -e $VTE_USER_CR_DIR/$f ] && {
            error "$f not found, aborting!"
            exit 1
        }
        [ -e $rep/$f ] && \rm -f $f;
        info "Copying $f ..."
        cp $VTE_USER_CR_DIR/$f $rep
    done
    info "Creating vte-${VTE_VERS}.cs ..."
    cat > $rep/vte-${VTE_VERS}.cs <<EOF
# ClearCase config spec to access to VTE ${VTE_VERS} release
#---------------------------------------------------
element * ${VTE_LABEL_REL} -nocheckout

# Note that you must have access to both following vobs:
#      /vb_sw_linux_ap_vta  
#  and /vb_sw_linux_ap_vte
EOF
}
