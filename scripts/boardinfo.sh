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

for fam in $*; 
do 
 echo "===[ $fam ]===";
 fgrep board config/family-$fam.cfg | \
 fgrep \[ | fgrep \] | \
 sed "s/:/ /g;s/\[//g;s/\]//g;s/chip//;s/\./ /g;s/board//g;" | \
 awk '{ printf "\t%-20s - %10s\n", $1,$2;}';
done
