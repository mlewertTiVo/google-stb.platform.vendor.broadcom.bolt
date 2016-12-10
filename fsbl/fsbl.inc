# ***************************************************************************
# Broadcom Proprietary and Confidential. (c)2016 Broadcom. All rights reserved.
#
# THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
# AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
# EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# ***************************************************************************

FSBL_S += \
 fsbl/fsbl-arm.c \
 fsbl/fsbl-clock.c \
 fsbl/fsbl-emmc.c \
 fsbl/fsbl-flash.c \
 fsbl/fsbl-glitch.S \
 fsbl/fsbl-hacks.c \
 fsbl/fsbl-main.c \
 fsbl/fsbl-minilib.c \
 fsbl/fsbl-pm.c \
 fsbl/fsbl-sec.c \
 fsbl/fsbl-secparams.c \
 fsbl/fsbl-shmoo.c \
 fsbl/fsbl-start.S \
 fsbl/fsbl-timer.c \
 fsbl/fsbl-uart.c \
 fsbl/fsbl-vectors.S \
 fsbl/fsbl-board.c \
 fsbl/fsbl-die.c \
 $(GEN)/$(FAMILY)/fsbl-pinmux.c


ifeq ($(CFG_BOARD_ID),1)
FSBL_S += \
 fsbl/fsbl-i2c.c
endif

ifeq ($(CFG_MHL),1)
FSBL_S += \
 fsbl/fsbl-mhl.c
endif

$(FSBL_S):

