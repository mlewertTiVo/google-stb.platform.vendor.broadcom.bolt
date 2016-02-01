############################################################
#     (c)2003-2009 Broadcom Corporation
#  
#  This program is the proprietary software of Broadcom Corporation and/or its licensors,
#  and may only be used, duplicated, modified or distributed pursuant to the terms and
#  conditions of a separate, written license agreement executed between you and Broadcom
#  (an "Authorized License").  Except as set forth in an Authorized License, Broadcom grants
#  no license (express or implied), right to use, or waiver of any kind with respect to the
#  Software, and Broadcom expressly reserves all rights in and to the Software and all
#  intellectual property rights therein.  IF YOU HAVE NO AUTHORIZED LICENSE, THEN YOU
#  HAVE NO RIGHT TO USE THIS SOFTWARE IN ANY WAY, AND SHOULD IMMEDIATELY
#  NOTIFY BROADCOM AND DISCONTINUE ALL USE OF THE SOFTWARE.  
#   
#  Except as expressly set forth in the Authorized License,
#   
#  1.     This program, including its structure, sequence and organization, constitutes the valuable trade
#  secrets of Broadcom, and you shall use all reasonable efforts to protect the confidentiality thereof,
#  and to use this information only in connection with your use of Broadcom integrated circuit products.
#   
#  2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS" 
#  AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES, REPRESENTATIONS OR 
#  WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT TO 
#  THE SOFTWARE.  BROADCOM SPECIFICALLY DISCLAIMS ANY AND ALL IMPLIED WARRANTIES 
#  OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE, 
#  LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION 
#  OR CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING OUT OF 
#  USE OR PERFORMANCE OF THE SOFTWARE.
#  
#  3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL BROADCOM OR ITS 
#  LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL, SPECIAL, INDIRECT, OR 
#  EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR IN ANY WAY RELATING TO YOUR 
#  USE OF OR INABILITY TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF 
#  THE POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF THE AMOUNT 
#  ACTUALLY PAID FOR THE SOFTWARE ITSELF OR U.S. $1, WHICHEVER IS GREATER. THESE 
#  LIMITATIONS SHALL APPLY NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF 
#  ANY LIMITED REMEDY.
# 
# $brcm_Workfile: Makefile $
# $brcm_Revision: 21 $
# $brcm_Date: 6/20/13 3:50p $
#
# Module Description:
#
# Revision History:
#
# Created: 08/28/2007 by Jon Garrett
#
# $brcm_Log: /BSEAV/app/splash/splashgen/Makefile $
# 
# 21   6/20/13 3:50p syang
# SW7346-1262: add table entry pick for bfmt and bvdc
# 
# 20   6/14/13 3:48p syang
# SW7346-1262: change  BVDC_FOR_BOOTLOADER to BVDC_FOR_BOOTUPDATER per
# Brian's request
# 
# 19   4/26/13 2:35p syang
# SW7346-1262:  take off linker msg print of unused code removing
# 
# 18   4/25/13 11:45p syang
# SW7346-1262: add option to build with reduced vdc for bootloader usage
# 
# 17   4/10/13 11:32a syang
# SW7435-676: avoid non-necessary compile
# 
# 16   3/30/13 12:18a syang
# SW7435-676: make sure re-compile in the case only splash_pal changes
# 
# 15   3/29/13 2:37p syang
# SW7435-676:  improve pal support
# 
# 14   3/29/13 11:38a syang
# SW7435-676: deep refactor for memc, surface and display number
# scalabilty; flexibility for diff configure combination; and easy
# adding of new chips
# 
# 13   2/14/13 7:46p syang
# SW7435-530: add install for splashgen and splash.bmp
# 
# 12   1/9/13 4:13p erickson
# SW7425-4362: nexus/base/include private path required
# 
# 11   11/2/12 10:16a mward
# SW7435-114: Build with NEXUS_PLATFORM_7435_1U4T=n for the 1u2t RTS
# mode.
# 
# 10   5/16/12 11:57a erickson
# SW7420-1264: use NEXUS_BIN_DIR
# 
# 9   1/23/12 3:22p katrep
# SW7231-317:disabled powermanagement if its enabled bby default
# 
# 8   8/11/10 6:06p mward
# SW7125-569:  Add splash support for 97125.  Don't fail if GL_SUPPORT=y.
# 
# 7   4/9/10 3:31p rjain
# SW7550-284: fixing makefile for SPLASH_NOHDM
# 
# 6   4/8/10 5:53p jrubio
# SWDEPRECATED-3716: Take out VBI
# 
# 5   4/8/10 5:42p jrubio
# SWDEPRECATED-3716: fix issue with NEXUS_BASE_ONLY Build
# 
# 4   9/21/09 5:25p nickh
# SW7420-351: Modify for Nexus build changes and add bmrc.inc file needed
# for building 7420
# 
# 3   6/16/09 6:09p katrep
# PR52386: Fixed the build issue
# 
# 2   5/11/09 5:28p shyam
# PR53637 : Fixes for PAL and DTV chip support
# 
# 1   4/8/09 4:07p shyam
# PR52386 : Build system now based on nexus
# 
# 4   4/8/09 12:41p shyam
# PR52386 : Port splash to nexus Base Build system
# 
# Hydra_Software_Devel/3   4/2/09 4:20p shyam
# PR53323 : Make Base build part of the make file
# 
# Hydra_Software_Devel/2   3/26/09 11:26a erickson
# PR53323: rename app, add obj subdir
#
# Hydra_Software_Devel/1   3/19/09 4:58p erickson
# PR53323: add test app for calling magnum directly from app
#
############################################################

ifdef COMSPEC
# Any DOS environment
NEXUS_TOP := $(shell cd ../../../../nexus && cd)
else
NEXUS_TOP := $(shell cd ../../../../nexus; pwd)
endif

APPS = splashgen

OBJDIR=obj

ifndef PLATFORM
$(error PLATFORM is not defined)
endif

NEXUS_BASE_ONLY=y
NEXUS_POWER_MANAGEMENT=n
export NEXUS_POWER_MANAGEMENT
BRDC_DISABLE_TRACK_EXECUTION=y

#this is required otherwise  get get corrrect $BCHP_CHIP
include $(NEXUS_TOP)/platforms/$(PLATFORM)/build/platform_app.inc

# include these before platform_app.inc and they will get sucked in
MAGNUM= $(NEXUS_TOP)/../magnum
# The app must include or build these manually because NEXUS_BASE_ONLY does not include them
MAGNUM= $(NEXUS_TOP)/../magnum
include $(MAGNUM)/portinginterface/vdc/bvdc.inc
include $(MAGNUM)/commonutils/rdc/brdc.inc
include $(MAGNUM)/portinginterface/pwm/bpwm.inc
include $(MAGNUM)/commonutils/fmt/bfmt.inc
include $(MAGNUM)/commonutils/sur/bsur.inc
include $(MAGNUM)/commonutils/avc/bavc.inc
include $(MAGNUM)/commonutils/pxl/bpxl.inc
include $(MAGNUM)/commonutils/mth/bmth.inc
include $(MAGNUM)/basemodules/mem/bmem.inc
include $(MAGNUM)/basemodules/int/bint.inc
include $(MAGNUM)/basemodules/tmr/btmr.inc
include $(MAGNUM)/portinginterface/i2c/bi2c.inc
include $(MAGNUM)/commonutils/mrc/bmrc.inc

ifneq ($(SPLASH_NOHDM),y)
CFLAGS += -DSPLASH_SUPPORT_HDM
include $(MAGNUM)/portinginterface/hdm/bhdm.inc
endif

ifeq ($(splash_pal),y)
CFLAGS += -DCFG_SPLASH_PAL
endif

ifeq ($(GL_SUPPORT),y)
LDFLAGS += -lm
endif

NEXUS_APP_INCLUDE_PATHS += $(foreach module, $(MAGNUM_MODULES), $($(module)_INCLUDES))

# we need to build these magnum modules
BUILD_MODULES=BVDC BRDC BFMT BSUR BAVC BPXL BMTH 
ifneq ($(SPLASH_NOHDM),y)
BUILD_MODULES += BI2C BHDM
endif
MAGNUM_DEFINES := $(sort $(foreach module, $(MAGNUM_MODULES), $($(module)_DEFINES)))
MAGNUM_CFLAGS := $(addprefix -D,$(MAGNUM_DEFINES))
CFLAGS += $(MAGNUM_CFLAGS)
EXTRA_SRC += $(foreach module, $(BUILD_MODULES), $($(module)_SOURCES))
VPATH = $(foreach module,$(BUILD_MODULES),$(dir $($(module)_SOURCES)))
vpath %.c $(VPATH)
EXTRA_OBJ = $(foreach file, $(patsubst %.c,%.o,$(EXTRA_SRC)), $(OBJDIR)/$(notdir $(file)))

.PHONY: nexus apps clean clean_apps $(APPS)

all: install

apps: $(OBJDIR) $(APPS)

$(OBJDIR):
	mkdir $(OBJDIR)

print:
	@echo $(EXTRA_OBJ)
	@echo $(VPATH)

# This builds nexus
nexus:
	$(MAKE) -C $(NEXUS_TOP)/build NEXUS_BASE_ONLY=y BREG_CAPTURE=y

# This cleans nexus and local apps
clean: clean_apps
	$(MAKE) -C $(NEXUS_TOP)/build clean

clean_apps:
	-$(RM) $(APPS) $(OBJDIR)/*.d $(OBJDIR)/*.o *.out

# This is the minimum needed to compile and link with Nexus
CFLAGS += $(NEXUS_CFLAGS) $(addprefix -I,$(NEXUS_APP_INCLUDE_PATHS)) $(addprefix -D,$(NEXUS_APP_DEFINES))
LDFLAGS += $(NEXUS_LDFLAGS) -L$(NEXUS_BIN_DIR) -lnexus -lpthread

# App Stuff 
ifeq ($(NEXUS_PLATFORM),97435)
ifneq ($(NEXUS_PLATFORM_7435_1U4T),n)
CFLAGS += -DNEXUS_PLATFORM_7435_BOX_TYPE_1u4t=1
endif
endif

CFLAGS += -I../common -I$(PLATFORM) -I../os/usermode  -I. 
CFLAGS += -I$(NEXUS_TOP)/base/include
VPATH += ./$(PLATFORM) ../common 

# build with shrinked vdc for BOOTUPDATER
ifeq ($(B_PI_FOR_BOOTUPDATER),y)
CFLAGS += -DBVDC_FOR_BOOTUPDATER=1
CFLAGS += -DBFMT_FOR_BOOTUPDATER=1
CFLAGS += -DBFMT_PICK_eNTSC=1
CFLAGS += -DBFMT_PICK_e480p=1
#CFLAGS += -DBFMT_PICK_e720p=1
#CFLAGS += -DBFMT_PICK_e1080i=1
#CFLAGS += -DBFMT_PICK_e1080p=1
#CFLAGS += -DBFMT_PICK_e1080p_24Hz=1
#CFLAGS += -DBFMT_PICK_e1080p_30Hz=1
CFLAGS += -Os -fdata-sections -ffunction-sections -Wl,--gc-sections 
#CFLAGS += -Os -fdata-sections -ffunction-sections -Wl,--gc-sections -Wl,--print-gc-sections
endif

APP_SRC = \
	splash_vdc_rulgen.c  \
	splash_setup_vdc.c \
	platformconfig.c \
	splash_bmp.c \
	splash_file.c 

APP_OBJ = $(foreach file, $(patsubst %.c,%.o,$(APP_SRC)), $(OBJDIR)/$(notdir $(file)))

# Always build with debug
ifneq ($(B_PI_FOR_BOOTUPDATER),y)
CFLAGS += -g
ifeq ($(VERBOSE),)
Q_=@
endif
endif

# Implicit rule for building local apps
$(OBJDIR)/%.o: %.c 
	@echo [Compile... $<]
	$(Q_)$(CC) -c -o $@ $< $(CFLAGS)

clean_app_only:
	-$(RM) $(APP_OBJ)

$(APPS) : clean_app_only $(EXTRA_OBJ) $(APP_OBJ) splash_main.c 
	@echo [Compile and Build... $@]
	$(Q_)$(CC) -o $@ splash_main.c  $(EXTRA_OBJ) $(APP_OBJ) $(CFLAGS) $(LDFLAGS)

install: nexus apps
	cp -f ${APPS} ${NEXUS_BIN_DIR}
	cp -f splash.bmp ${NEXUS_BIN_DIR}
	chmod a+x ${NEXUS_BIN_DIR}/nexus

