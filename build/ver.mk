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

BUILDYEAR := $(shell date +"%Y")
BUILDDATE := $(shell date +"%Y-%m-%d %H:%M:%S")
BUILDUSER := $(shell whoami)
BUILDHOST := $(shell hostname)
BUILDTAG := $(shell cat $(ROOT)/version 2>/dev/null || \
		    git describe --dirty 2> /dev/null || \
		    git rev-parse --short HEAD 2> /dev/null || \
		    echo "v9.99-noversion" 2>/dev/null)

TMP := $(shell perl -e 'print "$(BUILDTAG)" =~ /\bv(\d+)\.(\d\d)\b/ \
	? "$$1 $$2" : "9 99";')
BOLT_VER_MAJOR := $(word 1, $(TMP))
BOLT_VER_MINOR := $(word 2, $(TMP))

BOLT_VER_DT_COMPAT?=1
