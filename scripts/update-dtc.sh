#!/bin/bash

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

if [ -d dtc ]; then
 rm -fr dtc
fi
git clone git://github.com/virtualopensystems/dtc.git dtc
pushd thirdparty
commit=`git log --pretty=oneline | head -1 | awk '{printf "%.10s", $1;}'`
popd
rm -rf dtc/.g*

#  The produced tar file below is a dual GPL/BSD licensed package
# not under any Broadom license. See the README.license file 
# in the package for more information.
tar -czf thirdparty/GPL-BSD-dtc-$commit.tgz dtc
