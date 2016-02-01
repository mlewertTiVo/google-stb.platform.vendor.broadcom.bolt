# ***************************************************************************
# *     Copyright (c) 2012-2014, Broadcom Corporation
# *     All Rights Reserved
# *     Confidential Property of Broadcom Corporation
# *
# *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
# *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
# *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
# * 
# ***************************************************************************

# MEMC_GEN_0_CORE_REV_ID::DDR34_PHY_CONTROL_REGS_0_REVISION
#
ifeq ($(MEMC_REV),)
 MEMC_REV:=dummy
 FSBL_S += shmoo/dummy/lib/memsysinitlib.c
endif

ifeq ($(CFG_EMULATION),1)
 SHMOO_S =
 LIB_MEMSYS =
else
 ifneq ($(wildcard shmoo/$(MEMC_REV)/mcb-$(FAMILY).c),)
  SHMOO_S = shmoo/$(MEMC_REV)/mcb-$(FAMILY).c
 else
  # generic MCB
  SHMOO_S = shmoo/$(MEMC_REV)/mcb-common.c
 endif
endif

ifeq ($(CFG_NOSHMOO),1)
 FSBL_S += shmoo/memsys.c shmoo/memsys-$(FAMILY).c
endif

SHMOO_S += shmoo/shmoo_board.c

EXTRA_DEFINES += -Ishmoo/$(MEMC_REV)/include 

$(SHMOO_S):
