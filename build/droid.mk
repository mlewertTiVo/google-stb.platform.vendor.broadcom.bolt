###############################################################################
#
# Copyright 2014-2015 Broadcom Corporation.  All rights reserved.
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

# Define this variable to allow BOLT top-level makefile to call
# 'android_postpatch' build target after running 'postpatch' target
DROID_BSU_POSTPATCH := android_postpatch

ANDROID_TOP_DIR := android

ANDROID_SCRIPTS_DIR := $(ANDROID_TOP_DIR)/scripts
ANDROID_BOLT_INC_DIR := $(ANDROID_TOP_DIR)/boltinc

ANDROID_INC_FILES := \
	gen/$(FAMILY)/config.h \
	include/arch.h \
	include/bitops.h \
	include/boardcfg.h \
	include/bsu-api.h \
	include/byteorder.h \
	include/compiler.h \
	include/devfuncs.h \
	include/env_subr.h \
	include/error.h \
	include/fileops.h \
	include/iocb.h \
	include/iocb-defs.h \
	include/ioctl.h \
	include/lib_physio.h \
	include/lib_queue.h \
	include/lib_types.h \
	include/loader.h \
	include/ui_command.h \
	include/zimage.h \
	include/arm/arm.h \
	include/arm/arm-macros.h \
	include/arm/arm-start.h \
	thirdparty/libgcc/libgcc.a \
	include/$(FAMILY)/bchp_aon_ctrl.h \
	include/$(FAMILY)/bchp_common.h \
	build/tools.mk


# This target performs the following operations:
# 1. Create the new include directory inside Android BSU directory and
#    clean out existing auto-gen files except for chip-family that is *not*
#    intended to be building.
# 2. Copy include files used by Android BSU from top-level BOLT directory
#    to Android BSU directory
# 3. Change the license header for Android BSU code release from BRCM SLA to
#    BSD 3-clause license
# 4. Record the chip families supported by BOLT for building Android BSU
# 5. Strip out register definitions not used in Android BSU
#
android_postpatch:
	$(Q)echo
	$(Q)echo '  ------------------------------------------'
	$(Q)echo '  Auto-gen include files for Android BSU'
	$(Q)echo '  ------------------------------------------'
	$(Q)echo
	$(Q)echo "  Include files created in: $(ANDROID_BOLT_INC_DIR)"
	$(Q)echo "  Do NOT modify auto-gen include files"
	$(Q)echo
	$(Q)mkdir -p $(ANDROID_BOLT_INC_DIR)
	$(Q)rm -rf $(ANDROID_BOLT_INC_DIR)/gen/$(FAMILY)/*.h
	$(Q)rm -rf $(ANDROID_BOLT_INC_DIR)/include/*.h
	$(Q)rm -rf $(ANDROID_BOLT_INC_DIR)/include/$(FAMILY)/*.h
	$(Q)rm -rf $(ANDROID_BOLT_INC_DIR)/build/*
	$(Q)rm -rf $(ANDROID_BOLT_INC_DIR)/thirdparty/*
	$(Q)echo "  Copying files..."
	$(Q)echo
	$(Q)$(foreach f,$(ANDROID_INC_FILES), cp -pv --parents $f $(ANDROID_BOLT_INC_DIR)/;)
	$(Q)find $(ANDROID_BOLT_INC_DIR) -name "*.h" -exec sed -i -f $(ANDROID_SCRIPTS_DIR)/sed-h-file-lic.script {} +
	$(Q)find $(ANDROID_BOLT_INC_DIR) -name "*.mk" -exec sed -i -f $(ANDROID_SCRIPTS_DIR)/sed-mk-file-lic.script {} +
	$(Q)echo $(FAMILIES) > $(ANDROID_BOLT_INC_DIR)/build-family-list
	$(Q)$(ANDROID_SCRIPTS_DIR)/strip-hdr.py $(ANDROID_BOLT_INC_DIR)/include/$(FAMILY)
	$(Q)echo "Files in this directory are auto-generated. Do not modify." > $(ANDROID_BOLT_INC_DIR)/readme.txt
	$(Q)echo
	$(Q)echo '  Done! You can build Android BSU now.'

.PHONY: android_postpatch

