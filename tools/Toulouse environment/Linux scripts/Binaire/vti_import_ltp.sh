# TODO: handle 2 cases:
#  - LTP integration before CR integration starts
#  - LTP integration after CR integration starts

#- clearfsimport new ltp
#- merge release précédente vob vte avec le nouveau LTP

#
# LTP integration before CR integration starts
#

element * CHECKEDOUT
#-------------------------------
#Linux Test Projet
element /vb_sw_linux_ap_vte/... /main/LATEST
#-------------------------------

under Laramade:
   cd /pjt/linux_baseport
   tar xfz ltp-full-20060105.tgz

under Unix/csh:
   ct setview vw_apsw_svan01c
   diff --recursive /pjt/linux_baseport/ltp-full-20060105/ /vob/vb_sw_linux_ap_vte/ | tee ~/TMP/diff_ltp0106_ltp1205.log
   clearfsimport -comment "ltp-20060105 import" -mklabel LTP-20060105 -nsetevent -rmname -recurse /pjt/linux_baseport/ltp-full-20060105/* /vob/vb_sw_linux_ap_vte/.

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

ct findmerge /vob/vb_sw_linux_ap_vte -fver PREVIOUS_VTE_LABEL_REL -comment "integration LTP 20060105 with VTE 1.15" -merge -gmerge
ct ci -nc `ct lsco -cvi -r -s`
ct find /vob/vb_sw_linux_ap_vte -version 'version(.../int_tlsbo62202_zfr11_svan01c_sps-linux-ap-vte-1.16/LATEST)' -exec 'cleartool mklabel -rep CO_VTE_01.16.00I $CLEARCASE_PN'

