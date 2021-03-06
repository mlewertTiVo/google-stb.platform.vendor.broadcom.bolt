###############################################################################
#
# Copyright 2014 Broadcom Corporation.  All rights reserved.
#
# Unless you and Broadcom execute a separate written software license
# agreement governing use of this software, this software is licensed to you
# under the terms of the GNU General Public License version 2, available at
# http://www.broadcom.com/licenses/GPLv2.php (the "GPL").
#
# Notwithstanding the above, under no circumstances may you combine this
# software in any way with any other Broadcom software provided under a
# license other than the GPL, without Broadcom's express prior written
# consent.
#
###############################################################################

SAVED_FAMILY := $(shell [ -e $(GEN)/.family ] && cat $(GEN)/.family || echo unknown)
FAMILY = $(firstword $(filter $(FAMILIES), $(MAKECMDGOALS)) $(SAVED_FAMILY))

ifeq ($(filter clean distclean, $(MAKECMDGOALS)),)
include $(GEN)/android/boltinc/build/tools.mk
endif

ODIR := objs/$(FAMILY)
OBJS := init.o arm-eabi.o gpt_table_in_flash.o android_bsu.o android_boot.o android_fastboot.o
OBJS := $(addprefix $(ODIR)/, $(OBJS))

BSU = $(ODIR)/android_bsu.elf

INCFLAGS = \
	-I$(GEN)/android/boltinc/include \
	-I$(GEN)/android/boltinc/include/arm \
	-I$(GEN)/android/boltinc/include/$(FAMILY) \

EXTRA_DEFINES = -include $(GEN)/$(FAMILY)/config.h
CSTRICT	:= -Wall -Wmissing-field-initializers -Wundef -Wextra -Wno-unused-parameter -Wshadow -Werror

# Note: Build env var 'TARGET_BOARD_PLATFORM' is set when BOLT is built from
# Android build tree.  See BoardConfig.mk from Android tree.
COMMON_FLAGS:= $(INCFLAGS) $(ARCHFLAGS) $(EXTRA_DEFINES) -D_BOLT_
CFLAGS := $(COMMON_FLAGS) $(ARCH_CFLAGS) -nostdlib -ffreestanding -Os \
	  $(CSTRICT) -g -ffunction-sections -fdata-sections -DBSU -DDROID_BSU -DDROID_PRODUCT='"${TARGET_BOARD_PLATFORM}"'
AFLAGS := $(COMMON_FLAGS) -D__ASSEMBLER__ -D_ASMLANGUAGE
LDFLAGS := $(ARCH_LDFLAGS)
LIBGCC := $(GEN)/android/boltinc/thirdparty/libgcc/libgcc.a

$(ODIR)/%.o: %.c | $(ODIR)
	$(CC) -c $(CFLAGS) $(CFLAGS_$<) $< -o $@

$(ODIR)/%.o: %.S | $(ODIR)
	$(CC) -c $(AFLAGS) $(AFLAGS_$<) $< -o $@

all: famcheck $(FAMILY)

$(FAMILY): famcheck $(BSU)

$(BSU): $(OBJS)
	$(LD) $(LDFLAGS) -T android_bsu.lds -o $@ $(OBJS) $(LIBGCC)

clean:
	@if [ "$(FAMILY)" != "unknown" ]; then \
		echo "$(RM) -rf $(ODIR)" ; \
		$(RM) -rf $(ODIR) ; \
	fi

distclean:
	$(RM) -rf objs
	$(RM) -f .family

$(ODIR):
	mkdir -p $(ODIR)

famcheck:
	@if [ "$(FAMILY)" = "unknown" ]; then \
		echo; \
		echo "For the first build, please specify the chip, e.g. 'make 7445c0'"; \
		echo; \
		false; \
	else \
		if [ "$(FAMILY)" != "$(SAVED_FAMILY)" ]; then \
			echo -n $(FAMILY) > .family ; \
		fi \
	fi

.PHONY: all famcheck clean distclean $(FAMILY) $(BSU)
