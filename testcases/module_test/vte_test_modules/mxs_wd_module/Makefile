#this moudle is i.mx5x watch dog test

# Platforms with one watchdog only

obj-m= mxs_wdt_test.o
TARGET= mxs_wdt_test.ko

all: 
	@make -C $(KERNEL_SRCDIR) SUBDIRS=`pwd` KBUILD_OUTPUT=$(KLINUX_BLTDIR) ARCH=$(ARCH_CPU) CROSS_COMPILE=$(CROSS_COMPILER) modules
	@rm -rf *.mod.ko *.mod.c .*.cmd *.o .tmp_versions
	@echo "***********************************************"
	@echo "** ALL : $(TARGET)                    **"
	@echo "***********************************************"
	@echo

clean:
	@rm -f $(TARGET) *.mod.ko *.mod.c .*.cmd *.o
	@rm -rf .tmp_versions *.order  *.symvers
	@echo "***********************************************"
	@echo "** CLEAN : $(TARGET)                  **"
	@echo "***********************************************"
	@echo

install:
	@ln -f $(TARGET) ../../../bin
	@echo "***********************************************"
	@echo "** INSTALL : $(TARGET)                **"
	@echo "***********************************************"
	@echo
