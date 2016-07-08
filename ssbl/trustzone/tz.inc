# ***************************************************************************
# Broadcom Proprietary and Confidential. (c)2016 Broadcom. All rights reserved.
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

# TZ files limited to this dir as much as is possible
EXTRA_DEFINES += \
	-I$(ROOT)/ssbl/trustzone
