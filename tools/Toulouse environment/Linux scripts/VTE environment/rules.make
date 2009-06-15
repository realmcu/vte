
help: generic_help lbp_help vte_help lcov_help

generic_help:
	@echo "* Main targets:"
	@echo "  ============="
	@echo "  - ref2estr:"
	@echo "  - install_ref: Install Offical kernel and vte"
	@echo "  - install:"
	@echo "  - clean:"
	@echo "  - mrproper:"
	@echo ""



install: vte_install lbp_install 

install_ref: vte_install_ref lbp_install_ref 

clean: lbp_clean vte_clean lcov_clean

mrproper: lbp_mrproper vte_mrproper lcov_mrproper



include $(CONFIGDIR)/vte.$(VTE_VERSION).mk
include $(CONFIGDIR)/linuxos.linux.$(LBP_VERSION).mk

include $(CONFIGDIR)/lcov.mk
