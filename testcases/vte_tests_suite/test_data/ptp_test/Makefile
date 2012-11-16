
TARGETS = ptp_main ptp_main_1.02

all:
	@echo "***********************************************"
	@echo "** ALL : $(TARGETS)  test application  **"
	@echo "***********************************************"
	@echo
install:
	@for i in $(TARGETS); do \
		ln -f $$i ../../bin/ ;\
		echo "***********************************************" ; \
		echo "** INSTALL : $$i " ; \
		echo "***********************************************" ; \
		done
	@echo
clean:
	@echo "not to clean anything from third party"

