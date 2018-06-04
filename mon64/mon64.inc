# ***************************************************************************
# Broadcom Proprietary and Confidential. (c)2017 Broadcom. All rights reserved.
#
# THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
# AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
# EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# ***************************************************************************

EXTRA_DEFINES += -Imon64

MON64_SSBL_S = \
	mon64/mon64.c \
	mon64/ui_mon64.c

ifeq ($(TZ_SECURE_BOOT),y)
MON64_SSBL_S += \
	security/verify_mon64.c
endif

ifeq ($(TZ_SECURE_BOOT),y)
MON64_BIN_S = mon64/mon64_secure.bin
else
MON64_BIN_S = mon64/mon64.bin
endif
