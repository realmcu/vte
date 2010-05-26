#Copyright (C) 2005-2010 Freescale Semiconductor, Inc. All Rights Reserved.
#
#The code contained herein is licensed under the GNU General Public
#License. You may obtain a copy of the GNU General Public License
#Version 2 or later at the following locations:
#
#http://www.opensource.org/licenses/gpl-license.html
#http://www.gnu.org/copyleft/gpl.html
###############################################################################
#
#    @file   Makefile
#
#    @brief  configuration file to compile the VTE tests.
#
###############################################################################
#Revision History:
#                            Modification     Tracking
#Author/core ID                  Date          Number    Description of Changes
#-------------------------   ------------    ----------  -------------------------------------------
#Francois GAFFIE/RB657C       03/05/2004     TLSbo39140  add SHARP LCD test application
#Tony THOMASSIN/RB595C        18/05/2004     TLSbo39490  SPI test development 
#Ludovic DELASPRE/RC149C      28/05/2004     TLSbo39713  VTE 1.2 integration 
#Dmitriy Kazachkov/E1403C     16/06/2004     TLSbo39741  emi_tests fs_tests io_tests pm_tests rm_tests
#Ludovic DELASPRE/RC149C      29/06/2004     TLSbo39941  VTE 1.3 integration
#François GAFFIE/RB657C       20/07/2004     TLSbo40898  OSS sound driver test development
#Guillaume SCHMUCK/GSCH1C         --             --         --
#Dmitriy Kazachkov/e1403c     29/07/2004     TLSbo40261  sdma_tests mtd_tests
#Ludovic DELASPRE/RC149C      02/08/2004     TLSbo40841  VTE 1.4 integration
#V.BECKER/rc023c              01/09/2004     TLSbo40417  irda_tests
#V.BECKER/rc023c              07/09/2004     TLSbo40423  mmc tests
#Dmitriy Kazachkov/e1403c     31/08/2004     TLSbo40411,418,419  added security, i2c, mu tests
#Ludovic DELASPRE/RC149C      16/09/2004     TLSbo42022  VTE 1.5 integration
#Igor SEMENCHUKOV/SMNG001C    01/10/2004     TLSbo41672  Add slcdc_tests
#Ludovic DELASPRE/RC149C      07/10/2004     TLSbo42821  VTE 1.5.1 integration
#Ludovic DELASPRE/RC149C      15/11/2004     TLSbo44180  VTE 1.6 integration
#L.Delaspre/rc149c            08/12/2004     TLSbo40142  update with Freescale identity
#L.Delaspre/rc149c            18/01/2005     TLSbo45643  VTE 1.7 integration
#L.Delaspre/rc149c            02/03/2005     TLSbo47726  VTE 1.8 integration
#I.Semenchukov/smng001c       01/04/2005     TLSbo47812  Added IPC tests
#L.Delaspre/rc149c            14/04/2005     TLSbo48760  VTE 1.9 integration
#S.V-Guilhou/svan01c          15/09/2005     TLSbo53979  VTE 1.13 integration
#O.Davard/b02578              27/04/2006     TLSbo58840  Added USB test suite
#O.Davard/b02578              03/05/2006     TLSbo68696  imx27ads platform added
#V.Khalabuda/b00306           14/07/2006     TLSbo61860  Option behaviour for IPC
#V.Khalabuda/b00306           12/09/2006     TLSbo76799  Update the iMX27 test suite environment
#O.DAVARD/b02578	      10/06/2006     TLSbo80013  Remove mc13783_tests folder
#D.Kardakov/ID                02/26/2007     TLSbo79826  vpu_tests folder added
#V.Khalabuda/b00306           12/09/2006     ENGR37140   Update the Carmel test suite environment
#D.Khoroshev/b00313           07/22/2007     engr42278   Added imx32ads and mxc30031ads
#Z.Spring                           11/24/2008      n/a             Add GPS module test cases
#Z.Spring                           11/27/2008      ENGR100588  Reconstruct Wifi test cases
#                                                        platforms
#H.Huang/b20222              09/01/2009      --          update for vte release
#Z.Spring                     03/31/2010     n/a         Add support for imx53
###############################################################################
#Portability: ARM GCC
#
###############################################################################
#SUBDIRS  = TEMPLATE_tests

#add tools(platfm.sh), 2009.1.15, by Spring
SUBDIRS += tools

ifneq ($(ARCH_PLATFORM), $(filter imx27ads imx233 imx28 imx27evb imx32ads, $(ARCH_PLATFORM)))
SUBDIRS += v4l_and_ipu_tests
endif

ifneq ($(ARCH_PLATFORM), $(filter imx27ads imx233 imx28 imx27evb imx32ads, $(ARCH_PLATFORM)))
SUBDIRS += sdma_tests
endif

ifeq ($(ARCH_PLATFORM), $(filter imx233, $(ARCH_PLATFORM)))
SUBDIRS += iim_test
endif


# ARM11 processor and the StarCore DSP existing on MXC91231 and MXC91331 platforms

ifeq ($(ARCH_PLATFORM), $(filter mxc27530evb mxc27520evb mxc30030evb mxc30030ads mxc30030ads mxc30031ads mxc30020evb, $(ARCH_PLATFORM)))
SUBDIRS += ipc_tests
endif

# Driver MU is present only in the MXC91331, MXC91231 and MXC91131 architectures.
ifeq ($(ARCH_PLATFORM), $(filter mxc30030evb mxc30030ads mxc30031ads mxc27530evb mxc27520evb mxc91131evb mxc30020evb, $(ARCH_PLATFORM)))
SUBDIRS += mu_tests
endif

ifneq ($(ARCH_PLATFORM), mxc27520evb)
SUBDIRS += mtd_tests
endif

# Updated by Blake, drop this directory
ifeq ($(ARCH_PLATFORM), $(filter imx32ads mxc30031ads, $(ARCH_PLATFORM)))
SUBDIRS += dvfs_dptc_tests
endif

ifeq ($(ARCH_PLATFORM), $(filter imx51stack imx51, $(ARCH_PLATFORM)))
SUBDIRS += power_manager
endif

#Hake mxc fb test extension
SUBDIRS += fb_test
SUBDIRS += mmc_tests
#tina changed SUBDIRS += irda_tests
SUBDIRS += usb_otg_tests
#SUBDIRS += oss_sound_driver_tests #spring deleted for aplay,arecord
SUBDIRS += pm_tests
#SUBDIRS += rm_tests
SUBDIRS += GPT_tests
SUBDIRS += rtc_tests
SUBDIRS += security_tests
#SUBDIRS += spi_tests
#SUBDIRS += ssi_dam_tests
SUBDIRS += uart_tests
SUBDIRS += slcdc_tests
#modified by Spring, mx35&37 need to update PMIC code, platform control in subfolder
SUBDIRS += pmic_tests

SUBDIRS += ts_tests

#modified by Spring, control in sub-Makefile
SUBDIRS += keypad_tests

#SUBDIRS += udma_tests
SUBDIRS += i2c_tests
SUBDIRS += watchdog_tests
SUBDIRS += owire
SUBDIRS += systest

ifeq ($(ARCH_PLATFORM), $(filter imx35stack imx25stack imx51stack, $(ARCH_PLATFORM)))
SUBDIRS += libata_tests
endif

ifeq ($(ARCH_PLATFORM), $(filter imx25stack imx35stack imx51, $(ARCH_PLATFORM)))
SUBDIRS += can_tests
endif

ifeq ($(ARCH_PLATFORM), $(filter imx25stack, $(ARCH_PLATFORM)))
SUBDIRS += usb_host_test
endif

ifeq ($(ARCH_PLATFORM), $(filter imx35stack, $(ARCH_PLATFORM)))
SUBDIRS += asrc_tests
SUBDIRS += cdrom_tests
SUBDIRS += mlb_tests
SUBDIRS += tvin_tests
endif

SUBDIRS += tvout_tests

ifeq ($(ARCH_PLATFORM), $(filter imx31stack, $(ARCH_PLATFORM)))
SUBDIRS += acc_tests
SUBDIRS += mbx_tests
endif

ifeq ($(ARCH_PLATFORM), $(filter imx37stack imx51stack imx51, $(ARCH_PLATFORM)))
SUBDIRS += vpu_tests
endif


ifeq ($(ARCH_PLATFORM), $(filter imx31stack imx35stack imx37stack imx51stack, $(ARCH_PLATFORM)))
SUBDIRS += ipudev_test
endif

ifeq ($(ARCH_PLATFORM), $(filter imx31stack imx51stack imx51, $(ARCH_PLATFORM)))
SUBDIRS += camera_test
endif

ifeq ($(ARCH_PLATFORM), $(filter imx35stack imx51stack imx51, $(ARCH_PLATFORM)))
SUBDIRS += gpu_test
endif

SUBDIRS += emi_tests
SUBDIRS += network_tests
#tina changed SUBDIRS += dpm_tests
SUBDIRS += io_tests


#ifneq ($(ARCH_PLATFORM), $(filter imx32ads mxc30020evb mxc30031ads, $(ARCH_PLATFORM)))
SUBDIRS += fec_tests
#endif

ifeq ($(ARCH_PLATFORM), $(filter imx27ads imx27evb imx31ads imx32ads, $(ARCH_PLATFORM)))
SUBDIRS += ata_tests
endif

ifeq ($(ARCH_PLATFORM), imx32ads)
#SUBDIRS += i2c_tests
endif

#ifeq ($(ARCH_PLATFORM),$(filter imx27ads imx27evb mxc30031ads imx32ads, $(ARCH_PLATFORM)))
#SUBDIRS += v4l_and_emma_tests
#endif

#added by Spring, 2008.7.25, 
#add GPS, 2008.11.24, add BT, 2008.11.26, add WIFI, 2008.11.27, 
#add FM, 2008.12.11, add SPDIF, 2009.5.21
SUBDIRS += alsa_tests
SUBDIRS += fs_tests
SUBDIRS += gps_tests
SUBDIRS += bt_tests
ifeq ($(ARCH_PLATFORM), $(filter imx25stack imx31stack imx35stack imx51stack, $(ARCH_PLATFORM)))
SUBDIRS += wifi_tests
endif
SUBDIRS += fm_tests
SUBDIRS += spdif_tests

#add by Victor
SUBDIRS += storage_tests

#added by Blake 
ifeq ($(ARCH_PLATFORM), $(filter imx25stack imx31stack imx37stack, $(ARCH_PLATFORM)))
SUBDIRS += dptc_tests
endif
#added by Blake
ifeq ($(ARCH_PLATFORM), $(filter imx25stack imx31stack imx35stack imx51stack imx51 imx233, $(ARCH_PLATFORM)))
SUBDIRS += dvfs_tests
endif
#added by Blake
ifeq ($(ARCH_PLATFORM), $(filter imx37stack imx51stack imx51, $(ARCH_PLATFORM)))
SUBDIRS += backlight_tests
endif

all:

	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i ; done

install:
	-mkdir -p ../bin
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i install ; done

clean:
	@set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i clean ; done
