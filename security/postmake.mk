# ***************************************************************************
# *     Copyright (c) 2012-2014, Broadcom Corporation
# *     All Rights Reserved
# *     Confidential Property of Broadcom Corporation
# *
# *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
# *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
# *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
# * 
# ***************************************************************************

#  After bolt.bin has been made we can select a pair of BBL & BFWs to patch
# into the binary image.

#  Match versions of bbl and bfw, along with with prefix text. Note: don't
# put spaces between the list values or have a different number of
# elements in any list.

ifeq ($(FAMILY),7445d0)
 SEC_PFX := ba,bb
 SEC_BBL := 1.0.0,1.0.1
 SEC_BFW := 2.1.0,2.1.0
endif

ifeq ($(CFG_ZEUS4_1),1)
 SEC_PFX := ba,bb
 SEC_BBL = 1.0.0,2.0.0
 SEC_BFW = 1.0.0,2.0.0
endif

#default
ifeq ($(SEC_PFX),)
 SEC_PFX := ba,ba,bb
 SEC_BBL := 1.0.0,1.0.0,1.0.1
 SEC_BFW := 1.0.0,2.0.0,2.1.0
endif
