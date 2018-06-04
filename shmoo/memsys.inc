# ***************************************************************************
# Broadcom Proprietary and Confidential. (c)2018 Broadcom. All rights reserved.
#
# THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
# AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
# EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# ***************************************************************************

MEMSYS_S = shmoo/memsys_wrapper.c $(wildcard $(GEN)/$(FAMILY)/memsys-fixed*.c)

ifeq ($(CFG_EMULATION),1)
 LIB_MEMSYS =
else
 ifneq ($(MEMC_REV),dummy)
  ifneq ($(wildcard $(GEN)/$(FAMILY)/memsys-fixed-alt.c),)
    LIB_MEMSYS = $(ROOT)/shmoo/$(MEMC_ALT)/lib/memsysinitlib.a
  else
    LIB_MEMSYS = $(ROOT)/shmoo/$(MEMC_REV)/lib/memsysinitlib.a
  endif
  $(info STD MEMSYS: $(subst ./shmoo/,,$(LIB_MEMSYS)))
 endif
 ifneq ($(MEMC_ALT),)
  MEMSYS_ALT_S = shmoo/memsys_wrapper.c
  MEMSYS_ALT_LIB = $(ROOT)/shmoo/$(MEMC_ALT)/lib/memsysinitlib.a
  $(info ALT MEMSYS: $(subst ./shmoo/,,$(MEMSYS_ALT_LIB)))
 endif
endif

$(SHMOO_S):
