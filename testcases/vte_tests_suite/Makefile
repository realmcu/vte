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
#H.H/b20222					  07/16/2010	n/a			 use config_* define to build
###############################################################################
#Portability: ARM GCC
#
###############################################################################

include ./vte_configs/${ARCH_PLATFORM}_deconfigs


SUBDIRS-y                         += tools
SUBDIRS-y                         += kernel_tests
SUBDIRS-y                         += tool_chain_test 
SUBDIRS-y                         += runtests
SUBDIRS-$(CONFIG_VTE_DATA)        += test_data
SUBDIRS-$(CONFIG_VTE_SENSOR)      += sensor_tests
SUBDIRS-$(CONFIG_VTE_ALSA)        += alsa_tests
SUBDIRS-$(CONFIG_VTE_ASRC)        += asrc_tests
SUBDIRS-$(CONFIG_VTE_ATA)         += ata_tests
SUBDIRS-$(CONFIG_VTE_BL)          += backlight_tests
SUBDIRS-$(CONFIG_VTE_BT)          += bt_tests
SUBDIRS-$(CONFIG_VTE_CAMERA)      += camera_test
SUBDIRS-$(CONFIG_VTE_CAN)         += can_tests
SUBDIRS-$(CONFIG_VTE_CDROM)       += cdrom_tests
SUBDIRS-$(CONFIG_VTE_DPM)         += dpm_tests
SUBDIRS-$(CONFIG_VTE_DPTC)        += dptc_tests
SUBDIRS-$(CONFIG_VTE_DVFS_DPTC)   += dvfs_dptc_tests
SUBDIRS-$(CONFIG_VTE_DVFS)        += dvfs_tests
SUBDIRS-$(CONFIG_VTE_EMI)         += emi_tests
SUBDIRS-$(CONFIG_VTE_EPDC)        += epdc_tests
SUBDIRS-$(CONFIG_VTE_EPXP)        += epxp_test
SUBDIRS-$(CONFIG_VTE_ESAI)        += esai_tests
SUBDIRS-$(CONFIG_VTE_FB)          += fb_test
SUBDIRS-$(CONFIG_VTE_FEC)         += fec_tests
SUBDIRS-$(CONFIG_VTE_FM)          += fm_tests
SUBDIRS-$(CONFIG_VTE_FS)          += fs_tests
SUBDIRS-$(CONFIG_VTE_GPT)         += GPT_tests
SUBDIRS-$(CONFIG_VTE_GPU)         += gpu_test
SUBDIRS-$(CONFIG_VTE_GPS)         += gps_tests
SUBDIRS-$(CONFIG_VTE_I2C)         += i2c_tests
SUBDIRS-$(CONFIG_VTE_IIM)         += iim_test
SUBDIRS-$(CONFIG_VTE_IO)          += io_tests
SUBDIRS-$(CONFIG_VTE_IPC)         += ipc_tests
SUBDIRS-$(CONFIG_VTE_IPUDEV)      += ipudev_test
SUBDIRS-$(CONFIG_VTE_IRDA)        += irda_tests
SUBDIRS-$(CONFIG_VTE_KEYPAD)      += keypad_tests
SUBDIRS-$(CONFIG_VTE_LIBATA)      += libata_tests
SUBDIRS-$(CONFIG_VTE_MBX)         += mbx_tests
SUBDIRS-$(CONFIG_VTE_MLB)         += mlb_tests
SUBDIRS-$(CONFIG_VTE_MMC)         += mmc_tests
SUBDIRS-$(CONFIG_VTE_MTD)         += mtd_tests
SUBDIRS-$(CONFIG_VTE_MU)          += mu_tests
SUBDIRS-$(CONFIG_VTE_NETWORK)     += network_tests
SUBDIRS-$(CONFIG_VTE_OSS)         += oss_sound_driver_tests
SUBDIRS-$(CONFIG_VTE_OPT)         += otp_test
SUBDIRS-$(CONFIG_VTE_1W)          += owire
SUBDIRS-$(CONFIG_VTE_PM)          += pm_tests
SUBDIRS-$(CONFIG_VTE_PMIC)        += pmic_tests
SUBDIRS-$(CONFIG_VTE_POWER)       += power_manager
SUBDIRS-$(CONFIG_VTE_PTP)         += ptp_tests
SUBDIRS-$(CONFIG_VTE_RTC)         += rtc_tests
SUBDIRS-$(CONFIG_VTE_SDMA)        += sdma_tests
SUBDIRS-$(CONFIG_VTE_SEC)         += security_tests
SUBDIRS-$(CONFIG_VTE_SLCDC)       += slcdc_tests
SUBDIRS-$(CONFIG_VTE_SMP)         += smp_tests
SUBDIRS-$(CONFIG_VTE_SPDIF)       += spdif_tests
SUBDIRS-$(CONFIG_VTE_SPI)         += spi_tests
SUBDIRS-$(CONFIG_VTE_SSI_DMA)     += ssi_dam_tests
SUBDIRS-$(CONFIG_VTE_STORAGE)     += storage_tests
SUBDIRS-$(CONFIG_VTE_STRESS)      += stress_tests 
SUBDIRS-$(CONFIG_VTE_SYS)         += systest
SUBDIRS-$(CONFIG_VTE_TIMER)       += timer_test
SUBDIRS-$(CONFIG_VTE_TS)          += ts_tests
SUBDIRS-$(CONFIG_VTE_TVIN)        += tvin_tests
SUBDIRS-$(CONFIG_VTE_TVOUT)       += tvout_tests
SUBDIRS-$(CONFIG_VTE_UART)        += uart_tests
SUBDIRS-$(CONFIG_VTE_UDMA)        += udma_tests
SUBDIRS-$(CONFIG_VTE_USB_HOST)    += usb_host_test
SUBDIRS-$(CONFIG_VTE_USB_OTG)     += usb_otg_tests
SUBDIRS-$(CONFIG_VTE_V4L_IPU)     += v4l_and_ipu_tests
SUBDIRS-$(CONFIG_VTE_V4L_EMMA)    += v4l_and_emma_tests
SUBDIRS-$(CONFIG_VTE_V4L_PXP)     += v4l_and_pxp_tests
SUBDIRS-$(CONFIG_VTE_VPU)         += vpu_tests
SUBDIRS-$(CONFIG_VTE_WD)          += watchdog_tests
SUBDIRS-$(CONFIG_VTE_WIFI)        += wifi_tests

all:
	@set -e; for i in $(SUBDIRS-y); do $(MAKE) -C $$i ; done

install:
	-mkdir -p ../bin; for i in $(SUBDIRS-y); do $(MAKE) -C $$i install ; done 

clean:
	@set -e; for i in $(SUBDIRS-y); do $(MAKE) -C $$i clean ; done
