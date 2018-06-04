# ***************************************************************************
# Broadcom Proprietary and Confidential. (c)2016 Broadcom. All rights reserved.
#
# THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
# AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
# EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
#
# ***************************************************************************

BUILDYEAR := $(shell date +"%Y")
BUILDDATE := $(shell date +"%Y-%m-%d %H:%M:%S")
BUILDUSER := $(shell whoami)
BUILDNODE := $(shell uname -n)
BUILDHOST := $(shell perl -e 'print "$(BUILDNODE)" =~ /([^.]+)\.(.*)/ \
	? "$$1" : "$(BUILDNODE)"')
BUILDTAG := $(shell cat $(ROOT)/version 2>/dev/null || \
		    git describe --dirty 2> /dev/null || \
		    git rev-parse --short HEAD 2> /dev/null || \
		    echo "v9.99-noversion" 2>/dev/null)
BUILDCMD := $(shell perl ./scripts/checkpath.pl --no-annoy 2>/dev/null)
BUILDCMD := $(if $(BUILDCMD),$(word 1, $(BUILDCMD)) --version | head -1)
TOOLCHAINVER := $(if $(BUILDCMD),$(shell $(BUILDCMD)),unknown)
TMP := $(shell perl -e 'print "$(BUILDTAG)" =~ /\bv(\d+)\.(\d\d)\b/ \
	? "$$1 $$2" : "9 99";')
BOLT_VER_MAJOR := $(word 1, $(TMP))
BOLT_VER_MINOR := $(word 2, $(TMP))

BOLT_VER_DT_COMPAT?=1
