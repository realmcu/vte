
TARGETS-$(CONFIG_VTE_IMX5) += imx51_auto imx51_p_auto imx53_auto imx53_p_auto imx50_auto imx50_p_auto
TARGETS-$(CONFIG_VTE_IMX28) += imx28_auto imx28_p_auto

all:

install:
	-mkdir ../../../runtest
	for i in $$TARGETS-y; do \
	ln -f $$i ../../../runtest/ \
	done
clean:


