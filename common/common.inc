# ***************************************************************************
# *     Copyright (c) 2012-2015, Broadcom Corporation
# *     All Rights Reserved
# *     Confidential Property of Broadcom Corporation
# *
# *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
# *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
# *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
# *
# ***************************************************************************

include build/tools.mk

ifeq ($(ARCH),arm)
COMMON_S += \
	common/arch-mmu.c \
	common/arm-cache.c \
	common/arm-ddr.c

# S3 or H/W SHA
ifeq ($(filter $(CFG_HARDWARE_SHA) $(CFG_PM_S3), 1),1)
COMMON_S += \
	common/xpt_dma.c
endif
endif
