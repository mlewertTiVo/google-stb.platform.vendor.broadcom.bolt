# ***************************************************************************
# * Broadcom Proprietary and Confidential. (c)2016 Broadcom. All rights reserved.
# *
# *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
# *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
# *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
# *
# ***************************************************************************

STUB_S += \
	stub64/stub-start.64.S \
	stub64/stub-gic.64.S \
	stub64/stub-uart.64.c \
	stub64/stub-pm.64.c \

# Installed by SSBL to handle EL3 exceptions
SMM_S += \
	stub64/psci-vectors.64.S \
	stub64/stub-uart.64.c \
	stub64/psci-minilib.64.c \
	stub64/psci-powerctl.64.c \
	stub64/psci-core.64.c \
	stub64/stub-gic.64.S \
	stub64/bakery_lock_normal.64.c \
	stub64/psci-cache.64.c

SMM_S += \
	stub64/smm-setparms.64.c
