# ***************************************************************************
# *     Copyright (c) 2012-2013, Broadcom Corporation
# *     All Rights Reserved
# *     Confidential Property of Broadcom Corporation
# *
# *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
# *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
# *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
# * 
# ***************************************************************************

ifneq ($(V),1)
Q := @
pretty_print = @printf "  %-7s %s\n" $(1) $(2)
else
pretty_print =
endif

src2obj = $(sort $(addprefix $(ODIR)/,$(addsuffix .o,$(basename $(1)))))

rootincs = $(addprefix -I$(ROOT)/,$(1))

# autodepend recipe from: http://scottmcpeak.com/autodepend/autodepend.html
gen_autodep = \
	@$(CC) -MM $(3) $(2) > $(ODIR)/$(1).d.tmp; \
	sed -e 's|.*:|$(ODIR)/$(1).o:|' < $(ODIR)/$(1).d.tmp > \
        $(ODIR)/$(1).d; \
	sed -e 's/.*://' -e 's/\\$$//' < $(ODIR)/$(1).d.tmp | \
        fmt -1 | \
	sed -e 's/^ *//' -e 's/$$/:/' >> \
        $(ODIR)/$(1).d; \
	rm -f $(ODIR)/$(1).d.tmp


.PHONY: FORCE

$(ODIR)/%.o: %.c
	$(call pretty_print,CC,$@)
	$(Q)[ -d $(dir $@) ] || mkdir -p $(dir $@)
	$(call gen_autodep,$*,$<,$(CFLAGS))
	$(Q)$(CC) $(SAVE_TEMPS) -c $(CFLAGS) $(CFLAGS_$<) $< -o $@ -I$(dir $<)

$(ODIR)/%.o: %.S
	$(call pretty_print,AS,$@)
	$(Q)[ -d $(dir $@) ] || mkdir -p $(dir $@)
	$(call gen_autodep,$*,$<,$(AFLAGS))
	$(Q)$(CC) $(SAVE_TEMPS) -c $(AFLAGS) $(AFLAGS_$<) $< -o $@ -I$(dir $<)

%-stripped.o: %.o
	$(call pretty_print,STRIP,$@)
	$(Q)$(STRIP) --strip-all $< -o $@

%.bin: %.elf
	$(call pretty_print,OBJCOPY,$@)
	$(Q)$(OBJCOPY) --gap-fill $(FILLB) -O binary $< $@

%.asm: %.elf
	$(call pretty_print,OBJDUMP,$@)
	$(Q)rm -f $@
	$(Q)$(OBJDUMP) -Sd $< > $@

%.srec: %.elf
	$(call pretty_print,"SREC",$@)
	$(Q)$(OBJCOPY) -I $(ARCH_OFORMAT) -O srec $< $@

%_flash.srec: %.srec %.asm
	$(call pretty_print,"ADJ",$@)
	$(Q)$(OBJCOPY) --set-start=$(FSBL_TEXT_ADDR) -I srec -O srec $< $@
	$(Q)if command -v srec_info &> /dev/null ; then \
             srec_info $@; \
            fi

%.html: %.txt
	$(call pretty_print,HTML,$@)
	$(Q)asciidoc --section-numbers --doctype=book --out-file=$@ $<

define EMUSWP
$(1): $(2)
ifeq ($(FAMILY),3390a0)
	$$(call pretty_print,"ASIS",$$@)
	$$(Q)perl -e 'undef $$$$/; $$$$x=<STDIN>."\0\0\0"; \
		print pack("N*", unpack("N*",$$$$x));' \
		< $$< \
		> $$@
else
	$$(call pretty_print,"SWAP",$$@)
	$$(Q)perl -e 'undef $$$$/; $$$$x=<STDIN>."\0\0\0"; \
		print pack("N*", unpack("V*",$$$$x));' \
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

# "Super-SREC" map        => Veloce memory map
# ----------------           -----------------
# +---------------------+
# |         FSBL        | => 0xe000_0000 (flash)
# +---------------------+
# |   image (swapped)   | => 0x0000_8000 (DRAM)
# +---------------------+
# |    DTB (swapped)    | => 0x0000_1000 (DRAM)
# +---------------------+

$(eval $(call EMUSWP,$(ODIR)/img.bin.swp,$(VMLINUZ)))
$(eval $(call EMUSWP,$(ODIR)/config.dtb.swp,$(ODIR)/config.dtb))
$(eval $(call MAKE_SREC,$(ODIR)/img.bin.swp.srec,$(ODIR)/img.bin.swp,0x8000))
$(eval $(call MAKE_SREC,$(ODIR)/config.dtb.swp.srec,$(ODIR)/config.dtb.swp,0x1000))

$(ODIR)/fsbl-img-dtb.swp.srec: $(ODIR)/fsbl_flash.srec $(ODIR)/img.bin.swp.srec $(ODIR)/config.dtb.swp.srec
	$(call pretty_print,"CAT",$@)
	$(Q)cat $^ > $@
