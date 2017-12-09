# ***************************************************************************
# Broadcom Proprietary and Confidential. (c)2017 Broadcom. All rights reserved.
#
# THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
# AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
# EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# ***************************************************************************

famcheck: family_dirs
	$(Q)if [ "$(FAMILY)" = "unknown" ]; then \
		echo; \
		echo "For the first build, please specify the chip, e.g. 'make 7445d0'"; \
		echo; \
		false; \
	else \
		if [ "$(FAMILY)" != "$(SAVED_FAMILY)" ] ; then \
			echo -n $(FAMILY) > $(GEN)/.family ; \
		fi \
	fi; \
	if [ ! -f "$(CFG)" ]; then \
		echo; \
		echo "Config file $(CFG) not found"; \
		echo; \
		false; \
	else \
		if [ "$(CFG)" != "$(SAVED_CFG)" ] ; then \
			echo -n $(CFG) > $(GEN)/.config ; \
		fi \
	fi

stubpatch = \
	$(Q)if [ -f $(ODIR)/stub.64.bin ]; then \
		echo "  PATCH   $(ODIR)/stub.64.bin"; \
		scripts/bpatch.pl -a $(STUB64_OFFSET) \
		-p $(ODIR)/stub.64.bin \
		-i $(ODIR)/bolt.bin \
		-o $(ODIR)/bolt.bin > /dev/null 2>&1; \
	fi

smmpatch = \
	$(Q)if [ -f $(ODIR)/smm.64.bin ]; then \
		echo "  PATCH   $(ODIR)/smm.64.bin"; \
		scripts/bpatch.pl -a $(SMM64_OFFSET) \
		-p $(ODIR)/smm.64.bin \
		-i $(ODIR)/bolt.bin \
		-o $(ODIR)/bolt.bin > /dev/null 2>&1; \
	fi

ssbmpatch = \
	$(Q)if [ -f $(ODIR)/ssbm.bin ]; then \
		echo "  PATCH   $(ODIR)/ssbm.bin"; \
		scripts/bpatch.pl -a $(SSBM_TEXT_OFFS) \
		-p $(ODIR)/ssbm.bin \
		-i $(ODIR)/bolt.bin \
		-o $(ODIR)/bolt.bin > /dev/null 2>&1; \
	fi

ddr_update: $(GEN)/scripts/mcb2c
	$(Q)rm -f shmoo/$(MEMC_REV)/mcb-$(FAMILY).c
	$(Q)$(GEN)/scripts/mcb2c -c -i $(FAMILY) -m shmoo/$(MEMC_REV)/mcb/ \
		-o shmoo/$(MEMC_REV)/mcb-$(FAMILY).c

# append an 's' to the chip family e.g. make 7445d0s
_VERSWITCH:=%a0s %b0s s%c0s %d0s %e0s s%f0s

$(_VERSWITCH): FORCE
	$(Q)echo $@ | sed 's/s//;' > $(GEN)/.family
	$(Q)echo "FAMILY is now" `cat $(GEN)/.family`

boardinfo: FORCE
	$(Q)scripts/boardinfo.sh $(FAMILIES)

ifneq ($(MAKECMDGOALS),distclean)
  ifneq (,$(wildcard $(GEN)/$(FAMILY)/include.mk))
    include $(GEN)/$(FAMILY)/include.mk
  endif
endif

# BRCM internal, not expected to work for you.

# load SSBL directly into  ddr
# ----------------------------
#  This is for development only as it
# relies on brcm linkages to the JTAG unit via SOAP
# 1. make 7445d0 SOFTLOAD=1
# 2. Program the FSBL into flash
# 3. make softload BCM_JTAG=<IPADDR>
softload: $(ODIR)/bolt.elf
	$(Q)rm -f $(ODIR)/softload.elf
	$(Q)cp $(ODIR)/bolt.elf $(ODIR)/softload.elf
	$(Q)$(STRIP) --remove-section=.fsbl $(ODIR)/softload.elf > /dev/null 2>&1
	$(Q)$(STRIP) --remove-section=.avs_fw $(ODIR)/softload.elf > /dev/null 2>&1
	$(Q)$(STRIP) --remove-section=.bbl $(ODIR)/softload.elf > /dev/null 2>&1
	$(Q)$(STRIP) --remove-section=.bfw $(ODIR)/softload.elf > /dev/null 2>&1
	$(Q)$(STRIP) --remove-section=.memsys $(ODIR)/softload.elf > /dev/null 2>&1
	$(Q)$(STRIP) --remove-section=.shmoo $(ODIR)/softload.elf > /dev/null 2>&1
	$(Q)$(STRIP) --remove-section=.ARM.attributes $(ODIR)/softload.elf > /dev/null 2>&1
	$(Q)$(STRIP) --remove-section=.comment $(ODIR)/softload.elf > /dev/null 2>&1
	$(Q)$(OBJDUMP) -h $(ODIR)/softload.elf
	$(Q)$(OBJCOPY) -I $(ARCH_OFORMAT) -O binary $(ODIR)/softload.elf $(ODIR)/softload.bin
	$(Q)./scripts/softload.pl -file $(ODIR)/softload.bin -ip=$(BCM_JTAG) -addr=$(SSBL_RAM_ADDR)

# launch FSBL from BOLT
# ---------------------
#  This is for development only as the hardware
# is not in a default reset state. Boot with:
# boot -elf <IPADDR>:sfsbl.elf
sfsbl: $(ODIR)/fsbl.elf
	$(Q)rm -f $(ODIR)/sfsbl.elf
	$(Q)cp $(ODIR)/fsbl.elf $(ODIR)/sfsbl.elf
	$(Q)$(STRIP) --remove-section=.ARM.attributes $(ODIR)/sfsbl.elf > /dev/null 2>&1
	$(Q)$(STRIP) --remove-section=.comment $(ODIR)/sfsbl.elf > /dev/null 2>&1
	$(Q)$(OBJCOPY) --change-section-vma .fsbl=$(SRAM_ADDR) $(ODIR)/sfsbl.elf $(ODIR)/sfsbl.elf
	$(Q)$(OBJCOPY) --set-start=$(SRAM_ADDR) $(ODIR)/sfsbl.elf $(ODIR)/sfsbl.elf
	$(Q)$(OBJDUMP) -h $(ODIR)/sfsbl.elf

softwipe:
	$(Q)./scripts/softload.pl -wipeonly -ip=$(BCM_JTAG) -addr=$(SSBL_RAM_ADDR)

sec_clean:
	$(Q)find security -name \*.a | xargs rm -f

sec_show:
	$(Q)find security -name \*.a  -exec echo {} \; -exec $(AR) -t {} \; -exec echo "" \;

# add layouts cfg file deps as the cfg script will validate it for us beforehand.
$(GEN)/scripts/patcher.pl: $(ROOT)/config/layout*.cfg $(ROOT)/scripts/gen_layouts.sh $(ROOT)/scripts/bpatch.pl
	$(call pretty_print,APP,$@)
	$(Q) if [ -e $(GEN)/scripts/patcher.pl ]; then \
               rm -f $(GEN)/scripts/patcher.pl; \
             fi; \
             $(ROOT)/scripts/gen_layouts.sh > $(GEN)/scripts/patcher.pl && \
                     chmod a+x $(GEN)/scripts/patcher.pl;

.PHONY: softload softwipe boardinfo sfsbl sec_clean sec_show
