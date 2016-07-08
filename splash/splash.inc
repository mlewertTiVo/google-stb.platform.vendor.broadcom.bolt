# ***************************************************************************
# Broadcom Proprietary and Confidential. (c)2016 Broadcom. All rights reserved.
#
# THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
# AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
# EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# ***************************************************************************

SPLASH_S += \
  splash/splash-api.c

EXTRA_DEFINES += \
	-I$(ROOT)/splash \
	-I$(ROOT)/splash/BSEAV/app/splash/common \
	-I$(ROOT)/splash/BSEAV/app/splash/os/cfe \
	-I$(ROOT)/splash/BSEAV/app/splash/splashrun

ifeq ($(CFG_SPLASH),1)
  SPLASH_S += \
    splash/splash-glue.c \
	splash/BSEAV/app/splash/common/splash_bmp.c \
	splash/BSEAV/app/splash/splashrun/splash_script_load.c \
	splash/splash_vdc_run.c \
	splash/splash-audio-run.c \
	splash/splash-media.c

  ifeq ($(CFG_ZLIB),1)
    SPLASH_S += splash/splash-uncompress.c
  endif

  EXTRA_DEFINES += -I$(ROOT)/splash/BSEAV/app/splash/splashrun/9$(BCHP_CHIP)

  # check whether 'splashgen' generated files exist
  SPLASH_H_ROOT:=splash/BSEAV/app/splash/splashrun/9$(BCHP_CHIP)
  ifeq ($(CFG_SPLASH_PAL),1)
    SPLASH_POSTFIX := pal
  else
    SPLASH_POSTFIX := def
  endif

  # The existence of $(SPLASH_C_RUL) implies SPLASH_VERSION_2.
  SPLASH_C_RUL := $(SPLASH_H_ROOT)/splash_vdc_rul_$(SPLASH_POSTFIX).c
  SPLASH_H_RUL := $(SPLASH_H_ROOT)/splash_vdc_rul_$(SPLASH_POSTFIX).h
  SPLASH_H_REG := $(SPLASH_H_ROOT)/splash_vdc_reg_$(SPLASH_POSTFIX).h

  ifeq (,$(wildcard $(SPLASH_C_RUL)))
    # $(SPLASH_C_RUL) does not exist. $(SPLASH_H_RUL) is required.
    ifeq (,$(wildcard $(SPLASH_H_RUL)))
      SPLASH_SHOULD_RUN_SPLASHGEN := y
    endif
  else
    # $(SPLASH_C_RUL) exists, but should not co-exist with $(SPLASH_H_RUL).
    ifneq (,$(wildcard $(SPLASH_H_RUL)))
      SPLASH_AMBIGUOUS_SPLASHGEN := y
    endif
    EXTRA_DEFINES += -DSPLASH_VERSION_2
    SPLASH_S += $(SPLASH_C_RUL)
  endif

  # Regardless $(SPLASH_C_RUL) or $(SPLASH_H_RUL), $(SPLASH_H_REG) is required.
  ifeq (,$(wildcard $(SPLASH_H_REG)))
    SPLASH_SHOULD_RUN_SPLASHGEN := y
  endif

  define SPLASH_INFO_AMBIGUITY

  ------------------------------------------------------------------------
  Conflicting file names from 'splashgen' have been found under:
  '$(dir $(SPLASH_H_REG))'.
  Please remove unintended file(s) so that there is no ambiguity.

  For more information, please refer to:
    'doc/porting_bolt_to_customer_platforms.txt' in BOLT, or
    'BSEAV/app/splash/readme.txt' in reference software source package.
  ------------------------------------------------------------------------

  endef

  define SPLASH_INFO_RUN_SPLASHGEN

  ------------------------------------------------------------------------
  Please run 'splashgen' in reference software environment after adjusting
  options based on your target platform, and then copy the generated header
  files to: '$(dir $(SPLASH_H_REG))'.

  For more information, please refer to:
    'doc/porting_bolt_to_customer_platforms.txt' in BOLT, or
    'BSEAV/app/splash/readme.txt' in reference software source package.
  ------------------------------------------------------------------------

  endef

  ifeq ($(SPLASH_AMBIGUOUS_SPLASHGEN),y)
    $(info $(SPLASH_INFO_AMBIGUITY))
    $(error BUILD FAILURE due to conflicting splash files)
  endif

  ifeq ($(SPLASH_SHOULD_RUN_SPLASHGEN),y)
    $(info $(SPLASH_INFO_RUN_SPLASHGEN))
    $(error BUILD FAILURE due to missing splash files)
  endif
endif

$(SPLASH_S):
