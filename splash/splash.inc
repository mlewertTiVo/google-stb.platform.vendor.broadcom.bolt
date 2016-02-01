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

SPLASH_S += \
  splash/splash-api.c

EXTRA_DEFINES += -I$(ROOT)/splash

ifeq ($(CFG_SPLASH),1)
  SPLASH_S += \
    splash/splash-glue.c \
	splash/BSEAV/app/splash/common/splash_bmp.c \
	splash/BSEAV/app/splash/splashrun/splash_script_load.c \
	splash/splash_vdc_run.c \
	splash/splash-audio-run.c \
	splash/splash-media.c \
	splash/splash_vdc_replace.c

  ifeq ($(CFG_ZLIB),1)
    SPLASH_S += splash/splash-uncompress.c
  endif

  EXTRA_DEFINES += \
    -I$(ROOT)/splash/BSEAV/app/splash/common \
    -I$(ROOT)/splash/BSEAV/app/splash/os/cfe \
    -I$(ROOT)/splash/BSEAV/app/splash/splashrun \
    -I$(ROOT)/splash/BSEAV/app/splash/splashrun/9$(BCHP_CHIP) \

endif

$(SPLASH_S):
