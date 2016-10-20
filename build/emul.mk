# ***************************************************************************
# Broadcom Proprietary and Confidential. (c)2016 Broadcom. All rights reserved
#
# THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
# AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
# EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# ***************************************************************************

# Certain chip specific emulation environments require
# images to be endian swapped.
SWAPCHIPS:=

ifeq ($(BOOT_START_ADDRESS),)
	$(error include build/emul.mk only after gen/$$(FAMILY)/include.mk)
endif

ifeq ($(CFG_EMULATION),1)
# simplified FSBL can launch 32-bit kernel only, which should be at 0x8000
JUMP_TARGET ?= 0x8000
else
JUMP_TARGET ?= $(BOOT_START_ADDRESS)
endif
DTB_BASE ?= 0xF00000
CFG_EMULATION_JUMP_TARGET := $(JUMP_TARGET)
CFG_EMULATION_DTB_BASE := $(DTB_BASE)

ifeq ($(CFG_EMULATION),1)
# CFG_EMULATION_JUMP_TARGET and CFG_EMULATION_DTB_BASE are referenced
# in code for FSBL only emulation build
EXTRA_DEFINES += -DCFG_EMULATION_JUMP_TARGET=$(CFG_EMULATION_JUMP_TARGET)
EXTRA_DEFINES += -DCFG_EMULATION_DTB_BASE=$(CFG_EMULATION_DTB_BASE)
endif

ifeq ($(VMLINUZ),)
emu_fsbl: $(ODIR)/fsbl.srec

emu_bolt: $(ODIR)/bolt.srec

else
ifeq ($(SWAPIMG),)
emu_fsbl: $(ODIR)/fsbl-img-dtb.srec

emu_bolt: $(ODIR)/bolt-img-dtb.srec

else
emu_fsbl: $(ODIR)/fsbl-img-dtb.swp.srec

emu_bolt: $(ODIR)/bolt-img-dtb.swp.srec

endif
endif

define EMUSWP
$(1): $(2)
ifeq ($(filter $(FAMILY), $(SWAPCHIPS)), $(FAMILY))
	$$(call pretty_print,"SWAP",$$@)
	$$(Q)perl -e 'undef $$$$/; $$$$x=<STDIN>."\0\0\0"; \
		print pack("N*", unpack("V*",$$$$x));' \
		< $$< \
		> $$@
else
	$$(call pretty_print,"ASIS",$$@)
	$$(Q)perl -e 'undef $$$$/; $$$$x=<STDIN>."\0\0\0"; \
		print pack("N*", unpack("N*",$$$$x));' \
		< $$< \
		> $$@
endif
endef

define MAKE_SREC
$(1): $(2)
	$$(call pretty_print,"SREC",$$@)
	$$(Q)$$(OBJCOPY) -I binary -O srec --srec-forceS3 \
		--adjust-vma=$(3) -S $$< $$@
endef

# "Super-SREC" with JUMP_TARGET=0x8000 and DTB_BASE=0xF00000
# +-----------------+
# | S01E00006F62... |
# | S315E0000000... | FSBL/BOLT
# | ...             |
# | S705E0000000..  |
# + S01B00006F62... +
# | S31500008000... | Linux (optional)
# | ...             |
# | S70500008000..  |
# + S01E00006F62... +
# | S31500F00000... | DTB (optional, but required with Linux)
# | ...             |
# | S70500F00000..  |
# +-----------------+
#
# Memory map in Veloce
# +---------------------+
# |      FSBL/BOLT      | => 0xe000_0000 (FSBL_TEXT_ADDR from
# +---------------------+                       'gen/$(FAMILY)/include.mk')
# |    DTB (swapped)    | => 0x00F0_0000 $(DTB_BASE)
# +---------------------+
# |   image (swapped)   | => 0x0000_8000 $(JUMP_TARGET)
# +---------------------+

$(ODIR)/bolt.srec: postpatch
	$(call pretty_print,"SREC",$@)
# NO BSECK or BSECK-B only
# The function $(wildcard ) does not work for checking bolt-??.bin
# because bolt-??.bin is generated outside make.
	$(Q)if [ -r $(ODIR)/bolt-xx.bin ] ; \
	then \
		$(OBJCOPY) --adjust-vma=$(FSBL_TEXT_ADDR) --srec-forceS3 \
			-I binary -O srec $(ODIR)/bolt-xx.bin $@ ; \
	else \
		$(OBJCOPY) --adjust-vma=$(FSBL_TEXT_ADDR) --srec-forceS3 \
			-I binary -O srec $(ODIR)/bolt-bb.bin $@ ; \
	fi

$(eval $(call MAKE_SREC,$(ODIR)/fsbl.srec,$(ODIR)/fsbl.bin,$(FSBL_TEXT_ADDR)))
$(eval $(call EMUSWP,$(ODIR)/img.bin.swp,$(VMLINUZ)))
$(eval $(call MAKE_SREC,$(ODIR)/img.bin.swp.srec,$(ODIR)/img.bin.swp,$(CFG_EMULATION_JUMP_TARGET)))
$(eval $(call MAKE_SREC,$(ODIR)/img.bin.srec,$(VMLINUZ),$(CFG_EMULATION_JUMP_TARGET)))
$(eval $(call EMUSWP,$(ODIR)/config.dtb.swp,$(ODIR)/config.dtb))
$(eval $(call MAKE_SREC,$(ODIR)/config.dtb.swp.srec,$(ODIR)/config.dtb.swp,$(CFG_EMULATION_DTB_BASE)))
$(eval $(call MAKE_SREC,$(ODIR)/config.dtb.srec,$(ODIR)/config.dtb,$(CFG_EMULATION_DTB_BASE)))

$(ODIR)/fsbl-img-dtb.swp.srec: $(ODIR)/fsbl.srec $(ODIR)/img.bin.swp.srec $(ODIR)/config.dtb.swp.srec
	$(call pretty_print,"CAT",$@)
	$(Q)cat $^ > $@

$(ODIR)/fsbl-img-dtb.srec: $(ODIR)/fsbl.srec $(ODIR)/img.bin.srec $(ODIR)/config.dtb.srec
	$(call pretty_print,"CAT",$@)
	$(Q)cat $^ > $@

$(ODIR)/bolt-img-dtb.swp.srec: $(ODIR)/bolt.srec $(ODIR)/img.bin.swp.srec $(ODIR)/config.dtb.swp.srec
	$(call pretty_print,"CAT",$@)
	$(Q)cat $^ > $@

$(ODIR)/bolt-img-dtb.srec: $(ODIR)/bolt.srec $(ODIR)/img.bin.srec $(ODIR)/config.dtb.srec
	$(call pretty_print,"CAT",$@)
	$(Q)cat $^ > $@

