#!/bin/sh

# TODO: To put in vti_init_integration.sh:
#- add branch and label creation (useful when beginning a new VTE integration):
# branch is int_tlsbo66009_zfr11_cgag1c_sps-linux-ap-vte-2.1
# label is CO_VTE_02.01I
# This has to be done in both vte and vta VOBs
#- put new label on all files by default (when beginning a new VTE integration)
#- merge in the directory vb_sw_linux_ap_vte from the previous int branch

set -u
set +e
#set -x

me=$(basename $(which $0))


old_cs = /tmp/`basename $0`.cs.orig.$$
ct catcs > ${old_cs}
new_cs = /tmp/`basename $0`.cs.$$

previous_vers_major=""
previous_vers_minor=""
previous_cr_int=""
previous_cr_int_owner=""
[ -e ${VTE_USER_CFG_DIR}/VTE_VERS_MAJOR.previous ] && \
    previous_vers_major=$(cat ${VTE_USER_CFG_DIR}/VTE_VERS_MAJOR.previous)
[ -e ${VTE_USER_CFG_DIR}/VTE_VERS_MINOR.previous ] && \
    previous_vers_mino=$(cat ${VTE_USER_CFG_DIR}/VTE_VERS_MINOR.previous)
[ -e ${VTE_USER_CFG_DIR}/VTE_CR_INT.previous ] && \
    previous_cr_int=$(cat ${VTE_USER_CFG_DIR}/VTE_CR_INT.previous)
[ -e ${VTE_USER_CFG_DIR}/VTE_CR_INT_OWNER.previous ] && \
    previous_cr_int_owner=$(cat ${VTE_USER_CFG_DIR}/VTE_CR_INT_OWNER.previous)

question "Previous Major vesion?" "$previous_vers_major"
question "Previous Minor vesion?" "$previous_vers_minor"
question "Previous CR?" "$previous_cr_int"
question "Previous CR owner?" "$previous_cr_int_owner"



#
# IMPORT LTP
#

cat > ${new_cs} <<EOF
element * CHECKEDOUT
#-------------------------------
#Linux Test Projet
element /vb_sw_linux_ap_vte/... /main/LATEST
#-------------------------------
EOF

clearfsimport -comment "ltp-20060105 import" -mklabel LTP-20060105 -nsetevent -rmname -recurse /pjt/linux_baseport/ltp-full-20060105/* /vob/vb_sw_linux_ap_vte/

#
# Merge LTP
#

cat > ${new_cs} <<EOF
element * CHECKEDOUT
#-------------------------------
#branch to make the development as it is specified on the DDTS Change Request
element * .../int_tlsbo62202_zfr11_svan01c_sps-linux-ap-vte-1.16/LATEST
mkbranch int_tlsbo62202_zfr11_svan01c_sps-linux-ap-vte-1.16
#--------------------------------------------------------------------------- 
#Baseline LTP
element     /vb_sw_linux_ap_vte/... LTP-20060105
#--------------------------------------------------------------------------- 
#VTE Test Applications
element /vb_sw_linux_ap_vta/... /main/LATEST

#Linux Test Projet
element /vb_sw_linux_ap_vte/... /main/LATEST
#-------------------------------
EOF

# merge release précédente vob vte avec le nouveau LTP
ct findmerge /vob/vb_sw_linux_ap_vte -fver CO_VTE_01.15.00R -comment "integration LTP 20060105 with VTE 1.15" -merge -gmerge
ct ci -nc `ct lsco -cvi -r -s`
ct find /vob/vb_sw_linux_ap_vte -version 'version(.../int_tlsbo62202_zfr11_svan01c_sps-linux-ap-vte-1.16/LATEST)' -exec 'cleartool mklabel -rep CO_VTE_01.16.00I $CLEARCASE_PN'


# create new branches and labels
# cd /vob/vb_sw_linux_ap_vta && {
#  cleartool mkbrtype -c "Integration branch for VTE-$VTE_VERS" $VTE_BRANCH
#  cleartool mklbtype -c "Integration label for VTE-$VTE_VERS" $VTE_LABEL_INT
# }
#
# cd /vob/vb_sw_linux_ap_vte && {
#  cleartool mkbrtype -c "Integration branch for VTE-$VTE_VERS" $VTE_BRANCH
#  cleartool mklbtype -c "Integration label for VTE-$VTE_VERS" $VTE_LABEL_INT
# }

# Labelliser tous les fichiers de la vob VTE et VTA
ct mklabel -rec -rep $VTE_LABEL_INT /vob/vb_sw_linux_ap_vte
ct rmlabel $VTE_LABEL_INT /vob/vb_sw_linux_ap_vte/lost+found
ct mklabel -rec -rep $VTE_LABEL_INT /vob/vb_sw_linux_ap_vta
ct rmlabel $VTE_LABEL_INT /vob/vb_sw_linux_ap_vta/lost+found

ct setcs ${new_cs}
