
AUTO_TARGETS-$(CONFIG_VTE_IMX5) += imx51_auto imx51_BABBAGE_auto  imx53_auto imx50_auto imx53_ALSA_auto imx50_auto_p1 imx50_auto_p2 imx50_auto_p3 \
                                   imx53_LOCO_auto
AUTO_TARGETS-$(CONFIG_VTE_IMX28) += imx28_auto
P_AUTO_TARGETS-$(CONFIG_VTE_IMX5) += imx51_p_auto imx53_p_auto imx50_p_auto
P_AUTO_TARGETS-y += imx51_p_auto imx53_p_auto imx50_p_auto
P_AUTO_TARGETS-$(CONFIG_VTE_IMX28) += imx28_p_auto

all:

install:
	-mkdir ../../../runtest
	-for i in $(AUTO_TARGETS-y); do \
		ln -f $$i ../../../runtest/ ;\
	done
	-for i in $(P_AUTO_TARGETS-y); do \
		./patch_p_auto $$i;  \
		ln -f vte_$$i ../../../runtest/; \
		done
clean:
	-rm -rf vte_*


