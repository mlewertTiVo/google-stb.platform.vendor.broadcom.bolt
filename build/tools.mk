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

ARCH:=arm

ABI:=arm-linux-uclibcgnueabi

CROSS?=arm-linux-
CC:=$(CROSS)gcc
LD:=$(CROSS)ld
AR:=$(CROSS)ar
OBJCOPY:=$(CROSS)objcopy
OBJDUMP:=$(CROSS)objdump
STRIP:=$(CROSS)strip
SHELL:=bash

ARCHFLAGS:=-march=armv7-a -mabi=aapcs

ifeq ($(USE_THUMB),1)
 ARCHFLAGS += -mthumb
else
 ARCHFLAGS += -marm
endif

ARCH_LDFLAGS:=--architecture arm -EL

ARCH_CFLAGS := -fno-short-enums -mfloat-abi=softfp -D__LITTLE_ENDIAN=1

ARCH_RM:=.ARM.exidx

ARCH_OFORMAT:=elf32-littlearm
