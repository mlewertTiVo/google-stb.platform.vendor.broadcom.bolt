 ***************************************************************************
 *     Copyright (c) 2012-2013, Broadcom Corporation
 *     All Rights Reserved
 *     Confidential Property of Broadcom Corporation
 *
 *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
 *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
 *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
 * 
 ***************************************************************************

 This directory is to hold customer specific non-Broadcom code
to be added to the SSBL portion of BOLT. Some template files
are provided here to get you up and running. Note that we don't
support anything you add in here.
 The use case is that after unpacking a new version of BOLT you will
unpack your code here, specify your custom config file to enable
these options and then build.


 New user commands and 'ioctls' are enabled via CFG_VENDOR_EXTENSIONS.

 BOLT calls custom_init() after the system devices and environment variables
have been setup, but before autostart & STARTUP (envar) are checked.

 BOLT calls custom_main() before the final command prompt loop. This
function may never get called if autostart or STARTUP does something that
never returns control to BOLT, e.g. loading and running Linux. The bolt
command prompt will never appear if you do not return from this function.


To enable, open the file config/stdbuild.cfg and change:

/* added customer code */
 config VENDOR_EXTENSIONS OFF
 config CUSTOM_CODE       OFF

to:

/* added customer code */
 config VENDOR_EXTENSIONS on
 config CUSTOM_CODE       on
 
 Then you have to clean and remake the configuration for the
build flags to be picked up (if you previously built BOLT),
i.e. 'make clean' before building BOLT.
