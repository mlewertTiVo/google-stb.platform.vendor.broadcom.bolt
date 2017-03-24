 All files in this directory and its subdirectories have
licenses other than those subject to, or provided by, 
Broadcom Corporation.

 All files in this directory and its subdirectories are
subject to the terms of those individual licenses.


FREEWARE
--------
disarm.c-0.11.c *
 Was from http://web.ukonline.co.uk/g.mccaughan/g/software.html (defunct),
 it now appears here: http://www.mccaughan.org.uk/g/software.html

zlib-1.1.3.tar.gz ****
zlib-1.2.8.tar.gz ****
 http://www.zlib.net
 wget http://sourceforge.net/projects/libpng/files/zlib/1.1.3/zlib-1.1.3.tar.gz

zlib-1.1.3.patch
 Patch for BOLT use.


Dual GPL/BSD  
------------
GPL-BSD-dtc-g448faa43.tgz
 git clone git://github.com/virtualopensystems/dtc.git dtc **
v4.1.1.tar.gz
 https://libcoap.net/ **

GPL
---
thirdparty/libgcc/libgcc.a ***
 This file is from the toolchain which is used to build BOLT.

thirdparty/checkpatch.pl
 From the linux kernel scripts.  Used for style checking.

thirdparty/bloat-o-meter
 From the busybox build scripts.  Used for measuring what functions add to the
 binary size.  Requires a Python interpreter.


BSD
---
thirdparty/libfdt_env.patch            
 Patch to make BOLT able to compile libfdt.

hmac
 The hmac code was released under BSD in 2005 and has not changed since.
 The code is from https://github.com/ogay/hmac

---

* minimal license to acknowledge the author.

** libfdt may be used by BOLT under the BSD license, The 'dtc' 
  application code is under the GPL license and must not be 
  integrated with Broadcom proprietary code.

*** Used under GPLv3 + runtime linking exception.

**** "free but without warranty of any kind"

-------------------------------------------------------------------------------
