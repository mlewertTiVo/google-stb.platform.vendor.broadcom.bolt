#/bin/bash
# ***************************************************************************
# *     Copyright (c) 2013, Broadcom Corporation
# *     All Rights Reserved
# *     Confidential Property of Broadcom Corporation
# *
# *  THIS SOFTWARE MAY ONLY BE USED SUBJECT TO AN EXECUTED SOFTWARE LICENSE
# *  AGREEMENT  BETWEEN THE USER AND BROADCOM.  YOU HAVE NO RIGHT TO USE OR
# *  EXPLOIT THIS MATERIAL EXCEPT SUBJECT TO THE TERMS OF SUCH AN AGREEMENT.
# * 
# ***************************************************************************
#
# bump up the [version x.y] in all config files
# 
# Its a bit quick & dirty.
#

sf=scripts/config.pl

if [ $# -ne 1 ]; then
	echo "usage: scripts/bump x.y"
	echo "bump all config file versions to x.y"
	exit 1
fi

cfg=`fgrep -rnl [version config/*`;

for f in $cfg; do
	sed  -i "s/^\[version *\([0-9]*[.][0-9]*\)/\[version $1/;" $f
	echo $f `fgrep [version $f`
done

sed -i "s/THIS_VERSION *=> *\"\([0-9]*[.][0-9]*\)\";/THIS_VERSION => \"$1\";/;" $sf

echo $sf `fgrep 'THIS_VERSION =>' $sf`
