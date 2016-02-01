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
# E1

EXTRA_DEFINES += -I$(ROOT)/security 

ifeq ($(S_UNITTEST),y)
 EXTRA_DEFINES += -DS_UNITTEST=1 
endif

ifeq ($(S_UNITTEST_AUTOFLASH),y)
 EXTRA_DEFINES += -DS_UNITTEST_AUTOFLASH=1 
endif

ifeq ($(BFW_HASH_LOCK),y)
EXTRA_DEFINES += -DBFW_HASH_LOCK=1
endif

ifeq ($(SECURE_BOOT),y)
EXTRA_DEFINES += -DSECURE_BOOT=1 
endif

ifeq ($(BFW_LOAD),y)
EXTRA_DEFINES += -DBFW_LOAD=1 
endif

# 1. Build tests for non-secure BOLT (make SECURE_BOOT=y DUMMY_SECURITY=y)
# 2. Chip bringup without security libs (make DUMMY_SECURITY=y)
ifeq ($(DUMMY_SECURITY),y)
 FSBL_S += security/dummy.c
endif

SSBL_S += \
	security/ssbl-sec.c

BSL:=security/$(FAMILY)/bsl-lib.a
ASL:=security/$(FAMILY)/asl-lib.a

-include security/bsl.inc
-include security/asl.inc
-include security/common/custom_ca.inc
