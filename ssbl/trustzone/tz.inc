# ***************************************************************************
# Broadcom Proprietary and Confidential. (c)2018 Broadcom. All rights reserved.
#
# THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
# AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
# EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# ***************************************************************************

SSBL_S += \
	ssbl/trustzone/ui_tz.c \
	ssbl/trustzone/tz_main.c \
	ssbl/trustzone/tz_dt.c \
	ssbl/trustzone/tz_boot.c \
	ssbl/trustzone/tz_config.c \
	$(GEN)/dtc/libfdt/fdt_empty_tree.c

ifneq ($(CFG_MON64),1)
SSBL_S += \
	ssbl/trustzone/tz_smm.c
endif

ifeq ($(TZ_SECURE_BOOT),y)
SSBL_S += \
	security/verify_tz.c
endif

# TZ files limited to this dir as much as is possible
EXTRA_DEFINES += \
	-I$(ROOT)/ssbl/trustzone
