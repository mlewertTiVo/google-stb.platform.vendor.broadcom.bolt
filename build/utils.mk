# ***************************************************************************
# Broadcom Proprietary and Confidential. (c)2016 Broadcom. All rights reserved
#
# THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
# AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
# EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
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


gen_autodep64 = \
	@$(CC64) -MM $(3) $(2) > $(ODIR)/$(1).d.tmp; \
	sed -e 's|.*:|$(ODIR)/$(1).o:|' < $(ODIR)/$(1).d.tmp > \
        $(ODIR)/$(1).d; \
	sed -e 's/.*://' -e 's/\\$$//' < $(ODIR)/$(1).d.tmp | \
        fmt -1 | \
	sed -e 's/^ *//' -e 's/$$/:/' >> \
        $(ODIR)/$(1).d; \
	rm -f $(ODIR)/$(1).d.tmp


.PHONY: FORCE

$(ODIR)/%.64.o: %.64.c
	$(call pretty_print,CC64,$@)
	$(Q)[ -d $(dir $@) ] || mkdir -p $(dir $@)
	$(call gen_autodep64,$*,$<,$(CFLAGS64))
	$(Q)$(CC64) $(SAVE_TEMPS) -c $(CFLAGS64) $(CFLAGS64_$<) $< -o $@ -I$(dir $<)

$(ODIR)/%.o: %.c
	$(call pretty_print,CC,$@)
	$(Q)[ -d $(dir $@) ] || mkdir -p $(dir $@)
	$(call gen_autodep,$*,$<,$(CFLAGS))
	$(Q)$(CC) $(SAVE_TEMPS) -c $(CFLAGS) $(CFLAGS_$<) $< -o $@ -I$(dir $<)

$(ODIR)/%.64.o: %.64.S
	$(call pretty_print,AS64,$@)
	$(Q)[ -d $(dir $@) ] || mkdir -p $(dir $@)
	$(call gen_autodep64,$*,$<,$(AFLAGS64))
	$(Q)$(CC64) $(SAVE_TEMPS) -c $(AFLAGS64) $(AFLAGS64_$<) $< -o $@ -I$(dir $<)

$(ODIR)/%.o: %.S
	$(call pretty_print,AS,$@)
	$(Q)[ -d $(dir $@) ] || mkdir -p $(dir $@)
	$(call gen_autodep,$*,$<,$(AFLAGS))
	$(Q)$(CC) $(SAVE_TEMPS) -c $(AFLAGS) $(AFLAGS_$<) $< -o $@ -I$(dir $<)

%-stripped.64.o: %.64.o
	$(call pretty_print,STRIP64,$@)
	$(Q)$(STRIP64) --strip-all $< -o $@

%-stripped.o: %.o
	$(call pretty_print,STRIP,$@)
	$(Q)$(STRIP) --strip-all $< -o $@

%.64.bin: %.64.elf
	$(call pretty_print,OBJCPY64,$@)
	$(Q)$(OBJCOPY64) --gap-fill $(FILLB) -O binary $< $@

%.bin: %.elf
	$(call pretty_print,OBJCOPY,$@)
	$(Q)$(OBJCOPY) --gap-fill $(FILLB) -O binary $< $@

%.64.asm: %.64.elf
	$(call pretty_print,OBJDMP64,$@)
	$(Q)rm -f $@
	$(Q)$(OBJDUMP64) -Sd $< > $@

%.asm: %.elf
	$(call pretty_print,OBJDUMP,$@)
	$(Q)rm -f $@
	$(Q)$(OBJDUMP) -Sd $< > $@

%.html: %.txt
	$(call pretty_print,HTML,$@)
	$(Q)asciidoc --section-numbers --doctype=book --out-file=$@ $<

