# ***************************************************************************
# * Broadcom Proprietary and Confidential. (c)2016 Broadcom. All rights reserved.
# *
# *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
# *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
# *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
# *
# ***************************************************************************

# Installed by SSBL to handle MON exceptions

PSCI32_S += \
	psci32/psci32-vectors.S \
	psci32/secure-gic.S \
	psci32/psci32-minilib.c \
	psci32/psci32-uart.c \
	psci32/psci32-powerctl.c \
	psci32/psci32-cache.c \
	psci32/bakery_lock_normal.c \
	psci32/psci32-core.c
