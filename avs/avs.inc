# ***************************************************************************
# Broadcom Proprietary and Confidential. (c)2016 Broadcom. All rights reserved.
#
# THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
# AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
# EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# ***************************************************************************

# These parts have dual AVS Monitors
AVS_DUAL:=

# Whitelists
AVS_SETUPS   :=7250b0 7260a0 7268a0 7271a0 7364a0 7364b0 7364c0 7366c0 74371a0 7439b0 7445d0 7445e0
AVS_FIRMWARES:=7250b0 7260a0 7268a0 7271a0 7364a0 7364b0 7364c0 7366c0 74371a0 7439b0 7445d0 7445e0

# for bsu and temp_reset:
# These parts use an older set of conversion functions for PVTMON
AVS_OLD_CALCS:=74371
AVS_NO_OVER_TEMP:=74371

# These parts can be used on reference boards configured with one or two power domains
AVS_DUAL_DOMAINS:=7260a0 7268a0 7271a0

# These parts support dynamic voltage and frequency scaling
DVFS_SUPPPORT:=7260a0 7268a0 7271a0

# ***************************************************************************

AVS_COMMON_CODE:=avs/common

AVS_FSBL_S:= $(AVS_COMMON_CODE)/avs_api.c


# ***************************************************************************
#  Separate the class of devices by whether they have single or
# dual AVS Monitor blocks.
# ***************************************************************************

ifeq ($(filter $(FAMILY), $(AVS_DUAL)), $(FAMILY))
  AVS_CLASS:=avs/dual
  EXTRA_DEFINES += -DAVS_DUAL_MONITORS
else
ifeq ($(filter $(FAMILY), $(AVS_DUAL_DOMAINS)), $(FAMILY))
  EXTRA_DEFINES += -DAVS_DUAL_DOMAINS
endif
  AVS_CLASS:=avs/single
endif

ifeq ($(filter $(FAMILY), $(DVFS_SUPPPORT)), $(FAMILY))
  EXTRA_DEFINES += -DDVFS_SUPPPORT
  AVS_SSBL_S += $(AVS_COMMON_CODE)/avs_dvfs.c
endif

EXTRA_DEFINES += -I$(AVS_CLASS) -DAVS_CLASS=$(AVS_CLASS) -I$(AVS_COMMON_CODE)

# ***************************************************************************
# Only enable the AVS setup on these platforms.
# Its okay to run this part on all versions of these chips.
#  Note that we normally enable AVS_ENABLE_OVERTEMP to allow an
# over temperature condition to trigger a reset.
# ***************************************************************************

ifeq ($(filter $(FAMILY), $(AVS_SETUPS)), $(FAMILY))
 EXTRA_DEFINES += -DENABLE_AVS_INIT
 AVS_FSBL_S += $(AVS_CLASS)/avs_init.c
 AVS_FSBL_S += $(AVS_CLASS)/avs_temp_reset.c
endif

ifeq ($(filter $(FAMILY), $(AVS_NO_OVER_TEMP)), $(FAMILY))
 EXTRA_DEFINES += -DAVS_ENABLE_OVERTEMP=0
else
 EXTRA_DEFINES += -DAVS_ENABLE_OVERTEMP=1
endif


# ***************************************************************************
# Only enable the AVS firmware on these platforms.
# We use family to enable on specific platforms where its not-disabled in OTP
# ***************************************************************************

ifeq ($(filter $(FAMILY), $(AVS_FIRMWARES)), $(FAMILY))
 ifeq ($(filter $(EXTRA_DEFINES),-DENABLE_AVS_INIT),)
  $(error AVS: You cannot have firmware without setup!)
 endif
 EXTRA_DEFINES += -DENABLE_AVS_FIRMWARE
 #  We convert the binary into an object file that will be
 # placed in a specific location:
 AVS_FW_S   := $(AVS_CLASS)/avs_$(FAMILY)_fw.bin
 AVS_FSBL_S += $(AVS_CLASS)/avs_uart.c
 AVS_FSBL_S += $(AVS_COMMON_CODE)/avs_load_fw.c
else
 AVS_FW_S   :=  $(AVS_COMMON_CODE)/avs_dummy_fw.bin
endif

ifeq ($(filter $(FAMILY), $(AVS_OLD_CALCS)), $(FAMILY))
  EXTRA_DEFINES += -DAVS_USE_NEW_CONVERSION_FORMULAS=0
else
  EXTRA_DEFINES += -DAVS_USE_NEW_CONVERSION_FORMULAS=1
endif

ifeq ($(CFG_EMULATION),1)
 AVS_FSBL_S:=
endif

# ***************************************************************************
# These parts go into the SSBL section of the code.
# ***************************************************************************
AVS_SSBL_S += $(AVS_COMMON_CODE)/avs_bsu.c
