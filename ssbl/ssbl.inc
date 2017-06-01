# ***************************************************************************
# Broadcom Proprietary and Confidential. (c)2017 Broadcom. All rights reserved.
#
# THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
# AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
# EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# ***************************************************************************


SSBL_S += \
	ssbl/arch/arm-start.S \
	ssbl/arch/arm-config.S \
	ssbl/arch/arm-launch.S \
	ssbl/arch/arm-eabi-ssbl.S \
	ssbl/arch/arm-vectors.S \
	ssbl/arch/ssbl_main.c \
	ssbl/arch/memsys_ssbl.c \
	ssbl/main/zimage.c \
	ssbl/main/devtree.c \
	ssbl/main/dtshow.c \
	ssbl/main/dtbolt.c \
	ssbl/main/dtbolt_compile.c \

SSBL_S += \
	$(GEN)/dtc/libfdt/fdt.c \
	$(GEN)/dtc/libfdt/fdt_ro.c \
	$(GEN)/dtc/libfdt/fdt_wip.c \
	$(GEN)/dtc/libfdt/fdt_sw.c \
	$(GEN)/dtc/libfdt/fdt_rw.c \
	$(GEN)/dtc/libfdt/fdt_strerror.c 

ifeq ($(SECURE_BOOT),y)
SSBL_S += \
	ssbl/arch/ssbl_main_sec.c \
	ssbl/ui/ui_secureload.c \
	security/bsp_utils.c \
	security/mem_auth_new.c \
	security/decrypt_ssbl.c \
	security/xpt_dma.c \
	security/verify_kernel.c
endif

ifeq ($(CFG_ENET),1)
SSBL_S += \
	ssbl/dev/dev_bcmethsw.c \
	ssbl/dev/dev_bcmgphy.c
endif

ifeq ($(CFG_GENET),1)
SSBL_S += \
	ssbl/dev/dev_genet.c
endif

ifeq ($(CFG_SYSTEMPORT),1)
SSBL_S += \
	ssbl/dev/dev_systemport.c \
	ssbl/dev/dev_sf2.c
endif

ifeq ($(CFG_SATA),1)
SSBL_S += \
	ssbl/dev/sata/dev_sata.c \
	ssbl/dev/sata/um_ahci.c \
	ssbl/dev/sata/fifo.c \
	ssbl/dev/sata/mem_pool.c \
	ssbl/dev/sata/os.c \
	ssbl/dev/sata/stubs.c
endif

ifeq ($(CFG_USB),1)
SSBL_S += \
	ssbl/usb/usbd.c \
	ssbl/usb/usbdebug.c \
	ssbl/usb/usbdevs.c \
	ssbl/usb/usbeth.c \
	ssbl/usb/usbhid.c \
	ssbl/usb/usbhub.c \
	ssbl/usb/usbmain.c \
	ssbl/usb/usbmass.c \
	ssbl/usb/usbserial.c \
	ssbl/usb/usbdt.c \
	ssbl/usb/usb-brcm-common-init.c

ifeq ($(CFG_USB_EHCI_OHCI),1)
SSBL_S += \
	ssbl/usb/ohci.c \
	ssbl/usb/ehci.c
endif

ifeq ($(CFG_USB_XHCI),1)
SSBL_S += \
	ssbl/usb/xhci.c
endif

ifeq ($(CFG_USB_BDC),1)
SSBL_S += \
	ssbl/usb/bdc.c
endif
endif

SSBL_S += \
	$(GEN)/$(FAMILY)/rts.c \
	\
	ssbl/arch/bcm_devices.c \
	ssbl/arch/bcm_init_avs.c \
	ssbl/arch/bcm_init_enet.c \
	ssbl/arch/bcm_init_flash.c \
	ssbl/arch/bcm_init_hdmi.c \
	ssbl/arch/bcm_init_history.c \
	ssbl/arch/bcm_init_sata.c \
	ssbl/arch/bcm_init_rts.c \
	\
	ssbl/arch/board.c \
	ssbl/arch/arch-$(ARCH).c \
	$(GEN)/$(FAMILY)/board_params.c \
	ssbl/arch/board_update.c \
	ssbl/arch/common_init.c \
	ssbl/arch/exchandler.c \
	ssbl/arch/supplement-fsbl.c \
	thirdparty/disarm.c-0.11.c \
	ssbl/dev/dev_16550_uart.c \
	ssbl/dev/dev_mem.c \
	ssbl/dev/dev_nandflash.c \
	ssbl/dev/dev_norflash.c \
	ssbl/dev/dev_spiflash.c \
	ssbl/dev/macutils.c \
	ssbl/lib/hexdump.c \
	ssbl/lib/lib_ctype.c \
	ssbl/lib/lib_malloc.c \
	ssbl/lib/lib_misc.c \
	ssbl/lib/lib_printf.c \
	ssbl/lib/lib_qsort.c \
	ssbl/lib/lib_queue.c \
	ssbl/lib/lib_string2.c \
	ssbl/lib/lib_string.c \
	ssbl/lib/$(ARCH)-string.S \
	ssbl/lib/hmac_sha2.c \
	ssbl/lib/sha2.c \
	ssbl/main/attach.c \
	ssbl/main/background.c \
	ssbl/main/boot.c \
	ssbl/main/console.c \
	ssbl/main/devfuncs.c \
	ssbl/main/error.c \
	ssbl/main/fatfs.c \
	ssbl/main/parttbl.c \
	ssbl/main/filesys.c \
	ssbl/main/flash.c \
	ssbl/main/gisb.c \
	ssbl/main/iocb_dispatch.c \
	ssbl/main/ldr_elf.c \
	ssbl/main/ldr_raw.c \
	ssbl/main/ldr_srec.c \
	ssbl/main/ldr_zimg.c \
	ssbl/main/loader.c \
	ssbl/main/main.c \
	ssbl/main/macaddr.c \
	ssbl/main/rawfs.c \
	ssbl/main/reserve-memory.c \
	ssbl/main/savedata.c \
	ssbl/main/timer.c \
	ssbl/main/xreq.c \
	ssbl/main/zlibfs.c \
	ssbl/main/env_subr.c \
	ssbl/main/nvram_subr.c \
	ssbl/main/psci.c \
	ssbl/ui/ui_init.c \
	ssbl/ui/ui_cmddisp.c \
	ssbl/ui/ui_command.c \
	ssbl/ui/ui_devcmds.c \
	ssbl/ui/ui_envcmds.c \
	ssbl/ui/ui_examcmds.c \
	ssbl/ui/ui_flash.c \
	ssbl/ui/ui_loadcmds.c \
	ssbl/ui/ui_maccmds.c \
	ssbl/ui/ui_memcmds.c \
	ssbl/ui/ui_misccmds.c \
	ssbl/ui/ui_test_disk.c \
	ssbl/ui/ui_test_flash.c \
	ssbl/ui/ui_test_uart.c \
	ssbl/ui/ui_devtree.c \
	ssbl/ui/ui_memtest.c \
	ssbl/ui/ui_shacmds.c \
	ssbl/ui/ui_gpio.c

ifeq ($(CFG_EMMC_FLASH),1)
SSBL_S += \
	ssbl/dev/dev_emmcflash.c \
	ssbl/dev/dev_emmc_core.c \
	ssbl/dev/dev_emmc_rpmb.c
endif

ifeq ($(CFG_MONITOR_OVERTEMP),1)
SSBL_S += \
	ssbl/main/overtemp.c
endif

ifeq ($(CFG_ZLIB),1)
ifeq ($(ZLIB_VER),1.2.8)
SSBL_S += \
	$(GEN)/zlib-$(ZLIB_VER)/adler32.c \
	$(GEN)/zlib-$(ZLIB_VER)/crc32.c \
	$(GEN)/zlib-$(ZLIB_VER)/infback.c \
	$(GEN)/zlib-$(ZLIB_VER)/inffast.c \
	$(GEN)/zlib-$(ZLIB_VER)/inflate.c \
	$(GEN)/zlib-$(ZLIB_VER)/inftrees.c \
	$(GEN)/zlib-$(ZLIB_VER)/uncompr.c \
	$(GEN)/zlib-$(ZLIB_VER)/zutil.c
else
SSBL_S += \
	$(GEN)/zlib-$(ZLIB_VER)/adler32.c \
	$(GEN)/zlib-$(ZLIB_VER)/infblock.c \
	$(GEN)/zlib-$(ZLIB_VER)/infcodes.c \
	$(GEN)/zlib-$(ZLIB_VER)/inffast.c \
	$(GEN)/zlib-$(ZLIB_VER)/inflate.c \
	$(GEN)/zlib-$(ZLIB_VER)/inftrees.c \
	$(GEN)/zlib-$(ZLIB_VER)/infutil.c \
	$(GEN)/zlib-$(ZLIB_VER)/uncompr.c \
	$(GEN)/zlib-$(ZLIB_VER)/zutil.c
endif
endif

ifeq ($(CFG_NETWORK),1)
SSBL_S += \
	ssbl/net/net_api.c \
	ssbl/net/net_arp.c \
	ssbl/net/net_dhcp.c \
	ssbl/net/net_dns.c \
	ssbl/net/net_ether.c \
	ssbl/net/net_icmp.c \
	ssbl/net/net_ip.c \
	ssbl/net/net_tftp.c \
	ssbl/net/net_udp.c \
	ssbl/ui/ui_netcmds.c \
	ssbl/ui/ui_test_ether.c \
	ssbl/net/net_mdio.c
endif

ifeq ($(CFG_TCP),1)
SSBL_S += \
	ssbl/net/net_tcpbuf.c \
	ssbl/net/net_tcp.c \
	ssbl/ui/ui_tcpcmds \
	ssbl/net/dev_tcpconsole.c
endif
ifeq ($(CFG_SSDP),1)
SSBL_S += \
	ssbl/net/net_ssdp.c \
	ssbl/net/net_socket.c \
	ssbl/ui/ui_ssdpcmds
endif
ifeq ($(CFG_COAP),1)
SSBL_S += \
	ssbl/ui/ui_coapcmds.c \
	$(GEN)/libcoap-$(COAP_VER)/pdu.c \
	$(GEN)/libcoap-$(COAP_VER)/net.c \
	$(GEN)/libcoap-$(COAP_VER)/debug.c \
	$(GEN)/libcoap-$(COAP_VER)/encode.c \
	$(GEN)/libcoap-$(COAP_VER)/uri.c \
	$(GEN)/libcoap-$(COAP_VER)/resource.c \
	$(GEN)/libcoap-$(COAP_VER)/hashkey.c \
	$(GEN)/libcoap-$(COAP_VER)/option.c \
	$(GEN)/libcoap-$(COAP_VER)/async.c \
	$(GEN)/libcoap-$(COAP_VER)/subscribe.c \
	$(GEN)/libcoap-$(COAP_VER)/block.c \
	$(GEN)/libcoap-$(COAP_VER)/brcm/coap_api.c \
	$(GEN)/libcoap-$(COAP_VER)/brcm/osal.c \
	$(GEN)/libcoap-$(COAP_VER)/brcm/handler_common.c \
	$(GEN)/libcoap-$(COAP_VER)/brcm/handler_env.c \
	$(GEN)/libcoap-$(COAP_VER)/brcm/handler_memory.c \
	$(GEN)/libcoap-$(COAP_VER)/brcm/handler_misc.c
endif
ifeq ($(CFG_BSU),1)
SSBL_S += \
	ssbl/main/bsu.c
endif

ifeq ($(S_UNITTEST),y)
SSBL_S += \
	ssbl/main/bolt_aegis.c
endif

ifeq ($(CFG_TRUSTZONE_CMD),1)
include ssbl/trustzone/tz.inc
endif

$(SSBL_S):
