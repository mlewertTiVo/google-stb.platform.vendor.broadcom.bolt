# ***************************************************************************
# Broadcom Proprietary and Confidential. (c)2017 Broadcom. All rights reserved.
#
# THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
# AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
# EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# ***************************************************************************

# MEMC_GEN_0_CORE_REV_ID::DDR34_PHY_CONTROL_REGS_0_REVISION
#
ifeq ($(MEMC_REV),)
 MEMC_REV:=dummy
 FSBL_S += shmoo/dummy/lib/memsysinitlib.c
endif

ifeq ($(MEMC_REV),B220_H00)
 SHMOO_API := V5
endif

ifeq ($(MEMC_REV),B300_H00)
 SHMOO_API := V5
endif

ifeq ($(MEMC_REV),B310_G00)
 SHMOO_API := V5
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

ifeq ($(SHMOO_API),V5)
 API_DIR := api-v5
else
 API_DIR := api-v1
endif

ifeq ($(CFG_NOSHMOO),1)
 FSBL_S += shmoo/noshmoo.c
 EXTRA_DEFINES += -Ishmoo/dummy
 ifneq ($(CFG_EMULATION),1)
  ifneq ($(CFG_FULL_EMULATION),1)
   FSBL_S += shmoo/memsys-$(FAMILY).c
  endif
 endif
else
 FSBL_S += shmoo/$(API_DIR)/memsys.c
 EXTRA_DEFINES += -Ifsbl
 EXTRA_DEFINES += -Ishmoo/$(API_DIR)
endif

SHMOO_S += shmoo/shmoo_board.c

EXTRA_DEFINES += -Ishmoo/$(MEMC_REV)/include

$(SHMOO_S):
