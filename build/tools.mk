# ***************************************************************************
# Broadcom Proprietary and Confidential. (c)2016 Broadcom. All rights reserved.
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

# NOTE: The first pass may find STUB64_START as being undefined, up
# until config.pl runs and we start the gcc build in earnest.
ifneq ($(STUB64_START),)
# https://gcc.gnu.org/gcc-4.9/changes.html
#
# Quote: "The -mrestrict-it option can be used with -march=armv7-a 
#	  or the -march=armv7ve options to make code generation fully
#	  compatible with the deprecated instructions in ARMv8-A."
#
# Should work: ARCHFLAGS:=-march=armv7-a -mabi=aapcs -mrestrict-it, but
# gcc sometimes generates bad IT block assembly with stbgcc-4.8-1.1, see
# bug: https://gcc.gnu.org/bugzilla/show_bug.cgi?id=67591
ARCHFLAGS:= -mcpu=cortex-a53 -mabi=aapcs
else
ARCHFLAGS:=-march=armv7-a -mabi=aapcs
endif

ifeq ($(USE_THUMB),1)
 ARCHFLAGS += -mthumb
else
 ARCHFLAGS += -marm
endif

ARCH_LDFLAGS:=--architecture arm -EL

ARCH_CFLAGS := -fno-short-enums -mfloat-abi=softfp -D__LITTLE_ENDIAN=1

ARCH_RM:=.ARM.exidx

ARCH_OFORMAT:=elf32-littlearm

# ----------------------------------

CROSS64?=aarch64-linux-
CC64:=$(CROSS64)gcc
LD64:=$(CROSS64)ld
AR64:=$(CROSS64)ar
OBJCOPY64:=$(CROSS64)objcopy
OBJDUMP64:=$(CROSS64)objdump
STRIP64:=$(CROSS64)strip

ARCHFLAGS64:= -march=armv8-a+nosimd+nofp 

ARCH_LDFLAGS64:= -EL 

ARCH_CFLAGS64:= -fno-short-enums -D__LITTLE_ENDIAN=1 -mstrict-align -mcmodel=small -mtune=cortex-a53 -fno-pic -mgeneral-regs-only 

ARCH_RM64:=.ARM.exidx

ARCH_OFORMAT64:=elf64-littleaarch64

# EOF
