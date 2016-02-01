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
# $brcm_Revision: 13 $
# $brcm_Date: 7/19/13 1:54p $
#
# Module Description:
#
# Revision History:
#
# Created: 08/28/2007 by Jon Garrett
#
# $brcm_Log: /BSEAV/app/splash/splashrun/Makefile $
# 
# 13   7/19/13 1:54p pntruong
# SW7429-701: Added option to scale up splash bmp to fullscreen and
# support for CLUT bmp file as well.
# 
# 12   5/23/13 1:00p syang
# SW7145-55: update to the fact that nexus/bin is now outside of vob
# 
# 11   4/1/13 6:36p syang
# SW7435-676:  include splash_vdc_reg.h and splash_vdc_rul.h from
# nexus/bin directly
# 
# 10   3/30/13 12:20a syang
# SW7435-676: make sure the right script files are used
# 
# 9   3/29/13 2:38p syang
# SW7435-676:  improve pal support
# 
# 8   3/29/13 11:40a syang
# SW7435-676: deep refactor for memc, surface and display number
# scalabilty; flexibility for diff configure combination; and easy
# adding of new chips
# 
# 7   2/14/13 7:48p syang
# SW7435-530: add install for splashrun and splash_vdc_rul_def.h,
# splash_vdc_reg_def.h; auto pull splash_vdc_rul.h and
# splash_vdc_rul_reg.h from /nexus/bin
# 
# 6   1/15/13 2:22p mward
# SW7425-4362: nexus/base/include private path required
# 
# 5   11/2/12 10:16a mward
# SW7435-114: Build with NEXUS_PLATFORM_7435_1U4T=n for the 1u2t RTS
# mode.
# 
# 4   1/23/12 3:22p katrep
# SW7231-317:disabled powermanagement if its enabled bby default
# 
# 3   4/8/10 5:52p jrubio
# SWDEPRECATED-3716: fix issue with NEXUS_BASE_ONLY Build
# 
# 2   9/21/09 5:51p nickh
# SW7420-351: Modify for Nexus build changes
# 
# 1   4/8/09 4:19p shyam
# PR52386 : Build system now based on nexus
# 
# 3   4/8/09 2:06p shyam
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

APPS = splashrun

OBJDIR=obj

ifndef PLATFORM
$(error PLATFORM is not defined)
endif

splash_dir := ${PLATFORM}
SPLASH_DIR = $(splash_dir)

ifeq ($(splash_pal),y)
CFLAGS += -DCFG_SPLASH_PAL
SPLASH_REG_H = splash_vdc_reg_pal.h
SPLASH_RUL_H = splash_vdc_rul_pal.h
else
SPLASH_REG_H = splash_vdc_reg_def.h
SPLASH_RUL_H = splash_vdc_rul_def.h
endif

NEXUS_BASE_ONLY=y
NEXUS_POWER_MANAGEMENT=n
export NEXUS_POWER_MANAGEMENT

include $(NEXUS_TOP)/platforms/$(PLATFORM)/build/platform_app.inc

# include these before platform_app.inc and they will get sucked in
MAGNUM= $(NEXUS_TOP)/../magnum
include $(MAGNUM)/portinginterface/vdc/bvdc.inc
include $(MAGNUM)/commonutils/rdc/brdc.inc
include $(MAGNUM)/portinginterface/pwm/bpwm.inc
include $(MAGNUM)/commonutils/mrc/bmrc.inc

include $(MAGNUM)/commonutils/fmt/bfmt.inc
include $(MAGNUM)/commonutils/sur/bsur.inc
include $(MAGNUM)/commonutils/avc/bavc.inc
include $(MAGNUM)/commonutils/pxl/bpxl.inc
include $(MAGNUM)/commonutils/mth/bmth.inc


include $(MAGNUM)/basemodules/mem/bmem.inc
include $(MAGNUM)/basemodules/int/bint.inc
include $(MAGNUM)/basemodules/tmr/btmr.inc

# we need to build these magnum modules
ifdef MORE_REQUIRED
BUILD_MODULES=BFMT BSUR BAVC BPXL BMTH
endif

NEXUS_APP_INCLUDE_PATHS += $(foreach module, $(MAGNUM_MODULES), $($(module)_INCLUDES))

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
CFLAGS += -I../common  -I$(NEXUS_BIN_DIR) -I$(SPLASH_DIR) -I../os/usermode  -I. 
CFLAGS += -I$(NEXUS_TOP)/base/include
VPATH += ./$(PLATFORM) ../common 


APP_SRC = \
	splash_vdc_run.c  \
	splash_script_load.c  \
	splash_bmp.c \
	splash_file.c \
	splash_main.c  

APP_OBJ = $(foreach file, $(patsubst %.c,%.o,$(APP_SRC)), $(OBJDIR)/$(notdir $(file)))

# Always build with debug
CFLAGS += -g
ifeq ($(VERBOSE),)
Q_=@
endif

# Implicit rule for building local apps
$(OBJDIR)/%.o: %.c ${NEXUS_BIN_DIR}/splash_vdc_reg.h ${NEXUS_BIN_DIR}/splash_vdc_rul.h
	@echo [Compile... $<]
	$(Q_)$(CC) -c -o $@ $< $(CFLAGS)

$(APPS) : ${SPLASH_DIR}/$(SPLASH_REG_H) ${SPLASH_DIR}/$(SPLASH_RUL_H) $(EXTRA_OBJ) $(APP_OBJ) splash_main.c 
	@echo [Compile... $<]
	$(Q_)$(CC) -o $@ $(EXTRA_OBJ) $(APP_OBJ) $(CFLAGS) $(LDFLAGS)

# copy from ${NEXUS_BIN_DIR}
${SPLASH_DIR}/$(SPLASH_REG_H): ${NEXUS_BIN_DIR}/splash_vdc_reg.h 
	@mkdir -p ${SPLASH_DIR}/
	cp -f ${NEXUS_BIN_DIR}/splash_vdc_reg.h ${SPLASH_DIR}/$(SPLASH_REG_H)

# copy from ${NEXUS_BIN_DIR} 
${SPLASH_DIR}/$(SPLASH_RUL_H): ${NEXUS_BIN_DIR}/splash_vdc_rul.h 
	@mkdir -p ${SPLASH_DIR}/
	cp -f ${NEXUS_BIN_DIR}/splash_vdc_rul.h ${SPLASH_DIR}/$(SPLASH_RUL_H)

install: nexus apps
	cp -f $(APPS) ${NEXUS_BIN_DIR}
	chmod a+x ${NEXUS_BIN_DIR}/nexus
